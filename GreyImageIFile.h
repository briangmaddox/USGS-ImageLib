#ifndef _GREYIMAGEIFILE_H_
#define _GREYIMAGEIFILE_H_
// $Id: GreyImageIFile.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class handles the reading of raw greyscale (8 bit) raster images

#include <fstream>
#include "ImageIFile.h"
#include "RawGreyIDataHandler.h"

namespace USGSImageLib
{
class GreyImageIFile : public ImageIFile
{
public:
  GreyImageIFile(std::string& infile, std::string& hdr)
    throw(ImageException, std::bad_alloc);
  GreyImageIFile(std::string& infile) throw(ImageException, std::bad_alloc);
  virtual ~GreyImageIFile();

  // Standard ImageIFile interface

  virtual void getPalette(Palette* inpalette)
    throw(ImageException, std::bad_alloc);

  virtual void getPixel(long int x, long int y, AbstractPixel* inpixel)
    throw(ImageException, std::bad_alloc);
  virtual void getScanline(long int row, GreyPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getScanline(long int row, RGBPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRectangle(long int x1, long int y1,
                            long int x2, long int y2, GreyPixel* inarray)
    throw(ImageException, std::bad_alloc);

  virtual void getRectangle(long int x1, long int y1,
                            long int x2, long int y2,
                            RGBPixel* inarray)
    throw(ImageException, std::bad_alloc);

  virtual void getRawPixel(long int x, long int y, unsigned char* inpixel)
    throw(ImageException, std::bad_alloc);
  virtual void getRawScanline(long int row, unsigned char* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRawRectangle(long int x1, long int y1, 
                               long x2, long y2, unsigned char* inarray)
    throw(ImageException, std::bad_alloc);

protected:
  std::ifstream*       Stream;
  std::string          headerFile;
  RawIDataHandler*     datahandler;

  void readHeader() throw(ImageException, std::bad_alloc);
};

} // namespace
#endif
