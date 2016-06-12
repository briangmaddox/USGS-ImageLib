#ifndef _LRUNODE_CPP_
#define _LRUNDOE_CPP_
// $Id: LRUNode.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Christopher Bilderback - USGS MCMC SES
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "LRUNode.h"

using namespace USGSImageLib;

//**********************************************************
LRUNode::LRUNode() : scanline(0), clock(0)
{
}

//********************************************************
LRUNode::LRUNode(unsigned char * inscanline, long int inclock) :
  scanline(inscanline), clock(inclock) 
{
}

//*********************************************************
LRUNode::~LRUNode()
{
  //delete the scanline
  delete scanline;
}

#endif
