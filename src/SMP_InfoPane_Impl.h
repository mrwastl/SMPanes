/*
 *************************************************************************
 *
 * Implementation part for SMP_InfoPane.h
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

//#include "SMP_TextPane.h"

template <typename smpRGB>
SMP_InfoPane<smpRGB>::SMP_InfoPane(uint16_t width, uint16_t height, smpRGB* rgbBuffer)
: SMP_TextPane<smpRGB>::SMP_TextPane(width, height, rgbBuffer) {
   this->id = 0;
}
