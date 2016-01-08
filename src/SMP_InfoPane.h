/*
 *************************************************************************
 *
 * SMP_InfoPane.h
 * Pane for displaying messages that can be assoziated to an ID
 *
 *************************************************************************
 *
 * copyright (C)  2015-2016  wolfgang astleitner
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

#ifndef SMP_INFOPANE_h
#define SMP_INFOPANE_h

#include "SMP_TextPane.h"
//#include "SMPD_Base.h"

#define SMPANES_ALLOCATE_INFOPANE(pane_name, storage_depth, width, height) \
    static RGB_TYPE(storage_depth) pane_name##panedata[1*width*height];    \
    static SMP_InfoPane<RGB_TYPE(storage_depth)> pane_name(width, height, pane_name##panedata)


template <typename smpRGB>
class SMP_InfoPane : public SMP_TextPane<smpRGB> {
  public:
                          SMP_InfoPane(uint16_t width, uint16_t height, smpRGB* rgbBuffer = NULL);

    virtual void          setID(uint8_t id) { this->id = id; };
            uint8_t       getID()           { return this->id; };

  protected:
            uint8_t       id;
};


#include "SMP_InfoPane_Impl.h"


#endif // SMP_INFOPANE_h