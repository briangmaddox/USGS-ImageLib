#ifndef _GEOTIFFIMAGEOFILE_CPP_
#define _GEOTIFFIMAGEOFILE_CPP_
// $Id: GeoTIFFImageOFile.cpp,v 1.2 2003-06-28 16:28:40 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:40 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This is the implementation of the GeoTIFFImageOFile class

#include <sstream>
#include "GeoTIFFImageOFile.h"
#include "UTMN.h"
#include "SPCSMapper.h"
#include "xtiffio.h"

using namespace USGSImageLib;

// *************************************************************************
GeoTIFFImageOFile::GeoTIFFImageOFile(std::string& infilename, long int width,
                                     long int height, int photometric)
  : TIFFImageOFile(infilename, width, height, photometric, true), gtif(0)
{
  setImageType(IMAGE_GTIF);

  // Set the GTIF pointer
  try
  {
    if (!(tif = XTIFFOpen(infilename.c_str(), "w")))
      throw ImageException(IMAGE_FILE_OPEN_ERR);

    setupTIFF();

    if (!(gtif = GTIFNew(tif)))
      throw ImageException(IMAGE_FILE_OPEN_ERR);
  }
  catch (...)
  {
    setNoDataBit();
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }
}

// ************************************************************************
GeoTIFFImageOFile::~GeoTIFFImageOFile()
{
  try
  {
    GTIFWriteKeys(gtif);
    GTIFFree(gtif);
    if (hasPalette())
      TIFFSetField(tif, TIFFTAG_COLORMAP, red, green, blue);
    XTIFFClose(tif);
  }
  catch (...)
  {
  }
}


// *************************************************************************
void GeoTIFFImageOFile::setTiePoints(double* tp, int number)
  throw (ImageException)
{
  bool exceptflag = false;

  try
  {
    if (TIFFSetField(tif, TIFFTAG_GEOTIEPOINTS, number, tp) != 1)
    {
      setFailWBit();
      exceptflag = true;
    }
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }

  if (exceptflag)
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}


// *************************************************************************
void GeoTIFFImageOFile::setPixelScale(double* inscale)
  throw (ImageException)
{
  bool exceptflag = false;

  try
  {
   if (TIFFSetField(tif, TIFFTAG_GEOPIXELSCALE, 3, inscale) != 1)
   {
     exceptflag = true;
     setFailWBit();
   }
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }

  if (exceptflag)
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}

// *************************************************************************
void GeoTIFFImageOFile::setModelType(uint16 inmodeltype)
  throw (ImageException)
{
  setGeoKey(GTModelTypeGeoKey, inmodeltype);
}

  
// *************************************************************************
void GeoTIFFImageOFile::setRasterType(uint16 inrastertype)
  throw(ImageException)
{
  setGeoKey(GTRasterTypeGeoKey, inrastertype);
}


// **************************************************************************
void GeoTIFFImageOFile::setProjectedCSType(uint16 inpcstype)
  throw(ImageException)
{
  setGeoKey(ProjectedCSTypeGeoKey, inpcstype);
}


// *************************************************************************
void GeoTIFFImageOFile::setProjectedCSCitation(std::string incitation)
  throw(ImageException)
{
  setGeoKey(PCSCitationGeoKey, incitation);
}

// *************************************************************************
void GeoTIFFImageOFile::setVerticalCSType(uint16 inverttype)
  throw(ImageException)
{
  setGeoKey(VerticalCSTypeGeoKey, inverttype);
}

// ************************************************************************
void GeoTIFFImageOFile::setVerticalUnits(uint16 vertunits)
  throw(ImageException)
{
  setGeoKey(VerticalUnitsGeoKey, vertunits);
}


// *************************************************************************
void GeoTIFFImageOFile::setVerticalCitation(std::string incitation)
  throw(ImageException)
{
  setGeoKey(VerticalCitationGeoKey, incitation);
}


// *************************************************************************
void GeoTIFFImageOFile::setAngularUnits(uint16 inunits)
  throw(ImageException)
{
  setGeoKey(GeogAngularUnitsGeoKey, inunits);
}

// *************************************************************************
void GeoTIFFImageOFile::setLinearUnits(uint16 inunits)
  throw(ImageException)
{
  setGeoKey(ProjLinearUnitsGeoKey, inunits);
}


// *************************************************************************
void GeoTIFFImageOFile::setGeographicType(uint16 intype)
  throw(ImageException)
{
  setGeoKey(GeographicTypeGeoKey, intype);
}

// *************************************************************************
void GeoTIFFImageOFile::setGeodeticDatum(uint16 indatum)
  throw(ImageException)
{
  setGeoKey(GeogGeodeticDatumGeoKey, indatum);
}


// ************************************************************************
void GeoTIFFImageOFile::setUSGS_UTM(double x, double y,
                                    double xres, double yres,
                                    unsigned short utmzone, int nadtype)
  throw (ImageException)
{
  double tp[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double res[3] = { 0.0, 0.0, 0.0 };
  std::string cit; // to store the PCS Citation
  unsigned short pcstype;

  std::ostringstream os;  // output string stream to get the data

  // Set the tiepoints
  tp[3] = x;
  tp[4] = y;

  // set the Pixel Scale
  res[0] = xres;
  res[1] = yres;

  // NOTE THAT ONLY NORTHERN HEMISPHERE CASES ARE HANDLED

  // These switch statements handle the NAD encoding in the GeoTIFF. By
  // default, NAD27 is assumed.  To add support for another datum, add
  // another value to the switch statement
  switch(nadtype)
  {
  case 1927:   // NAD27
    pcstype = UTMN + utmzone;
    os << "UTM Zone " << utmzone << " N with NAD27" << std::ends;
    break;
  case GCS_NAD27: // NAD27
    pcstype = UTMN + utmzone;
    os << "UTM Zone " << utmzone << " N with NAD27" << std::ends;
    break;
  case 1983:   // NAD83
    pcstype = UTMN83 + utmzone;
    os << "UTM Zone " << utmzone << " N with NAD83" << std::ends;
    break;
  case GCS_NAD83: // NAD83
    pcstype = UTMN83 + utmzone;
    os << "UTM Zone " << utmzone << " N with NAD83" << std::ends;
    break;
  case GCS_WGS_84:
    pcstype = WGS84 + utmzone;
    os << "UTM Zone " << utmzone << " N with WGS84" << std::ends;
    break;
  default:       // throw an exception if it's not here
    throw ImageException(IMAGE_INVALID_OP_ERR);
  }
	
 
  try
  {
    cit = os.str(); // get the data and put it into the string
    //    os.freeze(0);   // so the stream will delete its own memory
    setTiePoints(tp, 6);
    setPixelScale(res);
    setGeoKey(GTModelTypeGeoKey, (unsigned short)ModelProjected);
    setGeoKey(GTRasterTypeGeoKey, (unsigned short)RasterPixelIsArea);
    setGeoKey(ProjectedCSTypeGeoKey, pcstype);
    setGeoKey(PCSCitationGeoKey, cit);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


void GeoTIFFImageOFile::setUSGS_SPCS(double x, double y,
                                     double xres, double yres,
                                     unsigned short spcszone, int nadtype)
  throw (ImageException)
{
  double tp[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  double res[3] = { 0.0, 0.0, 0.0 };
  SPCSMapper spcsmapper;
  unsigned short spcstype;  // to hold the GeoTIFF PCSType

  switch(nadtype)
  {
  case 1927:
    spcsmapper.initNAD27Mappings();
    break;
  case 1983:
    spcsmapper.initNAD83Mappings();
    break;
  default: // If it's not here, throw an exception and run
    throw ImageException(IMAGE_INVALID_OP_ERR);
  }

  tp[3] = x;
  tp[4] = y;
  res[0] = xres;
  res[1] = yres;

  spcsmapper.getGeoTIFFCode(spcszone, spcstype);
  if (spcstype == 0) // invalid spcs zone
    throw ImageException(IMAGE_INVALID_OP_ERR);

  try
  {
    setTiePoints(tp, 6);
    setPixelScale(res);
    setGeoKey(GTModelTypeGeoKey, (unsigned short)ModelProjected);
    setGeoKey(GTRasterTypeGeoKey, (unsigned short)RasterPixelIsArea);
    setGeoKey(ProjectedCSTypeGeoKey, spcstype);
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}

// *************************************************************************
void GeoTIFFImageOFile::setGeoKey(geokey_t key, unsigned short sval)
  throw(ImageException)
{
  bool exceptflag = false;

  try
  {
    if (!(GTIFKeySet(gtif, key, TYPE_SHORT, 1, sval)))
    {
      setFailWBit();
      exceptflag = true;
    }
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }

  if (exceptflag)
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}

// *************************************************************************
void GeoTIFFImageOFile::setGeoKey(geokey_t key, double dval)
  throw (ImageException)
{
  bool exceptflag = false;

  try
  {
    if (!(GTIFKeySet(gtif, key, TYPE_DOUBLE, 1, dval)))
    {
      setFailWBit();
      exceptflag = true;
    }
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }

  if (exceptflag)
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}

// ***********************************************************************
void GeoTIFFImageOFile::setGeoKey(geokey_t key, char* cval)
  throw (ImageException)
{
  bool exceptflag = false;

  try
  {
    if (!(GTIFKeySet(gtif, key, TYPE_ASCII, 0, cval)))
    {
      setFailWBit();
      exceptflag = true;
    }
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }

  if (exceptflag)
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}

// ***********************************************************************
void GeoTIFFImageOFile::setGeoKey(geokey_t key, std::string& sval)
  throw (ImageException)
{
  bool exceptflag = false;

  try
  {
    if (!(GTIFKeySet(gtif, key, TYPE_ASCII, 0, sval.c_str())))
    {
      setFailWBit();
      exceptflag = true;
    }
  }
  catch (...)
  {
    setFailWBit();
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }

  if (exceptflag)
    throw ImageException(IMAGE_FILE_WRITE_ERR);
}

#endif
