#ifndef _TIFFIMAGEIFILE_H_
#define _TIFFIMAGEIFILE_H_
// $Id: TIFFImageIFile.h,v 1.1.1.1 2002-02-21 00:19:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:01 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class reads in TIFF files.

#include <string>
#include "ImageIFile.h"
#include "tiff.h"
#include "tiffio.h"

namespace USGSImageLib
{
class TIFFImageIFile : public ImageIFile
{
 public:
  TIFFImageIFile(std::string& tfilename, bool usemmap = true,
                 bool isgeotiff = false); 
  virtual ~TIFFImageIFile(); 
  
  void setupTIFF()                                     throw (ImageException);
  bool getCompression (uint16& incompression)          throw (ImageException);
  bool getXResolution(float& inxdpi)                   throw (ImageException);
  bool getYResolution(float& inydpi)                   throw (ImageException);
  bool getResUnits(uint16& inresunits)                 throw (ImageException);
  bool getSubfileType(uint32& intype)                  throw (ImageException);
  bool getPlanarConfig(uint16& inconfig)               throw (ImageException);
  bool getRowsPerStrip(uint32& inrows)                 throw (ImageException);
  bool getImageDescription(std::string& indesc)        throw (ImageException);
  bool getDateTime(std::string& intime)                throw (ImageException);
  bool getCopyright(std::string& incopy)               throw (ImageException);
  bool getFillOrder(uint16& infillorder)               throw (ImageException);
  bool getDocumentName(std::string& indocname)         throw (ImageException);
  bool getMake(std::string& inmake)                    throw (ImageException);
  bool getModel(std::string& inmodel)                  throw (ImageException);
  bool getOrientation(uint16& inorientation)           throw (ImageException);
  bool getMinSampleValue(uint16& invalue)              throw (ImageException);
  bool getMaxSampleValue(uint16& invalue)              throw (ImageException);
  bool getPageName(std::string& inname)                throw (ImageException);
  bool getPageNumber(uint16& innumber)                 throw (ImageException);
  bool getSoftware(std::string& insoftware)            throw (ImageException);
  bool getArtist(std::string& inartist)                throw (ImageException);
  bool getHostComputer(std::string& incomputer)        throw (ImageException);
  // These are kinda not used much, but they return different from the 
  // expected norm so had to make them separate functions here.
  bool getDotRange(uint16& inone, uint16& intwo)       throw (ImageException);
  bool getExtraSamples(uint16& innum, uint16* inarr)   throw (ImageException);
  bool getHalftoneHints(uint16& inhighlight,
                        uint16& inshadow)              throw (ImageException);
  bool getJPEGTables(u_short& incount, void* intable)  throw (ImageException);
  bool getPageNumber(uint16& innum, uint16& intotal)   throw (ImageException);
  bool getSubIFD(uint16& incount, uint32* array)       throw (ImageException);
  bool getTransferFunction(uint16* arr1, uint16* arr2,
                           uint16* arr3)               throw (ImageException);
  bool getYCBCRSubsampling(uint16& inhoriz, 
                          uint16& invert)              throw (ImageException);
  bool getICCProfile(uint32& count, void* data)        throw (ImageException);

  bool getTag(ttag_t tag, uint32& val)      throw (ImageException);
  bool getTag(ttag_t tag, uint16& val)      throw (ImageException);
  bool getTag(ttag_t tag, uint32** val)     throw (ImageException);
  bool getTag(ttag_t tag, uint16** val)     throw (ImageException);
  bool getTag(ttag_t tag, float& val)       throw (ImageException);
  bool getTag(ttag_t tag, float** val)      throw (ImageException);
  // This is a wrapper for the char* version
  bool getTag(ttag_t tag, std::string& val) throw (ImageException);
  bool getTag(ttag_t tag, u_char*** val)    throw (ImageException);

  //  Standard ImageIFile interface

  virtual void getPalette(Palette* inpalette)
    throw (ImageException, std::bad_alloc);
  
  virtual void getRawPixel(long int x, long int y, unsigned char* inarray)
    throw (ImageException, std::bad_alloc);
  virtual void getRawScanline(long int row, unsigned char* inarray)
    throw (ImageException, std::bad_alloc);
  // This function allows the user to make the actual TIFF call to return
  // data of different types (ie, 16 bit TIFFs)
  virtual void getRawScanline(long int row, tdata_t inarray)
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
  // These functions handle the unpacking of the data into a "normal" 8 bit
  // array.
  void unpack4(unsigned char* dest, unsigned char* input) 
    const throw(ImageException);
  void unpack1(unsigned char* dest, unsigned char* input) 
    const throw(ImageException);

  TIFF* tif;
  uint16* red;         // red, green, and blue are for
  uint16* green;       // the tiff colormap
  uint16* blue;
  uint16 planarconfig; // to store for later use
  uint16 Compression;  // store it from the TIFF
  long int rows_strip; // so we don't have to recompute for getRawScanline
  bool   isGeotiff;
};

} // namespace
#endif

