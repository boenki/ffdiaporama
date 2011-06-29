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
    CurrentBrush    =TheCurrentBrush;
    IsFirstInitDone =false;            // true when first show window was done

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT",QFileInfo(GlobalMainWindow->Diaporama->ProjectFileName).absolutePath(),true);  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    // Init embeded widgets
    for (int Factor=150;Factor>=10;Factor-=10) ui->VolumeReductionFactorCB->addItem(QString("%1%").arg(Factor));
    ui->StartPosEd->setTime(CurrentBrush->Video->StartPos);
    ui->EndPosEd->setTime(CurrentBrush->Video->EndPos);
    SetActualDuration();

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

    connect(ui->VolumeReductionFactorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(MusicReduceFactorChange(int)));
    connect(ui->DefStartPosBT,SIGNAL(clicked()),this,SLOT(s_DefStartPos()));
    connect(ui->DefEndPosBT,SIGNAL(clicked()),this,SLOT(s_DefEndPos()));
    connect(ui->SeekLeftBt,SIGNAL(clicked()),this,SLOT(s_SeekLeft()));
    connect(ui->SeekRightBt,SIGNAL(clicked()),this,SLOT(s_SeekRight()));
    connect(ui->StartPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditStartPos(QTime)));
    connect(ui->EndPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditEndPos(QTime)));
}

//====================================================================================================================

DlgVideoEdit::~DlgVideoEdit() {
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgVideoEdit::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgVideoEdit);
}

//====================================================================================================================

void DlgVideoEdit::SetSavedWindowGeometry() {
    GlobalMainWindow->ApplicationConfig->DlgVideoEditWSP->ApplyToWindow(this);
    if (!ui->VideoPlayer->IsValide) {
        ui->VideoPlayer->StartPlay(CurrentBrush->Video,GlobalMainWindow->ApplicationConfig->PreviewFPS);
        ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                        QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos)-QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                        -1,0,-1,0);
    }
}

//====================================================================================================================

void DlgVideoEdit::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    if (IsFirstInitDone) return;    // Ensure we do this only one time
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
    IsFirstInitDone=true;                                   // Set this flag to true to indicate that now we can prepeare display
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

//====================================================================================================================

void DlgVideoEdit::s_DefStartPos() {
    CurrentBrush->Video->StartPos=ui->VideoPlayer->GetCurrentPos();
    ui->StartPosEd->setTime(CurrentBrush->Video->StartPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos)-QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    -1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoEdit::s_EditStartPos(QTime NewValue) {
    CurrentBrush->Video->StartPos=NewValue;
    ui->StartPosEd->setTime(CurrentBrush->Video->StartPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos)-QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    -1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoEdit::SetActualDuration() {
    QTime Duration;

    Duration=QTime(0,0,0,0).addMSecs(CurrentBrush->Video->StartPos.msecsTo(CurrentBrush->Video->EndPos));

    ui->ActualDuration->setText(Duration.toString("hh:mm:ss.zzz"));
    ui->EndPosEd->setMinimumTime(CurrentBrush->Video->StartPos);
    ui->StartPosEd->setMaximumTime(CurrentBrush->Video->EndPos);
    ui->VolumeReductionFactorCB->setCurrentIndex(ui->VolumeReductionFactorCB->findText(QString("%1%").arg(int(CurrentBrush->SoundVolume*100))));
}

//====================================================================================================================

void DlgVideoEdit::MusicReduceFactorChange(int) {
    QString Volume=ui->VolumeReductionFactorCB->currentText();
    if (Volume!="") Volume=Volume.left(Volume.length()-1);  // Remove %
    CurrentBrush->SoundVolume=double(Volume.toInt())/100;
}

//====================================================================================================================

void DlgVideoEdit::s_DefEndPos() {
    CurrentBrush->Video->EndPos=ui->VideoPlayer->GetCurrentPos();
    ui->EndPosEd->setTime(CurrentBrush->Video->EndPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos)-QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    -1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoEdit::s_EditEndPos(QTime NewValue) {
    CurrentBrush->Video->EndPos=NewValue;
    ui->EndPosEd->setTime(CurrentBrush->Video->EndPos);
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos)-QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    -1,0,-1,0);
    SetActualDuration();
}

//====================================================================================================================

void DlgVideoEdit::s_SeekLeft() {
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos));
    ui->VideoPlayer->SetPlayerToPause();
}

//====================================================================================================================

void DlgVideoEdit::s_SeekRight() {
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos));
    ui->VideoPlayer->SetPlayerToPause();
}
