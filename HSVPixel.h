#ifndef _HSVPIXEL_H_
#define _HSVPIXEL_H_
// $Id: HSVPixel.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class deals with pixels on the Hue-Saturation-Value colorspace

#include "AbstractPixel.h"
#include "ImageException.h"

namespace USGSImageLib
{

class HSVPixel : public AbstractPixel
{
public:
  HSVPixel() throw () {};
  HSVPixel(float inhue, float insaturation,
           float invalue) throw();
  virtual ~HSVPixel() {};

  virtual bool getRed(unsigned short& inred)     const throw();
  virtual bool getGreen(unsigned short& ingreen) const throw();
  virtual bool getBlue(unsigned short& inblue)   const throw();

  virtual bool getGrey(unsigned short& ingrey)   const throw();

  virtual bool getHue(float& inhue)               const throw();
  virtual bool getSaturation(float& insaturation) const throw();
  virtual bool getValue(float& invalue)           const throw();

  virtual bool setByRGB(unsigned short inred, unsigned short ingreen,
                        unsigned short inblue) throw();

  virtual bool setByHSV(float& inhue, float& invalue, float& insaturation)
    throw();

  virtual bool setGrey(unsigned short ingrey) throw();

  virtual bool setHue(float inhue)               throw();
  virtual bool setSaturation(float insaturation) throw();
  virtual bool setValue(float invalue)           throw();

  virtual bool setRed(unsigned short inred)      throw();
  virtual bool setGreen(unsigned short ingreen)  throw();
  virtual bool setBlue(unsigned short inblue)    throw();

protected:

  float Hue;
  float Saturation;
  float Value;

};

} // namespace
#endif
