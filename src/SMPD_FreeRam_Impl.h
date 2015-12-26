/*
 *************************************************************************
 *
 * Implementation part for SMPD_FreeRam.h
 * Class for getting free ram (for debugging only)
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

SMPD_FreeRam::SMPD_FreeRam (uint32_t updateInterval) : SMPD_Base(updateInterval) {
  this->value = 0;
}


// https://forum.pjrc.com/threads/23256-Get-Free-Memory-for-Teensy-3-0
bool SMPD_FreeRam::update(uint32_t currMS) {
  uint32_t stackTop;
  uint32_t heapTop;
  uint32_t newValue;

  // current position of the stack.
  stackTop = (uint32_t) &stackTop;

  // current position of heap.
  void* hTop = malloc(1);
  heapTop = (uint32_t) hTop;
  free(hTop);

  // The difference is the free, available ram.
  newValue = stackTop - heapTop;

  this->status = OK;
  this->lastSuccessfulCheck = this->lastCheck = currMS;

  if (this->value != newValue) {
    this->value = newValue;
    return true;
  }
  return false;
}

String SMPD_FreeRam::getMessage() {
  return String(this->value);
}
