/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011 Dominique Levray <levray.dominique@bbox.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
   ====================================================================== */

#ifndef _cLuLoImageCACHE_H
#define _cLuLoImageCACHE_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

//===================================================

class cLuLoImageCacheObject {
public:
    QString FileName;                               // filename
    QImage  *CacheImage;                            // Cache image (Preview mode)
    QImage  *CacheFullImage;                        // Cache image (Full image mode)
    QImage  *UnfilteredImage;                       // Cache image (Preview image with no filter)
    int     ImageOrientation;                       // Image orientation (EXIF)

    cLuLoImageCacheObject(QString FileName);
    ~cLuLoImageCacheObject();

    void    ClearAll();
    void    ClearCacheAndCacheFull();

    QImage *ValidateUnfilteredImage();
    QImage *ValidateCacheFullImage(cFilterTransformObject *Filter=NULL);
    QImage *ValidateCacheImage(cFilterTransformObject *Filter=NULL);
};

//===================================================

class cLuLoImageCache {
public:
    QList<cLuLoImageCacheObject *>    List;           // Fifo list

    cLuLoImageCache();

    void                    FreeMemoryToMaxValue(int MaxValue);
    int                     MemoryUsed();
    cLuLoImageCacheObject   *FindObject(QString FileName,bool SetAtTop=true);   // Find object corresponding to FileName
    QImage                  *GetCacheImage(QString FileName);                   // Cache image (Preview mode)
    QImage                  *GetCacheFullImage(QString FileName);               // Cache image (Full image mode)
    QImage                  *GetUnfilteredImage(QString FileName);              // Cache image (Preview image with no filter)
};

#endif // _cLuLoImageCACHE_H
