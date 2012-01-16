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

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::MainWindow";
    #endif

    ApplicationConfig     =new cApplicationConfig(this);
    DriveList             =new cDriveList();
    IsFirstInitDone       =false;
    Icon_DISPLAY_DATA_S   =QIcon("img/DISPLAY_DATA_S.png");
    Icon_DISPLAY_DATA     =QIcon("img/DISPLAY_DATA.png");
    Icon_DISPLAY_WEB_S    =QIcon("img/DISPLAY_WEB_S.png");
    Icon_DISPLAY_WEB      =QIcon("img/DISPLAY_WEB_S.png");
    Icon_DISPLAY_JUKEBOX_S=QIcon("img/DISPLAY_JUKEBOX_S.png");
    Icon_DISPLAY_JUKEBOX  =QIcon("img/DISPLAY_JUKEBOX.png");
}

//====================================================================================================================

MainWindow::~MainWindow() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::~MainWindow";
    #endif
    delete ui;
    delete ApplicationConfig;
    delete DriveList;
}

//====================================================================================================================

void MainWindow::InitWindow(QString ForceLanguage,QApplication *App) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::InitWindow";
    #endif

    ApplicationConfig->InitConfigurationValues(ForceLanguage,App);
    PreloadSystemIcons();

    // Register all formats and codecs for libavformat/libavcodec/etc ...
    ApplicationConfig->DeviceModelList.Initffmpeg();

    ui->setupUi(this);
    setWindowTitle(QString("%1 %2").arg(APPLICATION_NAME).arg(APPLICATION_VERSION));

    // Transfert settings to tree
    ui->FolderTree->ShowHidden          =ApplicationConfig->ShowHiddenFilesAndDir;
    ui->FolderTree->ShowMntDrive        =ApplicationConfig->ShowMntDrive;

    // Transfert settings to table
    ui->FolderTable->ShowHidden         =ApplicationConfig->ShowHiddenFilesAndDir;
    ui->FolderTable->ShowMntDrive       =ApplicationConfig->ShowMntDrive;
    ui->FolderTable->ShowFoldersFirst   =ApplicationConfig->ShowFoldersFirst;

    // do some init ...
    ui->Action_Mode_BT->setIcon(*GetIconMode());
    ui->FileInfoLabel->DisplayMode=DISPLAY_WEBLONG;
    ui->FileInfoLabel->setVisible((ApplicationConfig->CurrentMode!=DISPLAY_WEBSHORT)&&(ApplicationConfig->CurrentMode!=DISPLAY_WEBLONG));

    ApplicationConfig->MainWinWSP->ApplyToWindow(this);     // Restore window position
    if (ApplicationConfig->SplitterSizeAndPos!="") ui->Splitter->restoreState(QByteArray::fromHex(ApplicationConfig->SplitterSizeAndPos.toUtf8()));

    // Initialise integrated controls and list
    DriveList->UpdateDriveList();
    ui->FolderTree->InitDrives(DriveList);
    ui->FolderTable->SetMode(DriveList,ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);

    ui->ToolBoxNormal->setCurrentIndex(0);
    RefreshControls();

    connect(ui->FolderTree,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(s_currentTreeItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));

    connect(ui->FolderTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_currentTableItemChanged()));
    connect(ui->FolderTable,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(s_itemDoubleClicked(QTableWidgetItem *)));
    connect(ui->FolderTable,SIGNAL(DoubleClickEvent()),this,SLOT(s_itemDoubleClicked()));
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
        if (!Checkffmpeg(Status,ApplicationConfig)) QTimer::singleShot(500,this,SLOT(s_DlgCheckConfig()));
    }
}

//====================================================================================================================

void MainWindow::closeEvent(QCloseEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::closeEvent";
    #endif
    // Save configuration
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ApplicationConfig->MainWinWSP->SaveWindowState(this);
    ApplicationConfig->SplitterSizeAndPos=QString(QByteArray(ui->Splitter->saveState()).toHex());
    ApplicationConfig->SaveConfigurationFile();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void MainWindow::resizeEvent(QResizeEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::resizeEvent";
    #endif

    if ((ApplicationConfig->CurrentMode==DISPLAY_ICON48)||(ApplicationConfig->CurrentMode==DISPLAY_ICON100)) {
        ui->FolderTable->setUpdatesEnabled(false);
        ui->FolderTable->SetMode(this->DriveList,ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);
        s_currentTreeItemChanged(ui->FolderTree->currentItem(),NULL);
        ui->FolderTable->setUpdatesEnabled(true);
    }
}

//====================================================================================================================

void MainWindow::RefreshControls() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::RefreshControls";
    #endif

    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();

    ui->PlayBt->setEnabled((Media)&&((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)||(Media->ObjectType==OBJECTTYPE_THUMBNAIL)||(Media->ObjectType==OBJECTTYPE_FFDFILE)));
    ui->InfoBt->setEnabled((Media)&&((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)||(Media->ObjectType==OBJECTTYPE_THUMBNAIL)||(Media->ObjectType==OBJECTTYPE_FFDFILE)));
    ui->WizardBt->setEnabled((Media)&&((Media->ObjectType==OBJECTTYPE_IMAGEFILE)||(Media->ObjectType==OBJECTTYPE_VIDEOFILE)||(Media->ObjectType==OBJECTTYPE_MUSICFILE)));
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
        case DISPLAY_ICON100:   return &Icon_DISPLAY_JUKEBOX_S;
    }
    return NULL;
}

//====================================================================================================================

void MainWindow::s_currentTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_currentTreeItemChanged";
    #endif

    ApplicationConfig->CurrentPath=ui->FolderTree->GetFolderPath(current,false);
    ui->FileInfoLabel->SetupFileInfoLabel(NULL);

    ui->FolderTree->RefreshItemByPath(ui->FolderTree->GetFolderPath(current,true),false);
    DoRefreshFolderInfo();

    ui->CurrentPathED->setText(ApplicationConfig->CurrentPath);
    ui->FolderIcon->setPixmap(DriveList->GetFolderIcon(ApplicationConfig->CurrentPath).pixmap(48,48));
    ui->FolderTable->FillListFolder(ApplicationConfig->CurrentPath,ApplicationConfig);
}

//====================================================================================================================

void MainWindow::DoRefreshFolderInfo() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_currentTreeItemChanged";
    #endif
    DriveList->UpdateDriveList();   // To update free space on drive
    cDriveDesc *HDD=ui->FolderTree->SearchRealDrive(ApplicationConfig->CurrentPath);
    if (HDD) {
        if (ui->FolderTable->ScanMediaList.isRunning()) {
            ui->HDDSizePgr->setMaximum(0);
            ui->HDDSizePgr->setValue(0);
            ui->HDDSizePgr->setFormat("%P%");
            ui->HDDSizePgr->setAlignment(Qt::AlignHCenter);
            ui->FolderInfoLabel->setText("");
        } else {
            // Ensure Used and Size fit in an _int32 value for QProgressBar
            qlonglong Used=HDD->Used,Size=HDD->Size;
            while (Used>1024*1024) { Used=Used/1024; Size=Size/1024; }
            ui->HDDSizePgr->setMaximum(Size);
            ui->HDDSizePgr->setValue(Used);
            ui->HDDSizePgr->setFormat(GetTextSize(HDD->Used)+"/"+GetTextSize(HDD->Size));
            ui->HDDSizePgr->setAlignment(Qt::AlignHCenter);
        }
        ui->FolderInfoLabel->setText(QString("%1/%2").arg(ui->FolderTable->CurrentShowFilesNumber).arg(ui->FolderTable->CurrentTotalFilesNumber)+" "+QApplication::translate("MainWindow","files")+" - "+
                                     QString("%1").arg(ui->FolderTable->CurrentShowFolderNumber)+" "+QApplication::translate("MainWindow","folders")+" - "+
                                     QApplication::translate("MainWindow","Total size:")+QString("%1/%2").arg(GetTextSize(ui->FolderTable->CurrentShowFolderSize)).arg(GetTextSize(ui->FolderTable->CurrentTotalFolderSize))+" - "+
                                     QApplication::translate("MainWindow","Total duration:")+ui->FolderTable->CurrentShowDuration.toString("HH:mm:ss"));
    } else {
        ui->HDDSizePgr->setMaximum(0);
        ui->HDDSizePgr->setValue(0);
        ui->HDDSizePgr->setFormat("%P%");
        ui->HDDSizePgr->setAlignment(Qt::AlignHCenter);
        ui->FolderInfoLabel->setText("");
    }
}

//====================================================================================================================

void MainWindow::s_currentTableItemChanged() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_currentTableItemChanged";
    #endif

    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();
    if (ui->FileInfoLabel->isVisible()) {
        ui->FileInfoLabel->SetupFileInfoLabel(Media);
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
        ApplicationConfig->MainWinWSP->SaveWindowState(this);
        ApplicationConfig->SplitterSizeAndPos=QString(QByteArray(ui->Splitter->saveState()).toHex());
        ApplicationConfig->SaveConfigurationFile();
        QApplication::restoreOverrideCursor();
        // Transfert settings to tree
        ui->FolderTree->ShowHidden          =ApplicationConfig->ShowHiddenFilesAndDir;
        ui->FolderTree->ShowMntDrive        =ApplicationConfig->ShowMntDrive;

        // Transfert settings to table
        ui->FolderTable->ShowHidden         =ApplicationConfig->ShowHiddenFilesAndDir;
        ui->FolderTable->ShowMntDrive       =ApplicationConfig->ShowMntDrive;
        ui->FolderTable->ShowFoldersFirst   =ApplicationConfig->ShowFoldersFirst;
        ui->FolderTable->CurrentMode        =ApplicationConfig->CurrentMode;
        ui->FolderTable->CurrentFilter      =ApplicationConfig->CurrentFilter;

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
    if (!Checkffmpeg(Status,ApplicationConfig)) {
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

QAction *MainWindow::CreateMenuAction(QIcon Icon,QString Text,int Data,bool Checkable,bool IsCheck) {
    QAction *Action;
    Action=new QAction(Icon,Text,this);
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

    // Exec menu
    QAction *Action=ContextMenu->exec(QCursor::pos());
    if ((Action)&&(ApplicationConfig->CurrentMode!=Action->data().toInt())) {
        ApplicationConfig->CurrentMode=Action->data().toInt();
        ui->Action_Mode_BT->setIcon(*GetIconMode());
        ui->FileInfoLabel->setVisible((ApplicationConfig->CurrentMode!=DISPLAY_WEBSHORT)&&(ApplicationConfig->CurrentMode!=DISPLAY_WEBLONG));
        ui->FolderTable->SetMode(this->DriveList,ApplicationConfig->CurrentMode,ApplicationConfig->CurrentFilter);
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
    ContextMenu->addAction(CreateMenuAction(DefaultFILEIcon, QApplication::translate("MainWindow","All files"),                OBJECTTYPE_UNMANAGED,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_UNMANAGED));
    ContextMenu->addAction(CreateMenuAction(DefaultFILEIcon, QApplication::translate("MainWindow","Managed files"),            OBJECTTYPE_MANAGED,  true,ApplicationConfig->CurrentFilter==OBJECTTYPE_MANAGED));
    ContextMenu->addAction(CreateMenuAction(DefaultIMAGEIcon,QApplication::translate("MainWindow","Image files"),              OBJECTTYPE_IMAGEFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_IMAGEFILE));
    ContextMenu->addAction(CreateMenuAction(DefaultVIDEOIcon,QApplication::translate("MainWindow","Video files"),              OBJECTTYPE_VIDEOFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_VIDEOFILE));
    ContextMenu->addAction(CreateMenuAction(DefaultMUSICIcon,QApplication::translate("MainWindow","Music files"),              OBJECTTYPE_MUSICFILE,true,ApplicationConfig->CurrentFilter==OBJECTTYPE_MUSICFILE));
    ContextMenu->addAction(CreateMenuAction(DefaultFFDIcon,  QApplication::translate("MainWindow","ffDiaporama project files"),OBJECTTYPE_FFDFILE,  true,ApplicationConfig->CurrentFilter==OBJECTTYPE_FFDFILE));

    // Exec menu
    QAction *Action=ContextMenu->exec(QCursor::pos());
    if ((Action)&&(ApplicationConfig->CurrentFilter!=Action->data().toInt())) {
        ApplicationConfig->CurrentFilter=Action->data().toInt();
        ui->FolderTable->SetMode(this->DriveList,ui->FolderTable->CurrentMode,ApplicationConfig->CurrentFilter);
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

void MainWindow::s_ActionFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_ActionFile";
    #endif

    ui->WizardBt->setDown(false);
}

//====================================================================================================================

void MainWindow::s_itemDoubleClicked() {
    s_itemDoubleClicked(NULL);
}

void MainWindow::s_itemDoubleClicked(QTableWidgetItem *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_itemDoubleClicked";
    #endif
    s_OpenFile();
}
