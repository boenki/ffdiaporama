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
#include "_SpeedWave.h"
#include "_Shape.h"
#include "cLuLoImageCache.h"
#include "cBaseMediaFile.h"
#include "cSoundBlockList.h"

//============================================
// Auto Framing
//============================================

class cAutoFramingDef {
public:
    QString     ToolTip;
    int         GeometryType;
    bool        IsInternal;

    cAutoFramingDef() {}
    cAutoFramingDef(QString ToolTip,bool IsInternal,int GeometryType);
};
extern cAutoFramingDef AUTOFRAMINGDEF[NBR_AUTOFRAMING];
void   AutoFramingDefInit();

//============================================
// Brush
//============================================

extern  QBrush  Transparent;    // Transparent brush
QBrush  *GetGradientBrush(QRectF Rect,int BrushType,int GradientOrientation,QString ColorD,QString ColorF,QString ColorIntermed,double Intermediate);

//*********************************************************************************************************************************************
// Base object for background library object
//*********************************************************************************************************************************************

class   cBackgroundObject {
public:
    bool                    IsValide;
    QString                 FilePath;
    QDateTime               ModifDateTime;
    cBaseApplicationConfig *ApplicationConfig;
    QString                 Name;

    cBackgroundObject(QString FileName,cBaseApplicationConfig *ApplicationConfig);

    QImage* GetBackgroundImage();
    QImage  GetBackgroundThumb(int Geometry);

private:
    int         CurrentGeometry;
    QImage      Thumbnail[3];
};

//*********************************************************************************************************************************************
// Global class containing background library
//*********************************************************************************************************************************************

class   cBackgroundList {
public:
    QList<cBackgroundObject> List;                       // list of brush

    cBackgroundList();

    void    ScanDisk(QString Path,cBaseApplicationConfig *ApplicationConfig);
    int     SearchImage(QString NameToFind);
};
extern  cBackgroundList BackgroundList;

//*********************************************************************************************************************************************
// Base object for brush object
//*********************************************************************************************************************************************

class cBrushDefinition {
public:
    int                     TypeComposition;            // Type of composition object (COMPOSITIONTYPE_BACKGROUND, COMPOSITIONTYPE_OBJECT, COMPOSITIONTYPE_SHOT)
    int                     BrushType;                  // 0=no brush, 1=Solid, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
    int                     PatternType;                // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    int                     GradientOrientation;        // 0=Radial, 1->4=Linear from a corner, 5->9=Linear from a border
    QString                 ColorD;                     // First Color
    QString                 ColorF;                     // Last Color
    QString                 ColorIntermed;              // Intermediate Color
    double                  Intermediate;               // Intermediate position of 2nd color (in %) for gradient 3 colors
    QString                 BrushImage;                 // Image name if image from library
    double                  SoundVolume;                // Volume of soundtrack
    bool                    Deinterlace;                // Add a YADIF filter to deinterlace video (on/off)
    cImageFile              *Image;                     // Embeded Object for title and image type
    cVideoFile              *Video;                     // Embeded Object for video type

    // Image correction part
    double                  X;                          // X position (in %) relative to up/left corner
    double                  Y;                          // Y position (in %) relative to up/left corner
    double                  ZoomFactor;                 // Zoom factor (in %)
    double                  ImageRotation;              // Image rotation (in °)
    int                     Brightness;                 // Brightness adjustment
    int                     Contrast;                   // Contrast adjustment
    double                  Gamma;                      // Gamma adjustment
    int                     Red;                        // Red adjustment
    int                     Green;                      // Green adjustment
    int                     Blue;                       // Blue adjustment
    bool                    LockGeometry;               // True if geometry is locked
    double                  AspectRatio;                // Aspect Ratio of image
    bool                    FullFilling;                // Background image disk only : If true aspect ratio is not keep and image is deformed to fill the frame

    double                  GaussBlurSharpenSigma,BlurSharpenRadius;       // Blur/Sharpen parameters
    int                     TypeBlurSharpen,QuickBlurSharpenSigma;
    double                  Desat,Swirl,Implode;        // Filter parameters

    int                     OnOffFilter;                // On-Off filter = combination of Despeckle, Equalize, Gray and Negative;
    int                     ImageSpeedWave;

    // Link to global objects
    cBaseApplicationConfig  *ApplicationConfig;
    cBackgroundList         *BackgroundList;

                            cBrushDefinition(cBaseApplicationConfig *TheApplicationConfig,cBackgroundList *TheBackgroundList);
                            ~cBrushDefinition();

    void                    InitDefaultValues();

    void                    CopyFromBrushDefinition(cBrushDefinition *BrushToCopy);
    void                    SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool                    LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag);
    QBrush                  *GetBrush(QRectF Rect,bool PreviewMode,int Position,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush);

    int                     GetImageType();

    // Image correction part
    QImage                  ApplyFilter(QImage Image);
    QImage                  ApplyFilters(QImage NewRenderImage,double TheBrightness,double TheContrast,double TheGamma,double TheRed,double TheGreen,double TheBlue,double TheDesat,bool ProgressifOnOffFilter,cBrushDefinition *PreviousBrush,double PctDone);


    // Utility functions used to draw thumbnails of image
    QImage                  *ImageToWorkspace(QImage *SrcImage,int WantedSize,qreal &maxw,qreal &maxh,qreal &minw,qreal &minh,qreal &x1,qreal &x2,qreal &x3,qreal &x4,qreal &y1,qreal &y2,qreal &y3,qreal &y4);
    void                    ApplyMaskToImageToWorkspace(QImage *SrcImage,QRectF CurSelRect,int BackgroundForm,int AutoFramingStyle=-1);
    void                    ApplyMaskToImageToWorkspace(QImage *SrcImage,int AutoFramingStyle,int BackgroundForm,int WantedSize,qreal maxw,qreal maxh,qreal minw,qreal minh,qreal X,qreal Y,qreal ZoomFactor,qreal AspectRatio,qreal ProjectGeometry);

    // Utility functions used to define default framing
    int                     GetCurrentFramingStyle(qreal ProjectGeometry);
    bool                    CalcWorkspace(qreal &dmax,qreal &maxw,qreal &maxh,qreal &minw,qreal &minh,qreal &x1,qreal &x2,qreal &x3,qreal &x4,qreal &y1,qreal &y2,qreal &y3,qreal &y4);
    void                    ApplyAutoFraming(int AutoAdjust,qreal ProjectGeometry);
    void                    s_AdjustWTop(qreal ProjectGeometry);
    void                    s_AdjustWMidle(qreal ProjectGeometry);
    void                    s_AdjustWBottom(qreal ProjectGeometry);
    void                    s_AdjustHLeft(qreal ProjectGeometry);
    void                    s_AdjustHMidle(qreal ProjectGeometry);
    void                    s_AdjustHRight(qreal ProjectGeometry);
    void                    s_AdjustWH();
    void                    s_AdjustMinWTop(qreal ProjectGeometry);
    void                    s_AdjustMinWMidle(qreal ProjectGeometry);
    void                    s_AdjustMinWBottom(qreal ProjectGeometry);
    void                    s_AdjustMinHLeft(qreal ProjectGeometry);
    void                    s_AdjustMinHMidle(qreal ProjectGeometry);
    void                    s_AdjustMinHRight(qreal ProjectGeometry);
    void                    s_AdjustMinWH();

private:
    QBrush                  *GetLibraryBrush(QRectF Rect);
    QBrush                  *GetImageDiskBrush(QRectF Rect,bool PreviewMode,int64_t Position,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush);
    int                     GetHeightForWidth(int WantedWith,QRectF Rect);
    int                     GetWidthForHeight(int WantedHeight,QRectF Rect);
};

#endif // CBRUSHDEFINITION_H
