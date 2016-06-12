#ifndef _GREYPALETTE_CPP_
#define _GREYPALETTE_CPP_
// $Id: GreyPalette.cpp,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// Implements member functions for the class GreyPalette 

#include "GreyPalette.h"
#include "ImageCommon.h"

using namespace USGSImageLib;

GreyPalette::GreyPalette() throw()
  : Palette(0)
{
}


// ***************************************************************************
GreyPalette::GreyPalette(long int n, GreyPixel* m) 
  throw(ImageException, std::bad_alloc)
    : Palette(n)
{
  long int i;
  unsigned short greyvalue;
  GreyPixel temp;

  if (m) // Only process if a non-NULL palette was passed in
  {
    NoEntries = n;

    for (i = 0; i < NoEntries; i++)
    {
      m[i].getGrey(greyvalue);
      temp.setGrey(greyvalue);
      Map.push_back(temp);
    }
  }
  else
  {
    //Resize the vector
    Map.resize(n);
  }
}


// ***************************************************************************
GreyPalette::GreyPalette(std::vector<GreyPixel>& invector) 
  throw(ImageException, std::bad_alloc)
    : Palette(invector.size())
{
  long int myloop;
  long int invector_size = invector.size();
  unsigned short greyvalue;
  GreyPixel temp;
  
  NoEntries = 0;

  if (invector_size != 0) // Again, don't do anything if the vector is empty
  {
    for (myloop = 0; myloop < invector_size; myloop++)
    {
      invector[myloop].getGrey(greyvalue);
      temp.setGrey(greyvalue);
      Map.push_back(temp);
      NoEntries++;
    }
  }
}
  
  
// ***************************************************************************
GreyPalette::~GreyPalette()
{
}


// ***************************************************************************
bool GreyPalette::getEntry(long int n, GreyPixel& inpixel) const throw()
{
  unsigned short greyvalue;

  if ( (n < 0) || (n > NoEntries) )
    return false;
  else
  {
    Map[n].getGrey(greyvalue);
    inpixel.setGrey(greyvalue);
  }
  return true;
}


// ***************************************************************************
bool GreyPalette::setEntry(long int n, GreyPixel& inpixel) throw()
{
  unsigned short greyvalue;
  long int newsize;

  if (n < 0)
    return false;
  
  // Again, we need to make sure we handle the zero based array stuff here
  // BGM 19 May 2001
  newsize = n + 1;

  try
  {
    if (NoEntries < newsize)
    {
      Map.resize(newsize);
      NoEntries = newsize;
    }
    inpixel.getGrey(greyvalue);
    Map[n].setGrey(greyvalue);
    
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool GreyPalette::setPalette(long int n, GreyPixel* inarray)
  throw (ImageException)
{
  // Only use this if the palette hasn't been set previously
  if ( (n < 0) || (NoEntries != 0) )
    throw ImageException(IMAGE_INVALID_OP_ERR);

  long int loopcount;
  unsigned short greyval;

  try
  {
    NoEntries = n;
    Map.resize(n);

    for (loopcount = 0; loopcount < n; loopcount++)
    {
      inarray[loopcount].getGrey(greyval);
      Map[loopcount].setGrey(greyval);
    }
  
    return true;
    
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}
 
  
// ***************************************************************************
GreyPixel GreyPalette::operator[](long int n) const throw()
{
  return Map[n];
}

// **************************************************************************
void GreyPalette::setNoEntries(long int n) throw()
{
  Palette::setNoEntries(n);
  Map.resize(n);
}    

#endif // #ifndef GREYPalette
