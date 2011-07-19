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

#include "DlgSlideProperties.h"
#include "ui_DlgSlideProperties.h"
#include "wgt_QCustomThumbnails.h"
#include "DlgSlideProperties.h"
#include "DlgImageCorrection.h"
#include "DlgVideoEdit.h"
#include "DlgTextEdit.h"
#include "mainwindow.h"

DlgSlideProperties::DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent):QDialog(parent),ui(new Ui::DlgSlideProperties) {
    ui->setupUi(this);
    this->DiaporamaObject=DiaporamaObject;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT",QFileInfo(DiaporamaObject->Parent->ProjectFileName).absolutePath(),true);  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    IsFirstInitDone     = false;                // True when first show window was done
    InRefreshSceneImage = false;                // True if process is currently in RefreshSceneImage function

    scene           = NULL;
    NextZValue      = 500;
    BackgroundImage = NULL;
    CompositionList = NULL;
    StopMAJSpinbox  = false;
    BLOCKCHSIZE     = false;

    MagneticRuler.MagneticRuler=DiaporamaObject->Parent->ApplicationConfig->SlideRuler;

    // Init combo box Background form
    for (int i=0;i<12;i++) ui->BackgroundFormCB->addItem("");
    MakeFormIcon(ui->BackgroundFormCB);

    // Init combo box Background shadow form
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","None"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","Shadow upper left"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","Shadow upper right"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","Shadow bottom left"));
    ui->ShadowEffectCB->addItem(QApplication::translate("DlgSlideProperties","Shadow bottom right"));
    ui->ShadowEffectED->setRange(1,100);

    // Init combo box external border style
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::SolidLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DotLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashDotLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashDotDotLine);
    MakeBorderStyleIcon(ui->PenStyleCB);
    // Init shape Borders
    ui->PenSizeEd->setMinimum(0);       ui->PenSizeEd->setMaximum(30);

    // Init combo box Background opacity
    ui->OpacityCB->addItem("100%");
    ui->OpacityCB->addItem(" 75%");
    ui->OpacityCB->addItem(" 50%");
    ui->OpacityCB->addItem(" 25%");

    ui->RotateXED->setRange(-180,180);      ui->RotateXSLD->setRange(-180,180);
    ui->RotateYED->setRange(-180,180);      ui->RotateYSLD->setRange(-180,180);
    ui->RotateZED->setRange(-180,180);      ui->RotateZSLD->setRange(-180,180);

    // Init Spinbox

    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        ui->PosXEd->setDecimals(2);             ui->PosXEd->setSingleStep(1);       ui->PosXEd->setSuffix("%");
        ui->PosYEd->setDecimals(2);             ui->PosYEd->setSingleStep(1);       ui->PosYEd->setSuffix("%");
        ui->WidthEd->setDecimals(2);            ui->WidthEd->setSingleStep(1);      ui->WidthEd->setSuffix("%");
        ui->HeightEd->setDecimals(2);           ui->HeightEd->setSingleStep(1);     ui->HeightEd->setSuffix("%");
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        ui->PosXEd->setDecimals(0);             ui->PosXEd->setSingleStep(1);       ui->PosXEd->setSuffix("");
        ui->PosYEd->setDecimals(0);             ui->PosYEd->setSingleStep(1);       ui->PosYEd->setSuffix("");
        ui->WidthEd->setDecimals(0);            ui->WidthEd->setSingleStep(1);      ui->WidthEd->setSuffix("");
        ui->HeightEd->setDecimals(0);           ui->HeightEd->setSingleStep(1);     ui->HeightEd->setSuffix("");
    }

    // Init combo box Background  type
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgSlideProperties","No brush"));              ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_NOBRUSH)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgSlideProperties","Solid brush"));           ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_SOLID)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgSlideProperties","Pattern brush"));         ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_PATTERN)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgSlideProperties","Gradient 2 colors"));     ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT2)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgSlideProperties","Gradient 3 colors"));     ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT3)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgSlideProperties","Image from library"));    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_IMAGELIBRARY)));

    // Init image geometry combo box
    ui->ImageGeometryCB->addItem(QApplication::translate("DlgSlideProperties","Project geometry"));
    ui->ImageGeometryCB->addItem(QApplication::translate("DlgSlideProperties","Image geometry"));
    ui->ImageGeometryCB->addItem(QApplication::translate("DlgSlideProperties","Custom geometry"));

    ui->BlockTabWidget->setCurrentIndex(0); // Ensure page 0

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
    connect(ui->TVMarginsBT,SIGNAL(clicked()),this,SLOT(s_TVMarginsBt()));

    connect(ui->CopyBlockBT,SIGNAL(clicked()),this,SLOT(s_CopyBlockBT()));
    connect(ui->CutBlockBT,SIGNAL(clicked()),this,SLOT(s_CutBlockBT()));
    connect(ui->PasteBlockBT,SIGNAL(clicked()),this,SLOT(s_PasteBlockBT()));

    connect(ui->VisibleBT,SIGNAL(clicked()),this,SLOT(ChangeVisibleState()));
    connect(ui->SoundBT,SIGNAL(clicked()),this,SLOT(GetSound()));

    connect(ui->ShotLeftBt,SIGNAL(clicked()),this,SLOT(ShotLeft()));
    connect(ui->ShotRightBt,SIGNAL(clicked()),this,SLOT(ShotRight()));
    connect(ui->BlockUpBT,SIGNAL(clicked()),this,SLOT(BlockUp()));
    connect(ui->BlockDownBT,SIGNAL(clicked()),this,SLOT(BlockDown()));

    connect(ui->SlideNameED,SIGNAL(textEdited(QString)),this,SLOT(s_SlideNameChange(QString)));
    connect(ui->ShotDurationED,SIGNAL(timeChanged(QTime)),this,SLOT(s_ShotDurationChange(QTime)));

    connect(ui->TextEditBT,SIGNAL(clicked()),this,SLOT(TextEditor()));
    connect(ui->ImageEditCorrectBT,SIGNAL(clicked()),this,SLOT(ImageEditCorrect()));
    connect(ui->VideoEditBT,SIGNAL(clicked()),this,SLOT(VideoEdit()));
    connect(ui->FileNameBT,SIGNAL(clicked()),this,SLOT(ChangeBrushDiskFile()));

    connect(ui->PosXEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgPosXValue(double)));
    connect(ui->PosYEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgPosYValue(double)));
    connect(ui->WidthEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgWidthValue(double)));
    connect(ui->HeightEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgHeightValue(double)));
    connect(ui->BackgroundFormCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBackgroundForm(int)));
    connect(ui->OpacityCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeOpacity(int)));
    connect(ui->PenStyleCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangePenStyle(int)));
    connect(ui->ShadowEffectCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChgShadowFormValue(int)));
    connect(ui->ShadowEffectED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgShadowDistanceValue(int)));

    connect(ui->RotateXED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateXValue(int))); connect(ui->RotateXSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateXValue(int)));
    connect(ui->RotateYED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateYValue(int))); connect(ui->RotateYSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateYValue(int)));
    connect(ui->RotateZED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateZValue(int))); connect(ui->RotateZSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateZValue(int)));
    connect(ui->PenColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChPenColorCB(int)));
    connect(ui->PenSizeEd,SIGNAL(valueChanged(int)),this,SLOT(s_ChgPenSize(int)));
    connect(ui->ShadowColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChgShadowColorCB(int)));

    // Brush part
    connect(ui->BrushTypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBrushTypeCombo(int)));
    connect(ui->PatternBrushCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexPatternBrushCombo(int)));
    connect(ui->OrientationCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientOrientationCombo(int)));
    connect(ui->FirstColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFirstColorCombo(int)));
    connect(ui->FinalColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFinalColorCombo(int)));
    connect(ui->IntermColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientIntermColorCombo(int)));
    connect(ui->BackgroundCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexBackgroundCombo(int)));
    connect(ui->IntermPosSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_IntermPosSliderMoved(int)));
    connect(ui->IntermPosED,SIGNAL(valueChanged(int)),this,SLOT(s_IntermPosED(int)));

    connect(ui->TransformationCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChTransformationCB(int)));
    connect(ui->BlurRadiusSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_BlurRadiusSliderMoved(int)));
    connect(ui->BlurRadiusED,SIGNAL(valueChanged(int)),this,SLOT(s_BlurRadiusSliderMoved(int)));
    connect(ui->BlurSigmaSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_BlurSigmaSliderMoved(int)));
    connect(ui->BlurSigmaED,SIGNAL(valueChanged(double)),this,SLOT(s_BlurSigmaValueED(double)));

    // Image part
    connect(ui->ImageGeometryCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeImageGeometry(int)));

    // Shot table part
    connect(ui->ShotTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_ShotTable_SelectionChanged()));
    connect(ui->AddShot,SIGNAL(pressed()),this,SLOT(s_ShotTable_AddShot()));
    connect(ui->RemoveShot,SIGNAL(pressed()),this,SLOT(s_ShotTable_RemoveShot()));

    // Block table/scene part
    connect(ui->BlockTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_BlockTable_SelectionChanged()));
    connect(ui->AddTextBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewTextBlock()));
    connect(ui->AddFileBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_AddNewFileBlock()));
    connect(ui->RemoveBlock,SIGNAL(pressed()),this,SLOT(s_BlockTable_RemoveBlock()));
}

//====================================================================================================================

DlgSlideProperties::~DlgSlideProperties() {
    Clean();
    StopMAJSpinbox=true;
    if (BackgroundImage!=NULL) {
        delete BackgroundImage;
        BackgroundImage=NULL;
    }
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgSlideProperties::Clean() {
    StopMAJSpinbox=true;
    // Delete scene and all of it's content, if exist
    if (scene!=NULL) {
        // delete all items
        while (scene->items().count()>0) {
            QGraphicsItem *Item=scene->items().at(0);
            QString       data =Item->data(0).toString();

            scene->removeItem(Item);    // Remove item from the scene

            if (data=="CustomGraphicsRectItem")         delete (cCustomGraphicsRectItem *)Item;
            else if (data=="ResizeGraphicsRectItem")    delete ((cResizeGraphicsRectItem *)Item)->RectItem;
            else if (data=="image")                     delete (QGraphicsPixmapItem *)Item;
            else ExitApplicationWithFatalError("Unkwnon item type in DlgSlideProperties::Clean");
        }

        delete scene;
        scene=NULL;
    }
    StopMAJSpinbox=false;
}

//====================================================================================================================

void DlgSlideProperties::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgSlideProperties);
}

//====================================================================================================================

void DlgSlideProperties::resizeEvent(QResizeEvent *) {
    if (IsFirstInitDone) RefreshShotTable(ui->ShotTable->currentColumn());      // Fill the ShotTable and select 1st shot
}

//====================================================================================================================

void DlgSlideProperties::SetSavedWindowGeometry() {
    DiaporamaObject->Parent->ApplicationConfig->DlgSlidePropertiesWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgSlideProperties::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    if (IsFirstInitDone) return;
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
    IsFirstInitDone=true;                                   // Set this flag to true to indicate that now we can prepeare display
    RefreshShotTable(0);                                    // Fill the ShotTable and select 1st shot
}

//====================================================================================================================

void DlgSlideProperties::GetForDisplayUnit(double &DisplayW,double &DisplayH) {
    if (DiaporamaObject->Parent->ApplicationConfig->ImageGeometry==GEOMETRY_4_3)            { DisplayW=1440; DisplayH=1080; }
    else if (DiaporamaObject->Parent->ApplicationConfig->ImageGeometry==GEOMETRY_16_9)      { DisplayW=1920; DisplayH=1080; }
    else /*if (DiaporamaObject->Parent->ApplicationConfig->ImageGeometry==GEOMETRY_40_17)*/ { DisplayW=1920; DisplayH=816;  }
}

//====================================================================================================================

void DlgSlideProperties::s_SlideNameChange(QString NewText) {
    DiaporamaObject->SlideName=NewText;
}

//====================================================================================================================

void DlgSlideProperties::s_ShotDurationChange(QTime NewValue) {
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    DiaporamaObject->List[Current].StaticDuration=QTime(0,0,0,0).msecsTo(NewValue);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::reject() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgSlidePropertiesWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") DiaporamaObject->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    done(1);
}

//====================================================================================================================

void DlgSlideProperties::accept() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgSlidePropertiesWSP->SaveWindowState(this);
    // Close the box
    done(0);
}

//====================================================================================================================

void DlgSlideProperties::GetSound() {
    int CurrentShot=ui->ShotTable->currentColumn();
    if ((CurrentShot>=0)&&(CurrentShot<DiaporamaObject->List.count())) CompositionList=&DiaporamaObject->List[CurrentShot].ShotComposition;
    int CurrentBlock=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CompositionList)&&(CurrentBlock>=0)&&(CurrentBlock<CompositionList->List.count())) CurrentTextItem=&CompositionList->List[CurrentBlock];

    bool IsVideo  =(CurrentTextItem)&&(CurrentTextItem->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentTextItem->BackgroundBrush.Video!=NULL);
    bool HaveSound=IsVideo && (CurrentTextItem->BackgroundBrush.SoundVolume!=0);

    // Only if this block is a video and don't have sound yet
    if (IsVideo && !HaveSound) {
        for (int i=0;i<CompositionList->List.count();i++) {
            if ((CurrentTextItem!=&CompositionList->List[i])&&(CompositionList->List[i].BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&(CompositionList->List[i].BackgroundBrush.Video))
                CompositionList->List[i].BackgroundBrush.SoundVolume=0;
        }
        CurrentTextItem->BackgroundBrush.SoundVolume=1;
        RefreshBlockTable(ui->BlockTable->currentRow());
    }
}

//====================================================================================================================

void DlgSlideProperties::RefreshControls() {
    // Ensure box is init and Current contain index of currented selected sequence
    if ((!IsFirstInitDone)||(!CompositionList)||(StopMAJSpinbox)) return;

    int CurrentShot=ui->ShotTable->currentColumn();
    if ((CurrentShot<0)||(CurrentShot>=DiaporamaObject->List.count())) return;

    StopMAJSpinbox=true;    // Disable reintrence in this RefreshControls function
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //--------------------------------------------------------------------
    // Update controls
    //--------------------------------------------------------------------
    CompositionList=&DiaporamaObject->List[CurrentShot].ShotComposition;
    ui->ShotLeftBt->setEnabled(CurrentShot>0);
    ui->ShotRightBt->setEnabled(CurrentShot<ui->ShotTable->columnCount()-1);

    int CurrentBlock=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CompositionList)&&(CurrentBlock>=0)&&(CurrentBlock<CompositionList->List.count())) CurrentTextItem=&CompositionList->List[CurrentBlock];
    ui->BlockUpBT->setEnabled(CurrentBlock>0);
    ui->BlockDownBT->setEnabled(CurrentBlock<ui->BlockTable->rowCount()-1);
    ui->RemoveShot->setEnabled(DiaporamaObject->List.count()>1);

    ui->CopyBlockBT->setEnabled(CurrentTextItem!=NULL);
    ui->CutBlockBT->setEnabled(CurrentTextItem!=NULL);
    ui->PasteBlockBT->setEnabled(GlobalMainWindow->Clipboard_Block!=NULL);

    // Slide name & duration and shot duration
    ui->SlideNameED->setText(DiaporamaObject->SlideName);
    ui->SlideDurationLabel->setText(QTime(0,0,0,0).addMSecs(DiaporamaObject->GetDuration()).toString("hh:mm:ss.zzz"));
    ui->ShotDurationED->setTime(QTime(0,0,0,0).addMSecs(DiaporamaObject->List[CurrentShot].StaticDuration));

    // Minimum shot duration display
    int  AddingDuration=0;
    if (CurrentShot==(DiaporamaObject->List.count()-1)) {   // If it's the last shot
        int TotalDuration=DiaporamaObject->GetDuration();
        int Duration=0;
        for (int i=0;i<DiaporamaObject->List.count();i++) Duration=Duration+DiaporamaObject->List[i].StaticDuration;
        if (Duration<TotalDuration) AddingDuration=TotalDuration-Duration;
    }
    if (AddingDuration==0) ui->MinShotDurationLabel->setText("");
        else ui->MinShotDurationLabel->setText(QString(QApplication::translate("DlgImageProperties","Lengthened to %1 sec")).arg(double(DiaporamaObject->List[CurrentShot].StaticDuration+AddingDuration)/1000));

    //====================================================================================================================

    cBrushDefinition    *CurrentBrush   =(CurrentTextItem!=NULL)?&CurrentTextItem->BackgroundBrush:NULL;
    bool                IsVisible       =(CurrentTextItem)&&(CurrentTextItem->IsVisible);
    bool                IsVideo         =(CurrentTextItem)&&(CurrentTextItem->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentTextItem->BackgroundBrush.Video!=NULL);
    bool                HaveSound       =IsVideo && (CurrentTextItem->BackgroundBrush.SoundVolume!=0);

    ui->VisibleBT->setIcon(QIcon(QString(!IsVisible?ICON_VISIBLE_OK:ICON_VISIBLE_KO)));
    ui->SoundBT->setIcon(QIcon(QString(!HaveSound?ICON_SOUND_OK:ICON_SOUND_KO)));
    ui->TVMarginsBT->setIcon(QIcon(QString(MagneticRuler.MagneticRuler?ICON_RULER_ON:ICON_RULER_OFF)));
    ui->SoundBT->setEnabled(IsVideo && !HaveSound);

    // Brush TAB part
    bool Allow_Brush  =(CurrentBrush!=NULL)&&(CurrentBrush->BrushType!=BRUSHTYPE_IMAGEDISK);
    bool Allow_Color1 =(Allow_Brush)&&((CurrentBrush->BrushType==BRUSHTYPE_SOLID)||(CurrentBrush->BrushType==BRUSHTYPE_PATTERN)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color2 =(Allow_Brush)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color3 =(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3);
    bool Allow_Pattern=(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_PATTERN);
    bool Allow_Library=(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGELIBRARY);
    bool Allow_File   =(IsVisible)&&(CurrentBrush!=NULL)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK);

    ui->BrushTypeLabel->setVisible(Allow_Brush);                                ui->BrushTypeLabel->setEnabled(IsVisible && Allow_Brush);
    ui->BrushTypeCombo->setVisible(Allow_Brush);                                ui->BrushTypeCombo->setEnabled(IsVisible && Allow_Brush);
    ui->ColorLabel1->setVisible(Allow_Color1);                                  ui->ColorLabel1->setEnabled(IsVisible && Allow_Color1);
    ui->ColorLabel2->setVisible(Allow_Color1);                                  ui->ColorLabel2->setEnabled(IsVisible && Allow_Color1);
    ui->FirstColorCombo->setVisible(Allow_Color1);                              ui->FirstColorCombo->setEnabled(IsVisible && Allow_Color1);
    ui->FinalColorCombo->setVisible(Allow_Color2);                              ui->FinalColorCombo->setEnabled(IsVisible && Allow_Color2);
    ui->IntermColorCombo->setVisible(Allow_Color3);                             ui->IntermColorCombo->setEnabled(IsVisible && Allow_Color3);
    ui->OrientationLabel->setVisible(Allow_Color2);                             ui->OrientationLabel->setEnabled(IsVisible && Allow_Color2);
    ui->OrientationCombo->setVisible(Allow_Color2);                             ui->OrientationCombo->setEnabled(IsVisible && Allow_Color2);
    ui->IntermPosLabel->setVisible(Allow_Color3);                               ui->IntermPosLabel->setEnabled(IsVisible && Allow_Color3);
    ui->IntermPosSlider->setVisible(Allow_Color3);                              ui->IntermPosSlider->setEnabled(IsVisible && Allow_Color3);
    ui->IntermPosED->setVisible(Allow_Color3);                                  ui->IntermPosED->setEnabled(IsVisible && Allow_Color3);
    ui->PatternLabel->setVisible(Allow_Pattern);                                ui->PatternLabel->setEnabled(IsVisible && Allow_Pattern);
    ui->PatternBrushCombo->setVisible(Allow_Pattern);                           ui->PatternBrushCombo->setEnabled(IsVisible && Allow_Pattern);
    ui->ImageLibraryLabel->setVisible(Allow_Library);                           ui->ImageLibraryLabel->setEnabled(IsVisible && Allow_Library);
    ui->BackgroundCombo->setVisible(Allow_Library);                             ui->BackgroundCombo->setEnabled(IsVisible && Allow_Library);
    ui->FileNameLabel->setVisible(Allow_File);                                  ui->FileNameLabel->setEnabled(IsVisible && Allow_File);
    ui->ImageGeometryLabel->setVisible(Allow_File);                             ui->ImageGeometryLabel->setEnabled(IsVisible && Allow_File);
    ui->ImageGeometryCB->setVisible(Allow_File);                                ui->ImageGeometryCB->setEnabled(IsVisible && Allow_File);

    ui->ImageEditCorrectBT->setEnabled(IsVisible && Allow_File);
    ui->VideoEditBT->setEnabled(IsVisible && Allow_File && (CurrentBrush->Video!=NULL));
    ui->TextEditBT->setEnabled(IsVisible);
    ui->VisibleBT->setEnabled(CurrentTextItem!=NULL);

    ui->FileNameED->setVisible(Allow_File);                                     ui->FileNameED->setEnabled(IsVisible && Allow_File);
    ui->FileNameBT->setVisible(Allow_File);                                     ui->FileNameBT->setEnabled(IsVisible && Allow_File);
    ui->TransformationLabel->setVisible(Allow_File);                            ui->TransformationLabel->setEnabled(IsVisible && Allow_File);
    ui->TransformationCB->setVisible(Allow_File);                               ui->TransformationCB->setEnabled(IsVisible && Allow_File);
    ui->BlurLabel->setVisible(!IsVideo && Allow_File);                          ui->BlurLabel->setEnabled(!IsVideo && IsVisible && Allow_File);
    ui->BlurSigmaED->setVisible(!IsVideo && Allow_File);                        ui->BlurSigmaED->setEnabled(!IsVideo && IsVisible && Allow_File);
    ui->BlurSigmaSlider->setVisible(!IsVideo && Allow_File);                    ui->BlurSigmaSlider->setEnabled(!IsVideo && IsVisible && Allow_File);
    ui->BlurRadiusLabel->setVisible(!IsVideo && Allow_File);                    ui->BlurRadiusLabel->setEnabled(!IsVideo && IsVisible && Allow_File);
    ui->BlurRadiusED->setVisible(!IsVideo && Allow_File);                       ui->BlurRadiusED->setEnabled(!IsVideo && IsVisible && Allow_File);
    ui->BlurRadiusSlider->setVisible(!IsVideo && Allow_File);                   ui->BlurRadiusSlider->setEnabled(!IsVideo && IsVisible && Allow_File);

    if (CurrentBrush!=NULL) {
        // Set brush type combo index
        for (int i=0;i<ui->BrushTypeCombo->count();i++) if (ui->BrushTypeCombo->itemData(i).toInt()==CurrentBrush->BrushType) ui->BrushTypeCombo->setCurrentIndex(i);
        ui->PatternBrushCombo->SetCurrentBrush(CurrentBrush);
        ui->FirstColorCombo->SetCurrentColor(&CurrentBrush->ColorD);
        ui->IntermColorCombo->SetCurrentColor(&CurrentBrush->ColorIntermed);
        ui->FinalColorCombo->SetCurrentColor(&CurrentBrush->ColorF);
        ui->OrientationCombo->SetCurrentBrush(CurrentBrush);
        ui->FirstColorCombo->SetCurrentColor(&CurrentBrush->ColorD);

        // Set controls depending on brush type
        switch (CurrentBrush->BrushType) {
            case BRUSHTYPE_NOBRUSH :
                break;
            case BRUSHTYPE_PATTERN :
            case BRUSHTYPE_SOLID :          break;
                ui->ColorLabel1->setText(QApplication::translate("DlgSlideProperties","Color :"));
                break;
            case BRUSHTYPE_GRADIENT3 :
            case BRUSHTYPE_GRADIENT2 :
                ui->IntermPosSlider->setValue(CurrentBrush->Intermediate*100);
                ui->IntermPosED->setValue(CurrentBrush->Intermediate*100);
                ui->ColorLabel1->setText(QApplication::translate("DlgSlideProperties","Colors :"));
                break;
            case BRUSHTYPE_IMAGELIBRARY :
                // Ensure BrushImage is valide
                if ((BackgroundList.SearchImage(CurrentBrush->BrushImage)==-1)&&(BackgroundList.List.count()>0)) CurrentBrush->BrushImage=BackgroundList.List[0].Name;
                ui->BackgroundCombo->SetCurrentBackground(CurrentBrush->BrushImage);
                break;
            case BRUSHTYPE_IMAGEDISK :
                cCustomGraphicsRectItem *RectItem=GetSelectItem();
                // Adjust aspect ratio (if custom mode)
                if ((RectItem)&&(!RectItem->KeepAspectRatio)) CurrentBrush->BrushFileCorrect.AspectRatio=(CurrentTextItem->h*ymax)/(CurrentTextItem->w*xmax);
                ui->FileNameED->setText(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video?CurrentBrush->Video->FileName:"");
                ui->ImageGeometryCB->setCurrentIndex(CurrentBrush->BrushFileCorrect.ImageGeometry);
                QImage *SourceImage=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,true,NULL):
                             CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),true,NULL,1,false,NULL):
                             NULL);
                ui->TransformationCB->SetCurrentFilter(SourceImage,CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform.OnOffFilter:&CurrentBrush->Image->BrushFileTransform.OnOffFilter);
                delete SourceImage;
                ui->BlurSigmaSlider->setValue((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurSigma*10);
                ui->BlurSigmaED->setValue((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurSigma);
                ui->BlurRadiusSlider->setValue(int((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurRadius));
                ui->BlurRadiusED->setValue(int((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurRadius));
                break;
        }
    }

    ui->RemoveBlock->setEnabled(CurrentTextItem!=NULL);

    //***********************
    // Size & Position
    //***********************
    ui->PosXEd->setEnabled(IsVisible);
    ui->PosYEd->setEnabled(IsVisible);
    ui->WidthEd->setEnabled(IsVisible);
    ui->HeightEd->setEnabled(IsVisible);
    ui->RotateXED->setEnabled(IsVisible);
    ui->RotateXSLD->setEnabled(IsVisible);
    ui->RotateYED->setEnabled(IsVisible);
    ui->RotateYSLD->setEnabled(IsVisible);
    ui->RotateZED->setEnabled(IsVisible);
    ui->RotateZSLD->setEnabled(IsVisible);
    ui->PosSize_X->setEnabled(IsVisible);
    ui->PosSize_Y->setEnabled(IsVisible);
    ui->PosSize_Width->setEnabled(IsVisible);
    ui->PosSize_Height->setEnabled(IsVisible);
    ui->Rotate_X->setEnabled(IsVisible);
    ui->Rotate_Y->setEnabled(IsVisible);
    ui->Rotate_Z->setEnabled(IsVisible);

    //***********************
    // Shape part
    //***********************
    ui->BackgroundFormCB->setEnabled(IsVisible);
    ui->PenSizeEd->setEnabled(IsVisible);
    ui->PenColorCB->setEnabled(IsVisible&&(CurrentTextItem)&&(CurrentTextItem->PenSize!=0));
    ui->PenStyleCB->setEnabled(IsVisible&&(CurrentTextItem)&&(CurrentTextItem->PenSize!=0));
    ui->OpacityCB->setEnabled(IsVisible);
    ui->ShadowEffectCB->setEnabled(IsVisible);
    ui->ShadowEffectED->setEnabled((IsVisible)&&(CurrentTextItem)&&(CurrentTextItem->FormShadow!=0));
    ui->ShadowColorCB->setEnabled((IsVisible)&&(CurrentTextItem)&&(CurrentTextItem->FormShadow!=0));

    // Refresh Scene Image
    RefreshSceneImage();

    // Refresh information zone
    UpdateDockInfo();

    QApplication::restoreOverrideCursor();
    StopMAJSpinbox=false;
}

//====================================================================================================================
// Refresh background image of the scene

void DlgSlideProperties::RefreshSceneImage() {
    if (InRefreshSceneImage) return;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    InRefreshSceneImage=true;

    // Get current shot object composition list
    int                 CurrentShot     =ui->ShotTable->currentColumn();
    cCompositionList    *CurrentList    =NULL;
    if ((CurrentShot>=0)&&(CurrentShot<DiaporamaObject->List.count())) CurrentList=&DiaporamaObject->List[CurrentShot].ShotComposition;

    // Get current block object in the current shot object composition list
    int                 CurrentBlock    =ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CurrentList)&&(CurrentBlock>=0)&&(CurrentBlock<CurrentList->List.count())) CurrentTextItem=&CurrentList->List[CurrentBlock];

    // Draw image of the scene under the background
    QPixmap NewImage=QPixmap::fromImage(*BackgroundImage);
    QPainter P;
    P.begin(&NewImage);

    for (int i=0;i<CurrentList->List.count();i++) if (CurrentList->List[i].IsVisible) {
        // Draw composition block
        if (CurrentList->List[i].BackgroundBrush.Video) {
            // If it's a video block, calc video position
            int StartVideoPos=QTime(0,0,0,0).msecsTo(CurrentList->List[i].BackgroundBrush.Video->StartPos);
            for (int k=0;k<CurrentShot;k++) {
                for (int l=0;l<DiaporamaObject->List[k].ShotComposition.List.count();l++) {
                    if (DiaporamaObject->List[k].ShotComposition.List[l].IndexKey==CurrentList->List[i].IndexKey) {
                        if (DiaporamaObject->List[k].ShotComposition.List[l].IsVisible) StartVideoPos+=DiaporamaObject->List[k].StaticDuration;
                        l=DiaporamaObject->List[k].ShotComposition.List.count();    // stop loop
                    }
                }
            }
            CurrentList->List[i].DrawCompositionObject(&P,double(ymax)/double(1080),0,0,xmax,ymax,true,0,StartVideoPos,NULL,1,NULL,true);
        } else CurrentList->List[i].DrawCompositionObject(&P,double(ymax)/double(1080),0,0,xmax,ymax,true,0,0,NULL,1,NULL,true);

        // Draw frame border
        if (CurrentTextItem==&CurrentList->List[i]) {
            // draw rect out of the rectangle
            P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
            QPen pen=QPen(Qt::red);
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            P.setPen(pen);
            P.setBrush(Qt::NoBrush);
            P.drawRect(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,CurrentTextItem->w*xmax,CurrentTextItem->h*ymax));
            P.setCompositionMode(QPainter::CompositionMode_SourceOver);

            // Update controls with position & size value
            if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
                ui->PosXEd->setRange(-200,200);      ui->PosXEd->setValue(CurrentTextItem->x*100);
                ui->PosYEd->setRange(-200,200);      ui->PosYEd->setValue(CurrentTextItem->y*100);
                ui->WidthEd->setRange(3,200);        ui->WidthEd->setValue(CurrentTextItem->w*100);
                ui->HeightEd->setRange(3,200);       ui->HeightEd->setValue(CurrentTextItem->h*100);
            } else { // DisplayUnit==DISPLAYUNIT_PIXELS
                double DisplayW,DisplayH;   GetForDisplayUnit(DisplayW,DisplayH);
                ui->PosXEd->setRange(-2*DisplayW,2*DisplayW);   ui->PosXEd->setValue(  CurrentTextItem->x*DisplayW);
                ui->PosYEd->setRange(-2*DisplayH,2*DisplayH);   ui->PosYEd->setValue(  CurrentTextItem->y*DisplayH);
                ui->WidthEd->setRange(3,2*DisplayW);            ui->WidthEd->setValue( CurrentTextItem->w*DisplayW);
                ui->HeightEd->setRange(3,2*DisplayH);           ui->HeightEd->setValue(CurrentTextItem->h*DisplayH);
            }
            ui->RotateXED->setValue(CurrentTextItem->RotateXAxis);      ui->RotateXSLD->setValue(CurrentTextItem->RotateXAxis);
            ui->RotateYED->setValue(CurrentTextItem->RotateYAxis);      ui->RotateYSLD->setValue(CurrentTextItem->RotateYAxis);
            ui->RotateZED->setValue(CurrentTextItem->RotateZAxis);      ui->RotateZSLD->setValue(CurrentTextItem->RotateZAxis);

            if ((CurrentTextItem->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentTextItem->BackgroundBrush.BrushFileCorrect.ImageGeometry==GEOMETRY_CUSTOM))
                CurrentTextItem->BackgroundBrush.BrushFileCorrect.AspectRatio=(CurrentTextItem->h*ymax)/(CurrentTextItem->w*xmax);

            //***********************
            // Shape TAB
            //***********************
            if (CurrentTextItem->BackgroundForm-1!=ui->BackgroundFormCB->currentIndex())    ui->BackgroundFormCB->setCurrentIndex(CurrentTextItem->BackgroundForm-1);
            if (CurrentTextItem->PenSize!=ui->PenSizeEd->value())                           ui->PenSizeEd->setValue(int(CurrentTextItem->PenSize));
            if (CurrentTextItem->Opacity!=ui->OpacityCB->currentIndex())                    ui->OpacityCB->setCurrentIndex(CurrentTextItem->Opacity);
            if (CurrentTextItem->FormShadow!=ui->ShadowEffectCB->currentIndex())            ui->ShadowEffectCB->setCurrentIndex(CurrentTextItem->FormShadow);
            if (CurrentTextItem->FormShadowDistance!=ui->ShadowEffectED->value())           ui->ShadowEffectED->setValue(int(CurrentTextItem->FormShadowDistance));
            ui->PenColorCB->SetCurrentColor(&CurrentTextItem->PenColor);
            ui->ShadowColorCB->SetCurrentColor(&CurrentTextItem->FormShadowColor);

            for (int i=0;i<ui->PenStyleCB->count();i++) if (ui->PenStyleCB->itemData(i).toInt()==CurrentTextItem->PenStyle) {
                if (i!=ui->PenStyleCB->currentIndex()) ui->PenStyleCB->setCurrentIndex(i);
                break;
            }

        } else {
            // draw rect out of the rectangle
            P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
            QPen pen=QPen(Qt::lightGray);
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            P.setPen(pen);
            P.setBrush(Qt::NoBrush);
            P.drawRect(QRectF(CurrentList->List[i].x*xmax,CurrentList->List[i].y*ymax,CurrentList->List[i].w*xmax,CurrentList->List[i].h*ymax));
            P.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }
    }

    // Draw rullers if they was enabled
    if (MagneticRuler.MagneticRuler==true) {
        QColor col=QColor(0,255,0);
        QPen   pen=QPen(col);
        pen.setWidth(2);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setStyle(Qt::DotLine);
        P.setPen(pen);
        P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        P.setBrush(Qt::NoBrush);
        P.drawLine(MagneticRuler.MagnetX1,0,MagneticRuler.MagnetX1,ymax);
        P.drawLine(0,MagneticRuler.MagnetY1,xmax,MagneticRuler.MagnetY1);
        P.drawLine(MagneticRuler.MagnetX2,0,MagneticRuler.MagnetX2,ymax);
        P.drawLine(0,MagneticRuler.MagnetY2,xmax,MagneticRuler.MagnetY2);
        P.drawLine(MagneticRuler.MagnetX3,0,MagneticRuler.MagnetX3,ymax);
        P.drawLine(0,MagneticRuler.MagnetY3,xmax,MagneticRuler.MagnetY3);
        P.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    P.end();

    // Remove old image if exist
    int i=0;
    while ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()!="image")) i++;
    if ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()=="image")) {
        QGraphicsPixmapItem *im=(QGraphicsPixmapItem *)scene->items().at(i);
        scene->removeItem(im);
        delete im;
    }

    // Add image to the background of the scene
    QGraphicsPixmapItem *im=scene->addPixmap(NewImage);    // Image will be delete by scene
    im->setData(0,QVariant(QString("image")));
    im->setZValue(200);
    im->setPos(0,0);
    //delete NewImage;

    // Refresh thumbnail
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->setUpdatesEnabled(true);

    InRefreshSceneImage=false;
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void DlgSlideProperties::RefreshSceneImageAndCache(cCompositionObject *CurrentTextItem,cBrushDefinition *CurrentBrush) {
    if (CurrentBrush->Image) {
        if (CurrentBrush->Image->CacheImage) {
            if (CurrentBrush->Image->CacheImage!=CurrentBrush->Image->CacheFullImage) delete CurrentBrush->Image->CacheImage;
            CurrentBrush->Image->CacheImage=NULL;
        }
        if (CurrentBrush->Image->CacheFullImage) {
            delete CurrentBrush->Image->CacheFullImage;
            CurrentBrush->Image->CacheFullImage=NULL;
        }
    } else if (CurrentBrush->Video) {
        if (CurrentBrush->Video->CacheFirstImage) {
            delete CurrentBrush->Video->CacheFirstImage;
            CurrentBrush->Video->CacheFirstImage=NULL;
        }
        if (CurrentBrush->Video->CacheLastImage) {
            delete CurrentBrush->Video->CacheLastImage;
            CurrentBrush->Video->CacheLastImage=NULL;
        }
        for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i].ShotComposition.List.count();j++)
            if ((DiaporamaObject->List[i].ShotComposition.List[j].IndexKey==CurrentTextItem->IndexKey)&&(DiaporamaObject->List[i].ShotComposition.List[j].CachedBrushBrush)) {
            delete DiaporamaObject->List[i].ShotComposition.List[j].CachedBrushBrush;
            DiaporamaObject->List[i].ShotComposition.List[j].CachedBrushBrush=NULL;
        }
    }
    RefreshSceneImage();
}

//====================================================================================================================
// Display or hide TV Margins
//====================================================================================================================

void DlgSlideProperties::s_TVMarginsBt() {
    if (MagneticRuler.MagneticRuler==true) MagneticRuler.MagneticRuler=false; else MagneticRuler.MagneticRuler=true;
    DiaporamaObject->Parent->ApplicationConfig->SlideRuler=MagneticRuler.MagneticRuler;
    RefreshControls();
}

//====================================================================================================================
// Update dock informations
//====================================================================================================================

void DlgSlideProperties::UpdateDockInfo() {
    while (ui->TableInfo->rowCount()>0) ui->TableInfo->removeRow(0);
    int CurrentBlock=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=NULL;
    if ((CompositionList)&&(CurrentBlock>=0)&&(CurrentBlock<CompositionList->List.count())) CurrentTextItem=&CompositionList->List[CurrentBlock];
    if (CurrentTextItem) {
        ui->TableInfo->insertRow(ui->TableInfo->rowCount());
        ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Object type")));
        ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(CurrentTextItem->BackgroundBrush.BrushType!=BRUSHTYPE_IMAGEDISK?QApplication::translate("DlgSlideProperties","Title"):
                                                                                  CurrentTextItem->BackgroundBrush.Image!=NULL?QApplication::translate("DlgSlideProperties","Image"):
                                                                                  QApplication::translate("DlgSlideProperties","Video")));

        if (CurrentTextItem->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK) {
            ui->TableInfo->insertRow(ui->TableInfo->rowCount());
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Filename")));
            ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(CurrentTextItem->BackgroundBrush.Image!=NULL?QFileInfo(CurrentTextItem->BackgroundBrush.Image->FileName).fileName():
                                                                                      CurrentTextItem->BackgroundBrush.Video!=NULL?QFileInfo(CurrentTextItem->BackgroundBrush.Video->FileName).fileName():""));

            if (CurrentTextItem->BackgroundBrush.Image!=NULL) for (int i=0;i<CurrentTextItem->BackgroundBrush.Image->ExivValue.count();i++) {
                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                QString Value=CurrentTextItem->BackgroundBrush.Image->ExivValue[i];
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(Value.left(Value.indexOf("##"))));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(Value.right(Value.length()-Value.indexOf("##")-QString("##").length())));
            } else if (CurrentTextItem->BackgroundBrush.Video!=NULL) {
                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Image size")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1x%2").arg(CurrentTextItem->BackgroundBrush.Video->ImageWidth).arg(CurrentTextItem->BackgroundBrush.Video->ImageHeight)));

                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Video format")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(CurrentTextItem->BackgroundBrush.Video->VideoDecoderCodec->name));

                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Bitrate")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(int(double(CurrentTextItem->BackgroundBrush.Video->ffmpegVideoFile->bit_rate)/1024))+" k/s"));

                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Frame rate")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(
                        int(double(CurrentTextItem->BackgroundBrush.Video->ffmpegVideoFile->streams[CurrentTextItem->BackgroundBrush.Video->VideoStreamNumber]->r_frame_rate.num)/
                            double(CurrentTextItem->BackgroundBrush.Video->ffmpegVideoFile->streams[CurrentTextItem->BackgroundBrush.Video->VideoStreamNumber]->r_frame_rate.den)))
                        +" "+QApplication::translate("DlgSlideProperties","fps","frame per second")));

                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Aspect ratio")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg((double(CurrentTextItem->BackgroundBrush.Video->ImageWidth)*CurrentTextItem->BackgroundBrush.Video->AspectRatio)/double(CurrentTextItem->BackgroundBrush.Video->ImageHeight),0,'f',3)));

                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Audio format")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(CurrentTextItem->BackgroundBrush.Video->AudioDecoderCodec->name));

                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Frequency")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(CurrentTextItem->BackgroundBrush.Video->ffmpegVideoFile->streams[CurrentTextItem->BackgroundBrush.Video->AudioStreamNumber]->codec->sample_rate)
                        +" "+QApplication::translate("DlgSlideProperties","hz","audio frequency")));

                ui->TableInfo->insertRow(ui->TableInfo->rowCount());
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,0,new QTableWidgetItem(QApplication::translate("DlgSlideProperties","Channels")));
                ui->TableInfo->setItem(ui->TableInfo->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(CurrentTextItem->BackgroundBrush.Video->ffmpegVideoFile->streams[CurrentTextItem->BackgroundBrush.Video->AudioStreamNumber]->codec->channels)));
            }
            ui->TableInfo->resizeColumnsToContents();
            ui->TableInfo->resizeRowsToContents();
        }
    }
}

//====================================================================================================================

cCustomGraphicsRectItem *DlgSlideProperties::GetSelectItem() {
    if (scene==NULL) return NULL;
    cCustomGraphicsRectItem *CurrentTextItem=NULL;
    for (int i=0;i<scene->selectedItems().count();i++) {
        QGraphicsItem   *Item=scene->selectedItems()[i];
        QString         data =Item->data(0).toString();
        if (data=="CustomGraphicsRectItem")      CurrentTextItem=(cCustomGraphicsRectItem *)Item;
        else if (data=="ResizeGraphicsRectItem") CurrentTextItem=((cResizeGraphicsRectItem *)Item)->RectItem;
    }
    return CurrentTextItem;
}

//====================================================================================================================

cCompositionObject *DlgSlideProperties::GetSelectedGlobalCompositionObject() {
    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL;
    int i=0;
    while ((i<DiaporamaObject->ObjectComposition.List.count())&&(DiaporamaObject->ObjectComposition.List[i].IndexKey!=CompositionList->List[CurrentBlock].IndexKey)) i++;
    if (i<DiaporamaObject->ObjectComposition.List.count()) return &DiaporamaObject->ObjectComposition.List[i]; else return NULL;
}

cCompositionObject *DlgSlideProperties::GetSelectedCompositionObject() {
    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return NULL; else return &CompositionList->List[CurrentBlock];
}

cBrushDefinition *DlgSlideProperties::GetCurrentGlobalBrush() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((CurrentTextItem)&&(CurrentTextItem->Opacity<4)) return &CurrentTextItem->BackgroundBrush;
        else return NULL;
}

cBrushDefinition *DlgSlideProperties::GetCurrentBrush() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((CurrentTextItem)&&(CurrentTextItem->Opacity<4)) return &CurrentTextItem->BackgroundBrush;
        else return NULL;
}

//====================================================================================================================
// Handler for position, size & rotation controls
//====================================================================================================================

//========= X position
void DlgSlideProperties::s_ChgPosXValue(double Value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();     if (RectItem==NULL)     return;

    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        CurrentTextItem->x=Value/100;
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        double DisplayW,DisplayH;   GetForDisplayUnit(DisplayW,DisplayH);
        CurrentTextItem->x=(Value/DisplayW);
    }
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshSceneImage();
}

//========= Y position
void DlgSlideProperties::s_ChgPosYValue(double Value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();     if (RectItem==NULL)     return;
    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        CurrentTextItem->y=Value/100;
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        double DisplayW,DisplayH;   GetForDisplayUnit(DisplayW,DisplayH);
        CurrentTextItem->y=(Value/DisplayH);
    }
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshSceneImage();
}

//========= Width
void DlgSlideProperties::s_ChgWidthValue(double Value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();     if (RectItem==NULL)     return;
    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        CurrentTextItem->w=Value/100;
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        double DisplayW,DisplayH;   GetForDisplayUnit(DisplayW,DisplayH);
        CurrentTextItem->w=(Value/DisplayW);
    }
    if (RectItem->KeepAspectRatio) CurrentTextItem->h=((CurrentTextItem->w*xmax)*RectItem->AspectRatio)/ymax;
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshSceneImage();
}

//========= Height
void DlgSlideProperties::s_ChgHeightValue(double Value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();     if (RectItem==NULL)     return;
    if (DiaporamaObject->Parent->ApplicationConfig->DisplayUnit==DISPLAYUNIT_PERCENT) {
        CurrentTextItem->h=Value/100;
    } else { // DisplayUnit==DISPLAYUNIT_PIXELS
        double DisplayW,DisplayH;   GetForDisplayUnit(DisplayW,DisplayH);
        CurrentTextItem->h=(Value/DisplayH);
    }
    if (RectItem->KeepAspectRatio) CurrentTextItem->w=((CurrentTextItem->h*ymax)/RectItem->AspectRatio)/xmax;
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshSceneImage();
}

//========= X Rotation value
void DlgSlideProperties::s_ChgRotateXValue(int Value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;

    CurrentTextItem->RotateXAxis=Value;
    RefreshSceneImage();
}

//========= Y Rotation value
void DlgSlideProperties::s_ChgRotateYValue(int Value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;

    CurrentTextItem->RotateYAxis=Value;
    RefreshSceneImage();
}

//========= Z Rotation value
void DlgSlideProperties::s_ChgRotateZValue(int Value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;

    CurrentTextItem->RotateZAxis=Value;
    RefreshSceneImage();
}

//***********************
// Shape part
//***********************

//========= Background forme
void DlgSlideProperties::s_ChangeBackgroundForm(int Style) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->BackgroundForm=Style+1;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshSceneImage();
}

//========= Opacity
void DlgSlideProperties::s_ChangeOpacity(int Style) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->Opacity=Style;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshSceneImage();
}

//========= Border pen size
void DlgSlideProperties::s_ChgPenSize(int) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->PenSize=ui->PenSizeEd->value();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    ui->PenColorCB->setEnabled(CurrentTextItem->PenSize!=0);
    ui->PenStyleCB->setEnabled(CurrentTextItem->PenSize!=0);
    RefreshSceneImage();
}

//========= Border pen style
void DlgSlideProperties::s_ChangePenStyle(int index) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->PenStyle=ui->PenStyleCB->itemData(index).toInt();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshSceneImage();
}

//========= Border pen color
void DlgSlideProperties::s_ChPenColorCB(int) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->PenColor=ui->PenColorCB->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshSceneImage();
}

//========= Shape shadow style
void DlgSlideProperties::s_ChgShadowFormValue(int value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->FormShadow=value;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    ui->ShadowEffectED->setEnabled(CurrentTextItem->FormShadow!=0);
    ui->ShadowColorCB->setEnabled(CurrentTextItem->FormShadow!=0);
    RefreshSceneImage();
}

//========= Shape shadow distance
void DlgSlideProperties::s_ChgShadowDistanceValue(int value) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->FormShadowDistance =value;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshSceneImage();
}

//========= shadow color
void DlgSlideProperties::s_ChgShadowColorCB(int) {
    if (StopMAJSpinbox || InRefreshSceneImage) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->FormShadowColor=ui->ShadowColorCB->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshSceneImage();
}

//====================================================================================================================

void DlgSlideProperties::MakeFormIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0);
        Object.Text                     ="";
        Object.x                        =0;
        Object.y                        =0;
        Object.w                        =1;
        Object.h                        =1;
        Object.BackgroundForm           =i+1;
        Object.Opacity                  =4;
        Object.PenSize                  =1;
        Object.PenStyle                 =Qt::SolidLine;
        Object.PenColor                 ="#000000";
        Object.BackgroundBrush.BrushType=BRUSHTYPE_NOBRUSH;
        QPixmap  Image(32,32);
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,32,32),"#ffffff");
        Object.DrawCompositionObject(&Painter,1,0,0,32,32,true,0,0,NULL,1,NULL,false);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

void DlgSlideProperties::MakeBorderStyleIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        QPixmap  Image(32,32);
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,32,32),"#ffffff");
        QPen Pen;
        Pen.setColor(Qt::black);
        Pen.setStyle((Qt::PenStyle)UICB->itemData(i).toInt());
        Pen.setWidth(2);
        Painter.setPen(Pen);
        Painter.setBrush(QBrush(QColor("#ffffff")));
        Painter.drawLine(0,16,32,16);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeBrushTypeCombo(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->BrushType=ui->BrushTypeCombo->itemData(Value).toInt();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_IntermPosSliderMoved(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->Intermediate=double(Value)/100;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_IntermPosED(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->Intermediate=double(Value)/100;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}


//====================================================================================================================

void DlgSlideProperties::ChangeBrushDiskFile() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)||(CurrentBrush->BrushType!=BRUSHTYPE_IMAGEDISK)) return;

    QString NewFile=QFileDialog::getOpenFileName(this,
                                                 QApplication::translate("DlgSlideProperties","Select a file"),
                                                 GlobalMainWindow->ApplicationConfig->RememberLastDirectories?GlobalMainWindow->ApplicationConfig->LastMediaPath:"",
                                                 GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(CurrentBrush->Image?cApplicationConfig::IMAGEFILE:cApplicationConfig::VIDEOFILE));
    QApplication::processEvents();
    if (NewFile=="") return;
    if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use
    QString BrushFileName=QFileInfo(NewFile).absoluteFilePath();

    if (CurrentBrush->Image)            CurrentBrush->Image->GetInformationFromFile(BrushFileName);
        else if (CurrentBrush->Video)   CurrentBrush->Video->GetInformationFromFile(BrushFileName,false);

    // free CachedBrushBrush
    for (int j=0;j<DiaporamaObject->List.count();j++) for (int k=0;k<DiaporamaObject->List[j].ShotComposition.List.count();k++) {
        if (DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush) {
            delete DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush;
            DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush=NULL;
        }
    }

    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshBlockTable(ui->BlockTable->currentRow());
}

//====================================================================================================================

void DlgSlideProperties::s_ChTransformationCB(int) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)||(CurrentBrush->BrushType!=BRUSHTYPE_IMAGEDISK)) return;
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->OnOffFilter=ui->TransformationCB->GetCurrentFilter();
    RefreshSceneImageAndCache(CurrentTextItem,CurrentBrush);
}

//====================================================================================================================

void DlgSlideProperties::s_BlurSigmaSliderMoved(int Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)||(CurrentBrush->BrushType!=BRUSHTYPE_IMAGEDISK)) return;
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurSigma=double(Value)/10;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaED->setValue(Filter->BlurSigma);
    RefreshSceneImageAndCache(CurrentTextItem,CurrentBrush);
}

//====================================================================================================================

void DlgSlideProperties::s_BlurSigmaValueED(double Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)||(CurrentBrush->BrushType!=BRUSHTYPE_IMAGEDISK)) return;
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurSigma=Value;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaED->setValue(Filter->BlurSigma);
    RefreshSceneImageAndCache(CurrentTextItem,CurrentBrush);
}

//====================================================================================================================

void DlgSlideProperties::s_BlurRadiusSliderMoved(int Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)||(CurrentBrush->BrushType!=BRUSHTYPE_IMAGEDISK)) return;
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurRadius=double(Value);
    ui->BlurRadiusSlider->setValue(int(Filter->BlurRadius));
    ui->BlurRadiusED->setValue(int(Filter->BlurRadius));
    RefreshSceneImageAndCache(CurrentTextItem,CurrentBrush);
}

//====================================================================================================================
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Pattern shape combo
void DlgSlideProperties::s_ChIndexPatternBrushCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->PatternType=ui->PatternBrushCombo->GetCurrentBrush()->PatternType;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Gradient shape orientation
void DlgSlideProperties::s_ChIndexGradientOrientationCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->GradientOrientation=ui->OrientationCombo->GetCurrentBrush()->GradientOrientation;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Shape/Gradient shape first color
void DlgSlideProperties::s_ChIndexGradientFirstColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->ColorD=ui->FirstColorCombo->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Gradient shape last color
void DlgSlideProperties::s_ChIndexGradientFinalColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->ColorF=ui->FinalColorCombo->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Gradient shape intermediate color
void DlgSlideProperties::s_ChIndexGradientIntermColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->ColorIntermed=ui->IntermColorCombo->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Background image
void DlgSlideProperties::s_ChIndexBackgroundCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->BrushImage=ui->BackgroundCombo->GetCurrentBackground();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= geometry of embeded image or video
void DlgSlideProperties::s_ChangeImageGeometry(int value) {
    if (StopMAJSpinbox) return;
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cBrushDefinition        *CurrentBrush   =GetCurrentBrush();                 if (!CurrentBrush)      return;
    cCustomGraphicsRectItem *RectItem       =GetSelectItem();                   if (!RectItem)          return;
    QImage                  *Image          =CurrentBrush->Image?CurrentBrush->Image->CacheImage:CurrentBrush->Video?CurrentBrush->Video->CacheFirstImage:NULL;
    if (!Image) return;

    double NewW=1;
    double NewH=1;
    CurrentBrush->BrushFileCorrect.ImageGeometry=value;

    switch (value) {
    case GEOMETRY_PROJECT:
        CurrentBrush->BrushFileCorrect.AspectRatio=double(GlobalMainWindow->Diaporama->InternalHeight)/double(GlobalMainWindow->Diaporama->InternalWidth);
        RectItem->KeepAspectRatio=true;
        break;
    case GEOMETRY_IMAGE:
        // if image have rotation => compute new image to determine rotated image aspect ratio
        if (CurrentBrush->BrushFileCorrect.ImageRotation!=0) {
            QTransform matrix;
            matrix.rotate(CurrentBrush->BrushFileCorrect.ImageRotation,Qt::ZAxis);
            QImage *SourceImage=new QImage(Image->transformed(matrix));
            CurrentBrush->BrushFileCorrect.AspectRatio=double(SourceImage->height())/double(SourceImage->width());
            delete SourceImage;
        // if image have no rotation => determine image aspect ratio
        } else CurrentBrush->BrushFileCorrect.AspectRatio=double(Image->height())/double(Image->width());
        RectItem->KeepAspectRatio=true;
        break;
    default:
        CurrentBrush->BrushFileCorrect.AspectRatio=(CurrentTextItem->h*GlobalMainWindow->Diaporama->InternalHeight)/(CurrentTextItem->w*GlobalMainWindow->Diaporama->InternalWidth);
        RectItem->KeepAspectRatio=true;
        break;
    }

    RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
    NewW=CurrentTextItem->w*GlobalMainWindow->Diaporama->InternalWidth;
    NewH=NewW*CurrentBrush->BrushFileCorrect.AspectRatio;
    NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
    NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;

    if (NewH>1) {
        NewH=CurrentTextItem->h*GlobalMainWindow->Diaporama->InternalHeight;
        NewW=NewH/CurrentBrush->BrushFileCorrect.AspectRatio;
        NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
        NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
    }

    CurrentTextItem->w=NewW;
    CurrentTextItem->h=NewH;

    // Update RectItem
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();

    // free CachedBrushBrush
    for (int j=0;j<DiaporamaObject->List.count();j++) for (int k=0;k<DiaporamaObject->List[j].ShotComposition.List.count();k++) {
        if (DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush) {
            delete DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush;
            DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush=NULL;
        }
    }

    RefreshControls();
}

//====================================================================================================================
// Buttons associated to a Dialog box
//====================================================================================================================

//========= Open text editor
void DlgSlideProperties::TextEditor() {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if (DlgTextEdit(CurrentTextItem,this).exec()==0) {
        ApplyGlobalPropertiesToAllShots(CurrentTextItem);
        ui->BlockTable->item(ui->BlockTable->currentRow(),2)->setText(CurrentTextItem->Text);
        RefreshControls();
    }
}

//========= Open image correction editor
void DlgSlideProperties::ImageEditCorrect() {
    if (StopMAJSpinbox) return;
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cBrushDefinition        *CurrentBrush=GetCurrentBrush();                    if (!CurrentBrush) return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();                          if (!RectItem) return;

    if (CurrentBrush->Image) {
        DlgImageCorrection(CurrentTextItem->BackgroundForm,CurrentBrush,&CurrentBrush->BrushFileCorrect,CurrentBrush->Image->CacheImage,this).exec();
    } else if (CurrentBrush->Video) {
        // Compute position of video
        int Position=0;
        for (int i=0;i<ui->ShotTable->currentColumn();i++) for (int j=0;j<DiaporamaObject->List[i].ShotComposition.List.count();j++)
            if (DiaporamaObject->List[i].ShotComposition.List[j].IndexKey==CurrentTextItem->IndexKey) {
                if (DiaporamaObject->List[i].ShotComposition.List[j].IsVisible) Position+=DiaporamaObject->List[i].StaticDuration;
        }
        QImage *CacheImage=CurrentBrush->Video->ImageAt(true,Position,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),true,NULL,1,false,&CurrentBrush->Video->BrushFileTransform,false);
        if (CacheImage) {
            DlgImageCorrection(CurrentTextItem->BackgroundForm,CurrentBrush,&CurrentBrush->BrushFileCorrect,CacheImage,this).exec();
            delete CacheImage;
        }
    }

    // free CachedBrushBrush
    for (int j=0;j<DiaporamaObject->List.count();j++) for (int k=0;k<DiaporamaObject->List[j].ShotComposition.List.count();k++) {
        if (DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush) {
            delete DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush;
            DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush=NULL;
        }
    }

    if (CurrentBrush->BrushFileCorrect.ImageGeometry==GEOMETRY_CUSTOM) {
        RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
        CurrentTextItem->h=(CurrentTextItem->w*xmax*RectItem->AspectRatio)/ymax;

        // Adjust height and width to image stay in screen
        if (((CurrentTextItem->y+CurrentTextItem->h)*ymax)>ymax) {
            CurrentTextItem->h=1-CurrentTextItem->y;
            CurrentTextItem->w=((CurrentTextItem->h*ymax)/RectItem->AspectRatio)/xmax;
        }

        // Update RectItem
        RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    }
    RefreshControls();
}

//========= Open video editor
void DlgSlideProperties::VideoEdit() {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();        if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cBrushDefinition    *CurrentBrush=&CurrentTextItem->BackgroundBrush;        if (!CurrentBrush) return;
    if (!CurrentBrush->Video) return;
    DlgVideoEdit(CurrentBrush,this).exec();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);

    // free CachedBrushBrush
    for (int j=0;j<DiaporamaObject->List.count();j++) for (int k=0;k<DiaporamaObject->List[j].ShotComposition.List.count();k++) {
        if (DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush) {
            delete DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush;
            DiaporamaObject->List[j].ShotComposition.List[k].CachedBrushBrush=NULL;
        }
    }

    RefreshControls();
}

//====================================================================================================================
// Shot table part
//====================================================================================================================

// Refresh the ShotTable

void DlgSlideProperties::RefreshShotTable(int SetCurrentIndex) {
    int CellHeight=ui->ShotTable->rowHeight(0);
    int CellWidth =GlobalMainWindow->Diaporama->GetWidthForHeight(CellHeight);

    ui->ShotTable->setUpdatesEnabled(false);
    StopMAJSpinbox=true;
    while (ui->ShotTable->columnCount()>0) ui->ShotTable->removeColumn(0);    // Clear all
    for (int i=0;i<DiaporamaObject->List.count();i++) {
        ui->ShotTable->insertColumn(i);
        wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->ShotTable,THUMBNAILTYPE_SHOT);
        Object->DiaporamaObject=DiaporamaObject;
        ui->ShotTable->setCellWidget(0,i,Object);
        ui->ShotTable->setColumnWidth(i,CellWidth);
    }
    ui->ShotTable->setUpdatesEnabled(true);
    StopMAJSpinbox=false;
    ui->ShotTable->setCurrentCell(0,SetCurrentIndex);
}

//====================================================================================================================
// User select a shot in the ShotTable widget

void DlgSlideProperties::s_ShotTable_SelectionChanged() {
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    CompositionList=&DiaporamaObject->List[Current].ShotComposition;
    RefreshBlockTable(0);
}

//====================================================================================================================

void DlgSlideProperties::s_ShotTable_AddShot() {
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;

    DiaporamaObject->List.insert(Current+1,cDiaporamaShot(DiaporamaObject));
    cDiaporamaShot *imagesequence=&DiaporamaObject->List[Current+1];
    for (int i=0;i<CompositionList->List.count();i++) {
        imagesequence->ShotComposition.List.append(cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionList->List[i].IndexKey));
        imagesequence->ShotComposition.List[i].CopyFromCompositionObject(&CompositionList->List[i]);
    }
    RefreshShotTable(Current+1);
}

//====================================================================================================================

void DlgSlideProperties::s_ShotTable_RemoveShot() {
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    if (DiaporamaObject->List.count()<2) return;
    if (QMessageBox::question(this,QApplication::translate("DlgSlideProperties","Remove shot"),QApplication::translate("DlgSlideProperties","Are you sure to want to delete this shot?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No) return;
    DiaporamaObject->List.removeAt(Current);
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->removeColumn(Current);
    ui->ShotTable->setUpdatesEnabled(true);
    RefreshShotTable(ui->ShotTable->currentColumn()>0?ui->ShotTable->currentColumn():0);
}

//====================================================================================================================
// Block table/scene part
//====================================================================================================================

// Refresh the BlockTable

void DlgSlideProperties::RefreshBlockTable(int SetCurrentIndex) {
    // Delete scene and all of it's content, if exist
    StopMAJSpinbox=true;
    if (scene) {
        // delete all items
        while (scene->items().count()>0) {
            QGraphicsItem *Item=scene->items().at(0);
            QString       data =Item->data(0).toString();

            scene->removeItem(Item);    // Remove item from the scene

            if (data=="CustomGraphicsRectItem") {
                delete (cCustomGraphicsRectItem *)Item;
            } else if (data=="image") {
                delete (QGraphicsPixmapItem *)Item;
            } else if (data!="ResizeGraphicsRectItem") {
                ExitApplicationWithFatalError("Unkwnon item type in DlgSlideProperties::Clean");
            }
        }
    }
    // Calculate scene size
    xmax=ui->GraphicsView->width();
    ymax=GlobalMainWindow->Diaporama->GetHeightForWidth(xmax);
    if (ymax>ui->GraphicsView->height()) {
        ymax=ui->GraphicsView->height();
        xmax=GlobalMainWindow->Diaporama->GetWidthForHeight(ymax);
    }
    // Ensure scene is created. If not : create it
    if (!scene) {
        // create the scene
        scene = new QGraphicsScene();
        connect(scene,SIGNAL(selectionChanged()),this,SLOT(s_Scene_SelectionChanged()));

        // Setup scene to control
        ui->GraphicsView->setScene(scene);
        ui->GraphicsView->setInteractive(true);
        ui->GraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    }

    // Check if BackgroundImage have correct size
    if ((BackgroundImage)&&((xmax!=BackgroundImage->width())||(ymax!=BackgroundImage->height()))) {
        delete BackgroundImage;
        BackgroundImage=NULL;
    }

    // Prepare BackgroundImage if not exist
    if (!BackgroundImage) {
        QPainter Painter;
        BackgroundImage=new QImage(xmax,ymax,QImage::Format_ARGB32_Premultiplied);
        Painter.begin(BackgroundImage);
        DiaporamaObject->Parent->PrepareBackground(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject),xmax,ymax,&Painter,0,0,false);
        Painter.end();

        // Ensure scene size if correct
        scene->setSceneRect(QRectF(0,0,xmax,ymax));
        ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);
    }

    // define 5% TV Margins
    MagneticRuler.MagnetX1=xmax*0.05;
    MagneticRuler.MagnetY1=ymax*0.05;
    MagneticRuler.MagnetX2=xmax-xmax*0.05;
    MagneticRuler.MagnetY2=ymax-ymax*0.05;

    // define centering ruller
    MagneticRuler.MagnetX3=xmax*0.5;
    MagneticRuler.MagnetY3=ymax*0.5;

    // Fill the scene with block item by creating cCustomGraphicsRectItem associate to existing cCompositionObject
    NextZValue=500;
    for (int i=0;i<CompositionList->List.count();i++) if (CompositionList->List[i].IsVisible) {
        // Create and add to scene a cCustomGraphicsRectItem
        new cCustomGraphicsRectItem(scene,NextZValue,&CompositionList->List[i].x,&CompositionList->List[i].y,
                    NULL,&CompositionList->List[i].w,&CompositionList->List[i].h,xmax,ymax,
                    ((CompositionList->List[i].BackgroundBrush.BrushFileCorrect.ImageGeometry==GEOMETRY_CUSTOM)&&
                    (CompositionList->List[i].BackgroundBrush.BrushType!=BRUSHTYPE_IMAGEDISK))?false:true,
                    CompositionList->List[i].BackgroundBrush.BrushFileCorrect.AspectRatio,
                    &MagneticRuler,this,TYPE_DlgSlideProperties,CompositionList->List[i].IndexKey);

    }
    NextZValue+=10;  // 10 by 10 step for ZValue
    StopMAJSpinbox=false;

    ui->BlockTable->setUpdatesEnabled(false);
    while (ui->BlockTable->rowCount()>0) ui->BlockTable->removeRow(0);    // Clear all
    for (int i=0;i<CompositionList->List.count();i++) {
        ui->BlockTable->insertRow(i);
        cCompositionObject  *CompoObject=&CompositionList->List[i];
        QTableWidgetItem    *ItemC0,*ItemC1,*ItemC2;
        // Image block
        if ((CompoObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&(CompoObject->BackgroundBrush.Image)) {
            ItemC0=new QTableWidgetItem(QIcon(CompoObject->IsVisible?ICON_OBJECT_IMAGE:ICON_OBJECT_IMAGEHIDE),"");
            ItemC1=new QTableWidgetItem("");
            ItemC2=new QTableWidgetItem(QFileInfo(CompoObject->BackgroundBrush.Image?CompoObject->BackgroundBrush.Image->FileName:CompoObject->BackgroundBrush.Video?CompoObject->BackgroundBrush.Video->FileName:"").fileName());
        // Video block
        } else if ((CompoObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&(CompoObject->BackgroundBrush.Video)) {
            ItemC0=new QTableWidgetItem(QIcon(CompoObject->IsVisible?ICON_OBJECT_MOVIE:ICON_OBJECT_MOVIEHIDE),"");
            ItemC1=CompoObject->BackgroundBrush.SoundVolume!=0?new QTableWidgetItem(QIcon(ICON_SOUND_OK),""):new QTableWidgetItem("");
            ItemC2=new QTableWidgetItem(QFileInfo(CompoObject->BackgroundBrush.Image?CompoObject->BackgroundBrush.Image->FileName:CompoObject->BackgroundBrush.Video?CompoObject->BackgroundBrush.Video->FileName:"").fileName());
        // Text block
        } else {
            ItemC0=new QTableWidgetItem(QIcon(CompoObject->IsVisible?ICON_OBJECT_TEXT:ICON_OBJECT_TEXTHIDE),"");
            ItemC1=new QTableWidgetItem("");
            ItemC2=new QTableWidgetItem(CompoObject->Text);
        }
        if (!CompoObject->IsVisible) {
            QFont  font=QFont("Sans serif",9,QFont::Normal,QFont::StyleItalic);
            ItemC2->setFont(font);
        }
        ui->BlockTable->setItem(i,0,ItemC0);
        ui->BlockTable->setItem(i,1,ItemC1);
        ui->BlockTable->setItem(i,2,ItemC2);
    }
    ui->BlockTable->setColumnWidth(0,24);
    ui->BlockTable->setColumnWidth(1,24);
    ui->BlockTable->resizeRowsToContents();
    ui->BlockTable->setUpdatesEnabled(true);
    ui->BlockTable->setCurrentCell(SetCurrentIndex,0);      // Changing current index make a call to RefreshControls();
    if (ui->BlockTable->rowCount()==0) RefreshControls();   // except if no block in the list
}

//====================================================================================================================
// User select a block in the BlockTable widget

void DlgSlideProperties::s_BlockTable_SelectionChanged() {
    if (StopMAJSpinbox) return;
    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return;
    StopMAJSpinbox=true;
    // Select item
    for (int i=0;i<scene->items().count();i++) {
        QGraphicsItem   *Item=scene->items().at(i);
        QString         data =Item->data(0).toString();
        if ((data=="CustomGraphicsRectItem")&&(((cCustomGraphicsRectItem *)Item)->IndexKey==CompositionList->List[CurrentBlock].IndexKey)) scene->items().at(i)->setSelected(true);
            else scene->items().at(i)->setSelected(false);
    }
    StopMAJSpinbox=false;
    RefreshControls();
}

//====================================================================================================================
// User select a block in the scene widget

void DlgSlideProperties::s_Scene_SelectionChanged() {
    if (StopMAJSpinbox) return;
    cCustomGraphicsRectItem  *CurrentTextItem=GetSelectItem();
    if (CurrentTextItem==NULL) {
        ui->BlockTable->setCurrentCell(-1,-1);
        RefreshControls();
        return;
    }
    StopMAJSpinbox=true;
    ui->BlockTable->setUpdatesEnabled(false);
    for (int i=0;i<ui->BlockTable->rowCount();i++) {
        if (CompositionList->List[i].IndexKey==CurrentTextItem->IndexKey) ui->BlockTable->setCurrentCell(i,0);
    }
    ui->BlockTable->setUpdatesEnabled(true);
    StopMAJSpinbox=false;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewTextBlock() {
    int CurrentShot=ui->ShotTable->currentColumn();

    // Create and append a composition block to the object list
    DiaporamaObject->ObjectComposition.List.append(cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey));
    cCompositionObject *CompositionObject=&DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
    CompositionObject->BackgroundBrush.BrushFileCorrect.ImageGeometry=GEOMETRY_CUSTOM;
    CompositionObject->BackgroundBrush.BrushFileCorrect.AspectRatio=1;
    CompositionObject->Text=QApplication::translate("DlgSlideProperties","Text","Default text value");

    // Now create and append a shot composition block to all shot
    for (int i=0;i<DiaporamaObject->List.count();i++) {
        DiaporamaObject->List[i].ShotComposition.List.append(cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey));
        DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].CopyFromCompositionObject(CompositionObject);
        // Ensure new object is not visible in previous shot
        if (i<CurrentShot) DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].IsVisible=false;
    }

    // Inc NextIndexKey
    DiaporamaObject->NextIndexKey++;

    // 10 by 10 step for ZValue
    NextZValue+=10;

    RefreshBlockTable(CompositionList->List.count()-1);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_AddNewFileBlock() {
    QString NewFile=QFileDialog::getOpenFileName(this,
                                                 QApplication::translate("DlgSlideProperties","Select a file"),
                                                 GlobalMainWindow->ApplicationConfig->RememberLastDirectories?GlobalMainWindow->ApplicationConfig->LastMediaPath:"",
                                                 GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(cApplicationConfig::ALLFILE));
    QApplication::processEvents();
    if (NewFile=="") return;
    if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

    // Create and append a composition block to the object list
    DiaporamaObject->ObjectComposition.List.append(cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey));
    cCompositionObject  *CompositionObject=&DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
    cBrushDefinition    *CurrentBrush=&CompositionObject->BackgroundBrush;
    int                 CurrentShot=ui->ShotTable->currentColumn();

    CompositionObject->Text     ="";
    CompositionObject->PenSize  =0;
    CurrentBrush->BrushType     =BRUSHTYPE_IMAGEDISK;

    QString BrushFileName =QFileInfo(NewFile).absoluteFilePath();

    bool    IsValide =false;
    QString Extension=QFileInfo(BrushFileName).suffix().toLower();

    // Search if file is an image
    for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowImageExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowImageExtension[i]==Extension) {
        // Create an image wrapper
        CurrentBrush->Image=new cimagefilewrapper();
        IsValide=CurrentBrush->Image->GetInformationFromFile(BrushFileName);
        if (!IsValide) {
            delete CurrentBrush->Image;
            CurrentBrush->Image=NULL;
        }
        break;
    }
    // If it's not an image : search if file is a video
    if (CurrentBrush->Image==NULL) for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowVideoExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowVideoExtension[i]==Extension) {
        // Create a video wrapper
        CurrentBrush->Video=new cvideofilewrapper();
        IsValide=CurrentBrush->Video->GetInformationFromFile(BrushFileName,false);
        if (!IsValide) {
            delete CurrentBrush->Video;
            CurrentBrush->Video=NULL;
        } else {
            CurrentBrush->Video->EndPos=CurrentBrush->Video->Duration;
            //DiaporamaObject->List[0].StaticDuration=CurrentBrush->Video->StartPos.msecsTo(CurrentBrush->Video->EndPos);
        }
        break;
    }
    if (IsValide) {

        QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,true,&CurrentBrush->Image->BrushFileTransform):
                       CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),true,NULL,1,false,&CurrentBrush->Video->BrushFileTransform):
                       NULL);
        if (Image) {
            // Calc hypothenuse of the image rectangle
            double  Hyp     =sqrt(Image->width()*Image->width()+Image->height()*Image->height());

            // setup BrushFileCorrect to full image
            CurrentBrush->BrushFileCorrect.ImageGeometry=GEOMETRY_IMAGE;
            CurrentBrush->BrushFileCorrect.X            =((Hyp-double(Image->width()))/2)/Hyp;
            CurrentBrush->BrushFileCorrect.Y            =((Hyp-double(Image->height()))/2)/Hyp;
            CurrentBrush->BrushFileCorrect.ZoomFactor   =double(Image->width())/Hyp;
            CurrentBrush->BrushFileCorrect.AspectRatio  =double(Image->height())/double(Image->width());
            double NewW=CompositionObject->w*GlobalMainWindow->Diaporama->InternalWidth;
            double NewH=NewW*CurrentBrush->BrushFileCorrect.AspectRatio;
            NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
            NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
            if (NewH>1) {
                NewH=CompositionObject->h*GlobalMainWindow->Diaporama->InternalHeight;
                NewW=NewH/CurrentBrush->BrushFileCorrect.AspectRatio;
                NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
                NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
            }
            CompositionObject->w=NewW;
            CompositionObject->h=NewH;
            delete Image;
        }
    }

    // Now create and append a shot composition block to all shot
    for (int i=0;i<DiaporamaObject->List.count();i++) {
        DiaporamaObject->List[i].ShotComposition.List.append(cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey));
        DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].CopyFromCompositionObject(CompositionObject);
        // Ensure new object is not visible in previous shot
        if (i<CurrentShot) DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].IsVisible=false;
    }

    // If this object is a video will gain sound from this shots !
    if (CurrentBrush->Video!=NULL) for (int k=0;k<DiaporamaObject->List.count();k++) for (int l=0;l<DiaporamaObject->List[k].ShotComposition.List.count();l++)
        if ((DiaporamaObject->List[k].ShotComposition.List[l].BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&
            (DiaporamaObject->List[k].ShotComposition.List[l].BackgroundBrush.Video)) {
            if (k<CurrentShot) {
                // Set this new block to SoundVolume=0
                if (CurrentBrush->Video==DiaporamaObject->List[k].ShotComposition.List[l].BackgroundBrush.Video)
                    DiaporamaObject->List[k].ShotComposition.List[l].BackgroundBrush.SoundVolume=0;
            } else {
                // Set all other block to SoundVolume=0 and this block to SoundVolume=1
                if (CurrentBrush->Video!=DiaporamaObject->List[k].ShotComposition.List[l].BackgroundBrush.Video)
                    DiaporamaObject->List[k].ShotComposition.List[l].BackgroundBrush.SoundVolume=0;
                else DiaporamaObject->List[k].ShotComposition.List[l].BackgroundBrush.SoundVolume=1;
            }
    }

    // Inc NextIndexKey
    DiaporamaObject->NextIndexKey++;

    // 10 by 10 step for ZValue
    NextZValue+=10;

    RefreshBlockTable(CompositionList->List.count()-1);
}

//====================================================================================================================

void DlgSlideProperties::s_BlockTable_RemoveBlock() {
    int                 CurrentRow=ui->BlockTable->currentRow();
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (QMessageBox::question(this,QApplication::translate("DlgSlideProperties","Remove bloc"),QApplication::translate("DlgSlideProperties","Are you sure to want to delete this bloc?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No) return;
    int i=0;
    while ((i<CompositionList->List.count())&&(CompositionList->List[i].IndexKey!=CurrentTextItem->IndexKey)) i++;

    if ((i<CompositionList->List.count())) {
        int KeyToDelete=CompositionList->List[i].IndexKey;
        StopMAJSpinbox=true;
        for (int j=0;j<DiaporamaObject->List.count();j++) {
            int k=0;
            while (k<DiaporamaObject->List[j].ShotComposition.List.count()) {
                if (DiaporamaObject->List[j].ShotComposition.List[k].IndexKey==KeyToDelete) DiaporamaObject->List[j].ShotComposition.List.removeAt(k);
                    else k++;
            }
        }
        int k=0;
        while (k<DiaporamaObject->ObjectComposition.List.count()) {
            if (DiaporamaObject->ObjectComposition.List[k].IndexKey==KeyToDelete) DiaporamaObject->ObjectComposition.List.removeAt(k);
                else k++;
        }

        StopMAJSpinbox=false;
    }
    RefreshBlockTable(CurrentRow>0?CurrentRow-1:0);
}

//====================================================================================================================

void DlgSlideProperties::CopyBlockProperties(cCompositionObject *SourceBlock,cCompositionObject *DestBlock) {
    if (SourceBlock==DestBlock) return;

    // Attribut of the text part
    DestBlock->Text                 =SourceBlock->Text;                     // Text of the object
    DestBlock->FontName             =SourceBlock->FontName;                 // font name
    DestBlock->FontSize             =SourceBlock->FontSize;                 // font size
    DestBlock->FontColor            =SourceBlock->FontColor;                // font color
    DestBlock->FontShadowColor      =SourceBlock->FontShadowColor;          // font shadow color
    DestBlock->IsBold               =SourceBlock->IsBold;                   // true if bold mode
    DestBlock->IsItalic             =SourceBlock->IsItalic;                 // true if Italic mode
    DestBlock->IsUnderline          =SourceBlock->IsUnderline;              // true if Underline mode
    DestBlock->HAlign               =SourceBlock->HAlign;                   // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
    DestBlock->VAlign               =SourceBlock->VAlign;                   // Vertical alignement : 0=up, 1=center, 2=bottom
    DestBlock->StyleText            =SourceBlock->StyleText;                // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right

    // Attribut of the shap part
    DestBlock->BackgroundForm       =SourceBlock->BackgroundForm;           // Type of the form : 0=None, 1=Rectangle, 2=RoundRect, 3=Buble, 4=Ellipse, 5=Triangle UP (Polygon)
    DestBlock->PenSize              =SourceBlock->PenSize;                  // Width of the border of the form
    DestBlock->PenStyle             =SourceBlock->PenStyle;                 // Style of the pen border of the form
    DestBlock->PenColor             =SourceBlock->PenColor;                 // Color of the border of the form
    DestBlock->FormShadow           =SourceBlock->FormShadow;               // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    DestBlock->FormShadowDistance   =SourceBlock->FormShadowDistance;       // Distance from form to shadow
    DestBlock->FormShadowColor      =SourceBlock->FormShadowColor;          // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    DestBlock->Opacity              =SourceBlock->Opacity;                  // Opacity of the form

    // Attribut of the BackgroundBrush of the shap part
    DestBlock->BackgroundBrush.BrushType           =SourceBlock->BackgroundBrush.BrushType;
    DestBlock->BackgroundBrush.PatternType         =SourceBlock->BackgroundBrush.PatternType;
    DestBlock->BackgroundBrush.GradientOrientation =SourceBlock->BackgroundBrush.GradientOrientation;
    DestBlock->BackgroundBrush.ColorD              =SourceBlock->BackgroundBrush.ColorD;
    DestBlock->BackgroundBrush.ColorF              =SourceBlock->BackgroundBrush.ColorF;
    DestBlock->BackgroundBrush.ColorIntermed       =SourceBlock->BackgroundBrush.ColorIntermed;
    DestBlock->BackgroundBrush.Intermediate        =SourceBlock->BackgroundBrush.Intermediate;
    DestBlock->BackgroundBrush.BrushImage          =SourceBlock->BackgroundBrush.BrushImage;
}

void DlgSlideProperties::ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock) {
    // Apply to GlobalComposition objects
    for (int j=0;j<DiaporamaObject->ObjectComposition.List.count();j++) if (GlobalBlock->IndexKey==DiaporamaObject->ObjectComposition.List[j].IndexKey)
        CopyBlockProperties(GlobalBlock,&DiaporamaObject->ObjectComposition.List[j]);

    // Apply to Shots Composition objects
    for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i].ShotComposition.List.count();j++) if (GlobalBlock->IndexKey==DiaporamaObject->List[i].ShotComposition.List[j].IndexKey)
        CopyBlockProperties(GlobalBlock,&DiaporamaObject->List[i].ShotComposition.List[j]);
}

//====================================================================================================================

void DlgSlideProperties::ChangeVisibleState() {
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();
    if (!CurrentTextItem)   return;
    CurrentTextItem->IsVisible=!CurrentTextItem->IsVisible;
    if (CurrentTextItem->BackgroundBrush.Video!=NULL) {
        if (!CurrentTextItem->IsVisible) {
            CurrentTextItem->BackgroundBrush.SoundVolume=0;
        } else {
            int CurrentShot=ui->ShotTable->currentColumn();
            bool SomeOneHaveSound=false;
            // Parse table to know if a block have sound for this shot
            for (int i=0;i<DiaporamaObject->List[CurrentShot].ShotComposition.List.count();i++)
                if ((DiaporamaObject->List[CurrentShot].ShotComposition.List[i].BackgroundBrush.Video!=NULL)&&
                    (DiaporamaObject->List[CurrentShot].ShotComposition.List[i].BackgroundBrush.SoundVolume!=0)) SomeOneHaveSound=true;
            // If no block have sound => get sound to this video
            if (!SomeOneHaveSound) CurrentTextItem->BackgroundBrush.SoundVolume=1;
        }
    }
    RefreshBlockTable(ui->BlockTable->currentRow());
}

//====================================================================================================================

void DlgSlideProperties::ShotLeft() {
    int CurrentShot=ui->ShotTable->currentColumn();
    DiaporamaObject->List.swap(CurrentShot,CurrentShot-1);
    RefreshShotTable(CurrentShot-1);
}

//====================================================================================================================

void DlgSlideProperties::ShotRight() {
    int CurrentShot=ui->ShotTable->currentColumn();
    DiaporamaObject->List.swap(CurrentShot+1,CurrentShot);
    RefreshShotTable(CurrentShot+1);
}

//====================================================================================================================

void DlgSlideProperties::BlockUp() {
    int CurrentBlock=ui->BlockTable->currentRow();
    CompositionList->List.swap(CurrentBlock,CurrentBlock-1);
    RefreshBlockTable(CurrentBlock-1);
}

//====================================================================================================================

void DlgSlideProperties::BlockDown() {
    int CurrentBlock=ui->BlockTable->currentRow();
    CompositionList->List.swap(CurrentBlock+1,CurrentBlock);
    RefreshBlockTable(CurrentBlock+1);
}

//====================================================================================================================

void DlgSlideProperties::s_CopyBlockBT() {
    cCompositionObject      *GlobalBlock=GetSelectedGlobalCompositionObject();
    cCompositionObject      *ShotBlock  =GetSelectedCompositionObject();
    if ((!GlobalBlock)||(!ShotBlock))   return;

    if (GlobalMainWindow->Clipboard_Block) delete GlobalMainWindow->Clipboard_Block;
    GlobalMainWindow->Clipboard_Block=new QDomDocument(APPLICATION_NAME);
    QDomElement root=GlobalMainWindow->Clipboard_Block->createElement("CLIPBOARD");
    GlobalMainWindow->Clipboard_Block->appendChild(root);
    GlobalBlock->SaveToXML(root,"CLIPBOARD-BLOCK-GLOBAL",QFileInfo(DiaporamaObject->Parent->ProjectFileName).absolutePath(),true);  // Save global object
    ShotBlock->SaveToXML(root,"CLIPBOARD-BLOCK-SHOT",QFileInfo(DiaporamaObject->Parent->ProjectFileName).absolutePath(),true);      // Save shot object
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_CutBlockBT() {
    cCompositionObject      *GlobalBlock=GetSelectedGlobalCompositionObject();
    cCompositionObject      *ShotBlock  =GetSelectedCompositionObject();
    if ((!GlobalBlock)||(!ShotBlock))   return;

    if (GlobalMainWindow->Clipboard_Block) delete GlobalMainWindow->Clipboard_Block;
    GlobalMainWindow->Clipboard_Block=new QDomDocument(APPLICATION_NAME);
    QDomElement root=GlobalMainWindow->Clipboard_Block->createElement("CLIPBOARD");
    GlobalMainWindow->Clipboard_Block->appendChild(root);
    GlobalBlock->SaveToXML(root,"CLIPBOARD-BLOCK-GLOBAL",QFileInfo(DiaporamaObject->Parent->ProjectFileName).absolutePath(),true);  // Save global object
    ShotBlock->SaveToXML(root,"CLIPBOARD-BLOCK-SHOT",QFileInfo(DiaporamaObject->Parent->ProjectFileName).absolutePath(),true);      // Save shot object
    s_BlockTable_RemoveBlock();
}

//====================================================================================================================

void DlgSlideProperties::s_PasteBlockBT() {
    if (!GlobalMainWindow->Clipboard_Block) return;

    QDomElement root=GlobalMainWindow->Clipboard_Block->documentElement();
    if (root.tagName()=="CLIPBOARD") {
        int CurrentShot=ui->ShotTable->currentColumn();

        // Create and append a composition block to the object list
        DiaporamaObject->ObjectComposition.List.append(cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey));
        cCompositionObject *GlobalBlock=&DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
        GlobalBlock->LoadFromXML(root,"CLIPBOARD-BLOCK-GLOBAL","",NULL);
        GlobalBlock->IndexKey=DiaporamaObject->NextIndexKey;

        cCompositionObject ShotBlock(COMPOSITIONTYPE_SHOT,DiaporamaObject->NextIndexKey);
        ShotBlock.LoadFromXML(root,"CLIPBOARD-BLOCK-SHOT","",NULL);
        ShotBlock.IndexKey=DiaporamaObject->NextIndexKey;
        ShotBlock.BackgroundBrush.Image=GlobalBlock->BackgroundBrush.Image;
        ShotBlock.BackgroundBrush.Video=GlobalBlock->BackgroundBrush.Video;
        ShotBlock.Text=GlobalBlock->Text;
        if (ShotBlock.Text!="") {
            ShotBlock.FontName        =GlobalBlock->FontName;
            ShotBlock.FontSize        =GlobalBlock->FontSize;
            ShotBlock.FontColor       =GlobalBlock->FontColor;
            ShotBlock.FontShadowColor =GlobalBlock->FontShadowColor;
            ShotBlock.IsBold          =GlobalBlock->IsBold;
            ShotBlock.IsItalic        =GlobalBlock->IsItalic;
            ShotBlock.IsUnderline     =GlobalBlock->IsUnderline;
            ShotBlock.HAlign          =GlobalBlock->HAlign;
            ShotBlock.VAlign          =GlobalBlock->VAlign;
            ShotBlock.StyleText       =GlobalBlock->StyleText;
        }

        // Now create and append a shot composition block to all shot
        for (int i=0;i<DiaporamaObject->List.count();i++) {
            DiaporamaObject->List[i].ShotComposition.List.append(cCompositionObject(COMPOSITIONTYPE_SHOT,DiaporamaObject->NextIndexKey));
            DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].CopyFromCompositionObject(&ShotBlock);
            // Ensure new object is not visible in previous shot
            if (i<CurrentShot) {
                DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].IsVisible=false;
                // Ensure unvisible video have no sound !
                if (DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].BackgroundBrush.Video!=NULL)
                    DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].BackgroundBrush.SoundVolume=0;
            }
        }

        // Inc NextIndexKey
        DiaporamaObject->NextIndexKey++;

        // 10 by 10 step for ZValue
        NextZValue+=10;

        RefreshBlockTable(CompositionList->List.count()-1);
    }
}
