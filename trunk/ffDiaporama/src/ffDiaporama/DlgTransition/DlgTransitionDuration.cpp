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

#include "DlgTransitionDuration.h"
#include "ui_DlgTransitionDuration.h"

//====================================================================================================================

DlgTransitionDuration::DlgTransitionDuration(qlonglong Duration,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgTransitionDuration) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTransitionDuration::DlgTransitionDuration");
    ui->setupUi(this);
    OkBt            =ui->OKBT;
    CancelBt        =ui->CancelBt;
    HelpBt          =ui->HelpBT;
    this->Duration  =Duration;
}

//====================================================================================================================

DlgTransitionDuration::~DlgTransitionDuration() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTransitionDuration::~DlgTransitionDuration");
    delete ui;
}

//====================================================================================================================

void DlgTransitionDuration::DoInitDialog() {
   ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTransitionDuration::DoInitDialog");

   QString sDuration=QString("%1").arg(double(Duration)/double(1000),0,'f');
   while (sDuration.endsWith('0')) sDuration=sDuration.left(sDuration.length()-1);
   while (sDuration.endsWith('.')) sDuration=sDuration.left(sDuration.length()-1);
   ui->TransitionDurationCB->setCurrentIndex(ui->TransitionDurationCB->findText(sDuration));
}

//====================================================================================================================

void DlgTransitionDuration::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgTransitionDuration::DoAccept");
    Duration=qlonglong(ui->TransitionDurationCB->currentText().toDouble()*double(1000));
}

