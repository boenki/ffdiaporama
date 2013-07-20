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

#include "../../engine/cTextFrame.h"
#include "../../CustomCtrl/cCTexteFrameComboBox.h"

#include "../DlgSlide/DlgImageComposer.h"

//====================================================================================================================

DlgffDPjrProperties::DlgffDPjrProperties(bool IsPrjCreate,cDiaporama *ffdProject,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgffDPjrProperties) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::DlgffDPjrProperties");

    this->IsPrjCreate=IsPrjCreate;
    this->ffdProject =ffdProject;
    ui->setupUi(this);

    CancelBt=ui->CancelBt;
    OkBt    =ui->OkBt;
    HelpTT  =ui->HelpTT;
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
    ui->DateED->setText(ffdProject->ProjectInfo->LongDate);
    ui->DateEdit->setDate(ffdProject->ProjectInfo->EventDate);
    ui->DateEdit->setDisplayFormat(BaseApplicationConfig->ShortDateFormat);
    ui->OverrideDateCB->setChecked(ffdProject->ProjectInfo->OverrideDate);
    ui->DateED->setEnabled(ui->OverrideDateCB->isChecked());
    ui->AuthorED->setText(ffdProject->ProjectInfo->Author);
    ui->AlbumED->setText(ffdProject->ProjectInfo->Album);
    ui->CommentED->setPlainText(ffdProject->ProjectInfo->Comment);
    ui->LanguageED->setText(ffdProject->ProjectInfo->DefaultLanguage);
    ui->TransitionSpeedWaveCB->SetCurrentValue(ffdProject->TransitionSpeedWave);
    ui->BlockSpeedWaveCB->SetCurrentValue(ffdProject->BlockAnimSpeedWave);
    ui->ImageSpeedWaveCB->SetCurrentValue(ffdProject->ImageAnimSpeedWave);
    if (BaseApplicationConfig->ID3V2Comptatibility) {
        ui->TitleED->setMaxLength(30);
        ui->AuthorED->setMaxLength(30);
        ui->AlbumED->setMaxLength(30);
    }
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

    ui->ThumbCB->PrepareTable(BaseApplicationConfig->ThumbnailModels);
    ui->ThumbCB->SetCurrentModel(ffdProject->ThumbnailName);

    // Define handler
    connect(ui->DateEdit,SIGNAL(dateChanged(const QDate &)),this,SLOT(EventDateChanged(const QDate &)));
    connect(ui->OverrideDateCB,SIGNAL(stateChanged(int)),this,SLOT(OverrideDateChanged(int)));
    connect(ui->EditThumbBT,SIGNAL(clicked()),this,SLOT(EditThumb()));
    connect(ui->ThumbCB,SIGNAL(itemSelectionHaveChanged()),this,SLOT(ThumbChanged()));

    ThumbChanged();
}

//====================================================================================================================

DlgffDPjrProperties::~DlgffDPjrProperties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::DoInitDialog");

    ffdProject->CloseUnusedLibAv(ffdProject->CurrentCol);
    delete ui;
}

//====================================================================================================================
// Initiale Undo

void DlgffDPjrProperties::PrepareGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::PrepareGlobalUndo");

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG"); // Create xml document and root
    ffdProject->ProjectInfo->SaveToXML(root);
    ffdProject->ProjectThumbnail->SaveToXML(root,"UNDO-DLG-ProjectThumbnail",ffdProject->ProjectFileName,true);
    Undo->appendChild(root); // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgffDPjrProperties::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::DoGlobalUndo");

    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") {
        ffdProject->ProjectInfo->LoadFromXML(root);
        ffdProject->ProjectThumbnail->LoadFromXML(root,"UNDO-DLG-ProjectThumbnail","",NULL);
    }
}

//====================================================================================================================
// Call when user click on Ok button

bool DlgffDPjrProperties::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::DoAccept");

    if (IsPrjCreate) ffdProject->ImageGeometry=ui->GeometryCombo->currentIndex();
    ffdProject->ProjectInfo->Title          =ui->TitleED->text();
    ffdProject->ProjectInfo->LongDate       =ui->DateED->text();
    ffdProject->ProjectInfo->EventDate      =ui->DateEdit->date();
    ffdProject->ProjectInfo->OverrideDate   =ui->OverrideDateCB->isChecked();
    ffdProject->ProjectInfo->Author         =ui->AuthorED->text();
    ffdProject->ProjectInfo->Album          =ui->AlbumED->text();
    ffdProject->ProjectInfo->Comment        =ui->CommentED->toPlainText();
    ffdProject->ProjectInfo->DefaultLanguage=ui->LanguageED->text();
    ffdProject->TransitionSpeedWave         =ui->TransitionSpeedWaveCB->GetCurrentValue();
    ffdProject->BlockAnimSpeedWave          =ui->BlockSpeedWaveCB->GetCurrentValue();
    ffdProject->ImageAnimSpeedWave          =ui->ImageSpeedWaveCB->GetCurrentValue();
    ffdProject->ThumbnailName               =ui->ThumbCB->GetCurrentModel();
    return true;
}

//====================================================================================================================

void DlgffDPjrProperties::EventDateChanged(const QDate &NewDate) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::EventDateChanged");
    ffdProject->ProjectInfo->EventDate=NewDate;
    if (!ffdProject->ProjectInfo->OverrideDate) {
        ffdProject->ProjectInfo->LongDate =ffdProject->ProjectInfo->FormatLongDate();
        ui->DateED->setText(ffdProject->ProjectInfo->LongDate);
    }
}

//====================================================================================================================

void DlgffDPjrProperties::OverrideDateChanged(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::OverrideDateChanged");
    ffdProject->ProjectInfo->OverrideDate=ui->OverrideDateCB->isChecked();
    if (!ffdProject->ProjectInfo->OverrideDate) {
        ffdProject->ProjectInfo->LongDate=ffdProject->ProjectInfo->FormatLongDate();
        ui->DateED->setText(ffdProject->ProjectInfo->LongDate);
    }
    ui->DateED->setEnabled(ui->OverrideDateCB->isChecked());
}

//====================================================================================================================

void DlgffDPjrProperties::EditThumb() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::EditThumb");
    ffdProject->ThumbnailName=ui->ThumbCB->GetCurrentModel();
    int ThumbnailIndex=BaseApplicationConfig->ThumbnailModels->SearchModel(ffdProject->ThumbnailName);
    if (!BaseApplicationConfig->ThumbnailModels->List[ThumbnailIndex].IsCustom) {
        QString Text=BaseApplicationConfig->ThumbnailModels->List[ThumbnailIndex].Name+"-"+QApplication::translate("DlgffDPjrProperties","custom");
        bool    Ok   =true;
        bool    Found=true;
        while (Ok && Found) {
            Found=false;
            Text =CustomInputDialog(this,QApplication::translate("DlgffDPjrProperties","Create a custom thumbnail model"),QApplication::translate("MainWindow","Name:"),QLineEdit::Normal,Text,&Ok);

            if (Ok && Text.isEmpty()) {
                Ok=CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgffDPjrProperties","Create a custom thumbnail model"),
                                                 QApplication::translate("DlgffDPjrProperties","Error: The name can't be empty\nRetry ?"),
                                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
            } else if (Ok) {
                for (int i=0;i<BaseApplicationConfig->ThumbnailModels->List.count();i++) if (BaseApplicationConfig->ThumbnailModels->List[i].Name.toUpper()==Text.toUpper()) {
                    Ok=CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgffDPjrProperties","Create a custom thumbnail model"),
                                                     QApplication::translate("DlgffDPjrProperties","Error: This name is already used\nRetry ?"),
                                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes;
                    Found=true;
                }
            }
        }
        if (Ok && !Found && !Text.isEmpty()) {
            QString NewName=BaseApplicationConfig->ThumbnailModels->CustomModelPath;
            if (!NewName.endsWith(QDir::separator())) NewName=NewName+QDir::separator();
            NewName=NewName+Text+".thb";
            if (ffdProject->ProjectThumbnail->SaveThumbnail(NewName)) {
                ffdProject->ThumbnailName=Text;
                BaseApplicationConfig->ThumbnailModels->FillModelType(ffd_MODELTYPE_THUMBNAIL);
                ui->ThumbCB->PrepareTable(BaseApplicationConfig->ThumbnailModels);
                ui->ThumbCB->SetCurrentModel(ffdProject->ThumbnailName);
                DlgImageComposer Dlg(ffdProject,BaseApplicationConfig,BaseApplicationConfig->DlgImageComposerThumbWSP,this);
                Dlg.InitDialog();
                if ((Dlg.exec()==0)&&(ffdProject->ProjectThumbnail->SaveThumbnail(NewName))) {
                    BaseApplicationConfig->ThumbnailModels->FillModelType(ffd_MODELTYPE_THUMBNAIL);
                    ui->ThumbCB->PrepareTable(BaseApplicationConfig->ThumbnailModels);
                    ui->ThumbCB->SetCurrentModel(ffdProject->ThumbnailName);
                }
            }
        }
    } else {
        QString NewName=BaseApplicationConfig->ThumbnailModels->CustomModelPath;
        if (!NewName.endsWith(QDir::separator())) NewName=NewName+QDir::separator();
        NewName=NewName+ffdProject->ThumbnailName+".thb";
        DlgImageComposer Dlg(ffdProject,BaseApplicationConfig,BaseApplicationConfig->DlgImageComposerThumbWSP,this);
        Dlg.InitDialog();
        if ((Dlg.exec()==0)&&(ffdProject->ProjectThumbnail->SaveThumbnail(NewName))) {
            BaseApplicationConfig->ThumbnailModels->FillModelType(ffd_MODELTYPE_THUMBNAIL);
            ui->ThumbCB->PrepareTable(BaseApplicationConfig->ThumbnailModels);
            ui->ThumbCB->SetCurrentModel(ffdProject->ThumbnailName);
        }
    }
}

//====================================================================================================================

void DlgffDPjrProperties::ThumbChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgffDPjrProperties::ThumbChanged");
    ffdProject->ThumbnailName=ui->ThumbCB->GetCurrentModel();
    int ThumbnailIndex=BaseApplicationConfig->ThumbnailModels->SearchModel(ffdProject->ThumbnailName);
    if (BaseApplicationConfig->ThumbnailModels->List[ThumbnailIndex].IsCustom)
         ui->EditThumbBT->setText(QApplication::translate("DlgffDPjrProperties","Edit this custom model"));
    else ui->EditThumbBT->setText(QApplication::translate("DlgffDPjrProperties","Create a custom model based on this model"));
}
