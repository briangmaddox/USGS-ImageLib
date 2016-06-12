Summary: USGS C++ Image Library
Name: ImageLib
%define ver    0.8.0
%define 
%define rel    1
%define 
%define prefix /usr/local
Version: 0.8.0
Release: 1
Copyright: LGPL
Group: Libraries
Source0: http://www.fidnet.com/~bmaddox/ImageLib.tar.gz
Url: http://www.usgs.gov
Packager: Brian G. Maddox
Excludeos: i386

%description
This library contains a series of C++ classes designed to provide a common interface for reading/writing various raster image file formats.  Supported formats currently are CRL, TIFF, GeoTIFF, raw grey/RGB data, and USGS Digital Orthophoto's (keyword and record).



%prep
rm -rf $RPM_BUILD_DIR/ImageLib
zcat $RPM_SOURCE_DIR/ImageLib.tar.gz |tar -xvf -

%setup


%build
configure --prefix=/usr/local
make


%install
make install


%clean
rm -rf $RPM_BUILD_ROOT


%files
%doc README
/usr/local/include/ImageLib/AbstractPixel.h
/usr/local/include/ImageLib/GreyImageIFile.h
/usr/local/include/ImageLib/ImageFile.h
/usr/local/include/ImageLib/RGBPixel.h
/usr/local/include/ImageLib/SPCSMapper.h
/usr/local/include/ImageLib/CRLImageIFile.h
/usr/local/include/ImageLib/GreyPalette.h
/usr/local/include/ImageLib/ImageIFile.h
/usr/local/include/ImageLib/RawDataHandler.h
/usr/local/include/ImageLib/TIFFImageIFile.h
/usr/local/include/ImageLib/CacheManager.h
/usr/local/include/ImageLib/GreyPixel.h
/usr/local/include/ImageLib/ImageOFile.h
/usr/local/include/ImageLib/RawGreyIDataHandler.h
/usr/local/include/ImageLib/TIFFImageOFile.h
/usr/local/include/ImageLib/DOQImageIFile.h
/usr/local/include/ImageLib/HSVPixel.h
/usr/local/include/ImageLib/ImageProjection.h
/usr/local/include/ImageLib/RawGreyODataHandler.h
/usr/local/include/ImageLib/UTMN.h
/usr/local/include/ImageLib/DOQ_RecImageIFile.h
/usr/local/include/ImageLib/ImageCommon.h
/usr/local/include/ImageLib/JFIFImageIFile.h
/usr/local/include/ImageLib/RawIDataHandler.h
/usr/local/include/ImageLib/GeoTIFFImageIFile.h
/usr/local/include/ImageLib/ImageData.h
/usr/local/include/ImageLib/Palette.h
/usr/local/include/ImageLib/RawODataHandler.h
/usr/local/include/ImageLib/GeoTIFFImageOFile.h
/usr/local/include/ImageLib/ImageException.h
/usr/local/include/ImageLib/RGBPalette.h
/usr/local/include/ImageLib/RawRGBIDataHandler.h
/usr/local/lib/libImageLib.a

