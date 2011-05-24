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
    FLAGSTOPITEMSELECTION   = NULL;
    FileInfo                = NULL;
    Diaporama               = NULL;
    IsValide                = false;
    Flag_InTimer            = false;
    IsInit                  = false;

    DisplayMSec             = false;
    IconPlay                = QIcon(ICON_PLAYERPLAY);
    IconPause               = QIcon(ICON_PLAYERPAUSE);
    PlayerPlayMode          = false;                                // Is player currently play mode
    PlayerPauseMode         = false;                                // Is player currently plause mode
    IsSliderProcess         = false;
    ActualPosition          = -1;
    tDuration               =QTime(0,0,0,0);

    ui->Position->setFixedWidth(DisplayMSec?170:130);
    ui->BufferState->setFixedWidth(DisplayMSec?170:130);
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
}

//============================================================================================

wgt_QVideoPlayer::~wgt_QVideoPlayer() {
    SetPlayerToPause();         // Ensure player is correctly stoped
    delete ui;
}

//============================================================================================

void wgt_QVideoPlayer::closeEvent(QCloseEvent *) {
    SetPlayerToPause();
}

//====================================================================================================================

void wgt_QVideoPlayer::showEvent(QShowEvent *) {
    if ((!IsInit)&&(Diaporama==NULL)) {
        SetPlayerToPlay();
        IsInit=true;
    }
}

//============================================================================================

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
    PlayerPlayMode  = true;
    PlayerPauseMode = true;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
    QApplication::restoreOverrideCursor();
    return true;
}

//============================================================================================
// Init a video show
//============================================================================================

bool wgt_QVideoPlayer::StartPlay(cvideofilewrapper *FileInfo,double WantedFPS) {
    if (FileInfo==NULL) return false;
    this->FileInfo =FileInfo;
    this->WantedFPS=WantedFPS;
    IsValide=true;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    SetActualDuration(QTime(0,0,0,0).msecsTo(FileInfo->Duration));
    resizeEvent(NULL);
    PlayerPlayMode  = true;
    PlayerPauseMode = true;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
    QApplication::restoreOverrideCursor();
    return true;
}

//============================================================================================
// Pause -> play
//============================================================================================

void wgt_QVideoPlayer::SetPlayerToPlay() {
    if (!(PlayerPlayMode && PlayerPauseMode)) return;
    PlayerPlayMode  = true;
    PlayerPauseMode = false;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPlay);

    // Start timer
    TimerTick=true;
    Timer.start(int((double((uint64_t)AV_TIME_BASE)/double(FileInfo?WantedFPS:Diaporama->ApplicationConfig->PreviewFPS))/1000)/2);   // Start Timer emulating FPS
}

//============================================================================================
// Play -> pause
//============================================================================================

void wgt_QVideoPlayer::SetPlayerToPause() {
    if (!(PlayerPlayMode && !PlayerPauseMode)) return;
    Timer.stop();                                   // Stop Timer
    ThreadPrepareMontage.waitForFinished();         // Thread preparation of image
    ThreadDoAssembly.waitForFinished();             // Thread for make assembly of background and images
    SDL_LockAudio();                                // Ensure callback will not be call now
    MixedMusic.ClearList();                         // Free sound buffers
    SDL_UnlockAudio();  // Ensure callback can now be call
    ImageList.ClearList();                          // Free ImageList
    PlayerPlayMode  = true;
    PlayerPauseMode = true;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
    ui->BufferState->setValue(0);
}

//============================================================================================
// Click on the play/pause button
//============================================================================================

void wgt_QVideoPlayer::s_VideoPlayerPlayPauseBT() {
    if ((!PlayerPlayMode)||((PlayerPlayMode && PlayerPauseMode)))    SetPlayerToPlay();      // Stop/Pause -> play
        else if (PlayerPlayMode && !PlayerPauseMode)                  SetPlayerToPause();     // Pause -> play
}

//============================================================================================
// Click on the handle of the slider
//============================================================================================

void wgt_QVideoPlayer::s_SliderPressed() {
    PreviousPause    = PlayerPauseMode;    // Save pause state
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

    //***********************************************************************
    // If playing
    //***********************************************************************
    if (PlayerPlayMode && !PlayerPauseMode) {
        if (((FileInfo)&&(ActualPosition>=QTime(0,0,0,0).msecsTo(tDuration)))||((Diaporama)&&(Value>=Diaporama->GetDuration()))) {
            SetPlayerToPause();    // Stop sound if it's the end
        } else {
            if (ImageList.List.count()>0) {                        // Process
                // Retrieve frame informations
                cDiaporamaObjectInfo *Frame=ImageList.DetachFirstImage();

                // Display frame
                if (Frame->RenderedImage) ui->MovieFrame->setPixmap(QPixmap().fromImage(*Frame->RenderedImage));

                // If Diaporama mode and needed, set Diaporama to an other object
                if (Diaporama) {
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
                }

                // Free frame
                delete Frame;

                // Start sound (if not previously started)
                if ((PlayerPlayMode) && (!PlayerPauseMode) && (SDL_GetAudioStatus()!=SDL_AUDIO_PLAYING)) SDL_PauseAudio(0);
            }
        }
    //***********************************************************************
    // If moving by user
    //***********************************************************************
    } else {

        // Free sound buffers
        SDL_LockAudio();                                // Ensure callback will not be call now
        MixedMusic.ClearList();                         // Free sound buffers
        SDL_UnlockAudio();                              // Ensure callback can now be call

        // Free ImageList
        ImageList.ClearList();

        if (FileInfo) {

            QImage *VideoImage=FileInfo->ImageAt(true,ActualPosition,false,NULL,1,false,NULL);
            ui->MovieFrame->setPixmap(QPixmap().fromImage(VideoImage->scaledToHeight(ui->MovieFrame->height())));  // Display frame
            delete VideoImage;

        } else if (Diaporama) {
            // Create a frame from actual position
            cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(NULL,Value,Diaporama,double(1000)/Diaporama->ApplicationConfig->PreviewFPS);
            PrepareImage(Frame,false);          // This will add frame to the ImageList
            Frame=ImageList.DetachFirstImage(); // Then detach frame from the ImageList
            ui->MovieFrame->setPixmap(QPixmap().fromImage(*Frame->RenderedImage));  // Display frame

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
    cDiaporamaObjectInfo *PreviousFrame=ImageList.GetLastImage();
    int LastPosition=0;

    // Add image to the list if it's not full
    if ((ImageList.List.count()<BUFFERING_NBR_FRAME)&&(ThreadPrepareMontage.isRunning()==false)) {

        if (FileInfo) {
            // Calc LastPosition
            LastPosition=(PreviousFrame==NULL)?ActualPosition:PreviousFrame->CurrentObject_InObjectTime;
            // Create a new frame object
            cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(PreviousFrame,0,NULL,0);
            NewFrame->CurrentObject_StartTime   =0;
            NewFrame->CurrentObject_InObjectTime=LastPosition+int(double(1000)/WantedFPS);
            QImage *Temp=FileInfo->ImageAt(true,ActualPosition,false,&MixedMusic,1,false,NULL);
            QImage *Temp2=new QImage(Temp->scaledToHeight(ui->MovieFrame->height()));
            NewFrame->RenderedImage=Temp2;
            delete Temp;
            if (NewFrame->RenderedImage) ImageList.AppendImage(NewFrame); else delete NewFrame;

        } else if (Diaporama) {
            // Calc LastPosition
            LastPosition=(PreviousFrame==NULL)?Diaporama->CurrentPosition:PreviousFrame->CurrentObject_StartTime+PreviousFrame->CurrentObject_InObjectTime;
            // Create a new frame object
            cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(PreviousFrame,LastPosition+int(double(1000)/Diaporama->ApplicationConfig->PreviewFPS),Diaporama,double(1000)/Diaporama->ApplicationConfig->PreviewFPS);
            // Start thread to prepare
            ThreadPrepareMontage=QtConcurrent::run(this,&wgt_QVideoPlayer::PrepareImage,NewFrame,true);
        }
    }
    ui->BufferState->setValue(ImageList.List.count());
    if (ImageList.List.count()<2)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: red;\n}");
    else if (ImageList.List.count()<4)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: yellow;\n}");
    else if (ImageList.List.count()<=BUFFERING_NBR_FRAME)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: green;\n}");

    // if TimerTick update the preview
    if (TimerTick) {
        if (ImageList.List.count()==0) ThreadPrepareMontage.waitForFinished();
        // Move slider to the position of the next frame
        if ((ImageList.List.count()>1)&&(ui->CustomRuller->Slider!=NULL))
            s_SliderMoved(ImageList.GetFirstImage()->CurrentObject_StartTime+ImageList.GetFirstImage()->CurrentObject_InObjectTime);
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
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)&&(Frame->CurrentObject->TypeObject!=DIAPORAMAOBJECTTYPE_IMAGE)) {
            Frame->CurrentObject_SoundTrackMontage=new cSoundBlockList();
            Frame->CurrentObject_SoundTrackMontage->SetFPS(Diaporama->ApplicationConfig->PreviewFPS);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)&&(Frame->TransitObject->TypeObject!=DIAPORAMAOBJECTTYPE_IMAGE)) {
            Frame->TransitObject_SoundTrackMontage=new cSoundBlockList();
            Frame->TransitObject_SoundTrackMontage->SetFPS(Diaporama->ApplicationConfig->PreviewFPS);
        }
    }

    // Ensure background, image and soundtrack is ready (in thread mode)
    Diaporama->LoadSources(Frame,ui->MovieFrame->width(),ui->MovieFrame->height(),true);

    // Do Assembly
    ThreadDoAssembly=QtConcurrent::run(Diaporama,&cDiaporama::DoAssembly,Frame,ui->MovieFrame->width(),ui->MovieFrame->height());

    if ((SoundWanted)&&(Frame->CurrentObject)) {
        // Calc number of packet to mix
        int MaxPacket=Frame->CurrentObject_MusicTrack->List.count();
        if ((Frame->CurrentObject_SoundTrackMontage)&&
            (Frame->CurrentObject_SoundTrackMontage->List.count()>0)&&
            (MaxPacket>Frame->CurrentObject_SoundTrackMontage->List.count())) MaxPacket=Frame->CurrentObject_SoundTrackMontage->List.count();
        if (MaxPacket>MixedMusic.NbrPacketForFPS) MaxPacket=MixedMusic.NbrPacketForFPS;

        // Append mixed musique to the queue
        for (int j=0;j<MaxPacket;j++)
            MixedMusic.MixAppendPacket(Frame->CurrentObject_MusicTrack->DetachFirstPacket(),(Frame->CurrentObject_SoundTrackMontage!=NULL)?Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket():NULL);
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
    ActualPosition=-1;
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
    ui->Position->setFixedWidth(DisplayMSec?170:130);
    ui->BufferState->setFixedWidth(DisplayMSec?170:130);
    ui->Position->setText(GetCurrentPos().toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss")+" / "+tDuration.toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
}
