#ifndef _LRUCACHEMANAGER_CPP_
#define _LRUCACHEMANAGER_CPP_
// $Id: LRUCacheManager.cpp,v 1.2 2003-06-28 16:28:40 bmaddox Exp $
// Christopher Bilderback - USGS MCMC SES
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:40 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "LRUCacheManager.h"

using namespace USGSImageLib;

//*************************************************************************
LRUCacheManager::LRUCacheManager(ImageIFile* inimage)  throw(ImageException)  
  : CacheManager(inimage), clock(0)
{
  int counter = 0;
  
  for (counter = 0; counter < cachesize; counter++)
  {
    delete [] _cache[counter];
  }
  delete [] _cache;
  _cache = 0;

  imageobj = inimage;   //store the image
  cachesize = 1;
  lowerbound = 0;
  upperbound = 0;  
  //get the image data
  setupImageData();
  valid = true;  //we are good to go
};

  

//***************************************************************************
LRUCacheManager::LRUCacheManager(ImageIFile* inimage, int numscanlines)   
  throw(ImageException) 
  : CacheManager(inimage) 

{
  int counter = 0;
  
  for (counter = 0; counter < cachesize; counter++)
  {
     delete [] _cache[counter];
  }
  delete [] _cache;
  _cache = 0;

  imageobj = inimage;   //store the image
  cachesize = numscanlines;
  lowerbound = 0;
  upperbound = 0;

  //get the image data
  setupImageData();
  valid = true;  //we are good to go
}


//********************************************************************
LRUCacheManager::~LRUCacheManager()
{
  LRUNode* temp = 0;
  std::map<long int, LRUNode*>::iterator it;

  for(it = hcache.begin(); it != hcache.end(); it++)
  {
    temp = (*it).second;
    delete temp;
  }
  //hcache's destructor will take care of rest
}



//******************************************************************
void LRUCacheManager::resetCacheSize(int numscanlines, long int startat) 
  throw(ImageException, std::bad_alloc)
{
  cachesize = numscanlines; //set the cache size
}
  

//**************************************************************************
void LRUCacheManager::getRawScanline(long int inscanline, 
                                     unsigned char* inarray) 
  throw(ImageException)
{
  const unsigned char* temp = 0;
  int counter = 0;
  
  //get the scanline 
  temp = getRawScanline(inscanline);
  
  for (; counter < width*samplesperpixel*bitspersample; counter++)
  {
    inarray[counter] = temp[counter];
  }

}
 
//***************************************************************************
const unsigned char* LRUCacheManager::getRawScanline(long int inscanline) 
  throw(ImageException)
{
  std::map<long int, LRUNode*>::iterator it;
  const unsigned char * scanline;

  try
  {
    //first see if it is in the cache
    it = hcache.find(inscanline);
    
    if (it != hcache.end())
    {
      scanline = (*it).second->getScanline();
      (*it).second->setClock(clock);
    }
    else
    {
      //not in the cache
      if (static_cast<int>(hcache.size()) > cachesize)
      {
        scanline = doLRUop(inscanline);
      }
      else
        scanline = load(inscanline);
    }

    if (clock > MAX_CLOCK)
    {
      clock = 1;
      //reset the clocks
      for (it = hcache.begin(); it != hcache.end(); it++)
      {
        (*it).second->setClock(0);
      }
    }
    else
      ++clock;
    
    return scanline;
  }
  catch(ImageException& e)
  {
    throw e;
  }
  catch(...)
  {
    throw ImageException(IMAGE_INVALID_OP_ERR);
  }
}


//*************************************************************************
const unsigned char* LRUCacheManager::doLRUop(long int inscanline) 
  throw(ImageException)
{
  LRUNode* temp = 0;
  long int max = 0;
  //  unsigned char* scanline(0);
  std::map<long int, LRUNode*>::iterator it, store;

  try
  {
    store = hcache.end();
    for (it = hcache.begin(); it != hcache.end(); it++)
    {
      if ((clock - (*it).second->getClock()) > max)
      {
        store = it;
        max = clock - (*it).second->getClock();
        // break out of loop here?
      }
    }

    //we found something
    if (store != hcache.end())
    {
      temp = (*store).second;
      hcache.erase(store);
      //read a new line (reuse memory)
      imageobj->getRawScanline(inscanline, temp->getScanline());
      //put it in the cache
      hcache[inscanline] = temp;
      return temp->getScanline();
    }
    else
    {
      //handle case were nothing was found
      temp = (*hcache.begin()).second;
      hcache.erase(hcache.begin());
      //read a new line (reuse memory)
      imageobj->getRawScanline(inscanline, temp->getScanline());
      //put it in the cache
      hcache[inscanline] = temp;
      return temp->getScanline();
    }
  }
  catch(ImageException & e)
  {
    throw e;
  }
  catch(...)
  {
    throw ImageException(IMAGE_INVALID_OP_ERR);
  }
}

//***********************************************************************  
const unsigned char* LRUCacheManager::load(long int inscanline) 
  throw(ImageException)
{
  LRUNode* temp = 0;
  unsigned char * scanline = 0;
  
  try
  {
    //create the scanline
    if (!(scanline = new (std::nothrow) unsigned char [width * samplesperpixel*
                                                       bitspersample]))
      throw std::bad_alloc();
    
    //read the scanline
    imageobj->getRawScanline(inscanline, scanline);
    
    //create the LRUNode
    if (!(temp = new (std::nothrow) LRUNode(scanline, clock)))
      throw std::bad_alloc();
    
    //put it in the cache
    hcache[inscanline] = temp;

    //return the scanline
    return scanline;
  }
  catch(ImageException & e)
  {
    throw e;
  }
  catch(...)
  {
    delete scanline;
    delete temp;
    //re throw
    throw ImageException(IMAGE_INVALID_OP_ERR);
  }
    
}

#endif
