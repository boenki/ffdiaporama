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

#include "DlgffDPjrProperties.h"
#include "ui_DlgffDPjrProperties.h"

//#define DEBUGMODE

//====================================================================================================================

DlgffDPjrProperties::DlgffDPjrProperties(cffDProjectFile *ffdProject,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgffDPjrProperties) {

    #ifdef DEBUGMODE
    qDebug() << "IN:DlgffDPjrProperties::DlgffDPjrProperties";
    #endif

    this->ffdProject=ffdProject;
    ui->setupUi(this);
    CancelBt=ui->CancelBt;
    OkBt    =ui->OkBt;
    HelpBt  =ui->HelpBT;
}

//====================================================================================================================
// Initialise dialog

void DlgffDPjrProperties::DoInitDialog() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgffDPjrProperties::DoInitDialog";
    #endif

    ui->TitleED->setText(ffdProject->Title);
    ui->AuthorED->setText(ffdProject->Author);
    ui->AlbumED->setText(ffdProject->Album);
    ui->YearED->setValue(ffdProject->Year);
    ui->CommentED->setPlainText(ffdProject->Comment);
    ui->LanguageED->setText(ffdProject->DefaultLanguage);
}

//====================================================================================================================

DlgffDPjrProperties::~DlgffDPjrProperties() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgffDPjrProperties::DoInitDialog";
    #endif
    delete ui;
}

//====================================================================================================================
// Call when user click on Ok button

void DlgffDPjrProperties::DoAccept() {
    #ifdef DEBUGMODE
    qDebug() << "IN:DlgffDPjrProperties::DoAccept";
    #endif
    ffdProject->Title           =ui->TitleED->text();
    ffdProject->Author          =ui->AuthorED->text();
    ffdProject->Album           =ui->AlbumED->text();
    ffdProject->Year            =ui->YearED->value();
    ffdProject->Comment         =ui->CommentED->toPlainText();
    ffdProject->DefaultLanguage =ui->LanguageED->text();
}
