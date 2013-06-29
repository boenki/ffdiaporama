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
#include "_Diaporama.h"

#define SUPPORTED_COMBINATION       0
#define UNSUPPORTED_COMBINATION     1
#define INVALID_COMBINATION         2

int CheckEncoderCapabilities(VFORMAT_ID FormatId,CodecID VideoCodec,CodecID AudioCodec);

//************************************************

class cEncodeVideo {
public:
    cDiaporama          *Diaporama;
    bool                IsOpen;

    // Container parameters & buffers
    QString             OutputFileName;
    int                 FromSlide,ToSlide;
    qlonglong           NbrFrame;                       // Number of frame to generate
    AVFormatContext     *Container;

    // Video parameters & buffers
    AVRational          VideoFrameRate;
    int                 VideoBitrate;
    int                 VideoCodecSubId;
    sIMAGEDEF           *ImageDef;

    int64_t             LastVideoPts;
    int64_t             IncreasingVideoPts;
    int64_t             VideoFrameNbr;
    AVStream            *VideoStream;
    double              dFPS;
    int                 InternalWidth,InternalHeight,ExtendV;
    AVFrame             *VideoFrame;
    struct SwsContext   *VideoFrameConverter;           // Converter from QImage to YUV image
    u_int8_t            *VideoEncodeBuffer;             // Buffer for encoded image
    int                 VideoEncodeBufferSize;          // Buffer for encoded image
    int64_t             VideoFrameBufSize;
    u_int8_t            *VideoFrameBuf;

    // Audio parameters & buffers
    int                 AudioChannels;
    int                 AudioBitrate;
    int                 AudioSampleRate;
    int                 AudioCodecSubId;

    int64_t             LastAudioPts;
    int64_t             IncreasingAudioPts;
    int64_t             AudioFrameNbr;
    AVStream            *AudioStream;
    AVFrame             *AudioFrame;
    u_int8_t            *AudioResamplerBuffer;          // Buffer for sampled audio
    int                 AudioResamplerBufferSize;
    #if defined(LIBAV_08)
    ReSampleContext         *AudioResampler;            // Audio resampler
    #elif defined(USELIBSWRESAMPLE)
    SwrContext              *AudioResampler;
    #elif defined(USELIBAVRESAMPLE)
    AVAudioResampleContext  *AudioResampler;
    #else
    ReSampleContext         *AudioResampler;            // Audio resampler
    #endif

    // Progress display settings
    bool                StopProcessWanted;              // True if user click on cancel or close during encoding process
    QTime               StartTime;                      // Time the process start
    QTime               LastCheckTime;                  // Last time the loop start
    QLabel              *InfoLabelA1,*InfoLabelA2,*InfoLabelA3,*InfoLabelA4;
    QLabel              *InfoLabelB1,*InfoLabelB2,*InfoLabelB3,*InfoLabelB4;
    QLabel              *ElapsedTimeLabel,*SlideNumberLabel,*FrameNumberLabel,*FPSLabel,*SlideProgressBarLabel;
    QProgressBar        *SlideProgressBar,*TotalProgressBar;

    cEncodeVideo();
    ~cEncodeVideo();

    bool            OpenEncoder(cDiaporama *Diaporama,QString OutputFileName,int FromSlide,int ToSlide,
                        bool EncodeVideo,int VideoCodecSubId,sIMAGEDEF *ImageDef,int ImageWidth,int ImageHeight,int ExtendV,int InternalWidth,int InternalHeight,AVRational PixelAspectRatio,int VideoBitrate,
                        bool EncodeAudio,int AudioCodecSubId,int AudioChannels,int AudioBitrate,int AudioSampleRate,QString Language);
    bool            DoEncode();
    void            CloseEncoder();

private:
    void            InitDisplay();
    void            DisplayProgress(qlonglong RenderedFrame,qlonglong Position,int Column,int ColumnStart);

    int             getThreadFlags(CodecID codecId);
    bool            AddStream(AVStream **Stream,AVCodec **codec,const char *CodecName,AVMediaType Type);
    bool            OpenVideoStream(sVideoCodecDef *VideoCodecDef,int VideoCodecSubId,AVRational VideoFrameRate,int ImageWidth,int ImageHeight,AVRational PixelAspectRatio,int VideoBitrate);
    bool            OpenAudioStream(sAudioCodecDef *AudioCodecDef,int &AudioChannels,int &AudioBitrate,int &AudioSampleRate,QString Language);

    bool            PrepareTAG(QString Language);
    QString         AdjustMETA(QString Text);

    void            EncodeMusic(cSoundBlockList *ToEncodeMusic,bool &Continue);
    void            EncodeVideo(QImage *ImageList,bool &Continue);
};

#endif // _ENCODEVIDEO_H
