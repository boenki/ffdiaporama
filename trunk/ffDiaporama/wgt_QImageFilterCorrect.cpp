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

#include <QCoreApplication>
#include "wgt_QImageFilterCorrect.h"
#include "ui_wgt_QImageFilterCorrect.h"
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QScrollBar>
#include "cdiaporama.h"
#include "fmt_filters.h"

wgt_QImageFilterCorrect::wgt_QImageFilterCorrect(QWidget *parent) : QWidget(parent),ui(new Ui::wgt_QImageFilterCorrect) {
    ui->setupUi(this);
    Filter          = NULL;
    scene           = NULL;
}

//====================================================================================================================

wgt_QImageFilterCorrect::~wgt_QImageFilterCorrect() {
    delete ui;  // Deleting this make deletion of scene and all included object
    scene=NULL;
}

//====================================================================================================================

void wgt_QImageFilterCorrect::SetFilter(cFilterCorrectObject *TheFilter,cDiaporamaShot *TheDiaporamaShotObject) {
    // First init
    bool FirstInit=(Filter==NULL);

    Filter              =TheFilter;
    DiaporamaShotObject =TheDiaporamaShotObject;

    // Load image from source at position 0 with no filter
    if ((Filter!=NULL)&&(scene!=NULL)) {
        // Calc and adjust ui->GraphicsView depending on geometry
        xmax=ui->GraphicsView->width();
        ymax=DiaporamaShotObject->Parent->Parent->GetHeightForWidth(xmax);
        if (ymax<ui->GraphicsView->height()) {
            ymax=ui->GraphicsView->height();
            xmax=DiaporamaShotObject->Parent->Parent->GetWidthForHeight(ymax);
        }
        // Update the scene
        scene->setSceneRect(QRectF(0,0,xmax,ymax));
        ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);

        RefreshBackgroundImage();
    }

    // Other filter
    ui->BrightnessSlider->setValue(Filter->Brightness);     ui->BrightnessValue->setValue(Filter->Brightness);
    ui->ContrastSlider->setValue(Filter->Contrast);         ui->ContrastValue->setValue(Filter->Contrast);
    ui->GammaSlider->setValue(Filter->Gamma*100);           ui->GammaValue->setValue(Filter->Gamma);
    ui->RedSlider->setValue(Filter->Red);                   ui->RedValue->setValue(Filter->Red);
    ui->GreenSlider->setValue(Filter->Green);               ui->GreenValue->setValue(Filter->Green);
    ui->BlueSlider->setValue(Filter->Blue);                 ui->BlueValue->setValue(Filter->Blue);

    if (FirstInit) {
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
}

//====================================================================================================================

void wgt_QImageFilterCorrect::resizeEvent(QResizeEvent *) {
    if (scene!=NULL) {
        // Calc and adjust ui->GraphicsView depending on geometry
        xmax=ui->GraphicsView->width();
        ymax=DiaporamaShotObject->Parent->Parent->GetHeightForWidth(xmax);
        if (ymax<ui->GraphicsView->height()) {
            ymax=ui->GraphicsView->height();
            xmax=DiaporamaShotObject->Parent->Parent->GetWidthForHeight(ymax);
        }
        // Create the scene
        scene->setSceneRect(QRectF(0,0,xmax,ymax));
        ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);

        RefreshBackgroundImage();
    }
}

//====================================================================================================================

void wgt_QImageFilterCorrect::showEvent(QShowEvent *) {
    if (scene==NULL) {
        // Create the scene
        xmax=ui->GraphicsView->width();
        ymax=ui->GraphicsView->height();

        scene = new QGraphicsScene();
        ui->GraphicsView->setScene(scene);
        ui->GraphicsView->setInteractive(true);
        ui->GraphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        ui->GraphicsView->fitInView(QRectF(0,0,xmax,ymax),Qt::KeepAspectRatio);

        RefreshBackgroundImage();
    }
}

//====================================================================================================================

void wgt_QImageFilterCorrect::RefreshBackgroundImage() {
    // Be sure all object are ok !
    if ((Filter==NULL)||(scene==NULL)) return;

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
    int Position = 0;
    for (i=0;&DiaporamaShotObject->Parent->List[i]!=DiaporamaShotObject;i++)
        Position=Position+(i>0?DiaporamaShotObject->Parent->List[i].GetMobilDuration():0)+DiaporamaShotObject->Parent->List[i].GetStaticDuration();
    if (i>0) Position=Position+DiaporamaShotObject->GetMobilDuration();
    QImage *CacheImage=DiaporamaShotObject->Parent->CanvasImageAt(xmax,ymax,Position,NULL,0,0,NULL,NULL,true,false,true,NULL);
    //Filter->ApplyFilter(CacheImage);
    QGraphicsPixmapItem *im=scene->addPixmap(QPixmap::fromImage(*CacheImage));
    delete CacheImage;  // No future need of this

    // Define QGraphicsPixmapItem properties
    im->setData(0,QVariant(QString("image")));
    im->setZValue(20);
    im->setPos(0,0);
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void wgt_QImageFilterCorrect::s_BrightnessSliderMoved(int Value) {
    Filter->Brightness=Value;
    ui->BrightnessSlider->setValue(Filter->Brightness);
    ui->BrightnessValue->setValue(Filter->Brightness);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QImageFilterCorrect::s_ContrastSliderMoved(int Value) {
    Filter->Contrast=Value;
    ui->ContrastSlider->setValue(Filter->Contrast);
    ui->ContrastValue->setValue(Filter->Contrast);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QImageFilterCorrect::s_GammaSliderMoved(int Value) {
    Filter->Gamma=double(Value)/100;
    ui->GammaSlider->setValue(Filter->Gamma*100);
    ui->GammaValue->setValue(Filter->Gamma);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QImageFilterCorrect::s_GammaValueED(double Value) {
    Filter->Gamma=Value;
    ui->GammaSlider->setValue(Filter->Gamma*100);
    ui->GammaValue->setValue(Filter->Gamma);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QImageFilterCorrect::s_RedSliderMoved(int Value) {
    Filter->Red=Value;
    ui->RedSlider->setValue(Filter->Red);
    ui->RedValue->setValue(Filter->Red);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QImageFilterCorrect::s_GreenSliderMoved(int Value) {
    Filter->Green=Value;
    ui->GreenSlider->setValue(Filter->Green);
    ui->GreenValue->setValue(Filter->Green);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}

//====================================================================================================================

void wgt_QImageFilterCorrect::s_BlueSliderMoved(int Value) {
    Filter->Blue=Value;
    ui->BlueSlider->setValue(Filter->Blue);
    ui->BlueValue->setValue(Filter->Blue);
    RefreshBackgroundImage();
    emit ModifyDataSignal();
}
