/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <domledom@laposte.net>

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

#include "wgt_QEditVideo.h"
#include "ui_wgt_QEditVideo.h"
#include "DlgImage/DlgImageCorrection.h"

//====================================================================================================================

wgt_QEditVideo::wgt_QEditVideo(QWidget *parent):QWidget(parent),ui(new Ui::wgt_QEditVideo) {
    ui->setupUi(this);
    StopMaj=false;
}

//====================================================================================================================

wgt_QEditVideo::~wgt_QEditVideo() {
    delete ui;
}

//====================================================================================================================

void wgt_QEditVideo::DoInitWidget(QCustomDialog *ParentDialog,cBrushDefinition *CurrentBrush) {
    this->ParentDialog                  =ParentDialog;
    this->CurrentBrush                  =CurrentBrush;
    ui->VideoPlayer->ApplicationConfig  =ParentDialog->ApplicationConfig;
    ui->SeekLeftBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->SeekRightBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipForward));
}

//====================================================================================================================

void wgt_QEditVideo::DoInitDialog() {
    // Init embeded widgets
    for (int Factor=150;Factor>=0;Factor-=10) ui->VolumeReductionFactorCB->addItem(QString("%1%").arg(Factor));

    // Define specifique handler for video
    connect(ui->VolumeReductionFactorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_MusicReduceFactorChange(int)));
    connect(ui->DefStartPosBT,SIGNAL(clicked()),this,SLOT(s_DefStartPos()));
    connect(ui->DefEndPosBT,SIGNAL(clicked()),this,SLOT(s_DefEndPos()));
    connect(ui->SeekLeftBt,SIGNAL(clicked()),this,SLOT(s_SeekLeft()));
    connect(ui->SeekRightBt,SIGNAL(clicked()),this,SLOT(s_SeekRight()));
    connect(ui->DeinterlaceBt,SIGNAL(stateChanged(int)),this,SLOT(s_Deinterlace(int)));
    connect(ui->StartPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditStartPos(QTime)));
    connect(ui->EndPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditEndPos(QTime)));
    connect(ui->VideoPlayer,SIGNAL(SaveImageEvent()),this,SLOT(s_Event_SaveImageEvent()));

    ui->VideoPlayer->StartPlay(((cVideoFile *)CurrentBrush->MediaObject),ParentDialog->ApplicationConfig->PreviewFPS);
    ui->EndPosEd->setMaximumTime(((cVideoFile *)CurrentBrush->MediaObject)->Duration);
    RefreshControls();
}

//====================================================================================================================

void wgt_QEditVideo::WinFocus() {
    ui->VideoPlayer->SetPlayerToPlay();
}

void wgt_QEditVideo::LostFocus() {
    ui->VideoPlayer->SetPlayerToPause();
}

//====================================================================================================================

void wgt_QEditVideo::RefreshControls(bool EmitToParent) {
    if (StopMaj) return;
    StopMaj=true;
    QTime Duration=QTime(0,0,0,0).addMSecs(((cVideoFile *)CurrentBrush->MediaObject)->StartPos.msecsTo(((cVideoFile *)CurrentBrush->MediaObject)->EndPos));
    ui->DeinterlaceBt->setChecked(CurrentBrush->Deinterlace);
    ui->ActualDuration->setText(Duration.toString("hh:mm:ss.zzz"));
    ui->StartPosEd->setMaximumTime(((cVideoFile *)CurrentBrush->MediaObject)->EndPos);    ui->StartPosEd->setTime(((cVideoFile *)CurrentBrush->MediaObject)->StartPos);
    ui->EndPosEd->setMinimumTime(((cVideoFile *)CurrentBrush->MediaObject)->StartPos);    ui->EndPosEd->setTime(((cVideoFile *)CurrentBrush->MediaObject)->EndPos);
    ui->VolumeReductionFactorCB->setCurrentIndex(ui->VolumeReductionFactorCB->findText(QString("%1%").arg(int(CurrentBrush->SoundVolume*100))));
    ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(((cVideoFile *)CurrentBrush->MediaObject)->StartPos),
                                    QTime(0,0,0,0).msecsTo(((cVideoFile *)CurrentBrush->MediaObject)->EndPos)-QTime(0,0,0,0).msecsTo(((cVideoFile *)CurrentBrush->MediaObject)->StartPos),
                                    -1,0,-1,0);
    ui->VideoPlayer->Deinterlace=CurrentBrush->Deinterlace;
    StopMaj=false;
    if (EmitToParent) emit DoRefreshControls();
}

//====================================================================================================================

void wgt_QEditVideo::s_Deinterlace(int) {
    if (StopMaj) return;
    ((DlgImageCorrection *)ParentDialog)->AppendPartialUndo(DlgImageCorrection::UNDOACTION_VIDEOPART,ui->VolumeReductionFactorCB,true);
    CurrentBrush->Deinterlace=ui->DeinterlaceBt->isChecked();
    RefreshControls();
}

//====================================================================================================================

void wgt_QEditVideo::s_Event_SaveImageEvent() {
    ui->VideoPlayer->SetPlayerToPause();
    QString OutputFileName=ParentDialog->ApplicationConfig->SettingsTable->GetTextValue(QString("%1_path").arg(BrowserTypeDef[BROWSER_TYPE_CAPTUREIMAGE].BROWSERString),DefaultCaptureImage);
    QString Filter="JPG (*.jpg)";
    if (!OutputFileName.endsWith(QDir::separator())) OutputFileName=OutputFileName+QDir::separator();
    OutputFileName=OutputFileName+QApplication::translate("MainWindow","Capture image");
    OutputFileName=QFileDialog::getSaveFileName(this,QApplication::translate("MainWindow","Select destination file"),OutputFileName,"PNG (*.png);;JPG (*.jpg)",&Filter);
    if (OutputFileName!="") {
        if (ParentDialog->ApplicationConfig->RememberLastDirectories) ParentDialog->ApplicationConfig->SettingsTable->SetTextValue(QString("%1_path").arg(BrowserTypeDef[BROWSER_TYPE_CAPTUREIMAGE].BROWSERString),QFileInfo(OutputFileName).absolutePath());     // Keep folder for next use
        if ((Filter.toLower().indexOf("png")!=-1)&&(!OutputFileName.endsWith(".png"))) OutputFileName=OutputFileName+".png";
        if ((Filter.toLower().indexOf("jpg")!=-1)&&(!OutputFileName.endsWith(".jpg"))) OutputFileName=OutputFileName+".jpg";

        QImage *Image=((cVideoFile *)CurrentBrush->MediaObject)->ImageAt(false,ui->VideoPlayer->ActualPosition,NULL,CurrentBrush->Deinterlace,1,false,true);
        Image->save(OutputFileName,0,100);
        delete Image;
    }
}

//====================================================================================================================

void wgt_QEditVideo::s_MusicReduceFactorChange(int) {
    if (StopMaj) return;
    ((DlgImageCorrection *)ParentDialog)->AppendPartialUndo(DlgImageCorrection::UNDOACTION_VIDEOPART,ui->VolumeReductionFactorCB,true);
    QString Volume=ui->VolumeReductionFactorCB->currentText();
    if (Volume!="") Volume=Volume.left(Volume.length()-1);  // Remove %
    CurrentBrush->SoundVolume=double(Volume.toInt())/100;
    RefreshControls();
}

//====================================================================================================================

void wgt_QEditVideo::s_DefStartPos() {
    if (StopMaj) return;
    ((DlgImageCorrection *)ParentDialog)->AppendPartialUndo(DlgImageCorrection::UNDOACTION_VIDEOPART,ui->StartPosEd,true);
    ((cVideoFile *)CurrentBrush->MediaObject)->StartPos=ui->VideoPlayer->GetCurrentPos();
    RefreshControls();
}

//====================================================================================================================

void wgt_QEditVideo::s_EditStartPos(QTime NewValue) {
    if (StopMaj) return;
    ((DlgImageCorrection *)ParentDialog)->AppendPartialUndo(DlgImageCorrection::UNDOACTION_VIDEOPART,ui->StartPosEd,false);
    ((cVideoFile *)CurrentBrush->MediaObject)->StartPos=NewValue;
    RefreshControls();
}

//====================================================================================================================

void wgt_QEditVideo::s_DefEndPos() {
    if (StopMaj) return;
    ((DlgImageCorrection *)ParentDialog)->AppendPartialUndo(DlgImageCorrection::UNDOACTION_VIDEOPART,ui->EndPosEd,true);
    ((cVideoFile *)CurrentBrush->MediaObject)->EndPos=ui->VideoPlayer->GetCurrentPos();
    RefreshControls();
}

//====================================================================================================================

void wgt_QEditVideo::s_EditEndPos(QTime NewValue) {
    if (StopMaj) return;
    ((DlgImageCorrection *)ParentDialog)->AppendPartialUndo(DlgImageCorrection::UNDOACTION_VIDEOPART,ui->EndPosEd,false);
    ((cVideoFile *)CurrentBrush->MediaObject)->EndPos=NewValue;
    ui->EndPosEd->setTime(((cVideoFile *)CurrentBrush->MediaObject)->EndPos);
    RefreshControls();
}

//====================================================================================================================

void wgt_QEditVideo::s_SeekLeft() {
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(((cVideoFile *)CurrentBrush->MediaObject)->StartPos));
    ui->VideoPlayer->SetPlayerToPause();
}

//====================================================================================================================

void wgt_QEditVideo::s_SeekRight() {
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(((cVideoFile *)CurrentBrush->MediaObject)->EndPos));
    ui->VideoPlayer->SetPlayerToPause();
}
