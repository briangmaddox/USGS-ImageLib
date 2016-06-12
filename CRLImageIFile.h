#ifndef _CRLIMAGEIFILE_H_
#define _CRLIMAGEIFILE_H_
// $Id: CRLImageIFile.h,v 1.1.1.1 2002-02-21 00:18:56 bmaddox Exp $
// Christopher Bilderback - USGS MCMC SES
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:56 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class reads Intergraph Tiled format images.

#include <fstream>
#include "ImageIFile.h"
#include "RGBPalette.h"

namespace USGSImageLib
{

typedef struct
{
  unsigned char slot;
  unsigned char red;
  unsigned char green;
  unsigned char blue;
} ColorCell;

struct Tile
{
  long int xorg;
  long int yorg;
  long int height;
  long int width;
  unsigned char* data;
  ~Tile();
};

typedef struct
{
  long int offset;
  long int alloc;
  long int used;
} DirEntry;


class CRLImageIFile : public ImageIFile
{
public:
  CRLImageIFile(std::string& filename);
  virtual ~CRLImageIFile();
  
  //tiled handling functions
  void getMaxTileWidth(long int& inwidth)
    const throw(ImageException);
  void getMaxTileHeight(long int& inheight)
    const throw(ImageException);
  void getTileWidth(int tile_num, long int& inwidth)
    const throw(ImageException);
  void getTileHeight(int tile_num, long int& inheight)
    const throw(ImageException);
  void getTilesWide(int& inwidth)
    const throw(ImageException);
  void getTilesHigh(int& inheight)
    const throw(ImageException);
  void getNumTiles(int& innumber)
    const throw(ImageException);
  
  virtual void getPalette(Palette* inpalette)
    throw(ImageException, std::bad_alloc);
  
  virtual void getPixel(long x, long y, AbstractPixel* inpixel)
    throw(ImageException, std::bad_alloc);
    
  virtual void getScanline(long row, RGBPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getScanline(long row, GreyPixel* inarray)
    throw(ImageException, std::bad_alloc);
    
  virtual void getRectangle(long x1, long y1, long x2, long y2,
			    RGBPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRectangle(long x1, long y1, long x2, long y2,
			    GreyPixel* inarray)
    throw(ImageException, std::bad_alloc);

  virtual void getRawPixel(long x, long y, unsigned char* inpixel)
    throw(ImageException, std::bad_alloc);
  virtual void getRawScanline(long row, unsigned char* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRawRectangle(long x1, long y1, long x2, long y2,
                               unsigned char* inarray) 
    throw (ImageException, std::bad_alloc);
  
protected:
  int  readWord()       const throw(ImageException);
  long readDWord()      const throw(ImageException);
  void readColorMap()   throw(ImageException, std::bad_alloc);
  void readTileDir()    throw(ImageException, std::bad_alloc);
  long getTileSize(int tile_num) const throw(ImageException);
  void readScanLine(unsigned char* scanline) 
    throw (ImageException, std::bad_alloc);
  
  // File access
  void getFileWidth(long int& inwidth)    throw(ImageException);
  void getFileHeight(long int& inheight)  throw(ImageException);
  
  void getColorMap(const ColorCell* inmap) const throw();
  void setupPalette() throw (std::bad_alloc);
  
  void getTile(int tile_num, Tile* intile)
    throw(ImageException, std::bad_alloc);
  
  void getScanlineOffsets()
    throw(ImageException, std::bad_alloc);
    
private:
  std::ifstream*    Stream;
  long              dir_offset;
  ColorCell*        color_map;
  int               co_map_entries;
  DirEntry*         tile_dir;
  RGBPalette*       palette;
  
  //tile cache stuff
  Tile** cache;
  int * lookuptable;
  int * tilenum;
  int * nexttile;
  int cachesize;
  
  bool tiled;
  long TopOfImageData;
  long maxtilewidth;
  int tileswide;
  int tileshigh;
  int* s_off;
};

} // namespace

#endif

