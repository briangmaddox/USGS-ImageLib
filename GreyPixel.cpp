#ifndef _GREYPIXEL_CPP_
#define _GREYPIXEL_CPP_
// $Id: GreyPixel.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last Modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "GreyPixel.h"
#include "ImageException.h"
using namespace USGSImageLib;

// ***************************************************************************
GreyPixel::GreyPixel(unsigned short ingrey) throw()
  : Grey(ingrey)
{
}

// **************************************************************************
bool GreyPixel::setRed(unsigned short inred) throw()
{
  Grey = (21 * inred + 43 * Grey) / 64;
  return true;
}

// **************************************************************************
bool GreyPixel::setGreen(unsigned short ingreen) throw()
{
  Grey = (32 * ingreen + 32 * Grey) / 64;
  return true;
}

// **************************************************************************
bool GreyPixel::setBlue(unsigned short inblue) throw()
{
  Grey = (11 * inblue + 53 * Grey) / 64;
  return true;
}

// **************************************************************************
bool GreyPixel::setGrey(unsigned short ingrey) throw()
{
  Grey = ingrey;
  return true;
}

// **************************************************************************
bool GreyPixel::setByHSV(float& inhue, float& insaturation, 
			 float& invalue) throw()
{
  Grey = (unsigned short)invalue;
  return true;
}

// **************************************************************************
bool GreyPixel::getRed(unsigned short& inred) const throw()
{
  inred = Grey;
  return true;
}

// **************************************************************************
bool GreyPixel::getGreen(unsigned short & ingreen) const throw()
{
  ingreen = Grey;
  return true;
}

// **************************************************************************
bool GreyPixel::getBlue(unsigned short& inblue) const throw()
{
  inblue = Grey;
  return true;
}

// **************************************************************************
bool GreyPixel::getGrey(unsigned short& ingrey) const throw()
{
  ingrey = Grey;
  return true;
}

// **************************************************************************
bool GreyPixel::getHue(float& inhue) const throw()
{
  return false;  // undefined hue...achromatic
}

// **************************************************************************
bool GreyPixel::getSaturation(float& insaturation) const throw()
{
  insaturation = 0;  // 0 saturation for achromatic case
  return true;  
}

// **************************************************************************
bool GreyPixel::getValue(float& invalue) const throw()
{
  invalue = (unsigned short)Grey; // Grey is the value for HSV
  return true; 
}

#endif
