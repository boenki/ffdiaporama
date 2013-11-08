/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

#include "DlgFileExplorer.h"
#include "ui_DlgFileExplorer.h"
#include "../../CustomCtrl/QCustomFolderTable.h"
#include "../../CustomCtrl/QCustomFolderTree.h"
#include "../DlgInfoFile/DlgInfoFile.h"
#include "../DlgManageFavorite/DlgManageFavorite.h"

#define LATENCY 5

DlgFileExplorer::DlgFileExplorer(int AllowedFilter,int CurrentFilter,bool AllowMultipleSelection,bool AllowDragDrop,
                QString StartupPath,QString TheBoxTitle,cBaseApplicationConfig *ApplicationConfig,QWidget *parent):
                QCustomDialog(ApplicationConfig,parent),ui(new Ui::DlgFileExplorer) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::DlgFileExplorer");

    // Initialise UI
    ui->setupUi(this);

    TypeWindowState         =TypeWindowState_withsplitterpos;
    Splitter                =ui->BrowserWidget;
    CurrentPath             =StartupPath;
    DlgWorkingTaskDialog    =NULL;
    CancelAction            =false;
    CurrentDriveCheck       =0;

    OkBt        =ui->OKBT;
    CancelBt    =ui->CancelBt;
    HelpBt      =ui->HelpBT;
    HelpFile    ="0109";
    UndoBt      =NULL;

    BoxTitle    =TheBoxTitle;
    setWindowTitle(BoxTitle);

    // Initialise integrated browser
    ui->FolderTable->AllowedFilter          =AllowedFilter;
    ui->FolderTable->ShowHiddenFilesAndDir  =BaseApplicationConfig->ShowHiddenFilesAndDir;
    ui->FolderTable->DisplayFileName        =BaseApplicationConfig->DisplayFileName;

    ui->FolderTree->ApplicationConfig       =BaseApplicationConfig;
    ui->FolderTable->ApplicationConfig      =BaseApplicationConfig;

    ui->FolderTree->IsRemoveAllowed         =false;
    ui->FolderTree->IsRenameAllowed         =false;
    ui->FolderTree->IsCreateFolderAllowed   =false;

    BaseApplicationConfig->DriveList->UpdateDriveList();
    ui->FolderTree->InitDrives();

    ui->RefreshBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload));
    ui->UpFolderBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogToParent));
    ui->PreviousFolderBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowBack));

    ui->FolderTable->SetMode(BaseApplicationConfig->CurrentMode,CurrentFilter);
    ui->FolderTable->setDragDropMode(AllowDragDrop?QAbstractItemView::DragOnly:QAbstractItemView::NoDragDrop);
    ui->FolderTable->setSelectionMode(AllowMultipleSelection?QAbstractItemView::ExtendedSelection:QAbstractItemView::SingleSelection);
}

//====================================================================================================================

DlgFileExplorer::~DlgFileExplorer() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::~DlgFileExplorer");
    delete ui;
}

//====================================================================================================================

void DlgFileExplorer::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::DoInitDialog");

    ui->actionBrowserOpen->setIconVisibleInMenu(true);
    ui->actionBrowserProperties->setIconVisibleInMenu(true);
    ui->actionAddToFavorite->setIconVisibleInMenu(true);
    ui->actionManageFavorite->setIconVisibleInMenu(true);

    // Browser event
    connect(ui->FolderTree,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(s_Browser_FolderTreeItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
    connect(ui->FolderTable,SIGNAL(itemSelectionChanged()),this,SLOT(DoBrowserRefreshSelectedFileInfo()));
    connect(ui->PreviousFolderBt,SIGNAL(released()),this,SLOT(s_Browser_SetToPrevious()));
    connect(ui->UpFolderBt,SIGNAL(released()),this,SLOT(s_Browser_SetToUpper()));
    connect(ui->FolderTree,SIGNAL(ActionRefreshAll()),this,SLOT(s_Browser_RefreshAll()));
    connect(ui->FolderTree,SIGNAL(ActionRefreshHere()),this,SLOT(s_Browser_RefreshHere()));
    connect(ui->FolderTable,SIGNAL(RefreshFolderInfo()),this,SLOT(DoBrowserRefreshFolderInfo()));
    connect(ui->FolderTable,SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(s_Browser_DoubleClicked(QMouseEvent *)));
    connect(ui->FolderTable,SIGNAL(Refresh()),this,SLOT(s_Browser_RefreshAll()));
    connect(ui->FolderTable,SIGNAL(RightClickEvent(QMouseEvent *)),this,SLOT(s_Browser_RightClicked(QMouseEvent *)));
    connect(ui->actionBrowserOpen,SIGNAL(triggered()),this,SLOT(s_Browser_OpenFolder()));
    connect(ui->actionBrowserProperties,SIGNAL(triggered()),this,SLOT(s_Browser_Properties()));
    connect(ui->RefreshBt,SIGNAL(released()),this,SLOT(s_Browser_RefreshAll()));
    connect(ui->FavoriteBt,SIGNAL(pressed()),this,SLOT(s_Browser_Favorite()));
    connect(ui->actionAddToFavorite,SIGNAL(triggered()),this,SLOT(s_Browser_AddToFavorite()));
    connect(ui->actionManageFavorite,SIGNAL(triggered()),this,SLOT(s_Browser_ManageFavorite()));
    connect(ui->ActionModeBt,SIGNAL(pressed()),this,SLOT(s_Browser_ChangeDisplayMode()));

    if (CurrentPath!="") ui->FolderTree->SetSelectItemByPath(ui->FolderTree->RealPathToTreePath(CurrentPath));
    if ((CurrentPath=="")||(ui->FolderTree->GetCurrentFolderPath()!=CurrentPath)) {
        CurrentPath=BaseApplicationConfig->CurrentPath;
        ui->FolderTree->SetSelectItemByPath(CurrentPath);
    }
}

//====================================================================================================================

void DlgFileExplorer::PrepareGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::PrepareGlobalUndo");
}

//====================================================================================================================

void DlgFileExplorer::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::DoGlobalUndo");
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_FolderTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_FolderTreeItemChanged");

    CurrentPath=ui->FolderTree->GetFolderPath(current,false);

    ui->FolderTree->RefreshItemByPath(ui->FolderTree->GetFolderPath(current,true),false);
    ui->CurrentPathED->setText(CurrentPath);
    ui->FolderIcon->setPixmap(BaseApplicationConfig->DriveList->GetFolderIcon(CurrentPath).pixmap(16,16));

    QString Path=CurrentPath;
    #ifdef Q_OS_WIN
        Path.replace("%HOMEDRIVE%%HOMEPATH%",BaseApplicationConfig->DriveList->List[0].Path,Qt::CaseInsensitive);
        Path.replace("%USERPROFILE%",BaseApplicationConfig->DriveList->List[0].Path,Qt::CaseInsensitive);
        Path=QDir::toNativeSeparators(Path);
        if (QDir(Path).canonicalPath()!="") Path=QDir(Path).canonicalPath(); // Resolved eventual .lnk files
    #endif
    ui->FolderTable->FillListFolder(Path);
    DoBrowserRefreshFolderInfo();
    //RefreshControls();
}

//====================================================================================================================

void DlgFileExplorer::DoBrowserRefreshFolderInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_FolderTreeItemChanged");
    BaseApplicationConfig->DriveList->UpdateDriveList();   // To update free space on drive
    cDriveDesc *HDD=ui->FolderTree->SearchRealDrive(CurrentPath);
    if (HDD) {
        // If scan in progress
        if (ui->FolderTable->ScanMediaListProgress) {
            ui->FolderPgr->setMaximum(0);
            ui->FolderPgr->setValue(0);
            ui->FolderPgr->setFormat("%P%");
            ui->FolderPgr->setAlignment(Qt::AlignHCenter);
            ui->FolderDuration->setText("--:--:--.---");

        // If scan is finished
        } else {
            // Ensure Used and Size fit in an _int32 value for QProgressBar
            int64_t Used=HDD->Used,Size=HDD->Size;
            while (Used>1024*1024) { Used=Used/1024; Size=Size/1024; }
            ui->FolderPgr->setMaximum(Size);
            ui->FolderPgr->setValue(Used);
            ui->FolderPgr->setFormat(GetTextSize(HDD->Used)+"/"+GetTextSize(HDD->Size));
            ui->FolderPgr->setAlignment(Qt::AlignHCenter);

            int64_t   duration=ui->FolderTable->CurrentShowDuration;
            int         msec =duration % 1000;          duration=duration/1000;
            int         sec  =duration % 60;            duration=duration/60;
            int         mn   =duration % 60;            duration=duration/60;
            int         hours=duration % 24;
            int         days =duration / 24;
            if (days>0) ui->FolderDuration->setText(QString("%1.%2:%3:%4.%5").arg(ito2a(days)).arg(ito2a(hours)).arg(ito2a(mn)).arg(ito2a(sec)).arg(ito3a(msec)));
                else    ui->FolderDuration->setText(QString("%1:%2:%3.%4").arg(ito2a(hours)).arg(ito2a(mn)).arg(ito2a(sec)).arg(ito3a(msec)));
        }

        ui->NbrFiles->setText(QString("%1").arg(ui->FolderTable->CurrentShowFilesNumber));
        ui->NbrFolders->setText(QString("%1").arg(ui->FolderTable->CurrentShowFolderNumber));
        ui->FolderSize->setText(QString("%1").arg(GetTextSize(ui->FolderTable->CurrentShowFolderSize)));

    } else {
        ui->FolderPgr->setMaximum(0);
        ui->FolderPgr->setValue(0);
        ui->FolderPgr->setFormat("%P%");
        ui->FolderPgr->setAlignment(Qt::AlignHCenter);
        ui->NbrFiles->setText("");
        ui->NbrFolders->setText("");
        ui->FolderSize->setText("");
        ui->FolderDuration->setText("");
    }
    DoBrowserRefreshSelectedFileInfo();
}

//====================================================================================================================

void DlgFileExplorer::DoBrowserRefreshSelectedFileInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::DoBrowserRefreshSelectedFileInfo");

    QList<cBaseMediaFile*> MediaList;
    ui->FolderTable->GetCurrentSelectedMediaFile(&MediaList);

    if (MediaList.count()==0) {
        // No selection

        ui->FileIcon->setPixmap(QPixmap());
        ui->FileInfo1a->setText("");
        ui->FileInfo2a->setText("");
        ui->FileInfo3a->setText("");

    } else if (MediaList.count()==1) {
        // One file selection

        cBaseMediaFile *MediaObject=MediaList[0];
        QStringList     TempExtProperties;
        BaseApplicationConfig->FilesTable->GetExtendedProperties(MediaObject->FileKey,&TempExtProperties);
        ui->FileIcon->setPixmap(QPixmap().fromImage(MediaObject->GetIcon(cCustomIcon::ICON100,true).scaledToHeight(48,Qt::SmoothTransformation)));

        QString FStr=MediaObject->GetFileSizeStr();
        if (FStr!="") ui->FileInfo1a->setText(QString("%1 (%2)").arg(MediaObject->ShortName()).arg(FStr));
            else ui->FileInfo1a->setText(MediaObject->ShortName());
        if (QTime(0,0,0,0).msecsTo(MediaObject->Duration)>0) ui->FileInfo2a->setText(QString("%1-%2").arg(MediaObject->GetTechInfo(&TempExtProperties)).arg(MediaObject->Duration.toString("HH:mm:ss.zzz")));
            else ui->FileInfo2a->setText(MediaObject->GetTechInfo(&TempExtProperties));
        ui->FileInfo3a->setText(MediaObject->GetTAGInfo(&TempExtProperties));

    } else if (MediaList.count()>1) {
        // Multi file select

        // Do qualification of files
        bool    IsFind;
        QStringList FileExtensions;
        QList<int>  ObjectTypes;
        int64_t   TotalDuration=0;
        int64_t   TotalSize    =0;

        for (int i=0;i<MediaList.count();i++) {
            QString FileExtension=QFileInfo(MediaList[i]->FileName()).completeSuffix();
            IsFind=false;   for (int j=0;j<ObjectTypes.count();j++)     if (MediaList[i]->ObjectType==ObjectTypes[j])   IsFind=true; if (!IsFind) ObjectTypes.append(MediaList[i]->ObjectType);
            IsFind=false;   for (int j=0;j<FileExtensions.count();j++)  if (FileExtension==FileExtensions[j])           IsFind=true; if (!IsFind) FileExtensions.append(FileExtension);

            if ((MediaList[i]->ObjectType==OBJECTTYPE_MUSICFILE)||(MediaList[i]->ObjectType==OBJECTTYPE_VIDEOFILE)||(MediaList[i]->ObjectType==OBJECTTYPE_FFDFILE))
                TotalDuration=TotalDuration+QTime(0,0,0,0).msecsTo(MediaList[i]->Duration);

            TotalSize=TotalSize+MediaList[i]->FileSize;
        }

        if (TotalDuration!=0) {
            int         msec =TotalDuration % 1000;          TotalDuration=TotalDuration/1000;
            int         sec  =TotalDuration % 60;            TotalDuration=TotalDuration/60;
            int         mn   =TotalDuration % 60;            TotalDuration=TotalDuration/60;
            int         hours=TotalDuration % 24;
            int         days =TotalDuration / 24;
            if (days>0) ui->FileInfo2a->setText(QApplication::translate("MainWindow","Total duration:")+QString("%1.%2:%3:%4.%5").arg(ito2a(days)).arg(ito2a(hours)).arg(ito2a(mn)).arg(ito2a(sec)).arg(ito3a(msec)));
                else    ui->FileInfo2a->setText(QApplication::translate("MainWindow","Total duration:")+QString("%1:%2:%3.%4").arg(ito2a(hours)).arg(ito2a(mn)).arg(ito2a(sec)).arg(ito3a(msec)));
        } else ui->FileInfo2a->setText("");

        if (TotalSize!=0) ui->FileInfo3a->setText(QApplication::translate("MainWindow","Total size:%1").arg(GetTextSize(TotalSize)));
            else          ui->FileInfo3a->setText("");

        if (ObjectTypes.count()==1) {
            switch (ObjectTypes[0]) {
                case OBJECTTYPE_MUSICFILE :
                    ui->FileIcon->setPixmap(QPixmap().fromImage(BaseApplicationConfig->DefaultMUSICIcon.Icon100.scaledToHeight(48,Qt::SmoothTransformation)));
                    ui->FileInfo1a->setText(QApplication::translate("MainWindow","%1 audio files").arg(MediaList.count()));
                    break;
                case OBJECTTYPE_VIDEOFILE :
                    ui->FileIcon->setPixmap(QPixmap().fromImage(BaseApplicationConfig->DefaultVIDEOIcon.Icon100.scaledToHeight(48,Qt::SmoothTransformation)));
                    ui->FileInfo1a->setText(QApplication::translate("MainWindow","%1 video files").arg(MediaList.count()));
                    break;
                case OBJECTTYPE_IMAGEFILE :
                    ui->FileIcon->setPixmap(QPixmap().fromImage(BaseApplicationConfig->DefaultIMAGEIcon.Icon100.scaledToHeight(48,Qt::SmoothTransformation)));
                    ui->FileInfo1a->setText(QApplication::translate("MainWindow","%1 image files").arg(MediaList.count()));
                    break;
                case OBJECTTYPE_FFDFILE   :
                    ui->FileIcon->setPixmap(QPixmap().fromImage(BaseApplicationConfig->DefaultFFDIcon.Icon100.scaledToHeight(48,Qt::SmoothTransformation)));
                    ui->FileInfo1a->setText(QApplication::translate("MainWindow","%1 ffDiaporama project files").arg(MediaList.count()));
                    break;
                case OBJECTTYPE_FOLDER    :
                    ui->FileIcon->setPixmap(QPixmap().fromImage(BaseApplicationConfig->DefaultFOLDERIcon.Icon100.scaledToHeight(48,Qt::SmoothTransformation)));
                    ui->FileInfo1a->setText(QApplication::translate("MainWindow","%1 folders").arg(MediaList.count()));
                    break;
                default:
                    ui->FileIcon->setPixmap(QPixmap());
                    ui->FileInfo1a->setText(QApplication::translate("MainWindow","%1 files").arg(MediaList.count()));
                    ui->FileInfo2a->setText(QApplication::translate("MainWindow","Multiple file types"));
                    break;
            }


        } else {
            ui->FileIcon->setPixmap(QPixmap());
            ui->FileInfo1a->setText(QApplication::translate("MainWindow","%1 files").arg(MediaList.count()));
            ui->FileInfo2a->setText(QApplication::translate("MainWindow","Multiple file types"));
        }
    }
    while (!MediaList.isEmpty()) delete MediaList.takeLast();
    //RefreshControls();
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_SetToPrevious() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_SetToPrevious");
    QString Path=ui->FolderTable->BrowseToPreviousPath();
    if (Path!="") ui->FolderTree->SetSelectItemByPath(Path);
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_SetToUpper() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_SetToUpper");
    QString Path=ui->FolderTable->BrowseToUpperPath();
    if (Path!="") ui->FolderTree->SetSelectItemByPath(Path);
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_RefreshAll() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_RefreshAll");
    CurrentDriveCheck=0;
    CancelAction     =false;
    if (DlgWorkingTaskDialog) {
        DlgWorkingTaskDialog->close();
        delete DlgWorkingTaskDialog;
        DlgWorkingTaskDialog=NULL;
    }
    DlgWorkingTaskDialog=new DlgWorkingTask(QApplication::translate("MainWindow","Refresh All"),&CancelAction,BaseApplicationConfig,this);
    DlgWorkingTaskDialog->InitDialog();
    DlgWorkingTaskDialog->SetMaxValue(BaseApplicationConfig->DriveList->List.count()+2,0);
    DlgWorkingTaskDialog->DisplayText(QApplication::translate("MainWindow","update drive list"));
    QTimer::singleShot(LATENCY,this,SLOT(s_Browser_RefreshDriveList()));
    DlgWorkingTaskDialog->exec();
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_RefreshDriveList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_RefreshDriveList");
    // Refresh drive list
    ui->FolderTree->RefreshDriveList();
    DlgWorkingTaskDialog->SetMaxValue(BaseApplicationConfig->DriveList->List.count()+2,0);
    DlgWorkingTaskDialog->DisplayText(QApplication::translate("MainWindow","update drive (%1)").arg(BaseApplicationConfig->DriveList->List[CurrentDriveCheck].Label));
    DlgWorkingTaskDialog->DisplayProgress(1+BaseApplicationConfig->DriveList->List.count()-CurrentDriveCheck);
    QTimer::singleShot(LATENCY,this,SLOT(s_Browser_RefreshDrive()));
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_RefreshDrive() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_RefreshDrive");
    if (CurrentDriveCheck<BaseApplicationConfig->DriveList->List.count()) ui->FolderTree->RefreshItemByPath(BaseApplicationConfig->DriveList->List[CurrentDriveCheck].Label,true);
    CurrentDriveCheck++;
    if ((!CancelAction)&&(CurrentDriveCheck<BaseApplicationConfig->DriveList->List.count())) {
        DlgWorkingTaskDialog->DisplayText(QApplication::translate("MainWindow","update drive (%1)").arg(BaseApplicationConfig->DriveList->List[CurrentDriveCheck].Label));
        DlgWorkingTaskDialog->DisplayProgress(1+BaseApplicationConfig->DriveList->List.count()-CurrentDriveCheck);
        QTimer::singleShot(LATENCY,this,SLOT(s_Browser_RefreshDrive()));
    } else {
        DlgWorkingTaskDialog->DisplayText(QApplication::translate("MainWindow","update current folder"));
        DlgWorkingTaskDialog->DisplayProgress(DlgWorkingTaskDialog->MaxValue);
        QTimer::singleShot(LATENCY,this,SLOT(s_Browser_RefreshHere()));
    }
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_RefreshHere() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_RefreshHere");
    s_Browser_FolderTreeItemChanged(ui->FolderTree->currentItem(),NULL);
    if (DlgWorkingTaskDialog) {
        DlgWorkingTaskDialog->close();
        delete DlgWorkingTaskDialog;
        DlgWorkingTaskDialog=NULL;
    }
}

//====================================================================================================================

bool DlgFileExplorer::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_OpenFolder");
    QList<cBaseMediaFile*> MediaList;
    ui->FolderTable->GetCurrentSelectedMediaFile(&MediaList);
    bool HaveFolder=false;
    bool Ret=true;
    for (int i=0;i<MediaList.count();i++) if (MediaList[i]->ObjectType==OBJECTTYPE_FOLDER) HaveFolder=true;
    if (MediaList.count()==0) Ret=false; else if (HaveFolder) {
        if (MediaList.count()==1) {
            s_Browser_OpenFolder();
            Ret=false;
        } else {
            CustomMessageBox(this,QMessageBox::Information,BoxTitle,QApplication::translate("DlgFileExplorer","The selection can't include folders."),QMessageBox::Ok,QMessageBox::Ok);
            Ret=false;
        }
    }
    while (!MediaList.isEmpty()) delete MediaList.takeLast();
    return Ret;
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_DoubleClicked(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_DoubleClicked");
    s_Browser_OpenFolder();
}

void DlgFileExplorer::s_Browser_OpenFolder() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_OpenFolder");
    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();
    if (Media) {
        if (Media->ObjectType==OBJECTTYPE_FOLDER) {
            QString Path=ui->FolderTree->GetCurrentFolderPath();
            if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
            Path=Path+Media->ShortName();
            ui->FolderTree->SetSelectItemByPath(Path);
        } else accept();
        delete Media;
    }
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_RightClicked(QMouseEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_RightClicked");

    QList<cBaseMediaFile*> MediaList;
    ui->FolderTable->GetCurrentSelectedMediaFile(&MediaList);
    if (MediaList.count()==0) return;

    bool    Multiple=(MediaList.count()>1);
    bool    IsFind;

    // Do qualification of files
    QStringList FileExtensions;
    QList<int>  ObjectTypes;

    for (int i=0;i<MediaList.count();i++) {
        QString FileExtension=QFileInfo(MediaList[i]->FileName()).completeSuffix();
        IsFind=false;   for (int j=0;j<ObjectTypes.count();j++)     if (MediaList[i]->ObjectType==ObjectTypes[j])   IsFind=true; if (!IsFind) ObjectTypes.append(MediaList[i]->ObjectType);
        IsFind=false;   for (int j=0;j<FileExtensions.count();j++)  if (FileExtension==FileExtensions[j])           IsFind=true; if (!IsFind) FileExtensions.append(FileExtension);
    }

    // Single object type only
    if (((ObjectTypes.count()==1)&&(ObjectTypes[0]!=OBJECTTYPE_UNMANAGED))||((ObjectTypes.count()==2)&&((ObjectTypes[0]==OBJECTTYPE_VIDEOFILE)||(ObjectTypes[0]==OBJECTTYPE_IMAGEFILE))&&((ObjectTypes[1]==OBJECTTYPE_VIDEOFILE)||(ObjectTypes[1]==OBJECTTYPE_IMAGEFILE)))) {
        QMenu *ContextMenu=new QMenu(this);
        switch (ObjectTypes[0]) {
            case OBJECTTYPE_MUSICFILE :
            case OBJECTTYPE_VIDEOFILE :
            case OBJECTTYPE_IMAGEFILE :
            case OBJECTTYPE_FFDFILE   :
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserProperties);
                break;
            case OBJECTTYPE_FOLDER    :
                if (!Multiple)  ContextMenu->addAction(ui->actionBrowserOpen);
                break;
        }
        ContextMenu->exec(QCursor::pos());
        delete ContextMenu;
    }
    while (!MediaList.isEmpty()) delete MediaList.takeLast();
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_Properties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_Properties");
    cBaseMediaFile *Media=ui->FolderTable->GetCurrentMediaFile();
    if (Media) {
        DlgInfoFile Dlg(Media,BaseApplicationConfig,this);
        Dlg.InitDialog();
        Dlg.exec();
        delete Media;
    }
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_Favorite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_Favorite");
    QMenu *ContextMenu=new QMenu(this);

    ContextMenu->addAction(ui->actionAddToFavorite);
    ContextMenu->addSeparator();
    for (int i=0;i<BaseApplicationConfig->BrowserFavorites.count();i++) {
        QStringList Texts=BaseApplicationConfig->BrowserFavorites[i].split("###");
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
    ui->FavoriteBt->setDown(false);
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_AddToFavorite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_AddToFavorite");

    bool    Ok,Continue=true;
    QString Text=CurrentPath;
    while (Text.indexOf(QDir::separator())!=-1) Text=Text.mid(Text.indexOf(QDir::separator())+1);
    while (Continue) {
        Continue=false;
        Text=CustomInputDialog(this,QApplication::translate("MainWindow","Add to favorite"),QApplication::translate("MainWindow","Favorite name:"),QLineEdit::Normal,Text,&Ok);
        if (Ok && !Text.isEmpty()) {

            int i=0;
            while ((i<BaseApplicationConfig->BrowserFavorites.count())&&(!BaseApplicationConfig->BrowserFavorites[i].startsWith(Text+"###"))) i++;
            if ((i<BaseApplicationConfig->BrowserFavorites.count())&&(BaseApplicationConfig->BrowserFavorites[i].startsWith(Text+"###"))) {
                if (CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Add to favorite"),
                                          QApplication::translate("MainWindow","A favorite with this name already exists.\nDo you want to overwrite it?"),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)
                    BaseApplicationConfig->BrowserFavorites[i]=Text+"###"+CurrentPath;
                else Continue=true;

            } else {

                int i=0;
                while ((i<BaseApplicationConfig->BrowserFavorites.count())&&(!BaseApplicationConfig->BrowserFavorites[i].endsWith("###"+CurrentPath))) i++;
                if ((i<BaseApplicationConfig->BrowserFavorites.count())&&(BaseApplicationConfig->BrowserFavorites[i].endsWith("###"+CurrentPath))) {
                    if (CustomMessageBox(this,QMessageBox::Question,QApplication::translate("MainWindow","Add to favorite"),
                                              QApplication::translate("MainWindow","A favorite with for this path already exists.\nDo you want to overwrite it?"),
                                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)
                        BaseApplicationConfig->BrowserFavorites[i]=Text+"###"+CurrentPath;
                    else Continue=true;

                } else {
                    BaseApplicationConfig->BrowserFavorites.append(Text+"###"+CurrentPath);
                }
            }
        }
    }
    BaseApplicationConfig->BrowserFavorites.sort();
}

//====================================================================================================================

void DlgFileExplorer::s_Browser_ManageFavorite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_ManageFavorite");
    DlgManageFavorite Dlg(&BaseApplicationConfig->BrowserFavorites,BaseApplicationConfig,this);
    Dlg.InitDialog();
    Dlg.exec();
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

void DlgFileExplorer::s_Browser_ChangeDisplayMode() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::s_Browser_ChangeDisplayMode");

    // Create menu
    QMenu *ContextMenu=new QMenu(this);
    ContextMenu->addAction(CreateMenuAction(QIcon(":/img/DISPLAY_DATA.png"),   QApplication::translate("MainWindow","Details view"),ACTIONTYPE_DISPLAYMODE|DISPLAY_DATA,       true,ui->FolderTable->CurrentMode==DISPLAY_DATA));
    ContextMenu->addAction(CreateMenuAction(QIcon(":/img/DISPLAY_JUKEBOX.png"),QApplication::translate("MainWindow","Icon view"),   ACTIONTYPE_DISPLAYMODE|DISPLAY_ICON100,    true,ui->FolderTable->CurrentMode==DISPLAY_ICON100));
    ContextMenu->addSeparator();
    if ((ui->FolderTable->AllowedFilter&FILTERALLOW_OBJECTTYPE_UNMANAGED)!=0) ContextMenu->addAction(CreateMenuAction(BaseApplicationConfig->DefaultFILEIcon.GetIcon(cCustomIcon::ICON16), QApplication::translate("MainWindow","All files"),                 ACTIONTYPE_FILTERMODE|OBJECTTYPE_UNMANAGED,true,ui->FolderTable->CurrentFilter==OBJECTTYPE_UNMANAGED));
    if ((ui->FolderTable->AllowedFilter&FILTERALLOW_OBJECTTYPE_MANAGED)!=0)   ContextMenu->addAction(CreateMenuAction(BaseApplicationConfig->DefaultFILEIcon.GetIcon(cCustomIcon::ICON16), QApplication::translate("MainWindow","Managed files"),             ACTIONTYPE_FILTERMODE|OBJECTTYPE_MANAGED,  true,ui->FolderTable->CurrentFilter==OBJECTTYPE_MANAGED));
    if ((ui->FolderTable->AllowedFilter&FILTERALLOW_OBJECTTYPE_IMAGEFILE)!=0) ContextMenu->addAction(CreateMenuAction(BaseApplicationConfig->DefaultIMAGEIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Image files"),               ACTIONTYPE_FILTERMODE|OBJECTTYPE_IMAGEFILE,true,ui->FolderTable->CurrentFilter==OBJECTTYPE_IMAGEFILE));
    if ((ui->FolderTable->AllowedFilter&FILTERALLOW_OBJECTTYPE_VIDEOFILE)!=0) ContextMenu->addAction(CreateMenuAction(BaseApplicationConfig->DefaultVIDEOIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Video files"),               ACTIONTYPE_FILTERMODE|OBJECTTYPE_VIDEOFILE,true,ui->FolderTable->CurrentFilter==OBJECTTYPE_VIDEOFILE));
    if ((ui->FolderTable->AllowedFilter&FILTERALLOW_OBJECTTYPE_MUSICFILE)!=0) ContextMenu->addAction(CreateMenuAction(BaseApplicationConfig->DefaultMUSICIcon.GetIcon(cCustomIcon::ICON16),QApplication::translate("MainWindow","Music files"),               ACTIONTYPE_FILTERMODE|OBJECTTYPE_MUSICFILE,true,ui->FolderTable->CurrentFilter==OBJECTTYPE_MUSICFILE));
    if ((ui->FolderTable->AllowedFilter&FILTERALLOW_OBJECTTYPE_FFDFILE)!=0)   ContextMenu->addAction(CreateMenuAction(BaseApplicationConfig->DefaultFFDIcon.GetIcon(cCustomIcon::ICON16),  QApplication::translate("MainWindow","ffDiaporama project files"), ACTIONTYPE_FILTERMODE|OBJECTTYPE_FFDFILE,  true,ui->FolderTable->CurrentFilter==OBJECTTYPE_FFDFILE));
    ContextMenu->addSeparator();
    if (ui->FolderTable->ShowHiddenFilesAndDir) ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Visible_KO.png"),  QApplication::translate("MainWindow","Hide hidden files and folders"), ACTIONTYPE_ONOFFOPTIONS|ONOFFOPTIONS_HIDEHIDDEN,  true,false));
        else                   ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Visible_OK.png"),  QApplication::translate("MainWindow","Show hidden files and folders"), ACTIONTYPE_ONOFFOPTIONS|ONOFFOPTIONS_SHOWHIDDEN,  true,false));
    if (ui->FolderTable->CurrentMode==DISPLAY_ICON100) {
        if (ui->FolderTable->DisplayFileName)   ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Visible_KO.png"),  QApplication::translate("MainWindow","Hide files name"), ACTIONTYPE_ONOFFOPTIONS|ONOFFOPTIONS_HIDEFILENAME,  true,false));
            else               ContextMenu->addAction(CreateMenuAction(QIcon(":/img/Visible_OK.png"),  QApplication::translate("MainWindow","Show files name"), ACTIONTYPE_ONOFFOPTIONS|ONOFFOPTIONS_SHOWFILENAME,  true,false));
    }

    // Exec menu
    QAction *Action=ContextMenu->exec(QCursor::pos());
    if (Action) {
        int ActionType=Action->data().toInt() & ACTIONTYPE_ACTIONTYPE;
        int SubAction =Action->data().toInt() & (~ACTIONTYPE_ACTIONTYPE);
        if (ActionType==ACTIONTYPE_DISPLAYMODE) {
            if (ui->FolderTable->CurrentMode!=SubAction) {
                ui->FolderTable->CurrentMode=SubAction;
                ui->FolderTable->SetMode(ui->FolderTable->CurrentMode,ui->FolderTable->CurrentFilter);
                s_Browser_FolderTreeItemChanged(ui->FolderTree->currentItem(),NULL);
            }
        } else if (ActionType==ACTIONTYPE_FILTERMODE) {
            if (ui->FolderTable->CurrentFilter!=SubAction) {
                ui->FolderTable->CurrentFilter=SubAction;
                ui->FolderTable->SetMode(ui->FolderTable->CurrentMode,ui->FolderTable->CurrentFilter);
                s_Browser_FolderTreeItemChanged(ui->FolderTree->currentItem(),NULL);
            }
        } else if (ActionType==ACTIONTYPE_ONOFFOPTIONS) {
            if ((SubAction==ONOFFOPTIONS_SHOWHIDDEN)||(SubAction==ONOFFOPTIONS_HIDEHIDDEN)) {
                ui->FolderTable->ShowHiddenFilesAndDir=(SubAction==ONOFFOPTIONS_SHOWHIDDEN);
                s_Browser_RefreshAll();
            } else if ((SubAction==ONOFFOPTIONS_SHOWFILENAME)||(SubAction==ONOFFOPTIONS_HIDEFILENAME)) {
                ui->FolderTable->DisplayFileName=(SubAction==ONOFFOPTIONS_SHOWFILENAME);
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
// UTILITY FUNCTIONS
//====================================================================================================================

QAction *DlgFileExplorer::CreateMenuAction(QImage *Icon,QString Text,int Data,bool Checkable,bool IsCheck) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::CreateMenuAction");
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

QAction *DlgFileExplorer::CreateMenuAction(QIcon Icon,QString Text,int Data,bool Checkable,bool IsCheck) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgFileExplorer::CreateMenuAction");
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

QStringList DlgFileExplorer::GetCurrentSelectedFiles() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomFolderTable::GetCurrentSelectedFiles");
    return ui->FolderTable->GetCurrentSelectedFiles();
}
