/*
 *************************************************************************
 *
 * Implementation part for SMPD_TeensyNTP.h
 * Class for Teensy RTC time chip, time is set by NTP (using an EtherCard based board)
 * and timezone corrected
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

SMPD_TeensyNTPTZ::SMPD_TeensyNTPTZ (uint32_t updateInterval, Timezone & tz, EtherCard & ethercard, String ntpServer, uint8_t ntpPort)
  : SMPD_TeensyNTP(updateInterval, ethercard, ntpServer, ntpPort) {
  this->tz = &tz;
}


time_t SMPD_TeensyNTPTZ::getEpochTime(bool utc) {
  return (utc || (!this->tz)) ? now() : ( tz->toLocal(now()));
}
