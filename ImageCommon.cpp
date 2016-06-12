#ifndef _IMAGECOMMON_CPP_
#define _IMAGECOMMON_CPP_
// $Id: ImageCommon.cpp,v 1.3 2003-06-28 16:28:40 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:40 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Implementation of aux functions that the ImageLib needs

#include "ImageCommon.h"
#include <ctype.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <sstream>

namespace USGSImageLib
{

// Define ImageVersion here. BGM 1 February 2003
const std::string ImageVersion = "3.0";

// *************************************************************************
// Borrowed this function from Stroustrup's "The C++ Programming Language"
int cmp_nocase(const std::string& s, const std::string& s2)
{
  std::string::const_iterator p = s.begin();
  std::string::const_iterator p2 = s2.begin();
  
  while ( (p != s.end()) && (p2 != s2.end()) )
  {
    if (toupper(*p) != toupper(*p2))
      return ( (toupper(*p) < toupper(*p2)) ? -1 : 1);
    ++p;
    ++p2;
  }
  // size is unsigned
  return ( (s2.size() == s.size()) ? 0 : (s.size() < s2.size()) ? -1 : 1);
}

// ***************************************************************************
// Search on the keyword.  Note that this assumes that the keyword is at the
// beginning of the line (kinda like it's supposed to be) 
bool search_key(const std::string& s, char* inchar) throw()
{
  std::istringstream is(s); // Construst a stringstream based on the buffer
  std::string keyword;

  is >> keyword; // grab the keyword at the beginning of the line.  This
                 // should ignore whitespace so we should be ok here

  if (keyword == inchar)
    return true;

  return false;
}

// **************************************************************************
double MAX(double x, double y)
{
  if (x >= y)
    return x;
  else
    return y;
}


// ***************************************************************************
double MIN(double x, double y)
{
  if (x < y)
    return x;
  else
    return y;
}


// ***************************************************************************
void RGBtoHSV(unsigned short inred, unsigned short ingreen,
              unsigned short inblue, 
              float& inhue, float& insat, float& invalue)
{
  float mymin, mymax, delta;
  
  mymin = MIN(MIN(inred, ingreen), inblue);
  mymax = MAX(MAX(inred, ingreen), inblue);
  invalue = mymax;

  delta = mymax - mymin;

  if (mymax != 0.0)
    insat = delta / mymax;
  else
  {
    insat = 0.0;
    inhue = -1;
    return;
  }
  
  if (inred == mymax)
    inhue = (ingreen - inblue) / delta; // between yellow and magenta
  else if (ingreen == mymax)          
    inhue = 2 + (inblue - inred) / delta;  // between cyan and yellow
  else
    inhue = 4 + (inred - ingreen) / delta; // between magenta & cyan
  
  inhue *= 60;
  if (inhue < 0)
    inhue += 360;  // degrees
  
  return;
}


// ***************************************************************************
// Also shamelessly stolen from Foley
void HSVtoRGB(unsigned short& inred, unsigned short& ingreen,
              unsigned short& inblue,
              float inhue, float invalue, float insat)
{
  int i;
  float f, p, q, t;

  if (insat == 0.0) // achromatic (grey)
  {
    inred = ingreen = inblue = static_cast<unsigned short>(invalue);
    return;
  }

  inhue /= 60.0; // sector 0 to 5
  i = static_cast<int>(floor(inhue));
  f = inhue - i; // factorial part of inhue
  p = invalue * (1 - insat);
  q = invalue * (1 - insat * f);
  t = invalue * (1 - insat * (1 - f) );
  
  switch (i)
  {
  case 0:
    inred = static_cast<unsigned short>(invalue);
    ingreen = static_cast<unsigned short>(t);
    inblue = static_cast<unsigned short>(p);
    break;
  case 1:
    inred = static_cast<unsigned short>(q);
    ingreen = static_cast<unsigned short>(invalue);
    inblue = static_cast<unsigned short>(p);
    break;
  case 2:
    inred = static_cast<unsigned short>(p);
    ingreen = static_cast<unsigned short>(invalue);
    inblue = static_cast<unsigned short>(t);
    break;
  case 3:
    inred = static_cast<unsigned short>(p);
    ingreen = static_cast<unsigned short>(q);
    inblue = static_cast<unsigned short>(invalue);
    break;
  case 4:
    inred = static_cast<unsigned short>(t);
    ingreen = static_cast<unsigned short>(p);
    inblue = static_cast<unsigned short>(invalue);
    break;
  default:
    inred = static_cast<unsigned short>(invalue);
    ingreen = static_cast<unsigned short>(p);
    inblue = static_cast<unsigned short>(q);
    break;
  }

  return;
}

} // namespace
#endif
