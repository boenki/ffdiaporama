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

#include "_StyleDefinitions.h"
#include "_SoundDefinitions.h"
#include "_ImagesDefinitions.h"
#include "_ApplicationDefinitions.h"

#include "../sharedfiles/DlgCheckConfig.h"
#include "../sharedfiles/DlgffDPjrProperties.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "wgt_QCustomThumbnails.h"
#include "cCustomTableWidget.h"

#include <QClipboard>
#include <QMimeData>
#include <QDomElement>
#include <QDomDocument>
#include <QSplashScreen>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>

#include "DlgAbout.h"
#include "DlgBackgroundProperties.h"
#include "DlgMusicProperties.h"
#include "DlgSlideProperties.h"
#include "DlgTransitionProperties.h"
#include "DlgApplicationSettings.h"
#include "DlgRenderVideo.h"

//#define DEBUGMODE

MainWindow  *GlobalMainWindow=NULL;

//====================================================================================================================

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::MainWindow";
    #endif

    ApplicationConfig       =new cApplicationConfig(this);
    CurrentThreadId         =this->thread()->currentThreadId();
    InternetBUILDVERSION    ="";
    GlobalMainWindow        =this;
    IsFirstInitDone         =false;                 // true when first show window was done
    FLAGSTOPITEMSELECTION   =false;        // Flag to stop Item Selection process for delete and move of object
    DragItemSource          =-1;
    DragItemDest            =-1;
    IsDragOn                =0;
    InPlayerUpdate          =false;
    CurrentRenderingDialog  =NULL;
    setAcceptDrops(true);
    ApplicationConfig->ParentWindow=this;
}

//====================================================================================================================

void MainWindow::InitWindow(QString ForceLanguage,QApplication *App) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::InitWindow";
    #endif

    AddToSystemProperties(QString(STARTINGPATH_STR)+AdjustDirForOS(QDir::currentPath()));
    ApplicationConfig->InitConfigurationValues(ForceLanguage,App);

    QSplashScreen screen;
    screen.setPixmap(QPixmap(":/img/splash.png"));
    screen.show();

    ui->setupUi(this);
    ui->preview->FLAGSTOPITEMSELECTION=&FLAGSTOPITEMSELECTION;
    ui->preview2->FLAGSTOPITEMSELECTION=&FLAGSTOPITEMSELECTION;
    ui->ToolBoxNormal->setCurrentIndex(0);

    AddToSystemProperties(QString(WORKINGPATH_STR)+AdjustDirForOS(QDir::currentPath()));
    AddToSystemProperties(QString(SYSTEMLOCAL_STR)+ApplicationConfig->CurrentLanguage);
    AddToSystemProperties(QString(LOADEDLOCAL_STR)+AdjustDirForOS(QDir().absoluteFilePath(QString("locale")+QDir::separator()+QString("locale_")+ApplicationConfig->CurrentLanguage+".qm")));
    AddSeparatorToSystemProperties();
    Transparent.setTextureImage(QImage(":/img/transparent.png"));  // Load transparent brush
    AddToSystemProperties(QString(VERSIONQT_STR)+QString(qVersion()));
    AddToSystemProperties(QString(FMTFILTERVERSION_STR)+"0.6.4-Licence=LGPL");

    // Now, we have application settings then we can init all others
    screen.showMessage(QApplication::translate("MainWindow","Loading system icons..."),Qt::AlignHCenter|Qt::AlignBottom);
    ApplicationConfig->PreloadSystemIcons();

    // Now, we have application settings then we can init SDL
    screen.showMessage(QApplication::translate("MainWindow","Starting SDL..."),Qt::AlignHCenter|Qt::AlignBottom);
    SDLFirstInit(ApplicationConfig->PreviewFPS,ApplicationConfig->SDLAudioOldMode);
    AddToSystemProperties(QString(SDLVERSION_STR)+QString("%1").arg(SDL_MAJOR_VERSION)+"."+QString("%1").arg(SDL_MINOR_VERSION)+"."+QString("%1").arg(SDL_PATCHLEVEL)+"-Licence=GPL version 2.1 or later");

    // Register all formats and codecs for libavformat/libavcodec/etc ...
    screen.showMessage(QApplication::translate("MainWindow","Starting ffmpeg..."),Qt::AlignHCenter|Qt::AlignBottom);
    ApplicationConfig->DeviceModelList.Initffmpeg();

    // Display ffmpeg versions
    AddToSystemProperties(QString(LIBAVCODECVERSION_STR)+QString("%1").arg(LIBAVCODEC_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MINOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MICRO)+"."+QString("%1").arg(avcodec_version())+"-Licence="+QString(avcodec_license()));
    AddToSystemProperties(QString(LIBAVFORMATVERSION_STR)+QString("%1").arg(LIBAVFORMAT_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MINOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MICRO)+"."+QString("%1").arg(avformat_version())+"-Licence="+QString(avformat_license()));
    AddToSystemProperties(QString(LIBSWSCALEVERSION_STR)+QString("%1").arg(LIBSWSCALE_VERSION_MAJOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MINOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MICRO)+"."+QString("%1").arg(swscale_version())+"-Licence="+QString(swscale_license()));

    // Display finding codecs & formats
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered video codecs for encoding :"));
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) if (VIDEOCODECDEF[i].IsFind) AddToSystemProperties("  "+QString(VIDEOCODECDEF[i].LongName)+"-ffmpeg codec:"+QString(VIDEOCODECDEF[i].ShortName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered audio codecs for encoding :"));
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) if (AUDIOCODECDEF[i].IsFind) AddToSystemProperties("  "+QString(AUDIOCODECDEF[i].LongName)+"-ffmpeg codec:"+QString(AUDIOCODECDEF[i].ShortName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Registered container formats for encoding :"));
    for (int i=0;i<NBR_FORMATDEF;i++)     if (FORMATDEF[i].IsFind) AddToSystemProperties("  "+QString(FORMATDEF[i].LongName));
    AddSeparatorToSystemProperties();   AddToSystemProperties(QString("%1").arg(ApplicationConfig->DeviceModelList.RenderDeviceModel.count())+QApplication::translate("MainWindow"," Device registered for rendering"));

    AddSeparatorToSystemProperties();   AddToSystemProperties(QApplication::translate("MainWindow","Library :"));
    QString Path;
    screen.showMessage(QApplication::translate("MainWindow","Loading background library..."),Qt::AlignHCenter|Qt::AlignBottom);
    Path="background";      BackgroundList.ScanDisk(Path,GEOMETRY_16_9); AddToSystemProperties(QString("  %1").arg(BackgroundList.List.count())+QApplication::translate("MainWindow"," images loaded into the background-library from ")+QDir(Path).absolutePath());
    screen.showMessage(QApplication::translate("MainWindow","Loading no-luma transitions..."),Qt::AlignHCenter|Qt::AlignBottom);
    for (int i=0;i<TRANSITIONMAXSUBTYPE_BASE;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_BASE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_ZOOMINOUT;i++)  IconList.List.append(cIconObject(TRANSITIONFAMILLY_ZOOMINOUT,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_SLIDE;i++)      IconList.List.append(cIconObject(TRANSITIONFAMILLY_SLIDE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_PUSH;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_PUSH,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_DEFORM;i++)     IconList.List.append(cIconObject(TRANSITIONFAMILLY_DEFORM,i));
    AddToSystemProperties(QString("  %1").arg(IconList.List.count())+QApplication::translate("MainWindow"," no-luma transitions loaded into the transition-library"));
    screen.showMessage(QApplication::translate("MainWindow","Loading luma transitions..."),Qt::AlignHCenter|Qt::AlignBottom);
    Path="luma/Bar";        LumaList_Bar.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BAR);         AddToSystemProperties(QString("  %1").arg(LumaList_Bar.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Box";        LumaList_Box.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BOX);         AddToSystemProperties(QString("  %1").arg(LumaList_Box.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Center";     LumaList_Center.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CENTER);   AddToSystemProperties(QString("  %1").arg(LumaList_Center.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Checker";    LumaList_Checker.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CHECKER); AddToSystemProperties(QString("  %1").arg(LumaList_Checker.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Clock";      LumaList_Clock.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CLOCK);     AddToSystemProperties(QString("  %1").arg(LumaList_Clock.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    Path="luma/Snake";      LumaList_Snake.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_SNAKE);     AddToSystemProperties(QString("  %1").arg(LumaList_Snake.List.count())+QApplication::translate("MainWindow"," luma transitions loaded into the transition-library from ")+QDir(Path).absolutePath());
    AddToSystemProperties(QApplication::translate("MainWindow","  Total:")+QString("%1").arg(IconList.List.count())+QApplication::translate("MainWindow"," transitions loaded into the transition-library"));

    QFile file("BUILDVERSION.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        CurrentAppVersion=QString(file.readLine());
        if (CurrentAppVersion.endsWith("\n"))   CurrentAppVersion=CurrentAppVersion.left(CurrentAppVersion.length()-QString("\n").length());
        while (CurrentAppVersion.endsWith(" ")) CurrentAppVersion=CurrentAppVersion.left(CurrentAppVersion.length()-1);
        if (CurrentAppVersion.lastIndexOf(" ")) CurrentAppVersion=CurrentAppVersion.mid(CurrentAppVersion.lastIndexOf(" ")+1);
        file.close();
    }

    // Because now we have local installed, then we can translate collection style name
    ApplicationConfig->StyleTextCollection.DoTranslateCollection();
    ApplicationConfig->StyleTextBackgroundCollection.DoTranslateCollection();
    ApplicationConfig->StyleCoordinateCollection.DoTranslateCollection();
    ApplicationConfig->StyleBlockShapeCollection.DoTranslateCollection();
    ApplicationConfig->StyleImageFramingCollection.DoTranslateCollection();

    ApplicationConfig->ImagesCache.MaxValue=ApplicationConfig->MemCacheMaxValue;

    Diaporama=new cDiaporama(ApplicationConfig);
    Diaporama->Timeline=ui->timeline;
    ui->preview->InitDiaporamaPlay(Diaporama);
    ui->preview2->InitDiaporamaPlay(Diaporama);
    connect(ui->preview,SIGNAL(DoubleClick()),this,SLOT(s_Event_DoubleClickedOnObject()));
    connect(ui->preview2,SIGNAL(DoubleClick()),this,SLOT(s_Event_DoubleClickedOnObject()));

    ui->ZoomMinusBT->setEnabled(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH);
    ui->ZoomPlusBT->setEnabled(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH);

    // We have finish with the SplashScreen
    screen.hide();

    connect(ui->ToolBoxNormal,SIGNAL(currentChanged(int)),this,SLOT(s_Event_ToolbarChanged(int)));

    // Help menu
    connect(ui->Action_About_BT,SIGNAL(released()),this,SLOT(s_Action_About()));                            connect(ui->Action_About_BT_2,SIGNAL(released()),this,SLOT(s_Action_About()));
    connect(ui->ActionDocumentation_BT,SIGNAL(released()),this,SLOT(s_Action_Documentation()));             connect(ui->ActionDocumentation_BT_2,SIGNAL(released()),this,SLOT(s_Action_Documentation()));
    connect(ui->ActionNewFunctions_BT,SIGNAL(released()),this,SLOT(s_Action_NewFunctions()));               connect(ui->ActionNewFunctions_BT_2,SIGNAL(released()),this,SLOT(s_Action_NewFunctions()));

    // File menu
    connect(ui->Action_New_BT,SIGNAL(released()),this,SLOT(s_Action_New()));                                connect(ui->Action_New_BT_2,SIGNAL(released()),this,SLOT(s_Action_New()));
    connect(ui->Action_Open_BT,SIGNAL(released()),this,SLOT(s_Action_Open()));                              connect(ui->Action_Open_BT_2,SIGNAL(released()),this,SLOT(s_Action_Open()));
    connect(ui->Action_OpenRecent_BT,SIGNAL(pressed()),this,SLOT(s_Action_OpenRecent()));                   connect(ui->Action_OpenRecent_BT_2,SIGNAL(pressed()),this,SLOT(s_Action_OpenRecent()));
    connect(ui->Action_Save_BT,SIGNAL(released()),this,SLOT(s_Action_Save()));                              connect(ui->Action_Save_BT_2,SIGNAL(released()),this,SLOT(s_Action_Save()));
    connect(ui->ActionSave_as_BT,SIGNAL(released()),this,SLOT(s_Action_SaveAs()));                          connect(ui->ActionSave_as_BT_2,SIGNAL(released()),this,SLOT(s_Action_SaveAs()));
    connect(ui->Action_PrjProperties_BT,SIGNAL(released()),this,SLOT(s_Action_ProjectProperties()));        connect(ui->Action_PrjProperties_BT_2,SIGNAL(released()),this,SLOT(s_Action_ProjectProperties()));
    connect(ui->ActionConfiguration_BT,SIGNAL(released()),this,SLOT(s_Action_ChangeApplicationSettings())); connect(ui->ActionConfiguration_BT_2,SIGNAL(released()),this,SLOT(s_Action_ChangeApplicationSettings()));
    connect(ui->Action_Exit_BT,SIGNAL(released()),this,SLOT(s_Action_Exit()));                              connect(ui->Action_Exit_BT_2,SIGNAL(released()),this,SLOT(s_Action_Exit()));

    // Project menu
    connect(ui->ActionAdd_BT,SIGNAL(released()),this,SLOT(s_Action_AddFile()));                             connect(ui->ActionAdd_BT_2,SIGNAL(released()),this,SLOT(s_Action_AddFile()));
    connect(ui->ActionAddtitle_BT,SIGNAL(released()),this,SLOT(s_Action_AddTitle()));                       connect(ui->ActionAddtitle_BT_2,SIGNAL(released()),this,SLOT(s_Action_AddTitle()));
    connect(ui->ActionAddProject_BT,SIGNAL(released()),this,SLOT(s_Action_AddProject()));                   connect(ui->ActionAddProject_BT_2,SIGNAL(released()),this,SLOT(s_Action_AddProject()));
    connect(ui->ActionRemove_BT,SIGNAL(released()),this,SLOT(s_Action_RemoveObject()));                     connect(ui->ActionRemove_BT_2,SIGNAL(released()),this,SLOT(s_Action_RemoveObject()));
    connect(ui->ActionCut_BT,SIGNAL(released()),this,SLOT(s_Action_CutToClipboard()));                      connect(ui->ActionCut_BT_2,SIGNAL(released()),this,SLOT(s_Action_CutToClipboard()));
    connect(ui->ActionCopy_BT,SIGNAL(released()),this,SLOT(s_Action_CopyToClipboard()));                    connect(ui->ActionCopy_BT_2,SIGNAL(released()),this,SLOT(s_Action_CopyToClipboard()));
    connect(ui->ActionPaste_BT,SIGNAL(released()),this,SLOT(s_Action_PasteFromClipboard()));                connect(ui->ActionPaste_BT_2,SIGNAL(released()),this,SLOT(s_Action_PasteFromClipboard()));
    connect(ui->ActionEdit_BT,SIGNAL(pressed()),this,SLOT(s_Action_EditObject()));                          connect(ui->ActionEdit_BT_2,SIGNAL(pressed()),this,SLOT(s_Action_EditObject()));

    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(s_Event_ClipboardChanged()));

    connect(ui->actionEdit_background,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnBackground()));
    connect(ui->actionEdit_background_transition,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnTransitionBackground()));
    connect(ui->actionEdit_object,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnObject()));
    connect(ui->actionEdit_object_in_transition,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnTransition()));
    connect(ui->actionEdit_music,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnMusic()));

    // Render menu
    connect(ui->ActionRender_BT,SIGNAL(released()),this,SLOT(s_Action_RenderVideo()));                          connect(ui->ActionRender_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderVideo()));
    connect(ui->ActionSmartphone_BT,SIGNAL(released()),this,SLOT(s_Action_RenderSmartphone()));                 connect(ui->ActionSmartphone_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderSmartphone()));
    connect(ui->ActionMultimedia_BT,SIGNAL(released()),this,SLOT(s_Action_RenderMultimedia()));                 connect(ui->ActionMultimedia_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderMultimedia()));
    connect(ui->ActionForTheWEB_BT,SIGNAL(released()),this,SLOT(s_Action_RenderForTheWEB()));                   connect(ui->ActionForTheWEB_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderForTheWEB()));

    // Timeline
    connect(ui->ZoomPlusBT,SIGNAL(released()),this,SLOT(s_Action_ZoomPlus()));
    connect(ui->ZoomMinusBT,SIGNAL(released()),this,SLOT(s_Action_ZoomMinus()));
    connect(ui->timeline,SIGNAL(itemSelectionChanged()),this,SLOT(s_Event_TimelineSelectionChanged()));
    connect(ui->timeline,SIGNAL(DragMoveItem()),this,SLOT(s_Event_TimelineDragMoveItem()));

    connect(ui->PartitionBT,SIGNAL(released()),this,SLOT(s_Action_ChPartitionMode()));
    connect(ui->Partition2BT,SIGNAL(released()),this,SLOT(s_Action_ChPartitionMode()));
    connect(ui->TABTooltip,SIGNAL(linkActivated(const QString)),this,SLOT(s_Action_OpenTABHelpLink(const QString)));

    // Prepare title bar depending on running version
    TitleBar=QString(APPLICATION_NAME)+QString(" ")+QString(APPLICATION_VERSION);
    if ((TitleBar.indexOf("devel")!=-1)||(TitleBar.indexOf("beta")!=-1)) TitleBar=TitleBar+QString(" - ")+CurrentAppVersion;

    // Some other init
    ui->StatusBar_SlideNumber->setText(QApplication::translate("MainWindow","Slide : ")+"0 / 0");
    s_Event_ToolbarChanged(0);
    ToStatusBar("");
    SetModifyFlag(false);           // Setup title window and do first RefreshControls();
    s_Event_ClipboardChanged();     // Setup clipboard button state

    if (ApplicationConfig->CheckConfigAtStartup) QTimer::singleShot(500,this,SLOT(s_Action_DlgCheckConfig())); else {
        QString Status;
        if (!Checkffmpeg(Status)) QTimer::singleShot(500,this,SLOT(s_Action_DlgCheckConfig()));
    }
}

//====================================================================================================================

MainWindow::~MainWindow() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::~MainWindow";
    #endif
    delete Diaporama;
    delete ApplicationConfig;
    SDLLastClose();
    delete ui;
}

//====================================================================================================================
// A REVOIR !!
void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::keyReleaseEvent";
    #endif
    bool Find=false;

    if ((!ApplicationConfig->PartitionMode)&&(ui->ToolBoxNormal->currentIndex()!=0)) {
        Find=true;
        if (event->matches(QKeySequence::Quit))         s_Action_Exit();
        else if (event->matches(QKeySequence::New))     s_Action_New();
        else if (event->matches(QKeySequence::Open))    s_Action_Open();
        else if (event->matches(QKeySequence::Save))    s_Action_Save();
        else if (event->matches(QKeySequence::SaveAs))  s_Action_SaveAs();
        else Find=false;
    }
    if ((!Find)&&(!ApplicationConfig->PartitionMode)&&(ui->ToolBoxNormal->currentIndex()!=1)) {
        Find=true;
        if (event->matches(QKeySequence::Copy))         s_Action_CopyToClipboard();
        else if (event->matches(QKeySequence::Cut))     s_Action_CutToClipboard();
        else if (event->matches(QKeySequence::Paste))   s_Action_PasteFromClipboard();
        else if (event->matches(QKeySequence::Delete))  s_Action_RemoveObject();
        //else if (event->matches(QKeySequence::ZoomIn))  s_Action_ZoomPlus();
        //else if (event->matches(QKeySequence::ZoomOut)) s_Action_ZoomMinus();
        else if (event->key()==Qt::Key_Insert)          s_Action_AddFile();
        else Find=false;
    }
    if ((!Find)&&(!ApplicationConfig->PartitionMode)&&(ui->ToolBoxNormal->currentIndex()!=3)) {
        Find=true;
        if (event->key()==Qt::Key_F1)                   s_Action_Documentation();
        else Find=false;
    }
    if (!Find) {
        Find=true;
        if (event->key()==Qt::Key_F5)                   s_Event_DoubleClickedOnBackground();
        else if (event->key()==Qt::Key_F6)              s_Event_DoubleClickedOnObject();
        else if (event->key()==Qt::Key_F7)              s_Event_DoubleClickedOnMusic();
        else if (event->key()==Qt::Key_F8)              s_Event_DoubleClickedOnTransition();
        else Find=false;
    }

    if (!Find) QMainWindow::keyReleaseEvent(event);
}

//====================================================================================================================

void MainWindow::ToStatusBar(QString Text) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::ToStatusBar";
    #endif
    if (Text=="") {
        if (InternetBUILDVERSION!="") ui->StatusBar_General->setText(InternetBUILDVERSION); else ui->StatusBar_General->setText("");
    } else {
        ui->StatusBar_General->setText(Text);
    }
}

//====================================================================================================================

void MainWindow::SetTimelineHeight() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::SetTimelineHeight";
    #endif
    if (!ApplicationConfig->PartitionMode) {
        ui->scrollArea->setVisible(true);
        ui->ToolBoxPartition->setVisible(false);
        ui->ToolBoxNormal->setVisible(true);
        ui->preview->setVisible(true);
        ui->preview2->setVisible(false);
        ui->TABTooltip->setVisible(true);
        ui->TABToolimg->setVisible(true);
        ui->PartitionBT->setEnabled(false);
        ui->PartitionBT->setDown(true);
        ui->Partition2BT->setEnabled(true);
        QApplication::processEvents();          // Give time to Qt to redefine position of each control and preview height !
        ui->preview->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview->height()-32));
    } else {
        ui->scrollArea->setVisible(false);
        ui->ToolBoxPartition->setVisible(true);
        ui->ToolBoxNormal->setVisible(false);
        ui->preview->setVisible(false);
        ui->preview2->setVisible(true);
        ui->preview2->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview2->height()-32));
        ui->TABTooltip->setVisible(false);
        ui->TABToolimg->setVisible(false);
        ui->PartitionBT->setEnabled(true);
        ui->Partition2BT->setEnabled(false);
        ui->Partition2BT->setDown(true);
    }
    QApplication::processEvents();          // Give time to Qt to redefine position of each control and redraw timeline
    ui->timeline->SetTimelineHeight(ApplicationConfig->PartitionMode);
    QApplication::processEvents();          // Give time to Qt to redefine position of each control and redraw timeline
}

//====================================================================================================================

void MainWindow::closeEvent(QCloseEvent *Event) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::closeEvent";
    #endif
    ui->preview->SetPlayerToPause(); // Ensure player is stop
    ui->preview2->SetPlayerToPause(); // Ensure player is stop
    if (Diaporama->IsModify) {
        int Bt=QMessageBox::question(this,QApplication::translate("MainWindow","Close application"),QApplication::translate("MainWindow","Want to save the project before closing?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
        if (Bt==QMessageBox::Yes) s_Action_Save();
        if (Bt==QMessageBox::Cancel) {
            Event->setAccepted(false);
            return;
        }
    }
    if (isMaximized()) {
        ApplicationConfig->MainWinWSP->IsMaximized=true;
        showNormal();
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    } else ApplicationConfig->MainWinWSP->IsMaximized=false;
    if (isMinimized()) {
        showNormal();
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ApplicationConfig->MainWinWSP->SaveWindowState(this);
    ApplicationConfig->SaveConfigurationFile();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::resizeEvent(QResizeEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::resizeEvent";
    #endif
    ui->preview->SetPlayerToPause();  // Ensure player is stop
    ui->preview2->SetPlayerToPause(); // Ensure player is stop
    SetTimelineHeight();
}

//====================================================================================================================

void MainWindow::showEvent(QShowEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::showEvent";
    #endif
    if (!IsFirstInitDone) {
        IsFirstInitDone=true;                                   // do this only one time
        ApplicationConfig->MainWinWSP->ApplyToWindow(this);     // Restore window position
        if (ApplicationConfig->MainWinWSP->IsMaximized) QTimer::singleShot(500,this,SLOT(DoMaximized()));
        // Start a network process to give last ffdiaporama version from internet web site
        QNetworkAccessManager *mNetworkManager=new QNetworkAccessManager(this);
        connect(mNetworkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(s_Event_NetworkReply(QNetworkReply*)));
        QUrl            url(BUILDVERSION_WEBURL);
        QNetworkReply   *reply  = mNetworkManager->get(QNetworkRequest(url));
        reply->deleteLater();
    }
}

//====================================================================================================================

void MainWindow::DoMaximized() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::DoMaximized";
    #endif
    showMaximized();
}

//====================================================================================================================
// Function use when reading BUILDVERSION from WEB Site
//====================================================================================================================

void MainWindow::s_Event_NetworkReply(QNetworkReply* reply) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_NetworkReply";
    #endif
    if (reply->error()==QNetworkReply::NoError) {
        int httpstatuscode=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
        if ((httpstatuscode>=200)&&(httpstatuscode<300)&&(reply->isReadable())) {
            InternetBUILDVERSION=QString::fromUtf8(reply->readAll().data());
            if (InternetBUILDVERSION.endsWith("\n"))   InternetBUILDVERSION=InternetBUILDVERSION.left(InternetBUILDVERSION.length()-QString("\n").length());
            while (InternetBUILDVERSION.endsWith(" ")) InternetBUILDVERSION=InternetBUILDVERSION.left(InternetBUILDVERSION.length()-1);
            if (InternetBUILDVERSION.lastIndexOf(" ")) InternetBUILDVERSION=InternetBUILDVERSION.mid(InternetBUILDVERSION.lastIndexOf(" ")+1);
            int CurrentVersion =CurrentAppVersion.toInt();
            int InternetVersion=InternetBUILDVERSION.toInt();
            if (InternetVersion>CurrentVersion) InternetBUILDVERSION=QApplication::translate("MainWindow","A new ffDiaporama release is available from WEB site. Please update from http://ffdiaporama.tuxfamily.org !");
                else InternetBUILDVERSION="";
        } else InternetBUILDVERSION="";
    } else InternetBUILDVERSION="";
    ToStatusBar(InternetBUILDVERSION);
}

//====================================================================================================================

void MainWindow::OpenHelp(QString HelpFile) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::OpenHelp";
    #endif
    if (HelpFile.startsWith(("file://"))) {
        QDesktopServices::openUrl(QUrl(HelpFile));
    } else if (HelpFile.startsWith(("http://"))) {
        QString HelpPath;
        if ((ApplicationConfig->CurrentLanguage!="fr")&&(ApplicationConfig->CurrentLanguage!="es")&&(ApplicationConfig->CurrentLanguage!="it")&&(ApplicationConfig->CurrentLanguage!="en")) HelpPath=HelpFile.replace("<local>","en");
            else HelpPath=HelpFile.replace("<local>",ApplicationConfig->CurrentLanguage);
        QDesktopServices::openUrl(QUrl(HelpPath));
    } else {
        QString HelpPath;
        if ((ApplicationConfig->CurrentLanguage!="fr")&&(ApplicationConfig->CurrentLanguage!="es")&&(ApplicationConfig->CurrentLanguage!="it")&&(ApplicationConfig->CurrentLanguage!="en")) HelpPath=WIKI_CMS_PATH+HelpFile+"&lang=en";
            else HelpPath=WIKI_CMS_PATH+HelpFile+"&lang="+ApplicationConfig->CurrentLanguage;
        QDesktopServices::openUrl(QUrl(HelpPath));
    }
}

//====================================================================================================================

void MainWindow::RefreshControls() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::RefreshControls";
    #endif
    // Timeline actions
    ui->ActionRemove_BT->setEnabled(ui->timeline->NbrItem()>0);                                             ui->ActionRemove_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionEdit_BT->setEnabled(ui->timeline->NbrItem()>0);                                               ui->ActionEdit_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ZoomMinusBT->setEnabled((ui->timeline->NbrItem()>0)&&(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH));
    ui->ZoomPlusBT->setEnabled((ui->timeline->NbrItem()>0)&&(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH));

    // File menu
    ui->Action_Save_BT->setEnabled(Diaporama->IsModify);                                                    ui->Action_Save_BT_2->setEnabled(Diaporama->IsModify);
    ui->ActionSave_as_BT->setEnabled(Diaporama->List.count()>0);                                            ui->ActionSave_as_BT_2->setEnabled(Diaporama->List.count()>0);
    ui->Action_OpenRecent_BT->setEnabled(ApplicationConfig->RecentFile.count()>0);                          ui->Action_OpenRecent_BT_2->setEnabled(ApplicationConfig->RecentFile.count()>0);

    // Project menu
    ui->actionEdit_background->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_background_transition->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_object->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_object_in_transition->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_music->setEnabled(ui->timeline->NbrItem()>0);

    // Clipboard_Object
    ui->ActionCopy_BT->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->ActionCopy_BT_2->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->ActionCut_BT->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->ActionCut_BT_2->setEnabled(ui->timeline->CurrentSelected()>=0);

    ui->ActionRender_BT->setEnabled(ui->timeline->NbrItem()>0);                                             ui->ActionRender_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionSmartphone_BT->setEnabled(ui->timeline->NbrItem()>0);                                         ui->ActionSmartphone_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionMultimedia_BT->setEnabled(ui->timeline->NbrItem()>0);                                         ui->ActionMultimedia_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionForTheWEB_BT->setEnabled(ui->timeline->NbrItem()>0);                                          ui->ActionForTheWEB_BT_2->setEnabled(ui->timeline->NbrItem()>0);

    ui->StatusBar_SlideNumber->setText(QApplication::translate("MainWindow","Slide : ")+QString("%1").arg(Diaporama->CurrentCol+1)+" / "+QString("%1").arg(Diaporama->List.count()));
}

//====================================================================================================================

void MainWindow::SetModifyFlag(bool IsModify) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::SetModifyFlag";
    #endif
    Diaporama->IsModify=IsModify;
    this->setWindowTitle(TitleBar+QString("-")+
                         (Diaporama->ProjectFileName!=""?Diaporama->ProjectFileName:QApplication::translate("MainWindow","<new project>","when project have no name define"))+
                         (Diaporama->IsModify?"*":""));
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_Action_About() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_About";
    #endif
    ui->Action_About_BT->setDown(false);
    ui->Action_About_BT_2->setDown(false);
    DlgAbout Dlg("",ApplicationConfig,ApplicationConfig->DlgAboutWSP,this);
    Dlg.InitDialog();
    Dlg.exec();
}

//====================================================================================================================

void MainWindow::s_Action_DlgCheckConfig() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_DlgCheckConfig";
    #endif
    DlgCheckConfig Dlg(HELPFILE_DlgCheckConfig,ApplicationConfig,ApplicationConfig->DlgCheckConfigWSP,this);
    Dlg.InitDialog();
    Dlg.exec();

    QString Status;
    if (!Checkffmpeg(Status)) {
        QMessageBox::critical(this,APPLICATION_NAME,QApplication::translate("MainWindow","Configuration not correct!"));
        close();
    }
}

//====================================================================================================================

void MainWindow::s_Action_Documentation() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_Documentation";
    #endif
    ui->ActionDocumentation_BT->setDown(false);
    ui->ActionDocumentation_BT_2->setDown(false);
    QDesktopServices::openUrl(QUrl(QString(HELPFILE_SUPPORT).replace("<local>",ApplicationConfig->GetValideWEBLanguage(ApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void MainWindow::s_Action_NewFunctions() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_NewFunctions";
    #endif
    ui->ActionNewFunctions_BT->setDown(false);
    ui->ActionNewFunctions_BT_2->setDown(false);
    QDesktopServices::openUrl(QUrl(QString(HELPFILE_NEWS).replace("<local>",ApplicationConfig->GetValideWEBLanguage(ApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void MainWindow::s_Action_Exit() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_Exit";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_Exit()));
        return;
    }

    close();
}

//====================================================================================================================

void MainWindow::s_Action_ZoomPlus() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_ZoomPlus";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_ZoomPlus()));
        return;
    }

    if ((ui->timeline->rowHeight(0)-ApplicationConfig->TimelineHeight/2-TIMELINESOUNDHEIGHT*2)<TIMELINEMAXHEIGH) {
        ApplicationConfig->TimelineHeight+=20;
        SetTimelineHeight();
    }
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_Action_ZoomMinus() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_ZoomMinus";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_ZoomMinus()));
        return;
    }

    if ((ui->timeline->rowHeight(0)-ApplicationConfig->TimelineHeight/2-TIMELINESOUNDHEIGHT*2)>TIMELINEMINHEIGH) {
        ApplicationConfig->TimelineHeight-=20;
        SetTimelineHeight();
    }
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_Action_ChPartitionMode() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_ChPartitionMode";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_ChPartitionMode()));
        return;
    }

    int Selected=ui->timeline->CurrentSelected(); // Save current seleted item
    ApplicationConfig->PartitionMode=!ApplicationConfig->PartitionMode;
    SetTimelineHeight();
    // Re-select previous current seleted item
    if ((Selected>=0)&&(Selected<ui->timeline->NbrItem())) ui->timeline->SetCurrentCell(Selected);
    (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
    (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->Resize();
}

//====================================================================================================================
// Double click on widget in the object track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnObject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_DoubleClickedOnObject";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Event_DoubleClickedOnObject()));
        return;
    }

    bool DoneAgain=true;
    while (DoneAgain) {
        DoneAgain=false;
        int Ret=DlgSlideProperties(Diaporama->List[Diaporama->CurrentCol],this).exec();
        if (Ret!=1) {
            SetModifyFlag(true);
            if (Diaporama->List[Diaporama->CurrentCol]->Thumbnail) {
                delete Diaporama->List[Diaporama->CurrentCol]->Thumbnail;
                Diaporama->List[Diaporama->CurrentCol]->Thumbnail=NULL;
            }
            (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol)-1);
            AdjustRuller();
        }
        if ((Ret==2)||(Ret==3)) {
            Diaporama->CurrentCol=Diaporama->CurrentCol+((Ret==2)?-1:1);
            Diaporama->Timeline->SetCurrentCell(Diaporama->CurrentCol);

            // Update slider mark
            if (Diaporama->List.count()>0)
                (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SetStartEndPos(
                        Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                               // Current slide
                        Diaporama->List[Diaporama->CurrentCol]->GetDuration(),
                        (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1), // Previous slide
                        (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1]->GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                        Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,        // Next slide
                        Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1]->GetDuration():0);
            else (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SetStartEndPos(0,0,-1,0,-1,0);
            // open dialog again
            DoneAgain=true;
        }
    }
}

//====================================================================================================================
// Double click on transition part of widget in the object track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnTransition() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_DoubleClickedOnTransition";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Event_DoubleClickedOnTransition()));
        return;
    }

    if (DlgTransitionProperties(Diaporama->List[Diaporama->CurrentCol],false,this).exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Double click on sound part of widget in the object track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnVideoSound() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_DoubleClickedOnVideoSound";
    #endif
    s_Event_DoubleClickedOnObject();  // No separated process at this time !
}

//====================================================================================================================
// // Double click on widget in the background track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnBackground() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_DoubleClickedOnBackground";
    #endif
    if (Diaporama->CurrentCol>=Diaporama->List.count()) return;
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Event_DoubleClickedOnBackground()));
        return;
    }

    if (DlgBackgroundProperties(Diaporama->List[Diaporama->CurrentCol],this).exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Double click on transition part of widget in the background track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnTransitionBackground() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_DoubleClickedOnTransitionBackground";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Event_DoubleClickedOnTransitionBackground()));
        return;
    }

//    QMessageBox::critical(this,QApplication::translate("MainWindow","Not implemented"),QApplication::translate("MainWindow","Sorry, not yet done !"));
/*
    if (DlgTransitionProperties(&(Diaporama->List[Diaporama->CurrentCol]),true,this).exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
*/
}

//====================================================================================================================
// // Double click on widget in the music track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnMusic() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_DoubleClickedOnMusic";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Event_DoubleClickedOnMusic()));
        return;
    }

    if (DlgMusicProperties(Diaporama->List[Diaporama->CurrentCol],this).exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================

void MainWindow::s_Event_TimelineDragMoveItem() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_TimelineDragMoveItem";
    #endif
    if (DragItemSource<DragItemDest) DragItemDest--;
    Diaporama->List.move(DragItemSource,DragItemDest);
    ui->timeline->setUpdatesEnabled(false);
    ui->timeline->SetCurrentCell(DragItemDest);
    ui->timeline->setUpdatesEnabled(true);  // Reset timeline painting
}

//====================================================================================================================
// Current diaporama object selection changed
//====================================================================================================================

void MainWindow::s_Event_TimelineSelectionChanged() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_TimelineSelectionChanged";
    #endif
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Event_TimelineSelectionChanged()));
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (!FLAGSTOPITEMSELECTION) {
        int Selected=ui->timeline->CurrentSelected();

        if (Selected>=Diaporama->List.count()) {
            Selected=Diaporama->List.count()-1;
            FLAGSTOPITEMSELECTION=true;
            ui->timeline->SetCurrentCell(Selected);
            FLAGSTOPITEMSELECTION=false;
        }
        if (Selected<0) {
            Selected=0;
            FLAGSTOPITEMSELECTION=true;
            ui->timeline->SetCurrentCell(Selected);
            FLAGSTOPITEMSELECTION=false;
        }
        if (Diaporama->CurrentCol<0) Diaporama->CurrentCol=0;

        if ((Diaporama->CurrentCol!=Selected)||((Diaporama->List.count()==1)&&(Diaporama->CurrentCol==0))) {
            //************************************************************
            // We are here only if user has click on the timeline
            //************************************************************

            if (Diaporama->List.count()>0) {
                ui->preview->SetPlayerToPause();    // Ensure player is stop
                ui->preview2->SetPlayerToPause();   // Ensure player is stop
                Diaporama->CurrentCol=Selected;
                Diaporama->CurrentPosition=Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol);
                if (Diaporama->List[Diaporama->CurrentCol]->GetTransitDuration()>0) Diaporama->CurrentPosition--;
                AdjustRuller();
            } else {
                (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(0);
                (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SetStartEndPos(0,0,-1,0,-1,0);
            }
        }
        RefreshControls();
    }
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Update dock informations
//====================================================================================================================

void MainWindow::s_Action_OpenTABHelpLink(const QString Link) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_OpenTABHelpLink";
    #endif
    OpenHelp(Link);
}

void MainWindow::s_Event_ToolbarChanged(int MenuIndex) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_ToolbarChanged";
    #endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString Html;
    switch (MenuIndex) {
    case 0: Html=QApplication::translate("MainWindow","<html><body>Select a project to open or to create a new project<br>"\
                                         "To discover ffDiaporama:<br><a href=\"1567\">Consult the WIKI</a></body></html>");
            break;
    case 1: Html=QApplication::translate("MainWindow","<html><body>Add empty slides or slides based on photos or videos<br>"\
                                         "To discover how to build your slide show and to animate slides:<br><a href=\"1306\">Discover the principles of functioning of ffDiaporama</a></body></html>");
            break;
    case 2: Html=QApplication::translate("MainWindow","<html><body>Select the equipment type that you plan to use for your video<br>"\
                                         "To discover how to render videos:<br><a href=\"1322\">Consult the rendering videos WIKI page</a></body></html>");
            break;
    case 3: Html=QApplication::translate("MainWindow","<html><body>Visit the ffDiaporama Web site to use the forum,<br>"\
                "consult tutorials and learn the lastest news:<br><a href=\"http://ffdiaporama.tuxfamily.org\">http://ffdiaporama.tuxfamily.org</a></body></html>");
            break;
    }

    ui->TABTooltip->setText(Html);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Render project
//====================================================================================================================

void MainWindow::s_Action_RenderVideo() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_RenderVideo";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_RenderVideo()));
        return;
    }
    ui->ActionRender_BT->setDown(false);
    ui->ActionRender_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo(*Diaporama,EXPORTMODE_ADVANCED,this).exec();
    CurrentRenderingDialog=NULL;
    AdjustRuller();
}

void MainWindow::s_Action_RenderSmartphone() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_RenderSmartphone";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_RenderSmartphone()));
        return;
    }
    ui->ActionSmartphone_BT->setDown(false);
    ui->ActionSmartphone_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo(*Diaporama,MODE_SMARTPHONE,this).exec();
    CurrentRenderingDialog=NULL;
    AdjustRuller();
}

void MainWindow::s_Action_RenderMultimedia() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_RenderMultimedia";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_RenderMultimedia()));
        return;
    }
    ui->ActionMultimedia_BT->setDown(false);
    ui->ActionMultimedia_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo(*Diaporama,MODE_MULTIMEDIASYS,this).exec();
    CurrentRenderingDialog=NULL;
    AdjustRuller();
}

void MainWindow::s_Action_RenderForTheWEB() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_RenderForTheWEB";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_RenderForTheWEB()));
        return;
    }
    ui->ActionForTheWEB_BT->setDown(false);
    ui->ActionForTheWEB_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo(*Diaporama,MODE_FORTHEWEB,this).exec();
    CurrentRenderingDialog=NULL;
    AdjustRuller();
}

//====================================================================================================================
// Project properties
//====================================================================================================================

void MainWindow::s_Action_ProjectProperties() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_ProjectProperties";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_ProjectProperties()));
        return;
    }
    ui->Action_PrjProperties_BT->setDown(false);
    ui->Action_PrjProperties_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgffDPjrProperties Dlg(Diaporama->ProjectInfo,HELPFILE_DlgffDPjrProperties,ApplicationConfig,ApplicationConfig->DlgffDPjrPropertiesWSP,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) SetModifyFlag(true);
}

//====================================================================================================================
// Change application settings
//====================================================================================================================

void MainWindow::s_Action_ChangeApplicationSettings() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_ChangeApplicationSettings";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_ChangeApplicationSettings()));
        return;
    }
    ui->ActionConfiguration_BT->setDown(false);
    ui->ActionConfiguration_BT_2->setDown(false);

    DlgApplicationSettings Dlg(HELPFILE_DlgApplicationSettings,ApplicationConfig,ApplicationConfig->DlgApplicationSettingsWSP,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) {
        ToStatusBar(QApplication::translate("MainWindow","Saving configuration file and applying new configuration ..."));
        QTimer::singleShot(500,this,SLOT(DoChangeApplicationSettings()));
        ApplicationConfig->ImagesCache.MaxValue=ApplicationConfig->MemCacheMaxValue;
    }
}

void MainWindow::DoChangeApplicationSettings() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::DoChangeApplicationSettings";
    #endif
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->preview->WantedFPS=ApplicationConfig->PreviewFPS;
    ui->preview2->WantedFPS=ApplicationConfig->PreviewFPS;
    SDLSetFPS(ApplicationConfig->PreviewFPS,ApplicationConfig->SDLAudioOldMode);  // Reinit SDL if Preview FPS has changed
    // Save configuration
    //ApplicationConfig->MainWinWSP->SaveWindowState(this); // Do not change get WindowState for mainwindow except when closing
    ApplicationConfig->SaveConfigurationFile();
    QApplication::restoreOverrideCursor();
    ToStatusBar("");
}

//====================================================================================================================
// New project
//====================================================================================================================

void MainWindow::s_Action_New() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_New";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_New()));
        return;
    }
    ui->Action_New_BT->setDown(false);
    ui->Action_New_BT_2->setDown(false);

    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QApplication::translate("MainWindow","New project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_Action_Save();

    ApplicationConfig->ImagesCache.List.clear();
    cDiaporama *NewDiaporama=new cDiaporama(ApplicationConfig);

    // Clean actual timeline and diaporama
    FLAGSTOPITEMSELECTION=true;
    ui->timeline->setUpdatesEnabled(false);
    ui->timeline->CleanAll();
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
    ui->preview2->InitDiaporamaPlay(Diaporama);
    ui->preview2->SetActualDuration(Diaporama->GetDuration());
    ui->preview2->SetStartEndPos(0,0,-1,0,-1,0);
    SetTimelineHeight();
    RefreshControls();
    SetModifyFlag(false);
}

//====================================================================================================================
// Open an existing project
//====================================================================================================================

void MainWindow::s_Action_OpenRecent() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_OpenRecent";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_OpenRecent()));
        return;
    }
    QMenu *ContextMenu=new QMenu(this);
    for (int i=ApplicationConfig->RecentFile.count()-1;i>=0;i--) ContextMenu->addAction(AdjustDirForOS(ApplicationConfig->RecentFile.at(i)));
    QAction *Action=ContextMenu->exec(QCursor::pos());
    QString Selected="";
    if (Action) Selected=Action->iconText();
    delete ContextMenu;
    ui->Action_OpenRecent_BT->setDown(false);
    ui->Action_OpenRecent_BT_2->setDown(false);

    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QApplication::translate("MainWindow","Open project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_Action_Save();

    if (Selected!="") {
        ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(Selected).fileName());
        FileForIO=Selected;
        QTimer::singleShot(500,this,SLOT(DoOpenFile()));
    }
}

void MainWindow::s_Action_Open() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_Open";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_Open()));
        return;
    }
    ui->Action_Open_BT->setDown(false);
    ui->Action_Open_BT_2->setDown(false);

    if ((Diaporama->IsModify)&&(QMessageBox::question(this,QApplication::translate("MainWindow","Open project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_Action_Save();

    QString ProjectFileName=QFileDialog::getOpenFileName(this,QApplication::translate("MainWindow","Open project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (ProjectFileName!="") {
        ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(ProjectFileName).fileName());
        FileForIO=ProjectFileName;
        QTimer::singleShot(500,this,SLOT(DoOpenFile()));
    }
}

void MainWindow::DoOpenFileParam() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::DoOpenFileParam";
    #endif
    ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(FileForIO).fileName());
    QTimer::singleShot(500,this,SLOT(DoOpenFile()));
}

void MainWindow::DoOpenFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::DoOpenFile";
    #endif

    QString ProjectFileName=AdjustDirForOS(FileForIO);

    // Check if ffDRevision is not > current ffDRevision
    cffDProjectFile File(ApplicationConfig);
    if (File.GetInformationFromFile(ProjectFileName,NULL,NULL)) {
        File.GetFullInformationFromFile();
        if ((File.ffDRevision.toInt()>CurrentAppVersion.toInt())&&(QMessageBox::question(this,QApplication::translate("MainWindow","Open project"),
        QApplication::translate("MainWindow","This project was created with a newer version of ffDiaporama.\nIf you continue, you take the risk of losing data!\nDo you want to open it nevertheless?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No))
            return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ApplicationConfig->ImagesCache.List.clear();

    // Manage Recent files list
    for (int i=0;i<ApplicationConfig->RecentFile.count();i++) if (AdjustDirForOS(ApplicationConfig->RecentFile.at(i))==ProjectFileName) {
        ApplicationConfig->RecentFile.removeAt(i);
        break;
    }

    ApplicationConfig->LastProjectPath=QFileInfo(ProjectFileName).dir().absolutePath();
    // Clean actual timeline and diaporama
    FLAGSTOPITEMSELECTION=true;
    ui->timeline->setUpdatesEnabled(false);
    ui->timeline->CleanAll();
    delete Diaporama;
    Diaporama=NULL;
    ui->timeline->setUpdatesEnabled(true);
    FLAGSTOPITEMSELECTION=false;

    // Create new diaporama
    Diaporama=new cDiaporama(ApplicationConfig);
    Diaporama->Timeline=ui->timeline;

    // Init GUI for this project
    ui->preview->InitDiaporamaPlay(Diaporama);
    ui->preview2->InitDiaporamaPlay(Diaporama);
    SetTimelineHeight();

    // Load file
    SetModifyFlag(false);
    Diaporama->LoadFile(this,ProjectFileName);
    for (int i=0;i<Diaporama->List.count();i++) AddObjectToTimeLine(i);
    AdjustRuller();
    SetModifyFlag(Diaporama->IsModify);
    if (Diaporama->List.count()>0) (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetTransitionDuration(0));
        else (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(0);

    ApplicationConfig->RecentFile.append(ProjectFileName);
    while (ApplicationConfig->RecentFile.count()>10) ApplicationConfig->RecentFile.takeFirst();
    ui->timeline->SetCurrentCell(0);
    RefreshControls();
    QApplication::restoreOverrideCursor();
    ToStatusBar("");
}

//====================================================================================================================
// Save current project
//====================================================================================================================

void MainWindow::s_Action_Save() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_Save";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    ui->Action_Save_BT->setDown(false);
    ui->Action_Save_BT_2->setDown(false);

    if (Diaporama->ProjectFileName=="") s_Action_SaveAs(); else {
        ToStatusBar(QApplication::translate("MainWindow","Saving project file ...")+QFileInfo(Diaporama->ProjectFileName).fileName());
        DoSaveFile();
    }
}

//====================================================================================================================
void MainWindow::DoSaveFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::DoSaveFile";
    #endif
    if (Diaporama->SaveFile(this)) SetModifyFlag(false);
    ToStatusBar("");
}

//====================================================================================================================
// Save current project as
//====================================================================================================================

void MainWindow::s_Action_SaveAs() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_SaveAs";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    ui->ActionSave_as_BT->setDown(false);
    ui->ActionSave_as_BT_2->setDown(false);

    // Save project
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
        ToStatusBar(QApplication::translate("MainWindow","Saving project file ...")+QFileInfo(Diaporama->ProjectFileName).fileName());
        DoSaveFile();
    }
}

//====================================================================================================================
// Add a title object
//====================================================================================================================

void MainWindow::s_Action_AddTitle() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_AddTitle";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_AddTitle()));
        return;
    }
    ui->ActionAddtitle_BT->setDown(false);
    ui->ActionAddtitle_BT_2->setDown(false);

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    int SavedCurIndex=ApplicationConfig->AppendObject?Diaporama->List.count():Diaporama->CurrentCol;
    int CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
    if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;

    Diaporama->List.insert(CurIndex,new cDiaporamaObject(Diaporama));
    cDiaporamaObject *DiaporamaObject       =Diaporama->List[CurIndex];
    DiaporamaObject->List[0]->Parent        =DiaporamaObject;
    DiaporamaObject->List[0]->StaticDuration=ApplicationConfig->NoShotDuration;
    DiaporamaObject->Parent                 =Diaporama;
    DiaporamaObject->TypeObject             =DIAPORAMAOBJECTTYPE_EMPTY;

    if (Diaporama->ApplicationConfig->RandomTransition) {
        qsrand(QTime(0,0,0,0).msecsTo(QTime::currentTime()));
        int Random=qrand();
        Random=int(double(IconList.List.count())*(double(Random)/double(RAND_MAX)));
        if (Random<IconList.List.count()) {
            Diaporama->List[CurIndex]->TransitionFamilly=IconList.List[Random].TransitionFamilly;
            Diaporama->List[CurIndex]->TransitionSubType=IconList.List[Random].TransitionSubType;
        }
    } else {
        Diaporama->List[CurIndex]->TransitionFamilly=Diaporama->ApplicationConfig->DefaultTransitionFamilly;
        Diaporama->List[CurIndex]->TransitionSubType=Diaporama->ApplicationConfig->DefaultTransitionSubType;
    }
    Diaporama->List[CurIndex]->TransitionDuration=Diaporama->ApplicationConfig->DefaultTransitionDuration;
    AddObjectToTimeLine(CurIndex);
    ui->timeline->SetCurrentCell(SavedCurIndex+1);
    SetModifyFlag(true);
    AdjustRuller();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Add an object (image or video)
//====================================================================================================================

void MainWindow::s_Action_AddFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_AddFile";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_AddFile()));
        return;
    }
    ui->ActionAdd_BT->setDown(false);
    ui->ActionAdd_BT_2->setDown(false);

    FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("MainWindow","Add files"),
                                                       ApplicationConfig->RememberLastDirectories?ApplicationConfig->LastMediaPath:"",
                                                       ApplicationConfig->GetFilterForMediaFile(cBaseApplicationConfig::ALLFILE));
    if (FileList.count()>0) {

        // Calc position of new object depending on ApplicationConfig->AppendObject

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

        ToStatusBar(QApplication::translate("MainWindow","Add file to project :")+QFileInfo(FileList[0]).fileName());
        QTimer::singleShot(500,this,SLOT(s_Action_DoAddFile()));
    }
}

//====================================================================================================================

void MainWindow::DoAddDragAndDropFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_DoAddDragAndDropFile";
    #endif
    QTimer::singleShot(500,this,SLOT(s_Action_DoAddFile()));
}

//====================================================================================================================

void MainWindow::s_Action_DoAddFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_DoAddFile";
    #endif

    if (FileList.count()==0) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString NewFile      =FileList.takeFirst();
    int     ChapterNum   =-1;

    // if it's a ffDiaporama project file
    if ((QFileInfo(NewFile).suffix()!="")&&(QFileInfo(NewFile).suffix().toLower()=="ffd")) {

        ApplicationConfig->LastProjectPath=QFileInfo(NewFile).dir().absolutePath();
        Diaporama->AppendFile(this,NewFile,CurIndex);  // Append file

    } else {
        // Image or video file

        // Chapter adjustement
        if (NewFile.contains("#CHAP_")) {
            ChapterNum=NewFile.mid(NewFile.indexOf("#CHAP_")+QString("#CHAP_").length()).toInt();
            NewFile   =NewFile.left(NewFile.indexOf("#CHAP_"));
        }

        QString         BrushFileName=QFileInfo(NewFile).absoluteFilePath();
        QString         Extension=QFileInfo(BrushFileName).suffix().toLower();
        QString         ErrorMessage=QApplication::translate("MainWindow","Format not supported","Error message");
        cBaseMediaFile  *MediaFile=NULL;

        if (ApplicationConfig->AllowImageExtension.contains(Extension))          MediaFile=new cImageFile(ApplicationConfig);
            else if (ApplicationConfig->AllowVideoExtension.contains(Extension)) MediaFile=new cVideoFile(OBJECTTYPE_VIDEOFILE,ApplicationConfig);

        bool IsValide=((MediaFile)&&(MediaFile->GetInformationFromFile(BrushFileName,NULL,NULL)));
        if ((IsValide)&&(MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)) IsValide=((cVideoFile *)MediaFile)->OpenCodecAndFile();
        if (!IsValide) {
            QMessageBox::critical(NULL,QApplication::translate("MainWindow","Error","Error message"),NewFile,QMessageBox::Close);
            if (MediaFile) delete MediaFile;
            QApplication::restoreOverrideCursor();
            return;
        }

        // if file is a video then check if file have at least one sound track compatible
        if ((IsValide)&&(MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)&&(((cVideoFile *)MediaFile)->AudioStreamNumber!=-1)&&(!(
                (((cVideoFile *)MediaFile)->ffmpegAudioFile->streams[((cVideoFile *)MediaFile)->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_S16)||
                (((cVideoFile *)MediaFile)->ffmpegAudioFile->streams[((cVideoFile *)MediaFile)->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_U8)
            ))) {
            ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only audio track with unsigned 8 bits or signed 16 bits sample format","Error message");
            QMessageBox::critical(NULL,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
            if (MediaFile) delete MediaFile;
            QApplication::restoreOverrideCursor();
            return;
        }

        if ((IsValide)&&(MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)&&(((cVideoFile *)MediaFile)->AudioStreamNumber!=-1)&&(((cVideoFile *)MediaFile)->ffmpegAudioFile->streams[((cVideoFile *)MediaFile)->AudioStreamNumber]->codec->channels>2)) {
            ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only mono or stereo audio track","Error message");
            QMessageBox::critical(NULL,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
            if (MediaFile) delete MediaFile;
            QApplication::restoreOverrideCursor();
            return;
        }

        if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

        //**********************************************
        // Chapter management
        //**********************************************

        if ((ChapterNum==-1)&&(MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)&&(((cVideoFile *)MediaFile)->NbrChapters>1)&&(QMessageBox::question(this,QApplication::translate("cBaseMediaFile","Add video file"),
            QApplication::translate("MainWindow","This video files contains more than one chapter.\nDo you want to create one slide for each chapters ?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) {

            // Define Chapter index for this file
            ChapterNum=0;

            // Insert this file again at top for each chapters
            for (int i=((cVideoFile *)MediaFile)->NbrChapters-1;i>0;i--) FileList.insert(0,NewFile+"#CHAP_"+QString("%1").arg(i));
        }

        //**********************************************
        // Create Diaporama Object and load first image
        //**********************************************

        Diaporama->List.insert(CurIndex,new cDiaporamaObject(Diaporama));
        cDiaporamaObject *DiaporamaObject       =Diaporama->List[CurIndex];
        DiaporamaObject->List[0]->Parent        =DiaporamaObject;
        DiaporamaObject->List[0]->StaticDuration=ApplicationConfig->NoShotDuration;
        DiaporamaObject->Parent                 =Diaporama;
        DiaporamaObject->TypeObject             =DIAPORAMAOBJECTTYPE_EMPTY;

        // Create and append a composition block to the object list
        DiaporamaObject->ObjectComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey,ApplicationConfig));
        cCompositionObject *CompositionObject   =DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
        cBrushDefinition   *CurrentBrush        =CompositionObject->BackgroundBrush;

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

        //*****************************************************
        // Transfert mediafile to brush and chapter management
        //*****************************************************

        if (MediaFile->ObjectType==OBJECTTYPE_IMAGEFILE) {
            CurrentBrush->Image=(cImageFile *)MediaFile;
        } else if (MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE) {
            CurrentBrush->Video=(cVideoFile *)MediaFile;
            DiaporamaObject->List[0]->StaticDuration=1000;
            if (ChapterNum>=0) {
                QString ChapterStr=QString("%1").arg(ChapterNum);
                while (ChapterStr.length()<3) ChapterStr="0"+ChapterStr;
                ChapterStr="Chapter_"+ChapterStr+":";
                QString Start=MediaFile->GetInformationValue(ChapterStr+"Start");
                QString End  =MediaFile->GetInformationValue(ChapterStr+"End");
                CurrentBrush->Video->StartPos=QTime().fromString(Start);
                CurrentBrush->Video->EndPos  =QTime().fromString(End);
                DiaporamaObject->SlideName   =MediaFile->GetInformationValue(ChapterStr+"title");
            } else CurrentBrush->Video->EndPos=CurrentBrush->Video->Duration;
        }

        //*****************************************************
        // Try to load an image to ensure all is ok
        //*****************************************************
        QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,&CurrentBrush->Image->BrushFileTransform):
                       CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,0,NULL,1,false,&CurrentBrush->Video->BrushFileTransform,false):
                       NULL);

        if (!Image) {
            QMessageBox::critical(NULL,QApplication::translate("MainWindow","Error","Error message"),NewFile,QMessageBox::Close);
            delete MediaFile;
            delete Diaporama->List.takeAt(CurIndex);
            QApplication::restoreOverrideCursor();
            return;
        }

        // No future need of this
        delete Image;

        //**********************************************
        // Apply default style to media file
        //**********************************************

        // Apply Styles for texte
        CompositionObject->ApplyTextStyle(ApplicationConfig->StyleTextCollection.GetStyleDef(ApplicationConfig->StyleTextCollection.DecodeString(ApplicationConfig->DefaultBlockSL_IMG_TextST)));

        // Apply Styles for shape
        CompositionObject->ApplyBlockShapeStyle(ApplicationConfig->StyleBlockShapeCollection.GetStyleDef(ApplicationConfig->StyleBlockShapeCollection.DecodeString(ApplicationConfig->DefaultBlockSL_IMG_ShapeST)));

        // Apply styles for coordinates
        // Force filtering for CoordinateStyle
        ApplicationConfig->StyleCoordinateCollection.SetImageGeometryFilter(Diaporama->ImageGeometry,CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry);
        CompositionObject->ApplyCoordinateStyle(ApplicationConfig->StyleCoordinateCollection.GetStyleDef(ApplicationConfig->StyleCoordinateCollection.DecodeString(
            ApplicationConfig->DefaultBlockSL_IMG_CoordST[CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry][Diaporama->ImageGeometry])));

        // Special case for nonstandard image => force to image geometry constraint and adapt frame coordinates
        if ((CurrentBrush->Image?CurrentBrush->Image->ObjectGeometry:CurrentBrush->Video->ObjectGeometry)==IMAGE_GEOMETRY_UNKNOWN) {
            double ImageGeometry=1;
            double ProjectGeometry=1;
            double NewW,NewH;

            switch (Diaporama->ImageGeometry) {
                case GEOMETRY_4_3   : ProjectGeometry=double(1440)/double(1920);  break;
                case GEOMETRY_16_9  : ProjectGeometry=double(1080)/double(1920);  break;
                case GEOMETRY_40_17 : ProjectGeometry=double(816)/double(1920);   break;

            }
            ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment
            switch (DiaporamaObject->Parent->ApplicationConfig->DefaultBlockSL_CLIPARTLOCK[DiaporamaObject->Parent->ImageGeometry]) {
                case 0 :    // Adjust to Full in lock to image geometry mode
                    ImageGeometry=1;
                    if (CurrentBrush->Image)            ImageGeometry=double(CurrentBrush->Image->ImageHeight)/double(CurrentBrush->Image->ImageWidth);
                        else if (CurrentBrush->Video)   ImageGeometry=double(CurrentBrush->Video->ImageHeight)/double(CurrentBrush->Video->ImageWidth);
                    CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                    CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingF);
                    NewW=CompositionObject->w*Diaporama->InternalWidth;
                    NewH=NewW*CurrentBrush->AspectRatio;
                    NewW=NewW/Diaporama->InternalWidth;
                    NewH=NewH/Diaporama->InternalHeight;
                    if (NewH>1) {
                        NewH=CompositionObject->h*Diaporama->InternalHeight;
                        NewW=NewH/CurrentBrush->AspectRatio;
                        NewW=NewW/Diaporama->InternalWidth;
                        NewH=NewH/Diaporama->InternalHeight;
                    }
                    CompositionObject->w=NewW;
                    CompositionObject->h=NewH;
                    break;
                case 1 :    // Lock to project geometry - To full
                    ImageGeometry=ProjectGeometry;
                    CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                    CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingF);
                    break;
                case 2 :    // Lock to project geometry - To width
                    ImageGeometry=ProjectGeometry;
                    CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                    CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingW);
                    break;
                case 3 :    // Lock to project geometry - To height
                    ImageGeometry=ProjectGeometry;
                    CurrentBrush->InitDefaultFramingStyle(true,ImageGeometry);
                    CurrentBrush->ApplyStyle(true,CurrentBrush->DefaultFramingH);
                    break;
            }
        }

        //*************************************************************
        // Now create and append a shot composition block to all shot
        //*************************************************************
        for (int i=0;i<DiaporamaObject->List.count();i++) {
            DiaporamaObject->List[i]->ShotComposition.List.append(new cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey,ApplicationConfig));
            DiaporamaObject->List[i]->ShotComposition.List[DiaporamaObject->List[i]->ShotComposition.List.count()-1]->CopyFromCompositionObject(CompositionObject);
        }

        //*************************************************************
        // Now setup transition
        //*************************************************************
        if (ChapterNum<1) {
            if (Diaporama->ApplicationConfig->RandomTransition) {
                qsrand(QTime(0,0,0,0).msecsTo(QTime::currentTime()));
                int Random=qrand();
                Random=int(double(IconList.List.count())*(double(Random)/double(RAND_MAX)));
                if (Random<IconList.List.count()) {
                    Diaporama->List[CurIndex]->TransitionFamilly=IconList.List[Random].TransitionFamilly;
                    Diaporama->List[CurIndex]->TransitionSubType=IconList.List[Random].TransitionSubType;
                }
            } else {
                Diaporama->List[CurIndex]->TransitionFamilly=Diaporama->ApplicationConfig->DefaultTransitionFamilly;
                Diaporama->List[CurIndex]->TransitionSubType=Diaporama->ApplicationConfig->DefaultTransitionSubType;
            }
            Diaporama->List[CurIndex]->TransitionDuration=Diaporama->ApplicationConfig->DefaultTransitionDuration;
        } else {
            // No transition for chapter > 1
            Diaporama->List[CurIndex]->TransitionFamilly =0;
            Diaporama->List[CurIndex]->TransitionSubType =0;
            Diaporama->List[CurIndex]->TransitionDuration=0;
        }
        if (ChapterNum>=0) {
            // But keep chapter information
            Diaporama->List[CurIndex]->StartNewChapter   =true;
        }

        //*************************************************************
        // Adjust project and display
        //*************************************************************

        // Inc NextIndexKey
        DiaporamaObject->NextIndexKey++;
        AddObjectToTimeLine(CurIndex++);
    }
    // Set current selection to first new object
    ui->timeline->SetCurrentCell(SavedCurIndex+1);

    // Set title flag
    AdjustRuller();
    SetModifyFlag(true);
    QApplication::restoreOverrideCursor();

    // If file list contains other file then send a newer signal message to proceed the next one
    if (FileList.count()>0) {
        ToStatusBar(QApplication::translate("MainWindow","Add file to project :")+QFileInfo(FileList[0]).fileName());
        QTimer::singleShot(500,this,SLOT(s_Action_DoAddFile()));
    } else ToStatusBar("");
}

//====================================================================================================================
// Add an (sub) project
//====================================================================================================================

void MainWindow::s_Action_AddProject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_AddProject";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_AddProject()));
        return;
    }
    ui->ActionAddProject_BT->setDown(false);
    ui->ActionAddProject_BT_2->setDown(false);

    FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("MainWindow","Add a sub project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (FileList.count()>0) {
        // Calc position of new object depending on ApplicationConfig->AppendObject
        if (ApplicationConfig->AppendObject) {
            SavedCurIndex   =Diaporama->List.count();
            CurIndex        =Diaporama->List.count();
        } else {
            SavedCurIndex=Diaporama->CurrentCol;
            CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
            if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;
        }
        ToStatusBar(QApplication::translate("MainWindow","Add project file :")+QFileInfo(FileList[0]).fileName());
        QTimer::singleShot(500,this,SLOT(s_Action_DoAddFile()));
    }
}

//====================================================================================================================

void MainWindow::s_Action_EditObject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_EditObject";
    #endif
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionEdit_background);
    ContextMenu->addAction(ui->actionEdit_object);
    ContextMenu->addAction(ui->actionEdit_music);
    ContextMenu->addAction(ui->actionEdit_object_in_transition);
    ContextMenu->addAction(ui->actionEdit_background_transition);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->ActionEdit_BT->setDown(false);
    ui->ActionEdit_BT_2->setDown(false);
}

//====================================================================================================================

void MainWindow::s_Action_RemoveObject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_RemoveObject";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_RemoveObject()));
        return;
    }
    ui->ActionRemove_BT->setDown(false);
    ui->ActionRemove_BT_2->setDown(false);

    int Current=ui->timeline->CurrentSelected();
    if ((Current<0)||(Current>=Diaporama->List.count())) return;

    if ((ApplicationConfig->AskUserToRemove)&&(QMessageBox::question(this,QApplication::translate("MainWindow","Remove slide"),QApplication::translate("MainWindow","Are you sure to want to delete this slide?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->timeline->setUpdatesEnabled(false);
    FLAGSTOPITEMSELECTION=true;
    delete Diaporama->List.takeAt(Current);
    if (Current>=Diaporama->List.count()) Current=Diaporama->List.count()-1;
    ui->timeline->ResetDisplay(Current);    // FLAGSTOPITEMSELECTION is set to false by ResetDisplay
    (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->GetObjectStartPosition(Current)+Diaporama->GetTransitionDuration(Current));
    SetModifyFlag(true);
    AdjustRuller();
    ui->timeline->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_Action_CutToClipboard() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_CutToClipboard";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_CutToClipboard()));
        return;
    }
    ui->ActionCut_BT->setDown(false);
    ui->ActionCut_BT_2->setDown(false);

    int Current=ui->timeline->CurrentSelected();
    if ((Current<0)||(Current>Diaporama->List.count()-1)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Create xml document and root
    QDomDocument Object=QDomDocument(APPLICATION_NAME);
    QDomElement  root  =Object.createElement("CLIPBOARD");
    Diaporama->List[Current]->SaveToXML(root,"CLIPBOARD-OBJECT",Diaporama->ProjectFileName,true);
    Object.appendChild(root);

    // Transfert xml document to clipboard
    QMimeData *SlideData=new QMimeData();
    SlideData->setData("ffDiaporama/slide",Object.toByteArray());
    QApplication::clipboard()->setMimeData(SlideData);

    s_Action_RemoveObject();   // RefreshControls() done by s_Action_RemoveObject()
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_Action_CopyToClipboard() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_CopyToClipboard";
    #endif
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_CopyToClipboard()));
        return;
    }
    ui->ActionCopy_BT->setDown(false);
    ui->ActionCopy_BT_2->setDown(false);

    int Current=ui->timeline->CurrentSelected();
    if ((Current<0)||(Current>Diaporama->List.count()-1)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Create xml document and root
    QDomDocument Object=QDomDocument(APPLICATION_NAME);
    QDomElement  root  =Object.createElement("CLIPBOARD");
    Diaporama->List[Current]->SaveToXML(root,"CLIPBOARD-OBJECT",Diaporama->ProjectFileName,true);
    Object.appendChild(root);

    // Transfert xml document to clipboard
    QMimeData *SlideData=new QMimeData();
    SlideData->setData("ffDiaporama/slide",Object.toByteArray());
    QApplication::clipboard()->setMimeData(SlideData);

    RefreshControls();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_Action_PasteFromClipboard() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Action_PasteFromClipboard";
    #endif

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(500,this,SLOT(s_Action_PasteFromClipboard()));
        return;
    }
    ui->ActionPaste_BT->setDown(false);
    ui->ActionPaste_BT_2->setDown(false);

    // Calc position of new object depending on ApplicationConfig->AppendObject
    int SavedCurIndex=ApplicationConfig->AppendObject?Diaporama->List.count():Diaporama->CurrentCol;
    int CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
    if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;

    Diaporama->List.insert(CurIndex,new cDiaporamaObject(Diaporama));

    const QMimeData *SlideData=QApplication::clipboard()->mimeData();
    if (SlideData->hasFormat("ffDiaporama/slide")) {
        QDomDocument Object=QDomDocument(APPLICATION_NAME);
        Object.setContent(SlideData->data("ffDiaporama/slide"));
        if ((Object.elementsByTagName("CLIPBOARD").length()>0)&&(Object.elementsByTagName("CLIPBOARD").item(0).isElement()==true)) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QDomElement root=Object.elementsByTagName("CLIPBOARD").item(0).toElement();
            ui->timeline->setUpdatesEnabled(false);
            Diaporama->List[CurIndex]->LoadFromXML(root,"CLIPBOARD-OBJECT","",NULL);
            AddObjectToTimeLine(CurIndex);
            SetModifyFlag(true);
            // Set current selection to first new object
            ui->timeline->SetCurrentCell(SavedCurIndex+1);
            AdjustRuller();
            ui->timeline->setUpdatesEnabled(true);
            QApplication::restoreOverrideCursor();
        }
    }
}

//====================================================================================================================

void MainWindow::s_Event_ClipboardChanged() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Event_ClipboardChanged";
    #endif

    ui->ActionPaste_BT->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/slide")));
    ui->ActionPaste_BT_2->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/slide")));
}

//====================================================================================================================

void MainWindow::AddObjectToTimeLine(int CurIndex) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::AddObjectToTimeLine";
    #endif
    ui->timeline->AddObjectToTimeLine(CurIndex);
}

//====================================================================================================================
// Adjust preview ruller depending on current Disporama Currentcol
//====================================================================================================================

void MainWindow::AdjustRuller() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::AdjustRuller";
    #endif
    ui->preview->SetActualDuration(Diaporama->GetDuration());
    ui->preview2->SetActualDuration(Diaporama->GetDuration());
    if (Diaporama->List.count()>0)  {
        Diaporama->ProjectInfo->Duration=Diaporama->GetDuration();
        Diaporama->ProjectInfo->NbrSlide=Diaporama->List.count();
        ui->preview->SetStartEndPos(
                Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                           // Current slide
                Diaporama->List[Diaporama->CurrentCol]->GetDuration(),
                (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1),                            // Previous slide
                (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1]->GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,    // Next slide
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1]->GetDuration():0);
        ui->preview2->SetStartEndPos(
                Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                           // Current slide
                Diaporama->List[Diaporama->CurrentCol]->GetDuration(),
                (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1),                            // Previous slide
                (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1]->GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,    // Next slide
                Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1]->GetDuration():0);
    } else {
        ui->preview->SetStartEndPos(0,0,-1,0,-1,0);
        ui->preview2->SetStartEndPos(0,0,-1,0,-1,0);
    }
    ui->timeline->repaint();
    (ApplicationConfig->PartitionMode?ui->preview2:ui->preview)->SeekPlayer(Diaporama->CurrentPosition);
    RefreshControls();
}
