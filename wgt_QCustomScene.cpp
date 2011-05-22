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

#include "wgt_QCustomScene.h"
#include "ui_wgt_QCustomScene.h"
#include "wgt_QCustomScene.h"
#include "mainwindow.h"

//*****************************************************************************************************************************
// Custom widget object
//*****************************************************************************************************************************

wgt_QCustomScene::wgt_QCustomScene(QWidget *parent):QWidget(parent),ui(new Ui::wgt_QCustomScene) {
    ui->setupUi(this);
    IsFirstInitDone = false;                 // true when first show window was done
    FLAGSTOPED      = false;
    CorrectObject   = NULL;
    scene           = NULL;
    cadre           = NULL;
    RulerOn         = new QIcon("icons/ruler_ok.png");
    RulerOff        = new QIcon("icons/ruler_ko.png");

    MagneticRuller.MagnetX1     = -1;
    MagneticRuller.MagnetX2     = -1;
    MagneticRuller.MagnetY1     = -1;
    MagneticRuller.MagnetY2     = -1;
    MagneticRuller.MagneticRuler= true;

    //ui->GraphicsView->setAttribute(Qt::WA_OpaquePaintEvent);

    ui->XValue->setMinimum(0);              ui->XValue->setMaximum(99.99);
    ui->YValue->setMinimum(0);              ui->YValue->setMaximum(99.99);
    ui->ZoomValue->setMinimum(0);           ui->ZoomValue->setMaximum(100);
    ui->RotateED->setMinimum(-180);         ui->RotateED->setMaximum(180);

    connect(ui->RotateED,SIGNAL(valueChanged(int)),this,SLOT(s_RotationEDChanged(int)));
    connect(ui->XValue,SIGNAL(valueChanged(double)),this,SLOT(s_XValueEDChanged(double)));
    connect(ui->YValue,SIGNAL(valueChanged(double)),this,SLOT(s_YValueEDChanged(double)));
    connect(ui->ZoomValue,SIGNAL(valueChanged(double)),this,SLOT(s_ZoomValueEDChanged(double)));
    connect(ui->RotateLeftBT,SIGNAL(clicked()),this,SLOT(s_RotateLeft()));
    connect(ui->RotateRightBT,SIGNAL(clicked()),this,SLOT(s_RotateRight()));
    connect(ui->AdjustHBT,SIGNAL(clicked()),this,SLOT(s_AdjustH()));
    connect(ui->AdjustWBT,SIGNAL(clicked()),this,SLOT(s_AdjustW()));
    connect(ui->AdjustWHBT,SIGNAL(clicked()),this,SLOT(s_AdjustWH()));
    connect(ui->MagneticEdgeBt,SIGNAL(clicked()),this,SLOT(s_MagneticEdgeBt()));

    connect(ui->BrightnessSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_BrightnessSliderMoved(int)));
    connect(ui->BrightnessValue,SIGNAL(valueChanged(int)),this,SLOT(s_BrightnessSliderMoved(int)));
    connect(ui->ContrastSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_ContrastSliderMoved(int)));
    connect(ui->ContrastValue,SIGNAL(valueChanged(int)),this,SLOT(s_ContrastSliderMoved(int)));
    connect(ui->GammaSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_GammaSliderMoved(int)));
    connect(ui->GammaValue,SIGNAL(valueChanged(double)),this,SLOT(s_GammaValueED(double)));
    connect(ui->RedSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_RedSliderMoved(int)));
    connect(ui->RedValue,SIGNAL(valueChanged(int)),this,SLOT(s_RedSliderMoved(int)));
    connect(ui->GreenSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_GreenSliderMoved(int)));
    connect(ui->GreenValue,SIGNAL(valueChanged(int)),this,SLOT(s_GreenSliderMoved(int)));
    connect(ui->BlueSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_BlueSliderMoved(int)));
    connect(ui->BlueValue,SIGNAL(valueChanged(int)),this,SLOT(s_BlueSliderMoved(int)));
}

//====================================================================================================================

wgt_QCustomScene::~wgt_QCustomScene() {
    delete ui;  // Deleting this make deletion of scene and all included object
    scene=NULL;
    if (RulerOn) {
        delete RulerOn;
        RulerOn=NULL;
    }
    if (RulerOff) {
        delete RulerOff;
        RulerOff=NULL;
    }
}

//====================================================================================================================

void wgt_QCustomScene::showEvent(QShowEvent *) {
    if (!IsFirstInitDone) {
        IsFirstInitDone=true;                   // Set this flag to true to indicate that now we can prepeare display
        SetDiaporamaShot(CorrectObject);        // Make a new object init process
    }
}

//====================================================================================================================

void wgt_QCustomScene::resizeEvent(QResizeEvent *) {
    if (IsFirstInitDone) SetDiaporamaShot(CorrectObject);        // Make a new object init process
}

//====================================================================================================================

void wgt_QCustomScene::SetDiaporamaShot(cFilterCorrectObject *TheCorrectObject) {
    CorrectObject=TheCorrectObject;
    if (cadre!=NULL) {
        scene->removeItem(cadre);
        delete cadre;
        cadre=NULL;
    }
    if (scene!=NULL) {
        delete scene;
        scene=NULL;
    }

    // Calc and adjust ui->GraphicsView depending on geometry
    xmax=ui->GraphicsView->width();
    ymax=GlobalMainWindow->Diaporama->GetHeightForWidth(xmax);
    if (ymax<ui->GraphicsView->height()) {
        ymax=ui->GraphicsView->height();
        xmax=GlobalMainWindow->Diaporama->GetWidthForHeight(ymax);
    }
    // Create the scene
    scene = new QGraphicsScene();
    scene->setSceneRect(QRectF(0,0,xmax,ymax));

    // Setup scene to control
    ui->GraphicsView->setScene(scene);
    ui->GraphicsView->setInteractive(true);
    ui->GraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);
    RefreshWidget();
    RefreshControls();
}

//====================================================================================================================

void wgt_QCustomScene::RefreshWidget() {
    if (scene!=NULL) {
        // Resize et repos all item in the scene
        for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
            cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
            RectItem->setPos(CorrectObject->X*xmax,CorrectObject->Y*ymax);
            QRectF Rect=RectItem->mapRectFromScene(QRectF(CorrectObject->X*xmax,CorrectObject->Y*ymax,xmax*CorrectObject->ZoomFactor,ymax*CorrectObject->ZoomFactor));
            RectItem->setRect(Rect);
            RectItem->RecalcEmbededResizeRectItem();
        }
        // Refresh image
        RefreshBackgroundImage();
    }
}

//====================================================================================================================

void wgt_QCustomScene::s_XValueEDChanged(double Value) {
    if (FLAGSTOPED || (CorrectObject==NULL)) return;
    CorrectObject->X=Value/100;
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QCustomScene::s_YValueEDChanged(double Value) {
    if (FLAGSTOPED || (CorrectObject==NULL)) return;
    CorrectObject->Y=Value/100;
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QCustomScene::s_ZoomValueEDChanged(double Value) {
    if (FLAGSTOPED || (CorrectObject==NULL)) return;
    CorrectObject->ZoomFactor=Value/100;
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QCustomScene::s_RotationEDChanged(int Value) {
    if (FLAGSTOPED || (CorrectObject==NULL)) return;
    if (Value<-180) Value=360-Value;
    if (Value>180)  Value=-360-Value;
    CorrectObject->ImageRotation=Value;
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QCustomScene::s_RotateLeft() {
    if (CorrectObject==NULL) return;
    int Value=(((CorrectObject->ImageRotation-90)/90)*90);
    if (Value<=-180) Value=360-Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void wgt_QCustomScene::s_RotateRight() {
    if (CorrectObject==NULL) return;
    int Value=(((CorrectObject->ImageRotation+90)/90)*90);
    if (Value>180) Value=-360+Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void wgt_QCustomScene::s_AdjustW() {
    if (CorrectObject==NULL) return;
    CorrectObject->X=MagneticRuller.MagnetX1/xmax;
    CorrectObject->ZoomFactor=(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1)/xmax;
    CorrectObject->Y=((ymax-GlobalMainWindow->Diaporama->GetHeightForWidth(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1))/2)/ymax;
    RefreshWidget();
}

//====================================================================================================================

void wgt_QCustomScene::s_AdjustH() {
    if (CorrectObject==NULL) return;
    CorrectObject->Y=MagneticRuller.MagnetY1/ymax;
    CorrectObject->ZoomFactor=(MagneticRuller.MagnetY2-MagneticRuller.MagnetY1)/ymax;
    CorrectObject->X=((xmax-GlobalMainWindow->Diaporama->GetWidthForHeight(MagneticRuller.MagnetY2-MagneticRuller.MagnetY1))/2)/xmax;
    RefreshWidget();
}

//====================================================================================================================

void wgt_QCustomScene::s_AdjustWH() {
    if (CorrectObject==NULL) return;
    CorrectObject->X=MagneticRuller.MagnetX1/xmax;
    CorrectObject->ZoomFactor=(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1)/xmax;
    CorrectObject->Y=((ymax-GlobalMainWindow->Diaporama->GetHeightForWidth(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1))/2)/ymax;
    if (CorrectObject->Y>MagneticRuller.MagnetY1/ymax) s_AdjustH(); else RefreshWidget();
}

//====================================================================================================================

void wgt_QCustomScene::RefreshControls() {
    if (CorrectObject==NULL) return;
    FLAGSTOPED=true;
    ui->XValue->setRange(0,100-CorrectObject->ZoomFactor*100);
    ui->XValue->setSingleStep(double(100)/20);
    ui->XValue->setValue(CorrectObject->X*100);
    ui->YValue->setRange(0,100-CorrectObject->ZoomFactor*100);
    ui->YValue->setSingleStep(double(100)/20);
    ui->YValue->setValue(CorrectObject->Y*100);
    ui->ZoomValue->setRange(1,100);
    ui->ZoomValue->setSingleStep(double(100)/20);
    ui->ZoomValue->setValue(CorrectObject->ZoomFactor*100);

    ui->RotateED->setValue(CorrectObject->ImageRotation);

    // FilterCorrection
    ui->BrightnessSlider->setValue(CorrectObject->Brightness);     ui->BrightnessValue->setValue(CorrectObject->Brightness);
    ui->ContrastSlider->setValue(CorrectObject->Contrast);         ui->ContrastValue->setValue(CorrectObject->Contrast);
    ui->GammaSlider->setValue(CorrectObject->Gamma*100);           ui->GammaValue->setValue(CorrectObject->Gamma);
    ui->RedSlider->setValue(CorrectObject->Red);                   ui->RedValue->setValue(CorrectObject->Red);
    ui->GreenSlider->setValue(CorrectObject->Green);               ui->GreenValue->setValue(CorrectObject->Green);
    ui->BlueSlider->setValue(CorrectObject->Blue);                 ui->BlueValue->setValue(CorrectObject->Blue);

    FLAGSTOPED=false;
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QCustomScene::s_MagneticEdgeBt() {
    if (MagneticRuller.MagneticRuler==true) MagneticRuller.MagneticRuler=false; else MagneticRuller.MagneticRuler=true;
    ui->MagneticEdgeBt->setIcon(*(MagneticRuller.MagneticRuler?RulerOn:RulerOff));
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QCustomScene::RefreshBackgroundImage() {
    if (CorrectObject==NULL) return;

    // Ensure scene is valide
    if (scene==NULL) return;

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
    if (cadre==NULL) {
        xmax=ui->GraphicsView->width();
        ymax=GlobalMainWindow->Diaporama->GetHeightForWidth(xmax);
        if (ymax<ui->GraphicsView->height()) {
            ymax=ui->GraphicsView->height();
            xmax=GlobalMainWindow->Diaporama->GetWidthForHeight(ymax);
        }
        CorrectObject->AspectRatio=ymax/xmax;
        cadre=new cCustomGraphicsRectItem(scene,300,&CorrectObject->X,&CorrectObject->Y,&CorrectObject->ZoomFactor,NULL,NULL,xmax,ymax,true,CorrectObject->AspectRatio,&MagneticRuller,this,TYPE_wgt_QCustomScene);
    }

    // Prepare CacheImage
    QRectF  ImagePosition;
    QImage *NewImage=GlobalMainWindow->Diaporama->List[GlobalMainWindow->Diaporama->CurrentCol].CanvasImageAt(xmax,ymax,0,NULL,0,0,&ImagePosition,&CorrectObject->ImageRotation,false,true,false,NULL);
    // Calc magnetic ruller guides positions
    MagneticRuller.MagnetX1=ImagePosition.x();
    MagneticRuller.MagnetX2=ImagePosition.x()+ImagePosition.width();
    MagneticRuller.MagnetY1=ImagePosition.y();
    MagneticRuller.MagnetY2=ImagePosition.y()+ImagePosition.height();

    // Draw selection rectangle for cadre

    // draw rect out of the rectangle
    QPainter P;
    P.begin(NewImage);
    P.setPen(Qt::NoPen);
    P.setBrush(QBrush(0x555555));
    P.setOpacity(0.75);
    P.drawRect(QRectF(0,0,xmax,CorrectObject->Y*ymax));
    P.drawRect(QRectF(0,CorrectObject->Y*ymax,CorrectObject->X*xmax,CorrectObject->ZoomFactor*ymax));
    P.drawRect(QRectF(CorrectObject->X*xmax+CorrectObject->ZoomFactor*xmax,CorrectObject->Y*ymax,xmax,CorrectObject->ZoomFactor*ymax));
    P.drawRect(QRectF(0,CorrectObject->Y*ymax+CorrectObject->ZoomFactor*ymax,xmax,ymax));
    P.setOpacity(1);
    P.setBrush(Qt::NoBrush);

    // draw guides
    P.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    if (MagneticRuller.MagneticRuler==true) {
        QColor col=QColor(255,0,0);
        QPen   pen=QPen(col);
        pen.setWidth(2);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setStyle(Qt::DotLine);
        P.setPen(pen);
        if (MagneticRuller.MagnetX1!=-1) P.drawLine(MagneticRuller.MagnetX1,0,MagneticRuller.MagnetX1,ymax);
        if (MagneticRuller.MagnetX2!=-1) P.drawLine(MagneticRuller.MagnetX2,0,MagneticRuller.MagnetX2,ymax);
        if (MagneticRuller.MagnetY1!=-1) P.drawLine(0,MagneticRuller.MagnetY1,xmax,MagneticRuller.MagnetY1);
        if (MagneticRuller.MagnetY2!=-1) P.drawLine(0,MagneticRuller.MagnetY2,xmax,MagneticRuller.MagnetY2);
    }
    P.setCompositionMode(QPainter::CompositionMode_SourceOver);
    P.end();

    // Add image to the background of the scene
    QGraphicsPixmapItem *im=scene->addPixmap(QPixmap::fromImage(*NewImage));    // Image will be delete by scene
    im->setData(0,QVariant(QString("image")));
    im->setZValue(200);
    im->setPos(0,0);
    delete NewImage;

    RefreshControls();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void wgt_QCustomScene::s_BrightnessSliderMoved(int Value) {
    CorrectObject->Brightness=Value;
    ui->BrightnessSlider->setValue(CorrectObject->Brightness);
    ui->BrightnessValue->setValue(CorrectObject->Brightness);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QCustomScene::s_ContrastSliderMoved(int Value) {
    CorrectObject->Contrast=Value;
    ui->ContrastSlider->setValue(CorrectObject->Contrast);
    ui->ContrastValue->setValue(CorrectObject->Contrast);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QCustomScene::s_GammaSliderMoved(int Value) {
    CorrectObject->Gamma=double(Value)/100;
    ui->GammaSlider->setValue(CorrectObject->Gamma*100);
    ui->GammaValue->setValue(CorrectObject->Gamma);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QCustomScene::s_GammaValueED(double Value) {
    CorrectObject->Gamma=Value;
    ui->GammaSlider->setValue(CorrectObject->Gamma*100);
    ui->GammaValue->setValue(CorrectObject->Gamma);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QCustomScene::s_RedSliderMoved(int Value) {
    CorrectObject->Red=Value;
    ui->RedSlider->setValue(CorrectObject->Red);
    ui->RedValue->setValue(CorrectObject->Red);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QCustomScene::s_GreenSliderMoved(int Value) {
    CorrectObject->Green=Value;
    ui->GreenSlider->setValue(CorrectObject->Green);
    ui->GreenValue->setValue(CorrectObject->Green);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QCustomScene::s_BlueSliderMoved(int Value) {
    CorrectObject->Blue=Value;
    ui->BlueSlider->setValue(CorrectObject->Blue);
    ui->BlueValue->setValue(CorrectObject->Blue);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}
