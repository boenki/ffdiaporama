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

#include "DlgVideoEdit.h"
#include "ui_DlgVideoEdit.h"
#include "mainwindow.h"

DlgVideoEdit::DlgVideoEdit(cBrushDefinition *TheCurrentBrush,QWidget *parent):QDialog(parent),ui(new Ui::DlgVideoEdit) {
    ui->setupUi(this);
    CurrentBrush=TheCurrentBrush;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT","");     // Save object
    Undo->appendChild(root);                                // Add object to xml document
}

DlgVideoEdit::~DlgVideoEdit() {
    delete ui;
}

//====================================================================================================================

void DlgVideoEdit::Help() {
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/HelpTransitionProperties.php")));
}

//====================================================================================================================

void DlgVideoEdit::SetSavedWindowGeometry() {
    GlobalMainWindow->ApplicationConfig->DlgVideoEditWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgVideoEdit::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgVideoEdit::reject() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgVideoEditWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    done(1);
}

//====================================================================================================================

void DlgVideoEdit::accept() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgVideoEditWSP->SaveWindowState(this);


    // Close the box
    done(0);
}

