#ifndef _HSVPIXEL_CPP_
#define _HSVPIXEL_CPP_
// $Id: HSVPixel.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "HSVPixel.h"
#include "ImageCommon.h"

using namespace USGSImageLib;

// ***************************************************************************
HSVPixel::HSVPixel(float inhue, float insaturation,
                   float invalue) throw()
{
  if ( !(insaturation == 0) && !(inhue < 360) )
  {
    Hue = inhue;
    Saturation = insaturation;
    Value = invalue;
  }
}


// **************************************************************************
bool HSVPixel::getRed(unsigned short& inred) const throw()
{
  unsigned short tgreen, tblue;

  HSVtoRGB(inred, tgreen, tblue, Hue, Value, Saturation);

  return true;
}


// **************************************************************************
bool HSVPixel::getGreen(unsigned short& ingreen) const throw()
{
  unsigned short tred, tblue;

  HSVtoRGB(tred, ingreen, tblue, Hue, Value, Saturation);

  return true;
}


// **************************************************************************
bool HSVPixel::getBlue(unsigned short& inblue) const throw()
{
  unsigned short tred, tgreen;

  HSVtoRGB(tred, tgreen, inblue, Hue, Value, Saturation);

  return true;
}


// **************************************************************************
bool HSVPixel::getGrey(unsigned short& ingrey) const throw()
{
  unsigned short red, green, blue;
  getRed(red);
  getGreen(green);
  getBlue(blue);

  ingrey =  (21 * red + 32 * green + 11 * blue) / 64;
  return true;
}


// **************************************************************************
bool HSVPixel::getHue(float& inhue) const throw()
{
  inhue = Hue;
  return true;
} 
          

// **************************************************************************
bool HSVPixel::getSaturation(float& insaturation) const throw()
{
  insaturation = Saturation;
  return true;
}          


// **************************************************************************
bool HSVPixel::getValue(float& invalue) const throw()
{
  invalue = Value;
  return true;
}


// **************************************************************************
bool HSVPixel::setByRGB(unsigned short inred, unsigned short ingreen,
                        unsigned short inblue) throw()
{
  RGBtoHSV(inred, ingreen, inblue, Hue, Value, Saturation);

  return true;
}


// **************************************************************************
bool HSVPixel::setByHSV(float& inhue, float& invalue, float& insaturation)
  throw()
{
  Hue = inhue;
  Value = invalue;
  Saturation = insaturation;
  return true;
}


// **************************************************************************
bool HSVPixel::setHue(float inhue) throw()
{
  Hue = inhue;
  return true;
}


// **************************************************************************
bool HSVPixel::setSaturation(float insaturation) throw()
{
  Saturation = insaturation;
  return true;
}


// **************************************************************************
bool HSVPixel::setValue(float invalue) throw()
{
  Value = invalue;
  return true;
}


// **************************************************************************
bool HSVPixel::setGrey(unsigned short ingrey) throw()
{
  Hue = 512;
  Saturation = 0;
  Value = ingrey;
  return true;
}


// **************************************************************************
bool HSVPixel::setRed(unsigned short inred) throw()
{
  return false; // You kinda need R,G,and B components to convert to HSV
}


// **************************************************************************
bool HSVPixel::setGreen(unsigned short ingreen) throw()
{
  return false; // same here
}


// **************************************************************************
bool HSVPixel::setBlue(unsigned short inblue) throw()
{
  return false; // and here
}

#endif
