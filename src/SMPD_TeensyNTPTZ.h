/*
 *************************************************************************
 *
 * SMPD_TeensyNTP.h
 * Class for Teensy RTC time chip, time is set by NTP (using an EtherCard based board)
 * and timezone corrected
 *
 * Required non standard libraries:
 *   EtherCard:         https://github.com/jcw/ethercard
 *   Timezone:          https://github.com/JChristensen/Timezone
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

#ifndef SMPD_TEENSYNTPTZ_h
#define SMPD_TEENSYNTPTZ_h

#include "SMPD_TeensyNTP.h"
#include <EtherCard.h>
#include <Timezone.h>

class SMPD_TeensyNTPTZ : public SMPD_TeensyNTP {
  public:
                       SMPD_TeensyNTPTZ(
                          uint32_t updateInterval,  // updateInterval will be ignored in here
                          Timezone & tz, EtherCard & ethercard,
                          String ntpServer, uint8_t ntpPort = 123
                       );
    virtual void       setTZShift(int16_t tzshift) { this->tzshift = 0; }; // disable method
    virtual time_t     getEpochTime(bool utc = false);
  protected:
            Timezone * tz;
  private:
};

#include "SMPD_TeensyNTPTZ_Impl.h"

#endif // SMPD_TEENSYNTPTZ_h