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

#ifndef CDIAPORAMA_H
#define CDIAPORAMA_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"
#include "_Transition.h"

// Include some additional standard class
#include "cBaseApplicationConfig.h"
#include "cBaseMediaFile.h"
#include "cBrushDefinition.h"
#include "cTextFrame.h"
#include "_SDL_Support.h"

// Specific inclusions
class cCompositionList;
class cCompositionObject;
class cDiaporamaShot;
class cDiaporamaObjectInfo;
class cDiaporama;
class cDiaporamaObject;

// Shot type definition
#define SHOTTYPE_STATIC                     0
#define SHOTTYPE_MOBIL                      1
#define SHOTTYPE_VIDEO                      2

//============================================
// Block animations
//============================================

#define BLOCKANIMTYPE_NONE                  0
#define BLOCKANIMTYPE_MULTIPLETURN          1
#define BLOCKANIMTYPE_DISSOLVE              2

#define BLOCKANIMVALUE_APPEAR               0
#define BLOCKANIMVALUE_DISAPPEAR            1
#define BLOCKANIMVALUE_BLINK_SLOW           2
#define BLOCKANIMVALUE_BLINK_MEDIUM         3
#define BLOCKANIMVALUE_BLINK_FAST           4
#define BLOCKANIMVALUE_BLINK_VERYFAST       5

//============================================
// Text margins
//============================================

#define TEXTMARGINS_FULLSHAPE               0
#define TEXTMARGINS_SHAPEDEFAULT            1
#define TEXTMARGINS_CUSTOM                  2

//============================================
// Default values
//============================================

#define DEFAULT_FONT_FAMILLY                "Arial"
#define DEFAULT_FONT_SIZE                   12
#define DEFAULT_FONT_COLOR                  "#ffffaa"
#define DEFAULT_FONT_SHADOWCOLOR            "#333333"
#define DEFAULT_FONT_ISBOLD                 false
#define DEFAULT_FONT_ISITALIC               false
#define DEFAULT_FONT_ISUNDERLINE            false
#define DEFAULT_FONT_HALIGN                 1
#define DEFAULT_FONT_VALIGN                 1
#define DEFAULT_FONT_TEXTEFFECT             5

#define DEFAULT_SHAPE_OPACITY               0
#define DEFAULT_SHAPE_BORDERSIZE            0
#define DEFAULT_SHAPE_BORDERCOLOR           "#333333"
#define DEFAULT_SHAPE_SHADOWCOLOR           "#000000"
#define DEFAULT_SHAPE_INTERNALBORDERSIZE    6
#define DEFAULT_SHAPE_INTERNALBORDERCOLOR1  "#808080"
#define DEFAULT_SHAPE_INTERNALBORDERCOLOR2  "#c0c0c0"
#define DEFAULT_SHAPE_BRUSHCOLORD           "#808080"

//*********************************************************************************************************************************************
// Base object for composition definition
//*********************************************************************************************************************************************

class cCompositionObjectContext {
public:
    bool                    NeedPreparedBrush;
    cCompositionObject      *Object;
    double                  width,height;
    bool                    PreviewMode;
    bool                    AddStartPos;
    int64_t               VideoPosition,StartPosToAdd,ShotDuration;
    cSoundBlockList         *SoundTrackMontage;
    double                  BlockPctDone,ImagePctDone;
    cCompositionObject      *PrevCompoObject;
    bool                    UseBrushCache,Transfo,EnableAnimation;
    double                  NewX,NewY,NewW,NewH;
    double                  TheX,TheY,TheW,TheH;
    double                  TheRotateZAxis,TheRotateXAxis,TheRotateYAxis;
    double                  TheTxtZoomLevel,TheTxtScrollX,TheTxtScrollY;
    double                  X,Y,W,H;
    double                  DestOpacity;
    bool                    IsCurrentObject;
    QList<QPolygonF>        PolygonList;
    QRectF                  ShapeRect;
    cDiaporamaShot          *CurShot;
    cDiaporamaShot          *PreviousShot;
    cDiaporamaObjectInfo    *Info;
    int                     ObjectNumber;

    cCompositionObjectContext(int ObjectNumber,bool PreviewMode,bool IsCurrentObject,cDiaporamaObjectInfo *Info,double width,double height,
                              cDiaporamaShot *CurShot,cDiaporamaShot *PreviousShot,cSoundBlockList *SoundTrackMontage,bool AddStartPos,int64_t ShotDuration);
    void Compute();
};

//**********************************

class cCompositionObject {
public:
    int                 TypeComposition;        // Type of composition object (COMPOSITIONTYPE_BACKGROUND, COMPOSITIONTYPE_OBJECT, COMPOSITIONTYPE_SHOT)
    int                 IndexKey;
    bool                IsVisible;              // True if block is visible during this shot
    bool                SameAsPrevShot;         // If true and not first shot then use the same value as in precedent shot

    // Coordinates attributs of the object (Shot values)
    double              x,y,w,h;                // Position (x,y) and size (width,height)
    double              RotateZAxis;            // Rotation from Z axis
    double              RotateXAxis;            // Rotation from X axis
    double              RotateYAxis;            // Rotation from Y axis

    // Block Annimation
    int                 BlockSpeedWave;         // Speed wave for block animation
    int                 BlockAnimType;          // Type of block animation (#define BLOCKANIMTYPE_)
    int                 TurnZAxis;              // BLOCKANIMTYPE_MULTIPLETURN : Number of turn from Z axis
    int                 TurnXAxis;              // BLOCKANIMTYPE_MULTIPLETURN : Number of turn from X axis
    int                 TurnYAxis;              // BLOCKANIMTYPE_MULTIPLETURN : Number of turn from Y axis
    int                 Dissolve;               // BLOCKANIMTYPE_DISSOLVE     : Dissolve value

    // Attribut of the text part (Global values)
    QString             Text;                   // Text of the object
    QString             TextClipArtName;        // ClipArt name (if text clipart mode)
    QString             FontName;               // font name
    int                 FontSize;               // font size
    QString             FontColor;              // font color
    QString             FontShadowColor;        // font shadow color
    bool                IsBold;                 // true if bold mode
    bool                IsItalic;               // true if Italic mode
    bool                IsUnderline;            // true if Underline mode
    int                 HAlign;                 // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
    int                 VAlign;                 // Vertical alignement : 0=up, 1=center, 2=bottom
    int                 StyleText;              // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right
    int                 TxtZoomLevel;           // Zoom Level for text
    int                 TxtScrollX;             // Scrolling X for text
    int                 TxtScrollY;             // Scrolling Y for text
    int                 TMType;                 // Text margins type (0=full shape, 1=shape default, 2=custom)
    double              TMx,TMy,TMw,TMh;        // Text margins

    // Attribut of the shap part (Global values)
    int                 BackgroundForm;         // Type of the form : 0=None, 1=Rectangle, 2=RoundRect, 3=Buble, 4=Ellipse, 5=Triangle UP (Polygon)
    cBrushDefinition    *BackgroundBrush;       // Brush of the background of the form
    int                 PenSize;                // Width of the border of the form
    int                 PenStyle;               // Style of the pen border of the form
    QString             PenColor;               // Color of the border of the form
    int                 FormShadow;             // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    int                 FormShadowDistance;     // Distance from form to shadow
    QString             FormShadowColor;        // Color of the shadow of the form
    int                 Opacity;                // Opacity of the form

    cBaseApplicationConfig *ApplicationConfig;

    cCompositionObject(int TypeComposition,int IndexKey,cBaseApplicationConfig *TheApplicationConfig);
    ~cCompositionObject();

    void        CopyFromCompositionObject(cCompositionObject *CompositionObjectToCopy);
    void        DrawCompositionObject(cDiaporamaObject *Object,QPainter *Painter,double  ADJUST_RATIO,double width,double height,bool PreviewMode,int64_t Position,
                                      cSoundBlockList *SoundTrackMontage,double BlockPctDone,double ImagePctDone,cCompositionObject *PreviousCompositionObject,
                                      bool UseBrushCache,int64_t ShotDuration,bool EnableAnimation,
                                      bool Transfo=false,double NewX=0,double NewY=0,double NewW=0,double NewH=0,
                                      bool DisplayTextMargin=false,cCompositionObjectContext *PreparedBrush=NULL);

    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath,bool CheckTypeComposition=true);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,cCompositionList *ObjectComposition,QStringList *AliasList,bool CheckTypeComposition=true);

    QRectF      GetTextMargin(QRectF Workspace,double  ADJUST_RATIO);
    void        ApplyTextMargin(int TMType);

    // Style managment functions
    int         GetAutoCompoSize(int ffDProjectGeometry);
    void        ApplyAutoCompoSize(int AutoCompoStyle,int ffDProjectGeometry,bool AllowMove=true);

    QString     GetCoordinateStyle();
    void        ApplyCoordinateStyle(QString StyleDef);

    QString     GetTextStyle();
    void        ApplyTextStyle(QString StyleDef);

    QString     GetBackgroundStyle();
    void        ApplyBackgroundStyle(QString StyleDef);

    QString     GetBlockShapeStyle();
    void        ApplyBlockShapeStyle(QString StyleDef);

private:
    QRectF      GetPrivateTextMargin();
};

//*********************************************************************************************************************************************
// Global class containing composition list
//*********************************************************************************************************************************************

class cCompositionList {
public:
    int                         TypeComposition;            // Type of composition list
    QList<cCompositionObject*>  List;                       // list of cCompositionObject

    cCompositionList();
    ~cCompositionList();

    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,cCompositionList *ObjectComposition,QStringList *AliasList,cBaseApplicationConfig *ApplicationConfig);
};

//*********************************************************************************************************************************************
// Base object for scene definition
//*********************************************************************************************************************************************
class cDiaporamaShot {
public:
    cDiaporamaObject        *Parent;
    int64_t                 StaticDuration;         // Duration (in msec) of the static part animation
    cCompositionList        ShotComposition;        // Shot Composition object list

    cDiaporamaShot(cDiaporamaObject *Parent);
    ~cDiaporamaShot();

    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath,bool LimitedInfo);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,cCompositionList *ObjectComposition,QStringList *AliasList);
};

//*********************************************************************************************************************************************
// class containing one slide
//*********************************************************************************************************************************************
class cDiaporamaObject {
public:
    cDiaporama              *Parent;                    // Link to global object
    bool                    StartNewChapter;            // if true then start a new chapter from this slide
    QString                 SlideName;                  // Display name of the slide
    QList<cDiaporamaShot *> List;                       // list of scene definition

    // Background definition
    bool                    BackgroundType;             // Background type : false=same as precedent - true=new background definition
    cBrushDefinition        *BackgroundBrush;           // Background brush

    // Object definition
    cCompositionList        ObjectComposition;          // Composition object list
    int                     NextIndexKey;               // Next index key (incremental value)

    // Music definition
    bool                    MusicType;                  // Music type : false=same as precedent - true=new playlist definition
    bool                    MusicPause;                 // true if music is pause during this object
    bool                    MusicReduceVolume;          // true if volume if reduce by MusicReduceFactor
    double                  MusicReduceFactor;          // factor for volume reduction if MusicReduceVolume is true

    // Transition
    int                     TransitionFamilly;          // Transition familly
    int                     TransitionSubType;          // Transition type in the familly
    int64_t                 TransitionDuration;         // Transition duration (in msec)
    int                     TransitionSpeedWave;        // Transition SpeedWave

    QImage                  *Thumbnail;                 // Thumbnail cached image
    QList<cMusicObject>     MusicList;                  // List of music definition

    cDiaporamaObject(cDiaporama *Parent);
    ~cDiaporamaObject();

    QString                 GetDisplayName();
    int64_t                 GetCumulTransitDuration();
    int64_t                 GetDuration();
    void                    DrawThumbnail(int ThumbWidth,int ThumbHeight,QPainter *Painter,int AddX,int AddY,int ShotNumber=0);   // Draw Thumb @ position 0
    void                    SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool                    LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList);
    int64_t                 GetTransitDuration();
    int                     GetSpeedWave();
    void                    LoadThumbnail(QString ThumbnailName);
    bool                    SaveThumbnail(QString ThumbnailName);
    int                     ComputeChapterNumber();
    int                     GetSlideNumber();
};

//*********************************************************************************************************************************************
// Technical class object for rendering
//*********************************************************************************************************************************************
class cDiaporamaObjectInfo {
public:
    //=====> All objects
    QImage              *RenderedImage;                         // Final image rendered
    bool                FreeRenderedImage;                      // True if allow to delete RenderedImage during destructor
    double              FrameDuration;                          // Duration of a frame (in msec)

    //=====> Current object
    int                 CurrentObject_Number;                   // Object number
    int64_t             CurrentObject_StartTime;                // Position (in msec) of the first frame relative to the diaporama
    int64_t             CurrentObject_InObjectTime;             // Position (in msec) in the object
    cDiaporamaObject    *CurrentObject;                         // Link to the current object
    int                 CurrentObject_ShotSequenceNumber;       // Number of the shot sequence in the current object
    cDiaporamaShot      *CurrentObject_CurrentShot;             // Link to the current shot in the current object
    int                 CurrentObject_CurrentShotType;          // Type of the current shot : Static/Mobil/Video
    int64_t             CurrentObject_ShotDuration;             // Time the static shot end (if CurrentObject_CurrentShotType=SHOTTYPE_STATIC)
    double              CurrentObject_PCTDone;                  // PCT achevement for static shot
    int                 CurrentObject_BackgroundIndex;          // Object number containing current background definition
    QBrush              *CurrentObject_BackgroundBrush;         // Current background brush
    bool                CurrentObject_FreeBackgroundBrush;      // True if allow to delete CurrentObject_BackgroundBrush during destructor
    QImage              *CurrentObject_PreparedBackground;      // Current image produce for background
    bool                CurrentObject_FreePreparedBackground;   // True if allow to delete CurrentObject_FreePreparedBackground during destructor
    cSoundBlockList     *CurrentObject_SoundTrackMontage;       // Sound for playing sound from montage track
    bool                CurrentObject_FreeSoundTrackMontage;    // True if allow to delete CurrentObject_SoundTrackMontage during destructor
    QImage              *CurrentObject_PreparedImage;           // Current image prepared
    bool                CurrentObject_FreePreparedImage;        // True if allow to delete CurrentObject_PreparedImage during destructor
    cSoundBlockList     *CurrentObject_MusicTrack;              // Sound for playing music from music track
    bool                CurrentObject_FreeMusicTrack;           // True if allow to delete CurrentObject_MusicTrack during destructor
    cMusicObject        *CurrentObject_MusicObject;             // Ref to the current playing music

    //=====> Transitionnal object
    bool                IsTransition;                           // True if transition in progress
    double              TransitionPCTDone;                      // PCT achevement for transition
    int                 TransitionFamilly;                      // Transition familly
    int                 TransitionSubType;                      // Transition type in the familly
    int64_t             TransitionDuration;                     // Transition duration (in msec)
    int                 TransitObject_Number;                   // Object number
    int64_t             TransitObject_StartTime;                // Position (in msec) of the first frame relative to the diaporama
    int64_t             TransitObject_InObjectTime;             // Position (in msec) in the object
    cDiaporamaObject    *TransitObject;                         // Link to the current object
    int                 TransitObject_ShotSequenceNumber;       // Number of the shot sequence in the current object
    cDiaporamaShot      *TransitObject_CurrentShot;             // Link to the current shot in the current object
    int                 TransitObject_CurrentShotType;          // Type of the current shot : Static/Mobil/Video
    int64_t             TransitObject_ShotDuration;             // Time the static shot end (if TransitObject_CurrentShotType=SHOTTYPE_STATIC)
    double              TransitObject_PCTDone;                  // PCT achevement for static shot
    int                 TransitObject_BackgroundIndex;          // Object number containing current background definition
    QBrush              *TransitObject_BackgroundBrush;         // Current background brush
    bool                TransitObject_FreeBackgroundBrush;      // True if allow to delete TransitObject_BackgroundBrush during destructor
    QImage              *TransitObject_PreparedBackground;      // Current image produce for background
    bool                TransitObject_FreePreparedBackground;   // True if allow to delete TransitObject_PreparedBackground during destructor
    cSoundBlockList     *TransitObject_SoundTrackMontage;       // Sound for playing sound from montage track
    bool                TransitObject_FreeSoundTrackMontage;    // True if allow to delete TransitObject_SoundTrackMontage during destructor
    QImage              *TransitObject_PreparedImage;           // Current image prepared
    bool                TransitObject_FreePreparedImage;        // True if allow to delete TransitObject_PreparedImage during destructor
    cSoundBlockList     *TransitObject_MusicTrack;              // Sound for playing music from music track
    bool                TransitObject_FreeMusicTrack;           // True if allow to delete TransitObject_MusicTrack during destructor
    cMusicObject        *TransitObject_MusicObject;             // Ref to the current playing music

    cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame);
    cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame,int64_t TimePosition,cDiaporama *Diaporama,double FrameDuration,bool WantSound);
    ~cDiaporamaObjectInfo();
    bool IsShotStatic(cDiaporamaObject *Object,int ShotNumber);
};

//*********************************************************************************************************************************************
// Global class containing the project
//*********************************************************************************************************************************************
class cDiaporama {
public:
    cBaseApplicationConfig  *ApplicationConfig;

    cffDProjectFile         *ProjectInfo;

    QString                 ThumbnailName;
    cDiaporamaObject        *ProjectThumbnail;

    int                     CurrentCol;             // Current position in the timeline (column)
    int64_t                 CurrentPosition;        // Current position in the timeline (msec)

    int                     CurrentChapter;
    QString                 CurrentChapterName;

    bool                    IsModify;               // true if project was modify
    QString                 ProjectFileName;        // Path and name of current file project

    // Output rendering values
    int                     ImageGeometry;          // Project image geometry for image rendering
    int                     InternalWidth;          // Real width for image rendering
    int                     InternalHeight;         // Real height for image rendering

    // Speed wave
    int                     TransitionSpeedWave;    // Speed wave for transition
    int                     BlockAnimSpeedWave;     // Speed wave for block animation
    int                     ImageAnimSpeedWave;     // Speed wave for image framing and correction animation

    // slides objects
    QList<cDiaporamaObject *> List;                   // list of all media object

    cDiaporama(cBaseApplicationConfig *ApplicationConfig,bool LoadDefaultModel=true);
    ~cDiaporama();
    int                     GetHeightForWidth(int WantedWith);
    int                     GetWidthForHeight(int WantedHeight);
    int                     GetObjectIndex(cDiaporamaObject *ObjectToFind);
    int64_t                 GetDuration();
    int64_t                 GetPartialDuration(int from,int to);
    int64_t                 GetObjectStartPosition(int index);
    int64_t                 GetTransitionDuration(int index);
    void                    PrepareBackground(int ObjectIndex,int Width,int Height,QPainter *Painter,int AddX,int AddY);
    cMusicObject            *GetMusicObject(int ObjectIndex,int64_t &StartPosition,int *CountObject=NULL,int *IndexObject=NULL);
    void                    DefineSizeAndGeometry(int Geometry);                        // Init size and geometry
    bool                    SaveFile(QWidget *ParentWindow);
    void                    UpdateChapterInformation();

    // Thread functions
    void                    PrepareMusicBloc(bool PreviewMode,int Column,int64_t Position,cSoundBlockList *MusicTrack);
    void                    LoadSources(cDiaporamaObjectInfo *Info,int W,int H,bool PreviewMode,bool AddStartPos);
    void                    DoAssembly(double PCT,cDiaporamaObjectInfo *Info,int W,int H,QImage::Format QTFMT=QImage::Format_ARGB32_Premultiplied);

    // Memory
    void                    CloseUnusedLibAv(int CurrentCell);

    void                    PrepareImage(cDiaporamaObjectInfo *Info,int W,int H,bool IsCurrentObject,bool PreviewMode,bool AddStartPos);
};

//****************************************************************************

#endif // CDIAPORAMA_H
