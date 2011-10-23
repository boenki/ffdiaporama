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

#include "DlgAbout.h"
#include "ui_DlgAbout.h"
#include "mainwindow.h"

DlgAbout::DlgAbout(QWidget *parent):QDialog(parent),ui(new Ui::DlgAbout) {
    ui->setupUi(this);

#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
#else
    setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
#endif

    ui->ApplicationNameLabel->setText(QString(APPLICATION_VERSION));
    ui->ApplicationReleaseLabel->setText(CurrentAppVersion);
    ui->SystemInfoED->setText(SystemProperties);
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->QtWebSiteBT,SIGNAL(clicked()),this,SLOT(WQtWebSite()));
    connect(ui->ffMPEGWebSiteBT,SIGNAL(clicked()),this,SLOT(ffMPEGWebSite()));
    connect(ui->SDLWebSiteBT,SIGNAL(clicked()),this,SLOT(SDLWebSite()));
    connect(ui->EXIV2WebSiteBT,SIGNAL(clicked()),this,SLOT(EXIV2WebSite()));
    connect(ui->FMTFWebSiteBT,SIGNAL(clicked()),this,SLOT(FMTFWebSite()));
    connect(ui->CrystalWebSiteBT,SIGNAL(clicked()),this,SLOT(CrystalWebSite()));
    connect(ui->CC2WebSiteBT,SIGNAL(clicked()),this,SLOT(CC2WebSite()));

}

DlgAbout::~DlgAbout() {
    delete ui;
}

void DlgAbout::WQtWebSite() {
    QDesktopServices::openUrl(QUrl("http://qt.nokia.com/products/"));
}

void DlgAbout::ffMPEGWebSite() {
    QDesktopServices::openUrl(QUrl("http://www.ffmpeg.org/"));
}

void DlgAbout::SDLWebSite() {
    QDesktopServices::openUrl(QUrl("http://www.libsdl.org/"));
}

void DlgAbout::EXIV2WebSite() {
    QDesktopServices::openUrl(QUrl("http://www.exiv2.org/"));
}

void DlgAbout::FMTFWebSite() {
    QDesktopServices::openUrl(QUrl("http://ksquirrel.sourceforge.net/subprojects.php"));
}

void DlgAbout::CrystalWebSite() {
    QDesktopServices::openUrl(QUrl("http://www.everaldo.com/crystal/"));
}

void DlgAbout::CC2WebSite() {
    QDesktopServices::openUrl(QUrl("http://creativecommons.org/"));
}
