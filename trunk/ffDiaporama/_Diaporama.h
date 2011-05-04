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

#ifndef CDIAPORAMA_H
#define CDIAPORAMA_H

//========================================
// Basic inclusions (common to all files)
//========================================
#include "_GlobalDefines.h"

//===============================
// Specific inclusions
//===============================
#include "_ApplicationDefinitions.h"
#include "_ImagesDefinitions.h"
#include "_SoundDefinitions.h"
#include "_ImageFileWrapper.h"
#include "_VideoFileWrapper.h"

class cDiaporama;
class cDiaporamaObject;

//*********************************************************************************************************************************************
// Base object for composition definition
//*********************************************************************************************************************************************

class cCompositionObject {
public:
    int         ZValue;             // Z value ordering (low is background)

    // Attribut of the text object
    QString             Text;                   // Text of the object
    double               x,y,w,h;                // Position (x,y) and size (width,height)
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

    // Attribut of the form object
    int                 BackgroundForm;         // Type of the form : 0=None, 1=Rectangle, 2=RoundRect, 3=Buble, 4=Ellipse, 5=Triangle UP (Polygon)
    cBrushDefinition    BackgroundBrush;        // Brush of the background of the form
    int                 BackgroundTransparent;  // Transparency of the background of the form
    int                 PenSize;                // Width of the pen of the form
    QString             PenColor;               // Color of the pen of the form

    cCompositionObject();

    void        DrawCompositionObject(QPainter &Painter,int AddX,int AddY,int width,int height);
    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath);
};

//*********************************************************************************************************************************************
// Global class containing composition list
//*********************************************************************************************************************************************

class cCompositionList {
public:
    QList<cCompositionObject>     List;                       // list of cCompositionObject

    cCompositionList();
    ~cCompositionList();

    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath);
};

//*********************************************************************************************************************************************
// Base object for scene definition
//*********************************************************************************************************************************************
class cDiaporamaShot {
public:
    cDiaporamaObject        *Parent;
    bool                    DefaultStaticDuration;  // true if object use Diaporama duration instead of it's own duration
    int                     StaticDuration;         // Duration (in msec) of the static part animation
    bool                    DefaultMobilDuration;   // true if object use Diaporama duration instead of it's own duration
    int                     MobilDuration;          // Duration (in msec) of the static part animation
    double                  X;                      // X position (in %) relative to up/left corner
    double                  Y;                      // Y position (in %) relative to up/left corner
    double                  ZoomFactor;             // Zoom factor (in %)
    int                     ImageRotation;          // Image rotation (in °)
    cFilterCorrectObject    FilterCorrection;       // Image correction
    cCompositionList        ShotComposition;        // Shot Composition object list
    QTime                   StartPos;               // Start position (video only)
    QTime                   EndPos;                 // End position (video only)

    cDiaporamaShot(cDiaporamaObject *Parent);
    ~cDiaporamaShot();

    int         GetStaticDuration();
    int         GetMobilDuration();

    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath);
};

//*********************************************************************************************************************************************
// Base object for all media type
//*********************************************************************************************************************************************
class cDiaporamaObject {
public:
    cDiaporama              *Parent;                    // Link to global object
    int                     TypeObject;                 // Type of object
    cimagefilewrapper       *Image;                     // Embeded Object for title and image type
    cvideofilewrapper       *Video;                     // Embeded Object for video type
    QList<cDiaporamaShot>   List;                       // list of scene definition
    cCompositionList        ObjectComposition;          // Object Composition object list
    cFilterTransformObject  FilterTransform;            // Filters tranformation value for source image

    // Background definition
    bool                    BackgroundType;             // Background type : false=same as precedent - true=new background definition
    cBrushDefinition        BackgroundBrush;            // Background brush
    cCompositionList        BackgroundComposition;      // Background Composition object list
    bool                    FullCanvas;                 // true if Object use full canvas (height is hypothenuse of the image rectangle and width is calc from aspect ratio)

    // Music definition
    bool                    MusicType;                  // Music type : false=same as precedent - true=new playlist definition
    bool                    MusicPause;                 // true if music is pause during this object
    bool                    MusicReduceVolume;          // true if volume if reduce by MusicReduceFactor
    double                  MusicReduceFactor;          // factor for volume reduction if MusicReduceVolume is true

    // Sound definition
    double                  SoundVolume;                // Volume of soundtrack (for video only)

    // Transition
    int                     TransitionFamilly;          // Transition familly
    int                     TransitionSubType;          // Transition type in the familly
    int                     TransitionDuration;         // Transition duration (in msec)

    QList<cMusicObject>     MusicList;                  // List of music definition

    cDiaporamaObject(cDiaporama *Parent);
    ~cDiaporamaObject();

    bool                    LoadMedia(QString filename,int MediaType);
    QString                 GetDisplayName();
    int                     GetCumulTransitDuration();
    int                     GetDuration();

    // Load image from source object at given position(video) and Apply filter
    QImage                  *GetImageAt(int Position,bool VideoCachedMode,cSoundBlockList *SoundTrackMontage);

    // Load image and put it on a Canvas of size Width x Height
    QImage                  *CanvasImageAt(int Width,int Height,int Position,QPainter *Painter,int AddX,int AddY,QRectF *ImagePosition,int *ForcedImageRotation,bool VideoCachedMode,bool ApplyShotText,bool ApplyShotFilter,cSoundBlockList *SoundTrackMontage);
    void                    PrepareImage(QPainter *P,int Width,int Height,int Position,QImage *LastLoadedImage,int AddX,int AddY,QRectF *ImagePosition,int *ForcedImageRotation,bool ApplyShotText,bool ApplyShotFilter);

    // Function to calculate canvas
    void                    CalcFullCanvas(double RealImageW,double RealImageH,double &VirtImageW,double &VirtImageH);
    void                    CalcNormalCanvas(double RealImageW,double RealImageH,double &VirtImageW,double &VirtImageH);
    void                    CalcTransformations(int Sequence,double PctDone,double &XFactor,double &YFactor,double &ZoomFactor,double &RotateFactor,cFilterCorrectObject &FilterCorrection);
    void                    ApplyDefaultFraming(int DefaultFraming);
    void                    SwitchShotsToNormalCanvas();
    void                    SwitchShotsToFullCanvas();

    void                    SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath);
    bool                    LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath);
};

//*********************************************************************************************************************************************
// Class object for rendering
//*********************************************************************************************************************************************
class cDiaporamaObjectInfo {
public:
    //=====> Current object
    int                 CurrentObject_Number;                   // Object number
    int                 CurrentObject_StartTime;                // Position (in msec) of the first frame relative to the diaporama
    int                 CurrentObject_InObjectTime;             // Position (in msec) in the object
    cDiaporamaObject    *CurrentObject;                         // Link to the current object
    int                 CurrentObject_ShotSequenceNumber;       // Number of the shot sequence in the current object
    cDiaporamaShot      *CurrentObject_CurrentShot;             // Link to the current shot in the current object
    int                 CurrentObject_CurrentShotType;          // Type of the current shot : Static/Mobil/Video
    int                 CurrentObject_EndStaticShot;            // Time the static shot end (if CurrentObject_CurrentShotType=SHOTTYPE_STATIC)
    double              CurrentObject_MobilPCTDone;             // PCT achevement for mobil shot (if CurrentObject_CurrentShotType=SHOTTYPE_MOBIL)
    QImage              *CurrentObject_SourceImage;             // Current image loaded for image or video or created for title
    bool                CurrentObject_FreeSourceImage;          // True if allow to delete CurrentObject_SourceImage during destructor
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
    double              TransitionPCTEnd;                       // PCT achevement @ end of this frame for fade in/out
    int                 TransitionFamilly;                      // Transition familly
    int                 TransitionSubType;                      // Transition type in the familly
    int                 TransitionDuration;                     // Transition duration (in msec)
    int                 TransitObject_Number;                   // Object number
    int                 TransitObject_StartTime;                // Position (in msec) of the first frame relative to the diaporama
    int                 TransitObject_InObjectTime;             // Position (in msec) in the object
    cDiaporamaObject    *TransitObject;                         // Link to the current object
    int                 TransitObject_ShotSequenceNumber;       // Number of the shot sequence in the current object
    cDiaporamaShot      *TransitObject_CurrentShot;             // Link to the current shot in the current object
    int                 TransitObject_CurrentShotType;          // Type of the current shot : Static/Mobil/Video
    int                 TransitObject_EndStaticShot;            // Time the static shot end (if TransitObject_CurrentShotType=SHOTTYPE_STATIC)
    double              TransitObject_MobilPCTDone;             // PCT achevement for mobil shot (if TransitObject_CurrentShotType=SHOTTYPE_MOBIL)
    QImage              *TransitObject_SourceImage;             // Current image loaded for image or video or created for title
    bool                TransitObject_FreeSourceImage;          // True if allow to delete TransitObject_SourceImage during destructor
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

    QImage              *RenderedImage;                         // Final image rendered
    bool                FreeRenderedImage;                      // True if allow to delete RenderedImage during destructor

    cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame);
    cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame,int TimePosition,cDiaporama *Diaporama,double FrameDuration);
    ~cDiaporamaObjectInfo();
};

//*********************************************************************************************************************************************
// Global class containing media objects
//*********************************************************************************************************************************************
class cDiaporama {
public:
    cApplicationConfig      *ApplicationConfig;
    QTableWidget            *Timeline;              // Link to timeline
    int                     CurrentCol;             // Current position in the timeline (column)
    int                     CurrentPosition;        // Current position in the timeline (msec)

    QBrush                  Transparent;            // Transparent brush

    // Default value
    int                     NoShotDuration;         // Default duration for fixed image when is alone (no shot)
    int                     FixedDuration;          // Default duration for fixed image
    int                     MobilDuration;          // Default duration for mobil image
    int                     SpeedWave;              // Default Speed wave methode

    bool                    IsModify;               // true if project was modify
    QString                 ProjectFileName;        // Path and name of current file project

    // Output rendering values
    int                     ImageGeometry;          // Project image geometry for image rendering
    int                     LastImageSize;          // Last image size use for rendering
    int                     LastStandard;           // Last standard use for rendering
    int                     InternalWidth;          // Real width for image rendering
    int                     InternalHeight;         // Real height for image rendering
    int                     OutputFileFormat;       // Container format
    QString                 OutputFileName;         // filename with path for the last rendering file
    QString                 VideoCodec;             // Last video codec used
    double                  VideoFrameRate;         // Last video frame rate used
    int                     VideoBitRate;           // Last video bit rate used
    QString                 AudioCodec;             // Last audio codec used
    int                     AudioFrequency;         // Last audio frequency used
    int                     AudioBitRate;           // Last audio bit rate used

    // slides objects
    QList<cDiaporamaObject> List;                   // list of all media object

    cDiaporama(cApplicationConfig *ApplicationConfig);
    ~cDiaporama();
    void                Clean();
    int                 GetHeightForWidth(int WantedWith);
    int                 GetWidthForHeight(int WantedHeight);
    int                 GetObjectIndex(cDiaporamaObject *ObjectToFind);
    int                 GetDuration();
    int                 GetObjectStartPosition(int index);
    int                 GetTransitionDuration(int index);
    void                PrepareBackground(int ObjectIndex,int Width,int Height,QPainter *Painter,int AddX,int AddY,bool ApplyComposition);
    cMusicObject        *GetMusicObject(int ObjectIndex,int &StartPosition,int *CountObject=NULL);
    void                DefineSizeAndGeometry(int Geometry);                        // Init size and geometry
    bool                SaveFile(QWidget *ParentWindow);
    bool                LoadFile(QWidget *ParentWindow,QString ProjectFileName);
    bool                AppendFile(QWidget *ParentWindow,QString ProjectFileName);

    // Thread functions
    void                PrepareMusicBloc(int Column,int Position,cSoundBlockList *MusicTrack);
    void                PrepareImage(cDiaporamaObjectInfo *Info,int W,int H,int Extend,bool IsCurrentObject);
    void                LoadSources(cDiaporamaObjectInfo *Info,int W,int H,bool PreviewMode);
    void                DoAssembly(cDiaporamaObjectInfo *Info,int W,int H);

    // Transition
    void                DoBasic(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H);
    void                DoZoom(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H);
    void                DoSlide(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H);
    void                DoPush(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H);
    void                DoLuma(cLumaList *List,cDiaporamaObjectInfo *Info,QPainter *P,int W,int H);

    // Threaded functions
    void ThreadLoadSourceVideoImage(cDiaporamaObjectInfo *Info,bool PreviewMode,bool SoundOnly);
    void ThreadLoadSourcePhotoImage(cDiaporamaObjectInfo *Info,bool PreviewMode);
    void ThreadLoadTransitVideoImage(cDiaporamaObjectInfo *Info,bool PreviewMode,bool SoundOnly);
    void ThreadLoadTransitPhotoImage(cDiaporamaObjectInfo *Info,bool PreviewMode);

};

//****************************************************************************

#endif // CDIAPORAMA_H
