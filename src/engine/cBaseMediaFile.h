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
#include "cDeviceModelDef.h"                // Contains ffmpeg include
#include "cSoundBlockList.h"
#include "cCustomIcon.h"

// Other third party library inclusions

//****************************************************************************************************************************************************************
// EXIV2 PART
//****************************************************************************************************************************************************************
#include <exiv2/exif.hpp>
#if (EXIV2_MAJOR_VERSION==0) && (EXIV2_MINOR_VERSION>20)
    #include <exiv2/exiv2.hpp>
    #define EXIV2WITHPREVIEW
#else
    #include <exiv2/image.hpp>
#endif

//****************************************************************************************************************************************************************
// TAGLIB PART
//****************************************************************************************************************************************************************

#include <taglib/fileref.h>
#include <taglib/tbytevector.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v2framefactory.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>
#include <taglib/mp4file.h>
#include <taglib/vorbisfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/asffile.h>
#include <taglib/mp4tag.h>
#include <taglib/mp4item.h>
#include <taglib/mp4coverart.h>

#if (TAGLIB_MAJOR_VERSION>=1) && (TAGLIB_MINOR_VERSION>=7)
    #define TAGLIBWITHFLAC
#endif
#ifdef TAGLIB_WITH_ASF
    #if (TAGLIB_WITH_ASF>=1)
        #define TAGLIBWITHASF
        #if (TAGLIB_MAJOR_VERSION>=1) && (TAGLIB_MINOR_VERSION>=7)
            #define TAGLIBWITHASFPICTURE
        #endif
    #endif
#endif
#ifdef TAGLIB_WITH_MP4
    #if (TAGLIB_WITH_MP4>=1)
        #define TAGLIBWITHMP4
    #endif
#endif

//****************************************************************************************************************************************************************

class cBaseMediaFile : public cCustomIcon {
public:
    int                     ObjectType;
    bool                    IsValide;                       // if true then object if initialise
    bool                    IsInformationValide;            // if true then information list if fuly initialise
    int                     ObjectGeometry;                 // Image geometry of the embeded image or video
    QString                 FileName;                       // filename
    QString                 ShortName;                      // filename without path
    QString                 FileExtension;                  // file extension
    qlonglong               FileSize;                       // filesize
    QString                 FileSizeText;                   // filesize in text mode
    QDateTime               CreatDateTime;                  // Original date/time
    QDateTime               ModifDateTime;                  // Last modified date/time
    int                     ImageWidth;                     // Widht of normal image
    int                     ImageHeight;                    // Height of normal image
    int                     ImageOrientation;               // EXIF ImageOrientation (or -1)
    double                  AspectRatio;                    // Aspect ratio
    int                     ImageType;                      // Type of image (depending on type object and size)
    cBaseApplicationConfig *ApplicationConfig;
    QStringList             InformationList;

    cBaseMediaFile(cBaseApplicationConfig *ApplicationConfig);
    virtual                 ~cBaseMediaFile();

    virtual void            Reset();
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

    virtual bool            GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag);
    virtual QString         GetFileTypeStr();
    virtual bool            IsFilteredFile(int RequireObjectType);
    virtual void            GetFullInformationFromFile()                    {/*Nothing to do*/}
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { return ApplicationConfig->DefaultFILEIcon.GetIcon(Size); }
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
    virtual void            GetFullInformationFromFile();
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { return ApplicationConfig->DefaultFOLDERIcon.GetIcon(Size); }
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
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { return ApplicationConfig->DefaultFFDIcon.GetIcon(Size); }

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

    virtual QImage          *ImageAt(bool PreviewMode);
    virtual QString         GetFileTypeStr();
    virtual bool            IsFilteredFile(int RequireObjectType);
    virtual void            GetFullInformationFromFile();
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size) { return (ObjectType==OBJECTTYPE_THUMBNAIL?ApplicationConfig->DefaultThumbIcon:ApplicationConfig->DefaultIMAGEIcon).GetIcon(Size); }
    virtual QString         GetTechInfo();
    virtual QString         GetTAGInfo();
};

//*********************************************************************************************************************************************
// Video file
//*********************************************************************************************************************************************
extern int MAXCACHEIMAGE;

class cImageInCache {
public:
    qlonglong   Position;
    QImage      Image;
    cImageInCache(qlonglong Position,QImage *Image);
};

class cVideoFile : public cBaseMediaFile {
public:
    bool                    IsOpen;                     // True if ffmpeg open on this file
    bool                    MusicOnly;                  // True if object is a music only file
    bool                    IsVorbis;                   // True if vorbis version must be use instead of MP3/WAV version
    bool                    IsMTS;                      // True if file is a MTS file
    QTime                   StartPos;                   // Start position
    QTime                   EndPos;                     // End position
    QString                 Container;                  // Container type (get from file extension)
    QTime                   Duration;                   // Duration of the video
    double                  dEndFileCachePos;           // Position of the cache image of last image of the video
    QString                 VideoCodecInfo;
    QString                 AudioCodecInfo;
    int                     NbrChapters;                // Number of chapters in the file

    // Video part
    AVFormatContext         *ffmpegVideoFile;           // LibAVFormat context
    AVCodec                 *VideoDecoderCodec;         // Associated LibAVCodec for video stream
    int                     VideoStreamNumber;          // Number of the video stream
    int                     VideoTrackNbr;              // Number of video stream in file
    AVFrame                 *FrameBufferYUV;
    bool                    FrameBufferYUVReady;        // true if FrameBufferYUV is ready to convert
    int64_t                 FrameBufferYUVPosition;     // If FrameBufferYUV is ready to convert then keep FrameBufferYUV position
    qlonglong               start_time;                 // At first seek 0, keep the AVFormatContext value

    // Audio part
    AVFormatContext         *ffmpegAudioFile;           // LibAVFormat context
    AVCodec                 *AudioDecoderCodec;         // Associated LibAVCodec for audio stream
    int                     AudioStreamNumber;          // Number of the audio stream
    int                     AudioTrackNbr;              // Number of audio stream in file
    int64_t                 LastAudioReadedPosition;    // Use to keep the last readed position to determine if a seek is needed

    QList<cImageInCache>    CacheImage;

    explicit                cVideoFile(int WantedObjectType,cBaseApplicationConfig *ApplicationConfig);
                            ~cVideoFile();
    virtual void            Reset(int TheWantedObjectType);

    virtual QString         GetFileTypeStr();
    virtual bool            IsFilteredFile(int RequireObjectType);
    virtual void            GetFullInformationFromFile();
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size);

    virtual QString         GetTechInfo();
    virtual QString         GetTAGInfo();

    virtual bool            OpenCodecAndFile();
    virtual void            CloseCodecAndFile();

    virtual QImage          *ImageAt(bool PreviewMode,qlonglong Position,qlonglong StartPosToAdd,cSoundBlockList *SoundTrackMontage,bool Deinterlace,double Volume,bool ForceSoundOnly,bool DontUseEndPos);
    virtual QImage          *ReadVideoFrame(bool PreviewMode,qlonglong Position,bool DontUseEndPos,bool Deinterlace);
    virtual void            ReadAudioFrame(bool PreviewMode,qlonglong Position,cSoundBlockList *SoundTrackBloc,double Volume,bool DontUseEndPos);      // MP3 and WAV
    virtual QImage          *ConvertYUVToRGB(bool PreviewMode);

    //*********************
    // video filters part
    //*********************
    #ifdef VIDEO_LIBAVFILTER
        AVFilterGraph           *VideoFilterGraph;
        AVFilterContext         *VideoFilterIn;
        AVFilterContext         *VideoFilterOut;
        QString                 m_filters;
        QString                 m_filters_next;

        enum EFilterFlags {
            FILTER_NONE                 = 0x0,
            FILTER_DEINTERLACE_YADIF    = 0x1,
            FILTER_DEINTERLACE_ANY      = 0xf,
            FILTER_DEINTERLACE_FLAGGED  = 0x10,
            FILTER_DEINTERLACE_HALFED   = 0x20
        };

        virtual unsigned int    SetFilters(unsigned int flags);
        virtual int             VideoFilter_Open(QString Filters);
        virtual void            VideoFilter_Close();
        virtual int             VideoFilter_Process();
    #endif

    //*********************
    // audio filters part
    //*********************
    #ifdef AUDIO_LIBAVFILTER
        AVFilterGraph           *AudioFilterGraph;
        AVFilterContext         *AudioFilterIn;
        AVFilterContext         *AudioFilterOut;
        virtual int             AudioFilter_Open(QString Filters);
        virtual void            AudioFilter_Close();
    #endif
};

//*********************************************************************************************************************************************
// Music file
//*********************************************************************************************************************************************

class cMusicObject : public cVideoFile {
public:
    double              Volume;                 // Volume as % from 10% to 150%

    cMusicObject(cBaseApplicationConfig *ApplicationConfig);

    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag);
    bool        LoadMedia(QString &filename,QStringList *AliasList,bool *ModifyFlag);
};

#endif // CBASEMEDIAFILE_H
