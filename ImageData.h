#ifndef _IMAGEDATA_H_
#define _IMAGEDATA_H_
// $Id: ImageData.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - UGSS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Defines the base Image Data class
// This is the base class for all of the different Image objects.  It holds
// various image information such as photometric, bitspersample, ect

#include "AbstractPixel.h"
#include "Palette.h"
#include "ImageCommon.h"

namespace USGSImageLib
{

class ImageData
{
public:
  ImageData() throw(ImageException, std::bad_alloc);
  ImageData(long int width, long int height) 
    throw(ImageException, std::bad_alloc);
  virtual ~ImageData();
  
  // Functions to return the state of the object
  bool good()        const throw();
  bool bad()         const throw();
  bool noData()      const throw();
  bool failedRead()  const throw();
  bool failedWrite() const throw();
  void clear()             throw();

  bool hasPalette()                                const throw();
  void getWidth(long int& inwidth)                 const throw();
  void getHeight(long int& inheight)               const throw();
  void getBitsPerSample(int& inbps)                const throw();
  void getSamplesPerPixel(int& insps)              const throw();
  void getPhotometric(int& inphotometric)          const throw();
  void getImageType(int& intype)                   const throw();
  void setWidth(long int inwidth)                        throw();
  void setHeight(long int inheight)                      throw();
  void setNoDataBit()                                    throw();
  void setFailRBit()                                     throw();
  void setFailWBit()                                     throw();
  void setImageType(int intype)                          throw();
  void setHasPalette()                                   throw();
  void unsetHasPalette()                                 throw();

  // Allow these to be overloaded since certain types might need to
  // These are allowed to return a boolean value as some of the child classes
  // may need this functionality BGM 20 Oct 2000
  virtual bool setBitsPerSample(int)   throw(ImageException);
  virtual bool setSamplesPerPixel(int) throw(ImageException);
  virtual bool setPhotometric(int p)   throw();



protected:
  long int Width;
  long int Height;
  int      bitsPerSample;
  int      samplesPerPixel;
  int      Photometric;  // Uses values in ImageCommon.h
  int      ImageType;  // DOQ, GIF, RGB, ect

  enum IDState
  {
    goodbit = 0,
    nodatabit = 1,
    failrbit = 2,
    failwbit = 4
  };

  unsigned int State;
  bool _hasPalette;
};

// ***************************************************************************
inline
void ImageData::setWidth(long int inwidth) throw()
{
  Width = inwidth;
}

// ***************************************************************************
inline
void ImageData::setHeight(long int inheight) throw()
{
  Height = inheight;
}

// ***************************************************************************
inline
bool ImageData::setBitsPerSample(int b) throw(ImageException)
{
  bitsPerSample = b;

  return true;
}

// ***************************************************************************
inline
bool ImageData::setSamplesPerPixel(int s) throw(ImageException)
{
  samplesPerPixel = s;

  return true;
}

// ***************************************************************************
inline
bool ImageData::setPhotometric(int p) throw()
{
  Photometric = p;

  return true;
}

// ***************************************************************************
inline
void ImageData::setImageType(int intype) throw()
{
  ImageType = intype;
}

// ***************************************************************************
inline
void ImageData::getWidth(long int& inwidth) const throw()
{
  inwidth = Width;
}

// ***************************************************************************
inline
void ImageData::getHeight(long int& inheight) const throw()
{
  inheight = Height;
}

// ***************************************************************************
inline
void ImageData::getBitsPerSample(int& inbps) const throw()
{
  inbps = bitsPerSample;
}

// ***************************************************************************
inline
void ImageData::getSamplesPerPixel(int& insps) const throw()
{
  insps = samplesPerPixel;
}

// ***************************************************************************
inline
void ImageData::getPhotometric(int& inphotometric) const throw()
{
  inphotometric = Photometric;
}

// ***************************************************************************
inline
void ImageData::getImageType(int& intype) const throw()
{
  intype = ImageType;
}

// ***************************************************************************
inline
void ImageData::setHasPalette() throw()
{
  _hasPalette = true;
}

// ***************************************************************************
inline
void ImageData::unsetHasPalette() throw()
{
  _hasPalette = false;
}

// ***************************************************************************
inline
bool ImageData::hasPalette() const throw()
{
  return _hasPalette;
}

// ***************************************************************************
inline
bool ImageData::good() const throw()
{
  return (State == goodbit);
}

// ***************************************************************************
inline
bool ImageData::bad() const throw()
{
  return (State != goodbit);
}

// ***************************************************************************
inline
bool ImageData::noData() const throw()
{
  return ((State & nodatabit) != 0);
}

// ***************************************************************************
inline
bool ImageData::failedRead() const throw()
{
  return ((State & failrbit) != 0);
}

// ***************************************************************************
inline
bool ImageData::failedWrite() const throw()
{
  return ((State & failwbit) != 0);
}

// ***************************************************************************
inline
void ImageData::clear() throw()
{
  State = goodbit;
}

// ***************************************************************************
inline
void ImageData::setNoDataBit() throw()
{
  State = State | nodatabit;
}

// ***************************************************************************
inline
void ImageData::setFailRBit() throw()
{
  State = State | failrbit;
}

// ***************************************************************************
inline
void ImageData::setFailWBit() throw()
{
  State = State | failwbit;
}

} // namespace
#endif
