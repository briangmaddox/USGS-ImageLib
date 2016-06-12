#ifndef TIFFIMAGEOFILE_H
#define TIFFIMAGEOFILE_H
// $Id: TIFFImageOFile.h,v 1.1.1.1 2002-02-21 00:19:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:01 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class serves as a wrapper for Sam Leffer's libtiff.  It currently 
// supports multiple versions of tiff, as well as some beta support for writing
// multi-page tiff's

#include "ImageOFile.h"
#include "tiff.h"
#include "tiffio.h"

namespace USGSImageLib
{
class TIFFImageOFile : public ImageOFile
{
public:
  TIFFImageOFile() throw (ImageException, std::bad_alloc);
  TIFFImageOFile(std::string& tfilename, long tw, long th, int inphotometric,
                 bool isgeotiff = false)
    throw (ImageException, std::bad_alloc);

  virtual ~TIFFImageOFile();

  bool setupTIFF()                                    throw ();
  bool setCompression (uint16 incompression)          throw (ImageException);
  bool setXResolution(float xdpi)                     throw (ImageException);
  bool setYResolution(float xdpi)                     throw (ImageException);
  bool setResUnits(uint16 inresunits)                 throw (ImageException);
  bool setSubfileType (uint32 intype)                 throw (ImageException);
  bool setPlanarConfig (uint16 inconfig)              throw (ImageException);
  bool setRowsPerStrip (uint32 inrows)                throw (ImageException);
  bool setImageDescription (std::string indesc)       throw (ImageException);
  bool setDateTime (std::string intime)               throw (ImageException);
  bool setBitsPerSample(int inbps)                    throw (ImageException);
  bool setSamplesPerPixel(int insps)                  throw (ImageException);
  bool setDocumentName(std::string indocname)         throw (ImageException);
  bool setMake(std::string inmake)                    throw (ImageException);
  bool setModel(std::string inmodel)                  throw (ImageException);
  bool setOrientation(uint16 inorientation)           throw (ImageException);
  bool setMinSampleValue(uint16 invalue)              throw (ImageException);
  bool setMaxSampleValue(uint16 invalue)              throw (ImageException);
  bool setPageName(std::string inname)                throw (ImageException);
  bool setPageNumber(uint16 innumber, uint16 total=0) throw (ImageException);
  bool setSoftware(std::string insoftware)            throw (ImageException);
  bool setArtist(std::string inartist)                throw (ImageException);
  bool setHostComputer(std::string incomputer)        throw (ImageException);
  // These functions are here because they can't quite fit the normal
  // interface (ie, have multiple inputs)
  bool setDotRange(uint16 ina, uint16 inb)             throw (ImageException);
  bool setExtraSamples(uint16 count, uint16* arr)      throw (ImageException);
  bool setHalftoneHints(uint16 ina, uint16 inb)        throw (ImageException);
  bool setJPEGTables(uint32 count, void* tables)       throw (ImageException);
  bool setSubIFD(uint16 count, uint32* offsets)        throw (ImageException);
  bool setTransferFunction(uint16* arr1, uint16* arr2,
                           uint16* arr3)               throw (ImageException);
  bool setYCBCRSampling(uint16 inhoriz, uint16 invert) throw (ImageException);
  bool setICCProfile(uint32 count, void* data)         throw (ImageException);

  bool setTag(ttag_t tag, uint32 val)          throw (ImageException);
  bool setTag(ttag_t tag, uint16 val)          throw (ImageException);
  bool setTag(ttag_t tag, float val)           throw (ImageException);
  bool setTag(ttag_t tag, float* val)          throw (ImageException);
  bool setTag(ttag_t tag, uint16* val)         throw (ImageException);
  bool setTag(ttag_t tag, u_char** val)        throw (ImageException);
  bool setTag(ttag_t tag, std::string sval)    throw (ImageException);

  // Standard ImageOFile interface
  virtual void setPalette(Palette* p) throw (ImageException, std::bad_alloc);

  // These next 3 functions take the input array's as voids since a TIFF can
  // return 8 to 32 bit data elements.  The user must allocate the memory in
  // advance and then pass it in here
  virtual void putRawPixel(long int x,long int y, void* outpixel)
    throw (ImageException, std::bad_alloc);
  virtual void putRawScanline(long row, void* outarray)
    throw (ImageException, std::bad_alloc);
  virtual void putRawRectangle(long int x1, long int y1,
                               long int x2, long int y2,
                               void* outarray)
    throw (ImageException, std::bad_alloc);


  virtual void putPixel(long int x, long int y, AbstractPixel* outpixel)
    throw (ImageException, std::bad_alloc);
  virtual void putScanline(long int row, RGBPixel* outarray)
    throw (ImageException, std::bad_alloc);
  virtual void putScanline(long int row, GreyPixel* outarray)
    throw (ImageException, std::bad_alloc);

  virtual void updateColormap(RGBPixel* p) throw();
  virtual void writeColormap()             throw(ImageException);


protected:
  // Define our bits for the tiff settings
  enum TIFFBITS
  {
    TIFFWIDTH        = 1,
    TIFFLENGTH       = 2,
    TIFFCOMPRESSION  = 4,
    TIFFPHOTOMETRIC  = 8,
    TIFFROWSSTRIP    = 16,
    TIFFXRES         = 32,
    TIFFYRES         = 64,
    TIFFRESUNIT      = 128,
    TIFFPLANARCNFG   = 256,
    TIFFBPS          = 512,
    TIFFCOLORMAP     = 1024,
    TIFFSAMPLEPIXEL  = 2048,
    TIFFSUBFILE      = 4096,
    TIFFIMAGEDESC    = 8192,
    TIFFDATETIME     = 16384
  };
    
  TIFF* tif;
  uint16* red;         // red, green, and blue are for
  uint16* green;       // the tiff colormap
  uint16* blue;
  unsigned char* buffer;
  unsigned int scan_line;
  int color_count;
  long int baselinebits;       // hold which baseline tags are set
  bool isGeotiff;

  void setSubfileBit()        throw();
  void setWidthBit()          throw();
  void setLengthBit()         throw();
  void setSamplesPixelBit()   throw();
  void setBitsSampleBit()     throw();
  void setCompressionBit()    throw();
  void setPhotometricBit()    throw();
  void setXResBit()           throw();
  void setYResBit()           throw();
  void setResUnitBit()        throw();
  void setRowsStripBit()      throw();
  void setColormapBit()       throw();
  void setImageDescriptBit()  throw();
  void setDateTimeBit()       throw();
  void setPlanarConfigBit()   throw();
};

} // namespace
#endif
