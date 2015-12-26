/*
 *************************************************************************
 *
 * SMPD_Time.h
 * Abstract base class for time classes (set/get date/time)
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

#ifndef SMPD_TIME_h
#define SMPD_TIME_h

#include "SMPD_Base.h"

class SMPD_Time : public SMPD_Base {
  public:
    virtual time_t  getEpochTime(bool utc = false)  = 0;
    virtual bool    update(uint32_t currMS = 0) { return true; }
  protected:
                    SMPD_Time(
                      uint32_t updateInterval = (6 * 3600 * 1000)  // default update every 6 hours (eg. if synched to NTP)
                    );
  private:
};

#include "SMPD_Time_Impl.h"

#endif // SMPD_TIME_h