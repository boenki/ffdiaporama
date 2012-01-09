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

#include "DlgAbout.h"
#include "ui_DlgAbout.h"

#include "cApplicationConfig.h"

//#define DEBUGMODE

//====================================================================================================================

DlgAbout::DlgAbout(QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgAbout) {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::DlgAbout";
    #endif
    ui->setupUi(this);
    OkBt    =ui->OKBT;
    //CancelBt=;
    //HelpBt  =ui->HelpBT;
}

//====================================================================================================================

DlgAbout::~DlgAbout() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::~DlgAbout";
    #endif
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgAbout::DoInitDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::DoInitDialog";
    #endif

    ui->ApplicationNameLabel->setText(QString(APPLICATION_VERSION));
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->QtWebSiteBT,SIGNAL(clicked()),this,SLOT(WQtWebSite()));
    connect(ui->ffMPEGWebSiteBT,SIGNAL(clicked()),this,SLOT(ffMPEGWebSite()));
    connect(ui->EXIV2WebSiteBT,SIGNAL(clicked()),this,SLOT(EXIV2WebSite()));
    connect(ui->FMTFWebSiteBT,SIGNAL(clicked()),this,SLOT(FMTFWebSite()));
    connect(ui->CrystalWebSiteBT,SIGNAL(clicked()),this,SLOT(CrystalWebSite()));
    connect(ui->CC2WebSiteBT,SIGNAL(clicked()),this,SLOT(CC2WebSite()));
    connect(ui->TAGLibWebSiteBT,SIGNAL(clicked()),this,SLOT(TAGLibWebSite()));
}

//====================================================================================================================

void DlgAbout::WQtWebSite() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::WQtWebSite";
    #endif
    QDesktopServices::openUrl(QUrl("http://qt.nokia.com/products/"));
}

//====================================================================================================================

void DlgAbout::TAGLibWebSite() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::TAGLibWebSite";
    #endif
    QDesktopServices::openUrl(QUrl("http://developer.kde.org/~wheeler/taglib.html"));
}

//====================================================================================================================

void DlgAbout::ffMPEGWebSite() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::ffMPEGWebSite";
    #endif
    QDesktopServices::openUrl(QUrl("http://www.ffmpeg.org/"));
}

//====================================================================================================================

void DlgAbout::EXIV2WebSite() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::EXIV2WebSite";
    #endif
    QDesktopServices::openUrl(QUrl("http://www.exiv2.org/"));
}

//====================================================================================================================

void DlgAbout::FMTFWebSite() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::FMTFWebSite";
    #endif
    QDesktopServices::openUrl(QUrl("http://ksquirrel.sourceforge.net/subprojects.php"));
}

//====================================================================================================================

void DlgAbout::CrystalWebSite() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::CrystalWebSite";
    #endif
    QDesktopServices::openUrl(QUrl("http://www.everaldo.com/crystal/"));
}

//====================================================================================================================

void DlgAbout::CC2WebSite() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgAbout::CC2WebSite";
    #endif
    QDesktopServices::openUrl(QUrl("http://creativecommons.org/"));
}
