#ifndef _RAWRGBIDATAHANDLER_H_
#define _RAWRGBIDATAHANDLER_H_
// $Id: RawRGBIDataHandler.h,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class handles the reading of raw RGB data from a file, attempting
// to handle BIP, BIL, BSQ, ect data.  It's actually a helper class, where you
// pass in a pointer to an already opened file stream and the position where
// the image data starts.

#include "RawIDataHandler.h"
#include <vector>
#include <fstream>

namespace USGSImageLib
{
class RawRGBIDataHandler : public RawIDataHandler
{
public:
  RawRGBIDataHandler(std::ifstream* instream, long int inposition,
		     long int inwidth, long int inheight,
		     int inphotometric = PHOTO_RGB,
		     char* inbandcontent = "RGB") throw(ImageException);
  virtual ~RawRGBIDataHandler();

  // This functions allows the user to set the Band type
  bool setBandContent(char* inbandcontent);

  // This function scans through the file and creates a palette.  Note: On
  // RGB images, this can take a REALLY long time.
  void getPalette(Palette* inpalette)
    const throw(ImageException, std::bad_alloc);

  // These functions return AbstractPixel arrays.  They assume the caller has
  // allocated the memory they need before calling them
  void getScanline(long int row, RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc);
  void getScanline(long int row, GreyPixel* inarray)
    const throw(ImageException, std::bad_alloc);
  void getPixel(long int x, long int y, AbstractPixel* inpixel)
    const throw(ImageException);
  void getRectangle(long int x1, long int y1, long int x2, long int y2,
                    RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc);
  void getRectangle(long int x1, long int y1, long int x2, long int y2,
                    GreyPixel* inarray)
    const throw(ImageException, std::bad_alloc);

  void getRedBand(long int row, RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc);
  void getGreenBand(long int row, RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc);
  void getBlueBand(long int row, RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc);

  // These functions return the actual scanlines in the image file (if 
  // possible).  They assume the caller as allocated memory beforehand.
  // Note:  These functions return the data in RGB triplets, reguardless
  // of the organization or banding in the file
  void getRawScanline(long int row, unsigned char* inarray)
    const throw(ImageException, std::bad_alloc);
  void getRawPixel(long int x, long int y, unsigned char* inpixel)
    const throw(ImageException, std::bad_alloc);
  void getRawRectangle(long int x1, long int y1, long int x2, long int y2,
                       unsigned char* inarray)
    const throw(ImageException, std::bad_alloc);
  void getRawRedBand(long int row, unsigned char* inarray)
    const throw(ImageException, std::bad_alloc);
  void getRawGreenBand(long int row, unsigned char* inarray)
    const throw(ImageException, std::bad_alloc);
  void getRawBlueBand(long int row, unsigned char* inarray)
    const throw(ImageException, std::bad_alloc);

protected:
  char bandcontent[4];  // hold the color order of the file, ie, RGB, BGR, ect
  bool rgbordered;      // if it's already in RGB and is a BIP, don't worry
                        // about rearranging
 
private:
};

} // namespace
#endif
