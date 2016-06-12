#ifndef _IMAGEOFILE_H_
#define _IMAGEOFILE_H_
// $Id: ImageOFile.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "ImageFile.h"
#include "RGBPixel.h"
#include "GreyPixel.h"

namespace USGSImageLib
{
class ImageOFile : public ImageFile
{
public:
  
  ImageOFile() throw (ImageException, std::bad_alloc);
  ImageOFile(std::string& infile, long int width, long int height) 
    throw (ImageException, std::bad_alloc);
  virtual ~ImageOFile();

  // Virtual functions to outline behaviour 
  virtual void setPalette(Palette* p)
    throw (ImageException, std::bad_alloc)=0;

  // get/put functions use column major order, ie (x,y) == (column,row)
  virtual void putPixel(long x, long y, AbstractPixel* outpixel)
    throw (ImageException, std::bad_alloc)=0;
  virtual void putScanline(long row, RGBPixel* outarray)
    throw (ImageException, std::bad_alloc)=0;
  virtual void putScanline(long row, GreyPixel* outarray)
    throw (ImageException, std::bad_alloc)=0;
  
  // These are set to voids as there can be many different data types output
  virtual void putRawPixel(long int x,long int y, void* outpixel)
    throw (ImageException, std::bad_alloc)=0;
  virtual void putRawScanline(long row, void* outarray)
    throw (ImageException, std::bad_alloc)=0;
  virtual void putRawRectangle(long int x1, long int y1,
                               long int x2, long int y2,
                               void* outarray)
    throw (ImageException, std::bad_alloc)=0;
};

} // namespace
#endif
