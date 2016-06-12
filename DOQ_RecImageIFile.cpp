#ifndef _DOQRECIMAGEIFILE_CPP_
#define _DOQRECIMAGEIFILE_CPP_
// $Id: DOQ_RecImageIFile.cpp,v 1.2 2003-06-28 16:28:40 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:40 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class implements the reading of the 1992 record format DOQ's

#include <sstream>
#include "DOQ_RecImageIFile.h"

using namespace USGSImageLib;

#ifdef _WIN32
#pragma warning( disable : 4291 ) // Disable warning messages for new(nothrow)
#endif

// ***************************************************************************
DOQ_RecImageIFile::DOQ_RecImageIFile(std::string& infile)
  throw(ImageException, std::bad_alloc)
  : ImageIFile(infile),
    TopOfImageData(0), dataorder(0), bandtypes(0), elevationstorage(0),
    bandelevationstorage(0), verticaldatum(0), horizontaldatum(0), 
    sechorizontaldatum(0), rotationangle(0.0), groundrefsystem(0), zone(0),
    planimetricunits(0),
    a(0.0), b(0.0), c(0.0), e(0.0), f(0.0), xc(0.0), yc(0.0),
    seca(0.0), secb(0.0), secc(0.0), secd(0.0), sece(0.0), secf(0.0),
    secxc(0.0), secyc(0.0),
    elevationunits(0), minimumelevation(0.0), maximumelevation(0.0),
    maxcontourinterval(0), maxcontourunits(0),
    mincontourinterval(0), mincontourunits(0),
    suspectvoids(0), testpointnums(0), pixelprocessingalgo(0), 
    mosaickedimage(0), focallength(0.0), flightheight(0), 
    radiometricres(0), resampledres(0.0), compressedsize(0),
    compressioncontrol(0)
{

  // more init stuff
  SWQuadCorners[0] = SWQuadCorners[1] = 0.0;
  NWQuadCorners[0] = NWQuadCorners[1] = 0.0;
  NEQuadCorners[0] = NEQuadCorners[1] = 0.0;
  SEQuadCorners[0] = SEQuadCorners[1] = 0.0;
  SWSecQuadCorners[0] = SWSecQuadCorners[1] = 0.0;
  NWSecQuadCorners[0] = NWSecQuadCorners[1] = 0.0;
  NESecQuadCorners[0] = NESecQuadCorners[1] = 0.0;
  SESecQuadCorners[0] = NESecQuadCorners[1] = 0.0;
  SWPixelCorners[0] = SWPixelCorners[1] = 0;
  NWPixelCorners[0] = NWPixelCorners[1] = 0;
  NEPixelCorners[0] = NEPixelCorners[1] = 0;
  SEPixelCorners[0] = SEPixelCorners[1] = 0;
  SWSecPixelCorners[0] = SWSecPixelCorners[1] = 0;
  NWSecPixelCorners[0] = NWSecPixelCorners[1] = 0;
  NESecPixelCorners[0] = NESecPixelCorners[1] = 0;
  SESecPixelCorners[0] = SESecPixelCorners[1] = 0;
  origins[0] = origins[1] = 0.0; 
  secorigins[0] = secorigins[1] = 0.0;
  groundres[0] = groundres[1] = groundres[2] = 0.0;
  pixelres[0] = pixelres[1] = pixelres[2] = 0.0;
  rmse[0] = rmse[1] = 0.0;
  productiondate[0] = productiondate[1] = productiondate[2] = 0;
  sourcephotodate[0] = sourcephotodate[1] = sourcephotodate[2] = 0;
  scanningres[0] = scanningres[1] = 0.0;

  setImageType(IMAGE_DOQ);
  // open the input stream
  if (!(Stream = new(std::nothrow) std::ifstream(infile.c_str(),
				   std::ios::in | std::ios::binary)))
    throw std::bad_alloc();
  if (Stream->fail())
  {
    setNoDataBit();
    delete Stream;
    throw ImageException(IMAGE_FILE_OPEN_ERR);
  }
  // Must have opened the file ok so try to go on
  if (readHeader())
  {
    if ( (bandtypes >= 1) && (bandtypes <= 4) )
    {
      // just treat single band color data as greyscale and let the caller
      // worry about it
      setPhotometric(PHOTO_GREY);
      if (!(datahandler = new(std::nothrow) RawGreyIDataHandler(Stream,
								TopOfImageData,
								Width, Height,
								Photometric)))
      {
	delete Stream;
	setFailRBit();
	throw std::bad_alloc();
      }
      samplesPerPixel = 1;
      bitsPerSample = 8;
    }
    else if (bandtypes == 5)
    {
      setPhotometric(PHOTO_RGB);
      if (!(datahandler = new(std::nothrow) RawRGBIDataHandler(Stream,
							       TopOfImageData,
							       Width, Height,
							       Photometric,
							       "RGB")))
      {
	delete Stream;
	setFailRBit();
	throw std::bad_alloc();
      }
      samplesPerPixel = 3;
      bitsPerSample = 8;
    }
    else // something we can't handle so run away
    {
      delete Stream;
      setFailRBit();
      throw ImageException(IMAGE_FILE_OPEN_ERR);
    }
  }
  else
  {
    delete Stream;
    throw ImageException(IMAGE_FILE_READ_ERR);
  }
}


// ***************************************************************************
DOQ_RecImageIFile::~DOQ_RecImageIFile()
{
  try
  {
    nations.clear();
    states.clear();
    counties.clear();
    delete Stream;
    delete datahandler;
  }
  catch (...) // don't throw, just exit
  {
  }
}


// ***************************************************************************
bool DOQ_RecImageIFile::readHeader()
  throw (ImageException, std::bad_alloc)
{
  char buffer[400];
  char tbuff[50];
  char* tptr;

  memset(buffer, 0, 400);
  Stream->read(buffer, 400);
  tptr = buffer;

  // Parse the record 1 stuff out
  quadname = std::string(tptr, 38);
  tptr += 38;
  quadrant = std::string(tptr, 2);
  tptr += 2;
  nations.push_back(std::string(tptr, 2));
  tptr += 2;
  nations.push_back(std::string(tptr, 2));
  tptr += 2;
  states.push_back(std::string(tptr, 2));
  tptr += 2;
  states.push_back(std::string(tptr, 2));
  tptr += 2;
  states.push_back(std::string(tptr, 2));
  tptr += 2;
  states.push_back(std::string(tptr, 2));
  tptr += 2;
  counties.push_back(std::string(tptr, 3)); // State 1 Counties
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr ,3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3)); // State 2 Counties
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3)); // State 3 Counties
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3)); // State 4 Counties
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 3;
  counties.push_back(std::string(tptr, 3));
  tptr += 5;
  filler = std::string(tptr, 23);
  tptr += 23;
  producer = std::string(tptr, 4);
  tptr += 4;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  dataorder = atoi(tbuff);
  tptr += 3;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  Height = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  Width = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  bandtypes = atoi(tbuff);
  tptr += 3;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  elevationstorage = atoi(tbuff);
  tptr += 3;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  bandelevationstorage = atoi(tbuff);
  tptr += 3;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 2);
  verticaldatum = atoi(tbuff);
  tptr += 2;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 2);
  horizontaldatum = atoi(tbuff);
  tptr += 2;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 2);
  sechorizontaldatum = atoi(tbuff);
  tptr += 2;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  rotationangle = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  groundrefsystem = atoi(tbuff);
  tptr += 3;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  zone = atoi(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  planimetricunits = atoi(tptr);
  tptr += 3;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  SWQuadCorners[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  SWQuadCorners[1] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  NWQuadCorners[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  NWQuadCorners[1] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  NEQuadCorners[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  NEQuadCorners[1] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  SEQuadCorners[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  SEQuadCorners[1] = myatof(tbuff);
  
  // Record 2
  memset(buffer, 0, 400);
  Stream->seekg( (Width - 400), std::ios::cur);
  Stream->read(buffer, 400);

  tptr = buffer;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  a = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  b = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  c = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  d = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  e = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  f = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  xc = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  yc = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  SWSecQuadCorners[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  SWSecQuadCorners[1] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  NWSecQuadCorners[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  NWSecQuadCorners[1] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  NESecQuadCorners[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  NESecQuadCorners[1] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  SESecQuadCorners[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  SESecQuadCorners[1] = myatof(tbuff);

  // Record 3
  memset(buffer, 0, 400);
  Stream->seekg( (Width - 400), std::ios::cur);
  Stream->read(buffer, 400);

  tptr = buffer;

  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  seca = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  secb = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  secc = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  secd = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  sece = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  secf = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  secxc = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  secyc = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  SWPixelCorners[1] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  SWPixelCorners[0] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  NWPixelCorners[1] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  NWPixelCorners[0] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  NEPixelCorners[1] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  NEPixelCorners[0] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  SEPixelCorners[1] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  SEPixelCorners[0] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  SWSecPixelCorners[1] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  SWSecPixelCorners[0] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  NWSecPixelCorners[1] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  NWSecPixelCorners[0] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  NESecPixelCorners[1] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  NESecPixelCorners[0] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  SESecPixelCorners[1] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  SESecPixelCorners[0] = atol(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  origins[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  origins[1] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  secorigins[0] = myatof(tbuff);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 24);
  secorigins[1] = myatof(tbuff);

  // Record 4
  memset(buffer, 0, 400);
  Stream->seekg( (Width - 400), std::ios::cur);
  Stream->read(buffer, 400);
  tptr = buffer;

  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  elevationunits = atoi(tbuff);
  tptr += 3;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 10);
  minimumelevation = myatof(tbuff);
  tptr += 10;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 10);
  maximumelevation = myatof(tbuff);
  tptr += 10;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 12);
  groundres[0] = myatof(tbuff);
  tptr += 12;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 12);
  groundres[1] = myatof(tbuff);
  tptr += 12;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 12);
  groundres[2] = myatof(tbuff);
  tptr += 12;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 12);
  pixelres[0] = myatof(tbuff);
  tptr += 12;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 12);
  pixelres[1] = myatof(tbuff);
  tptr += 12;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 12);
  pixelres[2] = myatof(tbuff);
  tptr += 12;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 5);
  maxcontourinterval = atoi(tbuff);
  tptr += 5;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 1);
  maxcontourunits = atoi(tbuff);
  tptr += 1;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 5);
  mincontourinterval = atoi(tbuff);
  tptr += 5;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 1);
  mincontourunits = atoi(tbuff);
  tptr += 1;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 2);
  suspectvoids = atoi(tbuff);
  tptr += 2;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  rmse[0] = myatof(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  rmse[1] = myatof(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 4);
  testpointnums = atoi(tbuff);
  tptr += 4;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 2);
  pixelprocessingalgo = atoi(tbuff);
  tptr += 2;
  productionsystem = std::string(tptr, 24);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  productiondate[0] = atoi(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 2);
  productiondate[1] = atoi(tbuff);
  tptr += 2;
  memset(tbuff, 0, 50);
  strncpy(tbuff,  tptr, 2);
  productiondate[2] = atoi(tbuff);
  tptr += 2;
  filmtype = std::string(tptr, 24);
  tptr += 24;
  sourcephotoid = std::string(tptr, 24);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  mosaickedimage = atoi(tbuff);
  tptr += 3;
  leafflag = std::string(tptr, 2);
  tptr += 2;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 4);
  sourcephotodate[0] = atoi(tbuff);
  tptr += 4;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 2);
  sourcephotodate[1] = atoi(tbuff);
  tptr += 2;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 2);
  sourcephotodate[2] = atoi(tbuff);
  tptr += 2;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 8);
  focallength = myatof(tbuff);
  tptr += 8;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 10);
  flightheight = atol(tbuff);
  tptr += 10;
  scannertype = std::string(tptr, 24);
  tptr += 24;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  scanningres[0] = myatof(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  scanningres[1] = myatof(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  scannersampleres[0] = myatof(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  scannersampleres[1] = myatof(tbuff);
  tptr += 6;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 3);
  radiometricres = atoi(tbuff);
  tptr += 3;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 6);
  resampledres = myatof(tbuff);
  tptr += 6;
  compressionflag = std::string(tptr, 1);
  tptr += 1;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 9);
  compressedsize = atol(tbuff);
  tptr += 9;
  compressionalgorithm = std::string(tptr, 20);
  tptr += 20;
  compressiontype = std::string(tptr, 1);
  tptr += 1;
  memset(tbuff, 0, 50);
  strncpy(tbuff, tptr, 4);
  compressioncontrol = atoi(tbuff);
  
  Stream->seekg( (Width - 400), std::ios::cur); // to the top of the image data
  TopOfImageData = Stream->tellg();

  return true;
}


// ***************************************************************************
void DOQ_RecImageIFile::getQuadName(std::string& inname) const throw()
{
  inname = quadname;
}


// ***************************************************************************
void DOQ_RecImageIFile::getQuadrant(std::string& inquadrant) const throw()
{
  inquadrant = quadrant;
}


// ***************************************************************************
void DOQ_RecImageIFile::getNumNations(int& insize) const throw()
{
  insize = nations.size();
}


// ***************************************************************************
// Remember, you call this counting from 1, it's stored 0 based
void DOQ_RecImageIFile::getNation(std::string& innation, int num) const throw()
{
  if ( (num < 0) || (num > 2) )
    innation = std::string("");
  else
    innation = nations[num - 1]; // to offset from 0 for caller
}


// ***************************************************************************
void DOQ_RecImageIFile::getNumStates(int& insize) const throw()
{
  insize = states.size();
}


// ***************************************************************************
void DOQ_RecImageIFile::getState(std::string& instate, int num) const throw()
{
  if ( (num < 0) || (num > 4))
    instate = std::string("");
  else
    instate = states[num - 1];
}


// ***************************************************************************
void DOQ_RecImageIFile::getStateCounty(int& incode, int statenum,
				       int countynum) const throw()
{
  if ( (statenum < 0) || (statenum > 4) || (countynum < 0) || (countynum > 5))
    incode = -999;
  else
    incode = atoi(counties[( (statenum - 1) * 5 + countynum - 1)].c_str());
}


// ***************************************************************************
void DOQ_RecImageIFile::getFiller1(std::string& infiller) const throw()
{
  infiller = filler;
}


// ***************************************************************************
void DOQ_RecImageIFile::getProducer(std::string& inproducer) const throw()
{
  inproducer = producer;
}


// ***************************************************************************
void DOQ_RecImageIFile::getDataOrder(int& inorder) const throw()
{
  inorder = dataorder;
}


// ***************************************************************************
void DOQ_RecImageIFile::getLines(long int& inlines) const throw()
{
  inlines = Height;
}


// ***************************************************************************
void DOQ_RecImageIFile::getSamples(long int& insamples) const throw()
{
  insamples = Width;
}


// ***************************************************************************
void DOQ_RecImageIFile::getBandTypes(int& intypes) const throw()
{
  intypes = bandtypes;
}


// ***************************************************************************
void DOQ_RecImageIFile::getElevationStorage(int& instorage) const throw()
{
  instorage = elevationstorage;
}


// ***************************************************************************
void DOQ_RecImageIFile::getBandElevationStorage(int& instorage) const throw()
{
  instorage = bandelevationstorage;
}

// ***************************************************************************
void DOQ_RecImageIFile::getVerticalDatum(int& indatum) const throw()
{
  indatum = verticaldatum;
}

// ***************************************************************************
void DOQ_RecImageIFile::getHorizontalDatum(int& indatum) const throw()
{
  indatum = horizontaldatum;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecondaryHorizontalDatum(int& indatum) const throw()
{
  indatum = sechorizontaldatum;
}

// ***************************************************************************
void DOQ_RecImageIFile::getAngleRotation(double& inangle) const throw()
{
  inangle = rotationangle;
}

// ***************************************************************************
void DOQ_RecImageIFile::getGroundReferenceSystem(int& inrefsys) const throw()
{
  inrefsys = groundrefsystem;
}

// ***************************************************************************
void DOQ_RecImageIFile::getCoordinateZone(int& inzone) const throw()
{
  inzone = zone;
}

// ***************************************************************************
void DOQ_RecImageIFile::getHorizontalUnits(int& inunits) const throw()
{
  inunits = planimetricunits;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSWQuadX(double& inx) const throw()
{
  inx = SWQuadCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSWQuadY(double& iny) const throw()
{
  iny = SWQuadCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNWQuadX(double& inx) const throw()
{
  inx = NWQuadCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNWQuadY(double& iny) const throw()
{
  iny = NWQuadCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNEQuadX(double& inx) const throw()
{
  inx = NEQuadCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNEQuadY(double& iny) const throw()
{
  iny = NEQuadCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSEQuadX(double& inx) const throw()
{
  inx = SEQuadCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSEQuadY(double& iny) const throw()
{
  iny = SEQuadCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getA(double& ina) const throw()
{
  ina = a;
}

// ***************************************************************************
void DOQ_RecImageIFile::getB(double& inb) const throw()
{
  inb = b;
}

// ***************************************************************************
void DOQ_RecImageIFile::getC(double& inc) const throw()
{
  inc = c;
}

// ***************************************************************************
void DOQ_RecImageIFile::getD(double& ind) const throw()
{
  ind = d;
}

// ***************************************************************************
void DOQ_RecImageIFile::getE(double& ine) const throw()
{
  ine = e;
}

// ***************************************************************************
void DOQ_RecImageIFile::getF(double& inf) const throw()
{
  inf = f;
}

// ***************************************************************************
void DOQ_RecImageIFile::getXc(double& inxc) const throw()
{
  inxc = xc;
}

// ***************************************************************************
void DOQ_RecImageIFile::getYc(double& inyc) const throw()
{
  inyc = yc;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecSWQuadX(double& inx) const throw()
{
  inx = SWSecQuadCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecSWQuadY(double& iny) const throw()
{
  iny = SWSecQuadCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecNWQuadX(double& inx) const throw()
{
  inx = NWSecQuadCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecNWQuadY(double& iny) const throw()
{
  iny = NWSecQuadCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecNEQuadX(double& inx) const throw()
{
  inx = NESecQuadCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecNEQuadY(double& iny) const throw()
{
  iny = NESecQuadCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecSEQuadX(double& inx) const throw()
{
  inx = SESecQuadCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecSEQuadY(double& iny) const throw()
{
  iny = SESecQuadCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecA(double& ina) const throw()
{
  ina = seca;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecB(double& inb) const throw()
{
  inb = secb;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecC(double& inc) const throw()
{
  inc = secc;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecD(double& ind) const throw()
{
  ind = secd;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecE(double& ine) const throw()
{
  ine = sece;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecF(double& inf) const throw()
{
  inf = secf;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecXc(double& inxc) const throw()
{
  inxc = secxc;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecYc(double& inyc) const throw()
{
  inyc = secyc;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSWPixelX(long int& inx) const throw()
{
  inx = SWPixelCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSWPixelY(long int& iny) const throw()
{
  iny = SWPixelCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNWPixelX(long int& inx) const throw()
{
  inx = NWPixelCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNWPixelY(long int& iny) const throw()
{
  iny = NWPixelCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNEPixelX(long int& inx) const throw()
{
  inx = NEPixelCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNEPixelY(long int& iny) const throw()
{
  iny = NEPixelCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSEPixelX(long int& inx) const throw()
{
  inx = SEPixelCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSEPixelY(long int& iny) const throw()
{
  iny = SEPixelCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecSWPixelX(long int& inx) const throw()
{
  inx = SWSecPixelCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecSWPixelY(long int& iny) const throw()
{
  iny = SWSecPixelCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecNWPixelX(long int& inx) const throw()
{
  inx = NWSecPixelCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecNWPixelY(long int& iny) const throw()
{
  iny = NWSecPixelCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecNEPixelX(long int& inx) const  throw()
{
  inx = NESecPixelCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecNEPixelY(long int& iny) const throw()
{
  iny = NESecPixelCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecSEPixelX(long int& inx) const throw()
{
  inx = SESecPixelCorners[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecSEPixelY(long int& iny) const throw()
{
  iny = SESecPixelCorners[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getXOrigin(double& inx) const throw()
{
  inx = origins[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getYOrigin(double& iny) const throw()
{
  iny = origins[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecondaryXOrigin(double& inx) const throw()
{
  inx = secorigins[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getSecondaryYOrigin(double& iny) const throw()
{
  iny = secorigins[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getElevationUnits(int& inunits) const throw()
{
  inunits = elevationunits;
}

// ***************************************************************************
void DOQ_RecImageIFile::getMinimumElevation(double& inelev) const throw()
{
  inelev = minimumelevation;
}

// ***************************************************************************
void DOQ_RecImageIFile::getMaximumElevation(double& inelev) const throw()
{
  inelev = maximumelevation;
}

// ***************************************************************************
void DOQ_RecImageIFile::getGroundXRes(double& inres) const throw()
{
  inres = groundres[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getGroundYRes(double& inres) const throw()
{
  inres = groundres[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getGroundZRes(double& inres) const throw()
{
  inres = groundres[2];
}

// ***************************************************************************
void DOQ_RecImageIFile::getPixelXRes(double& inres) const throw()
{
  inres = pixelres[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getPixelYRes(double& inres) const throw()
{
  inres = pixelres[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getPixelZRes(double& inres) const throw()
{
  inres = pixelres[2];
}

// ***************************************************************************
void DOQ_RecImageIFile::getMaxContourInterval(int& inint) const throw()
{
  inint = maxcontourinterval;
}

// ***************************************************************************
void DOQ_RecImageIFile::getMaxContourUnits(int& inunits) const throw()
{
  inunits = maxcontourunits;
}

// ***************************************************************************
void DOQ_RecImageIFile::getMinContourInterval(int& inint) const throw()
{
  inint = mincontourinterval;
}

// ***************************************************************************
void DOQ_RecImageIFile::getMinContourUnits(int& inunits) const throw()
{
  inunits = mincontourunits;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSuspectVoidAreas(int& inint) const throw()
{
  inint = suspectvoids;
}

// ***************************************************************************
void DOQ_RecImageIFile::getRMSEX(double& inrmse) const throw()
{
  inrmse = rmse[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getRMSEY(double& inrmse) const throw()
{
  inrmse = rmse[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getNumTestPoints(int& inpoints) const throw()
{
  inpoints = testpointnums;
}

// ***************************************************************************
void DOQ_RecImageIFile::getPixelProcessingAlgorithm(int& inalgo) const throw()
{
  inalgo = pixelprocessingalgo;
}

// ***************************************************************************
void DOQ_RecImageIFile::getProductionSystem(std::string& insys) const throw()
{
  insys = productionsystem;
}

// ***************************************************************************
void DOQ_RecImageIFile::getProductionDate(std::string& indate) const throw()
{
  std::stringstream tempstream;

  tempstream << productiondate[0] << std::string(" ") << productiondate[1]
             << std::string(" ") << productiondate[2] << std::ends;

  tempstream >> indate;

  return;
}

// ***************************************************************************
void DOQ_RecImageIFile::getFilmType(std::string& intype) const throw()
{
  intype = filmtype;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSourcePhotoID(std::string& inid) const throw()
{
  inid = sourcephotoid;
}

// ***************************************************************************
void DOQ_RecImageIFile::getMosaickedImage(int& incode) const throw()
{
  incode = mosaickedimage;
}

// ***************************************************************************
void DOQ_RecImageIFile::getLeafFlag(std::string& inflag) const throw()
{
  inflag = leafflag;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSourcePhotoDate(std::string& indate) const throw()
{
  std::stringstream tempstream;

  tempstream << sourcephotodate[0] << std::string(" ") << sourcephotodate[1]
             << std::string(" ") << sourcephotodate[2] << std::ends;

  tempstream >> indate;

  return;
}

// ***************************************************************************
void DOQ_RecImageIFile::getCameraFocalLength(double& inlen) const throw()
{
  inlen = focallength;
}

// ***************************************************************************
void DOQ_RecImageIFile::getSourceFlightHeight(long int& inheight) const throw()
{
  inheight = flightheight;
}

// ***************************************************************************
void DOQ_RecImageIFile::getScannerType(std::string& intype) const throw()
{
  intype = scannertype;
}

// ***************************************************************************
void DOQ_RecImageIFile::getScanningXRes(double& inres) const throw()
{
  inres = scanningres[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getScanningYRes(double& inres) const throw()
{
  inres = scanningres[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getScannerXSampleRes(double& inres) const throw()
{
  inres = scannersampleres[0];
}

// ***************************************************************************
void DOQ_RecImageIFile::getScannerYSampleRes(double& inres) const throw()
{
  inres = scannersampleres[1];
}

// ***************************************************************************
void DOQ_RecImageIFile::getRadiometricResolution(int& inres) const throw()
{
  inres = radiometricres;
}

// ***************************************************************************
void DOQ_RecImageIFile::getResampledResolution(double& inres) const throw()
{
  inres = resampledres;
}


// ***************************************************************************
void DOQ_RecImageIFile::getCompressionFlag(std::string& inflag) const throw()
{
  inflag = compressionflag;
}

// ***************************************************************************
void DOQ_RecImageIFile::getCompressedFileSize(long int& insize) const throw()
{
  insize = compressedsize;
}

// ***************************************************************************
void DOQ_RecImageIFile::getCompressionAlgorithm(std::string& inalgo) 
  const throw()
{
  inalgo = compressionalgorithm;
}

// ***************************************************************************
void DOQ_RecImageIFile::getCompressionType(std::string& intype) const throw()
{
  intype = compressiontype;
}

// ***************************************************************************
void DOQ_RecImageIFile::getCompressionControl(int& incontrol) const throw()
{
  incontrol = compressioncontrol;
}

// ***************************************************************************
// This function creates a palette by scanning the file.  Note that this
// is a slow operation and should be avoided, especially with the color
// DOQ data
void DOQ_RecImageIFile::getPalette(Palette* inpalette)
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
void DOQ_RecImageIFile::getPixel(long x, long y, AbstractPixel* inpixel)
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
void DOQ_RecImageIFile::getScanline(long row, RGBPixel* inarray)
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
void DOQ_RecImageIFile::getScanline(long row, GreyPixel* inarray)
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
void DOQ_RecImageIFile::getRectangle(long x1, long y1, long x2, long y2,
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
void DOQ_RecImageIFile::getRectangle(long x1, long y1, long x2, long y2,
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
void DOQ_RecImageIFile::getRawPixel(long x, long y, unsigned char* inpixel)
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
void DOQ_RecImageIFile::getRawScanline(long row, unsigned char* inarray)
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
void DOQ_RecImageIFile::getRawRectangle(long x1, long y1, long x2, long y2,
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


// ***************************************************************************
// This is a wrapper for the atof function since under unix it doesn't play
// well with D+/- notation
double DOQ_RecImageIFile::myatof(const char* mystr)
{
  int length = strlen(mystr);
  double result;
  char* tempbuffer = new char[length + 1];
  char* foo;

  strcpy(tempbuffer, mystr);

  if ( (foo = strstr(tempbuffer, "D")) != NULL)
    *foo = 'E'; // substitute an E in place of the D
  
  result = atof(tempbuffer);
  delete [] tempbuffer;
  return result;
}

#ifdef _WIN32
#pragma warning( default : 4291 )  // Disable warning messages
#endif

#endif // #ifndef DOQREC..
