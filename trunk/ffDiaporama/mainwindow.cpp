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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wgt_QCustomThumbnails.h"

#include "DlgAbout.h"
#include "DlgVideoProperties.h"
#include "DlgImageProperties.h"
#include "DlgBackgroundProperties.h"
#include "DlgMusicProperties.h"
#include "DlgSlideProperties.h"
#include "DlgTransitionProperties.h"
#include "DlgProject.h"
#include "DlgApplicationSettings.h"
#include "DlgRenderVideo.h"

#include "QSplashScreen"

MainWindow  *GlobalMainWindow=NULL;

//====================================================================================================================

MainWindow::MainWindow(cApplicationConfig *TheCurrentApplicationConfig,QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    GlobalMainWindow=this;
    IsFirstInitDone=false;                 // true when first show window was done
    ui->setupUi(this);
    FLAGSTOPITEMSELECTION   =false;        // Flag to stop Item Selection process for delete and move of object
    Clipboard               =NULL;
    ui->preview->FLAGSTOPITEMSELECTION=&FLAGSTOPITEMSELECTION;

    ApplicationConfig=TheCurrentApplicationConfig;
    ApplicationConfig->ParentWindow=this;

    QSplashScreen screen;
    screen.setPixmap(QPixmap("icons/splash.png"));
    screen.show();
    QCoreApplication::processEvents();  // Give time to interface !

    // Search if a BUILDVERSION.txt file exist
    QFile file("BUILDVERSION.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString Line=QString(file.readLine());
        if (Line.endsWith("\n")) Line=Line.left(Line.length()-QString("\n").length());
        AddToSystemProperties(QString(BUILDVERSION_STR)+QString(APPLICATION_NAME)+" "+QString(APPLICATION_VERSION)+" "+QString(Line));
     }
    file.close();
    AddToSystemProperties(QString(VERSIONQT_STR)+QString(qVersion()));
    AddToSystemProperties(QString(FMTFILTERVERSION_STR)+"0.6.4-Licence=LGPL");

    // Now, we have application settings then we can init SDL
    screen.showMessage(QCoreApplication::translate("MainWindow","Starting SDL..."),Qt::AlignHCenter|Qt::AlignBottom);
    SDLFirstInit(ApplicationConfig->PreviewFPS);
    AddToSystemProperties(QString(SDLVERSION_STR)+QString("%1").arg(SDL_MAJOR_VERSION)+"."+QString("%1").arg(SDL_MINOR_VERSION)+"."+QString("%1").arg(SDL_PATCHLEVEL)+"-Licence=GPL version 2.1 or later");

    // Register all formats and codecs for libavformat/libavcodec/etc ...
    screen.showMessage(QCoreApplication::translate("MainWindow","Starting ffmpeg..."),Qt::AlignHCenter|Qt::AlignBottom);
    avcodec_init();
    av_register_all();
    //QString Conf;

    AddToSystemProperties(QString(LIBAVCODECVERSION_STR)+QString("%1").arg(LIBAVCODEC_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MINOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MICRO)+"."+QString("%1").arg(avcodec_version())+"-Licence="+QString(avcodec_license()));
    //Conf=QString(avcodec_configuration());  Conf.replace(" --","\n  --"); AddToSystemProperties("  "+Conf+"\n");

    AddToSystemProperties(QString(LIBAVFORMATVERSION_STR)+QString("%1").arg(LIBAVFORMAT_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MINOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MICRO)+"."+QString("%1").arg(avformat_version())+"-Licence="+QString(avformat_license()));
    //Conf=QString(avformat_configuration());  Conf.replace(" --","\n  --");  AddToSystemProperties("  "+Conf+"\n");

    AddToSystemProperties(QString(LIBSWSCALEVERSION_STR)+QString("%1").arg(LIBSWSCALE_VERSION_MAJOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MINOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MICRO)+"."+QString("%1").arg(swscale_version())+"-Licence="+QString(swscale_license()));
    //Conf=QString(swscale_configuration());  Conf.replace(" --","\n  --");   AddToSystemProperties("  "+Conf+"\n");

    // Check codec to know if they was finded
    AVCodec *p=NULL;
    while ((p=av_codec_next(p))) {
        if (p->type==AVMEDIA_TYPE_AUDIO) {
            for (int i=0;i<NBR_AUDIOCODECDEF;i++) if ((p->id==AUDIOCODECDEF[i].Codec_id)&&(p->encode!=NULL)&&(!AUDIOCODECDEF[i].IsFind)) {
                AUDIOCODECDEF[i].IsFind=true;
                strcpy(AUDIOCODECDEF[i].ShortName,p->name);
            }
            if (QString(p->name)==QString("libfaac")) strcpy(AUDIOCODECDEF[2].ShortName,p->name);
        }
        if (p->type==AVMEDIA_TYPE_VIDEO) for (int i=0;i<NBR_VIDEOCODECDEF;i++) if ((p->id==VIDEOCODECDEF[i].Codec_id)&&(p->encode!=NULL)&&(!VIDEOCODECDEF[i].IsFind)) {
            VIDEOCODECDEF[i].IsFind=true;
            strcpy(VIDEOCODECDEF[i].ShortName,p->name);
        }
        if (QString(p->name)==QString("libxvid")) strcpy(VIDEOCODECDEF[2].ShortName,p->name);
    }

    // Check format to know if they was finded
    AVOutputFormat *ofmt=NULL;
    while ((ofmt=av_oformat_next(ofmt))) {
        for (int i=0;i<NBR_FORMATDEF;i++) if (strcmp(ofmt->name,FORMATDEF[i].ShortName)==0) {
            QString     AllowedCodec=FORMATDEF[i].PossibleVideoCodec;
            QString     Codec="";
            int         Index=0;
            bool        IsFindVideoCodec=false;
            bool        IsFindAudioCodec=false;

            while (AllowedCodec.length()>0) {
                Index=AllowedCodec.indexOf("#");
                if (Index>0) {
                    Codec=AllowedCodec.left(Index);
                    AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
                } else {
                    Codec=AllowedCodec;
                    AllowedCodec="";
                }
                // Now find index of this codec in the VIDEOCODECDEF
                Index=0;
                while ((Index<NBR_VIDEOCODECDEF)&&(Codec!=QString(VIDEOCODECDEF[Index].ShortName))) Index++;
                if ((Index<NBR_VIDEOCODECDEF)&&(VIDEOCODECDEF[Index].IsFind)) IsFindVideoCodec=true;
            }
            AllowedCodec=FORMATDEF[i].PossibleAudioCodec;
            Codec="";
            Index=0;
            while (AllowedCodec.length()>0) {
                Index=AllowedCodec.indexOf("#");
                if (Index>0) {
                    Codec=AllowedCodec.left(Index);
                    AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
                } else {
                    Codec=AllowedCodec;
                    AllowedCodec="";
                }
                // Now find index of this codec in the AUDIOCODECDEF
                Index=0;
                while ((Index<NBR_AUDIOCODECDEF)&&(Codec!=QString(AUDIOCODECDEF[Index].ShortName))) Index++;
                if ((Index<NBR_AUDIOCODECDEF)&&(AUDIOCODECDEF[Index].IsFind)) IsFindAudioCodec=true;
            }
            FORMATDEF[i].IsFind=IsFindAudioCodec && IsFindVideoCodec;
        }
    }

    // Display finding codecs & formats
    AddSeparatorToSystemProperties();   AddToSystemProperties(QCoreApplication::translate("MainWindow","Registered video codecs for encoding :"));
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) if (VIDEOCODECDEF[i].IsFind) AddToSystemProperties("  "+QString(VIDEOCODECDEF[i].LongName)+"-ffmpeg codec:"+QString(VIDEOCODECDEF[i].ShortName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QCoreApplication::translate("MainWindow","Registered audio codecs for encoding :"));
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) if (AUDIOCODECDEF[i].IsFind) AddToSystemProperties("  "+QString(AUDIOCODECDEF[i].LongName)+"-ffmpeg codec:"+QString(AUDIOCODECDEF[i].ShortName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QCoreApplication::translate("MainWindow","Registered container formats for encoding :"));
    for (int i=0;i<NBR_FORMATDEF;i++)     if (FORMATDEF[i].IsFind) AddToSystemProperties("  "+QString(FORMATDEF[i].LongName));

    AddSeparatorToSystemProperties();   AddToSystemProperties(QCoreApplication::translate("MainWindow","Library :"));
    QString Path;
    screen.showMessage(QCoreApplication::translate("MainWindow","Loading background library..."),Qt::AlignHCenter|Qt::AlignBottom);
    Path="background";      BackgroundList.ScanDisk(Path,GEOMETRY_16_9); AddToSystemProperties(QString("  %1").arg(BackgroundList.List.count())+QCoreApplication::translate("MainWindow"," images loaded into the background-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    screen.showMessage(QCoreApplication::translate("MainWindow","Loading no-luma transitions..."),Qt::AlignHCenter|Qt::AlignBottom);
    for (int i=0;i<TRANSITIONMAXSUBTYPE_BASE;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_BASE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_ZOOMINOUT;i++)  IconList.List.append(cIconObject(TRANSITIONFAMILLY_ZOOMINOUT,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_SLIDE;i++)      IconList.List.append(cIconObject(TRANSITIONFAMILLY_SLIDE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_PUSH;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_PUSH,i));
    AddToSystemProperties(QString("  %1").arg(IconList.List.count())+QCoreApplication::translate("MainWindow"," no-luma transitions loaded into the transition-library"));
    screen.showMessage(QCoreApplication::translate("MainWindow","Loading luma transitions..."),Qt::AlignHCenter|Qt::AlignBottom);
    Path="luma/Bar";        LumaList_Bar.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BAR);         AddToSystemProperties(QString("  %1").arg(LumaList_Bar.List.count())+QCoreApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Box";        LumaList_Box.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BOX);         AddToSystemProperties(QString("  %1").arg(LumaList_Box.List.count())+QCoreApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Center";     LumaList_Center.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CENTER);   AddToSystemProperties(QString("  %1").arg(LumaList_Center.List.count())+QCoreApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Checker";    LumaList_Checker.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CHECKER); AddToSystemProperties(QString("  %1").arg(LumaList_Checker.List.count())+QCoreApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Clock";      LumaList_Clock.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CLOCK);     AddToSystemProperties(QString("  %1").arg(LumaList_Clock.List.count())+QCoreApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Snake";      LumaList_Snake.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_SNAKE);     AddToSystemProperties(QString("  %1").arg(LumaList_Snake.List.count())+QCoreApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    AddToSystemProperties(QCoreApplication::translate("MainWindow","  Total:")+QString("%1").arg(IconList.List.count())+QCoreApplication::translate("MainWindow"," transitions loaded into the transition-library"));

    // Force timeline scroll bar properties
    ui->timeline->horizontalScrollBar()->setStyleSheet("height: 14px; margin: 0px; padding: 0px;");
    ui->timeline->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->timeline->verticalHeader()->setResizeMode(QHeaderView::Fixed);

    Diaporama=new cDiaporama(ApplicationConfig);
    Diaporama->Timeline=ui->timeline;
    ui->preview->InitDiaporamaPlay(Diaporama);

    SetModifyFlag(false);
    ui->ZoomMinusBT->setEnabled(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH);
    ui->ZoomPlusBT->setEnabled(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH);

    screen.hide();

    // Help menu
    connect(ui->action_About,SIGNAL(triggered()),this,SLOT(s_About()));
    connect(ui->actionDocumentation,SIGNAL(triggered()),this,SLOT(s_Documentation()));
    connect(ui->actionNewFunctions,SIGNAL(triggered()),this,SLOT(s_NewFunctions()));

    // File menu
    connect(ui->action_New,SIGNAL(triggered()),this,SLOT(s_action_New()));
    connect(ui->action_Open,SIGNAL(triggered()),this,SLOT(s_action_Open()));
    connect(ui->action_Save,SIGNAL(triggered()),this,SLOT(s_action_Save()));
    connect(ui->actionSave_as,SIGNAL(triggered()),this,SLOT(s_action_SaveAs()));
    connect(ui->action_Exit,SIGNAL(triggered()),this,SLOT(s_action_Exit()));

    // Project menu
    connect(ui->actionAdd,SIGNAL(triggered()),this,SLOT(s_action_AddFile()));
    connect(ui->actionAddtitle,SIGNAL(triggered()),this,SLOT(s_action_AddTitle()));
    connect(ui->actionAddProject,SIGNAL(triggered()),this,SLOT(s_action_AddProject()));
    connect(ui->actionRemove,SIGNAL(triggered()),this,SLOT(s_RemoveObject()));
    connect(ui->actionMove_left,SIGNAL(triggered()),this,SLOT(s_LeftObject()));
    connect(ui->actionMove_right,SIGNAL(triggered()),this,SLOT(s_RightObject()));
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(s_CutToClipboard()));
    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(s_CopyToClipboard()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(s_PasteFromClipboard()));
    connect(ui->actionChangeProjectSettings,SIGNAL(triggered()),this,SLOT(s_ChangeProjectSettings()));
    connect(ui->ChangeProjectSettingsBt,SIGNAL(pressed()),this,SLOT(s_ChangeProjectSettings()));
    connect(ui->actionEdit_background,SIGNAL(triggered()),this,SLOT(s_BackgroundDoubleClicked()));
    connect(ui->actionEdit_background_transition,SIGNAL(triggered()),this,SLOT(s_TransitionBackgroundDoubleClicked()));
    connect(ui->actionEdit_object,SIGNAL(triggered()),this,SLOT(s_ItemDoubleClicked()));
    connect(ui->actionEdit_object_in_transition,SIGNAL(triggered()),this,SLOT(s_TransitionItemDoubleClicked()));
    connect(ui->actionEdit_music,SIGNAL(triggered()),this,SLOT(s_MusicDoubleClicked()));

    // Tools menu
    connect(ui->actionRender,SIGNAL(triggered()),this,SLOT(s_RenderVideo()));
    connect(ui->actionConfiguration,SIGNAL(triggered()),this,SLOT(s_ChangeApplicationSettings()));      connect(ui->ConfigurationBt,SIGNAL(pressed()),this,SLOT(s_ChangeApplicationSettings()));

    // Timeline
    connect(ui->ZoomPlusBT,SIGNAL(pressed()),this,SLOT(s_action_ZoomPlus()));
    connect(ui->ZoomMinusBT,SIGNAL(pressed()),this,SLOT(s_action_ZoomMinus()));
    connect(ui->timeline,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));
}

//====================================================================================================================

MainWindow::~MainWindow() {
    delete ui;
    delete Diaporama;
    delete ApplicationConfig;
    SDLLastClose();
}

//====================================================================================================================

void MainWindow::SetTimelineHeight() {
    ui->timeline->setFixedHeight(
            15+                                     // Horizontal slider and marges
            ApplicationConfig->TimelineHeight/2+    // Background
            ApplicationConfig->TimelineHeight+      // Montage
            TIMELINESOUNDHEIGHT*2+4                 // Music
    );
    ui->timeline->setRowHeight(TRACKBACKGROUND,ApplicationConfig->TimelineHeight/2);    // Background
    ui->timeline->setRowHeight(TRACKMONTAGE,ApplicationConfig->TimelineHeight);         // Montage
    ui->timeline->setRowHeight(TRACKMUSIC,TIMELINESOUNDHEIGHT*2);                       // Music
    for (int i=0;i<ui->timeline->columnCount();i++) ui->timeline->setColumnWidth(i,Diaporama->GetWidthForHeight(ui->timeline->rowHeight(TRACKMONTAGE)-5)+32+ADJUSTXCOLUMN);
    ui->preview->setFixedWidth(Diaporama->GetWidthForHeight(ui->centralwidget->height()-ui->timeline->height()-31));
    ui->preview->repaint();
}

//====================================================================================================================

void MainWindow::closeEvent(QCloseEvent *) {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QCoreApplication::translate("MainWindow","Close application"),QCoreApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_action_Save();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ApplicationConfig->MainWinWSP->SaveWindowState(this);
    ApplicationConfig->SaveConfigurationFile();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::resizeEvent(QResizeEvent *) {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    SetTimelineHeight();
}

//====================================================================================================================

void MainWindow::showEvent(QShowEvent *) {
    if (!IsFirstInitDone) {
        IsFirstInitDone=true;                                   // do this only one time
        ApplicationConfig->MainWinWSP->ApplyToWindow(this);     // Restore window position
        SetTimelineHeight();                                    // setup initial size
        RefreshControls();
    }
}

//====================================================================================================================

void MainWindow::RefreshControls() {
    // Timeline actions
    ui->actionRemove->setEnabled(ui->timeline->columnCount()>0);
    ui->actionMove_left->setEnabled((ui->timeline->currentColumn()>0)&&(ui->timeline->columnCount()>1));
    ui->actionMove_right->setEnabled(ui->timeline->currentColumn()<ui->timeline->columnCount()-1);

    // File menu
    ui->action_Save->setEnabled(Diaporama->IsModify);
    ui->actionSave_as->setEnabled(Diaporama->IsModify);

    // Project menu
    ui->actionEdit_background->setEnabled(ui->timeline->columnCount()>0);
    ui->actionEdit_background_transition->setEnabled(ui->timeline->columnCount()>0);
    ui->actionEdit_object->setEnabled(ui->timeline->columnCount()>0);
    ui->actionEdit_object_in_transition->setEnabled(ui->timeline->columnCount()>0);
    ui->actionEdit_music->setEnabled(ui->timeline->columnCount()>0);

    // Clipboard
    ui->actionPaste->setEnabled(Clipboard!=NULL);
    ui->actionCopy->setEnabled(ui->timeline->currentColumn()>=0);
    ui->actionCut->setEnabled(ui->timeline->currentColumn()>=0);

    ui->actionRender->setEnabled(ui->timeline->columnCount()>0);
}

//====================================================================================================================

void MainWindow::SetModifyFlag(bool IsModify) {
    Diaporama->IsModify=IsModify;
    this->setWindowTitle(QString(APPLICATION_NAME)+QString(" ")+QString(APPLICATION_VERSION)+QString("-")+
                         (Diaporama->ProjectFileName!=""?Diaporama->ProjectFileName:QCoreApplication::translate("MainWindow","<new project>","when project have no name define"))+
                         (Diaporama->IsModify?"*":""));
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_About() {
    DlgAbout(this).exec();
}

//====================================================================================================================

void MainWindow::s_Documentation() {
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/Support.php")));
}

//====================================================================================================================

void MainWindow::s_NewFunctions() {
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/News.php")));
}

//====================================================================================================================

void MainWindow::s_action_Exit() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    close();
}

//====================================================================================================================

void MainWindow::s_action_ZoomPlus() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (ui->timeline->rowHeight(TRACKMONTAGE)<TIMELINEMAXHEIGH) ApplicationConfig->TimelineHeight+=20;
    ui->ZoomMinusBT->setEnabled(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH);
    ui->ZoomPlusBT->setEnabled(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH);
    SetTimelineHeight();
}

//====================================================================================================================

void MainWindow::s_action_ZoomMinus() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (ui->timeline->rowHeight(TRACKMONTAGE)>TIMELINEMINHEIGH) ApplicationConfig->TimelineHeight-=20;
    ui->ZoomMinusBT->setEnabled(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH);
    ui->ZoomPlusBT->setEnabled(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH);
    SetTimelineHeight();
}

//====================================================================================================================
// Double click on widget in the object track
//====================================================================================================================

void MainWindow::s_ItemDoubleClicked() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    bool IsOk=false;
    switch (Diaporama->List[Diaporama->CurrentCol].TypeObject) {
        case DIAPORAMAOBJECTTYPE_EMPTY: IsOk=(DlgSlideProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec()==0);     break;
        case DIAPORAMAOBJECTTYPE_IMAGE: IsOk=(DlgImageProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec()==0);     break;
        case DIAPORAMAOBJECTTYPE_VIDEO: IsOk=(DlgVideoProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec()==0);     break;
    }
    if (IsOk) {
        SetModifyFlag(true);
        if (Diaporama->List[Diaporama->CurrentCol].Thumbnail) {
            delete Diaporama->List[Diaporama->CurrentCol].Thumbnail;
            Diaporama->List[Diaporama->CurrentCol].Thumbnail=NULL;
        }
        ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Double click on transition part of widget in the object track
//====================================================================================================================

void MainWindow::s_TransitionItemDoubleClicked() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (DlgTransitionProperties(&(Diaporama->List[Diaporama->CurrentCol]),false,this).exec()==0) {
        SetModifyFlag(true);
        ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Double click on sound part of widget in the object track
//====================================================================================================================

void MainWindow::s_SoundItemDoubleClicked() {
    s_ItemDoubleClicked();  // No separated process at this time !
}

//====================================================================================================================
// // Double click on widget in the background track
//====================================================================================================================

void MainWindow::s_BackgroundDoubleClicked() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (DlgBackgroundProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec()==0) {
        SetModifyFlag(true);
        ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Double click on transition part of widget in the background track
//====================================================================================================================

void MainWindow::s_TransitionBackgroundDoubleClicked() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    QMessageBox::critical(this,QCoreApplication::translate("MainWindow","Not implemented"),QCoreApplication::translate("MainWindow","Sorry, not yet done !"));
/*
    if (DlgTransitionProperties(&(Diaporama->List[Diaporama->CurrentCol]),true,this).exec()==0) {
        SetModifyFlag(true);
        ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
*/
}

//====================================================================================================================
// // Double click on widget in the music track
//====================================================================================================================

void MainWindow::s_MusicDoubleClicked() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (DlgMusicProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec()==0) {
        SetModifyFlag(true);
        ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Current diaporama object selection changed
//====================================================================================================================

void MainWindow::s_ItemSelectionChanged() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (!FLAGSTOPITEMSELECTION) {
        if (Diaporama->CurrentCol<0) Diaporama->CurrentCol=0;
        if ((Diaporama->CurrentCol!=ui->timeline->currentColumn())||((Diaporama->List.count()==1)&&(Diaporama->CurrentCol==0))) {
            //************************************************************
            // We are here only if user has click on the timeline
            //************************************************************
            ui->preview->SetPlayerToPause(); // Ensure player is stop
            if (Diaporama->List.count()>0) {
                Diaporama->CurrentCol=ui->timeline->currentColumn();
                Diaporama->CurrentPosition=Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol);
                if ((Diaporama->List[Diaporama->CurrentCol].TransitionFamilly!=0)||(Diaporama->List[Diaporama->CurrentCol].TransitionSubType!=0)) Diaporama->CurrentPosition--;
                ui->preview->SeekPlayer(Diaporama->CurrentPosition);
                AdjustRuller();
            } else {
                ui->preview->SeekPlayer(0);
                ui->preview->SetStartEndPos(0,0,-1,0,-1,0);
            }
        }
    }
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Update dock informations
//====================================================================================================================

void MainWindow::UpdateDockInfo() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    while (ui->TableInfo->rowCount()>0) ui->TableInfo->removeRow(0);
    if ((Diaporama->CurrentCol>=0)&&(Diaporama->CurrentCol<Diaporama->List.count())) {
        cDiaporamaObject *Obj=&Diaporama->List[Diaporama->CurrentCol];

        ui->TableInfo->insertRow(ui->TableInfo->rowCount());
        ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Object type")));
        ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(Obj->TypeObject==DIAPORAMAOBJECTTYPE_EMPTY?QCoreApplication::translate("MainWindow","Title"):
                                                                                  Obj->TypeObject==DIAPORAMAOBJECTTYPE_IMAGE?QCoreApplication::translate("MainWindow","Image"):
                                                                                  QCoreApplication::translate("MainWindow","Video")));

        ui->TableInfo->insertRow(ui->TableInfo->rowCount());
        ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Duration")));
        ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem((QString("%1").arg(double(Obj->GetDuration())/1000,0,'f',1)).trimmed()));

        if ((Obj->Image!=NULL)||(Obj->Video!=NULL)) {
            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Filename")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(Obj->Image!=NULL?QFileInfo(Obj->Image->FileName).fileName():Obj->Video!=NULL?QFileInfo(Obj->Video->FileName).fileName():""));
        }
        if (Obj->Image!=NULL) for (int i=0;i<Obj->Image->ExivValue.count();i++) {
            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            QString Value=Obj->Image->ExivValue[i];
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(Value.left(Value.indexOf("##"))));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(Value.right(Value.length()-Value.indexOf("##")-QString("##").length())));
        } else if (Obj->Video!=NULL) {
            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Image size")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1x%2").arg(Obj->Video->ImageWidth).arg(Obj->Video->ImageHeight)));

            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Video format")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(Obj->Video->VideoDecoderCodec->name));

            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Bitrate")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(int(double(Obj->Video->ffmpegVideoFile->bit_rate)/1024))+" k/s"));

            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Frame rate")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(
                    int(double(Obj->Video->ffmpegVideoFile->streams[Obj->Video->VideoStreamNumber]->r_frame_rate.num)/
                        double(Obj->Video->ffmpegVideoFile->streams[Obj->Video->VideoStreamNumber]->r_frame_rate.den)))
                    +" "+QCoreApplication::translate("MainWindow","fps","frame per second")));

            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Aspect ratio")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg((double(Obj->Video->ImageWidth)*Obj->Video->AspectRatio)/double(Obj->Video->ImageHeight),0,'f',3)));

            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Audio format")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(Obj->Video->AudioDecoderCodec->name));

            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Frequency")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(Obj->Video->ffmpegVideoFile->streams[Obj->Video->AudioStreamNumber]->codec->sample_rate)
                    +" "+QCoreApplication::translate("MainWindow","hz","audio frequency")));

            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QCoreApplication::translate("MainWindow","Channels")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(Obj->Video->ffmpegVideoFile->streams[Obj->Video->AudioStreamNumber]->codec->channels)));
        }
        ui->TableInfo->resizeColumnsToContents();
        ui->TableInfo->resizeRowsToContents();
    }
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Change actual project settings
//====================================================================================================================

void MainWindow::s_RenderVideo() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    DlgRenderVideo(*Diaporama,this).exec();
    SetModifyFlag(true);
    AdjustRuller();
}

//====================================================================================================================
// Change actual project settings
//====================================================================================================================

void MainWindow::s_ChangeProjectSettings() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (DlgProject(*Diaporama,false,this).exec()==0) {
        SetModifyFlag(true);
        AdjustRuller();
    }
}

//====================================================================================================================
// Change application settings
//====================================================================================================================

void MainWindow::s_ChangeApplicationSettings() {
    ui->preview->SetPlayerToPause();                            // Ensure player is stop
    DlgApplicationSettings(*ApplicationConfig,this).exec();
    SDLSetFPS(ApplicationConfig->PreviewFPS);                   // Reinit SDL if Preview FPS has changed
}

//====================================================================================================================
// New project
//====================================================================================================================

void MainWindow::s_action_New() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QCoreApplication::translate("MainWindow","New project"),QCoreApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_action_Save();

    cDiaporama *NewDiaporama=new cDiaporama(ApplicationConfig);
    if (DlgProject(*NewDiaporama,true,this).exec()==0) {

        // Clean actual timeline and diaporama
        FLAGSTOPITEMSELECTION=true;
        ui->timeline->setUpdatesEnabled(false);
        while (ui->timeline->columnCount()>0) ui->timeline->removeColumn(ui->timeline->columnCount()-1);
        delete Diaporama;
        Diaporama=NULL;
        ui->timeline->setUpdatesEnabled(true);
        FLAGSTOPITEMSELECTION=false;

        // Create new diaporama
        Diaporama=NewDiaporama;
        BackgroundList.ScanDisk("background",Diaporama->ImageGeometry);
        Diaporama->Timeline=ui->timeline;
        ui->preview->InitDiaporamaPlay(Diaporama);
        ui->preview->SetActualDuration(Diaporama->GetDuration());
        ui->preview->SetStartEndPos(0,0,-1,0,-1,0);
        SetTimelineHeight();
        RefreshControls();
        ui->preview->Resize();
        SetModifyFlag(false);
        UpdateDockInfo();
    } else delete NewDiaporama;
}

//====================================================================================================================
// Open an existing project
//====================================================================================================================

void MainWindow::s_action_Open() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QCoreApplication::translate("MainWindow","Open project"),QCoreApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_action_Save();

    QCoreApplication::processEvents();
    QString ProjectFileName=QFileDialog::getOpenFileName(this,QCoreApplication::translate("MainWindow","Open project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (ProjectFileName!="") {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        ApplicationConfig->LastProjectPath=QFileInfo(ProjectFileName).dir().absolutePath();
        // Clean actual timeline and diaporama
        FLAGSTOPITEMSELECTION=true;
        ui->timeline->setUpdatesEnabled(false);
        while (ui->timeline->columnCount()>0) ui->timeline->removeColumn(ui->timeline->columnCount()-1);
        delete Diaporama;
        Diaporama=NULL;
        ui->timeline->setUpdatesEnabled(true);
        FLAGSTOPITEMSELECTION=false;

        // Create new diaporama
        Diaporama=new cDiaporama(ApplicationConfig);
        //BackgroundList.ScanDisk("background",Diaporama->ImageGeometry); // Reload list with the correct geometry
        //LumaList.ScanDisk("luma",Diaporama->ImageGeometry);             // Reload list with the correct geometry
        Diaporama->Timeline=ui->timeline;

        // Init GUI for this project
        ui->preview->InitDiaporamaPlay(Diaporama);
        SetTimelineHeight();

        // Load file
        Diaporama->LoadFile(this,ProjectFileName);
        ui->timeline->setCurrentCell(0,0);
        SetModifyFlag(false);
        AdjustRuller();
        QApplication::restoreOverrideCursor();
        if (Diaporama->List.count()>0) ui->preview->SeekPlayer(Diaporama->List[0].TransitionDuration); else ui->preview->SeekPlayer(0);
    }
}

//====================================================================================================================
// Save current project
//====================================================================================================================

void MainWindow::s_action_Save() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (Diaporama->ProjectFileName=="") s_action_SaveAs(); else {
        if (Diaporama->SaveFile(this)) SetModifyFlag(false);
    }
}

//====================================================================================================================
// Save current project as
//====================================================================================================================

void MainWindow::s_action_SaveAs() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    Diaporama->ProjectFileName=QFileDialog::getSaveFileName(this,QCoreApplication::translate("MainWindow","Save project as"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (Diaporama->ProjectFileName!="") {
        if (QFileInfo(Diaporama->ProjectFileName).suffix()!="ffd") Diaporama->ProjectFileName=Diaporama->ProjectFileName+".ffd";
        ApplicationConfig->LastProjectPath=QFileInfo(Diaporama->ProjectFileName).dir().absolutePath();
        if (Diaporama->SaveFile(this)) SetModifyFlag(false);
    }
}

//====================================================================================================================
// Add a title object
//====================================================================================================================

void MainWindow::s_action_AddTitle() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int SavedCurIndex=ApplicationConfig->AppendObject?Diaporama->List.count():Diaporama->CurrentCol;
    int CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
    if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;

    Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
    Diaporama->List[CurIndex].List[0].Parent=&Diaporama->List[CurIndex];
//
    Diaporama->List[CurIndex].List[0].DefaultStaticDuration=0;
    Diaporama->List[CurIndex].List[0].DefaultMobilDuration=0;
//
    Diaporama->List[CurIndex].TypeObject=DIAPORAMAOBJECTTYPE_EMPTY;
    Diaporama->List[CurIndex].ApplyDefaultFraming(ApplicationConfig->DefaultFraming);
    if (Diaporama->ApplicationConfig->RandomTransition) {
        qsrand(QTime(0,0,0,0).msecsTo(QTime::currentTime()));
        int Random=qrand();
        Random=int(double(IconList.List.count())*(double(Random)/double(RAND_MAX)));
        if (Random<IconList.List.count()) {
            Diaporama->List[CurIndex].TransitionFamilly=IconList.List[Random].TransitionFamilly;
            Diaporama->List[CurIndex].TransitionSubType=IconList.List[Random].TransitionSubType;
        }
    } else {
        Diaporama->List[CurIndex].TransitionFamilly=Diaporama->ApplicationConfig->DefaultTransitionFamilly;
        Diaporama->List[CurIndex].TransitionSubType=Diaporama->ApplicationConfig->DefaultTransitionSubType;
    }
    Diaporama->List[CurIndex].TransitionDuration=Diaporama->ApplicationConfig->DefaultTransitionDuration;
    AddObjectToTimeLine(CurIndex);
    Diaporama->Timeline->setCurrentCell(TRACKMONTAGE,SavedCurIndex+1);
    SetModifyFlag(true);
    AdjustRuller();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Add an object (image or video)
//====================================================================================================================

void MainWindow::s_action_AddFile() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop

    QStringList FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("MainWindow","Add files"),
                                                       ApplicationConfig->RememberLastDirectories?ApplicationConfig->LastMediaPath:"",
                                                       ApplicationConfig->GetFilterForMediaFile(cApplicationConfig::ALLFILE));
    QCoreApplication::processEvents();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // Calc position of new object depending on ApplicationConfig->AppendObject
    int SavedCurIndex=0;
    int CurIndex     =0;

    if (ApplicationConfig->AppendObject) {
        SavedCurIndex   =Diaporama->List.count();
        CurIndex        =Diaporama->List.count();
    } else {
        SavedCurIndex=Diaporama->CurrentCol;
        CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
        if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;
    }
    // Add files
    for (int i=0;i<FileList.count();i++) {
        QString NewFile=FileList[i];
        if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

        Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
        Diaporama->List[CurIndex].List[0].Parent=&Diaporama->List[CurIndex];
        if (Diaporama->List[CurIndex].LoadMedia(NewFile,ApplicationConfig->AllowImageExtension.contains(QFileInfo(NewFile).suffix())?DIAPORAMAOBJECTTYPE_IMAGE:DIAPORAMAOBJECTTYPE_VIDEO)) {
            Diaporama->List[CurIndex].ApplyDefaultFraming(ApplicationConfig->DefaultFraming);
            if (Diaporama->ApplicationConfig->RandomTransition) {
                qsrand(QTime(0,0,0,0).msecsTo(QTime::currentTime()));
                int Random=qrand();
                Random=int(double(IconList.List.count())*(double(Random)/double(RAND_MAX)));
                if (Random<IconList.List.count()) {
                    Diaporama->List[CurIndex].TransitionFamilly=IconList.List[Random].TransitionFamilly;
                    Diaporama->List[CurIndex].TransitionSubType=IconList.List[Random].TransitionSubType;
                }
            } else {
                Diaporama->List[CurIndex].TransitionFamilly=Diaporama->ApplicationConfig->DefaultTransitionFamilly;
                Diaporama->List[CurIndex].TransitionSubType=Diaporama->ApplicationConfig->DefaultTransitionSubType;
            }
            Diaporama->List[CurIndex].TransitionDuration=Diaporama->ApplicationConfig->DefaultTransitionDuration;
            AddObjectToTimeLine(CurIndex);
            CurIndex++;
            SetModifyFlag(true);
        } else {
            QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+QCoreApplication::translate("MainWindow","Format not supported","Error message"),QMessageBox::Close);
            Diaporama->List.removeAt(CurIndex);
        }
    }

    // Set current selection to first new object
    Diaporama->Timeline->setCurrentCell(TRACKMONTAGE,SavedCurIndex+1);

    // Set title flag
    AdjustRuller();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Add an (sub) project
//====================================================================================================================

void MainWindow::s_action_AddProject() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    QCoreApplication::processEvents();
    QString ProjectFileName=QFileDialog::getOpenFileName(this,QCoreApplication::translate("MainWindow","Add a sub project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (ProjectFileName!="") {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        ApplicationConfig->LastProjectPath=QFileInfo(ProjectFileName).dir().absolutePath();
        Diaporama->AppendFile(this,ProjectFileName);  // Append file
        SetModifyFlag(true);
        AdjustRuller();
        QApplication::restoreOverrideCursor();
    }
}

//====================================================================================================================

void MainWindow::AddObjectToTimeLine(int CurIndex) {
    wgt_QCustomThumbnails *ObjectBackground=new wgt_QCustomThumbnails(ui->timeline,Diaporama,THUMBNAILTYPE_OBJECTBACKGROUND);
    connect(ObjectBackground,SIGNAL(EditMediaObject()),this,SLOT(s_BackgroundDoubleClicked()));
    connect(ObjectBackground,SIGNAL(EditTransition()),this,SLOT(s_TransitionBackgroundDoubleClicked()));

    wgt_QCustomThumbnails *ObjectObject    =new wgt_QCustomThumbnails(ui->timeline,Diaporama,THUMBNAILTYPE_OBJECTSEQUENCE);
    connect(ObjectObject,SIGNAL(EditMediaObject()),this,SLOT(s_ItemDoubleClicked()));
    connect(ObjectObject,SIGNAL(EditTransition()),this,SLOT(s_TransitionItemDoubleClicked()));
    connect(ObjectObject,SIGNAL(EditSoundTrack()),this,SLOT(s_SoundItemDoubleClicked()));

    wgt_QCustomThumbnails *ObjectMusic     =new wgt_QCustomThumbnails(ui->timeline,Diaporama,THUMBNAILTYPE_OBJECTMUSIC);
    connect(ObjectMusic,SIGNAL(EditSoundTrack()),this,SLOT(s_MusicDoubleClicked()));

    ui->timeline->insertColumn(CurIndex);
    ui->timeline->setColumnWidth(CurIndex,Diaporama->GetWidthForHeight(ui->timeline->rowHeight(TRACKMONTAGE)-5)+32+ADJUSTXCOLUMN);
    ui->timeline->setCellWidget(TRACKBACKGROUND,CurIndex,ObjectBackground);
    ui->timeline->setCellWidget(TRACKMONTAGE,CurIndex,ObjectObject);
    ui->timeline->setCellWidget(TRACKMUSIC,CurIndex,ObjectMusic);

    ui->preview->SetActualDuration(Diaporama->GetDuration());   // Refresh duration
    if ((Diaporama->CurrentCol<0)||(Diaporama->CurrentCol==CurIndex)) {
        Diaporama->CurrentCol=CurIndex;
        ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(CurIndex)+Diaporama->GetTransitionDuration(CurIndex));
        AdjustRuller();
    }
    QCoreApplication::processEvents();
}

//====================================================================================================================

void MainWindow::s_RemoveObject() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    int Current=ui->timeline->currentColumn();
    if ((Current<0)||(Current>=Diaporama->List.count())) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->timeline->setUpdatesEnabled(false);
    FLAGSTOPITEMSELECTION=true;
    Diaporama->List.removeAt(Current);
    ui->timeline->removeColumn(Current);
    FLAGSTOPITEMSELECTION=false;
    if (Current>=Diaporama->List.count()) Current=Diaporama->List.count()-1;
    ui->timeline->setCurrentCell(TRACKMONTAGE,Current);
    ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(Current)+Diaporama->GetTransitionDuration(Current));
    SetModifyFlag(true);
    AdjustRuller();
    ui->timeline->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_LeftObject() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    int Current=ui->timeline->currentColumn();
    if ((Current<1)||(Current>=Diaporama->List.count())) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->timeline->setUpdatesEnabled(false);
    Diaporama->List.swap(Current,Current-1);
    ui->timeline->setCurrentCell(TRACKMONTAGE,Current-1);
    ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(Current-1)+Diaporama->GetTransitionDuration(Current-1));
    SetModifyFlag(true);
    AdjustRuller();
    ui->timeline->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_RightObject() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    int Current=ui->timeline->currentColumn();
    if ((Current<0)||(Current>=Diaporama->List.count()-1)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->timeline->setUpdatesEnabled(false);
    Diaporama->List.swap(Current,Current+1);
    ui->timeline->setCurrentCell(TRACKMONTAGE,Current+1);
    ui->preview->SeekPlayer(Diaporama->GetObjectStartPosition(Current+1)+Diaporama->GetTransitionDuration(Current+1));
    SetModifyFlag(true);
    AdjustRuller();
    ui->timeline->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_CutToClipboard() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    int Current=ui->timeline->currentColumn();
    if ((Current<0)||(Current>Diaporama->List.count()-1)) return;

    QDomElement root;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (Clipboard!=NULL) delete Clipboard;
    Clipboard=new QDomDocument(APPLICATION_NAME);

    // Create xml document and root
    root=Clipboard->createElement("CLIPBOARD");
    Clipboard->appendChild(root);
    Diaporama->List[Current].SaveToXML(root,"CLIPBOARD-OBJECT","");

    s_RemoveObject();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_CopyToClipboard() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    int Current=ui->timeline->currentColumn();
    if ((Current<0)||(Current>Diaporama->List.count()-1)) return;

    QDomElement root;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (Clipboard!=NULL) delete Clipboard;
    Clipboard=new QDomDocument(APPLICATION_NAME);

    // Create xml document and root
    root=Clipboard->createElement("CLIPBOARD");
    Clipboard->appendChild(root);
    Diaporama->List[Current].SaveToXML(root,"CLIPBOARD-OBJECT","");

    RefreshControls();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_PasteFromClipboard() {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    // Calc position of new object depending on ApplicationConfig->AppendObject
    int SavedCurIndex=ApplicationConfig->AppendObject?Diaporama->List.count():Diaporama->CurrentCol;
    int CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
    if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;

    Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
    //Diaporama->List[CurIndex].List[0].Parent=&Diaporama->List[CurIndex];

    QDomElement root=Clipboard->documentElement();
    if (root.tagName()=="CLIPBOARD") {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        ui->timeline->setUpdatesEnabled(false);
        Diaporama->List[CurIndex].LoadFromXML(root,"CLIPBOARD-OBJECT","");
        AddObjectToTimeLine(CurIndex);
        SetModifyFlag(true);
        // Set current selection to first new object
        Diaporama->Timeline->setCurrentCell(TRACKMONTAGE,SavedCurIndex+1);
        AdjustRuller();
        ui->timeline->setUpdatesEnabled(true);
        QApplication::restoreOverrideCursor();
    }
}

//====================================================================================================================
// Adjust preview ruller depending on current Disporama Currentcol
//====================================================================================================================

void MainWindow::AdjustRuller() {
    ui->preview->SetActualDuration(Diaporama->GetDuration());
    if (Diaporama->List.count()>0)  {
        ui->preview->SetStartEndPos(
                Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                           // Current slide
                Diaporama->List[Diaporama->CurrentCol].GetDuration(),
                (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1),                            // Previous slide
                (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1].GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,    // Next slide
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1].GetDuration():0);
    } else ui->preview->SetStartEndPos(0,0,-1,0,-1,0);
    ui->timeline->repaint();
    ui->preview->repaint();
    UpdateDockInfo();
    RefreshControls();
}
