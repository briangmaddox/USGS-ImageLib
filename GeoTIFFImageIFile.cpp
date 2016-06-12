#ifndef _GEOTIFFIMAGEIFILE_CPP_
#define _GEOTIFFIMAGEIFILE_CPP_
// $Id: GeoTIFFImageIFile.cpp,v 1.3 2003-10-06 02:56:03 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-10-06 02:56:03 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "GeoTIFFImageIFile.h"
#include "ImageProjection.h"
#include "SPCSMapper.h"
#include "xtiffio.h"

using namespace USGSImageLib;

// **************************************************************************
GeoTIFFImageIFile::GeoTIFFImageIFile(std::string& tfilename, bool usemmap)
  : TIFFImageIFile(tfilename, usemmap, true)
{
  setImageType(IMAGE_GTIF);
  char openflags[3];

  try
  {
    if (usemmap)
      strncpy(openflags, "r", 2);
    else
      strncpy(openflags, "rm", 3);

    if (!(tif = XTIFFOpen(tfilename.c_str(), openflags)))
      throw ImageException(IMAGE_FILE_OPEN_ERR);

    setupTIFF();

    if (!(gtif = GTIFNew(tif)))
      setFailRBit();
  }
  catch (...)
  {
    setFailRBit();
  }
}


// *************************************************************************
GeoTIFFImageIFile::~GeoTIFFImageIFile()
{
  try
  {
    XTIFFClose(tif);
    GTIFFree(gtif);
  }
  catch(...)
  {
  }
}


// **************************************************************************
bool GeoTIFFImageIFile::getTiePoints(double* tp, unsigned short& number)
  throw (ImageException)
{
  // libtiff will return the array into this and dynamically allocate mem for
  // it.  It will also dynamically deallocate mem so leave it alone
  double* temppts;

  try
  {
    if (TIFFGetField(tif, TIFFTAG_GEOTIEPOINTS, &number, &temppts) == 0)
      return false;
    for (int tcount = 0; tcount < number; tcount++)
      tp[tcount] = temppts[tcount];

    return true;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// *************************************************************************
bool GeoTIFFImageIFile::getPixelScale(double* scale)
  throw (ImageException)
{
  double* temppts;
  unsigned short tnumber = 3;

  try
  {
    if (TIFFGetField(tif, TIFFTAG_GEOPIXELSCALE, &tnumber, &temppts) == 0)
      return false;

    for (int tcount = 0; tcount < tnumber; tcount++)
      scale[tcount] = temppts[tcount];

    return true;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
bool GeoTIFFImageIFile::getModelType(uint16& inmodeltype)
  throw (ImageException)
{
  return getGeoKey(GTModelTypeGeoKey, inmodeltype);
}


// **************************************************************************
bool GeoTIFFImageIFile::getRasterType(uint16& inrastertype)
  throw (ImageException)
{
  return getGeoKey(GTRasterTypeGeoKey, inrastertype);
}


// **************************************************************************
bool GeoTIFFImageIFile::getProjectedCSType(uint16& inpcstype)
  throw (ImageException)
{
  return getGeoKey(ProjectedCSTypeGeoKey, inpcstype);
}


// **************************************************************************
bool GeoTIFFImageIFile::getProjectedCSCitation(std::string& incitation)
  throw(ImageException)
{
  return getGeoKey(PCSCitationGeoKey, incitation);
}


// **************************************************************************
bool GeoTIFFImageIFile::getVerticalCSType(uint16& intype)
  throw (ImageException)
{
  return getGeoKey(VerticalCSTypeGeoKey, intype);
}

// **************************************************************************
bool GeoTIFFImageIFile::getVerticalCitation(std::string& incitation)
  throw (ImageException)
{
  return getGeoKey(VerticalCitationGeoKey, incitation);
}


// **************************************************************************
bool GeoTIFFImageIFile::getAngularUnits(uint16& inunits)
  throw (ImageException)
{
  return getGeoKey(GeogAngularUnitsGeoKey, inunits);
}


// **************************************************************************
bool GeoTIFFImageIFile::getLinearUnits(uint16& inunits)
  throw (ImageException)
{
  return getGeoKey(ProjLinearUnitsGeoKey, inunits);
}


// **************************************************************************
bool GeoTIFFImageIFile::getVerticalUnits(uint16& inunits)
  throw (ImageException)
{
  return getGeoKey(VerticalUnitsGeoKey, inunits);
}

// **************************************************************************
bool GeoTIFFImageIFile::getGeographicType(uint16& intype)
  throw (ImageException)
{
  return getGeoKey(GeographicTypeGeoKey, intype);
}

// **************************************************************************
bool GeoTIFFImageIFile::getProjection(int& inprojection)
  throw(ImageException)
{
  uint16 modeltype; // to hold the model type of the image
  try
  {
    if (!getModelType(modeltype))
    {
      inprojection = UNDEFINEDPROJSYS;
      return false;
    }

    // Now that we got the model type, see if it's projected or geographic and
    // act accordingly.
    if (modeltype == ModelTypeProjected)
    {
      return getPCSProjection(inprojection);
    }
    else if (modeltype == ModelTypeGeographic)
    {
      inprojection = GEOGRAPHIC;
      return true;
    }
    else
    {
      inprojection = UNDEFINEDPROJSYS;
      return false;
    }
  }
  catch (...)
  {
    inprojection = UNDEFINEDPROJSYS;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
bool GeoTIFFImageIFile::getDatum(int& indatum)
  throw(ImageException)
{
  int tprojsys;

  // First we need to figure out if it's a projected or geographic system
  if (!getProjection(tprojsys))
  {
    indatum = UNKNOWNDATUM;
    return false;
  }

  // Now that we have the projection type, act accordingly to get the datum
  if (tprojsys == GEOGRAPHIC)
    return getGeographicDatum(tprojsys, indatum);
  else // handle for Projected Models
    return getPCSDatum(tprojsys, indatum);
}

// **************************************************************************
bool GeoTIFFImageIFile::getZone(unsigned short& zone)
  throw(ImageException)
{
  uint16 pcstype;
  SPCSMapper spcsmapper;
  try
  {
    if (!getProjectedCSType(pcstype))
      return false;

    if ( (pcstype >= 32701) && (pcstype <= 32760) )
      zone = -1 * (pcstype % 100);
    else if ( (pcstype >= 32501) && (pcstype <= 32560) )
      zone = -1 * (pcstype % 100);
    else if ( (pcstype >= 32301) && (pcstype <= 32360) )
      zone = -1 * (pcstype % 100);
    else if ( (pcstype >= 32100) && (pcstype <= 32161) )
    {
      spcsmapper.initNAD83Mappings();
      spcsmapper.getUSGSCode(pcstype, zone);
    }
    else if ( (pcstype >= 32001) && (pcstype <= 32060) )
    {
      spcsmapper.initNAD27Mappings();
      spcsmapper.getUSGSCode(pcstype, zone);
    }
    else if ( (pcstype >= 26929) && (pcstype <= 26998) )
    {
      spcsmapper.initNAD83Mappings();
      spcsmapper.getUSGSCode(pcstype, zone);
    }
    else if ( (pcstype >= 26729) && (pcstype <= 26798) )
    {
      spcsmapper.initNAD27Mappings();
      spcsmapper.getUSGSCode(pcstype, zone);
    }
    else if ( (pcstype >= 24818) && (pcstype <= 24821) )
      zone = 60 - (pcstype % 100);
    else
      zone = pcstype % 100;

    return true;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// **************************************************************************
bool GeoTIFFImageIFile::getGeoKeyInfo(geokey_t key, int& size, 
                                      tagtype_t& type)
  throw(ImageException)
{
  try
  {
    if (!GTIFKeyInfo(gtif, key, &size, &type))
      return false;
    else
      return true;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
bool GeoTIFFImageIFile::getGeoKey(geokey_t key, uint16& val)
  throw (ImageException)
{
  try
  {
    if (!GTIFKeyGet(gtif, key, &val, 0, 1))
      return false;

    return true;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
bool GeoTIFFImageIFile::getGeoKey(geokey_t key, std::string& val)
  throw (ImageException, std::bad_alloc)
{
  char* tempptr = 0;
  int stringlength = 0;

  // Dummy variables, just used so the call succeeds.
  int tsize;
  tagtype_t ttype;
  
  try
  {
    // Bug fixed 9 Dec 2002 - Originally goofed and passed in PCSCitation
    // GeoKey instead of key.  Found by Larry Moore. BGM
    stringlength = GTIFKeyInfo(gtif, key, &tsize, &ttype);
    if (stringlength > 0)
    {
      if (!(tempptr = new(std::nothrow) char[stringlength]))
        throw std::bad_alloc();
      if (!GTIFKeyGet(gtif, key, tempptr, 0, stringlength))
      {
        delete [] tempptr;
        val = std::string("");
        return false;
      }
      val = std::string(tempptr);
      delete [] tempptr;
      return true;
    }
    else
    {
      val = std::string("");
      return false;
    }
  }
  catch (...)
  {
    delete [] tempptr;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// **************************************************************************
bool GeoTIFFImageIFile::getGeoKey(geokey_t key, double& val)
  throw (ImageException)
{
  try
  {
    if (!GTIFKeyGet(gtif, key, &val, 0, 1))
      return false;

    return true;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
bool GeoTIFFImageIFile::getKeyName(geokey_t key, std::string& name)
  throw (ImageException)
{
  char* tempchar = 0; // to hold the return from libgeotiff

  try
  {
    tempchar = GTIFKeyName(key);
    if (!tempchar)
    {
      name = std::string("");
      return false;
    }

    name = std::string(tempchar);
    delete [] tempchar;
    return true;
  }
  catch (...)
  {
    delete [] tempchar;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// **************************************************************************
bool GeoTIFFImageIFile::getValueName(geokey_t key, int value,
                                     std::string& name)
  throw (ImageException)
{
  char* tempchar = 0; // to hold the return from libgeotiff

  try
  {
    tempchar = GTIFValueName(key, value);
    if (!tempchar)
    {
      name = std::string("");
      return false;
    }

    name = std::string(tempchar);
    delete [] tempchar;
    return true;
  }
  catch (...)
  {
    delete [] tempchar;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// **************************************************************************
// This function also sets the zone and datum for this image object.  It's
// done here since it would be rather foolish to sit and go through another
// large set of if/then's
bool GeoTIFFImageIFile::getPCSProjection(int& inprojection)
  throw (ImageException)
{
  uint16 projcstype; // to hold the type

  try
  {
    if (!getProjectedCSType(projcstype))
    {
      inprojection = UNDEFINEDPROJSYS;
      return false;
    }

    // Now that we have the projected cs key, go ahead and return the zone

    if ( (projcstype >= 32701) && (projcstype <= 32760) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 32601) && (projcstype <= 32660) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 32501) && (projcstype <= 32560) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 32401) && (projcstype <= 32460) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 32301) && (projcstype <= 32360) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 32201) && (projcstype <= 32260) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 32100) && (projcstype <= 32161) )
    {
      inprojection = STATEPLANE;
      return true;
    }
    else if ( (projcstype >= 32001) && (projcstype <= 32060) )
    {
      inprojection = STATEPLANE;
      return true;
    }
    else if ( (projcstype >= 26929) && (projcstype <= 26998) )
    {
      inprojection = STATEPLANE;
      return true;
    }
    else if ( (projcstype >= 26903) && (projcstype <= 26923) )
    {
      inprojection = STATEPLANE;
      return true;
    }
    else if ( (projcstype >= 26729) && (projcstype <= 26798) )
    {
      inprojection = STATEPLANE;
      return true;
    }
    else if ( (projcstype >= 26703) && (projcstype <= 26722) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 24818) && (projcstype <= 24821) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 23947) && (projcstype <= 24048) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 23028) && (projcstype <= 23038) )
    {
      inprojection = UTM;
      return true;
    }
    else if ( (projcstype >= 20137) && (projcstype <= 20138) )
    {
      inprojection = UTM;
      return true;
    }
    else
    {
      inprojection = UNDEFINEDPROJSYS;
      return false;
    }
  }
  catch (...)
  {
    inprojection = UNDEFINEDPROJSYS;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}

// **************************************************************************
bool GeoTIFFImageIFile::getGeographicDatum(int pcscode, int& indatum)
  throw(ImageException)
{
  uint16 tvalue;
  try
  {
    if (GTIFKeyGet(gtif, GeographicTypeGeoKey, &tvalue, 0, 1))
    {
      indatum = tvalue;
      return true;
    }
    else if (GTIFKeyGet(gtif, GeogGeodeticDatumGeoKey, &tvalue, 0, 1) )
    {
      indatum = tvalue;
      return true;
    }
    else return false;
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_READ_ERR);

  }
}

// **************************************************************************
bool GeoTIFFImageIFile::getPCSDatum(int pcscode, int& indatum)
  throw(ImageException)
{
  uint16 tvalue;
  try
  {
    if (!getProjectedCSType(tvalue))
    {
      indatum = UNKNOWNDATUM;
      return false;
    }
    
    // Now that we have the projected cs key, go ahead and return the zone

    if ( (tvalue >= 32701) && (tvalue <= 32760) )
    {
      indatum = Datum_WGS84;
      return true;
    }
    else if ( (tvalue >= 32601) && (tvalue <= 32660) )
    {
      indatum = Datum_WGS84;
      return true;
    }
    else if ( (tvalue >= 32501) && (tvalue <= 32560) )
    {
      indatum = Datum_WGS72;
      return true;
    }
    else if ( (tvalue >= 32401) && (tvalue <= 32460) )
    {
      indatum = Datum_WGS72;
      return true;
    }
    else if ( (tvalue >= 32301) && (tvalue <= 32360) )
    {
      indatum = Datum_WGS72;
      return true;
    }
    else if ( (tvalue >= 32201) && (tvalue <= 32260) )
    {
      indatum = Datum_WGS72;
      return true;
    }
    else if ( (tvalue >= 32100) && (tvalue <= 32161) )
    {
      indatum = Datum_North_American_Datum_1983;
      return true;
    }
    else if ( (tvalue >= 32001) && (tvalue <= 32060) )
    {
      indatum = Datum_North_American_Datum_1927;
      return true;
    }
    else if ( (tvalue >= 26929) && (tvalue <= 26998) )
    {
      indatum = Datum_North_American_Datum_1983;
      return true;
    }
    else if ( (tvalue >= 26903) && (tvalue <= 26923) )
    {
      indatum = Datum_North_American_Datum_1983;
      return true;
    }
    else if ( (tvalue >= 26729) && (tvalue <= 26798) )
    {
      indatum = Datum_North_American_Datum_1927;
      return true;
    }
    else if ( (tvalue >= 26703) && (tvalue <= 26722) )
    {
      indatum = Datum_North_American_Datum_1927;
      return true;
    }
    else if ( (tvalue >= 24818) && (tvalue <= 24821) )
    {
      indatum = Datum_Provisional_S_American_Datum_1956;
      return true;
    }
    else if ( (tvalue >= 23947) && (tvalue <= 24048) )
    {
      indatum = Datum_Indian_1954;
      return true;
    }
    else if ( (tvalue >= 23028) && (tvalue <= 23038) )
    {
      indatum = Datum_European_Datum_1950;
      return true;
    }
    else if ( (tvalue >= 20137) && (tvalue <= 20138) )
    {
      indatum = Datum_Adindan;
      return true;
    }
    else
    {
      indatum = UNKNOWNDATUM;
      return false;
    }
  }
  catch (...)
  {
    indatum = UNKNOWNDATUM;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}




#endif
