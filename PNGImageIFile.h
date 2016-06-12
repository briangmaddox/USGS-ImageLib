/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

/*This class reads png (www.libpng.org/pub/png) image files and is really just
  a wrapper on the c libpng.  It was designed to be able to read VERY large 
  png files and is therefore somewhat inefficient.
  Originally written by Christopher Bilderback, SES USGS*/

#ifndef _PNGIMAGEIFILE_H_
#define _PNGIMAGEIFILE_H_


#include <fstream>
#include <vector>
#include <png.h>
#include "ImageIFile.h"
#include "RGBPalette.h"

//this is needed for newer versions of png
#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

namespace USGSImageLib
{
//The png reading class
class PNGImageIFile : public ImageIFile
{
public:
  
  //Constructor and Destructor
  PNGImageIFile(std::string & infile) 
    throw(ImageException, std::bad_alloc);
  ~PNGImageIFile();

  /**
     Png is a "streaming" image file format.  This means that there is no
     built in mechanism for random access.  So this function is provided to
     cache scanlines so that they can be available without rereading the image.
     If no cache is set (the default) and an out of order scanline is read,
     then the class will reread the file until the desired scanline is found. 
     Note this makes ops such as getrectangle extremely slow so caching is
     recommended. CBB 6/15/01 **/
  void setScanlineCache(long int number_of_lines) 
    throw(ImageException, std::bad_alloc);

  //implememnts the ImageIFile interface 
  virtual void getPalette(Palette* inpalette)
    throw(ImageException, std::bad_alloc);

  virtual void getPixel(long x, long y, AbstractPixel* inpixel)
    throw(ImageException, std::bad_alloc);
  virtual void getScanline(long row, RGBPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getScanline(long row, GreyPixel* inarray)
    throw(ImageException, std::bad_alloc);
  //  virtual void getScanline(long row, HSVPixel* inarray)
  //    throw(ImageException, std::bad_alloc);
  virtual void getRectangle(long x1, long y1, long x2, long y2,
        RGBPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRectangle(long x1, long y1, long x2, long y2,
                            GreyPixel* inarray)
    throw(ImageException, std::bad_alloc);
  //  virtual void getRectangle(long x1, long y1, long x2, long y2,
  //                            HSVPixel* inarray)
  //    throw(ImageException, std::bad_alloc);


  virtual void getRawPixel(long x, long y, unsigned char* inpixel)
    throw(ImageException, std::bad_alloc);
  virtual void getRawScanline(long row, unsigned char* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRawRectangle(long x1, long y1, long x2, long y2,
                               unsigned char* inarray)
    throw(ImageException, std::bad_alloc);

  //added to support 16-bps raw data (for 48 bit pngs)
  virtual void getRawPixel(long x, long y, short unsigned int * inpixel)
    throw(ImageException, std::bad_alloc);
  virtual void getRawScanline(long row, short unsigned int * inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRawRectangle(long x1, long y1, long x2, long y2,
                               short unsigned int * inarray)
    throw(ImageException, std::bad_alloc);


  virtual void getRawRGBPixel(long x, long y, RawRGBPixel* pix)
    throw(ImageException, std::bad_alloc);
  virtual void getRawRGBScanline(long row, RawRGBPixel* pixbuf)
    throw(ImageException, std::bad_alloc); 
  virtual void getRawRGBRectangle(long x1, long y1, long x2, long y2,
                                  RawRGBPixel* pixbuf)
    throw(ImageException, std::bad_alloc);
  
  virtual void getOptimalRectangle(long int& w, long int& h)
    throw(ImageException, std::bad_alloc);

protected:

  //this function setups up the palette
  void setupPalette() throw(ImageException, std::bad_alloc);
  
  //this function reallocates a cache around a particular scanline
  void reCache(long int line) throw(ImageException, std::bad_alloc);

  RGBPalette*  palette;     //holds the palette if we need it
  
  //libpng structs
  png_structp png_ptr;
  png_infop   info_ptr;
  FILE * pngfile;                    //png uses the c io
  long int currentline;              //the current line that is to be read
  int number_passes;                 //for interlace handling stuff
  long int cache_size;               //the number of scanlines in the cache
  unsigned char** cache;             //the scanline cache
  long int cache_start;              //what row does the cache start on
  long int bytesperline;             //the number of bytes in a scanline

};
  
}//namespace

#endif
