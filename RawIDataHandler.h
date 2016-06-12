#ifndef _RAWIDATAHANDLER_H_
#define _RAWIDATAHANDLER_H_
// $Id: RawIDataHandler.h,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// 21 May 1998
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This is the base class for the Raw Data input handler.  This base class
// is virtual.  The use of this class and its children is allow multiple
// classes to read raw data without having to duplicate input routines in
// all their code.
// Usage:  Basically, this class is passed in the ifstream* and the file
// position where the actual data begins.  Note, you really really should
// be careful you don't delete the ifstream you passed in here and then
// still try to use this class!

#include <new>
#include <fstream>
#include "RawDataHandler.h"
#include "Palette.h"
#include "RGBPixel.h"
#include "GreyPixel.h"

namespace USGSImageLib
{

class RawIDataHandler : public RawDataHandler  
{
public:
  RawIDataHandler(std::ifstream* instream, long int inposition,
                  long int inwidth, long int inheight,
                  int inphotometric = PHOTO_GREY)
    throw(ImageException);
  virtual ~RawIDataHandler();
  
  virtual void getPalette(Palette* inpalette)
    const throw(ImageException, std::bad_alloc)=0;
  
  // These are the new interface functions.  The caller allocates memory and
  // sends it into these functions instead of them returning mem.
  virtual void getScanline(long int row, RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc)=0;
  virtual void getScanline(long int row, GreyPixel* inarray)
    const throw(ImageException, std::bad_alloc)=0;
  virtual void getPixel(long int x, long int y, AbstractPixel* inpixel)
    const throw(ImageException, std::bad_alloc)=0;
  virtual void getRectangle(long int x1, long int y1, long int x2, long int y2,
                            RGBPixel* inarray)
    const throw(ImageException, std::bad_alloc)=0;
  virtual void getRectangle(long int x1, long int y1, long int x2, long int y2,
                            GreyPixel* inarray)
    const throw(ImageException, std::bad_alloc)=0;

  // These functions return the actual values in the scanline in the image.
  // They are the new functions which take memory from the caller
  virtual void getRawScanline(long int row, unsigned char* inarray)
    const throw(ImageException, std::bad_alloc)=0;
  virtual void getRawPixel(long int x, long int y, unsigned char* inpixel)
    const throw(ImageException, std::bad_alloc)=0;
  virtual void getRawRectangle(long int x1, long int y1,
                               long int x2, long int y2,
                               unsigned char* inarray)
    const throw(ImageException, std::bad_alloc)=0;

protected:
  long int dataposition;  // Position at which to start reading
  std::ifstream* Stream;      // the stream pointer


private:

};

} // namespace

#endif // !defined(RAWIDATAHANDLER_H)

