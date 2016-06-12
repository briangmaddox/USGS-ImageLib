#ifndef _RAWODATAHANDLER_CPP_
#define _RAWODATAHANDLER_CPP_
// $Id: RawODataHandler.cpp,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// 21 May 1998
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $
// Implementation for the output raw data base class

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "RawODataHandler.h"

using namespace USGSImageLib;

// ***************************************************************************
RawODataHandler::RawODataHandler(std::ofstream* outstream,
                                 long int outposition,
                                 long int outwidth, long int outheight,
                                 int photometric)
  throw(ImageException)
  : RawDataHandler(outwidth, outheight, photometric),
    dataposition(outposition),
    Stream(outstream)
{
}


// ***************************************************************************
RawODataHandler::~RawODataHandler()
{
}



#endif // #ifndef RAWODATAHANDLER_CPP
