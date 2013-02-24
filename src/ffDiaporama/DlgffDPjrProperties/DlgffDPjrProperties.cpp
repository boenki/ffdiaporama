/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

//====================================================================================================================

DlgffDPjrProperties::DlgffDPjrProperties(bool IsPrjCreate,cDiaporama *ffdProject,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgffDPjrProperties) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::DlgffDPjrProperties");

    this->IsPrjCreate=IsPrjCreate;
    this->ffdProject =ffdProject;
    ui->setupUi(this);

    CancelBt=ui->CancelBt;
    OkBt    =ui->OkBt;
    HelpBt  =ui->HelpBT;
}

//====================================================================================================================
// Initialise dialog

void DlgffDPjrProperties::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::DoInitDialog");

    if (IsPrjCreate) {
        setWindowTitle(QApplication::translate("DlgffDPjrProperties","New project"));
        OkBt->setText(QApplication::translate("DlgffDPjrProperties","Create project"));
    } else {
        setWindowTitle(QApplication::translate("DlgffDPjrProperties","Project properties"));
    }
    ui->tabWidget->setCurrentIndex(0);
    ui->GeometryCombo->setCurrentIndex(ffdProject->ImageGeometry);
    ui->GeometryCombo->setEnabled(IsPrjCreate);
    ui->TitleED->setText(ffdProject->ProjectInfo->Title);
    ui->AuthorED->setText(ffdProject->ProjectInfo->Author);
    ui->AlbumED->setText(ffdProject->ProjectInfo->Album);
    ui->YearED->setValue(ffdProject->ProjectInfo->Year);
    ui->CommentED->setPlainText(ffdProject->ProjectInfo->Comment);
    ui->LanguageED->setText(ffdProject->ProjectInfo->DefaultLanguage);
    ui->TransitionSpeedWaveCB->SetCurrentValue(ffdProject->TransitionSpeedWave);
    ui->BlockSpeedWaveCB->SetCurrentValue(ffdProject->BlockAnimSpeedWave);
    ui->ImageSpeedWaveCB->SetCurrentValue(ffdProject->ImageAnimSpeedWave);

    DoInitTableWidget(ui->ChapterTable,QApplication::translate("DlgffDPjrProperties","#;Slide;Title;Start;End;Duration","Column headers"));
    for (int i=0;i<ffdProject->ProjectInfo->NbrChapters;i++) {
        QString ChapterNum=QString("%1").arg(i); while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
        ChapterNum="Chapter_"+ChapterNum+":";
        QColor Background=((i & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
        ui->ChapterTable->insertRow(ui->ChapterTable->rowCount());
        ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,0,CreateItem(QString("%1").arg(i+1),Qt::AlignLeft|Qt::AlignVCenter,Background));
        ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,1,CreateItem(ffdProject->ProjectInfo->GetInformationValue(ChapterNum+"InSlide"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,2,CreateItem(ffdProject->ProjectInfo->GetInformationValue(ChapterNum+"title"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,3,CreateItem(ffdProject->ProjectInfo->GetInformationValue(ChapterNum+"Start"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,4,CreateItem(ffdProject->ProjectInfo->GetInformationValue(ChapterNum+"End"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,5,CreateItem(ffdProject->ProjectInfo->GetInformationValue(ChapterNum+"Duration"),Qt::AlignLeft|Qt::AlignVCenter,Background));
    }
    DoResizeColumnsTableWidget(ui->ChapterTable);
}

//====================================================================================================================

DlgffDPjrProperties::~DlgffDPjrProperties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::DoInitDialog");

    delete ui;
}

//====================================================================================================================
// Call when user click on Ok button

bool DlgffDPjrProperties::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::DoAccept");

    if (IsPrjCreate) ffdProject->ImageGeometry=ui->GeometryCombo->currentIndex();
    ffdProject->ProjectInfo->Title          =ui->TitleED->text();
    ffdProject->ProjectInfo->Author         =ui->AuthorED->text();
    ffdProject->ProjectInfo->Album          =ui->AlbumED->text();
    ffdProject->ProjectInfo->Year           =ui->YearED->value();
    ffdProject->ProjectInfo->Comment        =ui->CommentED->toPlainText();
    ffdProject->ProjectInfo->DefaultLanguage=ui->LanguageED->text();
    ffdProject->TransitionSpeedWave         =ui->TransitionSpeedWaveCB->GetCurrentValue();
    ffdProject->BlockAnimSpeedWave          =ui->BlockSpeedWaveCB->GetCurrentValue();
    ffdProject->ImageAnimSpeedWave          =ui->ImageSpeedWaveCB->GetCurrentValue();
    return true;
}
