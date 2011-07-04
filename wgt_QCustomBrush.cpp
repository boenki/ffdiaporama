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

#include "wgt_QCustomBrush.h"
#include "ui_wgt_QCustomBrush.h"
#include "mainwindow.h"

wgt_QCustomBrush::wgt_QCustomBrush(QWidget *parent) : QWidget(parent),ui(new Ui::wgt_QCustomBrush) {
    ui->setupUi(this);
    StopMAJSpinbox  = false;
    CurrentBrush    = NULL;
}

//====================================================================================================================

wgt_QCustomBrush::~wgt_QCustomBrush() {
    delete ui;
}

//====================================================================================================================

void wgt_QCustomBrush::InitWidget(bool AllowBrushTypeNoBrush,bool AllowBrushTypePattern) {
    connect(ui->BrushTypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBrushTypeCombo(int)));

    // Init combo box Background  type
    if (AllowBrushTypeNoBrush) {
        ui->BrushTypeCombo->addItem(QApplication::translate("wgt_QCustomBrush","No brush"));
        ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_NOBRUSH)));
    }
    ui->BrushTypeCombo->addItem(QApplication::translate("wgt_QCustomBrush","Solid brush"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_SOLID)));
    if (AllowBrushTypePattern) {
        ui->BrushTypeCombo->addItem(QApplication::translate("wgt_QCustomBrush","Pattern brush"));
        ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_PATTERN)));
    }
    ui->BrushTypeCombo->addItem(QApplication::translate("wgt_QCustomBrush","Gradient 2 colors"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT2)));

    ui->BrushTypeCombo->addItem(QApplication::translate("wgt_QCustomBrush","Gradient 3 colors"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT3)));

    ui->BrushTypeCombo->addItem(QApplication::translate("wgt_QCustomBrush","Image from library"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_IMAGELIBRARY)));

    // Handler for custom color/brush/pattern/gradient combo box index change
    connect(ui->PatternBrushCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexPatternBrushCombo(int)));
    connect(ui->OrientationCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientOrientationCombo(int)));
    connect(ui->FirstColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFirstColorCombo(int)));
    connect(ui->FinalColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFinalColorCombo(int)));
    connect(ui->IntermColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientIntermColorCombo(int)));
    connect(ui->BackgroundCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexBackgroundCombo(int)));

    // Intermediate position for gradient 3 colors
    connect(ui->IntermPosSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_IntermPosSliderMoved(int)));
    connect(ui->IntermPosED,SIGNAL(valueChanged(int)),this,SLOT(s_IntermPosED(int)));

    connect(ui->ImageFileBT,SIGNAL(pressed()),this,SLOT(s_SelectFile()));
}

//====================================================================================================================

void wgt_QCustomBrush::RefreshControls(cBrushDefinition *TheCurrentBrush,bool Allowed) {
    CurrentBrush=TheCurrentBrush;

    if (CurrentBrush!=NULL) {

        // Ensure BrushImage is valide
        if ((BackgroundList.SearchImage(CurrentBrush->BrushImage)==-1)&&(BackgroundList.List.count()>0)) CurrentBrush->BrushImage=BackgroundList.List[0].Name;

        StopMAJSpinbox=true;    // Disable reintrence in this RefreshControls function
        for (int i=0;i<ui->BrushTypeCombo->count();i++) if (ui->BrushTypeCombo->itemData(i).toInt()==CurrentBrush->BrushType) ui->BrushTypeCombo->setCurrentIndex(i);
        ui->FirstColorCombo->SetCurrentColor(&CurrentBrush->ColorD);
        ui->FinalColorCombo->SetCurrentColor(&CurrentBrush->ColorF);
        ui->IntermColorCombo->SetCurrentColor(&CurrentBrush->ColorIntermed);
        ui->PatternBrushCombo->SetCurrentBrush(CurrentBrush);
        ui->OrientationCombo->SetCurrentBrush(CurrentBrush);
        ui->BackgroundCombo->SetCurrentBackground(CurrentBrush->BrushImage);
        ui->IntermPosSlider->setValue(CurrentBrush->Intermediate*100);
        ui->IntermPosED->setValue(CurrentBrush->Intermediate*100);
        StopMAJSpinbox=false;

        ui->BrushTypeLabel->setVisible(Allowed);
        ui->BrushTypeCombo->setVisible(Allowed);
        ui->BrushTypeCombo->setEnabled(Allowed);

        bool FirstColorAllowed=(Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_SOLID)||(CurrentBrush->BrushType==BRUSHTYPE_PATTERN)||
                                           (CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->FirstColorLabel->setVisible((Allowed)&&(FirstColorAllowed));
        ui->FirstColorSpacer->setVisible((Allowed)&&(FirstColorAllowed));
        ui->FirstColorCombo->setVisible((Allowed)&&(FirstColorAllowed));
        ui->FirstColorCombo->setEnabled((Allowed)&&(FirstColorAllowed));

        ui->PatternSpacer->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_PATTERN));
        ui->PatternLabel->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_PATTERN));
        ui->PatternBrushCombo->setEnabled((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_PATTERN));
        ui->PatternBrushCombo->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_PATTERN));

        ui->FinalColorLabel->setVisible((Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3)));
        ui->FinalColorSpacer->setVisible((Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3)));
        ui->FinalColorCombo->setVisible((Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3)));
        ui->FinalColorCombo->setEnabled((Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3)));

        ui->IntermColorLabel->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermColorSpacer->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermColorCombo->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermColorCombo->setEnabled((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));

        ui->IntermPosLabel->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosSpacer->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosSlider->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosSlider->setEnabled((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosED->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosED->setEnabled((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3));

        ui->OrientationSpacer->setVisible((Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3)));
        ui->OrientationLabel->setVisible((Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3)));
        ui->OrientationCombo->setVisible((Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3)));
        ui->OrientationCombo->setEnabled((Allowed)&&((CurrentBrush->BrushType==BRUSHTYPE_GRADIENT2)||(CurrentBrush->BrushType==BRUSHTYPE_GRADIENT3)));

        ui->ImageLibraryLabel->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGELIBRARY));
        ui->BackgroundCombo->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGELIBRARY));

        ui->ImageFileLabel->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageFileED->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageFileBT->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageGeometryLabel->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageGeometryCB->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageEditCorrectBT->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageEditTransformBT->setVisible((Allowed)&&(CurrentBrush->BrushType==BRUSHTYPE_IMAGEDISK));

        ui->ImageFileED->setText((CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video?CurrentBrush->Video->FileName:""));

    } else {
        ui->BrushTypeLabel->setVisible(false);
        ui->BrushTypeCombo->setDisabled(true);      ui->BrushTypeCombo->setVisible(false);
        ui->FirstColorLabel->setVisible(false);     ui->FirstColorSpacer->setVisible(false);
        ui->FirstColorCombo->setEnabled(false);     ui->FirstColorCombo->setVisible(false);
        ui->PatternLabel->setVisible(false);        ui->PatternSpacer->setVisible(false);
        ui->PatternBrushCombo->setEnabled(false);   ui->PatternBrushCombo->setVisible(false);
        ui->FinalColorLabel->setVisible(false);     ui->FinalColorSpacer->setVisible(false);
        ui->FinalColorCombo->setVisible(false);     ui->FinalColorCombo->setEnabled(false);
        ui->IntermColorLabel->setVisible(false);    ui->IntermColorSpacer->setVisible(false);
        ui->IntermColorCombo->setVisible(false);    ui->IntermColorCombo->setEnabled(false);
        ui->IntermPosLabel->setVisible(false);      ui->IntermPosSpacer->setVisible(false);
        ui->IntermPosSlider->setVisible(false);     ui->IntermPosSlider->setEnabled(false);
        ui->IntermPosED->setVisible(false);         ui->IntermPosED->setEnabled(false);
        ui->OrientationLabel->setVisible(false);    ui->OrientationSpacer->setVisible(false);
        ui->OrientationCombo->setVisible(false);    ui->OrientationCombo->setEnabled(false);
        ui->ImageLibraryLabel->setVisible(false);   ui->BackgroundCombo->setVisible(false);
        ui->ImageFileLabel->setVisible(false);      ui->ImageFileED->setVisible(false);         ui->ImageFileBT->setVisible(false);
        ui->ImageGeometryLabel->setVisible(false);  ui->ImageGeometryCB->setVisible(false);
        ui->ImageEditCorrectBT->setVisible(false);  ui->ImageEditTransformBT->setVisible(false);
    }
}

//====================================================================================================================

void wgt_QCustomBrush::s_ChangeBrushTypeCombo(int Value) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->BrushType=ui->BrushTypeCombo->itemData(Value).toInt();
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_QCustomBrush::s_SelectFile() {
}

//====================================================================================================================

void wgt_QCustomBrush::s_IntermPosSliderMoved(int Value) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->Intermediate=double(Value)/100;
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_QCustomBrush::s_IntermPosED(int Value) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->Intermediate=double(Value)/100;
    emit NeedRefreshControls();
}

//====================================================================================================================
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Pattern shape combo
void wgt_QCustomBrush::s_ChIndexPatternBrushCombo(int) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->PatternType=ui->PatternBrushCombo->GetCurrentBrush()->PatternType;
    emit NeedRefreshControls();
}

//========= Gradient shape orientation
void wgt_QCustomBrush::s_ChIndexGradientOrientationCombo(int) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->GradientOrientation=ui->OrientationCombo->GetCurrentBrush()->GradientOrientation;
    emit NeedRefreshControls();
}

//========= Shape/Gradient shape first color
void wgt_QCustomBrush::s_ChIndexGradientFirstColorCombo(int) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->ColorD=ui->FirstColorCombo->GetCurrentColor();
    emit NeedRefreshControls();
}

//========= Gradient shape last color
void wgt_QCustomBrush::s_ChIndexGradientFinalColorCombo(int) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->ColorF=ui->FinalColorCombo->GetCurrentColor();
    emit NeedRefreshControls();
}

//========= Gradient shape intermediate color
void wgt_QCustomBrush::s_ChIndexGradientIntermColorCombo(int) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->ColorIntermed=ui->IntermColorCombo->GetCurrentColor();
    emit NeedRefreshControls();
}

//========= Background image
void wgt_QCustomBrush::s_ChIndexBackgroundCombo(int) {
    if (StopMAJSpinbox) return;
    if (CurrentBrush==NULL) return;
    CurrentBrush->BrushImage=ui->BackgroundCombo->GetCurrentBackground();
    emit NeedRefreshControls();
}
