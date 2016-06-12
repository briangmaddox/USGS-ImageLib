#ifndef _IMAGEFILE_CPP_
#define _IMAGEFILE_CPP_
// $Id: ImageFile.cpp,v 1.1.1.1 2002-02-21 00:19:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:01 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Implements class ImageFile

#include <string>
#include "ImageFile.h"

using namespace USGSImageLib;

// ***************************************************************************
ImageFile::ImageFile(void) throw(ImageException, std::bad_alloc)
  : ImageData()
{
}


// ***************************************************************************
ImageFile::ImageFile(std::string& fn, long int width, long int height)
  throw(ImageException, std::bad_alloc)
    : ImageData(width, height), filename(fn)
{
}


// ***************************************************************************
ImageFile::ImageFile(std::string& fn) throw(ImageException, std::bad_alloc)
  : ImageData(), filename(fn)
{
}
    

// ***************************************************************************
ImageFile::~ImageFile()
{
}


// ***************************************************************************
void ImageFile::setFileName(std::string& inname) throw()
{
  filename = inname;
}


// ***************************************************************************
void ImageFile::getFileName(std::string& _filename) const throw()
{
  _filename = filename;
}


// ***************************************************************************
bool ImageFile::validateRow(long int& row) const throw()
{
  if ( (row < 0) || (row >= Height) )
    return false;
  else
    return true;
}


// ***************************************************************************
bool ImageFile::validateXY(long int& x, long int& y) const throw()
{
  if ( (x < 0) || (x >= Width) || (y < 0) || (y >= Height) )
    return false;
  else
    return true;
}

// ***************************************************************************
// Normalize the passed in coordinates.  By normalization, the coordinates
// will be modified so that (x1, y1) represents the upper left corner of the
// rectangle and (x2, y2) is the lower right corner.

bool ImageFile::normalizeCoords(long int& x1, long int& y1,
                                long int& x2, long int& y2)
  const throw()
{
  long int tempvalue;

  if ( (x1 < 0) || (x2 < 0) || (x1 > Width) || (x2 > Width) || (y1 < 0) ||
       (y2 < 0) || (y1 >= Height) || (y2 >= Height) )
    return false;

  // Ok, so the coordinates are within the bound of the image.  Now normalize
  // them
  // For the x values, greater is based on left to right
  if (x2 < x1) // need to switch the x coordinates
  {
    tempvalue = x1;
    x1 = x2;
    x2 = tempvalue;
  }
  
  // For the y values, greates is based top to bottom
  if (y1 > y2)
  {
    tempvalue = y1;
    y1 = y2;
    y2 = tempvalue;
  }

  return true;
}

#endif
