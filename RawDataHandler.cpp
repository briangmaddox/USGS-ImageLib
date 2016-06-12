#ifndef _RAWDATAHANDLER_CPP_
#define _RAWDATAHANDLER_CPP_
// $Id: RawDataHandler.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// 21 May 1998

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "RawDataHandler.h"

using namespace USGSImageLib;

// ***************************************************************************
// Default/Parameterized constructor
RawDataHandler::RawDataHandler(long int inwidth, long int inheight,
                               int inphotometric) throw()
  : Height(inheight), Width(inwidth), Photometric(inphotometric)
{
  setBytesPerPixel();
}


// ***************************************************************************
RawDataHandler::~RawDataHandler()
{
}


// ***************************************************************************
// Set the height of the image
bool RawDataHandler::setHeight(long int inheight) throw()
{
  Height = inheight;
  return true;
}


// ***************************************************************************
bool RawDataHandler::setWidth(long int inwidth) throw()
{
  Width = inwidth;
  ByteWidth = Width * BytesPerPixel;
  return true;
}


// ****************************************************************************
bool RawDataHandler::setPhotometric(int inphotometric) throw()
{
  if ( (inphotometric < PHOTO_BILEVEL) || (inphotometric > PHOTO_UNKNOWN) )
    return false;
  else
    Photometric = inphotometric;

  setBytesPerPixel();
  return true;
}

// ***************************************************************************
bool RawDataHandler::setBytesPerPixel() throw()
{
  switch (Photometric)
  {
  case PHOTO_GREY:
  {
    BytesPerPixel = 1;
    break;
  }
  case PHOTO_RGB:
  case PHOTO_RGB_BIL:
  case PHOTO_RGB_BSQ:
  {
    BytesPerPixel = 3;
    break;
  }
  default:
    BytesPerPixel = 3;
  }
  ByteWidth = BytesPerPixel * Width; // change the byte width of the image
  return true;
}

// **************************************************************************
bool RawDataHandler::validateRow(long int& row) const throw()
{
  if ( (row < 0) || (row >= Height) )
    return false;
  else
    return true;
}

// **************************************************************************
bool RawDataHandler::validateXY(long int& x, long int& y) const throw()
{
  if ( (x < 0) || (x >= Width) || (y < 0) || (y >= Height) )
    return false;
  else
    return true;
}


#endif // #ifndef RAWDATAHANDLER_CPP

