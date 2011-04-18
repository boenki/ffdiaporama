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

#include <QColorDialog>
#include "cdiaporama.h"
#include "wgt_QBackgroundWidget.h"
#include "wgt_QBackgroundDefWidget.h"
#include "ui_wgt_QBackgroundDefWidget.h"

wgt_QBackgroundDefWidget::wgt_QBackgroundDefWidget(QWidget *parent):QWidget(parent),ui(new Ui::wgt_QBackgroundDefWidget) {
    ui->setupUi(this);
    DiaporamaObject=NULL;
    ui->CustomBrushWidget->InitWidget(false,false);
    connect(ui->CustomBrushWidget,SIGNAL(NeedRefreshControls()),this,SLOT(SetupUi()));
}

//====================================================================================================================

wgt_QBackgroundDefWidget::~wgt_QBackgroundDefWidget() {
    delete ui;
}

//====================================================================================================================

void wgt_QBackgroundDefWidget::SetDiaporamaObject(cDiaporamaObject *TheDiaporamaObject) {
    DiaporamaObject=TheDiaporamaObject;
    if (DiaporamaObject!=NULL) {
        ui->SameBackgroundRD->setChecked(!DiaporamaObject->BackgroundType);
        ui->NewBackgroundRD->setChecked(DiaporamaObject->BackgroundType);

        // Connect signals
        connect(ui->SameBackgroundRD,SIGNAL(clicked()),this,SLOT(s_SameBackground()));
        connect(ui->NewBackgroundRD,SIGNAL(clicked()),this,SLOT(s_NewBackground()));
        SetupUi();
    } else ui->CustomBrushWidget->RefreshControls(NULL,false);
}

//====================================================================================================================

void wgt_QBackgroundDefWidget::SetupUi() {
    if (DiaporamaObject!=NULL) ui->CustomBrushWidget->RefreshControls(&DiaporamaObject->BackgroundBrush,ui->NewBackgroundRD->isChecked());
    else ui->CustomBrushWidget->RefreshControls(NULL,false);

    emit BackgroundTypeChange();
}

//====================================================================================================================

void wgt_QBackgroundDefWidget::s_SameBackground() {
    if (DiaporamaObject==NULL) return;
    DiaporamaObject->BackgroundType=false;
    SetupUi();
}

//====================================================================================================================

void wgt_QBackgroundDefWidget::s_NewBackground() {
    if (DiaporamaObject==NULL) return;
    DiaporamaObject->BackgroundType=true;
    SetupUi();
}
