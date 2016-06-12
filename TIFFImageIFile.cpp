#ifndef _TIFFIMAGEIFILE_CPP_
#define _TIFFIMAGEIFILE_CPP_
// $Id: TIFFImageIFile.cpp,v 1.5 2005-01-20 00:29:17 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2005-01-20 00:29:17 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Implementation of the TIFFImageIFile class
#include <cmath>
#include "TIFFImageIFile.h"
#include "RGBPalette.h"

using namespace USGSImageLib;

// ************************************************************************
// This class will attempt to open the TIFF based on the TIFF library's calls.
// If some of the tags are missing (ie, compression, photometric, ect, it will
// throw an exception and not open as those are necessary to handle the image
// data.  For other baseline tags, it will still open even if they're not
// present (ie, the resolutions)
TIFFImageIFile::TIFFImageIFile (std::string& tfilename, bool usemmap,
                                bool isgeotiff)
  : ImageIFile(tfilename), tif(0), red(0), green(0), blue(0), 
    planarconfig(0), Compression(0), isGeotiff(isgeotiff)
{
  char openflags[3];

  try
  {
    // This specifies whether or not to have libtiff mmap() files.  This can
    // cause problems if you're dealing with really really big tiffs and you
    // don't have a huge amount of RAM.  This is caused by the OS having to do
    // a lot of swapping if you fill up RAM.
    if (usemmap)
      strncpy(openflags, "r", 2);
    else
      strncpy(openflags, "rm", 3);
    
    if (!isGeotiff)
    {
      if ((tif = TIFFOpen(tfilename.c_str(), openflags)) == NULL)
      {
        setNoDataBit();
        throw ImageException(IMAGE_FILE_OPEN_ERR);
      }

      setupTIFF();  // go populate the TIFF tags
      setImageType(IMAGE_TIFF);
    }
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }
}


// **************************************************************************
// Destructor.  Just close the TIFF and exit.
TIFFImageIFile::~TIFFImageIFile()
{
  try
  {
    if (!isGeotiff)
      TIFFClose(tif);
  }
  catch (...)  // can't throw, just make sure it doesn't crash us
  {
  }
}
  

// ***************************************************************************
// This function reads in the base TIFF tags that are needed to populate the
// object.
void TIFFImageIFile::setupTIFF() throw (ImageException)
{
  uint16 iphotometric;
  // Added this stuff to make sure the calls to libtiff suceed and to make sure
  // that it's more portable when I go playing on the SPARCS.  BGM 29 Dec 1998
  uint32 _width, _height;
  uint16 _bitspersample, _samplespixel;

  try
  {
    // Now that we're opened, make a check to see if it's tiled or not. If
    // so, throw an exception and abort since this class doesn't handle
    // tiled TIFFs (Hint to the reader, try inheriting from this...)
    if (TIFFIsTiled(tif))
      throw ImageException(IMAGE_INVALID_TYPE_ERR);
    // These flags aren't exactly optional.  You need these to describe the
    // image.  If they're not there, throw an exception and abort load.
    if (TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &_width) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    if (TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &_height) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    if (TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &iphotometric) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    if (iphotometric == PHOTOMETRIC_PALETTE)
      if (TIFFGetField(tif, TIFFTAG_COLORMAP, &red, &green, &blue) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    if (TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &_bitspersample) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    if (TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &_samplespixel) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    if (TIFFGetField(tif, TIFFTAG_COMPRESSION, &Compression) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    if (TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarconfig) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    if (TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rows_strip) == 0)
      throw ImageException(IMAGE_FILE_OPEN_ERR);

    Width           = _width;
    Height          = _height;
    bitsPerSample   = _bitspersample;
    samplesPerPixel = _samplespixel;

    // Check to see that if it's a palette color image, that the bits/sample
    // are 8.  TIFF supports a bits/sample of 1 to 8 bits for palette color
    // images, but we're going to wuss out for now and only handle 1, 4, 
    // or 8 bps palette color TIFF's
    if ( (iphotometric == PHOTOMETRIC_PALETTE) && 
        ((bitsPerSample != 8) && (bitsPerSample != 4) && (bitsPerSample != 1)))
      throw ImageException(IMAGE_INVALID_TYPE_ERR);


    // Go through and set the photometric of the image
    switch (iphotometric)
    {
    case PHOTOMETRIC_MINISWHITE:
      Photometric = PHOTO_MINISWHITE;
      break;
    case PHOTOMETRIC_MINISBLACK:
      Photometric = PHOTO_MINISBLACK;;
      break;
    case PHOTOMETRIC_RGB:
      Photometric = PHOTO_RGB;
      break;
    case PHOTOMETRIC_PALETTE:
      Photometric = PHOTO_PALETTE;
      setHasPalette();
      break;
    default:
      Photometric = PHOTO_UNKNOWN;
    }
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }
}


// ***************************************************************************
// If we get here, the compression tag should be set so just return it to the
// caller.
bool TIFFImageIFile::getCompression(uint16& incompression)
  throw (ImageException)
{
  incompression = Compression;
  return true;
}


// ***************************************************************************
// Query the TIFF for the resolution.  If it's not there, return false to the
// caller
bool TIFFImageIFile::getXResolution(float& inxdpi)
  throw (ImageException)
{
  return getTag(TIFFTAG_XRESOLUTION, inxdpi);
}


// ***************************************************************************
// Same as for getXResolution. 
bool TIFFImageIFile::getYResolution(float& inydpi)
  throw (ImageException)
{
  return getTag(TIFFTAG_YRESOLUTION, inydpi);
}


// ***************************************************************************
bool TIFFImageIFile::getResUnits(uint16& inresunits)
  throw (ImageException)
{
  return getTag(TIFFTAG_RESOLUTIONUNIT, inresunits);
}


// ***************************************************************************
bool TIFFImageIFile::getSubfileType(uint32& intype)
  throw (ImageException)
{
  return getTag(TIFFTAG_SUBFILETYPE, intype);
}

// ***************************************************************************
bool TIFFImageIFile::getPlanarConfig(uint16& inconfig)
  throw (ImageException)
{
  inconfig = planarconfig;
  return true;
}


// ***************************************************************************
bool TIFFImageIFile::getRowsPerStrip(uint32& inrows)
  throw (ImageException)
{
  //  return getTag(TIFFTAG_ROWSPERSTRIP, inrows);
  inrows = rows_strip;
  return true;
}


// **************************************************************************
bool TIFFImageIFile::getImageDescription(std::string& indesc)
	throw (ImageException)
{
  return getTag(TIFFTAG_IMAGEDESCRIPTION, indesc);
}


// **************************************************************************
bool TIFFImageIFile::getDateTime(std::string& intime)
  throw (ImageException)
{
  return getTag(TIFFTAG_DATETIME, intime);
}


// ************************************************************************
bool TIFFImageIFile::getCopyright(std::string& incopy)
  throw (ImageException)
{
  return getTag(TIFFTAG_COPYRIGHT, incopy);
}


// ************************************************************************
bool TIFFImageIFile::getFillOrder(uint16& infillorder)
  throw (ImageException)
{
  return getTag(TIFFTAG_FILLORDER, infillorder);
}

// ************************************************************************
bool TIFFImageIFile::getDocumentName(std::string& inname)
  throw (ImageException)
{
  return getTag(TIFFTAG_DOCUMENTNAME, inname);
}

// ************************************************************************
bool TIFFImageIFile::getMake(std::string& inmake)
  throw (ImageException)
{
  return getTag(TIFFTAG_MAKE, inmake);
}

// ************************************************************************
bool TIFFImageIFile::getModel(std::string& inmodel)
  throw (ImageException)
{
  return getTag(TIFFTAG_MODEL, inmodel);
}

// ************************************************************************
bool TIFFImageIFile::getOrientation(uint16& inorientation)
  throw (ImageException)
{
  return getTag(TIFFTAG_ORIENTATION, inorientation);
}

// ************************************************************************
bool TIFFImageIFile::getMinSampleValue(uint16& invalue)
  throw (ImageException)
{
  return getTag(TIFFTAG_MINSAMPLEVALUE, invalue);
}

// ************************************************************************
bool TIFFImageIFile::getMaxSampleValue(uint16& invalue)
  throw (ImageException)
{
  return getTag(TIFFTAG_MAXSAMPLEVALUE, invalue);
}

// ************************************************************************
bool TIFFImageIFile::getPageName(std::string& inname)
  throw (ImageException)
{
  return getTag(TIFFTAG_PAGENAME, inname);
}

// ************************************************************************
bool TIFFImageIFile::getPageNumber(uint16& innumber)
  throw (ImageException)
{
  return getTag(TIFFTAG_PAGENUMBER, innumber);
}

// ************************************************************************
bool TIFFImageIFile::getSoftware(std::string& insoftware)
  throw (ImageException)
{
  return getTag(TIFFTAG_SOFTWARE, insoftware);
}

// ************************************************************************
bool TIFFImageIFile::getArtist(std::string& inartist)
  throw (ImageException)
{
  return getTag(TIFFTAG_ARTIST, inartist);
}

// ************************************************************************
bool TIFFImageIFile::getHostComputer(std::string& incomputer)
  throw (ImageException)
{
  return getTag(TIFFTAG_HOSTCOMPUTER, incomputer);
}

// ************************************************************************
bool TIFFImageIFile::getDotRange(uint16& inone, uint16& intwo)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_DOTRANGE, &inone, &intwo);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ************************************************************************
bool TIFFImageIFile::getExtraSamples(uint16& innum, uint16* inarr)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &innum, &inarr);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ************************************************************************
bool TIFFImageIFile::getHalftoneHints(uint16& inhighlight, uint16& inshadow)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_HALFTONEHINTS, &inhighlight, &inshadow);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ************************************************************************
bool TIFFImageIFile::getJPEGTables(u_short& incount, void* intable)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_JPEGTABLES, &incount, &intable);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ************************************************************************
bool TIFFImageIFile::getPageNumber(uint16& innum, uint16& intotal)
  throw(ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_PAGENUMBER, &innum, &intotal);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ************************************************************************
bool TIFFImageIFile::getSubIFD(uint16& incount, uint32* array)
  throw(ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_SUBIFD, &incount, &array);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
} 


// ************************************************************************
// When calling this function, make sure that you only use the valid data
// arrays.  Read the TIFF standard for that this means.  If you don't know
// what this means, don't mess with this :)
bool TIFFImageIFile::getTransferFunction(uint16* arr1, uint16* arr2,
                                         uint16* arr3)
  throw(ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_TRANSFERFUNCTION, &arr1, &arr1, &arr3);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
} 


// ************************************************************************
bool TIFFImageIFile::getYCBCRSubsampling(uint16& inhoriz, uint16& invert)
  throw(ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_YCBCRSUBSAMPLING, &inhoriz, &invert);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
} 


// ************************************************************************
bool TIFFImageIFile::getICCProfile(uint32& count, void* data)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, TIFFTAG_ICCPROFILE, &count, &data);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
} 

// ************************************************************************
bool TIFFImageIFile::getTag(ttag_t tag, uint32& val)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, tag, &val);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ************************************************************************
bool TIFFImageIFile::getTag(ttag_t tag, uint16& val)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, tag, &val);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***********************************************************************
bool TIFFImageIFile::getTag(ttag_t tag, uint32** val)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, tag, val);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ************************************************************************
bool TIFFImageIFile::getTag(ttag_t tag, uint16** val)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, tag, val);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ************************************************************************
bool TIFFImageIFile::getTag(ttag_t tag, float& val)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, tag, &val);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ************************************************************************
bool TIFFImageIFile::getTag(ttag_t tag, float** val)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, tag, val);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// *************************************************************************
bool TIFFImageIFile::getTag(ttag_t tag, std::string& val)
	throw (ImageException)
{
  char* tempstr;

  try
  {
    if (TIFFGetField(tif, tag, &tempstr))
    {
      val = std::string(tempstr);

      return true;
    }
    else
      return false;
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// *************************************************************************
bool TIFFImageIFile::getTag(ttag_t tag, u_char*** val)
  throw (ImageException)
{
  try
  {
    return TIFFGetField(tif, tag, val);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// *************************************************************************
void TIFFImageIFile::getPalette(Palette* inpalette)
	throw (ImageException, std::bad_alloc)
{
  RGBPixel* rgb_pixel;
  RGBPalette* tpalette = (RGBPalette*)inpalette;

  // Can't get a palette where one doesn't exist....
  if (Photometric != PHOTO_PALETTE)
    throw ImageException(IMAGE_INVALID_OP_ERR);

  if (!(rgb_pixel = new(std::nothrow) RGBPixel[256]))
    throw std::bad_alloc();

  try
  {
    for(int x = 0; x < 256; x++)
    {
      rgb_pixel[x].setRed(red[x] / 256);
      rgb_pixel[x].setGreen(green[x] / 256);
      rgb_pixel[x].setBlue(blue[x] / 256);
    }

    tpalette->setPalette(256, rgb_pixel);

    delete [] rgb_pixel;
    return;
  }
  catch (...)
  {
    setFailRBit();
    delete [] rgb_pixel;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
void TIFFImageIFile::getRawPixel(long x, long y, unsigned char* inarray)
  throw (ImageException, std::bad_alloc)
{
  if (!validateXY(x, y))
    throw ImageException(IMAGE_BOUNDING_ERR);

  unsigned char* tbuffer;
  
  if (!(tbuffer = new(std::nothrow) 
        unsigned char[samplesPerPixel * Width]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(y, tbuffer);

    if (bitsPerSample == 8) // normal 8bit tiff, just put it into the array
      memcpy(inarray, &(tbuffer[x * samplesPerPixel]), samplesPerPixel);
    else if (bitsPerSample == 4) // only return the first 4 bits
    {
      long int temp1, temp2;

      for (int offset = 0; offset < samplesPerPixel; offset++)
      {
        temp1 = x / 2;
        temp2 = x % 2;
        if (temp2 == 0)
          inarray[offset] = tbuffer[temp1 * samplesPerPixel + offset] >> 4;
        else
          inarray[offset] = tbuffer[temp1 * samplesPerPixel + offset] & 0x0f;
      }
    }
    delete [] tbuffer;
  }
  catch (...)
  {
    delete [] tbuffer;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// **************************************************************************
// This function gets the TIFF scanline as is from the TIFF library and does
// not encode it with a palette or anything.  For RGB TIFF's, it'll return
// the actual values of the scanline so the caller will need to take that into
// account when they use this function.
// Modified this function to handle the case when the TIFF's have multi-
// plane data. BGM 29 Dec 1998
// Modified this to use TIFFReadEncoded scanline since TIFFReadScanline can't
// handle quite a few things. BGM 04 July 2001
void TIFFImageIFile::getRawScanline(long int row, unsigned char* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned char* tbuffer = 0;
  long int tstrip = static_cast<long int>(row / rows_strip);
  long int tstart = tstrip * rows_strip;
  long int stripsize = TIFFStripSize(tif);
  long int twidth = static_cast<long int>(std::ceil(
                              static_cast<float>(stripsize) / rows_strip));
  // Twas a bug here that showed itself with multiple rows/strip.  Took out
  // the extra samplesPerPixel BGM 19 Jan 2005
  long int seekpos = twidth * (row - tstart);

  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  try
  {
    if (planarconfig == PLANARCONFIG_CONTIG)
    {
      if (!(tbuffer = new(std::nothrow) unsigned char[stripsize]))
        throw std::bad_alloc();

      if (TIFFReadEncodedStrip(tif, tstrip, tbuffer, -1) == -1)
        throw ImageException(IMAGE_FILE_READ_ERR);

      switch (bitsPerSample)
      {
      case 8: // Easiest case, can just memcpy the data
        {
          memcpy(inarray, tbuffer + seekpos, twidth);
          break;
        }
      case 4:
        {
          unpack4(inarray, tbuffer + seekpos);;
          break;
        }
      case 1:
        {
          unpack1(inarray, tbuffer + seekpos);
      
          break;
        }
      }
      
      delete [] tbuffer;
      return;
    }
    else if (planarconfig == PLANARCONFIG_SEPARATE)
    {
      uint16 sampleloop;
      for (sampleloop = 0; sampleloop < samplesPerPixel; sampleloop++)
        if (TIFFReadScanline(tif, inarray, row, sampleloop) == -1)
          throw ImageException(IMAGE_FILE_READ_ERR);
    }
    else
      throw ImageException(IMAGE_INVALID_OP_ERR);
  }
  catch (...)
  {
    setFailRBit();
    delete [] tbuffer;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// *************************************************************************
// This version of the function is the more generic one.  It mimics the TIFF
// library's call and allows the user to read different data types (ie, the
// user can read 16 bit TIFF's and what not.
// This function will just get the data and copy it over.  It won't do any
// conversions at all, that's up to the user.  This is so that it can handle
// things like 16 bit TIFF's.  It does still use ReadEncodedStrip like the
// above, though.
void TIFFImageIFile::getRawScanline(long int row, tdata_t inarray)
  throw (ImageException, std::bad_alloc)
{
  tdata_t* tbuffer = 0;
  uint32 _row(row);
  long int tstrip = static_cast<long int>(row / rows_strip);
  long int tstart = tstrip * rows_strip;
  long int stripsize = TIFFStripSize(tif);
  long int twidth = static_cast<long int>(std::ceil(
                              static_cast<float>(stripsize) / rows_strip));
  // As with the above, took out the extra samplesPerPixel.  BGM 19 Jan 2005
  long int seekpos = twidth * (row - tstart);

  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  try
  {
    if (planarconfig == PLANARCONFIG_CONTIG)
    {
      if (!(tbuffer = new(std::nothrow) tdata_t[stripsize]))
        throw std::bad_alloc();
      
      if (TIFFReadEncodedStrip(tif, tstrip, inarray, -1) == -1)
        throw ImageException(IMAGE_FILE_READ_ERR);

      memcpy(inarray, tbuffer + seekpos, twidth);
      
      delete [] tbuffer;
      return;
    }
    else if (planarconfig == PLANARCONFIG_SEPARATE)
    {
      uint16 sampleloop;
      for (sampleloop = 0; sampleloop < samplesPerPixel; sampleloop++)
        if (TIFFReadScanline(tif, inarray, _row, sampleloop) == -1)
          throw ImageException(IMAGE_FILE_READ_ERR);
    }
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
// As usual, this assumes the user has already allocated memory and is passing
// it into this function.
void TIFFImageIFile::getRawRectangle(long int x1, long int y1,
                                     long int x2, long int y2,
                                     unsigned char* inarray)
  throw (ImageException, std::bad_alloc)
{
  if (!validateXY(x1, y1) || !validateXY(x2, y2))
    throw ImageException(IMAGE_BOUNDING_ERR);
  
  long int rwidth, rheight; // width and height of the rectangle
  long int loopcount;       // to loop through scanlines
  long int poscount;        // position within the passed in array
  unsigned char* linepos;   // to hold the temp position in the returned array
  unsigned char* rectpos;   // our position in the passed in array
  unsigned char* templine;  // to hold the actual temporary scanline

  if (!(templine = new(std::nothrow) 
        unsigned char[samplesPerPixel * Width]))
    throw std::bad_alloc();

  rheight = y2 - y1 + 1;
  // Changed this to multiply by samplesPerPixel in case it's more than one
  // SPP.  BGM 7 Sept 2001.
  rwidth = (x2 - x1 + 1) * samplesPerPixel;

  poscount = 0;
  try
  {
    for (loopcount = y1; loopcount <= y2; loopcount++)
    {
      getRawScanline(loopcount, templine);
      // Also changed this since it would get the incorrect array position if
      // SPP > 1.  BGM 7 Sept 2001
      linepos = &(templine[x1 * samplesPerPixel]); // get our position in the
                                                   // scanline
      rectpos = &(inarray[poscount]); // get our position in the rectangle
      memcpy(rectpos, linepos, rwidth);
      poscount += rwidth; // go to the next block in the rectangle array 
                          // (treat it as a one dimensional array)
    }
    
    delete [] templine;

    return;
  }
  catch (...)
  {
    setFailRBit();
    delete [] templine;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
void TIFFImageIFile::getPixel(long int x, long int y, AbstractPixel* inpixel)
  throw (ImageException, std::bad_alloc)
{
  // If it's a palette color image, return the colors from the palette for
  // this index.  Otherwise, go get the raw data and set it
  try
  {
    if (Photometric == PHOTO_PALETTE)
    {
      unsigned char tpixel; // temporarily pixel
      
      getRawPixel(x, y, &tpixel);
      inpixel->setRed(red[tpixel] / 256);
      inpixel->setGreen(green[tpixel] / 256);
      inpixel->setBlue(blue[tpixel] / 256);
      return;
    }
    else if ( (Photometric == PHOTO_GREY) || (Photometric == PHOTO_MINISWHITE))
    {
      unsigned char tpixel;
      getRawPixel(x, y, &tpixel);
      inpixel->setGrey(tpixel);
      return;
    }
    else if (Photometric == PHOTO_RGB)
    {
      unsigned char tpixel[3];
      getRawPixel(x, y, tpixel);
      inpixel->setRed(tpixel[0]);
      inpixel->setGreen(tpixel[1]);
      inpixel->setBlue(tpixel[2]);
      return;
    }
  }
  catch (...)
  {
    setFailRBit();
    throw;
  }
}


// **************************************************************************
void TIFFImageIFile::getScanline(long int row, GreyPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  long int loopcount;
  RGBPixel* temprgb;
  unsigned short greyval;

  if (!(temprgb = new(std::nothrow) RGBPixel[Width]))
    throw std::bad_alloc();

  try
  {
    getScanline(row, temprgb);
    for (loopcount = 0; loopcount < Width; loopcount++)
    {
      temprgb[loopcount].getGrey(greyval);
      inarray[loopcount].setGrey(greyval);
    }
    delete [] temprgb;
    return;
  }
  catch (...)
  {
    delete [] temprgb;
    throw;
  }
}


// ***************************************************************************
void TIFFImageIFile::getScanline(long int row, RGBPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned char* tline;  // to hold the scanline
  long int loopcount;

  if (!(tline = new(std::nothrow) unsigned char[samplesPerPixel * Width]))
    throw std::bad_alloc();

  try
  {
    if (Photometric == PHOTO_PALETTE)  // need a palette to do this
    {
      getRawScanline(row, tline);
      if (bitsPerSample == 8) // just set things
      {
        for (loopcount = 0; loopcount < Width; loopcount++)
        {
          inarray[loopcount].setRed(red[tline[loopcount]] / 256);
          inarray[loopcount].setGreen(green[tline[loopcount]] / 256);
          inarray[loopcount].setBlue(blue[tline[loopcount]] / 256);
        }
      }
      else if (bitsPerSample == 4) // handle 4bit encoding
      {
        long int poscount = 0;
        for (loopcount = 0; loopcount < (Width - 1); loopcount += 2)
        {
          inarray[loopcount].setRed(red[(tline[poscount] >> 4)] / 256);
          inarray[loopcount].setGreen(green[(tline[poscount] >> 4)] / 256);
          inarray[loopcount].setBlue(blue[(tline[poscount] >> 4)] / 256);
          inarray[loopcount + 1].setRed(red[(tline[poscount] & 0x0f)] / 256);
          inarray[loopcount + 1].setGreen(green[(tline[poscount] & 0x0f)]
                                                / 256);
          inarray[loopcount + 1].setBlue(blue[(tline[poscount] & 0x0f)] / 256);
          poscount++;
        }
      }
      
      delete [] tline;
      return;
    }
    // Assume a GreyPixel* for this one
    else if ((Photometric == PHOTO_GREY) || (Photometric == PHOTO_MINISWHITE))
    {
      getRawScanline(row, tline);
      if (bitsPerSample == 8) // just loop through and set the values
      {
        for (loopcount = 0; loopcount < Width; loopcount++)
        {
          inarray[loopcount].setGrey(tline[loopcount]);
        }
      }
      // *********************************************************************
      // Note:  I'm not quite sure you can even have this case BGM
      else if (bitsPerSample == 4) // handle for 4 bit tiff's
      {
        long int poscount = 0;
        for (loopcount = 0; loopcount < (Width - 1); loopcount += 2)
        {
          inarray[loopcount].setGrey(tline[poscount] >> 4);
          inarray[loopcount + 1].setGrey(tline[poscount] & 0x0f);
          poscount++;
        }
      }
      delete [] tline;
      return;
    }
    // ***********************************************************************
    // Am assuming you'll only see 8 bit, 3 bytes per pixel tiff's here
    else if (Photometric == PHOTO_RGB)
    {
      long int poscount = 0;
      getRawScanline(row, tline);
      // just go through and set the pixels
      for (loopcount = 0; loopcount < Width; loopcount++)
      {
        inarray[loopcount].setRed(tline[poscount]);
        inarray[loopcount].setGreen(tline[poscount + 1]);
        inarray[loopcount].setBlue(tline[poscount + 2]);
        poscount += 3;
      }
      delete [] tline;
      return;
    }
  }
  catch (...)
  {
    setFailRBit();
    delete [] tline;
    throw;
  }
}


// **************************************************************************
void TIFFImageIFile::getRectangle(long int x1, long int y1,
                                  long int x2, long int y2,
                                  GreyPixel* inarray)
  throw(ImageException, std::bad_alloc)
{
  long int recwidth, recheight;
  long int recsize;
  long int loopcount;
  unsigned short greyval;
  RGBPixel* temprgb;

  recwidth = x2 - x1 + 1;
  recheight = y2 - y1 + 1;
  recsize = recwidth * recheight;

  if (!(temprgb = new(std::nothrow) RGBPixel[recsize]))
    throw std::bad_alloc();

  try
  {
    getRectangle(x1, y1, x2, y2, temprgb);
    for (loopcount = 0; loopcount < recsize; loopcount++)
    {
      temprgb[loopcount].getGrey(greyval);
      inarray[loopcount].setGrey(greyval);
    }
    delete [] temprgb;
    return;
  }
  catch (...)
  {
    delete [] temprgb;
    throw;
  }
}


// ***************************************************************************
void TIFFImageIFile::getRectangle(long int x1, long int y1,
                                  long int x2, long int y2,
                                  RGBPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  long int recwidth, recheight; // width and height of the rectangle
  long int recsize;             // memory to allocate
  long int loopcount;           // to go through the line
  unsigned char* tbuffer;       // to hold the returned buffer

  recwidth = x2 - x1 + 1;
  recheight = y2 - y1 + 1;
  recsize = recwidth * recheight;

  if (!(tbuffer = new(std::nothrow) unsigned char[recsize * samplesPerPixel]))
    throw std::bad_alloc();

  try
  {
    if (Photometric == PHOTO_PALETTE)
    {
      getRawRectangle(x1, y1, x2, y2, tbuffer);

      if (bitsPerSample == 8) // standard encoding
      {
        for (loopcount = 0; loopcount < recsize; loopcount++)
        {
          inarray[loopcount].setRed(red[tbuffer[loopcount]] / 256);
          inarray[loopcount].setGreen(green[tbuffer[loopcount]] / 256);
          inarray[loopcount].setBlue(blue[tbuffer[loopcount]] / 256);
        }
      }
      else if (bitsPerSample == 4) // handle for 4 bit encoding
      {
        long int tpos = 0;
        for (loopcount = 0; loopcount < (recsize - 1); loopcount += 2)
        {
          inarray[loopcount].setRed(red[(tbuffer[tpos] >> 4)] / 256);
          inarray[loopcount].setGreen(green[(tbuffer[tpos] >> 4)] / 256);
          inarray[loopcount].setBlue(blue[(tbuffer[tpos] >> 4)] / 256);
          inarray[loopcount + 1].setRed(red[(tbuffer[tpos] & 0x0f)] / 256);
          inarray[loopcount + 1].setGreen(green[(tbuffer[tpos] & 0x0f)] / 256);
          inarray[loopcount + 1].setBlue(blue[(tbuffer[tpos] & 0x0f)] / 256);
          tpos++;
        }
      }
      delete [] tbuffer;
      return;
    }
    else if ( (Photometric == PHOTO_GREY) || (Photometric == PHOTO_MINISWHITE))
    {
      getRawRectangle(x1, y1, x2, y2, tbuffer);

      if (bitsPerSample == 8)
      {
        for (loopcount = 0; loopcount < recsize; loopcount++)
        {
          inarray[loopcount].setGrey(tbuffer[loopcount]);
        }
      }
      else if (bitsPerSample == 4)
      {
        long int tpos = 0;
        for (loopcount = 0; loopcount < recsize; loopcount++)
        {
          inarray[loopcount].setGrey(tbuffer[tpos] >> 4);
          inarray[loopcount + 1].setGrey(tbuffer[tpos] & 0x0f);
          tpos++;
        }
      }

      delete [] tbuffer;
      return;
    }
    else if (Photometric == PHOTO_RGB)
    {
      long int tpos = 0;

      getRawRectangle(x1, y1, x2, y2, tbuffer);

      for (loopcount = 0; loopcount < recsize; loopcount++)
      {
        inarray[loopcount].setRed(tbuffer[tpos]);
        inarray[loopcount].setGreen(tbuffer[tpos + 1]);
        inarray[loopcount].setBlue(tbuffer[tpos + 2]);
        tpos += 3;
      }
      delete [] tbuffer;
      return;
    }
  }
  catch (...)
  {
    setFailRBit();
    delete [] tbuffer;
    throw;
  }
}


// ***************************************************************************
// Go through the input array and unpack the 4 bit data into an 8 bit array
// while preserving the value.
void TIFFImageIFile::unpack4(unsigned char* dest, unsigned char* input)
  const throw(ImageException)
{
  long int loopcounter;
  long int inputcounter = 0;
  
  try
  {
    for (loopcounter = 0; loopcounter < Width; loopcounter += 2)
    {
      dest[loopcounter] = 
        static_cast<unsigned char>((input[inputcounter] >> 4) & 0x0f);
      dest[loopcounter + 1] = 
        static_cast<unsigned char>(input[inputcounter] & 0x0f);
      inputcounter++;
    }

    return;
  }
  catch (...)
  {
    throw ImageException(IMAGE_UNPACK_ERR);
  }
}


// ***************************************************************************
// This will convert from 1 to 8 bit.
void TIFFImageIFile::unpack1(unsigned char* dest, unsigned char* input)
  const throw (ImageException)
{
  long int loopcounter;
  long int inputcounter = 0;
  int bitcounter;          // Loopcounter within the byte
  unsigned char tval;
  long int tcounter;

  try
  {
    for (loopcounter = 0; loopcounter < Width; loopcounter += 8)
    {
      tcounter = 0;
      for (bitcounter = 7; bitcounter >= 0; bitcounter--)
      {
        tval = (input[inputcounter] >> bitcounter) & 0x01;
        
        if (tval)
          dest[loopcounter + tcounter] = 255;
        else
          dest[loopcounter + tcounter] = 0;

        tcounter++; // Advance to the next byte in dest
      }
      inputcounter++; // Now advance to the next byte in input
    }
    return;
  }
  catch (...)
  {
    throw ImageException(IMAGE_UNPACK_ERR);
  }
}

#endif
