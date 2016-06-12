#ifndef _IMAGEEXCEPTION_H_
#define _IMAGEEXCEPTION_H_
// $Id: ImageException.h,v 1.1.1.1 2002-02-21 00:18:59 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last Modified by $Author: bmaddox $ on $Date: 2002-02-21 00:18:59 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

// This is the ImageException class.  This class handles exception handling in
// the Image library. Note, in most cases, the actual generated exception is
// caught and dealt with, then the Image Library specific one thrown

#include <exception>
#include <string>
#include <new>

namespace USGSImageLib
{
// Define some common error types we might encounter.
#define IMAGE_MEM_ALLOC_ERR    0
#define IMAGE_FILE_OPEN_ERR    1
#define IMAGE_FILE_CLOSE_ERR   2
#define IMAGE_FILE_READ_ERR    3
#define IMAGE_FILE_WRITE_ERR   4
#define IMAGE_BOUNDING_ERR     5
#define IMAGE_INVALID_OP_ERR   6
#define IMAGE_INVALID_TYPE_ERR 7
#define IMAGE_UNPACK_ERR       8
#define IMAGE_UNKNOWN_ERR      255


class ImageException
{
public:
  ImageException(short int inexception) throw();
  ImageException() throw();
  virtual ~ImageException();

  // in case you want to change it
  bool setException(short int inexception)          throw();
  void getException(short int& inexception )  const throw();
  void getString(std::string& instring)       const throw();

protected:
  short int myexception;  // the actual exception passed in
};

} // namespace
#endif 
