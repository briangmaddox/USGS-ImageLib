#ifndef _IMAGE_EXCEPTION_CPP_
#define _IMAGE_EXCEPTION_CPP_
// $Id: ImageException.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES -  bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Defination of the ImageException class.

#include "ImageException.h"

using namespace USGSImageLib;

// ***************************************************************************
ImageException::ImageException() throw()
  : myexception(IMAGE_UNKNOWN_ERR)
{
}

// **************************************************************************
ImageException::~ImageException()
{
}

// **************************************************************************
ImageException::ImageException(short int inexception) throw()
  : myexception(inexception)
{
}

// **************************************************************************
bool ImageException::setException(short int inexception) throw()
{
  myexception = inexception;
  return true;
}

// **************************************************************************
void ImageException::getException(short int& inexception) const throw()
{
  inexception = myexception;
}

// **************************************************************************
void ImageException::getString(std::string& instring) const throw()
{
  switch (myexception)
  {
  case IMAGE_MEM_ALLOC_ERR:
    instring = std::string("Image: Memory allocation error");
    break;
  case IMAGE_FILE_OPEN_ERR:
    instring = std::string("Image: File open error");
    break;
  case IMAGE_FILE_CLOSE_ERR:
    instring = std::string("Image: File close error");
    break;
  case IMAGE_FILE_READ_ERR:
    instring = std::string("Image: File read error");
    break;
  case IMAGE_FILE_WRITE_ERR:
    instring = std::string("Image: File write error");
    break;
  case IMAGE_BOUNDING_ERR:
    instring = std::string("Image: Bounding error");
    break;
  case IMAGE_INVALID_OP_ERR:
    instring = std::string("Image: Invalid operation error");
    break;
  case IMAGE_INVALID_TYPE_ERR:
    instring = std::string("Image: Invalid type error");
    break;
  default:
    instring = std::string("IMAGE_UNKNOWN_ERR");
  }
}

#endif
