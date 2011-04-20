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

#include "DlgProject.h"
#include "ui_DlgProject.h"

DlgProject::DlgProject(cDiaporama &TheDiaporama,bool NewProjectMode,QWidget *parent):QDialog(parent),ui(new Ui::DlgProject) {
    ui->setupUi(this);

    if (!NewProjectMode) setWindowTitle(QCoreApplication::translate("DlgProject","Change project settings","Window title"));

    Diaporama=&TheDiaporama;
    ui->NoShotED->setValue(Diaporama->NoShotDuration/1000);
    ui->StaticShotED->setValue(Diaporama->FixedDuration/1000);
    ui->MobilShotED->setValue(Diaporama->MobilDuration/1000);
    ui->SpeedWaveCombo->setCurrentIndex(Diaporama->SpeedWave);
    ui->GeometryCombo->setCurrentIndex(Diaporama->ImageGeometry);
    ui->GeometryCombo->setEnabled(NewProjectMode);

    // Define handler
    connect(ui->CancelBt,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OkBt,SIGNAL(clicked()),this,SLOT(accept()));
}

//====================================================================================================================

DlgProject::~DlgProject() {
    delete ui;
}

//====================================================================================================================

void DlgProject::SetSavedWindowGeometry() {
    Diaporama->ApplicationConfig->DlgProjectSettingsWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgProject::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgProject::reject() {
    // Save Window size and position
    Diaporama->ApplicationConfig->DlgProjectSettingsWSP->SaveWindowState(this);
    done(1);
}

//====================================================================================================================

void DlgProject::accept() {
    Diaporama->NoShotDuration    =ui->NoShotED->value()*1000;
    Diaporama->FixedDuration     =ui->StaticShotED->value()*1000;
    Diaporama->MobilDuration     =ui->MobilShotED->value()*1000;
    Diaporama->SpeedWave         =ui->SpeedWaveCombo->currentIndex();
    Diaporama->ImageGeometry     =ui->GeometryCombo->currentIndex();
    Diaporama->DefineSizeAndGeometry(Diaporama->ImageGeometry);

    // Save Window size and position
    Diaporama->ApplicationConfig->DlgProjectSettingsWSP->SaveWindowState(this);
    done(0);
}

