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

#ifndef VIDEOFILEWRAPPER_H
#define VIDEOFILEWRAPPER_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_SoundDefinitions.h"

//*********************************************************************************************************************************************
// Internal object / object list to spool image to decode
//*********************************************************************************************************************************************

class DecodeVideoObject {
public:
    AVPacket    *StreamPacket;
    double      FramePosition;
    AVStream    *VideoStream;
    double      dPosition;
    bool        IsKeyFrame;

    DecodeVideoObject(AVPacket *StreamPacket,double FramePosition,AVStream *VideoStream,double dPosition,bool IsKeyFrame);
    ~DecodeVideoObject();
};

class DecodeVideoObjectList {
public:
    QList<DecodeVideoObject *>  List;

    DecodeVideoObjectList();
    ~DecodeVideoObjectList();

    DecodeVideoObject *DetachFirstPacket();
};

//*********************************************************************************************************************************************
// video file wrapper : encapsulate all stream type objects
//*********************************************************************************************************************************************

class cvideofilewrapper {
public:
    bool                    IsValide;               // if true then object if fuly initialise
    bool                    MusicOnly;              // True if object is a music only file
    QString                 FileName;               // filename
    double                  SoundVolume;            // Volume of soundtrack
    QTime                   StartPos;               // Start position
    QTime                   EndPos;                 // End position
    QString                 Container;              // Container type (get from file extension)
    QTime                   Duration;               // Duration of the video
    QDateTime               CreatDateTime;          // Original date/time
    QDateTime               ModifDateTime;          // Last modified date/time
    int                     ImageWidth;             // Widht of normal image
    int                     ImageHeight;            // Height of normal image
    QImage                  *CacheFirstImage;       // Cache image of first image of the video (Preview mode only)
    DecodeVideoObjectList   VideoObjectList;        // Spool list of DecodeVideoObject
    int                     AdjustTimeStamp;
    bool                    CodecUsePTS;            // true if codec use PTS (h264) if if we use only DTS

    // LibAVFormat/Codec/SWScale part
    AVFormatContext         *ffmpegVideoFile;       // LibAVFormat context
    AVCodec                 *VideoDecoderCodec;     // Associated LibAVCodec for video stream
    AVCodec                 *AudioDecoderCodec;     // Associated LibAVCodec for audio stream
    int                     VideoStreamNumber;      // Number of the video stream
    int                     AudioStreamNumber;      // Number of the audio stream
    double                  AspectRatio;            // Aspect ratio

    int                     NextPacketPosition;     // Use to keep the last NextPacketPosition to determine if a seek is needed

    cvideofilewrapper();
    ~cvideofilewrapper();

    bool        GetInformationFromFile(QString &GivenFileName,bool MusicOnly);
    QImage      *ImageAt(bool PreviewMode,int Position,bool ForceLoadDisk,cSoundBlockList *SoundTrackMontage,double Volume,bool ForceSoundOnly,cFilterTransformObject *Filter);

    int         PreviousPosition;
    bool        IsReadVideoStarted;
    bool        StartReadVideoStream(int Position);
    QProcess    *ReadVideoProcess;
    QImage      *ReadNext(int Position);
    void        StopReadVideoStream();

    QString     GetVideoCodecName(QString Codec);
    QString     GetContainerName(QString Codec);
    QString     GetMovieGeometryName(QString Geometry);
    QString     GetSizeName(QString Size,int X,int Y);

private:
    void        CloseVideoFileReader();
    QImage      *ReadVideoFrame(int Position,cSoundBlockList *SoundTrackMontage,double Volume,bool ForceSoundOnly);
    QImage      *YUVStreamToQImage(double dPosition,bool GetFirstValide);
};

#endif // VIDEOFILEWRAPPER_H
