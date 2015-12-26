/*
 *************************************************************************
 *
 * SMPD_Base.cpp - Implementation part
 * Abstract base class for all sensor classes
 *
 * Note: sole .cpp file to make arduino-IDE happy (needs at least one .c/.cpp-file)
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

#include "SMPD_Base.h"

SMPD_Base::SMPD_Base (uint32_t updateInterval) {
  this->status = OK;
  this->lastCheck = 0;
  this->lastSuccessfulCheck = 0;
  this->updateInterval = updateInterval;
  this->updateRetryPct = 10; // retry after 10% time of updateInterval
}

bool SMPD_Base::update (uint32_t currMS) {
  return false;
}


DataStatus SMPD_Base::getStatus () {
  if (this->status == OK) {
    return OK;
  }
  // if erraneous checks are within limits (updateInterval * 3) return OK anyways
  if (
    this->lastSuccessfulCheck && 
    ( (this->lastCheck - this->lastSuccessfulCheck) < (this->updateInterval * 3 ) )
  ) {
    return OK;
  }
  return this->status;
}


void SMPD_Base::setRetryUpdatePercent(uint8_t pct) {
  this->updateRetryPct = (pct <= 100) ? pct : 100;
}


/*inline*/ bool SMPD_Base::doUpdate(uint32_t currMS) {
  return ( 
    (!this->lastCheck) ||
    ( (this->status != OK) && ( (currMS - this->lastCheck) > ((this->updateInterval * this->updateRetryPct) / 100) ) ) ||
    ( (currMS - this->lastCheck) > this->updateInterval )
  );
}
