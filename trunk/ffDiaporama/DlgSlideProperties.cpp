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

DlgSlideProperties::DlgSlideProperties(cDiaporamaObject *DiaporamaObject,QWidget *parent):QDialog(parent),ui(new Ui::DlgSlideProperties) {
    ui->setupUi(this);
    this->DiaporamaObject   = DiaporamaObject;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT","");  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    // Fill the ShotTable
    for (int i=0;i<DiaporamaObject->List.count();i++) {
        ui->ShotTable->insertColumn(i);
        wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->ShotTable,DiaporamaObject->Parent,THUMBNAILTYPE_SHOT);
        ui->ShotTable->setCellWidget(0,i,Object);
        ui->ShotTable->setColumnWidth(i,DiaporamaObject->Parent->GetWidthForHeight(ui->ShotTable->rowHeight(0)));
    }

    IsFirstInitDone = false;                 // true when first show window was done
    scene           = NULL;
    NextZValue      = 500;
    BackgroundImage = NULL;
    CompositionList = NULL;
    StopMAJSpinbox  = false;
    BLOCKCHSIZE     = false;

    // Force Bloc properties tab widget to first tab
    ui->BlocPropertiesTabWidget->setCurrentIndex(0);

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

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

    connect(ui->ImageEditCorrectBT,SIGNAL(clicked()),this,SLOT(ImageEditCorrect()));
    connect(ui->ImageEditTransformBT,SIGNAL(clicked()),this,SLOT(ImageEditTransform()));
    connect(ui->VideoEditBT,SIGNAL(clicked()),this,SLOT(VideoEdit()));

    connect(ui->ShotTable,SIGNAL(itemSelectionChanged()),this,SLOT(s_CurrentShotSelectionChanged()));

    connect(ui->AddTextBlock,SIGNAL(pressed()),this,SLOT(s_AddNewTextBlock()));
    connect(ui->AddFileBlock,SIGNAL(pressed()),this,SLOT(s_AddNewFileBlock()));
    connect(ui->RemoveBlock,SIGNAL(pressed()),this,SLOT(s_RemoveBlock()));


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

    // Set current shot object to finish dialog initialisation
    ui->ShotTable->setCurrentCell(0,0);
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
    if (IsFirstInitDone) {
        if (BackgroundImage!=NULL) {
            delete BackgroundImage;
            BackgroundImage=NULL;
        }
        Clean();
        RefreshBackgroundImage();
    }
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
    SetCompositionObject(CompositionList);                  // Make a new object init process
}

//====================================================================================================================

QSize DlgSlideProperties::GetSceneBoxSize() {
    return QSize(ui->GraphicsView->width(),ui->GraphicsView->height());
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

void DlgSlideProperties::s_CurrentShotSelectionChanged() {
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;

    // Fill the BlocTable
    ui->BlocTable->setUpdatesEnabled(false);
    while (ui->BlocTable->rowCount()>0) ui->BlocTable->removeRow(0);
    for (int i=0;i<DiaporamaObject->List[Current].ShotComposition.List.count();i++) {
        ui->BlocTable->insertRow(i);
        ui->BlocTable->setItem(i,0,new QTableWidgetItem(DiaporamaObject->List[Current].ShotComposition.List[i].Text));
    }
    ui->BlocTable->setUpdatesEnabled(true);

    // Refresh embeded widget
    SetCompositionObject(&DiaporamaObject->List[Current].ShotComposition);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::RefreshControls() {
    if (StopMAJSpinbox) return;
    // Ensure Current contain index of currented selected sequence
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;

    StopMAJSpinbox=true;    // Disable reintrence in this RefreshControls function

    //--------------------------------------------------------------------
    // Update controls
    //--------------------------------------------------------------------

/*
    ui->StaticSetCustomBt->setChecked(DiaporamaObject->List[Current].DefaultStaticDuration);
    ui->StaticCustomEd->setValue(DiaporamaObject->List[Current].StaticDuration/1000);
    if (DiaporamaObject->List[Current].DefaultStaticDuration) {
        ui->StaticCustomEd->setEnabled(false);
        ui->StaticCustomEd->setVisible(false);
        ui->StaticSpacer->setVisible(true);
    } else {
        ui->StaticCustomEd->setEnabled(true);
        ui->StaticCustomEd->setVisible(true);
        ui->StaticSpacer->setVisible(false);
    }

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

    ui->removeSequenceBT->setEnabled(ui->TableSeq->rowCount()>1);
    ui->UpSequenceBT->setEnabled((Current>0)&&(ui->TableSeq->rowCount()>1));
    ui->DownSequenceBT->setEnabled(Current<ui->TableSeq->rowCount()-1);
    ui->FullCanvasCB->setChecked(DiaporamaObject->FullCanvas);

    ui->CustomScene->RefreshControls();
*/

    //====================================================================================================================

    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    cBrushDefinition    *CurrentBrush   =(CurrentTextItem!=NULL)?&CurrentTextItem->BackgroundBrush:NULL;

    // Brush TAB part
    bool Allow_Brush  =(CurrentBrush!=NULL)&&(CurrentBrush->BrushType!=BRUSHTYPE_IMAGEDISK);
    bool Allow_Color1 =(Allow_Brush)&&((CurrentBrush->BrushType==BRUSHTYPE_SOLID)||(CurrentBrush->BrushType==BRUSHTYPE_PATTERN)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color2 =(Allow_Brush)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
    bool Allow_Color3 =(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3);
    bool Allow_Pattern=(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_PATTERN);
    bool Allow_Library=(Allow_Brush)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGELIBRARY);
    bool Allow_File   =(CurrentBrush!=NULL)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK);

    ui->BackgroundLine->setVisible(Allow_Brush);        ui->BrushTypeLabel->setVisible(Allow_Brush);        ui->BrushTypeCombo->setVisible(Allow_Brush);
    ui->ColorLabel1->setVisible(Allow_Color1);          ui->ColorLabel2->setVisible(Allow_Color1);
    ui->FirstColorCombo->setVisible(Allow_Color1);      ui->FinalColorCombo->setVisible(Allow_Color2);      ui->IntermColorCombo->setVisible(Allow_Color3);
    ui->OrientationLabel->setVisible(Allow_Color2);     ui->OrientationCombo->setVisible(Allow_Color2);
    ui->IntermPosLabel->setVisible(Allow_Color3);       ui->IntermPosSlider->setVisible(Allow_Color3);      ui->IntermPosED->setVisible(Allow_Color3);
    ui->PatternLabel->setVisible(Allow_Pattern);        ui->PatternBrushCombo->setVisible(Allow_Pattern);
    ui->ImageLibraryLabel->setVisible(Allow_Library);   ui->BackgroundCombo->setVisible(Allow_Library);
    ui->LineImage->setVisible(Allow_File);              ui->FileNameLabel->setVisible(Allow_File);
    ui->FileNameED->setVisible(Allow_File);             ui->FileNameBT->setVisible(Allow_File);
    ui->ImageGeometryLabel->setVisible(Allow_File);     ui->ImageGeometryCB->setVisible(Allow_File);
    ui->ImageEditCorrectBT->setVisible(Allow_File);     ui->ImageEditTransformBT->setVisible(Allow_File);
    ui->ImageEditTransformLabel->setVisible(Allow_File);
    ui->VideoEditLabel->setVisible(Allow_File && (CurrentBrush->Video!=NULL));
    ui->VideoEditBT->setVisible(Allow_File && (CurrentBrush->Video!=NULL));

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
                ui->FileNameED->setText(CurrentBrush->BrushFileName);
                ui->ImageGeometryCB->setCurrentIndex(CurrentBrush->BrushFileCorrect.ImageGeometry);
                break;
        }
    }

    if (CurrentTextItem!=NULL) {

        //***********************
        // Text TAB
        //***********************
        BLOCKCHSIZE=true;
        ui->fontSize->setDisabled(false);
        ui->fontSize->setCurrentIndex(ui->fontSize->findText(QString("%1").arg(CurrentTextItem->FontSize)));
        BLOCKCHSIZE=false;

        ui->RemoveBlock->setDisabled(false);

        ui->textLeft->setDisabled(false);       ui->textLeft->setChecked(CurrentTextItem->HAlign==0);       ui->textLeft->setDown(CurrentTextItem->HAlign==0);
        ui->textCenter->setDisabled(false);     ui->textCenter->setChecked(CurrentTextItem->HAlign==1);     ui->textCenter->setDown(CurrentTextItem->HAlign==1);
        ui->textJustif->setDisabled(false);     ui->textJustif->setChecked(CurrentTextItem->HAlign==3);     ui->textJustif->setDown(CurrentTextItem->HAlign==3);
        ui->textRight->setDisabled(false);      ui->textRight->setChecked(CurrentTextItem->HAlign==2);      ui->textRight->setDown(CurrentTextItem->HAlign==2);
        ui->textUp->setDisabled(false);         ui->textUp->setChecked(CurrentTextItem->VAlign==0);         ui->textUp->setDown(CurrentTextItem->VAlign==0);
        ui->textVCenter->setDisabled(false);    ui->textVCenter->setChecked(CurrentTextItem->VAlign==1);    ui->textVCenter->setDown(CurrentTextItem->VAlign==1);
        ui->textBottom->setDisabled(false);     ui->textBottom->setChecked(CurrentTextItem->VAlign==2);     ui->textBottom->setDown(CurrentTextItem->VAlign==2);
        ui->bold->setDisabled(false);           ui->bold->setChecked(CurrentTextItem->IsBold);              ui->bold->setDown(CurrentTextItem->IsBold);
        ui->Italic->setDisabled(false);         ui->Italic->setChecked(CurrentTextItem->IsItalic);          ui->Italic->setDown(CurrentTextItem->IsItalic);
        ui->Souligne->setDisabled(false);       ui->Souligne->setChecked(CurrentTextItem->IsUnderline);     ui->Souligne->setDown(CurrentTextItem->IsUnderline);

        ui->plainTextEdit->setDisabled(false);  if (ui->plainTextEdit->toPlainText()!=CurrentTextItem->Text) ui->plainTextEdit->setPlainText(CurrentTextItem->Text);
        ui->fontStyleCB->setDisabled(false);    ui->fontStyleCB->setCurrentIndex(ui->fontStyleCB->findText(QString(CurrentTextItem->FontName)));
        ui->fontEffectCB->setDisabled(false);   ui->fontEffectCB->setCurrentIndex(CurrentTextItem->StyleText);

        ui->FontColorCombo->setDisabled(false);
        ui->FontColorCombo->SetCurrentColor(&CurrentTextItem->FontColor);

        ui->StyleShadowColorCombo->setDisabled(CurrentTextItem->StyleText==0);
        ui->StyleShadowColorCombo->SetCurrentColor(&CurrentTextItem->FontShadowColor);

        //***********************
        // Shape TAB
        //***********************
        ui->BackgroundFormCB->setDisabled(false);                   ui->BackgroundFormCB->setCurrentIndex(CurrentTextItem->BackgroundForm-1);
        ui->PenSizeEd->setEnabled(true);                            ui->PenSizeEd->setValue(int(CurrentTextItem->PenSize));
        ui->PenColorCB->setDisabled(CurrentTextItem->PenSize==0);   ui->PenColorCB->SetCurrentColor(&CurrentTextItem->PenColor);
        ui->PenStyleCB->setDisabled(CurrentTextItem->PenSize==0);
        for (int i=0;i<ui->PenStyleCB->count();i++) if (ui->PenStyleCB->itemData(i).toInt()==CurrentTextItem->PenStyle) {
            ui->PenStyleCB->setCurrentIndex(i);
            break;
        }

        //***********************
        // Size & Position TAB
        //***********************
        ui->PosXEd->setDisabled(false);     ui->PosXEd->setRange(0,99-CurrentTextItem->w*100);          ui->PosXEd->setValue(CurrentTextItem->x*100);   ui->PosXEd->setSingleStep(int(100/20));
        ui->PosYEd->setDisabled(false);     ui->PosYEd->setRange(0,99-CurrentTextItem->h*100);          ui->PosYEd->setValue(CurrentTextItem->y*100);   ui->PosYEd->setSingleStep(int(100/20));
        ui->OpacityCB->setDisabled(false);  ui->OpacityCB->setCurrentIndex(CurrentTextItem->Opacity);
        ui->WidthEd->setDisabled(false);    ui->WidthEd->setRange(3,99-CurrentTextItem->x*100);         ui->WidthEd->setValue(CurrentTextItem->w*100);  ui->WidthEd->setSingleStep(int(100/20));
        ui->HeightEd->setDisabled(false);   ui->HeightEd->setRange(3,99-CurrentTextItem->y*100);        ui->HeightEd->setValue(CurrentTextItem->h*100); ui->HeightEd->setSingleStep(int(100/20));
        ui->RotateXED->setDisabled(false);  ui->RotateXED->setValue(CurrentTextItem->RotateXAxis);      ui->RotateXSLD->setDisabled(false);             ui->RotateXSLD->setValue(CurrentTextItem->RotateXAxis);
        ui->RotateYED->setDisabled(false);  ui->RotateYED->setValue(CurrentTextItem->RotateYAxis);      ui->RotateYSLD->setDisabled(false);             ui->RotateYSLD->setValue(CurrentTextItem->RotateYAxis);
        ui->RotateZED->setDisabled(false);  ui->RotateZED->setValue(CurrentTextItem->RotateZAxis);      ui->RotateZSLD->setDisabled(false);             ui->RotateZSLD->setValue(CurrentTextItem->RotateZAxis);
        ui->ShadowEffectCB->setDisabled(false);                                                             ui->ShadowEffectCB->setCurrentIndex(CurrentTextItem->FormShadow);
        ui->ShadowEffectED->setDisabled(false);                                                             ui->ShadowEffectED->setValue(int(CurrentTextItem->FormShadowDistance));

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

    RefreshBackgroundImage();

    // Refresh thumbnail
    ui->ShotTable->cellWidget(0,Current)->repaint();
    StopMAJSpinbox=false;
}

//====================================================================================================================

void DlgSlideProperties::SetCompositionObject(cCompositionList *TheCompositionList) {
    Clean(); // Clean scene
    CompositionList=TheCompositionList;
    RefreshBackgroundImage();               // Prepare the background

    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::RefreshBackgroundImage() {
    if ((!IsFirstInitDone)||(!CompositionList)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Prepare BackgroundImage if not exist
    if (!BackgroundImage) {
        QSize SceneboxSize=GetSceneBoxSize();
        if (DiaporamaObject->Parent->GetHeightForWidth(SceneboxSize.width())<SceneboxSize.height()) SceneboxSize=QSize(DiaporamaObject->Parent->GetWidthForHeight(SceneboxSize.height()),SceneboxSize.height());
            else SceneboxSize=QSize(SceneboxSize.width(),DiaporamaObject->Parent->GetHeightForWidth(SceneboxSize.width()));

        double   Ratio=1;
        if (DiaporamaObject->TypeObject==DIAPORAMAOBJECTTYPE_EMPTY) {
            QPainter Painter;
            double w,h;
            w=SceneboxSize.width();
            h=DiaporamaObject->Parent->GetHeightForWidth(w);
            if (h<SceneboxSize.height()) {
                h=SceneboxSize.height();
                w=DiaporamaObject->Parent->GetWidthForHeight(h);
            }
            BackgroundImage=new QImage(SceneboxSize.width(),SceneboxSize.height(),QImage::Format_ARGB32_Premultiplied);
            Painter.begin(BackgroundImage);
            DiaporamaObject->Parent->PrepareBackground(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject),SceneboxSize.width(),SceneboxSize.height(),&Painter,0,0,false);
            Painter.end();
            Ratio=w/h;
        }
    }

    // Ensure scene is created
    if (!scene) {
        // Calc and adjust ui->SceneBox depending on geometry
        xmax=ui->GraphicsView->width();
        ymax=int(double(xmax)*(double(BackgroundImage->height())/double(BackgroundImage->width())));

        // create the scene
        scene = new QGraphicsScene();
        connect(scene,SIGNAL(selectionChanged()),this,SLOT(s_SelectionChangeEvent()));
        scene->setSceneRect(QRectF(0,0,xmax,ymax));

        // Setup scene to control
        ui->GraphicsView->setScene(scene);
        ui->GraphicsView->setInteractive(true);
        ui->GraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);

        // Create cCustomGraphicsRectItem associate to existing cCompositionObject
        NextZValue=500;
        for (int i=0;i<CompositionList->List.count();i++) {
            // Create and add to scene a cCustomGraphicsRectItem
            new cCustomGraphicsRectItem(scene,CompositionList->List[i].ZValue,&CompositionList->List[i].x,&CompositionList->List[i].y,
                                        NULL,&CompositionList->List[i].w,&CompositionList->List[i].h,xmax,ymax,false,0,NULL,this,TYPE_DlgSlideProperties);
            if (NextZValue<CompositionList->List[i].ZValue) NextZValue=CompositionList->List[i].ZValue;
        }
        NextZValue+=10;  // 10 by 10 step for ZValue
    }

    // Draw image of the scene under the background
    QPixmap *NewImage=new QPixmap(QPixmap::fromImage(BackgroundImage->scaled(xmax,ymax)));
    QPainter P;
    P.begin(NewImage);

    ADJUST_RATIO=double(ymax)/double(1080);    // fixe Adjustment ratio for this slide

    for (int i=0;i<CompositionList->List.count();i++) {
        // Draw composition
        CompositionList->List[i].DrawCompositionObject(P,0,0,xmax,ymax);
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
    QGraphicsPixmapItem *im=scene->addPixmap(*NewImage);    // Image will be delete by scene
    im->setData(0,QVariant(QString("image")));
    im->setZValue(200);
    im->setPos(0,0);
    delete NewImage;

    QApplication::restoreOverrideCursor();
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

cCompositionObject *DlgSlideProperties::GetSelectedCompositionObject() {
    cCustomGraphicsRectItem *CurrentCustomGraphicsRectItem=GetSelectItem();
    if (CurrentCustomGraphicsRectItem!=NULL) {
        for (int i=0;i<CompositionList->List.count();i++) if (CompositionList->List[i].ZValue==CurrentCustomGraphicsRectItem->zValue()) return &CompositionList->List[i];
    }
    return NULL;
}

cBrushDefinition *DlgSlideProperties::GetCurrentBrush() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if ((CurrentTextItem)&&(CurrentTextItem->Opacity<4)) return &CurrentTextItem->BackgroundBrush;
        else return NULL;
}

//====================================================================================================================

void DlgSlideProperties::s_SelectionChangeEvent() {
    if (StopMAJSpinbox) return;
    RefreshBackgroundImage();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_addShot() {
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;

    cDiaporamaShot *imagesequence   =new cDiaporamaShot(DiaporamaObject);
    imagesequence->FilterCorrection =DiaporamaObject->List[Current].FilterCorrection;

    DiaporamaObject->List.append(*imagesequence);
    ui->ShotTable->insertColumn(ui->ShotTable->columnCount());
    wgt_QCustomThumbnails *Object=new wgt_QCustomThumbnails(ui->ShotTable,DiaporamaObject->Parent,THUMBNAILTYPE_SHOT);
    ui->ShotTable->setCellWidget(0,ui->ShotTable->columnCount()-1,Object);
    ui->ShotTable->setColumnWidth(ui->ShotTable->columnCount()-1,DiaporamaObject->Parent->GetWidthForHeight(ui->ShotTable->rowHeight(0)));
    ui->ShotTable->setCurrentCell(ui->ShotTable->columnCount()-1,0);
    SetCompositionObject(CompositionList);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_removeShot() {
    int Current=ui->ShotTable->currentColumn();
    if ((Current<0)||(Current>=DiaporamaObject->List.count())) return;
    if (DiaporamaObject->List.count()<2) return;
    DiaporamaObject->List.removeAt(Current);
    ui->ShotTable->setUpdatesEnabled(false);
    ui->ShotTable->removeColumn(Current);
    ui->ShotTable->setUpdatesEnabled(true);
    SetCompositionObject(CompositionList);
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_AddNewTextBlock() {
    CompositionList->List.append(cCompositionObject());
    cCompositionObject *CompositionObject=&CompositionList->List[CompositionList->List.count()-1];
    CompositionObject->ZValue=NextZValue;
    CompositionObject->Text=QCoreApplication::translate("DlgSlideProperties","Text","Default text value");

    // Unselect all item
    while (scene->selectedItems().count()>0) scene->selectedItems()[0]->setSelected(false);

    // Create and add to scene a cCustomGraphicsRectItem
    cCustomGraphicsRectItem *GraphicsRectItem=new cCustomGraphicsRectItem(scene,NextZValue,&CompositionObject->x,&CompositionObject->y,
                                                                          NULL,&CompositionObject->w,&CompositionObject->h,xmax,ymax,false,0,NULL,this,TYPE_DlgSlideProperties);

    // select new item
    GraphicsRectItem->setSelected(true);

    // 10 by 10 step for ZValue
    NextZValue+=10;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_AddNewFileBlock() {
    QString NewFile=QFileDialog::getOpenFileName(this,
                                                 QApplication::translate("DlgSlideProperties","Select a file"),
                                                 GlobalMainWindow->ApplicationConfig->RememberLastDirectories?GlobalMainWindow->ApplicationConfig->LastMediaPath:"",
                                                 GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(cApplicationConfig::ALLFILE));
    QCoreApplication::processEvents();
    if (NewFile=="") return;
    if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

    // Create a new block item and append it on the list
    CompositionList->List.append(cCompositionObject());
    cCompositionObject *CurrentBlock=&CompositionList->List[CompositionList->List.count()-1];
    cBrushDefinition   *CurrentBrush=&CurrentBlock->BackgroundBrush;

    CurrentBlock->ZValue       =NextZValue;
    CurrentBlock->Text         ="";
    CurrentBlock->PenSize      =0;
    CurrentBrush->BrushFileName=QFileInfo(NewFile).absoluteFilePath();
    CurrentBrush->BrushType    =BRUSHTYPE_IMAGEDISK;

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
        }
        break;
    }
    if (IsValide) {

        QImage *Image=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,GlobalMainWindow->ApplicationConfig->PreviewMaxHeight,true):
                       CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,GlobalMainWindow->ApplicationConfig->PreviewMaxHeight,0,true,true,NULL,1,false):
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
            double NewW=CurrentBlock->w*GlobalMainWindow->Diaporama->InternalWidth;
            double NewH=NewW*CurrentBrush->BrushFileCorrect.AspectRatio;
            NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
            NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
            if (NewH>1) {
                NewH=CurrentBlock->h*GlobalMainWindow->Diaporama->InternalHeight;
                NewW=NewH/CurrentBrush->BrushFileCorrect.AspectRatio;
                NewW=NewW/GlobalMainWindow->Diaporama->InternalWidth;
                NewH=NewH/GlobalMainWindow->Diaporama->InternalHeight;
            }
            CurrentBlock->w=NewW;
            CurrentBlock->h=NewH;
            delete Image;
        }
    }

    // Now, add this new block to the scene

    // Unselect all item
    while (scene->selectedItems().count()>0) scene->selectedItems()[0]->setSelected(false);

    // Create and add to scene a cCustomGraphicsRectItem
    cCustomGraphicsRectItem *GraphicsRectItem=new cCustomGraphicsRectItem(scene,NextZValue,&CurrentBlock->x,&CurrentBlock->y,NULL,&CurrentBlock->w,&CurrentBlock->h,xmax,ymax,
                                                                          CurrentBrush->BrushFileCorrect.ImageGeometry!=GEOMETRY_CUSTOM,CurrentBrush->BrushFileCorrect.AspectRatio,
                                                                          NULL,this,TYPE_DlgSlideProperties);

    // select new item
    GraphicsRectItem->setSelected(true);

    // 10 by 10 step for ZValue
    NextZValue+=10;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_RemoveBlock() {
    cCompositionObject      *CurrentComposeItem=GetSelectedCompositionObject();
    cCustomGraphicsRectItem *CurrentRectItem   =GetSelectItem();
    if ((CurrentComposeItem==NULL)||(CurrentRectItem==NULL)) return;
    int i=0;
    while ((i<CompositionList->List.count())&&(&CompositionList->List[i]!=CurrentComposeItem)) i++;
    if ((i<CompositionList->List.count())&&(&CompositionList->List[i]==CurrentComposeItem)) {
        delete CurrentRectItem;             // delete object from the scene
        CompositionList->List.removeAt(i);  // delete CurrentComposeItem is done by removeAt
    }
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetBold() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (CurrentTextItem->IsBold==true) CurrentTextItem->IsBold=false; else CurrentTextItem->IsBold=true;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetItalic() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (CurrentTextItem->IsItalic==true) CurrentTextItem->IsItalic=false; else CurrentTextItem->IsItalic=true;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetUnderline() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (CurrentTextItem->IsUnderline==true) CurrentTextItem->IsUnderline=false; else CurrentTextItem->IsUnderline=true;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextLeft() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->HAlign=0;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextCenter() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->HAlign=1;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextRight() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->HAlign=2;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextJustif() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->HAlign=3;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextUp() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->VAlign=0;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextVCenter() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->VAlign=1;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_SetTextBottom() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->VAlign=2;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeFont(QFont font) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (font.family()!="") CurrentTextItem->FontName=font.family();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeSizeFont(QString size) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if ((size!="")&&(BLOCKCHSIZE==false)) CurrentTextItem->FontSize=size.toInt();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeStyleFont(int Style) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->StyleText=Style;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgPosXValue(double Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
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
    if (CurrentTextItem==NULL) return;
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
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();
    if (RectItem==NULL) return;
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
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    cCustomGraphicsRectItem *RectItem=GetSelectItem();
    if (RectItem==NULL) return;
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
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->BackgroundForm=Style+1;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeOpacity(int Style) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->Opacity=Style;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgPenSize(int Value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    if (StopMAJSpinbox) return;
    CurrentTextItem->PenSize=Value;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_plainTextEditChange() {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->Text=ui->plainTextEdit->toPlainText();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangePenStyle(int index) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->PenStyle=ui->PenStyleCB->itemData(index).toInt();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgShadowFormValue(int value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->FormShadow=value;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChgShadowDistanceValue(int value) {
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->FormShadowDistance =value;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::MakeFormIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object;
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
        Object.DrawCompositionObject(Painter,0,0,32,32);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

void DlgSlideProperties::MakeTextStyleIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object;
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
        Object.DrawCompositionObject(Painter,0,0,32,32);
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
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Font color
void DlgSlideProperties::s_ChIndexFontColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->FontColor=ui->FontColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Text shadow color
void DlgSlideProperties::s_ChIndexFontShadowColorCombo(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->FontShadowColor=ui->StyleShadowColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Shape border color
void DlgSlideProperties::s_ChPenColorCB(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->PenColor=ui->PenColorCB->GetCurrentColor();
    RefreshControls();
}

//====================================================================================================================
// Handler for rotation controls
//====================================================================================================================

//========= Z Value
void DlgSlideProperties::s_ChgRotateZValue(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->RotateZAxis=Value;
    RefreshControls();
}

//========= X Value
void DlgSlideProperties::s_ChgRotateXValue(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->RotateXAxis=Value;
    RefreshControls();
}

//========= Y Value
void DlgSlideProperties::s_ChgRotateYValue(int Value) {
    if (StopMAJSpinbox) return;
    cCompositionObject  *CurrentTextItem=GetSelectedCompositionObject();
    if (CurrentTextItem==NULL) return;
    CurrentTextItem->RotateYAxis=Value;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_ChangeBrushTypeCombo(int Value) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->BrushType=ui->BrushTypeCombo->itemData(Value).toInt();
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_IntermPosSliderMoved(int Value) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->Intermediate=double(Value)/100;
    RefreshControls();
}

//====================================================================================================================

void DlgSlideProperties::s_IntermPosED(int Value) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->Intermediate=double(Value)/100;
    RefreshControls();
}

//====================================================================================================================
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Pattern shape combo
void DlgSlideProperties::s_ChIndexPatternBrushCombo(int) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->PatternType=ui->PatternBrushCombo->GetCurrentBrush()->PatternType;
    RefreshControls();
}

//========= Gradient shape orientation
void DlgSlideProperties::s_ChIndexGradientOrientationCombo(int) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->GradientOrientation=ui->OrientationCombo->GetCurrentBrush()->GradientOrientation;
    RefreshControls();
}

//========= Shape/Gradient shape first color
void DlgSlideProperties::s_ChIndexGradientFirstColorCombo(int) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->ColorD=ui->FirstColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Gradient shape last color
void DlgSlideProperties::s_ChIndexGradientFinalColorCombo(int) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->ColorF=ui->FinalColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Gradient shape intermediate color
void DlgSlideProperties::s_ChIndexGradientIntermColorCombo(int) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->ColorIntermed=ui->IntermColorCombo->GetCurrentColor();
    RefreshControls();
}

//========= Background image
void DlgSlideProperties::s_ChIndexBackgroundCombo(int) {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;
    CurrentBrush->BrushImage=ui->BackgroundCombo->GetCurrentBackground();
    RefreshControls();
}

//========= geometry of embeded image or video
void DlgSlideProperties::s_ChangeImageGeometry(int) {
    if (StopMAJSpinbox) return;
    cCompositionObject      *CurrentTextItem=GetSelectedCompositionObject();    if (!CurrentTextItem)   return;
    cBrushDefinition        *CurrentBrush   =GetCurrentBrush();                 if (!CurrentBrush)      return;
    cCustomGraphicsRectItem *RectItem       =GetSelectItem();                   if (!RectItem)          return;
    QImage                  *Image          =CurrentBrush->Image?CurrentBrush->Image->CacheImage:CurrentBrush->Video?CurrentBrush->Video->CacheFirstImage:NULL;
    if (!Image) return;

    CurrentBrush->BrushFileCorrect.ImageGeometry=ui->ImageGeometryCB->currentIndex();

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

        RefreshControls();
    } else {
        RectItem->AspectRatio=1;
        RectItem->KeepAspectRatio=false;
    }
}

//========= Button opending Dialog box
void DlgSlideProperties::ImageEditCorrect() {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;

}

void DlgSlideProperties::ImageEditTransform() {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;

}

void DlgSlideProperties::VideoEdit() {
    if (StopMAJSpinbox) return;
    cBrushDefinition *CurrentBrush=GetCurrentBrush();
    if (!CurrentBrush) return;

}
