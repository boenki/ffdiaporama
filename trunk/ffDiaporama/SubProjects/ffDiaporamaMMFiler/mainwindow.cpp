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

#include "../VariousWidgets/DlgCheckConfig.h"
#include "../VariousClass/cBaseMediaFile.h"
#include "../VariousClass/QCustomFolderTable.h"
#include "../VariousClass/QCustomFolderTree.h"
#include "../VariousClass/QCustomHorizSplitter.h"

#include "DlgApplicationSettings.h"
#include "DlgAbout.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//#define DEBUGMODE

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::MainWindow";
    #endif

    ApplicationConfig   =new cApplicationConfig(this);
    DriveList           =new cDriveList();
    IsFirstInitDone     =false;
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
    ui->FolderTable->CurrentFilter      =ApplicationConfig->CurrentFilter;

    // do some init ...

    ApplicationConfig->MainWinWSP->ApplyToWindow(this);     // Restore window position
    if (ApplicationConfig->SplitterSizeAndPos!="") ui->Splitter->restoreState(QByteArray::fromHex(ApplicationConfig->SplitterSizeAndPos.toUtf8()));

    // Initialise integrated controls and list
    DriveList->UpdateDriveList();
    ui->FolderTree->InitDrives(DriveList);
    ui->FolderTable->SetMode(DriveList,DISPLAY_DATA,ApplicationConfig->CurrentFilter);

    connect(ui->FolderTree,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(s_currentTreeItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
    connect(ui->FolderTable,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(s_currentTableItemChanged(QTableWidgetItem *,QTableWidgetItem *)));

    connect(ui->RefreshBt,SIGNAL(pressed()),this,SLOT(s_Refresh()));
    connect(ui->ActionConfiguration_BT,SIGNAL(pressed()),this,SLOT(s_Config()));

    connect(ui->Action_About_BT,SIGNAL(pressed()),this,SLOT(s_About()));
    connect(ui->ActionDocumentation_BT,SIGNAL(pressed()),this,SLOT(s_Documentation()));
    connect(ui->ActionNewFunctions_BT,SIGNAL(pressed()),this,SLOT(s_NewFunctions()));
    connect(ui->Action_Exit_BT,SIGNAL(pressed()),this,SLOT(s_action_Exit()));

    connect(ui->Action_Filter_BT,SIGNAL(pressed()),this,SLOT(s_action_Filter()));

    ui->FolderTree->SetSelectItemByPath(ui->FolderTree->RealPathToTreePath(ApplicationConfig->CurrentPath));
    if (ui->FolderTree->GetCurrentFolderPath()!=ApplicationConfig->CurrentPath) ui->FolderTree->SetSelectItemByPath(QApplication::translate("QCustomFolderTree","Personal folder"));

    if (ApplicationConfig->CheckConfigAtStartup) QTimer::singleShot(500,this,SLOT(s_DlgCheckConfig())); else {
        QString Status;
        if ((!CheckExiv2(Status,ApplicationConfig))||(!Checkffmpeg(Status,ApplicationConfig))) QTimer::singleShot(500,this,SLOT(s_DlgCheckConfig()));
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
}

//====================================================================================================================

void MainWindow::s_currentTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_currentTreeItemChanged";
    #endif

    ApplicationConfig->CurrentPath=ui->FolderTree->GetFolderPath(current,false);
    ui->FolderTree->RefreshItemByPath(ui->FolderTree->GetFolderPath(current,true),false);
    ui->CurrentPathED->setText(ApplicationConfig->CurrentPath);
    ui->FolderIcon->setPixmap(DriveList->GetFolderIcon(ApplicationConfig->CurrentPath).pixmap(48,48));
    cDriveDesc *HDD=ui->FolderTree->SearchRealDrive(ApplicationConfig->CurrentPath);
    int NbrFiles=ui->FolderTable->FillListFolder(ApplicationConfig->CurrentPath,ApplicationConfig,ApplicationConfig->CurrentFilter);
    if (HDD) {
        // Ensure Used and Size fit in an _int32 value for QProgressBar
        qlonglong Used=HDD->Used,Size=HDD->Size;
        while (Used>1024*1024) { Used=Used/1024; Size=Size/1024; }
        ui->HDDSizePgr->setMaximum(Size);
        ui->HDDSizePgr->setValue(Used);
        ui->HDDSizePgr->setFormat(GetTextSize(HDD->Used)+"/"+GetTextSize(HDD->Size));
        ui->FolderInfoLabel->setText(QString("%1/%2").arg(ui->FolderTable->NbrFilesDisplayed()).arg(NbrFiles).trimmed()+QApplication::translate("QCustomFolderTree"," items"));
    } else {
        ui->HDDSizePgr->setMaximum(0);
        ui->HDDSizePgr->setValue(0);
        ui->HDDSizePgr->setFormat("%P%");
        ui->FolderInfoLabel->setText("");
    }
}

//====================================================================================================================

void MainWindow::s_currentTableItemChanged(QTableWidgetItem *,QTableWidgetItem *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_currentTableItemChanged";
    #endif

    cBaseMediaFile  *Media=NULL;
    if (ui->FolderTable->currentRow()>=0) {
        QString ShortName="";
        ShortName=ui->FolderTable->item(ui->FolderTable->currentRow(),0)->text();
        int     i=0;
        while ((i<ui->FolderTable->MediaList.count())&&(ui->FolderTable->MediaList[i]->ShortName!=ShortName)) i++;
        if ((i<ui->FolderTable->MediaList.count())&&(ui->FolderTable->MediaList[i]->ShortName==ShortName)) Media=ui->FolderTable->MediaList[i];
    }
    if (!Media) {
        ui->FileInfoLabel->IconLeft         =QIcon();
        ui->FileInfoLabel->TextLeftUpper    ="";
        ui->FileInfoLabel->TextLeftBottom   ="";
        ui->FileInfoLabel->TextRightUpper   ="";
    } else {
        ui->FileInfoLabel->IconLeft         =Media->Icon;
        ui->FileInfoLabel->TextLeftUpper    =Media->ShortName;
        ui->FileInfoLabel->TextLeftBottom   =Media->WEBInfo;
        ui->FileInfoLabel->TextRightUpper   =Media->GetTypeText()+" ("+Media->FileSizeText+")";
    }
    ui->FileInfoLabel->repaint();
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
    if ((!CheckExiv2(Status,ApplicationConfig))||(!Checkffmpeg(Status,ApplicationConfig))) {
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

