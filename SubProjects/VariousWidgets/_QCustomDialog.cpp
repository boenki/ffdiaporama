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

#include "_QCustomDialog.h"

//#define DEBUGMODE

QCustomDialog::QCustomDialog(QString HelpURL,cBaseApplicationConfig *BaseApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):QDialog(parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::QCustomDialog";
    #endif

    this->HelpURL               =HelpURL;
    this->BaseApplicationConfig =BaseApplicationConfig;
    this->DlgWSP                =DlgWSP;
    Undo                        =NULL;
    OkBt                        =NULL;
    CancelBt                    =NULL;
    HelpBt                      =NULL;

    #if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
        setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
    #else
        setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
    #endif

}

//====================================================================================================================

QCustomDialog::~QCustomDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::~QCustomDialog";
    #endif
    if (Undo) {
        delete Undo;
        Undo=NULL;
    }
}

//====================================================================================================================
// Initialise dialog

void QCustomDialog::InitDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::InitDialog";
    #endif

    // Define handler for standard buttons
    if (OkBt)       connect(OkBt,SIGNAL(clicked()),this,SLOT(accept()));
    if (CancelBt)   connect(CancelBt,SIGNAL(clicked()),this,SLOT(reject()));

    if (HelpBt) {
        if (HelpURL!="") connect(HelpBt,SIGNAL(clicked()),this,SLOT(doHelp()));
            else HelpBt->setVisible(false);
    }

    // Restore window size and position
    if (DlgWSP) DlgWSP->ApplyToWindow(this);

    // Prepare undo
    PrepareGlobalUndo();

    // Initialise dialog
    DoInitDialog();
}

//====================================================================================================================

void QCustomDialog::doHelp() {
    #ifdef DEBUGMODE
    qDebug() << "IN:QCustomDialog::Help";
    #endif
    if (HelpURL!="") QDesktopServices::openUrl(QUrl(HelpURL.replace("<local>",BaseApplicationConfig->GetValideWEBLanguage(BaseApplicationConfig->CurrentLanguage))));
}

//====================================================================================================================

void QCustomDialog::accept() {
    #ifdef QCustomDialog
    qDebug() << "IN:QCustomDialog::accept";
    #endif
    // Save Window size and position
    DlgWSP->SaveWindowState(this);

    // call overloaded function
    DoAccept();

    // Close dialog
    done(0);
}

//====================================================================================================================

void QCustomDialog::reject() {
    #ifdef QCustomDialog
    qDebug() << "IN:QCustomDialog::accept";
    #endif
    // Save Window size and position
    DlgWSP->SaveWindowState(this);

    // Undo change
    DoGlobalUndo();

    // call overloaded function
    DoRejet();

    // Close dialog
    done(1);
}
