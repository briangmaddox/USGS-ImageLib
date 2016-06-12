#ifndef _JPEGIMAGEIFILE_H_
#define _JPEGIMAGEIFILE_H_
// $Id: JPEGImageIFile.h,v 1.2 2004-12-14 23:40:33 bmaddox Exp $
// Brian Maddox - gowaddle@yahoo.com
// Last modified by $Author: bmaddox $ on $Date: 2004-12-14 23:40:33 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include <string>
#include <cstdio>
#include "RGBPixel.h"
#include "GreyPixel.h"
#include "ImageIFile.h"
extern "C"
{
#include "jpeglib.h"
#include "jerror.h"
}

namespace USGSImageLib
{
class JPEGImageIFile : public ImageIFile
{
public:
  JPEGImageIFile(std::string& infilename);
  virtual ~JPEGImageIFile();

  // Special JPEG specific parameters
  bool setDitherMode(J_DITHER_MODE& inmode)           throw();
  bool selectColorMappedOutput()                      throw();
  bool setNumberOutputColors(int& indesired_colors)   throw();
  bool setTwoPassQuantize()                           throw();
  bool setDCTMethod(J_DCT_METHOD& inmethod)           throw();
  bool setFancyUpsampling(bool& on)                   throw();
  bool setBlockSmoothing(bool& on)                    throw();
  bool sawAdobeMarker()                         const throw();

  // Get functions for JPEG specific information
  void getNumberPaletteColors(int& numcolors)   const throw();
  void getJFIFMajorVersion(UINT8& inmajor)      const throw();
  void getJFIFMinorVersion(UINT8& inminor)      const throw();
  void getDensityUnit(UINT8& inunit)            const throw();
  void getXDensity(UINT16& inxden)              const throw();
  void getYDensity(UINT16& inyden)              const throw();

  // Standard image interface
  virtual void getPalette(Palette* inpalette)
    throw (ImageException, std::bad_alloc);

  virtual void getRawPixel(long int x, long int y, unsigned char* inarray)
    throw (ImageException, std::bad_alloc);
  virtual void getRawScanline(long int row, unsigned char* inarray)
    throw (ImageException, std::bad_alloc);
  virtual void getRawRectangle(long int x1, long int y1,
                               long int x2, long int y2,
                               unsigned char* inarray)
    throw (ImageException, std::bad_alloc);
  
  virtual void getPixel(long int x, long int y, AbstractPixel* inpixel)
    throw (ImageException, std::bad_alloc);
  virtual void getScanline(long int row, RGBPixel* inarray)
    throw (ImageException, std::bad_alloc);
  virtual void getScanline(long int row, GreyPixel* inarray)
    throw (ImageException, std::bad_alloc);
  virtual void getRectangle(long int x1, long int y1,long int x2, long int y2,
                            RGBPixel* inarray)
    throw (ImageException, std::bad_alloc);
  virtual void getRectangle(long int x1, long int y1, long int x2, long int y2,
                            GreyPixel* inarray)
    throw (ImageException, std::bad_alloc);
  
protected:
  struct jpeg_decompress_struct jpeginfo; // libjpeg object
  struct jpeg_error_mgr         jerr;     // jpeg error object
  std::FILE*                    jpegfile; // C stype FILE pointer

  bool     startdecompress; // If we've called start_decompression or not
  long int curr_read;       // The scanline we're sittiing on

  // These are fields that further identify information about the JPEG.
  UINT8  major_version, minor_version;
  UINT8  density_unit;
  UINT8  adobe_transform;
  bool   saw_adobe_marker;
  UINT16 x_density, y_density;

  // Temporary scanline storage buffer.  
  JSAMPARRAY scanlinebuffer;

  // Open the JPEG to avoid code duplication
  void openJPEG() throw(ImageException);
  // Reload the JPEG for when we loop through reading scanlines
  void resetJPEG() throw(ImageException);

};
  
// ***************************************************************************
inline void JPEGImageIFile::getNumberPaletteColors(int& numcolors) const 
  throw()
{
  numcolors = jpeginfo.actual_number_of_colors;
  return;
}

// ***************************************************************************
inline bool JPEGImageIFile::sawAdobeMarker() const throw()
{
  return saw_adobe_marker;
}

// ***************************************************************************
inline void JPEGImageIFile::getJFIFMajorVersion(UINT8& inmajor) const throw()
{
  inmajor = major_version;
  return;
}

// ***************************************************************************
inline void JPEGImageIFile::getJFIFMinorVersion(UINT8& inminor) const throw()
{
  inminor = minor_version;
  return;
}

// ***************************************************************************
inline void JPEGImageIFile::getDensityUnit(UINT8& inunit) const throw()
{
  inunit = density_unit;
  return;
}

// ***************************************************************************
inline void JPEGImageIFile::getXDensity(UINT16& inxden) const throw()
{
  inxden = x_density;
  return;
}

// **************************************************************************
inline void JPEGImageIFile::getYDensity(UINT16& inyden) const throw()
{
  inyden = y_density;
  return;
}

}

#endif
