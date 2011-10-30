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

#include "DlgBackgroundProperties.h"
#include "DlgImageCorrection.h"
#include "ui_DlgBackgroundProperties.h"
#include "mainwindow.h"

DlgBackgroundProperties::DlgBackgroundProperties(cDiaporamaObject *TheDiaporamaObject,QWidget *parent):QDialog(parent),ui(new Ui::DlgBackgroundProperties) {
    ui->setupUi(this);
    DiaporamaObject = TheDiaporamaObject;
    StopMAJSpinbox  = false;
    IsFirstInitDone = false;

#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
#else
    setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
#endif

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    DiaporamaObject->SaveToXML(root,"UNDO-DLG-OBJECT",QFileInfo(DiaporamaObject->Parent->ProjectFileName).absolutePath(),true);  // Save object
    Undo->appendChild(root);                                // Add object to xml document

    ui->SameBackgroundRD->setChecked(!DiaporamaObject->BackgroundType);
    ui->NewBackgroundRD->setChecked(DiaporamaObject->BackgroundType);

    // Init combo box Background  type
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgBackgroundProperties","Solid brush"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_SOLID)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgBackgroundProperties","Gradient 2 colors"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT2)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgBackgroundProperties","Gradient 3 colors"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_GRADIENT3)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgBackgroundProperties","Image from library"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_IMAGELIBRARY)));
    ui->BrushTypeCombo->addItem(QApplication::translate("DlgBackgroundProperties","Image from disk"));
    ui->BrushTypeCombo->setItemData(ui->BrushTypeCombo->count()-1,QVariant(int(BRUSHTYPE_IMAGEDISK)));

    // Connect signals
    connect(ui->SameBackgroundRD,SIGNAL(clicked()),this,SLOT(s_SameBackground()));
    connect(ui->NewBackgroundRD,SIGNAL(clicked()),this,SLOT(s_NewBackground()));

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

    connect(ui->BrushTypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChangeBrushTypeCombo(int)));

    // Handler for custom color/brush/pattern/gradient combo box index change
    connect(ui->PatternBrushCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexPatternBrushCombo(int)));
    connect(ui->OrientationCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientOrientationCombo(int)));
    connect(ui->FirstColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFirstColorCombo(int)));
    connect(ui->FinalColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientFinalColorCombo(int)));
    connect(ui->IntermColorCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexGradientIntermColorCombo(int)));
    connect(ui->BackgroundCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChIndexBackgroundCombo(int)));

    // Intermediate position for gradient 3 colors
    connect(ui->IntermPosSlider,SIGNAL(valueChanged(int)),this,SLOT(s_IntermPosSliderMoved(int)));
    connect(ui->IntermPosED,SIGNAL(valueChanged(int)),this,SLOT(s_IntermPosED(int)));

    connect(ui->ImageFileBT,SIGNAL(pressed()),this,SLOT(s_SelectFile()));
    connect(ui->ImageEditCorrectBT,SIGNAL(pressed()),this,SLOT(s_ImageEditCorrect()));
}

//====================================================================================================================

DlgBackgroundProperties::~DlgBackgroundProperties() {
    delete ui;
    delete Undo;
}

//====================================================================================================================

void DlgBackgroundProperties::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgBackgroundProperties);
}

//====================================================================================================================

void DlgBackgroundProperties::SetSavedWindowGeometry() {
    DiaporamaObject->Parent->ApplicationConfig->DlgBackgroundPropertiesWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgBackgroundProperties::resizeEvent(QResizeEvent *) {
    if (IsFirstInitDone) RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================

void DlgBackgroundProperties::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    if (IsFirstInitDone) return;
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
    IsFirstInitDone=true; // Set this flag to true to indicate that now we can prepeare display
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================

void DlgBackgroundProperties::reject() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgBackgroundPropertiesWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") {
        QStringList AliasList;
        DiaporamaObject->LoadFromXML(root,"UNDO-DLG-OBJECT","",AliasList);
    }
    done(1);
}

//====================================================================================================================

void DlgBackgroundProperties::accept() {
    // Save Window size and position
    DiaporamaObject->Parent->ApplicationConfig->DlgBackgroundPropertiesWSP->SaveWindowState(this);
    done(0);
}

//====================================================================================================================

void DlgBackgroundProperties::SetupUi() {
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================

void DlgBackgroundProperties::s_SameBackground() {
    if (DiaporamaObject==NULL) return;
    DiaporamaObject->BackgroundType=false;
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================

void DlgBackgroundProperties::s_NewBackground() {
    if (DiaporamaObject==NULL) return;
    DiaporamaObject->BackgroundType=true;
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================

void DlgBackgroundProperties::RefreshControls(bool Allowed) {
    if (!IsFirstInitDone) return;

    if (Allowed) {

        // Ensure BrushImage is valide
        if ((BackgroundList.SearchImage(DiaporamaObject->BackgroundBrush.BrushImage)==-1)&&(BackgroundList.List.count()>0)) DiaporamaObject->BackgroundBrush.BrushImage=BackgroundList.List[0].Name;

        StopMAJSpinbox=true;    // Disable reintrence in this RefreshControls function
        for (int i=0;i<ui->BrushTypeCombo->count();i++) if (ui->BrushTypeCombo->itemData(i).toInt()==DiaporamaObject->BackgroundBrush.BrushType) ui->BrushTypeCombo->setCurrentIndex(i);
        ui->FirstColorCombo->SetCurrentColor(&DiaporamaObject->BackgroundBrush.ColorD);
        ui->FinalColorCombo->SetCurrentColor(&DiaporamaObject->BackgroundBrush.ColorF);
        ui->IntermColorCombo->SetCurrentColor(&DiaporamaObject->BackgroundBrush.ColorIntermed);
        ui->PatternBrushCombo->SetCurrentBrush(&DiaporamaObject->BackgroundBrush);
        ui->OrientationCombo->SetCurrentBrush(&DiaporamaObject->BackgroundBrush);
        ui->BackgroundCombo->SetCurrentBackground(DiaporamaObject->BackgroundBrush.BrushImage);
        ui->IntermPosSlider->setValue(DiaporamaObject->BackgroundBrush.Intermediate*100);
        ui->IntermPosED->setValue(DiaporamaObject->BackgroundBrush.Intermediate*100);
        StopMAJSpinbox=false;

        ui->BrushTypeLabel->setVisible(Allowed);
        ui->BrushTypeCombo->setVisible(Allowed);
        ui->BrushTypeCombo->setEnabled(Allowed);

        bool FirstColorAllowed=(Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_SOLID)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_PATTERN)||
                                           (DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->FirstColorLabel->setVisible((Allowed)&&(FirstColorAllowed));
        ui->FirstColorSpacer->setVisible((Allowed)&&(FirstColorAllowed));
        ui->FirstColorCombo->setVisible((Allowed)&&(FirstColorAllowed));
        ui->FirstColorCombo->setEnabled((Allowed)&&(FirstColorAllowed));

        ui->PatternSpacer->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_PATTERN));
        ui->PatternLabel->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_PATTERN));
        ui->PatternBrushCombo->setEnabled((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_PATTERN));
        ui->PatternBrushCombo->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_PATTERN));

        ui->FinalColorLabel->setVisible((Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3)));
        ui->FinalColorSpacer->setVisible((Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3)));
        ui->FinalColorCombo->setVisible((Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3)));
        ui->FinalColorCombo->setEnabled((Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3)));

        ui->IntermColorLabel->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermColorSpacer->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermColorCombo->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermColorCombo->setEnabled((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));

        ui->IntermPosLabel->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosSpacer->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosSlider->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosSlider->setEnabled((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosED->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));
        ui->IntermPosED->setEnabled((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3));

        ui->OrientationSpacer->setVisible((Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3)));
        ui->OrientationLabel->setVisible((Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3)));
        ui->OrientationCombo->setVisible((Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3)));
        ui->OrientationCombo->setEnabled((Allowed)&&((DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT2)||(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_GRADIENT3)));

        ui->ImageLibraryLabel->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGELIBRARY));
        ui->BackgroundCombo->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGELIBRARY));
        ui->BackgroundComboSpacer->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGELIBRARY));

        ui->ImageFileLabel->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageFileED->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageFileBT->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageEditCorrectBT->setVisible((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK));
        ui->ImageEditCorrectBT->setEnabled((Allowed)&&(DiaporamaObject->BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&(DiaporamaObject->BackgroundBrush.Image!=NULL));
        ui->ImageFileED->setText(DiaporamaObject->BackgroundBrush.Image?DiaporamaObject->BackgroundBrush.Image->FileName:"");

        ui->Preview->setVisible(true);
        QApplication::processEvents();

        int H=ui->Preview->height();
        int W=DiaporamaObject->Parent->GetWidthForHeight(H);
        QImage Background=QImage(W,H,QImage::Format_ARGB32_Premultiplied);
        QPainter Painter;
        Painter.begin(&Background);
        Painter.fillRect(0,0,W,H,Qt::black);
        DiaporamaObject->Parent->PrepareBackground(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject),W,H,&Painter,0,0,false);
        Painter.end();
        ui->Preview->setPixmap(QPixmap::fromImage(Background));

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
        ui->ImageLibraryLabel->setVisible(false);   ui->BackgroundCombo->setVisible(false);     ui->BackgroundComboSpacer->setVisible(false);
        ui->ImageFileLabel->setVisible(false);      ui->ImageFileED->setVisible(false);         ui->ImageFileBT->setVisible(false);
        ui->ImageEditCorrectBT->setVisible(false);
        ui->Preview->setVisible(false);
    }
}

//====================================================================================================================

void DlgBackgroundProperties::s_ChangeBrushTypeCombo(int Value) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.BrushType=ui->BrushTypeCombo->itemData(Value).toInt();
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================

void DlgBackgroundProperties::s_SelectFile() {
    QString NewFile=QFileDialog::getOpenFileName(this,
                                                 QApplication::translate("DlgBackgroundProperties","Select a file"),
                                                 GlobalMainWindow->ApplicationConfig->RememberLastDirectories?GlobalMainWindow->ApplicationConfig->LastMediaPath:"",
                                                 GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(cApplicationConfig::IMAGEFILE));
    QApplication::processEvents();
    if (NewFile=="") return;
    if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use
    QString BrushFileName=QFileInfo(NewFile).absoluteFilePath();
    if (DiaporamaObject->BackgroundBrush.Image) {
        delete DiaporamaObject->BackgroundBrush.Image;
        DiaporamaObject->BackgroundBrush.Image=NULL;
    }
    DiaporamaObject->BackgroundBrush.Image=new cimagefilewrapper();
    QStringList AliasList;
    bool IsValide=DiaporamaObject->BackgroundBrush.Image->GetInformationFromFile(BrushFileName,AliasList);
    if (!IsValide) {
        delete DiaporamaObject->BackgroundBrush.Image;
        DiaporamaObject->BackgroundBrush.Image=NULL;
    } else {
        QImage *Image=DiaporamaObject->BackgroundBrush.Image->ImageAt(true,true,&DiaporamaObject->BackgroundBrush.Image->BrushFileTransform);
        if (Image) {
            DiaporamaObject->BackgroundBrush.InitDefaultFramingStyle(true,double(DiaporamaObject->Parent->InternalHeight)/double(DiaporamaObject->Parent->InternalWidth));
            DiaporamaObject->BackgroundBrush.ApplyStyle(true,DiaporamaObject->BackgroundBrush.DefaultFramingF); // Adjust to Full
            delete Image;
        } else {
            delete DiaporamaObject->BackgroundBrush.Image;
            DiaporamaObject->BackgroundBrush.Image=NULL;
        }
    }
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================

void DlgBackgroundProperties::s_IntermPosSliderMoved(int Value) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.Intermediate=double(Value)/100;
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================

void DlgBackgroundProperties::s_IntermPosED(int Value) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.Intermediate=double(Value)/100;
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//====================================================================================================================
// Handler for custom color/brush/pattern/gradient combo box index change
//====================================================================================================================

//========= Pattern shape combo
void DlgBackgroundProperties::s_ChIndexPatternBrushCombo(int) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.PatternType=ui->PatternBrushCombo->GetCurrentBrush()->PatternType;
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//========= Gradient shape orientation
void DlgBackgroundProperties::s_ChIndexGradientOrientationCombo(int) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.GradientOrientation=ui->OrientationCombo->GetCurrentBrush()->GradientOrientation;
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//========= Shape/Gradient shape first color
void DlgBackgroundProperties::s_ChIndexGradientFirstColorCombo(int) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.ColorD=ui->FirstColorCombo->GetCurrentColor();
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//========= Gradient shape last color
void DlgBackgroundProperties::s_ChIndexGradientFinalColorCombo(int) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.ColorF=ui->FinalColorCombo->GetCurrentColor();
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//========= Gradient shape intermediate color
void DlgBackgroundProperties::s_ChIndexGradientIntermColorCombo(int) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.ColorIntermed=ui->IntermColorCombo->GetCurrentColor();
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//========= Background image
void DlgBackgroundProperties::s_ChIndexBackgroundCombo(int) {
    if (StopMAJSpinbox) return;
    DiaporamaObject->BackgroundBrush.BrushImage=ui->BackgroundCombo->GetCurrentBackground();
    RefreshControls(ui->NewBackgroundRD->isChecked());
}

//========= Image file correction
void DlgBackgroundProperties::s_ImageEditCorrect() {
    if (DiaporamaObject->BackgroundBrush.Image) {
        DlgImageCorrection(NULL,1,&DiaporamaObject->BackgroundBrush,&DiaporamaObject->BackgroundBrush.BrushFileCorrect,0,this).exec();
        RefreshControls(ui->NewBackgroundRD->isChecked());
    }
}
