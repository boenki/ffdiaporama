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

#include "DlgVideoProperties.h"
#include "ui_DlgVideoProperties.h"

//======================================
// Specific defines for this dialog box
//======================================
#define PAGE_EDITVIDEO              0
#define PAGE_GLOBALFILTER           1
#define PAGE_COMPOSITION            2
#define PAGE_FRAMING                3

//**********************************************************************************************************************************************

DlgVideoProperties::DlgVideoProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent) : QDialog(parent),ui(new Ui::DlgVideoProperties) {
    ui->setupUi(this);
    this->DiaporamaObject=DiaporamaObject;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT","");  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    // Init embeded widgets
    ui->ImageFilters->SetFilter(&DiaporamaObject->FilterTransform,DiaporamaObject);
    ui->CustomScene->SetDiaporamaScene(&DiaporamaObject->List[0]);
    ui->CompositionWidget->SetCompositionObject(&DiaporamaObject->ObjectComposition,NULL);

    ui->tabWidget->setCurrentIndex(0);
    ui->FullCanvasCB->setVisible(false);
    ui->StartPosEd->setTime(DiaporamaObject->List[0].StartPos);
    ui->EndPosEd->setTime(DiaporamaObject->List[0].EndPos);
    SetActualDuration();

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->CompositionWidget,SIGNAL(NeedRefreshBackgroundImage()),this,SLOT(s_CompositionNeedRefreshBackgroundImage()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(s_TabCurrentChanged(int)));

    connect(ui->DefStartPosBT,SIGNAL(clicked()),this,SLOT(s_DefStartPos()));
    connect(ui->DefEndPosBT,SIGNAL(clicked()),this,SLOT(s_DefEndPos()));
    connect(ui->SeekLeftBt,SIGNAL(clicked()),this,SLOT(s_SeekLeft()));
    connect(ui->SeekRightBt,SIGNAL(clicked()),this,SLOT(s_SeekRight()));
    connect(ui->StartPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditStartPos(QTime)));
    connect(ui->EndPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditEndPos(QTime)));
    connect(ui->FullCanvasCB,SIGNAL(clicked()),this,SLOT(s_FullCanvas()));
}

//====================================================================================================================

DlgVideoProperties::~DlgVideoProperties() {
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgVideoProperties::showEvent(QShowEvent *) {
    DiaporamaObject->Parent->ApplicationConfig->DlgVideoPropertiesWSP->ApplyToWindow(this);
    if (!ui->VideoPlayer->IsValide) {
        ui->VideoPlayer->StartPlay(DiaporamaObject->Video);
        ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].StartPos),QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].EndPos),-1,0,-1,0);
    }
}

//====================================================================================================================

void DlgVideoProperties::reject() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgVideoPropertiesWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") DiaporamaObject->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    done(1);
}

//====================================================================================================================

void DlgVideoProperties::accept() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgVideoPropertiesWSP->SaveWindowState(this);
    // Close the box
    done(0);
}

//====================================================================================================================

void DlgVideoProperties::s_TabCurrentChanged(int Page) {
    // Force player to pause if player tab is not use
    if (Page!=PAGE_EDITVIDEO) ui->VideoPlayer->SetPlayerToPause();
    if (Page==PAGE_COMPOSITION) s_CompositionNeedRefreshBackgroundImage();
    if (Page==PAGE_FRAMING) ui->CustomScene->RefreshWidget();
    ui->FullCanvasCB->setVisible(Page==PAGE_FRAMING);
}

//====================================================================================================================

void DlgVideoProperties::s_CompositionNeedRefreshBackgroundImage() {
    QSize SceneboxSize=ui->CompositionWidget->GetSceneBoxSize();
    if (DiaporamaObject->Parent->GetHeightForWidth(SceneboxSize.width())<SceneboxSize.height()) SceneboxSize=QSize(DiaporamaObject->Parent->GetWidthForHeight(SceneboxSize.height()),SceneboxSize.height());
        else SceneboxSize=QSize(SceneboxSize.width(),DiaporamaObject->Parent->GetHeightForWidth(SceneboxSize.width()));
    // Set image
    QPixmap *Image;
    if (DiaporamaObject->TypeObject==DIAPORAMAOBJECTTYPE_EMPTY) {
        QPainter Painter;
        Image=new QPixmap(SceneboxSize.width(),SceneboxSize.height());
        Painter.begin(Image);
        DiaporamaObject->Parent->PrepareBackground(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject),SceneboxSize.width(),SceneboxSize.height(),&Painter,0,0,false);
        Painter.end();
    } else {
        QImage *TheImage1=DiaporamaObject->GetImageAt(0,true,NULL);
        QImage TheImage2=TheImage1->scaled(SceneboxSize.width(),height());
        DiaporamaObject->FilterTransform.ApplyFilter(&TheImage2);
        Image=new QPixmap(QPixmap().fromImage(TheImage2));
        delete TheImage1;
    }
    ui->CompositionWidget->SetCompositionObject(&DiaporamaObject->ObjectComposition,new QPixmap(Image->scaled(SceneboxSize.width(),SceneboxSize.height())));
    delete Image;
}

//====================================================================================================================

void DlgVideoProperties::s_DefStartPos() {
    DiaporamaObject->List[0].StartPos=ui->VideoPlayer->GetCurrentPos();
    ui->StartPosEd->setTime(DiaporamaObject->List[0].StartPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].StartPos),QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].EndPos),-1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoProperties::s_EditStartPos(QTime NewValue) {
    DiaporamaObject->List[0].StartPos=NewValue;
    ui->StartPosEd->setTime(DiaporamaObject->List[0].StartPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].StartPos),QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].EndPos),-1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoProperties::SetActualDuration() {
    QTime Duration;

    Duration=QTime(0,0,0,0).addMSecs(DiaporamaObject->List[0].StartPos.msecsTo(DiaporamaObject->List[0].EndPos));
    DiaporamaObject->List[0].StaticDuration=DiaporamaObject->List[0].StartPos.msecsTo(DiaporamaObject->List[0].EndPos);
    ui->ActualDuration->setText(Duration.toString("hh:mm:ss.zzz"));
    ui->EndPosEd->setMinimumTime(DiaporamaObject->List[0].StartPos);
    ui->StartPosEd->setMaximumTime(DiaporamaObject->List[0].EndPos);
}

//====================================================================================================================

void DlgVideoProperties::s_DefEndPos() {
    DiaporamaObject->List[0].EndPos=ui->VideoPlayer->GetCurrentPos();
    ui->EndPosEd->setTime(DiaporamaObject->List[0].EndPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].StartPos),QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].EndPos),-1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoProperties::s_EditEndPos(QTime NewValue) {
    DiaporamaObject->List[0].EndPos=NewValue;
    ui->EndPosEd->setTime(DiaporamaObject->List[0].EndPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].StartPos),QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].EndPos),-1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoProperties::s_SeekLeft() {
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].StartPos));
}

//====================================================================================================================

void DlgVideoProperties::s_SeekRight() {
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(DiaporamaObject->List[0].EndPos));
}

//====================================================================================================================

void DlgVideoProperties::s_FullCanvas() {
    if (DiaporamaObject->FullCanvas) {
        DiaporamaObject->FullCanvas=false;
        DiaporamaObject->SwitchShotsToNormalCanvas(); // adapt all shot value
    } else {
        DiaporamaObject->FullCanvas=true;
        DiaporamaObject->SwitchShotsToFullCanvas();   // adapt all shot value
    }
    ui->CustomScene->RefreshWidget();
}
