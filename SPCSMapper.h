#ifndef _SPCSMAPPER_H_
#define _SPCSMAPPER_H_
// $Id: SPCSMapper.h,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// The sole purpose of this class is to handle the mappings from GeoTIFF
// tags to USGS State Plane Zones.  It may also contain some helper functions
// related to SPCS

#include "ImageProjection.h"

namespace USGSImageLib
{

class SPCSMapper  
{
public:
  SPCSMapper();
  virtual ~SPCSMapper();

  void initNAD27Mappings() throw();
  void initNAD83Mappings() throw();
  // convert GeoTIFF->USGS
  bool getUSGSCode(int ingeotiffcode, unsigned short& usgscode)    throw();
  // convert USGS->GeoTIFF
  bool getGeoTIFFCode(int inspcscode, unsigned short& geotiffcode) throw();

private:
  bool initialized; // have we called one of the init functions
  unsigned short mappings[6000];
};

} // namespace

#endif
