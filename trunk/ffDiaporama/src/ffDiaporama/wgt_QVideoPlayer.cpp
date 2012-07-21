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

#include "wgt_QVideoPlayer.h"
#include "mainwindow.h"
#include "ui_wgt_QVideoPlayer.h"

#define ICON_PLAYERPLAY                     ":/img/player_play.png"                 // FileName of play icon
#define ICON_PLAYERPAUSE                    ":/img/player_pause.png"                // FileName of pause icon

#define BUFFERING_NBR_FRAME                 5                                     // Number of frame wanted in the playing buffer

//*********************************************************************************************************************************************
// Base object for Movie frame
//*********************************************************************************************************************************************
QMovieLabel::QMovieLabel(QWidget *parent):QLabel(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QMovieLabel::QMovieLabel");
}

QMovieLabel::~QMovieLabel() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QMovieLabel::~QMovieLabel");

}

void QMovieLabel::mouseDoubleClickEvent(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QMovieLabel::mouseDoubleClickEvent");
    emit DoubleClick();
}

void QMovieLabel::mouseReleaseEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QMovieLabel::mousePressEvent");
    if (event->button()==Qt::RightButton) emit RightClickEvent(event);  else QLabel::mouseReleaseEvent(event);
}

void QMovieLabel::SetImage(QImage Image) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QMovieLabel::SetImage");
    CurrentImage=Image.copy();
    repaint();
}

void QMovieLabel::SetImage(QImage *Image) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QMovieLabel::SetImage");
    CurrentImage=Image->copy();
    repaint();
}

void QMovieLabel::paintEvent(QPaintEvent * /*event*/) {
    if (CurrentImage.isNull()) return;
    QPainter Painter(this);
    //Painter.save();
    Painter.drawImage(0,0,CurrentImage);
    //Painter.restore();
}


//*********************************************************************************************************************************************
// Base object for image manipulation
//*********************************************************************************************************************************************

cImageList::cImageList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageList::cImageList");
}

//====================================================================================================================

cImageList::~cImageList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageList::~cImageList");
    ClearList();
}

//====================================================================================================================
// Clear the list (make av_free of each packet)
//====================================================================================================================
void cImageList::ClearList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageList::ClearList");
    while (List.count()>0) {
        cDiaporamaObjectInfo *Frame=DetachFirstImage();
        if (Frame) delete(Frame);
    }
}

//====================================================================================================================
// Detach the first image of the list (do not make delete)
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::DetachFirstImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageList::DetachFirstImage");
    if (List.count()>0) return (cDiaporamaObjectInfo *)List.takeFirst(); else return NULL;
}

//====================================================================================================================
// Retreve a link to the first frame in the list
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::GetFirstImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageList::GetFirstImage");
    if (List.count()>0) return (cDiaporamaObjectInfo *)List[0]; else return NULL;
}

//====================================================================================================================
// Retreve a link to the last frame in the list
//====================================================================================================================
cDiaporamaObjectInfo *cImageList::GetLastImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageList::GetLastImage");
    if (List.count()>0) return (cDiaporamaObjectInfo *)List[List.count()-1]; else return NULL;
}

//====================================================================================================================
// Append a packet to the end of the list
//====================================================================================================================
void cImageList::AppendImage(cDiaporamaObjectInfo *Frame) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageList::AppendImage");
    List.append(Frame);
}

//====================================================================================================================

wgt_QVideoPlayer::wgt_QVideoPlayer(QWidget *parent) : QWidget(parent),ui(new Ui::wgt_QVideoPlayer) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::wgt_QVideoPlayer");
    ui->setupUi(this);
    FLAGSTOPITEMSELECTION   = NULL;
    FileInfo                = NULL;
    Diaporama               = NULL;
    WantedFPS               = 12.5;
    IsValide                = false;
    Flag_InTimer            = false;
    IsInit                  = false;
    DisplayMSec             = true;                                 // add msec to display
    IconPause               = QIcon(ICON_PLAYERPLAY);
    IconPlay                = QIcon(ICON_PLAYERPAUSE);
    PlayerPlayMode          = false;                                // Is player currently play mode
    PlayerPauseMode         = false;                                // Is player currently plause mode
    IsSliderProcess         = false;
    ActualPosition          = -1;
    tDuration               = QTime(0,0,0,0);
    ActualDisplay           = NULL;
    ResetPositionWanted     = false;

    this->FileInfo      = FileInfo;
    ui->CustomRuller->ActiveSlider(0);
    ui->CustomRuller->setSingleStep(25);

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
    connect(ui->MovieFrame,SIGNAL(DoubleClick()),this,SLOT(s_DoubleClick()));
    connect(ui->MovieFrame,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_RightClickEvent(QMouseEvent *)));

    // Slider control
    connect(ui->CustomRuller,SIGNAL(sliderPressed()),this,SLOT(s_SliderPressed()));
    connect(ui->CustomRuller,SIGNAL(sliderReleased()),this,SLOT(s_SliderReleased()));
    connect(ui->CustomRuller,SIGNAL(valueChanged(int)),this,SLOT(s_SliderMoved(int)));
    connect(ui->CustomRuller,SIGNAL(PositionChangeByUser()),this,SLOT(s_PositionChangeByUser()));
    connect(ui->VideoPlayerSaveImageBT,SIGNAL(pressed()),this,SLOT(s_SaveImage()));
}

//============================================================================================

wgt_QVideoPlayer::~wgt_QVideoPlayer() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::~wgt_QVideoPlayer");
    SetPlayerToPause();         // Ensure player is correctly stoped
    delete ui;
}

//============================================================================================

void wgt_QVideoPlayer::closeEvent(QCloseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::closeEvent");
    SetPlayerToPause();
}

//====================================================================================================================

void wgt_QVideoPlayer::showEvent(QShowEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::showEvent");
    if ((!IsInit)&&(Diaporama==NULL)) {
        SetPlayerToPlay();
        IsInit=true;
    }
}

//============================================================================================

void wgt_QVideoPlayer::s_SaveImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_SaveImage");
    emit SaveImageEvent();
}

//============================================================================================

void wgt_QVideoPlayer::s_DoubleClick() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_DoubleClick");
    emit DoubleClick();
}

//============================================================================================

void wgt_QVideoPlayer::s_RightClickEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_RightClickEvent");
    emit RightClickEvent(event);
}

//============================================================================================

int wgt_QVideoPlayer::GetButtonBarHeight() {
    return ui->VideoPlayerPlayPauseBT->height();
}

//============================================================================================

void wgt_QVideoPlayer::resizeEvent(QResizeEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::resizeEvent");
    Resize();
}

void wgt_QVideoPlayer::Resize() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::Resize");

    if ((FileInfo==NULL)&&(Diaporama==NULL)) return;
    SetPlayerToPause();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int HeightBT=ui->VideoPlayerPlayPauseBT->height();
    int TheHeight=height()-HeightBT;
    int TheWidth =0;
    int ToSet    =0;

    if ((FileInfo!=NULL)&&(FileInfo->IsValide)) {
        TheWidth=(int(double(TheHeight)*(double(FileInfo->ImageWidth)/double(FileInfo->ImageHeight)))/2)*2; // Rounded value to a multiple of 2
        if (TheWidth>width()) {
            TheWidth=(width()/2)*2; // Rounded value to a multiple of 2
            TheHeight=int(double(TheWidth)*(double(FileInfo->ImageHeight)/double(FileInfo->ImageWidth)));
        }
    } else if (Diaporama!=NULL) {
        TheWidth=(Diaporama->GetWidthForHeight(TheHeight)/2)*2; // Rounded value to a multiple of 2
        if (TheWidth>width()) TheWidth =(width()/2)*2;    // Rounded value to a multiple of 2
        TheHeight=Diaporama->GetHeightForWidth(TheWidth);
    }
    ToSet=height()-TheHeight-HeightBT;
    ui->SpacerV1->setFixedHeight(ToSet);
    ToSet=width()-TheWidth;
    ui->SpacerH1->setFixedWidth(ToSet/2);
    ui->SpacerH2->setFixedWidth(ToSet-(ToSet/2));
    ui->MovieFrame->setFixedWidth(TheWidth);
    ui->MovieFrame->setFixedHeight(TheHeight);

    if (ui->CustomRuller!=NULL) ui->CustomRuller->setFixedWidth(TheWidth-ui->VideoPlayerPlayPauseBT->width()-ui->Position->width()-ui->VideoPlayerSaveImageBT->width());
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::SetBackgroundColor");

    QString Sheet=QString("background-color: rgb(%1,%2,%3);").arg(Background.red(),10).arg(Background.green(),10).arg(Background.blue(),10);
}

//============================================================================================

void wgt_QVideoPlayer::EnableWidget(bool State) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::EnableWidget");

    if (ui->CustomRuller!=NULL) ui->CustomRuller->setEnabled(State);
}

//============================================================================================
// Init a diaporama show
//============================================================================================

bool wgt_QVideoPlayer::InitDiaporamaPlay(cDiaporama *Diaporama) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::InitDiaporamaPlay");

    if (Diaporama==NULL) return false;
    this->Diaporama     =Diaporama;
    WantedFPS           =Diaporama->ApplicationConfig->PreviewFPS;
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

bool wgt_QVideoPlayer::StartPlay(cVideoFile *theFileInfo,double theWantedFPS) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::StartPlay");

    if (theFileInfo==NULL) return false;
    FileInfo =theFileInfo;
    WantedFPS=theWantedFPS;
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::SetPlayerToPlay");
    if (!(PlayerPlayMode && PlayerPauseMode)) return;
    PlayerPlayMode  = true;
    PlayerPauseMode = false;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPlay);

    // Start timer
    TimerTick=true;
    Timer.start(int((double((uint64_t)AV_TIME_BASE)/WantedFPS)/1000)/2);   // Start Timer
}

//============================================================================================
// Play -> pause
//============================================================================================

void wgt_QVideoPlayer::SetPlayerToPause() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::SetPlayerToPause");
    if (!(PlayerPlayMode && !PlayerPauseMode)) return;
    Timer.stop();                                   // Stop Timer
    if (SDL_GetAudioStatus()==SDL_AUDIO_PLAYING) SDL_PauseAudio(1);
    if (ThreadPrepareVideo.isRunning()) ThreadPrepareVideo.waitForFinished();
    if (ThreadPrepareImage.isRunning()) ThreadPrepareImage.waitForFinished();
    MixedMusic.ClearList();                         // Free sound buffers
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_VideoPlayerPlayPauseBT");
    if ((!PlayerPlayMode)||((PlayerPlayMode && PlayerPauseMode)))    SetPlayerToPlay();      // Stop/Pause -> play
        else if (PlayerPlayMode && !PlayerPauseMode)                 SetPlayerToPause();     // Pause -> play
}

//============================================================================================
// Click on the handle of the slider
//============================================================================================

void wgt_QVideoPlayer::s_SliderPressed() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_SliderPressed");
    PreviousPause    = PlayerPauseMode;    // Save pause state
    IsSliderProcess  = true;
    SetPlayerToPause();
}

//============================================================================================
// En slider process
//============================================================================================

void wgt_QVideoPlayer::s_SliderReleased() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_SliderReleased");
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_SliderMoved");
    if (GlobalMainWindow->InPlayerUpdate) return;
    GlobalMainWindow->InPlayerUpdate=true;

    if (ResetPositionWanted) SetPlayerToPause();

    // Update display in controls
    ui->CustomRuller->setValue(Value);
    ActualPosition=Value;
    ui->Position->setText(GetCurrentPos().toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
    ui->Duration->setText(tDuration.toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));

    //***********************************************************************
    // If playing
    //***********************************************************************
    if (PlayerPlayMode && !PlayerPauseMode) {

        if (((FileInfo)&&(ActualPosition>=QTime(0,0,0,0).msecsTo(tDuration)))||((Diaporama)&&(Value>=Diaporama->GetDuration()))) {

            SetPlayerToPause();    // Stop if it's the end

        } else if (ImageList.List.count()>1) {                        // Process
            // Retrieve frame information
            cDiaporamaObjectInfo *Frame=ImageList.DetachFirstImage();

            // Display frame
            if ((Frame->RenderedImage)&&(ActualDisplay!=Frame->RenderedImage))
                ui->MovieFrame->SetImage(Frame->RenderedImage);

            ActualDisplay=Frame->RenderedImage;

            // If Diaporama mode and needed, set Diaporama to another object
            if (Diaporama) {
                if (Diaporama->CurrentCol!=Frame->CurrentObject_Number) {
                    Diaporama->CurrentCol=Frame->CurrentObject_Number;
                    GlobalMainWindow->SetTimelineCurrentCell(Frame->CurrentObject_Number);

                    // Update slider mark
                    if (Diaporama->List.count()>0)
                        SetStartEndPos(
                                Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                           // Current slide
                                Diaporama->List[Diaporama->CurrentCol]->GetDuration(),
                                (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1),                            // Previous slide
                                (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1]->GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,    // Next slide
                                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1]->GetDuration():0);
                    else SetStartEndPos(0,0,-1,0,-1,0);
               }
               Diaporama->CurrentPosition=Value;
            }

            // Start sound (if not previously started)
            if ((PlayerPlayMode) && (!PlayerPauseMode) && (SDL_GetAudioStatus()!=SDL_AUDIO_PLAYING)) SDL_PauseAudio(0);

            // Free frame
            delete Frame;
        }

    //***********************************************************************
    // If moving by user
    //***********************************************************************
    } else {

        ActualDisplay=NULL;

        if (FileInfo) {

            QImage *VideoImage=FileInfo->ImageAt(true,ActualPosition,0,NULL,1,false,NULL,false);
            if (VideoImage) {
                // Display frame
                ui->MovieFrame->SetImage(VideoImage->scaledToHeight(ui->MovieFrame->height()));
                delete VideoImage;
            }

        } else if (Diaporama) {
            // Create a frame from actual position
            cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(NULL,Value,Diaporama,double(1000)/WantedFPS);
            PrepareImage(Frame,false,true);         // This will add frame to the ImageList
            Frame=ImageList.DetachFirstImage();     // Then detach frame from the ImageList
            // Display frame
            ui->MovieFrame->SetImage(Frame->RenderedImage);

            // If needed, set Diaporama to another object
            if (Diaporama->CurrentCol!=Frame->CurrentObject_Number) {
                if (FLAGSTOPITEMSELECTION!=NULL) *FLAGSTOPITEMSELECTION=true;    // Ensure mainwindow no modify player widget position
                Diaporama->CurrentCol=Frame->CurrentObject_Number;
                GlobalMainWindow->SetTimelineCurrentCell(Frame->CurrentObject_Number);
                if (FLAGSTOPITEMSELECTION!=NULL) *FLAGSTOPITEMSELECTION=false;

                // Update slider mark
                if (Diaporama->List.count()>0)
                    SetStartEndPos(
                            Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                               // Current slide
                            Diaporama->List[Diaporama->CurrentCol]->GetDuration(),
                            (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1), // Previous slide
                            (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1]->GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                            Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,        // Next slide
                            Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1]->GetDuration():0);

            }
            Diaporama->CurrentPosition=Value;

            // Free frame
            delete Frame;
        }
    }
    GlobalMainWindow->InPlayerUpdate=false;
}

//============================================================================================
// Timer event
//============================================================================================
void wgt_QVideoPlayer::s_TimerEvent() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_TimerEvent");
    if ((Flag_InTimer==true)||(IsSliderProcess))    return;     // No re-entrance
    if (!(PlayerPlayMode && !PlayerPauseMode))      return;     // Only if play mode

    Flag_InTimer=true;
    TimerTick=!TimerTick;

    int LastPosition=0;

    if (ResetPositionWanted) {
        MixedMusic.ClearList();                         // Free sound buffers
        ImageList.ClearList();                          // Free ImageList
        ResetPositionWanted=false;
    }

    // If no image in the list then create the first
    if (ImageList.List.count()==0) {

        // If no image in the list then prepare a first frame
        if (FileInfo) {
            LastPosition=ActualPosition;
            cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(NULL,LastPosition+int(double(1000)/WantedFPS),NULL,int(double(1000)/WantedFPS));
            NewFrame->CurrentObject_StartTime   =0;
            PrepareVideoFrame(NewFrame,NewFrame->CurrentObject_InObjectTime);
        } else {
            LastPosition=Diaporama->CurrentPosition;
            cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(NULL,LastPosition+int(double(1000)/WantedFPS),Diaporama,double(1000)/WantedFPS);
            PrepareImage(NewFrame,true,true);
        }

    }

    cDiaporamaObjectInfo *PreviousFrame=ImageList.GetLastImage();

    if (FileInfo) LastPosition=PreviousFrame->CurrentObject_InObjectTime;
        else if (Diaporama) LastPosition=PreviousFrame->CurrentObject_StartTime+PreviousFrame->CurrentObject_InObjectTime;

    // Add image to the list if it's not full
    if ((FileInfo)&&(ImageList.List.count()<BUFFERING_NBR_FRAME)&&(!ThreadPrepareVideo.isRunning())) {

        LastPosition=PreviousFrame->CurrentObject_InObjectTime;
        cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(PreviousFrame,LastPosition+int(double(1000)/WantedFPS),NULL,int(double(1000)/WantedFPS));
        NewFrame->CurrentObject_StartTime   =0;
        ThreadPrepareVideo.setFuture(QtConcurrent::run(this,&wgt_QVideoPlayer::PrepareVideoFrame,NewFrame,NewFrame->CurrentObject_InObjectTime));

    } else if (((Diaporama)&&(ImageList.List.count()<BUFFERING_NBR_FRAME))&&(!ThreadPrepareImage.isRunning()))  {

        LastPosition=PreviousFrame->CurrentObject_StartTime+PreviousFrame->CurrentObject_InObjectTime;
        cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(PreviousFrame,LastPosition+int(double(1000)/WantedFPS),Diaporama,double(1000)/WantedFPS);
        ThreadPrepareImage.setFuture(QtConcurrent::run(this,&wgt_QVideoPlayer::PrepareImage,NewFrame,true,true));

    }

    // if TimerTick update the preview
    if ((TimerTick)&&(ui->CustomRuller!=NULL)) s_SliderMoved(ImageList.GetFirstImage()->CurrentObject_StartTime+ImageList.GetFirstImage()->CurrentObject_InObjectTime);

    ui->BufferState->setValue(ImageList.List.count());
    if (ImageList.List.count()<2)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: red;\n}");
    else if (ImageList.List.count()<4)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: yellow;\n}");
    else if (ImageList.List.count()<=BUFFERING_NBR_FRAME)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 1px;\npadding-bottom: 2px;\npadding-left: 1px;\npadding-right: 1px;\n}\nQProgressBar::chunk:horizontal {\nbackground: green;\n}");

    Flag_InTimer=false;
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
//============================================================================================

void wgt_QVideoPlayer::PrepareImage(cDiaporamaObjectInfo *Frame,bool SoundWanted,bool AddStartPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::PrepareImage");
    if (SoundWanted) {
        // Ensure MusicTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_MusicTrack==NULL)) {
            Frame->CurrentObject_MusicTrack=new cSDLSoundBlockList();
            Frame->CurrentObject_MusicTrack->SetFPS(WantedFPS);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
            Frame->TransitObject_MusicTrack=new cSDLSoundBlockList();
            Frame->TransitObject_MusicTrack->SetFPS(WantedFPS);
        }

        // Ensure SoundTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
            Frame->CurrentObject_SoundTrackMontage=new cSDLSoundBlockList();
            Frame->CurrentObject_SoundTrackMontage->SetFPS(WantedFPS);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
            Frame->TransitObject_SoundTrackMontage=new cSDLSoundBlockList();
            Frame->TransitObject_SoundTrackMontage->SetFPS(WantedFPS);
        }
    }

    // Ensure background, image and soundtrack is ready
    Diaporama->LoadSources(Frame,double(ui->MovieFrame->height())/double(1080),ui->MovieFrame->width(),ui->MovieFrame->height(),true,AddStartPos);

    // Do Assembly
    Diaporama->DoAssembly(Frame,ui->MovieFrame->width(),ui->MovieFrame->height());

    if ((SoundWanted)&&(Frame->CurrentObject)) {
        // Calc number of packet to mix
        int MaxPacket=Frame->CurrentObject_MusicTrack->List.count();
        if ((Frame->CurrentObject_SoundTrackMontage)&&
            (Frame->CurrentObject_SoundTrackMontage->List.count()>0)&&
            (MaxPacket>Frame->CurrentObject_SoundTrackMontage->List.count())) MaxPacket=Frame->CurrentObject_SoundTrackMontage->List.count();
        if (MaxPacket>MixedMusic.NbrPacketForFPS) MaxPacket=MixedMusic.NbrPacketForFPS;

        // Append mixed musique to the queue
        for (int j=0;j<MaxPacket;j++) MixedMusic.MixAppendPacket(Frame->CurrentObject_MusicTrack->DetachFirstPacket(),(Frame->CurrentObject_SoundTrackMontage!=NULL)?Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket():NULL);
    }

    // Append this image to the queue
    ImageList.AppendImage(Frame);
}

void wgt_QVideoPlayer::PrepareVideoFrame(cDiaporamaObjectInfo *NewFrame,int Position) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::PrepareVideoFrame");
    QImage *Temp=FileInfo->ImageAt(true,Position,0,&MixedMusic,1,false,NULL,true);
    if (Temp) {
        QImage *Temp2=new QImage(Temp->scaledToHeight(ui->MovieFrame->height()));
        NewFrame->RenderedImage=Temp2;
        delete Temp;
    }
    if (NewFrame->RenderedImage) ImageList.AppendImage(NewFrame);
        else delete NewFrame;
}
//============================================================================================
// Define zone selection on the ruller
//============================================================================================

void wgt_QVideoPlayer::SetStartEndPos(int StartPos,int Duration,int PreviousStartPos,int PrevisousDuration,int NextStartPos,int NextDuration) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::SetStartEndPos");
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::SeekPlayer");
    ActualPosition=-1;
    s_SliderMoved(Value);
}

//============================================================================================
// return current position in QTime format
//============================================================================================

QTime wgt_QVideoPlayer::GetCurrentPos() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::GetCurrentPos");
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::GetActualDuration");
    return tDuration;
}

//============================================================================================
// define duration (in msec)
//============================================================================================

void wgt_QVideoPlayer::SetActualDuration(int Duration) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::SetActualDuration");
    if (ui->CustomRuller!=NULL) {
        ui->CustomRuller->setMaximum(Duration-1);
        //ui->CustomRuller->repaint();
    }
    ui->CustomRuller->TotalDuration=Duration;
    //ui->CustomRuller->repaint();
    int     TimeMSec    =(Duration %1000);
    int     TimeSec     =int(Duration/1000);
    int     TimeHour    =TimeSec/(60*60);
    int     TimeMinute  =(TimeSec%(60*60))/60;
    tDuration.setHMS(TimeHour,TimeMinute,TimeSec%60,TimeMSec);
    ui->Position->setText(GetCurrentPos().toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
    ui->Duration->setText(tDuration.toString(DisplayMSec?"hh:mm:ss.zzz":"hh:mm:ss"));
}

//============================================================================================

void wgt_QVideoPlayer::s_PositionChangeByUser() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QVideoPlayer::s_PositionChangeByUser");
    ResetPositionWanted=true;
}
