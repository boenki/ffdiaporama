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
#include <QComboBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DEBUGMODE

//====================================================================================================================

cApplicationConfig::cApplicationConfig():cBaseApplicationConfig(APPLICATION_GROUPNAME,APPLICATION_NAME,APPLICATION_VERSION,CONFIGFILEEXT,CONFIGFILE_ROOTNAME) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::cApplicationConfig";
    #endif
}

//====================================================================================================================

cApplicationConfig::~cApplicationConfig() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::~cApplicationConfig";
    #endif

}

//====================================================================================================================

void cApplicationConfig::InitValues() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::InitValues";
    #endif

    SplitterSizeAndPos      ="";
    ShowHiddenFilesAndDir   =false;
    ShowMntDrive            =false;
    #if defined(Q_OS_WIN)
    QSettings Settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",QSettings::NativeFormat);
    CurrentPath=Settings.value("Personal").toString();
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    CurrentPath="~";   // User home folder
    #endif

}

//====================================================================================================================

void cApplicationConfig::SaveValueToXML(QDomElement &domDocument) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::SaveValueToXML";
    #endif

    QDomDocument    Document;
    QDomElement     Element;

    Element=Document.createElement("Options");
    Element.setAttribute("SplitterSizeAndPos",      SplitterSizeAndPos);
    Element.setAttribute("ShowHiddenFilesAndDir",   ShowHiddenFilesAndDir?"1":"0");
    Element.setAttribute("ShowMntDrive",            ShowMntDrive?"1":"0");
    Element.setAttribute("CurrentPath",             CurrentPath);
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cApplicationConfig::LoadValueFromXML(QDomElement domDocument,LoadConfigFileType /*TypeConfigFile*/) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::LoadValueFromXML";
    #endif

    if ((domDocument.elementsByTagName("Options").length()>0)&&(domDocument.elementsByTagName("Options").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("Options").item(0).toElement();
        if (Element.hasAttribute("SplitterSizeAndPos"))     SplitterSizeAndPos=Element.attribute("SplitterSizeAndPos");
        if (Element.hasAttribute("ShowHiddenFilesAndDir"))  ShowHiddenFilesAndDir=Element.attribute("ShowHiddenFilesAndDir")=="1";
        if (Element.hasAttribute("ShowMntDrive"))           ShowMntDrive=Element.attribute("ShowMntDrive")=="1";
        if (Element.hasAttribute("CurrentPath"))            CurrentPath=Element.attribute("CurrentPath");
    }
    return true;
}

//====================================================================================================================
//====================================================================================================================

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::MainWindow";
    #endif

    ApplicationConfig=new cApplicationConfig();
    IsFirstInitDone =false;
}

//====================================================================================================================

MainWindow::~MainWindow() {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::~MainWindow";
    #endif
    delete ui;
    delete ApplicationConfig;
}

//====================================================================================================================

void MainWindow::InitWindow(QString ForceLanguage,QApplication *App) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::InitWindow";
    #endif

    ApplicationConfig->InitConfigurationValues(ForceLanguage,App);
    ui->setupUi(this);
    setWindowTitle(QString("%1 %2 %3").arg(APPLICATION_GROUPNAME).arg(APPLICATION_NAME).arg(APPLICATION_VERSION));

    // Transfert settings to tree
    ui->FolderTree->ShowHidden      =ApplicationConfig->ShowHiddenFilesAndDir;
    ui->FolderTree->ShowMntDrive    =ApplicationConfig->ShowMntDrive;

    // Transfert settings to table
    ui->FolderTable->ShowHidden     =ApplicationConfig->ShowHiddenFilesAndDir;
    ui->FolderTable->ShowMntDrive   =ApplicationConfig->ShowMntDrive;

    // do some init ...

    ApplicationConfig->MainWinWSP->ApplyToWindow(this);     // Restore window position
    if (ApplicationConfig->SplitterSizeAndPos!="") ui->Splitter->restoreState(QByteArray::fromHex(ApplicationConfig->SplitterSizeAndPos.toUtf8()));
    ui->FolderTree->CreateHardDriveList();
    ui->FolderTable->SetMode(ui->FolderTree,DISPLAY_DATA);

    connect(ui->FolderTree,SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),this,SLOT(s_currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));

    ui->FolderTree->SetSelectItemByPath(ui->FolderTree->RealPathToTreePath(ApplicationConfig->CurrentPath));
    if (ui->FolderTree->GetCurrentFolderPath()!=ApplicationConfig->CurrentPath) ui->FolderTree->SetSelectItemByPath(QApplication::translate("QCustomFolderTree","Personal folder"));
}

//====================================================================================================================

void MainWindow::closeEvent(QCloseEvent *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::closeEvent";
    #endif
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

void MainWindow::s_currentItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *) {
    #ifdef DEBUGMODE
    qDebug() << "IN:MainWindow::s_currentItemChanged";
    #endif

    QString FolderInfo,UnitStr;

    ApplicationConfig->CurrentPath=ui->FolderTree->GetFolderPath(current,false);
    ui->FolderTree->RefreshItemByPath(ui->FolderTree->GetFolderPath(current,true));
    ui->CurrentPathED->setText(ApplicationConfig->CurrentPath);
    ui->FolderIcon->setPixmap(ui->FolderTree->GetIcon(ApplicationConfig->CurrentPath).pixmap(48,48));
    cHardDriveDescriptor *HDD=ui->FolderTree->SearchRealDrive(ApplicationConfig->CurrentPath);
    int NbrFiles=ui->FolderTable->FillListFolder(ApplicationConfig->CurrentPath);
    if (HDD) {
        int         Unit=0;
        qlonglong   Used=HDD->Used;
        qlonglong   Size=HDD->Size;
        while ((Used>1024*1024)&&(Unit<3)) {
            Unit++;
            Used=Used/1024;
            Size=Size/1024;
        }
        switch (Unit) {
            case 0 : UnitStr=QApplication::translate("QCustomFolderTree","Mb","Unit Mb");   break;
            case 1 : UnitStr=QApplication::translate("QCustomFolderTree","Gb","Unit Gb");   break;
            case 2 : UnitStr=QApplication::translate("QCustomFolderTree","Tb","Unit Tb");   break;
        }
        ui->HDDSizePgr->setMaximum(Size);
        ui->HDDSizePgr->setValue(Used);
        FolderInfo=QString("%1").arg(NbrFiles).trimmed()+QApplication::translate("QCustomFolderTree"," files - ")+GetTextSize(HDD->Used)+"/"+GetTextSize(HDD->Size);
    } else {
        ui->HDDSizePgr->setMaximum(0);
        ui->HDDSizePgr->setValue(0);
        FolderInfo="";
    }
    ui->FolderInfoLabel->setText(FolderInfo);
}
