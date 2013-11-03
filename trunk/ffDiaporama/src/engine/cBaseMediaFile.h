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

#if QT_VERSION >= 0x050000
    #include <QtSvg/QtSvg>
#else
    #include <QtSvg>
#endif

// Include some common various class
#include "cDeviceModelDef.h"                // Contains Libav include
#include "cSoundBlockList.h"
#include "cCustomIcon.h"

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

#if defined(LIBAV) && (LIBAVVERSIONINT<=8)
    //****************************************************************************************************************************************************************
    // TAGLIB PART only if LIBAV 8 after, thumbnails are reading using libav
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
#endif

//============================================
// Class for exporting project
//============================================

class cReplaceObject {
public:
    QString SourceFileName;
    QString DestFileName;
    cReplaceObject(QString SourceFileName,QString DestFileName) {this->SourceFileName=SourceFileName; this->DestFileName=DestFileName;}
};

class cReplaceObjectList {
public:
    QList<cReplaceObject> List;

            cReplaceObjectList();

    void    SearchAppendObject(QString SourceFileName);
    QString GetDestinationFileName(QString SourceFileName);
};

//****************************************************************************************************************************************************************

class cBaseMediaFile : public cCustomIcon {
public:
    cBaseApplicationConfig *ApplicationConfig;
    int                     ObjectType;

    qlonglong               FileKey;                        // Key index of this file in the Files table of the database
    qlonglong               FolderKey;                      // Key index of the folder containing this file in the Folders table of the database
    QStringList             InformationList;

    int64_t                 FileSize;                       // filesize
    QDateTime               CreatDateTime;                  // Original date/time
    QDateTime               ModifDateTime;                  // Last modified date/time

    int                     ImageWidth;                     // Widht of normal image
    int                     ImageHeight;                    // Height of normal image
    int                     ImageOrientation;               // EXIF ImageOrientation (or -1)
    int                     ObjectGeometry;                 // Image geometry of the embeded image or video
    double                  AspectRatio;                    // Aspect ratio
    QTime                   Duration;                       // Duration of the video

    bool                    IsValide;                       // if true then object if initialise
    bool                    IsInformationValide;            // if true then information list if fuly initialise

    cBaseMediaFile(cBaseApplicationConfig *ApplicationConfig);
    virtual                 ~cBaseMediaFile();

    virtual QString         FileName();
    virtual QString         ShortName();
    virtual bool            LoadBasicInformationFromDatabase(QDomElement root)=0;
    virtual void            SaveBasicInformationToDatabase(QDomElement *root)=0;

    virtual void            Reset();
    virtual bool            GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag,qlonglong FolderKey);
    virtual bool            GetFullInformationFromFile()=0;
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

    virtual bool            SaveAllInformationToDatabase();
};

//*********************************************************************************************************************************************
// Image file
//*********************************************************************************************************************************************
class cUnmanagedFile : public cBaseMediaFile {
public:
    explicit cUnmanagedFile(cBaseApplicationConfig *ApplicationConfig);

    virtual QString         GetFileTypeStr();
    virtual bool            LoadBasicInformationFromDatabase(QDomElement)       {return true;}
    virtual void            SaveBasicInformationToDatabase(QDomElement *)       {/*Nothing to do*/}
    virtual bool            GetFullInformationFromFile()                        {return SaveAllInformationToDatabase();}
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size)     { return ApplicationConfig->DefaultFILEIcon.GetIcon(Size); }
    virtual QString         GetTechInfo()                                       { return ""; }
    virtual QString         GetTAGInfo()                                        { return ""; }
};

//*********************************************************************************************************************************************
// Image file
//*********************************************************************************************************************************************
class cFolder : public cBaseMediaFile {
public:
    explicit cFolder(cBaseApplicationConfig *ApplicationConfig);

    virtual QString         GetFileTypeStr();
    virtual bool            LoadBasicInformationFromDatabase(QDomElement)       {return true;}
    virtual void            SaveBasicInformationToDatabase(QDomElement *)       {/*Nothing to do*/}
    virtual bool            GetFullInformationFromFile();
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size)     { return ApplicationConfig->DefaultFOLDERIcon.GetIcon(Size); }
    virtual QString         GetTechInfo()                                       { return ""; }
    virtual QString         GetTAGInfo()                                        { return ""; }
};

//*********************************************************************************************************************************************
// ffDiaporama project file
//*********************************************************************************************************************************************
class cffDProjectFile : public cBaseMediaFile {
public:
    // TAG values
    QString     Title;              // 30 or 200 char depending on ID3V2 compatibility option
    QString     Author;             // 30 or 200 char depending on ID3V2 compatibility option
    QString     Album;              // 30 or 200 char depending on ID3V2 compatibility option
    QDate       EventDate;
    bool        OverrideDate;
    QString     LongDate;           // Project dates
    QString     Comment;            // Free text - free size
    QString     Composer;           // ffDiaporama version
    int         NbrSlide;           // (Number of slide in project)
    QString     ffDRevision;        // ffD Application version (in reverse date format)
    QString     DefaultLanguage;    // Default Language (ISO 639 language code)
    int         NbrChapters;        // Number of chapters in the file

    explicit cffDProjectFile(cBaseApplicationConfig *ApplicationConfig);

    void                    InitDefaultValues();

    virtual QString         GetFileTypeStr();
    virtual bool            LoadBasicInformationFromDatabase(QDomElement root);
    virtual void            SaveBasicInformationToDatabase(QDomElement *root);
    virtual bool            GetFullInformationFromFile();
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
    bool                    IsVectorImg,NoExifData;
    QSvgRenderer            *VectorImage;

    explicit                cImageFile(cBaseApplicationConfig *ApplicationConfig);
                            ~cImageFile();

    virtual bool            GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag,qlonglong FolderKey);
    virtual QImage          *ImageAt(bool PreviewMode);
    virtual QString         GetFileTypeStr();
    virtual bool            LoadBasicInformationFromDatabase(QDomElement root);
    virtual void            SaveBasicInformationToDatabase(QDomElement *root);
    virtual bool            GetFullInformationFromFile();
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
    int64_t     Position;
    AVFrame     *FiltFrame,*FrameBufferYUV;
    cImageInCache(int64_t Position,AVFrame *FiltFrame,AVFrame *FrameBufferYUV);
    ~cImageInCache();
};

class cVideoFile : public cBaseMediaFile {
public:
    bool                    IsOpen;                             // True if Libav open on this file
    bool                    MusicOnly;                          // True if object is a music only file
    bool                    IsVorbis;                           // True if vorbis version must be use instead of MP3/WAV version
    bool                    IsMTS;                              // True if file is a MTS file
    QTime                   StartPos;                           // Start position
    QTime                   EndPos;                             // End position
    QString                 Container;                          // Container type (get from file extension)
    QString                 VideoCodecInfo;
    QString                 AudioCodecInfo;
    int                     NbrChapters;                        // Number of chapters in the file
    AVFormatContext         *LibavAudioFile,*LibavVideoFile;    // LibAVFormat contexts
    int64_t                 LastAudioReadedPosition;            // Use to keep the last readed position to determine if a seek is needed
    int64_t                 LastVideoReadedPosition;            // Use to keep the last readed position to determine if a seek is needed

    // Video part
    AVCodec                 *VideoDecoderCodec;         // Associated LibAVCodec for video stream
    int                     VideoStreamNumber;          // Number of the video stream
    int                     VideoTrackNbr;              // Number of video stream in file
    AVFrame                 *FrameBufferYUV;
    bool                    FrameBufferYUVReady;        // true if FrameBufferYUV is ready to convert
    int64_t                 FrameBufferYUVPosition;     // If FrameBufferYUV is ready to convert then keep FrameBufferYUV position
    QImage                  LastImage;                  // Keep last image return
    QList<cImageInCache *>  CacheImage;

    // Audio part
    AVCodec                 *AudioDecoderCodec;         // Associated LibAVCodec for audio stream
    int                     AudioStreamNumber;          // Number of the audio stream
    int                     AudioTrackNbr;              // Number of audio stream in file

    // Audio resampling
    #if defined(LIBAV) && (LIBAVVERSIONINT<=8)
    ReSampleContext         *RSC;
    #elif defined(LIBAV) && (LIBAVVERSIONINT<=9)
    AVAudioResampleContext  *RSC;
    uint64_t                RSC_InChannelLayout,RSC_OutChannelLayout;
    #elif defined(FFMPEG)
    SwrContext              *RSC;
    uint64_t                RSC_InChannelLayout,RSC_OutChannelLayout;
    #endif
    int                     RSC_InChannels,RSC_OutChannels;
    int                     RSC_InSampleRate,RSC_OutSampleRate;
    AVSampleFormat          RSC_InSampleFmt,RSC_OutSampleFmt;

    explicit                cVideoFile(int WantedObjectType,cBaseApplicationConfig *ApplicationConfig);
                            ~cVideoFile();
    virtual void            Reset(int TheWantedObjectType);

    virtual QString         GetFileTypeStr();
    virtual bool            LoadBasicInformationFromDatabase(QDomElement root);
    virtual void            SaveBasicInformationToDatabase(QDomElement *root);
    virtual bool            GetFullInformationFromFile();
    virtual QImage          *GetDefaultTypeIcon(cCustomIcon::IconSize Size);

    virtual QString         GetTechInfo();
    virtual QString         GetTAGInfo();

    virtual int             getThreadFlags(AVCodecID ID);

    virtual bool            OpenCodecAndFile();
    virtual void            CloseCodecAndFile();

    virtual QImage          *ImageAt(bool PreviewMode,int64_t Position,cSoundBlockList *SoundTrackMontage,bool Deinterlace,double Volume,bool ForceSoundOnly,bool DontUseEndPos);
    virtual QImage          *ReadFrame(bool PreviewMode,int64_t Position,bool DontUseEndPos,bool Deinterlace,cSoundBlockList *SoundTrackBloc,double Volume,bool ForceSoundOnly);
    virtual QImage          *ConvertYUVToRGB(bool PreviewMode,AVFrame *Frame);

    virtual bool            SeekFile(AVStream *VideoStream,AVStream *AudioStream,int64_t Position);
    virtual void            CloseResampler();
    virtual void            CheckResampler(int RSC_InChannels,int RSC_OutChannels,AVSampleFormat RSC_InSampleFmt,AVSampleFormat RSC_OutSampleFmt,int RSC_InSampleRate,int RSC_OutSampleRate
                                                #if (defined(LIBAV)&&(LIBAVVERSIONINT>=9)) || defined(FFMPEG)
                                                   ,uint64_t RSC_InChannelLayout,uint64_t RSC_OutChannelLayout
                                                #endif
                                          );
    virtual u_int8_t        *Resample(AVFrame *Frame,int64_t *SizeDecoded,int DstSampleSize);

    //*********************
    // video filters part
    //*********************
    AVFilterGraph           *VideoFilterGraph;
    AVFilterContext         *VideoFilterIn;
    AVFilterContext         *VideoFilterOut;

    virtual int             VideoFilter_Open();
    virtual void            VideoFilter_Close();
    #if defined(LIBAV) || (FFMPEGVERSIONINT<201)
        virtual int         VideoFilter_Process();
    #endif
};

//*********************************************************************************************************************************************
// Music file
//*********************************************************************************************************************************************

class cMusicObject : public cVideoFile {
public:
    double              Volume;                 // Volume as % from 10% to 150%

    cMusicObject(cBaseApplicationConfig *ApplicationConfig);

    virtual bool            LoadBasicInformationFromDatabase(QDomElement root);
    virtual void            SaveBasicInformationToDatabase(QDomElement *root);

    void                    SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath,cReplaceObjectList *ReplaceList);
    bool                    LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag);
    bool                    LoadMedia(QString &filename,QStringList *AliasList,bool *ModifyFlag);
};

#endif // CBASEMEDIAFILE_H
