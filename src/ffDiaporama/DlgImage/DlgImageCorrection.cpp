/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <domledom@laposte.net>

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
#include "DlgFileExplorer/DlgFileExplorer.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSplashScreen>

#include "cImgInteractiveZone.h"

#define ICON_RULER_ON                       ":/img/ruler_ok.png"
#define ICON_RULER_OFF                      ":/img/ruler_ko.png"
#define ICON_GEOMETRY_LOCKED                ":/img/Geometry_Lock.png"
#define ICON_GEOMETRY_UNLOCKED              ":/img/Geometry_Unlock.png"
#define ICON_GEOMETRY_PROJECT               ":/img/Geometry_ProjectLock.png"
#define ICON_GEOMETRY_IMAGE                 ":/img/Geometry_ImageLock.png"

#define ICON_IMAGE_FILE                     ":/img/Image_File.png"
#define ICON_MOVIE_FILE                     ":/img/Video_File.png"

DlgImageCorrection::DlgImageCorrection(cCompositionObject *TheCompoObject,int *TheBackgroundForm,cBrushDefinition *TheCurrentBrush,
                                       int TheVideoPosition,ffd_GEOMETRY TheProjectGeometry,int TheDefaultSpeedWave,
                                       cBaseApplicationConfig *ApplicationConfig,QWidget *parent):
                                       QCustomDialog(ApplicationConfig,parent),ui(new Ui::DlgImageCorrection) {

    ui->setupUi(this);

    VideoWidget     =NULL;

    OkBt            =ui->OKBT;
    CancelBt        =ui->CancelBt;
    HelpBt          =ui->HelpBt;
    HelpFile        ="0110";
    UndoBt          =ui->UndoBT;
    UndoReloadImage =false;
    StopMaj      =false;
    StopMaj    =false;
    StopMaj         =false;
    ffDPrjGeometry  =TheProjectGeometry;
    CurrentBrush    =TheCurrentBrush;
    CompoObject     =TheCompoObject;
    BackgroundForm  =TheBackgroundForm;
    DefaultSpeedWave=TheDefaultSpeedWave;
    ui->InteractiveZone->MagneticRuler=ApplicationConfig->FramingRuler;
    ui->InteractiveZone->InitCachedImage(TheCompoObject,(BackgroundForm!=NULL)?(*TheBackgroundForm):1,TheCurrentBrush,TheVideoPosition);
}

//====================================================================================================================

DlgImageCorrection::~DlgImageCorrection() {
    delete ui;  // Deleting this make deletion of scene and all included object
}

//====================================================================================================================
// Initialise dialog

void DlgImageCorrection::DoInitDialog() {
    //*******************************
    // Retire le filtre wave car trop de problème de transparence pour le moment !
    //*******************************

    ui->WaveAmpSlider->setVisible(false);
    ui->WaveAmpValue->setVisible(false);
    ui->WaveAmpResetBT->setVisible(false);
    ui->WaveFreqSlider->setVisible(false);
    ui->WaveFreqValue->setVisible(false);
    ui->WaveFreqResetBT->setVisible(false);
    ui->Wavelabel1->setVisible(false);
    ui->Wavelabel2->setVisible(false);
    ui->Wavelabel3->setVisible(false);

    //*******************************

    // Set title of dialog
    switch (CurrentBrush->MediaObject->ObjectType) {
        case OBJECTTYPE_VIDEOFILE: setWindowTitle(QApplication::translate("DlgSlideProperties","Correct or reframe image",                  "Action title in slide edit dialog + dialog title of image edit dialog")); break;
        case OBJECTTYPE_IMAGEFILE: setWindowTitle(QApplication::translate("DlgSlideProperties","Correct, reframe or cut video",             "Action title in slide edit dialog + dialog title of image edit dialog")); break;
        case OBJECTTYPE_GMAPSMAP:  setWindowTitle(QApplication::translate("DlgSlideProperties","Correct, reframe or cut a Google Maps map", "Action title in slide edit dialog + dialog title of image edit dialog")); break;
        default: break;
    }

    // Get Project Geometry
    switch (ffDPrjGeometry) {
        case GEOMETRY_4_3:      ProjectGeometry=double(1440)/double(1920);  break;
        case GEOMETRY_40_17:    ProjectGeometry=double(816)/double(1920);   break;
        case GEOMETRY_SQUARE:   ProjectGeometry=1;                          break;
        case GEOMETRY_16_9:
        default:                ProjectGeometry=double(1080)/double(1920);  break;

    }
    ProjectGeometry=GetDoubleValue(QString("%1").arg(ProjectGeometry,0,'e'));  // Rounded to same number as style managment

    // Get Image Geometry
    ImageGeometry=qreal(CurrentBrush->MediaObject->ImageHeight)/qreal(CurrentBrush->MediaObject->ImageWidth);
    ImageGeometry=GetDoubleValue(QString("%1").arg(ImageGeometry,0,'e'));  // Rounded to same number as style managment


    ui->RulersBT->setIcon(QIcon(ApplicationConfig->FramingRuler?QString(ICON_RULER_ON):QString(ICON_RULER_OFF)));

    ui->RotateED->setMinimum(-180);
    ui->RotateED->setMaximum(180);

    ui->XValue->setSingleStep(1);  ui->XValue->setRange(-200,200);
    ui->YValue->setSingleStep(1);  ui->YValue->setRange(-200,200);
    ui->WValue->setSingleStep(1);  ui->WValue->setRange(1,200);
    ui->HValue->setSingleStep(1);  ui->HValue->setRange(1,200);

    bool AllowChangeFile=false;
    switch (CurrentBrush->MediaObject->ObjectType) {
        case OBJECTTYPE_IMAGEFILE:      ui->TabWidget->setCurrentIndex(0);  AllowChangeFile=true;   break;
        case OBJECTTYPE_IMAGEVECTOR:    ui->TabWidget->setCurrentIndex(0);  AllowChangeFile=true;   break;
        case OBJECTTYPE_IMAGECLIPBOARD: ui->TabWidget->setCurrentIndex(0);  AllowChangeFile=false;  break;
        case OBJECTTYPE_GMAPSMAP:       ui->TabWidget->setCurrentIndex(1);  AllowChangeFile=false;  break;
        case OBJECTTYPE_VIDEOFILE:      CreateVideoTag();                   AllowChangeFile=true;   break;
        default: break; // To avoid warning
    }

    if (AllowChangeFile) {
        ui->VideoPositionLabel->setVisible(CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE);
        ui->FileLabel->setPixmap(CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE?QPixmap(ICON_MOVIE_FILE):QPixmap(ICON_IMAGE_FILE));
        connect(ui->FileNameBT,SIGNAL(clicked()),this,SLOT(s_ChangeFile()));    // Define specifique handler for changing file
    } else {
        ui->VideoPositionLabel->setVisible(false);
        ui->FileNameBT->setVisible(false);
        ui->FileNameED->setVisible(false);
        ui->FileLabel->setVisible(false);
    }

    if (BackgroundForm==NULL) {
        // Framing Style
        ui->FramingStyleCB->setVisible(false);
        ui->FramingStyleLabel->setVisible(false);
        // Background form
        ui->BackgroundFormCB->setVisible(false);
        ui->BackgroundFormLabel->setVisible(false);
        // Speed wave
        ui->SpeedWaveCB->setVisible(false);
        ui->SpeedWaveLabel->setVisible(false);
        ui->SpeedWaveSection->setVisible(false);
    } else {
        // Background form
        for (int i=0;i<ShapeFormDefinition.count();i++) if (ShapeFormDefinition.at(i).Enable) ui->BackgroundFormCB->addItem(ShapeFormDefinition.at(i).Name,QVariant(i));
        MakeFormIcon(ui->BackgroundFormCB);
        // Speed wave
        if (DefaultSpeedWave==SPEEDWAVE_DISABLE) ui->SpeedWaveCB->setEnabled(false); else ui->SpeedWaveCB->AddProjectDefault(DefaultSpeedWave);
        connect(ui->SpeedWaveCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_SpeedWaveChanged(int)));
    }

    // Define common handler
    connect(ui->TabWidget,SIGNAL(currentChanged(int)),this,SLOT(s_TabWidgetChanged(int)));
    connect(ui->BackgroundFormCB,SIGNAL(itemSelectionHaveChanged()),this,SLOT(s_ShapeBackgroundForm()));
    connect(ui->RotateED,SIGNAL(valueChanged(double)),this,SLOT(s_RotationEDChanged(double)));
    connect(ui->XValue,SIGNAL(valueChanged(double)),this,SLOT(s_XValueEDChanged(double)));
    connect(ui->YValue,SIGNAL(valueChanged(double)),this,SLOT(s_YValueEDChanged(double)));
    connect(ui->WValue,SIGNAL(valueChanged(double)),this,SLOT(s_WValueEDChanged(double)));
    connect(ui->HValue,SIGNAL(valueChanged(double)),this,SLOT(s_HValueEDChanged(double)));
    connect(ui->RotateLeftBT,SIGNAL(clicked()),this,SLOT(s_RotateLeft()));
    connect(ui->RotateRightBT,SIGNAL(clicked()),this,SLOT(s_RotateRight()));
    connect(ui->FramingStyleCB,SIGNAL(itemSelectionHaveChanged()),this,SLOT(s_FramingStyleChanged()));
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
    connect(ui->DesatSlider,SIGNAL(valueChanged(int)),this,SLOT(s_DesatSliderMoved(int)));
    connect(ui->DesatValue,SIGNAL(valueChanged(int)),this,SLOT(s_DesatSliderMoved(int)));
    connect(ui->DesatResetBT,SIGNAL(clicked()),this,SLOT(s_DesatReset()));

    connect(ui->SwirlSlider,SIGNAL(valueChanged(int)),this,SLOT(s_SwirlSliderMoved(int)));
    connect(ui->SwirlValue,SIGNAL(valueChanged(int)),this,SLOT(s_SwirlSliderMoved(int)));
    connect(ui->SwirlResetBT,SIGNAL(clicked()),this,SLOT(s_SwirlReset()));
    connect(ui->ImplodeSlider,SIGNAL(valueChanged(int)),this,SLOT(s_ImplodeSliderMoved(int)));
    connect(ui->ImplodeValue,SIGNAL(valueChanged(int)),this,SLOT(s_ImplodeSliderMoved(int)));
    connect(ui->ImplodeResetBT,SIGNAL(clicked()),this,SLOT(s_ImplodeReset()));

    connect(ui->FilterOnOff_GrayCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Gray_Changed(int)));
    connect(ui->FilterOnOff_EqualizeCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Equalize_Changed(int)));
    connect(ui->FilterOnOff_DespeckleCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Despeckle_Changed(int)));
    connect(ui->FilterOnOff_NegativeCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Negative_Changed(int)));
    connect(ui->FilterOnOff_EmbossCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Emboss_Changed(int)));
    connect(ui->FilterOnOff_EdgeCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Edge_Changed(int)));
    connect(ui->FilterOnOff_AntialiasCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Antialias_Changed(int)));
    connect(ui->FilterOnOff_NormalizeCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Normalize_Changed(int)));
    connect(ui->FilterOnOff_CharcoalCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Charcoal_Changed(int)));
    connect(ui->FilterOnOff_OilCB,SIGNAL(stateChanged(int)),this,SLOT(s_OnOffFilter_Oil_Changed(int)));

    connect(ui->BlurSharpenTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_BlurSharpenTypeChanged(int)));
    connect(ui->BlurSharpenRadiusSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlurSharpenRadiusSliderMoved(int)));
    connect(ui->BlurSharpenRadiusED,SIGNAL(valueChanged(int)),this,SLOT(s_BlurSharpenRadiusSliderMoved(int)));
    connect(ui->BlurSharpenSigmaSlider,SIGNAL(valueChanged(int)),this,SLOT(s_BlurSharpenSigmaSliderMoved(int)));
    connect(ui->BlurSharpenSigmaSB,SIGNAL(valueChanged(double)),this,SLOT(s_BlurSharpenSigmaValueED(double)));
    connect(ui->QuickBlurSharpenSigmaSlider,SIGNAL(valueChanged(int)),this,SLOT(s_QuickBlurSharpenSigmaSliderMoved(int)));
    connect(ui->QuickBlurSharpenSigmaSB,SIGNAL(valueChanged(int)),this,SLOT(s_QuickBlurSharpenSigmaSliderMoved(int)));
    connect(ui->QuickBlurSharpenSigmaResetBT,SIGNAL(clicked()),this,SLOT(s_QuickBlurSharpenSigmaReset()));
    connect(ui->BlurSharpenSigmaResetBT,SIGNAL(clicked()),this,SLOT(s_BlurSharpenSigmaReset()));
    connect(ui->BlurSharpenRadiusResetBT,SIGNAL(clicked()),this,SLOT(s_BlurSharpenRadiusReset()));

    connect(ui->InteractiveZone,SIGNAL(TransformBlock(qreal,qreal,qreal,qreal)),this,SLOT(s_IntZoneTransformBlocks(qreal,qreal,qreal,qreal)));
    connect(ui->InteractiveZone,SIGNAL(DisplayTransformBlock(qreal,qreal,qreal,qreal)),this,SLOT(s_DisplayIntZoneTransformBlocks(qreal,qreal,qreal,qreal)));

    CurrentFramingStyle=-100;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::AppendPartialUndo(int ActionType,QWidget *FocusWindow,bool ForceAdd) {
    QCustomDialog::AppendPartialUndo(ActionType,FocusWindow,ForceAdd,ui->TabWidget->currentWidget());
}

//====================================================================================================================

void DlgImageCorrection::DoPartialUndo() {
    if (UndoDataList.count()==0) {
        if (Undo!=NULL) DoGlobalUndo();
    } else {
        sUndoData Data=UndoDataList.last();
        UndoDataList.removeLast();
        QDomDocument DomDocument("UNDO");
        if (DomDocument.setContent(Data.Data,true)) {
            QDomElement root=DomDocument.documentElement();
            if (root.tagName()=="UNDO-DATA") {
                if (Data.FocusTab) ui->TabWidget->setCurrentIndex(ui->TabWidget->indexOf(Data.FocusTab));
                ApplyPartialUndo(Data.ActionType,root);
            }
        }
        if (Data.FocusWindow!=NULL) Data.FocusWindow->setFocus();
    }
    if (UndoBt) UndoBt->setEnabled(UndoDataList.count()>0);
}


//====================================================================================================================

void DlgImageCorrection::CreateVideoTag() {
    QHBoxLayout *VideoLayout=new QHBoxLayout(ui->TabWidget);
    VideoLayout->setSpacing(0);
    VideoLayout->setObjectName(QStringLiteral("VideoLayout"));
    VideoLayout->setContentsMargins(0,0,0,0);
    VideoWidget=new wgt_QEditVideo();
    VideoWidget->setObjectName(QStringLiteral("TabVideo"));
    VideoWidget->DoInitWidget(this,CurrentBrush);
    VideoWidget->DoInitDialog();
    VideoLayout->addWidget(VideoWidget);
    QIcon VideoIcon;
    VideoIcon.addFile(":/img/EditMovie.png",QSize(),QIcon::Normal,QIcon::Off);
    ui->TabWidget->addTab(VideoWidget,VideoIcon,QString());
    ui->TabWidget->setTabText(ui->TabWidget->indexOf(VideoWidget),QStringLiteral(""));
    ui->TabWidget->setCurrentIndex(ui->TabWidget->indexOf(VideoWidget));
}

//====================================================================================================================

void DlgImageCorrection::MakeFormIcon(QComboBox *UICB) {
    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,ApplicationConfig,this);
        Object.x                        =0;
        Object.y                        =0;
        Object.w                        =1;
        Object.h                        =1;
        Object.BackgroundForm           =UICB->itemData(i).toInt();
        Object.Opacity                  =4;
        Object.PenSize                  =1;
        Object.PenStyle                 =Qt::SolidLine;
        Object.PenColor                 ="#000000";
        Object.BackgroundBrush->ColorD  ="#FFFFFF";
        Object.BackgroundBrush->BrushType=BRUSHTYPE_SOLID;
        QPixmap  Image(UICB->iconSize());
        QPainter Painter;
        Painter.begin(&Image);
        Painter.fillRect(QRect(0,0,UICB->iconSize().width(),UICB->iconSize().height()),"#ffffff");
        Object.DrawCompositionObject(NULL,&Painter,1,UICB->iconSize().width(),UICB->iconSize().height(),true,0,NULL,1,1,NULL,false,0,false);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
}

//====================================================================================================================
// Initiale Undo

void DlgImageCorrection::PrepareGlobalUndo() {
    // Save objects before modification for cancel button
    UndoBrushFileName=CurrentBrush->MediaObject->FileName();

    Undo=new QDomDocument(APPLICATION_NAME);
    QDomElement root=Undo->createElement("UNDO-DLG");           // Create xml document and root
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT","",true,NULL);    // Save object
    if (CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE) {
        root.setAttribute("StartPos",   ((cVideoFile *)CurrentBrush->MediaObject)->StartPos.toString("HH:mm:ss.zzz"));
        root.setAttribute("EndPos",     ((cVideoFile *)CurrentBrush->MediaObject)->EndPos.toString("HH:mm:ss.zzz"));
        root.setAttribute("SoundVolume",QString("%1").arg(CurrentBrush->SoundVolume,0,'f'));
        root.setAttribute("Deinterlace",CurrentBrush->Deinterlace?"1":"0");                                 // Deinterlace YADIF filter
    }
    Undo->appendChild(root);                                    // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgImageCorrection::DoGlobalUndo() {
    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL);
    if ((CurrentBrush->MediaObject)&&(CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE)) {
        ((cVideoFile *)CurrentBrush->MediaObject)->StartPos=QTime().fromString(root.attribute("StartPos"));
        ((cVideoFile *)CurrentBrush->MediaObject)->EndPos  =QTime().fromString(root.attribute("EndPos"));
        CurrentBrush->SoundVolume                           =GetDoubleValue(root,"SoundVolume");
        CurrentBrush->Deinterlace                           =root.attribute("Deinterlace")=="1";
    }
    if (UndoReloadImage) CurrentBrush->MediaObject->GetInformationFromFile(UndoBrushFileName,NULL,NULL,-1);
}

//====================================================================================================================

void DlgImageCorrection::PreparePartialUndo(int /*ActionType*/,QDomElement root) {
    QString BrushFileName=CurrentBrush->MediaObject->FileName();
    root.setAttribute("BrushFileName",BrushFileName);
    root.setAttribute("BackgroundForm",ui->InteractiveZone->BackgroundForm);
    CurrentBrush->SaveToXML(root,"UNDO-DLG-OBJECT","",true,NULL);    // Save object

    if (CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE) {
        root.setAttribute("StartPos",       ((cVideoFile *)CurrentBrush->MediaObject)->StartPos.toString("HH:mm:ss.zzz"));             // Start position (video only)
        root.setAttribute("EndPos",     ((cVideoFile *)CurrentBrush->MediaObject)->EndPos.toString("HH:mm:ss.zzz"));                   // End position (video only)
        root.setAttribute("SoundVolume",QString("%1").arg(CurrentBrush->SoundVolume,0,'f'));                                            // Volume of soundtrack (for video only)
        root.setAttribute("Deinterlace",CurrentBrush->Deinterlace?"1":"0");                                                             // Deinterlace YADIF filter
    }
}

//====================================================================================================================

void DlgImageCorrection::ApplyPartialUndo(int /*ActionType*/,QDomElement root) {
    QString BrushFileName=root.attribute("BrushFileName");
    ui->InteractiveZone->BackgroundForm=root.attribute("BackgroundForm").toInt();
    CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL);

    if (CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE) {
        ((cVideoFile *)CurrentBrush->MediaObject)->StartPos=QTime().fromString(root.attribute("StartPos"));
        ((cVideoFile *)CurrentBrush->MediaObject)->EndPos  =QTime().fromString(root.attribute("EndPos"));
        CurrentBrush->SoundVolume                          =GetDoubleValue(root,"SoundVolume");
        CurrentBrush->Deinterlace                          =root.attribute("Deinterlace")=="1";
    }

    if (BrushFileName!=CurrentBrush->MediaObject->FileName()) {
        ApplicationConfig->ImagesCache.RemoveImageObject(CurrentBrush->MediaObject->FileKey);
        if (CurrentBrush->MediaObject->ObjectType!=OBJECTTYPE_VIDEOFILE) CurrentBrush->MediaObject->Reset();
            else ((cVideoFile *)CurrentBrush->MediaObject)->Reset(OBJECTTYPE_VIDEOFILE);
        CurrentBrush->MediaObject->GetInformationFromFile(BrushFileName,NULL,NULL,-1);

        // Redo initialisation of controls
        ImageGeometry=qreal(CurrentBrush->MediaObject->ImageHeight)/qreal(CurrentBrush->MediaObject->ImageWidth);
        ImageGeometry=GetDoubleValue(QString("%1").arg(ImageGeometry,0,'e'));  // Rounded to same number as style managment
        ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
        ui->InteractiveZone->RefreshDisplay();
        int OldFramingStyle=CurrentFramingStyle;
        CurrentFramingStyle=-100;
        CurrentBrush->ApplyAutoFraming(OldFramingStyle,ProjectGeometry);
        ui->FramingStyleCB->SetCurrentFraming(OldFramingStyle);
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_TabWidgetChanged(int NewTab) {
    //if ((VideoWidget)&&(NewTab==ui->TabWidget->indexOf(VideoWidget))) VideoWidget->WinFocus(); else
    if ((VideoWidget)&&(NewTab!=ui->TabWidget->indexOf(VideoWidget))) VideoWidget->LostFocus();

    if (NewTab==0) ui->InteractiveZone->setFocus();

    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::resizeEvent(QResizeEvent *ev) {
    QCustomDialog::resizeEvent(ev);
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::showEvent(QShowEvent *ev) {
    QCustomDialog::showEvent(ev);
    ui->InteractiveZone->RefreshDisplay();
 }

//====================================================================================================================

bool DlgImageCorrection::DoAccept() {
    if (BackgroundForm) *BackgroundForm=ui->InteractiveZone->BackgroundForm;    // not do this for background
    return true;
}

//====================================================================================================================

void DlgImageCorrection::s_XValueEDChanged(double Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->X=Value/100;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_YValueEDChanged(double Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Y=Value/100;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_WValueEDChanged(double Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    if (CurrentBrush->LockGeometry) {
        CurrentBrush->ZoomFactor=Value/100;
    } else {
        qreal newH=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio*ui->InteractiveZone->Hyp.Image;
        CurrentBrush->ZoomFactor=Value/100;
        qreal newW=CurrentBrush->ZoomFactor*ui->InteractiveZone->Hyp.Image;
        CurrentBrush->AspectRatio=newH/newW;
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_HValueEDChanged(double Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    qreal newH=(Value/100)*ui->InteractiveZone->Hyp.Image;
    if (CurrentBrush->LockGeometry) {
        qreal newW=newH/CurrentBrush->AspectRatio;
        CurrentBrush->ZoomFactor=newW/ui->InteractiveZone->Hyp.Image;
    } else {
        qreal newW=CurrentBrush->ZoomFactor*ui->InteractiveZone->Hyp.Image;
        CurrentBrush->AspectRatio=newH/newW;
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotationEDChanged(double Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    if (Value<-180) Value=360-Value;
    if (Value>180)  Value=-360-Value;
    CurrentBrush->ImageRotation=Value;
    CurrentFramingStyle=ui->FramingStyleCB->GetCurrentFraming();
    CurrentBrush->ApplyAutoFraming(CurrentFramingStyle,ProjectGeometry);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotateLeft() {
    if (StopMaj) return;
    qreal Value=(int((CurrentBrush->ImageRotation-90)/90)*90);
    if (Value<=-180) Value=360-Value;
    ui->RotateED->setValue(Value);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RotateRight() {
    if (StopMaj) return;
    qreal Value=(int((CurrentBrush->ImageRotation+90)/90)*90);
    if (Value>180) Value=-360+Value;
    ui->RotateED->setValue(Value);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::RefreshControls() {
    if (StopMaj) return;
    StopMaj=true;

    //***********************************************

    // Framing and form
    ui->BackgroundFormCB->PrepareFrameShapeTable(true,0,ui->InteractiveZone->BackgroundForm);
    ui->BackgroundFormCB->SetCurrentFrameShape(ui->InteractiveZone->BackgroundForm);
    ui->XValue->setValue(CurrentBrush->X*100);
    ui->YValue->setValue(CurrentBrush->Y*100);
    ui->WValue->setValue(CurrentBrush->ZoomFactor*100);
    ui->HValue->setValue(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio*100);

    // Image rotation
    ui->RotateED->setValue(CurrentBrush->ImageRotation);

    // Luminosity, contrast and gamma
    ui->BrightnessSlider->setValue(CurrentBrush->Brightness);     ui->BrightnessValue->setValue(CurrentBrush->Brightness);
    ui->GammaSlider->setValue(CurrentBrush->Gamma*100);           ui->GammaValue->setValue(CurrentBrush->Gamma);
    ui->FilterOnOff_NormalizeCB->setChecked((CurrentBrush->OnOffFilter & FilterNormalize)!=0);
    ui->ContrastSlider->setValue(CurrentBrush->Contrast);
    ui->ContrastValue->setValue(CurrentBrush->Contrast);
    ui->ContrastSlider->setVisible(!ui->FilterOnOff_NormalizeCB->isChecked());
    ui->ContrastValue->setVisible(!ui->FilterOnOff_NormalizeCB->isChecked());
    ui->ContrastResetBT->setVisible(!ui->FilterOnOff_NormalizeCB->isChecked());
    ui->ContrastLabel->setVisible(!ui->FilterOnOff_NormalizeCB->isChecked());

    // Image enhancement
    ui->BlurSharpenTypeCB->setCurrentIndex(CurrentBrush->TypeBlurSharpen);
    ui->BlurSharpenSigmaSlider->setValue(CurrentBrush->GaussBlurSharpenSigma*10);
    ui->BlurSharpenSigmaSB->setValue(CurrentBrush->GaussBlurSharpenSigma);
    ui->QuickBlurSharpenSigmaSlider->setValue(CurrentBrush->QuickBlurSharpenSigma); ui->BlurSharpenSigmaSlider->setVisible(ui->BlurSharpenTypeCB->currentIndex()==1);
    ui->QuickBlurSharpenSigmaSB->setValue(CurrentBrush->QuickBlurSharpenSigma);     ui->BlurSharpenSigmaSB->setVisible(ui->BlurSharpenTypeCB->currentIndex()==1);
    ui->QuickBlurSharpenSigmaSlider->setValue(CurrentBrush->QuickBlurSharpenSigma); ui->QuickBlurSharpenSigmaSlider->setVisible(ui->BlurSharpenTypeCB->currentIndex()==0);
    ui->QuickBlurSharpenSigmaSB->setValue(CurrentBrush->QuickBlurSharpenSigma);     ui->QuickBlurSharpenSigmaSB->setVisible(ui->BlurSharpenTypeCB->currentIndex()==0);
    ui->BlurSharpenSigmaResetBT->setVisible(ui->BlurSharpenTypeCB->currentIndex()==1);
    ui->QuickBlurSharpenSigmaResetBT->setVisible(ui->BlurSharpenTypeCB->currentIndex()==0);
    ui->BlurSharpenRadiusSlider->setValue(int(CurrentBrush->BlurSharpenRadius));
    ui->BlurSharpenRadiusED->setValue(int(CurrentBrush->BlurSharpenRadius));
    ui->FilterOnOff_DespeckleCB->setChecked((CurrentBrush->OnOffFilter & FilterDespeckle)!=0);
    ui->FilterOnOff_AntialiasCB->setChecked((CurrentBrush->OnOffFilter & FilterAntialias)!=0);

    // Color enhancement
    ui->FilterOnOff_GrayCB->setChecked(     (CurrentBrush->OnOffFilter & FilterGray)!=0);
    ui->FilterOnOff_EqualizeCB->setChecked( (CurrentBrush->OnOffFilter & FilterEqualize)!=0);
    ui->RedSlider->setValue(CurrentBrush->Red);                   ui->RedValue->setValue(CurrentBrush->Red);
    ui->GreenSlider->setValue(CurrentBrush->Green);               ui->GreenValue->setValue(CurrentBrush->Green);
    ui->BlueSlider->setValue(CurrentBrush->Blue);                 ui->BlueValue->setValue(CurrentBrush->Blue);
    ui->DesatSlider->setValue(int(CurrentBrush->Desat*100));      ui->DesatValue->setValue(int(CurrentBrush->Desat*100));

    // Artistic effects
    ui->FilterOnOff_NegativeCB->setChecked( (CurrentBrush->OnOffFilter & FilterNegative)!=0);
    ui->FilterOnOff_EmbossCB->setChecked(   (CurrentBrush->OnOffFilter & FilterEmboss)!=0);
    ui->FilterOnOff_EdgeCB->setChecked(     (CurrentBrush->OnOffFilter & FilterEdge)!=0);
    ui->FilterOnOff_CharcoalCB->setChecked( (CurrentBrush->OnOffFilter & FilterCharcoal)!=0);
    ui->FilterOnOff_OilCB->setChecked(      (CurrentBrush->OnOffFilter & FilterOil)!=0);

    // Image distorsion
    ui->SwirlSlider->setValue(int(CurrentBrush->Swirl));            ui->SwirlValue->setValue(int(CurrentBrush->Swirl));
    ui->ImplodeSlider->setValue(int(CurrentBrush->Implode*100));    ui->ImplodeValue->setValue(int(CurrentBrush->Implode*100));

    // Speed wave
    if (DefaultSpeedWave!=SPEEDWAVE_DISABLE) ui->SpeedWaveCB->SetCurrentValue(CurrentBrush->ImageSpeedWave);

    // File
    ui->FileNameED->setText(CurrentBrush->MediaObject->FileName());

    // Embeded widget
    if ((VideoWidget)&&(ui->TabWidget->currentWidget()==VideoWidget)) VideoWidget->RefreshControls(false);

    if (ui->VideoPositionLabel->isVisible()) {
        QString VideoPosition=QApplication::translate("DlgImageCorrection","Video position :")+((cVideoFile *)CurrentBrush->MediaObject)->StartPos.addMSecs(ui->InteractiveZone->VideoPosition).toString("hh:mm:ss.zzz");
        ui->VideoPositionLabel->setText(VideoPosition);
    }

    if (ui->TabWidget->currentIndex()==0) {
        if (ui->InteractiveZone->CachedImage) {
            ui->FramingStyleCB->X=CurrentBrush->X;
            ui->FramingStyleCB->Y=CurrentBrush->Y;
            ui->FramingStyleCB->ZoomFactor=CurrentBrush->ZoomFactor;
            ui->FramingStyleCB->AspectRatio=CurrentBrush->AspectRatio;
            ui->FramingStyleCB->PrepareFramingStyleTable(true,FILTERFRAMING_ALL,CurrentBrush,ui->InteractiveZone->CachedImage,ui->InteractiveZone->BackgroundForm,ProjectGeometry);
        }
        int NewFramingStyle=CurrentBrush->GetCurrentFramingStyle(ProjectGeometry);
        if (CurrentFramingStyle!=NewFramingStyle) {
            CurrentFramingStyle=NewFramingStyle;
            ui->FramingStyleCB->SetCurrentFraming(CurrentFramingStyle);
        }
        ui->FramingStyleCB->MakeIcons();
        ui->InteractiveZone->RefreshDisplay();
    }

    StopMaj=false;
}

//====================================================================================================================

void DlgImageCorrection::s_ChangeFile() {
    bool        IsValide=true;
    QString     ActualFilePath=QFileInfo(CurrentBrush->MediaObject->FileName()).absolutePath();
    QStringList FileList;
    QString     NewFile="";

    BROWSER_TYPE_ID BrowserType=CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_IMAGEFILE?BROWSER_TYPE_IMAGEONLY:
                                CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_IMAGEVECTOR?BROWSER_TYPE_IMAGEVECTOR:
                                CurrentBrush->MediaObject->ObjectType==OBJECTTYPE_VIDEOFILE?BROWSER_TYPE_VIDEOONLY:
                                BROWSER_TYPE_MEDIAFILES;
    if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->SettingsTable->SetTextValue(QString("%1_path").arg(BrowserTypeDef[BrowserType].BROWSERString),QDir::toNativeSeparators(ActualFilePath));
    DlgFileExplorer Dlg(BrowserType,false,false,false,QApplication::translate("CommonInfoMsg","Select a file"),ApplicationConfig,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) {
        FileList=Dlg.GetCurrentSelectedFiles();
        if (FileList.count()==1) NewFile=FileList.at(0);
    }

    if (NewFile=="") return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);

    QString NewBrushFileName=QFileInfo(NewFile).absoluteFilePath();
    QString OldBrushFileName=CurrentBrush->MediaObject->FileName();

    ApplicationConfig->ImagesCache.RemoveImageObject(CurrentBrush->MediaObject->FileKey);
    if (CurrentBrush->MediaObject->ObjectType!=OBJECTTYPE_VIDEOFILE) CurrentBrush->MediaObject->Reset();
        else ((cVideoFile *)CurrentBrush->MediaObject)->Reset(OBJECTTYPE_VIDEOFILE);

    IsValide=(CurrentBrush->MediaObject->GetInformationFromFile(NewBrushFileName,NULL,NULL,-1)&&(CurrentBrush->MediaObject->CheckFormatValide(this)));

    // It error reload previous
    if (!IsValide) IsValide=(CurrentBrush->MediaObject->GetInformationFromFile(OldBrushFileName,NULL,NULL,-1)&&(CurrentBrush->MediaObject->CheckFormatValide(this)));

    if (IsValide) {
        // Redo initialisation of controls
        ImageGeometry=qreal(CurrentBrush->MediaObject->ImageHeight)/qreal(CurrentBrush->MediaObject->ImageWidth);
        ImageGeometry=GetDoubleValue(QString("%1").arg(ImageGeometry,0,'e'));  // Rounded to same number as style managment
        ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
        ui->InteractiveZone->RefreshDisplay();
        int OldFramingStyle=CurrentFramingStyle;
        CurrentFramingStyle=-100;
        CurrentBrush->ApplyAutoFraming(OldFramingStyle,ProjectGeometry);
        ui->FramingStyleCB->SetCurrentFraming(OldFramingStyle);
        UndoReloadImage=true;
    }

    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Gray_Changed(int) {
    if (StopMaj) return;
    if (((CurrentBrush->OnOffFilter & FilterGray)!=0)==ui->FilterOnOff_GrayCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_GrayCB->isChecked())    CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterGray;
        else                                    CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterGray;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Equalize_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterEqualize)!=0)==ui->FilterOnOff_EqualizeCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_EqualizeCB->isChecked())    CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterEqualize;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterEqualize;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Despeckle_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterDespeckle)!=0)==ui->FilterOnOff_DespeckleCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_DespeckleCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterDespeckle;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterDespeckle;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Negative_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterNegative)!=0)==ui->FilterOnOff_NegativeCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_NegativeCB->isChecked())    CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterNegative;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterNegative;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Emboss_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterEmboss)!=0)==ui->FilterOnOff_EmbossCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_EmbossCB->isChecked())      CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterEmboss;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterEmboss;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Edge_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterEdge)!=0)==ui->FilterOnOff_EdgeCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_EdgeCB->isChecked())        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterEdge;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterEdge;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Antialias_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterAntialias)!=0)==ui->FilterOnOff_AntialiasCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_AntialiasCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterAntialias;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterAntialias;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Normalize_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterNormalize)!=0)==ui->FilterOnOff_NormalizeCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_NormalizeCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterNormalize;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterNormalize;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Charcoal_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterCharcoal)!=0)==ui->FilterOnOff_CharcoalCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_CharcoalCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterCharcoal;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterCharcoal;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Oil_Changed(int) {
    if (((CurrentBrush->OnOffFilter & FilterOil)!=0)==ui->FilterOnOff_OilCB->isChecked()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    if (ui->FilterOnOff_OilCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterOil;
        else                                  CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterOil;
    RefreshControls();
}

//====================================================================================================================
// Blur/Sharpen
//====================================================================================================================


void DlgImageCorrection::s_BlurSharpenTypeChanged(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->TypeBlurSharpen=Value;
    RefreshControls();
}

void DlgImageCorrection::s_BlurSharpenSigmaSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->GaussBlurSharpenSigma=qreal(Value)/10;
    ui->BlurSharpenSigmaSlider->setValue(CurrentBrush->GaussBlurSharpenSigma*10);
    ui->BlurSharpenSigmaSB->setValue(CurrentBrush->GaussBlurSharpenSigma);
    RefreshControls();
}

void DlgImageCorrection::s_BlurSharpenSigmaValueED(double Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->GaussBlurSharpenSigma=Value;
    ui->BlurSharpenSigmaSlider->setValue(CurrentBrush->GaussBlurSharpenSigma*10);
    ui->BlurSharpenSigmaSB->setValue(CurrentBrush->GaussBlurSharpenSigma);
    RefreshControls();
}

void DlgImageCorrection::s_QuickBlurSharpenSigmaSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->QuickBlurSharpenSigma=qreal(Value);
    ui->QuickBlurSharpenSigmaSlider->setValue(int(CurrentBrush->QuickBlurSharpenSigma));
    ui->QuickBlurSharpenSigmaSB->setValue(int(CurrentBrush->QuickBlurSharpenSigma));
    RefreshControls();
}

void DlgImageCorrection::s_BlurSharpenRadiusSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->BlurSharpenRadius=qreal(Value);
    ui->BlurSharpenRadiusSlider->setValue(int(CurrentBrush->BlurSharpenRadius));
    ui->BlurSharpenRadiusED->setValue(int(CurrentBrush->BlurSharpenRadius));
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RulersBT() {
    ApplicationConfig->FramingRuler=!ApplicationConfig->FramingRuler;
    ui->InteractiveZone->MagneticRuler=ApplicationConfig->FramingRuler;
    ui->RulersBT->setIcon(QIcon(ApplicationConfig->FramingRuler?QString(ICON_RULER_ON):QString(ICON_RULER_OFF)));
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::s_BrightnessSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Brightness=Value;
    ui->BrightnessSlider->setValue(CurrentBrush->Brightness);
    ui->BrightnessValue->setValue(CurrentBrush->Brightness);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_ContrastSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Contrast=Value;
    ui->ContrastSlider->setValue(CurrentBrush->Contrast);
    ui->ContrastValue->setValue(CurrentBrush->Contrast);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_GammaSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Gamma=qreal(Value)/100;
    ui->GammaSlider->setValue(CurrentBrush->Gamma*100);
    ui->GammaValue->setValue(CurrentBrush->Gamma);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_GammaValueED(double Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Gamma=Value;
    ui->GammaSlider->setValue(CurrentBrush->Gamma*100);
    ui->GammaValue->setValue(CurrentBrush->Gamma);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_RedSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Red=Value;
    ui->RedSlider->setValue(CurrentBrush->Red);
    ui->RedValue->setValue(CurrentBrush->Red);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_GreenSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Green=Value;
    ui->GreenSlider->setValue(CurrentBrush->Green);
    ui->GreenValue->setValue(CurrentBrush->Green);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_BlueSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Blue=Value;
    ui->BlueSlider->setValue(CurrentBrush->Blue);
    ui->BlueValue->setValue(CurrentBrush->Blue);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_DesatSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Desat=double(Value)/100;
    ui->DesatSlider->setValue(int(CurrentBrush->Desat*100));
    ui->DesatValue->setValue(int(CurrentBrush->Desat*100));
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_SwirlSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Swirl=double(Value);
    ui->SwirlSlider->setValue(int(CurrentBrush->Swirl));
    ui->SwirlValue->setValue(int(CurrentBrush->Swirl));
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_ImplodeSliderMoved(int Value) {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,false);
    CurrentBrush->Implode=double(Value)/100;
    ui->ImplodeSlider->setValue(int(CurrentBrush->Implode*100));
    ui->ImplodeValue->setValue(int(CurrentBrush->Implode*100));
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_ShapeBackgroundForm() {
    if (StopMaj) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->BackgroundFormCB,false);
    ui->InteractiveZone->BackgroundForm=ui->BackgroundFormCB->GetCurrentFrameShape();
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_FramingStyleChanged() {
    if (StopMaj) return;
    if (CurrentFramingStyle==ui->FramingStyleCB->GetCurrentFraming()) return;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->InteractiveZone,true);
    CurrentFramingStyle=ui->FramingStyleCB->GetCurrentFraming();
    switch (ui->FramingStyleCB->GetCurrentFraming()) {
        case -1 :
            CurrentFramingStyle=CurrentBrush->GetCurrentFramingStyle(ProjectGeometry);
            ui->FramingStyleCB->SetCurrentFraming(CurrentFramingStyle);
            break;
        case AUTOFRAMING_CUSTOMUNLOCK   :
            CurrentBrush->LockGeometry=false;
            CurrentBrush->X           =ui->FramingStyleCB->X;
            CurrentBrush->Y           =ui->FramingStyleCB->Y;
            CurrentBrush->AspectRatio =ui->FramingStyleCB->AspectRatio;
            CurrentBrush->ZoomFactor  =ui->FramingStyleCB->ZoomFactor;
            break;
        case AUTOFRAMING_CUSTOMLOCK     :
            CurrentBrush->LockGeometry=true;
            CurrentBrush->X           =ui->FramingStyleCB->X;
            CurrentBrush->Y           =ui->FramingStyleCB->Y;
            CurrentBrush->AspectRatio =ui->FramingStyleCB->AspectRatio;
            CurrentBrush->ZoomFactor  =ui->FramingStyleCB->ZoomFactor;
            break;
        case AUTOFRAMING_CUSTOMIMGLOCK  :
            CurrentBrush->LockGeometry=true;
            CurrentBrush->X           =ui->FramingStyleCB->X;
            CurrentBrush->Y           =ui->FramingStyleCB->Y;
            CurrentBrush->AspectRatio =ImageGeometry;
            CurrentBrush->ZoomFactor  =ui->FramingStyleCB->ZoomFactor;
            break;
        case AUTOFRAMING_CUSTOMPRJLOCK  :
            CurrentBrush->LockGeometry=true;
            CurrentBrush->X           =ui->FramingStyleCB->X;
            CurrentBrush->Y           =ui->FramingStyleCB->Y;
            CurrentBrush->AspectRatio =ProjectGeometry;
            CurrentBrush->ZoomFactor  =ui->FramingStyleCB->ZoomFactor;
            break;

        default : CurrentBrush->ApplyAutoFraming(ui->FramingStyleCB->GetCurrentFraming(),ProjectGeometry); break;
    }
    CurrentFramingStyle=CurrentBrush->GetCurrentFramingStyle(ProjectGeometry);
    RefreshControls();
}

//====================================================================================================================
// Handler for interactive zone
//====================================================================================================================

void DlgImageCorrection::s_IntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y) {
    AppendPartialUndo(UNDOACTION_INTERACTIVEMOVERESIZE,ui->InteractiveZone,true);

    CurrentBrush->X=CurrentBrush->X+Move_X;
    CurrentBrush->Y=CurrentBrush->Y+Move_Y;
    if (!CurrentBrush->LockGeometry) {
        qreal NewH=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio+Scale_Y;
        CurrentBrush->AspectRatio=NewH/(CurrentBrush->ZoomFactor+Scale_X);
    }
    CurrentBrush->ZoomFactor=CurrentBrush->ZoomFactor+Scale_X;

    ui->InteractiveZone->Move_X =0;
    ui->InteractiveZone->Move_Y =0;
    ui->InteractiveZone->Scale_X=0;
    ui->InteractiveZone->Scale_Y=0;

    RefreshControls();
}

void DlgImageCorrection::s_DisplayIntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y) {
    qreal NewX=CurrentBrush->X+Move_X;
    qreal NewY=CurrentBrush->Y+Move_Y;
    qreal NewW=CurrentBrush->ZoomFactor+Scale_X;
    qreal NewH=(CurrentBrush->LockGeometry?(CurrentBrush->ZoomFactor+Scale_X)*CurrentBrush->AspectRatio:CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio+Scale_Y);

    StopMaj=true;
    ui->XValue->setValue(NewX*100);
    ui->YValue->setValue(NewY*100);
    ui->WValue->setValue(NewW*100);
    ui->HValue->setValue(NewH*100);
    StopMaj=false;
}

//====================================================================================================================

void DlgImageCorrection::s_SpeedWaveChanged(int) {
    if (StopMaj) return;
    StopMaj=true;
    AppendPartialUndo(UNDOACTION_IMAGEEDITZONE,ui->SpeedWaveCB,true);
    CurrentBrush->ImageSpeedWave=ui->SpeedWaveCB->GetCurrentValue();
    RefreshControls();
    StopMaj=false;
}
