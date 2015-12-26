/*
 *************************************************************************
 *
 * SMPD_idDHT.h
 * Class for DHT11 / DHT22 sensors (interrupt driven sensor read)
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

#ifndef SMPD_idDHT_h
#define SMPD_idDHT_h

#include "SMPD_DHT.h"
#include <idDHTLib.h>

class SMPD_idDHT : public SMPD_DHT {
  public:
                       SMPD_idDHT(
                         uint8_t dataPin = 0, uint8_t interruptNr = 0,
                         bool dht22 = false,
                         uint32_t updateInterval = (30 * 1000)  // default update every 30 seconds
                       );
                      ~SMPD_idDHT();
    virtual bool       update(uint32_t currMS = 0);
  protected:
            idDHTLib * dht;
  private:
};

#include "SMPD_idDHT_Impl.h"

#endif // SMPD_idDHT_h