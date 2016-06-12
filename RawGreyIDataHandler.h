#ifndef _RAWGREYIDATAHANDLER_H_
#define _RAWGREYIDATAHANDLER_H_
// $Id: RawGreyIDataHandler.h,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// 24 May 1998
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class handles the reading of raw grey data from a file

#include "RawIDataHandler.h"
#include <vector>
#include <fstream>

namespace USGSImageLib
{
class RawGreyIDataHandler : public RawIDataHandler  
{
 public:
  // Construction/Destruction
  RawGreyIDataHandler(std::ifstream* instream, long int inposition, 
                      long int inwidth, long int inheight,
                      int inphotometric = PHOTO_GREY)
    throw(ImageException);
  virtual ~RawGreyIDataHandler();

  
  // This function scans through the file and creates a palette.  Note that
  // it may take a while to return!
  void getPalette(Palette* inpalette) const 
    throw(ImageException, std::bad_alloc);

  // These are the new functions to return AbstractPixel arrays.  They assume
  // the caller has allocated the memory they need.
  void getScanline(long int row, GreyPixel* inarray)
    const throw(ImageException, std::bad_alloc);
  void getScanline(long int row, RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc);
  void getPixel(long int x, long int y, AbstractPixel* inpixel)
    const throw(ImageException);
  void getRectangle(long int x1, long int y1, long int x2, long int y2,
                    GreyPixel* inarray)
    const throw(ImageException, std::bad_alloc);
  void getRectangle(long int x1, long int y1, long int x2, long int y2,
                    RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc);

  // These are the new functions to return the actual scanline in the image
  // file.  They assume the caller has allocated the memory beforehand.
  void getRawScanline(long int row, unsigned char* inarray)
    const throw(ImageException);
  void getRawPixel(long int x, long int y, unsigned char* inpixel)
    const throw(ImageException);
  void getRawRectangle(long int x1, long int y1, long int x2, long int y2,
                       unsigned char* inarray)
    const throw(ImageException);

protected:

private:
};

} // namespace

#endif // !defined(RAWGREYIDATAHANDLER_H)
