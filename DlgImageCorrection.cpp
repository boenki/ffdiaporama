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

#include "DlgImageCorrection.h"
#include "ui_DlgImageCorrection.h"
#include "mainwindow.h"

DlgImageCorrection::DlgImageCorrection(cCompositionObject *TheCurrentTextItem,int TheBackgroundForm,cBrushDefinition *TheCurrentBrush,int TheVideoPosition,QWidget *parent):QDialog(parent),ui(new Ui::DlgImageCorrection) {
    ui->setupUi(this);
    BackgroundForm  =TheBackgroundForm;
    CurrentTextItem =TheCurrentTextItem;
    CurrentBrush    =TheCurrentBrush;
    VideoPosition   =TheVideoPosition;
    UndoReloadImage =false;

#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
#else
    setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
#endif

    if (CurrentBrush->Image) {
        CachedImage=CurrentBrush->Image->ImageAt(true,true,NULL);
        GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.SetImageGeometryFilter(GlobalMainWindow->Diaporama->ImageGeometry,CurrentBrush->Image->ObjectGeometry);
     } else if (CurrentBrush->Video) {
        CachedImage=CurrentBrush->Video->ImageAt(true,VideoPosition,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),true,NULL,1,false,NULL,false);
        GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.SetImageGeometryFilter(GlobalMainWindow->Diaporama->ImageGeometry,CurrentBrush->Video->ObjectGeometry);
    }

    ui->LockGeometryCB->view()->setFixedWidth(300);

    ui->LockGeometryCB->addItem(QIcon(ICON_GEOMETRY_UNLOCKED),QApplication::translate("DlgImageCorrection","Unlock"));
    ui->LockGeometryCB->addItem(QIcon(ICON_GEOMETRY_LOCKED),  QApplication::translate("DlgImageCorrection","Lock to this geometry"));
    ui->LockGeometryCB->addItem(QIcon(ICON_GEOMETRY_PROJECT), QApplication::translate("DlgImageCorrection","Lock to project geometry"));
    ui->LockGeometryCB->addItem(QIcon(ICON_GEOMETRY_IMAGE),   QApplication::translate("DlgImageCorrection","Lock to image geometry"));

    // Save objects before modification for cancel button
    UndoBrushFileName=(CurrentBrush->Image!=NULL)?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName;

    UndoSlide=new QDomDocument(APPLICATION_NAME);
    QDomElement root=UndoSlide->createElement("UNDO-DLG");  // Create xml document and root
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT",GlobalMainWindow->Diaporama->ProjectFileName,true);  // Save object
    UndoSlide->appendChild(root);                           // Add object to xml document

    UndoShot=new QDomDocument(APPLICATION_NAME);
    root=UndoShot->createElement("UNDO-DLG");               // Create xml document and root
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->SaveToXML(root,"UNDO-DLG-OBJECT");              // Save object
    UndoShot->appendChild(root);                            // Add object to xml document

    IsFirstInitDone = false;                                // true when first show window was done
    FLAGSTOPED      = false;
    FLAGSTOPSPIN    = false;
    scene           = NULL;
    cadre           = NULL;

    switch (GlobalMainWindow->Diaporama->ImageGeometry) {
        case GEOMETRY_4_3   : ProjectGeometry=double(1440)/double(1920);  break;
        case GEOMETRY_16_9  : ProjectGeometry=double(1080)/double(1920);  break;
        case GEOMETRY_40_17 : ProjectGeometry=double(816)/double(1920);   break;

    }
    ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment

    if (CurrentBrush->Image)            ImageGeometry=double(CurrentBrush->Image->ImageHeight)/double(CurrentBrush->Image->ImageWidth);
        else if (CurrentBrush->Video)   ImageGeometry=double(CurrentBrush->Video->ImageHeight)/double(CurrentBrush->Video->ImageWidth);
    ImageGeometry=QString("%1").arg(ImageGeometry,0,'e').toDouble();  // Rounded to same number as style managment

    MagneticRuler.MagnetX1     = -1;
    MagneticRuler.MagnetY1     = -1;
    MagneticRuler.MagnetX2     = -1;
    MagneticRuler.MagnetY2     = -1;
    MagneticRuler.MagnetX3     = -1000; // Disable centering ruller
    MagneticRuler.MagnetY3     = -1000; // Disable centering ruller
    MagneticRuler.MagneticRuler= GlobalMainWindow->Diaporama->ApplicationConfig->FramingRuler;

    ui->RotateED->setMinimum(-180);
    ui->RotateED->setMaximum(180);

    ui->XValue->setSingleStep(1);  ui->XValue->setRange(-200,200);
    ui->YValue->setSingleStep(1);  ui->YValue->setRange(-200,200);
    ui->WValue->setSingleStep(1);  ui->WValue->setRange(1,200);
    ui->HValue->setSingleStep(1);  ui->HValue->setRange(1,200);

    ui->TransformationCB->SetCurrentFilter(CachedImage,CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform.OnOffFilter:&CurrentBrush->Image->BrushFileTransform.OnOffFilter);

    // If it's not an image then disable blur/sharpen
    if (CurrentBrush->Image==NULL) {
        ui->BlurLabel->setVisible(false);
        ui->BlurSigmaSlider->setVisible(false);
        ui->BlurSigmaSB->setVisible(false);
        ui->BlurSharpenResetBT->setVisible(false);
        ui->BlurRadiusLabel->setVisible(false);
        ui->BlurRadiusSlider->setVisible(false);
        ui->BlurRadiusED->setVisible(false);
        ui->RadiusResetBT->setVisible(false);
        ui->FileLabel->setPixmap(QPixmap(ICON_OBJECT_MOVIE));
    }

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

    connect(ui->RotateED,SIGNAL(valueChanged(double)),this,SLOT(s_RotationEDChanged(double)));
    connect(ui->XValue,SIGNAL(valueChanged(double)),this,SLOT(s_XValueEDChanged(double)));
    connect(ui->YValue,SIGNAL(valueChanged(double)),this,SLOT(s_YValueEDChanged(double)));
    connect(ui->WValue,SIGNAL(valueChanged(double)),this,SLOT(s_WValueEDChanged(double)));
    connect(ui->HValue,SIGNAL(valueChanged(double)),this,SLOT(s_HValueEDChanged(double)));
    connect(ui->RotateLeftBT,SIGNAL(clicked()),this,SLOT(s_RotateLeft()));
    connect(ui->RotateRightBT,SIGNAL(clicked()),this,SLOT(s_RotateRight()));
    connect(ui->AdjustHBT,SIGNAL(clicked()),this,SLOT(s_AdjustH()));
    connect(ui->AdjustWBT,SIGNAL(clicked()),this,SLOT(s_AdjustW()));
    connect(ui->AdjustWHBT,SIGNAL(clicked()),this,SLOT(s_AdjustWH()));
    connect(ui->MagneticEdgeBt,SIGNAL(clicked()),this,SLOT(s_MagneticEdgeBt()));

    connect(ui->BrightnessSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BrightnessSliderMoved(int)));
    connect(ui->BrightnessValue,SIGNAL(valueChanged(int)),this,SLOT(s_BrightnessSliderMoved(int)));
    connect(ui->BrightnessResetBT,SIGNAL(clicked()),this,SLOT(s_BrightnessReset()));
    connect(ui->ContrastSlider,SIGNAL(valueChanged(int)),this,SLOT(s_ContrastSliderMoved(int)));
    connect(ui->ContrastValue,SIGNAL(valueChanged(int)),this,SLOT(s_ContrastSliderMoved(int)));
    connect(ui->ContrastResetBT,SIGNAL(clicked()),this,SLOT(s_ContrastReset()));
    connect(ui->GammaSlider,SIGNAL(valueChanged(int)),this,SLOT(s_GammaSliderMoved(int)));
    connect(ui->GammaValue,SIGNAL(valueChanged(double)),this,SLOT(s_GammaValueED(double)));
    connect(ui->GammaResetBT,SIGNAL(clicked()),this,SLOT(s_GammaReset()));
    connect(ui->RedSlider,SIGNAL(valueChanged(int)),this,SLOT(s_RedSliderMoved(int)));
    connect(ui->RedValue,SIGNAL(valueChanged(int)),this,SLOT(s_RedSliderMoved(int)));
    connect(ui->RedResetBT,SIGNAL(clicked()),this,SLOT(s_RedReset()));
    connect(ui->GreenSlider,SIGNAL(valueChanged(int)),this,SLOT(s_GreenSliderMoved(int)));
    connect(ui->GreenValue,SIGNAL(valueChanged(int)),this,SLOT(s_GreenSliderMoved(int)));
    connect(ui->GreenResetBT,SIGNAL(clicked()),this,SLOT(s_GreenReset()));
    connect(ui->BlueSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlueSliderMoved(int)));
    connect(ui->BlueValue,SIGNAL(valueChanged(int)),this,SLOT(s_BlueSliderMoved(int)));
    connect(ui->BlueResetBT,SIGNAL(clicked()),this,SLOT(s_BlueReset()));

    connect(ui->TransformationCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChTransformationCB(int)));
    connect(ui->BlurRadiusSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlurRadiusSliderMoved(int)));
    connect(ui->BlurRadiusED,SIGNAL(valueChanged(int)),this,SLOT(s_BlurRadiusSliderMoved(int)));
    connect(ui->BlurSigmaSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlurSigmaSliderMoved(int)));
    connect(ui->BlurSigmaSB,SIGNAL(valueChanged(double)),this,SLOT(s_BlurSigmaValueED(double)));
    connect(ui->BlurSharpenResetBT,SIGNAL(clicked()),this,SLOT(s_BlurSharpenReset()));
    connect(ui->RadiusResetBT,SIGNAL(clicked()),this,SLOT(s_RadiusReset()));

    connect(ui->LockGeometryCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_LockGeometryCB(int)));
    connect(ui->FramingStyleBT,SIGNAL(pressed()),this,SLOT(s_FramingStyleBT()));
}

DlgImageCorrection::~DlgImageCorrection() {
    delete ui;  // Deleting this make deletion of scene and all included object
    scene=NULL;
    delete CachedImage;
}

//====================================================================================================================

void DlgImageCorrection::s_BrightnessReset() {
    s_BrightnessSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_ContrastReset() {
    s_ContrastSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_GammaReset() {
    s_GammaValueED(1);
}

//====================================================================================================================

void DlgImageCorrection::s_RedReset() {
    s_RedSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_GreenReset() {
    s_GreenSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_BlueReset() {
    s_BlueSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_BlurSharpenReset() {
    s_BlurSigmaSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_RadiusReset() {
    s_BlurRadiusSliderMoved(5);
}


//====================================================================================================================

void DlgImageCorrection::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgImageCorrection);
}

//====================================================================================================================

void DlgImageCorrection::SetSavedWindowGeometry() {
    GlobalMainWindow->ApplicationConfig->DlgImageCorrectionWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgImageCorrection::resizeEvent(QResizeEvent *) {
    if (scene!=NULL) {
        if (cadre!=NULL) {
            scene->removeItem(cadre);
            delete cadre;
            cadre=NULL;
        }
        //Select the most lower between ui->GraphicsView.width and ui->GraphicsView.height as scene rectangle
        if (ui->GraphicsView->width()<ui->GraphicsView->height()) {
            xmax=ui->GraphicsView->width();
            ymax=xmax;
        } else {
            ymax=ui->GraphicsView->height();
            xmax=ymax;
        }
        scene->setSceneRect(QRectF(0,0,xmax,ymax));
        ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);
        RefreshBackgroundImage();
    }
}

//====================================================================================================================

void DlgImageCorrection::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));

    if (cadre!=NULL) {
        scene->removeItem(cadre);
        delete cadre;
        cadre=NULL;
    }
    if (scene!=NULL) {
        delete scene;
        scene=NULL;
    }

    //Select the most lower between ui->GraphicsView.width and ui->GraphicsView.height as scene rectangle
    if (ui->GraphicsView->width()<ui->GraphicsView->height()) {
        xmax=ui->GraphicsView->width();
        ymax=xmax;
    } else {
        ymax=ui->GraphicsView->height();
        xmax=ymax;
    }
    // Create the scene
    scene = new QGraphicsScene();
    scene->setSceneRect(QRectF(0,0,xmax,ymax));

    // Setup scene to control
    ui->GraphicsView->setScene(scene);
    ui->GraphicsView->setInteractive(true);
    ui->GraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);
    RefreshControls();
 }

//====================================================================================================================

void DlgImageCorrection::reject() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgImageCorrectionWSP->SaveWindowState(this);

    // Restore objects
    QDomElement root=UndoSlide->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL);

    root=UndoShot->documentElement();
    if (root.tagName()=="UNDO-DLG") {
        cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
        Filter->LoadFromXML(root,"UNDO-DLG-OBJECT");
    }
    if (UndoReloadImage) {
        if (CurrentBrush->Image)            CurrentBrush->Image->GetInformationFromFile(UndoBrushFileName,NULL,NULL);
            else if (CurrentBrush->Video)   CurrentBrush->Video->GetInformationFromFile(UndoBrushFileName,NULL,NULL);
        delete CachedImage;
        CachedImage=NULL;
        if (CurrentBrush->Image) CachedImage=CurrentBrush->Image->ImageAt(true,true,NULL);
            else if (CurrentBrush->Video) CachedImage=CurrentBrush->Video->ImageAt(true,VideoPosition,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),true,NULL,1,false,NULL,false);
    }

    done(1);
}

//====================================================================================================================

void DlgImageCorrection::accept() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgImageCorrectionWSP->SaveWindowState(this);

    // Close the box
    done(0);
}

//====================================================================================================================

void DlgImageCorrection::s_XValueEDChanged(double Value) {
    if (FLAGSTOPED) return;
    CurrentBrush->BrushFileCorrect.X=Value/100;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_YValueEDChanged(double Value) {
    if (FLAGSTOPED) return;
    CurrentBrush->BrushFileCorrect.Y=Value/100;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_WValueEDChanged(double Value) {
    if (FLAGSTOPED) return;
    if (CurrentBrush->BrushFileCorrect.LockGeometry) {
        CurrentBrush->BrushFileCorrect.ZoomFactor=Value/100;
    } else {
        double newH=CurrentBrush->BrushFileCorrect.ZoomFactor*CurrentBrush->BrushFileCorrect.AspectRatio*ymax;
        CurrentBrush->BrushFileCorrect.ZoomFactor=Value/100;
        double newW=CurrentBrush->BrushFileCorrect.ZoomFactor*xmax;
        CurrentBrush->BrushFileCorrect.AspectRatio=newH/newW;
    }
    // Resize and repos all item in the scene
    for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
        cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
        RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
        RectItem->setPos(CurrentBrush->BrushFileCorrect.X*xmax,CurrentBrush->BrushFileCorrect.Y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentBrush->BrushFileCorrect.X*xmax,
                                                      CurrentBrush->BrushFileCorrect.Y*ymax,
                                                      xmax*CurrentBrush->BrushFileCorrect.ZoomFactor,
                                                      xmax*CurrentBrush->BrushFileCorrect.ZoomFactor*CurrentBrush->BrushFileCorrect.AspectRatio));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_HValueEDChanged(double Value) {
    if (FLAGSTOPED) return;
    double newH=(Value/100)*ymax;
    if (CurrentBrush->BrushFileCorrect.LockGeometry) {
        double newW=newH/CurrentBrush->BrushFileCorrect.AspectRatio;
        CurrentBrush->BrushFileCorrect.ZoomFactor=newW/xmax;
    } else {
        double newW=CurrentBrush->BrushFileCorrect.ZoomFactor*xmax;
        CurrentBrush->BrushFileCorrect.AspectRatio=newH/newW;
    }
    // Resize and repos all item in the scene
    for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
        cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
        RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
        RectItem->setPos(CurrentBrush->BrushFileCorrect.X*xmax,CurrentBrush->BrushFileCorrect.Y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentBrush->BrushFileCorrect.X*xmax,
                                                      CurrentBrush->BrushFileCorrect.Y*ymax,
                                                      xmax*CurrentBrush->BrushFileCorrect.ZoomFactor,
                                                      xmax*CurrentBrush->BrushFileCorrect.ZoomFactor*CurrentBrush->BrushFileCorrect.AspectRatio));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotationEDChanged(double Value) {
    if (FLAGSTOPED) return;
    if (Value<-180) Value=360-Value;
    if (Value>180)  Value=-360-Value;
    CurrentBrush->BrushFileCorrect.ImageRotation=Value;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotateLeft() {
    double Value=(int((CurrentBrush->BrushFileCorrect.ImageRotation-90)/90)*90);
    if (Value<=-180) Value=360-Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_RotateRight() {
    double Value=(int((CurrentBrush->BrushFileCorrect.ImageRotation+90)/90)*90);
    if (Value>180) Value=-360+Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustW() {
    double W=MagneticRuler.MagnetX2-MagneticRuler.MagnetX1;
    double H=W*CurrentBrush->BrushFileCorrect.AspectRatio;
    CurrentBrush->BrushFileCorrect.X=((xmax-W)/2)/xmax;
    CurrentBrush->BrushFileCorrect.Y=((ymax-H)/2)/ymax;
    CurrentBrush->BrushFileCorrect.ZoomFactor=W/xmax;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustH() {
    double H=MagneticRuler.MagnetY2-MagneticRuler.MagnetY1;
    double W=H/CurrentBrush->BrushFileCorrect.AspectRatio;
    CurrentBrush->BrushFileCorrect.X=((xmax-W)/2)/xmax;
    CurrentBrush->BrushFileCorrect.Y=((ymax-H)/2)/ymax;
    CurrentBrush->BrushFileCorrect.ZoomFactor=W/xmax;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustWH() {
    // Special case for custom geometry -> use all the image then change aspect ratio to image aspect ratio
    if (!CurrentBrush->BrushFileCorrect.LockGeometry) {
        double W=MagneticRuler.MagnetX2-MagneticRuler.MagnetX1;
        double H=MagneticRuler.MagnetY2-MagneticRuler.MagnetY1;
        CurrentBrush->BrushFileCorrect.AspectRatio=H/W;
        CurrentBrush->BrushFileCorrect.X=((xmax-W)/2)/xmax;
        CurrentBrush->BrushFileCorrect.Y=((ymax-H)/2)/ymax;
        CurrentBrush->BrushFileCorrect.ZoomFactor=W/xmax;
        for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
            cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
            RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
        }
        RefreshControls();
    } else {
        double W=MagneticRuler.MagnetX2-MagneticRuler.MagnetX1;
        double H=W*CurrentBrush->BrushFileCorrect.AspectRatio;
        if (H<MagneticRuler.MagnetY2-MagneticRuler.MagnetY1) s_AdjustH(); else s_AdjustW();
    }
}

//====================================================================================================================

void DlgImageCorrection::RefreshControls() {
    if ((!scene)||(FLAGSTOPED)) return;
    FLAGSTOPED=true;

    if (((CurrentBrush->Image==NULL)&&(CurrentBrush->Video==NULL))||
        ((CurrentBrush->Image!=NULL)&&(CurrentBrush->Image->ObjectGeometry==IMAGE_GEOMETRY_UNKNOWN))||
        ((CurrentBrush->Video!=NULL)&&(CurrentBrush->Video->ObjectGeometry==IMAGE_GEOMETRY_UNKNOWN))) {
        ui->FramingStyleED->setText(QApplication::translate("DlgSlideProperties","No style for nonstandard geometry image"));
        ui->FramingStyleBT->setEnabled(false);
    } else {
        ui->FramingStyleED->setText(GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.GetStyleName(CurrentBrush->GetFramingStyle()));
        ui->FramingStyleBT->setEnabled(true);
    }

    ui->XValue->setValue(CurrentBrush->BrushFileCorrect.X*100);
    ui->YValue->setValue(CurrentBrush->BrushFileCorrect.Y*100);
    ui->WValue->setValue(CurrentBrush->BrushFileCorrect.ZoomFactor*100);
    ui->HValue->setValue(CurrentBrush->BrushFileCorrect.ZoomFactor*CurrentBrush->BrushFileCorrect.AspectRatio*100);

    if (!CurrentBrush->BrushFileCorrect.LockGeometry)                        ui->LockGeometryCB->setCurrentIndex(0);
    else if (CurrentBrush->BrushFileCorrect.AspectRatio==ProjectGeometry)    ui->LockGeometryCB->setCurrentIndex(2);
    else if (CurrentBrush->BrushFileCorrect.AspectRatio==ImageGeometry)      ui->LockGeometryCB->setCurrentIndex(3);
    else                                                        ui->LockGeometryCB->setCurrentIndex(1);

    ui->RotateED->setValue(CurrentBrush->BrushFileCorrect.ImageRotation);

    // FilterCorrection
    ui->BrightnessSlider->setValue(CurrentBrush->BrushFileCorrect.Brightness);     ui->BrightnessValue->setValue(CurrentBrush->BrushFileCorrect.Brightness);
    ui->ContrastSlider->setValue(CurrentBrush->BrushFileCorrect.Contrast);         ui->ContrastValue->setValue(CurrentBrush->BrushFileCorrect.Contrast);
    ui->GammaSlider->setValue(CurrentBrush->BrushFileCorrect.Gamma*100);           ui->GammaValue->setValue(CurrentBrush->BrushFileCorrect.Gamma);
    ui->RedSlider->setValue(CurrentBrush->BrushFileCorrect.Red);                   ui->RedValue->setValue(CurrentBrush->BrushFileCorrect.Red);
    ui->GreenSlider->setValue(CurrentBrush->BrushFileCorrect.Green);               ui->GreenValue->setValue(CurrentBrush->BrushFileCorrect.Green);
    ui->BlueSlider->setValue(CurrentBrush->BrushFileCorrect.Blue);                 ui->BlueValue->setValue(CurrentBrush->BrushFileCorrect.Blue);

    ui->BlurSigmaSlider->setValue((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurSigma*10);
    ui->BlurSigmaSB->setValue((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurSigma);
    ui->BlurRadiusSlider->setValue(int((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurRadius));
    ui->BlurRadiusED->setValue(int((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurRadius));

    // Resize and repos all item in the scene
    for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
        cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];

        // Set aspect ratio from Brush to Rect if geometrie is not custom or from rect to brush if geometrie is custom
        if (CurrentBrush->BrushFileCorrect.LockGeometry)
            RectItem->AspectRatio=CurrentBrush->BrushFileCorrect.AspectRatio;
            else CurrentBrush->BrushFileCorrect.AspectRatio=RectItem->AspectRatio;

        RectItem->setPos(CurrentBrush->BrushFileCorrect.X*xmax,CurrentBrush->BrushFileCorrect.Y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(CurrentBrush->BrushFileCorrect.X*xmax,
                                                      CurrentBrush->BrushFileCorrect.Y*ymax,
                                                      xmax*CurrentBrush->BrushFileCorrect.ZoomFactor,
                                                      xmax*CurrentBrush->BrushFileCorrect.ZoomFactor*CurrentBrush->BrushFileCorrect.AspectRatio));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    }

    ui->FileNameED->setText(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video?CurrentBrush->Video->FileName:"");
    connect(ui->FileNameBT,SIGNAL(clicked()),this,SLOT(ChangeBrushDiskFile()));

    // Refresh image
    RefreshBackgroundImage();
    FLAGSTOPED=false;
}

//====================================================================================================================

void DlgImageCorrection::ChangeBrushDiskFile() {
    QString ActualFilePath=QFileInfo(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName).absolutePath();

    QString NewFile=QFileDialog::getOpenFileName(this,
                                                 QApplication::translate("DlgSlideProperties","Select a file"),
                                                 ActualFilePath,//GlobalMainWindow->ApplicationConfig->RememberLastDirectories?GlobalMainWindow->ApplicationConfig->LastMediaPath:"",
                                                 GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(CurrentBrush->Image?IMAGEFILE:VIDEOFILE));
    QApplication::processEvents();
    if (NewFile=="") return;
    if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

    QString NewBrushFileName=QFileInfo(NewFile).absoluteFilePath();
    QString OldBrushFileName=CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName;
    QImage  *NewCachedImage =NULL;

    if (CurrentBrush->Image) {
        CurrentBrush->Image->GetInformationFromFile(NewBrushFileName,NULL,NULL);
        if (CurrentBrush->Image->IsValide) NewCachedImage=CurrentBrush->Image->ImageAt(true,true,NULL);
    } else if (CurrentBrush->Video) {
        if (CurrentBrush->Video->GetInformationFromFile(NewBrushFileName,NULL,NULL)&&(CurrentBrush->Video->OpenCodecAndFile()))
            NewCachedImage=CurrentBrush->Video->ImageAt(true,VideoPosition,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),true,NULL,1,false,NULL,false);

        if (!NewCachedImage) {
            QMessageBox::critical(NULL,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+QApplication::translate("MainWindow","Format not supported","Error message"),QMessageBox::Close);
            CurrentBrush->Video->GetInformationFromFile(OldBrushFileName,NULL,NULL);
            CurrentBrush->Video->OpenCodecAndFile();
        }
    }
    if (NewCachedImage) {
        delete CachedImage;
        CachedImage=NewCachedImage;
        UndoReloadImage=true;
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_ChTransformationCB(int) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->OnOffFilter=ui->TransformationCB->GetCurrentFilter();
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_BlurSigmaSliderMoved(int Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurSigma=double(Value)/10;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaSB->setValue(Filter->BlurSigma);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_BlurSigmaValueED(double Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurSigma=Value;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaSB->setValue(Filter->BlurSigma);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_BlurRadiusSliderMoved(int Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurRadius=double(Value);
    ui->BlurRadiusSlider->setValue(int(Filter->BlurRadius));
    ui->BlurRadiusED->setValue(int(Filter->BlurRadius));
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_MagneticEdgeBt() {
    if (MagneticRuler.MagneticRuler==true) MagneticRuler.MagneticRuler=false; else MagneticRuler.MagneticRuler=true;
    ui->MagneticEdgeBt->setIcon(QIcon(MagneticRuler.MagneticRuler?QString(ICON_RULER_ON):QString(ICON_RULER_OFF)));
    GlobalMainWindow->Diaporama->ApplicationConfig->FramingRuler=MagneticRuler.MagneticRuler;
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::RefreshBackgroundImage() {
    if (!scene) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Remove old image if exist
    int i=0;
    while ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()!="image")) i++;
    if ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()=="image")) {
        QGraphicsPixmapItem *im=(QGraphicsPixmapItem *)scene->items().at(i);
        scene->removeItem(im);
        delete im;
    }
    scene->clearSelection();

    // Create selection box
    if (cadre==NULL) cadre=new cCustomGraphicsRectItem(scene,300,&CurrentBrush->BrushFileCorrect.X,&CurrentBrush->BrushFileCorrect.Y,&CurrentBrush->BrushFileCorrect.ZoomFactor,
                                                       NULL,NULL,xmax,ymax,CurrentBrush->BrushFileCorrect.LockGeometry,
                                                       CurrentBrush->BrushFileCorrect.AspectRatio,&MagneticRuler,this,TYPE_DlgImageCorrection,0,true);

    // Prepare CacheImage
    QImage   *NewImage=new QImage(xmax,ymax,QImage::Format_ARGB32_Premultiplied);
    QImage   *SourceImage=NULL;
    QPainter P;

    double SizeRatio=double(xmax)/double(ui->GraphicsView->width());
    int    WithPen  =int(SizeRatio); if (double(WithPen)<SizeRatio) WithPen++;

    // Rotate image if needed and create a SourceImage
    if (CurrentBrush->BrushFileCorrect.ImageRotation!=0) {
        QTransform matrix;
        matrix.rotate(CurrentBrush->BrushFileCorrect.ImageRotation,Qt::ZAxis);
        SourceImage=new QImage(CachedImage->transformed(matrix,GlobalMainWindow->ApplicationConfig->Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));

    // If no rotation then SourceImage=CachedImage
    } else SourceImage=CachedImage;

    // Calc coordinates of the part in the source image
    double  RealImageW=double(SourceImage->width());                  // Get real image widht
    double  RealImageH=double(SourceImage->height());                 // Get real image height
    double  Hyp =sqrt(CachedImage->width()*CachedImage->width()+CachedImage->height()*CachedImage->height());   // Calc hypothenuse of the image to define full canvas
    double  DstX=((Hyp-RealImageW)/2)*(xmax/Hyp);
    double  DstY=((Hyp-RealImageH)/2)*(ymax/Hyp);
    double  DstW=RealImageW*(xmax/Hyp);
    double  DstH=RealImageH*(ymax/Hyp);

    QImage ToUseImage=SourceImage->scaled(DstW,DstH);
    if (CurrentBrush->Image) CurrentBrush->Image->BrushFileTransform.ApplyFilter(&ToUseImage);
        else if (CurrentBrush->Video) CurrentBrush->Video->BrushFileTransform.ApplyFilter(&ToUseImage);
    CurrentBrush->BrushFileCorrect.ApplyFilter(&ToUseImage);

    P.begin(NewImage);
    P.fillRect(QRectF(0,0,xmax,ymax),Transparent);
    P.drawImage(QRectF(DstX,DstY,DstW,DstH),ToUseImage,QRectF(0,0,DstW,DstH));
    if (SourceImage!=CachedImage) delete SourceImage;

    //********************************************************

    // Calc magnetic ruller guides positions
    MagneticRuler.MagnetX1=DstX;
    MagneticRuler.MagnetX2=DstX+DstW;
    MagneticRuler.MagnetY1=DstY;
    MagneticRuler.MagnetY2=DstY+DstH;

    // Draw selection rectangle for cadre
    double X=CurrentBrush->BrushFileCorrect.X*xmax;
    double Y=CurrentBrush->BrushFileCorrect.Y*ymax;
    double W=CurrentBrush->BrushFileCorrect.ZoomFactor*xmax;
    double H=W*CurrentBrush->BrushFileCorrect.AspectRatio;

    QImage      *FormImage=new QImage(xmax,ymax,QImage::Format_ARGB32_Premultiplied);
    QPainter    PB;
    PB.begin(FormImage);
    PB.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
    PB.setPen(Qt::NoPen);
    PB.fillRect(QRect(0,0,xmax,ymax),QBrush(0x555555));
    PB.setCompositionMode(QPainter::CompositionMode_Source);
    PB.setBrush(Qt::transparent);
    DrawShape(PB,BackgroundForm,X,Y,W,H,X+(W/2),Y+(H/2));
    PB.setCompositionMode(QPainter::CompositionMode_SourceOver);
    PB.end();
    P.setOpacity(0.75);
    P.drawImage(0,0,*FormImage);
    P.setOpacity(1);
    delete FormImage;

    // draw guides
    P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    QColor col=QColor(255,0,0);
    QPen   pen=QPen(col);
    pen.setWidth(WithPen);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setStyle(Qt::DotLine);
    P.setPen(pen);
    P.setBrush(Qt::NoBrush);
    P.drawRect(X,Y,W,H);
    if (MagneticRuler.MagneticRuler==true) {
        QColor col=QColor(0,255,0);
        QPen   pen=QPen(col);
        pen.setWidth(WithPen);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setStyle(Qt::DotLine);
        P.setPen(pen);
        if (MagneticRuler.MagnetX1!=-1) P.drawLine(MagneticRuler.MagnetX1,0,MagneticRuler.MagnetX1,ymax);
        if (MagneticRuler.MagnetX2!=-1) P.drawLine(MagneticRuler.MagnetX2,0,MagneticRuler.MagnetX2,ymax);
        if (MagneticRuler.MagnetY1!=-1) P.drawLine(0,MagneticRuler.MagnetY1,xmax,MagneticRuler.MagnetY1);
        if (MagneticRuler.MagnetY2!=-1) P.drawLine(0,MagneticRuler.MagnetY2,xmax,MagneticRuler.MagnetY2);
    }
    P.setCompositionMode(QPainter::CompositionMode_SourceOver);
    P.end();

    // Add image to the background of the scene
    QGraphicsPixmapItem *im=scene->addPixmap(QPixmap::fromImage(*NewImage));
    im->setData(0,QVariant(QString("image")));
    im->setZValue(200);
    im->setPos(0,0);
    delete NewImage;

    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void DlgImageCorrection::s_BrightnessSliderMoved(int Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    CurrentBrush->BrushFileCorrect.Brightness=Value;
    ui->BrightnessSlider->setValue(CurrentBrush->BrushFileCorrect.Brightness);
    ui->BrightnessValue->setValue(CurrentBrush->BrushFileCorrect.Brightness);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_ContrastSliderMoved(int Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    CurrentBrush->BrushFileCorrect.Contrast=Value;
    ui->ContrastSlider->setValue(CurrentBrush->BrushFileCorrect.Contrast);
    ui->ContrastValue->setValue(CurrentBrush->BrushFileCorrect.Contrast);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_GammaSliderMoved(int Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    CurrentBrush->BrushFileCorrect.Gamma=double(Value)/100;
    ui->GammaSlider->setValue(CurrentBrush->BrushFileCorrect.Gamma*100);
    ui->GammaValue->setValue(CurrentBrush->BrushFileCorrect.Gamma);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_GammaValueED(double Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    CurrentBrush->BrushFileCorrect.Gamma=Value;
    ui->GammaSlider->setValue(CurrentBrush->BrushFileCorrect.Gamma*100);
    ui->GammaValue->setValue(CurrentBrush->BrushFileCorrect.Gamma);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_RedSliderMoved(int Value) {
    CurrentBrush->BrushFileCorrect.Red=Value;
    ui->RedSlider->setValue(CurrentBrush->BrushFileCorrect.Red);
    ui->RedValue->setValue(CurrentBrush->BrushFileCorrect.Red);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_GreenSliderMoved(int Value) {
    CurrentBrush->BrushFileCorrect.Green=Value;
    ui->GreenSlider->setValue(CurrentBrush->BrushFileCorrect.Green);
    ui->GreenValue->setValue(CurrentBrush->BrushFileCorrect.Green);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_BlueSliderMoved(int Value) {
    CurrentBrush->BrushFileCorrect.Blue=Value;
    ui->BlueSlider->setValue(CurrentBrush->BrushFileCorrect.Blue);
    ui->BlueValue->setValue(CurrentBrush->BrushFileCorrect.Blue);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_LockGeometryCB(int Value) {
    switch (Value) {
        case 0 :
            CurrentBrush->BrushFileCorrect.LockGeometry=false;
            break;
        case 1 :
            CurrentBrush->BrushFileCorrect.LockGeometry=true;
            break;
        case 2 :
            CurrentBrush->BrushFileCorrect.LockGeometry=true;
            CurrentBrush->BrushFileCorrect.AspectRatio =ProjectGeometry;
            break;
        case 3 :
            CurrentBrush->BrushFileCorrect.LockGeometry=true;
            CurrentBrush->BrushFileCorrect.AspectRatio =ImageGeometry;
            break;
    }
    if (cadre!=NULL) {
        cadre->KeepAspectRatio=CurrentBrush->BrushFileCorrect.LockGeometry;
        RefreshControls();
    }
}

//====================================================================================================================
// Handler for style sheet management
//====================================================================================================================

void DlgImageCorrection::s_FramingStyleBT() {
    if (!CurrentTextItem) return;
    QString ActualStyle=CurrentTextItem->GetFramingStyle();
    QString Item=GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.PopupCollectionMenu(this,ActualStyle);
    ui->FramingStyleBT->setDown(false);
    if (Item!="") {
        QStringList List;
        GlobalMainWindow->ApplicationConfig->StyleImageFramingCollection.StringToStringList(Item,List);
        for (int i=0;i<List.count();i++) {
            if      (List[i].startsWith("X:"))              CurrentBrush->BrushFileCorrect.X             =List[i].mid(QString("X:").length()).toDouble();
            else if (List[i].startsWith("Y:"))              CurrentBrush->BrushFileCorrect.Y             =List[i].mid(QString("Y:").length()).toDouble();
            else if (List[i].startsWith("ZoomFactor:"))     CurrentBrush->BrushFileCorrect.ZoomFactor    =List[i].mid(QString("ZoomFactor:").length()).toDouble();
            else if (List[i].startsWith("LockGeometry:"))   CurrentBrush->BrushFileCorrect.LockGeometry  =List[i].mid(QString("LockGeometry:").length()).toInt()==1;
            else if (List[i].startsWith("AspectRatio:"))    CurrentBrush->BrushFileCorrect.AspectRatio   =List[i].mid(QString("AspectRatio:").length()).toDouble();
        }
    }
    RefreshControls();
}
