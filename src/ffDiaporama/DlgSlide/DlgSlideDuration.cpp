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

#include "DlgSlideDuration.h"
#include "ui_DlgSlideDuration.h"

//====================================================================================================================

DlgSlideDuration::DlgSlideDuration(qlonglong Duration,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgSlideDuration) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideDuration::DlgSlideDuration");
    ui->setupUi(this);
    OkBt            =ui->OKBT;
    CancelBt        =ui->CancelBt;
    HelpBt          =ui->HelpBT;
    this->Duration  =Duration;
}

//====================================================================================================================

DlgSlideDuration::~DlgSlideDuration() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideDuration::~DlgSlideDuration");
    delete ui;
}

//====================================================================================================================

void DlgSlideDuration::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideDuration::DoInitDialog");
    ui->ShotDurationED->setTime(QTime(0,0,0,0).addMSecs(Duration));
}

//====================================================================================================================

bool DlgSlideDuration::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgSlideDuration::DoAccept");
    Duration=QTime(0,0,0,0).msecsTo(ui->ShotDurationED->time());
    return true;
}

