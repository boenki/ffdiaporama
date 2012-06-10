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

#include "DlgWorkingTask.h"
#include "ui_DlgWorkingTask.h"

DlgWorkingTask::DlgWorkingTask(QString Title,bool *CancelAction,cBaseApplicationConfig *ApplicationConfig,QWidget *parent):
    QCustomDialog("",ApplicationConfig,NULL,parent),ui(new Ui::DlgWorkingTask) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgWorkingTask::DlgWorkingTask");

    ui->setupUi(this);
    CancelBt        =ui->CancelBt;
    CancelActionFlag=CancelAction;

    setWindowTitle(Title);
}

DlgWorkingTask::~DlgWorkingTask() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgWorkingTask::~DlgWorkingTask");
    delete ui;
}

//====================================================================================================================

void DlgWorkingTask::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgWorkingTask::DoInitDialog");
    (*CancelActionFlag)=false;
}

//====================================================================================================================

void DlgWorkingTask::DoRejet() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgWorkingTask::DoRejet");
    (*CancelActionFlag)=true;
}

//====================================================================================================================

void DlgWorkingTask::DisplayText(QString Text) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgWorkingTask::DisplayText");
    ui->ActionLabel->setText(Text);
}

//====================================================================================================================

void DlgWorkingTask::DisplayProgress(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgWorkingTask::DisplayProgress");
    ui->ProgressBar->setValue(Value);
}

//====================================================================================================================

void DlgWorkingTask::SetMaxValue(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgWorkingTask::SetMaxValue");
    MaxValue=Value;
    ui->ProgressBar->setMaximum(MaxValue);
}
