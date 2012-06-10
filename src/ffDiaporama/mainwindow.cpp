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
#include "_ImagesDefinitions.h"
#include "_ApplicationDefinitions.h"
#include "cCustomSlideTable.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QMimeData>
#include <QDomElement>
#include <QDomDocument>
#include <QSplashScreen>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>

#include <../engine/QCustomFolderTree.h>
#include <../engine/QCustomFolderTable.h>
#include <../engine/QCustomHorizSplitter.h>
#include <../engine/QCustomFileInfoLabel.h>

#include "DlgInfoFile/DlgInfoFile.h"
#include "DlgCheckConfig/DlgCheckConfig.h"
#include "DlgffDPjrProperties/DlgffDPjrProperties.h"
#include "DlgRenderVideo/DlgRenderVideo.h"
#include "DlgAbout/DlgAbout.h"
#include "DlgTransition/DlgTransitionProperties.h"
#include "DlgMusic/DlgMusicProperties.h"
#include "DlgBackground/DlgBackgroundProperties.h"
#include "DlgSlide/DlgSlideProperties.h"
#include "DlgAppSettings/DlgApplicationSettings.h"
#include "DlgManageFavorite/DlgManageFavorite.h"

MainWindow  *GlobalMainWindow=NULL;

#define LATENCY 50

//====================================================================================================================

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::MainWindow");

    ApplicationConfig       =new cApplicationConfig(this);
    CurrentThreadId         =this->thread()->currentThreadId();
    InternetBUILDVERSION    ="";
    GlobalMainWindow        =this;
    IsFirstInitDone         =false;        // true when first show window was done
    FLAGSTOPITEMSELECTION   =false;        // Flag to stop Item Selection process for delete and move of object
    DragItemSource          =-1;
    DragItemDest            =-1;
    IsDragOn                =0;
    InPlayerUpdate          =false;
    DriveList               =new cDriveList(ApplicationConfig);
    DlgWorkingTaskDialog    =NULL;
    setAcceptDrops(true);
    ApplicationConfig->ParentWindow=this;
}

//====================================================================================================================

void MainWindow::InitWindow(QString ForceLanguage,QApplication *App) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::InitWindow");

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
    screen.showMessage(QApplication::translate("MainWindow","Starting libav..."),Qt::AlignHCenter|Qt::AlignBottom);
    if (!ApplicationConfig->DeviceModelList.Initffmpeg(ApplicationConfig->BinaryEncoderPath)) exit(1);

    // Display libav versions
    AddToSystemProperties(QString(LIBAVCODECVERSION_STR)+QString("%1").arg(LIBAVCODEC_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MINOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MICRO)+"."+QString("%1").arg(avcodec_version())+"-Licence="+QString(avcodec_license()));
    AddToSystemProperties(QString(LIBAVFORMATVERSION_STR)+QString("%1").arg(LIBAVFORMAT_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MINOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MICRO)+"."+QString("%1").arg(avformat_version())+"-Licence="+QString(avformat_license()));
    AddToSystemProperties(QString(LIBSWSCALEVERSION_STR)+QString("%1").arg(LIBSWSCALE_VERSION_MAJOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MINOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MICRO)+"."+QString("%1").arg(swscale_version())+"-Licence="+QString(swscale_license()));

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

    // Force icon in contextual menu
    ui->actionAddTitle->setIconVisibleInMenu(true);
    ui->actionAddFiles->setIconVisibleInMenu(true);
    ui->actionAddProject->setIconVisibleInMenu(true);
    ui->actionEdit_background->setIconVisibleInMenu(true);
    ui->actionEdit_object->setIconVisibleInMenu(true);
    ui->actionEdit_music->setIconVisibleInMenu(true);
    ui->actionEdit_object_in_transition->setIconVisibleInMenu(true);
    ui->actionCut->setIconVisibleInMenu(true);
    ui->actionCopy->setIconVisibleInMenu(true);
    ui->actionPaste->setIconVisibleInMenu(true);
    ui->actionRemove->setIconVisibleInMenu(true);
    ui->actionAddToFavorite->setIconVisibleInMenu(true);
    ui->actionManageFavorite->setIconVisibleInMenu(true);
    ui->actionBrowserOpen->setIconVisibleInMenu(true);
    ui->actionBrowserProperties->setIconVisibleInMenu(true);
    ui->actionBrowserRemoveFile->setIconVisibleInMenu(true);
    ui->actionBrowserUseAsPlaylist->setIconVisibleInMenu(true);
    ui->actionBrowserAddFiles->setIconVisibleInMenu(true);
    ui->actionBrowserAddProject->setIconVisibleInMenu(true);
    ui->actionBrowserRenameFile->setIconVisibleInMenu(true);

    // Initialise integrated browser
    ui->FileInfoLabel->DisplayMode=DISPLAY_WEBLONG;
    ui->FileInfoLabel->setVisible(ApplicationConfig->CurrentMode!=DISPLAY_WEBLONG);
    ui->FolderTree->ApplicationConfig =ApplicationConfig;
    ui->FolderTree->IsRemoveAllowed   =true;
    ui->FolderTree->IsRenameAllowed   =true;
    ui->FolderTable->ApplicationConfig=ApplicationConfig;
    ui->FileInfoLabel->DisplayMode=DISPLAY_WEBLONG;
    ui->FileInfoLabel->setVisible(ApplicationConfig->CurrentMode!=DISPLAY_WEBLONG);
    DriveList->UpdateDriveList();
    ui->FolderTree->InitDrives(DriveList);
    ui->FolderTable->SetMode(ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);

    // Initialise diaporama
    Diaporama=new cDiaporama(ApplicationConfig);
    ui->preview->InitDiaporamaPlay(Diaporama);
    ui->preview2->InitDiaporamaPlay(Diaporama);

    ui->ZoomMinusBT->setEnabled(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH);
    ui->ZoomPlusBT->setEnabled(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH);

    // We have finish with the SplashScreen
    screen.hide();

    connect(ui->TABTooltip,SIGNAL(linkActivated(const QString)),this,SLOT(s_Action_OpenTABHelpLink(const QString)));
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
    connect(ui->ActionAdd_BT,SIGNAL(released()),this,SLOT(s_Action_AddFile()));
    connect(ui->ActionAdd_BT_2,SIGNAL(released()),this,SLOT(s_Action_AddFile()));
    connect(ui->actionAddFiles,SIGNAL(triggered()),this,SLOT(s_Action_AddFile()));

    connect(ui->ActionAddtitle_BT,SIGNAL(released()),this,SLOT(s_Action_AddTitle()));
    connect(ui->ActionAddtitle_BT_2,SIGNAL(released()),this,SLOT(s_Action_AddTitle()));
    connect(ui->actionAddTitle,SIGNAL(triggered()),this,SLOT(s_Action_AddTitle()));

    connect(ui->ActionAddProject_BT,SIGNAL(released()),this,SLOT(s_Action_AddProject()));
    connect(ui->ActionAddProject_BT_2,SIGNAL(released()),this,SLOT(s_Action_AddProject()));
    connect(ui->actionAddProject,SIGNAL(triggered()),this,SLOT(s_Action_AddProject()));

    connect(ui->ActionRemove_BT,SIGNAL(released()),this,SLOT(s_Action_RemoveObject()));
    connect(ui->ActionRemove_BT_2,SIGNAL(released()),this,SLOT(s_Action_RemoveObject()));
    connect(ui->actionRemove,SIGNAL(triggered()),this,SLOT(s_Action_RemoveObject()));
    connect(ui->ActionCut_BT,SIGNAL(released()),this,SLOT(s_Action_CutToClipboard()));
    connect(ui->ActionCut_BT_2,SIGNAL(released()),this,SLOT(s_Action_CutToClipboard()));
    connect(ui->actionCut,SIGNAL(triggered()),this,SLOT(s_Action_CutToClipboard()));
    connect(ui->ActionCopy_BT,SIGNAL(released()),this,SLOT(s_Action_CopyToClipboard()));
    connect(ui->ActionCopy_BT_2,SIGNAL(released()),this,SLOT(s_Action_CopyToClipboard()));
    connect(ui->actionCopy,SIGNAL(triggered()),this,SLOT(s_Action_CopyToClipboard()));
    connect(ui->ActionPaste_BT,SIGNAL(released()),this,SLOT(s_Action_PasteFromClipboard()));
    connect(ui->ActionPaste_BT_2,SIGNAL(released()),this,SLOT(s_Action_PasteFromClipboard()));
    connect(ui->actionPaste,SIGNAL(triggered()),this,SLOT(s_Action_PasteFromClipboard()));
    connect(ui->ActionEdit_BT,SIGNAL(pressed()),this,SLOT(s_Action_EditObject()));
    connect(ui->ActionEdit_BT_2,SIGNAL(pressed()),this,SLOT(s_Action_EditObject()));

    connect(QApplication::clipboard(),SIGNAL(dataChanged()),this,SLOT(s_Event_ClipboardChanged()));

    connect(ui->actionEdit_background,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnBackground()));
    connect(ui->actionEdit_object,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnObject()));
    connect(ui->actionEdit_object_in_transition,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnTransition()));
    connect(ui->actionEdit_music,SIGNAL(triggered()),this,SLOT(s_Event_DoubleClickedOnMusic()));

    // Render menu
    connect(ui->ActionRender_BT,SIGNAL(released()),this,SLOT(s_Action_RenderVideo()));                          connect(ui->ActionRender_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderVideo()));
    connect(ui->ActionSmartphone_BT,SIGNAL(released()),this,SLOT(s_Action_RenderSmartphone()));                 connect(ui->ActionSmartphone_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderSmartphone()));
    connect(ui->ActionMultimedia_BT,SIGNAL(released()),this,SLOT(s_Action_RenderMultimedia()));                 connect(ui->ActionMultimedia_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderMultimedia()));
    connect(ui->ActionForTheWEB_BT,SIGNAL(released()),this,SLOT(s_Action_RenderForTheWEB()));                   connect(ui->ActionForTheWEB_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderForTheWEB()));
    connect(ui->ActionLossLess_BT,SIGNAL(released()),this,SLOT(s_Action_RenderLossLess()));                     connect(ui->ActionLossLess_BT_2,SIGNAL(released()),this,SLOT(s_Action_RenderLossLess()));

    // Timeline
    connect(ui->ZoomPlusBT,SIGNAL(released()),this,SLOT(s_Action_ZoomPlus()));
    connect(ui->ZoomMinusBT,SIGNAL(released()),this,SLOT(s_Action_ZoomMinus()));
    connect(ui->timeline,SIGNAL(itemSelectionChanged()),this,SLOT(s_Event_TimelineSelectionChanged()));
    connect(ui->timeline,SIGNAL(DragMoveItem()),this,SLOT(s_Event_TimelineDragMoveItem()));
    connect(ui->timeline,SIGNAL(DoAddDragAndDropFile()),this,SLOT(s_Event_TimelineAddDragAndDropFile()));
    connect(ui->PartitionBT,SIGNAL(released()),this,SLOT(s_Action_ChWindowDisplayMode_ToPlayerMode()));
    connect(ui->Partition2BT,SIGNAL(released()),this,SLOT(s_Action_ChWindowDisplayMode_ToPartitionMode()));
    connect(ui->Partition3BT,SIGNAL(released()),this,SLOT(s_Action_ChWindowDisplayMode_ToBrowserMode()));

    // Contextual menu
    connect(ui->timeline,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_Event_ContextualMenu(QMouseEvent *)));
    connect(ui->preview,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_Event_ContextualMenu(QMouseEvent *)));
    connect(ui->preview2,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_Event_ContextualMenu(QMouseEvent *)));

    // double click
    connect(ui->preview,SIGNAL(DoubleClick()),this,SLOT(s_Event_DoubleClickedOnObject()));
    connect(ui->preview2,SIGNAL(DoubleClick()),this,SLOT(s_Event_DoubleClickedOnObject()));

    // Save image event
    connect(ui->preview,SIGNAL(SaveImageEvent()),this,SLOT(s_Event_SaveImageEvent()));
    connect(ui->preview2,SIGNAL(SaveImageEvent()),this,SLOT(s_Event_SaveImageEvent()));

    // Browser event
    connect(ui->FolderTree,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(s_Browser_FloderTreeItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
    connect(ui->FolderTree,SIGNAL(ActionRemoveFolder()),this,SLOT(s_Browser_RemoveFolder()));
    connect(ui->FolderTree,SIGNAL(ActionRenameFolder()),this,SLOT(s_Browser_RenameFolder()));
    connect(ui->FolderTable,SIGNAL(itemSelectionChanged()),this,SLOT(DoBrowserRefreshSelectedFileInfo()));
    connect(ui->FolderTable,SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(s_Browser_DoubleClicked(QMouseEvent *)));
    connect(ui->FolderTable,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_Browser_RightClicked(QMouseEvent *)));
    connect(ui->FolderTable,SIGNAL(RefreshFolderInfo()),this,SLOT(DoBrowserRefreshFolderInfo()));
    connect(ui->ActionModeBt,SIGNAL(pressed()),this,SLOT(s_Browser_ChangeDisplayMode()));
    connect(ui->RefreshBt,SIGNAL(released()),this,SLOT(s_Browser_RefreshAll()));
    connect(ui->PreviousFolderBt,SIGNAL(released()),this,SLOT(s_Browser_SetToPrevious()));
    connect(ui->UpFolderBt,SIGNAL(released()),this,SLOT(s_Browser_SetToUpper()));
    connect(ui->FavoriteBt,SIGNAL(pressed()),this,SLOT(s_Browser_Favorite()));
    connect(ui->actionAddToFavorite,SIGNAL(triggered()),this,SLOT(s_Browser_AddToFavorite()));
    connect(ui->actionManageFavorite,SIGNAL(triggered()),this,SLOT(s_Browser_ManageFavorite()));
    connect(ui->actionBrowserOpen,SIGNAL(triggered()),this,SLOT(s_Browser_OpenFile()));
    connect(ui->actionBrowserProperties,SIGNAL(triggered()),this,SLOT(s_Browser_Properties()));
    connect(ui->actionBrowserRemoveFile,SIGNAL(triggered()),this,SLOT(s_Browser_RemoveFile()));
    connect(ui->actionBrowserRenameFile,SIGNAL(triggered()),this,SLOT(s_Browser_RenameFile()));
    connect(ui->actionBrowserUseAsPlaylist,SIGNAL(triggered()),this,SLOT(s_Browser_UseAsPlaylist()));
    connect(ui->actionBrowserAddFiles,SIGNAL(triggered()),this,SLOT(s_Browser_AddFiles()));
    connect(ui->actionBrowserAddProject,SIGNAL(triggered()),this,SLOT(s_Browser_AddFiles()));

    ui->FolderTree->SetSelectItemByPath(ui->FolderTree->RealPathToTreePath(ApplicationConfig->CurrentPath));
    if (ui->FolderTree->GetCurrentFolderPath()!=ApplicationConfig->CurrentPath) ui->FolderTree->SetSelectItemByPath(PersonalFolder);

    // Prepare title bar depending on running version
    TitleBar=QString(APPLICATION_NAME)+QString(" ")+QString(APPLICATION_VERSION);
    if ((TitleBar.indexOf("devel")!=-1)||(TitleBar.indexOf("beta")!=-1)) TitleBar=TitleBar+QString(" - ")+CurrentAppVersion;

    // Some other init
    LastLogMessageTime=QTime::currentTime();
    EventReceiver=this; // Connect Event Receiver so now we accept LOG messages
    ui->StatusBar_SlideNumber->setText(QApplication::translate("MainWindow","Slide : ")+"0 / 0");
    s_Event_ToolbarChanged(0);
    ToStatusBar("");
    SetModifyFlag(false);           // Setup title window and do first RefreshControls();
    s_Event_ClipboardChanged();     // Setup clipboard button state

    if (ApplicationConfig->CheckConfigAtStartup) QTimer::singleShot(LATENCY,this,SLOT(s_Action_DlgCheckConfig())); else {
        QString Status;
        if (!Checkffmpeg(Status,ApplicationConfig)) QTimer::singleShot(LATENCY,this,SLOT(s_Action_DlgCheckConfig()));
    }
}

//====================================================================================================================

MainWindow::~MainWindow() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::~MainWindow");

    delete Diaporama;
    delete ApplicationConfig;
    SDLLastClose();
    delete ui;
    delete DriveList;
}

//====================================================================================================================

void MainWindow::customEvent(QEvent *event) {
    if (event->type()!=BaseAppEvent) QMainWindow::customEvent(event); else while (!EventList.isEmpty()) {
        QString Event      =EventList.takeFirst();
        int     EventType  =((QString)(Event.split("###;###")[0])).toInt();
        QString EventParam =Event.split("###;###")[1];

        if (EventType==EVENT_GeneralLogChanged) {
            //int     MessageType =((QString)EventParam.split("###:###")[0]).toInt();
            QString Message     =EventParam.split("###:###")[1];
            //QString EventSource =EventParam.split("###:###")[2];
            ToStatusBar(Message);
            LastLogMessageTime=QTime::currentTime();
            QTimer::singleShot(1000,this,SLOT(s_CleanStatusBar()));
        }
    }
}

//====================================================================================================================

void MainWindow::s_CleanStatusBar() {
    if (LastLogMessageTime.msecsTo(QTime::currentTime())>=500) ToStatusBar("");
    //LastLogMessageTime=QTime::currentTime();
}

//====================================================================================================================
// A REVOIR !!
void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::keyReleaseEvent");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::ToStatusBar");

    if (Text=="") {
        if (InternetBUILDVERSION!="") ui->StatusBar_General->setText(InternetBUILDVERSION); else ui->StatusBar_General->setText("");
    } else {
        ui->StatusBar_General->setText(Text);
    }
}

//====================================================================================================================

void MainWindow::SetTimelineHeight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::SetTimelineHeight");
    switch (ApplicationConfig->WindowDisplayMode) {
        case DISPLAYWINDOWMODE_PLAYER:
            ApplicationConfig->PartitionMode=false;
            ui->BrowserWidget->setVisible(false);
            ui->scrollArea->setVisible(true);
            ui->ToolBoxPartition->setVisible(false);
            ui->ToolBoxNormal->setVisible(true);
            ui->preview->setVisible(true);
            ui->preview2->setVisible(false);
            ui->TABTooltip->setVisible(true);
            ui->TABToolimg->setVisible(true);
            ui->PartitionBT->setDown(true);
            ui->PartitionBT->setEnabled(false);
            ui->Partition2BT->setEnabled(true);
            ui->Partition3BT->setEnabled(true);
            QApplication::processEvents();          // Give time to Qt to redefine position of each control and preview height !
            ui->preview->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview->height()-32));
            break;
        case DISPLAYWINDOWMODE_PARTITION:
            ApplicationConfig->PartitionMode=true;
            ui->BrowserWidget->setVisible(false);
            ui->scrollArea->setVisible(false);
            ui->ToolBoxPartition->setVisible(true);
            ui->ToolBoxNormal->setVisible(false);
            ui->preview->setVisible(false);
            ui->preview2->setVisible(true);
            ui->preview2->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview2->height()-32));
            ui->TABTooltip->setVisible(false);
            ui->TABToolimg->setVisible(false);
            ui->Partition2BT->setDown(true);
            ui->PartitionBT->setEnabled(true);
            ui->Partition2BT->setEnabled(false);
            ui->Partition3BT->setEnabled(true);
            break;
        case DISPLAYWINDOWMODE_BROWSER:
            ApplicationConfig->PartitionMode=false;
            ui->BrowserWidget->setVisible(true);
            ui->scrollArea->setVisible(false);
            ui->ToolBoxPartition->setVisible(true);
            ui->ToolBoxNormal->setVisible(false);
            ui->preview->setVisible(false);
            ui->preview2->setVisible(true);
            ui->preview2->setFixedWidth(Diaporama->GetWidthForHeight(ui->preview2->height()-32));
            ui->TABTooltip->setVisible(false);
            ui->TABToolimg->setVisible(false);
            ui->Partition3BT->setDown(true);
            ui->PartitionBT->setEnabled(true);
            ui->Partition2BT->setEnabled(true);
            ui->Partition3BT->setEnabled(false);
            break;
    }

    QApplication::processEvents();          // Give time to Qt to redefine position of each control and redraw timeline
    ui->timeline->SetTimelineHeight(ApplicationConfig->PartitionMode);
    QApplication::processEvents();          // Give time to Qt to redefine position of each control and redraw timeline
}

//====================================================================================================================

void MainWindow::closeEvent(QCloseEvent *Event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::closeEvent");

    ui->preview->SetPlayerToPause(); // Ensure player is stop
    ui->preview2->SetPlayerToPause(); // Ensure player is stop
    if (Diaporama->IsModify) {
        int Bt=CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Close application"),QApplication::translate("MainWindow","Want to save the project before closing?"),
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
    ApplicationConfig->BrowserWidgetSplitter=QString(QByteArray(ui->BrowserWidget->saveState()).toHex());
    ApplicationConfig->MainWinWSP->SaveWindowState(this);
    ApplicationConfig->SaveConfigurationFile();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::resizeEvent(QResizeEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::resizeEvent");

    ui->preview->SetPlayerToPause();  // Ensure player is stop
    ui->preview2->SetPlayerToPause(); // Ensure player is stop
    SetTimelineHeight();
}

//====================================================================================================================

void MainWindow::showEvent(QShowEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::showEvent");

    if (!IsFirstInitDone) {
        IsFirstInitDone=true;                                   // do this only one time
        ui->BrowserWidget->restoreState(QByteArray::fromHex(ApplicationConfig->BrowserWidgetSplitter.toUtf8()));
        ApplicationConfig->MainWinWSP->ApplyToWindow(this);     // Restore window position
        if (ApplicationConfig->MainWinWSP->IsMaximized) QTimer::singleShot(LATENCY,this,SLOT(DoMaximized()));
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::DoMaximized");

    showMaximized();
}

//====================================================================================================================
// Function use when reading BUILDVERSION from WEB Site
//====================================================================================================================

void MainWindow::s_Event_NetworkReply(QNetworkReply* reply) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_NetworkReply");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::OpenHelp");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::RefreshControls");

    // Timeline actions
    ui->ActionRemove_BT->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionRemove_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionRemove->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionEdit_BT->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionEdit_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ZoomMinusBT->setEnabled((ui->timeline->NbrItem()>0)&&(ApplicationConfig->TimelineHeight>TIMELINEMINHEIGH));
    ui->ZoomPlusBT->setEnabled((ui->timeline->NbrItem()>0)&&(ApplicationConfig->TimelineHeight<TIMELINEMAXHEIGH));

    // File menu
    ui->Action_Save_BT->setEnabled(Diaporama->IsModify);
    ui->Action_Save_BT_2->setEnabled(Diaporama->IsModify);
    ui->ActionSave_as_BT->setEnabled(Diaporama->List.count()>0);
    ui->ActionSave_as_BT_2->setEnabled(Diaporama->List.count()>0);
    ui->Action_OpenRecent_BT->setEnabled(ApplicationConfig->RecentFile.count()>0);
    ui->Action_OpenRecent_BT_2->setEnabled(ApplicationConfig->RecentFile.count()>0);

    // Project menu
    ui->actionEdit_background->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_object->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_object_in_transition->setEnabled(ui->timeline->NbrItem()>0);
    ui->actionEdit_music->setEnabled(ui->timeline->NbrItem()>0);

    // Clipboard_Object
    ui->ActionCopy_BT->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->ActionCopy_BT_2->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->actionCopy->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->ActionCut_BT->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->ActionCut_BT_2->setEnabled(ui->timeline->CurrentSelected()>=0);
    ui->actionCut->setEnabled(ui->timeline->CurrentSelected()>=0);

    // Render menu
    ui->ActionRender_BT->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionRender_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionSmartphone_BT->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionSmartphone_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionMultimedia_BT->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionMultimedia_BT_2->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionForTheWEB_BT->setEnabled(ui->timeline->NbrItem()>0);
    ui->ActionForTheWEB_BT_2->setEnabled(ui->timeline->NbrItem()>0);

    ui->ActionLossLess_BT->setEnabled((ui->timeline->NbrItem()>0)&&(AUDIOCODECDEF[7].IsFind)&&(VIDEOCODECDEF[8].IsFind)&&(FORMATDEF[2].IsFind));
    ui->ActionLossLess_BT_2->setEnabled((ui->timeline->NbrItem()>0)&&(AUDIOCODECDEF[7].IsFind)&&(VIDEOCODECDEF[8].IsFind)&&(FORMATDEF[2].IsFind));

    // Browser
    ui->PreviousFolderBt->setEnabled(ui->FolderTable->CanBrowseToPreviousPath());
    ui->UpFolderBt->setEnabled(ui->FolderTable->CanBrowseToUpperPath());

    // StatusBar
    ui->StatusBar_SlideNumber->setText(QApplication::translate("MainWindow","Slide : ")+QString("%1").arg(Diaporama->CurrentCol+1)+" / "+QString("%1").arg(Diaporama->List.count()));
}

//====================================================================================================================

void MainWindow::SetModifyFlag(bool IsModify) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::SetModifyFlag");

    Diaporama->IsModify=IsModify;
    this->setWindowTitle(TitleBar+QString("-")+
                         (Diaporama->ProjectFileName!=""?Diaporama->ProjectFileName:QApplication::translate("MainWindow","<new project>","when project have no name define"))+
                         (Diaporama->IsModify?"*":""));
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_Event_SetModifyFlag() {
    SetModifyFlag(true);
}

//====================================================================================================================

void MainWindow::SetTimelineCurrentCell(int Cell) {
    ui->timeline->SetCurrentCell(Cell);
}

//====================================================================================================================

void MainWindow::s_Action_About() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_About");

    ui->Action_About_BT->setDown(false);
    ui->Action_About_BT_2->setDown(false);
    DlgAbout Dlg("",ApplicationConfig,ApplicationConfig->DlgAboutWSP,this);
    Dlg.InitDialog();
    Dlg.exec();
}

//====================================================================================================================

void MainWindow::s_Action_DlgCheckConfig() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_DlgCheckConfig");

    DlgCheckConfig Dlg(HELPFILE_DlgCheckConfig,ApplicationConfig,ApplicationConfig->DlgCheckConfigWSP,this);
    Dlg.InitDialog();
    Dlg.exec();

    QString Status;
    if (!Checkffmpeg(Status,ApplicationConfig)) {
        CustomMessageBox(this,QMessageBox::Critical,APPLICATION_NAME,QApplication::translate("MainWindow","Configuration not correct!"));
        close();
    }
}

//====================================================================================================================

void MainWindow::s_Action_Documentation() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_Documentation");

    ui->ActionDocumentation_BT->setDown(false);
    ui->ActionDocumentation_BT_2->setDown(false);
    QDesktopServices::openUrl(QUrl(QString(HELPFILE_SUPPORT).replace("<local>",ApplicationConfig->GetValideWEBLanguage(ApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void MainWindow::s_Action_NewFunctions() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_NewFunctions");

    ui->ActionNewFunctions_BT->setDown(false);
    ui->ActionNewFunctions_BT_2->setDown(false);
    QDesktopServices::openUrl(QUrl(QString(HELPFILE_NEWS).replace("<local>",ApplicationConfig->GetValideWEBLanguage(ApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void MainWindow::s_Action_Exit() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_Exit");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_Exit()));
        return;
    }

    close();
}

//====================================================================================================================

void MainWindow::s_Action_ZoomPlus() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_ZoomPlus");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_ZoomPlus()));
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_ZoomMinus");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_ZoomMinus()));
        return;
    }

    if ((ui->timeline->rowHeight(0)-ApplicationConfig->TimelineHeight/2-TIMELINESOUNDHEIGHT*2)>TIMELINEMINHEIGH) {
        ApplicationConfig->TimelineHeight-=20;
        SetTimelineHeight();
    }
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_Action_ChWindowDisplayMode_ToPlayerMode() {
    s_Action_ChWindowDisplayMode(DISPLAYWINDOWMODE_PLAYER);
}

//====================================================================================================================

void MainWindow::s_Action_ChWindowDisplayMode_ToPartitionMode() {
    s_Action_ChWindowDisplayMode(DISPLAYWINDOWMODE_PARTITION);
}

//====================================================================================================================

void MainWindow::s_Action_ChWindowDisplayMode_ToBrowserMode() {
    s_Action_ChWindowDisplayMode(DISPLAYWINDOWMODE_BROWSER);
}

//====================================================================================================================

void MainWindow::s_Action_ChWindowDisplayMode(int Mode) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_ChWindowDisplayMode");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_ChWindowDisplayMode(Mode)));
        return;
    }

    int Selected=ui->timeline->CurrentSelected(); // Save current seleted item

    ApplicationConfig->WindowDisplayMode=Mode;
    SetTimelineHeight();

    // Re-select previous current seleted item
    if ((Selected>=0)&&(Selected<ui->timeline->NbrItem())) ui->timeline->SetCurrentCell(Selected);
    (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
    (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->Resize();
}

//====================================================================================================================
// Double click on widget in the object track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_DoubleClickedOnObject");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Event_DoubleClickedOnObject()));
        return;
    }
    if (Diaporama->List.count()==0) return;

    bool DoneAgain=true;
    while (DoneAgain) {
        DoneAgain=false;
        DlgSlideProperties Dlg(Diaporama->List[Diaporama->CurrentCol],HELPFILE_DlgSlideProperties,ApplicationConfig,ApplicationConfig->DlgSlidePropertiesWSP,this);
        Dlg.InitDialog();
        connect(&Dlg,SIGNAL(SetModifyFlag()),this,SLOT(s_Event_SetModifyFlag()));
        int Ret=Dlg.exec();
        if (Ret!=1) {
            SetModifyFlag(true);
            if (Diaporama->List[Diaporama->CurrentCol]->Thumbnail) {
                delete Diaporama->List[Diaporama->CurrentCol]->Thumbnail;
                Diaporama->List[Diaporama->CurrentCol]->Thumbnail=NULL;
            }
            (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol)-1);
            AdjustRuller();
        }
        if ((Ret==2)||(Ret==3)) {
            Diaporama->CurrentCol=Diaporama->CurrentCol+((Ret==2)?-1:1);
            SetTimelineCurrentCell(Diaporama->CurrentCol);

            // Update slider mark
            if (Diaporama->List.count()>0)
                (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SetStartEndPos(
                        Diaporama->GetObjectStartPosition(Diaporama->CurrentCol),                                                               // Current slide
                        Diaporama->List[Diaporama->CurrentCol]->GetDuration(),
                        (Diaporama->CurrentCol>0)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol-1):((Diaporama->CurrentCol==0)?0:-1), // Previous slide
                        (Diaporama->CurrentCol>0)?Diaporama->List[Diaporama->CurrentCol-1]->GetDuration():((Diaporama->CurrentCol==0)?Diaporama->GetTransitionDuration(Diaporama->CurrentCol):0),
                        Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->GetObjectStartPosition(Diaporama->CurrentCol+1):-1,        // Next slide
                        Diaporama->CurrentCol<(Diaporama->List.count()-1)?Diaporama->List[Diaporama->CurrentCol+1]->GetDuration():0);
            else (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SetStartEndPos(0,0,-1,0,-1,0);
            // open dialog again
            DoneAgain=true;
        }
    }
}

//====================================================================================================================
// Double click on transition part of widget in the object track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnTransition() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_DoubleClickedOnTransition");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Event_DoubleClickedOnTransition()));
        return;
    }

    DlgTransitionProperties Dlg(Diaporama->List[Diaporama->CurrentCol],HELPFILE_DlgTransitionProperties,ApplicationConfig,ApplicationConfig->DlgTransitionPropertiesWSP,this);
    Dlg.InitDialog();
    int Ret=Dlg.exec();
    if (Ret==0) {
        SetModifyFlag(true);
        (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================
// Double click on sound part of widget in the object track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnVideoSound() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_DoubleClickedOnVideoSound");

    s_Event_DoubleClickedOnObject();  // No separated process at this time !
}

//====================================================================================================================
// // Double click on widget in the background track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnBackground() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_DoubleClickedOnBackground");

    if (Diaporama->CurrentCol>=Diaporama->List.count()) return;
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Event_DoubleClickedOnBackground()));
        return;
    }
    DlgBackgroundProperties Dlg(Diaporama->List[Diaporama->CurrentCol],HELPFILE_DlgBackgroundProperties,ApplicationConfig,ApplicationConfig->DlgBackgroundPropertiesWSP,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(RefreshDisplay()),this,SLOT(s_Event_RefreshDisplay()));
    if (Dlg.exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================

void MainWindow::s_Event_RefreshDisplay() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_RefreshDisplay");
    (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
}

//====================================================================================================================
// // Double click on widget in the music track
//====================================================================================================================

void MainWindow::s_Event_DoubleClickedOnMusic() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_DoubleClickedOnMusic");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Event_DoubleClickedOnMusic()));
        return;
    }

    DlgMusicProperties Dlg(Diaporama->List[Diaporama->CurrentCol],HELPFILE_DlgMusicProperties,ApplicationConfig,ApplicationConfig->DlgMusicPropertiesWSP,this);
    Dlg.InitDialog();
    connect(&Dlg,SIGNAL(SetModifyFlag()),this,SLOT(s_Event_SetModifyFlag()));
    if (Dlg.exec()==0) {
        SetModifyFlag(true);
        (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->GetObjectStartPosition(Diaporama->CurrentCol)+Diaporama->GetTransitionDuration(Diaporama->CurrentCol));
        AdjustRuller();
    }
}

//====================================================================================================================

void MainWindow::s_Event_TimelineDragMoveItem() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_TimelineDragMoveItem");

    if (DragItemSource<DragItemDest) DragItemDest--;
    Diaporama->List.move(DragItemSource,DragItemDest);
    SetModifyFlag(true);
    ui->timeline->SetCurrentCell(DragItemDest);
}

//====================================================================================================================
// Current diaporama object selection changed
//====================================================================================================================

void MainWindow::s_Event_TimelineSelectionChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_TimelineSelectionChanged");

    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Event_TimelineSelectionChanged()));
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
                (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(0);
                (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SetStartEndPos(0,0,-1,0,-1,0);
            }
        }
        RefreshControls();
    }
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================
// Update dock information
//====================================================================================================================

void MainWindow::s_Action_OpenTABHelpLink(const QString Link) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_OpenTABHelpLink");

    OpenHelp(Link);
}

void MainWindow::s_Event_ToolbarChanged(int MenuIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_ToolbarChanged");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QString Html;
    switch (MenuIndex) {
    case 0: Html=QApplication::translate("MainWindow","<html><body>Select a project to open or to create a new project<br>"\
                                         "To discover ffDiaporama:<br><a href=\"%1\">Consult the WIKI</a></body></html>").arg(HELPFILE_WIKIINDEX);
            break;
    case 1: Html=QApplication::translate("MainWindow","<html><body>Add empty slides or slides based on photos or videos<br>"\
                                         "To discover how to build your slide show and to animate slides:<br><a href=\"%1\">Discover the principles of functioning of ffDiaporama</a></body></html>").arg(HELPFILE_PRINCIPLESINDEX);
            break;
    case 2: Html=QApplication::translate("MainWindow","<html><body>Select the equipment type that you plan to use for your video<br>"\
                                         "To discover how to render videos:<br><a href=\"%1\">Consult the rendering videos WIKI page</a></body></html>").arg(HELPFILE_RENDERINDEX);
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_RenderVideo");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_RenderVideo()));
        return;
    }
    ui->ActionRender_BT->setDown(false);
    ui->ActionRender_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo Dlg(*Diaporama,EXPORTMODE_ADVANCED,HELPFILE_DlgRenderVideo,ApplicationConfig,ApplicationConfig->DlgRenderVideoWSP,this);
    connect(&Dlg,SIGNAL(SetModifyFlag()),this,SLOT(s_Event_SetModifyFlag()));
    Dlg.InitDialog();
    Dlg.exec();
    AdjustRuller();
}

void MainWindow::s_Action_RenderSmartphone() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_RenderSmartphone");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_RenderSmartphone()));
        return;
    }
    ui->ActionSmartphone_BT->setDown(false);
    ui->ActionSmartphone_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo Dlg(*Diaporama,MODE_SMARTPHONE,HELPFILE_DlgRenderVideo,ApplicationConfig,ApplicationConfig->DlgRenderVideoWSP,this);
    connect(&Dlg,SIGNAL(SetModifyFlag()),this,SLOT(s_Event_SetModifyFlag()));
    Dlg.InitDialog();
    Dlg.exec();
    AdjustRuller();
}

void MainWindow::s_Action_RenderMultimedia() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_RenderMultimedia");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_RenderMultimedia()));
        return;
    }
    ui->ActionMultimedia_BT->setDown(false);
    ui->ActionMultimedia_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo Dlg(*Diaporama,MODE_MULTIMEDIASYS,HELPFILE_DlgRenderVideo,ApplicationConfig,ApplicationConfig->DlgRenderVideoWSP,this);
    connect(&Dlg,SIGNAL(SetModifyFlag()),this,SLOT(s_Event_SetModifyFlag()));
    Dlg.InitDialog();
    Dlg.exec();
    AdjustRuller();
}

void MainWindow::s_Action_RenderForTheWEB() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_RenderForTheWEB");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_RenderForTheWEB()));
        return;
    }
    ui->ActionForTheWEB_BT->setDown(false);
    ui->ActionForTheWEB_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo Dlg(*Diaporama,MODE_FORTHEWEB,HELPFILE_DlgRenderVideo,ApplicationConfig,ApplicationConfig->DlgRenderVideoWSP,this);
    connect(&Dlg,SIGNAL(SetModifyFlag()),this,SLOT(s_Event_SetModifyFlag()));
    Dlg.InitDialog();
    Dlg.exec();
    AdjustRuller();
}

void MainWindow::s_Action_RenderLossLess() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_RenderLossLess");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_RenderForTheWEB()));
        return;
    }
    ui->ActionLossLess_BT->setDown(false);
    ui->ActionLossLess_BT_2->setDown(false);

    if (Diaporama->IsModify) Diaporama->UpdateChapterInformation();
    DlgRenderVideo Dlg(*Diaporama,MODE_LOSSLESS,HELPFILE_DlgRenderVideo,ApplicationConfig,ApplicationConfig->DlgRenderVideoWSP,this);
    connect(&Dlg,SIGNAL(SetModifyFlag()),this,SLOT(s_Event_SetModifyFlag()));
    Dlg.InitDialog();
    Dlg.exec();
    AdjustRuller();
}

//====================================================================================================================
// Project properties
//====================================================================================================================

void MainWindow::s_Action_ProjectProperties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_ProjectProperties");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_ProjectProperties()));
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_ChangeApplicationSettings");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_ChangeApplicationSettings()));
        return;
    }
    ui->ActionConfiguration_BT->setDown(false);
    ui->ActionConfiguration_BT_2->setDown(false);

    DlgApplicationSettings Dlg(HELPFILE_DlgApplicationSettings,ApplicationConfig,ApplicationConfig->DlgApplicationSettingsWSP,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) {
        ToStatusBar(QApplication::translate("MainWindow","Saving configuration file and applying new configuration ..."));
        QTimer::singleShot(LATENCY,this,SLOT(DoChangeApplicationSettings()));
        ApplicationConfig->ImagesCache.MaxValue=ApplicationConfig->MemCacheMaxValue;
    }
}

void MainWindow::DoChangeApplicationSettings() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::DoChangeApplicationSettings");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_New");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_New()));
        return;
    }
    ui->Action_New_BT->setDown(false);
    ui->Action_New_BT_2->setDown(false);

    if ((Diaporama->IsModify)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","New project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_Action_Save();

    while (ApplicationConfig->ImagesCache.List.count()>0) delete ApplicationConfig->ImagesCache.List.takeLast();
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_OpenRecent");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_OpenRecent()));
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

    if ((Diaporama->IsModify)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Open project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_Action_Save();

    if (Selected!="") {
        ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(Selected).fileName());
        FileForIO=Selected;
        QTimer::singleShot(LATENCY,this,SLOT(DoOpenFile()));
    }
}

void MainWindow::s_Action_Open() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_Open");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_Open()));
        return;
    }
    ui->Action_Open_BT->setDown(false);
    ui->Action_Open_BT_2->setDown(false);

    if ((Diaporama->IsModify)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Open project"),QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) s_Action_Save();

    QString ProjectFileName=QFileDialog::getOpenFileName(this,QApplication::translate("MainWindow","Open project"),ApplicationConfig->LastProjectPath,QString("ffDiaporama (*.ffd)"));
    if (ProjectFileName!="") {
        ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(ProjectFileName).fileName());
        FileForIO=ProjectFileName;
        QTimer::singleShot(LATENCY,this,SLOT(DoOpenFile()));
    }
}

void MainWindow::DoOpenFileParam() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::DoOpenFileParam");

    ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(FileForIO).fileName());
    QTimer::singleShot(LATENCY,this,SLOT(DoOpenFile()));
}

void MainWindow::DoOpenFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::DoOpenFile");

    QString ProjectFileName=AdjustDirForOS(FileForIO);

    // Check if ffDRevision is not > current ffDRevision
    cffDProjectFile File(ApplicationConfig);
    if (File.GetInformationFromFile(ProjectFileName,NULL,NULL)) {
        File.GetFullInformationFromFile();
        if ((File.ffDRevision.toInt()>CurrentAppVersion.toInt())&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Open project"),
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
    if (Diaporama->List.count()>0) (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->GetTransitionDuration(0));
        else (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(0);

    ApplicationConfig->RecentFile.append(ProjectFileName);
    while (ApplicationConfig->RecentFile.count()>10) ApplicationConfig->RecentFile.takeFirst();
    ui->timeline->SetCurrentCell(0);
    RefreshControls();
    SetTimelineHeight();
    QApplication::restoreOverrideCursor();
    ToStatusBar("");
}

//====================================================================================================================
// Save current project
//====================================================================================================================

void MainWindow::s_Action_Save() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_Save");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::DoSaveFile");

    if (Diaporama->SaveFile(this)) SetModifyFlag(false);
    ToStatusBar("");
}

//====================================================================================================================
// Save current project as
//====================================================================================================================

void MainWindow::s_Action_SaveAs() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_SaveAs");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_AddTitle");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_AddTitle()));
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_AddFile");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_AddFile()));
        return;
    }
    ui->ActionAdd_BT->setDown(false);
    ui->ActionAdd_BT_2->setDown(false);

    FileList=QFileDialog::getOpenFileNames(this,QApplication::translate("MainWindow","Add files"),
                                                ApplicationConfig->RememberLastDirectories?ApplicationConfig->LastMediaPath:"",
                                                ApplicationConfig->GetFilterForMediaFile(cBaseApplicationConfig::ALLFILE),
                                                0,0);
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

        SortFileList();

        DlgWorkingTaskDialog=new DlgWorkingTask(QApplication::translate("MainWindow","Add file to project"),&CancelAction,ApplicationConfig,this);
        DlgWorkingTaskDialog->InitDialog();
        DlgWorkingTaskDialog->SetMaxValue(FileList.count());
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_DoAddFile()));
        DlgWorkingTaskDialog->exec();
    }
}

//====================================================================================================================

void MainWindow::s_Event_TimelineAddDragAndDropFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_TimelineAddDragAndDropFile");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Event_TimelineAddDragAndDropFile()));
        return;
    }

    SavedCurIndex =DragItemDest>0?DragItemDest-1:0;
    CurIndex      =DragItemDest;

    SortFileList();

    DlgWorkingTaskDialog=new DlgWorkingTask(QApplication::translate("MainWindow","Add file to project"),&CancelAction,ApplicationConfig,this);
    DlgWorkingTaskDialog->InitDialog();
    DlgWorkingTaskDialog->SetMaxValue(FileList.count());
    QTimer::singleShot(LATENCY,this,SLOT(s_Action_DoAddFile()));
    DlgWorkingTaskDialog->exec();
}

//====================================================================================================================

void MainWindow::s_Action_DoAddFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_DoAddFile");

    if ((FileList.count()==0)||(CancelAction)) {
        if (DlgWorkingTaskDialog) DlgWorkingTaskDialog->close();
        delete DlgWorkingTaskDialog;
        DlgWorkingTaskDialog=NULL;
        FileList.clear();
        return;
    }

    if (DlgWorkingTaskDialog) {
        DlgWorkingTaskDialog->DisplayText(QApplication::translate("MainWindow","Add file to project :")+QFileInfo(FileList[0]).fileName());
        DlgWorkingTaskDialog->DisplayProgress(DlgWorkingTaskDialog->MaxValue-FileList.count());
    }

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
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),NewFile,QMessageBox::Close);
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
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
            if (MediaFile) delete MediaFile;
            QApplication::restoreOverrideCursor();
            return;
        }

        if ((IsValide)&&(MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)&&(((cVideoFile *)MediaFile)->AudioStreamNumber!=-1)&&(((cVideoFile *)MediaFile)->ffmpegAudioFile->streams[((cVideoFile *)MediaFile)->AudioStreamNumber]->codec->channels>2)) {
            ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only mono or stereo audio track","Error message");
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
            if (MediaFile) delete MediaFile;
            QApplication::restoreOverrideCursor();
            return;
        }

        if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

        //**********************************************
        // Chapter management
        //**********************************************

        if ((ChapterNum==-1)&&(MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)&&(((cVideoFile *)MediaFile)->NbrChapters>1)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("cBaseMediaFile","Add video file"),
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
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),NewFile,QMessageBox::Close);
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
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_DoAddFile()));
    } else {
        ToStatusBar("");
        if (DlgWorkingTaskDialog) DlgWorkingTaskDialog->close();
        delete DlgWorkingTaskDialog;
        DlgWorkingTaskDialog=NULL;
    }
}

//====================================================================================================================
// Add an (sub) project
//====================================================================================================================

void MainWindow::s_Action_AddProject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_AddProject");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_AddProject()));
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
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_DoAddFile()));
    }
}

//====================================================================================================================

void MainWindow::s_Event_SaveImageEvent() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_SaveImageEvent");
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Event_SaveImageEvent()));
        return;
    }
    QStringList Size;
    QMenu *ContextMenu=new QMenu(this);
    for (int i=0;i<NBR_SIZEDEF;i++)
        Size.append(QString("%1x%2").arg(DefImageFormat[0][ApplicationConfig->ImageGeometry][i].Width).arg(Diaporama->GetHeightForWidth(DefImageFormat[0][ApplicationConfig->ImageGeometry][i].Width)));

    // Sort list
    for (int i=0;i<Size.count();i++) {
        for (int j=0;j<Size.count()-1;j++) {
            int a=Size[j].left(Size[j].indexOf("x")).toInt();
            int b=Size[j+1].left(Size[j+1].indexOf("x")).toInt();
            if (a>b) Size.swap(j,j+1);
        }
    }

    for (int i=0;i<Size.count();i++) {
        QAction *UpdateAction=new QAction(QApplication::translate("MainWindow","Capture the image ")+Size[i],this);
        UpdateAction->setFont(QFont("Sans Serif",9));
        ContextMenu->addAction(UpdateAction);
    }
    QAction *Ret=ContextMenu->exec(QCursor::pos());
    if (Ret!=NULL) {
        QString Format=Ret->text().mid(QApplication::translate("MainWindow","Capture the image ").length());
        int Width =Format.left(Format.indexOf("x")).toInt();
        int Height=Format.mid(Format.indexOf("x")+1).toInt();
        QString OutputFileName=ApplicationConfig->LastCaptureImage;
        QString Filter="JPG (*.jpg)";
        if (!OutputFileName.endsWith(QDir::separator())) OutputFileName=OutputFileName+QDir::separator();
        OutputFileName=OutputFileName+QApplication::translate("MainWindow","Capture image");
        OutputFileName=QFileDialog::getSaveFileName(this,QApplication::translate("MainWindow","Select destination file"),OutputFileName,"PNG (*.png);;JPG (*.jpg)",&Filter);
        if (OutputFileName!="") {
            if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastCaptureImage=QFileInfo(OutputFileName).absolutePath();     // Keep folder for next use
            if ((Filter.toLower().indexOf("png")!=-1)&&(!OutputFileName.endsWith(".png"))) OutputFileName=OutputFileName+".png";
            if ((Filter.toLower().indexOf("jpg")!=-1)&&(!OutputFileName.endsWith(".jpg"))) OutputFileName=OutputFileName+".jpg";
            cDiaporamaObjectInfo *Frame=new cDiaporamaObjectInfo(NULL,Diaporama->CurrentPosition,Diaporama,1);
            Diaporama->LoadSources(Frame,double(Height)/double(1080),Width,Height,false,0);
            Diaporama->DoAssembly(Frame,Width,Height);
            Frame->RenderedImage->save(OutputFileName,0,100);
            delete Frame;
        }
    }
    delete ContextMenu;
}

//====================================================================================================================

void MainWindow::s_Event_ContextualMenu(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_EditObject");
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Event_ContextualMenu()));
        return;
    }

    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionAddTitle);
    ContextMenu->addAction(ui->actionAddFiles);
    ContextMenu->addAction(ui->actionAddProject);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionEdit_background);
    ContextMenu->addAction(ui->actionEdit_object);
    ContextMenu->addAction(ui->actionEdit_music);
    ContextMenu->addAction(ui->actionEdit_object_in_transition);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionCut);
    ContextMenu->addAction(ui->actionCopy);
    ContextMenu->addAction(ui->actionPaste);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionRemove);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
}

//====================================================================================================================

void MainWindow::s_Action_EditObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_EditObject");
    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_EditObject()));
        return;
    }

    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(ui->actionEdit_background);
    ContextMenu->addAction(ui->actionEdit_object);
    ContextMenu->addAction(ui->actionEdit_music);
    ContextMenu->addAction(ui->actionEdit_object_in_transition);
    //ContextMenu->addAction(ui->actionEdit_background_transition);
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
    ui->ActionEdit_BT->setDown(false);
    ui->ActionEdit_BT_2->setDown(false);
}

//====================================================================================================================

void MainWindow::s_Action_RemoveObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_RemoveObject");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_RemoveObject()));
        return;
    }
    ui->ActionRemove_BT->setDown(false);
    ui->ActionRemove_BT_2->setDown(false);

    int Current=ui->timeline->CurrentSelected();
    if ((Current<0)||(Current>=Diaporama->List.count())) return;

    if ((ApplicationConfig->AskUserToRemove)&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Remove slide"),QApplication::translate("MainWindow","Are you sure to want to delete this slide?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->timeline->setUpdatesEnabled(false);
    FLAGSTOPITEMSELECTION=true;
    delete Diaporama->List.takeAt(Current);
    if (Current>=Diaporama->List.count()) Current=Diaporama->List.count()-1;
    ui->timeline->ResetDisplay(Current);    // FLAGSTOPITEMSELECTION is set to false by ResetDisplay
    (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->GetObjectStartPosition(Current)+Diaporama->GetTransitionDuration(Current));
    SetModifyFlag(true);
    AdjustRuller();
    ui->timeline->setUpdatesEnabled(true);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::s_Action_CutToClipboard() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_CutToClipboard");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_CutToClipboard()));
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_CopyToClipboard");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_CopyToClipboard()));
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Action_PasteFromClipboard");

    ui->preview->SetPlayerToPause();    // Ensure player is stop
    ui->preview2->SetPlayerToPause();   // Ensure player is stop
    if (InPlayerUpdate) {               // Resend message and quit if player have not finish to update it's display
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_PasteFromClipboard()));
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Event_ClipboardChanged");

    ui->ActionPaste_BT->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/slide")));
    ui->ActionPaste_BT_2->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/slide")));
    ui->actionPaste->setEnabled((QApplication::clipboard())&&(QApplication::clipboard()->mimeData())&&(QApplication::clipboard()->mimeData()->hasFormat("ffDiaporama/slide")));
}

//====================================================================================================================

void MainWindow::AddObjectToTimeLine(int CurIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::AddObjectToTimeLine");

    ui->timeline->AddObjectToTimeLine(CurIndex);
}

//====================================================================================================================
// Adjust preview ruller depending on current Disporama Currentcol
//====================================================================================================================

void MainWindow::AdjustRuller() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::AdjustRuller");

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
    (ApplicationConfig->WindowDisplayMode==DISPLAYWINDOWMODE_PLAYER?ui->preview:ui->preview2)->SeekPlayer(Diaporama->CurrentPosition);
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_Browser_FloderTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_FloderTreeItemChanged");

    ApplicationConfig->CurrentPath=ui->FolderTree->GetFolderPath(current,false);
    QList<cBaseMediaFile*> EmptyList;
    ui->FileInfoLabel->SetupFileInfoLabel(EmptyList);

    ui->FolderTree->RefreshItemByPath(ui->FolderTree->GetFolderPath(current,true),false);
    DoBrowserRefreshFolderInfo();

    ui->CurrentPathED->setText(ApplicationConfig->CurrentPath);
    ui->FolderIcon->setPixmap(DriveList->GetFolderIcon(ApplicationConfig->CurrentPath).pixmap(16,16));

    QString Path=ApplicationConfig->CurrentPath;
    #ifdef Q_OS_WIN
        Path.replace("%HOMEDRIVE%%HOMEPATH%",DriveList->List[0].Path,Qt::CaseInsensitive);
        Path.replace("%USERPROFILE%",DriveList->List[0].Path,Qt::CaseInsensitive);
        Path=AdjustDirForOS(Path);
        if (QDir(Path).canonicalPath()!="") Path=QDir(Path).canonicalPath(); // Resolved eventual .lnk files
    #endif
    ui->FolderTable->FillListFolder(Path);
    RefreshControls();
}

//====================================================================================================================

void MainWindow::DoBrowserRefreshFolderInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_FloderTreeItemChanged");
    DriveList->UpdateDriveList();   // To update free space on drive
    cDriveDesc *HDD=ui->FolderTree->SearchRealDrive(ApplicationConfig->CurrentPath);
    if (HDD) {
        // If scan in progress
        if (ui->FolderTable->ScanMediaListProgress) {
            ui->HDDSizePgr->setMaximum(0);
            ui->HDDSizePgr->setValue(0);
            ui->HDDSizePgr->setFormat("%P%");
            ui->HDDSizePgr->setAlignment(Qt::AlignHCenter);
            ui->FolderInfoLabel->setText("");

        // If scan is finished
        } else {
            // Ensure Used and Size fit in an _int32 value for QProgressBar
            qlonglong Used=HDD->Used,Size=HDD->Size;
            while (Used>1024*1024) { Used=Used/1024; Size=Size/1024; }
            ui->HDDSizePgr->setMaximum(Size);
            ui->HDDSizePgr->setValue(Used);
            ui->HDDSizePgr->setFormat(GetTextSize(HDD->Used)+"/"+GetTextSize(HDD->Size));
            ui->HDDSizePgr->setAlignment(Qt::AlignHCenter);
        }
        QString ToDisplay=QString("%1/%2").arg(ui->FolderTable->CurrentShowFilesNumber).arg(ui->FolderTable->CurrentTotalFilesNumber)+" "+QApplication::translate("MainWindow","files")+" - "+
                          QString("%1").arg(ui->FolderTable->CurrentShowFolderNumber)+" "+QApplication::translate("MainWindow","folders")+" - "+
                          QApplication::translate("MainWindow","Total size:")+QString("%1/%2").arg(GetTextSize(ui->FolderTable->CurrentShowFolderSize)).arg(GetTextSize(ui->FolderTable->CurrentTotalFolderSize));
        if (ui->FolderTable->CurrentShowDuration!=QTime(0,0,0,0)) ToDisplay=ToDisplay+" - "+QApplication::translate("MainWindow","Total duration:")+ui->FolderTable->CurrentShowDuration.toString("HH:mm:ss");
        ui->FolderInfoLabel->setText(ToDisplay);
    } else {
        ui->HDDSizePgr->setMaximum(0);
        ui->HDDSizePgr->setValue(0);
        ui->HDDSizePgr->setFormat("%P%");
        ui->HDDSizePgr->setAlignment(Qt::AlignHCenter);
        ui->FolderInfoLabel->setText("");
    }
}

//====================================================================================================================

void MainWindow::DoBrowserRefreshSelectedFileInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::DoBrowserRefreshSelectedFileInfo");

    if (ui->FileInfoLabel->isVisible()) {
        ui->FileInfoLabel->SetupFileInfoLabel(ui->FolderTable->GetCurrentSelectedMediaFile());
        ui->FileInfoLabel->repaint();
    }
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_Browser_RefreshAll() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_RefreshAll");

    ui->FolderTree->RefreshDriveList();
    s_Browser_FloderTreeItemChanged(ui->FolderTree->currentItem(),NULL);
}

//====================================================================================================================

void MainWindow::s_Browser_SetToPrevious() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_SetToPrevious");
    QString Path=ui->FolderTable->BrowseToPreviousPath();
    if (Path!="") ui->FolderTree->SetSelectItemByPath(Path);
}

//====================================================================================================================

void MainWindow::s_Browser_SetToUpper() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_SetToUpper");
    QString Path=ui->FolderTable->BrowseToUpperPath();
    if (Path!="") ui->FolderTree->SetSelectItemByPath(Path);
}

//====================================================================================================================

void MainWindow::s_Browser_Favorite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_Favorite");
    QMenu *ContextMenu=new QMenu(this);

    ContextMenu->addAction(ui->actionAddToFavorite);
    ContextMenu->addSeparator();
    for (int i=0;i<ApplicationConfig->BrowserFavorites.count();i++) {
        QStringList Texts=ApplicationConfig->BrowserFavorites[i].split("###");
        QAction *Action=new QAction(QIcon(":/img/favorite.png"),QString("%1 [%2]").arg(Texts[0]).arg(Texts[1]),this);
        Action->setIconVisibleInMenu(true);
        Action->setFont(QFont("Sans Serif",9));
        ContextMenu->addAction(Action);
    }
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionManageFavorite);
    QAction *Ret=ContextMenu->exec(QCursor::pos());
    if ((Ret!=NULL)&&(Ret!=ui->actionAddToFavorite)&&(Ret!=ui->actionManageFavorite)&&(Ret->text()!="")) {
        QStringList Texts=Ret->text().split(" [");
        ui->FolderTree->SetSelectItemByPath(Texts[1].left(Texts[1].length()-1));
    }
    delete ContextMenu;
    ui->ActionEdit_BT->setDown(false);
}

//====================================================================================================================

void MainWindow::s_Browser_AddToFavorite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_AddToFavorite");

    bool    Ok,Continue=true;
    QString Text=ApplicationConfig->CurrentPath;
    while (Text.indexOf(QDir::separator())!=-1) Text=Text.mid(Text.indexOf(QDir::separator())+1);
    while (Continue) {
        Continue=false;
        Text=CustomInputDialog(this,QApplication::translate("MainWindow","Add to favorite"),QApplication::translate("MainWindow","Favorite name:"),QLineEdit::Normal,Text,&Ok);
        if (Ok && !Text.isEmpty()) {

            int i=0;
            while ((i<ApplicationConfig->BrowserFavorites.count())&&(!ApplicationConfig->BrowserFavorites[i].startsWith(Text+"###"))) i++;
            if ((i<ApplicationConfig->BrowserFavorites.count())&&(ApplicationConfig->BrowserFavorites[i].startsWith(Text+"###"))) {
                if (CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Add to favorite"),
                                          QApplication::translate("MainWindow","A favorite with this name already exist.\nDo you want to overwrite-it ?"),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)
                    ApplicationConfig->BrowserFavorites[i]=Text+"###"+ApplicationConfig->CurrentPath;
                else Continue=true;

            } else {

                int i=0;
                while ((i<ApplicationConfig->BrowserFavorites.count())&&(!ApplicationConfig->BrowserFavorites[i].endsWith("###"+ApplicationConfig->CurrentPath))) i++;
                if ((i<ApplicationConfig->BrowserFavorites.count())&&(ApplicationConfig->BrowserFavorites[i].endsWith("###"+ApplicationConfig->CurrentPath))) {
                    if (CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Add to favorite"),
                                              QApplication::translate("MainWindow","A favorite with for this path already exist.\nDo you want to overwrite-it ?"),
                                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)
                        ApplicationConfig->BrowserFavorites[i]=Text+"###"+ApplicationConfig->CurrentPath;
                    else Continue=true;

                } else {
                    ApplicationConfig->BrowserFavorites.append(Text+"###"+ApplicationConfig->CurrentPath);
                }
            }
        }
    }
    ApplicationConfig->BrowserFavorites.sort();
}

//====================================================================================================================

void MainWindow::s_Browser_ManageFavorite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_ManageFavorite");
    DlgManageFavorite Dlg(&ApplicationConfig->BrowserFavorites,HELPFILE_DlgManageFavorite,ApplicationConfig,ApplicationConfig->DlgManageFavoriteWSP,this);
    Dlg.InitDialog();
    Dlg.exec();
}

//====================================================================================================================

bool MainWindow::s_Browser_InRemoveFolder(QString FolderPath) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_InRemoveFolder");

    QDir            Folder(FolderPath);
    QFileInfoList   Dirs=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Hidden);

    foreach(QFileInfo Dir,Dirs)
        if ((Dir.isDir())&&(Dir.absoluteFilePath()!=FolderPath)&&(Dir.fileName()!=".")&&(Dir.fileName()!=".."))
            if (!s_Browser_InRemoveFolder(Dir.absoluteFilePath())) return false;

    QFileInfoList  Files=Folder.entryInfoList(QDir::Dirs|QDir::AllEntries|QDir::Hidden);
    foreach(QFileInfo File,Files) if (!File.isDir()) if (!QFile(File.absoluteFilePath()).remove()) {
        ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Impossible to remove file %1 - error %2:%3").arg(File.fileName()).arg(errno).arg(QString().fromLocal8Bit(strerror(errno))));
        return false;
    }

    if (!QDir().rmdir(FolderPath)) {
        ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Impossible to remove folder %1 - error %2:%3").arg(FolderPath).arg(errno).arg(QString().fromLocal8Bit(strerror(errno))));
        return false;
    } else {
        ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Successfully remove folder (and all is content) %1").arg(FolderPath));
        return true;
    }
}

//====================================================================================================================

void MainWindow::s_Browser_RemoveFolder() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_RemoveFolder");

    QString FolderPath=ui->FolderTree->GetFolderPath(ui->FolderTree->currentItem(),false);
    #ifdef Q_OS_LINUX
        if (FolderPath.startsWith("~")) FolderPath=QDir::homePath()+FolderPath.mid(1);
    #endif
    #ifdef Q_OS_WIN
        if (FolderPath.startsWith(PersonalFolder)) FolderPath=QDir::homePath()+FolderPath.mid(PersonalFolder.length());
        FolderPath=AdjustDirForOS(FolderPath);
    #endif

    QString NewFolderPath=ui->FolderTree->GetFolderPath(ui->FolderTree->currentItem(),true);
    if (NewFolderPath.lastIndexOf(QDir::separator())!=-1) NewFolderPath=NewFolderPath.left(NewFolderPath.lastIndexOf(QDir::separator()));

    if (CustomMessageBox(this,QMessageBox::Question,APPLICATION_NAME,QApplication::translate("MainWindow","Are you sure to remove this folder ?\n(Warning: Content will not be moved to trash)")+"\n"+FolderPath,QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes) {
        s_Browser_InRemoveFolder(FolderPath);
        ui->FolderTree->SetSelectItemByPath(NewFolderPath);
    }
}

//====================================================================================================================

void MainWindow::s_Browser_RenameFolder() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_RenameFolder");
    bool    Ok;
    bool    PersoF=false;

    QString FolderPath=ui->FolderTree->GetFolderPath(ui->FolderTree->currentItem(),false);
    #ifdef Q_OS_LINUX
        if (FolderPath.startsWith("~")) {
            FolderPath=QDir::homePath()+FolderPath.mid(1);
            PersoF=true;
        }
    #endif
    #ifdef Q_OS_WIN
        if (FolderPath.startsWith(PersonalFolder)) {
            FolderPath=QDir::homePath()+FolderPath.mid(PersonalFolder.length());
            FolderPath=AdjustDirForOS(FolderPath);
            PersoF=true;
        }
    #endif
    QString SrcFolder    =FolderPath;
    QString SubFolderName=FolderPath;
    if (SubFolderName.endsWith(QDir::separator())) SubFolderName=SubFolderName.lastIndexOf(SubFolderName.length()-1);
    if (SubFolderName.indexOf(QDir::separator())!=-1) {
        FolderPath=FolderPath.left(FolderPath.lastIndexOf(QDir::separator()));
        SubFolderName=SubFolderName.mid(SubFolderName.lastIndexOf(QDir::separator())+1);
        SubFolderName=CustomInputDialog(this,QApplication::translate("MainWindow","Rename folder"),QApplication::translate("MainWindow","Folder:"),QLineEdit::Normal,SubFolderName,&Ok);
        if (Ok && !SubFolderName.isEmpty()) {
            if (!QDir().rename(SrcFolder,FolderPath+QDir::separator()+SubFolderName)) CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Rename folder"),QApplication::translate("MainWindow","Impossible to rename folder!"),QMessageBox::Ok);
            else {
                if (PersoF) {
                    #ifdef Q_OS_LINUX
                    FolderPath.replace(QDir::homePath(),"~");
                    #endif
                    #ifdef Q_OS_WIN
                    FolderPath.replace(AdjustDirForOS(QDir::homePath()),PersonalFolder);
                    #endif
                }
                ui->FolderTree->RefreshItemByPath(ui->FolderTree->GetFolderPath(ui->FolderTree->currentItem()->parent(),true),false);
                ui->FolderTree->SetSelectItemByPath(FolderPath+QDir::separator()+SubFolderName);
            }
        }
    }
}

//====================================================================================================================
#define ACTIONTYPE_ACTIONTYPE       0x0f00
#define ACTIONTYPE_DISPLAYMODE      0x0100
#define ACTIONTYPE_FILTERMODE       0x0200
#define ACTIONTYPE_ONOFFOPTIONS     0x0400
#define ONOFFOPTIONS_SHOWHIDDEN     1
#define ONOFFOPTIONS_HIDEHIDDEN     2
#define ONOFFOPTIONS_HIDEFILENAME   3
#define ONOFFOPTIONS_SHOWFILENAME   4

void MainWindow::s_Browser_ChangeDisplayMode() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_ChangeDisplayMode");

    // Create menu
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(CreateMenuAction(QIcon(":/img/DISPLAY_DATA.png"),                                 QApplication::translate("MainWindow","Details view"),              ACTIONTYPE_DISPLAYMODE|DISPLAY_DATA,       true,ApplicationConfig->CurrentMode==DISPLAY_DATA));
    ContextMenu->addAction(CreateMenuAction(QIcon(":/img/DISPLAY_WEB.png"),                                  QApplication::translate("MainWindow","Summary view"),              ACTIONTYPE_DISPLAYMODE|DISPLAY_WEBLONG,    true,ApplicationConfig->CurrentMode==DISPLAY_WEBLONG));
    ContextMenu->addAction(CreateMenuAction(QIcon(":/img/DISPLAY_JUKEBOX.png"),                              QApplication::translate("MainWindow","Icon view"),                 ACTIONTYPE_DISPLAYMODE|DISPLAY_ICON100,    true,ApplicationConfig->CurrentMode==DISPLAY_ICON100));
    ContextMenu->addSeparator();
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultFILEIcon.GetIcon(cCustomIcon::ICON16), QApplication::translate("MainWindow","All files"),                 ACTIONTYPE_FILTERMODE|OBJECTTYPE_UNMANAGED,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_UNMANAGED));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultFILEIcon.GetIcon(cCustomIcon::ICON16), QApplication::translate("MainWindow","Managed files"),             ACTIONTYPE_FILTERMODE|OBJECTTYPE_MANAGED,  true,ApplicationConfig->CurrentFilter==OBJECTTYPE_MANAGED));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultIMAGEIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Image files"),               ACTIONTYPE_FILTERMODE|OBJECTTYPE_IMAGEFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_IMAGEFILE));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultVIDEOIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Video files"),               ACTIONTYPE_FILTERMODE|OBJECTTYPE_VIDEOFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_VIDEOFILE));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultMUSICIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Music files"),               ACTIONTYPE_FILTERMODE|OBJECTTYPE_MUSICFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_MUSICFILE));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultFFDIcon.GetIcon(cCustomIcon::ICON16),  QApplication::translate("MainWindow","ffDiaporama project files"), ACTIONTYPE_FILTERMODE|OBJECTTYPE_FFDFILE,  true,ApplicationConfig->CurrentFilter==OBJECTTYPE_FFDFILE));
    ContextMenu->addSeparator();
    if (ApplicationConfig->ShowHiddenFilesAndDir) ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Visible_KO.png"),  QApplication::translate("MainWindow","Hide hidden files and folders"), ACTIONTYPE_ONOFFOPTIONS|ONOFFOPTIONS_HIDEHIDDEN,  true,false));
        else                                      ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Visible_OK.png"),  QApplication::translate("MainWindow","Show hidden files and folders"), ACTIONTYPE_ONOFFOPTIONS|ONOFFOPTIONS_SHOWHIDDEN,  true,false));
    if (ApplicationConfig->CurrentMode==DISPLAY_ICON100) {
        if (ApplicationConfig->DisplayFileName)   ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Visible_KO.png"),  QApplication::translate("MainWindow","Hide files name"), ACTIONTYPE_ONOFFOPTIONS|ONOFFOPTIONS_HIDEFILENAME,  true,false));
            else                                  ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Visible_OK.png"),  QApplication::translate("MainWindow","Show files name"), ACTIONTYPE_ONOFFOPTIONS|ONOFFOPTIONS_SHOWFILENAME,  true,false));
    }

    // Exec menu
    QAction *Action=ContextMenu->exec(QCursor::pos());
    if (Action) {
        int ActionType=Action->data().toInt() & ACTIONTYPE_ACTIONTYPE;
        int SubAction =Action->data().toInt() & (~ACTIONTYPE_ACTIONTYPE);
        if (ActionType==ACTIONTYPE_DISPLAYMODE) {
            if (ApplicationConfig->CurrentMode!=SubAction) {
                ApplicationConfig->CurrentMode=SubAction;
                ui->FileInfoLabel->setVisible(ApplicationConfig->CurrentMode!=DISPLAY_WEBLONG);
                ui->FolderTable->SetMode(ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);
                s_Browser_FloderTreeItemChanged(ui->FolderTree->currentItem(),NULL);
            }
        } else if (ActionType==ACTIONTYPE_FILTERMODE) {
            if (ApplicationConfig->CurrentFilter!=SubAction) {
                ApplicationConfig->CurrentFilter=SubAction;
                ui->FolderTable->SetMode(ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);
                s_Browser_FloderTreeItemChanged(ui->FolderTree->currentItem(),NULL);
            }
        } else if (ActionType==ACTIONTYPE_ONOFFOPTIONS) {
            if ((SubAction==ONOFFOPTIONS_SHOWHIDDEN)||(SubAction==ONOFFOPTIONS_HIDEHIDDEN)) {
                ApplicationConfig->ShowHiddenFilesAndDir=(SubAction==ONOFFOPTIONS_SHOWHIDDEN);
                s_Browser_RefreshAll();
            } else if ((SubAction==ONOFFOPTIONS_SHOWFILENAME)||(SubAction==ONOFFOPTIONS_HIDEFILENAME)) {
                ApplicationConfig->DisplayFileName=(SubAction==ONOFFOPTIONS_SHOWFILENAME);
                s_Browser_RefreshAll();
            }
        }
    }

    // delete menu
    while (ContextMenu->actions().count()) delete ContextMenu->actions().takeLast();
    delete ContextMenu;

    // set up button
    ui->ActionModeBt->setDown(false);
}

//====================================================================================================================

void MainWindow::s_Browser_DoubleClicked(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_DoubleClicked");
    s_Browser_OpenFile();
}

//====================================================================================================================

void MainWindow::s_Browser_OpenFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_OpenFile");

    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();
    if (Media) {
        if ((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)||(Media->ObjectType==OBJECTTYPE_THUMBNAIL))
            QDesktopServices::openUrl(QUrl().fromLocalFile(Media->FileName));
        else if (Media->ObjectType==OBJECTTYPE_FOLDER) {
            QString Path=ui->FolderTree->GetCurrentFolderPath();
            if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
            Path=Path+Media->ShortName;
            ui->FolderTree->SetSelectItemByPath(Path);
        } else if (Media->ObjectType==OBJECTTYPE_FFDFILE) {
                FileForIO=Media->FileName;
                int Ret=QMessageBox::Yes;
                if (Diaporama->IsModify) {
                    Ret=CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Open project"),
                        QApplication::translate("MainWindow","Current project has been modified.\nDo you want to save-it ?"),
                        QMessageBox::Cancel|QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
                    if (Ret==QMessageBox::Yes) s_Action_Save();
                }
                if (Ret!=QMessageBox::Cancel) {
                    ToStatusBar(QApplication::translate("MainWindow","Open file :")+QFileInfo(FileForIO).fileName());
                    QTimer::singleShot(LATENCY,this,SLOT(DoOpenFile()));
                }
        }
    }
}

//====================================================================================================================

void MainWindow::s_Browser_RightClicked(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_RightClicked");

    QList<cBaseMediaFile*> MediaList=ui->FolderTable->GetCurrentSelectedMediaFile();
    if (MediaList.count()==0) return;

    bool    Multiple=(MediaList.count()>1);
    bool    IsFind;

    // Do qualification of files
    QStringList FileExtensions;
    QList<int>  ObjectTypes;

    for (int i=0;i<MediaList.count();i++) {
        IsFind=false;   for (int j=0;j<ObjectTypes.count();j++)     if (MediaList[i]->ObjectType==ObjectTypes[j])       IsFind=true; if (!IsFind) ObjectTypes.append(MediaList[i]->ObjectType);
        IsFind=false;   for (int j=0;j<FileExtensions.count();j++)  if (MediaList[i]->FileExtension==FileExtensions[j]) IsFind=true; if (!IsFind) FileExtensions.append(MediaList[i]->FileExtension);
    }

    QMenu *ContextMenu=new QMenu(this);
    if (((ObjectTypes.count()==1)&&(ObjectTypes[0]!=OBJECTTYPE_UNMANAGED))||((ObjectTypes.count()==2)&&((ObjectTypes[0]==OBJECTTYPE_VIDEOFILE)||(ObjectTypes[0]==OBJECTTYPE_IMAGEFILE))&&((ObjectTypes[1]==OBJECTTYPE_VIDEOFILE)||(ObjectTypes[1]==OBJECTTYPE_IMAGEFILE)))) {
        // Single object type
        switch (ObjectTypes[0]) {
            case OBJECTTYPE_MUSICFILE :
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserOpen);
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserProperties);
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserRenameFile);
                ContextMenu->addAction(ui->actionBrowserRemoveFile);
                ContextMenu->addSeparator();
                ContextMenu->addAction(ui->actionBrowserUseAsPlaylist);
                break;
            case OBJECTTYPE_VIDEOFILE :
            case OBJECTTYPE_IMAGEFILE :
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserOpen);
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserProperties);
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserRenameFile);
                ContextMenu->addAction(ui->actionBrowserRemoveFile);
                ContextMenu->addSeparator();
                ContextMenu->addAction(ui->actionBrowserAddFiles);
                break;
            case OBJECTTYPE_FFDFILE   :
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserOpen);
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserProperties);
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserRenameFile);
                ContextMenu->addAction(ui->actionBrowserRemoveFile);
                ContextMenu->addSeparator();
                ContextMenu->addAction(ui->actionBrowserAddProject);
                break;
            case OBJECTTYPE_FOLDER    :
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserOpen);
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserRenameFile);
                ContextMenu->addAction(ui->actionBrowserRemoveFile);
                break;
        }
    } else {
        // Multiple object type
        ContextMenu->addAction(ui->actionBrowserRemoveFile);
    }
    ContextMenu->exec(QCursor::pos());
    delete ContextMenu;
}

//====================================================================================================================

void MainWindow::s_Browser_Properties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_Properties");
    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();
    if (Media) {
        DlgInfoFile Dlg(Media,HELPFILE_DlgInfoFile,ApplicationConfig,ApplicationConfig->DlgInfoFileWSP,this);
        Dlg.InitDialog();
        Dlg.exec();
    }
}

//====================================================================================================================

void MainWindow::s_Browser_AddFiles() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_AddFiles");
    QList<cBaseMediaFile*> MediaList=ui->FolderTable->GetCurrentSelectedMediaFile();
    if (MediaList.count()>0) {

        // Calc position of new object depending on ApplicationConfig->AppendObject
        if (ApplicationConfig->AppendObject) {
            SavedCurIndex   =Diaporama->List.count();
            CurIndex        =Diaporama->List.count();
        } else {
            SavedCurIndex=Diaporama->CurrentCol;
            CurIndex=Diaporama->List.count()!=0?SavedCurIndex+1:0;
            if (SavedCurIndex==Diaporama->List.count()) SavedCurIndex--;
        }
        FileList.clear();
        for (int i=0;i<MediaList.count();i++) FileList.append(MediaList[i]->FileName);

        SortFileList();

        ToStatusBar(QApplication::translate("MainWindow","Add file to project :")+QFileInfo(FileList[0]).fileName());
        QTimer::singleShot(LATENCY,this,SLOT(s_Action_DoAddFile()));
    }
}

//====================================================================================================================

void MainWindow::s_Browser_RemoveFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_RemoveFile");
    QList<cBaseMediaFile*> MediaList=ui->FolderTable->GetCurrentSelectedMediaFile();
    if (MediaList.count()>0) {

    }
    CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Work in progress"),QApplication::translate("MainWindow","Sorry, not yet!"),QMessageBox::Ok);
}

//====================================================================================================================

void MainWindow::s_Browser_RenameFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_RenameFile");
    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();
    if (Media) {
    }
    CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Work in progress"),QApplication::translate("MainWindow","Sorry, not yet!"),QMessageBox::Ok);
}

//====================================================================================================================

void MainWindow::s_Browser_UseAsPlaylist() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::s_Browser_UseAsPlaylist");
    QList<cBaseMediaFile*> MediaList=ui->FolderTable->GetCurrentSelectedMediaFile();
    if (MediaList.count()>0) {

    }
    CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("MainWindow","Work in progress"),QApplication::translate("MainWindow","Sorry, not yet!"),QMessageBox::Ok);
}

//====================================================================================================================
// UTILITY FUNCTIONS
//====================================================================================================================

QAction *MainWindow::CreateMenuAction(QImage *Icon,QString Text,int Data,bool Checkable,bool IsCheck) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::CreateMenuAction");
    QAction *Action;
    if (Icon) Action=new QAction(QIcon(QPixmap().fromImage(*Icon)),Text,this);
        else Action=new QAction(Text,this);
    Action->setIconVisibleInMenu(true);
    Action->setCheckable(Checkable);
    Action->setFont(QFont("Sans Serif",9));
    if (Checkable) Action->setChecked(IsCheck);
    Action->setData(QVariant(Data));
    return Action;
}

//====================================================================================================================

QAction *MainWindow::CreateMenuAction(QIcon Icon,QString Text,int Data,bool Checkable,bool IsCheck) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::CreateMenuAction");
    QAction *Action;
    Action=new QAction(Icon,Text,this);
    Action->setIconVisibleInMenu(true);
    Action->setCheckable(Checkable);
    Action->setFont(QFont("Sans Serif",9));
    if (Checkable) Action->setChecked(IsCheck);
    Action->setData(QVariant(Data));
    return Action;
}

//====================================================================================================================

void MainWindow::SortFileList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:MainWindow::SortFileList");
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
}
