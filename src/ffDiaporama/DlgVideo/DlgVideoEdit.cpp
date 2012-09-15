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

#include "DlgVideoEdit.h"
#include "ui_DlgVideoEdit.h"
#include <QFileDialog>

// Undo actions
#define UNDOACTION_STARTPOS     1
#define UNDOACTION_ENDPOS       2
#define UNDOACTION_VOLUME       3
#define UNDOACTION_DEINTERLACE  4

DlgVideoEdit::DlgVideoEdit(cBrushDefinition *TheCurrentBrush,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgVideoEdit) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::DlgVideoEdit");
    ui->setupUi(this);

    OkBt            =ui->OKBT;
    CancelBt        =ui->CancelBt;
    HelpBt          =ui->HelpBT;
    UndoBt          =ui->UndoBT;
    CurrentBrush    =TheCurrentBrush;
    StopMaj         =false;
}

//====================================================================================================================

DlgVideoEdit::~DlgVideoEdit() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::~DlgVideoEdit");
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgVideoEdit::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::DoInitDialog");

    // Init embeded widgets
    for (int Factor=150;Factor>=0;Factor-=10) ui->VolumeReductionFactorCB->addItem(QString("%1%").arg(Factor));
    RefreshControls();

    // Define handler
    connect(ui->VolumeReductionFactorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(MusicReduceFactorChange(int)));
    connect(ui->DefStartPosBT,SIGNAL(clicked()),this,SLOT(s_DefStartPos()));
    connect(ui->DefEndPosBT,SIGNAL(clicked()),this,SLOT(s_DefEndPos()));
    connect(ui->SeekLeftBt,SIGNAL(clicked()),this,SLOT(s_SeekLeft()));
    connect(ui->SeekRightBt,SIGNAL(clicked()),this,SLOT(s_SeekRight()));
    connect(ui->DeinterlaceBt,SIGNAL(stateChanged(int)),this,SLOT(s_Deinterlace(int)));
    connect(ui->StartPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditStartPos(QTime)));
    connect(ui->EndPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditEndPos(QTime)));
    connect(ui->VideoPlayer,SIGNAL(SaveImageEvent()),this,SLOT(s_Event_SaveImageEvent()));

    #ifndef LIBAVFILTER
    ui->DeinterlaceBt->setEnabled(false);
    #endif
}

//====================================================================================================================
// Initiale Undo

void DlgVideoEdit::PrepareGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::PrepareGlobalUndo");

    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    root.setAttribute("StartPos",CurrentBrush->Video->StartPos.toString("HH:mm:ss.zzz"));
    root.setAttribute("EndPos",CurrentBrush->Video->EndPos.toString("HH:mm:ss.zzz"));
    root.setAttribute("SoundVolume",QString("%1").arg(CurrentBrush->SoundVolume,0,'f'));
    Undo->appendChild(root);                                // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgVideoEdit::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::DoGlobalUndo");

    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") {
        CurrentBrush->Video->StartPos=QTime().fromString(root.attribute("StartPos"));
        CurrentBrush->Video->EndPos  =QTime().fromString(root.attribute("EndPos"));
        CurrentBrush->SoundVolume    =root.attribute("SoundVolume").toDouble();
    }
}

//====================================================================================================================

void DlgVideoEdit::PreparePartialUndo(int /*ActionType*/,QDomElement root) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::PreparePartialUndo");

    root.setAttribute("StartPos",CurrentBrush->Video->StartPos.toString("HH:mm:ss.zzz"));               // Start position (video only)
    root.setAttribute("EndPos",CurrentBrush->Video->EndPos.toString("HH:mm:ss.zzz"));                   // End position (video only)
    root.setAttribute("SoundVolume",QString("%1").arg(CurrentBrush->SoundVolume,0,'f'));                // Volume of soundtrack (for video only)
    root.setAttribute("Deinterlace",CurrentBrush->Deinterlace?"1":"0");                                 // Deinterlace YADIF filter
}

//====================================================================================================================

void DlgVideoEdit::ApplyPartialUndo(int /*ActionType*/,QDomElement root) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::ApplyPartialUndo");

    CurrentBrush->Video->StartPos=QTime().fromString(root.attribute("StartPos"));
    CurrentBrush->Video->EndPos  =QTime().fromString(root.attribute("EndPos"));
    CurrentBrush->SoundVolume    =root.attribute("SoundVolume").toDouble();
    CurrentBrush->Deinterlace    =root.attribute("Deinterlace")=="1";
    RefreshControls();
}

//====================================================================================================================

void DlgVideoEdit::RestoreWindowState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::RestoreWindowState");
    QCustomDialog::RestoreWindowState();
    if (!ui->VideoPlayer->IsValide) {
        ui->VideoPlayer->StartPlay(CurrentBrush->Video,((cApplicationConfig *)BaseApplicationConfig)->PreviewFPS);
        ui->EndPosEd->setMaximumTime(CurrentBrush->Video->Duration);
        RefreshControls();
    }
}

//====================================================================================================================

void DlgVideoEdit::RefreshControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::RefreshControls");

    StopMaj=true;
    QTime Duration=QTime(0,0,0,0).addMSecs(CurrentBrush->Video->StartPos.msecsTo(CurrentBrush->Video->EndPos));
    ui->DeinterlaceBt->setChecked(CurrentBrush->Deinterlace);
    ui->ActualDuration->setText(Duration.toString("hh:mm:ss.zzz"));
    ui->StartPosEd->setMaximumTime(CurrentBrush->Video->EndPos);    ui->StartPosEd->setTime(CurrentBrush->Video->StartPos);
    ui->EndPosEd->setMinimumTime(CurrentBrush->Video->StartPos);    ui->EndPosEd->setTime(CurrentBrush->Video->EndPos);
    ui->VolumeReductionFactorCB->setCurrentIndex(ui->VolumeReductionFactorCB->findText(QString("%1%").arg(int(CurrentBrush->SoundVolume*100))));
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos)-QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                    -1,0,-1,0);
    ui->VideoPlayer->Deinterlace=CurrentBrush->Deinterlace;
    StopMaj=false;
}

//====================================================================================================================

void DlgVideoEdit::s_Deinterlace(int) {
    if (CurrentBrush->Deinterlace!=ui->DeinterlaceBt->isChecked()) {
        AppendPartialUndo(UNDOACTION_DEINTERLACE,ui->VolumeReductionFactorCB,true);
        CurrentBrush->Deinterlace=ui->DeinterlaceBt->isChecked();
        RefreshControls();
    }
}

//====================================================================================================================

void DlgVideoEdit::s_Event_SaveImageEvent() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::s_Event_SaveImageEvent");
    ui->VideoPlayer->SetPlayerToPause();
    QString OutputFileName=((cApplicationConfig *)BaseApplicationConfig)->LastCaptureImage;
    QString Filter="JPG (*.jpg)";
    if (!OutputFileName.endsWith(QDir::separator())) OutputFileName=OutputFileName+QDir::separator();
    OutputFileName=OutputFileName+QApplication::translate("MainWindow","Capture image");
    OutputFileName=QFileDialog::getSaveFileName(this,QApplication::translate("MainWindow","Select destination file"),OutputFileName,"PNG (*.png);;JPG (*.jpg)",&Filter);
    if (OutputFileName!="") {
        if (((cApplicationConfig *)BaseApplicationConfig)->RememberLastDirectories) ((cApplicationConfig *)BaseApplicationConfig)->LastCaptureImage=QFileInfo(OutputFileName).absolutePath();     // Keep folder for next use
        if ((Filter.toLower().indexOf("png")!=-1)&&(!OutputFileName.endsWith(".png"))) OutputFileName=OutputFileName+".png";
        if ((Filter.toLower().indexOf("jpg")!=-1)&&(!OutputFileName.endsWith(".jpg"))) OutputFileName=OutputFileName+".jpg";

        QImage *Image=CurrentBrush->Video->ImageAt(false,ui->VideoPlayer->ActualPosition,0,NULL,CurrentBrush->Deinterlace,1,false,NULL,true);
        Image->save(OutputFileName,0,100);
        delete Image;
    }
}

//====================================================================================================================

void DlgVideoEdit::MusicReduceFactorChange(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::MusicReduceFactorChange");
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_VOLUME,ui->VolumeReductionFactorCB,true);
    QString Volume=ui->VolumeReductionFactorCB->currentText();
    if (Volume!="") Volume=Volume.left(Volume.length()-1);  // Remove %
    CurrentBrush->SoundVolume=double(Volume.toInt())/100;
}

//====================================================================================================================

void DlgVideoEdit::s_DefStartPos() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::s_DefStartPos");
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_STARTPOS,ui->StartPosEd,true);
    CurrentBrush->Video->StartPos=ui->VideoPlayer->GetCurrentPos();
    RefreshControls();
}

//====================================================================================================================

void DlgVideoEdit::s_EditStartPos(QTime NewValue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::s_EditStartPos");
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_STARTPOS,ui->StartPosEd,false);
    CurrentBrush->Video->StartPos=NewValue;
    RefreshControls();
}

//====================================================================================================================

void DlgVideoEdit::s_DefEndPos() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::s_DefEndPos");
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_ENDPOS,ui->EndPosEd,true);
    CurrentBrush->Video->EndPos=ui->VideoPlayer->GetCurrentPos();
    RefreshControls();
}

//====================================================================================================================

void DlgVideoEdit::s_EditEndPos(QTime NewValue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::s_EditEndPos");
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_ENDPOS,ui->EndPosEd,false);
    CurrentBrush->Video->EndPos=NewValue;
    ui->EndPosEd->setTime(CurrentBrush->Video->EndPos);
    RefreshControls();
}

//====================================================================================================================

void DlgVideoEdit::s_SeekLeft() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::s_SeekLeft");
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos));
    ui->VideoPlayer->SetPlayerToPause();
}

//====================================================================================================================

void DlgVideoEdit::s_SeekRight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgVideoEdit::s_SeekRight");
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos));
    ui->VideoPlayer->SetPlayerToPause();
}
