/*
 *************************************************************************
 *
 * SMP_Pane.h
 * Abstract base classes SMP_Base and SMP_Pane for all derived Pane-classes
 *
 * SMP_Base is the template-free base class of SMP_Pane and also defines
 * some static methods for chaining panes and process these in one go.
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

#ifndef SMP_PANE_h
#define SMP_PANE_h

#include "Layer.h"
#include "MatrixCommon.h"

#define SM_SUPPORT_ADDITIONAL_COLOURSPACES 1

typedef enum LayerType {
  none = -2,
  undefined = -1,
  background = 0,
  scrolling,
  indexed
} LayerType;

typedef enum Align {
  left = 0,
  top = 0,
  center = 1,
  middle = 1,
  right = 2,
  bottom = 2
} Align;


class SMP_Base {
  public:
    virtual   bool            update(uint32_t currMS = 0) = 0;
    virtual   void            draw() = 0;
    virtual   uint8_t         getID() { return 0; };
    virtual   void            setMessage(String message, uint32_t currMS = 0) {};

              void            chainPane(SMP_Base & nextPane);
              SMP_Base      * getNextPane() { return this->nextPane; };

    /* poor man's reflection. very ugly, better solution would be nice */
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
              bool            setParent(SMLayerBackground<rgb8,0>& parent) { return internalSetParent(parent, background, 8);};
              bool            setParent(SMLayerBackground<rgb16,0>& parent) { return internalSetParent(parent, background, 16);};
#endif
              bool            setParent(SMLayerBackground<rgb24,0>& parent) { return internalSetParent(parent, background, 24);};
              bool            setParent(SMLayerBackground<rgb48,0>& parent) { return internalSetParent(parent, background, 48);};
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
              bool            setParent(SMLayerScrolling<rgb8,0>& parent) { return internalSetParent(parent, indexed, 8);};
              bool            setParent(SMLayerScrolling<rgb16,0>& parent) { return internalSetParent(parent, indexed, 16);};
#endif
              bool            setParent(SMLayerScrolling<rgb24,0>& parent) { return internalSetParent(parent, indexed, 24);};
              bool            setParent(SMLayerScrolling<rgb48,0>& parent) { return internalSetParent(parent, indexed, 48);};
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
              bool            setParent(SMLayerIndexed<rgb8,0>& parent) { return internalSetParent(parent, scrolling, 8);};
              bool            setParent(SMLayerIndexed<rgb16,0>& parent) { return internalSetParent(parent, scrolling, 16);};
#endif
              bool            setParent(SMLayerIndexed<rgb24,0>& parent) { return internalSetParent(parent, scrolling, 24);};
              bool            setParent(SMLayerIndexed<rgb48,0>& parent) { return internalSetParent(parent, scrolling, 48);};

    /* static public methods */
    static    void            initChain(uint16_t matrixWidth, uint16_t matrixHeight);
#if SM_SUPPORT_ADDITIONAL_COLOURSPACES == 1
    static    void            chainAdd(SMP_Base & pane, SMLayerBackground<rgb8,0>& parent);
    static    void            chainAdd(SMP_Base & pane, SMLayerBackground<rgb16,0>& parent);
#endif
    static    void            chainAdd(SMP_Base & pane, SMLayerBackground<rgb24,0>& parent);
    static    void            chainAdd(SMP_Base & pane, SMLayerBackground<rgb48,0>& parent);
    static    bool            updateAll(uint32_t currMS = 0);
    static    void            drawAll();

    static    bool            sendMessage(uint8_t id, String message);

  protected:
                              SMP_Base();
    virtual   bool            internalSetParent (SM_Layer& parent, LayerType parentType, uint8_t parentDepth) = 0;

              SMP_Base      * nextPane; // for chaining

              SM_Layer      * parentLayer;
              LayerType       parentType;
              uint8_t         parentDepth;

    /* static protected methods and fields */
    static    void            internalChainAdd(SMP_Base & pane);

    static    SMP_Base      * firstPane;
    static    SMP_Base      * lastPane;
    static    uint16_t        matrixWidth;
    static    uint16_t        matrixHeight;

};


template <typename smpRGB>
class SMP_Pane : public SMP_Base {
  public:
              void            setPosition (uint16_t x = 0, uint16_t y = 0, Align posAlignHor = left, Align posAlignVert = top);
              void            setTransparent(bool transparent) { this->transparent = transparent; };
              void            setBorder(bool drawBorder = true);
              bool            isTransparent() { return this->transparent; }
              bool            isBorder() { return this->drawBorder; }
              void            setRotation(rotationDegrees newrotation);
              uint16_t        getX() { return this->x; };
              uint16_t        getY() { return this->y; };
              uint16_t        getWidth() { return this->w; };
              uint16_t        getHeight() { return this->h; };
              rotationDegrees getRotation() { return this->rotation; };
              void            setColours(smpRGB fgCol, smpRGB bgCol = rgb24(0, 0, 0), smpRGB borderCol = rgb24(0, 0, 0));

              bool            update(uint32_t currMS = 0);
    virtual   void            run();
    virtual   void            stop(bool clear = false) = 0;
    virtual   void            draw() = 0;

  protected:
                              SMP_Pane();
    virtual   bool            internalSetParent (SM_Layer& parent, LayerType parentType, uint8_t parentDepth);

    virtual   void            updateContent(uint32_t currMS = 0) = 0;

              bool            contentChanged;

              bool            transparent;
              bool            active;
              rotationDegrees rotation;
              uint16_t        w;
              uint16_t        h;
              uint16_t        x;
              uint16_t        y;

              SM_Layer      * contentLayer;
              LayerType       contentType;

              smpRGB          bgCol;
              smpRGB          fgCol;
              smpRGB          borderCol;
              bool            drawBorder;
              bool            fgWhite; // for indexed parent layer
  private:
};


#include "SMP_Pane_Impl.h"

#endif // SM_PANE_h