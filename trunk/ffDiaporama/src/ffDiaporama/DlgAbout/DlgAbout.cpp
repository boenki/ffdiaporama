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

//====================================================================================================================

DlgAbout::DlgAbout(cBaseApplicationConfig *ApplicationConfig,QWidget *parent):
    QCustomDialog(ApplicationConfig,parent),ui(new Ui::DlgAbout) {

    ui->setupUi(this);
    OkBt    =ui->OKBT;
    //CancelBt=;
    //HelpBt  =ui->HelpBT;
}

//====================================================================================================================

DlgAbout::~DlgAbout() {
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgAbout::DoInitDialog() {
    QString FName,Text;
    QFile   File;

    FName=QString("clipart")+QDir::separator()+QString("openclipart-0.18-svgonly-readme.txt");
    File.setFileName(FName);
    if (File.open(QIODevice::ReadOnly|QIODevice::Text)) {
        Text=File.readAll();
        ui->OpenclipartED->setText(Text);
        File.close();
    } else {
        ui->tabWidget->removeTab(3);
    }
    FName=QString("background")+QDir::separator()+QString("texturemate")+QDir::separator()+QString("readme.txt");
    File.setFileName(FName);
    if (File.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString Text=File.readAll();
        ui->TexturemateED->setText(Text);
        File.close();
    } else {
        ui->tabWidget->removeTab(2);
    }
    QFile File2("authors.txt");
    if (File2.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QString Text=File2.readAll();
        ui->CreditsED->setText(Text);
        File2.close();
    } else {
        ui->tabWidget->removeTab(1);
    }

    ui->ApplicationReleaseLabel->setText(CurrentAppVersion);
    ui->ApplicationNameLabel->setText(BaseApplicationConfig->ApplicationVersion);
    ui->tabWidget->setCurrentIndex(0);
}
