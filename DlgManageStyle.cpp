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

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#include <winbase.h>
#endif

// Specific inclusions
#include "DlgManageStyle.h"
#include "ui_DlgManageStyle.h"

#include "mainwindow.h"

//============================================================================================================================
// Manage style dialog
//============================================================================================================================

DlgManageStyle::DlgManageStyle(cStyleCollection *TheCollection,QWidget *parent):QDialog(parent),ui(new Ui::DlgManageStyle) {
    ui->setupUi(this);
    TheCollection=Collection;

#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
#else
    setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
#endif

    // Define handler
    connect(ui->CancelBt,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OkBt,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
}

//====================================================================================================================

DlgManageStyle::~DlgManageStyle() {
    delete ui;
}

//====================================================================================================================

void DlgManageStyle::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgManageStyle);
}

//====================================================================================================================

void DlgManageStyle::SetSavedWindowGeometry() {
    GlobalMainWindow->ApplicationConfig->DlgManageStyleWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgManageStyle::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgManageStyle::reject() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgManageStyleWSP->SaveWindowState(this);
    done(1);
}

//====================================================================================================================

void DlgManageStyle::accept() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgManageStyleWSP->SaveWindowState(this);
    done(0);
}
