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

#ifndef CBASEMEDIAFILE_H
#define CBASEMEDIAFILE_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some common various class
#include "cBaseApplicationConfig.h"

// Include some additional standard class
#include <QString>
#include <QTime>
#include <QDateTime>
#include <QImage>

// Include some common various class
#include "cFilterTransformObject.h"
#include "cSoundBlockList.h"
#include "cLuLoImageCache.h"
#include "cBaseMediaFile.h"

// Other third party library inclusions : ffmpeg
extern "C" {
    #include <libavutil/common.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}
#ifndef AV_SAMPLE_FMT_S16
    #define AV_SAMPLE_FMT_NONE  SAMPLE_FMT_NONE
    #define AV_SAMPLE_FMT_U8    SAMPLE_FMT_U8
    #define AV_SAMPLE_FMT_S16   SAMPLE_FMT_S16
    #define AV_SAMPLE_FMT_S32   SAMPLE_FMT_S32
    #define AV_SAMPLE_FMT_FLT   SAMPLE_FMT_FLT
    #define AV_SAMPLE_FMT_DBL   SAMPLE_FMT_DBL
    #define AV_SAMPLE_FMT_NB    SAMPLE_FMT_NB
#endif

// Define possible values for images geometry
#define IMAGE_GEOMETRY_UNKNOWN              0   // undefined image geometry
#define IMAGE_GEOMETRY_3_2                  1   // Standard 3:2 landscape image
#define IMAGE_GEOMETRY_2_3                  2   // Standard 3:2 portrait image
#define IMAGE_GEOMETRY_4_3                  3   // Standard 4:3 landscape image
#define IMAGE_GEOMETRY_3_4                  4   // Standard 4:3 portrait image
#define IMAGE_GEOMETRY_16_9                 5   // Standard 16:9 landscape image
#define IMAGE_GEOMETRY_9_16                 6   // Standard 16:9 portrait image
#define IMAGE_GEOMETRY_40_17                7   // Standard cinema landscape image
#define IMAGE_GEOMETRY_17_40                8   // Standard cinema portrait image

#define OBJECTTYPE_UNMANAGED    0
#define OBJECTTYPE_MANAGED      1
#define OBJECTTYPE_FOLDER       2
#define OBJECTTYPE_FFDFILE      3
#define OBJECTTYPE_IMAGEFILE    4
#define OBJECTTYPE_VIDEOFILE    5
#define OBJECTTYPE_MUSICFILE    6
#define OBJECTTYPE_THUMBNAIL    7
#define OBJECTTYPE_MUSICORVIDEO 100

class cBaseMediaFile : public cCustomIcon {
public:
    int                     ObjectType;
    bool                    IsValide;                       // if true then object if initialise
    bool                    IsInformationValide;            // if true then information list if fuly initialise
    int                     ObjectGeometry;                 // Image geometry of the embeded image or video
    QString                 FileName;                       // filename
    QString                 ShortName;                      // filename without path
    qlonglong               FileSize;                       // filesize
    QString                 FileSizeText;                   // filesize in text mode
    QDateTime               CreatDateTime;                  // Original date/time
    QDateTime               ModifDateTime;                  // Last modified date/time
    int                     ImageWidth;                     // Widht of normal image
    int                     ImageHeight;                    // Height of normal image
    double                  AspectRatio;                    // Aspect ratio
    cFilterTransformObject  BrushFileTransform;             // Image transformation if image from disk
    cBaseApplicationConfig *ApplicationConfig;
    QStringList             InformationList;

    cBaseMediaFile(cBaseApplicationConfig *ApplicationConfig);
    ~cBaseMediaFile();

    virtual bool            GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag);
    virtual bool            IsFilteredFile(int RequireObjectType)=0;
    virtual void            GetFullInformationFromFile()=0;
    virtual QString         GetInformationValue(QString ValueToSearch);
    virtual QString         GetCumulInfoStr(QString Key1,QString Key2);

    enum    ImageSizeFmt {FULLWEB,SIZEONLY,FMTONLY,GEOONLY};
    virtual QString         GetImageSizeStr(ImageSizeFmt Fmt=FULLWEB);      // Return image size as formated string
    virtual QString         GetImageGeometryStr();                          // Return image geometry as formated string
    virtual QString         GetFileTypeStr()=0;
    virtual QString         GetFileDateTimeStr(bool Created=false);         // Return file date/time as formated string
    virtual QString         GetFileSizeStr();                               // Return file size as formated string

    virtual QString         GetTechInfo()=0;                                // Return technical information as formated string
    virtual QString         GetTAGInfo()=0;                                 // Return TAG information as formated string

    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size)=0;
};

//*********************************************************************************************************************************************
// Image file
//*********************************************************************************************************************************************
class cUnmanagedFile : public cBaseMediaFile {
public:
    explicit cUnmanagedFile(cBaseApplicationConfig *ApplicationConfig);

    virtual QString         GetFileTypeStr();
    virtual bool            IsFilteredFile(int RequireObjectType);
    virtual void            GetFullInformationFromFile()                    {/*Nothing to do*/}
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { if (!IsInformationValide) return ApplicationConfig->DefaultDelayedIcon.GetIcon(Size); else return ApplicationConfig->DefaultFILEIcon.GetIcon(Size); }
    virtual QString         GetTechInfo()                                   { return ""; }
    virtual QString         GetTAGInfo()                                    { return ""; }
};

//*********************************************************************************************************************************************
// Image file
//*********************************************************************************************************************************************
class cFolder : public cBaseMediaFile {
public:
    explicit cFolder(cBaseApplicationConfig *ApplicationConfig);

    virtual bool            GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag);
    virtual QString         GetFileTypeStr();
    virtual bool            IsFilteredFile(int RequireObjectType);
    virtual void            GetFullInformationFromFile()                    {/*Nothing to do*/}
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { if (!IsInformationValide) return ApplicationConfig->DefaultDelayedIcon.GetIcon(Size); else return ApplicationConfig->DefaultFOLDERIcon.GetIcon(Size); }
    virtual QString         GetTechInfo()                                   { return ""; }
    virtual QString         GetTAGInfo()                                    { return ""; }
};

//*********************************************************************************************************************************************
// ffDiaporama project file
//*********************************************************************************************************************************************
class cffDProjectFile : public cBaseMediaFile {
public:
    // TAG values
    QString     Title;              // 30 char
    QString     Author;             // 30 char
    QString     Album;              // 30 char
    int         Year;               // Year - 4 digits
    QString     Comment;            // Free text - free size
    QString     Composer;           // ffDiaporama version
    qlonglong   Duration;           // (Duration in msec)
    int         NbrSlide;           // (Number of slide in project)
    QString     ffDRevision;        // ffD Application version (in reverse date format)
    QString     DefaultLanguage;    // Default Language (ISO 639 language code)
    int         NbrChapters;        // Number of chapters in the file

    explicit cffDProjectFile(cBaseApplicationConfig *ApplicationConfig);

    virtual bool            GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag);
    virtual QString         GetFileTypeStr();
    virtual bool            IsFilteredFile(int RequireObjectType);
    virtual void            GetFullInformationFromFile();
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { if (!IsInformationValide) return ApplicationConfig->DefaultDelayedIcon.GetIcon(Size); else return ApplicationConfig->DefaultFFDIcon.GetIcon(Size); }

    virtual QString         GetTechInfo();
    virtual QString         GetTAGInfo();

    void                    SaveToXML(QDomElement &domDocument);
    bool                    LoadFromXML(QDomElement domDocument);
};

//*********************************************************************************************************************************************
// Image file
//*********************************************************************************************************************************************
class cImageFile : public cBaseMediaFile {
public:
    explicit cImageFile(cBaseApplicationConfig *ApplicationConfig);

    virtual QImage          *ImageAt(bool PreviewMode,bool ForceLoadDisk,cFilterTransformObject *Filter);
    virtual bool            CallEXIF(int &ImageOrientation);
    virtual QString         GetFileTypeStr();
    virtual bool            IsFilteredFile(int RequireObjectType);
    virtual void            GetFullInformationFromFile();
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { if (!IsInformationValide) return ApplicationConfig->DefaultDelayedIcon.GetIcon(Size); else return (ObjectType==OBJECTTYPE_THUMBNAIL?ApplicationConfig->DefaultThumbIcon:ApplicationConfig->DefaultIMAGEIcon).GetIcon(Size); }
    virtual QString         GetTechInfo();
    virtual QString         GetTAGInfo();
};

//*********************************************************************************************************************************************
// Video file
//*********************************************************************************************************************************************

class cVideoFile : public cBaseMediaFile {
public:
    enum WantedObjectTypeFmt {MUSICORVIDEO,VIDEOFILE,MUSICFILE};

    bool                    IsOpen;                     // True if ffmpeg open on this file
    bool                    MusicOnly;                  // True if object is a music only file
    WantedObjectTypeFmt     WantedObjectType;
    bool                    IsVorbis;                   // True if vorbis version must be use instead of MP3/WAV version
    QTime                   StartPos;                   // Start position
    QTime                   EndPos;                     // End position
    QString                 Container;                  // Container type (get from file extension)
    QTime                   Duration;                   // Duration of the video
    QImage                  *CacheFirstImage;           // Cache image of first image of the video (Preview mode only)
    QImage                  *CacheLastImage;            // Cache image of last image of the video (Preview mode only)
    double                  dEndFileCachePos;           // Position of the cache image of last image of the video
    bool                    CodecUsePTS;                // true if codec use PTS (h264) if if we use only DTS
    QString                 VideoCodecInfo;
    QString                 AudioCodecInfo;

    // Video part
    AVFormatContext         *ffmpegVideoFile;           // LibAVFormat context
    AVCodec                 *VideoDecoderCodec;         // Associated LibAVCodec for video stream
    int                     VideoStreamNumber;          // Number of the video stream
    int                     VideoTrackNbr;              // Number of video stream in file
    AVFrame                 *FrameBufferYUV;
    bool                    FrameBufferYUVReady;        // true if FrameBufferYUV is ready to convert
    int64_t                 FrameBufferYUVPosition;     // If FrameBufferYUV is ready to convert then keep FrameBufferYUV position

    // Audio part
    AVFormatContext         *ffmpegAudioFile;           // LibAVFormat context
    AVCodec                 *AudioDecoderCodec;         // Associated LibAVCodec for audio stream
    int                     AudioStreamNumber;          // Number of the audio stream
    int                     AudioTrackNbr;              // Number of audio stream in file
    int64_t                 LastAudioReadedPosition;    // Use to keep the last readed position to determine if a seek is needed

    int                     NbrChapters;                // Number of chapters in the file

    explicit    cVideoFile(WantedObjectTypeFmt WantedObjectType,cBaseApplicationConfig *ApplicationConfig);
    ~cVideoFile();

    virtual bool            GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag);

    virtual QString         GetFileTypeStr();
    virtual bool            IsFilteredFile(int RequireObjectType);
    virtual void            GetFullInformationFromFile();
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { if (!IsInformationValide) return ApplicationConfig->DefaultDelayedIcon.GetIcon(Size); else return (ObjectType==OBJECTTYPE_MUSICFILE?ApplicationConfig->DefaultMUSICIcon:ApplicationConfig->DefaultVIDEOIcon).GetIcon(Size); }

    virtual QString         GetTechInfo();
    virtual QString         GetTAGInfo();

    virtual bool            OpenCodecAndFile();
    virtual void            CloseCodecAndFile();

    virtual QImage          *ImageAt(bool PreviewMode,qlonglong Position,qlonglong StartPosToAdd,bool ForceLoadDisk,cSoundBlockList *SoundTrackMontage,double Volume,bool ForceSoundOnly,cFilterTransformObject *Filter,bool DontUseEndPos);
    virtual QImage          *ReadVideoFrame(qlonglong Position,bool DontUseEndPos);
    virtual void            ReadAudioFrame(bool PreviewMode,qlonglong Position,cSoundBlockList *SoundTrackBloc,double Volume,bool DontUseEndPos);      // MP3 and WAV
    virtual QImage          *ConvertYUVToRGB();
};

#endif // CBASEMEDIAFILE_H
