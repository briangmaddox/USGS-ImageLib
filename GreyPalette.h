#ifndef _GREYPALETTE_H_
#define _GREYPALETTE_H_
// $Id: GreyPalette.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This is the GreyPalette class.  It contains methods and functions to
// set and get entries for a greyscale palette

#include <vector>
#include "GreyPixel.h"
#include "Palette.h"

namespace USGSImageLib
{
class GreyPalette : public Palette
{
public:
  GreyPalette() throw();
  GreyPalette(long int n, GreyPixel* m = 0) 
    throw(ImageException, std::bad_alloc);
  GreyPalette(std::vector<GreyPixel>& invector)
    throw(ImageException, std::bad_alloc);
  virtual ~GreyPalette();
  
  bool getEntry (long int n, GreyPixel& inpixel) const throw();
  bool setEntry (long int n, GreyPixel& inpixel) throw();

  // Allow the user to set the full palette at a later time
  bool setPalette(long int n, GreyPixel* inarray) throw(ImageException);

  GreyPixel operator[](long int n) const throw();
  
  /** added this so that if a class changes the number of entries then
      setNoEntries will actually resize the number of entries in the class.
      CBB 6/20/2001 **/
  void setNoEntries(long int n) throw();

protected:
  std::vector<GreyPixel> Map;

};

} // namespace
#endif
