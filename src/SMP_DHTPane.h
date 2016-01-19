/*
 *************************************************************************
 *
 * SMP_DHTPane.h
 * Pane for displaying temperature / humidity information provided by a DHT-sensor
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

#ifndef SMP_DHTPANE_h
#define SMP_DHTPANE_h

#include "SMP_TextPane.h"
#include "SMPD_DHT.h"

#define SMPANES_ALLOCATE_DHTPANE(pane_name, storage_depth, width, height) \
    static RGB_TYPE(storage_depth) pane_name##panedata[1*width*height];   \
    static SMP_DHTPane<RGB_TYPE(storage_depth)> pane_name(width, height, pane_name##panedata)


typedef enum DHTFeature {
  Celsius = 0,
  Fahrenheit = 1,
  Kelvin = 2,
  Humidity = 10
} DHTFeature;



template <typename smpRGB>
class SMP_DHTPane : public SMP_TextPane<smpRGB> {
  public:
                    SMP_DHTPane(uint16_t width, uint16_t height, smpRGB* rgbBuffer = NULL);

            void    setDataProvider(SMPD_DHT & dataProvider) { this->dataProvider = & dataProvider; };
            void    setFeature(DHTFeature feature, bool showUnit = true);

    /* deactivated methods */
    virtual void    setMessage(String message) { this->message = ""; };

  protected:
    virtual void    updateContent(uint32_t currMS = 0);
    virtual void    drawContent();
    SMPD_DHT      * dataProvider;
    DHTFeature      feature;
    bool            showUnit;
    String          unitText;
    float           oldValue;
    DataStatus      status;
};


#include "SMP_DHTPane_Impl.h"


#endif // SMP_DHTPANE_h