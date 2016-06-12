#ifndef _IMAGE_COMMON_H_
#define _IMAGE_COMMON_H_
// $Id: ImageCommon.h,v 1.2 2003-06-28 16:28:40 bmaddox Exp $
// Brian Maddox - bmaddox@usgs.gov
// 21 May 1998
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:40 $
// This file defines some common constants for use by the Image Library

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "ImageException.h"
#include <string>

namespace USGSImageLib
{
// These constants are used to identify the photometric of the image
#define PHOTO_BILEVEL    0
#define PHOTO_GREY       1
#define PHOTO_GRAY       1
#define PHOTO_RGB        2
#define PHOTO_RGB_BIP    2 // Band Interleaved by Pixel
#define PHOTO_RGB_BIL    3 // Band Interleaved by Line
#define PHOTO_RGB_BSQ    4 // Band Sequential
#define PHOTO_HSV        5
#define PHOTO_HLS        6
#define PHOTO_CMY        7
#define PHOTO_YIQ        8
#define PHOTO_PALETTE    9
#define PHOTO_MINISBLACK 1
#define PHOTO_MINISWHITE 10 
#define PHOTO_UNKNOWN    255

// These constants define the image type.
#define IMAGE_RGB     0
#define IMAGE_GREY    1
#define IMAGE_TIFF    2
#define IMAGE_DGP     3
#define IMAGE_CRL     4
#define IMAGE_DOQ     5
#define IMAGE_GIF     6
#define IMAGE_PBM     7
#define IMAGE_XBM     8
#define IMAGE_BMP     9
#define IMAGE_PCX     10 
#define IMAGE_JPEG    11 
#define IMAGE_GTIF    12
#define IMAGE_PNG     13
#define IMAGE_ERROR   254
#define IMAGE_UNKNOWN 255 

// Heh, fixed this since I wasn't paying attention and didn't declare the
// global var properly here BGM 1 February 2003
extern const std::string ImageVersion;

//  Struct RawRGBPixel for efficiency enhancements.
struct RawRGBPixel
{
   unsigned char Red;
   unsigned char Green;
   unsigned char Blue;
};

// Some aux functions we need
// Perform a case insensitive string comparison
int cmp_nocase(const std::string& s, const std::string& s2);
// Search the input string for the keyword given in inchar
bool search_key(const std::string& s, char* inchar) throw();

// For consistency's sake
double MAX(double x, double y);
double MIN(double x, double y);

// Functions taken from Foley to convert between RGB and HSV colorspaces
void RGBtoHSV(unsigned short inred, unsigned short ingreen,
	      unsigned short inblue, 
	      float& inhue, float& insat, float& invalue);
void HSVtoRGB(unsigned short& inred, unsigned short& ingreen,
	      unsigned short& inblue,
	      float inhue, float invalue, float insat);
 
} // namespace

#endif // #ifndef
