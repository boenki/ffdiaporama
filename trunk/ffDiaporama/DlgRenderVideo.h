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

#ifndef DLGRENDERVIDEO_H
#define DLGRENDERVIDEO_H

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
    #include <libavutil/common.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}
#endif

#include <QDialog>
#include <QFuture>
#include "cdiaporama.h"

namespace Ui {
    class DlgRenderVideo;
}

class DlgRenderVideo : public QDialog {
Q_OBJECT
public:
    cDiaporama      *Diaporama;
    bool            StopProcessWanted;              // True if user click on cancel or close during encoding process
    int             Extend;                         // amout of padding (top and bottom) for cinema mode with DVD
    int             VideoCodecIndex;                // Index of video codec
    int             AudioCodecIndex;                // Index of audio codec
    double          FPS;

    // Display control
    int             NbrFrame;                       // Number of frame to generate
    int             Position;                       // Render current position
    int             ColumnStart;                    // Render start position of current object
    int             Column;                         // Render current object
    int             RenderedFrame;                  // Real number of rendered frame (delayed frame included)
    QTime           StartTime;                      // Time the process start
    QTime           LastCheckTime;                  // Last time the loop start


    // Sound blocs
    cSoundBlockList MusicTrack;                     // Sound for playing sound from music track
    cSoundBlockList PreviousMusicTrack;             // Sound for playing sound from previous music track
    cSoundBlockList MixedMusic;                     // Sound to play
    cSoundBlockList EncodedAudio;

    // Select dest file values
    QString         FileFormat;

    // LIBAV part
    bool            IsDestFileOpen;
    AVFormatContext *OutputFormatContext;
    AVStream        *VideoStream;
    AVStream        *AudioStream;
    AVCodecContext  *VideoCodecContext;
    AVCodecContext  *AudioCodecContext;
    AVCodec         *VideoCodec;
    AVCodec         *AudioCodec;

    // Thread control
    QFuture<void>   ThreadPrepareMusic;         // Thread preparation of music
    QFuture<void>   ThreadDoAssembly;           // Thread for make assembly of background and images
    QFuture<bool>   ThreadWriteVideoFrame;      // Thread write video frame
    bool            IsThreadWriteVideoFrame;    // true if ThreadWriteVideoFrame was previously started
    QFuture<bool>   ThreadWriteAudioFrame;      // Thread write audio frame
    bool            IsThreadWriteAudioFrame;    // true if ThreadWriteAudioFrame was previously started

    explicit DlgRenderVideo(cDiaporama &Diaporama,QWidget *parent = 0);
    ~DlgRenderVideo();

protected:
    virtual void showEvent(QShowEvent *);
    virtual void reject();
    virtual void accept();

private slots:
    void        SetSavedWindowGeometry();
    void        InitImageSizeCombo(int);
    void        SelectDestinationFile();
    void        AdjustDestinationFile();
    void        FileFormatCombo(int);
    void        InitVideoBitRateCB(int);
    void        InitAudioBitRateCB(int);

private:
    QImage  *PrepareImage(int Column,int Position);

    bool    OpenDestFile(int VideoCodecIndex,int AudioCodecIndex);
    bool    CreateVideoStream();
    bool    CreateAudioStream();

    bool    WriteVideoFrame(int FrameNumber,AVFrame *VideoFramePicture,int Width,int Height);
    bool    WriteAudioFrame(int16_t *Buffer,int AudioLen);
    void    flush_ffmpeg_VideoStream(int Width,int Height);

    void    RefreshDisplayControl();

    AVFrame *QImageToYUVStream(QImage *Image);

    Ui::DlgRenderVideo *ui;
};

#endif // DLGRENDERVIDEO_H
