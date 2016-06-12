#ifndef _GEOTIFFIMAGEIFILE_H_
#define _GEOTIFFIMAGEIFILE_H_
// $Id: GeoTIFFImageIFile.h,v 1.1.1.1 2002-02-21 00:18:58 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:58 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Defines the GeoTIFF input handle class

#include "TIFFImageIFile.h"
#include "geotiff.h"
#include "geotiffio.h"

namespace USGSImageLib
{
class GeoTIFFImageIFile : public TIFFImageIFile
{
 public:
  GeoTIFFImageIFile(std::string& tfilename, bool usemmap = true);
  virtual ~GeoTIFFImageIFile();

  bool getTiePoints(double* tp, unsigned short& number) throw(ImageException);
  bool getPixelScale(double* scale)                     throw(ImageException);
  bool getModelType(uint16& inmodeltype)                throw(ImageException);
  bool getRasterType(uint16& inrastertype)              throw(ImageException);
  bool getProjectedCSType(uint16& inpcstype)            throw(ImageException);
  bool getProjectedCSCitation(std::string& incitation)  throw(ImageException);
  bool getVerticalCSType(uint16& intype)                throw(ImageException);
  bool getVerticalCitation(std::string& incitation)     throw(ImageException);
  bool getAngularUnits(uint16& inunits)                 throw(ImageException);
  bool getLinearUnits(uint16& inunits)                  throw(ImageException);
  bool getVerticalUnits(uint16& inunits)                throw(ImageException);
  bool getGeographicType(uint16& intype)                throw(ImageException);
  bool getProjection(int& inprojection)                 throw(ImageException);
  bool getDatum(int& indatum)                           throw(ImageException);
  bool getZone(unsigned short& zone)                    throw(ImageException);

  // Generic Key Functions
  bool getGeoKeyInfo(geokey_t key, int& size, tagtype_t& type)
    throw(ImageException);

  bool getGeoKey(geokey_t key, uint16& val)         throw(ImageException);
  bool getGeoKey(geokey_t key, double& val)         throw(ImageException);
  bool getGeoKey(geokey_t key, std::string& val)
    throw(ImageException, std::bad_alloc);
  bool getKeyName(geokey_t key, std::string& name)  throw(ImageException);
  bool getValueName(geokey_t key, int value, std::string& name)
    throw(ImageException);

protected:
  bool getPCSProjection(int& projection)             throw(ImageException);
  bool getGeographicDatum(int pcscode, int& indatum) throw(ImageException);
  bool getPCSDatum(int pcscode, int& indatum)        throw(ImageException);
  
  GTIF* gtif;
};

} // namespace

#endif
