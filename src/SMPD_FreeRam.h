/*
 *************************************************************************
 *
 * SMPD_FreeRam.h
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

#ifndef SMPD_FREERAM_h
#define SMPD_FREERAM_h

#include "SMPD_Base.h"

class SMPD_FreeRam : public SMPD_Base {
  public:
                     SMPD_FreeRam(uint32_t updateInterval = 0 );  // no update interval needed for this one
            bool     update(uint32_t currMS = 0);
            String   getMessage();

  protected:
            uint32_t value;
  private:
};

#include "SMPD_FreeRam_Impl.h"

#endif // SMPD_FreeRam_h