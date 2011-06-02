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
#include "mainwindow.h"
#include "DlgImageCorrection.h"
#include "DlgImageTransformation.h"
#include "DlgVideoEdit.h"

// Arrow buttons flag definition
bool    ArrowBT_BlockList           =true;
bool    ArrowBT_GlobalBlock         =true;
bool    ArrowBT_ShapeProperties     =true;
bool    ArrowBT_TexProperties       =true;
bool    ArrowBT_DuringShotProperties=true;
bool    ArrowBT_VideoProperties     =true;
bool    ArrowBT_ImageProperties     =true;

DlgSlideProperties::DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent):QDialog(parent),ui(new Ui::DlgSlideProperties) {
    ui->setupUi(this);
    this->DiaporamaObject=DiaporamaObject;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT","");  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    IsFirstInitDone = false;                 // true when first show window was done
    scene           = NULL;
    NextZValue      = 500;
    BackgroundImage = NULL;
    CompositionList = NULL;
    StopMAJSpinbox  = false;
    BLOCKCHSIZE     = false;

    // Init check box
    ui->textLeft->setCheckable(true);
    ui->textCenter->setCheckable(true);
    ui->textJustif->setCheckable(true);
    ui->textRight->setCheckable(true);
    ui->textUp->setCheckable(true);
    ui->textVCenter->setCheckable(true);
    ui->textBottom->setCheckable(true);

    // Init font size
    QList<int>  sizes=QFontDatabase::standardSizes();
    QStringList Ssizes;
    for (int i=0;i<sizes.count();i++) Ssizes.append(QString("%1").arg(sizes[i]));
    ui->fontSize->insertItems(0,Ssizes);
    ui->fontSize->setCurrentIndex(6);

    // Init editor
    ui->plainTextEdit->setWordWrapMode(QTextOption::NoWrap);

    // Init combo box FontEffect
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","No effect"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Outerline"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Shadow upper left"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Shadow upper right"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Shadow bottom left"));
    ui->fontEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Shadow bottom right"));
    MakeTextStyleIcon(ui->fontEffectCB);

    // Init combo box Background form
    for (int i=0;i<12;i++) ui->BackgroundFormCB->addItem("");
    MakeFormIcon(ui->BackgroundFormCB);

    // Init combo box Background shadow form
    ui->ShadowEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","None"));
    ui->ShadowEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Shadow upper left"));
    ui->ShadowEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Shadow upper right"));
    ui->ShadowEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Shadow bottom left"));
    ui->ShadowEffectCB->addItem(QCoreApplication::translate("DlgSlideProperties","Shadow bottom right"));
    ui->ShadowEffectED->setRange(1,30);

    // Init combo box external border style
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::SolidLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DotLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashDotLine);
    ui->PenStyleCB->addItem("");    ui->PenStyleCB->setItemData(ui->PenStyleCB->count()-1,(int)Qt::DashDotDotLine);
    MakeBorderStyleIcon(ui->PenStyleCB);

    // Init combo box Background opacity
    ui->OpacityCB->addItem("100%");
    ui->OpacityCB->addItem(" 75%");
    ui->OpacityCB->addItem(" 50%");
    ui->OpacityCB->addItem(" 25%");

    ui->RotateXED->setRange(-180,180);     ui->RotateXSLD->setRange(-180,180);
    ui->RotateYED->setRange(-180,180);     ui->RotateYSLD->setRange(-180,180);
    ui->RotateZED->setRange(-180,180);     ui->RotateZSLD->setRange(-180,180);

    // Init Spinbox
    ui->PosXEd->setDecimals(2);
    ui->PosYEd->setDecimals(2);
    ui->WidthEd->setDecimals(2);
    ui->HeightEd->setDecimals(2);

    // Init shape Borders
    ui->PenSizeEd->setMinimum(0);       ui->PenSizeEd->setMaximum(30);

    // Init combo box Background  type
    ui->BrushTypeCombo->addItem(QCoreApplication::translate("DlgSlideProperties","No brush"));              ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_NOBRUSH)));
    ui->BrushTypeCombo->addItem(QCoreApplication::translate("DlgSlideProperties","Solid brush"));           ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_SOLID)));
    ui->BrushTypeCombo->addItem(QCoreApplication::translate("DlgSlideProperties","Pattern brush"));         ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_PATTERN)));
    ui->BrushTypeCombo->addItem(QCoreApplication::translate("DlgSlideProperties","Gradient 2 colors"));     ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT2)));
    ui->BrushTypeCombo->addItem(QCoreApplication::translate("DlgSlideProperties","Gradient 3 colors"));     ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT3)));
    ui->BrushTypeCombo->addItem(QCoreApplication::translate("DlgSlideProperties","Image from library"));    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_IMAGELIBRARY)));

    // Init image geometry combo box
    ui->ImageGeometryCB->addItem(QCoreApplication::translate("DlgSlideProperties","Project geometry"));
    ui->ImageGeometryCB->addItem(QCoreApplication::translate("DlgSlideProperties","Image geometry"));
    ui->ImageGeometryCB->addItem(QCoreApplication::translate("DlgSlideProperties","Custom geometry"));
    ui->VideoGeometryCB->addItem(QCoreApplication::translate("DlgSlideProperties","Project geometry"));
    ui->VideoGeometryCB->addItem(QCoreApplication::translate("DlgSlideProperties","Image geometry"));
    ui->VideoGeometryCB->addItem(QCoreApplication::translate("DlgSlideProperties","Custom geometry"));

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
    connect(ui->IsVisibleCB,SIGNAL(clicked()),this,SLOT(ChangeVisibleState()));

    connect(ui->ShotLeftBt,SIGNAL(clicked()),this,SLOT(ShotLeft()));
    connect(ui->ShotRightBt,SIGNAL(clicked()),this,SLOT(ShotRight()));
    connect(ui->BlockUpBT,SIGNAL(clicked()),this,SLOT(BlockUp()));
    connect(ui->BlockDownBT,SIGNAL(clicked()),this,SLOT(BlockDown()));

    connect(ui->SlideNameED,SIGNAL(textEdited(QString)),this,SLOT(s_SlideNameChange(QString)));
    connect(ui->ShotDurationED,SIGNAL(valueChanged(double)),this,SLOT(s_ShotDurationChange(double)));

    connect(ui->ArrowBT_BlockList,SIGNAL(clicked()),this,SLOT(SwitchArrowBT_BlockList()));
    connect(ui->ArrowBT_GlobalBlock,SIGNAL(clicked()),this,SLOT(SwitchArrowBT_GlobalBlock()));
    connect(ui->ArrowBT_ShapeProperties,SIGNAL(clicked()),this,SLOT(SwitchArrowBT_ShapeProperties()));
    connect(ui->ArrowBT_TexProperties,SIGNAL(clicked()),this,SLOT(SwitchArrowBT_TexProperties()));
    connect(ui->ArrowBT_DuringShotProperties,SIGNAL(clicked()),this,SLOT(SwitchArrowBT_DuringShotProperties()));
    connect(ui->ArrowBT_VideoProperties,SIGNAL(clicked()),this,SLOT(SwitchArrowBT_VideoProperties()));
    connect(ui->ArrowBT_ImageProperties,SIGNAL(clicked()),this,SLOT(SwitchArrowBT_ImageProperties()));

    connect(ui->ImageEditCorrectBT,SIGNAL(clicked()),this,SLOT(ImageEditCorrect()));
    connect(ui->VideoEditCorrectBT,SIGNAL(clicked()),this,SLOT(ImageEditCorrect()));
    connect(ui->ImageEditTransformBT,SIGNAL(clicked()),this,SLOT(ImageEditTransform()));
    connect(ui->VideoEditBT,SIGNAL(clicked()),this,SLOT(VideoEdit()));

    connect(ui->fontStyleCB,SIGNAL(currentFontChanged(QFont)),this,SLOT(s_ChangeFont(QFont)));
    connect(ui->fontSize,SIGNAL(currentIndexChanged(QString)),this,SLOT(s_ChangeSizeFont(QString)));
    connect(ui->bold,SIGNAL(released()),this,SLOT(s_SetBold()));
    connect(ui->Italic,SIGNAL(released()),this,SLOT(s_SetItalic()));
    connect(ui->Souligne,SIGNAL(released()),this,SLOT(s_SetUnderline()));
    connect(ui->textLeft,SIGNAL(pressed()),this,SLOT(s_SetTextLeft()));
    connect(ui->textCenter,SIGNAL(pressed()),this,SLOT(s_SetTextCenter()));
    connect(ui->textRight,SIGNAL(pressed()),this,SLOT(s_SetTextRight()));
    connect(ui->textJustif,SIGNAL(pressed()),this,SLOT(s_SetTextJustif()));
    connect(ui->textUp,SIGNAL(pressed()),this,SLOT(s_SetTextUp()));
    connect(ui->textVCenter,SIGNAL(pressed()),this,SLOT(s_SetTextVCenter()));
    connect(ui->textBottom,SIGNAL(pressed()),this,SLOT(s_SetTextBottom()));
    connect(ui->fontEffectCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeStyleFont(int)));
    connect(ui->PosXEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgPosXValue(double)));
    connect(ui->PosYEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgPosYValue(double)));
    connect(ui->WidthEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgWidthValue(double)));
    connect(ui->HeightEd,SIGNAL(valueChanged(double)),this,SLOT(s_ChgHeightValue(double)));
    connect(ui->BackgroundFormCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBackgroundForm(int)));
    connect(ui->OpacityCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeOpacity(int)));
    connect(ui->plainTextEdit,SIGNAL(textChanged()),this,SLOT(s_plainTextEditChange()));
    connect(ui->FontColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexFontColorCombo(int)));
    connect(ui->StyleShadowColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexFontShadowColorCombo(int)));
    connect(ui->PenStyleCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangePenStyle(int)));
    connect(ui->ShadowEffectCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChgShadowFormValue(int)));
    connect(ui->ShadowEffectED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgShadowDistanceValue(int)));

    connect(ui->RotateXED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateXValue(int))); connect(ui->RotateXSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateXValue(int)));
    connect(ui->RotateYED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateYValue(int))); connect(ui->RotateYSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateYValue(int)));
    connect(ui->RotateZED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateZValue(int))); connect(ui->RotateZSLD,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRotateZValue(int)));
    connect(ui->PenColorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChPenColorCB(int)));
    connect(ui->PenSizeEd,SIGNAL(valueChanged(int)),this,SLOT(s_ChgPenSize(int)));

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

    // Image part
    connect(ui->ImageGeometryCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeImageGeometry(int)));
    connect(ui->VideoGeometryCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeImageGeometry(int)));

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
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/HelpSlideProperties.php")));
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

void DlgSlideProperties::s_SlideNameChange(QString NewText) {
    DiaporamaObject->SlideName=NewText;
}

//====================================================================================================================

void DlgSlideProperties::s_ShotDurationChange(double NewValue) {
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    DiaporamaObject->List[Current].StaticDuration=int(NewValue*1000);
    // Refresh total slide duration
    ui->SlideDurationLabel->setText(QTime(0,0,0,0).addMSecs(DiaporamaObject->GetDuration()).toString("hh:mm:ss.zzz"));
    // Refresh thumbnail
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->setUpdatesEnabled(true);
}

//====================================================================================================================
// Arrow buttons
//====================================================================================================================

void DlgSlideProperties::SwitchArrowBT_BlockList() {
    ArrowBT_BlockList=!ArrowBT_BlockList;
    RefreshControls();
}

void DlgSlideProperties::SwitchArrowBT_GlobalBlock() {
    ArrowBT_GlobalBlock=!ArrowBT_GlobalBlock;
    RefreshControls();
}

void DlgSlideProperties::SwitchArrowBT_ShapeProperties() {
    ArrowBT_ShapeProperties=!ArrowBT_ShapeProperties;
    RefreshControls();
}

void DlgSlideProperties::SwitchArrowBT_TexProperties() {
    ArrowBT_TexProperties=!ArrowBT_TexProperties;
    RefreshControls();
}

void DlgSlideProperties::SwitchArrowBT_DuringShotProperties() {
    ArrowBT_DuringShotProperties=!ArrowBT_DuringShotProperties;
    RefreshControls();
}

void DlgSlideProperties::SwitchArrowBT_VideoProperties() {
    ArrowBT_VideoProperties=!ArrowBT_VideoProperties;
    RefreshControls();
}

void DlgSlideProperties::SwitchArrowBT_ImageProperties() {
    ArrowBT_ImageProperties=!ArrowBT_ImageProperties;
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

void DlgSlideProperties::RefreshControls() {
    // Ensure box is init and Current contain index of currented selected sequence
    if ((!IsFirstInitDone)||(!CompositionList)||(StopMAJSpinbox)) return;
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    StopMAJSpinbox=true;    // Disable reintrence in this RefreshControls function
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Update arrow buttons
    ui->ArrowBT_BlockList->setArrowType(ArrowBT_BlockList?Qt::DownArrow:Qt::RightArrow);
    ui->ArrowBT_GlobalBlock->setArrowType(ArrowBT_GlobalBlock?Qt::DownArrow:Qt::RightArrow);
    ui->ArrowBT_ShapeProperties->setArrowType(ArrowBT_ShapeProperties?Qt::DownArrow:Qt::RightArrow);
    ui->ArrowBT_TexProperties->setArrowType(ArrowBT_TexProperties?Qt::DownArrow:Qt::RightArrow);
    ui->ArrowBT_DuringShotProperties->setArrowType(ArrowBT_DuringShotProperties?Qt::DownArrow:Qt::RightArrow);
    ui->ArrowBT_VideoProperties->setArrowType(ArrowBT_VideoProperties?Qt::DownArrow:Qt::RightArrow);
    ui->ArrowBT_ImageProperties->setArrowType(ArrowBT_ImageProperties?Qt::DownArrow:Qt::RightArrow);

    // Slide name & duration and shot duration
    ui->SlideNameED->setText(DiaporamaObject->SlideName);
    ui->SlideDurationLabel->setText(QTime(0,0,0,0).addMSecs(DiaporamaObject->GetDuration()).toString("hh:mm:ss.zzz"));
    ui->ShotDurationED->setValue(double(DiaporamaObject->List[Current].GetStaticDuration())/1000);

    //--------------------------------------------------------------------
    // Update controls
    //--------------------------------------------------------------------
    int CurrentShot=ui->ShotTable->currentColumn();
    ui->ShotLeftBt->setEnabled(CurrentShot>0);
    ui->ShotRightBt->setEnabled(CurrentShot<ui->ShotTable->columnCount()-1);

    int CurrentBlock=ui->BlockTable->currentRow();
    ui->BlockUpBT->setEnabled(CurrentBlock>0);
    ui->BlockDownBT->setEnabled(CurrentBlock<ui->BlockTable->rowCount()-1);

/*
    int  AddingDuration=0;
    if (Current==(DiaporamaObject->List.count()-1)) {   // If it's the last shot
        int TotalDuration  =DiaporamaObject->GetDuration();
        int Duration=0;
        for (int i=0;i<DiaporamaObject->List.count();i++) Duration=Duration+(i>0?DiaporamaObject->List[i].GetMobilDuration():0)+DiaporamaObject->List[i].GetStaticDuration();
        if (Duration<TotalDuration) AddingDuration=TotalDuration-Duration;
    }
    if (AddingDuration==0) {
        if (DiaporamaObject->List[Current].DefaultStaticDuration) {
            int Default=((DiaporamaObject->List.count()>1?DiaporamaObject->Parent->FixedDuration:DiaporamaObject->Parent->NoShotDuration)/1000);
            ui->StaticDefault->setText(QString(QCoreApplication::translate("DlgImageProperties","Default project value=%1 sec")).arg(Default));
        } else ui->StaticDefault->setText(QCoreApplication::translate("DlgImageProperties","sec"));
    } else {
        ui->StaticDefault->setText(QString(QCoreApplication::translate("DlgImageProperties","Lengthened to %1 sec to allow transitions")).arg((DiaporamaObject->List[Current].GetStaticDuration()+AddingDuration)/1000));
    }

    ui->MobilSetCustomBt->setChecked(DiaporamaObject->List[Current].DefaultMobilDuration);
    ui->MobilCustomEd->setValue(DiaporamaObject->List[Current].MobilDuration/1000);

    if (Current>0) {
        ui->MobilSetCustomBt->setVisible(true);
        if (DiaporamaObject->List[Current].DefaultMobilDuration) {
            ui->MobilCustomEd->setEnabled(false);
            ui->MobilCustomEd->setVisible(false);
            ui->MobilSpacer->setVisible(true);
            ui->MobilDefault->setText(QString(QCoreApplication::translate("DlgImageProperties","Default project value=%1 sec")).arg(DiaporamaObject->Parent->MobilDuration/1000));
        } else {
            ui->MobilCustomEd->setEnabled(true);
            ui->MobilCustomEd->setVisible(true);
            ui->MobilSpacer->setVisible(false);
            ui->MobilDefault->setText(QCoreApplication::translate("DlgImageProperties","sec"));
        }
    } else {
        ui->MobilDefault->setText("");
        ui->MobilSetCustomBt->setVisible(false);
        ui->MobilCustomEd->setVisible(false);
    }
*/

    //====================================================================================================================

    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush   =(CurrentTextItem!=NULL)?&CurrentTextItem->BackgroundBrush:NULL;
    bool                IsVisible       =(CurrentTextItem)&&(CurrentTextItem->IsVisible);

    ui->IsVisibleCB->setChecked(IsVisible);

    ui->ArrowBT_VideoProperties->setVisible(IsVisible);
    ui->ArrowBT_ImageProperties->setVisible(IsVisible);

    // Brush TAB part
    bool Allow_Brush  =(CurrentBrush!=NULL)&&(CurrentBrush->BrushType!=BRUSHTYPE_IMAGEDISK);
    bool Allow_Color1 =(Allow_Brush)&&((CurrentBrush->BrushType==BRUSHTYPE_SOLID)||(CurrentBrush->BrushType==BRUSHTYPE_PATTERN)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color2 =(Allow_Brush)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color3 =(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3);
    bool Allow_Pattern=(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_PATTERN);
    bool Allow_Library=(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGELIBRARY);
    bool Allow_File   =(IsVisible)&&(CurrentBrush!=NULL)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK);

    ui->BackgroundLine->setVisible(Allow_Brush);                                ui->BackgroundLine->setEnabled(IsVisible && Allow_Brush);
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
    ui->FileNameED->setVisible(Allow_File);                                     ui->FileNameED->setEnabled(IsVisible && Allow_File);
    ui->FileNameBT->setVisible(Allow_File);                                     ui->FileNameBT->setEnabled(IsVisible && Allow_File);
    ui->ImageGeometryLabel->setVisible(Allow_File);                             ui->ImageGeometryLabel->setEnabled(IsVisible && Allow_File);
    ui->VideoGeometryLabel->setVisible(Allow_File);                             ui->VideoGeometryLabel->setEnabled(IsVisible && Allow_File);
    ui->ImageGeometryCB->setVisible(Allow_File);                                ui->ImageGeometryCB->setEnabled(IsVisible && Allow_File);
    ui->VideoGeometryCB->setVisible(Allow_File);                                ui->VideoGeometryCB->setEnabled(IsVisible && Allow_File);
    ui->ImageEditCorrectBT->setVisible(Allow_File);                             ui->ImageEditCorrectBT->setEnabled(IsVisible && Allow_File);
    ui->VideoEditCorrectBT->setVisible(Allow_File);                             ui->VideoEditCorrectBT->setEnabled(IsVisible && Allow_File);
    ui->ImageEditTransformBT->setVisible(Allow_File);                           ui->ImageEditTransformBT->setEnabled(IsVisible && Allow_File);
    ui->ImageEditTransformLabel->setVisible(Allow_File);                        ui->ImageEditTransformLabel->setEnabled(IsVisible && Allow_File);
    ui->VideoEditLabel->setVisible(Allow_File && (CurrentBrush->Video!=NULL));  ui->VideoEditLabel->setEnabled(IsVisible && Allow_File && (CurrentBrush->Video!=NULL));
    ui->VideoEditBT->setVisible(Allow_File && (CurrentBrush->Video!=NULL));     ui->VideoEditBT->setEnabled(IsVisible && Allow_File && (CurrentBrush->Video!=NULL));

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
                ui->ColorLabel1->setText(QCoreApplication::translate("DlgSlideProperties","Color"));
                break;
            case BRUSHTYPE_GRADIENT3 :
            case BRUSHTYPE_GRADIENT2 :
                ui->IntermPosSlider->setValue(CurrentBrush->Intermediate*100);
                ui->IntermPosED->setValue(CurrentBrush->Intermediate*100);
                ui->ColorLabel1->setText(QCoreApplication::translate("DlgSlideProperties","Colors"));
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
                ui->FileNameED->setText(CurrentBrush->BrushFileName);
                ui->ImageGeometryCB->setCurrentIndex(CurrentBrush->BrushFileCorrect.ImageGeometry);
                ui->VideoGeometryCB->setCurrentIndex(CurrentBrush->BrushFileCorrect.ImageGeometry);
                break;
        }
    }

    if (CurrentTextItem!=NULL) {

        //***********************
        // Text TAB
        //***********************
        BLOCKCHSIZE=true;
        ui->fontSize->setEnabled(IsVisible);
        ui->fontSize->setCurrentIndex(ui->fontSize->findText(QString("%1").arg(CurrentTextItem->FontSize)));
        BLOCKCHSIZE=false;

        ui->RemoveBlock->setEnabled(IsVisible);

        ui->textLeft->setEnabled(IsVisible);        ui->textLeft->setChecked(CurrentTextItem->HAlign==0);       ui->textLeft->setDown(CurrentTextItem->HAlign==0);
        ui->textCenter->setEnabled(IsVisible);      ui->textCenter->setChecked(CurrentTextItem->HAlign==1);     ui->textCenter->setDown(CurrentTextItem->HAlign==1);
        ui->textJustif->setEnabled(IsVisible);      ui->textJustif->setChecked(CurrentTextItem->HAlign==3);     ui->textJustif->setDown(CurrentTextItem->HAlign==3);
        ui->textRight->setEnabled(IsVisible);       ui->textRight->setChecked(CurrentTextItem->HAlign==2);      ui->textRight->setDown(CurrentTextItem->HAlign==2);
        ui->textUp->setEnabled(IsVisible);          ui->textUp->setChecked(CurrentTextItem->VAlign==0);         ui->textUp->setDown(CurrentTextItem->VAlign==0);
        ui->textVCenter->setEnabled(IsVisible);     ui->textVCenter->setChecked(CurrentTextItem->VAlign==1);    ui->textVCenter->setDown(CurrentTextItem->VAlign==1);
        ui->textBottom->setEnabled(IsVisible);      ui->textBottom->setChecked(CurrentTextItem->VAlign==2);     ui->textBottom->setDown(CurrentTextItem->VAlign==2);
        ui->bold->setEnabled(IsVisible);            ui->bold->setChecked(CurrentTextItem->IsBold);              ui->bold->setDown(CurrentTextItem->IsBold);
        ui->Italic->setEnabled(IsVisible);          ui->Italic->setChecked(CurrentTextItem->IsItalic);          ui->Italic->setDown(CurrentTextItem->IsItalic);
        ui->Souligne->setEnabled(IsVisible);        ui->Souligne->setChecked(CurrentTextItem->IsUnderline);     ui->Souligne->setDown(CurrentTextItem->IsUnderline);

        ui->plainTextEdit->setEnabled(IsVisible);   if (ui->plainTextEdit->toPlainText()!=CurrentTextItem->Text) ui->plainTextEdit->setPlainText(CurrentTextItem->Text);
        ui->fontStyleCB->setEnabled(IsVisible);     ui->fontStyleCB->setCurrentIndex(ui->fontStyleCB->findText(QString(CurrentTextItem->FontName)));
        ui->fontEffectCB->setEnabled(IsVisible);    ui->fontEffectCB->setCurrentIndex(CurrentTextItem->StyleText);

        ui->FontColorCombo->setEnabled(IsVisible);  ui->FontColorCombo->SetCurrentColor(&CurrentTextItem->FontColor);

        ui->StyleShadowColorCombo->setEnabled(IsVisible && (CurrentTextItem->StyleText!=0));                    ui->StyleShadowColorCombo->SetCurrentColor(&CurrentTextItem->FontShadowColor);

        //***********************
        // Shape TAB
        //***********************
        ui->BackgroundFormCB->setEnabled(IsVisible);                            ui->BackgroundFormCB->setCurrentIndex(CurrentTextItem->BackgroundForm-1);
        ui->PenSizeEd->setEnabled(IsVisible);                                   ui->PenSizeEd->setValue(int(CurrentTextItem->PenSize));
        ui->PenColorCB->setEnabled(IsVisible&&(CurrentTextItem->PenSize!=0));   ui->PenColorCB->SetCurrentColor(&CurrentTextItem->PenColor);
        ui->PenStyleCB->setEnabled(IsVisible&&(CurrentTextItem->PenSize&=0));
        for (int i=0;i<ui->PenStyleCB->count();i++) if (ui->PenStyleCB->itemData(i).toInt()==CurrentTextItem->PenStyle) {
            ui->PenStyleCB->setCurrentIndex(i);
            break;
        }

        //***********************
        // Size & Position TAB
        //***********************
        ui->PosXEd->setEnabled(IsVisible);     ui->PosXEd->setRange(0,99-CurrentTextItem->w*100);          ui->PosXEd->setValue(CurrentTextItem->x*100);   ui->PosXEd->setSingleStep(int(100/20));
        ui->PosYEd->setEnabled(IsVisible);     ui->PosYEd->setRange(0,99-CurrentTextItem->h*100);          ui->PosYEd->setValue(CurrentTextItem->y*100);   ui->PosYEd->setSingleStep(int(100/20));
        ui->OpacityCB->setEnabled(IsVisible);  ui->OpacityCB->setCurrentIndex(CurrentTextItem->Opacity);
        ui->WidthEd->setEnabled(IsVisible);    ui->WidthEd->setRange(3,99-CurrentTextItem->x*100);         ui->WidthEd->setValue(CurrentTextItem->w*100);  ui->WidthEd->setSingleStep(int(100/20));
        ui->HeightEd->setEnabled(IsVisible);   ui->HeightEd->setRange(3,99-CurrentTextItem->y*100);        ui->HeightEd->setValue(CurrentTextItem->h*100); ui->HeightEd->setSingleStep(int(100/20));
        ui->RotateXED->setEnabled(IsVisible);  ui->RotateXED->setValue(CurrentTextItem->RotateXAxis);      ui->RotateXSLD->setDisabled(false);             ui->RotateXSLD->setValue(CurrentTextItem->RotateXAxis);
        ui->RotateYED->setEnabled(IsVisible);  ui->RotateYED->setValue(CurrentTextItem->RotateYAxis);      ui->RotateYSLD->setDisabled(false);             ui->RotateYSLD->setValue(CurrentTextItem->RotateYAxis);
        ui->RotateZED->setEnabled(IsVisible);  ui->RotateZED->setValue(CurrentTextItem->RotateZAxis);      ui->RotateZSLD->setDisabled(false);             ui->RotateZSLD->setValue(CurrentTextItem->RotateZAxis);
        ui->ShadowEffectCB->setEnabled(IsVisible);                                                         ui->ShadowEffectCB->setCurrentIndex(CurrentTextItem->FormShadow);
        ui->ShadowEffectED->setEnabled(IsVisible);                                                         ui->ShadowEffectED->setValue(int(CurrentTextItem->FormShadowDistance));

    } else {

        ui->RemoveBlock->setDisabled(true);

        //***********************
        // Text TAB
        //***********************
        ui->fontSize->setDisabled(true);
        ui->textLeft->setDisabled(true);
        ui->textCenter->setDisabled(true);
        ui->textJustif->setDisabled(true);
        ui->textRight->setDisabled(true);
        ui->textUp->setDisabled(true);
        ui->textVCenter->setDisabled(true);
        ui->textBottom->setDisabled(true);
        ui->bold->setDisabled(true);
        ui->Italic->setDisabled(true);
        ui->Souligne->setDisabled(true);
        ui->plainTextEdit->setPlainText("");
        ui->fontStyleCB->setDisabled(true);
        ui->fontEffectCB->setDisabled(true);
        ui->FontColorCombo->setDisabled(true);
        ui->StyleShadowColorCombo->setDisabled(true);

        //***********************
        // Shape TAB
        //***********************
        ui->BackgroundFormCB->setDisabled(true);
        ui->PenSizeEd->setDisabled(true);
        ui->PenColorCB->setDisabled(true);
        ui->PenStyleCB->setDisabled(true);

        //***********************
        // Size & Position TAB
        //***********************
        ui->PosXEd->setDisabled(true);      ui->PosXEd->setValue(0);
        ui->PosYEd->setDisabled(true);      ui->PosYEd->setValue(0);
        ui->WidthEd->setDisabled(true);     ui->WidthEd->setValue(0);
        ui->HeightEd->setDisabled(true);    ui->HeightEd->setValue(0);
        ui->OpacityCB->setDisabled(true);
        ui->RotateXED->setDisabled(true);   ui->RotateXED->setValue(0);
        ui->RotateXSLD->setDisabled(true);  ui->RotateXSLD->setValue(0);
        ui->RotateYED->setDisabled(true);   ui->RotateYED->setValue(0);
        ui->RotateYSLD->setDisabled(true);  ui->RotateYSLD->setValue(0);
        ui->RotateZED->setDisabled(true);   ui->RotateZED->setValue(0);
        ui->RotateZSLD->setDisabled(true);  ui->RotateZSLD->setValue(0);
        ui->ShadowEffectCB->setDisabled(true);
        ui->ShadowEffectED->setDisabled(true);
    }

    // Refresh background image of the scene

    // Draw image of the scene under the background
    QPixmap NewImage=QPixmap::fromImage(*BackgroundImage);
    QPainter P;
    P.begin(&NewImage);

    ADJUST_RATIO=double(ymax)/double(1080);    // fixe Adjustment ratio for this slide

    for (int i=0;i<CompositionList->List.count();i++) if (CompositionList->List[i].IsVisible) {
        // Draw composition
        CompositionList->List[i].DrawCompositionObject(&P,0,0,xmax,ymax,true,0,NULL,1,NULL);
        // Draw border
        if (GetSelectedCompositionObject()==&CompositionList->List[i]) {
            // draw rect out of the rectangle
            P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
            QPen pen=QPen(Qt::red);
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            P.setPen(pen);
            P.setBrush(Qt::NoBrush);
            P.drawRect(QRectF(CompositionList->List[i].x*xmax,CompositionList->List[i].y*ymax,CompositionList->List[i].w*xmax,CompositionList->List[i].h*ymax));
            P.setCompositionMode(QPainter::CompositionMode_SourceOver);
        } else {
            // draw rect out of the rectangle
            P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
            QPen pen=QPen(Qt::lightGray);
            pen.setWidth(2);
            pen.setStyle(Qt::DotLine);
            P.setPen(pen);
            P.setBrush(Qt::NoBrush);
            P.drawRect(QRectF(CompositionList->List[i].x*xmax,CompositionList->List[i].y*ymax,CompositionList->List[i].w*xmax,CompositionList->List[i].h*ymax));
            P.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }
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

    // BlockTable
    ui->BlockTable->setVisible(ArrowBT_BlockList);
    ui->GobalBlockWidget->setVisible(ArrowBT_GlobalBlock);
    ui->ShapeWidgetLayout->setVisible(ArrowBT_ShapeProperties);         ui->ShapeWidgetLayout->setEnabled(IsVisible);
    ui->TextPropertiesWidget->setVisible(ArrowBT_TexProperties);        ui->TextPropertiesWidget->setEnabled(IsVisible);
    ui->ShotWidgetLayout->setVisible(ArrowBT_DuringShotProperties);

    ui->ArrowBT_VideoProperties->setVisible((CurrentBrush)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentBrush->Video));
    ui->VideoPropertiesWidget->setVisible((ArrowBT_VideoProperties)&&(CurrentBrush)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentBrush->Video));

    ui->ArrowBT_ImageProperties->setVisible((CurrentBrush)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentBrush->Image));
    ui->ImagePropertiesWidget->setVisible((ArrowBT_ImageProperties)&&(CurrentBrush)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(CurrentBrush->Image));

    // Refresh thumbnail
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->setUpdatesEnabled(true);

    QApplication::restoreOverrideCursor();
    StopMAJSpinbox=false;
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

void DlgSlideProperties::s_SetBold() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if (CurrentTextItem->IsBold==true) CurrentTextItem->IsBold=false; else CurrentTextItem->IsBold=true;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetItalic() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if (CurrentTextItem->IsItalic==true) CurrentTextItem->IsItalic=false; else CurrentTextItem->IsItalic=true;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetUnderline() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if (CurrentTextItem->IsUnderline==true) CurrentTextItem->IsUnderline=false; else CurrentTextItem->IsUnderline=true;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextLeft() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->HAlign=0;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextCenter() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->HAlign=1;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextRight() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->HAlign=2;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextJustif() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->HAlign=3;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextUp() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->VAlign=0;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextVCenter() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->VAlign=1;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextBottom() {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->VAlign=2;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeFont(QFont font) {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if (font.family()!="") CurrentTextItem->FontName=font.family();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeSizeFont(QString size) {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if ((size!="")&&(BLOCKCHSIZE==false)) CurrentTextItem->FontSize=size.toInt();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeStyleFont(int Style) {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->StyleText=Style;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_plainTextEditChange() {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->Text=ui->plainTextEdit->toPlainText();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
    ui->BlockTable->item(ui->BlockTable->currentRow(),2)->setText(CurrentTextItem->Text);
}

//========= Font color
void DlgSlideProperties::s_ChIndexFontColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->FontColor=ui->FontColorCombo->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Text shadow color
void DlgSlideProperties::s_ChIndexFontShadowColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->FontShadowColor=ui->StyleShadowColorCombo->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgPosXValue(double Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->x=Value/100;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgPosYValue(double Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->y=Value/100;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgWidthValue(double Value) {
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cBrushDefinition        *CurrentBrush   =GetCurrentBrush();                 if (!CurrentBrush)      return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();                          if (RectItem==NULL)     return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->w=Value/100;
    if (RectItem->KeepAspectRatio) CurrentTextItem->h=((CurrentTextItem->w*xmax)*RectItem->AspectRatio)/ymax;
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgHeightValue(double Value) {
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cBrushDefinition        *CurrentBrush   =GetCurrentBrush();                 if (!CurrentBrush)      return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();                          if (RectItem==NULL)     return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->h=Value/100;
    if (RectItem->KeepAspectRatio) CurrentTextItem->w=((CurrentTextItem->h*ymax)/RectItem->AspectRatio)/xmax;
    RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
    RectItem->setRect(Rect);
    RectItem->RecalcEmbededResizeRectItem();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeBackgroundForm(int Style) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->BackgroundForm=Style+1;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeOpacity(int Style) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->Opacity=Style;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgPenSize(int Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->PenSize=Value;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangePenStyle(int index) {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->PenStyle=ui->PenStyleCB->itemData(index).toInt();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChPenColorCB(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->PenColor=ui->PenColorCB->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgShadowFormValue(int value) {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->FormShadow=value;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgShadowDistanceValue(int value) {
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->FormShadowDistance =value;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
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
        ADJUST_RATIO=1;
        Object.DrawCompositionObject(&Painter,0,0,32,32,true,0,NULL,1,NULL);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

void DlgSlideProperties::MakeTextStyleIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0);
        Object.Text="T";
        Object.x=0.15;
        Object.y=0.15;
        Object.w=0.7;
        Object.h=0.7;
        Object.HAlign           =1;                 // Center
        Object.VAlign           =1;                 // Center
        Object.FontColor        ="#00ff00";
        Object.FontShadowColor  ="#ff0000";
        Object.StyleText        =i;
        Object.FontSize         =200;
        Object.IsBold           =true;
        Object.PenSize          =0;
        Object.BackgroundForm   =1;
        Object.Opacity=0;
        QPixmap  Image(32,32);
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,32,32),"#ffffff");
        Object.DrawCompositionObject(&Painter,0,0,32,32,true,0,NULL,1,NULL);
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
// Handler for rotation controls
//====================================================================================================================

//========= Z Value
void DlgSlideProperties::s_ChgRotateZValue(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->RotateZAxis=Value;
    RefreshControls();
}

//========= X Value
void DlgSlideProperties::s_ChgRotateXValue(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->RotateXAxis=Value;
    RefreshControls();
}

//========= Y Value
void DlgSlideProperties::s_ChgRotateYValue(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    CurrentTextItem->RotateYAxis=Value;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeBrushTypeCombo(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->BrushType=ui->BrushTypeCombo->itemData(Value).toInt();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_IntermPosSliderMoved(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->Intermediate=double(Value)/100;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_IntermPosED(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->Intermediate=double(Value)/100;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//====================================================================================================================
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Pattern shape combo
void DlgSlideProperties::s_ChIndexPatternBrushCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->PatternType=ui->PatternBrushCombo->GetCurrentBrush()->PatternType;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Gradient shape orientation
void DlgSlideProperties::s_ChIndexGradientOrientationCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->GradientOrientation=ui->OrientationCombo->GetCurrentBrush()->GradientOrientation;
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Shape/Gradient shape first color
void DlgSlideProperties::s_ChIndexGradientFirstColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->ColorD=ui->FirstColorCombo->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Gradient shape last color
void DlgSlideProperties::s_ChIndexGradientFinalColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->ColorF=ui->FinalColorCombo->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Gradient shape intermediate color
void DlgSlideProperties::s_ChIndexGradientIntermColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)||(!CurrentBrush)) return;
    CurrentBrush->ColorIntermed=ui->IntermColorCombo->GetCurrentColor();
    ApplyGlobalPropertiesToAllShots(CurrentTextItem);
    RefreshControls();
}

//========= Background image
void DlgSlideProperties::s_ChIndexBackgroundCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedGlobalCompositionObject();
    cBrushDefinition    *CurrentBrush=GetCurrentGlobalBrush();
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

    CurrentBrush->BrushFileCorrect.ImageGeometry=value;

    if ((CurrentBrush->BrushFileCorrect.ImageGeometry==GEOMETRY_PROJECT)||(CurrentBrush->BrushFileCorrect.ImageGeometry==GEOMETRY_IMAGE)) {

        if (CurrentBrush->BrushFileCorrect.ImageGeometry==GEOMETRY_IMAGE) CurrentBrush->BrushFileCorrect.AspectRatio=double(Image->height())/double(Image->width());
        else if (CurrentBrush->BrushFileCorrect.ImageGeometry==GEOMETRY_PROJECT) CurrentBrush->BrushFileCorrect.AspectRatio=double(GlobalMainWindow->Diaporama->InternalHeight)/double(GlobalMainWindow->Diaporama->InternalWidth);

        double NewW=CurrentTextItem->w*GlobalMainWindow->Diaporama->InternalWidth;
        double NewH=NewW*CurrentBrush->BrushFileCorrect.AspectRatio;
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
        RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
        RectItem->KeepAspectRatio=true;
        RectItem->setPos(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentTextItem->x*xmax,CurrentTextItem->y*ymax,xmax*CurrentTextItem->w,ymax*CurrentTextItem->h));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    } else {
        RectItem->AspectRatio=1;
        RectItem->KeepAspectRatio=false;
    }
    RefreshControls();
}

//========= Button opending Dialog box
void DlgSlideProperties::ImageEditCorrect() {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();                    if (!CurrentBrush) return;
    DlgImageCorrection(CurrentTextItem->BackgroundForm,CurrentBrush,&CurrentBrush->BrushFileCorrect,this).exec();
    RefreshControls();
}

void DlgSlideProperties::ImageEditTransform() {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();                    if (!CurrentBrush) return;
    if (!CurrentBrush) return;
    if (DlgImageTransformation(CurrentBrush,this).exec()==0) {
        if ((CurrentBrush->Image)&&(CurrentBrush->Image->CacheImage)) {
            delete CurrentBrush->Image->CacheImage;
            CurrentBrush->Image->CacheImage=NULL;
        }
        if ((CurrentBrush->Video)&&(CurrentBrush->Video->CacheFirstImage)) {
            delete CurrentBrush->Video->CacheFirstImage;
            CurrentBrush->Video->CacheFirstImage=NULL;
        }
    }
    RefreshControls();
}

void DlgSlideProperties::VideoEdit() {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();    if ((!CurrentTextItem)||(!CurrentTextItem->IsVisible)) return;
    cBrushDefinition    *CurrentBrush=GetCurrentBrush();                    if (!CurrentBrush) return;
    DlgVideoEdit(CurrentBrush,this).exec();
    DiaporamaObject->List[0].StaticDuration=CurrentBrush->Video->StartPos.msecsTo(CurrentBrush->Video->EndPos);
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
    while (ui->ShotTable->columnCount()>0) ui->ShotTable->removeColumn(0);    // Clear all
    for (int i=0;i<DiaporamaObject->List.count();i++) {
        ui->ShotTable->insertColumn(i);
        wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->ShotTable,THUMBNAILTYPE_SHOT);
        Object->DiaporamaObject=DiaporamaObject;
        ui->ShotTable->setCellWidget(0,i,Object);
        ui->ShotTable->setColumnWidth(i,CellWidth);
    }
    ui->ShotTable->setUpdatesEnabled(true);
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
    DiaporamaObject->List.removeAt(Current);
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->removeColumn(Current);
    ui->ShotTable->setUpdatesEnabled(true);
    RefreshShotTable(ui->ShotTable->currentColumn()>0?ui->ShotTable->currentColumn()-1:0);
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

    // Fill the scene with block item by creating cCustomGraphicsRectItem associate to existing cCompositionObject
    NextZValue=500;
    for (int i=0;i<CompositionList->List.count();i++) if (CompositionList->List[i].IsVisible) {
        // Create and add to scene a cCustomGraphicsRectItem
        new cCustomGraphicsRectItem(scene,NextZValue,&CompositionList->List[i].x,&CompositionList->List[i].y,
                    NULL,&CompositionList->List[i].w,&CompositionList->List[i].h,xmax,ymax,
                    CompositionList->List[i].BackgroundBrush.BrushFileCorrect.ImageGeometry==GEOMETRY_CUSTOM?false:true,
                    CompositionList->List[i].BackgroundBrush.BrushFileCorrect.ImageGeometry==GEOMETRY_CUSTOM?1:CompositionList->List[i].BackgroundBrush.BrushFileCorrect.AspectRatio,
                    NULL,this,TYPE_DlgSlideProperties,CompositionList->List[i].IndexKey);

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
            ItemC2=new QTableWidgetItem(QFileInfo(CompoObject->BackgroundBrush.BrushFileName).fileName());
        // Video block
        } else if ((CompoObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&(CompoObject->BackgroundBrush.Video)) {
            ItemC0=new QTableWidgetItem(QIcon(CompoObject->IsVisible?ICON_OBJECT_MOVIE:ICON_OBJECT_MOVIEHIDE),"");
            ItemC1=new QTableWidgetItem(QIcon(ICON_OBJECT_SOUND),"");
            ItemC2=new QTableWidgetItem(QFileInfo(CompoObject->BackgroundBrush.BrushFileName).fileName());
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
    ui->BlockTable->setCurrentCell(SetCurrentIndex,0);
    ui->BlockTable->setUpdatesEnabled(true);
    RefreshControls();
}

//====================================================================================================================
// User select a block in the BlockTable widget

void DlgSlideProperties::s_BlockTable_SelectionChanged() {
    if (StopMAJSpinbox) return;
    int CurrentBlock=ui->BlockTable->currentRow();
    if ((CurrentBlock<0)||(CurrentBlock>=CompositionList->List.count())) return;
    // Select item
    for (int i=0;i<scene->items().count();i++) {
        QGraphicsItem   *Item=scene->items().at(i);
        QString         data =Item->data(0).toString();
        if ((data=="CustomGraphicsRectItem")&&(((cCustomGraphicsRectItem *)Item)->IndexKey==CompositionList->List[CurrentBlock].IndexKey)) scene->items().at(i)->setSelected(true);
            else scene->items().at(i)->setSelected(false);
    }
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
    // Create and append a composition block to the object list
    DiaporamaObject->ObjectComposition.List.append(cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey));
    cCompositionObject *CompositionObject=&DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
    CompositionObject->BackgroundBrush.BrushFileCorrect.ImageGeometry=GEOMETRY_CUSTOM;
    CompositionObject->BackgroundBrush.BrushFileCorrect.AspectRatio=1;
    CompositionObject->Text=QCoreApplication::translate("DlgSlideProperties","Text","Default text value");

    // Now create and append a shot composition block to all shot
    for (int i=0;i<DiaporamaObject->List.count();i++) {
        DiaporamaObject->List[i].ShotComposition.List.append(cCompositionObject(COMPOSITIONTYPE_SHOT,CompositionObject->IndexKey));
        DiaporamaObject->List[i].ShotComposition.List[DiaporamaObject->List[i].ShotComposition.List.count()-1].CopyFromCompositionObject(CompositionObject);
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
    QCoreApplication::processEvents();
    if (NewFile=="") return;
    if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

    // Create and append a composition block to the object list
    DiaporamaObject->ObjectComposition.List.append(cCompositionObject(COMPOSITIONTYPE_OBJECT,DiaporamaObject->NextIndexKey));
    cCompositionObject *CompositionObject=&DiaporamaObject->ObjectComposition.List[DiaporamaObject->ObjectComposition.List.count()-1];
    cBrushDefinition   *CurrentBrush=&CompositionObject->BackgroundBrush;

    CompositionObject->Text     ="";
    CompositionObject->PenSize  =0;
    CurrentBrush->BrushFileName =QFileInfo(NewFile).absoluteFilePath();
    CurrentBrush->BrushType     =BRUSHTYPE_IMAGEDISK;

    bool    IsValide =false;
    QString Extension=QFileInfo(CurrentBrush->BrushFileName).suffix().toLower();

    // Search if file is an image
    for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowImageExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowImageExtension[i]==Extension) {
        // Create an image wrapper
        CurrentBrush->Image=new cimagefilewrapper();
        IsValide=CurrentBrush->Image->GetInformationFromFile(CurrentBrush->BrushFileName);
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
        IsValide=CurrentBrush->Video->GetInformationFromFile(CurrentBrush->BrushFileName,false);
        if (!IsValide) {
            delete CurrentBrush->Video;
            CurrentBrush->Video=NULL;
        } else {
            CurrentBrush->Video->EndPos=CurrentBrush->Video->Duration;
            DiaporamaObject->List[0].StaticDuration=CurrentBrush->Video->StartPos.msecsTo(CurrentBrush->Video->EndPos);
        }
        break;
    }
    if (IsValide) {

        QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,true,&CurrentBrush->BrushFileTransform):
                       CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,true,NULL,1,false,&CurrentBrush->BrushFileTransform):
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

void DlgSlideProperties::ApplyGlobalPropertiesToAllShots(cCompositionObject *GlobalBlock) {
    for (int i=0;i<DiaporamaObject->List.count();i++) for (int j=0;j<DiaporamaObject->List[i].ShotComposition.List.count();j++) if (GlobalBlock->IndexKey==DiaporamaObject->List[i].ShotComposition.List[j].IndexKey) {
        // Attribut of the text part
        DiaporamaObject->List[i].ShotComposition.List[j].Text           =GlobalBlock->Text;                   // Text of the object
        DiaporamaObject->List[i].ShotComposition.List[j].FontName       =GlobalBlock->FontName;               // font name
        DiaporamaObject->List[i].ShotComposition.List[j].FontSize       =GlobalBlock->FontSize;               // font size
        DiaporamaObject->List[i].ShotComposition.List[j].FontColor      =GlobalBlock->FontColor;              // font color
        DiaporamaObject->List[i].ShotComposition.List[j].FontShadowColor=GlobalBlock->FontShadowColor;        // font shadow color
        DiaporamaObject->List[i].ShotComposition.List[j].IsBold         =GlobalBlock->IsBold;                 // true if bold mode
        DiaporamaObject->List[i].ShotComposition.List[j].IsItalic       =GlobalBlock->IsItalic;               // true if Italic mode
        DiaporamaObject->List[i].ShotComposition.List[j].IsUnderline    =GlobalBlock->IsUnderline;            // true if Underline mode
        DiaporamaObject->List[i].ShotComposition.List[j].HAlign         =GlobalBlock->HAlign;                 // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
        DiaporamaObject->List[i].ShotComposition.List[j].VAlign         =GlobalBlock->VAlign;                 // Vertical alignement : 0=up, 1=center, 2=bottom
        DiaporamaObject->List[i].ShotComposition.List[j].StyleText      =GlobalBlock->StyleText;              // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right

        // Attribut of the shap part
        DiaporamaObject->List[i].ShotComposition.List[j].BackgroundForm     =GlobalBlock->BackgroundForm;       // Type of the form : 0=None, 1=Rectangle, 2=RoundRect, 3=Buble, 4=Ellipse, 5=Triangle UP (Polygon)
        DiaporamaObject->List[i].ShotComposition.List[j].PenSize            =GlobalBlock->PenSize;              // Width of the border of the form
        DiaporamaObject->List[i].ShotComposition.List[j].PenStyle           =GlobalBlock->PenStyle;             // Style of the pen border of the form
        DiaporamaObject->List[i].ShotComposition.List[j].PenColor           =GlobalBlock->PenColor;             // Color of the border of the form
        DiaporamaObject->List[i].ShotComposition.List[j].FormShadow         =GlobalBlock->FormShadow;           // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
        DiaporamaObject->List[i].ShotComposition.List[j].FormShadowDistance =GlobalBlock->FormShadowDistance;   // Distance from form to shadow
        DiaporamaObject->List[i].ShotComposition.List[j].Opacity            =GlobalBlock->Opacity;              // Opacity of the form

        // Attribut of the BackgroundBrush of the shap part
        cBrushDefinition *BrushToCopy=&DiaporamaObject->List[i].ShotComposition.List[j].BackgroundBrush;
        BrushToCopy->BrushType           =GlobalBlock->BackgroundBrush.BrushType;
        BrushToCopy->PatternType         =GlobalBlock->BackgroundBrush.PatternType;
        BrushToCopy->GradientOrientation =GlobalBlock->BackgroundBrush.GradientOrientation;
        BrushToCopy->ColorD              =GlobalBlock->BackgroundBrush.ColorD;
        BrushToCopy->ColorF              =GlobalBlock->BackgroundBrush.ColorF;
        BrushToCopy->ColorIntermed       =GlobalBlock->BackgroundBrush.ColorIntermed;
        BrushToCopy->Intermediate        =GlobalBlock->BackgroundBrush.Intermediate;
        BrushToCopy->BrushImage          =GlobalBlock->BackgroundBrush.BrushImage;
        //BrushToCopy->BrushFileName       =GlobalBlock->BackgroundBrush.BrushFileName;

    }
}

//====================================================================================================================

void DlgSlideProperties::ChangeVisibleState() {
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();
    if (!CurrentTextItem)   return;
    CurrentTextItem->IsVisible=ui->IsVisibleCB->isChecked();
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
