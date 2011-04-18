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
#include "cdiaporama.h"

//*****************************************************************************************************************************
// Custom widget object
//*****************************************************************************************************************************

wgt_QCustomScene::wgt_QCustomScene(QWidget *parent):QWidget(parent),ui(new Ui::wgt_QCustomScene) {
    ui->setupUi(this);
    FLAGSTOPED      = false;
    DiaporamaScene  = NULL;
    scene           = NULL;
    cadre           = NULL;
    CacheImage      = NULL;
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
}

//====================================================================================================================

wgt_QCustomScene::~wgt_QCustomScene() {
    delete ui;  // Deleting this make deletion of scene and all included object
    scene=NULL;
    if (CacheImage!=NULL) {
        delete CacheImage;
        CacheImage=NULL;
    }
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

void wgt_QCustomScene::SetDiaporamaScene(cDiaporamaShot *DiaporamaScene) {
    if (cadre!=NULL) {
        scene->removeItem(cadre);
        delete cadre;
        cadre=NULL;
    }
    if (CacheImage!=NULL) {
        delete CacheImage;
        CacheImage=NULL;
    }
    this->DiaporamaScene=DiaporamaScene;
}

//====================================================================================================================

void wgt_QCustomScene::resizeEvent(QResizeEvent *) {
    if (scene!=NULL) {
        // Calc and adjust ui->GraphicsView depending on geometry
        xmax=ui->GraphicsView->width();
        ymax=DiaporamaScene->Parent->Parent->GetHeightForWidth(xmax);
        if (ymax<ui->GraphicsView->height()) {
            ymax=ui->GraphicsView->height();
            xmax=DiaporamaScene->Parent->Parent->GetWidthForHeight(ymax);
        }
        // Create the scene
        scene->setSceneRect(QRectF(0,0,xmax,ymax));
        ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);
        RefreshWidget();
    }
}

//====================================================================================================================

void wgt_QCustomScene::RefreshWidget() {
    if (scene!=NULL) {
        // Resize et repos all item in the scene
        for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
            cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
            RectItem->setPos(DiaporamaScene->X*xmax,DiaporamaScene->Y*ymax);
            QRectF Rect=RectItem->mapRectFromScene(QRectF(DiaporamaScene->X*xmax,DiaporamaScene->Y*ymax,xmax*DiaporamaScene->ZoomFactor,ymax*DiaporamaScene->ZoomFactor));
            RectItem->setRect(Rect);
            RectItem->RecalcEmbededResizeRectItem();
        }
        // Refresh image
        RefreshBackgroundImage(true);
    }
}

//====================================================================================================================

void wgt_QCustomScene::showEvent(QShowEvent *) {
    if (scene==NULL) {
        //--------------------------------------------------------------------
        // Prepare the scene
        //--------------------------------------------------------------------
        // Calc and adjust ui->GraphicsView depending on geometry
        xmax=ui->GraphicsView->width();
        ymax=DiaporamaScene->Parent->Parent->GetHeightForWidth(xmax);
        if (ymax<ui->GraphicsView->height()) {
            ymax=ui->GraphicsView->height();
            xmax=DiaporamaScene->Parent->Parent->GetWidthForHeight(ymax);
        }
        // Create the scene
        scene = new QGraphicsScene();
        scene->setSceneRect(QRectF(0,0,xmax,ymax));

        // Setup scene to control
        ui->GraphicsView->setScene(scene);
        ui->GraphicsView->setInteractive(true);
        ui->GraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);
    } else RefreshWidget();
}

//====================================================================================================================

void wgt_QCustomScene::s_XValueEDChanged(double Value) {
    if (FLAGSTOPED || (DiaporamaScene==NULL)) return;
    DiaporamaScene->X=Value/100;
    RefreshBackgroundImage(true);
}

//====================================================================================================================

void wgt_QCustomScene::s_YValueEDChanged(double Value) {
    if (FLAGSTOPED || (DiaporamaScene==NULL)) return;
    DiaporamaScene->Y=Value/100;
    RefreshBackgroundImage(true);
}

//====================================================================================================================

void wgt_QCustomScene::s_ZoomValueEDChanged(double Value) {
    if (FLAGSTOPED || (DiaporamaScene==NULL)) return;
    DiaporamaScene->ZoomFactor=Value/100;
    RefreshBackgroundImage(true);
}

//====================================================================================================================

void wgt_QCustomScene::s_RotationEDChanged(int Value) {
    if (FLAGSTOPED || (DiaporamaScene==NULL)) return;
    if (Value<-180) Value=360-Value;
    if (Value>180)  Value=-360-Value;
    DiaporamaScene->ImageRotation=Value;
    RefreshBackgroundImage(true);
}

//====================================================================================================================

void wgt_QCustomScene::s_RotateLeft() {
    if (DiaporamaScene==NULL) return;
    int Value=(((DiaporamaScene->ImageRotation-90)/90)*90);
    if (Value<=-180) Value=360-Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void wgt_QCustomScene::s_RotateRight() {
    if (DiaporamaScene==NULL) return;
    int Value=(((DiaporamaScene->ImageRotation+90)/90)*90);
    if (Value>180) Value=-360+Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void wgt_QCustomScene::s_AdjustW() {
    if (DiaporamaScene==NULL) return;
    DiaporamaScene->X=MagneticRuller.MagnetX1/xmax;
    DiaporamaScene->ZoomFactor=(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1)/xmax;
    DiaporamaScene->Y=((ymax-DiaporamaScene->Parent->Parent->GetHeightForWidth(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1))/2)/ymax;
    RefreshWidget();
}

//====================================================================================================================

void wgt_QCustomScene::s_AdjustH() {
    if (DiaporamaScene==NULL) return;
    DiaporamaScene->Y=MagneticRuller.MagnetY1/ymax;
    DiaporamaScene->ZoomFactor=(MagneticRuller.MagnetY2-MagneticRuller.MagnetY1)/ymax;
    DiaporamaScene->X=((xmax-DiaporamaScene->Parent->Parent->GetWidthForHeight(MagneticRuller.MagnetY2-MagneticRuller.MagnetY1))/2)/xmax;
    RefreshWidget();
}

//====================================================================================================================

void wgt_QCustomScene::s_AdjustWH() {
    if (DiaporamaScene==NULL) return;
    DiaporamaScene->X=MagneticRuller.MagnetX1/xmax;
    DiaporamaScene->ZoomFactor=(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1)/xmax;
    DiaporamaScene->Y=((ymax-DiaporamaScene->Parent->Parent->GetHeightForWidth(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1))/2)/ymax;
    if (DiaporamaScene->Y>MagneticRuller.MagnetY1/ymax) s_AdjustH(); else RefreshWidget();
}

//====================================================================================================================

void wgt_QCustomScene::RefreshControls() {
    if (DiaporamaScene==NULL) return;
    FLAGSTOPED=true;
    ui->XValue->setRange(0,100-DiaporamaScene->ZoomFactor*100);
    ui->XValue->setSingleStep(double(100)/20);
    ui->XValue->setValue(DiaporamaScene->X*100);
    ui->YValue->setRange(0,100-DiaporamaScene->ZoomFactor*100);
    ui->YValue->setSingleStep(double(100)/20);
    ui->YValue->setValue(DiaporamaScene->Y*100);
    ui->ZoomValue->setRange(1,100);
    ui->ZoomValue->setSingleStep(double(100)/20);
    ui->ZoomValue->setValue(DiaporamaScene->ZoomFactor*100);

    ui->RotateED->setValue(DiaporamaScene->ImageRotation);
    FLAGSTOPED=false;
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QCustomScene::s_MagneticEdgeBt() {
    if (MagneticRuller.MagneticRuler==true) MagneticRuller.MagneticRuler=false; else MagneticRuller.MagneticRuler=true;
    ui->MagneticEdgeBt->setIcon(*(MagneticRuller.MagneticRuler?RulerOn:RulerOff));
    RefreshBackgroundImage(false);
}

//====================================================================================================================

void wgt_QCustomScene::RefreshBackgroundImage(bool ResetCache) {
    if (DiaporamaScene==NULL) return;

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

    // Ensure CacheImage is OK
    if ((ResetCache==true)&&(CacheImage!=NULL)) {
        delete CacheImage;
        CacheImage=NULL;
    }

    // Create selection box
    if (cadre==NULL) {
        xmax=ui->GraphicsView->width();
        ymax=DiaporamaScene->Parent->Parent->GetHeightForWidth(xmax);
        if (ymax<ui->GraphicsView->height()) {
            ymax=ui->GraphicsView->height();
            xmax=DiaporamaScene->Parent->Parent->GetWidthForHeight(ymax);
        }
        cadre=new cCustomGraphicsRectItem(scene,300,&DiaporamaScene->X,&DiaporamaScene->Y,&DiaporamaScene->ZoomFactor,NULL,NULL,xmax,ymax,true,&MagneticRuller,this,TYPE_wgt_QCustomScene);
    }

    // Prepare CacheImage
    if (CacheImage==NULL) {
        QRectF  ImagePosition;
        QImage *CacheImageImage=DiaporamaScene->Parent->CanvasImageAt(xmax,ymax,0,NULL,0,0,&ImagePosition,&DiaporamaScene->ImageRotation,true,false,true,NULL);
        DiaporamaScene->FilterCorrection.ApplyFilter(CacheImageImage);
        CacheImage=new QPixmap(QPixmap::fromImage(*CacheImageImage));
        delete CacheImageImage;

        // Calc magnetic ruller guides positions
        MagneticRuller.MagnetX1=ImagePosition.x();
        MagneticRuller.MagnetX2=ImagePosition.x()+ImagePosition.width();
        MagneticRuller.MagnetY1=ImagePosition.y();
        MagneticRuller.MagnetY2=ImagePosition.y()+ImagePosition.height();
    }

    // Draw selection rectangle for cadre

    // draw rect out of the rectangle
    QPixmap *NewImage=new QPixmap(CacheImage->copy());
    QPainter P;
    P.begin(NewImage);
    P.setPen(Qt::NoPen);
    P.setBrush(QBrush(0x555555));
    P.setOpacity(0.75);
    P.drawRect(QRectF(0,0,xmax,DiaporamaScene->Y*ymax));
    P.drawRect(QRectF(0,DiaporamaScene->Y*ymax,DiaporamaScene->X*xmax,DiaporamaScene->ZoomFactor*ymax));
    P.drawRect(QRectF(DiaporamaScene->X*xmax+DiaporamaScene->ZoomFactor*xmax,DiaporamaScene->Y*ymax,xmax,DiaporamaScene->ZoomFactor*ymax));
    P.drawRect(QRectF(0,DiaporamaScene->Y*ymax+DiaporamaScene->ZoomFactor*ymax,xmax,ymax));
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
    QGraphicsPixmapItem *im=scene->addPixmap(*NewImage);    // Image will be delete by scene
    im->setData(0,QVariant(QString("image")));
    im->setZValue(200);
    im->setPos(0,0);
//    delete NewImage;
    RefreshControls();
    QApplication::restoreOverrideCursor();
}
