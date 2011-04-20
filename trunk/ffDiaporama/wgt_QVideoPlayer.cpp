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

#include "wgt_QVideoPlayer.h"
#include "ui_wgt_QVideoPlayer.h"

//====================================================================================================================

void AudioCallback(void *userdata, Uint8 *stream, int len) {
    wgt_QVideoPlayer *VideoPlayer=(wgt_QVideoPlayer *)userdata;

    if (len!=VideoPlayer->MixedMusic.SoundPacketSize) {
        return;
    }
    int16_t *Packet=VideoPlayer->MixedMusic.DetachFirstPacket();

    // Copy data to hardware buffer
    if (Packet!=NULL) {
        memcpy(stream,(Uint8 *)Packet,VideoPlayer->MixedMusic.SoundPacketSize);
        av_free(Packet);
    }
}


//*********************************************************************************************************************************************
// Base object for image manipulation
//*********************************************************************************************************************************************

cImageList::cImageList() {
}

//====================================================================================================================

cImageList::~cImageList() {
    ClearList();
}

//====================================================================================================================
// Clear the list (make av_free of each packet)
//====================================================================================================================
void cImageList::ClearList() {
    while (List.count()>0) {
        cDiaporamaObjectInfo *Frame=DetachFirstImage();
        if (Frame) delete(Frame);
    }
}

//====================================================================================================================
// Detach the first image of the list (do not make delete)
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::DetachFirstImage() {
    if (List.count()>0) return (cDiaporamaObjectInfo *)List.takeFirst(); else return NULL;
}

//====================================================================================================================
// Retreve a link to the first frame in the list
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::GetFirstImage() {
    if (List.count()>0) return (cDiaporamaObjectInfo *)List[0]; else return NULL;
}

//====================================================================================================================
// Retreve a link to the last frame in the list
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::GetLastImage() {
    if (List.count()>0) return (cDiaporamaObjectInfo *)List[List.count()-1]; else return NULL;
}

//====================================================================================================================
// Append a packet to the end of the list
//====================================================================================================================
void cImageList::AppendImage(cDiaporamaObjectInfo *Frame) {
    List.append(Frame);
}

//====================================================================================================================

wgt_QVideoPlayer::wgt_QVideoPlayer(QWidget *parent) : QWidget(parent),ui(new Ui::wgt_QVideoPlayer) {
    ui->setupUi(this);
    SDLIsAudioOpen          = false;
    FLAGSTOPITEMSELECTION   = NULL;
    FileInfo                = NULL;
    Diaporama               = NULL;
    IsValide                = false;
    Flag_InTimer            = false;
    IsInit                  = false;

    DisplayMSec             = false;
    IconPlay                = QIcon("icons/player_play.png");
    IconPause               = QIcon(ICON_PLAYERPAUSE);
    MplayerPlayMode         = false;                                // Is MPlayer currently play mode
    MplayerPauseMode        = false;                                // Is MPlayer currently plause mode
    IsSliderProcess         = false;
    ActualPosition          = -1;

    ui->Position->setFixedWidth(DisplayMSec?160:120);
    ui->BufferState->setFixedWidth(DisplayMSec?160:120);
    ui->Position->setText(QTime(0,0,0,0).toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
    this->FileInfo      = FileInfo;
    ui->CustomRuller->ActiveSlider(0);

    ui->SpacerV1->setText("");
    ui->SpacerH1->setText("");
    ui->SpacerH2->setText("");

    ui->SpacerV1->setAttribute(Qt::WA_TranslucentBackground);   ui->SpacerV1->setWindowFlags(Qt::FramelessWindowHint);
    ui->SpacerH1->setAttribute(Qt::WA_TranslucentBackground);   ui->SpacerH1->setWindowFlags(Qt::FramelessWindowHint);
    ui->SpacerH2->setAttribute(Qt::WA_TranslucentBackground);   ui->SpacerH2->setWindowFlags(Qt::FramelessWindowHint);

    ui->MovieFrame->setText("");
    ui->MovieFrame->setAttribute(Qt::WA_OpaquePaintEvent);

    connect(&Timer,SIGNAL(timeout()),this,SLOT(s_TimerEvent()));
    connect(ui->VideoPlayerPlayPauseBT,SIGNAL(clicked()),this,SLOT(s_VideoPlayerPlayPauseBT()));

    // Slider control
    connect(ui->CustomRuller->Slider,SIGNAL(sliderPressed()),this,SLOT(s_SliderPressed()));
    connect(ui->CustomRuller->Slider,SIGNAL(sliderReleased()),this,SLOT(s_SliderReleased()));
    connect(ui->CustomRuller->Slider,SIGNAL(sliderMoved(int)),this,SLOT(s_SliderMoved(int)));

    // MPlayer signals
    connect(&MplayerProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(s_MPlayerMessage()));
    connect(&MplayerProcess,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(s_MPlayerFinished(int,QProcess::ExitStatus)));

    // Start SDL
    if (SDL_Init(SDL_INIT_AUDIO)) {
        qDebug()<<"wgt_QVideoPlayer::SetPlayerToPlay=Could not initialize SDL - "<<SDL_GetError();
        exit(1);
    }
}

//============================================================================================

wgt_QVideoPlayer::~wgt_QVideoPlayer() {
    ThreadPrepareMontage.waitForFinished();   // Thread preparation of image
    ThreadDoAssembly.waitForFinished();       // Thread for make assembly of background and images
    if (MplayerProcess.state()==QProcess::Running) {
        MplayerProcess.kill();
        MplayerProcess.waitForFinished();
    }
    Clean();
    if (SDLIsAudioOpen) SDL_Quit();
    delete ui;
}

//============================================================================================

void wgt_QVideoPlayer::SendToMPlayer(QString Message) {
    if (MplayerProcess.state()==QProcess::Running) MplayerProcess.write(Message.toLocal8Bit());
}

//============================================================================================

void wgt_QVideoPlayer::closeEvent(QCloseEvent *) {
    if (IsValide) Clean();
}

//====================================================================================================================

void wgt_QVideoPlayer::showEvent(QShowEvent *) {
    if ((!IsInit)&&(Diaporama==NULL)) {
        SetPlayerToPlay();
        IsInit=true;
    }
}

//============================================================================================

void wgt_QVideoPlayer::Clean() {
    SetPlayerToPause(); // Stop sound

    if (IsValide) {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        SendToMPlayer("stop\n");
        //if (!MplayerProcess.waitForFinished()) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error stoping MPlayer !","Error message"),QMessageBox::Close);
        IsValide=false;
        QApplication::restoreOverrideCursor();
    }
}

//============================================================================================

void wgt_QVideoPlayer::resizeEvent(QResizeEvent *) {
    Resize();
}

void wgt_QVideoPlayer::Resize() {
    if ((FileInfo==NULL)&&(Diaporama==NULL)) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int HeightBT=ui->VideoPlayerPlayPauseBT->height();
    int TheHeight=height()-HeightBT;
    int TheWidth =0;
    int ToSet    =0;

    if ((FileInfo!=NULL)&&(FileInfo->IsValide)) {
        TheWidth=int(double(TheHeight)*(double(FileInfo->ImageWidth)/double(FileInfo->ImageHeight)));
        if (TheWidth>width()) {
            TheWidth=width();
            TheHeight=int(double(TheWidth)*(double(FileInfo->ImageHeight)/double(FileInfo->ImageWidth)));
        }
    } else if (Diaporama!=NULL) {
        TheWidth=Diaporama->GetWidthForHeight(TheHeight);
        if (TheWidth>width()) {
            TheWidth =width();
            TheHeight=Diaporama->GetHeightForWidth(TheWidth);
        }
    }
    ToSet=height()-TheHeight-HeightBT;
    ui->SpacerV1->setFixedHeight(ToSet);
    ToSet=width()-TheWidth;
    ui->SpacerH1->setFixedWidth(ToSet/2);
    ui->SpacerH2->setFixedWidth(ToSet-(ToSet/2));
    ui->MovieFrame->setFixedWidth(TheWidth);
    ui->MovieFrame->setFixedHeight(TheHeight);

    ui->CustomRuller->setFixedWidth(TheWidth-ui->VideoPlayerPlayPauseBT->width()-ui->Position->width());
    if (ui->CustomRuller->Slider!=NULL) ui->CustomRuller->Slider->setFixedWidth(TheWidth-ui->VideoPlayerPlayPauseBT->width()-ui->Position->width());
    if (ActualPosition!=-1) {
        int aActualPosition=ActualPosition;
        ActualPosition=-1;
        s_SliderMoved(aActualPosition);
    } else {
        ActualPosition=-1;
        s_SliderMoved(0);
    }
    QApplication::restoreOverrideCursor();
}

//============================================================================================

void wgt_QVideoPlayer::SetBackgroundColor(QColor Background) {
    QString Sheet=QString("background-color: rgb(%1,%2,%3);").arg(Background.red(),10).arg(Background.green(),10).arg(Background.blue(),10);
}

//============================================================================================

void wgt_QVideoPlayer::EnableWidget(bool State) {
    if (ui->CustomRuller->Slider!=NULL) ui->CustomRuller->Slider->setEnabled(State);
}

//============================================================================================
// Init a diaporama show
//============================================================================================

bool wgt_QVideoPlayer::InitDiaporamaPlay(cDiaporama *Diaporama) {
    if (Diaporama==NULL) return false;
    this->Diaporama     =Diaporama;
    ImageList.Diaporama =Diaporama;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui->CustomRuller->ActiveSlider(Diaporama->GetDuration());
    MplayerPlayMode  = true;
    MplayerPauseMode = true;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPause);

    QApplication::restoreOverrideCursor();
    return true;
}

//============================================================================================
// Init a video show
//============================================================================================

bool wgt_QVideoPlayer::StartPlay(cvideofilewrapper *FileInfo) {
    if (FileInfo==NULL) return false;
    this->FileInfo=FileInfo;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui->CustomRuller->ActiveSlider(QTime(0,0,0,0).msecsTo(FileInfo->Duration));
    resizeEvent(NULL);

    QString Commande;

    // Compute MPlayer commande
    #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    PathMPLAYER = "mplayer/mplayer.exe";
    #else
    PathMPLAYER = "/usr/bin/mplayer";
    #endif
    Commande=PathMPLAYER+" -osdlevel 0 -slave -quiet -wid ";

    #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    Commande=Commande+QString().number(ui->MovieFrame->winId(),16);

    // AERO Flag for MPlayer (Vista/Windows 7)
    if (QSysInfo().WindowsVersion>=0x0080) Commande=Commande+" -vo gl:glfinish";
    else                                   Commande=Commande+" -vo directx:noaccel";
    #else
    Commande=Commande+QString().number(ui->MovieFrame->winId(),10);
    #endif

    Commande=Commande+" \""+FileInfo->FileName+"\"";
    MplayerProcess.start(Commande);
    if (!MplayerProcess.waitForStarted()) {
        QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error starting MPlayer. Please check configuration !","Error message"),QMessageBox::Close);
        Clean();
        QApplication::restoreOverrideCursor();
        return false;
    }
    QApplication::restoreOverrideCursor();
    return true;
}

//============================================================================================
// Trap mplayer messages
//============================================================================================

void wgt_QVideoPlayer::s_MPlayerMessage() {
    QString Message;
    while ((MplayerProcess.state()==QProcess::Running)&&(MplayerProcess.canReadLine())) {

        // Get Line
        Message=QString(MplayerProcess.readLine());         // Read a line
        Message.replace("\n","");                           // Remove end of line

        if (Message.startsWith("MPlayer")) {


        } if (Message.startsWith("Playing")) {

            if (!IsValide) SendToMPlayer("get_time_length\n");      // ask mplayer for movie length

        } else if (Message.startsWith("ANS_LENGTH")) {

            // Answer to get_time_length : ANS_LENGTH=xx.yy
            Message=Message.mid(QString("ANS_LENGTH=").length(),Message.length()-QString("ANS_LENGTH=").length());
            Message.replace("'","");
            Message.replace(" ","");
            Message.replace("\r","");
            bool Ok=false;
            long Duration=Message.toDouble(&Ok)*1000;
            if (Ok) {
                ui->CustomRuller->Slider->setMaximum(int(Duration)-1);
                ui->CustomRuller->TotalDuration=Duration;
                ui->CustomRuller->repaint();

                int     TimeMSec    =(Duration %1000);
                int     TimeSec     =int(Duration/1000);
                int     TimeHour    =TimeSec/(60*60);
                int     TimeMinute  =(TimeSec%(60*60))/60;
                tDuration.setHMS(TimeHour,TimeMinute,TimeSec%60,TimeMSec);
                SendToMPlayer("get_time_pos\n");      // ask mplayer for time position

                IsValide=true;
                EnableWidget(true);
            }

            MplayerPlayMode  = true;
            MplayerPauseMode = false;
            ui->VideoPlayerPlayPauseBT->setIcon(IconPlay);
            ui->Position->setFixedWidth(DisplayMSec?160:120);
            ui->BufferState->setFixedWidth(DisplayMSec?160:120);

            // Start Timer
            Timer.start(100);

            // Switch player to pause !
            SetPlayerToPause();

        } else if (Message.startsWith("ANS_TIME_POSITION")) {

            // Answer to get_time_pos : ANS_TIME_POSITION=xx.y
            if (IsValide) {
                Message=Message.mid(QString("ANS_TIME_POSITION=").length(),Message.length()-QString("ANS_TIME_POSITION=").length());
                Message.replace("'","");
                Message.replace(" ","");
                Message.replace("\r","");
                bool Ok=false;
                double Position=Message.toDouble(&Ok)*1000;
                if (Ok) {
                    ui->CustomRuller->Slider->setValue(int(Position));
                    ActualPosition=int(Position);
                    ui->Position->setText(GetCurrentPos().toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss")+"\n"+tDuration.toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
                }
            }

        } else {

            // Unknown message

        }
    }
}

//============================================================================================
// MPlayerFinished is call when mplayer process end
//============================================================================================

void wgt_QVideoPlayer::s_MPlayerFinished(int,QProcess::ExitStatus) {
    // Function call when play is finish
    IsValide        = false;
    MplayerPlayMode = false;
    MplayerPauseMode= false;
    EnableWidget(false);
    ui->VideoPlayerPlayPauseBT->setIcon(IconPlay);
}

//============================================================================================
// Pause -> play
//============================================================================================

void wgt_QVideoPlayer::SetPlayerToPlay() {
    if (!(MplayerPlayMode && MplayerPauseMode)) return;
    if (FileInfo!=NULL) {
        if ((!MplayerPlayMode)||(MplayerProcess.state()==QProcess::NotRunning)) {
            // Stop -> play
            StartPlay(FileInfo);
        } else {
            if (IsValide) {
                MplayerPlayMode  = true;
                MplayerPauseMode = false;
                ui->VideoPlayerPlayPauseBT->setIcon(IconPlay);
                SendToMPlayer("pause\n");
            }
        }
    } else if (Diaporama!=NULL) {
        MplayerPlayMode  = true;
        MplayerPauseMode = false;
        ui->VideoPlayerPlayPauseBT->setIcon(IconPlay);

        // Init MixedMusic
        MixedMusic.ClearList();

        // Free ImageList
        ImageList.ClearList();

        MixedMusic.SetFPS(Diaporama->ApplicationConfig->PreviewFPS);

        // Desired AudioSpec
        SDL_AudioSpec Desired;
        Desired.channels=MixedMusic.Channels;                               // Number of chanels
        Desired.format  =AUDIO_S16SYS;                                      // Sound format (pcm16le)
        Desired.freq    =MixedMusic.SamplingRate;                           // Frequency in Hz
        Desired.userdata=this;                                              // Link to MixedMusic object as userdata parameter
        Desired.callback=AudioCallback;                                     // Link to callback function
        Desired.samples =MixedMusic.SoundPacketSize/MixedMusic.Channels;    // In samples unit * chanels number !
        Desired.silence =0;

        // Initialisation of SDL_mixer
        if (SDL_OpenAudio(&Desired,&AudioSpec)<0) {
            qDebug()<<"wgt_QVideoPlayer::SetPlayerToPlay=Error in SDL_OpenAudio";
            exit(1);
        }
        SDLIsAudioOpen=true;
        // Display first frame
        //s_SliderMoved(Diaporama->CurrentPosition);

        // Start timer
        TimerTick=true;
        Timer.start(int((double((uint64_t)AV_TIME_BASE)/double(Diaporama->ApplicationConfig->PreviewFPS))/1000)/2);   // Start Timer emulating FPS
    }
}

//============================================================================================
// Play -> pause
//============================================================================================

void wgt_QVideoPlayer::SetPlayerToPause() {
    if (!(MplayerPlayMode && !MplayerPauseMode)) return;
    if (FileInfo!=NULL) {
        if (IsValide) {
            MplayerPlayMode  = true;
            MplayerPauseMode = true;
            ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
            SendToMPlayer("pause\n");
        }
    } else if (Diaporama!=NULL) {
        MplayerPlayMode  = true;
        MplayerPauseMode = true;
        ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
        Timer.stop();       // Stop Timer
        SDL_PauseAudio(1);  // Stop sound
        SDL_CloseAudio();   // Close audio

        // Free sound buffers
        MixedMusic.ClearList();

        // Free ImageList
        ImageList.ClearList();
    }
}

//============================================================================================
// Click on the play/pause button
//============================================================================================

void wgt_QVideoPlayer::s_VideoPlayerPlayPauseBT() {
    if ((!MplayerPlayMode)||((MplayerPlayMode && MplayerPauseMode)))    SetPlayerToPlay();      // Stop/Pause -> play
        else if (MplayerPlayMode && !MplayerPauseMode)                  SetPlayerToPause();     // Pause -> play
}

//============================================================================================
// Click on the handle of the slider
//============================================================================================

void wgt_QVideoPlayer::s_SliderPressed() {
    PreviousPause    = MplayerPauseMode;    // Save pause state
    IsSliderProcess  = true;
    SetPlayerToPause();
}

//============================================================================================
// En slider process
//============================================================================================

void wgt_QVideoPlayer::s_SliderReleased() {
    IsSliderProcess  = false;
    s_SliderMoved(ActualPosition);
    // Restore saved pause state
    if (!PreviousPause) SetPlayerToPlay(); //else if (FileInfo!=NULL) SendToMPlayer("pause\n");
}

//============================================================================================
// Slider is moving by user
// Slider is moving by timer
// Slider is moving by seek function
//============================================================================================

void wgt_QVideoPlayer::s_SliderMoved(int Value) {
    if (ActualPosition==Value) return;  // If no change !

    ui->CustomRuller->Slider->setValue(Value);
    ActualPosition=Value;
    // Update display in controls
    ui->Position->setText(GetCurrentPos().toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss")+" / "+tDuration.toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));


    if (FileInfo!=NULL) {
        // Send a seek command : when result will send by mplayer, s_MPlayerMessage() function trap it and set slider position
        // SEEK x 2=is a seek to an absolute position of <value> seconds
        if (!IsSliderProcess) {
            QString Commande=((MplayerPauseMode==true)?"pausing_keep ":"")+QString("seek %1 2\n").arg(Value/1000);
            SendToMPlayer(Commande.toLocal8Bit());
            ui->CustomRuller->Slider->setValue(int(Value));
        }
    } else if (Diaporama!=NULL) {
        //***********************************************************************
        // If playing
        //***********************************************************************
        if (MplayerPlayMode && !MplayerPauseMode) {
            if (Value>=Diaporama->GetDuration()) SetPlayerToPause();    // Stop sound if it's the end
            else {
                if (ImageList.List.count()>0) {                        // Process
                    // Retrieve frame informations
                    cDiaporamaObjectInfo *Frame=ImageList.DetachFirstImage();

                    // Display frame
                    ui->MovieFrame->setPixmap(QPixmap().fromImage(*Frame->RenderedImage));

                    // If needed, set Diaporama to an other object
                    if (Diaporama->CurrentCol!=Frame->CurrentObject_Number) {
                        if (FLAGSTOPITEMSELECTION!=NULL) *FLAGSTOPITEMSELECTION=true;    // Ensure mainwindow no modify player widget position
                        Diaporama->CurrentCol=Frame->CurrentObject_Number;
                        Diaporama->Timeline->setCurrentCell(0,Frame->CurrentObject_Number);
                        if (FLAGSTOPITEMSELECTION!=NULL) *FLAGSTOPITEMSELECTION=false;

                        // Update slider mark
                        if (Diaporama->List.count()>0)
                            SetStartEndPos(
                                    Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                           // Current slide
                                    Diaporama->List[Diaporama->CurrentCol].GetDuration(),
                                    (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1),                            // Previous slide
                                    (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1].GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                                    Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,    // Next slide
                                    Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1].GetDuration():0);
                        else SetStartEndPos(0,0,-1,0,-1,0);

                    }
                    Diaporama->CurrentPosition=Value;

                    // Free frame
                    delete Frame;

                    // Start sound (if not previously started)
                    if ((MplayerPlayMode) && (!MplayerPauseMode) && (SDL_GetAudioStatus()!=SDL_AUDIO_PLAYING)) SDL_PauseAudio(0);
                }
            }
        //***********************************************************************
        // If moving by user
        //***********************************************************************
        } else {

            // Free sound buffers
            MixedMusic.ClearList();

            // Free ImageList
            ImageList.ClearList();
            ui->BufferState->setValue(0);

            // Retrieve object informations
            cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(NULL,Value,Diaporama,double(1000)/Diaporama->ApplicationConfig->PreviewFPS);

            // Update display in preview
            PrepareImage(Frame,false);          // This will add frame to the ImageList
            Frame=ImageList.DetachFirstImage(); // Then detach frame from the ImageList

            // Display frame
            ui->MovieFrame->setPixmap(QPixmap().fromImage(*Frame->RenderedImage));

            // If needed, set Diaporama to an other object
            if (Diaporama->CurrentCol!=Frame->CurrentObject_Number) {
                if (FLAGSTOPITEMSELECTION!=NULL) *FLAGSTOPITEMSELECTION=true;    // Ensure mainwindow no modify player widget position
                Diaporama->CurrentCol=Frame->CurrentObject_Number;
                Diaporama->Timeline->setCurrentCell(0,Frame->CurrentObject_Number);
                if (FLAGSTOPITEMSELECTION!=NULL) *FLAGSTOPITEMSELECTION=false;

                // Update slider mark
                if (Diaporama->List.count()>0)
                    SetStartEndPos(
                            Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                               // Current slide
                            Diaporama->List[Diaporama->CurrentCol].GetDuration(),
                            (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1), // Previous slide
                            (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1].GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                            Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,        // Next slide
                            Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1].GetDuration():0);

            }
            Diaporama->CurrentPosition=Value;

            // Free frame
            delete Frame;
        }
    }
}

//============================================================================================
// Timer event
//============================================================================================
void wgt_QVideoPlayer::s_TimerEvent() {
    if ((Flag_InTimer==true)||(IsSliderProcess)) return;
    Flag_InTimer=true;
    if (FileInfo!=NULL) {

        if (IsValide && !MplayerPauseMode && !IsSliderProcess) SendToMPlayer("get_time_pos\n");      // ask mplayer for movie length

    } else if (Diaporama!=NULL) {
        cDiaporamaObjectInfo *PreviousFrame=ImageList.GetLastImage();
        int LastPosition=(PreviousFrame==NULL)?Diaporama->CurrentPosition:PreviousFrame->CurrentObject_StartTime+PreviousFrame->CurrentObject_InObjectTime;

        // Add image to the list if it's not full
        if ((ImageList.List.count()<BUFFERING_NBR_FRAME)&&(ThreadPrepareMontage.isRunning()==false)) {
            // Create a new frame object
            cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(PreviousFrame,LastPosition+int(double(1000)/Diaporama->ApplicationConfig->PreviewFPS),Diaporama,double(1000)/Diaporama->ApplicationConfig->PreviewFPS);

            // Start thread to prepare
            ThreadPrepareMontage=QtConcurrent::run(this,&wgt_QVideoPlayer::PrepareImage,NewFrame,true);
        }
        ui->BufferState->setValue(ImageList.List.count());

        // if TimerTick update the preview
        if (TimerTick) {
            if (ImageList.List.count()==0) ThreadPrepareMontage.waitForFinished();
            // Move slider to the position of the next frame
            if ((ImageList.List.count()>1)&&(ui->CustomRuller->Slider!=NULL))
                s_SliderMoved(ImageList.GetFirstImage()->CurrentObject_StartTime+ImageList.GetFirstImage()->CurrentObject_InObjectTime);
        }
    }
    Flag_InTimer=false;
    if (TimerTick) TimerTick=false; else TimerTick=true;
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
//============================================================================================

void wgt_QVideoPlayer::PrepareImage(cDiaporamaObjectInfo *Frame,bool SoundWanted) {
    if (SoundWanted) {
        // Ensure MusicTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_MusicTrack==NULL)) {
            Frame->CurrentObject_MusicTrack=new cSoundBlockList();
            Frame->CurrentObject_MusicTrack->SetFPS(Diaporama->ApplicationConfig->PreviewFPS);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
            Frame->TransitObject_MusicTrack=new cSoundBlockList();
            Frame->TransitObject_MusicTrack->SetFPS(Diaporama->ApplicationConfig->PreviewFPS);
        }

        // Ensure SoundTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)&&(Frame->CurrentObject->TypeObject==DIAPORAMAOBJECTTYPE_VIDEO)) {
            Frame->CurrentObject_SoundTrackMontage=new cSoundBlockList();
            Frame->CurrentObject_SoundTrackMontage->SetFPS(Diaporama->ApplicationConfig->PreviewFPS);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)&&(Frame->TransitObject->TypeObject==DIAPORAMAOBJECTTYPE_VIDEO)) {
            Frame->TransitObject_SoundTrackMontage=new cSoundBlockList();
            Frame->TransitObject_SoundTrackMontage->SetFPS(Diaporama->ApplicationConfig->PreviewFPS);
        }
    }

    // Ensure background, image and soundtrack is ready (in thread mode)
    Diaporama->LoadSources(Frame,ui->MovieFrame->width(),ui->MovieFrame->height(),true);

    // Prepare images
    Diaporama->PrepareImage(Frame,ui->MovieFrame->width(),ui->MovieFrame->height(),0,true);                             // Current Object
    if (Frame->IsTransition) Diaporama->PrepareImage(Frame,ui->MovieFrame->width(),ui->MovieFrame->height(),0,false);   // Transition Object

    // Do Assembly
    ThreadDoAssembly=QtConcurrent::run(Diaporama,&cDiaporama::DoAssembly,Frame,ui->MovieFrame->width(),ui->MovieFrame->height());

    if ((SoundWanted)&&(Frame->CurrentObject)) {
        // Calc number of packet to mix
        int MaxPacket=Frame->CurrentObject_MusicTrack->List.count();
        if ((Frame->CurrentObject_SoundTrackMontage)&&(MaxPacket>Frame->CurrentObject_SoundTrackMontage->List.count())) MaxPacket=Frame->CurrentObject_SoundTrackMontage->List.count();
        if (MaxPacket>MixedMusic.NbrPacketForFPS) MaxPacket=MixedMusic.NbrPacketForFPS;

        // Append mixed musique to the queue
        SDL_LockAudio();    // Ensure callback will not be call now
        for (int j=0;j<MaxPacket;j++)
            MixedMusic.MixAppendPacket(Frame->CurrentObject_MusicTrack->DetachFirstPacket(),(Frame->CurrentObject_SoundTrackMontage!=NULL)?Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket():NULL);
        SDL_UnlockAudio();  // Ensure callback can now be call
    }
    // Append this image to the queue
    ThreadDoAssembly.waitForFinished();
    ImageList.AppendImage(Frame);
}

//============================================================================================
// Define zone selection on the ruller
//============================================================================================

void wgt_QVideoPlayer::SetStartEndPos(int StartPos,int Duration,int PreviousStartPos,int PrevisousDuration,int NextStartPos,int NextDuration) {
    ui->CustomRuller->StartPos          =StartPos;
    ui->CustomRuller->EndPos            =StartPos+Duration;
    ui->CustomRuller->PreviousStartPos  =PreviousStartPos;
    ui->CustomRuller->PrevisousEndPos   =PreviousStartPos+PrevisousDuration;
    ui->CustomRuller->NextStartPos      =NextStartPos;
    ui->CustomRuller->NextEndPos        =NextStartPos+NextDuration;
    ui->CustomRuller->repaint();
}

//============================================================================================
// Seek slider public function
//============================================================================================

void wgt_QVideoPlayer::SeekPlayer(int Value) {
    s_SliderMoved(Value);
}

//============================================================================================
// return current position in QTime format
//============================================================================================

QTime wgt_QVideoPlayer::GetCurrentPos() {
    if (ActualPosition!=-1) {
        int     TimeMSec    =ActualPosition-(ActualPosition/1000)*1000;
        int     TimeSec     =int(ActualPosition/1000);
        int     TimeHour    =TimeSec/(60*60);
        int     TimeMinute  =(TimeSec%(60*60))/60;
        QTime   tPosition;
        tPosition.setHMS(TimeHour,TimeMinute,TimeSec%60,TimeMSec);
        return tPosition;
    } else return QTime(0,0,0,0);
}

//============================================================================================
// return current duration in QTime format
//============================================================================================

QTime wgt_QVideoPlayer::GetActualDuration() {
    return tDuration;
}

//============================================================================================
// define duration (in msec)
//============================================================================================

void wgt_QVideoPlayer::SetActualDuration(int Duration) {
    if (ui->CustomRuller->Slider!=NULL) {
        ui->CustomRuller->Slider->setMaximum(Duration-1);
        ui->CustomRuller->Slider->repaint();
    }
    ui->CustomRuller->TotalDuration=Duration;
    ui->CustomRuller->repaint();
    int     TimeMSec    =(Duration %1000);
    int     TimeSec     =int(Duration/1000);
    int     TimeHour    =TimeSec/(60*60);
    int     TimeMinute  =(TimeSec%(60*60))/60;
    tDuration.setHMS(TimeHour,TimeMinute,TimeSec%60,TimeMSec);
    ui->Position->setFixedWidth(DisplayMSec?160:120);
    ui->BufferState->setFixedWidth(DisplayMSec?160:120);
    ui->Position->setText(GetCurrentPos().toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss")+" / "+tDuration.toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
}
