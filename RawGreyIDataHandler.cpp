#ifndef _RAWGREYIDATAHANDLER_CPP_
#define _RAWGREYIDATAHANDLER_CPP_
// $Id: RawGreyIDataHandler.cpp,v 1.2 2003-06-28 16:28:41 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// 28 May 1998
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:41 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Implementation for the RawGreyIDataHandler class.  

#include "RawGreyIDataHandler.h"
#include "GreyPalette.h"
#include "ImageException.h"
#include <vector>

#ifdef _WIN32
#pragma warning( disable : 4291 )  // Disable warning messages
#endif

using namespace USGSImageLib;

// ***************************************************************************
RawGreyIDataHandler::RawGreyIDataHandler(std::ifstream* instream,
                                         long int inposition,
                                         long int inwidth,
                                         long int inheight,
                                         int inphotometric)
  throw(ImageException)
 : RawIDataHandler(instream, inposition, inwidth, inheight, inphotometric)
{
}


// ***************************************************************************
RawGreyIDataHandler::~RawGreyIDataHandler()
{
}


// ***************************************************************************
void RawGreyIDataHandler::getPalette(Palette* inpalette)
  const throw(ImageException, std::bad_alloc)
{
  unsigned long int numentries; // Number of entries in the palette
  long int rowloop;             // Loop through the rows of the image
  long int colloop;             // Loop through the columns of the image
  unsigned long int palloop;    // Palette loop counter
  bool found = false;  // whether or not the specific value was found
  GreyPalette* pal;
  unsigned char* line;
  std::vector<GreyPixel> lpixelbuff; // Vector to hold the entries in
  unsigned short tgrey;     // to hold the result of getGrey
  
  numentries = 0; // No entries initially (duh :)
  
  // Attempt to allocate the memory for a scanline buffer
  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();

  // Now go through and create the palette.  This could take a while
  for (rowloop = 0; rowloop < Height; rowloop++)
  {
    // Grab the actual scanline data
    try
    {
      getRawScanline(rowloop, line);
    }
    catch (...)
    {
      // something happened and we couldn't read the file.  Go ahead and
      // clean up what we can and rethrow the exception to the caller
      delete [] line;    // free the memory
      inpalette = 0; // in case it didn't get initialized
      lpixelbuff.clear();
      throw ImageException(IMAGE_FILE_READ_ERR);  // throw to the caller
    }
      
    for (colloop = 0; colloop < Width; colloop++)
    {
      found = false; // Haven't found the grey level yet
      
      // If the number of entries is 0, we really shouldn't bother searching.
      // (yeah, this is a duh I know) BGM 9 Mar 1999
      if (numentries != 0)
      {
	for (palloop = 0; palloop < numentries; palloop++)
	{
	  lpixelbuff[palloop].getGrey(tgrey);
	  if ( (tgrey == (unsigned short)line[colloop]) )
	    found = true;  // found the greylevel in the palette already
	}
      }

      if (!found)
      {
        // Check to see if we need to resize the palette array
        if (numentries == lpixelbuff.size())
          lpixelbuff.resize(lpixelbuff.size() + 256);

        // Add the entry to the next available element and increment # entries
        lpixelbuff[numentries++].setGrey((unsigned short)line[colloop]);
      } // if (!found)
    } // for (colloop = 0; colloop < Width; colloop++)
  } // for (rowloop = 0; rowloop < Height; rowloop++)

  // Create a new palette and init with our generated one
  if (!(pal = new(std::nothrow) GreyPalette(lpixelbuff)))
  {
    delete [] line;
    inpalette = 0; // in case it didn't get initialized
    lpixelbuff.clear();
    throw std::bad_alloc();
  }
  
  // delete the scanline buffer before we exit the function
  delete [] line;

  inpalette = pal;
  lpixelbuff.clear();
  return;
}


// ***************************************************************************
void RawGreyIDataHandler::getScanline(long int row, RGBPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  // Just make an array of GreyPixels and sent it to the GreyPixel getScanline
  // call.  Then, loop through and get the grey values and set them here
  GreyPixel* tempgrey;
  long int loopcount;
  unsigned short greyval;

  if (!(tempgrey = new(std::nothrow) GreyPixel[Width]))
    throw std::bad_alloc();

  try
  {
    getScanline(row, tempgrey);
    for (loopcount = 0; loopcount < Width; loopcount++)
    {
      tempgrey[loopcount].getGrey(greyval);
      inarray[loopcount].setGrey(greyval);
    }
    delete [] tempgrey;
    return;
  }
  catch (...)
  {
    delete [] tempgrey;
    throw; // to caller
  }
}


// ***************************************************************************
void RawGreyIDataHandler::getScanline(long int row, GreyPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line;
  long int loopcount;
  long int bytejump; // to actually go through the array BytesPerPixel per time

  // Allocate a scanline and make sure the operation succeeded
  if (!(line = new(std::nothrow) unsigned char[ByteWidth]) )
    throw std::bad_alloc();

  try
  {
    getRawScanline(row, line);
  }
  catch (...)
  {
    delete [] line;  // free the memory allocated
    throw; // so the caller can see what happened
  }

  bytejump = 0;
  for (loopcount = 0; loopcount < Width; loopcount++)
  {
    // encode the data in pixels
    inarray[loopcount].setGrey((unsigned short)line[bytejump]); 
    bytejump += BytesPerPixel;
  }

  delete [] line; // free up the memory
}


// ***************************************************************************
// Function to return a pixel into a user allocated variable
void RawGreyIDataHandler::getPixel(long int x, long int y, 
                                   AbstractPixel* inpixel)
  const throw(ImageException)
{
  unsigned char pixel;

  try
  {
    getRawPixel(x, y, &pixel);
  }
  catch (...)
  {
    throw; // just perculate up to caller
  }

  inpixel->setGrey((unsigned short)pixel);
}


// ***************************************************************************
void RawGreyIDataHandler::getRectangle(long int x1, long int y1,
                                       long int x2, long int y2,
                                       RGBPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  GreyPixel* tempgrey;
  unsigned short greyval;
  long int loopcount;
  long int _width, _height, _nelements;

  _width = x2 - x1 + 1;
  _height = y2 - y1 + 1;
  _nelements = _width * _height;
  
  if (!(tempgrey = new(std::nothrow) GreyPixel[_nelements]))
    throw std::bad_alloc();

  try
  {
    getRectangle(x1, y1, x2, y2, tempgrey);
    for (loopcount = 0; loopcount < _nelements; loopcount++)
    {
      tempgrey[loopcount].getGrey(greyval);
      inarray[loopcount].setGrey(greyval);
    }
    delete [] tempgrey;
    return;
  }
  catch (...)
  {
    delete [] tempgrey;
    throw;
  }
}


// ***************************************************************************
void RawGreyIDataHandler::getRectangle(long int x1, long int y1,
                                       long int x2, long int y2,
                                       GreyPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* buffer;
  long int _width;      // width of the rectangle
  long int _height;     // height of the rectangle
  long int _nelements;  // number of elements in the rectangle
  long int loopcount;   // standard loop counter variable
  long int bytejump;    // to go through the scanline

  _width = x2 - x1 + 1;
  _height = y2 - y1 + 1;
  _nelements = _width * _height;

  if (!(buffer = new(std::nothrow) unsigned char[_nelements * BytesPerPixel]) )
    throw std::bad_alloc();

  try
  {
    getRawRectangle(x1, y1, x2, y2, buffer);
  }
  catch (...)
  {
    delete [] buffer;   // free up the allocated memory
    throw; // so the caller knows something happened
  }

  bytejump = 0;
  for (loopcount = 0; loopcount < _nelements; loopcount++)
  {
    inarray[loopcount].setGrey((unsigned short)buffer[bytejump]);
    bytejump += BytesPerPixel;
  }

  delete [] buffer;  // don't need the raw data anymore
}


// ***************************************************************************
void RawGreyIDataHandler::getRawPixel(long int x, long int y,
                                      unsigned char* pixel)
  const throw(ImageException)
{
  long int seekval;

  if (!validateXY(x, y))
    throw ImageException(IMAGE_BOUNDING_ERR);

  seekval = y * ByteWidth + x + dataposition;

  try
  {
    Stream->seekg(seekval);  // go to the data position
    Stream->read(reinterpret_cast<char*>(pixel), BytesPerPixel );
  }
  catch(...) // the stream threw an exception
  {
    throw ImageException(IMAGE_FILE_READ_ERR); // send it up to the caller
  }
  if (Stream->fail()) // stream error that didn't throw an exception
    throw ImageException(IMAGE_FILE_READ_ERR);
}


// ***************************************************************************
void RawGreyIDataHandler::getRawScanline(long int row, unsigned char* inarray)
  const throw(ImageException)
{
  long int seekval;

  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  seekval = row * ByteWidth + dataposition;

  try
  {
    Stream->seekg(seekval);  // go to the data
    Stream->read(reinterpret_cast<char*>(inarray), ByteWidth);
  }
  catch (...) // the stream threw an exception
  {
    throw ImageException(IMAGE_FILE_READ_ERR); // just notify the caller
  }
  if (Stream->fail()) // error happened that didn't throw an exception
    throw ImageException(IMAGE_FILE_READ_ERR);
}


// ***************************************************************************
void RawGreyIDataHandler::getRawRectangle(long int x1, long int y1,
                                          long int x2, long int y2,
                                          unsigned char* inarray)
  const throw(ImageException)
{
  unsigned char* ptr;
  long int length;   // length of the rectangle
  long int ycounter; // counts in the y direction
  long int seekval;  // where to go get the data

  if (!validateXY(x1, y1) || !validateXY(x2, y2))
    throw ImageException(IMAGE_BOUNDING_ERR);

  length = x2 - x1 + 1;

  for (ycounter = y1; ycounter <= y2; ycounter++)
  {
    seekval = (ycounter * ByteWidth) + x1 + dataposition;
    ptr = &(inarray[(ycounter - y1) * length]); // set a pointer in raw array
    try
    {
      Stream->seekg(seekval);
      Stream->read(reinterpret_cast<char*>(ptr), length * BytesPerPixel);
    }
    catch (...)
    {
      throw ImageException(IMAGE_FILE_READ_ERR); // send the stream exception 
                                                 // to the caller
    }
    if (Stream->fail()) // error occurred that didn't throw an exception
    {
      throw ImageException(IMAGE_FILE_READ_ERR);
    }
  }
}

#ifdef _WIN32
#pragma warning( default : 4291 )  // turn it back on
#endif

#endif

