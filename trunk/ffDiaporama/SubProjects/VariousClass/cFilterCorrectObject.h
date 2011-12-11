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

#ifndef CFILTERTCORRECTOBJECT_H
#define CFILTERTCORRECTOBJECT_H

#include <QImage>
#include <QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

//*********************************************************************************************************************************************
// Base object for filters image = correction filters
//*********************************************************************************************************************************************

class   cFilterCorrectObject {
public:
    double      X;                      // X position (in %) relative to up/left corner
    double      Y;                      // Y position (in %) relative to up/left corner
    double      ZoomFactor;             // Zoom factor (in %)
    double      ImageRotation;          // Image rotation (in °)
    int         Brightness;             // Brightness adjustment
    int         Contrast;               // Contrast adjustment
    double      Gamma;                  // Gamma adjustment
    int         Red;                    // Red adjustment
    int         Green;                  // Green adjustment
    int         Blue;                   // Blue adjustment
    bool        LockGeometry;           // True if geometry is locked
    double      AspectRatio;            // Aspect Ratio of image
    bool        FullFilling;            // Background image disk only : If true aspect ratio is not keep and image is deformed to fill the frame

    cFilterCorrectObject();

    void        ApplyFilter(QImage *Image);
    void        SaveToXML(QDomElement &domDocument,QString ElementName);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName);

    QImage      *GetImage(QImage *SourceImage,int Width,int Height,double PCTDone,cFilterCorrectObject *PreviousFilter,bool Smoothing);
};

#endif  // CFILTERTCORRECTOBJECT_H
