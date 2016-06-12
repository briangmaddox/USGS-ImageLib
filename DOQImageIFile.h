#ifndef _DOQIMAGEIFILE_H_
#define _DOQIMAGEIFILE_H_
// $Id: DOQImageIFile.h,v 1.1.1.1 2002-02-21 00:18:57 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:57 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class implements the reading of keyword (12/96) Digital OrthoPhoto's.

#include <fstream>
#include <string>
#include <vector>
#include "ImageIFile.h"
#include "RawGreyIDataHandler.h"
#include "RawRGBIDataHandler.h"

namespace USGSImageLib
{
// This class is huge now.  Might split some of it up eventually
class DOQImageIFile : public ImageIFile
{
public:
  DOQImageIFile(std::string& fn) throw(ImageException, std::bad_alloc);
  virtual ~DOQImageIFile();

  // Methods to get at the header data

  // Identification Section
  void getQuadName(std::string& instring)   const throw();
  void getQuadrant(std::string& instring)   const throw();
  void getWestHour(int& inhour)             const throw();
  void getWestMinute(int& inminute)         const throw();
  void getWestSecond(float& insecond)       const throw();
  void getEastHour(int& inhour)             const throw();
  void getEastMinute(int& inminute)         const throw();
  void getEastSecond(float& insecond)       const throw();
  void getNorthHour(int& inhour)            const throw();
  void getNorthMinute(int& inminute)        const throw();
  void getNorthSecond(float& insecond)      const throw();
  void getSouthHour(int& inhour)            const throw();
  void getSouthMinute(int& inminute)        const throw();
  void getSouthSecond(float& insecond)      const throw();
  void getProdDate(std::string& instring)   const throw();

  // Display section
  void getRasterOrder(std::string& instring)            const throw();
  void getBandOrganization(std::string& instring)       const throw();
  void getNumBandContents(int& innumber)                const throw();
  void getBandContent(std::string& instring, int inpos) const throw();
  void getBitsPerPixel(int& inbps)                      const throw();
  void getSamples(long int& insamples)                  const throw();
  void getLines(long int& inlines)                      const throw();

  // Registration Section
  void getHorizontalDatum(std::string& indatum)             const throw();
  void getHorizontalCoordinateSystem(std::string& insystem) const throw();
  void getCoordinateZone(int& inzone)                       const throw();
  void getHorizontalUnits(std::string& inunits)             const throw();
  void getHorizontalResolution(float& inres)                const throw();
  void getSecondaryHorizontalDatum(std::string& indatum)    const throw();
  void getXOrigin(double& inx)                              const throw();
  void getYOrigin(double& iny)                              const throw();
  void getSecondaryXOrigin(double& inx)                     const throw();
  void getSecondaryYOrigin(double& iny)                     const throw();

  // Other Information
  // returns number of nations in storage
  void getNumNations(int& innations)                      const throw();
  void getNation(std::string& innation, int nationnumber) const throw();
  void getNumStates(int& instates)                        const throw();
  void getState(std::string& instate, int statenumber)    const throw();
  void getNWQuadX(double& inx)                            const throw();
  void getNWQuadY(double& iny)                            const throw();
  void getNEQuadX(double& inx)                            const throw();
  void getNEQuadY(double& iny)                            const throw();
  void getSEQuadX(double& inx)                            const throw();
  void getSEQuadY(double& iny)                            const throw();
  void getSWQuadX(double& inx)                            const throw();
  void getSWQuadY(double& iny)                            const throw();

  // Secondary Corner Point Values
  void getSecNWQuadX(double& inx)  const throw();
  void getSecNWQuadY(double& iny)  const throw();
  void getSecNEQuadX(double& inx)  const throw();
  void getSecNEQuadY(double& iny)  const throw();
  void getSecSEQuadX(double& inx)  const throw();
  void getSecSEQuadY(double& iny)  const throw();
  void getSecSWQuadX(double& inx)  const throw();
  void getSecSWQuadY(double& iny)  const throw();


  void getRMSE(float& inrmse)                                const throw();
  void getImageSource(std::string& insource)                 const throw();
  void getNumImageSources(int& insources)                    const throw();
  void getSourceImageID(std::string& inid, int numimage)     const throw();
  void getSourceImageDate(std::string& indate, int numimage) const throw();
  void getSourceDEMDate(std::string& indate)                 const throw();
  void getAgency(std::string& inagency)                      const throw();
  void getProducer(std::string& inproducer)                  const throw();
  void getProductionSystem(std::string& insystem)            const throw();
  void getCompression(std::string& incompression)            const throw();
  void getStandardVersion(std::string& inversion)            const throw();
  void getMetaDataDate(std::string& indate)                  const throw();
  void getDataSize(long int& insize)                         const throw();
  void getByteCount(long int& incount)                       const throw();

  // Standard ImageIFile interface

  // This function allocates memory to the user
  virtual void getPalette(Palette* inpalette)
    throw(ImageException, std::bad_alloc);

  // All of these functions assume column-major order, ie, (col, row)
  // These functions assume the caller has allocated memory to pass in.  
  virtual void getScanline(long int row, GreyPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getScanline(long int row, RGBPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getPixel(long int x, long int y, AbstractPixel* inpixel)
    throw(ImageException, std::bad_alloc);
  virtual void getRectangle(long int x1, long int y1, long int x2, long int y2,
                            RGBPixel* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRectangle(long int x1, long int y1, long int x2, long int y2,
                            GreyPixel* inarray)
    throw(ImageException, std::bad_alloc);

  virtual void getRawScanline(long int row, unsigned char* inarray)
    throw(ImageException, std::bad_alloc);
  virtual void getRawPixel(long int x, long int y, unsigned char* inpixel)
    throw(ImageException, std::bad_alloc);
  virtual void getRawRectangle(long int x1, long int y1,
                               long int x2, long int y2,
                               unsigned char* inarray)
    throw(ImageException, std::bad_alloc);

protected:
  std::ifstream*   Stream;          // input stream we'll be using
  RawIDataHandler* datahandler;     // helper class to actually read the data
  long int         TopOfImageData;  // File location - image data beginning

  char         buffer[80];
  //  char*        ret;
  char*        position; // position if found in buffer

  // Identification Section
  std::string quadName;
  std::string quadrant;
  // neatlines
  int         westHour;
  int         westMinute;
  float       westSecond;
  int         eastHour;
  int         eastMinute;
  float       eastSecond;
  int         northHour;
  int         northMinute;
  float       northSecond;
  int         southHour;
  int         southMinute;
  float       southSecond;

  std::string prodDate;  // production date
  
  // Display Section
  std::string              rasterOrder;       // left/right, top/bottom
  std::string              bandOrganization;  // single, bip, bsq, bil
  std::vector<std::string> bandContent;

  // Registration Section
  std::string horizontalDatum;
  std::string horizCoordSystem; // 0 = UTM, 1 = state plane, 2 = geographic
  int         coordinateZone;
  std::string horizontalUnits;
  float       horizontalResolution;
  std::string secHorizDatum;
  double      xOrigin;
  double      yOrigin;
  double      secxOrigin;
  double      secyOrigin;

  // Other Information
  std::vector<std::string> nations;
  std::vector<std::string> states;

  double      NWQuadCornerX;
  double      NWQuadCornerY;
  double      NEQuadCornerX;
  double      NEQuadCornerY;
  double      SEQuadCornerX;
  double      SEQuadCornerY;
  double      SWQuadCornerX;
  double      SWQuadCornerY;
  // Secondary corner point coordinates
  double      SNWQuadCornerX;
  double      SNWQuadCornerY;
  double      SNEQuadCornerX;
  double      SNEQuadCornerY;
  double      SSEQuadCornerX;
  double      SSEQuadCornerY;
  double      SSWQuadCornerX;
  double      SSWQuadCornerY;
  
  float         rmseXY; // Root Mean Square error
  std::string   imageSource;

  std::vector<std::string> sourceImageID;
  std::vector<std::string> sourceImageDate;

  std::string   sourceDEMDate;
  std::string   agency;
  std::string   producer;
  std::string   productionSystem;
  std::string   compression;
  std::string   standardVersion;
  std::string   metaDataDate;
  long int      dataSize;
  long int      byteCount;


  bool readBuffer()  throw(ImageException); // Read buffer from disk
  int  returnToken() throw();               // ID tag and return token


  // set to true if the triplets for color images are in R,G,B order
  bool RGBOrdered;

  // Methods to set data.  Note:  These functions are called after the 
  // whichToken method is called.  Once it returns the token, the parser
  // will call these functions and they will take the buffer and parse out
  // what they need accordingly 

  // Identification section
  bool setQuadName()      throw();
  bool setQuadrant()      throw();
  bool setWestLongitude() throw();
  bool setEastLongitude() throw();
  bool setNorthLatitude() throw();
  bool setSouthLatitude() throw();
  bool setProdDate()      throw();

  // Display Section
  bool setRasterOrder()      throw();
  bool setBandOrganization() throw();
  bool setBandContent()      throw();
  bool setBitsPerPixel()     throw();
  bool setSamplesLines()     throw();

  // Registration Section
  bool setHorizontalDatum()            throw();
  bool setHorizontalCoordinateSystem() throw();
  bool setCoordinateZone()             throw();
  bool setHorizontalUnits()            throw();
  bool setHorizontalResolution()       throw();
  bool setSecondaryHorizontalDatum()   throw();
  bool setOrigins()                    throw();  // sets both X and Y
  bool setSecondaryOrigins()           throw();  // sets both X and Y

  // Other Information
  bool setNation()        throw();
  bool setStates()        throw();
  bool setNWQuadCorners() throw();
  bool setNEQuadCorners() throw();
  bool setSEQuadCorners() throw();
  bool setSWQuadCorners() throw();

  // Secondary Corner Point Coordinates
  bool setSecondaryNWQuadCorner() throw();
  bool setSecondaryNEQuadCorner() throw();
  bool setSecondarySEQuadCorner() throw();
  bool setSecondarySWQuadCorner() throw();

  bool setRMSE()             throw();
  bool setImageSource()      throw();
  bool setSourceImageID()    throw();
  bool setSourceImageDate()  throw();
  bool setSourceDEMDate()    throw();
  bool setAgency()           throw();
  bool setProducer()         throw();
  bool setProductionSystem() throw();
  bool setCompression()      throw();
  bool setStandardVersion()  throw();
  bool setMetaDataDate()     throw();
  bool setDataSize()         throw();
  bool setByteCount()        throw();


  bool readHeader() throw(); // function to read in the DOQ header
};

} // namespace
#endif

