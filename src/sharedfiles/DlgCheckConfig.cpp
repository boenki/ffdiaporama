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
#include "cBaseMediaFile.h"

//#define DEBUGMODE

#define ICON_GREEN      ":/img/Green.png"
#define ICON_RED        ":/img/Red.png"

//====================================================================================================================

bool Checkffmpeg(QString &StatusStr) {
    #ifdef DEBUGMODE
    qDebug() << "IN:Checkffmpeg";
    #endif
    bool        ffmpegOK=true;
    QProcess    Process;

    //Process.setProcessChannelMode(QProcess::MergedChannels);
    Process.start("ffmpeg",QString("-version").split(";"));
    if (!Process.waitForStarted(-1)) {
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
        QString     Info=QString().fromLocal8Bit(Process.readAllStandardOutput())+
                         QString().fromLocal8Bit(Process.readAllStandardError());
        //qDebug()<<Info;
        if (Info.indexOf("ffmpeg version ")>=0) {
            StatusStr=Info.mid(Info.indexOf("ffmpeg version ")+QString("ffmpeg version ").length());
            StatusStr=StatusStr.left(StatusStr.indexOf("\n"));
            if (StatusStr.indexOf(QString(char(13)))>0) StatusStr=StatusStr.left(StatusStr.indexOf(QString(char(13))));
        } else {
            StatusStr=QApplication::translate("DlgCheckConfig","Unable to determine ffmpeg version");
            //ffmpegOK=false;
        }
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

    // exiv2
    StatusStr=QApplication::translate("DlgCheckConfig","LibExiv2 version:")+QString("%1.%2.%3").arg(EXIV2_MAJOR_VERSION).arg(EXIV2_MINOR_VERSION).arg(EXIV2_PATCH_VERSION);
    ui->ListWidget->addItem(new QListWidgetItem(StatusStr));
    #ifdef EXIV2WITHPREVIEW
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","LibExiv2 support for preview image")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));

    ui->ListWidget->addItem(new QListWidgetItem(""));

    // taglib
    StatusStr=QApplication::translate("DlgCheckConfig","TAGLib version:")+QString("%1.%2.%3").arg(TAGLIB_MAJOR_VERSION).arg(TAGLIB_MINOR_VERSION).arg(TAGLIB_PATCH_VERSION);
    ui->ListWidget->addItem(new QListWidgetItem(StatusStr));
    #ifdef TAGLIBWITHFLAC
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","TAGLib support for FLAC")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));
    #ifdef TAGLIBWITHASF
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","TAGLib support for ASF/WMA")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));
    #ifdef TAGLIB_WITH_MP4
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","TAGLib support for M4A/MP4")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));


    ui->ListWidget->addItem(new QListWidgetItem(""));

    // ffmpeg
    ui->ListWidget->addItem(new QListWidgetItem("ffmpeg"));
    Status=Checkffmpeg(StatusStr);
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","ffmpeg version:")+StatusStr));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","LIBAVCODEC version:")+QString("%1").arg(LIBAVCODEC_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MINOR)+"."+QString("%1").arg(LIBAVCODEC_VERSION_MICRO)+"."+QString("%1").arg(avcodec_version())));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","LIBAVFORMAT version:")+QString("%1").arg(LIBAVFORMAT_VERSION_MAJOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MINOR)+"."+QString("%1").arg(LIBAVFORMAT_VERSION_MICRO)+"."+QString("%1").arg(avformat_version())));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","LIBSWSCALE version:")+QString("%1").arg(LIBSWSCALE_VERSION_MAJOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MINOR)+"."+QString("%1").arg(LIBSWSCALE_VERSION_MICRO)+"."+QString("%1").arg(swscale_version())));

    #ifdef FFMPEGWITHTAG
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","ffmpeg support for TAG and CHAPTERS")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));

    ui->ListWidget->addItem(new QListWidgetItem(""));

    // ffmpeg
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","ffmpeg Audio Codecs")));
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(AUDIOCODECDEF[i].IsFind?QIcon(ICON_GREEN):QIcon(ICON_RED),QString(AUDIOCODECDEF[i].LongName)+" "+(AUDIOCODECDEF[i].IsFind?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    ui->ListWidget->addItem(new QListWidgetItem(""));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","ffmpeg Video Codecs")));
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(VIDEOCODECDEF[i].IsFind?QIcon(ICON_GREEN):QIcon(ICON_RED),QString(VIDEOCODECDEF[i].LongName)+" "+(VIDEOCODECDEF[i].IsFind?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    ui->ListWidget->addItem(new QListWidgetItem(""));
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
