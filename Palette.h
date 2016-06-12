#ifndef _PALLETE_H_
#define _PALLETE_H_
// $Id: Palette.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "ImageException.h"

namespace USGSImageLib
{
class Palette
{
 public:
  Palette()      throw();
  Palette(int n) throw();
  virtual ~Palette();

  int  getNoEntries()           const throw();
  /** Changed this so that if a class changes the number of entries then 
   *  setNoEntries will actually resize the number of entries in the class.
   *  CBB 6/20/2001 **/
  virtual void setNoEntries(long int n) throw();

 protected:
  long int NoEntries;
};

// *************************************************************************
inline int Palette::getNoEntries() const throw()
{
  return NoEntries;
}

// ***************************************************************************
inline void Palette::setNoEntries(long int n) throw()
{
  NoEntries = n;
}

} // namespace
#endif 
