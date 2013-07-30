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

#include "DlgAutoTitleSlide.h"
#include "ui_DlgAutoTitleSlide.h"
#include "../DlgffDPjrProperties/DlgffDPjrProperties.h"

#define TIMERFREQ   200

DlgAutoTitleSlide::DlgAutoTitleSlide(bool IsCreation,cDiaporamaObject *DiaporamaObject,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgAutoTitleSlide) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::DlgAutoTitleSlide");

    ui->setupUi(this);
    OkBt        =ui->OKBT;
    CancelBt    =ui->CancelBt;
    HelpTT      =ui->HelpTT;

    CurrentSlide                     =DiaporamaObject;
    ui->ModelTable->ApplicationConfig=ApplicationConfig;
    ui->ModelTable->CurrentSlide     =DiaporamaObject;
    this->IsCreation                 =IsCreation;

    if (IsCreation) {
        CurrentSlide->StartNewChapter=false;
        CurrentSlide->SlideName      ="<%AUTOTS_100000%>";
        CurrentSlide->Parent->UpdateChapterInformation();
    }

    OldName=CurrentSlide->SlideName;
}

//====================================================================================================================

DlgAutoTitleSlide::~DlgAutoTitleSlide() {
    Timer.stop();
    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgAutoTitleSlide::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::DoInitDialog");
    if (IsCreation) {
        setWindowTitle(QApplication::translate("DlgAutoTitleSlide","Add a predefined title slide"));
        ui->EditModeSpacer->setVisible(false);
        ui->OKPreviousBT->setVisible(false);
        ui->OKNextBT->setVisible(false);
    } else {
        setWindowTitle(QApplication::translate("DlgAutoTitleSlide","Edit a predefined title slide")+" - "+QApplication::translate("DlgSlideProperties","Slide")+QString(" %1/%2").arg(CurrentSlide->Parent->CurrentCol+1).arg(CurrentSlide->Parent->List.count()));
        ui->OKPreviousBT->setEnabled(CurrentSlide->Parent->CurrentCol>0);
        ui->OKNextBT->setEnabled(CurrentSlide->Parent->CurrentCol<CurrentSlide->Parent->List.count()-1);
    }

    RefreshControl();

    ui->ChapterEventDateED->setDisplayFormat(BaseApplicationConfig->ShortDateFormat);
    ui->ChapterNameED->setText(CurrentSlide->ChapterName);
    ui->ChapterEventDateED->setDate(CurrentSlide->OverrideProjectEventDate?CurrentSlide->ChapterEventDate:CurrentSlide->Parent->ProjectInfo->EventDate);
    ui->ChapterDateED->setText(CurrentSlide->OverrideProjectEventDate?CurrentSlide->OverrideChapterLongDate?CurrentSlide->ChapterLongDate:FormatLongDate(CurrentSlide->ChapterEventDate):CurrentSlide->Parent->ProjectInfo->LongDate);

    int CurrentType   =(CurrentSlide->GetAutoTSNumber()/100000)-1;
    int CurrentSubType=(CurrentSlide->GetAutoTSNumber()%100000)/10000;
    ui->SlideTypeCB->setCurrentIndex(CurrentType);
    s_ChSlideTypeCB(CurrentType);
    ui->SlideCatCB->setCurrentIndex(CurrentSubType);
    s_ChSlideCatCB(CurrentSubType);
    //ui->ModelTable->SetCurrentModel(OldName.mid(QString("<%AUTOTS_").length(),QString("000000").length()));

    connect(ui->OKPreviousBT,          SIGNAL(clicked()),this,SLOT(OKPrevious()));
    connect(ui->OKNextBT,              SIGNAL(clicked()),this,SLOT(OKNext()));
    connect(ui->ConvertSlideBT,        SIGNAL(clicked()),this,SLOT(OKConvert()));
    connect(ui->OverrideProjectDateCB, SIGNAL(stateChanged(int)),this,SLOT(OverrideProjectDateChanged(int)));
    connect(ui->OverrideDateCB,        SIGNAL(stateChanged(int)),this,SLOT(OverrideDateCBChanged(int)));
    connect(ui->ChapterEventDateED,    SIGNAL(dateChanged(const QDate &)),this,SLOT(ChapterEventDateChanged(const QDate &)));
    connect(ui->SlideTypeCB,           SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChSlideTypeCB(int)));
    connect(ui->SlideCatCB,            SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChSlideCatCB(int)));
    connect(ui->ChapterNameED,         SIGNAL(textChanged(QString)),this,SLOT(s_ChapterNameChanged(QString)));
    connect(ui->ChapterDateED,         SIGNAL(textChanged(QString)),this,SLOT(s_ChapterDateChanged(QString)));
    connect(ui->ProjectPropertiesBt,   SIGNAL(clicked()),this,SLOT(ProjectProperties()));
    connect(ui->ModelTable,            SIGNAL(DoubleClickEvent(QMouseEvent *)),this,SLOT(accept()));
    connect(&Timer,                    SIGNAL(timeout()),this,SLOT(s_TimerEvent()));
}

//====================================================================================================================
// RefreshControl

void DlgAutoTitleSlide::RefreshControl() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::RefreshControl");

    ui->OverrideProjectDateCB->setChecked(CurrentSlide->OverrideProjectEventDate);
    ui->OverrideDateCB->setChecked(CurrentSlide->OverrideChapterLongDate);
    ui->ChapterEventDateLabel->setEnabled(CurrentSlide->OverrideProjectEventDate);
    ui->ChapterEventDateED->setEnabled(CurrentSlide->OverrideProjectEventDate);
    ui->OverrideDateLabel->setEnabled(CurrentSlide->OverrideProjectEventDate);
    ui->ChapterDateED->setEnabled(CurrentSlide->OverrideProjectEventDate && CurrentSlide->OverrideChapterLongDate);
}

//====================================================================================================================
// Initiale Undo

void DlgAutoTitleSlide::PrepareGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::PrepareGlobalUndo");

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");                                               // Create xml document and root
    CurrentSlide->SaveToXML(root,"UNDO-DLG-OBJECT",CurrentSlide->Parent->ProjectFileName,true);     // Save object
    Undo->appendChild(root);                                                                        // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgAutoTitleSlide::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::DoGlobalUndo");

    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentSlide->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL);
}

//====================================================================================================================

bool DlgAutoTitleSlide::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::DoAccept");
    QString CurrentModel=ui->ModelTable->GetCurrentModel();
    if (!CurrentModel.isEmpty()) {
        CurrentSlide->SlideName=QString("<%AUTOTS_%1%>").arg(ui->ModelTable->GetCurrentModel());
        if (OldName!=CurrentSlide->SlideName) {
            CurrentSlide->LoadModelFromXMLData(ui->ModelTable->ModelTable->ModelType,
                                               ui->ModelTable->ModelTable->List[ui->ModelTable->ModelTable->SearchModel(ui->ModelTable->GetCurrentModel())].Model);
            emit SetModifyFlag();
        }

        CurrentSlide->OverrideProjectEventDate=ui->OverrideProjectDateCB->isChecked();
        CurrentSlide->OverrideChapterLongDate =ui->OverrideDateCB->isChecked();
        CurrentSlide->ChapterName             =ui->ChapterNameED->text();
        if (CurrentSlide->OverrideProjectEventDate) CurrentSlide->ChapterEventDate=ui->ChapterEventDateED->date();
        if (CurrentSlide->OverrideChapterLongDate)  CurrentSlide->ChapterLongDate=ui->ChapterDateED->text();

        CurrentSlide->Parent->UpdateChapterInformation();
        return true;
    } else {
        CustomMessageBox(this,QMessageBox::Information,this->windowTitle(),
                         QApplication::translate("DlgAutoTitleSlide","Please select a model first"),QMessageBox::Close);
        return false;
    }
}

void DlgAutoTitleSlide::OKPrevious() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::OKPrevious");

    if (DoAccept()) {
        SaveWindowState();  // Save Window size and position
        done(2);            // Close the box
    }
}

void DlgAutoTitleSlide::OKNext() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::OKNext");

    if (DoAccept()) {
        SaveWindowState();  // Save Window size and position
        done(3);            // Close the box
    }
}

void DlgAutoTitleSlide::OKConvert() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::OKConvert");

    if (DoAccept()) {
        CurrentSlide->SlideName=CurrentSlide->ChapterName;
        CurrentSlide->ChapterName="";
        SaveWindowState();  // Save Window size and position
        done(4);            // Close the box
    }
}

//====================================================================================================================

void DlgAutoTitleSlide::OverrideProjectDateChanged(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::OverrideProjectDateChanged");
    CurrentSlide->OverrideProjectEventDate=ui->OverrideProjectDateCB->isChecked();
    if (!CurrentSlide->OverrideProjectEventDate) {
        ui->ChapterEventDateED->setDate(CurrentSlide->Parent->ProjectInfo->EventDate);
        CurrentSlide->ChapterLongDate=FormatLongDate(CurrentSlide->Parent->ProjectInfo->EventDate);
        ui->ChapterDateED->setText(CurrentSlide->ChapterLongDate);
    }
    CurrentSlide->Parent->UpdateChapterInformation();
    emit SetModifyFlag();
    RefreshControl();
}

//====================================================================================================================

void DlgAutoTitleSlide::OverrideDateCBChanged(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::OverrideDateCBChanged");
    CurrentSlide->OverrideChapterLongDate=ui->OverrideDateCB->isChecked();
    if (!CurrentSlide->OverrideChapterLongDate) {
        CurrentSlide->ChapterLongDate=FormatLongDate(CurrentSlide->ChapterEventDate);
        ui->ChapterDateED->setText(CurrentSlide->ChapterLongDate);
    }
    CurrentSlide->Parent->UpdateChapterInformation();
    emit SetModifyFlag();
    RefreshControl();
}

//====================================================================================================================

void DlgAutoTitleSlide::ChapterEventDateChanged(const QDate &NewDate) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::ChapterEventDateChanged");
    CurrentSlide->ChapterEventDate=NewDate;
    if (!CurrentSlide->OverrideChapterLongDate) {
        CurrentSlide->ChapterLongDate=CurrentSlide->OverrideProjectEventDate?FormatLongDate(CurrentSlide->ChapterEventDate):CurrentSlide->Parent->ProjectInfo->LongDate;
        ui->ChapterDateED->setText(CurrentSlide->ChapterLongDate);
    }
    CurrentSlide->Parent->UpdateChapterInformation();
    emit SetModifyFlag();
}

//====================================================================================================================

void DlgAutoTitleSlide::s_ChSlideTypeCB(int CurrentType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::s_ChSlideTypeCB");
    Timer.stop();
    //int CurrentType=ui->SlideTypeCB->currentIndex();
    int SubType;
    ui->SlideCatCB->clear();
    switch (CurrentType) {
        case 0:
            for (SubType=0;SubType<MODELTYPE_PROJECTTITLE_CATNUMBER;SubType++) ui->SlideCatCB->addItem(BaseApplicationConfig->PrjTitleModels[CurrentSlide->Parent->ImageGeometry][SubType==9?MODELTYPE_PROJECTTITLE_CATNUMBER-1:SubType]->NameCategorie);
            CurrentSlide->StartNewChapter=false;
            break;
        case 1:
            for (SubType=0;SubType<MODELTYPE_CHAPTERTITLE_CATNUMBER;SubType++) ui->SlideCatCB->addItem(BaseApplicationConfig->CptTitleModels[CurrentSlide->Parent->ImageGeometry][SubType==9?MODELTYPE_CHAPTERTITLE_CATNUMBER-1:SubType]->NameCategorie);
            CurrentSlide->StartNewChapter=true;
            break;
        case 2:
            for (SubType=0;SubType<MODELTYPE_CREDITTITLE_CATNUMBER;SubType++) ui->SlideCatCB->addItem(BaseApplicationConfig->CreditTitleModels[CurrentSlide->Parent->ImageGeometry][SubType==9?MODELTYPE_CREDITTITLE_CATNUMBER-1:SubType]->NameCategorie);
            CurrentSlide->StartNewChapter=false;
            break;
    }
    CurrentSlide->Parent->UpdateInformation();
    ui->ChapterNameLabel->setVisible(CurrentSlide->StartNewChapter);
    ui->ChapterNameED->setVisible(CurrentSlide->StartNewChapter);
    ui->OverrideProjectDateCB->setVisible(CurrentSlide->StartNewChapter);
    ui->ChapterEventDateLabel->setVisible(CurrentSlide->StartNewChapter);
    ui->ChapterEventDateED->setVisible(CurrentSlide->StartNewChapter);
    ui->OverrideDateLabel->setVisible(CurrentSlide->StartNewChapter);
    ui->OverrideDateCB->setVisible(CurrentSlide->StartNewChapter);
    ui->ChapterDateED->setVisible(CurrentSlide->StartNewChapter);

    switch (CurrentType) {
        case 0 : ui->ModelTable->PrepareTable(BaseApplicationConfig->PrjTitleModels[CurrentSlide->Parent->ImageGeometry][0]);       break;
        case 1 : ui->ModelTable->PrepareTable(BaseApplicationConfig->CptTitleModels[CurrentSlide->Parent->ImageGeometry][0]);       break;
        case 2 : ui->ModelTable->PrepareTable(BaseApplicationConfig->CreditTitleModels[CurrentSlide->Parent->ImageGeometry][0]);    break;
    }
    ui->ModelTable->SetCurrentModel(OldName.mid(QString("<%AUTOTS_").length(),QString("000000").length()));
    ui->ModelTable->TimerPosition=0;
    Timer.start(TIMERFREQ);
    emit SetModifyFlag();
}

//====================================================================================================================

void DlgAutoTitleSlide::s_ChSlideCatCB(int CurrentSubType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::s_ChSlideCatCB");
    int CurrentType     =ui->SlideTypeCB->currentIndex();
    //int CurrentSubType  =ui->SlideCatCB->currentIndex();
    Timer.stop();
    switch (CurrentType) {
        case 0 : ui->ModelTable->PrepareTable(BaseApplicationConfig->PrjTitleModels[CurrentSlide->Parent->ImageGeometry][CurrentSubType==9?MODELTYPE_PROJECTTITLE_CATNUMBER-1:CurrentSubType]);     break;
        case 1 : ui->ModelTable->PrepareTable(BaseApplicationConfig->CptTitleModels[CurrentSlide->Parent->ImageGeometry][CurrentSubType==9?MODELTYPE_CHAPTERTITLE_CATNUMBER-1:CurrentSubType]);     break;
        case 2 : ui->ModelTable->PrepareTable(BaseApplicationConfig->CreditTitleModels[CurrentSlide->Parent->ImageGeometry][CurrentSubType==9?MODELTYPE_CREDITTITLE_CATNUMBER-1:CurrentSubType]);   break;
    }
    ui->ModelTable->SetCurrentModel(OldName.mid(QString("<%AUTOTS_").length(),QString("000000").length()));
    ui->ModelTable->TimerPosition=0;
    Timer.start(TIMERFREQ);
    emit SetModifyFlag();
}

//====================================================================================================================

void DlgAutoTitleSlide::s_ChapterNameChanged(QString) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::s_ChapterNameChanged");
    CurrentSlide->ChapterName=ui->ChapterNameED->text();
    CurrentSlide->Parent->UpdateChapterInformation();
    emit SetModifyFlag();
}

//====================================================================================================================

void DlgAutoTitleSlide::s_ChapterDateChanged(QString) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::s_ChapterDateChanged");
    CurrentSlide->ChapterLongDate=ui->ChapterDateED->text();
    CurrentSlide->Parent->UpdateChapterInformation();
    emit SetModifyFlag();
}

//====================================================================================================================

void DlgAutoTitleSlide::s_TimerEvent() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::s_TimerEvent");
    if (!CustomTitleModelTableLockPaint.tryLock(0)) return;
    ui->ModelTable->TimerPosition+=TIMERFREQ;
    ui->ModelTable->setUpdatesEnabled(false);
    ui->ModelTable->setUpdatesEnabled(true);
    CustomTitleModelTableLockPaint.unlock();
}

//====================================================================================================================

void DlgAutoTitleSlide::ProjectProperties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgAutoTitleSlide::ProjectProperties");

    Timer.stop();
    DlgffDPjrProperties Dlg(false,CurrentSlide->Parent,BaseApplicationConfig,BaseApplicationConfig->DlgffDPjrPropertiesWSP,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) emit SetModifyFlag();
    ui->ModelTable->TimerPosition=0;
    Timer.start(TIMERFREQ);
}
