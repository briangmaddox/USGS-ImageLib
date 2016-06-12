#ifndef _TIFFIMAGEOFILE_CPP_
#define _TIFFIMAGEOFILE_CPP_
// $Id: TIFFImageOFile.cpp,v 1.1.1.1 2002-02-21 00:19:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:01 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "TIFFImageOFile.h"
#include "RGBPalette.h"

using namespace USGSImageLib;

// ***********************************************************************
TIFFImageOFile::TIFFImageOFile(std::string& tfilename, long tw, long th,
                               int inphotometric, bool isgeotiff)
  throw (ImageException, std::bad_alloc)
  : ImageOFile(tfilename,tw,th), tif(0), red(0), green(0), blue(0), buffer(0),
    scan_line(0), color_count(0), baselinebits(0), isGeotiff(isgeotiff)
{
  // Set the internal Photometric type so the object knows how to write
  // itself
  Photometric = inphotometric;

  try
  {
    // If it's not a GeoTIFF, go ahead and open it as a normal TIFF file.
    // Otherwise, don't do anything.  GeoTIFFImageOFile will go ahead and
    // create the stuff and call setupTIFF()
    if (!isGeotiff)
    {
      if (!(tif = TIFFOpen(tfilename.c_str(), "w")))
        throw ImageException(IMAGE_FILE_OPEN_ERR);

      setupTIFF();
      setImageType(IMAGE_TIFF);
    }

    // Try to allocate our memory now
    if (!(buffer = new(std::nothrow) unsigned char[tw]))
      throw std::bad_alloc();

    if (!(red = new(std::nothrow) uint16[256]))
      throw std::bad_alloc();
    if (!(green = new(std::nothrow) uint16[256]))
      throw std::bad_alloc();
    if (!(blue = new(std::nothrow) uint16[256]))
      throw std::bad_alloc();

    if (inphotometric == PHOTO_PALETTE)
      setHasPalette();

    color_count = 0;
    
  }
  catch (...)
  {
    // if we get here, something way messed up happened
    setNoDataBit();
    // go ahead and clean up our memory
    if (buffer != 0)
      delete [] buffer;
    if (red != 0)
      delete [] red;
    if (green != 0)
      delete [] green;
    if (blue != 0)
      delete [] blue;

    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }
}   


// ************************************************************************
TIFFImageOFile::~TIFFImageOFile()
{
  // Not going to rethrow here, just catch the exception and deal with it since
  // the object would go away anyway and I DON'T want to deal with global
  // error flag variables
  try
  {
    if (!isGeotiff)
    {
      if (hasPalette())
        TIFFSetField(tif, TIFFTAG_COLORMAP, red, green, blue);
      TIFFClose(tif);
    }
    if (buffer != 0)
      delete [] buffer;
    if (red != 0)
      delete [] red;
    if (green != 0)
      delete [] green;
    if (blue != 0)
      delete [] blue;
  }
  catch (...)
  {
  }
}


// ***************************************************************************
// This function will perform the setup functions needed (create the TIFF 
// object, etc).  This is added since I wanted the ability to do things like
// distribute this but not force people to build libgeotiff.
bool TIFFImageOFile::setupTIFF() throw()
{
  uint32 _imwidth, _imheight;
  uint16 outphoto = 0; // TIFF photometric setting

  try
  {
    // Set these to make absolutely sure libtiff is happy
    _imwidth  = Width;
    _imheight = Height;

    // Since we're using the Image lib's photometric routines, go ahead and
    // map ours to the TIFF lib's
    switch (Photometric)
    {
    case PHOTO_MINISWHITE:
      outphoto = PHOTOMETRIC_MINISWHITE;
      break;
    case PHOTO_MINISBLACK:
      outphoto = PHOTOMETRIC_MINISBLACK;
      break;
    case PHOTO_RGB:
      outphoto = PHOTOMETRIC_RGB;
      break;
    case PHOTO_PALETTE:
      outphoto = PHOTOMETRIC_PALETTE;
      break;
    }

    // These next functions SHOULDN'T throw an exception, but I've never
    // been the trusting type, especially since I've seen lots of wierdness
    // from libtiff  :)  BGM
    if (TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, _imwidth) != 1)
      setNoDataBit();
    else
      setWidthBit();
    if (TIFFSetField(tif, TIFFTAG_IMAGELENGTH, _imheight) != 1)
      setNoDataBit();
    else
      setLengthBit();
    if (TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, outphoto) != 1)
      setNoDataBit();
    else
      setPhotometricBit();

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// *********************************************************************
// Go through and add the color to the colormap if it's not there, or find 
// the nearest match if the color slots are full
void TIFFImageOFile::updateColormap(RGBPixel* p) throw()
{
  long w; 
  unsigned char index;
  unsigned short tred = 0;
  unsigned short tgreen = 0;
  unsigned short tblue = 0;
  bool found = false;
  unsigned char loop;
  long unsigned int colormatch;


  for(w = 0; w < Width; w++)
  {
    index = 0;
    found = 0;
    p[w].getRed(tred);
    tred *= 256;         // get tiff red of first pixel
    p[w].getGreen(tgreen);
    tgreen *= 256;       // get tiff green of first pixel
    p[w].getBlue(tblue);
    tblue *=  256;       // get tiff blue of first pixel

    while ((index < color_count ) && (!found)) // look for the color in map
    {
      if ((tred  == red[index]) &&    // test to see if this color
          (tgreen == green[index]) && // matches the one in the
          (tblue  == blue[index]))    // color map at position [ind]
        found = true;
      index++; // increment index to move along
    }
    if (found)               // color is in the color table
    {
      buffer[w] = --index;   // put offset into TIFF buffer
    }
    else
      if (index < 256)   // color not in table and table not full
      {
        red[index] = tred;      // set red colormap value
        green[index] = tgreen;  // set green colormap value
        blue[index] = tblue;    // set blue color map value
        color_count++;          // add one to amount of colors
        buffer[w] = index;
      }
      else // color not in table and table full
      {
        index = 0;
        loop = 0;
        // get initial comparison
        colormatch = (tred - red[loop]) * (tred - red[loop]) +
                     (tgreen - green[loop]) * (tgreen - green[loop]) +
                     (tblue - blue[loop]) * (tblue - blue[loop]);
	
        for (loop = 1; loop < color_count; loop++)
        { 
          // find the closest color in the table for a match
          // if a closer match then choose it over the last one.
          if (((tred - red[loop]) * (tred - red[loop]) +
              (tgreen - green[loop]) * (tgreen - green[loop]) +
              (tblue - blue[loop]) * (tblue - blue[loop]))
              < static_cast<long>(colormatch) )
          {
            colormatch = (tred - red[loop]) * (tred - red[loop]) +
                         (tgreen - green[loop]) * (tgreen - green[loop]) +
                         (tblue - blue[loop]) * (tblue - blue[loop]);
            index = loop; // set current color index to current match
          }
        }
        buffer[w] = index;  // set buffer position to point at closest match
      }
  }
  setColormapBit(); // to make everyone happy
}


// ********************************************************************
// This might go away soon since destructor writes and many cases doesn't
// make sense to write early.  Note to self, think of good reason to keep
// this around.
void TIFFImageOFile::writeColormap(void)
  throw (ImageException)
{
  // I use this because to stay consistent, all failed writes throw an
  // exception and it is a bit silly to me to throw within the try block when
  // you go ahead and catch it right away
  bool exceptflag = false;

  // Don't write a colormap if it's not specified as a palette color image
  if (!hasPalette())
    throw ImageException(IMAGE_INVALID_OP_ERR);

  try
  {
    if (TIFFSetField(tif, TIFFTAG_COLORMAP, red, green, blue) != 1)
    {
      setFailWBit();
      exceptflag = true;
    }
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }

  if (exceptflag)
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}


// *********************************************************************
// This can't be done with libtiff, sorry
void TIFFImageOFile::putPixel(long int x, long int y, AbstractPixel* outpixel) 
  throw (ImageException, std::bad_alloc)
{
  setFailWBit();
  throw ImageException(IMAGE_INVALID_OP_ERR);
}


// ***************************************************************************
void TIFFImageOFile::putScanline(long int row, GreyPixel* outarray)
  throw (ImageException, std::bad_alloc)
{
  RGBPixel* temprgb;
  unsigned short greyval;
  long int loopcount;

  if (!(temprgb = new(std::nothrow) RGBPixel[Width]))
    throw std::bad_alloc();

  try
  {
    for (loopcount = 0; loopcount < Width; loopcount++)
    {
      outarray[loopcount].getGrey(greyval);
      temprgb[loopcount].setGrey(greyval);
    }
    putScanline(row, temprgb);
    delete [] temprgb;
    return;
  }
  catch (...)
  {
    delete [] temprgb;
    throw;
  }
}


// *********************************************************************
// Okie, if we've marked the image as having a palette, go ahead and build
// a colormap (if it doesn't already exist) and add the entries.  Otherwise,
// build an array if it's a RGB image or greyscale, fill it with the RGB 
// entries from the RGBPixel* array sent in, and just delegate it to
// putRawScanline.
void TIFFImageOFile::putScanline(long int row, RGBPixel* outarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned char* ucbuffer;
  long int bytejump;
  unsigned short ired, igreen, iblue, igrey;
  bool exceptflag = false; // do we throw an exception or not

  // Do some initial checking.  First check to make sure the baseline tags
  // are satisfied for all image types, then under each specific type check
  // to make sure their own dependencies are satisfied
  if (baselinebits < 511) // core tags not set
    throw ImageException(IMAGE_INVALID_OP_ERR);

  if (hasPalette()) // Assume if it's a palette, it's an RGB Image
  {
    if (baselinebits < 2047)
      throw ImageException(IMAGE_INVALID_OP_ERR);

    try
    {
      updateColormap(outarray);  // updates the colormap and fills
                                 // the buffer
      if (TIFFWriteScanline(tif, buffer, row, 0) != 1)
      {
        exceptflag = true;
        setFailWBit();
      }
      return;
    }
    catch (...)
    {
      setFailWBit();
      throw (ImageException(IMAGE_FILE_WRITE_ERR));
    }
  }
  else if (Photometric == PHOTO_RGB)
  {
    if ( (baselinebits < 1023) && ( (baselinebits | TIFFSAMPLEPIXEL) != 1) )
      throw ImageException(IMAGE_INVALID_OP_ERR);

    if (!(ucbuffer = new(std::nothrow) unsigned char[Width * 3]))
      throw std::bad_alloc();

    bytejump = 0;

    try
    {
      for (int tcount = 0; tcount < Width; tcount++)
      {
        outarray[tcount].getRed(ired);
        outarray[tcount].getGreen(igreen);
        outarray[tcount].getBlue(iblue);
        ucbuffer[bytejump]     = static_cast<unsigned char>(ired);
        ucbuffer[bytejump + 1] = static_cast<unsigned char>(igreen);
        ucbuffer[bytejump + 2] = static_cast<unsigned char>(iblue);
        bytejump += 3;
      }

      // now go ahead and let putRawScanline try to do it
      putRawScanline(row, (void*)ucbuffer);

      delete [] ucbuffer; // free up the memory
      return;
    }
    catch (...)
    {
      delete [] buffer;
      setFailWBit();
      throw ImageException (IMAGE_FILE_WRITE_ERR);
    }
  }
  else // if it's not a RGB image, assume greyscale data to write
  {
    if (baselinebits < 1023)
      throw ImageException(IMAGE_INVALID_OP_ERR);

    if (!(ucbuffer = new(std::nothrow) unsigned char[Width]))
      throw std::bad_alloc();
    
    try
    {
      for (int tcount = 0; tcount < Width; tcount++)
      {
        outarray[tcount].getGrey(igrey);
        ucbuffer[tcount] = static_cast<unsigned char>(igrey);
      }
      
      putRawScanline(row, ucbuffer);
      
      delete [] ucbuffer;
      return;
    }
    catch (...)
    {
      delete [] ucbuffer;
      setFailWBit();
      throw ImageException(IMAGE_FILE_WRITE_ERR);
    }
  }
  if (exceptflag) // something happened that didn't get caught
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}


// *********************************************************************
void TIFFImageOFile::putRawPixel(long int x, long int y, void* outpixel)
  throw (ImageException, std::bad_alloc)
{
  setFailWBit();
  throw ImageException(IMAGE_INVALID_OP_ERR);
}


// *********************************************************************
void TIFFImageOFile::putRawScanline(long int row, void* outarray)
  throw (ImageException, std::bad_alloc)
{
  bool exceptflag = false;

  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  uint32 _trow = row;

  try
  {
    if (TIFFWriteScanline(tif, outarray, _trow, 0) != 1)
    {
      setFailWBit();
      exceptflag = true;
    }
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException (IMAGE_FILE_WRITE_ERR);
  }

  if (exceptflag) // something happened that we didn't catch
    throw ImageException (IMAGE_FILE_WRITE_ERR);
}


// *********************************************************************
void TIFFImageOFile::putRawRectangle(long int x1, long int y1,
                                     long int x2, long int y2,
                                     void* outarray)
   throw (ImageException, std::bad_alloc)
{
  setFailWBit();
  throw ImageException(IMAGE_INVALID_OP_ERR);
}


// *********************************************************************
void TIFFImageOFile::setPalette(Palette* p)
  throw (ImageException, std::bad_alloc)
{
  int x;
  RGBPalette* pal = (RGBPalette*)p;
  int no_colors;
  bool exceptflag = false;
  uint16 tred, tgreen, tblue; // to hold the values
  no_colors = pal->getNoEntries();

  color_count = no_colors;
  try
  {
    for (x = 0; x < no_colors; x++)
    {
      (*pal)[x].getRed(tred);
      (*pal)[x].getGreen(tgreen);
      (*pal)[x].getBlue(tblue);
      red[x]   = tred * 256; 
      green[x] = tgreen * 256;
      blue[x]  = tblue * 256;
    }
    if(TIFFSetField(tif, TIFFTAG_COLORMAP, red, green, blue) != 1)
    {
      setFailWBit();
      exceptflag = true;
    }
    else
      setColormapBit();
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
  if (exceptflag)
    throw ImageException(IMAGE_FILE_WRITE_ERR);

  setColormapBit(); // since we set it successfully, go ahead and mark it
  setHasPalette(); // tell the Image lib it has a palette
}


// *********************************************************************
bool TIFFImageOFile::setCompression (uint16 incompression)
  throw (ImageException)
{
  if (!setTag(TIFFTAG_COMPRESSION, incompression))
    return false;

  setCompressionBit();
  return true;
}


// *********************************************************************
bool TIFFImageOFile::setXResolution(float xdpi)
  throw (ImageException)
{
  if (!setTag(TIFFTAG_XRESOLUTION, xdpi))
    return false;

  setXResBit();
  return true;
}


// *********************************************************************
bool TIFFImageOFile::setYResolution(float ydpi)
  throw (ImageException)
{
  if (!setTag(TIFFTAG_YRESOLUTION, ydpi))
    return false;

  setYResBit();
  return true;
}


// *********************************************************************
bool TIFFImageOFile::setResUnits(uint16 inresunits)
  throw(ImageException)
{
  if (!setTag(TIFFTAG_RESOLUTIONUNIT, inresunits))
    return false;

  setResUnitBit();
  return true;
}


// ********************************************************************
bool TIFFImageOFile::setSubfileType(uint32 intype)
  throw(ImageException)
{
  if (!setTag(TIFFTAG_SUBFILETYPE, intype))
    return false;

  setSubfileBit();
  return true;
}


// ***********************************************************************
bool TIFFImageOFile::setPlanarConfig(uint16 inconfig)
  throw (ImageException)
{
  if (!setTag(TIFFTAG_PLANARCONFIG, inconfig))
    return false;

  setPlanarConfigBit();
  return true;
}


// *********************************************************************
bool TIFFImageOFile::setRowsPerStrip(uint32 inrows)
  throw(ImageException)
{
  if (!setTag(TIFFTAG_ROWSPERSTRIP, inrows))
    return false;

  setRowsStripBit();
  return true;
}


// **********************************************************************
bool TIFFImageOFile::setImageDescription(std::string indesc)
  throw (ImageException)
{
  if (!setTag(TIFFTAG_IMAGEDESCRIPTION, indesc))
    return false;

  setImageDescriptBit();
  return true;
}

// ***********************************************************************
bool TIFFImageOFile::setDateTime(std::string intime)
  throw (ImageException)
{
  if (!setTag(TIFFTAG_DATETIME, intime))
    return false;

  setDateTimeBit();
  return true;
}


// ************************************************************************
bool TIFFImageOFile::setBitsPerSample(int inbps)
  throw (ImageException)
{
  bitsPerSample = inbps;

  if (!setTag(TIFFTAG_BITSPERSAMPLE, static_cast<uint16>(inbps)))
    return false;

  setBitsSampleBit();
  return true;
}


// ***********************************************************************
bool TIFFImageOFile::setSamplesPerPixel(int insps)
  throw(ImageException)
{
  samplesPerPixel = insps;
 
  if (!setTag(TIFFTAG_SAMPLESPERPIXEL, static_cast<uint16>(insps)))
    return false;

  setSamplesPixelBit();
  return true;
}

// ***********************************************************************
bool TIFFImageOFile::setDocumentName(std::string indocname)
  throw (ImageException)
{
  return setTag(TIFFTAG_DOCUMENTNAME, indocname);
}

// ***********************************************************************
bool TIFFImageOFile::setMake(std::string inmake)
  throw (ImageException)
{
  return setTag(TIFFTAG_MAKE, inmake);
}

// ***********************************************************************
bool TIFFImageOFile::setModel(std::string inmodel)
  throw (ImageException)
{
  return setTag(TIFFTAG_MODEL, inmodel);
}

// ***********************************************************************
bool TIFFImageOFile::setOrientation(uint16 inorientation)
  throw (ImageException)
{
  return setTag(TIFFTAG_ORIENTATION, inorientation);
}

// ***********************************************************************
bool TIFFImageOFile::setMinSampleValue(uint16 invalue)
  throw (ImageException)
{
  return setTag(TIFFTAG_MINSAMPLEVALUE, invalue);
}

// ***********************************************************************
bool TIFFImageOFile::setMaxSampleValue(uint16 invalue)
  throw (ImageException)
{
  return setTag(TIFFTAG_MAXSAMPLEVALUE, invalue);
}

// ***********************************************************************
bool TIFFImageOFile::setPageName(std::string inname)
  throw (ImageException)
{
  return setTag(TIFFTAG_PAGENAME, inname);
}

// ***********************************************************************
bool TIFFImageOFile::setPageNumber(uint16 innumber, uint16 total)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_PAGENUMBER, innumber, total);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}

// ***********************************************************************
bool TIFFImageOFile::setSoftware(std::string insoftware)
  throw (ImageException)
{
  return setTag(TIFFTAG_SOFTWARE, insoftware);
}

// ***********************************************************************
bool TIFFImageOFile::setArtist(std::string inartist)
  throw (ImageException)
{
  return setTag(TIFFTAG_ARTIST, inartist);
}

// ***********************************************************************
bool TIFFImageOFile::setHostComputer(std::string incomputer)
  throw (ImageException)
{
  return setTag(TIFFTAG_HOSTCOMPUTER, incomputer);
}

// ***********************************************************************
bool TIFFImageOFile::setDotRange(uint16 ina, uint16 inb)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_DOTRANGE, ina, inb);
  }
  catch(...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}

// ***********************************************************************
bool TIFFImageOFile::setExtraSamples(uint16 count, uint16* arr)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, count, arr);
  }
  catch(...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}

// ***********************************************************************
bool TIFFImageOFile::setHalftoneHints(uint16 ina, uint16 inb)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_HALFTONEHINTS, ina, inb);
  }
  catch(...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// ***********************************************************************
bool TIFFImageOFile::setJPEGTables(uint32 count, void* tables)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_JPEGTABLES, &count, tables);
  }
  catch(...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}

// ***********************************************************************
bool TIFFImageOFile::setSubIFD(uint16 count, uint32* offsets)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_SUBIFD, count, offsets);
  }
  catch(...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}

// ***********************************************************************
bool TIFFImageOFile::setTransferFunction(uint16* arr1, uint16* arr2,
                                         uint16* arr3)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_TRANSFERFUNCTION, arr1, arr2, arr3);
  }
  catch(...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}

// ***********************************************************************
bool TIFFImageOFile::setYCBCRSampling(uint16 inhoriz, uint16 invert)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_YCBCRSUBSAMPLING, inhoriz, invert);
  }
  catch(...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}

// ***********************************************************************
bool TIFFImageOFile::setICCProfile(uint32 count, void* data)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, TIFFTAG_ICCPROFILE, count, data);
  }
  catch(...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// ***********************************************************************
bool TIFFImageOFile::setTag(ttag_t tag, uint32 val)
  throw (ImageException)
{
  try
  {
    return TIFFSetField(tif, tag, val);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// *********************************************************************
bool TIFFImageOFile::setTag(ttag_t tag, uint16 val)
  throw(ImageException)
{
  try
  {
    return TIFFSetField(tif, tag, val);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// ***********************************************************************
bool TIFFImageOFile::setTag(ttag_t tag, float val)
  throw(ImageException)
{
  try
  {
    return TIFFSetField(tif, tag, val);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// *************************************************************************
bool TIFFImageOFile::setTag(ttag_t tag, float* val)
  throw(ImageException)
{
  try
  {
    return TIFFSetField(tif, tag, val);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// ************************************************************************
bool TIFFImageOFile::setTag(ttag_t tag, uint16* val)
  throw(ImageException)
{
  try
  {
    return TIFFSetField(tif, tag, val);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// *********************************************************************
bool TIFFImageOFile::setTag(ttag_t tag, u_char** val)
  throw(ImageException)
{
  try
  {
    return TIFFSetField(tif, tag, val);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// *************************************************************************
bool TIFFImageOFile::setTag(ttag_t tag, std::string val)
  throw(ImageException)
{
  try
  {
    return TIFFSetField(tif, tag, val.c_str());
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


//////////////////////////////////////////////////////////////////////////
// Protected Member Functions
// ***********************************************************************
void TIFFImageOFile::setSubfileBit() throw()
{
  baselinebits = baselinebits | TIFFSUBFILE;
}

// **********************************************************************
void TIFFImageOFile::setWidthBit() throw()
{
  baselinebits = baselinebits | TIFFWIDTH;
}

// **********************************************************************
void TIFFImageOFile::setLengthBit() throw()
{
  baselinebits = baselinebits | TIFFLENGTH;
}

// *********************************************************************
void TIFFImageOFile::setSamplesPixelBit() throw()
{
  baselinebits = baselinebits | TIFFSAMPLEPIXEL;
}

// *********************************************************************
void TIFFImageOFile::setBitsSampleBit() throw()
{
  baselinebits = baselinebits | TIFFBPS;
}

// *********************************************************************
void TIFFImageOFile::setCompressionBit()  throw()
{
  baselinebits |= TIFFCOMPRESSION;
}

// *********************************************************************
void TIFFImageOFile::setPhotometricBit() throw()
{
  baselinebits |= TIFFPHOTOMETRIC;
}

// *********************************************************************
void TIFFImageOFile::setXResBit() throw()
{
  baselinebits = baselinebits | TIFFXRES;
}

// ***********************************************************************
void TIFFImageOFile::setYResBit() throw()
{
  baselinebits = baselinebits | TIFFYRES;
}

// **********************************************************************
void TIFFImageOFile::setResUnitBit() throw()
{
  baselinebits = baselinebits | TIFFRESUNIT;
}

// **********************************************************************
void TIFFImageOFile::setRowsStripBit() throw()
{
  baselinebits = baselinebits | TIFFROWSSTRIP;
}

// *********************************************************************
void TIFFImageOFile::setColormapBit() throw()
{
  baselinebits = baselinebits | TIFFCOLORMAP;
}

// **********************************************************************
void TIFFImageOFile::setImageDescriptBit() throw()
{
  baselinebits = baselinebits | TIFFIMAGEDESC;
}

// **********************************************************************
void TIFFImageOFile::setDateTimeBit() throw ()
{
  baselinebits |= TIFFDATETIME;
}

// *********************************************************************
void TIFFImageOFile::setPlanarConfigBit()  throw()
{
  baselinebits |= TIFFPLANARCNFG;
}

#endif // #ifndef
