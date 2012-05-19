/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2012 Dominique Levray <levray.dominique@bbox.fr>

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

#ifndef CBRUSHDEFINITION_H
#define CBRUSHDEFINITION_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QRectF>
#include <QPainter>
#include <QImage>
#include <QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

// Include some common various class
#include "cFilterTransformObject.h"
#include "cLuLoImageCache.h"
#include "cBaseMediaFile.h"
#include "cSoundBlockList.h"

// Brush type definition
#define BRUSHTYPE_NOBRUSH                   0
#define BRUSHTYPE_SOLID                     1
#define BRUSHTYPE_PATTERN                   2
#define BRUSHTYPE_GRADIENT2                 3
#define BRUSHTYPE_GRADIENT3                 4
#define BRUSHTYPE_IMAGELIBRARY              5
#define BRUSHTYPE_IMAGEDISK                 6

// Brush Gradient orientation
#define GRADIENTORIENTATION_UPLEFT          0
#define GRADIENTORIENTATION_UP              1
#define GRADIENTORIENTATION_UPRIGHT         2
#define GRADIENTORIENTATION_LEFT            3
#define GRADIENTORIENTATION_RADIAL          4
#define GRADIENTORIENTATION_RIGHT           5
#define GRADIENTORIENTATION_BOTTOMLEFT      6
#define GRADIENTORIENTATION_BOTTOM          7
#define GRADIENTORIENTATION_BOTTOMRIGHT     8

// Composition type definition
#define COMPOSITIONTYPE_BACKGROUND          0
#define COMPOSITIONTYPE_OBJECT              1
#define COMPOSITIONTYPE_SHOT                2

// Utility function to create a gradient brush
QBrush *GetGradientBrush(QRectF Rect,int BrushType,int GradientOrientation,QString ColorD,QString ColorF,QString ColorIntermed,double Intermediate);

// Utility function to draw a shape
void    DrawShape(QPainter &Painter,int BackgroundForm,double left,double top,double width,double height,double CenterX,double CenterY);

// Utility function to draw a regular polygon shape
void    DrawPolygonR(QPainter &Painter,double width,double height,double CenterX,double CenterY,int MaxPoint,double StartAngle);

//*********************************************************************************************************************************************
// Base object for background library object
//*********************************************************************************************************************************************

class   cBackgroundObject {
public:
    bool        IsValide;
    QString     FilePath;
    QString     Name;
    QPixmap     Icon;
    int         Geometry;
    QImage      BackgroundImage;

    cBackgroundObject(QString FileName,int Geometry);

    void    SetGeometry(int Geometry);
};

//*********************************************************************************************************************************************
// Global class containing background library
//*********************************************************************************************************************************************

class   cBackgroundList {
public:
    int                         Geometry;
    QList<cBackgroundObject>    List;                       // list of brush

    cBackgroundList();

    void    ScanDisk(QString Path,int Geometry);
    int     SearchImage(QString NameToFind);
};

//*********************************************************************************************************************************************
// Base object for brush object
//*********************************************************************************************************************************************

class cBrushDefinition {
public:
    int                     TypeComposition;        // Type of composition object (COMPOSITIONTYPE_BACKGROUND, COMPOSITIONTYPE_OBJECT, COMPOSITIONTYPE_SHOT)
    int                     BrushType;              // 0=no brush, 1=Solid, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
    int                     PatternType;            // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    int                     GradientOrientation;    // 0=Radial, 1->4=Linear from a corner, 5->9=Linear from a border
    QString                 ColorD;                 // First Color
    QString                 ColorF;                 // Last Color
    QString                 ColorIntermed;          // Intermediate Color
    double                  Intermediate;           // Intermediate position of 2nd color (in %) for gradient 3 colors
    QString                 BrushImage;             // Image name if image from library
    double                  SoundVolume;            // Volume of soundtrack
    cImageFile              *Image;                 // Embeded Object for title and image type
    cVideoFile              *Video;                 // Embeded Object for video type
    QString                 DefaultFramingW;        // Default Framing when ADJUST_WITH
    QString                 DefaultFramingH;        // Default Framing when ADJUST_HEIGHT
    QString                 DefaultFramingF;        // Default Framing when ADJUST_FULL

    // Image correction part
    double                  X;                      // X position (in %) relative to up/left corner
    double                  Y;                      // Y position (in %) relative to up/left corner
    double                  ZoomFactor;             // Zoom factor (in %)
    double                  ImageRotation;          // Image rotation (in °)
    int                     Brightness;             // Brightness adjustment
    int                     Contrast;               // Contrast adjustment
    double                  Gamma;                  // Gamma adjustment
    int                     Red;                    // Red adjustment
    int                     Green;                  // Green adjustment
    int                     Blue;                   // Blue adjustment
    bool                    LockGeometry;           // True if geometry is locked
    double                  AspectRatio;            // Aspect Ratio of image
    bool                    FullFilling;            // Background image disk only : If true aspect ratio is not keep and image is deformed to fill the frame

    // Link to global objects
    cBaseApplicationConfig  *ApplicationConfig;
    cBackgroundList         *BackgroundList;

                            cBrushDefinition(cBaseApplicationConfig *TheApplicationConfig,cBackgroundList *TheBackgroundList);
                            ~cBrushDefinition();

    void                    CopyFromBrushDefinition(cBrushDefinition *BrushToCopy);
    void                    SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool                    LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag);
    QBrush                  *GetBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache=false);

    enum FramingType {ADJUST_WITH,ADJUST_HEIGHT,ADJUST_FULL};
    void                    GetDefaultFraming(FramingType TheFramingType,bool LockGeometry,double &X,double &Y,double &ZoomFactor,double &AspectRatio);
    void                    InitDefaultFramingStyle(bool LockGeometry,double AspectRatio);
    QString                 GetFramingStyle(double X,double Y,double ZoomFactor,double AspectRatio,double ImageRotation);
    QString                 GetFramingStyle();
    void                    ApplyStyle(bool LockGeometry,QString Style);

    // Image correction part
    void                    ApplyFilter(QImage *Image);


private:
    QBrush                  *GetLibraryBrush(QRectF Rect);
    QBrush                  *GetImageDiskBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache);
    int                     GetHeightForWidth(int WantedWith,QRectF Rect);
    int                     GetWidthForHeight(int WantedHeight,QRectF Rect);
};

#endif // CBRUSHDEFINITION_H
