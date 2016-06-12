#ifndef _DOQIMAGEIFILE_CPP_
#define _DOQIMAGEIFILE_CPP_
// $Id: DOQImageIFile.cpp,v 1.2 2003-06-28 16:28:40 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:40 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Implements the DOQImageIFile class
#include <sstream>
#include "DOQImageIFile.h"
#include "GreyPalette.h"
#include "RGBPalette.h"

#ifdef _WIN32
#pragma warning( disable : 4291 ) // Disable warning messages for new(nothrow)
#endif

using namespace USGSImageLib;

// ***************************************************************************
DOQImageIFile::DOQImageIFile(std::string& fn)
  throw(ImageException, std::bad_alloc)
  : ImageIFile(fn), Stream(0), datahandler(0), 
    TopOfImageData(0), position(0), westHour(0), westMinute(0),
    westSecond(0.0), eastHour(0), eastMinute(0), eastSecond(0.0),
    northHour(0), northMinute(0), northSecond(0.0), southHour(0),
    southMinute(0), southSecond(0.0), coordinateZone(0),
    horizontalResolution(0.0), xOrigin(0.0), yOrigin(0.0), 
    secxOrigin(0.0), secyOrigin(0.0), NWQuadCornerX(0.0), NWQuadCornerY(0.0),
    NEQuadCornerX(0.0), NEQuadCornerY(0.0), SEQuadCornerX(0.0),
    SEQuadCornerY(0.0), SWQuadCornerX(0.0), SWQuadCornerY(0.0),
    SNWQuadCornerX(0.0), SNWQuadCornerY(0.0), SNEQuadCornerX(0.0),
    SNEQuadCornerY(0.0), SSEQuadCornerX(0.0), SSEQuadCornerY(0.0),
    SSWQuadCornerX(0.0), SSWQuadCornerY(0.0), rmseXY(0.0),
    dataSize(0), byteCount(0), RGBOrdered(false)
{
  char tempbandorg[4] = "";

  setImageType(IMAGE_DOQ);

  if (!(Stream = new(std::nothrow) std::ifstream(fn.c_str(), 
                                   std::ios::in | std::ios::binary)))
    throw std::bad_alloc();
  if (Stream->fail()) // Something bad happened while opening
  {
    setNoDataBit();
    delete Stream;
    throw(ImageException(IMAGE_FILE_OPEN_ERR));
  }
  else // could open the file ok so go on.
  {
    if (readHeader() ) // Header info parsed so set some stuff
    {
      if (cmp_nocase(bandOrganization, std::string("SINGLE FILE")) == 0)
      {
        setPhotometric(PHOTO_GREY);
        // Make sure we can allocate our data handlers now
        if (!(datahandler = new(std::nothrow) RawGreyIDataHandler(Stream,
                                                             TopOfImageData,
                                                             Width, Height,
                                                             Photometric)))
        {
          // we can't continue since we can't do more than parse headers
          delete Stream;
          setFailRBit();
          throw std::bad_alloc();
        }
        // set the samples per pixel (1 in this case)
        samplesPerPixel = 1;
      }
      else // handle the color cases
      {
        if (cmp_nocase(bandOrganization, std::string("BIP")) == 0)
          setPhotometric(PHOTO_RGB);
        else if (cmp_nocase(bandOrganization, std::string("BIL")) == 0)
          setPhotometric(PHOTO_RGB_BIL);
        else if (cmp_nocase(bandOrganization, std::string("BSQ")) == 0)
          setPhotometric(PHOTO_RGB_BSQ);
        else // don't recognize this one so throw an error
        {
          delete Stream;
          setNoDataBit();
          throw ImageException(IMAGE_FILE_OPEN_ERR);
        }

        // heh heh heh, this looks cool, doesn't it ;)
        tempbandorg[0] = toupper(static_cast<int>(bandContent[0].c_str()[0]));
        tempbandorg[1] = toupper(static_cast<int>(bandContent[1].c_str()[0]));
        tempbandorg[2] = toupper(static_cast<int>(bandContent[2].c_str()[0]));
        tempbandorg[3] = '\0';

        if (std::string("RGB") == std::string(tempbandorg) )
          RGBOrdered = true; // so we don't have to test later on

        if (!(datahandler = new(std::nothrow) RawRGBIDataHandler(Stream,
                                                                TopOfImageData,
                                                                Width,Height,
                                                                Photometric,
                                                                tempbandorg)))
        {
          delete Stream;
          setFailRBit();
          throw std::bad_alloc();
        }
        samplesPerPixel = 3;
      } // else
    } // if (readHeader())
  } // else could open the file ok so go on
}



// ***************************************************************************
DOQImageIFile::~DOQImageIFile()
{
  try
  {
    delete Stream;
    delete datahandler;
    bandContent.clear();
    nations.clear();
    states.clear();
    sourceImageID.clear();
    sourceImageDate.clear();
  }
  catch (...)
  {
  }
}


// ***************************************************************************
// This function creates a palette by scanning the file.  Note that this
// is a slow operation and should be avoided, especially with the color
// DOQ data
void DOQImageIFile::getPalette(Palette* inpalette)
  throw(ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getPalette(inpalette);
  }
  catch (...)
  {
    setFailRBit();
    throw; // to notify caller
  }
}


// ****************************************************************************
// Gets the user specified pixel from the DOQ
void DOQImageIFile::getPixel(long x, long y, AbstractPixel* inpixel)
  throw(ImageException, std::bad_alloc)
{
  try
  { 
    datahandler->getPixel(x, y, inpixel);
  }
  catch (...)
  {
    setFailRBit();
    throw;
  }
}


// ***************************************************************************
void DOQImageIFile::getScanline(long row, RGBPixel* inarray)
  throw(ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getScanline(row, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw; // to notify caller
  }
}

// ****************************************************************************
void DOQImageIFile::getScanline(long row, GreyPixel* inarray)
  throw(ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getScanline(row, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw; // to notify caller
  }
}


// ***************************************************************************
void DOQImageIFile::getRectangle(long x1, long y1, long x2, long y2,
                                 GreyPixel* inarray)
  throw (ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRectangle(x1, y1, x2, y2, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw; // to notify caller
  }
}

// ***************************************************************************
void DOQImageIFile::getRectangle(long x1, long y1, long x2, long y2,
                                 RGBPixel* inarray)
  throw(ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRectangle(x1, y1, x2, y2, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw; // to notify caller
  }
}


// ***************************************************************************
void DOQImageIFile::getRawPixel(long x, long y, unsigned char* inpixel)
  throw(ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRawPixel(x, y, inpixel);
  }
  catch (...)
  {
    setFailRBit();
    throw; // just notify caller
  }
}

// ****************************************************************************
void DOQImageIFile::getRawScanline(long row, unsigned char* inarray)
  throw(ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRawScanline(row, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw; // to notify caller
  }
}

// ***************************************************************************
void DOQImageIFile::getRawRectangle(long x1, long y1, long x2, long y2,
                                    unsigned char* inarray)
  throw(ImageException, std::bad_alloc)
{
  try
  {
    datahandler->getRawRectangle(x1, y1, x2, y2, inarray);
  }
  catch (...)
  {
    setFailRBit();
    throw; // notify caller
  }
}

// ****************************************************************************
bool DOQImageIFile::readBuffer()
  throw(ImageException)
{
  memset(static_cast<void*>(buffer), 0, 80); // clear the buffer

  try
  {
    Stream->getline(buffer, 81);
  }
  catch (...)
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
  if (Stream->fail()) // error that didn't throw an exception
  {
    setFailRBit();
    throw ImageException(IMAGE_FILE_READ_ERR);
  }

  return true;
}


// ****************************************************************************
// Return the number of source images used
void DOQImageIFile::getNumImageSources(int& insources)
  const throw()
{
  insources = sourceImageID.size();
}


// ***************************************************************************
// Scans buffer and returns a numeric value based on what token is in it
int DOQImageIFile::returnToken()
  throw()
{
  std::string tstr(buffer); // Turn the buffer into a temp string

  // This is ugly, but..  Will worry about a better way later on.
  if (search_key(tstr, "BEGIN_USGS_DOQ_HEADER"))
    return 1;
  else if (search_key(tstr, "QUADRANGLE_NAME"))
    return 2;
  else if (search_key(tstr, "QUADRANT"))
    return 3;
  else if (search_key(tstr, "WEST_LONGITUDE"))
    return 4;
  else if (search_key(tstr, "EAST_LONGITUDE"))
    return 5;
  else if (search_key(tstr, "NORTH_LATITUDE"))
    return 6;
  else if (search_key(tstr, "SOUTH_LATITUDE"))
    return 7;
  else if (search_key(tstr, "PRODUCTION_DATE"))
    return 8;
  else if (search_key(tstr, "RASTER_ORDER"))
    return 9;
  else if (search_key(tstr, "BAND_ORGANIZATION"))
    return 10;
  else if (search_key(tstr, "BAND_CONTENT"))
    return 11;
  else if (search_key(tstr, "BITS_PER_PIXEL"))
    return 12;
  else if (search_key(tstr, "SAMPLES_AND_LINES"))
    return 13;
  else if (search_key(tstr, "HORIZONTAL_DATUM"))
    return 14;
  else if (search_key(tstr, "HORIZONTAL_COORDINATE_SYSTEM"))
    return 15;
  else if (search_key(tstr, "COORDINATE_ZONE"))
    return 16;
  else if (search_key(tstr, "HORIZONTAL_UNITS"))
    return 17;
  else if (search_key(tstr, "HORIZONTAL_RESOLUTION"))
    return 18;
  else if (search_key(tstr, "SECONDARY_HORIZONTAL_DATUM"))
    return 19;
  else if (search_key(tstr, "XY_ORIGIN"))
    return 20;
  else if (search_key(tstr, "SECONDARY_XY_ORIGIN"))
    return 21;
  else if (search_key(tstr, "NATION"))
    return 22;
  else if (search_key(tstr, "STATE"))
    return 23;
  else if (search_key(tstr, "NW_QUAD_CORNER_XY"))
    return 24;
  else if (search_key(tstr, "NE_QUAD_CORNER_XY"))
    return 25;
  else if (search_key(tstr, "SE_QUAD_CORNER_XY"))
    return 26;
  else if (search_key(tstr, "SW_QUAD_CORNER_XY"))
    return 27;
  else if (search_key(tstr, "SECONDARY_NW_QUAD_XY"))
    return 28;
  else if (search_key(tstr, "SECONDARY_NE_QUAD_XY"))
    return 29;
  else if (search_key(tstr, "SECONDARY_SE_QUAD_XY"))
    return 30;
  else if (search_key(tstr, "SECONDARY_SW_QUAD_XY"))
    return 31;
  else if (search_key(tstr, "RMSE_XY"))
    return 32;
  else if (search_key(tstr, "IMAGE_SOURCE"))
    return 33;
  else if (search_key(tstr, "SOURCE_IMAGE_ID"))
    return 34;
  else if (search_key(tstr, "SOURCE_IMAGE_DATE"))
    return 35;
  else if (search_key(tstr, "SOURCE_DEM_DATE"))
    return 36;
  else if (search_key(tstr, "AGENCY"))
    return 37;
  else if (search_key(tstr, "PRODUCER"))
    return 38;
  else if (search_key(tstr, "PRODUCTION_SYSTEM"))
    return 39;
  else if (search_key(tstr, "COMPRESSION"))
    return 40;
  else if (search_key(tstr, "STANDARD_VERSION"))
    return 41;
  else if (search_key(tstr, "METADATA_DATE"))
    return 42;
  else if (search_key(tstr, "DATA_FILE_SIZE"))
    return 43;
  else if (search_key(tstr, "BYTE_COUNT"))
    return 44;
  else if (search_key(tstr, "END_USGS_HEADER"))
    return 45;
  else
    return 666; // undefined tag
}
  

// ***************************************************************************
bool DOQImageIFile::readHeader()
  throw()
{
  int _tempToken;
  try
  {
    readBuffer();
    if (returnToken() != 1)
      return false;
  
    readBuffer();
    while ( (_tempToken = returnToken() ) != 45)
    {
      switch (_tempToken)
      {
      case 2:
        setQuadName(); break;
      case 3:
        setQuadrant(); break;
      case 4:
        setWestLongitude(); break;
      case 5:
        setEastLongitude(); break;
      case 6:
        setNorthLatitude();  break;
      case 7:
        setSouthLatitude();  break;
      case 8:
        setProdDate(); break;
      case 9:
        setRasterOrder();  break;
      case 10:
        setBandOrganization(); break;
      case 11:
        setBandContent(); break;
      case 12:
        setBitsPerPixel(); break;
      case 13:
        setSamplesLines(); break;
      case 14:
        setHorizontalDatum(); break;
      case 15:
        setHorizontalCoordinateSystem(); break;
      case 16:
        setCoordinateZone(); break;
      case 17:
        setHorizontalUnits(); break;
      case 18:
        setHorizontalResolution(); break;
      case 19:
        setSecondaryHorizontalDatum(); break;
      case 20:
        setOrigins(); break;
      case 21:
        setSecondaryOrigins(); break;
      case 22:
        setNation(); break;
      case 23:
        setStates(); break;
      case 24:
        setNWQuadCorners(); break;
      case 25:
        setNEQuadCorners(); break;
      case 26:
        setSEQuadCorners(); break;
      case 27:
        setSWQuadCorners(); break;
      case 28:
        setSecondaryNWQuadCorner(); break;
      case 29:
        setSecondaryNEQuadCorner(); break;
      case 30:
        setSecondarySEQuadCorner(); break;
      case 31:
        setSecondarySWQuadCorner(); break;
      case 32:
        setRMSE(); break;
      case 33:
        setImageSource(); break;
      case 34:
        setSourceImageID(); break;
      case 35:
        setSourceImageDate(); break;
      case 36:
        setSourceDEMDate(); break;
      case 37:
        setAgency(); break;
      case 38:
        setProducer(); break;
      case 39:
        setProductionSystem(); break;
      case 40:
        setCompression(); break;
      case 41:
        setStandardVersion(); break;
      case 42:
        setMetaDataDate(); break;
      case 43:
        setDataSize(); break;
      case 44:
        setByteCount(); break;
      default:
        {
        }
      }
    
      readBuffer();
    }
  
    TopOfImageData = Stream->tellg();
    return true;
  }
  catch (...)
  {
    // nothing to do really, just return false
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setQuadName()
  throw()
{
  std::string _temp, temp1;
  char* _end;

  try
  {
    position = strstr(buffer, "\""); // set pos to first quote
    if (position == NULL) // assume it's a single word type thing
    {
      temp1 = buffer;
      std::istringstream is(temp1);
      is >> _temp;
      is >> quadName;
      return true;
    }

    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL) // something happened and we couldn't find the end mark
      return false;
    *_end = '\0'; // set so we can make a string out of the substring

    quadName = position;
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setQuadrant()
  throw()
{
  char* _end;
  std::string _temp, temp1;

  try
  {
    position = strstr(buffer, "\""); // set position to first quote
    if (position == NULL) // assume it's the regular single word type thing
    {
      temp1 = buffer;
      std::istringstream is(temp1);
      is >> _temp;  // Skip past tag
      is >> quadrant;
      return true;
    }

    position = position++; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL) // no end mark present in the string
      return false;
    *_end = '\0';

    quadrant = position;
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setWestLongitude()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> westHour >> westMinute >> westSecond;
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setEastLongitude()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> eastHour >> eastMinute >> eastSecond;
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setNorthLatitude()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> northHour >> northMinute >> northSecond;
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSouthLatitude()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> southHour >> southMinute >> southSecond;
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setProdDate()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> prodDate;
    for (int _mycount = 0; _mycount <= 1; _mycount++)
    {
      is >> _temp;
      prodDate = prodDate + " " + _temp;
    }
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setRasterOrder()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> rasterOrder;
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setBandOrganization()
  throw()
{
  char* _end;
  std::string _temp, temp1;

  try
  {
    position = strstr(buffer, "\""); // set pos to first quote
    if (position == NULL) // check for other types first
    {
      temp1 = buffer;
      std::istringstream is(temp1);
      is >> _temp; // go past keyword
      is >> _temp; // get the value

      // Check if we can support it
      if ( (cmp_nocase(_temp, std::string("BIP")) == 0)
	   || (cmp_nocase(_temp, std::string("BIL")) == 0)
	   || (cmp_nocase(_temp, std::string("BSQ")) == 0) )
      {
        bandOrganization = _temp;
        return true;
      }
      else
      {  // Not something we can handle so return false
        setNoDataBit();
        bandOrganization = "";
        return false;
      }
    }
    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL) // couldn't find the end quote
    {
      setNoDataBit();  // can't read if we don't know it's organization
      return false;
    }
    *_end = '\0'; // set so we can make a string out of the substring
    bandOrganization = position;

    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
// Okie, here, have to keep track of whether or not there's already an entry
// in the list.  If so, add it to the next one
bool DOQImageIFile::setBandContent()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);

    is >> _temp; // grab keyword and ignore
    is >> _temp; // get the actual value of the field

    // First do this test, if the vector already has an entry and we hit
    // the greyscale tag, then something is bad wrong with the DOQ
    if ( (bandContent.size() > 0) 
         && (cmp_nocase(_temp, std::string("BLACK&WHITE")) == 0) )
    {
      setFailRBit();
      return false;
    }
    bandContent.push_back(_temp);
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setBitsPerPixel()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> bitsPerSample;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSamplesLines()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> Width;
    is >> Height;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setHorizontalDatum()
  throw()
{
  char* _end;
  std::string _temp, temp1;

  try
  {
    position = strstr(buffer, "\""); // try to find the first quote
    if (position == NULL) // no quote, assume a single word entry
    {
      temp1 = buffer;
      std::istringstream is(temp1);
      is >> _temp;
      is >> horizontalDatum;
      return true;
    }

    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL)
      return false;
    *_end = '\0'; // so we can make a string out of it
    horizontalDatum = position;
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setHorizontalCoordinateSystem()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> _temp;
    horizCoordSystem = _temp;

    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setCoordinateZone()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> coordinateZone;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setHorizontalUnits()
  throw()
{
  std::string _temp, temp1;
  char* _end;

  try
  {
    position = strstr(buffer, "\""); // try to find the first quote
    if (position == NULL) // assume a single word entry
    {
      temp1 = buffer;
      std::istringstream is(temp1);
      is >> _temp;
      is >> horizontalUnits;
      return true;
    }
  
    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL)
      return false;
    *_end = '\0'; // set so we can make a string out of it

    horizontalUnits = position;
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setHorizontalResolution()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> horizontalResolution;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSecondaryHorizontalDatum()
  throw()
{
  char* _end;
  std::string _temp, temp1;

  try
  {
    position = strstr(buffer, "\""); // try to find the first quote
    if (position == NULL) // assume a single word entry
    {
      temp1 = buffer;
      std::istringstream is(temp1);
      is >> _temp;
      is >> secHorizDatum;
      return true;
    }

    position++; // skip past the first quote mark
    _end = strstr(position, "\"");
    if (_end == NULL)
      return false;
    *_end = '\0'; // set so we can make a string out of it
    secHorizDatum = position;
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setOrigins()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> xOrigin >> yOrigin;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSecondaryOrigins()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> secxOrigin >> secyOrigin;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
// again, keep track of how many are there
bool DOQImageIFile::setNation()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> _temp;
    nations.push_back(_temp);
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setStates()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> _temp;
    states.push_back(_temp);
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setNWQuadCorners()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> NWQuadCornerX >> NWQuadCornerY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setNEQuadCorners()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> NEQuadCornerX >> NEQuadCornerY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSEQuadCorners()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> SEQuadCornerX >> SEQuadCornerY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSWQuadCorners()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> SWQuadCornerX >> SWQuadCornerY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSecondaryNWQuadCorner()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> SNWQuadCornerX >> SNWQuadCornerY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setSecondaryNEQuadCorner()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> SNEQuadCornerX >> SNEQuadCornerY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSecondarySEQuadCorner()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> SSEQuadCornerX >> SSEQuadCornerY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSecondarySWQuadCorner()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> SSWQuadCornerX >> SSWQuadCornerY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setRMSE()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;
    is >> rmseXY;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setImageSource()
  throw()
{
  char* _end;
  std::string _temp;

  try
  {
    position = strstr(buffer, "\""); // set pos to first quote
    if (position == NULL)
      return false;

    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL)
      return false;
    *_end = '\0'; // set so we can make a string out of the substring

    imageSource = position;
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSourceImageID()
  throw()
{
  char* _end;
  std::string _temp, temp1;

  try
  {
    position = strstr(buffer, "\""); // set pos to first quote
    if (position == NULL)
      return false;
    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL)
      return false;
    *_end = '\0'; // set so we can make a string out of the substring

    temp1 = position;
    sourceImageID.push_back(temp1);
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setSourceImageDate()
  throw()
{
  std::string _temp, temp1;
  std::string datestring;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);

    is >> _temp;
    is >> datestring;
    is >> _temp;
    datestring = datestring + " " + _temp;
    is >> _temp;
    datestring = datestring + " " + _temp;

    sourceImageDate.push_back(datestring);
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setSourceDEMDate()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> sourceDEMDate;
    is >> _temp;
    sourceDEMDate = sourceDEMDate + " " + _temp;
    is >> _temp;
    sourceDEMDate = sourceDEMDate + " " + _temp;

    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setAgency()
  throw()
{
  char* _end;
  std::string _temp;

  try
  {
    position = strstr(buffer, "\""); // set pos to first quote
    if (position == NULL)
      return false;

    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL)
      return false;
    *_end = '\0'; // set so we can make a string out of the substring

    agency = position;

    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setProducer()
  throw()
{
  char* _end;
  std::string _temp;

  try
  {
    position = strstr(buffer, "\""); // set pos to first quote
    if (position == NULL)
      return false;

    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL)
      return false;
    *_end = '\0'; // set so we can make a string out of the substring

    producer = position;
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setProductionSystem()
  throw()
{
  char* _end;
  std::string _temp;

  try
  {
    position = strstr(buffer, "\""); // set pos to first quote
    if (position == NULL)
      return false;

    position = position + 1; // skip past the first quotation mark
    _end = strstr(position, "\"");
    if (_end == NULL)
      return false;
    *_end = '\0'; // set so we can make a string out of the substring

    productionSystem = position;
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setCompression()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);

    is >> _temp;
    is >> compression;
    is >> _temp;
    compression = compression + " " + _temp;
    is >> _temp;
    compression = compression + " " + _temp;

    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setStandardVersion()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);

    is >> _temp;
    is >> standardVersion;
    is >> _temp;
    standardVersion = standardVersion + " " + _temp;

    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setMetaDataDate()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
    is >> _temp;

    is >> metaDataDate;
    is >> _temp;
    metaDataDate = metaDataDate + " " + _temp;
    is >> _temp;
    metaDataDate = metaDataDate + " " + _temp;

    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool DOQImageIFile::setDataSize()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> dataSize;

    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool DOQImageIFile::setByteCount()
  throw()
{
  std::string _temp, temp1;

  try
  {
    temp1 = buffer;
    std::istringstream is(temp1);
  
    is >> _temp;
    is >> byteCount;
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
void DOQImageIFile::getQuadName(std::string& instring)
  const throw()
{
  instring = quadName;
}

// ***************************************************************************
void DOQImageIFile::getQuadrant(std::string& instring)
  const throw()
{
  instring = quadrant;
}

// ***************************************************************************
void DOQImageIFile::getWestHour(int& inhour)
  const throw()
{
  inhour = westHour;
}

// ***************************************************************************
void DOQImageIFile::getWestMinute(int& inminute)
  const throw()
{
  inminute = westMinute;
}

// ***************************************************************************
void DOQImageIFile::getWestSecond(float& insecond)
  const throw()
{
  insecond = westSecond;
}

// ***************************************************************************
void DOQImageIFile::getEastHour(int& inhour)
  const throw()
{
  inhour = eastHour;
}

// ***************************************************************************
void DOQImageIFile::getEastMinute(int& inminute)
  const throw()
{
  inminute = eastMinute;
}

// ***************************************************************************
void DOQImageIFile::getEastSecond(float& insecond)
  const throw()
{
  insecond = eastSecond;
}

// ***************************************************************************
void DOQImageIFile::getNorthHour(int& inhour)
  const throw()
{
  inhour = northHour;
}

// ***************************************************************************
void DOQImageIFile::getNorthMinute(int& inminute)
  const throw()
{
  inminute = northMinute;
}

// ***************************************************************************
void DOQImageIFile::getNorthSecond(float& insecond)
  const throw()
{
  insecond = northSecond;
}

// ***************************************************************************
void DOQImageIFile::getSouthHour(int& inhour)
  const throw()
{
  inhour = southHour;
}

// ***************************************************************************
void DOQImageIFile::getSouthMinute(int& inminute)
  const throw()
{
  inminute = southMinute;
}

// ***************************************************************************
void DOQImageIFile::getSouthSecond(float& insecond)
  const throw()
{
  insecond = southSecond;
}

// ***************************************************************************
void DOQImageIFile::getProdDate(std::string& instring)
  const throw()
{
  instring = prodDate;
}

// ***************************************************************************
void DOQImageIFile::getRasterOrder(std::string& instring)
  const throw()
{
  instring = rasterOrder;
}

// ***************************************************************************
void DOQImageIFile::getBandOrganization(std::string& instring)
  const throw()
{
  instring = bandOrganization;
}

// ***************************************************************************
void DOQImageIFile::getNumBandContents(int& innumber)
  const throw()
{
  innumber = bandContent.size();
}

// ***************************************************************************
void DOQImageIFile::getBandContent(std::string& instring, int inpos)
  const throw()
{
  if ( (inpos <= static_cast<int>(bandContent.size())) && (inpos >= 1) )
    instring = bandContent[inpos - 1];
  else
    instring = std::string("");
}

// ***************************************************************************
void DOQImageIFile::getBitsPerPixel(int& inbps)
  const throw()
{
  inbps = bitsPerSample;
}


// ***************************************************************************
void DOQImageIFile::getSamples(long int& insamples)
  const throw()
{
  insamples = Width;
}


// ***************************************************************************
void DOQImageIFile::getLines(long int& inlines)
  const throw()
{
  inlines = Height;
}

// ***************************************************************************
void DOQImageIFile::getHorizontalDatum(std::string& indatum)
  const throw()
{
  indatum = horizontalDatum;
}

// ***************************************************************************
void DOQImageIFile::getHorizontalCoordinateSystem(std::string& insystem)
  const throw()
{
  insystem = horizCoordSystem;
}

// ***************************************************************************
void DOQImageIFile::getCoordinateZone(int& inzone)
  const throw()
{
  inzone = coordinateZone;
}

// ***************************************************************************
void DOQImageIFile::getHorizontalUnits(std::string& inunits)
  const throw()
{
  inunits = horizontalUnits;
}

// ***************************************************************************
void DOQImageIFile::getHorizontalResolution(float& inres)
  const throw()
{
  inres = horizontalResolution;
}

// ***************************************************************************
void DOQImageIFile::getSecondaryHorizontalDatum(std::string& indatum)
  const throw()
{
  indatum = secHorizDatum;
}

// ***************************************************************************
void DOQImageIFile::getXOrigin(double& inx)
  const throw()
{
  inx = xOrigin;
}

// ***************************************************************************
void DOQImageIFile::getYOrigin(double& iny)
  const throw()
{
  iny = yOrigin;
}

// ***************************************************************************
void DOQImageIFile::getSecondaryXOrigin(double& inx)
  const throw()
{
  inx = secxOrigin;
}

// ***************************************************************************
void DOQImageIFile::getSecondaryYOrigin(double& iny)
  const throw()
{
  iny = secyOrigin;
}

// ***************************************************************************
void DOQImageIFile::getNumNations(int& innations)
  const throw()
{
  innations = nations.size();
}

// ***************************************************************************
void DOQImageIFile::getNation(std::string& innation, int nationnumber)
  const throw()
{
  if ( (nationnumber > 0) && 
       (nationnumber <= static_cast<int>(nations.size())) )
    innation = nations[nationnumber - 1];
  else
    innation = std::string("");
}

// ***************************************************************************
void DOQImageIFile::getNumStates(int& instates)
  const throw()
{
  instates = states.size();
}

// ***************************************************************************
void DOQImageIFile::getState(std::string& instate, int statenumber)
  const throw()
{
  
  if ( (statenumber > 0) && (statenumber <= static_cast<int>(states.size())) )
    instate = states[statenumber - 1];
  else
    instate = std::string("");
}

// ***************************************************************************
void DOQImageIFile::getNWQuadX(double& inx)
  const throw()
{
  inx = NWQuadCornerX;
}

// ***************************************************************************
void DOQImageIFile::getNWQuadY(double& iny)
  const throw()
{
  iny = NWQuadCornerY;
}

// ***************************************************************************
void DOQImageIFile::getNEQuadX(double& inx)
  const throw()
{
  inx = NEQuadCornerX;
}

// ***************************************************************************
void DOQImageIFile::getNEQuadY(double& iny)
  const throw()
{
  iny = NEQuadCornerY;
}

// ***************************************************************************
void DOQImageIFile::getSEQuadX(double& inx)
  const throw()
{
  inx = SEQuadCornerX;
}

// ***************************************************************************
void DOQImageIFile::getSEQuadY(double& iny)
  const throw()
{
  iny = SEQuadCornerY;
}

// ***************************************************************************
void DOQImageIFile::getSWQuadX(double& inx)
  const throw()
{
  inx = SWQuadCornerX;
}

// ***************************************************************************
void DOQImageIFile::getSWQuadY(double& iny)
  const throw()
{
  iny = SWQuadCornerY;
}

// ***************************************************************************
void DOQImageIFile::getSecNWQuadX(double& inx)
  const throw()
{
  inx = SNWQuadCornerX;
}

// ***************************************************************************
void DOQImageIFile::getSecNWQuadY(double& iny)
  const throw()
{
  iny = SNWQuadCornerY;
}

// ***************************************************************************
void DOQImageIFile::getSecNEQuadX(double& inx)
  const throw()
{
  inx = SNEQuadCornerX;
}

// ***************************************************************************
void DOQImageIFile::getSecNEQuadY(double& iny)
  const throw()
{
  iny = SNEQuadCornerY;
}

// ***************************************************************************
void DOQImageIFile::getSecSEQuadX(double& inx)
  const throw()
{
  inx = SSEQuadCornerX;
}

// ***************************************************************************
void DOQImageIFile::getSecSEQuadY(double& iny)
  const throw()
{
  iny = SSEQuadCornerY;
}

// ***************************************************************************
void DOQImageIFile::getSecSWQuadX(double& inx)
  const throw()
{
  inx = SSWQuadCornerX;
}

// ***************************************************************************
void DOQImageIFile::getSecSWQuadY(double& iny)
  const throw()
{
  iny = SSWQuadCornerY;
}

// ***************************************************************************
void DOQImageIFile::getRMSE(float& inrmse)
  const throw()
{
  inrmse = rmseXY;
}

// ***************************************************************************
void DOQImageIFile::getImageSource(std::string& insource)
  const throw()
{
  insource = imageSource;
}

// ***************************************************************************
void DOQImageIFile::getSourceImageID(std::string& inid, int numimage)
  const throw()
{
  if ( (numimage > 0) && (numimage <= static_cast<int>(sourceImageID.size())) )
    inid = sourceImageID[numimage - 1];
  else
    inid = std::string("");
}

// ***************************************************************************
void DOQImageIFile::getSourceImageDate(std::string& indate, int numimage)
  const throw()
{
  if ( (numimage > 0) && 
       (numimage <= static_cast<int>(sourceImageDate.size())) )
    indate = sourceImageDate[numimage - 1];
  else
    indate = std::string("");
}

// ***************************************************************************
void DOQImageIFile::getSourceDEMDate(std::string& indate)
  const throw()
{
  indate = sourceDEMDate;
}

// ***************************************************************************
void DOQImageIFile::getAgency(std::string& inagency)
  const throw()
{
  inagency = agency;
}

// ***************************************************************************
void DOQImageIFile::getProducer(std::string& inproducer)
  const throw()
{
  inproducer = producer;
}

// ***************************************************************************
void DOQImageIFile::getProductionSystem(std::string& insystem)
  const throw()
{
  insystem = productionSystem;
}

// ***************************************************************************
void DOQImageIFile::getCompression(std::string& incompression)
  const throw()
{
  incompression = compression;
}

// ***************************************************************************
void DOQImageIFile::getStandardVersion(std::string& inversion)
  const throw()
{
  inversion = standardVersion;
}

// ***************************************************************************
void DOQImageIFile::getMetaDataDate(std::string& indate)
  const throw()
{
  indate = metaDataDate;
}

// ***************************************************************************
void DOQImageIFile::getDataSize(long int& insize)
  const throw()
{
  insize = dataSize;
}

// ***************************************************************************
void DOQImageIFile::getByteCount(long int& incount)
  const throw()
{
  incount = byteCount;
}

#ifdef _WIN32
#pragma warning( default : 4291 )  // Disable warning messages
#endif

#endif
