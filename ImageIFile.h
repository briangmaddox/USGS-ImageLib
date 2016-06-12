#ifndef _IMAGEIFILE_H_
#define _IMAGEIFILE_H_
// $Id: ImageIFile.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class is the base class which allows opening of images.  Note that
// it enforces read-only access

#include "ImageFile.h"
#include "RGBPixel.h"
#include "GreyPixel.h"

namespace USGSImageLib
{
class ImageIFile : public ImageFile
{
public:
  ImageIFile() throw(ImageException, std::bad_alloc);
  ImageIFile(std::string& fn) throw(ImageException, std::bad_alloc);
  virtual ~ImageIFile();

  virtual void getPalette(Palette* inpalette)
    throw(ImageException, std::bad_alloc)=0;


  virtual void getPixel(long x, long y, AbstractPixel* inpixel)
    throw(ImageException, std::bad_alloc)=0;
  virtual void getScanline(long row, RGBPixel* inarray)
    throw(ImageException, std::bad_alloc)=0;
  virtual void getScanline(long row, GreyPixel* inarray)
    throw(ImageException, std::bad_alloc)=0;
  virtual void getRectangle(long x1, long y1, long x2, long y2, 
			    RGBPixel* inarray)
    throw(ImageException, std::bad_alloc)=0;
  virtual void getRectangle(long x1, long y1, long x2, long y2,
                            GreyPixel* inarray)
    throw(ImageException, std::bad_alloc)=0;

  virtual void getRawPixel(long x, long y, unsigned char* inpixel)
    throw(ImageException, std::bad_alloc)=0;
  virtual void getRawScanline(long row, unsigned char* inarray)
    throw(ImageException, std::bad_alloc)=0;
  virtual void getRawRectangle(long x1, long y1, long x2, long y2,
                               unsigned char* inarray)
    throw(ImageException, std::bad_alloc)=0;
};

} // namespace
#endif
