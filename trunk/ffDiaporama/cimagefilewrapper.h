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

#ifndef CIMAGEFILEWRAPPER_H
#define CIMAGEFILEWRAPPER_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QImage>
#include "capplicationconfig.h"

class cDiaporamaObject;

class cimagefilewrapper {
public:
    bool                IsValide;               // if true then object if fuly initialise
    QString             FileName;               // filename
    QDateTime           CreatDateTime;          // Original date/time
    QDateTime           ModifDateTime;          // Last modified date/time
    int                 ImageWidth;             // Widht of normal image
    int                 ImageHeight;            // Height of normal image
    int                 ImageOrientation;       // Image orientation (EXIF)
    QImage              *CacheImage;            // Cache image (Preview mode only)

    QStringList         ExivValue;

    cimagefilewrapper();
    ~cimagefilewrapper();

    bool    GetInformationFromFile(cApplicationConfig *ApplicationConfig,QString GivenFileName);
    QImage  *ImageAt(bool PreviewMode,int PreviewMaxHeight,bool ForceLoadDisk);
};

#endif // CIMAGEFILEWRAPPER_H
