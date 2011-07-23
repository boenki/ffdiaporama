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
#include "DlgBackgroundProperties.h"
#include "DlgMusicProperties.h"
#include "DlgSlideProperties.h"
#include "DlgTransitionProperties.h"
#include "DlgApplicationSettings.h"
#include "DlgRenderVideo.h"

MainWindow  *GlobalMainWindow=NULL;

//====================================================================================================================

MainWindow::MainWindow(cApplicationConfig *TheCurrentApplicationConfig,QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    InternetBUILDVERSION    ="";
    GlobalMainWindow        =this;
    IsFirstInitDone         =false;                 // true when first show window was done
    FLAGSTOPITEMSELECTION   =false;        // Flag to stop Item Selection process for delete and move of object
    Clipboard_Object        =NULL;
    Clipboard_Block         =NULL;
    ui->preview->FLAGSTOPITEMSELECTION=&FLAGSTOPITEMSELECTION;

    ApplicationConfig=TheCurrentApplicationConfig;
    ApplicationConfig->ParentWindow=this;
    ui->Toolbox->setCurrentIndex(0);
    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    QSplashScreen screen;
    screen.setPixmap(QPixmap("img/splash.png"));
    screen.show();
    QApplication::processEvents();  // Give time to interface !

    AddToSystemProperties(QString(VERSIONQT_STR)+QString(qVersion()));
    AddToSystemProperties(QString(FMTFILTERVERSION_STR)+"0.6.4-Licence=LGPL");

    // Now, we have application settings then we can init SDL
    screen.showMessage(QApplication::translate("MainWindow","Starting SDL..."),Qt::AlignHCenter|Qt::AlignBottom);
    SDLFirstInit(ApplicationConfig->PreviewFPS);
    AddToSystemProperties(QString(SDLVERSION_STR)+QString("%1").arg(SDL_MAJOR_VERSION)+"."+QString("%1").arg(SDL_MINOR_VERSION)+"."+QString("%1").arg(SDL_PATCHLEVEL)+"-Licence=GPL version 2.1 or later");

    // Register all formats and codecs for libavformat/libavcodec/etc ...
    screen.showMessage(QApplication::translate("MainWindow","Starting ffmpeg..."),Qt::AlignHCenter|Qt::AlignBottom);
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
                while ((Index<NBR_VIDEOCODECDEF)&&(Codec!=QString(VIDEOCODECDEF[Index].FFD_VCODECST))) Index++;
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
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered video codecs for encoding :"));
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) if (VIDEOCODECDEF[i].IsFind) AddToSystemProperties("  "+QString(VIDEOCODECDEF[i].LongName)+"-ffmpeg codec:"+QString(VIDEOCODECDEF[i].ShortName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered audio codecs for encoding :"));
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) if (AUDIOCODECDEF[i].IsFind) AddToSystemProperties("  "+QString(AUDIOCODECDEF[i].LongName)+"-ffmpeg codec:"+QString(AUDIOCODECDEF[i].ShortName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered container formats for encoding :"));
    for (int i=0;i<NBR_FORMATDEF;i++)     if (FORMATDEF[i].IsFind) AddToSystemProperties("  "+QString(FORMATDEF[i].LongName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QString("%1").arg(ApplicationConfig->RenderDeviceModel.count())+QApplication::translate("MainWindow"," Device registered for rendering"));

    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Library :"));
    QString Path;
    screen.showMessage(QApplication::translate("MainWindow","Loading background library..."),Qt::AlignHCenter|Qt::AlignBottom);
    Path="background";      BackgroundList.ScanDisk(Path,GEOMETRY_16_9); AddToSystemProperties(QString("  %1").arg(BackgroundList.List.count())+QApplication::translate("MainWindow"," images loaded into the background-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    screen.showMessage(QApplication::translate("MainWindow","Loading no-luma transitions..."),Qt::AlignHCenter|Qt::AlignBottom);
    for (int i=0;i<TRANSITIONMAXSUBTYPE_BASE;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_BASE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_ZOOMINOUT;i++)  IconList.List.append(cIconObject(TRANSITIONFAMILLY_ZOOMINOUT,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_SLIDE;i++)      IconList.List.append(cIconObject(TRANSITIONFAMILLY_SLIDE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_PUSH;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_PUSH,i));
    AddToSystemProperties(QString("  %1").arg(IconList.List.count())+QApplication::translate("MainWindow"," no-luma transitions loaded into the transition-library"));
    screen.showMessage(QApplication::translate("MainWindow","Loading luma transitions..."),Qt::AlignHCenter|Qt::AlignBottom);
    Path="luma/Bar";        LumaList_Bar.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BAR);         AddToSystemProperties(QString("  %1").arg(LumaList_Bar.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Box";        LumaList_Box.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BOX);         AddToSystemProperties(QString("  %1").arg(LumaList_Box.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Center";     LumaList_Center.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CENTER);   AddToSystemProperties(QString("  %1").arg(LumaList_Center.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Checker";    LumaList_Checker.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CHECKER); AddToSystemProperties(QString("  %1").arg(LumaList_Checker.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Clock";      LumaList_Clock.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CLOCK);     AddToSystemProperties(QString("  %1").arg(LumaList_Clock.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    Path="luma/Snake";      LumaList_Snake.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_SNAKE);     AddToSystemProperties(QString("  %1").arg(LumaList_Snake.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+AdjustDirForOS(QDir(Path).absolutePath()));
    AddToSystemProperties(QApplication::translate("MainWindow","  Total:")+QString("%1").arg(IconList.List.count())+QApplication::translate("MainWindow"," transitions loaded into the transition-library"));

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

    connect(ui->Toolbox,SIGNAL(currentChanged(int)),this,SLOT(s_ToolbarChanged(int)));
    connect(ui->webView,SIGNAL(linkClicked(QUrl)),this,SLOT(s_WebViewOpen(QUrl)));

    // Help menu
    connect(ui->Action_About_BT,SIGNAL(pressed()),this,SLOT(s_About()));
    connect(ui->ActionDocumentation_BT,SIGNAL(pressed()),this,SLOT(s_Documentation()));
    connect(ui->ActionNewFunctions_BT,SIGNAL(pressed()),this,SLOT(s_NewFunctions()));

    // File menu
    connect(ui->Action_New_BT,SIGNAL(pressed()),this,SLOT(s_action_New()));
    connect(ui->Action_Open_BT,SIGNAL(pressed()),this,SLOT(s_action_Open()));
    connect(ui->Action_OpenRecent_BT,SIGNAL(pressed()),this,SLOT(s_action_OpenRecent()));
    connect(ui->Action_Save_BT,SIGNAL(pressed()),this,SLOT(s_action_Save()));
    connect(ui->ActionSave_as_BT,SIGNAL(pressed()),this,SLOT(s_action_SaveAs()));
    connect(ui->ActionConfiguration_BT,SIGNAL(pressed()),this,SLOT(s_ChangeApplicationSettings()));
    connect(ui->Action_Exit_BT,SIGNAL(pressed()),this,SLOT(s_action_Exit()));

    // Project menu
    connect(ui->ActionAdd_BT,SIGNAL(pressed()),this,SLOT(s_action_AddFile()));
    connect(ui->ActionAddtitle_BT,SIGNAL(pressed()),this,SLOT(s_action_AddTitle()));
    connect(ui->ActionAddProject_BT,SIGNAL(pressed()),this,SLOT(s_action_AddProject()));
    connect(ui->ActionRemove_BT,SIGNAL(pressed()),this,SLOT(s_RemoveObject()));
    connect(ui->ActionMove_left_BT,SIGNAL(pressed()),this,SLOT(s_LeftObject()));
    connect(ui->ActionMove_right_BT,SIGNAL(pressed()),this,SLOT(s_RightObject()));
    connect(ui->ActionCut_BT,SIGNAL(pressed()),this,SLOT(s_CutToClipboard()));
    connect(ui->ActionCopy_BT,SIGNAL(pressed()),this,SLOT(s_CopyToClipboard()));
    connect(ui->ActionPaste_BT,SIGNAL(pressed()),this,SLOT(s_PasteFromClipboard()));
    connect(ui->ActionEdit_BT,SIGNAL(pressed()),this,SLOT(s_action_Edit()));

    connect(ui->actionEdit_background,SIGNAL(triggered()),this,SLOT(s_BackgroundDoubleClicked()));
    connect(ui->actionEdit_background_transition,SIGNAL(triggered()),this,SLOT(s_TransitionBackgroundDoubleClicked()));
    connect(ui->actionEdit_object,SIGNAL(triggered()),this,SLOT(s_ItemDoubleClicked()));
    connect(ui->actionEdit_object_in_transition,SIGNAL(triggered()),this,SLOT(s_TransitionItemDoubleClicked()));
    connect(ui->actionEdit_music,SIGNAL(triggered()),this,SLOT(s_MusicDoubleClicked()));

    // Render menu
    connect(ui->ActionRender_BT,SIGNAL(pressed()),this,SLOT(s_RenderVideo()));
    connect(ui->ActionSmartphone_BT,SIGNAL(pressed()),this,SLOT(s_RenderSmartphone()));
    connect(ui->ActionMultimedia_BT,SIGNAL(pressed()),this,SLOT(s_RenderMultimedia()));
    connect(ui->ActionForTheWEB_BT,SIGNAL(pressed()),this,SLOT(s_RenderForTheWEB()));

    // Timeline
    connect(ui->ZoomPlusBT,SIGNAL(pressed()),this,SLOT(s_action_ZoomPlus()));
    connect(ui->ZoomMinusBT,SIGNAL(pressed()),this,SLOT(s_action_ZoomMinus()));
    connect(ui->timeline,SIGNAL(itemSelectionChanged()),this,SLOT(s_ItemSelectionChanged()));

    // Timer
    LastCount=0;
    connect(&Timer,SIGNAL(timeout()),this,SLOT(s_TimerEvent()));

    ui->StatusBar_SlideNumber->setText(QApplication::translate("MainWindow","Slide : ")+"0 / 0");
    s_ToolbarChanged(0);
}

//====================================================================================================================

MainWindow::~MainWindow() {
    Timer.stop();
    delete ui;
    delete Diaporama;
    delete ApplicationConfig;
    SDLLastClose();
    if (Clipboard_Block) {
        delete Clipboard_Block;
        Clipboard_Block=NULL;
    }
    if (Clipboard_Object) {
        delete Clipboard_Object;
        Clipboard_Object=NULL;
    }
}

//====================================================================================================================

void MainWindow::s_TimerEvent() {
    if (StatusBarList.count()>0) {
        while (StatusBarList.count()>1) StatusBarList.takeFirst();
        ui->StatusBar->setText(StatusBarList.takeFirst());
        LastCount=0;
    } else {
        LastCount++;
        if (LastCount==10) ui->StatusBar->setText(InternetBUILDVERSION);
    }
}

//====================================================================================================================

void MainWindow::SetTimelineHeight() {
    ui->timeline->setUpdatesEnabled(false);
    ui->timeline->setFixedHeight(
            15+                                     // Horizontal slider and marges
            ApplicationConfig->TimelineHeight/2+    // Background
            ApplicationConfig->TimelineHeight+      // Montage
            TIMELINESOUNDHEIGHT*2+4                 // Music
    );
    ui->timeline->setRowHeight(TRACKBACKGROUND,ApplicationConfig->TimelineHeight/2);    // Background
    ui->timeline->setRowHeight(TRACKMONTAGE,ApplicationConfig->TimelineHeight);         // Montage
    ui->timeline->setRowHeight(TRACKMUSIC,TIMELINESOUNDHEIGHT*2);                       // Music
    for (int i=0;i<ui->timeline->columnCount();i++) {
        ui->timeline->setColumnWidth(i,Diaporama->GetWidthForHeight(ui->timeline->rowHeight(TRACKMONTAGE)-5)+32+ADJUSTXCOLUMN);
        if (Diaporama->List[i].Thumbnail) {
            delete Diaporama->List[i].Thumbnail;
            Diaporama->List[i].Thumbnail=NULL;
        }
    }
    // Give time to Qt to redefine position of each control
    QApplication::processEvents();
    // Reset timeline painting
    ui->timeline->setUpdatesEnabled(true);
    // Adjust preview
    ui->preview->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview->height()-32));
}

//====================================================================================================================

void MainWindow::closeEvent(QCloseEvent *Event) {
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (Diaporama->IsModify) {
        int Bt=QMessageBox::question(this,QApplication::translate("MainWindow","Close application"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
        if (Bt==QMessageBox::Yes) s_action_Save();
        if (Bt==QMessageBox::Cancel) {
            Event->setAccepted(false);
            return;
        }
    }
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
        Timer.start(100);
        ui->preview->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview->height()-32));
        // Start a network process to give last ffdiaporama version from internet web site
        QNetworkAccessManager *mNetworkManager=new QNetworkAccessManager(this);
        connect(mNetworkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(onNetworkReply(QNetworkReply*)));
        QUrl            url(BUILDVERSION_WEBURL);
        QNetworkReply   *reply  = mNetworkManager->get(QNetworkRequest(url));
        reply->deleteLater();
    }
}

//====================================================================================================================
// Function use when reading BUILDVERSION from WEB Site
//====================================================================================================================

void MainWindow::onNetworkReply(QNetworkReply* reply) {
    if (reply->error()==QNetworkReply::NoError) {
        int httpstatuscode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
        if ((httpstatuscode>=200)&&(httpstatuscode<300)&&(reply->isReadable())) {
            QString Line;
            InternetBUILDVERSION=QString::fromUtf8(reply->readAll().data());
            if (InternetBUILDVERSION.endsWith("\n"))   InternetBUILDVERSION=InternetBUILDVERSION.left(InternetBUILDVERSION.length()-QString("\n").length());
            while (InternetBUILDVERSION.endsWith(" ")) InternetBUILDVERSION=InternetBUILDVERSION.left(InternetBUILDVERSION.length()-1);
            if (InternetBUILDVERSION.lastIndexOf(" ")) InternetBUILDVERSION=InternetBUILDVERSION.mid(InternetBUILDVERSION.lastIndexOf(" ")+1);

            QFile file("BUILDVERSION.txt");
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                Line=QString(file.readLine());
                if (Line.endsWith("\n")) Line=Line.left(Line.length()-QString("\n").length());
                while (Line.endsWith(" ")) Line=Line.left(Line.length()-1);
                if (Line.lastIndexOf(" ")) Line=Line.mid(Line.lastIndexOf(" ")+1);
            }
            int CurrentVersion =Line.toInt();
            int InternetVersion=InternetBUILDVERSION.toInt();
            if (InternetVersion>CurrentVersion) InternetBUILDVERSION=QApplication::translate("MainWindow","A new ffDiaporama release if available from WEB site. Please update from http://ffdiaporama.tuxfamily.org !");
                else InternetBUILDVERSION="";
        } else InternetBUILDVERSION="";
    } else InternetBUILDVERSION="";
    ui->StatusBar->setText(InternetBUILDVERSION);
}

//====================================================================================================================

void MainWindow::OpenHelp(QString HelpFile) {
    if (HelpFile.startsWith(("file://"))) {
        QDesktopServices::openUrl(QUrl(HelpFile));
    } else if (HelpFile.startsWith(("http://"))) {
        QString HelpPath=HelpFile.replace("<local>",CurrentLanguage);
        QDesktopServices::openUrl(QUrl(HelpPath));
    } else {
        QString HelpPath="WIKI/"+CurrentLanguage+"/"+HelpFile+".html";
        QDesktopServices::openUrl(QUrl(HelpPath));
    }
}

//====================================================================================================================

void MainWindow::RefreshControls() {
    // Timeline actions
    ui->ActionRemove_BT->setEnabled(ui->timeline->columnCount()>0);
    ui->ActionEdit_BT->setEnabled(ui->timeline->columnCount()>0);
    ui->ActionMove_left_BT->setEnabled((ui->timeline->currentColumn()>0)&&(ui->timeline->columnCount()>1));
    ui->ActionMove_right_BT->setEnabled(ui->timeline->currentColumn()<ui->timeline->columnCount()-1);

    // File menu
    ui->Action_Save_BT->setEnabled(Diaporama->IsModify);
    ui->ActionSave_as_BT->setEnabled(Diaporama->List.count()>0);
    ui->Action_OpenRecent_BT->setEnabled(ApplicationConfig->RecentFile.count()>0);

    // Project menu
    ui->actionEdit_background->setEnabled(ui->timeline->columnCount()>0);
    ui->actionEdit_background_transition->setEnabled(ui->timeline->columnCount()>0);
    ui->actionEdit_object->setEnabled(ui->timeline->columnCount()>0);
    ui->actionEdit_object_in_transition->setEnabled(ui->timeline->columnCount()>0);
    ui->actionEdit_music->setEnabled(ui->timeline->columnCount()>0);

    // Clipboard_Object
    ui->ActionPaste_BT->setEnabled(Clipboard_Object!=NULL);
    ui->ActionCopy_BT->setEnabled(ui->timeline->currentColumn()>=0);
    ui->ActionCut_BT->setEnabled(ui->timeline->currentColumn()>=0);

    ui->ActionRender_BT->setEnabled(ui->timeline->columnCount()>0);
    ui->ActionSmartphone_BT->setEnabled(ui->timeline->columnCount()>0);
    ui->ActionMultimedia_BT->setEnabled(ui->timeline->columnCount()>0);
    ui->ActionForTheWEB_BT->setEnabled(ui->timeline->columnCount()>0);

    ui->StatusBar_SlideNumber->setText(QApplication::translate("MainWindow","Slide : ")+QString("%1").arg(Diaporama->CurrentCol+(Diaporama->List.count()>0?1:0))+" / "+QString("%1").arg(Diaporama->List.count()));
}

//====================================================================================================================

void MainWindow::SetModifyFlag(bool IsModify) {
    Diaporama->IsModify=IsModify;
    this->setWindowTitle(QString(APPLICATION_NAME)+QString(" ")+QString(APPLICATION_VERSION)+QString("-")+
                         (Diaporama->ProjectFileName!=""?Diaporama->ProjectFileName:QApplication::translate("MainWindow","<new project>","when project have no name define"))+
                         (Diaporama->IsModify?"*":""));
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_About() {
    ui->Action_About_BT->setDown(false);
    DlgAbout(this).exec();
}

//====================================================================================================================

void MainWindow::s_Documentation() {
    ui->ActionDocumentation_BT->setDown(false);
    OpenHelp(HELPFILE_SUPPORT);
}

//====================================================================================================================

void MainWindow::s_NewFunctions() {
    ui->ActionNewFunctions_BT->setDown(false);
    OpenHelp(HELPFILE_NEWS);
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
    if (DlgSlideProperties(&(Diaporama->List[Diaporama->CurrentCol]),this).exec()==0) {
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
    QMessageBox::critical(this,QApplication::translate("MainWindow","Not implemented"),QApplication::translate("MainWindow","Sorry, not yet done !"));
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
        Diaporama->FreeUnusedMemory(Diaporama->CurrentCol);
    }
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Update dock informations
//====================================================================================================================

void MainWindow::s_WebViewOpen(QUrl Url) {
    OpenHelp(Url.toString());
}

void MainWindow::s_ToolbarChanged(int MenuIndex) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->webView->setUrl(QUrl("WIKI/"+CurrentLanguage+"/tab_00"+QString("%1").arg(MenuIndex+1)+".html"));
    ui->webView->history()->clear();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Render project
//====================================================================================================================

void MainWindow::s_RenderVideo() {
    ui->ActionRender_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    DlgRenderVideo(*Diaporama,EXPORTMODE_ADVANCED,this).exec();
    AdjustRuller();
}

void MainWindow::s_RenderSmartphone() {
    ui->ActionSmartphone_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    DlgRenderVideo(*Diaporama,EXPORTMODE_SMARTPHONE,this).exec();
    AdjustRuller();
}

void MainWindow::s_RenderMultimedia() {
    ui->ActionMultimedia_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    DlgRenderVideo(*Diaporama,EXPORTMODE_MULTIMEDIASYS,this).exec();
    AdjustRuller();
}

void MainWindow::s_RenderForTheWEB() {
    ui->ActionForTheWEB_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    DlgRenderVideo(*Diaporama,EXPORTMODE_FORTHEWEB,this).exec();
    AdjustRuller();
}

//====================================================================================================================
// Change application settings
//====================================================================================================================

void MainWindow::s_ChangeApplicationSettings() {
    ui->ActionConfiguration_BT->setDown(false);
    ui->preview->SetPlayerToPause();                            // Ensure player is stop
    DlgApplicationSettings(*ApplicationConfig,this).exec();
    SDLSetFPS(ApplicationConfig->PreviewFPS);                   // Reinit SDL if Preview FPS has changed
}

//====================================================================================================================
// New project
//====================================================================================================================

void MainWindow::s_action_New() {
    ui->Action_New_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QApplication::translate("MainWindow","New project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_action_Save();

    cDiaporama *NewDiaporama=new cDiaporama(ApplicationConfig);

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
}

//====================================================================================================================
// Open an existing project
//====================================================================================================================

void MainWindow::s_action_OpenRecent() {
    QMenu *ContextMenu=new QMenu(this);
    for (int i=ApplicationConfig->RecentFile.count()-1;i>=0;i--) ContextMenu->addAction(ApplicationConfig->RecentFile.at(i));
    QAction *Action=ContextMenu->exec(QCursor::pos());
    QString Selected="";
    if (Action) Selected=Action->iconText();
    delete ContextMenu;
    ui->Action_OpenRecent_BT->setDown(false);
    if (Selected!="") OpenFile(Selected);
}

void MainWindow::s_action_Open() {
    ui->Action_Open_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QApplication::translate("MainWindow","Open project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_action_Save();

    QApplication::processEvents();
    QString ProjectFileName=QFileDialog::getOpenFileName(this,QApplication::translate("MainWindow","Open project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (ProjectFileName!="") OpenFile(ProjectFileName);
}

void MainWindow::OpenFile(QString ProjectFileName) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Manage Recent files list
    for (int i=0;i<ApplicationConfig->RecentFile.count();i++) if (ApplicationConfig->RecentFile.at(i)==ProjectFileName) {
        ApplicationConfig->RecentFile.removeAt(i);
        break;
    }

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
    SetModifyFlag(false);
    Diaporama->LoadFile(this,ProjectFileName);
    ui->timeline->setCurrentCell(0,0);
    for (int i=0;i<Diaporama->List.count();i++) AddObjectToTimeLine(i);
    AdjustRuller();
    SetModifyFlag(Diaporama->IsModify);
    QApplication::restoreOverrideCursor();
    if (Diaporama->List.count()>0) ui->preview->SeekPlayer(Diaporama->List[0].TransitionDuration); else ui->preview->SeekPlayer(0);

    ApplicationConfig->RecentFile.append(ProjectFileName);
    while (ApplicationConfig->RecentFile.count()>10) ApplicationConfig->RecentFile.takeFirst();
}

//====================================================================================================================
// Save current project
//====================================================================================================================

void MainWindow::s_action_Save() {
    ui->Action_Save_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    if (Diaporama->ProjectFileName=="") s_action_SaveAs(); else {
        if (Diaporama->SaveFile(this)) SetModifyFlag(false);
    }
}

//====================================================================================================================
// Save current project as
//====================================================================================================================

void MainWindow::s_action_SaveAs() {
    ui->ActionSave_as_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    Diaporama->ProjectFileName=QFileDialog::getSaveFileName(this,QApplication::translate("MainWindow","Save project as"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (Diaporama->ProjectFileName!="") {
        if (QFileInfo(Diaporama->ProjectFileName).suffix()!="ffd") Diaporama->ProjectFileName=Diaporama->ProjectFileName+".ffd";
        ApplicationConfig->LastProjectPath=QFileInfo(Diaporama->ProjectFileName).dir().absolutePath();
        // Manage Recent files list
        for (int i=0;i<ApplicationConfig->RecentFile.count();i++) if (ApplicationConfig->RecentFile.at(i)==Diaporama->ProjectFileName) {
            ApplicationConfig->RecentFile.removeAt(i);
            break;
        }
        ApplicationConfig->RecentFile.append(Diaporama->ProjectFileName);
        while (ApplicationConfig->RecentFile.count()>10) ApplicationConfig->RecentFile.takeFirst();
        if (Diaporama->SaveFile(this)) SetModifyFlag(false);
    }
}

//====================================================================================================================
// Add a title object
//====================================================================================================================

void MainWindow::s_action_AddTitle() {
    ui->ActionAddtitle_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int SavedCurIndex=ApplicationConfig->AppendObject?Diaporama->List.count():Diaporama->CurrentCol;
    int CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
    if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;

    Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
    cDiaporamaObject *DiaporamaObject=&Diaporama->List[CurIndex];
    DiaporamaObject->List[0].Parent        =DiaporamaObject;
    DiaporamaObject->List[0].StaticDuration=GlobalMainWindow->ApplicationConfig->NoShotDuration;
    DiaporamaObject->Parent                =Diaporama;
    DiaporamaObject->TypeObject            =DIAPORAMAOBJECTTYPE_EMPTY;

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
    ui->ActionAdd_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop

    QStringList FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("MainWindow","Add files"),
                                                       ApplicationConfig->RememberLastDirectories?ApplicationConfig->LastMediaPath:"",
                                                       ApplicationConfig->GetFilterForMediaFile(cApplicationConfig::ALLFILE));
    QApplication::processEvents();
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

    // Sort files in the fileList
    if (Diaporama->ApplicationConfig->SortFile) {
        // Sort by last number
        for (int i=0;i<FileList.count();i++) for (int j=0;j<FileList.count()-1;j++) {
            QString NameA=QFileInfo(FileList[j]).completeBaseName();
            int NumA=NameA.length()-1;
            while ((NumA>0)&&(NameA[NumA]>='0')&&(NameA[NumA]<='9')) NumA--;
            if (NumA>=0) NumA=NameA.mid(NumA+1).toInt();

            QString NameB=QFileInfo(FileList[j+1]).completeBaseName();
            int NumB=NameB.length()-1;
            while ((NumB>0)&&(NameB[NumB]>='0')&&(NameB[NumB]<='9')) NumB--;
            if (NumB>=0) NumB=NameB.mid(NumB+1).toInt();

            if (NumA>NumB) FileList.swap(j,j+1);
        }
    } else {
        // Sort by alphabetical order
        for (int i=0;i<FileList.count();i++) for (int j=0;j<FileList.count()-1;j++) {
            if (QFileInfo(FileList[j]).completeBaseName()>QFileInfo(FileList[j+1]).completeBaseName()) FileList.swap(j,j+1);
        }
    }

    // Add files
    for (int i=0;i<FileList.count();i++) {
        QString NewFile=FileList[i];
        if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

        Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
        cDiaporamaObject *DiaporamaObject=&Diaporama->List[CurIndex];
        DiaporamaObject->List[0].Parent        =DiaporamaObject;
        DiaporamaObject->List[0].StaticDuration=GlobalMainWindow->ApplicationConfig->NoShotDuration;
        DiaporamaObject->Parent                =Diaporama;
        DiaporamaObject->TypeObject            =DIAPORAMAOBJECTTYPE_EMPTY;

        // Create and append a composition block to the object list
        DiaporamaObject->ObjectComposition.List.append(cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey));
        cCompositionObject *CompositionObject=&DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
        cBrushDefinition   *CurrentBrush=&CompositionObject->BackgroundBrush;

        // Set CompositionObject to full screen
        CompositionObject->x=0;
        CompositionObject->y=0;
        CompositionObject->w=1;
        CompositionObject->h=1;

        // Set other values
        CompositionObject->Text     ="";
        CompositionObject->PenSize  =0;
        CurrentBrush->BrushType     =BRUSHTYPE_IMAGEDISK;
        DiaporamaObject->SlideName  =QFileInfo(NewFile).fileName();

        QString BrushFileName =QFileInfo(NewFile).absoluteFilePath();

        bool    IsValide =false;
        QString Extension=QFileInfo(BrushFileName).suffix().toLower();

        // Search if file is an image
        for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowImageExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowImageExtension[i]==Extension) {
            // Create an image wrapper
            CurrentBrush->Image=new cimagefilewrapper();
            IsValide=CurrentBrush->Image->GetInformationFromFile(BrushFileName);
            if (!IsValide) {
                delete CurrentBrush->Image;
                CurrentBrush->Image=NULL;
            }
            break;
        }
        // If it's not an image : search if file is a video
        if (CurrentBrush->Image==NULL) for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowVideoExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowVideoExtension[i]==Extension) {
            // Create a video wrapper
            CurrentBrush->Video=new cvideofilewrapper();
            IsValide=CurrentBrush->Video->GetInformationFromFile(BrushFileName,false);
            if (!IsValide) {
                delete CurrentBrush->Video;
                CurrentBrush->Video=NULL;
            } else {
                CurrentBrush->Video->EndPos=CurrentBrush->Video->Duration;
                DiaporamaObject->List[0].StaticDuration=1000;
            }
            break;
        }
        if (IsValide) {
            QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,true,&CurrentBrush->Image->BrushFileTransform):
                           CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,0,true,NULL,1,false,&CurrentBrush->Video->BrushFileTransform):
                           NULL);
            if (Image) {
                CurrentBrush->BrushFileCorrect.ImageGeometry=GEOMETRY_PROJECT;
                CurrentBrush->BrushFileCorrect.AspectRatio  =double(Diaporama->InternalHeight)/double(Diaporama->InternalWidth);

                double  RealImageW=Image->width();
                double  RealImageH=Image->height();
                double  Hyp=sqrt(RealImageW*RealImageW+RealImageH*RealImageH);     // Calc hypothenuse
                double  VirtImageW=Hyp;                                            // Calc canvas size
                double  VirtImageH=Hyp;//Diaporama->GetHeightForWidth(VirtImageW);       // Calc canvas size
                double  MagnetX1=(VirtImageW-RealImageW)/2;
                double  MagnetX2=MagnetX1+RealImageW;
                double  MagnetY1=(VirtImageH-RealImageH)/2;
                double  MagnetY2=MagnetY1+RealImageH;
                double  W=0;
                double  H=0;
                switch (ApplicationConfig->DefaultFraming) {
                    case 0 :    // Adjust to Width
                        W=MagnetX2-MagnetX1;
                        H=W*CurrentBrush->BrushFileCorrect.AspectRatio;
                        break;
                    case 1 :    // Adjust to Height
                        H=MagnetY2-MagnetY1;
                        W=H/CurrentBrush->BrushFileCorrect.AspectRatio;
                        break;
                    case 2 :    // Adjust to Full
                        W=MagnetX2-MagnetX1;
                        H=W*CurrentBrush->BrushFileCorrect.AspectRatio;
                        if (H<MagnetY2-MagnetY1) {
                            H=MagnetY2-MagnetY1;
                            W=H/CurrentBrush->BrushFileCorrect.AspectRatio;
                        }
                        break;
                }
                CurrentBrush->BrushFileCorrect.X=((VirtImageW-W)/2)/VirtImageW;
                CurrentBrush->BrushFileCorrect.Y=((VirtImageH-H)/2)/VirtImageH;
                CurrentBrush->BrushFileCorrect.ZoomFactor=W/VirtImageW;
                delete Image;
            }

            // Now create and append a shot composition block to all shot
            for (int i=0;i<DiaporamaObject->List.count();i++) {
                DiaporamaObject->List[i].ShotComposition.List.append(cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey));
                DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].CopyFromCompositionObject(CompositionObject);
            }

            // Inc NextIndexKey
            DiaporamaObject->NextIndexKey++;

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
            QMessageBox::critical(NULL,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+QApplication::translate("MainWindow","Format not supported","Error message"),QMessageBox::Close);
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
    ui->ActionAddProject_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    QApplication::processEvents();
    QString ProjectFileName=QFileDialog::getOpenFileName(this,QApplication::translate("MainWindow","Add a sub project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
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
    QApplication::processEvents();  // Ensure message queue is empty !

    wgt_QCustomThumbnails *ObjectBackground=new wgt_QCustomThumbnails(ui->timeline,THUMBNAILTYPE_OBJECTBACKGROUND);
    connect(ObjectBackground,SIGNAL(EditMediaObject()),this,SLOT(s_BackgroundDoubleClicked()));
    connect(ObjectBackground,SIGNAL(EditTransition()),this,SLOT(s_TransitionBackgroundDoubleClicked()));

    wgt_QCustomThumbnails *ObjectObject    =new wgt_QCustomThumbnails(ui->timeline,THUMBNAILTYPE_OBJECTSEQUENCE);
    connect(ObjectObject,SIGNAL(EditMediaObject()),this,SLOT(s_ItemDoubleClicked()));
    connect(ObjectObject,SIGNAL(EditTransition()),this,SLOT(s_TransitionItemDoubleClicked()));
    connect(ObjectObject,SIGNAL(EditSoundTrack()),this,SLOT(s_SoundItemDoubleClicked()));

    wgt_QCustomThumbnails *ObjectMusic     =new wgt_QCustomThumbnails(ui->timeline,THUMBNAILTYPE_OBJECTMUSIC);
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
    QApplication::processEvents();
}

//====================================================================================================================

void MainWindow::s_action_Edit() {
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionEdit_background);
    ContextMenu->addAction(ui->actionEdit_object);
    ContextMenu->addAction(ui->actionEdit_music);
    ContextMenu->addAction(ui->actionEdit_object_in_transition);
    ContextMenu->addAction(ui->actionEdit_background_transition);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->ActionEdit_BT->setDown(false);
}

//====================================================================================================================

void MainWindow::s_RemoveObject() {
    ui->ActionRemove_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    int Current=ui->timeline->currentColumn();
    if ((Current<0)||(Current>=Diaporama->List.count())) return;

    if (QMessageBox::question(this,QApplication::translate("MainWindow","Remove slide"),QApplication::translate("MainWindow","Are you sure to want to delete this slide?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No) return;

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
    ui->ActionMove_left_BT->setDown(false);
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
    ui->ActionMove_right_BT->setDown(false);
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
    ui->ActionCut_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    int Current=ui->timeline->currentColumn();
    if ((Current<0)||(Current>Diaporama->List.count()-1)) return;

    QDomElement root;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (Clipboard_Object!=NULL) delete Clipboard_Object;
    Clipboard_Object=new QDomDocument(APPLICATION_NAME);

    // Create xml document and root
    root=Clipboard_Object->createElement("CLIPBOARD");
    Clipboard_Object->appendChild(root);
    Diaporama->List[Current].SaveToXML(root,"CLIPBOARD-OBJECT",QFileInfo(Diaporama->ProjectFileName).absolutePath(),true);
    s_RemoveObject();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_CopyToClipboard() {
    ui->ActionCopy_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    int Current=ui->timeline->currentColumn();
    if ((Current<0)||(Current>Diaporama->List.count()-1)) return;

    QDomElement root;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (Clipboard_Object!=NULL) delete Clipboard_Object;
    Clipboard_Object=new QDomDocument(APPLICATION_NAME);

    // Create xml document and root
    root=Clipboard_Object->createElement("CLIPBOARD");
    Clipboard_Object->appendChild(root);
    Diaporama->List[Current].SaveToXML(root,"CLIPBOARD-OBJECT",QFileInfo(Diaporama->ProjectFileName).absolutePath(),true);

    RefreshControls();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_PasteFromClipboard() {
    ui->ActionPaste_BT->setDown(false);
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    // Calc position of new object depending on ApplicationConfig->AppendObject
    int SavedCurIndex=ApplicationConfig->AppendObject?Diaporama->List.count():Diaporama->CurrentCol;
    int CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
    if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;

    Diaporama->List.insert(CurIndex,cDiaporamaObject(Diaporama));
    //Diaporama->List[CurIndex].List[0].Parent=&Diaporama->List[CurIndex];

    QDomElement root=Clipboard_Object->documentElement();
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
    RefreshControls();
}
