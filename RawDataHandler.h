#ifndef _RAWDATAHANDLER_H_
#define _RAWDATAHANDLER_H_
// $Id: RawDataHandler.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// 21 May 1998
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "ImageCommon.h"

namespace USGSImageLib
{

class RawDataHandler  
{
public:
  RawDataHandler(long int inwidth = 0, long int inheight = 0,
                 int inphotometric = PHOTO_UNKNOWN) throw();
  virtual ~RawDataHandler();
  
  // Functions to set the data members
  bool setPhotometric(int inphotometric) throw();
  bool setWidth(long int inwidth)        throw();
  bool setHeight(long int inheight)      throw();
  bool setBitsPerSample(int inbits)      throw();


  // Functions to get the data members
  void getPhotometric(int& inphotometric) const throw();
  void getWidth(long int& inwidth)        const throw();
  void getHeight(long int& inheight)      const throw();
  void getBitsPerSample(int& inbits)            throw();

protected:
  long int Height;
  long int Width;
  long int ByteWidth; // so the functions don't have to keep calculating this,
                      // This is the actual width of the image in bytes, taking
                      // into account the bytes per pixel
  int Photometric;    // type of image, IE, PHOTO_RGB

  int BitsPerSample;  // number of bits in each sample
  int BytesPerPixel;  // number of samples that make up each pixel
  
  // bounds checking
  bool validateRow(long int& row)           const throw(); 
  bool validateXY(long int& x, long int& y) const throw();

  // This guy just calculates the bytesperpixel and also changes bytewidth
  // if bytes per pixel changes....
  bool setBytesPerPixel() throw();

private:

};

// *************************************************************************
inline
void RawDataHandler::getHeight(long int& inheight) const throw()
{
  inheight = Height;
}

// ***************************************************************************
inline
void RawDataHandler::getWidth(long int& inwidth) const throw()
{
  inwidth = Width;
}

// ***************************************************************************
inline
void RawDataHandler::getPhotometric(int& inphotometric) const throw()
{
  inphotometric = Photometric;
}

// ***************************************************************************
inline
bool RawDataHandler::setBitsPerSample(int inbits) throw()
{
  if (inbits > 32)
    return false;
  else
    BitsPerSample = inbits;

  return true;
}

// ***************************************************************************
inline
void RawDataHandler::getBitsPerSample(int& inbits) throw()
{
  inbits = BitsPerSample;
}

} // namespace

#endif // !defined(RAWDATAHANDLER_H)



