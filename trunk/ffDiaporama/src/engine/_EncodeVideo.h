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

#ifndef _ENCODEVIDEO_H
#define _ENCODEVIDEO_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "../ffDiaporama/_Diaporama.h"
#include "../ffDiaporama/_ApplicationDefinitions.h"

class cEncodeVideo {
public:
    cDiaporama      *Diaporama;
    bool            IsOpen;
    QString         OutputFileName;
    sFormatDef      *FormatDef;                     // Link to FormatDef or NULL if error
    sVideoCodecDef  *VideoCodecDef;                 // Link to VideoCodecDef or NULL if no video part
    sAudioCodecDef  *AudioCodecDef;                 // Link to AudioCodecDef or NULL if no audio part

    AVOutputFormat  OutputFormat;
    AVCodec         *VideoCodec,*AudioCodec;
    AVFormatContext *Container;
    AVStream        *VideoStream,*AudioStream;
    int             VideoStreamNbr,AudioStreamNbr;  // Stream number

    // Audio buffers
    AVFrame         *AudioFrame;

    // Video buffers
    QList<AVFrame*> VideoFrameList;
    uint8_t         *EncodeBuffer;
    int             EncodeBufferSize;

    // Global settings
    int             InternalWidth,InternalHeight;
    int             FromSlide,ToSlide;

    // Video parameters
    double          VideoFrameRate;

    // Audio parameters
    int             AudioChannels;
    int             AudioBitrate;
    int             AudioSampleRate;

    bool            StopProcessWanted;              // True if user click on cancel or close during encoding process
    qlonglong       NbrFrame;                       // Number of frame to generate
    int64_t         CurAudioPts,CurVideoPts;
    QTime           StartTime;                      // Time the process start
    QTime           LastCheckTime;                  // Last time the loop start

    uint8_t                 *RPCK;
    #ifdef USELIBAVRESAMPLE
    AVAudioResampleContext  *RSC;
    #else
    ReSampleContext         *RSC;
    #endif

    QLabel          *ElapsedTimeLabel,*SlideNumberLabel,*FrameNumberLabel,*FPSLabel;
    QProgressBar    *SlideProgressBar,*TotalProgressBar;

    cEncodeVideo();
    ~cEncodeVideo();

    bool            OpenEncoder(cDiaporama *Diaporama,QString OutputFileName,int FromSlide,int ToSlide,
                        bool EncodeVideo,int VideoCodecId,int VideoCodecSubId,double FPS,int ImageWidth,int ImageHeight,int InternalWidth,int InternalHeight,AVRational PixelAspectRatio,int VideoBitrate,
                        bool EncodeAudio,int AudioCodecId,int AudioChannels,int AudioBitrate,int AudioSampleRate,QString Language);
    bool            DoEncode();
    void            CloseEncoder();

private:
    bool            OpenVideoStream(int VideoCodecSubId,double VideoFrameRate,int ImageWidth,int ImageHeight,AVRational PixelAspectRatio,int VideoBitrate);
    bool            OpenAudioStream(int AudioChannels,int AudioBitrate,int AudioSampleRate,QString Language);
    bool            PrepareTAG(QString Language);
    void            DisplayProgress(qlonglong RenderedFrame,qlonglong Position,int Column,int ColumnStart);
    QString         AdjustMETA(QString Text);
    AVRational      GetCodecTimeBase(const AVCodec *m_avVideoCodec,double FPS);

    void            EncodeMusic(cSoundBlockList *ToEncodeMusic,bool *Continue);
    void            EncodeVideo(cSoundBlockList *ToEncodeMusic,QImage *Image,bool *Continue);
};

#endif // _ENCODEVIDEO_H
