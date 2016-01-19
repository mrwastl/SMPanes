/*
 *************************************************************************
 *
 * Implementation part for SMP_TextPane.h
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

#include "SMP_Pane.h"

template <typename smpRGB>
SMP_TextPane<smpRGB>::SMP_TextPane(uint16_t width, uint16_t height, smpRGB* rgbBuffer) : SMP_Pane<smpRGB>::SMP_Pane() {
  this->w = width;
  this->h = height;

  this->transparent = true;
  this->x = 0;
  this->y = 0;

  this->textAlignHor = center;
  this->textAlignVert = middle;

  if (rgbBuffer) {
    this->contentLayer = new SMLayerBackground<smpRGB,0>(rgbBuffer, width, height);
  }
  if (this->contentLayer) {
    this->contentType = background;
    this->setRotation(rotation0); // required, or localWidth/Height are not set in SMLayerBackground
    /* but setFont() cannot set bitmap font directly yet */
    //((SMLayerBackground<smpRGB,0>*)this->contentLayer)->setFont( font5x7 );
  }
  this->parentLayer = NULL;
  this->message = "";
  this->autoFont = true;
  /* we need the bitmap font for calculations */
  this->fontChoice = font5x7;

  this->messageGenerator = NULL;
  this->calculateSizes();
}


template <typename smpRGB>
uint8_t SMP_TextPane<smpRGB>::calculateSizes() {
  if (this->autoFont) {
    // will be replaced through better code once if generic font setting is available in SmartMatrix ..
    uint16_t maxH = this->h - ((this->drawBorder) ? 4 : 0);
    if (maxH <= 6) {
      this->fontChoice = font3x5;    // fontHeight: 6 -> - 6
    } else if (maxH <= 9) {
      this->fontChoice = font5x7;    // fontHeight: 7 -> 7 - 9
    } else if (maxH <= 10) {
      this->fontChoice = font6x10;   // fontHeight: 10 -> 10
    } else if (maxH <= 12) {
      this->fontChoice = gohufont11; // fontHeight: 11 -> 11 - 12
    } else {
      this->fontChoice = font8x13;   // fontHeight: 13 -> 13 -
    }
  }
  this->font = (bitmap_font*) fontLookup( this->fontChoice );
  return 0;
}


template <typename smpRGB>
bool SMP_TextPane<smpRGB>::internalSetParent(SM_Layer* parent, LayerType parentType, uint8_t parentDepth) {
  if (parentType == background) {
    this->parentLayer = parent;
    this->parentType = parentType;
    this->parentDepth = parentDepth;
    return true;
  }
  /* we only supprt background layers here */
  return false;
}


template <typename smpRGB>
void SMP_TextPane<smpRGB>::stop(bool clear) {
  SMP_Pane<smpRGB>::stop(clear);
  if (clear && this->parentLayer) {
    ((SMLayerBackground<smpRGB,0>*)this->parentLayer)->fillScreen(rgb24(0,0,0));
  }
}


template <typename smpRGB>
void SMP_TextPane<smpRGB>::setMessage(String message, uint32_t currMS) {
  if (this->message != message) {
#if PANE_SERIAL_DEBUG == 1  && PANE_TEXTPANE_DEBUG > 0
    Serial.print("SMP_TextPane::setMessage(): message changed to: "); Serial.println(message);
#endif
    this->contentChanged = true;
    this->message = message;
  }
}


template <typename smpRGB>
void SMP_TextPane<smpRGB>::setTextAlign(Align hor, Align vert) {
  this->textAlignHor = hor;
  this->textAlignVert = vert;
}


template <typename smpRGB>
bool SMP_TextPane<smpRGB>::getTextAlignTrans(uint16_t* tX, uint16_t* tY, uint16_t* tW, uint16_t* tH, uint8_t strLen) {
  uint16_t strW = 0;
  uint16_t strH = 0;

  int16_t pX = 0;
  int16_t pY = 0;

  *tX = 0;
  *tY = 0;
  *tW = this->w;
  *tH = this->h;

  if (this->drawBorder) {
    *tX += 2;
    *tY += 2;
    *tW -= 4;
    *tH -= 4;
  }

  strW = strLen * this->font->Width - 1;
  strH = this->font->Height;

  switch(this->textAlignHor) {
    case center:
      pX = ( (*tW) - strW) / 2.0;
      break;
    case right:
      pX = ( (*tW) - strW );
      break;
    default: // left
      ; // do nothing
  }

  switch(this->textAlignVert) {
    case middle:
      pY = ( (*tH) - (strH - 1)) / 2.0;  // ignore descender  (g, p, j, ...)
      break;
    case bottom:
      pY = ( (*tH) - strH);
      break;
    default: // left
      ; // do nothing
  }

  if (pX > 0) *tX += pX;
  if (pY > 0) *tY += pY;

  return this->drawBorder;
}

template <typename smpRGB>
void SMP_TextPane<smpRGB>::updateContent(uint32_t currMS) {
  if ( ! this->active ) {
    return;
  }

  if (this->messageGenerator) {
    if (!currMS) {
      currMS = millis();
    }

    if (this->messageGenerator->update(currMS)) {
      String newMessage = this->messageGenerator->getMessage();

      if (newMessage != this->message) {
        this->contentChanged = true;
        this->message = newMessage;
      }
    }
  }
}


template <typename smpRGB>
void SMP_TextPane<smpRGB>::setFont(fontChoices fontChoice) {
  if (this->contentLayer) {
    this->autoFont = false;
    this->fontChoice = fontChoice;
    this->calculateSizes();
  }
};


#if 0
/* not yet supported by SmartMatrix :( */
template <typename smpRGB>
void SMP_TextPane<smpRGB>::setFont(bitmap_font * font) {
  if (this->contentLayer) {
    this->autoFont = false;
    this->font = font;
    ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->setFont( font );
  }
};
#endif


#define INLINE_TEXTPANE_DRAWPARENT(_layerdepth) \
  if (this->transparent) {\
    if (border) {\
      ((SMLayerBackground<RGB_TYPE(_layerdepth),0>*)this->parentLayer)->drawRectangle(this->x, this->y, this->x + this->w - 1, this->y + this->h - 1, this->borderCol); \
    } \
  } else {\
    if (border) { \
      ((SMLayerBackground<RGB_TYPE(_layerdepth),0>*)this->parentLayer)->fillRectangle(this->x, this->y, this->x + this->w - 1, this->y + this->h - 1, this->borderCol, this->bgCol); \
    } else { \
      ((SMLayerBackground<RGB_TYPE(_layerdepth),0>*)this->parentLayer)->fillRectangle(this->x, this->y, this->x + this->w - 1, this->y + this->h - 1, this->bgCol); \
    } \
  } \
  ((SMLayerBackground<RGB_TYPE(_layerdepth),0>*)this->parentLayer)->setFont( this->fontChoice ); \
  ((SMLayerBackground<RGB_TYPE(_layerdepth),0>*)this->parentLayer)->drawString(this->x + tx, this->y + ty, this->fgCol, this->message.c_str())


template <typename smpRGB>
void SMP_TextPane<smpRGB>::drawContent() {
  uint16_t tx = 0;
  uint16_t ty = 0;
  uint16_t tw = this->w;
  uint16_t th = this->h;
  bool border = false;

  border = this->getTextAlignTrans(&tx, &ty, &tw, &th, this->message.length());

  if (this->contentChanged && this->contentLayer) {
    if (border) {
      ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillRectangle(0, 0, this->w - 1, this->h - 1, this->borderCol, this->bgCol);
    } else {
      ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->fillScreen(this->bgCol);
    }
    /* ARRGGHH: font is static in background layer class: thus set font before writing ... */
    ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->setFont( this->fontChoice );
    ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->drawString(tx, ty, this->fgCol, this->message.c_str());
  } else if (this->parentType == background) {
    switch (this->parentDepth) { // need better solution for this
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
      case 8:
        INLINE_TEXTPANE_DRAWPARENT(8);
        break;
      case 16:
        INLINE_TEXTPANE_DRAWPARENT(16);
        break;
#endif
      case 24:
        INLINE_TEXTPANE_DRAWPARENT(24);
        break;
      default:
        INLINE_TEXTPANE_DRAWPARENT(48);
    }
  }
}


template <typename smpRGB>
void SMP_TextPane<smpRGB>::draw() {
  if ( ! (this->active && this->parentLayer) )
    return;

  uint16_t i, j;
  smpRGB col;

  this->drawContent();

  if (this->contentLayer) {
    for (j = 0; j < this->h; j++) {
      for (i = 0; i < this->w; i++) {
        col = ((SMLayerBackground<smpRGB,0>*)this->contentLayer)->readPixel(i, j);
        if ( !  ( this->transparent && col.red == this->bgCol.red && col.green == this->bgCol.green && col.blue == this->bgCol.blue ) ) {
          switch (this->parentDepth) { // need better solution for this
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
            case 8:
              ((SMLayerBackground<rgb8,0>*)this->parentLayer)->drawPixel(i+this->x, j+this->y, col);
              break;
            case 16:
              ((SMLayerBackground<rgb16,0>*)this->parentLayer)->drawPixel(i+this->x, j+this->y, col);
              break;
#endif
            case 24:
              ((SMLayerBackground<rgb24,0>*)this->parentLayer)->drawPixel(i+this->x, j+this->y, col);
              break;
            default:
              ((SMLayerBackground<rgb48,0>*)this->parentLayer)->drawPixel(i+this->x, j+this->y, col);
          }
        }
      }
    }
  }
  this->contentChanged = false;
}
