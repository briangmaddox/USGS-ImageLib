#ifndef _CACHEMANAGER_CPP_
#define _CACHEMANAGER_CPP_
// $Id: CacheManager.cpp,v 1.2 2003-06-28 16:28:40 bmaddox Exp $
// Brian Maddox - USGS MCMC SES
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:40 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "CacheManager.h"

using namespace USGSImageLib;

// ***************************************************************************
// Default constructor assumes 1 scanline
CacheManager::CacheManager(ImageIFile* inimage) throw(ImageException)
  : imageobj(inimage), cachesize(1), dosurround(false), valid(true)
{
  if (imageobj == 0) // user did something goofy
    throw ImageException(IMAGE_INVALID_OP_ERR);

  setupImageData();
  initScanlineCache();
  cacheScanlines(0);
}


// ***************************************************************************
CacheManager::CacheManager(ImageIFile* inimage, int numscanlines, 
                           bool surround)
  throw(ImageException)
  : imageobj(inimage), dosurround(surround), valid(true)
{
  if (imageobj == 0) // user did something goofy
    throw ImageException(IMAGE_INVALID_OP_ERR);

  if (surround)
    cachesize = 2 * numscanlines + 1;
  else
    cachesize = numscanlines;

  setupImageData();
  initScanlineCache();
  cacheScanlines(0);
}


// ***************************************************************************
CacheManager::~CacheManager()
{
  int loop;
  
  try
  {
    if (_cache)
    {
      for (loop = 0; loop < cachesize; loop++)
        delete [] _cache[loop];
      
      delete [] _cache;
    }

    return;
  }
  catch (...)
  {
    return;
  }
}


// ***************************************************************************
void CacheManager::resetCacheSize(int numscanlines, long int startat)
  throw (ImageException, std::bad_alloc)
{
  // Now delete the memory we already have in place
  for (int loop = 0; loop < cachesize; loop++)
    delete [] _cache[loop];

  delete [] _cache;

  // Oops!  Was previously done in the wrong spot above, my bad.
  // BGM 21 Feb 2001
  cachesize = numscanlines;

  // Now redo the memory allocation for the new size and reset the cache to
  // start at scanline 0
  initScanlineCache();
  cacheScanlines(startat);
}


// ***************************************************************************
void CacheManager::setupImageData() throw(ImageException)
{
  // go through and get our image information
  try
  {
    imageobj->getWidth(width);
    imageobj->getHeight(height);
    imageobj->getSamplesPerPixel(samplesperpixel);
    imageobj->getBitsPerSample(bitspersample);
    
    return;
  }
  catch (...)
  {
    valid = false;
    throw ImageException(IMAGE_INVALID_OP_ERR);
  }
}


// ***************************************************************************
void CacheManager::initScanlineCache() throw(ImageException, std::bad_alloc)
{
  try
  {
    // If the user passed in a cachesize that's greater than the size of the
    // image, go ahead and adjust the cachesize to equal the imagesize.
    // BGM 21 Feb 2001
    if (cachesize > height)
      cachesize = height;

    if (!(_cache = new(std::nothrow) unsigned char*[cachesize]))
      throw std::bad_alloc();
    
    // now go through and create the memory needed for this operation
    for (int loop = 0; loop < cachesize; loop++)
    {
      if (!(_cache[loop] = 
            new(std::nothrow) unsigned char[width * samplesperpixel]))
        throw std::bad_alloc();
    }
  }
  catch (...)
  {
    valid = false;
    throw; // rethrow to the user
  }
}


// ***************************************************************************
void CacheManager::cacheScanlines(long int rowstart) throw(ImageException)
{
  long int cacherow(rowstart);
  long int rowcounter;

  // First off, if it's at the top or bottom of the image, offset the bounds
  if (dosurround) // special handling if the cache surrounds the scanline
  {
    if (cacherow < ( (cachesize - 1) / 2))
    {
      cacherow = 0;
    }
    else if (cacherow >= (height - ( (cachesize - 1) / 2)))
    {
      cacherow = height - cachesize;
    }
    else
      cacherow = rowstart - ( (cachesize - 1) / 2);
  }
  else
  {
    if (cacherow <= (cachesize - 1)) // is too close to the beginning
      cacherow = 0;
    else if (cacherow >= (height - cachesize)) // is too close to the end
      cacherow = height - cachesize;
  }

  // Now tell the object which rows it has cached.
  lowerbound = cacherow;
  upperbound = cacherow + cachesize - 1;
  // Now go through and actually cache the scanlines 
  try
  {
    for (rowcounter = lowerbound; rowcounter <= upperbound; rowcounter++)
    {
      imageobj->getRawScanline(rowcounter, _cache[rowcounter - lowerbound]);
    }

    return; // everything should be ok here
  }
  catch(...)
  {
    valid = false;
    throw; // rethrow to the caller
  }
}
            

// ***************************************************************************
void CacheManager::getRawScanline(long int inscanline, unsigned char* inarray)
  throw(ImageException)
{
  try
  {
    // First of all, check to see if we need to recache our scanlines
    if ( (inscanline <= lowerbound) && (lowerbound != 0))
      cacheScanlines(inscanline);
    else
      if ((inscanline >= upperbound) && (upperbound != height-1))
        cacheScanlines(inscanline);

    // Now copy the scanline data into what the user passed to us
    memcpy(inarray, _cache[inscanline - lowerbound], 
           (sizeof(unsigned char) * width * samplesperpixel) );

    return;
  }
  catch (...)
  {
    valid = false;
    throw;
  }
}

// ***************************************************************************
const unsigned char* CacheManager::getRawScanline(long int inscanline)
  throw(ImageException)
{
  try
  {
    // First, check to see if we need to recache our scanlines
    if ( (inscanline <= lowerbound) && (lowerbound != 0))
      cacheScanlines(inscanline);
    else
      if ((inscanline >= upperbound) && (upperbound != height-1))
        cacheScanlines(inscanline);

    // now just return the pointer to the user
    return _cache[inscanline - lowerbound];
  }
  catch (...)
  {
    valid = false;
    throw;
  }
}
    
#endif
