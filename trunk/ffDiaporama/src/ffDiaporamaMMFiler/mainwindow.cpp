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

#include <QtCore>
#include <QtDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QIcon>
#include <QProcess>
#include <QTimer>
#include <QMessageBox>
#include <QSplitter>
#include <QComboBox>
#include <QDesktopServices>
#include <QMenu>

#include "../sharedfiles/cBaseMediaFile.h"
#include "../sharedfiles/QCustomFileInfoLabel.h"
#include "../sharedfiles/QCustomFolderTable.h"
#include "../sharedfiles/QCustomFolderTree.h"
#include "../sharedfiles/QCustomHorizSplitter.h"
#include "../sharedfiles/DlgCheckConfig.h"
#include "../sharedfiles/DlgInfoFile.h"

#include "QCustomJobTable.h"
#include "DlgApplicationSettings.h"
#include "DlgAbout.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//#define DEBUGMODE

QIcon   Icon_DISPLAY_DATA_S;
QIcon   Icon_DISPLAY_DATA;
QIcon   Icon_DISPLAY_WEB_S;
QIcon   Icon_DISPLAY_WEB;
QIcon   Icon_DISPLAY_JUKEBOX_S;
QIcon   Icon_DISPLAY_JUKEBOX;

QIcon   Icon_FILTER_NO;
QIcon   Icon_FILTER_FFD;
QIcon   Icon_FILTER_IMAGE;
QIcon   Icon_FILTER_MUSIC;
QIcon   Icon_FILTER_VIDEO;

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::MainWindow";
    #endif

    ApplicationConfig     =new cApplicationConfig(this);
    DriveList             =new cDriveList(ApplicationConfig);
    IsFirstInitDone       =false;
    CurrentJobThread      =-1;
    Icon_DISPLAY_DATA_S   =QIcon(":/img/DISPLAY_DATA_S.png");
    Icon_DISPLAY_DATA     =QIcon(":/img/DISPLAY_DATA.png");
    Icon_DISPLAY_WEB_S    =QIcon(":/img/DISPLAY_WEB_S.png");
    Icon_DISPLAY_WEB      =QIcon(":/img/DISPLAY_WEB_S.png");
    Icon_DISPLAY_JUKEBOX_S=QIcon(":/img/DISPLAY_JUKEBOX_S.png");
    Icon_DISPLAY_JUKEBOX  =QIcon(":/img/DISPLAY_JUKEBOX.png");

    Icon_FILTER_NO        =QIcon(":/img/FILTER_FILE_S.png");
    Icon_FILTER_FFD       =QIcon(":/img/FILTER_FFD_S.png");
    Icon_FILTER_IMAGE     =QIcon(":/img/FILTER_IMAGE_S.png");
    Icon_FILTER_MUSIC     =QIcon(":/img/FILTER_MUSIC_S.png");
    Icon_FILTER_VIDEO     =QIcon(":/img/FILTER_VIDEO_S.png");
}

//====================================================================================================================

MainWindow::~MainWindow() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::~MainWindow";
    #endif
    delete ui;
    delete DriveList;
    delete ApplicationConfig;
}

//====================================================================================================================

void MainWindow::InitWindow(QString ForceLanguage,QApplication *App) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::InitWindow";
    #endif

    ApplicationConfig->InitConfigurationValues(ForceLanguage,App);
    ApplicationConfig->PreloadSystemIcons();

    // Register all formats and codecs for libavformat/libavcodec/etc ...
    ApplicationConfig->DeviceModelList.Initffmpeg();

    ui->setupUi(this);
    setWindowTitle(QString("%1 %2").arg(APPLICATION_NAME).arg(APPLICATION_VERSION));
    ui->FolderTree->ApplicationConfig =ApplicationConfig;
    ui->FolderTable->ApplicationConfig=ApplicationConfig;
    ui->JobTable->ApplicationConfig   =ApplicationConfig;
    ui->JobTable->JobQueue            =&JobQueue;

    // do some init ...
    ui->Action_Mode_BT->setIcon(*GetIconMode());
    ui->Action_Filter_BT->setIcon(*GetIconFilter());
    ui->FileInfoLabel->DisplayMode=DISPLAY_WEBLONG;
    ui->FileInfoLabel->setVisible((ApplicationConfig->CurrentMode!=DISPLAY_WEBSHORT)&&(ApplicationConfig->CurrentMode!=DISPLAY_WEBLONG));

    // Initialise integrated controls and list
    DriveList->UpdateDriveList();
    ui->FolderTree->InitDrives(DriveList);
    ui->FolderTable->SetMode(ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);

    ui->ToolBoxNormal->setCurrentIndex(0);
    ui->JobTable->DoRefreshList();
    RefreshControls();

    ApplicationConfig->ImagesCache.MaxValue=ApplicationConfig->MemCacheMaxValue;

    connect(ui->FolderTree,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(s_currentTreeItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));

    connect(ui->FolderTable,SIGNAL(itemSelectionChanged()),this,SLOT(DoRefreshSelectedFileInfo()));
    connect(ui->FolderTable,SIGNAL(DoubleClickEvent()),this,SLOT(s_itemDoubleClicked()));
    connect(ui->FolderTable,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_itemRightClicked(QMouseEvent *)));
    connect(ui->FolderTable,SIGNAL(RefreshFolderInfo()),this,SLOT(DoRefreshFolderInfo()));

    connect(ui->RefreshBt,SIGNAL(pressed()),this,SLOT(s_Refresh()));
    connect(ui->PlayBt,SIGNAL(pressed()),this,SLOT(s_OpenFile()));
    connect(ui->InfoBt,SIGNAL(pressed()),this,SLOT(s_InfoFile()));
    connect(ui->WizardBt,SIGNAL(pressed()),this,SLOT(s_ActionFile()));

    connect(ui->ActionConfiguration_BT,SIGNAL(pressed()),this,SLOT(s_Config()));

    connect(ui->Action_About_BT,SIGNAL(pressed()),this,SLOT(s_About()));
    connect(ui->ActionDocumentation_BT,SIGNAL(pressed()),this,SLOT(s_Documentation()));
    connect(ui->ActionNewFunctions_BT,SIGNAL(pressed()),this,SLOT(s_NewFunctions()));
    connect(ui->Action_Exit_BT,SIGNAL(pressed()),this,SLOT(s_action_Exit()));

    connect(ui->Action_Mode_BT,SIGNAL(pressed()),this,SLOT(s_action_Mode()));
    connect(ui->Action_Filter_BT,SIGNAL(pressed()),this,SLOT(s_action_Filter()));

    ui->FolderTree->SetSelectItemByPath(ui->FolderTree->RealPathToTreePath(ApplicationConfig->CurrentPath));
    if (ui->FolderTree->GetCurrentFolderPath()!=ApplicationConfig->CurrentPath) ui->FolderTree->SetSelectItemByPath(QApplication::translate("QCustomFolderTree","Personal folder"));

    if (ApplicationConfig->CheckConfigAtStartup) QTimer::singleShot(500,this,SLOT(s_DlgCheckConfig())); else {
        QString Status;
        if (!Checkffmpeg(Status)) QTimer::singleShot(500,this,SLOT(s_DlgCheckConfig()));
    }

    connect(&Timer,SIGNAL(timeout()),this,SLOT(s_TimerEvent()));
    Timer.start(500);   // Start Timer
}

//====================================================================================================================

void MainWindow::closeEvent(QCloseEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::closeEvent";
    #endif
    Timer.stop();
    if (isMaximized()) {
        ApplicationConfig->MainWinWSP->IsMaximized=true;
        showNormal();
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    } else ApplicationConfig->MainWinWSP->IsMaximized=false;
    if (isMinimized()) {
        showNormal();
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    // Save configuration
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ApplicationConfig->MainWinWSP->SaveWindowState(this);
    ApplicationConfig->SplitterHSizeAndPos=QString(QByteArray(ui->SplitterH->saveState()).toHex());
    ApplicationConfig->SplitterVSizeAndPos=QString(QByteArray(ui->SplitterV->saveState()).toHex());
    ApplicationConfig->SaveConfigurationFile();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::showEvent(QShowEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::showEvent";
    #endif
    if (!IsFirstInitDone) {
        IsFirstInitDone=true;                                   // do this only one time
        ApplicationConfig->MainWinWSP->ApplyToWindow(this);     // Restore window position
        if (ApplicationConfig->SplitterHSizeAndPos!="") ui->SplitterH->restoreState(QByteArray::fromHex(ApplicationConfig->SplitterHSizeAndPos.toUtf8()));
        if (ApplicationConfig->SplitterVSizeAndPos!="") ui->SplitterV->restoreState(QByteArray::fromHex(ApplicationConfig->SplitterVSizeAndPos.toUtf8()));
        if (ApplicationConfig->MainWinWSP->IsMaximized) QTimer::singleShot(500,this,SLOT(DoMaximized()));
        /*// Start a network process to give last ffdiaporama version from internet web site
        QNetworkAccessManager *mNetworkManager=new QNetworkAccessManager(this);
        connect(mNetworkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(s_Event_NetworkReply(QNetworkReply*)));
        QUrl            url(BUILDVERSION_WEBURL);
        QNetworkReply   *reply  = mNetworkManager->get(QNetworkRequest(url));
        reply->deleteLater();*/
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

void MainWindow::resizeEvent(QResizeEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::resizeEvent";
    #endif
}

//====================================================================================================================

void MainWindow::RefreshControls() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::RefreshControls";
    #endif

    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();

    ui->PlayBt->setEnabled((Media)&&((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)||(Media->ObjectType==OBJECTTYPE_THUMBNAIL)||(Media->ObjectType==OBJECTTYPE_FFDFILE)));
    ui->InfoBt->setEnabled((Media)&&((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)||(Media->ObjectType==OBJECTTYPE_THUMBNAIL)||(Media->ObjectType==OBJECTTYPE_FFDFILE)));
    ui->WizardBt->setEnabled((Media)&&((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)||(Media->ObjectType==OBJECTTYPE_FOLDER)||(Media->ObjectType==OBJECTTYPE_FFDFILE)));
}

//====================================================================================================================

void MainWindow::s_TimerEvent() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_TimerEvent";
    #endif

    if (Thread.isRunning()) return;
    if (CurrentJobThread!=-1) ui->JobTable->DoRefreshAJob(CurrentJobThread);
    CurrentJobThread=-1;
    Thread.setFuture(QtConcurrent::run(this,&MainWindow::ThreadJob));
}

//====================================================================================================================

void MainWindow::ThreadJob() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::ConvertIMG";
    #endif
    int i=0;
    while ((i<JobQueue.List.count())&&(JobQueue.List[i]->JobStatus!=JOBSTATUS_READYTOSTART)) i++;
    if ((i<JobQueue.List.count())&&(JobQueue.List[i]->JobStatus==JOBSTATUS_READYTOSTART)) {
        CurrentJobThread=i;
        switch (JobQueue.List[i]->JobType) {
            case JOBTYPE_IMAGE_CONVERT_JPG     :
            case JOBTYPE_IMAGE_CONVERT_MULTJPG :
            case JOBTYPE_IMAGE_CONVERT_PNG     :
            case JOBTYPE_IMAGE_CONVERT_MULTPNG : JobQueue.ConvertIMG(JobQueue.List[i],this);

        }
    }
}

//====================================================================================================================

void MainWindow::customEvent(QEvent *event) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::RefreshJobStatus";
    #endif

    if      (event->type()==JobStatusChanged) ui->JobTable->DoRefreshAJob(CurrentJobThread);
    else if (event->type()==FileListChanged)  ui->FolderTable->RefreshListFolder();
    else QMainWindow::customEvent(event);
}

//====================================================================================================================

QIcon *MainWindow::GetIconMode() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::GetIconMode";
    #endif
    switch (ApplicationConfig->CurrentMode) {
        case DISPLAY_DATA:      return &Icon_DISPLAY_DATA_S;
        case DISPLAY_WEBSHORT:
        case DISPLAY_WEBLONG:   return &Icon_DISPLAY_WEB_S;
        case DISPLAY_ICON48:
        case DISPLAY_ICON100:
        case DISPLAY_ICONBIG:   return &Icon_DISPLAY_JUKEBOX_S;
    }
    return NULL;
}

//====================================================================================================================

QIcon *MainWindow::GetIconFilter() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::GetIconFilter";
    #endif
    switch (ApplicationConfig->CurrentFilter) {
        case OBJECTTYPE_UNMANAGED:
        case OBJECTTYPE_MANAGED:    return &Icon_FILTER_NO;
        case OBJECTTYPE_FFDFILE:    return &Icon_FILTER_FFD;
        case OBJECTTYPE_IMAGEFILE:  return &Icon_FILTER_IMAGE;
        case OBJECTTYPE_VIDEOFILE:  return &Icon_FILTER_VIDEO;
        case OBJECTTYPE_MUSICFILE:  return &Icon_FILTER_MUSIC;
    }
    return NULL;
}

//====================================================================================================================

void MainWindow::s_currentTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_currentTreeItemChanged";
    #endif

    ApplicationConfig->CurrentPath=ui->FolderTree->GetFolderPath(current,false);
    QList<cBaseMediaFile*> EmptyList;
    ui->FileInfoLabel->SetupFileInfoLabel(EmptyList);

    ui->FolderTree->RefreshItemByPath(ui->FolderTree->GetFolderPath(current,true),false);
    DoRefreshFolderInfo();

    ui->CurrentPathED->setText(ApplicationConfig->CurrentPath);
    ui->FolderIcon->setPixmap(DriveList->GetFolderIcon(ApplicationConfig->CurrentPath).pixmap(48,48));

    QString Path=ApplicationConfig->CurrentPath;
    #if defined(Q_OS_WIN)
        Path.replace("%HOMEDRIVE%%HOMEPATH%",DriveList->List[0].Path,Qt::CaseInsensitive);
        Path.replace("%USERPROFILE%",DriveList->List[0].Path,Qt::CaseInsensitive);
        Path=AdjustDirForOS(Path);
        if (QDir(Path).canonicalPath()!="") Path=QDir(Path).canonicalPath(); // Resolved eventual .lnk files
    #endif
    ui->FolderTable->FillListFolder(Path);
}

//====================================================================================================================

void MainWindow::DoRefreshFolderInfo() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_currentTreeItemChanged";
    #endif
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

void MainWindow::DoRefreshSelectedFileInfo() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::DoRefreshSelectedFileInfo";
    #endif

    if (ui->FileInfoLabel->isVisible()) {
        ui->FileInfoLabel->SetupFileInfoLabel(ui->FolderTable->GetCurrentSelectedMediaFile());
        ui->FileInfoLabel->repaint();
    }
    RefreshControls();
}

//====================================================================================================================

void MainWindow::s_Refresh() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Refresh";
    #endif

    ui->RefreshBt->setDown(false);
    ui->FolderTree->RefreshDriveList();
    s_currentTreeItemChanged(ui->FolderTree->currentItem(),NULL);
}

//====================================================================================================================

void MainWindow::s_Config() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Config";
    #endif

    DlgApplicationSettings Dlg(HELPFILE_DlgApplicationSettings,ApplicationConfig,ApplicationConfig->DlgApplicationSettingsWSP,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) {
        // Save configuration
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        // Do not change get WindowState for mainwindow except when closing
        //ApplicationConfig->MainWinWSP->SaveWindowState(this);
        //ApplicationConfig->SplitterSizeAndPos=QString(QByteArray(ui->Splitter->saveState()).toHex());
        ApplicationConfig->SaveConfigurationFile();
        QApplication::restoreOverrideCursor();
        ApplicationConfig->ImagesCache.MaxValue=ApplicationConfig->MemCacheMaxValue;

         // Refresh all
        s_Refresh();
        s_currentTreeItemChanged(ui->FolderTree->currentItem(),NULL);
    }
}

//====================================================================================================================

void MainWindow::s_DlgCheckConfig() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_DlgCheckConfig";
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

void MainWindow::s_About() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_About";
    #endif
    ui->Action_About_BT->setDown(false);
    DlgAbout Dlg("",ApplicationConfig,ApplicationConfig->DlgAboutWSP,this);
    Dlg.InitDialog();
    Dlg.exec();
}

//====================================================================================================================

void MainWindow::s_Documentation() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_Documentation";
    #endif
    ui->ActionDocumentation_BT->setDown(false);
    QDesktopServices::openUrl(QUrl(QString(HELPFILE_SUPPORT).replace("<local>",ApplicationConfig->GetValideWEBLanguage(ApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void MainWindow::s_NewFunctions() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_NewFunctions";
    #endif
    ui->ActionNewFunctions_BT->setDown(false);
    QDesktopServices::openUrl(QUrl(QString(HELPFILE_NEWS).replace("<local>",ApplicationConfig->GetValideWEBLanguage(ApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void MainWindow::s_action_Exit() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_Exit";
    #endif
    close();
}

//====================================================================================================================

QAction *MainWindow::CreateMenuAction(QImage *Icon,QString Text,int Data,bool Checkable,bool IsCheck) {
    QAction *Action;
    if (Icon) Action=new QAction(QIcon(QPixmap().fromImage(*Icon)),Text,this);
        else Action=new QAction(Text,this);
    Action->setIconVisibleInMenu(true);
    Action->setCheckable(Checkable);
    if (Checkable) Action->setChecked(IsCheck);
    Action->setData(QVariant(Data));
    return Action;
}

QAction *MainWindow::CreateMenuAction(QIcon Icon,QString Text,int Data,bool Checkable,bool IsCheck) {
    QAction *Action;
    Action=new QAction(Icon,Text,this);
    Action->setIconVisibleInMenu(true);
    Action->setCheckable(Checkable);
    if (Checkable) Action->setChecked(IsCheck);
    Action->setData(QVariant(Data));
    return Action;
}

//====================================================================================================================

void MainWindow::s_action_Mode() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_Mode";
    #endif

    // Create menu
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(CreateMenuAction(Icon_DISPLAY_DATA,   QApplication::translate("MainWindow","Details view"),      DISPLAY_DATA,    true,ApplicationConfig->CurrentMode==DISPLAY_DATA));
    ContextMenu->addAction(CreateMenuAction(Icon_DISPLAY_WEB,    QApplication::translate("MainWindow","Short summary view"),DISPLAY_WEBSHORT,true,ApplicationConfig->CurrentMode==DISPLAY_WEBSHORT));
    ContextMenu->addAction(CreateMenuAction(Icon_DISPLAY_WEB,    QApplication::translate("MainWindow","Long summary view"), DISPLAY_WEBLONG, true,ApplicationConfig->CurrentMode==DISPLAY_WEBLONG));
    ContextMenu->addAction(CreateMenuAction(Icon_DISPLAY_JUKEBOX,QApplication::translate("MainWindow","Small icon view"),   DISPLAY_ICON48,  true,ApplicationConfig->CurrentMode==DISPLAY_ICON48));
    ContextMenu->addAction(CreateMenuAction(Icon_DISPLAY_JUKEBOX,QApplication::translate("MainWindow","Medium icon view"),  DISPLAY_ICON100, true,ApplicationConfig->CurrentMode==DISPLAY_ICON100));
    ContextMenu->addAction(CreateMenuAction(Icon_DISPLAY_JUKEBOX,QApplication::translate("MainWindow","Images wall view"),  DISPLAY_ICONBIG, true,ApplicationConfig->CurrentMode==DISPLAY_ICONBIG));

    // Exec menu
    QAction *Action=ContextMenu->exec(QCursor::pos());
    if ((Action)&&(ApplicationConfig->CurrentMode!=Action->data().toInt())) {
        ApplicationConfig->CurrentMode=Action->data().toInt();
        ui->Action_Mode_BT->setIcon(*GetIconMode());
        ui->FileInfoLabel->setVisible((ApplicationConfig->CurrentMode!=DISPLAY_WEBSHORT)&&(ApplicationConfig->CurrentMode!=DISPLAY_WEBLONG));
        ui->FolderTable->SetMode(ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);
        s_currentTreeItemChanged(ui->FolderTree->currentItem(),NULL);
    }

    // delete menu
    while (ContextMenu->actions().count()) delete ContextMenu->actions().takeLast();
    delete ContextMenu;

    // set up button
    ui->Action_Mode_BT->setDown(false);
}

//====================================================================================================================

void MainWindow::s_action_Filter() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_action_Filter";
    #endif

    // Create menu
    QMenu   *ContextMenu=new QMenu(this);
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultFILEIcon.GetIcon(cCustomIcon::ICON16), QApplication::translate("MainWindow","All files"),                OBJECTTYPE_UNMANAGED,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_UNMANAGED));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultFILEIcon.GetIcon(cCustomIcon::ICON16), QApplication::translate("MainWindow","Managed files"),            OBJECTTYPE_MANAGED,  true,ApplicationConfig->CurrentFilter==OBJECTTYPE_MANAGED));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultIMAGEIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Image files"),              OBJECTTYPE_IMAGEFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_IMAGEFILE));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultVIDEOIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Video files"),              OBJECTTYPE_VIDEOFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_VIDEOFILE));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultMUSICIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Music files"),              OBJECTTYPE_MUSICFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_MUSICFILE));
    ContextMenu->addAction(CreateMenuAction(ApplicationConfig->DefaultFFDIcon.GetIcon(cCustomIcon::ICON16),  QApplication::translate("MainWindow","ffDiaporama project files"),OBJECTTYPE_FFDFILE,  true,ApplicationConfig->CurrentFilter==OBJECTTYPE_FFDFILE));

    // Exec menu
    QAction *Action=ContextMenu->exec(QCursor::pos());
    if ((Action)&&(ApplicationConfig->CurrentFilter!=Action->data().toInt())) {
        ApplicationConfig->CurrentFilter=Action->data().toInt();
        ui->Action_Filter_BT->setIcon(*GetIconFilter());
        ui->FolderTable->SetMode(ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);
        s_currentTreeItemChanged(ui->FolderTree->currentItem(),NULL);
    }

    // delete menu
    while (ContextMenu->actions().count()) delete ContextMenu->actions().takeLast();
    delete ContextMenu;

    // set up button
    ui->Action_Filter_BT->setDown(false);
}

//====================================================================================================================

void MainWindow::s_OpenFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_OpenFile";
    #endif

    ui->PlayBt->setDown(false);
    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();
    if (Media) {
        if ((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)||(Media->ObjectType==OBJECTTYPE_THUMBNAIL)||(Media->ObjectType==OBJECTTYPE_FFDFILE))
            QDesktopServices::openUrl(QUrl().fromLocalFile(Media->FileName));
        else if (Media->ObjectType==OBJECTTYPE_FOLDER) {
            QString Path=ui->FolderTree->GetCurrentFolderPath();
            if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
            Path=Path+Media->ShortName;
            ui->FolderTree->SetSelectItemByPath(Path);
        }
    }
}

//====================================================================================================================

void MainWindow::s_InfoFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_InfoFile";
    #endif

    ui->InfoBt->setDown(false);
    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();
    if (Media) {
        if ((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)||(Media->ObjectType==OBJECTTYPE_THUMBNAIL)||(Media->ObjectType==OBJECTTYPE_FFDFILE)) {
            DlgInfoFile Dlg(Media,HELPFILE_DlgInfoFile,ApplicationConfig,ApplicationConfig->DlgInfoFileWSP,this);
            Dlg.InitDialog();
            Dlg.exec();
        }
    }
}

//====================================================================================================================

void MainWindow::s_itemDoubleClicked() {
    s_OpenFile();
}

//====================================================================================================================

void MainWindow::s_itemDoubleClicked(QTableWidgetItem *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_itemDoubleClicked";
    #endif
    s_OpenFile();
}

//====================================================================================================================

void MainWindow::s_itemRightClicked(QMouseEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_itemRightClicked";
    #endif
    s_ActionFile();
}

//====================================================================================================================

void MainWindow::s_ActionFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_ActionFile";
    #endif

    QList<cBaseMediaFile*> MediaList=ui->FolderTable->GetCurrentSelectedMediaFile();
    if (MediaList.count()==0) {
        ui->WizardBt->setDown(false);
        return;
    }

    bool    Multiple=(MediaList.count()>1);
    bool    IsFind;

    // Do qualification of files
    QStringList FileExtensions;
    QList<int>  ObjectTypes;

    for (int i=0;i<MediaList.count();i++) {
        IsFind=false;   for (int j=0;j<ObjectTypes.count();j++)     if (MediaList[i]->ObjectType==ObjectTypes[j])       IsFind=true; if (!IsFind) ObjectTypes.append(MediaList[i]->ObjectType);
        IsFind=false;   for (int j=0;j<FileExtensions.count();j++)  if (MediaList[i]->FileExtension==FileExtensions[j]) IsFind=true; if (!IsFind) FileExtensions.append(MediaList[i]->FileExtension);
    }

    if (ObjectTypes.count()==1) {

        // Create menu
        QMenu   *ContextMenu=new QMenu(this);
        ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Action_Open.png"),JobQueue.JobTypeText[JOBTYPE_OPENFILE],JOBTYPE_OPENFILE,false,false));
        ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Action_Info.png"),JobQueue.JobTypeText[JOBTYPE_DISPLAYINFO],JOBTYPE_DISPLAYINFO,false,false));
        ContextMenu->addSeparator();
        switch (ObjectTypes[0]) {
            case OBJECTTYPE_MUSICFILE :
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Convert"),                           JOBTYPE_FFMPEGCONVERT_AUDIO,        false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Convert for ffDiaporama"),           JOBTYPE_FFMPEGCONVERT_AUDIOFFD,     false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Extract cover"),                     JOBTYPE_THUMBNAIL_EXTRACTCOVER,     false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Edit properties"),                   JOBTYPE_TAG_AUDIO,                  false,false));
                break;
            case OBJECTTYPE_VIDEOFILE :
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Convert"),                           JOBTYPE_FFMPEGCONVERT_VIDEO,        false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Convert for ffDiaporama"),           JOBTYPE_FFMPEGCONVERT_VIDEOFFD,     false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Extract audio track"),               JOBTYPE_FFMPEGEXTRACT_AUDIO,        false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Extract a subtitle"),                JOBTYPE_FFMPEGEXTRACT_SUBTITLES,    false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Extract a chapter"),                 JOBTYPE_FFMPEGEXTRACT_CHAPTER,      false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Extract all chapters"),              JOBTYPE_FFMPEGEXTRACT_ALLCHAPTERS,  false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Edit properties"),                   JOBTYPE_TAG_VIDEO,                  false,false));
                break;
            case OBJECTTYPE_IMAGEFILE :
                if (Multiple) {
                    //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Convert as folder thumbnail"),       JOBTYPE_THUMBNAIL_CREATEFOLDER,     false,false));
                    if ((FileExtensions.count()>1)||(FileExtensions[0]!="jpg")) ContextMenu->addAction(CreateMenuAction(NULL,JobQueue.JobTypeText[JOBTYPE_IMAGE_CONVERT_MULTJPG],JOBTYPE_IMAGE_CONVERT_MULTJPG,false,false));
                    if ((FileExtensions.count()>1)||(FileExtensions[0]!="png")) ContextMenu->addAction(CreateMenuAction(NULL,JobQueue.JobTypeText[JOBTYPE_IMAGE_CONVERT_MULTPNG],JOBTYPE_IMAGE_CONVERT_MULTPNG,false,false));
                } else {
                    if (FileExtensions[0]!="jpg") ContextMenu->addAction(CreateMenuAction(NULL,JobQueue.JobTypeText[JOBTYPE_IMAGE_CONVERT_JPG],JOBTYPE_IMAGE_CONVERT_JPG,false,false));
                    if (FileExtensions[0]!="png") ContextMenu->addAction(CreateMenuAction(NULL,JobQueue.JobTypeText[JOBTYPE_IMAGE_CONVERT_PNG],JOBTYPE_IMAGE_CONVERT_PNG,false,false));
                }
                break;
            case OBJECTTYPE_FFDFILE   :
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Render project"),                    JOBTYPE_FFDIAPORAMA_RENDER,         false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Archive project"),                   JOBTYPE_FFDIAPORAMA_ARCHIVE,        false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Edit properties"),                   JOBTYPE_TAG_FFD,                    false,false));
                break;
            case OBJECTTYPE_FOLDER    :
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Create an album playlist"),          JOBTYPE_PLAYLIST_CREATEALBUM,       false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Create an artist playlist"),         JOBTYPE_PLAYLIST_CREATEARTIST,      false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Create an album folder thumbnail"),  JOBTYPE_THUMBNAIL_CREATEALBUM,      false,false));
                //ContextMenu->addAction(CreateMenuAction(NULL,QApplication::translate("MainWindow","Create an artist folder thumbnail"), JOBTYPE_THUMBNAIL_CREATEARTIST,     false,false));
                break;
        }

        // Exec menu
        QAction *Action=ContextMenu->exec(QCursor::pos());
        if (Action) {
            int ActionType=Action->data().toInt();
            switch (ActionType) {
                case JOBTYPE_OPENFILE                   :   s_OpenFile();                                   break;
                case JOBTYPE_DISPLAYINFO                :   s_InfoFile();                                   break;
                case JOBTYPE_IMAGE_CONVERT_JPG          :
                case JOBTYPE_IMAGE_CONVERT_MULTJPG      :   s_ConvertIMG(&MediaList,"jpg",ActionType);      break;
                case JOBTYPE_IMAGE_CONVERT_PNG          :
                case JOBTYPE_IMAGE_CONVERT_MULTPNG      :   s_ConvertIMG(&MediaList,"png",ActionType);      break;
            }
        }

        // delete menu
        while (ContextMenu->actions().count()) delete ContextMenu->actions().takeLast();
        delete ContextMenu;
    }
    ui->WizardBt->setDown(false);
}

//====================================================================================================================

void MainWindow::s_ConvertIMG(QList<cBaseMediaFile*>*MediaList,QString DestFormat,int JobType) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_ConvertIMG";
    #endif

    cJob *Job=new cJob();
    Job->JobType=JobType;
    Job->DateTime=QDateTime::currentDateTime();
    Job->Command=DestFormat;
    for (int i=0;i<MediaList->count();i++) {
        cBaseMediaFile *Media=(cBaseMediaFile *)MediaList->at(i);
        if (Media->FileExtension!=DestFormat) Job->SourcesAndDests.append(Media->FileName+"##-##"+Media->FileName.left(Media->FileName.length()-Media->FileExtension.length())+DestFormat);
    }
    JobQueue.List.append(Job);
    ui->JobTable->DoRefreshList();
}
