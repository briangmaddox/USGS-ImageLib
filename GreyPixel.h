#ifndef _GREYPIXEL_H_
#define _GREYPIXEL_H_
// $Id: GreyPixel.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class defines the GreyPixel class, which is used to store greyscale
// pixels

#include "AbstractPixel.h"

namespace USGSImageLib
{
class GreyPixel : public AbstractPixel
{
public:
  GreyPixel() throw() {};
  GreyPixel(unsigned short ingrey) throw();
  virtual ~GreyPixel() {};

  // These return the intensities as if the grey value came from
  // only the color asked for
  virtual bool getRed(unsigned short& inred)     const throw();
  virtual bool getGreen(unsigned short& ingreen) const throw();
  virtual bool getBlue(unsigned short& inblue)   const throw();

  // This works as expected
  virtual bool getGrey(unsigned short& ingrey)   const throw();

  virtual bool getHue(float& inhue)               const throw();
  virtual bool getSaturation(float& insaturation) const throw();
  virtual bool getValue(float& invalue)           const throw();


  // sets 11/32 of the grey
  virtual bool setRed(unsigned short inred)     throw();
  // sets 1/2 of the grey
  virtual bool setGreen(unsigned short ingreen) throw();
  // sets 5/32 of the grey
  virtual bool setBlue(unsigned short inblue)   throw();

  virtual bool setGrey(unsigned short ingrey)   throw();

  virtual bool setByHSV(float& inhue, float& insaturation,
                        float& invalue) throw();


protected:
  unsigned short Grey;
};

} // namespace

#endif
