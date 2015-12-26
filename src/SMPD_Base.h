/*
 *************************************************************************
 *
 * SMPD_Base.h
 * Abstract base class for all sensor classes
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

#ifndef SMPD_BASE_h
#define SMPD_BASE_h

#include "WString.h" // or else: error 'String' does not name a type


typedef enum DataStatus {
  OK = 0,
  INITERR = 1,
  ERROR = 2,
  TIMEOUT = 3,
  UNKNOWN
} DataStatus;



class SMPD_Base {
  public:
            DataStatus getStatus();
    virtual bool       update(uint32_t currMS = 0);
    virtual String     getMessage() { return ""; };
            void       setRetryUpdatePercent(uint8_t pct);
  protected:
                       SMPD_Base(uint32_t updateInterval = (3 * 1000));

            bool       doUpdate(uint32_t currMS);
            DataStatus status;
            uint32_t   lastCheck;
            uint32_t   lastSuccessfulCheck;
            uint32_t   updateInterval;
            uint8_t    updateRetryPct;
  private:
};

#endif // SMPD_BASE_h