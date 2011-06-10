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

#include "DlgImageTransformation.h"
#include "ui_DlgImageTransformation.h"
#include "mainwindow.h"

DlgImageTransformation::DlgImageTransformation(cBrushDefinition *TheCurrentBrush,QWidget *parent):QDialog(parent),ui(new Ui::DlgImageTransformation) {
    ui->setupUi(this);
    CurrentBrush=TheCurrentBrush;

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");    // Create xml document and root
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT","");  // Save object
    Undo->appendChild(root);                             // Add object to xml document

    Filter      = (CurrentBrush->Image!=NULL)?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    scene       = NULL;
    SourceImage = NULL;
    ZoomLevel   = 1;

    // Load image from source at position 0 with no filter
    // Try to load image from source
    SourceImage=(CurrentBrush->Image?CurrentBrush->Image->ImageAt(true,true,NULL):
                 CurrentBrush->Video?CurrentBrush->Video->ImageAt(true,0,true,NULL,1,false,NULL):
                 NULL);

    ui->TransformationCB->SetCurrentFilter(SourceImage,&Filter->OnOffFilter);

    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaED->setValue(Filter->BlurSigma);

    ui->BlurRadiusSlider->setValue(int(Filter->BlurRadius));
    ui->BlurRadiusED->setValue(int(Filter->BlurRadius));

    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
    connect(ui->ZoomPlusBt,SIGNAL(clicked()),this,SLOT(s_ZoomPlus()));
    connect(ui->ZoomMinusBt,SIGNAL(clicked()),this,SLOT(s_ZoomMinus()));
    connect(ui->BlurRadiusSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_BlurRadiusSliderMoved(int)));
    connect(ui->BlurRadiusED,SIGNAL(valueChanged(int)),this,SLOT(s_BlurRadiusSliderMoved(int)));
    connect(ui->BlurSigmaSlider,SIGNAL(sliderMoved(int)),this,SLOT(s_BlurSigmaSliderMoved(int)));
    connect(ui->BlurSigmaED,SIGNAL(valueChanged(double)),this,SLOT(s_BlurSigmaValueED(double)));
    connect(ui->TransformationCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_ChTransformationCB(int)));
}

//====================================================================================================================

DlgImageTransformation::~DlgImageTransformation() {
    if (scene) {
        delete scene;
        scene=NULL;
    }
    delete ui;
    delete Undo;
    if (SourceImage!=NULL) {
        delete SourceImage;
        SourceImage=NULL;
    }
}

//====================================================================================================================

void DlgImageTransformation::Help() {
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString(HELPFILE_DlgImageTransformation)));
}

//====================================================================================================================

void DlgImageTransformation::SetSavedWindowGeometry() {
    GlobalMainWindow->ApplicationConfig->DlgImageTransformationWSP->ApplyToWindow(this);
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
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageTransformation::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    if (scene==NULL) QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgImageTransformation::reject() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgImageTransformationWSP->SaveWindowState(this);
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","");
    done(1);
}

//====================================================================================================================

void DlgImageTransformation::accept() {
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgImageTransformationWSP->SaveWindowState(this);

    // Close the box
    done(0);
}

//====================================================================================================================

void DlgImageTransformation::resizeEvent(QResizeEvent *) {
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

void DlgImageTransformation::RefreshBackgroundImage() {
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

void DlgImageTransformation::s_BlurSigmaSliderMoved(int Value) {
    if (SourceImage==NULL) return;
    Filter->BlurSigma=double(Value)/10;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaED->setValue(Filter->BlurSigma);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageTransformation::s_BlurSigmaValueED(double Value) {
    if (SourceImage==NULL) return;
    Filter->BlurSigma=Value;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaED->setValue(Filter->BlurSigma);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageTransformation::s_BlurRadiusSliderMoved(int Value) {
    if (SourceImage==NULL) return;
    Filter->BlurRadius=double(Value);
    ui->BlurRadiusSlider->setValue(int(Filter->BlurRadius));
    ui->BlurRadiusED->setValue(int(Filter->BlurRadius));
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageTransformation::s_ZoomPlus() {
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

void DlgImageTransformation::s_ZoomMinus() {
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

void DlgImageTransformation::s_ChTransformationCB(int) {
    Filter->OnOffFilter=ui->TransformationCB->GetCurrentFilter();
    RefreshBackgroundImage();
}

