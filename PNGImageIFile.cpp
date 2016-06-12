/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#ifndef _PNGIMAGEIFILE_CPP_
#define _PNGIMAGEIFILE_CPP_


#include "PNGImageIFile.h"

using namespace USGSImageLib;


//********************************************************
PNGImageIFile::PNGImageIFile(std::string & infile)  
  throw(ImageException, std::bad_alloc)
  : ImageIFile(infile),
    palette(0),
    png_ptr(0),
    info_ptr(0), 
    pngfile(0),
    currentline(0),
    number_passes(0),
    cache_size(0),
    cache_start(0),
    bytesperline(0)
{
  unsigned char buf[8];   //for png identification
  std::ifstream input;    //for reading the initial bytes
  png_uint_32 twidth = 0, theight = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0;
  png_color_16 *background = 0, black;

  black.index = 0;
  black.red = 0;
  black.green = 0;
  black.blue = 0;
  black.gray = 0;

  try
  {
    //set the Image Type
    ImageType = IMAGE_PNG; 
    
    //open the file
    input.open(infile.c_str(), std::ios::in | std::ios::binary);
    
    if (input.fail())
    {
      input.close();
      throw ImageException();
    }

    //try to read the first 8 bytes of the png
    input.read(reinterpret_cast<char*>(buf), 8);
    
    if (input.fail())
    {
      input.close();
      throw ImageException();
    }

    //check to see if the is a valid png file
    if(png_sig_cmp(buf, 0, 8))
    {
      input.close();
      throw ImageException();
    }

    //made it this far so it is probably a png
    input.close();
    
    //switch over to old c io
    pngfile = fopen(infile.c_str(), "rb");
    
    if (!pngfile)
      throw ImageException();
    
    //allocate the libpng structures
    png_ptr = png_create_read_struct
      (PNG_LIBPNG_VER_STRING, 0, 0, 0);
    
    if(!png_ptr)
      throw ImageException();
    
    info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr)
      throw ImageException();
    
    
    //do the stupid setjmp
    if (setjmp(png_jmpbuf(png_ptr)))
      throw ImageException();
    
    //init png io
    png_init_io(png_ptr, pngfile);
    
    //read the info
    png_read_info(png_ptr, info_ptr);
    
    png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
                 &interlace_type, 0, 0);
    
    //set the width and height of the image
    Width = twidth;
    Height = theight;
    
    //get the interlace handling stuff
    number_passes = png_set_interlace_handling(png_ptr);

    //branch on the color type
    switch(color_type)
    {
    case PNG_COLOR_TYPE_GRAY: 
      //gray scale case (bit depths 1, 2, 4, 8, 16)
      {
        Photometric = PHOTO_GRAY;
        if (bit_depth <= 8)
        {
          bitsPerSample = 8;
          png_set_expand(png_ptr);  //expand out to 8 bits
        }
        else
          bitsPerSample = 16;
        samplesPerPixel = 1;

      }
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      //gray scale with alpha (bit depths 8, 16)
      {
        Photometric = PHOTO_GRAY;
        bitsPerSample = bit_depth;
        samplesPerPixel = 1;

        //see if the image was nice enough to supply a alpha background
        if (png_get_bKGD(png_ptr, info_ptr, &background))
          png_set_background(png_ptr, background,
                             PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
        else //use a black background
        {
          png_set_background(png_ptr, &black, 
                             PNG_BACKGROUND_GAMMA_SCREEN, 1, 1.0);
        }
      }
      break;
    case PNG_COLOR_TYPE_PALETTE:
      //color (RGB) with palette (depths 1, 2, 4, 8)
      {
        Photometric = PHOTO_PALETTE;
        bitsPerSample = 8;
        png_set_packing(png_ptr); //tell lib png to unpack 1, 2, 4 pixels into
                                  //separate bytes
        samplesPerPixel = 1;
        //setup the palette
        setupPalette();
      }
      break;             
    case PNG_COLOR_TYPE_RGB:
      //RGB data (bit_depths 8, 16)
      {
        Photometric = PHOTO_RGB;
        bitsPerSample = bit_depth;
        samplesPerPixel = 3;
      }
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      //RGBA data (bit_depths 8, 16)
      {
        Photometric = PHOTO_RGB;
        bitsPerSample = bit_depth;
        samplesPerPixel = 3;
        //see if the image was nice enough to supply a alpha background  
        if (png_get_bKGD(png_ptr, info_ptr, &background))
          png_set_background(png_ptr, background,
                             PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
        else //use a black background
        {
          png_set_background(png_ptr, &black, 
                             PNG_BACKGROUND_GAMMA_SCREEN, 1, 1.0);
        }
      }
      break;
    default:
      throw ImageException();
    }

    bytesperline = Width*bitsPerSample*samplesPerPixel/8;


    //looks good create the cache
    setScanlineCache(1);
    
  }
  catch(...)
  {
      
    //we have trouble so set the fail bit
    setFailRBit();
    //destroy the structs
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    png_ptr = 0;
    info_ptr = 0;
    //close the file
    fclose(pngfile);
    
  }
}        
   
//*****************************************************************
PNGImageIFile::~PNGImageIFile()
{
  long int counter = 0;
  //check for the palette
  if (palette)
    delete palette;
  
  //loop through and destroy the cache
  for (; counter < cache_size; ++counter)
    delete [] cache[counter];
  
  delete [] cache;

 
  //destroy the png file structures
  png_destroy_read_struct(&png_ptr, &info_ptr, 0);
  //close the file
  fclose(pngfile);
}

//******************************************************************
void PNGImageIFile::setupPalette() 
  throw(ImageException, std::bad_alloc)
{
  png_color * cpal = 0;   //the c palette
  int color_num = 0;      //the number of entries in the palette
  RGBPixel tpixel;       //temporary pixel
  int counter = 0;        //counter for loops

  //get a pointer to the palette or throw
  if (!png_get_PLTE(png_ptr, info_ptr, &cpal, &color_num))
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
  
  //that went well so create the class palette
  if (!(palette = new (std::nothrow) RGBPalette(color_num)))
    throw std::bad_alloc();

  //May need to add something here to handle grayscale palette
  //stuff. CBB
  for(; counter < color_num; ++counter)
  {
    tpixel.setRed(cpal[counter].red);
    tpixel.setGreen(cpal[counter].green);
    tpixel.setBlue(cpal[counter].blue);
    palette->setEntry(counter, tpixel);
  }
}

//******************************************************************
void PNGImageIFile::
reCache(long int line) throw(ImageException, std::bad_alloc)
{
  unsigned char * trow = 0;
  long int counter = 0, pass = 0;
  long int cache_end = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0;
  png_uint_32 twidth = 0, theight = 0;
  png_color_16 *background = 0, black;

  //check the cache
  if (!cache)
    throw ImageException(IMAGE_FILE_READ_ERR);

  //Note if we came here then the "jig is up" and we have
  //to seek to front of the file, reread the info and seek to the
  //right line.
  if (!pngfile)
    throw ImageException(IMAGE_FILE_OPEN_ERR);

  //seek to the start of the file
  fseek(pngfile, 0, SEEK_SET); 
  //destroy the png file structures
  png_destroy_read_struct(&png_ptr, &info_ptr, 0);

  //allocate the libpng structures
  png_ptr = png_create_read_struct
    (PNG_LIBPNG_VER_STRING, 0, 0, 0);
    
  if(!png_ptr)
    throw ImageException();
  
  info_ptr = png_create_info_struct(png_ptr);
  
  if (!info_ptr)
    throw ImageException();
  
  
  //do the stupid setjmp
  if (setjmp(png_jmpbuf(png_ptr)))
    throw ImageException();
  
  //init png io
  png_init_io(png_ptr, pngfile);
  
    //read the info
  png_read_info(png_ptr, info_ptr);
    
  png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
                 &interlace_type, 0, 0);

  switch(color_type)
  {
    case PNG_COLOR_TYPE_GRAY_ALPHA:
    case PNG_COLOR_TYPE_RGB_ALPHA:
    {
       if (png_get_bKGD(png_ptr, info_ptr, &background))
          png_set_background(png_ptr, background,
                             PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
       else //use a black background
       {
         png_set_background(png_ptr, &black,
                             PNG_BACKGROUND_GAMMA_SCREEN, 1, 1.0);
       }
    }
    break;
    case PNG_COLOR_TYPE_GRAY:
    {
      if (bit_depth <= 8)
      {
        png_set_expand(png_ptr);  //expand out to 8 bits
      }
    }
    break;
    case PNG_COLOR_TYPE_PALETTE:
    {
      png_set_packing(png_ptr);   //unpack indices into 8 bit data
    }
    break;
  }

  //get the interlace handling stuff
  number_passes = png_set_interlace_handling(png_ptr);
  
  //figure out were the cache should start to cover the line
  cache_start = line - line%cache_size;

  if (cache_start + cache_size >= Height)
  {
    cache_end = Height - cache_start;
  }
  else
    cache_end = cache_size;

  try
  {

    //create the temporary row
    if (!(trow = new (std::nothrow) unsigned char [bytesperline]))
      throw std::bad_alloc();

    for (pass = 0; pass < number_passes; ++pass)
    {
      for (counter = 0; counter < cache_start; ++counter)
      {
        //read the temp row
        png_read_rows(png_ptr, &trow, NULL, 1);
      }
      
      //read cache_end rows
      png_read_rows(png_ptr, cache, NULL, cache_end);
      
      //read the rest
      for (counter = cache_start + cache_size; counter < Height ; ++counter)
      {
        png_read_rows(png_ptr, &trow, NULL, 1);
      }
    }
    
    delete [] trow;

  }
  catch(...)
  {
    if (cache)
    {
      for (counter = 0; counter < cache_size; ++counter)
        delete [] cache[counter];
      delete cache;
      cache = 0;
    }
    throw ImageException();
  }
     
}

//******************************************************************
void PNGImageIFile::setScanlineCache(long int number_of_lines) 
  throw(ImageException, std::bad_alloc)
{
  long int counter = 0;
  
  if (number_of_lines <= 0)
    number_of_lines = 1;


  if (number_of_lines != cache_size)
  {
    for (; counter < cache_size; ++counter)
      delete [] cache[counter];
    
    delete [] cache;
    cache = 0;
    cache_size = number_of_lines;
   
    try
    {
      if (!(cache = new (std::nothrow) unsigned char*[cache_size]))
        throw std::bad_alloc();

      for (counter = 0; counter < cache_size; ++counter)
        if(!(cache[counter] = new (std::nothrow) unsigned
                 char[bytesperline]))           throw std::bad_alloc();

      //reload the cache
      reCache(0);
    }
    catch(...)
    {
      if (cache)
      {
        for (counter = 0; counter < cache_size; ++counter)
          delete [] cache[counter];
    
        delete [] cache;
        cache = 0;
      }
    }
  }
}
   
//******************************************************************
void PNGImageIFile::getPalette(Palette* inpalette)
  throw(ImageException, std::bad_alloc)
{
  RGBPalette * temp = 0;
  RGBPixel temppixel;
  int counter = 0;
  int size = 0;


  //check for the existence of the palette or tell the user where to go
  if (!palette)
    throw ImageException(IMAGE_INVALID_OP_ERR);

  //could be a dangerous cast to make
  temp = (RGBPalette*)inpalette;
  size = palette->getNoEntries();

  for(; counter < size; ++counter)
  {
    palette->getEntry(counter, temppixel);
    //the palette classes are made to grow if they run out of room...
    temp->setEntry(counter, temppixel);
  }

}

//***********************************************************
void PNGImageIFile::getPixel(long x, long y, AbstractPixel* inpixel)
    throw(ImageException, std::bad_alloc)
{
  
  if (Photometric == PHOTO_PALETTE)
  {
    unsigned char tpixel; // temporarily pixel
    unsigned short int tcolor; //temporary color
    RGBPixel entry;
    
    //get the raw pixel
    getRawPixel(x, y, &tpixel);
    
    //get the entry out of the palette.
    palette->getEntry(tpixel, entry);
    
    entry.getRed(tcolor);
    inpixel->setRed(tcolor);
    
    entry.getGreen(tcolor);
    inpixel->setGreen(tcolor);
    
    entry.getBlue(tcolor);
    inpixel->setBlue(tcolor);
    return;
  }
  else 
    if (Photometric == PHOTO_GRAY)
    {
      unsigned char tpixel;
      getRawPixel(x, y, &tpixel);
      inpixel->setGrey(tpixel);
      return;
    }
  else 
    if (Photometric == PHOTO_RGB)
    {
      unsigned char tpixel[3];
      getRawPixel(x, y, tpixel);
      inpixel->setRed(tpixel[0]);
      inpixel->setGreen(tpixel[1]);
      inpixel->setBlue(tpixel[2]);
      return;
    }

}

//****************************************************************
void PNGImageIFile::getScanline(long row, RGBPixel* inarray)
  throw(ImageException, std::bad_alloc)
{
  unsigned char * trow = 0;
  unsigned short int * trow16 = 0;
  long int counter = 0, size(Width*samplesPerPixel);
  RGBPixel tpixel;
  unsigned short int color = 0;

  try
  {
    if (bitsPerSample != 16)
    {
      //create the raw scanline
      if(!(trow = new (std::nothrow) unsigned char [bytesperline]))
       throw std::bad_alloc();
      getRawScanline(row, trow);
    }
    else
    {
      if(!(trow16 = new (std::nothrow) unsigned short int[size]))
        throw std::bad_alloc();
      getRawScanline(row, trow16);
    }

    switch(Photometric)
    {
    case PHOTO_PALETTE:
      //check the palette
      if (!palette)
        throw ImageException(IMAGE_INVALID_OP_ERR);
      
      for(counter = 0; counter < Width; ++counter)
      {
        palette->getEntry(trow[counter],tpixel);
        tpixel.getRed(color);
        inarray[counter].setRed(color);
        tpixel.getGreen(color);
        inarray[counter].setGreen(color);
        tpixel.getBlue(color);
        inarray[counter].setBlue(color);
      }
      break;
    case PHOTO_RGB:
      //see if its 8 or 16 bit data
      if (bitsPerSample == 8)
      {
        for (counter = 0; counter < Width; ++counter)
        {
          inarray[counter].setRed(trow[counter*samplesPerPixel]);
          inarray[counter].setGreen(trow[counter*samplesPerPixel+1]);
          inarray[counter].setBlue(trow[counter*samplesPerPixel+2]);
        }
      }
      else
      {
        //16 bit
        for (counter = 0; counter < Width; ++counter)
        {
          inarray[counter].setRed(trow16[counter*samplesPerPixel]);
          inarray[counter].setGreen(trow16[counter*samplesPerPixel + 1]);
          inarray[counter].setBlue(trow16[counter*samplesPerPixel + 2]);
        }
      }
      break;
    case PHOTO_GRAY:
      if (bitsPerSample == 8)
      {
        for(counter = 0; counter < Width; ++counter)
        {
          inarray[counter].setGrey(trow[counter]);
        }
      }
      else //16 bits per sample
      {
        for (counter = 0; counter < Width; ++counter)
        {
          inarray[counter].setGrey(trow[counter]);
        }
      }
      break;
    default:
      throw ImageException(IMAGE_FILE_READ_ERR);
    }
    delete [] trow; //delete the temporary row
    delete [] trow16;
  }
  catch(ImageException & err)
  {
    delete [] trow;
    delete [] trow16;

    throw err;
  }
  catch(...)
  {
    delete [] trow;
    delete [] trow16;
    ImageException(IMAGE_FILE_READ_ERR);
  }
}

//****************************************************************
void PNGImageIFile::getScanline(long row, GreyPixel* inarray)
  throw(ImageException, std::bad_alloc)
{
  unsigned char * trow = 0;            //for 8 bit samples case
  short unsigned int * trow16 = 0;     //for 16 bit samples case
  short unsigned int color = 0;        //temporary color
  long int size(Width*samplesPerPixel);
  long int counter = 0;                //temporary counter
  RGBPixel tpixel;                    //temp pixel

  try
  {
    if (bitsPerSample != 16)
    {
      if(!(trow = new (std::nothrow) unsigned char[bytesperline]))
        throw std::bad_alloc();
      getRawScanline(row, trow);
    }
    else
    {
      if(!(trow16 = new (std::nothrow) short unsigned int [size]))
        throw std::bad_alloc();
      getRawScanline(row, trow16);
    }
    
    switch(Photometric)
    {
    case PHOTO_PALETTE:
      //check the palette
      if (!palette)
        throw ImageException(IMAGE_INVALID_OP_ERR);
      
      for(counter = 0; counter < Width; ++counter)
      {
          palette->getEntry(trow[counter], tpixel);
          
          tpixel.getRed(color);
          inarray[counter].setRed(color);
          tpixel.getGreen(color);
          inarray[counter].setGreen(color);
          tpixel.getBlue(color);
          inarray[counter].setBlue(color);

      }
      break;
    case PHOTO_RGB:
      //see if its 8 or 16 bit data
      if (bitsPerSample == 8)
      {
        for (counter = 0; counter < Width; ++counter)
        {
          inarray[counter].setRed(trow[counter*samplesPerPixel]);
          inarray[counter].setGreen(trow[counter*samplesPerPixel+1]);
          inarray[counter].setBlue(trow[counter*samplesPerPixel+2]);
        }
      }
      else
      {
        //16 bit
        for (counter = 0; counter < Width; ++counter)
        {
          inarray[counter].setRed(trow16[counter*samplesPerPixel]);
          inarray[counter].setGreen(trow16[counter*samplesPerPixel + 1]);
          inarray[counter].setBlue(trow16[counter*samplesPerPixel + 2]);
        }
      }
      break;
    case PHOTO_GRAY:
      //the 8 bit case
      if (bitsPerSample <= 8)
      {
        for(counter = 0; counter < Width; ++counter)
        {
          inarray[counter].setGrey(trow[counter]);
        }
      }
      else //16 bits per sample
      {
        for (counter = 0; counter < Width; ++counter)
        {
          inarray[counter].setGrey(trow16[counter]);
        }
      }
      break;
    default:
      throw ImageException(IMAGE_FILE_READ_ERR);
    }
    
    delete [] trow;
    delete [] trow16;
  }
  catch(ImageException & e)
  {
    //just rethrow 
    delete [] trow;
    delete [] trow16;
    throw e;
  }
  catch(...)
  {
    if(trow)
    {
      delete [] trow;
      delete [] trow16;
    }
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

//************************************************************
void PNGImageIFile::getRectangle(long x1, long y1, long x2, long y2, 
                                 RGBPixel* inarray)
  throw(ImageException, std::bad_alloc)
{
  long int lcounter = 0, wcounter = 0;     
  RGBPixel * trow = 0;                  //temporary row.

  //check the sanity of the user
  if (x1 > x2 || y1 > y2 || x1 > Width || x2 > Width || x1 < 0 || x2 < 0)
    throw ImageException(IMAGE_BOUNDING_ERR);
  
  try
  {
    //create the row
    if(!(trow = new (std::nothrow) RGBPixel[Width]))
      throw std::bad_alloc();
    
    //build the rect
    for (lcounter = y1; lcounter <= y2; ++lcounter)
    {
      getScanline(lcounter, trow);
      
      for (wcounter = x1; wcounter <= x2; ++wcounter)
        inarray[(x2-x1+1)*(lcounter-y1) + wcounter-x1] = trow[wcounter];
      
    }
    
    delete [] trow;
    
  }
  catch(ImageException & e)
  {
    if (trow)
      delete [] trow;
      
    throw e;
  }
  catch(...)
  {
    if (trow)
      delete [] trow;
     
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}     

//*********************************************************
void PNGImageIFile::getRectangle(long x1, long y1, long x2, long y2,
                          GreyPixel* inarray)
  throw(ImageException, std::bad_alloc)
{
  long int lcounter = 0, wcounter = 0;     
  GreyPixel * trow = 0;                  //temporary row.

  //check the sanity of the user
  if (x1 > x2 || y1 > y2 || x1 > Width || x2 > Width || x1 < 0 || x2 < 0)
    throw ImageException(IMAGE_BOUNDING_ERR);
  
  try
  {
    //create the row
    if(!(trow = new (std::nothrow) GreyPixel[Width]))
      throw std::bad_alloc();
    
    //build the rect
    for (lcounter = y1; lcounter <= y2; ++lcounter)
    {
      getScanline(lcounter, trow);
      
      for (wcounter = x1; wcounter <= x2; ++wcounter)
        inarray[(x2-x1+1)*(lcounter-y1) + wcounter-x1] = trow[wcounter];
      
    }
    
    delete [] trow;
    
  }
  catch(ImageException & e)
  {
    if (trow)
      delete [] trow;
      
    throw e;
  }
  catch(...)
  {
    if (trow)
      delete [] trow;
     
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


//***************************************************************
void PNGImageIFile::getRawPixel(long x, long y, short unsigned int * inpixel)
    throw(ImageException, std::bad_alloc)
{
   short unsigned int * trow = 0;
   int counter = 0;

   if (bitsPerSample !=16) //no reason to be in here
    throw ImageException(IMAGE_INVALID_OP_ERR);


  try
  {

    //create the row
    if(!(trow = new (std::nothrow)
        short unsigned int[Width*samplesPerPixel]))throw std::bad_alloc();

    //get the scanline
    getRawScanline(y, trow);


    for(; counter < samplesPerPixel; ++counter)
    {
      inpixel[counter] =
         trow[counter + x*samplesPerPixel];
    }

    //done with the temp row
    delete [] trow;
  }
  catch(ImageException & excep)
  {
    delete [] trow;
    //just rethrow
    throw excep;
  }
  catch(...)
  {
    delete [] trow;

    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}
 
//*************************************************************
void PNGImageIFile::getRawScanline(long row, short unsigned int * inarray)
  throw(ImageException, std::bad_alloc)
{
  long int counter = 0, size(Width*samplesPerPixel);
  if (bitsPerSample != 16)
    throw ImageException(IMAGE_INVALID_OP_ERR);

  if (bytesperline % 2 != 0)  //an impossibility
    throw ImageException(IMAGE_INVALID_OP_ERR);

  if(!good())
    throw ImageException(IMAGE_FILE_READ_ERR);

  //first check the cache
  if (!cache)
    throw ImageException(IMAGE_FILE_READ_ERR);

  //check the row bounds
  if (row >= Height || row < 0)
    throw ImageException(IMAGE_BOUNDING_ERR);

  //see if the scanline is in the cache
  if (row >= cache_start && row < (cache_start + cache_size))
  {
    for (; counter < size; ++counter)
      inarray[counter] =
        (static_cast<unsigned short int>
        (cache[row - cache_start][counter*2])
         << 8) + cache[row-cache_start][2*counter+1];
  }
  else
  {
    reCache(row);
    //copy the data
    for (; counter < size; ++counter)
      inarray[counter] =
        (static_cast<unsigned short int>
        (cache[row - cache_start][counter*2])
         << 8) + cache[row-cache_start][2*counter+1];

  }
}

//************************************************************
void PNGImageIFile::getRawRectangle(long x1, long y1, long x2, long y2,
                               short unsigned int * inarray)
  throw(ImageException, std::bad_alloc)
{
   long int lcounter = 0, wcounter = 0, counter = 0;
   short unsigned int * trow = 0;

   if (bitsPerSample < 16) //check to see if we are right to be here
    throw ImageException(IMAGE_INVALID_OP_ERR);

   try
   {
      //create the temporary row
      if (!(trow = new (std::nothrow) short unsigned
               int[Width*samplesPerPixel]))
        throw std::bad_alloc();

      for(lcounter = y1; lcounter <= y2; ++lcounter)
      {
        //get the scanline
        getRawScanline(lcounter, trow);

        for(wcounter = x1; wcounter <= x2; ++wcounter)
        {
          for(counter = 0; counter < samplesPerPixel; ++counter)
            inarray[((lcounter-y1)*(x2-x1+1) + (wcounter-x1))*samplesPerPixel
            + counter] =   trow[wcounter*samplesPerPixel + counter];
        }
      }


      delete [] trow;


   }
   catch(ImageException & err)
   {
      delete [] trow;
      throw err;
   }
   catch(...)
   {
      delete [] trow;
      throw ImageException(IMAGE_FILE_READ_ERR);
   }
}

//*********************************************************************
void PNGImageIFile::getRawPixel(long x, long y, unsigned char* inpixel)
  throw(ImageException, std::bad_alloc)
{
  unsigned char * trow = 0;
  long int counter = 0;

  //It is technically possible to store 16 bit samples in a
  //unsigned char array however there are overloaded functions
  //to handle this.
  if (bitsPerSample > 8)
    throw ImageException(IMAGE_INVALID_OP_ERR);

  try
  {
    //create the row
    if (!(trow = new (std::nothrow) unsigned char[bytesperline]))
      throw std::bad_alloc();

    //get the scanline
    getRawScanline(y, trow);

    for (counter = 0; counter < samplesPerPixel; ++counter)
      inpixel[counter] = trow[x*samplesPerPixel + counter];

    //delete the row
    delete [] trow;


  }
  catch(ImageException & err)
  {
    delete [] trow;

    throw err;
  }
  catch(...)
  {
    delete [] trow;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

//*******************************************************************
void PNGImageIFile::getRawScanline(long row, unsigned char* inarray)
  throw(ImageException, std::bad_alloc)
{
  long int counter = 0;

  //It is technically possible to store 16 bit samples in a
  //unsigned char array however there are overloaded functions
  //to handle this.
  if (bitsPerSample > 8)
    throw ImageException(IMAGE_INVALID_OP_ERR);


  if(!good())
    throw ImageException(IMAGE_FILE_READ_ERR);

  //first check the cache
  if (!cache)
    throw ImageException(IMAGE_FILE_READ_ERR);


  //check the row bounds
  if (row >= Height || row < 0)
    throw ImageException(IMAGE_BOUNDING_ERR);
    
  //see if the scanline is in the cache
  if (row >= cache_start && row < (cache_start + cache_size))
  {
    //should already be in the proper form
    for (; counter < bytesperline; ++counter)
      inarray[counter] = cache[row - cache_start][counter];
  }
  else
  {
    reCache(row);
    //copy the data over
    for (; counter < bytesperline; ++counter)
      inarray[counter] = cache[row - cache_start][counter];
    
  }
}
  

//********************************************************
void PNGImageIFile::getRawRectangle(long x1, long y1, long x2, long y2,
                             unsigned char* inarray)
  throw(ImageException, std::bad_alloc)
{
  long int lcounter = 0, wcounter = 0, counter = 0;
  unsigned char * trow = 0;                  //temporary row.

  //It is technically possible to store 16 bit samples in a
  //unsigned char array however there are overloaded functions
  //to handle this.
  if (bitsPerSample > 8)
    throw ImageException(IMAGE_INVALID_OP_ERR);

  //check the sanity of the user
  if (x1 > x2 || y1 > y2 || x1 > Width || x2 > Width || x1 < 0 || x2 < 0)
    throw ImageException(IMAGE_BOUNDING_ERR);
  
  try
  {
    //create the row
    if(!(trow = new (std::nothrow) unsigned char[bytesperline]))
      throw std::bad_alloc();

    for(lcounter = y1; lcounter <= y2; ++lcounter)
    {
      getRawScanline(lcounter, trow);

      for(wcounter = x1; wcounter <= x2; ++wcounter)
      {
        for (counter = 0; counter < samplesPerPixel; ++counter)
          inarray[((lcounter-y1)*(x2-x1+1)+(wcounter-x1))*samplesPerPixel
                    +  counter] = trow[wcounter*samplesPerPixel + counter];
      }

    }
    //delete the temporary row
    delete [] trow;
  }
  catch(ImageException & err)
  {
    delete [] trow;
    throw err;             //just rethrow
  }
  catch(...)
  {
    delete [] trow;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

//*****************************************************************
void PNGImageIFile::getRawRGBPixel(long x, long y, RawRGBPixel* pix)
  throw(ImageException, std::bad_alloc)
{
  RawRGBPixel * trow = 0;
  
  //check for 16 bit data
  if (bitsPerSample > 8)
    throw ImageException(IMAGE_INVALID_OP_ERR);

  try
  {
    //create the row
    if (!(trow = new (std::nothrow) RawRGBPixel[Width]))
      throw std::bad_alloc();
    
    //read the scanline
    getRawRGBScanline(y, trow);
    
    (*pix) = trow[x];
    delete [] trow;
    
  }
  catch(...)
  {
    if (trow)
      delete [] trow;
    
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
  
}

//****************************************************************
void PNGImageIFile::getRawRGBScanline(long row, RawRGBPixel* pixbuf)
  throw(ImageException, std::bad_alloc)
{
  RGBPixel * trow = 0;
  long int counter = 0;
  short unsigned int color = 0;
  //check for 16 bit data that can't be read to a RawRGB
  if (bitsPerSample > 8)
    throw ImageException(IMAGE_INVALID_OP_ERR);

  try
  {
    //create the row
    if (!(trow = new (std::nothrow) RGBPixel[Width]))
      throw std::bad_alloc();
    
    getScanline(row, trow);
    
    for (counter = 0; counter < Width; ++counter)
    {
      trow[counter].getRed(color);
      pixbuf[counter].Red = static_cast<unsigned char>(color);
      
      trow[counter].getGreen(color);
      pixbuf[counter].Green = static_cast<unsigned char>(color);

      trow[counter].getBlue(color);
      pixbuf[counter].Blue = static_cast<unsigned char>(color);
    }

    delete [] trow;
  }
  catch(...)
  {
    if (trow)
      delete [] trow;
    
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

//************************************************************
void PNGImageIFile::getRawRGBRectangle(long x1, long y1, long x2, long y2,
                                RawRGBPixel* pixbuf)
  throw(ImageException, std::bad_alloc)
{
  RawRGBPixel * trow = 0;
  
  long int lcounter = 0, wcounter = 0;

  //check to see if this is the rights bits per sample
  if(bitsPerSample > 8)
   throw ImageException(IMAGE_INVALID_OP_ERR);


  try
  {
    //create the row
    if(!(trow = new (std::nothrow) RawRGBPixel[Width]))
      throw std::bad_alloc();
    
    for (lcounter = y1; lcounter <= y2; ++lcounter)
    {
      
      //read the row
      getRawRGBScanline(lcounter, trow);
      
      for(wcounter = x1; wcounter <= x2; ++wcounter)
      {
        pixbuf[(lcounter-y1)*(x2-x1) + (wcounter-x1)] =
          trow[wcounter];
      }
    }

    delete [] trow;
  }
  catch(...)
  {
    if(trow)
      delete [] trow;
    
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}
          

//***********************************************************
void PNGImageIFile::getOptimalRectangle(long int& w, long int& h)
  throw(ImageException, std::bad_alloc)
{
  //this makes no sense here (or much sense any where else for that matter)
  w = Width;
  h = Height;
}

#endif

