#ifndef _IMAGEIFILE_CPP_
#define _IMAGEIFILE_CPP_
// $Id: ImageIFile.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

//  Implements member functions for the class ImageIFile 

#include "ImageIFile.h"

using namespace USGSImageLib;

// ***************************************************************************
ImageIFile::ImageIFile() throw(ImageException, std::bad_alloc)
  : ImageFile()
{
}


// ***************************************************************************
ImageIFile::ImageIFile(std::string& fn) throw(ImageException, std::bad_alloc)
  : ImageFile(fn)
{
}

// ***************************************************************************
ImageIFile::~ImageIFile()
{
}

#endif
