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

#ifndef IMAGEFILEWRAPPER_H
#define IMAGEFILEWRAPPER_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

class cimagefilewrapper {
public:
    bool                    IsValide;               // if true then object if fuly initialise
    QString                 FileName;               // filename
    QDateTime               CreatDateTime;          // Original date/time
    QDateTime               ModifDateTime;          // Last modified date/time
    int                     ImageWidth;             // Widht of normal image
    int                     ImageHeight;            // Height of normal image
    int                     ImageOrientation;       // Image orientation (EXIF)
    QImage                  *CacheImage;            // Cache image (Preview mode)
    QImage                  *CacheFullImage;        // Cache image (Full image mode)
    QImage                  *UnfilteredImage;       // Cache image (Preview image with no filter)
    cFilterTransformObject  BrushFileTransform;     // Image transformation if image from disk

    QStringList             ExivValue;

    cimagefilewrapper();
    ~cimagefilewrapper();

    bool    GetInformationFromFile(QString GivenFileName,QStringList &AliasList);
    QImage  *ImageAt(bool PreviewMode,bool ForceLoadDisk,cFilterTransformObject *Filter);
    bool    CallEXIF();
};

#endif // IMAGEFILEWRAPPER_H
