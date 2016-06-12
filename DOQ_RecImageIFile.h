#ifndef _DOQ_RECIMAGEIFILE_H_
#define _DOQ_RECIMAGEIFILE_H_
// $Id: DOQ_RecImageIFile.h,v 1.1.1.1 2002-02-21 00:19:01 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:01 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This class handles the reading of the 1992 Record format DOQ's.

#include <fstream>
#include <string>
#include "ImageIFile.h"
#include "RawGreyIDataHandler.h"
#include "RawRGBIDataHandler.h"

namespace USGSImageLib
{

class DOQ_RecImageIFile : public ImageIFile
{
 public:
  DOQ_RecImageIFile(std::string& infile)
    throw (ImageException, std::bad_alloc);
  ~DOQ_RecImageIFile();

  // Methods to get header data

  // Record 1
  void getQuadName(std::string& inname)                         const throw();
  void getQuadrant(std::string& inquadrant)                     const throw();
  void getNumNations(int& insize)                               const throw();
  void getNation(std::string& innation, int num)                const throw();
  void getNumStates(int& insize)                                const throw();
  void getState(std::string& instate, int num)                  const throw();
  void getStateCounty(int& incode, int statenum, int countynum) const throw();
  void getFiller1(std::string& infiller)                        const throw();
  void getProducer(std::string& inproducer)                     const throw();
  void getDataOrder(int& inorder)                               const throw();
  void getLines(long int& inlines)                              const throw();
  void getSamples(long int& insamples)                          const throw();
  void getBandTypes(int& intypes)                               const throw();
  void getElevationStorage(int& instorage)                      const throw();
  void getBandElevationStorage(int& instorage)                  const throw();
  void getVerticalDatum(int& indatum)                           const throw();
  void getHorizontalDatum(int& indatum)                         const throw();
  void getSecondaryHorizontalDatum(int& indatum)                const throw();
  void getAngleRotation(double& inangle)                        const throw();
  void getGroundReferenceSystem(int& inrefsys)                  const throw();
  void getCoordinateZone(int& inzone)                           const throw();
  void getHorizontalUnits(int& inunits)                         const throw();
  void getSWQuadX(double& inx)                                  const throw();
  void getSWQuadY(double& iny)                                  const throw();
  void getNWQuadX(double& inx)                                  const throw();
  void getNWQuadY(double& iny)                                  const throw();
  void getNEQuadX(double& inx)                                  const throw();
  void getNEQuadY(double& iny)                                  const throw();
  void getSEQuadX(double& inx)                                  const throw();
  void getSEQuadY(double& iny)                                  const throw();

  // Record 2
  // Transformation parameters
  void getA(double& ina)                   const throw();
  void getB(double& inb)                   const throw();
  void getC(double& inc)                   const throw();
  void getD(double& ind)                   const throw();
  void getE(double& ine)                   const throw();
  void getF(double& inf)                   const throw();
  void getXc(double& inxc)                 const throw();
  void getYc(double& inyc)                 const throw();
  void getSecSWQuadX(double& inx)          const throw();
  void getSecSWQuadY(double& iny)          const throw();
  void getSecNWQuadX(double& inx)          const throw();
  void getSecNWQuadY(double& iny)          const throw();
  void getSecNEQuadX(double& inx)          const throw();
  void getSecNEQuadY(double& iny)          const throw();
  void getSecSEQuadX(double& inx)          const throw();
  void getSecSEQuadY(double& iny)          const throw();
  // Record 3
  void getSecA(double& ina)                const throw();
  void getSecB(double& inb)                const throw();
  void getSecC(double& inc)                const throw();
  void getSecD(double& ind)                const throw();
  void getSecE(double& ine)                const throw();
  void getSecF(double& inf)                const throw();
  void getSecXc(double& inxc)              const throw();
  void getSecYc(double& inyc)              const throw();
  void getSWPixelX(long int& inx)          const throw();
  void getSWPixelY(long int& iny)          const throw();
  void getNWPixelX(long int& inx)          const throw();
  void getNWPixelY(long int& iny)          const throw();
  void getNEPixelX(long int& inx)          const throw();
  void getNEPixelY(long int& iny)          const throw();
  void getSEPixelX(long int& inx)          const throw();
  void getSEPixelY(long int& iny)          const throw();
  void getSecSWPixelX(long int& inx)       const throw();
  void getSecSWPixelY(long int& iny)       const throw();
  void getSecNWPixelX(long int& inx)       const throw();
  void getSecNWPixelY(long int& iny)       const throw();
  void getSecNEPixelX(long int& inx)       const throw();
  void getSecNEPixelY(long int& iny)       const throw();
  void getSecSEPixelX(long int& inx)       const throw();
  void getSecSEPixelY(long int& iny)       const throw();
  void getXOrigin(double& inx)             const throw();
  void getYOrigin(double& iny)             const throw();
  void getSecondaryXOrigin(double& inx)    const throw();
  void getSecondaryYOrigin(double& iny)    const throw();

  // Record 4
  void getElevationUnits(int& inunits)              const throw();
  void getMinimumElevation(double& inelev)          const throw();
  void getMaximumElevation(double& inelev)          const throw();
  void getGroundXRes(double& inres)                 const throw();
  void getGroundYRes(double& inres)                 const throw();
  void getGroundZRes(double& inres)                 const throw();
  void getPixelXRes(double& inres)                  const throw();
  void getPixelYRes(double& inres)                  const throw();
  void getPixelZRes(double& inres)                  const throw();
  void getMaxContourInterval(int& inint)            const throw();
  void getMaxContourUnits(int& inunits)             const throw();
  void getMinContourInterval(int& inint)            const throw();
  void getMinContourUnits(int& inunits)             const throw();
  void getSuspectVoidAreas(int& inint)              const throw();
  void getRMSEX(double& inrmse)                     const throw();
  void getRMSEY(double& inrmse)                     const throw();
  void getNumTestPoints(int& inpoints)              const throw();
  void getPixelProcessingAlgorithm(int& inalgo)     const throw();
  void getProductionSystem(std::string& insys)      const throw();
  void getProductionDate(std::string& indate)       const throw();
  void getFilmType(std::string& intype)             const throw();
  void getSourcePhotoID(std::string& inid)          const throw();
  void getMosaickedImage(int& incode)               const throw();
  void getLeafFlag(std::string& inflag)             const throw();
  void getSourcePhotoDate(std::string& indate)      const throw();
  void getCameraFocalLength(double& inlen)          const throw();
  void getSourceFlightHeight(long int& inheight)    const throw();
  void getScannerType(std::string& intype)          const throw();
  void getScanningXRes(double& inres)               const throw();
  void getScanningYRes(double& inres)               const throw();
  void getScannerXSampleRes(double& inres)          const throw();
  void getScannerYSampleRes(double& inres)          const throw();
  void getRadiometricResolution(int& inres)         const throw();
  void getResampledResolution(double& inres)        const throw();
  void getCompressionFlag(std::string& inflag)      const throw();
  void getCompressedFileSize(long int& insize)      const throw();
  void getCompressionAlgorithm(std::string& inalgo) const throw();
  void getCompressionType(std::string& intype)      const throw();
  void getCompressionControl(int& incontrol)        const throw();

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
  bool readHeader()  throw (ImageException, std::bad_alloc); // to ingest data
  double myatof(const char* mystr); // converts numbers using D+ to floating
  std::ifstream*   Stream;          // input stream we'll use
  RawIDataHandler* datahandler;     // for the helper class
  long int         TopOfImageData;  // Image Data location

  // Record 1 Data
  std::string              quadname;
  std::string              quadrant;
  std::vector<std::string> nations;
  std::vector<std::string> states;
  std::vector<std::string> counties;
  std::string              filler;
  std::string              producer;
  int                      dataorder;
  int                      bandtypes;
  int                      elevationstorage;
  int                      bandelevationstorage;
  int                      verticaldatum;
  int                      horizontaldatum;
  int                      sechorizontaldatum;
  double                   rotationangle;
  int                      groundrefsystem;
  int                      zone;
  int                      planimetricunits;
  double                   SWQuadCorners[2];
  double                   NWQuadCorners[2];
  double                   NEQuadCorners[2];
  double                   SEQuadCorners[2];
  
  // Record 2 Data
  // Projection parameters
  double a, b, c, d, e, f, xc, yc;
  double SWSecQuadCorners[2];
  double NWSecQuadCorners[2];
  double NESecQuadCorners[2];
  double SESecQuadCorners[2];

  // Record 3 Data
  double seca, secb, secc, secd, sece, secf, secxc, secyc;
  long int SWPixelCorners[2];
  long int NWPixelCorners[2];
  long int NEPixelCorners[2];
  long int SEPixelCorners[2];
  long int SWSecPixelCorners[2];
  long int NWSecPixelCorners[2];
  long int NESecPixelCorners[2];
  long int SESecPixelCorners[2];
  double   origins[2];
  double   secorigins[2];

  // Record 4 Data
  int         elevationunits;
  double      minimumelevation;
  double      maximumelevation;
  double      groundres[3];
  double      pixelres[3];
  int         maxcontourinterval;
  int         maxcontourunits;
  int         mincontourinterval;
  int         mincontourunits;
  int         suspectvoids;
  double      rmse[2];
  int         testpointnums;
  int         pixelprocessingalgo;
  std::string productionsystem;
  int         productiondate[3];
  std::string filmtype;
  std::string sourcephotoid;
  int         mosaickedimage;
  std::string leafflag;
  int         sourcephotodate[3];
  double      focallength;
  long int    flightheight;
  std::string scannertype;
  double      scanningres[2];
  double      scannersampleres[2];
  int         radiometricres;
  double      resampledres;
  std::string compressionflag;
  long int    compressedsize;
  std::string compressionalgorithm;
  std::string compressiontype;
  int         compressioncontrol;
};

}

#endif // ifndef
