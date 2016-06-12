#ifndef _RAWGREYODATAHANDLER_H_
#define _RAWGREYODATAHANDLER_H_
// $Id: RawGreyODataHandler.h,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class handles output of raw grey data

#include "RawODataHandler.h"
#include <fstream>

namespace USGSImageLib
{

class RawGreyODataHandler : public RawODataHandler  
{
public:
  RawGreyODataHandler(std::ofstream* outstream, long int outposition, 
                      long int outwidth, long int outheight, int photometric)
    throw(ImageException);
  virtual ~RawGreyODataHandler();

  // These functions take in Pixels and output them into the file
  void putPixel(AbstractPixel* outpixel, long int x, long int y)
    throw(ImageException);
  void putScanline(RGBPixel* outarray, long int row)
    throw (ImageException, std::bad_alloc);
  void putScanline(GreyPixel* outarray, long int row)
    throw (ImageException, std::bad_alloc);

  // These take in the raw values for the scanlines (unsigned chars) and output
  void putRawPixel(unsigned char* outpixel, long int x, long int y)
    throw(ImageException, std::bad_alloc);
  void putRawScanline(unsigned char* outarray, long int row)
    throw(ImageException, std::bad_alloc);
  void putRawRectangle(unsigned char* outarray, long int x1, long int y1, 
                       long int x2, long int y2)
    throw(ImageException, std::bad_alloc);
  
private:
  //   RawGreyODataHandler();
};

} // namespace

#endif // #ifndef _RAWGREYODATAHANDLER_H_
