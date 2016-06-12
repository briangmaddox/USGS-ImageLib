#ifndef _CRLIMAGEIFILE_CPP_
#define _CRLIMAGEIFILE_CPP_
// $Id: CRLImageIFile.cpp,v 1.2 2003-06-28 16:28:40 bmaddox Exp $
// Chris Bilderback - USGS MCMC SES - cbilderback@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:40 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/
 
// Implementation of the CRLImageIFile class

#ifdef _WIN32
#pragma warning( disable : 4291 ) // Disable VC warning messages for
                                  // new(nothrow)
#endif

#include "CRLImageIFile.h"

using namespace USGSImageLib;

// **************************************************************************
Tile::~Tile()
{
  try
  {
    delete data;
  }
  catch (...) 
  {
   // don't do anything since this is the destructor
  }
}

// **************************************************************************
CRLImageIFile::CRLImageIFile(std::string& filename)
  : Stream(0), color_map(0), tile_dir(0), palette(0),
    cache(0), lookuptable(0), tilenum(0), s_off(0)
{
  unsigned int data_type;
  int tilelookupsize, i, j;  
  try
  {
    //create the file stream
    if (!(Stream = new(std::nothrow) std::ifstream(filename.c_str(), 
                                            std::ios::in | std::ios::binary)))
      throw std::bad_alloc();
    //set image attributes
    setPhotometric(PHOTO_PALETTE);
    setHasPalette();
    setBitsPerSample(8);
    setSamplesPerPixel(1);
    setImageType(IMAGE_CRL);

    Stream->seekg(4, std::ios::beg);//seek to data type position
    data_type = readWord();//get data type word
    if (data_type == 65)//65 is a tiled file
      tiled = true;
    else if (data_type == 10)//10 is color run enconded data
      tiled = false;
    else
    {//this class doesn't handle any other type of data
      setNoDataBit();
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    }

    if (tiled)
    {
      getFileWidth(Width);
      getFileHeight(Height);
      readColorMap();//color map that maps RGB colors to pixel indexes
      readTileDir();//tile directory holds the data about the tiles
      setupPalette();

      //setup the cache
      getTilesWide(cachesize);
      if(!(cache = new (std::nothrow) Tile * [cachesize]))
        throw std::bad_alloc();
      if(!(tilenum = new (std::nothrow) int [cachesize]))
        throw std::bad_alloc();
      nexttile = tilenum;
      getNumTiles(tilelookupsize);
      if(!(lookuptable = new (std::nothrow) int [tilelookupsize]))
        throw std::bad_alloc();
	  
      for(i = 0; i < cachesize; i++)
      {
	cache[i] = 0;
	tilenum[i] = -1;
      }
      for(j = 0; j < tilelookupsize; j++)
        lookuptable[j] = -1;
    }
    else
    {
      Stream->seekg(2, std::ios::beg);  // move to the Words To Follow entry
      TopOfImageData = readWord();      // read Words To Follow
      TopOfImageData = (TopOfImageData + 2) * 2;  // convert the Words to
                                                  // Follow to bytes
      readColorMap();
      setupPalette();
      getFileWidth(Width);
      getFileHeight(Height);
      getScanlineOffsets();
      
    }
  }
  catch (...)
  {
    setNoDataBit();
    //check for existing exongenous data
    delete Stream;
    delete [] cache;
    delete [] tilenum;
    delete [] lookuptable;
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }
}


// **************************************************************************
CRLImageIFile::~CRLImageIFile()
{
  int i;
  
  try
  {
    //check for existing exongenous data and delete it
    delete Stream;
    delete palette;
    delete [] color_map;
    delete [] tile_dir;
    delete [] s_off;
    for ( i = 0; i < cachesize; i++)
    {
      delete cache[i];
    }
    delete [] cache;
    delete [] tilenum;
    delete [] lookuptable;
  }
  catch (...)
  {
    // don't do anything since destructor, just exit
  }
}


// **************************************************************************
// Gets the Image Width in pixels (bytes)
void CRLImageIFile::getFileWidth(long int& inwidth) throw(ImageException)
{
  try
  {
    Stream->seekg(184); // move to the pixels per line entry in the file header
    inwidth = readDWord(); //read in the pixels per line entry
    return;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
// Gets the Image Height in pixels (bytes)
void CRLImageIFile::getFileHeight(long int& inheight) throw(ImageException)
{
  try
  {
    Stream->seekg(188);//move to the Number of Lines in the file header
    inheight = readDWord();//read in the Number of Lines
    return;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
// Gets the Max Tile Width
void CRLImageIFile::getMaxTileWidth(long int & inwidth) 
  const throw (ImageException)
{
  try
  {
    //make sure that the image is tiled
    if (tiled)
    {
      Stream->seekg(dir_offset + 120);//move to tile size entry
      inwidth = readDWord();//read in the tile size entry
    }
    else
      throw;
  }
  catch(...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// Gets the Maximum Height of the the Tiles
void CRLImageIFile::getMaxTileHeight(long int& inheight)
  const throw(ImageException)
{
  getMaxTileWidth(inheight); // full tiles are always square
}


// ***************************************************************************
// Gets the width of a particular tile
void CRLImageIFile::getTileWidth(int tile_num, long int& inwidth)
  const throw(ImageException)
{
  int  tile_pos;//in what column does the tile live 
  
  try
  {
    if (tiled)
    {      
      		
      // find what column the tile is in
      tile_pos = ( (tile_num - 1) % tileswide ) + 1;

      if (tile_pos == tileswide)
      { // the tile was in the last and incomplete column
        inwidth = ((Width-1) % maxtilewidth) + 1;//get the tile width
      }
      else
        inwidth = maxtilewidth;//tile should be full
    }
   else
     inwidth = 0;
    
    return;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// Gets a specific tile height
void CRLImageIFile::getTileHeight(int tile_num, long& inheight)
  const throw(ImageException)
{

  try
  {
    //check for valid tiled file
    if (tiled)
    {
      //check to see if the tile resides in the last row
      if (tile_num <= (tileswide * (tileshigh - 1)))
	inheight = maxtilewidth;
      else
      {	
        //was not a full tile
        inheight = ((Height - 1) % maxtilewidth) + 1;
      }
    }
    else
      inheight = 0;
    
    return;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// Gets the number of tiles in the image width
void CRLImageIFile::getTilesWide(int& inheight)
  const throw(ImageException)
{
  try
  {
    //check for tiled file
    if (tiled)
    {
      //get the number of tiles in the file
      inheight = ( (Width + maxtilewidth - 1) / maxtilewidth);
    }
    else
      inheight = 0;//image was not tiled therefore this not applicable
    return;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// Get the number of tiles that fits in image height
void CRLImageIFile::getTilesHigh(int& inheight)
  const throw(ImageException)
{
  
  try
  {
    //check to see if the file is a valid tiled file
    if (tiled)
    {
      inheight = ( (Height + maxtilewidth - 1) / maxtilewidth);
      /*if you are wondering about using maxtilewidth as
        maxtileheight it is because tiles are SQUARE. */ 
    }
    else
      inheight = 0;//not tiled so return
    
    return;
  }
  catch(...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// Gets the total number of tiles in the file
void CRLImageIFile::getNumTiles(int& innumber)
  const throw(ImageException)
{

  try
  {
    //check to make sure the file is tiled
    if (tiled)
    {
      innumber = tileswide * tileshigh;
    }
    else
      innumber = 0;//not tiled so this invalid

    return;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// Returns the color map
void CRLImageIFile::getColorMap(const ColorCell* inmap) const throw()
{
  inmap = color_map;
  return;
}

// ***************************************************************************
// Gets a single tile
// Assumes memory for the tile and the tile data have been created
void CRLImageIFile::getTile(int tile_num, Tile* intile)
  throw(ImageException, std::bad_alloc)
{
 
  long  tilewidth, tileheight, tilesize;
  int offset = 0;
     
  try
  {
    if (tiled)
    {
      //check for this tile in the cache
      if (lookuptable[tile_num-1] > -1)
      {
        //set up tile attributes
        intile->width = cache[lookuptable[tile_num-1]]->width;
        intile->height = cache[lookuptable[tile_num-1]]->height;
        tilesize = intile->width * intile->height;
        intile->xorg = ((tile_num - 1) % tileswide) * maxtilewidth;
        intile->yorg = ((tile_num - 1) / tileshigh) * maxtilewidth;
                	
	//copy the actual tile data 
	memcpy(intile->data, cache[lookuptable[tile_num-1]]->data, tilesize);
      }
      else
      { 
        getTileWidth(tile_num, tilewidth);
        getTileHeight(tile_num, tileheight);
        tilesize = tilewidth*tileheight;
		
        //set the tile's data members
        intile->xorg = ((tile_num - 1) % tileswide) * maxtilewidth;
        intile->yorg = ((tile_num - 1) / tileshigh) * maxtilewidth;
        intile->width = tilewidth;
        intile->height = tileheight;
 	    				
        Stream->seekg(tile_dir[tile_num-1].offset);//move to tile data
        if (Stream->fail())
	  throw;
      		
        for (int i = 0; i < tileheight; i++)
	  readScanLine(intile->data + i * intile->width);
		
        //put this tile in the cache
        offset = nexttile - tilenum;
        if (offset >= cachesize)
        {
          offset = 0;
          nexttile = tilenum;
        }
        //check cache
        delete cache[offset];
        //create a new cache entry
        if(!(cache[offset] = new (std::nothrow) Tile))
          throw std::bad_alloc();
        if(!(cache[offset]->data = new (std::nothrow) unsigned char[tilesize]))
          throw std::bad_alloc();
        //copy the data
        memcpy(cache[offset]->data, intile->data, tilesize);
        //set tile attribs
        cache[offset]->height = intile->height;
        cache[offset]->width = intile->width;
        cache[offset]->xorg = intile->xorg;
        cache[offset]->yorg = intile->yorg;
			
        //check the offset
        if (tilenum[offset] > -1)
	  lookuptable[tilenum[offset]-1] = -1;//this tile no longer cached
        tilenum[offset] = tile_num;
        lookuptable[tile_num-1] = offset;
        nexttile++;

      }
      
    }
    else
    {
      delete intile;
      intile = 0;
    }

    return;
  }
  catch (...)
  {
    delete intile;
    intile = 0;
    delete cache[offset];
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
// Reads a word of data and inverts it
int CRLImageIFile::readWord()
  const throw (ImageException)
{
  unsigned char byte1 = 0, byte2 = 0;
  try
  {
    Stream->read(reinterpret_cast<char*>(&byte2),1);
    Stream->read(reinterpret_cast<char*>(&byte1),1);

    return ((byte1 << 8)+byte2);//return inverted Word
  }
  catch(...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
// Reads a Dword of data and inverts it
long CRLImageIFile::readDWord()
  const throw(ImageException)
{
  unsigned char byte1 = 0, byte2 = 0, byte3 = 0, byte4 = 0;

  try
  {
    Stream->read(reinterpret_cast<char*>(&byte4), 1);
    Stream->read(reinterpret_cast<char*>(&byte3), 1);
    Stream->read(reinterpret_cast<char*>(&byte2), 1);
    Stream->read(reinterpret_cast<char*>(&byte1), 1);

    return ( (byte1 << 24) + (byte2 << 16) + 
	     (byte3 << 8) + byte4);//return the inverted DWORD
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ***************************************************************************
void CRLImageIFile::readScanLine(unsigned char* scanline)
throw (ImageException, std::bad_alloc)
{
  int runs, i;
  int color = 0, pixels = 0;
  unsigned char* tptr = 0;
  int lineNum;
  unsigned char* buf = 0;

  try
  {
    //check for the "magic" scan number
    if (readWord() != 0x5900)
    {
      setNoDataBit();
      throw ImageException(IMAGE_FILE_READ_ERR);
    }
    
    // number of run lengths
    runs = ( (readWord() - 2) / 2);

    // discard line number
    lineNum = readWord();

    // attempt to allocate memory for our buffer
    if (!(buf = new(std::nothrow) unsigned char[runs * 4]))
      throw std::bad_alloc();

    // pixel offset
    if (readWord())
    {
      setNoDataBit();
      throw ImageException(IMAGE_FILE_READ_ERR);
    }
    
    Stream->read(reinterpret_cast<char*>(buf), runs * 4);
	
	//setup pointers
    tptr = scanline;
    
    for (i = 0; i < runs; i++)
    { //copy image data
      color = (buf[4 * i + 1] << 8) + buf[4 * i];
      pixels = (buf[4 * i + 3] << 8) + buf[4 * i + 2];
      memset(tptr, color, pixels);
      tptr += pixels;
    }

    delete [] buf;
    
  }
  catch(...)
  {
    delete [] buf;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
// reads in the color map
void CRLImageIFile::readColorMap()
  throw (ImageException, std::bad_alloc)
{
  int ctabletype, i;

  try
  {
    //seek to the color table type
    Stream->seekg(532, std::ios::beg);
    ctabletype = readWord();
    if (ctabletype != 2)//not Environ-V Color Table
      throw;//can't read color table
				
    Stream->seekg(536);
    co_map_entries = readDWord();//get the number of color table entries
		
    if (!(color_map = new(std::nothrow) ColorCell[co_map_entries]))
      throw std::bad_alloc();

    Stream->seekg(1024);//move to third block to get color cell entires
		
    for (i = 0; i < co_map_entries; i++)//get the color cell entries
    {
      color_map[i].slot = readWord();
      //shift right to get most significant bit 
      color_map[i].red = readWord() >> 8;
      color_map[i].green = readWord() >> 8;
      color_map[i].blue = readWord() >> 8;
    }
  }
  catch (...)
  {
    delete [] color_map;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ***************************************************************************
// Reads the TileDirectory
void CRLImageIFile::readTileDir()
  throw (ImageException, std::bad_alloc)
{
  char scannable;
  int numoftiles, i;// number-o-tiles
   
  try
  {
    if (!tiled)//check for tiled file
      throw;
					
    //see that the tiles are scannable
    Stream->seekg(195);
    Stream->get(scannable);
    if (!scannable)
      throw;
		
    Stream->seekg(2);//get the Words to Follow
    dir_offset = readWord();
    dir_offset = 2 * (dir_offset + 2);//covert Words to Bytes
    //get the max tile width
    getMaxTileWidth(maxtilewidth);
    getTilesWide(tileswide);
    getTilesHigh(tileshigh);
    //get number of tiles
    getNumTiles(numoftiles);
		
    if (!(tile_dir = new(std::nothrow) DirEntry[numoftiles]))
      throw std::bad_alloc();
		
    Stream->seekg(dir_offset+128);//move to tile data within tile dir			
    for (i = 0; i < numoftiles; i++)
    {
      //total offset is from the beginning of the file
      tile_dir[i].offset = dir_offset + readDWord();
      tile_dir[i].alloc = readDWord();//number of bytes allocated for the tile
      tile_dir[i].used = readDWord();//number of bytes used for this tile
    }
  }
  catch(...)
  {
    delete [] tile_dir;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ***************************************************************************
// Gets the scan line offsets
void CRLImageIFile::getScanlineOffsets()
  throw (ImageException, std::bad_alloc)
{
  long thisline, nextline, id, Wordstofollow;
  
  try
  {
    nextline = TopOfImageData;//setup nextline
    
    //try to create the offset array
    if (!(s_off = new(std::nothrow) int[Height]))
      throw std::bad_alloc();
    
    Stream->seekg(nextline);//move to the top of the image data
    
    for (thisline = 0; thisline < Height; thisline++)
    {
      //setup this line			
      s_off[thisline] = nextline;
      //get the scanline id
      id = readWord();
      //check for the magic number
      if (id != 0x5900)
      {
        setNoDataBit();
        throw;
      }
      
      Wordstofollow = readWord();
      nextline += (Wordstofollow * 2) + 4;//move to the next header
      Stream->seekg(nextline);
    }
  }
  catch(...)
  {
    delete [] s_off;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}	

			

// **************************************************************************
//Sets up CRL's RGB Palette from the file's color map
void CRLImageIFile::setupPalette()
  throw (std::bad_alloc)
{
  int i;//i might stand for index
  RGBPixel * temp = 0;
  
  try
  {
    if (!(temp = new (std::nothrow) RGBPixel[256]))
      throw std::bad_alloc();
    //initialize the array
    for (i = 0; i < 256; i++)
    {
      temp[i].setRed(0);
      temp[i].setGreen(0);
      temp[i].setBlue(0);
    }

    //fill out the palette
    for (i = 0; i < co_map_entries; i++)
    {
      //setup a pixel
      temp[color_map[i].slot].setRed(color_map[i].red);
      temp[color_map[i].slot].setGreen(color_map[i].green);
      temp[color_map[i].slot].setBlue(color_map[i].blue);
    }
    //delete the palette if it exists
    delete palette;
    //create the palette
    if (!(palette = new(std::nothrow) RGBPalette(256, temp)))
      throw std::bad_alloc();
    delete [] temp;

  }
  catch (...)
  {
    delete [] temp;
    delete palette;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ***************************************************************************
// assumes that memory for palette has been passed into the function for the
// color palette
void CRLImageIFile::getPalette(Palette* inpalette)
   throw (ImageException, std::bad_alloc)
{
  RGBPixel* rgbp = 0; //temporary RGB pixel
  RGBPalette* temppal = (RGBPalette*)inpalette;
  unsigned short color;
  int i;
  
  //check for the existance of the palette
  if (!palette)
    throw (IMAGE_INVALID_OP_ERR);
	
  //create an array of pixels
  if (!(rgbp = new(std::nothrow) RGBPixel[palette->getNoEntries()]))
    throw std::bad_alloc();

  try
  {
    //fill out the array of pixels
    for (i = 0; i < palette->getNoEntries(); i++)
    {
      (*palette)[i].getRed(color);
      rgbp[i].setRed(color);
      (*palette)[i].getGreen(color);
      rgbp[i].setGreen(color);
      (*palette)[i].getBlue(color);
      rgbp[i].setBlue(color);
    }
    //set the palette
    temppal->setPalette(palette->getNoEntries(), rgbp);

    delete[] rgbp;//clean the junk
  }
  catch(...)
  {
    delete[] rgbp;//error, but still have to clean junk
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}	

// ***************************************************************************
// Gets a RawScanline of data, assumes memory for scanline has been allocated
void CRLImageIFile::getRawScanline(long row, unsigned char* inarray)
  throw(ImageException, std::bad_alloc)
{
  Tile * temptile = 0;
  long int offset = 0, x, tilenum;
  long int intiley, tilecolumn;
  unsigned char *lpdata = 0, *lpinarray = 0;

  //check for valid row
  if (!validateRow(row))
    throw ImageException(IMAGE_BOUNDING_ERR);
  try
  {
    //check to see if tiled file
    if (tiled)
    {
      //create a new tile
      if (!(temptile = new(std::nothrow) Tile))
	throw std::bad_alloc();
      
      //create space for maximum tile size
      if (!(temptile->data = 
	    new (std::nothrow) unsigned char [maxtilewidth * maxtilewidth]))
	throw std::bad_alloc();			
      
      intiley = row % maxtilewidth;//get the in tile y
      tilecolumn = row / maxtilewidth;//get the tile column that the row is in
			
      //loop through the tiles that make up the scanline
      for (x = 0; x < tileswide; x++)
      {
        //get the tilenumber
        tilenum = tilecolumn * tileswide + x + 1;
        //get the particular tile
        getTile(tilenum, temptile);
        //setup pointers
        lpinarray = inarray + offset;
        lpdata = &(temptile->data[intiley * (temptile->width)]);
        //copy data
        memcpy(lpinarray, lpdata, (temptile->width));
        //move offset
        offset += (temptile->width);
      }
      delete temptile;
    }
    else
    {
      //move to the scanline
      Stream->seekg(s_off[row]);
      //read in the scan line
      readScanLine(inarray);
    }
  }
  catch(...)
  { 
    delete temptile;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ****************************************************************************
// Gets a RawPixel, assume that caller has allocated memory for the raw pixel
void CRLImageIFile::getRawPixel(long x, long y, unsigned char* inpixel)
  throw(ImageException, std::bad_alloc)
{
  long intilex, intiley, tilex, tiley, tilenumber;
  Tile* tartile = 0; //the tile that the target is in
  unsigned char * templine = 0;

  //check to see if the pixel is within the image
  if (!validateXY(x,y))
    throw ImageException(IMAGE_BOUNDING_ERR);

  try
  {
    //check for tiled case
    if (tiled)
    {
      //create a new tile
      if (!(tartile = new(std::nothrow) Tile))
	throw std::bad_alloc();
      
      //create tile data
      if (!(tartile->data =
	    new (std::nothrow) unsigned char[maxtilewidth*maxtilewidth]))
	throw std::bad_alloc();
      
      intilex = x % maxtilewidth;//set the x coord relative to the tile
      intiley = y % maxtilewidth;//set the y coord relative to the tile
      tilex = x / maxtilewidth;//set the tile x relative to the file
      tiley = y / maxtilewidth;//set the tile y relative to the file
	
      //find the number of the tile we want
      tilenumber = (tiley*tileswide)+tilex+1;
	
      getTile(tilenumber, tartile);//get the tile
      //set the pixel
      *inpixel = tartile->data[intiley * (tartile->width) + intilex];
		
      //delete memory allocated 
      delete tartile;
		
    }
    else // the file was not tiled
    {
		
      //allocate space for a new line
      if (!(templine = new (std::nothrow) unsigned char [Width]))
	throw std::bad_alloc();
      //move to the scanline that the pixel is in
      Stream->seekg(s_off[y]);
      //read in the line that the scanline is in 
      readScanLine(templine);
      //set the pixel
      *inpixel = templine[x];
      //clean garabage
      delete [] templine;
    }
  }
  catch(...)
  {
    delete tartile;
    delete templine;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}
// ***************************************************************************
// Gets a RawRectangle with horizantal scanlines
// Again, assumes memory has been passed in
void CRLImageIFile::getRawRectangle(long x1, long y1, long x2, long y2,
				    unsigned char * inarray)
  throw(ImageException, std::bad_alloc)
{
  Tile *  temptile=0;
  long column, row, intilex, intiley,
    imgx, imgy, tilewidth;
  int tilenumber, oldtilenumber=0;
  unsigned char * temppointer = inarray;//temporary pointer into the inarray
  unsigned char *lpline=0;
  
  //check for valid coordinates
  if (!(validateXY(x1, y1) || validateXY(x2, y2)))
    throw ImageException(IMAGE_BOUNDING_ERR);
  
  try
  {
    //check to see if its tiled
    if (tiled)
    {
      //attempt to create the tile
      if (!(temptile = new (std::nothrow) Tile))
	throw std::bad_alloc();
      
      if (!(temptile->data =
	    new (std::nothrow) unsigned char [maxtilewidth * maxtilewidth]))
	throw std::bad_alloc();
      
      //start at lower left of rect and move up to upper right reading as we go
      for (imgy = y1; imgy <= y2; imgy++)
      {
	//get what row of tiles we are in
	row = imgy / maxtilewidth;
	//get the y value within the tile
	intiley = imgy % maxtilewidth;
	//now go across and get the scanlines
	for (imgx = x1; imgx <= x2; imgx++)
	{
	  //get the column we are in
	  column = imgx / maxtilewidth;
	  //get the intile x
	  intilex = imgx % maxtilewidth;
	  //get the tile number
	  tilenumber = row * tileswide + column + 1;
	  //check to see if we are in the same tile
	  if (oldtilenumber != tilenumber)
	  {    
	    oldtilenumber = tilenumber;
	    //retrieve the tile
	    getTile(tilenumber, temptile);
	    //get the tilewidth
	    getTileWidth(tilenumber, tilewidth);
	  }
	  *temppointer = temptile->data[intiley*tilewidth+intilex];
	  temppointer++;//move the temporary pointer
	}
      }
      //clean garabage
      delete temptile;
    }
    else
    {
      //create space for a new line
      if (!(lpline = new (std::nothrow) unsigned char [Width]))
	throw std::bad_alloc();
      
      for (int y = y1; y <= y2; y++)
      {
	//move to the scanline
	Stream->seekg(s_off[y]);
	//read in the line
	readScanLine(lpline);
	//copy bytes
	for (int i = 0; i <= x2-x1; i++)
	  temppointer[i] = lpline[x1 + i]; 
	//move the lpinarray to the next line
	temppointer += x2-x1+1;
      }
      
      delete [] lpline;
    }
  }
  catch(...)
  {
    delete temptile;
    delete [] lpline;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ***********************************************************************
// Gets an Abstract Pixel
void CRLImageIFile::getPixel(long x, long y, AbstractPixel * inpixel)
  throw (ImageException, std::bad_alloc)
{
  
  unsigned char temppixel;
  short unsigned int color;
  try
  {
    //get the pixel
    getRawPixel(x, y, &temppixel);
    
    //set the pixel via RGB
    (*palette)[temppixel].getRed(color);
    (*inpixel).setRed(color);
    (*palette)[temppixel].getGreen(color);
    (*inpixel).setGreen(color);
    (*palette)[temppixel].getBlue(color);
    (*inpixel).setBlue(color);
  }
  catch(...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ************************************************************************
void CRLImageIFile::getScanline(long row, RGBPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned char * templine=0;
  int x;
  short unsigned int color;
  //try to create the line
  if(!(templine = new (std::nothrow) unsigned char[Width]))
    throw std::bad_alloc();
  try
  {
    //get the scanline
    getRawScanline(row, templine);
    //loop through the line and copy RGB data
    for ( x = 0; x < Width; x++)
    {
      (*palette)[templine[x]].getRed(color);
      inarray[x].setRed(color);
      (*palette)[templine[x]].getGreen(color);
      inarray[x].setGreen(color);
      (*palette)[templine[x]].getBlue(color);
      inarray[x].setBlue(color);
    }
    //clean garbage
    delete [] templine;
  }
  catch(...)
  {
    //templine is still around delete it
    delete [] templine;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ***************************************************************************
void CRLImageIFile::getScanline(long row, GreyPixel* inarray)
   throw (ImageException, std::bad_alloc)
{
  unsigned char * templine =0;//temp RGB line
  short unsigned int grey;//temp color
  int x;
  //try to create the line
  if (!(templine = new (std::nothrow) unsigned char[Width]))
    throw std::bad_alloc();
  try
  {
    //get the scanline in question
    getRawScanline(row, templine);
    //loop through the line setting grey values
    for (x = 0; x < Width; x++)
    {
      (*palette)[templine[x]].getGrey(grey);
      inarray[x].setGrey(grey);
    }
    //delete garbage
    delete [] templine;
  }
  catch(...)
  {
    //templine is still around delete it
    delete [] templine;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// ***************************************************************************
// Gets a RGB rectangle of data
void CRLImageIFile::getRectangle(long x1, long y1, long x2, long y2, 
				 RGBPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned char * temprect=0;//buffer for RawRectangle
  long area = (y2 - y1 + 1) * (x2 - x1 + 1);
  short unsigned int color;
  int x;
  //try to create temp rect
  if (!(temprect = new (std::nothrow) unsigned char[area]))
    throw std::bad_alloc();
  try
  {
    //get the raw rectangle
    getRawRectangle(x1, y1, x2, y2, temprect);
    //loop through values setting RGB values
    for( x = 0; x < area; x++)
    {
      (*palette)[temprect[x]].getRed(color);
      inarray[x].setRed(color);
      (*palette)[temprect[x]].getGreen(color);
      inarray[x].setGreen(color);
      (*palette)[temprect[x]].getBlue(color);
      inarray[x].setBlue(color);
    }
    //clean memory
    delete [] temprect;
  }
  catch(...)
  {
    //delete temprect
    delete [] temprect;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}
// ****************************************************************************
// Gets a Grey scale rectangle of data
void CRLImageIFile::getRectangle(long x1, long y1, long x2, long y2, 
				 GreyPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  unsigned char * temprect = 0;//temporary rectangle
  long area = (y2 - y1 + 1) * (x2 - x1 + 1);
  short unsigned int grey;
  int x;
  //try to create temp rect
  if (!(temprect = new (std::nothrow) unsigned char[area]))
    throw std::bad_alloc();

  try
  {
    //get the raw rectangle
    getRawRectangle(x1, y1, x2, y2, temprect);
    //loop through values setting Grey values
    for( x = 0; x < area; x++)
    {
      (*palette)[temprect[x]].getGrey(grey);
      inarray[x].setGrey(grey);
    }
    //clean memory
    delete [] temprect;
  }
  catch(...)
  {
    //delete temprect
    delete [] temprect;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

#endif
