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

#include "QCustomFolderTree.h"
#include "QCustomGetFolder.h"
#include "ui_QCustomGetFolder.h"

QString GetDestinationFolder(QString CurrentPath,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:GetDestinationFolder");
    QCustomGetFolder Dlg(CurrentPath,HelpURL,ApplicationConfig,DlgWSP,parent);
    Dlg.InitDialog();
    if (Dlg.exec()==0) return Dlg.CurrentPath; else return "";
}

//====================================================================================================================

QCustomGetFolder::QCustomGetFolder(QString TheCurrentPath,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::QCustomGetFolder) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomGetFolder::QCustomGetFolder");

    ui->setupUi(this);
    OkBt    =ui->OkBt;
    CancelBt=ui->CancelBt;
    HelpBt  =ui->HelpBT;

    CurrentPath=TheCurrentPath;
    #ifdef Q_OS_WIN
    #else
    if (CurrentPath.startsWith(QDir::homePath())) CurrentPath="~"+CurrentPath.mid(QDir::homePath().length());
    #endif
    ui->FolderTree->ApplicationConfig=ApplicationConfig;
    DriveList=NULL;
}

//====================================================================================================================

QCustomGetFolder::~QCustomGetFolder() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomGetFolder::~QCustomGetFolder");

    delete ui;
    delete DriveList;
}

//====================================================================================================================
// Initialise dialog

void QCustomGetFolder::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomGetFolder::DoInitDialog");

    DriveList=new cDriveList(BaseApplicationConfig);
    DriveList->UpdateDriveList();
    if (CurrentPath=="") CurrentPath=QApplication::translate("QCustomFolderTree","Personal folder");
    ui->FolderTree->InitDrives(DriveList);
    ui->FolderTree->SetSelectItemByPath(ui->FolderTree->RealPathToTreePath(CurrentPath));
    if (ui->FolderTree->GetCurrentFolderPath()!=CurrentPath) ui->FolderTree->SetSelectItemByPath(CurrentPath);

    connect(ui->FolderTree,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(s_currentTreeItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
}

//====================================================================================================================

void QCustomGetFolder::s_currentTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomGetFolder::s_currentTreeItemChanged");

    ui->FolderTree->RefreshItemByPath(ui->FolderTree->GetFolderPath(current,true),false);
}

//====================================================================================================================
// Call when user click on Ok button

void QCustomGetFolder::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomGetFolder::DoAccept");

    CurrentPath=ui->FolderTree->GetCurrentFolderPath();
    #ifdef Q_OS_WIN
        CurrentPath.replace("%HOMEDRIVE%%HOMEPATH%",DriveList->List[0].Path,Qt::CaseInsensitive);
        CurrentPath.replace("%USERPROFILE%",DriveList->List[0].Path,Qt::CaseInsensitive);
        CurrentPath=AdjustDirForOS(CurrentPath);
        if (QDir(CurrentPath).canonicalPath()!="") CurrentPath=QDir(CurrentPath).canonicalPath(); // Resolved eventual .lnk files
    #else
        if (CurrentPath.startsWith("~")) CurrentPath=QDir::homePath()+CurrentPath.mid(1);
    #endif
}
