#ifndef _GEOTIFFIMAGEOFILE_H_
#define _GEOTIFFIMAGEOFILE_H_
// $Id: GeoTIFFImageOFile.h,v 1.1.1.1 2002-02-21 00:18:58 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:58 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class handles the writing of GeoTIFF output.

#include <string>
#include "TIFFImageOFile.h"
#include "geotiff.h"
#include "geotiffio.h"

namespace USGSImageLib
{
class GeoTIFFImageOFile : public TIFFImageOFile
{
 public:
  // Use the constants from tiff.h for the photometric here
  GeoTIFFImageOFile(std::string& infilename, long int width, long int height,
                    int photometric);
  virtual ~GeoTIFFImageOFile();

  // Specific set functions
  void setTiePoints(double* tp, int number)           throw (ImageException);
  void setPixelScale(double* inscale)                 throw (ImageException);
  void setModelType(uint16 inmodeltype)               throw (ImageException);
  void setRasterType(uint16 inrastertype)             throw (ImageException);
  void setProjectedCSType(uint16 inpcstype)           throw (ImageException);
  void setProjectedCSCitation(std::string incitaion)  throw (ImageException);
  void setVerticalCSType(uint16 inverttype)           throw (ImageException);
  void setVerticalUnits(uint16 vertunits)             throw (ImageException);
  void setVerticalCitation(std::string incitation)    throw (ImageException);
  void setAngularUnits(uint16 inunits)                throw (ImageException);
  void setLinearUnits(uint16 inunits)                 throw (ImageException);
  void setGeographicType(uint16 intype)               throw (ImageException);
  void setGeodeticDatum(uint16 indatum)               throw (ImageException);
  
  // Pass in a USGS UTM zone number
  void setUSGS_UTM(double x, double y, double xres, double yres,
                   unsigned short utmzone, int nadtype) throw (ImageException);
  // Pass in a standard USGS SPCS zone number
  void setUSGS_SPCS(double x, double y, double xres, double yres,
                    unsigned short spcszone, int nadtype)
    throw (ImageException);
  
  // Generic set functions
  void setGeoKey(geokey_t key, unsigned short sval) throw (ImageException);
  void setGeoKey(geokey_t key, double dval)         throw (ImageException);
  void setGeoKey(geokey_t key, char* cval)          throw (ImageException);
  void setGeoKey(geokey_t key, std::string& sval)   throw (ImageException);

 protected:
  GTIF* gtif;
};

} // namespace
#endif
