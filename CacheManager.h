#ifndef _CACHEMANAGER_H_
#define _CACHEMANAGER_H_
// $Id: CacheManager.h,v 1.1.1.1 2002-02-21 00:18:55 bmaddox Exp $
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:55 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "ImageIFile.h"
#include <vector>

// This class provides a scanline caching mechanism for the caller.  It works
// by keeping an internal ImageIFile pointer to resolve at runtime.

namespace USGSImageLib
{

class CacheManager
{
 public:
  // Default constructor.  Defaults to caching 1 scanline
  CacheManager(ImageIFile* inimage) throw(ImageException);
  // Set the number to cache and also whether or not you want to surround the
  // input scanline or just cache the scanlines following it.  If surround is
  // true, then the number passed in is the number above and below the scanline
  // to cache if the scanline isn't already in the cache.
  CacheManager(ImageIFile* inimage, int numscanlines, bool surround = false)
    throw(ImageException);
  virtual ~CacheManager();

  // Allow the user to reset the size of the scanline cache.  Note: This will
  // delete all currently held scanlines and restart the cache at
  // scanline 0 unless the user passes in the second argument
  virtual void resetCacheSize(int numscanlines, long int startat = 0) 
    throw(ImageException, std::bad_alloc);

  // Allow the user to get the bounds of the scanline cache
  void getLowestScanlineNumber(long int& innumber)    const throw();
  void getHighestScanlineNumber(long int& innumber)   const throw();
  void getNumberScanlines(int& inscanlines)           const throw();

  // Allow the user to query if the cache is in a valid state or not
  bool good() const throw();

  // Get the user specified scanline.  If the scanline is not already in the
  // cache, recache then return to the user.
  virtual void getRawScanline(long int inscanline, unsigned char* inarray) 
    throw(ImageException);

  // This version allows the user to get a pointer to the actual scanline
  // cache internal to the object.
  virtual const unsigned char* getRawScanline(long int inscanline) 
    throw(ImageException);

 protected:
  void cacheScanlines(long int rowstart) throw(ImageException);
  // Initialize the scanline cache and allocate memory for it
  void initScanlineCache() throw(ImageException, std::bad_alloc);
  void setupImageData()    throw(ImageException);

  unsigned char** _cache;
  ImageIFile*     imageobj;    // pointer to the image object
  int             cachesize;   // number of scanlines to cache
  long int        lowerbound;  // lowest and highest scanlines being cached
  long int        upperbound;
  bool            dosurround;
  bool            valid;        // same as with image, is the object valid?

  // Information about the Image object itself
  long int  height;
  long int  width;
  int       samplesperpixel;
  int       bitspersample;
};

// ***************************************************************************
inline void CacheManager::getNumberScanlines(int& inscanlines) const throw()
{
  inscanlines = cachesize;
  return;
}

// ***************************************************************************
inline
void CacheManager::getLowestScanlineNumber(long int& innumber) const throw()
{
  innumber = lowerbound;
  return;
}

// ***************************************************************************
inline
void CacheManager::getHighestScanlineNumber(long int& innumber) const throw()
{
  innumber = upperbound;
  return;
}

// ***************************************************************************
inline bool CacheManager::good() const throw()
{
  return valid;
}

} // namespace

#endif
