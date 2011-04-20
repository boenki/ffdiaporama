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

#include "wgt_QBackgroundDefWidget.h"
#include "DlgBackgroundProperties.h"
#include "ui_DlgBackgroundProperties.h"

DlgBackgroundProperties::DlgBackgroundProperties(cDiaporamaObject *TheDiaporamaObject,QWidget *parent):QDialog(parent),ui(new Ui::DlgBackgroundProperties) {
    ui->setupUi(this);
    DiaporamaObject = TheDiaporamaObject;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT","");  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    // Init embeded widgets
    ui->BackgroundDefWidget->SetDiaporamaObject(DiaporamaObject);
    ui->CompositionWidget->SetCompositionObject(&DiaporamaObject->BackgroundComposition,NULL/*BackgroundImage*/);
    ui->tabWidget->setCurrentIndex(0);  // Start to first tab

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(s_TabCurrentChanged(int)));

    connect(ui->CompositionWidget,SIGNAL(NeedRefreshBackgroundImage()),this,SLOT(s_CompositionNeedRefreshBackgroundImage()));
    connect(ui->BackgroundDefWidget,SIGNAL(BackgroundTypeChange()),this,SLOT(s_BackgroundTypeChange()));

    s_BackgroundTypeChange();
}

//====================================================================================================================

DlgBackgroundProperties::~DlgBackgroundProperties() {
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgBackgroundProperties::SetSavedWindowGeometry() {
    DiaporamaObject->Parent->ApplicationConfig->DlgBackgroundPropertiesWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgBackgroundProperties::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgBackgroundProperties::s_BackgroundTypeChange() {
    // Enable or disable Composition widget depending on BackgroundType
    ui->tabWidget->setTabEnabled(1,DiaporamaObject->BackgroundType);
}

//====================================================================================================================

void DlgBackgroundProperties::s_TabCurrentChanged(int Page) {
    if (Page==1) {
        // Refresh background image
        s_CompositionNeedRefreshBackgroundImage();
    }
}

//====================================================================================================================

void DlgBackgroundProperties::s_CompositionNeedRefreshBackgroundImage() {
    QSize SceneboxSize=ui->CompositionWidget->GetSceneBoxSize();
    if (DiaporamaObject->Parent->GetHeightForWidth(SceneboxSize.width())<SceneboxSize.height()) SceneboxSize=QSize(DiaporamaObject->Parent->GetWidthForHeight(SceneboxSize.height()),SceneboxSize.height());
        else SceneboxSize=QSize(SceneboxSize.width(),DiaporamaObject->Parent->GetHeightForWidth(SceneboxSize.width()));
    // Create Background Image
    QPixmap     *BackgroundImage=new QPixmap(SceneboxSize);
    QPainter    Painter;
    Painter.begin(BackgroundImage);
    DiaporamaObject->Parent->PrepareBackground(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject),SceneboxSize.width(),SceneboxSize.height(),&Painter,0,0,false);
    Painter.end();
    // Set need image
    ui->CompositionWidget->SetCompositionObject(&DiaporamaObject->BackgroundComposition,BackgroundImage);
}

//====================================================================================================================

void DlgBackgroundProperties::reject() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgBackgroundPropertiesWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") DiaporamaObject->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    done(1);
}

//====================================================================================================================

void DlgBackgroundProperties::accept() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgBackgroundPropertiesWSP->SaveWindowState(this);
    done(0);
}

//====================================================================================================================

