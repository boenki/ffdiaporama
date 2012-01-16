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

#include "DlgCheckConfig.h"
#include "ui_DlgCheckConfig.h"

//#define DEBUGMODE

#define ICON_GREEN      "img/Green.png"
#define ICON_RED        "img/Red.png"

//====================================================================================================================
/*
bool CheckExiv2(QString &StatusStr,cBaseApplicationConfig *ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:CheckExiv2";
    #endif
    bool        ExifOK  =true;
    #ifdef Q_OS_WIN
    QString     Commande= AdjustDirForOS(QString("\""+ApplicationConfig->PathEXIV2+"\" -V"));
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    QString     Commande= AdjustDirForOS(QString(ApplicationConfig->PathEXIV2+" -V"));
    #endif
    QProcess    Process;

    Process.setProcessChannelMode(QProcess::MergedChannels);
    Process.start(Commande);
    if (!Process.waitForStarted()) {
        qDebug()<<Commande;
        qDebug()<<"Impossible to start exiv2";
        ExifOK=false;
    }
    if (ExifOK && !Process.waitForFinished()) {
        qDebug()<<"Error during exiv2 process";
        ExifOK=false;
    }
    if (ExifOK && (Process.exitStatus()<0)) {
        qDebug()<<"Exiv2 return error"<<Process.exitStatus();
        ExifOK=false;
    }
    if (ExifOK) {
        QString Info=QString().fromLocal8Bit(Process.readAllStandardOutput());
        StatusStr=Info.left(Info.indexOf("\n"));
        if (StatusStr.indexOf(QString(char(13)))>0) StatusStr=StatusStr.left(StatusStr.indexOf(QString(char(13))));
    } else StatusStr=QApplication::translate("DlgCheckConfig","Exiv2 not found - critical - application will stop !");

    Process.terminate();
    Process.close();

    return ExifOK;
}
*/
//====================================================================================================================

bool Checkffmpeg(QString &StatusStr,cBaseApplicationConfig *ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:Checkffmpeg";
    #endif
    bool        ffmpegOK=true;
    #ifdef Q_OS_WIN
    QString     Commande= AdjustDirForOS(QString("\""+ApplicationConfig->PathFFMPEG+"\" -version"));
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    QString     Commande= AdjustDirForOS(QString(ApplicationConfig->PathFFMPEG+" -version"));
    #endif
    QProcess    Process;

    Process.setProcessChannelMode(QProcess::MergedChannels);
    Process.start(Commande);
    if (!Process.waitForStarted()) {
        qDebug()<<Commande;
        qDebug()<<"Impossible to start ffmpeg";
        ffmpegOK=false;
    }
    if (ffmpegOK && !Process.waitForFinished()) {
        Process.kill();
        qDebug()<<"Error during ffmpeg process";
        ffmpegOK=false;
    }
    if (ffmpegOK && (Process.exitStatus()<0)) {
        qDebug()<<"ffmpeg return error"<<Process.exitStatus();
        ffmpegOK=false;
    }
    if (ffmpegOK) {
        QString Info=QString().fromLocal8Bit(Process.readAllStandardOutput());
        StatusStr=Info.left(Info.indexOf("\n"));
        if (StatusStr.indexOf(QString(char(13)))>0) StatusStr=StatusStr.left(StatusStr.indexOf(QString(char(13))));
    } else StatusStr=QApplication::translate("DlgCheckConfig","ffmpeg not found - critical - application will stop !");

    Process.terminate();
    Process.close();

    return ffmpegOK;
}

//====================================================================================================================

DlgCheckConfig::DlgCheckConfig(QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgCheckConfig) {

    #ifdef DEBUGMODE
    qDebug() << "IN:DlgCheckConfig::DlgCheckConfig";
    #endif

    ui->setupUi(this);
    OkBt    =ui->OKBT;
    //CancelBt=;
    HelpBt  =ui->HelpBT;
}

//====================================================================================================================

DlgCheckConfig::~DlgCheckConfig() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgCheckConfig::~DlgCheckConfig";
    #endif
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgCheckConfig::DoInitDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgCheckConfig::DoInitDialog";
    #endif

    QString StatusStr;
    bool    Status;

    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","External dependencies")));
    //Status=CheckExiv2(StatusStr,BaseApplicationConfig);   ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));
    Status=Checkffmpeg(StatusStr,BaseApplicationConfig);  ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));

    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","")));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","ffmpeg Audio Codecs")));
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(AUDIOCODECDEF[i].IsFind?QIcon(ICON_GREEN):QIcon(ICON_RED),QString(AUDIOCODECDEF[i].LongName)+" "+(AUDIOCODECDEF[i].IsFind?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","")));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","ffmpeg Video Codecs")));
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(VIDEOCODECDEF[i].IsFind?QIcon(ICON_GREEN):QIcon(ICON_RED),QString(VIDEOCODECDEF[i].LongName)+" "+(VIDEOCODECDEF[i].IsFind?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","")));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","ffmpeg Container Formats")));
    for (int i=0;i<NBR_FORMATDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(FORMATDEF[i].IsFind?QIcon(ICON_GREEN):QIcon(ICON_RED),QString(FORMATDEF[i].LongName)+" "+(FORMATDEF[i].IsFind?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    ui->CheckConfigAtStartupCB->setChecked(BaseApplicationConfig->CheckConfigAtStartup);
}

//====================================================================================================================
// Call when user click on Ok button

void DlgCheckConfig::DoAccept() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgCheckConfig::DoAccept";
    #endif
    BaseApplicationConfig->CheckConfigAtStartup=ui->CheckConfigAtStartupCB->isChecked();
}
