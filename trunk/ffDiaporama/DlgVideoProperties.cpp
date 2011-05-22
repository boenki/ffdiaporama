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
#define PAGE_FRAMING                2

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
    for (int Factor=150;Factor>=10;Factor-=10) ui->VolumeReductionFactorCB->addItem(QString("%1%").arg(Factor));
    ui->ImageFilters->SetFilter(&DiaporamaObject->FilterTransform,DiaporamaObject);
    ui->CustomScene->SetDiaporamaShot(&DiaporamaObject->List[0].FilterCorrection);

    ui->tabWidget->setCurrentIndex(0);
    ui->StartPosEd->setTime(DiaporamaObject->Video->StartPos);
    ui->EndPosEd->setTime(DiaporamaObject->Video->EndPos);
    SetActualDuration();

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(s_TabCurrentChanged(int)));

    connect(ui->VolumeReductionFactorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(MusicReduceFactorChange(int)));
    connect(ui->DefStartPosBT,SIGNAL(clicked()),this,SLOT(s_DefStartPos()));
    connect(ui->DefEndPosBT,SIGNAL(clicked()),this,SLOT(s_DefEndPos()));
    connect(ui->SeekLeftBt,SIGNAL(clicked()),this,SLOT(s_SeekLeft()));
    connect(ui->SeekRightBt,SIGNAL(clicked()),this,SLOT(s_SeekRight()));
    connect(ui->StartPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditStartPos(QTime)));
    connect(ui->EndPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditEndPos(QTime)));
}

//====================================================================================================================

DlgVideoProperties::~DlgVideoProperties() {
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgVideoProperties::Help() {
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/HelpVideoProperties.php")));
}

//====================================================================================================================

void DlgVideoProperties::showEvent(QShowEvent *) {
    DiaporamaObject->Parent->ApplicationConfig->DlgVideoPropertiesWSP->ApplyToWindow(this);
    if (!ui->VideoPlayer->IsValide) {
        ui->VideoPlayer->StartPlay(DiaporamaObject->Video,DiaporamaObject->Parent->ApplicationConfig->PreviewFPS);
        ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                        QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->EndPos)-QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                        -1,0,-1,0);
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
    if (Page==PAGE_FRAMING) ui->CustomScene->RefreshWidget();
}

//====================================================================================================================

void DlgVideoProperties::s_DefStartPos() {
    DiaporamaObject->Video->StartPos=ui->VideoPlayer->GetCurrentPos();
    ui->StartPosEd->setTime(DiaporamaObject->Video->StartPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->EndPos)-QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                    -1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoProperties::s_EditStartPos(QTime NewValue) {
    DiaporamaObject->Video->StartPos=NewValue;
    ui->StartPosEd->setTime(DiaporamaObject->Video->StartPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->EndPos)-QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                    -1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoProperties::SetActualDuration() {
    QTime Duration;

    Duration=QTime(0,0,0,0).addMSecs(DiaporamaObject->Video->StartPos.msecsTo(DiaporamaObject->Video->EndPos));
    DiaporamaObject->List[0].StaticDuration=DiaporamaObject->Video->StartPos.msecsTo(DiaporamaObject->Video->EndPos);
    ui->ActualDuration->setText(Duration.toString("hh:mm:ss.zzz"));
    ui->EndPosEd->setMinimumTime(DiaporamaObject->Video->StartPos);
    ui->StartPosEd->setMaximumTime(DiaporamaObject->Video->EndPos);
    ui->VolumeReductionFactorCB->setCurrentIndex(ui->VolumeReductionFactorCB->findText(QString("%1%").arg(int(DiaporamaObject->Video->SoundVolume*100))));
}

//====================================================================================================================

void DlgVideoProperties::MusicReduceFactorChange(int) {
    QString Volume=ui->VolumeReductionFactorCB->currentText();
    if (Volume!="") Volume=Volume.left(Volume.length()-1);  // Remove %
    DiaporamaObject->Video->SoundVolume=double(Volume.toInt())/100;
}

//====================================================================================================================

void DlgVideoProperties::s_DefEndPos() {
    DiaporamaObject->Video->EndPos=ui->VideoPlayer->GetCurrentPos();
    ui->EndPosEd->setTime(DiaporamaObject->Video->EndPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->EndPos)-QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                    -1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoProperties::s_EditEndPos(QTime NewValue) {
    DiaporamaObject->Video->EndPos=NewValue;
    ui->EndPosEd->setTime(DiaporamaObject->Video->EndPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->EndPos)-QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos),
                                    -1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoProperties::s_SeekLeft() {
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->StartPos));
}

//====================================================================================================================

void DlgVideoProperties::s_SeekRight() {
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(DiaporamaObject->Video->EndPos));
}
