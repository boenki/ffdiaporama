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

#include "wgt_QImageFilterTransform.h"
#include "ui_wgt_QImageFilterTransform.h"

wgt_QImageFilterTransform::wgt_QImageFilterTransform(QWidget *parent) : QWidget(parent),ui(new Ui::wgt_QImageFilterTransform) {
    ui->setupUi(this);
    Filter = NULL;
    scene           = NULL;
    SourceImage     = NULL;
    ZoomLevel       = 1;
    connect(ui->ZoomPlusBt,SIGNAL(clicked()),this,SLOT(s_ZoomPlus()));
    connect(ui->ZoomMinusBt,SIGNAL(clicked()),this,SLOT(s_ZoomMinus()));
}

//====================================================================================================================

wgt_QImageFilterTransform::~wgt_QImageFilterTransform() {
    delete ui;  // Deleting this make deletion of scene and all included object
    scene=NULL;
    if (SourceImage!=NULL) {
        delete SourceImage;
        SourceImage=NULL;
    }
}

//====================================================================================================================

void wgt_QImageFilterTransform::SetFilter(cFilterTransformObject *TheFilter,cDiaporamaObject *TheDiaporamaObject) {
    if (Filter!=NULL) return;
    Filter          =TheFilter;
    DiaporamaObject =TheDiaporamaObject;

    // If it's not first call
    if (SourceImage!=NULL) {
        delete SourceImage;
        SourceImage=NULL;
    }
    // Load image from source at position 0 with no filter
    if (Filter!=NULL) {
        // Try to load image from source
        if (DiaporamaObject->Video!=NULL)        SourceImage=DiaporamaObject->Video->ImageAt(true,DiaporamaObject->Parent->ApplicationConfig->PreviewMaxHeight,0,true,true,NULL,1,false);
        else if (DiaporamaObject->Image!=NULL)   SourceImage=DiaporamaObject->Image->ImageAt(true,DiaporamaObject->Parent->ApplicationConfig->PreviewMaxHeight,true);
    }

    ui->TransformationCB->SetCurrentFilter(SourceImage,&Filter->OnOffFilter);
    connect(ui->TransformationCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChTransformationCB(int)));

    // Other filter

    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaED->setValue(Filter->BlurSigma);
    connect(ui->BlurSigmaSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_BlurSigmaSliderMoved(int)));
    connect(ui->BlurSigmaED,SIGNAL(valueChanged(double)),this,SLOT(s_BlurSigmaValueED(double)));

    ui->BlurRadiusSlider->setValue(int(Filter->BlurRadius));
    ui->BlurRadiusED->setValue(int(Filter->BlurRadius));
    connect(ui->BlurRadiusSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_BlurRadiusSliderMoved(int)));
    connect(ui->BlurRadiusED,SIGNAL(valueChanged(int)),this,SLOT(s_BlurRadiusSliderMoved(int)));
}

//====================================================================================================================

void wgt_QImageFilterTransform::resizeEvent(QResizeEvent *) {
    if (scene!=NULL) {
        xmax=ui->GraphicsView->width();
        ymax=ui->GraphicsView->height();
        if (SourceImage!=NULL)  {

            // Get actual view rect
            QPointF topLeft     = ui->GraphicsView->mapToScene(0,0);
            QPointF bottomRight = ui->GraphicsView->mapToScene(xmax,ymax);
            QRectF  VisibleRect = QRectF(topLeft, bottomRight);

            // Update view
            ui->GraphicsView->fitInView(QRectF(0,0,double(SourceImage->width())/ZoomLevel,double(SourceImage->height())/ZoomLevel),Qt::KeepAspectRatio);

            // Ensure preview center was new center
            ui->GraphicsView->centerOn(QPointF(VisibleRect.x()+(VisibleRect.width()/2),VisibleRect.y()+(VisibleRect.height()/2)));

        } else ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::IgnoreAspectRatio);
        RefreshBackgroundImage();
    }
}

//====================================================================================================================

void wgt_QImageFilterTransform::showEvent(QShowEvent *) {
    if (scene==NULL) {
        // Create the scene
        scene = new QGraphicsScene();
        ui->GraphicsView->setScene(scene);
        ui->GraphicsView->setInteractive(true);

        xmax=ui->GraphicsView->width();
        ymax=ui->GraphicsView->height();
        if (SourceImage!=NULL) {
            scene->setSceneRect(QRectF(0,0,double(SourceImage->width())/ZoomLevel,double(SourceImage->height())/ZoomLevel));
            ui->GraphicsView->fitInView(QRectF(0,0,double(SourceImage->width())/ZoomLevel,double(SourceImage->height())/ZoomLevel),Qt::KeepAspectRatio);
        }
    }
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QImageFilterTransform::RefreshBackgroundImage() {
    // Be sure all object are ok !
    if ((Filter==NULL)||(SourceImage==NULL)||(scene==NULL)) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Remove old image if exist
    int i=0;
    while ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()!="image")) i++;
    if ((i<scene->items().count())&&(scene->items().at(i)->data(0).toString()=="image")) {
        // Get current QGraphicsPixmapItem
        QGraphicsPixmapItem *im=(QGraphicsPixmapItem *)scene->items().at(i);
        // Remove item from scene
        scene->removeItem(im);
        // Delete QGraphicsPixmapItem
        delete im;
    }

    // Add image to the background of the scene (Work only on a copy of the source image)
    QImage *CacheImage=new QImage(SourceImage->copy());
    Filter->ApplyFilter(CacheImage);
    QGraphicsPixmapItem *im=scene->addPixmap(QPixmap::fromImage(*CacheImage));
    delete CacheImage;  // No future need of this

    // Define QGraphicsPixmapItem properties
    im->setData(0,QVariant(QString("image")));
    im->setZValue(20);
    im->setPos(0,0);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void wgt_QImageFilterTransform::s_BlurSigmaSliderMoved(int Value) {
    if (SourceImage==NULL) return;
    Filter->BlurSigma=double(Value)/10;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaED->setValue(Filter->BlurSigma);
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QImageFilterTransform::s_BlurSigmaValueED(double Value) {
    if (SourceImage==NULL) return;
    Filter->BlurSigma=Value;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaED->setValue(Filter->BlurSigma);
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QImageFilterTransform::s_BlurRadiusSliderMoved(int Value) {
    if (SourceImage==NULL) return;
    Filter->BlurRadius=double(Value);
    ui->BlurRadiusSlider->setValue(int(Filter->BlurRadius));
    ui->BlurRadiusED->setValue(int(Filter->BlurRadius));
    RefreshBackgroundImage();
}

//====================================================================================================================

void wgt_QImageFilterTransform::s_ZoomPlus() {
    if ((Filter==NULL)||(scene==NULL)||(SourceImage==NULL)) return;

    // Get actual view rect
    QPointF topLeft     = ui->GraphicsView->mapToScene(0,0);
    QPointF bottomRight = ui->GraphicsView->mapToScene(xmax,ymax);
    QRectF  VisibleRect = QRectF(topLeft, bottomRight);

    // Update Zoomlevel
    ZoomLevel=ZoomLevel+1;
    ui->GraphicsView->fitInView(QRectF(0,0,double(SourceImage->width())/ZoomLevel,double(SourceImage->height())/ZoomLevel),Qt::KeepAspectRatio);

    // Ensure preview center was new center
    ui->GraphicsView->centerOn(QPointF(VisibleRect.x()+(VisibleRect.width()/2),VisibleRect.y()+(VisibleRect.height()/2)));
}

//====================================================================================================================

void wgt_QImageFilterTransform::s_ZoomMinus() {
    if ((Filter==NULL)||(scene==NULL)||(SourceImage==NULL)) return;
    if (ZoomLevel>1) {

        // Get actual view rect
        QPointF topLeft     = ui->GraphicsView->mapToScene(0,0);
        QPointF bottomRight = ui->GraphicsView->mapToScene(xmax,ymax);
        QRectF  VisibleRect = QRectF(topLeft, bottomRight);

        // Update Zoomlevel
        ZoomLevel=ZoomLevel-1;
        ui->GraphicsView->fitInView(QRectF(0,0,double(SourceImage->width())/ZoomLevel,double(SourceImage->height())/ZoomLevel),Qt::KeepAspectRatio);

        // Ensure preview center was new center
        ui->GraphicsView->centerOn(QPointF(VisibleRect.x()+(VisibleRect.width()/2),VisibleRect.y()+(VisibleRect.height()/2)));
    }
}

//====================================================================================================================

void wgt_QImageFilterTransform::s_ChTransformationCB(int) {
    Filter->OnOffFilter=ui->TransformationCB->GetCurrentFilter();
    RefreshBackgroundImage();
}
