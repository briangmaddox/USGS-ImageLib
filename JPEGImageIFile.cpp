#ifndef _JPEGIMAGEIFILE_CPP_
#define _JPEGIMAGEIFILE_CPP_
// $Id: JPEGImageIFile.cpp,v 1.3 2004-12-14 23:40:33 bmaddox Exp $
// Brian Maddox - gowaddle@yahoo.com
// Last modified by $Author: bmaddox $ on $Date: 2004-12-14 23:40:33 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "RGBPalette.h"
#include "JPEGImageIFile.h"

using namespace USGSImageLib;

// ***************************************************************************
JPEGImageIFile::JPEGImageIFile(std::string& infilename)
  : jpegfile(0), startdecompress(false), curr_read(0), major_version(0),
    minor_version(0), density_unit(0), adobe_transform(0), 
    saw_adobe_marker(false), x_density(0), y_density(0), scanlinebuffer(0)
{
  // Attempt to open the jpeg file first.  No sense of continuing if we can't
  // open the file.
  if ( (jpegfile = std::fopen(infilename.c_str(), "rb")) == NULL)
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  
  // Call openJPEG to init the JPEG object and read the header info
  openJPEG();

  // Since we got here, the jpeg must be ok.  Go ahead and set some of our
  // internal fields.
  Width           = jpeginfo.image_width;
  Height          = jpeginfo.image_height;
  samplesPerPixel = jpeginfo.num_components;
  bitsPerSample   = 8; // Most jpegs are 8 bit/sample, unless special
  
  switch (jpeginfo.jpeg_color_space)
  {
  case JCS_RGB:
    Photometric = PHOTO_RGB;
    break;
  case JCS_GRAYSCALE:
    Photometric = PHOTO_GRAY;
    break;
  case JCS_YCbCr:
    // Force libjpeg to convert to RGB colorspace and let the user change
    // the colorspace on their own
    jpeginfo.out_color_space = JCS_RGB;
    Photometric              = PHOTO_RGB;
    break;
  default:
    // If it's none of the above, then we can't handle it.  Might try to do
    // something for CMYK jpegs, though.
    setFailRBit();
  }
  
  // If the JFIF marker is present, record the version and resolution info
  if (jpeginfo.saw_JFIF_marker == TRUE)
  {
    major_version = jpeginfo.JFIF_major_version;
    minor_version = jpeginfo.JFIF_minor_version;
    density_unit  = jpeginfo.density_unit;
    x_density     = jpeginfo.X_density;
    y_density     = jpeginfo.Y_density;
  }
  
  // Now check for the Adobe APP14 marker
  if (jpeginfo.saw_Adobe_marker == TRUE)
  {
    saw_adobe_marker = true;
    adobe_transform  = jpeginfo.Adobe_transform;
  }
  
  ImageType = IMAGE_JPEG; // Set the type for ImageLib querying
}


// ***************************************************************************
// Clean up the JPEG object and close the stdio file
JPEGImageIFile::~JPEGImageIFile()
{
  if (startdecompress)
    jpeg_finish_decompress(&jpeginfo);
  jpeg_destroy_decompress(&jpeginfo);
  std::fclose(jpegfile);
}


// ***************************************************************************
// This function wraps jpeg_read_scanlines.  If you select color-
// mapped output, it returns the index into the palette.  Otherwise, it'll
// return gray or color data.  Being that JPEG is a streaming format, it can
// get really slow when trying to do random access.  No internal caching is
// done, so if you read a scanline out of order, it will have to go through
// and loop through the file.  This can be really slow on large images!
void JPEGImageIFile::getRawScanline(long int row, unsigned char* inarray)
  throw (ImageException, std::bad_alloc)
{
  long int skipcounter  = 0;
  //  long int scanlineloop = 0;
  
  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  try
  {
    // If we haven't already, call start_decompress so we can being
    // decompressing scanlines.  Note that this will also populate the jpeg
    // struct
    if (!startdecompress)
    {
      jpeg_start_decompress(&jpeginfo);
      startdecompress = true;
    }
    
    // Only allocate the buffer if it doesn't exist.  Then just let libjpeg
    // delete it in the destructor
    if (!scanlinebuffer)
      scanlinebuffer = (*jpeginfo.mem->alloc_sarray)((j_common_ptr)&jpeginfo,
                                                     JPOOL_IMAGE,
                                                     (Width * samplesPerPixel),
                                                     1);
    
    // We've requested the next scanline in order, so just call read_scanlines
    if (row == curr_read)
    {
      jpeg_read_scanlines(&jpeginfo, scanlinebuffer, 1);
      std::memcpy(inarray, scanlinebuffer[0], (Width * samplesPerPixel));
      ++curr_read;
    }
    else
    {
      // Go ahead and reset the JPEG so we can loop through again      
      resetJPEG();
      // Restart decompression
      jpeg_start_decompress(&jpeginfo);
      startdecompress = true;

      // Since we deleted the jpeg object, we have to have it reallocate
      // the scanline buffer here
      scanlinebuffer = (*jpeginfo.mem->alloc_sarray)((j_common_ptr)&jpeginfo,
                                                     JPOOL_IMAGE,
                                                     (Width * samplesPerPixel),
                                                     1);

      curr_read = 0;
      for (skipcounter = 0; skipcounter < row; ++skipcounter)
      {
        jpeg_read_scanlines(&jpeginfo, scanlinebuffer, 1);
      }
      // Now advance curr_read
      curr_read = curr_read + skipcounter;

      // Now the jpeg pointer should be at the scanline we need.  Just read
      // it in and advance the pointer
      jpeg_read_scanlines(&jpeginfo, scanlinebuffer, 1);
      std::memcpy(inarray, scanlinebuffer[0], (Width * samplesPerPixel));
      ++curr_read;
    }
    
    return;
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// ***** THIS NEEDS FIXED BRIAN!!!! ***************
// There are two methods of operation for this function:
// 1) libjpeg is told to quantize the data before you decompress.  In this
//    case, this function will just get the palette from libjpeg and return it
//    as a Palette.
// 2) The file is opened as normal.  For grey images, we throw an exception
//    since we don't allow colormaps for greyscale data.  Otherwise, create a
//    color map just like all the other ImageLib classes.  Note, if this is 
//    called on a 24 bit image, it will be VERY slow and take a lot of memory.
//    In other words, only call it on 256 color RGB images.
void JPEGImageIFile::getPalette(Palette* inpalette)
  throw (ImageException, std::bad_alloc)
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

  if (Photometric != PHOTO_RGB)
    throw ImageException(IMAGE_INVALID_OP_ERR);

  try
  {
    // Attempt to allocate the memory for a scanline buffer
    if (!(line = new(std::nothrow) unsigned char[Width * samplesPerPixel]))
      throw std::bad_alloc();
    
    // now go through and create the palette.  This could take a LONG while
    // and if it's a true color image, well, could result in a BIG palette :)
    for (rowloop = 0; rowloop < Height; rowloop++)
    {
      // grab the actual scanline data
      getRawScanline(rowloop, line);
      
      // Here we go.  Scan the palette list to see the color is in there, 
      // if not add it and loop again.
      for (colloop = 0; colloop < Width * 3; colloop += 3)
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
  catch (...)
  {
    // something happened and we couldn't read the file.  Go ahead and clean
    // up what we can and re-throw to notify the caller
    delete [] line;
    inpalette = 0;
    pixelbuff.clear();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// Get the raw pixel from the image.  If grey, only return 1 byte.  If RGB,
// return the triplet.
void JPEGImageIFile::getRawPixel(long int x, long int y, 
                                 unsigned char* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned char* tbuffer;

  if (!validateXY(x, y))
    throw ImageException(IMAGE_BOUNDING_ERR);
  
  if (!(tbuffer = new(std::nothrow) 
        unsigned char[samplesPerPixel * Width]))
    throw std::bad_alloc();

  try
  {
    getRawScanline(y, tbuffer);

    if (Photometric == PHOTO_RGB) // triplet
    {
      for (int foo = 0; foo < 3; foo++)
        inarray[foo] = tbuffer[3 * x + foo];
    }
    else // grayscale
      *inarray = tbuffer[x];

    delete [] tbuffer;
    return;
  }
  catch (...)
  {
    delete [] tbuffer;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// Convenience function to get a raw rectangle from the image.
void JPEGImageIFile::getRawRectangle(long int x1, long int y1,
                                     long int x2, long int y2,
                                     unsigned char* inarray)
  throw (ImageException, std::bad_alloc)
{
  if (!normalizeCoords(x1, y1, x2, y2))
    throw ImageException(IMAGE_BOUNDING_ERR);

  long int rwidth, rheight; // width and height of the rectangle
  long int loopcount;       // to loop through scanlines
  long int poscount;        // position within the passed in array
  unsigned char* linepos;   // to hold the temp position in the returned array
  unsigned char* rectpos;   // our position in the passed in array
  unsigned char* templine;  // to hold the actual temporary scanline

  if (!(templine = new(std::nothrow) unsigned char[samplesPerPixel * Width]))
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
      linepos = &(templine[x1 * samplesPerPixel]); // get our position in the
                                                   // scanline
      rectpos = &inarray[poscount]; // get our position in the rectangle
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
// Return a pixel in the image using one of the pixel objects
void JPEGImageIFile::getPixel(long int x, long int y, AbstractPixel* inpixel)
  throw (ImageException, std::bad_alloc)
{
  if (!validateXY(x, y))
    throw ImageException(IMAGE_BOUNDING_ERR);

  try
  {
    if (Photometric == PHOTO_RGB)
    {
      unsigned char tpixel[3];
      
      getRawPixel(x, y, tpixel);
      inpixel->setRed(tpixel[0]);
      inpixel->setGreen(tpixel[1]);
      inpixel->setBlue(tpixel[2]);

      return;
    }
    else if (Photometric == PHOTO_GRAY)
    {
      unsigned char tpixel;

      getRawPixel(x, y, &tpixel);
      inpixel->setGrey(tpixel);
      
      return;
    }
  }
  catch (...)
  {
    setFailRBit();
    throw;
  }
}


// ***************************************************************************
// Return the scanline in a preallocated array of greypixel pointers
void JPEGImageIFile::getScanline(long int row, GreyPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  long int       poscount;    // Position counter   
  long int       loopcount;
  unsigned char* tline;     // temp buffer

  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  if (Photometric == PHOTO_RGB)
  {
    if (!(tline = new(std::nothrow) unsigned char[samplesPerPixel * Width]))
      throw std::bad_alloc();
  }
  else
    if (!(tline = new(std::nothrow) unsigned char[Width]))
      throw std::bad_alloc();

  try
  {
    getRawScanline(row, tline);
    
    if (Photometric == PHOTO_RGB)
    {
      poscount = 0;
      for (loopcount = 0; loopcount < Width; loopcount++)
      {
        inarray[loopcount].setRed(tline[poscount]);
        inarray[loopcount].setGreen(tline[poscount + 1]);
        inarray[loopcount].setBlue(tline[poscount + 2]);
        poscount += 3;
      }
    }
    else
    {
      for (loopcount = 0; loopcount < Width; loopcount++)
        inarray[loopcount].setGrey(tline[loopcount]);
    }

    delete [] tline;
    
    return;
  }
  catch (...)
  {
    setFailRBit();
    delete [] tline;
    throw;
  }
}


// ***************************************************************************
// Return the scanline in an array of RGBPixel pointers
void JPEGImageIFile::getScanline(long int row, RGBPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned char* tline;     // temporary buffer
  long int       loopcount; 
  int            poscount;  // position counter

  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);

  if (Photometric == PHOTO_RGB)
  {
    if (!(tline = new(std::nothrow) unsigned char[samplesPerPixel * Width]))
      throw std::bad_alloc();
  }
  else
    if (!(tline = new(std::nothrow) unsigned char[Width]))
      throw std::bad_alloc();

  try
  {
    getRawScanline(row, tline);
    
    if (Photometric == PHOTO_RGB)
    {
      poscount = 0;
      for (loopcount = 0; loopcount < Width; loopcount++)
      {
        inarray[loopcount].setRed(tline[poscount]);
        inarray[loopcount].setGreen(tline[poscount + 1]);
        inarray[loopcount].setBlue(tline[poscount + 2]);
        poscount += 3;
      }
    }
    else
    {
      for (loopcount = 0; loopcount < Width; loopcount++)
        inarray[loopcount].setGrey(tline[loopcount]);
    }

    delete [] tline;
    
    return;
  }
  catch (...)
  {
    setFailRBit();
    delete [] tline;
    throw;
  }
}


// ***************************************************************************
void JPEGImageIFile::getRectangle(long int x1, long int y1, long int x2,
                                  long int y2, RGBPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned short red, green, blue; // to hold the pixel values
  long int counter, counter1;      // loop counters
  long int tpos = 0;               // position in the array
  RGBPixel* tarray;                // temporary array holder

  if (!normalizeCoords(x1, y1, x2, y2))
    throw ImageException (IMAGE_BOUNDING_ERR);

  if (!(tarray = new(std::nothrow) RGBPixel[Width]))
    throw std::bad_alloc();

  try
  {
    for (counter = y1; counter <= y2; counter++)
    {
      getScanline(y1, tarray);
      for (counter1 = x1; counter1 <= x2; counter1++)
      {
        tarray[counter1].getRed(red);
        tarray[counter1].getGreen(green);
        tarray[counter1].getBlue(blue);
        inarray[tpos].setRed(red);
        inarray[tpos].setGreen(green);
        inarray[tpos].setBlue(blue);
        ++tpos;
      }
    }
    
    delete [] tarray;
    return;
  }
  catch (...)
  {
    delete [] tarray;
    throw;
  }
}


// ***************************************************************************
void JPEGImageIFile::getRectangle(long int x1, long int y1, long int x2,
                                  long int y2, GreyPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned short greyval;      // to hold the pixel values
  long int counter, counter1;  // loop counters
  long int tpos = 0;           // position in the array
  GreyPixel* tarray;           // temporary array holder

  if (!normalizeCoords(x1, y1, x2, y2))
    throw ImageException (IMAGE_BOUNDING_ERR);

  if (!(tarray = new(std::nothrow) GreyPixel[Width]))
    throw std::bad_alloc();
  
  try
  {
    for (counter = y1; counter <= y2; counter++)
    {
      getScanline(y1, tarray);
      for (counter1 = x1; counter1 <= x2; counter1++)
      {
        tarray[counter1].getGrey(greyval);
        inarray[tpos].setGrey(greyval);
        ++tpos;
      }
    }
    
    delete [] tarray;
    return;
  }
  catch (...)
  {
    delete [] tarray;
    throw;
  }
}


// ***************************************************************************
// Select the dither mode for decompression.  Only allow this to set if we're
// not currently decompressing
bool JPEGImageIFile::setDitherMode(J_DITHER_MODE& inmode) throw()
{
  if (startdecompress)
    return false;

  // Must have selected quantization first
  if (jpeginfo.quantize_colors != TRUE)
    return false;

  jpeginfo.dither_mode = inmode;
  return true;
}


// ***************************************************************************
// Tell libjpeg to quantize image.  This function _MUST_ be called first
// before any other method that deals with color mapped output.  Otherwise,
// those methods will fail.
bool JPEGImageIFile::selectColorMappedOutput() throw()
{
  if (startdecompress)
    return false;

  jpeginfo.quantize_colors = TRUE;

  // Since the returned colormapped output will be a single byte index into a
  // colormap, we need to change the number of samples to be 1
  samplesPerPixel = 1;

  return true;
}


// ***************************************************************************
// Set the number of colors that libjpeg will use to create colormapped 
// output.
bool JPEGImageIFile::setNumberOutputColors(int& indesired_colors) throw()
{
  if (startdecompress)
    return false;
  
  // This enforces that you have to explicity select colormapped output FIRST
  // before you can set the number of output colors
  if (jpeginfo.quantize_colors != TRUE)
    return false;
  
  jpeginfo.desired_number_of_colors = indesired_colors;
  return true;
}


// ***************************************************************************
// Select libjpeg to perform a two pass quantization.  This will generally
// result in a better colormapped output, but can be slower.
bool JPEGImageIFile::setTwoPassQuantize() throw()
{
  if (startdecompress)
    return false;

  // Again, this enforces the requirement of explicity selecting colormapped
  // output first before calling the other related functions.
  if (jpeginfo.quantize_colors != TRUE)
    return false;

  jpeginfo.two_pass_quantize = TRUE;
  return true;
}


// ***************************************************************************
// Change the discreet cosine transform method used by libjpeg.  This varies
// from slow to base methods.  Consult the included file libjpeg.doc for
// details
bool JPEGImageIFile::setDCTMethod(J_DCT_METHOD& inmethod) throw()
{
  if (startdecompress)
    return false;

  jpeginfo.dct_method = inmethod;
  return true;
}


// ***************************************************************************
// Select whether or not to do a careful up-sampling of the chroma components.
// FALSE is faster, but may not look as good
bool JPEGImageIFile::setFancyUpsampling(bool& on) throw()
{
  if (startdecompress)
    return false;

  if (on)
    jpeginfo.do_fancy_upsampling = TRUE;
  else
    jpeginfo.do_fancy_upsampling = FALSE;

  return true;
}


// ***************************************************************************
// Select whether or not to apply interblock smoothing in the early stages of
// decoding on progressive JPEG files.  TRUE looks fuzzy, while FALSE can
// appear blocky
bool JPEGImageIFile::setBlockSmoothing(bool& on) throw()
{
  if (startdecompress)
    return false;

  if (on)
    jpeginfo.do_block_smoothing = TRUE;
  else
    jpeginfo.do_block_smoothing = FALSE;

  return true;
}


// ***************************************************************************
// Reopen the JPEG.  We need to do this since libJPEG doesn't provide a decent
// way to return to the beginning of a file to start again (JPEG being a 
// streaming file format).
void JPEGImageIFile::resetJPEG() throw(ImageException)
{
  // Destroy the compression object, which also handily aborts things
  jpeg_destroy_decompress(&jpeginfo);

  // Rewind the file pointer.  We don't close here so we can at least try to
  // avoid a lot of file opens and closes
  std::rewind(jpegfile);

  // Call openJPEG to recreate the JPEG object and reparse everything.
  openJPEG();
}


// ***************************************************************************
// We not only need to open the file in the constructor, but we also need to 
// reopen it to allow simulated random access.  The open functions are moved
// here to avoid duplicating code.  Note that we don't reopen the file here
// since we can keep the file pointer and just rewind it whenever we create a
// new jpeg object.
void JPEGImageIFile::openJPEG() throw (ImageException)
{
  try
  {
    // Set the error mechanism
    jpeginfo.err = jpeg_std_error(&jerr);
    // Initialize the JPEG decompression object
    jpeg_create_decompress(&jpeginfo);
    // Specify where the input data is coming from
    jpeg_stdio_src(&jpeginfo, jpegfile);
    
    // Now read the header in.  If something went wrong, throw an exception
    if (jpeg_read_header(&jpeginfo, TRUE) != JPEG_HEADER_OK)
      throw ImageException(IMAGE_FILE_OPEN_ERR);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_OPEN_ERR);
    return;
  }
}


#endif
