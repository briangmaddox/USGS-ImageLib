#ifndef _RGBPIXEL_CPP_
#define _RGBPIXEL_CPP_
// $Id: RGBPixel.cpp,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

//  Implements member functions for the class RGBPixel 

#include "RGBPixel.h"
#include "ImageCommon.h"

using namespace USGSImageLib;

// ***************************************************************************
RGBPixel::RGBPixel(unsigned short r, unsigned short g, 
                   unsigned short b) throw()
  : Red(r), Green(g), Blue(b)
{
}

// **************************************************************************
bool RGBPixel::getRed(unsigned short& inred) const throw()
{
  inred = Red;
  return true;
}

// **************************************************************************
bool RGBPixel::getGreen(unsigned short& ingreen) const throw()
{
  ingreen = Green;
  return true;
}

// **************************************************************************
bool RGBPixel::getBlue(unsigned short& inblue) const throw()
{
  inblue = Blue;
  return true;
}

// **************************************************************************
bool RGBPixel::getGrey(unsigned short& ingrey) const throw()
{
  //  This is the standard conversion from RGB to greyscale.
  //  It comes from the transformation from RGB to YIQ
  //  (the television standard).  The grayscale value is
  //  simply the Y (intensity) value from YIQ.

  ingrey = (21 * Red + 32 * Green + 11 * Blue) / 64;
  return true;
}

// **************************************************************************
bool RGBPixel::getHue(float& inhue) const throw()
{
  float tval, tsat;

  RGBtoHSV(Red, Green, Blue, inhue, tval, tsat);

  return true;
} 
          
// **************************************************************************
bool RGBPixel::getSaturation(float& insaturation) const throw()
{
  float thue, tval;
  
  RGBtoHSV(Red, Green, Blue, thue, insaturation, tval);
  
  return true;
}


// **************************************************************************
bool RGBPixel::getValue(float& invalue) const throw()
{
  float thue, tsat;
  
  RGBtoHSV(Red, Green, Blue, thue, invalue, tsat);

  return true;
}          
      
// **************************************************************************
bool RGBPixel::setRed(unsigned short inred) throw()
{
  Red = inred;
  return true;
}

// **************************************************************************
bool RGBPixel::setGreen(unsigned short ingreen) throw()
{
  Green = ingreen;
  return true;
}

// **************************************************************************
bool RGBPixel::setBlue(unsigned short inblue) throw()
{
  Blue = inblue;
  return true;
}

// **************************************************************************
bool RGBPixel::setGrey(unsigned short ingrey) throw()
{
  Red   = ingrey;
  Green = ingrey;
  Blue  = ingrey;
  return true;
}

// **************************************************************************
bool RGBPixel::setByHSV(float& inhue, float& insaturation,
                        float& invalue) throw()
{
  HSVtoRGB(Red, Green, Blue, inhue, invalue, insaturation);
  
  return true;
}

#endif
