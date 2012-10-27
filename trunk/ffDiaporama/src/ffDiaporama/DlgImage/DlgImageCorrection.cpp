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

enum UNDOACTION_ID {
    UNDOACTION_INTERACTIVEMOVERESIZE,
    UNDOACTION_EDITZONE_XVALUE,
    UNDOACTION_EDITZONE_YVALUE,
    UNDOACTION_EDITZONE_WVALUE,
    UNDOACTION_EDITZONE_HVALUE,
    UNDOACTION_EDITZONE_ROTATEVALUE,
    UNDOACTION_EDITZONE_FRAMING,
    UNDOACTION_EDITZONE_FILE,
    UNDOACTION_EDITZONE_TRANSFO,
    UNDOACTION_EDITZONE_BLURSHARPENSIGMA,
    UNDOACTION_EDITZONE_BLURSHARPENQUICK,
    UNDOACTION_EDITZONE_BLURSHARPENRADIUS,
    UNDOACTION_EDITZONE_EMBOSSSIGMA,
    UNDOACTION_EDITZONE_EMBOSSRADIUS,
    UNDOACTION_EDITZONE_BRIGHTNESS,
    UNDOACTION_EDITZONE_CONTRAST,
    UNDOACTION_EDITZONE_GAMMA,
    UNDOACTION_EDITZONE_RED,
    UNDOACTION_EDITZONE_GREEN,
    UNDOACTION_EDITZONE_BLUE,
    UNDOACTION_EDITZONE_GEOMETRY,
    UNDOACTION_EDITZONE_SHAPEFORM,
    UNDOACTION_EDITZONE_SPEEDWAVE,
    UNDOACTION_VIDEOPART_STARTPOS,
    UNDOACTION_VIDEOPART_ENDPOS,
    UNDOACTION_VIDEOPART_VOLUME,
    UNDOACTION_VIDEOPART_DEINTERLACE
};

DlgImageCorrection::DlgImageCorrection(cCompositionObject *TheCompoObject,int *TheBackgroundForm,cBrushDefinition *TheCurrentBrush,
                                       int TheVideoPosition,int TheImageGeometry,int TheDefaultSpeedWave,
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
    StopMaj         =false;
    ffDPrjGeometry  =TheImageGeometry;
    CurrentBrush    =TheCurrentBrush;
    CompoObject     =TheCompoObject;
    BackgroundForm  =TheBackgroundForm;
    IsVideo         =(CurrentBrush->Video!=NULL);
    DefaultSpeedWave=TheDefaultSpeedWave;
    ui->InteractiveZone->MagneticRuler=((cApplicationConfig *)BaseApplicationConfig)->FramingRuler;
    ui->InteractiveZone->InitCachedImage(TheCompoObject,(BackgroundForm!=NULL)?(*TheBackgroundForm):1,TheCurrentBrush,TheVideoPosition);
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

    // Set title of dialog
    setWindowTitle(!IsVideo?ffDText(ffDSection_DlgImageCorrection,0):ffDText(ffDSection_DlgImageCorrection,1));

    switch (ffDPrjGeometry) {
        case GEOMETRY_4_3   : ProjectGeometry=double(1440)/double(1920);  break;
        case GEOMETRY_16_9  : ProjectGeometry=double(1080)/double(1920);  break;
        case GEOMETRY_40_17 : ProjectGeometry=double(816)/double(1920);   break;

    }
    ProjectGeometry=QString("%1").arg(ProjectGeometry,0,'e').toDouble();  // Rounded to same number as style managment

    ImageGeometry=IsVideo?qreal(CurrentBrush->Video->ImageHeight)/qreal(CurrentBrush->Video->ImageWidth):qreal(CurrentBrush->Image->ImageHeight)/qreal(CurrentBrush->Image->ImageWidth);
    ImageGeometry=QString("%1").arg(ImageGeometry,0,'e').toDouble();  // Rounded to same number as style managment
    ui->RulersBT->setIcon(QIcon(((cApplicationConfig *)BaseApplicationConfig)->FramingRuler?QString(ICON_RULER_ON):QString(ICON_RULER_OFF)));

    ui->RotateED->setMinimum(-180);
    ui->RotateED->setMaximum(180);

    ui->XValue->setSingleStep(1);  ui->XValue->setRange(-200,200);
    ui->YValue->setSingleStep(1);  ui->YValue->setRange(-200,200);
    ui->WValue->setSingleStep(1);  ui->WValue->setRange(1,200);
    ui->HValue->setSingleStep(1);  ui->HValue->setRange(1,200);

    if (!IsVideo) {
        // If it's an image
        ui->TabWidget->setCurrentIndex(0);
        ui->InteractiveZone->setFocus();
        ui->TabWidget->removeTab(1);    // remove VideoTab
        ui->VideoPositionLabel->setVisible(false);

    } else {

        // If it's a video
        ui->TabWidget->setCurrentIndex(1);
        ui->FileLabel->setPixmap(QPixmap(ICON_MOVIE_FILE));

        // Init embeded widgets
        for (int Factor=150;Factor>=0;Factor-=10) ui->VolumeReductionFactorCB->addItem(QString("%1%").arg(Factor));
        RefreshControls();

        // Define specifique handler for video
        connect(ui->VolumeReductionFactorCB,SIGNAL(currentIndexChanged(int)),this,SLOT(MusicReduceFactorChange(int)));
        connect(ui->DefStartPosBT,SIGNAL(clicked()),this,SLOT(s_DefStartPos()));
        connect(ui->DefEndPosBT,SIGNAL(clicked()),this,SLOT(s_DefEndPos()));
        connect(ui->SeekLeftBt,SIGNAL(clicked()),this,SLOT(s_SeekLeft()));
        connect(ui->SeekRightBt,SIGNAL(clicked()),this,SLOT(s_SeekRight()));
        connect(ui->DeinterlaceBt,SIGNAL(stateChanged(int)),this,SLOT(s_Deinterlace(int)));
        connect(ui->StartPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditStartPos(QTime)));
        connect(ui->EndPosEd,SIGNAL(timeChanged(QTime)),this,SLOT(s_EditEndPos(QTime)));
        connect(ui->VideoPlayer,SIGNAL(SaveImageEvent()),this,SLOT(s_Event_SaveImageEvent()));

        #ifndef LIBAVFILTER
        ui->DeinterlaceBt->setEnabled(false);
        #endif
    }

    // Init combo box Background form
    if (BackgroundForm==NULL) ui->BackgroundFormCB->setEnabled(false); else {
        for (int i=0;i<ShapeFormDefinition.count();i++) if (ShapeFormDefinition.at(i).Enable) ui->BackgroundFormCB->addItem(ShapeFormDefinition.at(i).Name,QVariant(i));
        MakeFormIcon(ui->BackgroundFormCB);
    }

    // Speed wave
    if (DefaultSpeedWave==SPEEDWAVE_DISABLE) ui->SpeedWaveCB->setEnabled(false); else ui->SpeedWaveCB->AddProjectDefault(DefaultSpeedWave);
    connect(ui->SpeedWaveCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_SpeedWaveChanged(int)));

    // Define handler
    connect(ui->TabWidget,SIGNAL(currentChanged(int)),this,SLOT(s_TabWidgetChanged(int)));
    connect(ui->BackgroundFormCB,SIGNAL(itemSelectionHaveChanged()),this,SLOT(s_ShapeBackgroundForm()));
    connect(ui->RotateED,SIGNAL(valueChanged(double)),this,SLOT(s_RotationEDChanged(double)));
    connect(ui->XValue,SIGNAL(valueChanged(double)),this,SLOT(s_XValueEDChanged(double)));
    connect(ui->YValue,SIGNAL(valueChanged(double)),this,SLOT(s_YValueEDChanged(double)));
    connect(ui->WValue,SIGNAL(valueChanged(double)),this,SLOT(s_WValueEDChanged(double)));
    connect(ui->HValue,SIGNAL(valueChanged(double)),this,SLOT(s_HValueEDChanged(double)));
    connect(ui->RotateLeftBT,SIGNAL(clicked()),this,SLOT(s_RotateLeft()));
    connect(ui->RotateRightBT,SIGNAL(clicked()),this,SLOT(s_RotateRight()));
    connect(ui->FramingStyleCB,SIGNAL(itemSelectionHaveChanged()),this,SLOT(s_ItemSelectionHaveChanged()));
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
    connect(ui->WaveAmpSlider,SIGNAL(valueChanged(int)),this,SLOT(s_WaveAmpSliderMoved(int)));
    connect(ui->WaveAmpValue,SIGNAL(valueChanged(int)),this,SLOT(s_WaveAmpSliderMoved(int)));
    connect(ui->WaveAmpResetBT,SIGNAL(clicked()),this,SLOT(s_WaveAmpReset()));
    connect(ui->WaveFreqSlider,SIGNAL(valueChanged(int)),this,SLOT(s_WaveFreqSliderMoved(int)));
    connect(ui->WaveFreqValue,SIGNAL(valueChanged(int)),this,SLOT(s_WaveFreqSliderMoved(int)));
    connect(ui->WaveFreqResetBT,SIGNAL(clicked()),this,SLOT(s_WaveFreqReset()));

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

    connect(ui->FileNameBT,SIGNAL(clicked()),this,SLOT(s_ChangeFile()));
    connect(ui->InteractiveZone,SIGNAL(TransformBlock(qreal,qreal,qreal,qreal)),this,SLOT(s_IntZoneTransformBlocks(qreal,qreal,qreal,qreal)));
    connect(ui->InteractiveZone,SIGNAL(DisplayTransformBlock(qreal,qreal,qreal,qreal)),this,SLOT(s_DisplayIntZoneTransformBlocks(qreal,qreal,qreal,qreal)));

    CurrentFramingStyle=-100;
    UpdateFramingStyleCB();
    RefreshControls();
    //ui->scrollArea->setMinimumWidth(ui->scrollArea->viewport()->width()+20);
    //ui->scrollArea->setFixedWidth(ui->scrollArea->viewport()->width()+20);
}

//====================================================================================================================

void DlgImageCorrection::MakeFormIcon(QComboBox *UICB) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::MakeFormIcon");

    for (int i=0;i<UICB->count();i++) {
        cCompositionObject Object(COMPOSITIONTYPE_BACKGROUND,0,((cApplicationConfig *)BaseApplicationConfig));
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
        Object.DrawCompositionObject(&Painter,1,0,0,UICB->iconSize().width(),UICB->iconSize().height(),true,0,0,NULL,1,1,NULL,false,0,false);
        Painter.end();
        UICB->setItemIcon(i,QIcon(Image));
    }
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
    if (IsVideo) {
        root.setAttribute("StartPos",CurrentBrush->Video->StartPos.toString("HH:mm:ss.zzz"));
        root.setAttribute("EndPos",CurrentBrush->Video->EndPos.toString("HH:mm:ss.zzz"));
        root.setAttribute("SoundVolume",QString("%1").arg(CurrentBrush->SoundVolume,0,'f'));
        root.setAttribute("Deinterlace",CurrentBrush->Deinterlace?"1":"0");                                 // Deinterlace YADIF filter
    }
    Undo->appendChild(root);                                    // Add object to xml document
}

//====================================================================================================================
// Apply Undo : call when user click on Cancel button

void DlgImageCorrection::DoGlobalUndo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::DoGlobalUndo");

    QDomElement root=Undo->documentElement();
    if (root.tagName()=="UNDO-DLG") CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL);
    if (IsVideo) {
        CurrentBrush->Video->StartPos=QTime().fromString(root.attribute("StartPos"));
        CurrentBrush->Video->EndPos  =QTime().fromString(root.attribute("EndPos"));
        CurrentBrush->SoundVolume    =root.attribute("SoundVolume").toDouble();
        CurrentBrush->Deinterlace    =root.attribute("Deinterlace")=="1";
    }
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
    if (IsVideo) {
        root.setAttribute("StartPos",CurrentBrush->Video->StartPos.toString("HH:mm:ss.zzz"));               // Start position (video only)
        root.setAttribute("EndPos",CurrentBrush->Video->EndPos.toString("HH:mm:ss.zzz"));                   // End position (video only)
        root.setAttribute("SoundVolume",QString("%1").arg(CurrentBrush->SoundVolume,0,'f'));                // Volume of soundtrack (for video only)
        root.setAttribute("Deinterlace",CurrentBrush->Deinterlace?"1":"0");                                 // Deinterlace YADIF filter
    }
}

//====================================================================================================================

void DlgImageCorrection::ApplyPartialUndo(int /*ActionType*/,QDomElement root) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::ApplyPartialUndo");


    QString BrushFileName=root.attribute("BrushFileName");
    CurrentBrush->LoadFromXML(root,"UNDO-DLG-OBJECT","",NULL,NULL);
    if (IsVideo) {
        CurrentBrush->Video->StartPos=QTime().fromString(root.attribute("StartPos"));
        CurrentBrush->Video->EndPos  =QTime().fromString(root.attribute("EndPos"));
        CurrentBrush->SoundVolume    =root.attribute("SoundVolume").toDouble();
        CurrentBrush->Deinterlace    =root.attribute("Deinterlace")=="1";
    }
    if (BrushFileName!=((CurrentBrush->Image!=NULL)?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName)) {
        ((cApplicationConfig *)BaseApplicationConfig)->ImagesCache.RemoveVideoObject(BrushFileName);
        if (CurrentBrush->Image) {
            CurrentBrush->Image->Reset();
            CurrentBrush->Image->GetInformationFromFile(BrushFileName,NULL,NULL);
        } else if (CurrentBrush->Video) {
            CurrentBrush->Video->Reset(OBJECTTYPE_VIDEOFILE);
            CurrentBrush->Video->GetInformationFromFile(BrushFileName,NULL,NULL);
        }
        // Redo initialisation of controls
        ImageGeometry=CurrentBrush->Video!=NULL?qreal(CurrentBrush->Video->ImageHeight)/qreal(CurrentBrush->Video->ImageWidth):qreal(CurrentBrush->Image->ImageHeight)/qreal(CurrentBrush->Image->ImageWidth);
        ImageGeometry=QString("%1").arg(ImageGeometry,0,'e').toDouble();  // Rounded to same number as style managment
        ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
        ui->InteractiveZone->RefreshDisplay();
        int OldFramingStyle=CurrentFramingStyle;
        CurrentFramingStyle=-100;
        UpdateFramingStyleCB(true);
        CurrentBrush->ApplyAutoFraming(OldFramingStyle,ProjectGeometry);
        ui->FramingStyleCB->SetCurrentFraming(OldFramingStyle);
    }
    RefreshControls();
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::RestoreWindowState() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::RestoreWindowState");
    QCustomDialog::RestoreWindowState();
    if ((IsVideo)&&(!ui->VideoPlayer->IsValide)) {
        ui->VideoPlayer->StartPlay(CurrentBrush->Video,((cApplicationConfig *)BaseApplicationConfig)->PreviewFPS);
        ui->EndPosEd->setMaximumTime(CurrentBrush->Video->Duration);
        RefreshControls();
    }
}

//====================================================================================================================

void DlgImageCorrection::s_TabWidgetChanged(int NewTab) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_TabWidgetChanged");
    if (NewTab==0) {
        // Switch to image tab
        ui->VideoPlayer->SetPlayerToPause();
        if (IsVideo) ui->InteractiveZone->InitCachedImage(CompoObject,ui->InteractiveZone->BackgroundForm,CurrentBrush,ui->InteractiveZone->VideoPosition);
        ui->InteractiveZone->RefreshDisplay();
    }
    RefreshControls();
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

void DlgImageCorrection::s_DesatReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_DesatReset");
    s_DesatSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_SwirlReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_SwirlReset");
    s_SwirlSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_ImplodeReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_ImplodeReset");
    s_ImplodeSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_WaveAmpReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_WaveAmpReset");
    s_WaveAmpSliderMoved(0);
}

//====================================================================================================================

void DlgImageCorrection::s_WaveFreqReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_WaveFreqReset");
    s_WaveFreqSliderMoved(15);
}

//====================================================================================================================

void DlgImageCorrection::resizeEvent(QResizeEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::resizeEvent");
    //ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
    ui->InteractiveZone->RefreshDisplay();
}

//====================================================================================================================

void DlgImageCorrection::showEvent(QShowEvent *ev) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::showEvent");
    QCustomDialog::showEvent(ev);
    ui->InteractiveZone->RefreshDisplay();
    //ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
    //RefreshControls();
 }

//====================================================================================================================

void DlgImageCorrection::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::DoAccept");

    if (BackgroundForm) *BackgroundForm=ui->InteractiveZone->BackgroundForm;    // not do this for background
}

//====================================================================================================================

void DlgImageCorrection::s_XValueEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_XValueEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_XVALUE,ui->InteractiveZone,false);
    CurrentBrush->X=Value/100;
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_YValueEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_YValueEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_YVALUE,ui->InteractiveZone,false);
    CurrentBrush->Y=Value/100;
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_WValueEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_WValueEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_WVALUE,ui->InteractiveZone,false);
    if (CurrentBrush->LockGeometry) {
        CurrentBrush->ZoomFactor=Value/100;
    } else {
        qreal newH=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio*ui->InteractiveZone->Hyp.Image;
        CurrentBrush->ZoomFactor=Value/100;
        qreal newW=CurrentBrush->ZoomFactor*ui->InteractiveZone->Hyp.Image;
        CurrentBrush->AspectRatio=newH/newW;
    }
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_HValueEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_HValueEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_HVALUE,ui->InteractiveZone,false);
    qreal newH=(Value/100)*ui->InteractiveZone->Hyp.Image;
    if (CurrentBrush->LockGeometry) {
        qreal newW=newH/CurrentBrush->AspectRatio;
        CurrentBrush->ZoomFactor=newW/ui->InteractiveZone->Hyp.Image;
    } else {
        qreal newW=CurrentBrush->ZoomFactor*ui->InteractiveZone->Hyp.Image;
        CurrentBrush->AspectRatio=newH/newW;
    }
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_RotationEDChanged(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RotationEDChanged");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEVALUE,ui->InteractiveZone,false);
    if (Value<-180) Value=360-Value;
    if (Value>180)  Value=-360-Value;
    CurrentBrush->ImageRotation=Value;
    CurrentFramingStyle=ui->FramingStyleCB->GetCurrentFraming();
    CurrentBrush->ApplyAutoFraming(CurrentFramingStyle,ProjectGeometry);
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
    UpdateFramingStyleCB();
    ui->FramingStyleCB->SetCurrentFraming(CurrentFramingStyle);
}

//====================================================================================================================

void DlgImageCorrection::s_RotateLeft() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RotateLeft");
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEVALUE,ui->InteractiveZone,true);
    qreal Value=(int((CurrentBrush->ImageRotation-90)/90)*90);
    if (Value<=-180) Value=360-Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::s_RotateRight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_RotateRight");
    AppendPartialUndo(UNDOACTION_EDITZONE_ROTATEVALUE,ui->InteractiveZone,true);
    qreal Value=(int((CurrentBrush->ImageRotation+90)/90)*90);
    if (Value>180) Value=-360+Value;
    ui->RotateED->setValue(Value);
}

//====================================================================================================================

void DlgImageCorrection::UpdateFramingStyleCB(bool Reset) {
    if (ui->InteractiveZone->CachedImage) {
        ui->FramingStyleCB->X=CurrentBrush->X;
        ui->FramingStyleCB->Y=CurrentBrush->Y;
        ui->FramingStyleCB->ZoomFactor=CurrentBrush->ZoomFactor;
        ui->FramingStyleCB->AspectRatio=CurrentBrush->AspectRatio;
        ui->FramingStyleCB->PrepareFramingStyleTable(Reset,FILTERFRAMING_ALL,CurrentBrush,ui->InteractiveZone->CachedImage,ui->InteractiveZone->BackgroundForm,ProjectGeometry);
    }
    int NewFramingStyle=CurrentBrush->GetCurrentFramingStyle(ProjectGeometry);
    if (CurrentFramingStyle!=NewFramingStyle) {
        CurrentFramingStyle=NewFramingStyle;
        ui->FramingStyleCB->SetCurrentFraming(CurrentFramingStyle);
    }
}

//====================================================================================================================

void DlgImageCorrection::RefreshControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::RefreshControls");
    if (FLAGSTOPED) return;
    FLAGSTOPED=true;

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
    ui->WaveAmpSlider->setValue(int(CurrentBrush->WaveAmp));        ui->WaveAmpValue->setValue(int(CurrentBrush->WaveAmp));
    ui->WaveFreqSlider->setValue(int(CurrentBrush->WaveFreq));      ui->WaveFreqValue->setValue(int(CurrentBrush->WaveFreq));

    // Speed wave
    if (DefaultSpeedWave!=SPEEDWAVE_DISABLE) ui->SpeedWaveCB->SetCurrentValue(CurrentBrush->ImageSpeedWave);

    // File
    ui->FileNameED->setText(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video?CurrentBrush->Video->FileName:"");

    // Video TAB
    if ((IsVideo)&&(ui->TabWidget->currentIndex()==1)) {
        StopMaj=true;
        QTime Duration=QTime(0,0,0,0).addMSecs(CurrentBrush->Video->StartPos.msecsTo(CurrentBrush->Video->EndPos));
        ui->DeinterlaceBt->setChecked(CurrentBrush->Deinterlace);
        ui->ActualDuration->setText(Duration.toString("hh:mm:ss.zzz"));
        ui->StartPosEd->setMaximumTime(CurrentBrush->Video->EndPos);    ui->StartPosEd->setTime(CurrentBrush->Video->StartPos);
        ui->EndPosEd->setMinimumTime(CurrentBrush->Video->StartPos);    ui->EndPosEd->setTime(CurrentBrush->Video->EndPos);
        ui->VolumeReductionFactorCB->setCurrentIndex(ui->VolumeReductionFactorCB->findText(QString("%1%").arg(int(CurrentBrush->SoundVolume*100))));
        ui->VideoPlayer->SetStartEndPos(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                        QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos)-QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),
                                        -1,0,-1,0);
        ui->VideoPlayer->Deinterlace=CurrentBrush->Deinterlace;
        StopMaj=false;
    }

    if (IsVideo) {
        QString VideoPosition=QApplication::translate("DlgImageCorrection","Video position :")+CurrentBrush->Video->StartPos.addMSecs(ui->InteractiveZone->VideoPosition).toString("hh:mm:ss.zzz");
        ui->VideoPositionLabel->setText(VideoPosition);
    }

    ui->InteractiveZone->repaint();

    FLAGSTOPED=false;
}

//====================================================================================================================

void DlgImageCorrection::s_ChangeFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_ChangeFile");
    ui->VideoPlayer->SetPlayerToPause();

    bool    IsValide=true;
    QString ActualFilePath=QFileInfo(CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName).absolutePath();
    QString NewFile=QFileDialog::getOpenFileName(this,ffDText(ffDSection_CommonInfoMsg,0),
                                                 ActualFilePath,//((cApplicationConfig *)BaseApplicationConfig)->RememberLastDirectories?((cApplicationConfig *)BaseApplicationConfig)->LastMediaPath:"",
                                                 ((cApplicationConfig *)BaseApplicationConfig)->GetFilterForMediaFile(CurrentBrush->Image?cBaseApplicationConfig::IMAGEFILE:cBaseApplicationConfig::VIDEOFILE));
    QApplication::processEvents();
    if (NewFile=="") return;
    AppendPartialUndo(UNDOACTION_EDITZONE_FILE,ui->InteractiveZone,true);
    if (((cApplicationConfig *)BaseApplicationConfig)->RememberLastDirectories) ((cApplicationConfig *)BaseApplicationConfig)->LastMediaPath=QFileInfo(NewFile).absolutePath();     // Keep folder for next use

    QString NewBrushFileName=QFileInfo(NewFile).absoluteFilePath();
    QString OldBrushFileName=CurrentBrush->Image?CurrentBrush->Image->FileName:CurrentBrush->Video->FileName;

    ((cApplicationConfig *)BaseApplicationConfig)->ImagesCache.RemoveVideoObject(OldBrushFileName);
    if (!IsVideo) {
        // Image
        CurrentBrush->Image->Reset();
        CurrentBrush->Image->GetInformationFromFile(NewBrushFileName,NULL,NULL);
        IsValide=CurrentBrush->Image->IsValide;
        if (IsValide) CurrentBrush->Image->GetFullInformationFromFile();

    } else {
        // Video
        QString ErrorMessage=ffDText(ffDSection_CommonErrorMsg,0);
        CurrentBrush->Video->Reset(OBJECTTYPE_VIDEOFILE);
        if (CurrentBrush->Video->GetInformationFromFile(NewBrushFileName,NULL,NULL)&&(CurrentBrush->Video->OpenCodecAndFile())) {
            // Check if file have at least one sound track compatible
            if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(!(
                (CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_S16)||
                (CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_U8)
            ))) {
                ErrorMessage=ErrorMessage+"\n"+ffDText(ffDSection_CommonErrorMsg,1);
                IsValide=false;
            }
            if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->channels>2)) {
                ErrorMessage=ErrorMessage+"\n"+ffDText(ffDSection_CommonErrorMsg,2);
                IsValide=false;
            }
            if (!IsValide) {
                CustomMessageBox(NULL,QMessageBox::Critical,ffDText(ffDSection_CommonInfoMsg,1),NewFile+"\n\n"+ErrorMessage,QMessageBox::Close);
                CurrentBrush->Video->GetInformationFromFile(OldBrushFileName,NULL,NULL);
            } else CurrentBrush->Video->OpenCodecAndFile();
        }
    }
    if (IsValide) {
        // Redo initialisation of controls
        ImageGeometry=IsVideo?qreal(CurrentBrush->Video->ImageHeight)/qreal(CurrentBrush->Video->ImageWidth):qreal(CurrentBrush->Image->ImageHeight)/qreal(CurrentBrush->Image->ImageWidth);
        ImageGeometry=QString("%1").arg(ImageGeometry,0,'e').toDouble();  // Rounded to same number as style managment
        ui->InteractiveZone->InitCachedImage(ui->InteractiveZone->CompoObject,ui->InteractiveZone->BackgroundForm,ui->InteractiveZone->CurrentBrush,ui->InteractiveZone->VideoPosition);
        ui->InteractiveZone->RefreshDisplay();
        int OldFramingStyle=CurrentFramingStyle;
        CurrentFramingStyle=-100;
        UpdateFramingStyleCB(true);
        CurrentBrush->ApplyAutoFraming(OldFramingStyle,ProjectGeometry);
        ui->FramingStyleCB->SetCurrentFraming(OldFramingStyle);
        UndoReloadImage=true;
        if (IsVideo) {
            ui->TabWidget->setCurrentIndex(1);
            ui->VideoPlayer->SeekPlayer(0);
            ui->VideoPlayer->StartPlay(CurrentBrush->Video,((cApplicationConfig *)BaseApplicationConfig)->PreviewFPS);
            ui->EndPosEd->setMaximumTime(CurrentBrush->Video->Duration);
        }
    }
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Gray_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Gray_Changed");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    if (((CurrentBrush->OnOffFilter & FilterGray)!=0)==ui->FilterOnOff_GrayCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_GrayCB->isChecked())    CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterGray;
        else                                    CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterGray;
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Equalize_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Equalize_Changed");
    if (((CurrentBrush->OnOffFilter & FilterEqualize)!=0)==ui->FilterOnOff_EqualizeCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_EqualizeCB->isChecked())    CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterEqualize;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterEqualize;
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Despeckle_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Despeckle_Changed");
    if (((CurrentBrush->OnOffFilter & FilterDespeckle)!=0)==ui->FilterOnOff_DespeckleCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_DespeckleCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterDespeckle;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterDespeckle;
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Negative_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Negative_Changed");
    if (((CurrentBrush->OnOffFilter & FilterNegative)!=0)==ui->FilterOnOff_NegativeCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_NegativeCB->isChecked())    CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterNegative;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterNegative;
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Emboss_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Emboss_Changed");
    if (((CurrentBrush->OnOffFilter & FilterEmboss)!=0)==ui->FilterOnOff_EmbossCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_EmbossCB->isChecked())      CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterEmboss;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterEmboss;
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Edge_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Edge_Changed");
    if (((CurrentBrush->OnOffFilter & FilterEdge)!=0)==ui->FilterOnOff_EdgeCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_EdgeCB->isChecked())        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterEdge;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterEdge;
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Antialias_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Antialias_Changed");
    if (((CurrentBrush->OnOffFilter & FilterAntialias)!=0)==ui->FilterOnOff_AntialiasCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_AntialiasCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterAntialias;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterAntialias;
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Normalize_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Normalize_Changed");
    if (((CurrentBrush->OnOffFilter & FilterNormalize)!=0)==ui->FilterOnOff_NormalizeCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_NormalizeCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterNormalize;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterNormalize;
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Charcoal_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Charcoal_Changed");
    if (((CurrentBrush->OnOffFilter & FilterCharcoal)!=0)==ui->FilterOnOff_CharcoalCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_CharcoalCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterCharcoal;
        else                                        CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterCharcoal;
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================

void DlgImageCorrection::s_OnOffFilter_Oil_Changed(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_OnOffFilter_Oil_Changed");
    if (((CurrentBrush->OnOffFilter & FilterOil)!=0)==ui->FilterOnOff_OilCB->isChecked()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_TRANSFO,ui->InteractiveZone,true);
    if (ui->FilterOnOff_OilCB->isChecked())   CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter | FilterOil;
        else                                  CurrentBrush->OnOffFilter=CurrentBrush->OnOffFilter & ~FilterOil;
    ui->InteractiveZone->RefreshDisplay();
    RefreshControls();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================
// Blur/Sharpen
//====================================================================================================================


void DlgImageCorrection::s_BlurSharpenTypeChanged(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSharpenTypeChanged");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLURSHARPENSIGMA,ui->InteractiveZone,false);
    CurrentBrush->TypeBlurSharpen=Value;
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
    UpdateFramingStyleCB();
    RefreshControls();
}

void DlgImageCorrection::s_BlurSharpenSigmaSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSharpenSigmaSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLURSHARPENSIGMA,ui->InteractiveZone,false);
    CurrentBrush->GaussBlurSharpenSigma=qreal(Value)/10;
    ui->BlurSharpenSigmaSlider->setValue(CurrentBrush->GaussBlurSharpenSigma*10);
    ui->BlurSharpenSigmaSB->setValue(CurrentBrush->GaussBlurSharpenSigma);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
    UpdateFramingStyleCB();
}

void DlgImageCorrection::s_BlurSharpenSigmaValueED(double Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSharpenSigmaValueED");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLURSHARPENSIGMA,ui->InteractiveZone,false);
    CurrentBrush->GaussBlurSharpenSigma=Value;
    ui->BlurSharpenSigmaSlider->setValue(CurrentBrush->GaussBlurSharpenSigma*10);
    ui->BlurSharpenSigmaSB->setValue(CurrentBrush->GaussBlurSharpenSigma);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
    UpdateFramingStyleCB();
}

void DlgImageCorrection::s_QuickBlurSharpenSigmaSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_QuickBlurSharpenSigmaSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLURSHARPENQUICK,ui->InteractiveZone,false);
    CurrentBrush->QuickBlurSharpenSigma=qreal(Value);
    ui->QuickBlurSharpenSigmaSlider->setValue(int(CurrentBrush->QuickBlurSharpenSigma));
    ui->QuickBlurSharpenSigmaSB->setValue(int(CurrentBrush->QuickBlurSharpenSigma));
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
    UpdateFramingStyleCB();
}

void DlgImageCorrection::s_BlurSharpenRadiusSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSharpenRadiusSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLURSHARPENRADIUS,ui->InteractiveZone,false);
    CurrentBrush->BlurSharpenRadius=qreal(Value);
    ui->BlurSharpenRadiusSlider->setValue(int(CurrentBrush->BlurSharpenRadius));
    ui->BlurSharpenRadiusED->setValue(int(CurrentBrush->BlurSharpenRadius));
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
    UpdateFramingStyleCB();
}

void DlgImageCorrection::s_BlurSharpenSigmaReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSharpenSigmaReset");
    s_BlurSharpenSigmaSliderMoved(0);
}


void DlgImageCorrection::s_QuickBlurSharpenSigmaReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_QuickBlurSharpenSigmaReset");
    s_QuickBlurSharpenSigmaSliderMoved(0);
}

void DlgImageCorrection::s_BlurSharpenRadiusReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_BlurSharpenRadiusReset");
    s_BlurSharpenRadiusSliderMoved(5);
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
    UpdateFramingStyleCB();
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
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_GammaSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_GammaSliderMoved");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_GAMMA,ui->InteractiveZone,false);
    CurrentBrush->Gamma=qreal(Value)/100;
    ui->GammaSlider->setValue(CurrentBrush->Gamma*100);
    ui->GammaValue->setValue(CurrentBrush->Gamma);
    ui->InteractiveZone->RefreshDisplay();
    FLAGSTOPSPIN=false;
    UpdateFramingStyleCB();
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
    UpdateFramingStyleCB();
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
    UpdateFramingStyleCB();
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
    UpdateFramingStyleCB();
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
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_DesatSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_DesatSliderMoved");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLUE,ui->InteractiveZone,false);
    CurrentBrush->Desat=double(Value)/100;
    ui->DesatSlider->setValue(int(CurrentBrush->Desat*100));
    ui->DesatValue->setValue(int(CurrentBrush->Desat*100));
    ui->InteractiveZone->RefreshDisplay();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_SwirlSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_SwirlSliderMoved");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLUE,ui->InteractiveZone,false);
    CurrentBrush->Swirl=double(Value);
    ui->SwirlSlider->setValue(int(CurrentBrush->Swirl));
    ui->SwirlValue->setValue(int(CurrentBrush->Swirl));
    ui->InteractiveZone->RefreshDisplay();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_ImplodeSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_ImplodeSliderMoved");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLUE,ui->InteractiveZone,false);
    CurrentBrush->Implode=double(Value)/100;
    ui->ImplodeSlider->setValue(int(CurrentBrush->Implode*100));
    ui->ImplodeValue->setValue(int(CurrentBrush->Implode*100));
    ui->InteractiveZone->RefreshDisplay();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_WaveAmpSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_WaveAmpSliderMoved");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLUE,ui->InteractiveZone,false);
    CurrentBrush->WaveAmp=double(Value);
    ui->WaveAmpSlider->setValue(int(CurrentBrush->WaveAmp));
    ui->WaveAmpValue->setValue(int(CurrentBrush->WaveAmp));
    ui->InteractiveZone->RefreshDisplay();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_WaveFreqSliderMoved(int Value) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_WaveFreqSliderMoved");
    if (FLAGSTOPED) return;
    AppendPartialUndo(UNDOACTION_EDITZONE_BLUE,ui->InteractiveZone,false);
    CurrentBrush->WaveFreq=double(Value);
    ui->WaveFreqSlider->setValue(int(CurrentBrush->WaveFreq));
    ui->WaveFreqValue->setValue(int(CurrentBrush->WaveFreq));
    ui->InteractiveZone->RefreshDisplay();
    UpdateFramingStyleCB();
}

//====================================================================================================================

void DlgImageCorrection::s_ShapeBackgroundForm() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_ShapeBackgroundForm");
    AppendPartialUndo(UNDOACTION_EDITZONE_SHAPEFORM,ui->BackgroundFormCB,false);
    ui->InteractiveZone->BackgroundForm=ui->BackgroundFormCB->GetCurrentFrameShape();
    ui->InteractiveZone->RefreshDisplay();
    UpdateFramingStyleCB(true);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_ItemSelectionHaveChanged() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_ItemSelectionHaveChanged");
    if ((FLAGSTOPSPIN)||(FLAGSTOPED)) return;
    if (CurrentFramingStyle==ui->FramingStyleCB->GetCurrentFraming()) return;
    FLAGSTOPSPIN=true;
    AppendPartialUndo(UNDOACTION_EDITZONE_FRAMING,ui->InteractiveZone,true);
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
    UpdateFramingStyleCB();
    FLAGSTOPSPIN=false;
}

//====================================================================================================================
// Handler for interactive zone
//====================================================================================================================

void DlgImageCorrection::s_IntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_IntZoneTransformBlocks");
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
    UpdateFramingStyleCB();
}

void DlgImageCorrection::s_DisplayIntZoneTransformBlocks(qreal Move_X,qreal Move_Y,qreal Scale_X,qreal Scale_Y) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_DisplayIntZoneTransformBlocks");

    qreal NewX=CurrentBrush->X+Move_X;
    qreal NewY=CurrentBrush->Y+Move_Y;
    qreal NewW=CurrentBrush->ZoomFactor+Scale_X;
    qreal NewH=(CurrentBrush->LockGeometry?(CurrentBrush->ZoomFactor+Scale_X)*CurrentBrush->AspectRatio:CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio+Scale_Y);

    FLAGSTOPED=true;
    ui->XValue->setValue(NewX*100);
    ui->YValue->setValue(NewY*100);
    ui->WValue->setValue(NewW*100);
    ui->HValue->setValue(NewH*100);
    FLAGSTOPED=false;
}

//====================================================================================================================
// Handler for video controls
//====================================================================================================================

void DlgImageCorrection::s_Deinterlace(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_Deinterlace");
    if (!IsVideo) return;
    if (CurrentBrush->Deinterlace!=ui->DeinterlaceBt->isChecked()) {
        AppendPartialUndo(UNDOACTION_VIDEOPART_DEINTERLACE,ui->VolumeReductionFactorCB,true);
        CurrentBrush->Deinterlace=ui->DeinterlaceBt->isChecked();
        RefreshControls();
    }
}

//====================================================================================================================

void DlgImageCorrection::s_Event_SaveImageEvent() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_Event_SaveImageEvent");
    if (!IsVideo) return;
    ui->VideoPlayer->SetPlayerToPause();
    QString OutputFileName=((cApplicationConfig *)BaseApplicationConfig)->LastCaptureImage;
    QString Filter="JPG (*.jpg)";
    if (!OutputFileName.endsWith(QDir::separator())) OutputFileName=OutputFileName+QDir::separator();
    OutputFileName=OutputFileName+QApplication::translate("MainWindow","Capture image");
    OutputFileName=QFileDialog::getSaveFileName(this,QApplication::translate("MainWindow","Select destination file"),OutputFileName,"PNG (*.png);;JPG (*.jpg)",&Filter);
    if (OutputFileName!="") {
        if (((cApplicationConfig *)BaseApplicationConfig)->RememberLastDirectories) ((cApplicationConfig *)BaseApplicationConfig)->LastCaptureImage=QFileInfo(OutputFileName).absolutePath();     // Keep folder for next use
        if ((Filter.toLower().indexOf("png")!=-1)&&(!OutputFileName.endsWith(".png"))) OutputFileName=OutputFileName+".png";
        if ((Filter.toLower().indexOf("jpg")!=-1)&&(!OutputFileName.endsWith(".jpg"))) OutputFileName=OutputFileName+".jpg";

        QImage *Image=CurrentBrush->Video->ImageAt(false,ui->VideoPlayer->ActualPosition,0,NULL,CurrentBrush->Deinterlace,1,false,true);
        Image->save(OutputFileName,0,100);
        delete Image;
    }
}

//====================================================================================================================

void DlgImageCorrection::MusicReduceFactorChange(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::MusicReduceFactorChange");
    if ((!IsVideo)||(StopMaj)) return;
    AppendPartialUndo(UNDOACTION_VIDEOPART_VOLUME,ui->VolumeReductionFactorCB,true);
    QString Volume=ui->VolumeReductionFactorCB->currentText();
    if (Volume!="") Volume=Volume.left(Volume.length()-1);  // Remove %
    CurrentBrush->SoundVolume=double(Volume.toInt())/100;
}

//====================================================================================================================

void DlgImageCorrection::s_DefStartPos() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_DefStartPos");
    if ((!IsVideo)||(StopMaj)) return;
    AppendPartialUndo(UNDOACTION_VIDEOPART_STARTPOS,ui->StartPosEd,true);
    CurrentBrush->Video->StartPos=ui->VideoPlayer->GetCurrentPos();
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_EditStartPos(QTime NewValue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_EditStartPos");
    if ((!IsVideo)||(StopMaj)) return;
    AppendPartialUndo(UNDOACTION_VIDEOPART_STARTPOS,ui->StartPosEd,false);
    CurrentBrush->Video->StartPos=NewValue;
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_DefEndPos() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_DefEndPos");
    if ((!IsVideo)||(StopMaj)) return;
    AppendPartialUndo(UNDOACTION_VIDEOPART_ENDPOS,ui->EndPosEd,true);
    CurrentBrush->Video->EndPos=ui->VideoPlayer->GetCurrentPos();
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_EditEndPos(QTime NewValue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_EditEndPos");
    if ((!IsVideo)||(StopMaj)) return;
    AppendPartialUndo(UNDOACTION_VIDEOPART_ENDPOS,ui->EndPosEd,false);
    CurrentBrush->Video->EndPos=NewValue;
    ui->EndPosEd->setTime(CurrentBrush->Video->EndPos);
    RefreshControls();
}

//====================================================================================================================

void DlgImageCorrection::s_SeekLeft() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_SeekLeft");
    if (!IsVideo) return;
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos));
    ui->VideoPlayer->SetPlayerToPause();
}

//====================================================================================================================

void DlgImageCorrection::s_SeekRight() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_SeekRight");
    if (!IsVideo) return;
    ui->VideoPlayer->SeekPlayer(QTime(0,0,0,0).msecsTo(CurrentBrush->Video->EndPos));
    ui->VideoPlayer->SetPlayerToPause();
}

//====================================================================================================================

void DlgImageCorrection::s_SpeedWaveChanged(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgImageCorrection::s_SpeedWaveChanged");
    AppendPartialUndo(UNDOACTION_EDITZONE_SPEEDWAVE,ui->SpeedWaveCB,true);
    CurrentBrush->ImageSpeedWave=ui->SpeedWaveCB->GetCurrentValue();
    RefreshControls();
}
