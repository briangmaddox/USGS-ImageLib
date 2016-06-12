#ifndef _LRUNODE_H_
#define _LRUNODE_H_
// $Id: LRUNode.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Christopher Bilderback - USGS MCMC SES
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class represents a node in the LRUCache. It has a pointer for the
// scanline and a timestamp value used for determining the LRU

namespace USGSImageLib
{

class LRUNode
{

public:
  /**
   *This is the main constructor for the LRUNode
   *class.
   **/
  LRUNode();

  /**
   *Secondary constructor for the class allows the
   *node data to be passed in.
   **/
  LRUNode(unsigned char * inscanline, long int inclock);

  /**
   *Destructor deletes the scanline memory that is 
   *passed into the class.
   **/
  virtual ~LRUNode();

  /**
   *This function returns a const pointer to the scanline
   **/
  unsigned char * getScanline() const throw();

  /**
   *This function returns the clock for the scanline
   **/
  int getClock() const throw();

  /**
   *This function sets the scanline for the node
   **/
  void setScanline(unsigned char * inscanline) throw();
  
  /**
   *This function sets the clock for the node
   **/
  void setClock(long int inclock) throw();


protected:
  unsigned char * scanline; //the scanline
  long int clock;           //the LRU value
};

//inline functions
//*****************************************************
inline  unsigned char * LRUNode::getScanline() const throw()
{
  return scanline;
}

//*************************************************************
inline int LRUNode::getClock() const throw()
{
  return clock;
}

//************************************************************
inline void LRUNode::setScanline(unsigned char * inscanline) throw()
{
  scanline = inscanline;
}
  
//************************************************************
inline void LRUNode::setClock(long int inclock) throw()
{
  clock = inclock;
}

}//namespace

#endif





