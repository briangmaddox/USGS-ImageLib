#ifndef _PALETTE_CPP_
#define _PALETTE_CPP_
// $Id: Palette.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

//  Implements member functions for the class Pallette 

#include "Palette.h"

using namespace USGSImageLib;

// ***************************************************************************
Palette::Palette() throw()
  : NoEntries(0)
{
}

// ***************************************************************************
Palette::Palette(int n) throw()
  : NoEntries(n)
{
}


// ***************************************************************************
Palette::~Palette()
{
}

#endif
