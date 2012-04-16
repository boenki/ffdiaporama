/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2012 Dominique Levray <levray.dominique@bbox.fr>

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

#include "cImgInteractiveZone.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSplashScreen>

#define ICON_RULER_ON                       ":/img/ruler_ok.png"
#define ICON_RULER_OFF                      ":/img/ruler_ko.png"
#define ICON_GEOMETRY_LOCKED                ":/img/Geometry_Lock.png"
#define ICON_GEOMETRY_UNLOCKED              ":/img/Geometry_Unlock.png"
#define ICON_GEOMETRY_PROJECT               ":/img/Geometry_ProjectLock.png"
#define ICON_GEOMETRY_IMAGE                 ":/img/Geometry_ImageLock.png"
#define ICON_OBJECT_MOVIE                   ":/img/object_movie.png"

#define UNDOACTION_INTERACTIVEMOVERESIZE    1
#define UNDOACTION_EDITZONE_XVALUE          2
#define UNDOACTION_EDITZONE_YVALUE          3
#define UNDOACTION_EDITZONE_WVALUE          4
#define UNDOACTION_EDITZONE_HVALUE          5
#define UNDOACTION_EDITZONE_ROTATEVALUE     6
#define UNDOACTION_EDITZONE_FRAMING         7
#define UNDOACTION_EDITZONE_FILE            8
#define UNDOACTION_EDITZONE_TRANSFO         9
#define UNDOACTION_EDITZONE_BLUR            10
#define UNDOACTION_EDITZONE_BLURRD          11
#define UNDOACTION_EDITZONE_BRIGHTNESS      12
#define UNDOACTION_EDITZONE_CONTRAST        13
#define UNDOACTION_EDITZONE_GAMMA           14
#define UNDOACTION_EDITZONE_RED             15
#define UNDOACTION_EDITZONE_GREEN           16
#define UNDOACTION_EDITZONE_BLUE            17
#define UNDOACTION_EDITZONE_GEOMETRY        18

DlgImageCorrection::DlgImageCorrection(cCompositionObject *TheCompoObject,int TheBackgroundForm,cBrushDefinition *TheCurrentBrush,int TheVideoPosition,int TheImageGeometry,
    QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgImageCorrection) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::DlgImageCorrection");

    ui->setupUi(this);

    OkBt            =ui->OKBT;
    CancelBt        =ui->CancelBt;
    HelpBt          =ui->HelpBT;
    UndoBt          =ui->UndoBT;
    UndoReloadImage =false;
    FLAGSTOPED      =false;
    FLAGSTOPSPIN    =false;
    ffDPrjGeometry  =TheImageGeometry;
    CurrentBrush    =TheCurrentBrush;
    CompoObject     =TheCompoObject;
    ui->InteractiveZone->MagneticRuler=((cApplicationConfig *)BaseApplicationConfig)->FramingRuler;
    ui->InteractiveZone->InitCachedImage(TheCompoObject,TheBackgroundForm,TheCurrentBrush,TheVideoPosition);
}

//====================================================================================================================

DlgImageCorrection::~DlgImageCorrection() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::~DlgImageCorrection");
    delete ui;  // Deleting this make deletion of scene and all included object
}

//====================================================================================================================
// Initialise dialog

void DlgImageCorrection::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::DoInitDialog");

    if (CurrentBrush->Image) {
        InitialFilteredString=CurrentBrush->Image->BrushFileTransform.FilterToString();
        ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.SetImageGeometryFilter(ffDPrjGeometry,CurrentBrush->Image->ObjectGeometry);
     } else if (CurrentBrush->Video) {
        InitialFilteredString=CurrentBrush->Video->BrushFileTransform.FilterToString();
        ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.SetImageGeometryFilter(ffDPrjGeometry,CurrentBrush->Video->ObjectGeometry);
    }

    ui->LockGeometryCB->view()->setFixedWidth(300);

    ui->LockGeometryCB->addItem(QIcon(ICON_GEOMETRY_UNLOCKED),QApplication::translate("DlgImageCorrection","Unlock"));
    ui->LockGeometryCB->addItem(QIcon(ICON_GEOMETRY_LOCKED),  QApplication::translate("DlgImageCorrection","Lock to this geometry"));
    ui->LockGeometryCB->addItem(QIcon(ICON_GEOMETRY_PROJECT), QApplication::translate("DlgImageCorrection","Lock to project geometry"));
    ui->LockGeometryCB->addItem(QIcon(ICON_GEOMETRY_IMAGE),   QApplication::translate("DlgImageCorrection","Lock to image geometry"));

    switch (ffDPrjGeometry) {
        case GEOMETRY_4_3   : ProjectGeometry=double(1440)/double(1920);  break;
        case GEOMETRY_16_9  : ProjectGeometry=double(1080)/double(1920);  break;
        case GEOMETRY_40_17 : ProjectGeometry=double(816)/double(1920);   break;

    }
    ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment

    if (CurrentBrush->Image)            ImageGeometry=double(CurrentBrush->Image->ImageHeight)/double(CurrentBrush->Image->ImageWidth);
        else if (CurrentBrush->Video)   ImageGeometry=double(CurrentBrush->Video->ImageHeight)/double(CurrentBrush->Video->ImageWidth);
    ImageGeometry=QString("%1").arg(ImageGeometry,0,'e').toDouble();  // Rounded to same number as style managment
    ui->RulersBT->setIcon(QIcon(((cApplicationConfig *)BaseApplicationConfig)->FramingRuler?QString(ICON_RULER_ON):QString(ICON_RULER_OFF)));

    ui->RotateED->setMinimum(-180);
    ui->RotateED->setMaximum(180);

    ui->XValue->setSingleStep(1);  ui->XValue->setRange(-200,200);
    ui->YValue->setSingleStep(1);  ui->YValue->setRange(-200,200);
    ui->WValue->setSingleStep(1);  ui->WValue->setRange(1,200);
    ui->HValue->setSingleStep(1);  ui->HValue->setRange(1,200);

    OnOffFilter=CurrentBrush->Video?CurrentBrush->Video->BrushFileTransform.OnOffFilter:CurrentBrush->Image->BrushFileTransform.OnOffFilter;
    ui->TransformationCB->SetCurrentFilter(ui->InteractiveZone->CachedImage,&OnOffFilter);

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
    connect(ui->RulersBT,SIGNAL(clicked()),this,SLOT(s_RulersBT()));

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

    connect(ui->FileNameBT,SIGNAL(clicked()),this,SLOT(ChangeBrushDiskFile()));
    connect(ui->InteractiveZone,SIGNAL(TransformBlock(double,double,double,double)),this,SLOT(s_IntZoneTransformBlocks(double,double,double,double)));
    connect(ui->InteractiveZone,SIGNAL(DisplayTransformBlock(double,double,double,double)),this,SLOT(s_DisplayIntZoneTransformBlocks(double,double,double,double)));

    ui->InteractiveZone->setFocus();
}

//====================================================================================================================
// Initiale Undo

void DlgImageCorrection::PrepareGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::PrepareGlobalUndo");

    // Save objects before modification for cancel button
    UndoBrushFileName=(CurrentBrush->Image!=NULL)?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName;

    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");           // Create xml document and root
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT","",true);    // Save object
    if (CurrentBrush->Video) CurrentBrush->Video->BrushFileTransform.SaveToXML(root,"ImageTransformation");
        else                 CurrentBrush->Image->BrushFileTransform.SaveToXML(root,"ImageTransformation");
    Undo->appendChild(root);                                    // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgImageCorrection::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::DoGlobalUndo");

    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL);
    if (CurrentBrush->Video) CurrentBrush->Video->BrushFileTransform.LoadFromXML(root,"ImageTransformation");
        else                 CurrentBrush->Image->BrushFileTransform.LoadFromXML(root,"ImageTransformation");
    if (UndoReloadImage) {
        if (CurrentBrush->Image)            CurrentBrush->Image->GetInformationFromFile(UndoBrushFileName,NULL,NULL);
            else if (CurrentBrush->Video)   CurrentBrush->Video->GetInformationFromFile(UndoBrushFileName,NULL,NULL);
    }
}

//====================================================================================================================

void DlgImageCorrection::PreparePartialUndo(int /*ActionType*/,QDomElement root) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::PreparePartialUndo");

    QString BrushFileName=(CurrentBrush->Image!=NULL)?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName;
    root.setAttribute("BrushFileName",BrushFileName);
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT","",true);    // Save object
    if (CurrentBrush->Video) CurrentBrush->Video->BrushFileTransform.SaveToXML(root,"ImageTransformation");
        else                 CurrentBrush->Image->BrushFileTransform.SaveToXML(root,"ImageTransformation");
}

//====================================================================================================================

void DlgImageCorrection::ApplyPartialUndo(int /*ActionType*/,QDomElement root) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::ApplyPartialUndo");


    QString BrushFileName=root.attribute("BrushFileName");
    CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL);
    if (CurrentBrush->Video) CurrentBrush->Video->BrushFileTransform.LoadFromXML(root,"ImageTransformation");
        else                 CurrentBrush->Image->BrushFileTransform.LoadFromXML(root,"ImageTransformation");
    if (BrushFileName!=((CurrentBrush->Image!=NULL)?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName)) {
        if (CurrentBrush->Image)            CurrentBrush->Image->GetInformationFromFile(BrushFileName,NULL,NULL);
            else if (CurrentBrush->Video)   CurrentBrush->Video->GetInformationFromFile(BrushFileName,NULL,NULL);
    }
    RefreshControls();
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::s_BrightnessReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BrightnessReset");
    s_BrightnessSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_ContrastReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_ContrastReset");
    s_ContrastSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_GammaReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_GammaReset");
    s_GammaValueED(1);
}

//====================================================================================================================

void DlgImageCorrection::s_RedReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RedReset");
    s_RedSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_GreenReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_GreenReset");
    s_GreenSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_BlueReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlueReset");
    s_BlueSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_BlurSharpenReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSharpenReset");
    s_BlurSigmaSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_RadiusReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RadiusReset");
    s_BlurRadiusSliderMoved(5);
}


//====================================================================================================================

void DlgImageCorrection::resizeEvent(QResizeEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::resizeEvent");
    ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
}

//====================================================================================================================

void DlgImageCorrection::showEvent(QShowEvent *ev) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::showEvent");
    QCustomDialog::showEvent(ev);
    ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
    RefreshControls();
 }

//====================================================================================================================

void DlgImageCorrection::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::DoAccept");

    // Check if cached filtered file exist
    QString CachedFile=CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName;
    CachedFile=CachedFile.replace("."+QFileInfo(CachedFile).suffix(),"_ffd.jpg");
    QString NewFilter=CurrentBrush->Image?CurrentBrush->Image->BrushFileTransform.FilterToString():CurrentBrush->Video->BrushFileTransform.FilterToString();
    if ((NewFilter!="")&&(CurrentBrush->Image)&&((InitialFilteredString!=NewFilter)||((((cApplicationConfig *)BaseApplicationConfig)->AllowCachedTransfoImages)&&(!QFileInfo(CachedFile).exists())))) {
        if (QFileInfo(CachedFile).exists()) QFile::remove(CachedFile);
        if (((cApplicationConfig *)BaseApplicationConfig)->AllowCachedTransfoImages) {
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            QSplashScreen screen(this,QPixmap(":/img/splash.png"));
            screen.showMessage(QApplication::translate("DlgImageCorrection","Creating cached filtered file ..."),Qt::AlignHCenter|Qt::AlignBottom);
            screen.show();
            QApplication::processEvents();
            cLuLoImageCacheObject *ImageObject=((cApplicationConfig *)BaseApplicationConfig)->ImagesCache.FindObject(CurrentBrush->Image->FileName,CurrentBrush->Image->ModifDateTime,CurrentBrush->Image->ImageOrientation,NULL,true,true);
            QImage *UnfilteredImage=new QImage(ImageObject->ValidateCacheRenderImage()->copy());
            if (UnfilteredImage->format()!=QImage::Format_ARGB32_Premultiplied) {
                QImage *NewUnfiltered=new QImage(UnfilteredImage->convertToFormat(QImage::Format_ARGB32_Premultiplied));
                delete UnfilteredImage;
                UnfilteredImage=NewUnfiltered;
            }
            CurrentBrush->Image->BrushFileTransform.ApplyFilter(UnfilteredImage);
            UnfilteredImage->save(CachedFile,"jpg",100);
            delete UnfilteredImage;
            screen.hide();
            QApplication::restoreOverrideCursor();
        }
    }
}

//====================================================================================================================

void DlgImageCorrection::s_XValueEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_XValueEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_XVALUE,ui->InteractiveZone,false);
    CurrentBrush->X=Value/100;
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_YValueEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_YValueEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_YVALUE,ui->InteractiveZone,false);
    CurrentBrush->Y=Value/100;
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_WValueEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_WValueEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_WVALUE,ui->InteractiveZone,false);
    if (CurrentBrush->LockGeometry) {
        CurrentBrush->ZoomFactor=Value/100;
    } else {
        double newH=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio*ui->InteractiveZone->dmax;
        CurrentBrush->ZoomFactor=Value/100;
        double newW=CurrentBrush->ZoomFactor*ui->InteractiveZone->dmax;
        CurrentBrush->AspectRatio=newH/newW;
    }
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_HValueEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_HValueEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_HVALUE,ui->InteractiveZone,false);
    double newH=(Value/100)*ui->InteractiveZone->dmax;
    if (CurrentBrush->LockGeometry) {
        double newW=newH/CurrentBrush->AspectRatio;
        CurrentBrush->ZoomFactor=newW/ui->InteractiveZone->dmax;
    } else {
        double newW=CurrentBrush->ZoomFactor*ui->InteractiveZone->dmax;
        CurrentBrush->AspectRatio=newH/newW;
    }
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotationEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RotationEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEVALUE,ui->InteractiveZone,false);
    if (Value<-180) Value=360-Value;
    if (Value>180)  Value=-360-Value;
    CurrentBrush->ImageRotation=Value;
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotateLeft() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RotateLeft");
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEVALUE,ui->InteractiveZone,true);
    double Value=(int((CurrentBrush->ImageRotation-90)/90)*90);
    if (Value<=-180) Value=360-Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_RotateRight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RotateRight");
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEVALUE,ui->InteractiveZone,true);
    double Value=(int((CurrentBrush->ImageRotation+90)/90)*90);
    if (Value>180) Value=-360+Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_AdjustW() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_AdjustW");
    AppendPartialUndo(UNDOACTION_EDITZONE_FRAMING,ui->InteractiveZone,true);

    double W=ui->InteractiveZone->maxw;
    double H=W*CurrentBrush->AspectRatio;
    CurrentBrush->X=((ui->InteractiveZone->dmax-W)/2)/ui->InteractiveZone->dmax;
    CurrentBrush->Y=((ui->InteractiveZone->dmax-H)/2)/ui->InteractiveZone->dmax;
    CurrentBrush->ZoomFactor=W/ui->InteractiveZone->dmax;
    RefreshControls();

}

//====================================================================================================================

void DlgImageCorrection::s_AdjustH() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_AdjustH");
    AppendPartialUndo(UNDOACTION_EDITZONE_FRAMING,ui->InteractiveZone,true);

    double H=ui->InteractiveZone->maxh;
    double W=H/CurrentBrush->AspectRatio;
    CurrentBrush->X=((ui->InteractiveZone->dmax-W)/2)/ui->InteractiveZone->dmax;
    CurrentBrush->Y=((ui->InteractiveZone->dmax-H)/2)/ui->InteractiveZone->dmax;
    CurrentBrush->ZoomFactor=W/ui->InteractiveZone->dmax;
    RefreshControls();

}

//====================================================================================================================

void DlgImageCorrection::s_AdjustWH() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_AdjustWH");

    // Special case for custom geometry -> use all the image then change aspect ratio to image aspect ratio
    if (!CurrentBrush->LockGeometry) {
        AppendPartialUndo(UNDOACTION_EDITZONE_FRAMING,ui->InteractiveZone,true);
        double W=ui->InteractiveZone->maxw;
        double H=ui->InteractiveZone->maxh;
        CurrentBrush->AspectRatio=H/W;
        CurrentBrush->X=((ui->InteractiveZone->dmax-W)/2)/ui->InteractiveZone->dmax;
        CurrentBrush->Y=((ui->InteractiveZone->dmax-H)/2)/ui->InteractiveZone->dmax;
        CurrentBrush->ZoomFactor=W/ui->InteractiveZone->dmax;
        RefreshControls();
    } else {
        double W=ui->InteractiveZone->maxw;
        double H=W*CurrentBrush->AspectRatio;
        if (H<ui->InteractiveZone->maxh) s_AdjustH(); else s_AdjustW();
    }
}

//====================================================================================================================

void DlgImageCorrection::RefreshControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::RefreshControls");
    if (FLAGSTOPED) return;
    FLAGSTOPED=true;

    if (((CurrentBrush->Image==NULL)&&(CurrentBrush->Video==NULL))||
        ((CurrentBrush->Image!=NULL)&&(CurrentBrush->Image->ObjectGeometry==IMAGE_GEOMETRY_UNKNOWN))||
        ((CurrentBrush->Video!=NULL)&&(CurrentBrush->Video->ObjectGeometry==IMAGE_GEOMETRY_UNKNOWN))) {
        ui->FramingStyleED->setText(QApplication::translate("DlgImageCorrection","No style for nonstandard geometry image"));
        ui->FramingStyleBT->setEnabled(false);
    } else {
        ui->FramingStyleED->setText(((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.GetStyleName(CurrentBrush->GetFramingStyle()));
        ui->FramingStyleBT->setEnabled(true);
    }

    ui->XValue->setValue(CurrentBrush->X*100);
    ui->YValue->setValue(CurrentBrush->Y*100);
    ui->WValue->setValue(CurrentBrush->ZoomFactor*100);
    ui->HValue->setValue(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio*100);

    if (!CurrentBrush->LockGeometry)                        ui->LockGeometryCB->setCurrentIndex(0);
    else if (CurrentBrush->AspectRatio==ProjectGeometry)    ui->LockGeometryCB->setCurrentIndex(2);
    else if (CurrentBrush->AspectRatio==ImageGeometry)      ui->LockGeometryCB->setCurrentIndex(3);
    else                                                        ui->LockGeometryCB->setCurrentIndex(1);

    ui->RotateED->setValue(CurrentBrush->ImageRotation);

    // FilterCorrection
    ui->BrightnessSlider->setValue(CurrentBrush->Brightness);     ui->BrightnessValue->setValue(CurrentBrush->Brightness);
    ui->ContrastSlider->setValue(CurrentBrush->Contrast);         ui->ContrastValue->setValue(CurrentBrush->Contrast);
    ui->GammaSlider->setValue(CurrentBrush->Gamma*100);           ui->GammaValue->setValue(CurrentBrush->Gamma);
    ui->RedSlider->setValue(CurrentBrush->Red);                   ui->RedValue->setValue(CurrentBrush->Red);
    ui->GreenSlider->setValue(CurrentBrush->Green);               ui->GreenValue->setValue(CurrentBrush->Green);
    ui->BlueSlider->setValue(CurrentBrush->Blue);                 ui->BlueValue->setValue(CurrentBrush->Blue);

    ui->BlurSigmaSlider->setValue((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurSigma*10);
    ui->BlurSigmaSB->setValue((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurSigma);
    ui->BlurRadiusSlider->setValue(int((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurRadius));
    ui->BlurRadiusED->setValue(int((CurrentBrush->Video?&CurrentBrush->Video->BrushFileTransform:&CurrentBrush->Image->BrushFileTransform)->BlurRadius));

    ui->FileNameED->setText(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video?CurrentBrush->Video->FileName:"");

    OnOffFilter=CurrentBrush->Video?CurrentBrush->Video->BrushFileTransform.OnOffFilter:CurrentBrush->Image->BrushFileTransform.OnOffFilter;
    ui->TransformationCB->SetCurrentFilter(ui->InteractiveZone->CachedImage,&OnOffFilter);

    ui->InteractiveZone->repaint();

    FLAGSTOPED=false;
}

//====================================================================================================================

void DlgImageCorrection::ChangeBrushDiskFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::ChangeBrushDiskFile");
    QString ActualFilePath=QFileInfo(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName).absolutePath();

    QString NewFile=QFileDialog::getOpenFileName(this,
                                                 QApplication::translate("DlgImageCorrection","Select a file"),
                                                 ActualFilePath,//((cApplicationConfig *)BaseApplicationConfig)->RememberLastDirectories?((cApplicationConfig *)BaseApplicationConfig)->LastMediaPath:"",
                                                 ((cApplicationConfig *)BaseApplicationConfig)->GetFilterForMediaFile(CurrentBrush->Image?cBaseApplicationConfig::IMAGEFILE:cBaseApplicationConfig::VIDEOFILE));
    QApplication::processEvents();
    if (NewFile=="") return;
    AppendPartialUndo(UNDOACTION_EDITZONE_FILE,ui->InteractiveZone,true);
    if (((cApplicationConfig *)BaseApplicationConfig)->RememberLastDirectories) ((cApplicationConfig *)BaseApplicationConfig)->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

    QString NewBrushFileName=QFileInfo(NewFile).absoluteFilePath();
    QString OldBrushFileName=CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName;

    if (CurrentBrush->Image) {
        CurrentBrush->Image->GetInformationFromFile(NewBrushFileName,NULL,NULL);
        if (CurrentBrush->Image->IsValide) ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
        UndoReloadImage=true;
    } else if (CurrentBrush->Video) {
        QString ErrorMessage=QApplication::translate("MainWindow","Format not supported","Error message");
        bool    IsValide=true;
        if (CurrentBrush->Video->GetInformationFromFile(NewBrushFileName,NULL,NULL)&&(CurrentBrush->Video->OpenCodecAndFile())) {
            // Check if file have at least one sound track compatible
            if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(!(
                (CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_S16)||
                (CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_U8)
            ))) {
                ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only audio track with unsigned 8 bits or signed 16 bits sample format","Error message");
                IsValide=false;
            }
            if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->channels>2)) {
                ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only mono or stereo audio track","Error message");
                IsValide=false;
            }
            if (IsValide) ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);

            if (!IsValide) {
                CustomMessageBox(NULL,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
                CurrentBrush->Video->GetInformationFromFile(OldBrushFileName,NULL,NULL);
                CurrentBrush->Video->OpenCodecAndFile();
            } else UndoReloadImage=true;
        }
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_ChTransformationCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_ChTransformationCB");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->OnOffFilter=ui->TransformationCB->GetCurrentFilter();
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_BlurSigmaSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSigmaSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLUR,ui->InteractiveZone,false);
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurSigma=double(Value)/10;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaSB->setValue(Filter->BlurSigma);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_BlurSigmaValueED(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSigmaValueED");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLUR,ui->InteractiveZone,false);
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurSigma=Value;
    ui->BlurSigmaSlider->setValue(Filter->BlurSigma*10);
    ui->BlurSigmaSB->setValue(Filter->BlurSigma);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_BlurRadiusSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurRadiusSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLURRD,ui->InteractiveZone,false);
    cFilterTransformObject *Filter=CurrentBrush->Image?&CurrentBrush->Image->BrushFileTransform:&CurrentBrush->Video->BrushFileTransform;
    Filter->BlurRadius=double(Value);
    ui->BlurRadiusSlider->setValue(int(Filter->BlurRadius));
    ui->BlurRadiusED->setValue(int(Filter->BlurRadius));
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_RulersBT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RulersBT");
    ((cApplicationConfig *)BaseApplicationConfig)->FramingRuler=!((cApplicationConfig *)BaseApplicationConfig)->FramingRuler;
    ui->InteractiveZone->MagneticRuler=((cApplicationConfig *)BaseApplicationConfig)->FramingRuler;
    ui->RulersBT->setIcon(QIcon(((cApplicationConfig *)BaseApplicationConfig)->FramingRuler?QString(ICON_RULER_ON):QString(ICON_RULER_OFF)));
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::s_BrightnessSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BrightnessSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BRIGHTNESS,ui->InteractiveZone,false);
    CurrentBrush->Brightness=Value;
    ui->BrightnessSlider->setValue(CurrentBrush->Brightness);
    ui->BrightnessValue->setValue(CurrentBrush->Brightness);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_ContrastSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_ContrastSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_CONTRAST,ui->InteractiveZone,false);
    CurrentBrush->Contrast=Value;
    ui->ContrastSlider->setValue(CurrentBrush->Contrast);
    ui->ContrastValue->setValue(CurrentBrush->Contrast);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_GammaSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_GammaSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_GAMMA,ui->InteractiveZone,false);
    CurrentBrush->Gamma=double(Value)/100;
    ui->GammaSlider->setValue(CurrentBrush->Gamma*100);
    ui->GammaValue->setValue(CurrentBrush->Gamma);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_GammaValueED(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_GammaValueED");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_GAMMA,ui->InteractiveZone,false);
    CurrentBrush->Gamma=Value;
    ui->GammaSlider->setValue(CurrentBrush->Gamma*100);
    ui->GammaValue->setValue(CurrentBrush->Gamma);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_RedSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RedSliderMoved");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_RED,ui->InteractiveZone,false);
    CurrentBrush->Red=Value;
    ui->RedSlider->setValue(CurrentBrush->Red);
    ui->RedValue->setValue(CurrentBrush->Red);
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::s_GreenSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_GreenSliderMoved");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_GREEN,ui->InteractiveZone,false);
    CurrentBrush->Green=Value;
    ui->GreenSlider->setValue(CurrentBrush->Green);
    ui->GreenValue->setValue(CurrentBrush->Green);
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::s_BlueSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlueSliderMoved");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLUE,ui->InteractiveZone,false);
    CurrentBrush->Blue=Value;
    ui->BlueSlider->setValue(CurrentBrush->Blue);
    ui->BlueValue->setValue(CurrentBrush->Blue);
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::s_LockGeometryCB(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_LockGeometryCB");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_GEOMETRY,ui->InteractiveZone,true);
    switch (Value) {
        case 0 :
            CurrentBrush->LockGeometry=false;
            break;
        case 1 :
            CurrentBrush->LockGeometry=true;
            break;
        case 2 :
            CurrentBrush->LockGeometry=true;
            CurrentBrush->AspectRatio =ProjectGeometry;
            break;
        case 3 :
            CurrentBrush->LockGeometry=true;
            CurrentBrush->AspectRatio =ImageGeometry;
            break;
    }
    RefreshControls();
}

//====================================================================================================================
// Handler for style sheet management
//====================================================================================================================

void DlgImageCorrection::s_FramingStyleBT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_FramingStyleBT");
    if (!CompoObject) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_FRAMING,ui->InteractiveZone,true);
    QString ActualStyle=CompoObject->GetFramingStyle();
    QString Item=((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.PopupCollectionMenu(this,ActualStyle);
    ui->FramingStyleBT->setDown(false);
    if (Item!="") {
        QStringList List;
        ((cApplicationConfig *)BaseApplicationConfig)->StyleImageFramingCollection.StringToStringList(Item,List);
        for (int i=0;i<List.count();i++) {
            if      (List[i].startsWith("X:"))              CurrentBrush->X             =List[i].mid(QString("X:").length()).toDouble();
            else if (List[i].startsWith("Y:"))              CurrentBrush->Y             =List[i].mid(QString("Y:").length()).toDouble();
            else if (List[i].startsWith("ZoomFactor:"))     CurrentBrush->ZoomFactor    =List[i].mid(QString("ZoomFactor:").length()).toDouble();
            else if (List[i].startsWith("LockGeometry:"))   CurrentBrush->LockGeometry  =List[i].mid(QString("LockGeometry:").length()).toInt()==1;
            else if (List[i].startsWith("AspectRatio:"))    CurrentBrush->AspectRatio   =List[i].mid(QString("AspectRatio:").length()).toDouble();
        }
    }
    RefreshControls();
}

//====================================================================================================================
// Handler for interactive zone
//====================================================================================================================

void DlgImageCorrection::s_IntZoneTransformBlocks(double Move_X,double Move_Y,double Scale_X,double Scale_Y) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_IntZoneTransformBlocks");
    AppendPartialUndo(UNDOACTION_INTERACTIVEMOVERESIZE,ui->InteractiveZone,true);

    CurrentBrush->X=CurrentBrush->X+Move_X;
    CurrentBrush->Y=CurrentBrush->Y+Move_Y;
    if (!CurrentBrush->LockGeometry) {
        double NewH=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio+Scale_Y;
        CurrentBrush->AspectRatio=NewH/(CurrentBrush->ZoomFactor+Scale_X);
    }
    CurrentBrush->ZoomFactor=CurrentBrush->ZoomFactor+Scale_X;

    ui->InteractiveZone->Move_X =0;
    ui->InteractiveZone->Move_Y =0;
    ui->InteractiveZone->Scale_X=0;
    ui->InteractiveZone->Scale_Y=0;

    RefreshControls();
}

void DlgImageCorrection::s_DisplayIntZoneTransformBlocks(double Move_X,double Move_Y,double Scale_X,double Scale_Y) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_DisplayIntZoneTransformBlocks");

    double NewX=CurrentBrush->X+Move_X;
    double NewY=CurrentBrush->Y+Move_Y;
    double NewW=CurrentBrush->ZoomFactor+Scale_X;
    double NewH=(CurrentBrush->LockGeometry?(CurrentBrush->ZoomFactor+Scale_X)*CurrentBrush->AspectRatio:CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio+Scale_Y);

    FLAGSTOPED=true;
    ui->XValue->setValue(NewX*100);
    ui->YValue->setValue(NewY*100);
    ui->WValue->setValue(NewW*100);
    ui->HValue->setValue(NewH*100);
    FLAGSTOPED=false;

}
