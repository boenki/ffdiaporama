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

#include "wgt_QBackgroundWidget.h"
#include "ui_wgt_QBackgroundWidget.h"

wgt_QBackgroundWidget::wgt_QBackgroundWidget(QWidget *parent):QWidget(parent),ui(new Ui::wgt_QBackgroundWidget) {
    ui->setupUi(this);
    BackgroundObject=NULL;
}

wgt_QBackgroundWidget::~wgt_QBackgroundWidget() {
    delete ui;
}

void wgt_QBackgroundWidget::SetupWidget(cBackgroundObject *TheBackgroundObject) {
    BackgroundObject=TheBackgroundObject;
    ui->ImageWGT->setPixmap(BackgroundObject->Icon);
    ui->ImageWGT->setFixedWidth(BackgroundObject->Icon.width());
    ui->NameLabel->setText(BackgroundObject->Name);
    ui->WebSiteLabel->setText(BackgroundObject->WebSite);
    ui->LicenceLabel->setText(BackgroundObject->Licence);
}
