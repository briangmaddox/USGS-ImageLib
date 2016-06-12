#ifndef _RGBPALETTE_CPP_
#define _RGBPALETTE_CPP_
// $Id: RGBPalette.cpp,v 1.2 2003-06-28 16:28:41 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2003-06-28 16:28:41 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This is the implementation for the RGBPalette class

#include "RGBPalette.h"
#include "ImageCommon.h"

using namespace USGSImageLib;

// **************************************************************************
RGBPalette::RGBPalette(void) throw()
  : Palette(0)
{
}


// ***************************************************************************
RGBPalette::RGBPalette(long int n, RGBPixel* m) throw(std::bad_alloc)
  : Palette(n)
{
  long int i;
  unsigned short red, green, blue; // colors to pass in
  RGBPixel temp; // temp pixel to add to the vector

  if (m)
  { 
    NoEntries = n;
 
    for (i = 0; i < NoEntries; i++)
    {
      m[i].getRed(red);
      m[i].getGreen(green);
      m[i].getBlue(blue);
      temp.setRed(red); temp.setGreen(green); temp.setBlue(blue);
      Map.push_back(temp);
    }
  }
  else
  {
    //Added to resize the vector when array passed in is NULL
    Map.resize(n);
  }
}


// ***************************************************************************
RGBPalette::RGBPalette(std::vector<RGBPixel>& invector) throw(std::bad_alloc)
  : Palette(invector.size())
{
  long int loopcount;
  long int invector_size = invector.size();
  unsigned short red, green, blue;
  RGBPixel temp;

  NoEntries = 0;

  if (invector_size != 0) // if vector empty, don't bother
  {
    for (loopcount = 0; loopcount < invector_size; loopcount++)
    {
      invector[loopcount].getRed(red);
      invector[loopcount].getGreen(green);
      invector[loopcount].getBlue(blue);
      temp.setRed(red); temp.setGreen(green); temp.setBlue(blue);
      Map.push_back(temp);
      NoEntries++;
    }
  }
}

// ***************************************************************************
RGBPalette::~RGBPalette()
{
}


// ***************************************************************************
bool RGBPalette::getEntry(long int n, RGBPixel& inpixel) const throw()
{
  if ( (n < 0) || (n >= NoEntries))
    return false;

  unsigned short red, green, blue; // go ahead and alloc vars

  try
  {
    // get the temp values
    Map[n].getRed(red);
    Map[n].getGreen(green);
    Map[n].getBlue(blue);
    
    // set the input values
    inpixel.setRed(red);
    inpixel.setGreen(green);
    inpixel.setBlue(blue);
    
    return true;
  }
  catch (...)
  {
    return false;
  }
}


// ***************************************************************************
bool RGBPalette::setEntry(long int n, RGBPixel& inpixel) throw()
{
  if (n < 0)
    return false;

  unsigned short red, green, blue;
  long int newsize;
  // Heh, oops.  Since the vector is zero based, we need to add one element
  // so that it gets sized right.  Ie, it's empty, user wants element 1, we
  // size it by one, but vector[1] doesn't exist.  BGM 19 May 2001
  newsize = n + 1;

  try
  {
    // If the current palette isn't big enough to hold this, resize it 
    if (NoEntries < newsize)
    {
      Map.resize(newsize);
      NoEntries = newsize;
    }
    
    inpixel.getRed(red);
    inpixel.getGreen(green);
    inpixel.getBlue(blue);
    
    Map[n].setRed(red);
    Map[n].setGreen(green);
    Map[n].setBlue(blue);
    
    return true;
  }
  catch (...)
  {
    return false;
  }
}

// ***************************************************************************
bool RGBPalette::setPalette(long int n, RGBPixel* inarray)
  throw (ImageException)
{
  // Only use this if the palette hasn't been set previously
  if ( (n < 0) || (NoEntries != 0) )
    throw ImageException(IMAGE_INVALID_OP_ERR);

  long int loopcount;
  unsigned short red, green, blue;

  try
  {
    NoEntries = n;
    Map.resize(n);

    for (loopcount = 0; loopcount < n; loopcount++)
    {
      inarray[loopcount].getRed(red);
      inarray[loopcount].getGreen(green);
      inarray[loopcount].getBlue(blue);
      Map[loopcount].setRed(red);
      Map[loopcount].setGreen(green);
      Map[loopcount].setBlue(blue);
    }
    
    return true; 
  }
  catch (...)
  {
    throw ImageException(IMAGE_FILE_WRITE_ERR);
  }
}


// ***************************************************************************
RGBPixel RGBPalette::operator[](long int n) const throw()
{
  return Map[n];
}

// ***************************************************************************
void RGBPalette::setNoEntries(long int n) throw()
{
  Palette::setNoEntries(n);
  Map.resize(n);
}

#endif

