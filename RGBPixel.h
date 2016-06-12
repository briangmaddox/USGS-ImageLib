#ifndef _RGBPIXEL_H_
#define _RGBPIXEL_H_
// $Id: RGBPixel.h,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Defines the class RGBPixel, which is used to store pixels with Red,
// Green, and Blue components

#include "AbstractPixel.h"

namespace USGSImageLib
{
class RGBPixel : public AbstractPixel
{

public:
  RGBPixel() throw() {};
  RGBPixel(unsigned short r, unsigned short g, unsigned short b) throw();
  virtual ~RGBPixel() {};
  
  // These functions return the RGB components of the image
  virtual bool getRed(unsigned short& inred)     const throw();
  virtual bool getGreen(unsigned short& ingreen) const throw();
  virtual bool getBlue(unsigned short& inblue)   const throw();
  
  // This uses the standard transformation from RGB to the Y (intensity)
  // component of the YIQ colorspace.
  virtual bool getGrey(unsigned short& ingrey) const throw();


  // These return the HSV values of the image
  virtual bool getHue(float& inhue)               const throw();
  virtual bool getSaturation(float& insaturation) const throw();
  virtual bool getValue(float& invalue)           const throw();

  // These allow the user to set the RGB values of the image
  virtual bool setRed(unsigned short inred)     throw();
  virtual bool setGreen(unsigned short ingreen) throw();
  virtual bool setBlue(unsigned short inblue)   throw();

  // Sets Red = Green = Blue
  virtual bool setGrey(unsigned short ingrey) throw();  

  // These allow the user to set the RGB values by means of the HSV 
  // colorspace values (must pass them all in since you need all of them
  // to go to RGB coordinates
  virtual bool setByHSV(float& inhue, float& insaturation, 
                        float& invalue) throw();


protected:
  // These hold the RGB values of the image
  unsigned short Red;
  unsigned short Green;
  unsigned short Blue;
   
};

} // namespace
#endif
