/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

// Include some additional standard class
#include <QString>
#include <QImage>
#include <QList>

// Include some common various class
#include "cCustomIcon.h"

#define PREVIEWMAXHEIGHT    720         // Max height for preview image

//===================================================

#define LULOOBJECT_IMAGE    0
#define LULOOBJECT_VIDEO    1

class cLuLoImageCache;

class cLuLoImageCacheObject {
public:
    int             TypeObject;                             // One of the LULOOBJECT_ type
    QString         FileName;                               // Filename
    QDateTime       ModifDateTime;
    bool            Smoothing;                              // Smoothing
    QImage          *CachePreviewImage;                     // Cache image (Preview mode)
    QImage          *CacheRenderImage;                      // Cache image (Full image mode)
    QString         FilterString;                           // Filter string                    [For LULOOBJECT_IMAGE]
    int             ImageOrientation;                       // Image orientation (EXIF)         [For LULOOBJECT_IMAGE]
    int             Position;                               // Position in video                [For LULOOBJECT_VIDEO]
    cCustomIcon     *Video;                                 // Video                            [For LULOOBJECT_VIDEO]
    cLuLoImageCache *LuLoImageCache;                        // Link to parent LuLoImageCache collection

    // Constructor for image file
    cLuLoImageCacheObject(QString FileName,QDateTime ModifDateTime,int ImageOrientation,QString FilterString,bool Smoothing,cLuLoImageCache *Parent);

    // Constructor for video image
    cLuLoImageCacheObject(cCustomIcon *Video,int Position,bool Smoothing,cLuLoImageCache *Parent);

    ~cLuLoImageCacheObject();

    QImage      *ValidateCacheRenderImage();
    QImage      *ValidateCachePreviewImage();
};

//===================================================

class cLuLoImageCache {
public:
    QList<cLuLoImageCacheObject *>  List;           // Fifo list
    qlonglong                       MaxValue;       // Max memory used

    cLuLoImageCache();
    ~cLuLoImageCache();

    void                    FreeMemoryToMaxValue();
    qlonglong               MemoryUsed();

    // Find image object corresponding to FileName and filter
    cLuLoImageCacheObject   *FindObject(QString FileName,QDateTime ModifDateTime,int ImageOrientation,bool Smoothing,bool SetAtTop);

    // Find video image object corresponding to FileName and position
    cLuLoImageCacheObject   *FindObject(cCustomIcon *Video,int Position,bool Smoothing,bool SetAtTop);

    // Special case for slide dialog : Remove all object  of this name
    void                    RemoveVideoObject(QString FileName);
    void                    RemoveImageObject(QString FileName);
};

#endif // _cLuLoImageCACHE_H
