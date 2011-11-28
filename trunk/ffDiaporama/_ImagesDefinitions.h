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


#ifndef IMAGESDEFINITIONS_H
#define IMAGESDEFINITIONS_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Utility function to create a gradient brush
QBrush *GetGradientBrush(QRectF Rect,int BrushType,int GradientOrientation,QString ColorD,QString ColorF,QString ColorIntermed,double Intermediate);

//*********************************************************************************************************************************************
// Base object for filters image = transform filters
//*********************************************************************************************************************************************

class   cFilterTransformObject {
public:
    double   BlurSigma;
    double   BlurRadius;
    int      OnOffFilter;                // On-Off filter = combination of Despeckle, Equalize, Gray and Negative;

    cFilterTransformObject();

    void        ApplyFilter(QImage *Image);
    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath);
    QString     FilterToString();
};

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
    bool        Smoothing;              // Use Qt::Smooth instead of Qt::Fast
    bool        FullFilling;            // Background image disk only : If true aspect ratio is not keep and image is deformed to fill the frame

    cFilterCorrectObject();

    void        ApplyFilter(QImage *Image);
    void        SaveToXML(QDomElement &domDocument,QString ElementName);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName);

    QImage      *GetImage(QImage *SourceImage,int Width,int Height,double PCTDone,cFilterCorrectObject *PreviousFilter);
};

//*********************************************************************************************************************************************
// Base object for brush object
//*********************************************************************************************************************************************

class cimagefilewrapper;
class cvideofilewrapper;

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
    cFilterCorrectObject    BrushFileCorrect;       // Image correction if image from disk
    cimagefilewrapper       *Image;                 // Embeded Object for title and image type
    cvideofilewrapper       *Video;                 // Embeded Object for video type
    QString                 DefaultFramingW;        // Default Framing when ADJUST_WITH
    QString                 DefaultFramingH;        // Default Framing when ADJUST_HEIGHT
    QString                 DefaultFramingF;        // Default Framing when ADJUST_FULL

    cBrushDefinition();
    ~cBrushDefinition();
    void        CopyFromBrushDefinition(cBrushDefinition *BrushToCopy);
    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList &AliasList);
    QBrush      *GetBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush);

    enum FramingType {ADJUST_WITH,ADJUST_HEIGHT,ADJUST_FULL};
    void        GetDefaultFraming(FramingType TheFramingType,bool LockGeometry,double &X,double &Y,double &ZoomFactor,double &AspectRatio);
    void        InitDefaultFramingStyle(bool LockGeometry,double AspectRatio);
    QString     GetFramingStyle(double X,double Y,double ZoomFactor,double AspectRatio,double ImageRotation);
    QString     GetFramingStyle();
    void        ApplyStyle(bool LockGeometry,QString Style);

private:
    QBrush      *GetLibraryBrush(QRectF Rect);
    QBrush      *GetImageDiskBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush);
    int         GetHeightForWidth(int WantedWith,QRectF Rect);
    int         GetWidthForHeight(int WantedHeight,QRectF Rect);
};

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
    ~cBackgroundList();

    void    ScanDisk(QString Path,int Geometry);
    int     SearchImage(QString NameToFind);
};

//*********************************************************************************************************************************************
// Global class containing icons of transitions
//*********************************************************************************************************************************************

class cIconObject {
public:
    QImage  Icon;                       // The icon
    int     TransitionFamilly;          // Transition familly
    int     TransitionSubType;          // Transition type in the familly

    cIconObject(int TransitionFamilly,int TransitionSubType);
    cIconObject(int TransitionFamilly,int TransitionSubType,QImage LumaImage);
    ~cIconObject();
};

//*********************************************************************************************************************************************
// Global class containing icons library
//*********************************************************************************************************************************************

class   cIconList {
public:
    QList<cIconObject>  List;                       // list of icons

    cIconList();
    ~cIconList();

    QImage *GetIcon(int TransitionFamilly,int TransitionSubType);
};

//*********************************************************************************************************************************************
// Global class containing luma library
//*********************************************************************************************************************************************

class   cLumaListObject {
public:
    QImage  OriginalLuma;
    QImage  DlgLumaImage;
    QString Name;

    cLumaListObject(QString FileName);
    ~cLumaListObject();
};

class   cLumaList {
public:
    int                     Geometry;
    QList<cLumaListObject>  List;                       // list of Luma

    cLumaList();
    ~cLumaList();

    void    ScanDisk(QString Path,int TransitionFamilly);
    void    SetGeometry(int Geometry);
};

// static global values
extern  cBackgroundList BackgroundList;                         // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Bar;                           // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Box;                           // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Center;                        // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Checker;                       // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Clock;                         // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Snake;                         // Real definition in _ImagesDefinitions.cpp
extern  cIconList       IconList;                               // Real definition in _ImagesDefinitions.cpp

// static local values use to work with luma images
#define     LUMADLG_HEIGHT  80
extern int  LUMADLG_WIDTH;                                      // Real definition in _ImagesDefinitions.cpp

#endif // IMAGESDEFINITIONS_H
