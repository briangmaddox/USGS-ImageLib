#ifndef _RAWRGBIDATAHANDLER_CC_
#define _RAWRGBIDATAHANDLER_CC_
// $Id: RawRGBIDataHandler.cpp,v 1.3 2003-10-06 02:56:03 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-10-06 02:56:03 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Implementation of the RawRGBIDataHandler class

#include "RawRGBIDataHandler.h"
#include "RGBPalette.h"
#include <vector>

#ifdef _WIN32
#pragma warning( disable : 4291 )  // Disable warning messages for new
#endif

using namespace USGSImageLib;

// ***************************************************************************
RawRGBIDataHandler::RawRGBIDataHandler(std::ifstream* instream,
                                       long int inposition,
                                       long int inwidth,
                                       long int inheight,
                                       int inphotometric,
                                       char* inbandcontent)
  throw(ImageException)
  : RawIDataHandler(instream, inposition, inwidth, inheight, inphotometric)
{
  strncpy(bandcontent, inbandcontent, 4);
  if (strcmp(bandcontent, "RGB") == 0)
    rgbordered = true;
}


// ***************************************************************************
RawRGBIDataHandler::~RawRGBIDataHandler()
{
}


// ***************************************************************************
// Here we scan through the file to create the palette.  If it's a big file
// and true color, don't do this, it's for your own good :)
void RawRGBIDataHandler::getPalette(Palette* inpalette)
  const throw(ImageException, std::bad_alloc)
{
  unsigned long int numentries;     // number of entries in the palette
  long int rowloop;                 // loop through the rows of the image
  long int colloop;                 // loop throw the columns of the image
  unsigned long int palloop;        // palette loop counter
  bool found = false;               // cheeser of a flag
  RGBPalette* pal;                 // Pointer for the returned palette
  unsigned char* line;              // temporary scanline array
  std::vector<RGBPixel> pixelbuff;  // vector to hold the entries in
  unsigned short red, green, blue;  // to hold the RGB values to search for

  numentries = 0; // no entries initially

  // Attempt to allocate the memory for a scanline buffer
  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();

  // now go through and create the palette.  This could take a LONG while
  // and if it's a true color image, well, could result in a BIG palette :)
  for (rowloop = 0; rowloop < Height; rowloop++)
  {
    // grab the actual scanline data
    try
    {
      getRawScanline(rowloop, line);
    }
    catch (...)
    {
      // something happened and we couldn't read the file.  Go ahead and clean
      // up what we can and rethrow to notify the caller
      delete [] line;
      inpalette = 0;
      pixelbuff.clear();
      throw ImageException(IMAGE_FILE_READ_ERR);
    }

    // Here we go.  Scan the palette list to see the color is in there, if not
    // add it and loop again.
    for (colloop = 0; colloop < ByteWidth; colloop+=3)
    {
      found = false; // haven't found the triplet yet
      
      if (numentries != 0)
	for (palloop = 0; palloop < numentries; palloop++)
	{
	  // Set the search RGB values
	  pixelbuff[palloop].getRed(red);
	  pixelbuff[palloop].getGreen(green);
	  pixelbuff[palloop].getBlue(blue);
	  if ( (red   == static_cast<unsigned short>(line[colloop])) &&
	       (green == static_cast<unsigned short>(line[colloop + 1])) &&
	       (blue  == static_cast<unsigned short>(line[colloop + 2])) )
	    // color is present
	    found = true;
	}
      
      if (!found) // add it to the palette
      {
        // check to see if we need to resize the palette vector
        if (numentries == pixelbuff.size())
          pixelbuff.resize(numentries + 256);

        // add the entry to the next available slot and increment the number
        // of entries
        pixelbuff[numentries].setRed(line[colloop]);
        pixelbuff[numentries].setGreen(line[colloop + 1]);
        pixelbuff[numentries++].setBlue(line[colloop + 2]);
      } // if (!found)
    } // for (colloop)
  } // for rowloop

  // create a new palette and init with our generated one
  if (!(pal = new(std::nothrow) RGBPalette(pixelbuff)))
  {
    delete [] line;
    inpalette = 0;
    pixelbuff.clear();
    throw std::bad_alloc();
  }

  // delete the scanline buffer before we exit the function
  delete [] line;
  
  inpalette = pal;
  pixelbuff.clear();

  return;
}


// ***************************************************************************
void RawRGBIDataHandler::getScanline(long int row, GreyPixel* inarray)
  const throw(ImageException, std::bad_alloc)
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
void RawRGBIDataHandler::getScanline(long int row, RGBPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line;   // scanline 
  long int loopcount;
  long int bytejump;     // go through BytesPerPixel at a time

  // Allocate a scanline
  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(row, line);

    bytejump = 0;
    // Go through and set the RGB values of the input row
    for (loopcount = 0; loopcount < Width; loopcount++)
    {
      inarray[loopcount].setRed(line[bytejump]);
      inarray[loopcount].setGreen(line[bytejump + 1]);
      inarray[loopcount].setBlue(line[bytejump + 2]);
      bytejump += BytesPerPixel;
    }
    
    delete [] line;

    return;
  }
  catch (...)
  {
    delete [] line; // free memory
    inarray = 0;
    throw; // let caller know something happened
  }
}


// ***************************************************************************
void RawRGBIDataHandler::getPixel(long int x, long int y,
                                  AbstractPixel* inpixel)
  const throw(ImageException)
{
  unsigned char pixel[3]; // 3 components

  try
  {
    getRawPixel(x, y, pixel);
  }
  catch (...)
  {
    throw; // let the caller know something happened
  }  

  inpixel->setRed(pixel[0]);
  inpixel->setGreen(pixel[1]);
  inpixel->setBlue(pixel[2]);
  
  return;
}


// ***************************************************************************
void RawRGBIDataHandler::getRectangle(long int x1, long int y1,
                                      long int x2, long int y2,
                                      GreyPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  RGBPixel* temprgb;
  unsigned short greyval;
  long int loopcount;
  long int _width, _height, _nelements;

  _width = x2 - x1 + 1;
  _height = y2 - y1 + 1;
  _nelements = _width * _height;

  if (!(temprgb = new(std::nothrow) RGBPixel[_nelements]))
    throw std::bad_alloc();

  try
  {
    getRectangle(x1, y1, x2, y2, temprgb);
    for (loopcount = 0; loopcount < _nelements; loopcount++)
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
void RawRGBIDataHandler::getRectangle(long int x1, long int y1,
                                      long int x2, long int y2,
                                      RGBPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* buffer;
  long int _width;       // width of the rectangle
  long int _height;      // height ""   ""
  long int _nelements;   // Number of elements in the rectangle
  long int loopcount;    // loop counter
  long int bytejump;     // go through the scanline

  _width = x2 - x1 + 1;
  _height = y2 - y1 + 1;
  _nelements = _width * _height;

  if (!(buffer = new(std::nothrow) unsigned char[_nelements * BytesPerPixel]))
    throw std::bad_alloc();

  try
  {
    getRawRectangle(x1, y1, x2, y2, buffer);
  }
  catch (...)
  {
    delete [] buffer;  // free the memory
    throw ImageException(IMAGE_FILE_READ_ERR); // notify caller
  }

  bytejump = 0;
  for (loopcount = 0; loopcount < _nelements; loopcount++)
  {
    inarray[loopcount].setRed(buffer[bytejump]);
    inarray[loopcount].setGreen(buffer[bytejump + 1]);
    inarray[loopcount].setBlue(buffer[bytejump + 2]);
    bytejump += 3;
  }

  delete [] buffer;

  return;
}


// ***************************************************************************
void RawRGBIDataHandler::getRedBand(long int row, RGBPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line;
  long int loopcount;
  long int bytejump;

  if (!(line = new(std::nothrow) unsigned char[Width]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(row, line);
  }
  catch (...)
  {
    delete [] line; // clean up after ourselves
    throw;
  }

  bytejump = 0;
  // now go through and copy the red values into the input array
  for (loopcount = 0; loopcount < Width; loopcount++)
  {
    inarray[loopcount].setRed(line[bytejump]);
    bytejump += 3;
  }

  delete [] line;

  return;
}


// ***************************************************************************
void RawRGBIDataHandler::getGreenBand(long int row, RGBPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line;
  long int loopcount;
  long int bytejump;

  if (!(line = new(std::nothrow) unsigned char[Width]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(row, line);
  }
  catch (...)
  {
    delete [] line;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }

  bytejump = 0;
  // copy the green values into the array
  for (loopcount = 0; loopcount < Width; loopcount++)
  {
    inarray[loopcount].setGreen(line[bytejump + 1]);
    bytejump += 3;
  }

  delete [] line;
  return;
}


// ***************************************************************************
void RawRGBIDataHandler::getBlueBand(long int row, RGBPixel* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line;
  long int loopcount;
  long int bytejump = 0;

  if (!(line = new(std::nothrow) unsigned char[Width]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(row, line);
  }
  catch (...)
  {
    delete [] line;
    throw;
  }

  // go through and copy the blue values
  for (loopcount = 0; loopcount < Width; loopcount++)
  {
    inarray[loopcount].setBlue(line[bytejump + 2]);
    bytejump += 3;
  }

  delete [] line;
  return;
}


// ***************************************************************************
// This is a big function.  Basically, you have to go through and reorganize
// data into RGB format.
void RawRGBIDataHandler::getRawScanline(long int row, unsigned char* inarray)
  const throw(ImageException, std::bad_alloc)
{
  long int seekval;
  unsigned char* line1;  // hold the first band line
  long int loopcount;
  int bandcount;

  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  // If it's already in RGB BIP format, just handle as normal
  if (rgbordered && ( (Photometric == PHOTO_RGB) ||
		      (Photometric == PHOTO_RGB_BIP)) )
  {
    seekval = row * ByteWidth + dataposition;
    
    try
    {
      Stream->seekg(seekval);
      Stream->read(reinterpret_cast<char*>(inarray), ByteWidth);
    }
    catch (...)
    {
      throw ImageException(IMAGE_FILE_READ_ERR); // to notify caller
    }
    if (Stream->fail())
      throw ImageException(IMAGE_FILE_READ_ERR);

    return;
  }
  else // here we go...
  {
    if (Photometric == PHOTO_RGB_BIP) // it's bip, just misordered
    {
      seekval = row * ByteWidth + dataposition;

      if (!(line1 = new(std::nothrow) unsigned char[ByteWidth]))
	throw std::bad_alloc();

      try
      {
        Stream->seekg(seekval);
        Stream->read(reinterpret_cast<char*>(line1), ByteWidth);
      }
      catch (...) // in case the stream throws an exception
      {
        delete [] line1;
        throw ImageException(IMAGE_FILE_READ_ERR); // to notify caller
      }
      if (Stream->fail())
      {
        delete [] line1;
        throw ImageException(IMAGE_FILE_READ_ERR);
      }

      // Okie, this probably is bad, but can't really see a better way of
      // doing it.  Loop through, and then rearrange the data into RGB order
      // in the output array
      for (loopcount = 0; loopcount < ByteWidth; loopcount += 3)
      {
        for (bandcount = 0; bandcount < 3; bandcount++)
        {
          if (bandcontent[bandcount] == 'R')
            inarray[loopcount] = line1[loopcount + bandcount];
          else if (bandcontent[bandcount] == 'G')
            inarray[loopcount + 1] = line1[loopcount + bandcount];
          else
            inarray[loopcount + 2] = line1[loopcount + bandcount];
        }
      }
      
      delete [] line1; // free memory used

      return;
    }
    else if (Photometric == PHOTO_RGB_BIL)
    {
      unsigned char* scanlines[3];
      // Go through, read in 3 lines at a time, and then build the RGB triplet
      // array from the 3 sequential bands
      seekval = 3 * row * Width + dataposition;

      // Try to do our memory allocations.  Note:  Since BIL's store the red,
      // green, and blue bands separately, only allocate space for width
      // pixels.
      if (!(scanlines[0] = new(std::nothrow) unsigned char[Width]))
        throw std::bad_alloc();
      if (!(scanlines[1] = new(std::nothrow) unsigned char[Width]))
      {
        // clean up the previously allocated buffers
        delete [] scanlines[0];
        throw std::bad_alloc();
      }
      if (!(scanlines[2] = new(std::nothrow) unsigned char[Width]))
      {
        // clean up the previously allocated buffers
        delete [] scanlines[0];
        delete [] scanlines[1];
        throw std::bad_alloc();
      } 
     
      // now that our memory has been allocated, go ahead and read in the
      // seperate bands
      try
      {
        Stream->seekg(seekval);
        Stream->read(reinterpret_cast<char*>(scanlines[0]), Width);
        Stream->read(reinterpret_cast<char*>(scanlines[1]), Width);
        Stream->read(reinterpret_cast<char*>(scanlines[2]), Width);
      }
      catch (...)
      {
        delete [] scanlines[0];
        delete [] scanlines[1];
        delete [] scanlines[2];
        throw ImageException(IMAGE_FILE_READ_ERR);
      }
      if (Stream->fail())
      {
        // disk read error.  Clean up and throw to caller
        delete [] scanlines[0];
        delete [] scanlines[1];
        delete [] scanlines[2];
        throw ImageException(IMAGE_FILE_READ_ERR);
      }

      for (loopcount = 0; loopcount < Width; loopcount++)
      {
        for (bandcount = 0; bandcount < 3; bandcount++)
        {
          if (bandcontent[bandcount] == 'R')
            inarray[loopcount * 3] = scanlines[bandcount][loopcount];
          else if (bandcontent[bandcount] == 'G')
            inarray[loopcount * 3 + 1] = scanlines[bandcount][loopcount];
          else
	    inarray[loopcount * 3 + 2] = scanlines[bandcount][loopcount];
        }
      }

      // clean up the memory allocs
      delete [] scanlines[0];
      delete [] scanlines[1];
      delete [] scanlines[2];

      return;
    } // if Photometric == PHOTO_RGB
    else if (Photometric == PHOTO_RGB_BSQ)
    {
      unsigned char* scanlines[3];
      // okie, here you have to read in the line, skip down the
      // number of rows in the image, read the next, ect
      
      // Try to do our memory allocations
      if (!(scanlines[0] = new(std::nothrow) unsigned char[Width]))
        throw std::bad_alloc();
      if (!(scanlines[1] = new(std::nothrow) unsigned char[Width]))
      {
        delete [] scanlines[0]; // clean up after ourselves
        throw std::bad_alloc();
      }
      if (!(scanlines[2] = new(std::nothrow) unsigned char[Width]))
      {
        delete [] scanlines[0];
        delete [] scanlines[1];
        throw std::bad_alloc();
      }

      // try to go get the bands
      try
      {
	// set up to seek to the first band
	seekval = row * Width + dataposition;

        Stream->seekg(seekval);
        Stream->read(reinterpret_cast<char*>(scanlines[0]), Width);

        seekval = (Height + row - 1) * Width + dataposition;
        Stream->seekg(seekval);
        Stream->read(reinterpret_cast<char*>(scanlines[1]), Width);

        seekval = (2 * Height + row - 1) * Width + dataposition;
	Stream->seekg(seekval);
        Stream->read(reinterpret_cast<char*>(scanlines[2]), Width);
      }
      catch (...)
      {
        delete [] scanlines[0];
        delete [] scanlines[1];
        delete [] scanlines[2];
        throw ImageException(IMAGE_FILE_READ_ERR);
      }
      if (Stream->fail())
      {
        // clean up after ourselves
        delete [] scanlines[0];
        delete [] scanlines[1];
        delete [] scanlines[2];
        throw ImageException(IMAGE_FILE_READ_ERR);
      }

      for (loopcount = 0; loopcount < Width; loopcount++)
      {
        for (bandcount = 0; bandcount < 3; bandcount++)
        {
          if (bandcontent[bandcount] == 'R')
            inarray[loopcount * 3] = scanlines[bandcount][loopcount];
          else if (bandcontent[bandcount] == 'G')
            inarray[loopcount * 3 + 1] = scanlines[bandcount][loopcount];
          else
            inarray[loopcount * 3 + 2] = scanlines[bandcount][loopcount];
        }
      }
      
      // clean up after ourselves now
      delete [] scanlines[0];
      delete [] scanlines[1];
      delete [] scanlines[2];
    } // if Photometric == PHOTO_RGB_BSQ
  } // else

  return;
} // getRawScanline


// ***************************************************************************
void RawRGBIDataHandler::getRawPixel(long int x, long int y,
                                     unsigned char* inpixel)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line; // to hold the converted scanline

  if (!validateXY(x, y))
    throw ImageException(IMAGE_BOUNDING_ERR);

  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();
  
  // okie, now just call getRawScanline and let it do the work for me
  try
  {
    getRawScanline(y, line);
  }
  catch (...)
  {
    delete [] line; // clean up
    throw ImageException(IMAGE_FILE_READ_ERR); // to let caller know
  }

  for (int foo = 0; foo < 3; foo++)
    inpixel[foo] = line[3 * x + foo];

  delete [] line;
  return;
}


// ***************************************************************************
void RawRGBIDataHandler::getRawRectangle(long int x1, long int y1,
                                         long int x2, long int y2, 
                                         unsigned char* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* ptr;
  unsigned char* line;
  long int length;     // length of the rectangle
  long int ycounter;   // counts in the y direction
  long int tempcounter; // just a dummy counter 

  if (!validateXY(x1, y1) || !validateXY(x2, y2))
    throw ImageException(IMAGE_BOUNDING_ERR);

  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();

  length = x2 - x1 + 1;
  for (ycounter = y1; ycounter < y2; ycounter++)
  {
    try
    {
      getRawScanline(ycounter, line);
    }
    catch (...)
    {
      delete [] line;
      throw ImageException(IMAGE_FILE_READ_ERR); // to caller
    }

    ptr = &(inarray[(ycounter - y1) * length]);
    for (tempcounter = 0; tempcounter < (3 * length); tempcounter += 3)
    {
      ptr[tempcounter]     = line[x1 + tempcounter];
      ptr[tempcounter + 1] = line[x1 + tempcounter + 1];
      ptr[tempcounter + 2] = line[x1 + tempcounter + 2];
    }
  }

  delete [] line;
  return;
}

// ***************************************************************************
void RawRGBIDataHandler::getRawRedBand(long int row, unsigned char* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line;  // to hold the scanline
  long int loopcount;
  long int bytejump;
  
  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(row, line);
  }
  catch (...)
  {
    delete [] line;
    throw; // to notify caller
  }

  bytejump = 0;
  // go through and copy the red values into the input array
  for (loopcount = 0; loopcount < Width; loopcount++)
  {
    inarray[loopcount] = line[bytejump];
    bytejump += 3;
  }

  delete [] line;
  return;
}

// ***************************************************************************
void RawRGBIDataHandler::getRawGreenBand(long int row, unsigned char* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line;  // to hold the scanline
  long int loopcount;
  long int bytejump;

  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(row, line);
  }
  catch (...)
  {
    delete [] line;
    throw; // to notify caller
  }

  bytejump = 0;
  // go through and copy the red values into the input array
  for (loopcount = 0; loopcount < Width; loopcount++)
  {
    inarray[loopcount] = line[bytejump + 1];
    bytejump += 3;
  }

  delete [] line;
  return;
}


// ***************************************************************************
void RawRGBIDataHandler::getRawBlueBand(long int row, unsigned char* inarray)
  const throw(ImageException, std::bad_alloc)
{
  unsigned char* line;  // to hold the scanline
  long int loopcount;
  long int bytejump;
  
  if (!(line = new(std::nothrow) unsigned char[ByteWidth]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(row, line);
  }
  catch (...)
  {
    delete [] line;
    throw; // to notify caller
  }

  bytejump = 0;
  // go through and copy the red values into the input array
  for (loopcount = 0; loopcount < Width; loopcount++)
  {
    inarray[loopcount] = line[bytejump + 2];
    bytejump += 3;
  }

  delete [] line;
  return;
}


#ifdef _WIN32
#pragma warning( default : 4291 )  // Disable warning messages
#endif

#endif






