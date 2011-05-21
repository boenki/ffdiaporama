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

DlgImageCorrection::DlgImageCorrection(int TheBackgroundForm,cBrushDefinition *TheCurrentBrush,cFilterCorrectObject *TheBrushFileCorrect,QWidget *parent):QDialog(parent),ui(new Ui::DlgImageCorrection) {
    ui->setupUi(this);
    BackgroundForm  =TheBackgroundForm;
    CurrentBrush    =TheCurrentBrush;
    BrushFileCorrect=TheBrushFileCorrect;
    CachedImage     =CurrentBrush->Image?CurrentBrush->Image->CacheImage:CurrentBrush->Video->CacheFirstImage;      // If no image and no video : program will crash !

    // Save object before modification for cancel button
    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");       // Create xml document and root
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT","");     // Save object
    Undo->appendChild(root);                                // Add object to xml document

    IsFirstInitDone = false;                                // true when first show window was done
    FLAGSTOPED      = false;
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

    // Define handler
    connect(ui->CloseBT,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

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

DlgImageCorrection::~DlgImageCorrection() {
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

void DlgImageCorrection::Help() {
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/HelpTransitionProperties.php")));
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
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","");
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
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_YValueEDChanged(double Value) {
    if (FLAGSTOPED || (BrushFileCorrect==NULL)) return;
    BrushFileCorrect->Y=Value/100;
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_ZoomValueEDChanged(double Value) {
    if (FLAGSTOPED || (BrushFileCorrect==NULL)) return;
    BrushFileCorrect->ZoomFactor=Value/100;
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_RotationEDChanged(int Value) {
    if (FLAGSTOPED || (BrushFileCorrect==NULL)) return;
    if (Value<-180) Value=360-Value;
    if (Value>180)  Value=-360-Value;
    BrushFileCorrect->ImageRotation=Value;
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_RotateLeft() {
    if (BrushFileCorrect==NULL) return;
    int Value=(((BrushFileCorrect->ImageRotation-90)/90)*90);
    if (Value<=-180) Value=360-Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_RotateRight() {
    if (BrushFileCorrect==NULL) return;
    int Value=(((BrushFileCorrect->ImageRotation+90)/90)*90);
    if (Value>180) Value=-360+Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustW() {
    if (BrushFileCorrect==NULL) return;
    BrushFileCorrect->X=MagneticRuller.MagnetX1/xmax;
    BrushFileCorrect->ZoomFactor=(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1)/xmax;
    BrushFileCorrect->Y=((ymax-GlobalMainWindow->Diaporama->GetHeightForWidth(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1))/2)/ymax;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustH() {
    if (BrushFileCorrect==NULL) return;
    BrushFileCorrect->Y=MagneticRuller.MagnetY1/ymax;
    BrushFileCorrect->ZoomFactor=(MagneticRuller.MagnetY2-MagneticRuller.MagnetY1)/ymax;
    BrushFileCorrect->X=((xmax-GlobalMainWindow->Diaporama->GetWidthForHeight(MagneticRuller.MagnetY2-MagneticRuller.MagnetY1))/2)/xmax;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustWH() {
    if (BrushFileCorrect==NULL) return;
    BrushFileCorrect->X=MagneticRuller.MagnetX1/xmax;
    BrushFileCorrect->ZoomFactor=(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1)/xmax;
    BrushFileCorrect->Y=((ymax-GlobalMainWindow->Diaporama->GetHeightForWidth(MagneticRuller.MagnetX2-MagneticRuller.MagnetX1))/2)/ymax;
    if (BrushFileCorrect->Y>MagneticRuller.MagnetY1/ymax) s_AdjustH(); else RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::RefreshControls() {
    if ((BrushFileCorrect==NULL)||(!scene)||(FLAGSTOPED)) return;
    FLAGSTOPED=true;
    ui->XValue->setRange(0,100-BrushFileCorrect->ZoomFactor*100);
    ui->XValue->setSingleStep(double(100)/20);
    ui->XValue->setValue(BrushFileCorrect->X*100);
    ui->YValue->setRange(0,100-(BrushFileCorrect->ZoomFactor*BrushFileCorrect->AspectRatio)*100);
    ui->YValue->setSingleStep(double(100)/20);
    ui->YValue->setValue(BrushFileCorrect->Y*100);
    ui->ZoomValue->setRange(1,100);
    ui->ZoomValue->setSingleStep(double(100)/20);
    ui->ZoomValue->setValue(BrushFileCorrect->ZoomFactor*100);

    ui->RotateED->setValue(BrushFileCorrect->ImageRotation);

    // FilterCorrection
    ui->BrightnessSlider->setValue(BrushFileCorrect->Brightness);     ui->BrightnessValue->setValue(BrushFileCorrect->Brightness);
    ui->ContrastSlider->setValue(BrushFileCorrect->Contrast);         ui->ContrastValue->setValue(BrushFileCorrect->Contrast);
    ui->GammaSlider->setValue(BrushFileCorrect->Gamma*100);           ui->GammaValue->setValue(BrushFileCorrect->Gamma);
    ui->RedSlider->setValue(BrushFileCorrect->Red);                   ui->RedValue->setValue(BrushFileCorrect->Red);
    ui->GreenSlider->setValue(BrushFileCorrect->Green);               ui->GreenValue->setValue(BrushFileCorrect->Green);
    ui->BlueSlider->setValue(BrushFileCorrect->Blue);                 ui->BlueValue->setValue(BrushFileCorrect->Blue);

    // Resize et repos all item in the scene
    for (int i=0;i<scene->items().count();i++) if (scene->items()[i]->data(0).toString()=="CustomGraphicsRectItem") {
        cCustomGraphicsRectItem *RectItem=(cCustomGraphicsRectItem *)scene->items()[i];
        RectItem->setPos(BrushFileCorrect->X*xmax,BrushFileCorrect->Y*ymax);
        QRectF Rect=RectItem->mapRectFromScene(QRectF(BrushFileCorrect->X*xmax,BrushFileCorrect->Y*ymax,xmax*BrushFileCorrect->ZoomFactor,ymax*BrushFileCorrect->ZoomFactor));
        RectItem->setRect(Rect);
        RectItem->RecalcEmbededResizeRectItem();
    }
    // Refresh image
    RefreshBackgroundImage();
    FLAGSTOPED=false;
}

//====================================================================================================================

void DlgImageCorrection::s_MagneticEdgeBt() {
    if (MagneticRuller.MagneticRuler==true) MagneticRuller.MagneticRuler=false; else MagneticRuller.MagneticRuler=true;
    ui->MagneticEdgeBt->setIcon(*(MagneticRuller.MagneticRuler?RulerOn:RulerOff));
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
                                                       NULL,NULL,xmax,ymax,true,BrushFileCorrect->AspectRatio,&MagneticRuller,this,TYPE_DlgImageCorrection);

    // Prepare CacheImage
    QImage   *NewImage=new QImage(xmax,ymax,QImage::Format_ARGB32_Premultiplied);
    QImage   *SourceImage=NULL;
    QPainter P;

    // Rotate image if needed and create a SourceImage
    if (BrushFileCorrect->ImageRotation!=0) {
        QTransform matrix;
        matrix.rotate(BrushFileCorrect->ImageRotation,Qt::ZAxis);
        SourceImage=new QImage(CachedImage->transformed(matrix));

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

    P.begin(NewImage);
    P.fillRect(QRectF(0,0,xmax,ymax),GlobalMainWindow->Diaporama->Transparent);
    P.drawImage(QRectF(DstX,DstY,DstW,DstH),*SourceImage,QRectF(0,0,RealImageW,RealImageH));
    if (SourceImage!=CachedImage) delete SourceImage;
    BrushFileCorrect->ApplyFilter(NewImage);

    //********************************************************

    // Calc magnetic ruller guides positions
    MagneticRuller.MagnetX1=DstX;
    MagneticRuller.MagnetX2=DstX+DstW;
    MagneticRuller.MagnetY1=DstY;
    MagneticRuller.MagnetY2=DstY+DstH;

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
    if (MagneticRuller.MagneticRuler==true) {
        QColor col=QColor(255,0,0);
        QPen   pen=QPen(col);
        pen.setWidth(2);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setStyle(Qt::DotLine);
        P.setPen(pen);
        P.setBrush(Qt::NoBrush);
        P.drawRect(X,Y,W,H);
    }
    if (MagneticRuller.MagneticRuler==true) {
        QColor col=QColor(0,255,0);
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
    QGraphicsPixmapItem *im=scene->addPixmap(QPixmap::fromImage(*NewImage));
    im->setData(0,QVariant(QString("image")));
    im->setZValue(200);
    im->setPos(0,0);
    delete NewImage;

    RefreshControls();
    QApplication::restoreOverrideCursor();
}

//====================================================================================================================

void DlgImageCorrection::s_BrightnessSliderMoved(int Value) {
    BrushFileCorrect->Brightness=Value;
    ui->BrightnessSlider->setValue(BrushFileCorrect->Brightness);
    ui->BrightnessValue->setValue(BrushFileCorrect->Brightness);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_ContrastSliderMoved(int Value) {
    BrushFileCorrect->Contrast=Value;
    ui->ContrastSlider->setValue(BrushFileCorrect->Contrast);
    ui->ContrastValue->setValue(BrushFileCorrect->Contrast);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_GammaSliderMoved(int Value) {
    BrushFileCorrect->Gamma=double(Value)/100;
    ui->GammaSlider->setValue(BrushFileCorrect->Gamma*100);
    ui->GammaValue->setValue(BrushFileCorrect->Gamma);
    RefreshBackgroundImage();
}

//====================================================================================================================

void DlgImageCorrection::s_GammaValueED(double Value) {
    BrushFileCorrect->Gamma=Value;
    ui->GammaSlider->setValue(BrushFileCorrect->Gamma*100);
    ui->GammaValue->setValue(BrushFileCorrect->Gamma);
    RefreshBackgroundImage();
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
