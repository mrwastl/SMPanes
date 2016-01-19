/*
 *************************************************************************
 *
 * SMP_DateTimePane.h
 * Pane for displaying date / time with 7 segment-like digits
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

#ifndef SMP_DATETIMEPANE_h
#define SMP_DATETIMEPANE_h

#include "SMP_TextPane.h"
#include "SMPD_Time.h"

#define SMPANES_ALLOCATE_DATETIMEPANE(pane_name, storage_depth, width, height) \
    static RGB_TYPE(storage_depth) pane_name##panedata[1*width*height];        \
    static SMP_DateTimePane<RGB_TYPE(storage_depth)> pane_name(width, height, pane_name##panedata)


#define MAX_DATETIME_ELEMS 6


/* FORMAT string:
 * date / time place holders:
 *   %d  .. day 01..31
 *   %H  .. hour 00..23
 *   %I  .. hour 01..12
 *   %M  .. minute 00..59
 *   %m  .. month 01..12
 *   %S  .. seconds 00..60
 *   %y  .. year 00..99
 *   %Y  .. year 4-digit
 * separators:
 *  '-'  .. dash  eg: "%Y-%m-%d"
 *  '.'  .. dot   eg: "%d.%m.%Y"
 *  '/'  .. dot in the middle (instead of a slash)  eg: "%m/%d/%y"
 *  ':'  .. semicolon  eg: "%H:%M:%S"
 *  '::' .. two semicolons = blinking semicolon(s)  eg: "%H::%M"
 *  ' '  .. space - may be repeated   eg: "%H:%M  %Y-%m-%d"
 * 
 */

template <typename smpRGB>
class SMP_DateTimePane : public SMP_TextPane<smpRGB> {
  public:
                           SMP_DateTimePane(uint16_t width, uint16_t height, smpRGB* rgbBuffer = NULL);

            void           setDataProvider(SMPD_Time & dataProvider) { this->dataProvider = & dataProvider; };

            bool           setFormat(String format);

            void           setFX(uint8_t percent = 25);
            void           setFX(smpRGB fxCol);
            void           forceSegmentSettings(uint8_t segWidth, uint8_t segThick);

    /* deactivated methods */
    virtual void           setMessage(String message) { };
  protected:
    virtual uint8_t        calculateSizes();
            bool           processFormat();
    virtual void           updateContent(uint32_t currMS = 0);
    virtual void           drawContent();
            uint16_t       drawNumber(uint16_t x, uint16_t y, int16_t number, uint8_t digits);
            void           drawDigit(uint16_t x, uint16_t y, int8_t digit);
            void           drawBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool hor, smpRGB col);
    virtual bool           getTextAlignTrans(uint16_t* tX, uint16_t* tY, uint16_t* tW, uint16_t* tH, uint8_t dummy = 0);


            uint8_t        dateTimeElems;
            char           formatCode[MAX_DATETIME_ELEMS];
            char           separatorCode[MAX_DATETIME_ELEMS - 1];
            uint8_t        formatSize[MAX_DATETIME_ELEMS];
            uint8_t        separatorWidth[MAX_DATETIME_ELEMS - 1];

            SMPD_Time    * dataProvider;
            uint8_t        segWidth;
            uint8_t        segHeight;
            uint8_t        segThick;
            bool           drawFX;
            smpRGB         fgColFX;

            uint8_t        forceSegWidth;
            uint8_t        forceSegThick;

            uint8_t        resolution;
            uint8_t        segTCount;
            uint8_t        digitCount;
            uint16_t       drawWidth;

            uint8_t        oldSec;
            uint8_t        oldMin;
            uint8_t        oldHour;
            uint8_t        oldDay;
            uint8_t        oldMon;
            uint16_t       oldYear;

            bool           showSec;
            bool           showMin;
};


#include "SMP_DateTimePane_Impl.h"


#endif // SMP_DATETIMEPANE_h