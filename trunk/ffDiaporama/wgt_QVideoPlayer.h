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

#ifndef WGT_QVIDEOPLAYER_H
#define WGT_QVIDEOPLAYER_H

//====================================================================================================================
// SDL inclusion
//====================================================================================================================
#ifdef __cplusplus
extern "C" {
    #include <SDL/SDL.h>
}
#endif

#ifdef __MINGW32__
#undef main /* We don't want SDL to override our main() */
#endif

#include <QWidget>
#include <QProcess>
#include <QTime>
#include <QIcon>
#include <QTimer>
#include <QFuture>

#include "cdiaporama.h"
#include "cvideofilewrapper.h"

namespace Ui {
    class wgt_QVideoPlayer;
}

#define BUFFERING_NBR_FRAME 5                   // Number of frame wanted in the playing buffer

class cImageList {
public:
    QList<cDiaporamaObjectInfo *>   List;       // List of rendere image
    cDiaporama                      *Diaporama; // Link to the current Diaporama

    cImageList();
    ~cImageList();

    void                    ClearList();                                        // Clear the list (make delete of each image)
    cDiaporamaObjectInfo    *DetachFirstImage();                                // Detach the first image of the list (do not make delete)
    cDiaporamaObjectInfo    *GetFirstImage();                                   // Retreve a link to the first frame in the list
    cDiaporamaObjectInfo    *GetLastImage();                                    // Retreve a link to the last frame in the list
    void                    AppendImage(cDiaporamaObjectInfo *Frame);           // Append an image to the end of the list
};

class wgt_QVideoPlayer : public QWidget {
Q_OBJECT
public:
    bool                SDLIsAudioOpen;         // true if SDL work at least one time
    bool                *FLAGSTOPITEMSELECTION; // Flag to stop Item Selection process in the timeline
    cvideofilewrapper   *FileInfo;
    cDiaporama          *Diaporama;
    cImageList          ImageList;              // Collection of bufered image
    bool                IsValide;               // if true then object if fuly initialise
    bool                IsInit;                 // if true then player was first started
    QTime               StartPos;               // Start position
    QTime               EndPos;                 // End position
    QIcon               IconPlay;               // Icon : "images/player_play.png"
    QIcon               IconPause;              // Icon : "images/player_pause.png"
    bool                DisplayMSec;            // if True, display millisecondes instead of secondes
    bool                MplayerPlayMode;        // Is MPlayer currently play mode
    bool                MplayerPauseMode;       // Is MPlayer currently plause mode
    QProcess            MplayerProcess;         // MPlayer process
    QString             PathMPLAYER;            // FileName of mplayer (with path)
    QTimer              Timer;
    bool                IsSliderProcess;        // true is slider is currently moving by user
    int                 ActualPosition;         // Current position (in msec)
    bool                PreviousPause;          // Flag to keep pause state before slider process
    QTime               tDuration;              // Duration of the video
    QTime               LastTimeCheck;          // time save for plaing diaporama
    bool                Flag_InTimer;           // true if timer event is in progress
    bool                TimerTick;              // To use timer 1 time for 2 call

    // Thread control for preparing next frame
    QFuture<void>       ThreadPrepareMontage;   // Thread preparation of image
    QFuture<void>       ThreadPrepareMusic;     // Thread preparation of music
    QFuture<void>       ThreadDoAssembly;       // Thread for make assembly of background and images

    SDL_AudioSpec       AudioSpec;
    cSoundBlockList     MusicTrack;             // Sound for playing sound from music track
    cSoundBlockList     PreviousMusicTrack;     // Sound for playing sound from previous music track
    cSoundBlockList     MixedMusic;             // Sound to play

    explicit wgt_QVideoPlayer(QWidget *parent = 0);
    ~wgt_QVideoPlayer();

    void    Clean();
    bool    InitDiaporamaPlay(cDiaporama *Diaporama);
    bool    StartPlay(cvideofilewrapper *FileInfo);
    void    SendToMPlayer(QString Message);
    void    SetStartEndPos(int StartPos,int Duration,int PreviousStartPos,int PrevisousEndPos,int NextStartPos,int NextEndPos);
    void    SeekPlayer(int Value);
    QTime   GetCurrentPos();
    QTime   GetActualDuration();
    void    SetActualDuration(int Duration);

    void    SetPlayerToPause();
    void    SetPlayerToPlay();
    void    SetBackgroundColor(QColor Background);
    void    Resize();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void closeEvent(QCloseEvent *);
    virtual void showEvent(QShowEvent *);

private slots:
    void    s_TimerEvent();
    void    s_MPlayerMessage();
    void    s_MPlayerFinished(int,QProcess::ExitStatus);
    void    s_VideoPlayerPlayPauseBT();
    void    s_SliderPressed();
    void    s_SliderReleased();
    void    s_SliderMoved(int Value);

private:
    void    EnableWidget(bool State);
    void    PrepareImage(cDiaporamaObjectInfo *Frame,bool SoundWanted);
    void    PrepareMusicBloc(int Column,int Position);

    Ui::wgt_QVideoPlayer *ui;
};

#endif // WGT_QVIDEOPLAYER_H
