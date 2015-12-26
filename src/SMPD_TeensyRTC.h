/*
 *************************************************************************
 *
 * SMPD_TeensyRTC.h
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

#ifndef SMPD_TEENSYRTC_h
#define SMPD_TEENSYRTC_h

#include "SMPD_Time.h"

class SMPD_TeensyRTC : public SMPD_Time {
  public:
                    SMPD_TeensyRTC(uint32_t updateInterval = 0 );  // updateInterval will be ignored in here
    virtual void    setUTCTime(uint32_t time);
    virtual void    setTZShift(int16_t tzshift) { this->tzshift = tzshift; };
    virtual time_t  getEpochTime(bool utc = false);
  protected:
            int16_t tzshift;  // -/+ minutes timezone shift in minutes
  private:
};

#include "SMPD_TeensyRTC_Impl.h"

#endif // SMPD_TEENSYRTC_h