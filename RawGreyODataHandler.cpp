#ifndef _RAWGREYODATAHANDLER_CPP_
#define _RAWGREYODATAHANDLER_CPP_
// $Id: RawGreyODataHandler.cpp,v 1.2 2003-06-28 16:28:41 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:41 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Defines the class RawGreyODataHandler, which handles the output of raw
// Grey data to a file

#include "RawGreyODataHandler.h"

using namespace USGSImageLib;

// ***************************************************************************
RawGreyODataHandler::RawGreyODataHandler(std::ofstream* outstream, 
                                         long int outposition, 
                                         long int outwidth, long int outheight,
                                         int photometric)
  throw(ImageException)
 : RawODataHandler(outstream, outposition, outwidth, outheight, photometric)
{
}


// ***************************************************************************
RawGreyODataHandler::~RawGreyODataHandler()
{
}


// ***************************************************************************
void RawGreyODataHandler::putPixel(AbstractPixel* outpixel, long int x,
                                   long int y)
  throw(ImageException)
{
  unsigned short color;  // actual color of the image
  unsigned char _color;  // color to send to putRawPixel

  try
  {
    GreyPixel* temp = (GreyPixel*)outpixel;
    
    temp->getGrey(color);
    _color = static_cast<unsigned char>(color);

    putRawPixel(&_color, x, y);
  }
  catch (...)
  {
    throw; // so caller can be told something happened
  }
}


// ***************************************************************************
void RawGreyODataHandler::putScanline(RGBPixel* outarray, long int row)
  throw (ImageException, std::bad_alloc)
{
  GreyPixel* tempgrey;
  unsigned short greyval;
  long int loopcount;
  
  if (!(tempgrey = new(std::nothrow) GreyPixel[Width]))
    throw std::bad_alloc();

  try
  {
    for (loopcount = 0; loopcount < Width; loopcount++)
    {
      outarray[loopcount].getGrey(greyval);
      tempgrey[loopcount].setGrey(greyval);
    }
    putScanline(tempgrey, row);
    delete [] tempgrey;
    return;
  }
  catch (...)
  {
    delete [] tempgrey;
    throw; // to notify caller
  }
}


// ***************************************************************************
void RawGreyODataHandler::putScanline(GreyPixel* outarray, long int row)
  throw (ImageException, std::bad_alloc)
{
  long int loopcount;
  unsigned char* line;
  unsigned short _tempshort;

  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();

  // fill the output scanline with the color values
  for (loopcount = 0; loopcount < Width; loopcount++)
  {
    outarray[loopcount].getGrey(_tempshort);
    line[loopcount] = static_cast<unsigned char>(_tempshort);
  }

  try
  {
    putRawScanline(line, row);
    delete [] line;
    return;
  }
  catch (...)
  {
    delete [] line;
    throw; // to notify caller
  }
}


// ***************************************************************************
void RawGreyODataHandler::putRawPixel(unsigned char* outpixel, long int x,
                                      long int y)
  throw(ImageException, std::bad_alloc)
{
  if (!validateXY(x, y))
    throw ImageException(IMAGE_BOUNDING_ERR);

  long int seekval;
  
  seekval = dataposition + (y * Width) + x;
  try
  {
    Stream->seekp(seekval);
    Stream->write(reinterpret_cast<char*>(outpixel), BytesPerPixel);
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
  if (Stream->fail())
  {
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// ***************************************************************************
void RawGreyODataHandler::putRawScanline(unsigned char * outarray, 
                                         long int row)
  throw(ImageException, std::bad_alloc)
{
  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  long int seekval;
  
  seekval = dataposition + row * Width;
  try
  {
    Stream->seekp(seekval);
    Stream->write(reinterpret_cast<char*>(outarray), ByteWidth);
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
  if (Stream->fail())
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}

#endif
