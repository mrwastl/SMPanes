/*
 *************************************************************************
 *
 * SMPD_TeensyNTP.h
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

#ifndef SMPD_TEENSYNTP_h
#define SMPD_TEENSYNTP_h

#include "SMPD_TeensyRTC.h"
#include <EtherCard.h>

class SMPD_TeensyNTP : public SMPD_TeensyRTC {
  public:
                        
                        SMPD_TeensyNTP(
                          uint32_t updateInterval,  // updateInterval will be ignored in here
                          EtherCard & ethercard,
                          String ntpServer, uint8_t ntpPort = 123
                        );
    virtual void        setUTCTime(uint32_t time) {};  // disabled in this class
    virtual bool        update(uint32_t currMS = 0);
  protected:
            EtherCard * ethercard;
            String      ntpServer;
            uint8_t     ntpPort;
            uint32_t    timeFromNTP;
  private:
};

#include "SMPD_TeensyNTP_Impl.h"

#endif // SMPD_TEENSYNTP_h