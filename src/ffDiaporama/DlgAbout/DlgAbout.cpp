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

#include "DlgAbout.h"
#include "ui_DlgAbout.h"

#include "../_ApplicationDefinitions.h"

//====================================================================================================================

DlgAbout::DlgAbout(int HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgAbout) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::DlgAbout");

    ui->setupUi(this);
    OkBt    =ui->OKBT;
    //CancelBt=;
    //HelpBt  =ui->HelpBT;
}

//====================================================================================================================

DlgAbout::~DlgAbout() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::~DlgAbout");

    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgAbout::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::DoInitDialog");
    QString FName=QString("background")+QDir::separator()+QString("texturemate")+QDir::separator()+QString("readme.txt");
    QFile File(FName);
    if (File.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString Text=File.readAll();
        ui->TexturemateED->setText(Text);
        File.close();
    } else {
        ui->tabWidget->removeTab(2);
    }

    ui->ApplicationReleaseLabel->setText(CurrentAppVersion);
    ui->ApplicationNameLabel->setText(BaseApplicationConfig->ApplicationVersion);
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->QtWebSiteBT,SIGNAL(clicked()),this,SLOT(WQtWebSite()));
    connect(ui->ZeranoeWebSiteBT,SIGNAL(clicked()),this,SLOT(ZeranoeWebSite()));
    connect(ui->LibAVWebSiteBT,SIGNAL(clicked()),this,SLOT(LibAVWebSite()));
    connect(ui->SDLWebSiteBT,SIGNAL(clicked()),this,SLOT(SDLWebSite()));
    connect(ui->EXIV2WebSiteBT,SIGNAL(clicked()),this,SLOT(EXIV2WebSite()));
    connect(ui->FMTFWebSiteBT,SIGNAL(clicked()),this,SLOT(FMTFWebSite()));
    connect(ui->QIBWebSiteBT,SIGNAL(clicked()),this,SLOT(QIBWebSite()));
    connect(ui->CrystalWebSiteBT,SIGNAL(clicked()),this,SLOT(CrystalWebSite()));
    connect(ui->CC2WebSiteBT,SIGNAL(clicked()),this,SLOT(CC2WebSite()));
    connect(ui->TAGLibWebSiteBT,SIGNAL(clicked()),this,SLOT(TAGLibWebSite()));
}

//====================================================================================================================

void DlgAbout::WQtWebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::WQtWebSite");

    QDesktopServices::openUrl(QUrl("http://qt.digia.com/product/"));
}

//====================================================================================================================

void DlgAbout::ZeranoeWebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::ZeranoeWebSite");

    QDesktopServices::openUrl(QUrl("http://ffmpeg.zeranoe.com/builds/"));
}

//====================================================================================================================

void DlgAbout::LibAVWebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::LibavWebSite");

    QDesktopServices::openUrl(QUrl("http://libav.org/index.html"));
}

//====================================================================================================================

void DlgAbout::EXIV2WebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::EXIV2WebSite");

    QDesktopServices::openUrl(QUrl("http://www.exiv2.org/"));
}

//====================================================================================================================

void DlgAbout::FMTFWebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::FMTFWebSite");

    QDesktopServices::openUrl(QUrl("http://ksquirrel.sourceforge.net/subprojects.php"));
}

//====================================================================================================================

void DlgAbout::QIBWebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::QIBWebSite");

    QDesktopServices::openUrl(QUrl("http://qimageblitz.sourceforge.net/"));
}

//====================================================================================================================

void DlgAbout::CrystalWebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::CrystalWebSite");

    QDesktopServices::openUrl(QUrl("http://www.everaldo.com/crystal/"));
}

//====================================================================================================================

void DlgAbout::CC2WebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::CC2WebSite");

    QDesktopServices::openUrl(QUrl("http://creativecommons.org/"));
}

//====================================================================================================================
void DlgAbout::SDLWebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::SDLWebSite");

    QDesktopServices::openUrl(QUrl("http://www.libsdl.org/"));
}

//====================================================================================================================

void DlgAbout::TAGLibWebSite() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAbout::TAGLibWebSite");

    QDesktopServices::openUrl(QUrl("http://developer.kde.org/~wheeler/taglib.html"));
}
