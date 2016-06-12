#ifndef RAWODATAHANDLER_H
#define RAWODATAHANDLER_H
// $Id: RawODataHandler.h,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// 21 May 1998
// Last Modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $
// This class is the base class for the Raw Output classes

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include <new>
#include <fstream>
#include "RawDataHandler.h"
#include "RGBPixel.h"
#include "GreyPixel.h"

namespace USGSImageLib
{
class RawODataHandler : public RawDataHandler  
{
public:
  RawODataHandler(std::ofstream* outstream, long int outposition, 
                  long int outwidth, long int outheight, int photometric)
    throw(ImageException);
  virtual ~RawODataHandler();


  virtual void putPixel(AbstractPixel* outpixel, long int x, long int y)
    throw (ImageException, std::bad_alloc) =0;
  virtual void putScanline(RGBPixel* outarray, long int row)
    throw (ImageException, std::bad_alloc) =0;
  virtual void putScanline(GreyPixel* outarray, long int row)
    throw (ImageException, std::bad_alloc) = 0;

  virtual void putRawPixel(unsigned char* outpixel, long int x, long int y)
    throw (ImageException, std::bad_alloc) =0;
  virtual void putRawScanline(unsigned char* outarray, long int row)
    throw (ImageException, std::bad_alloc) =0;

protected:
  long int dataposition;
  std::ofstream* Stream;

private:
};

} // namespace
#endif // !defined(RAWODATAHANDLER_H)
