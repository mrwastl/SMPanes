/*
 *************************************************************************
 *
 * Implementation part for SMPD_TeensyNTP.h
 * Class for Teensy RTC time chip, time is set by NTP (using an EtherCard based board)
 *
 *************************************************************************
 *
 * copyright (C)  2015  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 *************************************************************************
 * This program is free software; you can redistribute it and/or modify   
 * it under the terms of the GNU General Public License as published by   
 * the Free Software Foundation; either version 2 of the License, or (at  
 * your option) any later version.                                        
 *                                                                        
 * This program is distributed in the hope that it will be useful, but    
 * WITHOUT ANY WARRANTY; without even the implied warranty of             
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      
 * General Public License for more details.                               
 *                                                                        
 * You should have received a copy of the GNU General Public License      
 * along with this program; if not, write to the Free Software            
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA              
 * 02111-1307, USA.  Or, point your browser to                            
 * http://www.gnu.org/copyleft/gpl.html                                   
 *************************************************************************
 */

#include <Time.h>

SMPD_TeensyNTP::SMPD_TeensyNTP (uint32_t updateInterval, EtherCard & ethercard, String ntpServer, uint8_t ntpPort)
  : SMPD_TeensyRTC(updateInterval) {
  this->ethercard = &ethercard;
  this->ntpServer = ntpServer;
  this->ntpPort = ntpPort;
}

bool SMPD_TeensyNTP::update(uint32_t currMS) {
  if (!currMS) {
    currMS = millis();
  }

  if (currMS - this->lastCheck > 100) {  // delay between ethercard check: at least 100 ms
    word plen = this->ethercard->packetReceive();
    this->ethercard->packetLoop(plen);
    if(plen > 0 && this->ethercard->ntpProcessAnswer(&(this->timeFromNTP), this->ntpPort)) {
      uint32_t newTime = this->timeFromNTP;
      newTime -= (2208988800UL - 1);   // NTP to EpochTime diff  + a second for delay correction
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_TEENSYNTP_DEBUG == 1
      uint8_t m = minute(newTime), h = hour(newTime);
      Serial.print("SMPD_TeensyNTP::update: Time from NTP: ");
      Serial.println(this->timeFromNTP);
      Serial.print("SMPD_TeensyNTP::update: setUTCTime: "); Serial.print( newTime);
      Serial.print(" / ");
      if (h <= 9) Serial.print("0"); Serial.print(h); Serial.print(":"); 
      if (m <= 9) Serial.print("0"); Serial.println(m);
#endif
      Teensy3Clock.set( newTime ); // set the RTC
      setTime( newTime );
      this->status = OK;
      this->lastSuccessfulCheck = currMS;
    }
  }

//  if (!this->lastCheck || (currMS - this->lastCheck > this->updateInterval)) {  // request ntp time from ntp server
  if ( this->doUpdate(currMS) ) {  // request ntp time from ntp server
    if (! this->ethercard->dnsLookup( this->ntpServer.c_str() ) ) {
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_TEENSYNTP_DEBUG == 1
      Serial.println( "SMPD_TeensyNTP::update: DNS lookup failed" );
#endif
      this->status = ERROR;
    } else {
#if DATAPROVIDER_SERIAL_DEBUG == 1 && DATAPROVIDER_TEENSYNTP_DEBUG == 1
      Serial.print( "SMPD_TeensyNTP::update: Send NTP request to ");
      Serial.print( ntpServer ); //Serial.print(" (");
      this->ethercard->printIp(" / ", ether.hisip);
#endif
      this->ethercard->ntpRequest(this->ethercard->hisip, this->ntpPort);
      this->status = OK;
    }
    this->lastCheck = currMS;
  }
  return false;  // always return false because we don't want to trigger a drawing-update here
}
