#ifndef _IMAGEOFILE_CPP_
#define _IMAGE_FILE_CPP_
// $Id: ImageOFile.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "ImageOFile.h"

using namespace USGSImageLib;

// ***************************************************************************
ImageOFile::ImageOFile()
  throw (ImageException, std::bad_alloc)
  : ImageFile()
{
}

// ***************************************************************************
ImageOFile::ImageOFile(std::string& infile, long int width, long height)
  throw (ImageException, std::bad_alloc)
  : ImageFile (infile, width, height)
{
}

// ***************************************************************************
ImageOFile::~ImageOFile()
{
}

#endif
