#ifndef _LRUCACHMANAGER_H
#define _LRUCACHMANAGER_H
// $Id: LRUCacheManager.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Christopher Bilderback - USGS MCMC SES
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "CacheManager.h"
#include "LRUNode.h"
#include <map>

// This class works as a extension to the cachemanger interface
// that provides a LRU cache interface. 

namespace USGSImageLib
{

#define MAX_CLOCK 1000000  //the period of the "clock"


class LRUCacheManager : public CacheManager
{
 public:
  
  /**
   *Main constructor for the class starts
   *a new cache out of image file and defaults the
   *cache size to 1 line
   **/
  LRUCacheManager(ImageIFile* inimage) throw(ImageException);

  /**
   *Secondary constructor allows the size of the cache to be 
   *set.
   **/
  LRUCacheManager(ImageIFile* inimage, int numscanlines) throw(ImageException);

  /**
   *Main destructor
   **/
  virtual ~LRUCacheManager();

  // Allow the user to reset the size of the scanline cache.  Note: This will
  // delete all currently held scanlines and restart the cache at
  // scanline 0 unless the user passes in the second argument
  virtual void resetCacheSize(int numscanlines, long int startat = 0) 
    throw(ImageException, std::bad_alloc);
  

  // Get the user specified scanline.  If the scanline is not already in the
  // cache, recache then return to the user.
  virtual void getRawScanline(long int inscanline, unsigned char* inarray) 
    throw(ImageException);

  // This version allows the user to get a pointer to the actual scanline
  // cache internal to the object.
  virtual const unsigned char* getRawScanline(long int inscanline) 
    throw(ImageException);


protected:
  //This function does the LRU op to load a new
  //scanline
  const unsigned char * doLRUop(long int inscanline) throw(ImageException);
  
  //This function loads a scanline from disk puts it in the
  //the hash table (giving it the current clock value) and
  //returns it.
  const unsigned char * load(long int inscanline) throw(ImageException);


  std::map<long int, LRUNode*> hcache;  //the LRU cache
  long int clock;                       //the pesudo clock
};

}//namespace

#endif
