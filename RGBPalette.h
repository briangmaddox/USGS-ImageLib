#ifndef _RGBPALETTE_H_
#define _RGBPALETTE_H_
// $Id: RGBPalette.h,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This is the definition of the RGBPalette class, which stores a palette
// based on RGB values

#include <stdlib.h>
#include <vector>
#include "Palette.h"
#include "RGBPixel.h"

namespace USGSImageLib
{

class RGBPalette : public Palette
{
public:
  RGBPalette() throw();
  RGBPalette(long int n, RGBPixel* m = 0)    throw(std::bad_alloc);
  RGBPalette(std::vector<RGBPixel>& invector) throw(std::bad_alloc);
  virtual ~RGBPalette();

  // These are the new functions which allow the user to pass in memory instead
  // of having it dynamically allocated and returned to them
  bool getEntry(long int n, RGBPixel& inpixel) const throw();
  bool setEntry(long int n, RGBPixel& inpixel)       throw();

  // Allows the user to set the full palette at a later time, for use with 
  // the default constructor
  bool setPalette(long int n, RGBPixel* inarray) throw(ImageException);

  RGBPixel operator[](long int n) const throw();
  
  /** added this so that if a class changes the number of entries then
      setNoEntries will actually resize the number of entries in the class.
      CBB 6/20/2001
   **/
  void setNoEntries(long int n) throw();

private:
  std::vector<RGBPixel> Map;
};

} // namespace
#endif
