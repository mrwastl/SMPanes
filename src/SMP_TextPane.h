/*
 *************************************************************************
 *
 * SMP_TextPane.h
 * Pane for displaying generic textual information
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

#ifndef SMP_TEXTPANE_h
#define SMP_TEXTPANE_h

#include "SMP_Pane.h"
#include "SMPD_Base.h"
#include "Layer_Background.h"

#define SMPANES_ALLOCATE_TEXTPANE(pane_name, storage_depth, width, height) \
    static RGB_TYPE(storage_depth) pane_name##panedata[1*width*height];    \
    static SMP_TextPane<RGB_TYPE(storage_depth)> pane_name(width, height, pane_name##panedata)


template <typename smpRGB>
class SMP_TextPane : public SMP_Pane<smpRGB> {
  public:
                          SMP_TextPane(uint16_t width, uint16_t height, smpRGB* rgbBuffer = NULL);

    virtual void          draw();

    virtual void          stop(bool clear = true);
    virtual void          setMessage(String message, uint32_t currMS = 0);

    virtual void          setMessageGenerator(SMPD_Base & messageGenerator) { this->messageGenerator = &messageGenerator; };

            void          setTextAlign(Align hor = center, Align vert = middle);

            void          setFont(fontChoices fontChoice);
            //void         setFont(bitmap_font * font); // not yet supported by SmartMatrix

  protected:
    virtual uint8_t       calculateSizes();
    virtual bool          internalSetParent(SM_Layer* parent, LayerType parentType, uint8_t parentDepth);
    virtual void          updateContent(uint32_t currMS = 0);
    virtual bool          getTextAlignTrans(uint16_t* tX, uint16_t* tY, uint16_t* tW, uint16_t* tH, uint8_t strLen);
    virtual void          drawContent();

            String        message;
            bool          autoFont;
            bitmap_font * font;
            fontChoices   fontChoice;
            SMPD_Base   * messageGenerator;

            Align         textAlignHor;
            Align         textAlignVert;
};


#include "SMP_TextPane_Impl.h"


#endif // SMP_TEXTPANE_h