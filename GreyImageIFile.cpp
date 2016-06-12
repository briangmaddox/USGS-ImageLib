#ifndef _GREYIMAGEIFILE_CPP_
#define _GREYIMAGEIFILE_CPP_
// $Id: GreyImageIFile.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include <string>
#include "GreyImageIFile.h"
#include "GreyPalette.h"

#ifdef _WIN32
#pragma warning( disable : 4291 )  // Disable warning messages
#endif

using namespace USGSImageLib;

// ***************************************************************************
GreyImageIFile::GreyImageIFile(std::string& infile, std::string& hdr)
  throw(ImageException, std::bad_alloc)
  : ImageIFile(infile), headerFile(hdr), datahandler(0)
{
  setImageType(IMAGE_GREY);
  setPhotometric(PHOTO_GREY);

  if (!(Stream = new(std::nothrow) std::ifstream(infile.c_str(), 
                                   std::ios::in | std::ios::binary)))
    throw std::bad_alloc();
  if (Stream->fail())
  {
    delete Stream;
    setNoDataBit();
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }

  try
  {
    readHeader();
  }
  catch (...)
  {
    setFailRBit();
    setNoDataBit();
    delete Stream;
    throw ImageException (IMAGE_FILE_OPEN_ERR);
  }

  if (!(datahandler = new(std::nothrow) RawGreyIDataHandler(Stream, 0, Width,
                                                            Height,
                                                            Photometric)))
  {
    setFailRBit();
    setNoDataBit();
    delete Stream;
    throw std::bad_alloc();
  }

  setBitsPerSample(8);
  setSamplesPerPixel(1);
}

// ****************************************************************************
GreyImageIFile::GreyImageIFile(std::string& infile)
  throw(ImageException, std::bad_alloc)
    : ImageIFile(infile), headerFile(infile)
{
  setImageType(IMAGE_GREY);
  setPhotometric(PHOTO_GREY);
  headerFile = headerFile + std::string(".hdr");

  if (!(Stream = new(std::nothrow) std::ifstream(infile.c_str(), 
                                   std::ios::in | std::ios::binary)))
    throw std::bad_alloc();
  if (Stream->fail())
  {
    delete Stream;
    setNoDataBit();
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }

  try
  {
    readHeader();
  }
  catch (...)
  {
    setFailRBit();
    setNoDataBit();
    delete Stream;
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }

  if (!(datahandler = new(std::nothrow) RawGreyIDataHandler(Stream, 0, Width,
                                                            Height,
                                                            Photometric) ) )
  {
    setFailRBit();
    setNoDataBit();
    delete Stream;
    throw std::bad_alloc();
  }

  setBitsPerSample(8);
  setSamplesPerPixel(1);
}


// ***************************************************************************
GreyImageIFile::~GreyImageIFile()
{
  delete Stream;
  delete datahandler;
}

// ***************************************************************************
void GreyImageIFile::getPalette(Palette* inpalette)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getPalette(inpalette);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR); // to notify caller
  }
}

// ****************************************************************************
void GreyImageIFile::getPixel(long x, long y, AbstractPixel* inpixel)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getPixel(x, y, inpixel);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ***************************************************************************
void GreyImageIFile::getScanline(long int row, RGBPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getScanline(row, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR); // to notify caller
  }
}

// ***************************************************************************
void GreyImageIFile::getScanline(long int row, GreyPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getScanline(row, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR); // to notify caller
  }
}

// ***************************************************************************
void GreyImageIFile::getRectangle(long int x1, long int y1,
                                  long int x2, long int y2,
                                  RGBPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRectangle(x1, y1, x2, y2, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR); // to notify caller
  }
}

// ***************************************************************************
void GreyImageIFile::getRectangle(long int x1, long int y1,
                                  long int x2, long int y2,
                                  GreyPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRectangle(x1, y1, x2, y2, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR); // to notify caller
  }
}


// ***************************************************************************
void GreyImageIFile::getRawPixel(long int x, long int y,
                                 unsigned char* inpixel)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRawPixel(x, y, inpixel);
  }
  catch (...)
  {
    setFailRBit();
    throw; // just notify caller
  }
}


// ***************************************************************************
void GreyImageIFile::getRawScanline(long int row, unsigned char* inarray)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRawScanline(row, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw; // to notify caller
  }
}

// ***************************************************************************
void GreyImageIFile::getRawRectangle(long int x1, long int y1,
                                     long int x2, long int y2,
                                     unsigned char* inarray)
  throw(ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRawRectangle(x1, y1, x2, y2, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw; // notify caller
  }
}


// ****************************************************************************
void GreyImageIFile::readHeader(void)
  throw (ImageException, std::bad_alloc)
{
  std::ifstream* hdrfile;
  long value1, value2;
  std::string key1, key2;
    
  value1 = value2 = 0;
  key1 = key2 = "";

  if (!(hdrfile = new(std::nothrow) std::ifstream(headerFile.c_str(), 
                                                  std::ios::in)))
    throw std::bad_alloc();
  if (hdrfile->fail())
  {
    delete hdrfile;
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }

  try
  {
    *hdrfile >> key1 >> value1;
    *hdrfile >> key2 >> value2;
  }
  catch (...)
  {
    delete hdrfile;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
  if (hdrfile->fail())
  {
    delete hdrfile;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }

  if ( cmp_nocase(key1, std::string("Width:")) == 0)
    Width = value1;
  if ( cmp_nocase(key1, std::string("Height:")) == 0)
    Height = value1;
  if ( cmp_nocase(key2, std::string("Height:")) == 0)
    Height = value2;
  if ( cmp_nocase(key2, std::string("Width:")) == 0)
    Width = value2;

  delete hdrfile;
  return;
}

#ifdef _WIN32
#pragma warning( default : 4291 )  // Disable warning messages
#endif

#endif
