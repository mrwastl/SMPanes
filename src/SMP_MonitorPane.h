/*
 *************************************************************************
 *
 * SMP_MonitorPane.h
 * Pane for displaying monitoring information (e.g. coming from nagios)
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

#ifndef SMP_MONITORPANE_h
#define SMP_MONITORPANE_h

#include "SMP_InfoPane.h"

#define SMPANES_ALLOCATE_MONITORPANE(pane_name, storage_depth, width, height) \
    static RGB_TYPE(storage_depth) pane_name##panedata[1*width*height];        \
    static SMP_MonitorPane<RGB_TYPE(storage_depth)> pane_name(width, height, pane_name##panedata)

#define MONITORING_COLUMNS 4

template <typename smpRGB>
class SMP_MonitorPane : public SMP_InfoPane<smpRGB> {
  public:
                           SMP_MonitorPane(uint16_t width, uint16_t height, smpRGB* rgbBuffer = NULL);


  protected:
    virtual void           updateContent(uint32_t currMS = 0);
    virtual void           setMessage(String message, uint32_t currMS = 0);

    virtual bool           getTextAlignTrans(uint16_t* tX, uint16_t* tY, uint16_t* tW, uint16_t* tH, uint8_t dummy = 0);

            bool           setColumnParams(uint8_t column, char id, smpRGB fg, smpRGB bg = rgb24(0x00, 0x00, 0x00));


            uint32_t       lastUpdate;
            uint16_t       drawWidth;
            uint16_t       expireTime;
            uint16_t       showChangedDuration;
            bool           saneData;

            smpRGB         okCol;
            smpRGB         errCol;
            smpRGB         chgCol;

            char           monID [ MONITORING_COLUMNS ];
            smpRGB         monFGCol [ MONITORING_COLUMNS ];
            smpRGB         monBGCol [ MONITORING_COLUMNS ];
            uint8_t        monValue [ MONITORING_COLUMNS ];
            uint32_t       monLastChanged [ MONITORING_COLUMNS ];
};


#include "SMP_MonitorPane_Impl.h"


#endif // SMP_MONITORPANE_h