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

#include "DlgCheckConfig.h"
#include "mainwindow.h"
#include "ui_DlgCheckConfig.h"


//====================================================================================================================

bool CheckExiv2(QString &StatusStr) {
    bool        ExifOK  =true;
    QString     Commande= AdjustDirForOS(QString(GlobalMainWindow->ApplicationConfig->PathEXIV2+" -V"));
    QProcess    Process;

    Process.setProcessChannelMode(QProcess::MergedChannels);
    Process.start(Commande);
    if (!Process.waitForStarted()) {
        qDebug()<<"Impossible to start exiv2 - no exif informations will be decode";
        ExifOK=false;
    }
    if (ExifOK && !Process.waitForFinished()) {
        qDebug()<<"Error during exiv2 process - no exif informations will be decode";
        ExifOK=false;
    }
    if (ExifOK && (Process.exitStatus()<0)) {
        qDebug()<<"Exiv2 return error"<<Process.exitStatus()<<"- no exif informations will be decode";
        ExifOK=false;
    }
    if (ExifOK) {
        QString Info=QString().fromLocal8Bit(Process.readAllStandardOutput());
        StatusStr=Info.left(Info.indexOf("\n"));
    } else StatusStr=QApplication::translate("DlgCheckConfig","Exiv2 not found - critical - application will stop !");

    return ExifOK;
}

//====================================================================================================================

bool Checkffmpeg(QString &StatusStr) {
    bool        ffmpegOK=true;
    QString     Commande= AdjustDirForOS(QString(GlobalMainWindow->ApplicationConfig->PathFFMPEG+" -version"));
    QProcess    Process;

    Process.setProcessChannelMode(QProcess::MergedChannels);
    Process.start(Commande);
    if (!Process.waitForStarted()) {
        qDebug()<<"Impossible to start exiv2 - no exif informations will be decode";
        ffmpegOK=false;
    }
    if (ffmpegOK && !Process.waitForFinished()) {
        qDebug()<<"Error during exiv2 process - no exif informations will be decode";
        ffmpegOK=false;
    }
    if (ffmpegOK && (Process.exitStatus()<0)) {
        qDebug()<<"Exiv2 return error"<<Process.exitStatus()<<"- no exif informations will be decode";
        ffmpegOK=false;
    }
    if (ffmpegOK) {
        QString Info=QString().fromLocal8Bit(Process.readAllStandardOutput());
        StatusStr=Info.left(Info.indexOf("\n"));
    } else StatusStr=QApplication::translate("DlgCheckConfig","ffmpeg not found - critical - application will stop !");

    return ffmpegOK;
}

//====================================================================================================================

DlgCheckConfig::DlgCheckConfig(QWidget *parent):QDialog(parent),ui(new Ui::DlgCheckConfig) {
    ui->setupUi(this);
    IsFirstInitDone =false;            // true when first show window was done

#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
#else
    setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
#endif

    ui->CheckConfigAtStartupCB->setChecked(GlobalMainWindow->ApplicationConfig->CheckConfigAtStartup);

    QString StatusStr;
    bool    Status;

    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","External dependencies")));
    Status=CheckExiv2(StatusStr);   ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));
    Status=Checkffmpeg(StatusStr);  ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),StatusStr));

    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","")));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","ffmpeg Audio Codecs")));
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(AUDIOCODECDEF[i].IsFind?QIcon(ICON_GREEN):QIcon(ICON_RED),QString(AUDIOCODECDEF[i].LongName)+" "+(AUDIOCODECDEF[i].IsFind?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","")));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","ffmpeg Videc Codecs")));
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(VIDEOCODECDEF[i].IsFind?QIcon(ICON_GREEN):QIcon(ICON_RED),QString(VIDEOCODECDEF[i].LongName)+" "+(VIDEOCODECDEF[i].IsFind?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","")));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","ffmpeg Container Formats")));
    for (int i=0;i<NBR_FORMATDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(FORMATDEF[i].IsFind?QIcon(ICON_GREEN):QIcon(ICON_RED),QString(FORMATDEF[i].LongName)+" "+(FORMATDEF[i].IsFind?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    // Define handler
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
}

DlgCheckConfig::~DlgCheckConfig() {
    delete ui;
}

//====================================================================================================================

void DlgCheckConfig::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgCheckConfig);
}

//====================================================================================================================

void DlgCheckConfig::SetSavedWindowGeometry() {
    GlobalMainWindow->ApplicationConfig->DlgCheckConfigWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgCheckConfig::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    if (IsFirstInitDone) return;    // Ensure we do this only one time
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
    IsFirstInitDone=true;                                   // Set this flag to true to indicate that now we can prepeare display
}

//====================================================================================================================

void DlgCheckConfig::accept() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgCheckConfigWSP->SaveWindowState(this);
    GlobalMainWindow->ApplicationConfig->CheckConfigAtStartup=ui->CheckConfigAtStartupCB->isChecked();

    // Close the box
    done(0);
}
