#ifndef _ABSTRACTPIXEL_H_
#define _ABSTRACTPIXEL_H_
// $Id: AbstractPixel.h,v 1.1.1.1 2002-02-21 00:18:55 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:55 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Defines abstract class Pixel
// This class is the base class for the various pixel types.  Note that it
// is a pure virtual class and not meant to be implemented

namespace USGSImageLib
{
class AbstractPixel
{
public:
  virtual bool getRed(unsigned short& inred) const throw() =0;
  virtual bool getGreen(unsigned short& ingreen) const throw() =0;
  virtual bool getBlue(unsigned short& inblue) const throw() =0;

  virtual bool getGrey(unsigned short& ingrey) const throw() =0;

  virtual bool getHue(float& inhue) const throw() =0;
  virtual bool getSaturation(float& insaturation) const throw() =0;
  virtual bool getValue(float& invalue) const throw() =0;

  virtual bool setRed(unsigned short) throw() =0;
  virtual bool setGreen(unsigned short) throw() =0;
  virtual bool setBlue(unsigned short) throw() =0;

  virtual bool setGrey(unsigned short) throw() =0;
  
  // Added this to allow setting color via HSV method.  BGM
  virtual bool setByHSV(float& inhue, float& insaturation,
                        float& invalue) throw() =0;
};

} // namespace

#endif // #ifndef
