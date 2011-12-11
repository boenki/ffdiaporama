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
#include "_ImagesDefinitions.h"

//*********************************************************************************************************************************************
// video file wrapper : encapsulate all stream type objects
//*********************************************************************************************************************************************

class cvideofilewrapper {
public:
    bool                    IsValide;                   // if true then object if fuly initialise
    int                     ObjectGeometry;             // Image geometry of the embeded image or video
    bool                    MusicOnly;                  // True if object is a music only file
    bool                    IsVorbis;                   // True if vorbis version must be use instead of MP3/WAV version
    QString                 FileName;                   // filename
    QTime                   StartPos;                   // Start position
    QTime                   EndPos;                     // End position
    QString                 Container;                  // Container type (get from file extension)
    QTime                   Duration;                   // Duration of the video
    QDateTime               CreatDateTime;              // Original date/time
    QDateTime               ModifDateTime;              // Last modified date/time
    int                     ImageWidth;                 // Widht of normal image
    int                     ImageHeight;                // Height of normal image
    QImage                  *CacheFirstImage;           // Cache image of first image of the video (Preview mode only)
    QImage                  *CacheLastImage;            // Cache image of last image of the video (Preview mode only)
    double                  dEndFileCachePos;           // Position of the cache image of last image of the video
    bool                    CodecUsePTS;                // true if codec use PTS (h264) if if we use only DTS
    cFilterTransformObject  BrushFileTransform;         // Image transformation if image from disk
    double                  AspectRatio;                // Aspect ratio

    // Video part
    AVFormatContext         *ffmpegVideoFile;           // LibAVFormat context
    AVCodec                 *VideoDecoderCodec;         // Associated LibAVCodec for video stream
    int                     VideoStreamNumber;          // Number of the video stream
    AVFrame                 *FrameBufferYUV;
    bool                    FrameBufferYUVReady;        // true if FrameBufferYUV is ready to convert
    int64_t                 FrameBufferYUVPosition;     // If FrameBufferYUV is ready to convert then keep FrameBufferYUV position

    // Audio part
    AVFormatContext         *ffmpegAudioFile;           // LibAVFormat context
    AVCodec                 *AudioDecoderCodec;         // Associated LibAVCodec for audio stream
    int                     AudioStreamNumber;          // Number of the audio stream
    int64_t                 LastAudioReadedPosition;    // Use to keep the last readed position to determine if a seek is needed

    cvideofilewrapper();
    ~cvideofilewrapper();

    void        CloseCodecAndFile();

    bool        GetInformationFromFile(QString GivenFileName,bool MusicOnly,QStringList &AliasList);
    QString     GetVideoCodecName(QString Codec);
    QString     GetContainerName(QString Codec);
    QString     GetMovieGeometryName(QString Geometry);
    QString     GetSizeName(QString Size,int X,int Y);

    QImage      *ImageAt(bool PreviewMode,qlonglong Position,qlonglong StartPosToAdd,bool ForceLoadDisk,cSoundBlockList *SoundTrackMontage,double Volume,bool ForceSoundOnly,cFilterTransformObject *Filter,bool DontUseEndPos=false);
    QImage      *ReadVideoFrame(qlonglong Position,bool DontUseEndPos);
    void        ReadAudioFrame(bool PreviewMode,qlonglong Position,cSoundBlockList *SoundTrackBloc,double Volume,bool DontUseEndPos);      // MP3 and WAV
    QImage      *ConvertYUVToRGB();
};

#endif // VIDEOFILEWRAPPER_H
