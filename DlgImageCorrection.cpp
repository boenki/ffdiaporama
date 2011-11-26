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

DlgImageCorrection::DlgImageCorrection(cCompositionObject *TheCurrentTextItem,int TheBackgroundForm,cBrushDefinition *TheCurrentBrush,cFilterCorrectObject *TheBrushFileCorrect,int TheVideoPosition,QWidget *parent):QDialog(parent),ui(new Ui::DlgImageCorrection) {
    ui->setupUi(this);
    BackgroundForm  =TheBackgroundForm;
    CurrentTextItem =TheCurrentTextItem;
    CurrentBrush    =TheCurrentBrush;
    BrushFileCorrect=TheBrushFileCorrect;
    VideoPosition   =TheVideoPosition;
    UndoReloadImage =false;

#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
#else
    setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
#endif

    if (CurrentBrush->Image) {
        CachedImage=CurrentBrush->Image->ImageAt(true,true,NULL,CurrentBrush->BrushFileCorrect.Smoothing);
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
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT",QFileInfo(GlobalMainWindow->Diaporama->ProjectFileName).absolutePath(),true);  // Save object
    UndoSlide->appendChild(root);                           // Add object to xml document

    UndoShot=new QDomDocument(APPLICATION_NAME);
    root=UndoShot->createElement("UNDO-DLG");               // Create xml document and root
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->SaveToXML(root,"UNDO-DLG-OBJECT",QFileInfo(GlobalMainWindow->Diaporama->ProjectFileName).absolutePath());  // Save object
    UndoShot->appendChild(root);                           // Add object to xml document

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
    if (root.tagName()=="UNDO-DLG") {
        QStringList AliasList;
        CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","",AliasList);
    }

    root=UndoShot->documentElement();
    if (root.tagName()=="UNDO-DLG") {
        cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
        Filter->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    }
    if (UndoReloadImage) {
        QStringList AliasList;
        if (CurrentBrush->Image)            CurrentBrush->Image->GetInformationFromFile(UndoBrushFileName,AliasList);
            else if (CurrentBrush->Video)   CurrentBrush->Video->GetInformationFromFile(UndoBrushFileName,false,AliasList);
        delete CachedImage;
        CachedImage=NULL;
        if (CurrentBrush->Image) CachedImage=CurrentBrush->Image->ImageAt(true,true,NULL,CurrentBrush->BrushFileCorrect.Smoothing);
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
    if (FLAGSTOPED || (BrushFileCorrect==NULL)) return;
    BrushFileCorrect->X=Value/100;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_YValueEDChanged(double Value) {
    if (FLAGSTOPED || (BrushFileCorrect==NULL)) return;
    BrushFileCorrect->Y=Value/100;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_WValueEDChanged(double Value) {
    if (FLAGSTOPED || (BrushFileCorrect==NULL)) return;
    if (BrushFileCorrect->LockGeometry) {
        BrushFileCorrect->ZoomFactor=Value/100;
    } else {
        double newH=BrushFileCorrect->ZoomFactor*BrushFileCorrect->AspectRatio*ymax;
        BrushFileCorrect->ZoomFactor=Value/100;
        double newW=BrushFileCorrect->ZoomFactor*xmax;
        BrushFileCorrect->AspectRatio=newH/newW;
    }
    // Resize and repos all item in the scene
    for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
        cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
        RectItem->AspectRatio=BrushFileCorrect->AspectRatio;
        RectItem->setPos(BrushFileCorrect->X*xmax,BrushFileCorrect->Y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(BrushFileCorrect->X*xmax,
                                                      BrushFileCorrect->Y*ymax,
                                                      xmax*BrushFileCorrect->ZoomFactor,
                                                      xmax*BrushFileCorrect->ZoomFactor*BrushFileCorrect->AspectRatio));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_HValueEDChanged(double Value) {
    if (FLAGSTOPED || (BrushFileCorrect==NULL)) return;
    double newH=(Value/100)*ymax;
    if (BrushFileCorrect->LockGeometry) {
        double newW=newH/BrushFileCorrect->AspectRatio;
        BrushFileCorrect->ZoomFactor=newW/xmax;
    } else {
        double newW=BrushFileCorrect->ZoomFactor*xmax;
        BrushFileCorrect->AspectRatio=newH/newW;
    }
    // Resize and repos all item in the scene
    for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
        cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
        RectItem->AspectRatio=BrushFileCorrect->AspectRatio;
        RectItem->setPos(BrushFileCorrect->X*xmax,BrushFileCorrect->Y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(BrushFileCorrect->X*xmax,
                                                      BrushFileCorrect->Y*ymax,
                                                      xmax*BrushFileCorrect->ZoomFactor,
                                                      xmax*BrushFileCorrect->ZoomFactor*BrushFileCorrect->AspectRatio));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotationEDChanged(double Value) {
    if (FLAGSTOPED || (BrushFileCorrect==NULL)) return;
    if (Value<-180) Value=360-Value;
    if (Value>180)  Value=-360-Value;
    BrushFileCorrect->ImageRotation=Value;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotateLeft() {
    if (BrushFileCorrect==NULL) return;
    double Value=(int((BrushFileCorrect->ImageRotation-90)/90)*90);
    if (Value<=-180) Value=360-Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_RotateRight() {
    if (BrushFileCorrect==NULL) return;
    double Value=(int((BrushFileCorrect->ImageRotation+90)/90)*90);
    if (Value>180) Value=-360+Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustW() {
    if (BrushFileCorrect==NULL) return;
    double W=MagneticRuler.MagnetX2-MagneticRuler.MagnetX1;
    double H=W*BrushFileCorrect->AspectRatio;
    BrushFileCorrect->X=((xmax-W)/2)/xmax;
    BrushFileCorrect->Y=((ymax-H)/2)/ymax;
    BrushFileCorrect->ZoomFactor=W/xmax;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustH() {
    if (BrushFileCorrect==NULL) return;
    double H=MagneticRuler.MagnetY2-MagneticRuler.MagnetY1;
    double W=H/BrushFileCorrect->AspectRatio;
    BrushFileCorrect->X=((xmax-W)/2)/xmax;
    BrushFileCorrect->Y=((ymax-H)/2)/ymax;
    BrushFileCorrect->ZoomFactor=W/xmax;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustWH() {
    if (BrushFileCorrect==NULL) return;
    // Special case for custom geometry -> use all the image then change aspect ratio to image aspect ratio
    if (!BrushFileCorrect->LockGeometry) {
        double W=MagneticRuler.MagnetX2-MagneticRuler.MagnetX1;
        double H=MagneticRuler.MagnetY2-MagneticRuler.MagnetY1;
        BrushFileCorrect->AspectRatio=H/W;
        BrushFileCorrect->X=((xmax-W)/2)/xmax;
        BrushFileCorrect->Y=((ymax-H)/2)/ymax;
        BrushFileCorrect->ZoomFactor=W/xmax;
        for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
            cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
            RectItem->AspectRatio=BrushFileCorrect->AspectRatio;
        }
        RefreshControls();
    } else {
        double W=MagneticRuler.MagnetX2-MagneticRuler.MagnetX1;
        double H=W*BrushFileCorrect->AspectRatio;
        if (H<MagneticRuler.MagnetY2-MagneticRuler.MagnetY1) s_AdjustH(); else s_AdjustW();
    }
}

//====================================================================================================================

void DlgImageCorrection::RefreshControls() {
    if ((BrushFileCorrect==NULL)||(!scene)||(FLAGSTOPED)) return;
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

    ui->XValue->setValue(BrushFileCorrect->X*100);
    ui->YValue->setValue(BrushFileCorrect->Y*100);
    ui->WValue->setValue(BrushFileCorrect->ZoomFactor*100);
    ui->HValue->setValue(BrushFileCorrect->ZoomFactor*BrushFileCorrect->AspectRatio*100);

    if (!BrushFileCorrect->LockGeometry)                        ui->LockGeometryCB->setCurrentIndex(0);
    else if (BrushFileCorrect->AspectRatio==ProjectGeometry)    ui->LockGeometryCB->setCurrentIndex(2);
    else if (BrushFileCorrect->AspectRatio==ImageGeometry)      ui->LockGeometryCB->setCurrentIndex(3);
    else                                                        ui->LockGeometryCB->setCurrentIndex(1);

    ui->RotateED->setValue(BrushFileCorrect->ImageRotation);

    // FilterCorrection
    ui->BrightnessSlider->setValue(BrushFileCorrect->Brightness);     ui->BrightnessValue->setValue(BrushFileCorrect->Brightness);
    ui->ContrastSlider->setValue(BrushFileCorrect->Contrast);         ui->ContrastValue->setValue(BrushFileCorrect->Contrast);
    ui->GammaSlider->setValue(BrushFileCorrect->Gamma*100);           ui->GammaValue->setValue(BrushFileCorrect->Gamma);
    ui->RedSlider->setValue(BrushFileCorrect->Red);                   ui->RedValue->setValue(BrushFileCorrect->Red);
    ui->GreenSlider->setValue(BrushFileCorrect->Green);               ui->GreenValue->setValue(BrushFileCorrect->Green);
    ui->BlueSlider->setValue(BrushFileCorrect->Blue);                 ui->BlueValue->setValue(BrushFileCorrect->Blue);

    ui->BlurSigmaSlider->setValue((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurSigma*10);
    ui->BlurSigmaSB->setValue((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurSigma);
    ui->BlurRadiusSlider->setValue(int((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurRadius));
    ui->BlurRadiusED->setValue(int((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurRadius));

    // Resize and repos all item in the scene
    for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
        cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];

        // Set aspect ratio from Brush to Rect if geometrie is not custom or from rect to brush if geometrie is custom
        if (BrushFileCorrect->LockGeometry)
            RectItem->AspectRatio=BrushFileCorrect->AspectRatio;
            else BrushFileCorrect->AspectRatio=RectItem->AspectRatio;

        RectItem->setPos(BrushFileCorrect->X*xmax,BrushFileCorrect->Y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(BrushFileCorrect->X*xmax,
                                                      BrushFileCorrect->Y*ymax,
                                                      xmax*BrushFileCorrect->ZoomFactor,
                                                      xmax*BrushFileCorrect->ZoomFactor*BrushFileCorrect->AspectRatio));
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
                                                 GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(CurrentBrush->Image?cApplicationConfig::IMAGEFILE:cApplicationConfig::VIDEOFILE));
    QApplication::processEvents();
    if (NewFile=="") return;
    if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use
    QString BrushFileName=QFileInfo(NewFile).absoluteFilePath();

    QStringList AliasList;
    if (CurrentBrush->Image)            CurrentBrush->Image->GetInformationFromFile(BrushFileName,AliasList);
        else if (CurrentBrush->Video)   CurrentBrush->Video->GetInformationFromFile(BrushFileName,false,AliasList);

    delete CachedImage;
    CachedImage=NULL;
    if (CurrentBrush->Image) CachedImage=CurrentBrush->Image->ImageAt(true,true,NULL,CurrentBrush->BrushFileCorrect.Smoothing);
        else if (CurrentBrush->Video) CachedImage=CurrentBrush->Video->ImageAt(true,VideoPosition,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),true,NULL,1,false,NULL,false);

    UndoReloadImage=true;

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
    if ((!BrushFileCorrect)||(!scene)) return;

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
    if (cadre==NULL) cadre=new cCustomGraphicsRectItem(scene,300,&BrushFileCorrect->X,&BrushFileCorrect->Y,&BrushFileCorrect->ZoomFactor,
                                                       NULL,NULL,xmax,ymax,BrushFileCorrect->LockGeometry,
                                                       BrushFileCorrect->AspectRatio,&MagneticRuler,this,TYPE_DlgImageCorrection,0,true);

    // Prepare CacheImage
    QImage   *NewImage=new QImage(xmax,ymax,QImage::Format_ARGB32_Premultiplied);
    QImage   *SourceImage=NULL;
    QPainter P;

    double SizeRatio=double(xmax)/double(ui->GraphicsView->width());
    int    WithPen  =int(SizeRatio); if (double(WithPen)<SizeRatio) WithPen++;

    // Rotate image if needed and create a SourceImage
    if (BrushFileCorrect->ImageRotation!=0) {
        QTransform matrix;
        matrix.rotate(BrushFileCorrect->ImageRotation,Qt::ZAxis);
        SourceImage=new QImage(CachedImage->transformed(matrix,BrushFileCorrect->Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));

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
    BrushFileCorrect->ApplyFilter(&ToUseImage);

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
    double X=BrushFileCorrect->X*xmax;
    double Y=BrushFileCorrect->Y*ymax;
    double W=BrushFileCorrect->ZoomFactor*xmax;
    double H=W*BrushFileCorrect->AspectRatio;

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
    BrushFileCorrect->Brightness=Value;
    ui->BrightnessSlider->setValue(BrushFileCorrect->Brightness);
    ui->BrightnessValue->setValue(BrushFileCorrect->Brightness);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_ContrastSliderMoved(int Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    BrushFileCorrect->Contrast=Value;
    ui->ContrastSlider->setValue(BrushFileCorrect->Contrast);
    ui->ContrastValue->setValue(BrushFileCorrect->Contrast);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_GammaSliderMoved(int Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    BrushFileCorrect->Gamma=double(Value)/100;
    ui->GammaSlider->setValue(BrushFileCorrect->Gamma*100);
    ui->GammaValue->setValue(BrushFileCorrect->Gamma);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_GammaValueED(double Value) {
    if (FLAGSTOPSPIN) return;
    FLAGSTOPSPIN=true;
    BrushFileCorrect->Gamma=Value;
    ui->GammaSlider->setValue(BrushFileCorrect->Gamma*100);
    ui->GammaValue->setValue(BrushFileCorrect->Gamma);
    RefreshBackgroundImage();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_RedSliderMoved(int Value) {
    BrushFileCorrect->Red=Value;
    ui->RedSlider->setValue(BrushFileCorrect->Red);
    ui->RedValue->setValue(BrushFileCorrect->Red);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_GreenSliderMoved(int Value) {
    BrushFileCorrect->Green=Value;
    ui->GreenSlider->setValue(BrushFileCorrect->Green);
    ui->GreenValue->setValue(BrushFileCorrect->Green);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_BlueSliderMoved(int Value) {
    BrushFileCorrect->Blue=Value;
    ui->BlueSlider->setValue(BrushFileCorrect->Blue);
    ui->BlueValue->setValue(BrushFileCorrect->Blue);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_LockGeometryCB(int Value) {
    switch (Value) {
        case 0 :
            BrushFileCorrect->LockGeometry=false;
            break;
        case 1 :
            BrushFileCorrect->LockGeometry=true;
            break;
        case 2 :
            BrushFileCorrect->LockGeometry=true;
            BrushFileCorrect->AspectRatio =ProjectGeometry;
            break;
        case 3 :
            BrushFileCorrect->LockGeometry=true;
            BrushFileCorrect->AspectRatio =ImageGeometry;
            break;
    }
    if (cadre!=NULL) {
        cadre->KeepAspectRatio=BrushFileCorrect->LockGeometry;
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
            if      (List[i].startsWith("X:"))              BrushFileCorrect->X             =List[i].mid(QString("X:").length()).toDouble();
            else if (List[i].startsWith("Y:"))              BrushFileCorrect->Y             =List[i].mid(QString("Y:").length()).toDouble();
            else if (List[i].startsWith("ZoomFactor:"))     BrushFileCorrect->ZoomFactor    =List[i].mid(QString("ZoomFactor:").length()).toDouble();
            else if (List[i].startsWith("LockGeometry:"))   BrushFileCorrect->LockGeometry  =List[i].mid(QString("LockGeometry:").length()).toInt()==1;
            else if (List[i].startsWith("AspectRatio:"))    BrushFileCorrect->AspectRatio   =List[i].mid(QString("AspectRatio:").length()).toDouble();
        }
    }
    RefreshControls();
}
