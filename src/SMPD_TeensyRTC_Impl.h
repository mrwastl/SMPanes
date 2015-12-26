/*
 *************************************************************************
 *
 * Implementation part for SMPD_TeensyRTC.h
 * Class for Teensy RTC time chip, time is set manually
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


#include "SMPD_Time.h"
#include <Time.h>

// we only have 1 teensy RTC, so a simple c-function is just fine ...
time_t getTeensy3RTCTime() {
  return Teensy3Clock.get();
}


SMPD_TeensyRTC::SMPD_TeensyRTC (uint32_t updateInterval) : SMPD_Time(updateInterval) {
  this->tzshift = 0;
  setSyncProvider(getTeensy3RTCTime);
  this->status =  ( timeStatus() == timeSet ) ? OK : INITERR;
}


void SMPD_TeensyRTC::setUTCTime(uint32_t time) {
  uint32_t off = 1;  // add n seconds as delay correction
  Teensy3Clock.set( time + off ); // set the RTC
  setTime( time + off );
}


time_t SMPD_TeensyRTC::getEpochTime(bool utc) {
  return (utc) ? now() : (now() + tzshift);
}
