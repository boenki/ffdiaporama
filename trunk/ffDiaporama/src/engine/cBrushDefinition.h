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

// Define possible values for images type
#define IMAGETYPE_UNKNOWN                   0
#define IMAGETYPE_PHOTOLANDSCAPE            1
#define IMAGETYPE_PHOTOPORTRAIT             2
#define IMAGETYPE_CLIPARTLANDSCAPE          3   // Cliparts are images less than 1920x1080
#define IMAGETYPE_CLIPARTPORTRAIT           4
#define IMAGETYPE_VIDEOLANDSCAPE            5
#define IMAGETYPE_VIDEOPORTRAIT             6
#define NBR_IMAGETYPE                       7

// Brush Auto Framing definition
#define AUTOFRAMING_CUSTOMUNLOCK            0
#define AUTOFRAMING_CUSTOMLOCK              1
#define AUTOFRAMING_CUSTOMIMGLOCK           2
#define AUTOFRAMING_CUSTOMPRJLOCK           3
#define AUTOFRAMING_FULLMAX                 4
#define AUTOFRAMING_FULLMIN                 5
#define AUTOFRAMING_HEIGHTLEFTMAX           6
#define AUTOFRAMING_HEIGHTLEFTMIN           7
#define AUTOFRAMING_HEIGHTMIDLEMAX          8
#define AUTOFRAMING_HEIGHTMIDLEMIN          9
#define AUTOFRAMING_HEIGHTRIGHTMAX          10
#define AUTOFRAMING_HEIGHTRIGHTMIN          11
#define AUTOFRAMING_WIDTHTOPMAX             12
#define AUTOFRAMING_WIDTHTOPMIN             13
#define AUTOFRAMING_WIDTHMIDLEMAX           14
#define AUTOFRAMING_WIDTHMIDLEMIN           15
#define AUTOFRAMING_WIDTHBOTTOMMAX          16
#define AUTOFRAMING_WIDTHBOTTOMMIN          17
#define NBR_AUTOFRAMING                     18

#define AUTOFRAMING_ICON_GEOMETRY_LOCKED    ":/img/Geometry_Lock.png"
#define AUTOFRAMING_ICON_GEOMETRY_UNLOCKED  ":/img/Geometry_Unlock.png"
#define AUTOFRAMING_ICON_GEOMETRY_PROJECT   ":/img/Geometry_ProjectLock.png"
#define AUTOFRAMING_ICON_GEOMETRY_IMAGE     ":/img/Geometry_ImageLock.png"

#define AUTOFRAMING_GEOMETRY_CUSTOM         0
#define AUTOFRAMING_GEOMETRY_PROJECT        1
#define AUTOFRAMING_GEOMETRY_IMAGE          2

//============================================

#define AUTOCOMPOSIZE_CUSTOM                0
#define AUTOCOMPOSIZE_REALSIZE              1
#define AUTOCOMPOSIZE_FULLSCREEN            2
#define AUTOCOMPOSIZE_TVMARGINS             3
#define AUTOCOMPOSIZE_TWOTHIRDSSCREEN       4
#define AUTOCOMPOSIZE_HALFSCREEN            5
#define AUTOCOMPOSIZE_THIRDSCREEN           6
#define AUTOCOMPOSIZE_QUARTERSCREEN         7

//============================================
// OnOffFilter mask definition

#define FilterEqualize                      0x0001
#define FilterDespeckle                     0x0002
#define FilterGray                          0x0004
#define FilterNegative                      0x0008
#define FilterEmboss                        0x0010
#define FilterEdge                          0x0020
#define FilterAntialias                     0x0040
#define FilterNormalize                     0x0080
#define FilterCharcoal                      0x0100
#define FilterOil                           0x0200

//============================================
// Shape definitions
//============================================

enum SHAPEFORM_ID {
    SHAPEFORM_NOSHAPE,
    SHAPEFORM_RECTANGLE,        SHAPEFORM_ROUNDRECT,        SHAPEFORM_BUBBLE,           SHAPEFORM_ELLIPSE,
    SHAPEFORM_TRIANGLEUP,       SHAPEFORM_TRIANGLERIGHT,    SHAPEFORM_TRIANGLEDOWN,     SHAPEFORM_TRIANGLELEFT,     SHAPEFORM_RHOMBUS,          SHAPEFORM_PENTAGON,         SHAPEFORM_HEXAGON,          SHAPEFORM_OCTOGON,
    SHAPEFORM_SIMPLEARROWUP,    SHAPEFORM_SIMPLEARROWRIGHT, SHAPEFORM_SIMPLEARROWDOWN,  SHAPEFORM_SIMPLEARROWLEFT,  SHAPEFORM_HEART,            SHAPEFORM_PUZZLEUL,         SHAPEFORM_PUZZLEUC,         SHAPEFORM_PUZZLEUR,
    SHAPEFORM_DOUBLEARROWVERT,  SHAPEFORM_DOUBLEARROWHORIZ, SHAPEFORM_DOUBLEARROWDIAG1, SHAPEFORM_DOUBLEARROWDIAG2, SHAPEFORM_SPADE,            SHAPEFORM_PUZZLEML,         SHAPEFORM_PUZZLEMC,         SHAPEFORM_PUZZLEMR,
    SHAPEFORM_RIGHTTRIANGLEUL,  SHAPEFORM_RIGHTTRIANGLEUR,  SHAPEFORM_RIGHTTRIANGLEDL,  SHAPEFORM_RIGHTTRIANGLEDR,  SHAPEFORM_CLUB,             SHAPEFORM_PUZZLEDL,         SHAPEFORM_PUZZLEDC,         SHAPEFORM_PUZZLEDR,
    SHAPEFORM_STRIANGLEUP,      SHAPEFORM_STRIANGLERIGHT,   SHAPEFORM_STRIANGLEDOWN,    SHAPEFORM_STRIANGLELEFT,    SHAPEFORM_PUSHEDUP,         SHAPEFORM_SHARPDOWN,        SHAPEFORM_STAR4,            SHAPEFORM_STAR5,
    SHAPEFORM_DTRIANGLEUP,      SHAPEFORM_DTRIANGLERIGHT,   SHAPEFORM_DTRIANGLEDOWN,    SHAPEFORM_DTRIANGLELEFT,    SHAPEFORM_DCHEVRONUP,       SHAPEFORM_SCHEVRONDOWN,     SHAPEFORM_STAR6,            SHAPEFORM_STAR8,
    SHAPEFORM_PUSHEDLEFT,       SHAPEFORM_DCHEVRONLEFT,     SHAPEFORM_SCHEVRONLEFT,     SHAPEFORM_SHARPLEFT,        SHAPEFORM_SCHEVRONUP,       SHAPEFORM_DCHEVRONDOWN,     SHAPEFORM_GEAR6,            SHAPEFORM_GEAR8,
    SHAPEFORM_SHARPRIGHT,       SHAPEFORM_SCHEVRONRIGHT,    SHAPEFORM_DCHEVRONRIGHT,    SHAPEFORM_PUSHEDRIGHT,      SHAPEFORM_SHARPUP,          SHAPEFORM_PUSHEDDOWN,       SHAPEFORM_GEAR10,           SHAPEFORM_GEAR12,
    NBR_SHAPEFORM                   // Last of the list !
};

class cShapeFormDefinition {
public:
    bool    Enable;
    QString Name;
    cShapeFormDefinition() {}
    cShapeFormDefinition(bool Enable,QString Name);
};

extern QList<cShapeFormDefinition> ShapeFormDefinition;
void   ShapeFormDefinitionInit();  // Utility function to init shape collection and translate shape names

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

//============================================
// Shape computations
//============================================

// Utilities functions to compute a polygon for a given form
QList<QPolygonF>    ComputePolygon(int BackgroundForm,qreal left,qreal top,qreal width,qreal height,qreal CenterX,qreal CenterY);
QRectF              PolygonToRectF(QList<QPolygonF> Polygon);

// Utilities functions to compute shape
QList<QPolygonF> ComputePolygonRect(QRectF Rect);
QList<QPolygonF> ComputePolygonRoundRect(QRectF Rect,qreal RayX,qreal RayY);
QList<QPolygonF> ComputePolygonEllipse(QRectF Rect);
QList<QPolygonF> ComputeHeart(QRectF Rect);
QList<QPolygonF> ComputeSpade(QRectF Rect);
QList<QPolygonF> ComputeClub(QRectF Rect);
QList<QPolygonF> ComputePolygonR(double width,double height,double CenterX,double CenterY,int MaxPoint,double StartAngle);
QList<QPolygonF> ComputeSingleChevron(QRectF Rect,int Angle,int Options);
QList<QPolygonF> ComputeDoubleChevron(QRectF Rect,int Angle);
QList<QPolygonF> ComputeSingleTriangle(QRectF Rect,int Angle);
QList<QPolygonF> ComputeDoubleTriangle(QRectF Rect,int Angle);
QList<QPolygonF> ComputeSimpleArrow(QRectF Rect,int Angle);
QList<QPolygonF> ComputeDoubleArrow(QRectF Rect,int Angle);
QList<QPolygonF> ComputeStar(QRectF Rect,int Pointed);
QList<QPolygonF> ComputeGear(QRectF Rect,int tooth);
QList<QPolygonF> ComputeRightTriangle(QRectF Rect,int Angle);
QList<QPolygonF> ComputePuzzle(QRectF Rect,int Forme);

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
    double                  WaveAmp,WaveFreq;           // Filter parameters

    int                     OnOffFilter;                // On-Off filter = combination of Despeckle, Equalize, Gray and Negative;

    // Link to global objects
    cBaseApplicationConfig  *ApplicationConfig;
    cBackgroundList         *BackgroundList;

                            cBrushDefinition(cBaseApplicationConfig *TheApplicationConfig,cBackgroundList *TheBackgroundList);
                            ~cBrushDefinition();

    void                    CopyFromBrushDefinition(cBrushDefinition *BrushToCopy);
    void                    SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool                    LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag);
    QBrush                  *GetBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache=false);

    int                     GetImageType();

    // Image correction part
    void                    ApplyFilter(QImage *Image);

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
    QBrush                  *GetImageDiskBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache);
    int                     GetHeightForWidth(int WantedWith,QRectF Rect);
    int                     GetWidthForHeight(int WantedHeight,QRectF Rect);
};

#endif // CBRUSHDEFINITION_H
