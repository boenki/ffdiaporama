/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2014 Dominique Levray <domledom@laposte.net>

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

#include "DlgEditMusic.h"
#include "ui_DlgEditMusic.h"

#define IconPause   QIcon(":/img/player_play.png")
#define IconPlay    QIcon(":/img/player_pause.png")

#define BUFFERING_NBR_FRAME                 10                                      // Number of frame wanted in the playing buffer

DlgEditMusic::DlgEditMusic(cMusicObject *TheMusicObject,cApplicationConfig *ApplicationConfig,QWidget *parent):
    QCustomDialog(ApplicationConfig,parent), ui(new Ui::DlgEditMusic) {
    ui->setupUi(this);

    OkBt                =ui->OKBT;
    CancelBt            =ui->CancelBt;
    HelpBt              =ui->HelpBt;
    HelpFile            ="0124";
    MusicObject         =TheMusicObject;
    StopMaj             =false;
    InSliderMoveEvent   =false;
    IsInit              =false;
    PlayerPlayMode      =false;
    PlayerPauseMode     =false;
    IsSliderProcess     =false;
    ActualPosition      =-1;
    ResetPositionWanted =false;
}

//====================================================================================================================

DlgEditMusic::~DlgEditMusic() {
    if (ThreadAnalyseMusic.isRunning()) {
        ui->CustomRuler->IsAnalysed=true;
        ThreadAnalyseMusic.waitForFinished();
    }
    SetPlayerToPause();
    delete ui;
}

//====================================================================================================================

void DlgEditMusic::resizeEvent(QResizeEvent *ev) {
    QCustomDialog::resizeEvent(ev);
    if (IsInit && ui->CustomRuler->IsAnalysed) {
        ui->CustomRuler->PrepareSoundWave();
        ui->CustomRuler->repaint();
    }
}

void DlgEditMusic::showEvent(QShowEvent *) {
    if (!IsInit) {
        PlayerPlayMode =true;
        PlayerPauseMode=true;
        IsInit         =true;
        // Start sound analyse
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        connect(&ThreadAnalyseMusic,SIGNAL(finished()),this,SLOT(s_EndSoundAnalyse()));
        ThreadAnalyseMusic.setFuture(QtConcurrent::run(this,&DlgEditMusic::s_AnalyseSound));
        Timer.start(500);   // to update display on windows
    }
}

// Do sound analyse
void DlgEditMusic::s_AnalyseSound() {
    ui->CustomRuler->AnalyseSound(MusicObject);
}

// End of sound analyse
void DlgEditMusic::s_EndSoundAnalyse() {
    ui->CustomRuler->setEnabled(true);
    ui->DefStartPosBT->setEnabled(true);
    ui->DefEndPosBT->setEnabled(true);
    ui->SeekLeftBt->setEnabled(true);
    ui->SeekRightBt->setEnabled(true);
    ui->StartPosEd->setEnabled(true);
    ui->EndPosEd->setEnabled(true);
    ui->VideoPlayerPlayPauseBT->setEnabled(true);
    Timer.stop();
    // Activate player
    SetPlayerToPlay();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void DlgEditMusic::DoInitDialog() {
    ui->SeekLeftBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->SeekRightBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->StartPosEd->setCurrentSection(QDateTimeEdit::MSecSection);  ui->StartPosEd->setCurrentSectionIndex(3);  ui->StartPosEd->MsecStep=MusicObject->GetFPSDuration();
    ui->EndPosEd->setCurrentSection(QDateTimeEdit::MSecSection);    ui->EndPosEd->setCurrentSectionIndex(3);    ui->EndPosEd->MsecStep  =MusicObject->GetFPSDuration();

    SDLFlushBuffers();
    Music.SetFPS(MixedMusic.WantedDuration,MixedMusic.Channels,MixedMusic.SamplingRate,MixedMusic.SampleFormat);

    ui->EndPosEd->setMaximumTime(MusicObject->Duration.addMSecs(-1));
    if (MusicObject->EndPos>=MusicObject->Duration) MusicObject->EndPos=MusicObject->Duration.addMSecs(-1);

    ui->Duration->setText(MusicObject->Duration.toString("hh:mm:ss.zzz"));
    ui->CustomRuler->setMaximum(QTime(0,0,0,0).msecsTo(MusicObject->Duration)-1);

    ui->CustomRuler->StartPos     =QTime(0,0,0,0).msecsTo(MusicObject->StartPos);
    ui->CustomRuler->EndPos       =QTime(0,0,0,0).msecsTo(MusicObject->EndPos);
    ui->CustomRuler->TotalDuration=QTime(0,0,0,0).msecsTo(MusicObject->Duration);
    ui->CustomRuler->EditStartEnd =true;
    ui->CustomRuler->setSingleStep(25);

    // Disable all during sound analyse
    ui->CustomRuler->setEnabled(false);
    ui->DefStartPosBT->setEnabled(false);
    ui->DefEndPosBT->setEnabled(false);
    ui->SeekLeftBt->setEnabled(false);
    ui->SeekRightBt->setEnabled(false);
    ui->StartPosEd->setEnabled(false);
    ui->EndPosEd->setEnabled(false);
    ui->VideoPlayerPlayPauseBT->setEnabled(false);

    SetCurrentPos(MusicObject->StartPos);
    RefreshControls();

    connect(&Timer,SIGNAL(timeout()),this,SLOT(s_TimerEvent()));
    connect(ui->VideoPlayerPlayPauseBT,SIGNAL(clicked()),this,SLOT(s_VideoPlayerPlayPauseBT()));

    // Slider controls
    connect(ui->CustomRuler,SIGNAL(sliderPressed()),this,SLOT(s_SliderPressed()));
    connect(ui->CustomRuler,SIGNAL(sliderReleased()),this,SLOT(s_SliderReleased()));
    connect(ui->CustomRuler,SIGNAL(valueChanged(int)),this,SLOT(s_SliderMoved(int)));
    connect(ui->CustomRuler,SIGNAL(PositionChangeByUser()),this,SLOT(s_PositionChangeByUser()));
    connect(ui->CustomRuler,SIGNAL(StartEndChangeByUser()),this,SLOT(s_StartEndChangeByUser()));
    connect(ui->CustomRuler,SIGNAL(StartEndChangeByUser()),this,SLOT(s_StartEndChangeByUser()));

    // Edit controls
    connect(ui->DefStartPosBT,SIGNAL(clicked()),this,SLOT(s_DefStartPos()));
    connect(ui->DefEndPosBT,SIGNAL(clicked()),this,SLOT(s_DefEndPos()));
    connect(ui->SeekLeftBt,SIGNAL(clicked()),this,SLOT(s_SeekLeft()));
    connect(ui->SeekRightBt,SIGNAL(clicked()),this,SLOT(s_SeekRight()));
    connect(ui->StartPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditStartPos(QTime)));
    connect(ui->EndPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditEndPos(QTime)));
}

//====================================================================================================================

void DlgEditMusic::RefreshControls() {
    if (StopMaj) return;
    StopMaj=true;
    QTime Duration=QTime(0,0,0,0).addMSecs(MusicObject->StartPos.msecsTo(MusicObject->EndPos));
    ui->ActualDuration->setText(Duration.toString("hh:mm:ss.zzz"));
    ui->StartPosEd->setMaximumTime(MusicObject->EndPos);    ui->StartPosEd->setTime(MusicObject->StartPos);
    ui->EndPosEd->setMinimumTime(MusicObject->StartPos);    ui->EndPosEd->setTime(MusicObject->EndPos);
    StopMaj=false;
}

//====================================================================================================================
// Initiale Undo

void DlgEditMusic::PrepareGlobalUndo() {
    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    MusicObject->SaveToXML(&root,"UNDO-DLG-OBJECT","",true,NULL,NULL,false);  // Save object
    Undo->appendChild(root);                                // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgEditMusic::DoGlobalUndo() {
    if (ThreadAnalyseMusic.isRunning()) {
        ui->CustomRuler->IsAnalysed=true;
        ThreadAnalyseMusic.waitForFinished();
    }
    SetPlayerToPause();
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") MusicObject->LoadFromXML(&root,"UNDO-DLG-OBJECT","",NULL,NULL);
}

//====================================================================================================================

void DlgEditMusic::WinFocus() {
    SetPlayerToPlay();
}

void DlgEditMusic::LostFocus() {
    SetPlayerToPause();
}

//============================================================================================

void DlgEditMusic::s_PositionChangeByUser() {
    ResetPositionWanted=true;
}

//====================================================================================================================

void DlgEditMusic::s_StartEndChangeByUser() {
    if (StopMaj) return;
    MusicObject->StartPos=QTime(0,0,0,0).addMSecs(ui->CustomRuler->StartPos);
    MusicObject->EndPos  =QTime(0,0,0,0).addMSecs(ui->CustomRuler->EndPos);
    ui->StartPosEd->setTime(MusicObject->StartPos);
    ui->EndPosEd->setTime(MusicObject->EndPos);
}

//============================================================================================
// Seek slider public function
//============================================================================================

void DlgEditMusic::SeekPlayer(int Value) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ActualPosition=-1;
    s_SliderMoved(Value);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void DlgEditMusic::s_DefStartPos() {
    if (StopMaj) return;
    MusicObject->StartPos=GetCurrentPos();
    RefreshControls();
}

//====================================================================================================================

void DlgEditMusic::s_EditStartPos(QTime NewValue) {
    if (StopMaj) return;
    MusicObject->StartPos=NewValue;
    SetCurrentPos(MusicObject->StartPos);
    RefreshControls();
}

//====================================================================================================================

void DlgEditMusic::s_DefEndPos() {
    if (StopMaj) return;
    MusicObject->EndPos=GetCurrentPos();
    RefreshControls();
}

//====================================================================================================================

void DlgEditMusic::s_EditEndPos(QTime NewValue) {
    if (StopMaj) return;
    MusicObject->EndPos=NewValue;
    ui->EndPosEd->setTime(MusicObject->EndPos);
    SetCurrentPos(MusicObject->EndPos);
    RefreshControls();
}

//====================================================================================================================

void DlgEditMusic::s_SeekLeft() {
    SeekPlayer(QTime(0,0,0,0).msecsTo(MusicObject->StartPos));
    SetPlayerToPause();
}

//====================================================================================================================

void DlgEditMusic::s_SeekRight() {
    SeekPlayer(QTime(0,0,0,0).msecsTo(MusicObject->EndPos));
    SetPlayerToPause();
}

//============================================================================================
// Pause -> play
//============================================================================================

void DlgEditMusic::SetPlayerToPlay() {
    if (!(PlayerPlayMode && PlayerPauseMode)) return;
    PlayerPlayMode  = true;
    PlayerPauseMode = false;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPlay);
    SDLFlushBuffers();

    // Start timer
    TimerTick           =true;
    PreviousTimerEvent  =QTime();
    TimerDelta          =0;
    Timer.start(int(double(1000)/ApplicationConfig->PreviewFPS)/2);   // Start Timer
}

//============================================================================================
// Play -> pause
//============================================================================================

void DlgEditMusic::SetPlayerToPause() {
    if (!(PlayerPlayMode && !PlayerPauseMode)) return;
    Timer.stop();                                   // Stop Timer
    if (ThreadPrepareMusic.isRunning()) ThreadPrepareMusic.waitForFinished();
    PlayerMutex.lock();
    if (SDL_GetAudioStatus()==SDL_AUDIO_PLAYING) SDL_PauseAudio(1);
    MixedMusic.ClearList();                         // Free sound buffers
    Music.ClearList();                              // Free sound buffers
    FrameList.ClearList();                          // Free FrameList
    PlayerPlayMode  = true;
    PlayerPauseMode = true;
    ui->VideoPlayerPlayPauseBT->setIcon(IconPause);
    ui->BufferState->setValue(0);
    SDLFlushBuffers();
    PlayerMutex.unlock();
}

//============================================================================================
// Click on the play/pause button
//============================================================================================

void DlgEditMusic::s_VideoPlayerPlayPauseBT() {
    if ((!PlayerPlayMode)||((PlayerPlayMode && PlayerPauseMode)))    SetPlayerToPlay();      // Stop/Pause -> play
        else if (PlayerPlayMode && !PlayerPauseMode)                 SetPlayerToPause();     // Pause -> play
}

//============================================================================================
// return current position in QTime format
//============================================================================================

QTime DlgEditMusic::GetCurrentPos() {
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
// Set current position in QTime format
//============================================================================================

void DlgEditMusic::SetCurrentPos(QTime Pos) {
    SeekPlayer(QTime(0,0,0,0).msecsTo(Pos));
}

//============================================================================================
// Click on the handle of the slider
//============================================================================================

void DlgEditMusic::s_SliderPressed() {
    PreviousPause    = PlayerPauseMode;    // Save pause state
    IsSliderProcess  = true;
    SetPlayerToPause();
}

//============================================================================================
// En slider process
//============================================================================================

void DlgEditMusic::s_SliderReleased() {
    IsSliderProcess  = false;
    s_SliderMoved(ActualPosition);
    // Restore saved pause state
    if (!PreviousPause) SetPlayerToPlay();
}

//============================================================================================
// Slider is moving by user
// Slider is moving by timer
// Slider is moving by seek function
//============================================================================================

void DlgEditMusic::s_SliderMoved(int Value) {
    if (ResetPositionWanted) SetPlayerToPause();

    if (InSliderMoveEvent) return;
    InSliderMoveEvent=true;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Update display in controls
    ui->CustomRuler->setValue(Value);
    ActualPosition=Value;
    ui->Position->setText(GetCurrentPos().toString("hh:mm:ss.zzz"));

    if (PlayerPlayMode && !PlayerPauseMode) {
        if (ActualPosition>=QTime(0,0,0,0).msecsTo(MusicObject->Duration)) {
            SetPlayerToPause(); // Stop if it's the end
        } else if (FrameList.List.count()>1) {                        // Process
            cDiaporamaObjectInfo *Frame=(cDiaporamaObjectInfo *)FrameList.DetachFirstFrame();   // Retrieve frame information
            if ((PlayerPlayMode) && (!PlayerPauseMode) && (SDL_GetAudioStatus()!=SDL_AUDIO_PLAYING)) SDL_PauseAudio(0);    // start if no yet started
            delete Frame;
        }
    }

    QApplication::restoreOverrideCursor();
    InSliderMoveEvent=false;
}

//============================================================================================
// Timer event
//============================================================================================

void DlgEditMusic::s_TimerEvent() {
    // on windows, repaint can't be send by thread, so use a timer for that
    if (!ui->CustomRuler->IsAnalysed) {
        ui->CustomRuler->repaint();
        return;
    }
    if (IsSliderProcess)                            return;     // No re-entrance
    if (!(PlayerPlayMode && !PlayerPauseMode))      return;     // Only if play mode

    TimerTick=!TimerTick;

    #ifdef Q_OS_WIN
    // Trylock is always true on Windows instead of unix/linux system
    if (TimerTick) {
    #else
    if (!PlayerMutex.tryLock()) { if (!TimerTick) return; else {
    #endif
        // specific case for windows because never a timer event can happens if a previous timer event was not ended
        // so next trylock is always true
        int Elapsed=0,Wanted=int(double(1000)/ApplicationConfig->PreviewFPS);
        if (!PreviousTimerEvent.isValid()) PreviousTimerEvent.start(); else Elapsed=PreviousTimerEvent.restart();
        if (Elapsed>Wanted) {
            TimerDelta+=Elapsed-Wanted;
            if (TimerDelta>=Wanted) {
                ToLog(LOGMSG_DEBUGTRACE,"FPS preview is too high: One frame lost");
                if (FrameList.List.count()>0) {
                    delete (cDiaporamaObjectInfo *)FrameList.DetachFirstFrame(); // Remove first frame if we loose one tick
                } else {
                    // Increase next position to one frame
                    ActualPosition+=Wanted;
                }
                TimerDelta-=Wanted;
            }
        }
    }
    #ifdef Q_OS_WIN
    PlayerMutex.lock();
    #else
    return;}
    #endif

    if (ThreadPrepareMusic.isRunning()) ThreadPrepareMusic.waitForFinished();

    int64_t LastPosition=0,NextPosition=0;

    if (ResetPositionWanted) {
        Mutex.lock();
        MixedMusic.ClearList();                         // Free sound buffers
        Music.ClearList();                              // Free sound buffers
        FrameList.ClearList();                          // Free FrameList
        ResetPositionWanted=false;
        Mutex.unlock();
    }

    // If no frame in the list then create the first
    if (FrameList.List.count()==0) {
        cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(NULL,ActualPosition,NULL,double(1000)/ApplicationConfig->PreviewFPS,true);
        NewFrame->CurrentObject_StartTime   =0;
        PrepareSoundFrame(NewFrame,NewFrame->CurrentObject_InObjectTime);
    }

    cDiaporamaObjectInfo *PreviousFrame=(cDiaporamaObjectInfo *)FrameList.GetLastFrame();
    LastPosition=PreviousFrame?PreviousFrame->CurrentObject_InObjectTime:0;
    NextPosition=LastPosition+int(double(1000)/ApplicationConfig->PreviewFPS);

    // Add frame to the list if it's not full
    if ((FrameList.List.count()<BUFFERING_NBR_FRAME)&&(!ThreadPrepareMusic.isRunning())) {
        cDiaporamaObjectInfo *NewFrame=new cDiaporamaObjectInfo(PreviousFrame,NextPosition,NULL,int(double(1000)/ApplicationConfig->PreviewFPS),true);
        NewFrame->CurrentObject_StartTime   =0;
        ThreadPrepareMusic.setFuture(QtConcurrent::run(this,&DlgEditMusic::PrepareSoundFrame,NewFrame,NewFrame->CurrentObject_InObjectTime));
    }
    PlayerMutex.unlock();
    if (TimerTick) s_SliderMoved(((cDiaporamaObjectInfo *)FrameList.GetFirstFrame())->CurrentObject_StartTime+((cDiaporamaObjectInfo *)FrameList.GetFirstFrame())->CurrentObject_InObjectTime);

    ui->BufferState->setValue(FrameList.List.count());
    if (FrameList.List.count()<2)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 0px;\npadding-bottom: 0px;\npadding-left: 0px;\npadding-right: 0px;\n}\nQProgressBar::chunk:horizontal {\nbackground: red;\n}");
    else if (FrameList.List.count()<4)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 0px;\npadding-bottom: 0px;\npadding-left: 0px;\npadding-right: 0px;\n}\nQProgressBar::chunk:horizontal {\nbackground: yellow;\n}");
    else if (FrameList.List.count()<=BUFFERING_NBR_FRAME)
        ui->BufferState->setStyleSheet("QProgressBar:horizontal {\nborder: 0px;\nborder-radius: 0px;\nbackground: black;\npadding-top: 0px;\npadding-bottom: 0px;\npadding-left: 0px;\npadding-right: 0px;\n}\nQProgressBar::chunk:horizontal {\nbackground: green;\n}");
}

void DlgEditMusic::PrepareSoundFrame(cDiaporamaObjectInfo *NewFrame,int Position) {
    MusicObject->ReadFrame(true,Position*1000,true,false,&Music,1,true);
    for (int j=0;j<Music.NbrPacketForFPS;j++) MixedMusic.AppendPacket(Music.CurrentPosition,Music.DetachFirstPacket());
    FrameList.AppendFrame(NewFrame);
}
