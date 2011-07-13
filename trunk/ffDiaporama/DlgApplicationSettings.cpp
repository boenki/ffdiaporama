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

#include "DlgApplicationSettings.h"
#include "mainwindow.h"
#include "ui_DlgApplicationSettings.h"

DlgApplicationSettings::DlgApplicationSettings(cApplicationConfig &TheApplicationConfig,QWidget *parent) : QDialog(parent),ui(new Ui::DlgApplicationSettings) {
    ui->setupUi(this);
    ApplicationConfig   =&TheApplicationConfig;
    IsDeviceChanged     =false;
    CurrentDevice       =-1;
    CurrentDeviceIndex  =-1;

    //********************************
    // LastDirectories part
    //********************************
    ui->RememberLastDirectoriesCH->setChecked(ApplicationConfig->RememberLastDirectories);

    //********************************
    // Preview Options part
    //********************************
    QString FPS=(QString("%1").arg(ApplicationConfig->PreviewFPS,0,'f')).trimmed();
    while (FPS.endsWith('0')) FPS=FPS.left(FPS.length()-1);
    if (FPS.endsWith('.')) FPS=FPS.left(FPS.length()-1);
    ui->ApplyTransfoDuringPreviewCB->setChecked(ApplicationConfig->ApplyTransfoPreview);
    ui->PreviewFrameRateCB->setCurrentIndex(ui->PreviewFrameRateCB->findText(FPS));

    //********************************
    // EditorOptions part
    //********************************
    ui->SortFileCB->setChecked(ApplicationConfig->SortFile);
    ui->AppendObjectCB->setCurrentIndex(ApplicationConfig->AppendObject?1:0);
    ui->FramingWidth->setChecked( ApplicationConfig->DefaultFraming==0);
    ui->FramingHeight->setChecked(ApplicationConfig->DefaultFraming==1);
    ui->FramingFull->setChecked(ApplicationConfig->DefaultFraming==2);
    ui->RandomTransitionRD->setChecked(ApplicationConfig->RandomTransition);
    ui->NoTransitionRD->setChecked((!ApplicationConfig->RandomTransition)&&(ApplicationConfig->DefaultTransitionFamilly==0)&&(ApplicationConfig->DefaultTransitionSubType==0));
    ui->BasicTransitionRD->setChecked((!ApplicationConfig->RandomTransition)&&(ApplicationConfig->DefaultTransitionFamilly==0)&&(ApplicationConfig->DefaultTransitionSubType==1));
    QString Duration=QString("%1").arg(double(ApplicationConfig->DefaultTransitionDuration)/double(1000),0,'f');
    while (Duration.endsWith('0')) Duration=Duration.left(Duration.length()-1);
    while (Duration.endsWith('.')) Duration=Duration.left(Duration.length()-1);
    ui->TransitionDurationCB->setCurrentIndex(ui->TransitionDurationCB->findText(Duration));
    ui->AskUserToRemove->setChecked(ApplicationConfig->AskUserToRemove);

    //********************************
    // ProjectDefault part
    //********************************
    ui->GeometryCombo->setCurrentIndex(ApplicationConfig->ImageGeometry);
    ui->NoShotED->setValue(ApplicationConfig->NoShotDuration/1000);
    ui->StaticShotED->setValue(ApplicationConfig->FixedDuration/1000);
    ui->SpeedWaveCombo->setCurrentIndex(ApplicationConfig->SpeedWave);

    //********************************
    // RenderDefault part
    //********************************
    InitImageSizeCombo(0);

    // Init format container combo
    for (int i=0;i<NBR_FORMATDEF;i++) if (FORMATDEF[i].IsFind) {
        ui->FileFormatCB->addItem(FORMATDEF[i].LongName,QVariant(i));
        if (i==ApplicationConfig->DefaultFormat) ui->FileFormatCB->setCurrentIndex(ui->FileFormatCB->count()-1);
    }
    if (ui->FileFormatCB->currentIndex()<0) ui->FileFormatCB->setCurrentIndex(0);
    // Image size & standard
    ui->StandardCombo->setCurrentIndex(ApplicationConfig->DefaultStandard);
    ui->SizeCombo->setCurrentIndex(ApplicationConfig->DefaultImageSize);
    // codec(s) & bitrate(s)
    FileFormatCombo(-1);     // For first initialisation : ChangeIndex=-1
    connect(ui->StandardCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitImageSizeCombo(int)));
    connect(ui->SizeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(FileFormatCombo(int)));
    connect(ui->FileFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(FileFormatCombo(int)));
    connect(ui->VideoFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));
    connect(ui->AudioFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitAudioBitRateCB(int)));

    //********************************
    // RestoreWindow part
    //********************************
    ui->RestoreWindowCH->setChecked(ApplicationConfig->RestoreWindow);
    //********************************
    // SSE2 part
    //********************************
    ui->DisableSSE2CB->setChecked(ApplicationConfig->DisableSSE2);

    //***********************************
    // Default Device type for rendering
    //***********************************

    // EXPORTMODE_SMARTPHONE
    QStringList List;
    for (int i=0;i<ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].count();i++)
        List.append(ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE][i]+"#"+QString("%1").arg(i));
    List.sort();
    for (int i=0;i<List.count();i++) {
        QString Item=List[i];
        int     ItemData=Item.mid(Item.lastIndexOf("#")+1).toInt();
        Item=Item.left(Item.lastIndexOf("#"));
        ui->SmartphoneTypeCB->addItem(Item,QVariant(ItemData));
        if (ApplicationConfig->DefaultSmartphoneType==ItemData)  ui->SmartphoneTypeCB->setCurrentIndex(i);
    }
    connect(ui->SmartphoneTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeSmartphoneTypeCB(int)));
    ChangeSmartphoneTypeCB(0);


    // EXPORTMODE_MULTIMEDIASYS
    List.clear();
    for (int i=0;i<ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].count();i++)
        List.append(ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS][i]+"#"+QString("%1").arg(i));
    List.sort();
    for (int i=0;i<List.count();i++) {
        QString Item=List[i];
        int     ItemData=Item.mid(Item.lastIndexOf("#")+1).toInt();
        Item=Item.left(Item.lastIndexOf("#"));
        ui->MMSystemTypeCB->addItem(Item,QVariant(ItemData));
        if (ApplicationConfig->DefaultMultimediaType==ItemData)  ui->MMSystemTypeCB->setCurrentIndex(i);
    }
    connect(ui->MMSystemTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeMMSystemTypeCB(int)));
    ChangeMMSystemTypeCB(0);

    // EXPORTMODE_FORTHEWEB
    List.clear();
    for (int i=0;i<ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_FORTHEWEB].count();i++)
        List.append(ApplicationConfig->TranslatedRenderSubtype[EXPORTMODE_FORTHEWEB][i]+"#"+QString("%1").arg(i));
    List.sort();
    for (int i=0;i<List.count();i++) {
        QString Item=List[i];
        int     ItemData=Item.mid(Item.lastIndexOf("#")+1).toInt();
        Item=Item.left(Item.lastIndexOf("#"));
        ui->ForTheWTypeCB->addItem(Item,QVariant(ItemData));
        if (ApplicationConfig->DefaultForTheWEBType==ItemData)  ui->ForTheWTypeCB->setCurrentIndex(i);
    }
    connect(ui->ForTheWTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeForTheWTypeCB(int)));
    ChangeForTheWTypeCB(0);

    // Tab Manage database
    ui->DBDeviceTypeCB->addItem(QIcon(ICON_SMARTPHONE),ApplicationConfig->TranslatedRenderType[1]);
    ui->DBDeviceTypeCB->addItem(QIcon(ICON_MULTIMEDIASYS),ApplicationConfig->TranslatedRenderType[2]);
    ui->DBDeviceTypeCB->addItem(QIcon(ICON_FORTHEWEB),ApplicationConfig->TranslatedRenderType[3]);
    ui->DBDeviceTypeCB->setCurrentIndex(0);
    for (int i=0;i<NBR_FORMATDEF;i++) if (FORMATDEF[i].IsFind) ui->DBFileFormatCB->addItem(FORMATDEF[i].LongName,QVariant(i));
    ui->DBFileFormatCB->setCurrentIndex(-1);
    DBFillTableDevice(0);
    DBImageSizeCombo(0);
    DBSelectionChanged();
    connect(ui->TableDevice,SIGNAL(itemSelectionChanged()),this,SLOT(DBSelectionChanged()));
    connect(ui->DBDeviceTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(DBFillTableDevice(int)));
    connect(ui->GeometryCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(DBImageSizeCombo(int)));
    connect(ui->DBStandardCB,SIGNAL(currentIndexChanged(int)),this,SLOT(DBImageSizeCombo(int)));
    connect(ui->DBFileFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(DBFileFormatCombo(int)));
    connect(ui->DBImageSizeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(DBChImageSizeCB(int)));
    connect(ui->DBDeviceModelED,SIGNAL(textChanged(QString)),this,SLOT(DBChModel(QString)));
    connect(ui->DBVideoFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(DBVideoBitRateCB(int)));
    connect(ui->DBAudioFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(DBAudioBitRateCB(int)));
    connect(ui->DBDeviceSubtypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(DBDeviceSubtypeCB(int)));
    connect(ui->DBVideoBitRateCB,SIGNAL(currentIndexChanged(int)),this,SLOT(DBChVideoBitRateCB(int)));
    connect(ui->DBAudioBitRateCB,SIGNAL(currentIndexChanged(int)),this,SLOT(DBChAudioBitRateCB(int)));
    connect(ui->DBApplyBT,SIGNAL(clicked()),this,SLOT(DBApplyChange()));
    connect(ui->DBAddDevice,SIGNAL(clicked()),this,SLOT(DBAddDevice()));
    connect(ui->DBRemoveBT,SIGNAL(clicked()),this,SLOT(DBRemoveBT()));
    connect(ui->DBResetBT,SIGNAL(clicked()),this,SLOT(DBResetToDefaultBT()));

    // Define handler
    connect(ui->CancelBt,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OkBt,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(TabChanged(int)));

    ui->tabWidget->setCurrentIndex(0);
}

//====================================================================================================================

DlgApplicationSettings::~DlgApplicationSettings() {
    delete ui;
}

//====================================================================================================================

void DlgApplicationSettings::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgApplicationSettings);
}

//====================================================================================================================

void DlgApplicationSettings::SetSavedWindowGeometry() {
    ApplicationConfig->DlgApplicationSettingsWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgApplicationSettings::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgApplicationSettings::reject() {
    // Save Window size and position
    ApplicationConfig->DlgApplicationSettingsWSP->SaveWindowState(this);
    done(1);
}

//====================================================================================================================

void DlgApplicationSettings::accept() {
    AskApplyDBChange();

    // Application options part
    ApplicationConfig->RememberLastDirectories  =ui->RememberLastDirectoriesCH->isChecked();
    ApplicationConfig->RestoreWindow            =ui->RestoreWindowCH->isChecked();
    ApplicationConfig->DisableSSE2              =ui->DisableSSE2CB->isChecked();

    // Preview Options part
    ApplicationConfig->ApplyTransfoPreview      =ui->ApplyTransfoDuringPreviewCB->isChecked();
    ApplicationConfig->PreviewFPS               =ui->PreviewFrameRateCB->currentText().toDouble();

    // Editor Options part
    ApplicationConfig->AppendObject    =ui->AppendObjectCB->currentIndex()==1;
    ApplicationConfig->SortFile        =ui->SortFileCB->isChecked();
    ApplicationConfig->AskUserToRemove =ui->AskUserToRemove->isChecked();
    if (ui->FramingWidth->isChecked())  ApplicationConfig->DefaultFraming=0;
    if (ui->FramingHeight->isChecked()) ApplicationConfig->DefaultFraming=1;
    if (ui->FramingFull->isChecked())   ApplicationConfig->DefaultFraming=2;
    ApplicationConfig->DefaultTransitionDuration=int(ui->TransitionDurationCB->currentText().toDouble()*double(1000));
    ApplicationConfig->RandomTransition         =ui->RandomTransitionRD->isChecked();
    ApplicationConfig->DefaultTransitionSubType =(ui->NoTransitionRD->isChecked()?0:1);
    ApplicationConfig->DefaultTransitionFamilly =0;

    // ProjectDefault part
    ApplicationConfig->NoShotDuration           =ui->NoShotED->value()*1000;
    ApplicationConfig->FixedDuration            =ui->StaticShotED->value()*1000;
    ApplicationConfig->SpeedWave                =ui->SpeedWaveCombo->currentIndex();
    ApplicationConfig->ImageGeometry            =ui->GeometryCombo->currentIndex();

    // RenderDefault part
    ApplicationConfig->DefaultStandard          =ui->StandardCombo->currentIndex();
    ApplicationConfig->DefaultImageSize         =ui->SizeCombo->currentIndex();
    ApplicationConfig->DefaultFormat            =ui->FileFormatCB->currentIndex();
    if (ApplicationConfig->DefaultFormat>=0) ApplicationConfig->DefaultFormat=ui->FileFormatCB->itemData(ApplicationConfig->DefaultFormat).toInt(); else ApplicationConfig->DefaultFormat=0;
    int Codec=ui->VideoFormatCB->currentIndex();
    if (Codec>=0) ApplicationConfig->DefaultVideoCodec=VIDEOCODECDEF[ui->VideoFormatCB->itemData(Codec).toInt()].FFD_VCODECST; else ApplicationConfig->DefaultVideoCodec="";
    Codec=ui->AudioFormatCB->currentIndex();
    if (Codec>=0) ApplicationConfig->DefaultAudioCodec=AUDIOCODECDEF[ui->AudioFormatCB->itemData(Codec).toInt()].ShortName; else ApplicationConfig->DefaultAudioCodec="";
    QString BitRate=ui->VideoBitRateCB->currentText();  if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1);    ApplicationConfig->DefaultVideoBitRate=BitRate.toInt();
    BitRate        =ui->AudioBitRateCB->currentText();  if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1);    ApplicationConfig->DefaultAudioBitRate=BitRate.toInt();

    ApplicationConfig->DefaultSmartphoneType =ui->SmartphoneTypeCB->itemData(ui->SmartphoneTypeCB->currentIndex()).toInt();
    ApplicationConfig->DefaultMultimediaType =ui->MMSystemTypeCB->itemData(ui->MMSystemTypeCB->currentIndex()).toInt();
    ApplicationConfig->DefaultForTheWEBType  =ui->ForTheWTypeCB->itemData(ui->ForTheWTypeCB->currentIndex()).toInt();

    QString Text=ui->SmartphoneModelCB->currentText();
    int i=0;
    while ((i<ApplicationConfig->RenderDeviceModel.count())&&(Text!=ApplicationConfig->RenderDeviceModel[i].DeviceName)) i++;
    if (Text==ApplicationConfig->RenderDeviceModel[i].DeviceName) ApplicationConfig->DefaultSmartphoneModel=ApplicationConfig->RenderDeviceModel[i].DeviceIndex;

    Text=ui->MMSystemModelCB->currentText();
    i=0;
    while ((i<ApplicationConfig->RenderDeviceModel.count())&&(Text!=ApplicationConfig->RenderDeviceModel[i].DeviceName)) i++;
    if (Text==ApplicationConfig->RenderDeviceModel[i].DeviceName) ApplicationConfig->DefaultMultimediaModel=ApplicationConfig->RenderDeviceModel[i].DeviceIndex;

    Text=ui->ForTheWModelCB->currentText();
    i=0;
    while ((i<ApplicationConfig->RenderDeviceModel.count())&&(Text!=ApplicationConfig->RenderDeviceModel[i].DeviceName)) i++;
    if (Text==ApplicationConfig->RenderDeviceModel[i].DeviceName) ApplicationConfig->DefaultForTheWEBModel=ApplicationConfig->RenderDeviceModel[i].DeviceIndex;

    // Save Window size and position
    ApplicationConfig->DlgApplicationSettingsWSP->SaveWindowState(this);
    done(0);
}

//====================================================================================================================

void DlgApplicationSettings::TabChanged(int) {
    AskApplyDBChange();
    ui->DBApplyBT->setVisible(ui->tabWidget->currentIndex()==3);
    if (ui->tabWidget->currentIndex()==2) {
        ChangeSmartphoneTypeCB(0);
        ChangeMMSystemTypeCB(0);
        ChangeForTheWTypeCB(0);
    }
}

//====================================================================================================================

void DlgApplicationSettings::AskApplyDBChange() {
    if ((IsDeviceChanged)&&(QMessageBox::question(this,QApplication::translate("DlgApplicationSettings","Apply modification"),
        QApplication::translate("DlgApplicationSettings","Do you whant to apply modification for this device?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)) DBApplyChange();
    IsDeviceChanged=false;
}

//====================================================================================================================

void DlgApplicationSettings::InitImageSizeCombo(int) {
    int Geometry=ui->GeometryCombo->currentIndex();
    int Standard=ui->StandardCombo->currentIndex();
    int ImageSize=ui->SizeCombo->currentIndex();
    ui->SizeCombo->clear();
    for (int i=0;i<NBR_SIZEDEF;i++) ui->SizeCombo->addItem(DefImageFormat[Standard][Geometry][i].Name);
    ui->SizeCombo->setCurrentIndex(ImageSize);
}

//====================================================================================================================

void DlgApplicationSettings::FileFormatCombo(int ChangeIndex) {
    ui->VideoFormatCB->clear();
    ui->AudioFormatCB->clear();

    //********* Video codec part
    int         CurrentFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
    QString     AllowedCodec=FORMATDEF[CurrentFormat].PossibleVideoCodec;
    QString     Codec="";
    int         Index=0;
    bool        IsFindCodec=false;
    while (AllowedCodec.length()>0) {
        Index=AllowedCodec.indexOf("#");
        if (Index>0) {
            Codec=AllowedCodec.left(Index);
            AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
        } else {
            Codec=AllowedCodec;
            AllowedCodec="";
        }
        // Now find index of this codec in the VIDEOCODECDEF
        Index=0;
        while ((Index<NBR_VIDEOCODECDEF)&&(Codec!=QString(VIDEOCODECDEF[Index].FFD_VCODECST))) Index++;
        if ((Index<NBR_VIDEOCODECDEF)&&(VIDEOCODECDEF[Index].IsFind)) {
            ui->VideoFormatCB->addItem(VIDEOCODECDEF[Index].LongName,QVariant(Index));
            if (Codec==QString(ApplicationConfig->DefaultVideoCodec)) {
                ui->VideoFormatCB->setCurrentIndex(ui->VideoFormatCB->count()-1);
                IsFindCodec=true;
            }
        }
    }
    if (!IsFindCodec) ui->VideoFormatCB->setCurrentIndex(0);
    ui->VideoFormatCB->setEnabled(ui->VideoFormatCB->count()>0);

    //********* Audio codec part
    AllowedCodec=FORMATDEF[CurrentFormat].PossibleAudioCodec;
    Codec="";
    Index=0;
    IsFindCodec=false;
    while (AllowedCodec.length()>0) {
        Index=AllowedCodec.indexOf("#");
        if (Index>0) {
            Codec=AllowedCodec.left(Index);
            AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
        } else {
            Codec=AllowedCodec;
            AllowedCodec="";
        }
        // Now find index of this codec in the AUDIOCODECDEF
        Index=0;
        while ((Index<NBR_AUDIOCODECDEF)&&(Codec!=QString(AUDIOCODECDEF[Index].ShortName))) Index++;
        if ((Index<NBR_AUDIOCODECDEF)&&(AUDIOCODECDEF[Index].IsFind)) {
            ui->AudioFormatCB->addItem(AUDIOCODECDEF[Index].LongName,QVariant(Index));
            if (Codec==QString(ApplicationConfig->DefaultAudioCodec)) {
                ui->AudioFormatCB->setCurrentIndex(ui->AudioFormatCB->count()-1);
                IsFindCodec=true;
            }
        }
    }
    if (!IsFindCodec) ui->AudioFormatCB->setCurrentIndex(0);
    ui->AudioFormatCB->setEnabled(ui->AudioFormatCB->count()>0);
    InitVideoBitRateCB(ChangeIndex);
    InitAudioBitRateCB(ChangeIndex);
}

//====================================================================================================================

void DlgApplicationSettings::InitVideoBitRateCB(int ChangeIndex) {
    ui->VideoBitRateCB->clear();
    int CurrentCodec=ui->VideoFormatCB->currentIndex();
    int CurrentSize =ui->SizeCombo->currentIndex();
    if (CurrentCodec>=0) {
        CurrentCodec=ui->VideoFormatCB->itemData(CurrentCodec).toInt();

        QString     AllowedBitRate=FORMATDEF[ui->FileFormatCB->currentIndex()].PossibleVideoCodec;
        QString     BitRate="";
        int         Index=0;
        bool        IsFindBitRate=false;
        AllowedBitRate=VIDEOCODECDEF[CurrentCodec].PossibleBitrate;
        BitRate="";
        Index=0;
        IsFindBitRate=false;
        QStringList List;
        while (AllowedBitRate.length()>0) {
            Index=AllowedBitRate.indexOf("#");
            if (Index>0) {
                BitRate=AllowedBitRate.left(Index);
                AllowedBitRate=AllowedBitRate.right(AllowedBitRate.length()-Index-1);
            } else {
                BitRate=AllowedBitRate;
                AllowedBitRate="";
            }
            List.append(BitRate);
        }
        for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) {
            QString NameA=List[j];      if (NameA.endsWith("k")) NameA=NameA.left(NameA.length()-1);
            int     NumA=NameA.toInt();
            QString NameB=List[j+1];    if (NameB.endsWith("k")) NameB=NameB.left(NameB.length()-1);
            int     NumB=NameB.toInt();
            if (NumA>NumB) List.swap(j,j+1);
        }
        for (int i=0;i<List.count();i++) {
            ui->VideoBitRateCB->addItem(List[i]);
            if ((ChangeIndex==-1)&&(List[i]==QString("%1k").arg(ApplicationConfig->DefaultVideoBitRate))) {
                ui->VideoBitRateCB->setCurrentIndex(ui->VideoBitRateCB->count()-1);
                IsFindBitRate=true;
            }
        }
        if (!IsFindBitRate) ui->VideoBitRateCB->setCurrentIndex(ui->VideoBitRateCB->findText(VIDEOCODECDEF[CurrentCodec].DefaultBitrate[CurrentSize]));
        ui->VideoBitRateCB->setEnabled(ui->VideoBitRateCB->count()>1);
    } else ui->VideoBitRateCB->setEnabled(false);
}

//====================================================================================================================

void DlgApplicationSettings::InitAudioBitRateCB(int ChangeIndex) {
    ui->AudioBitRateCB->clear();
    int CurrentCodec=ui->AudioFormatCB->currentIndex();
    if (CurrentCodec>=0) {
        CurrentCodec=ui->AudioFormatCB->itemData(CurrentCodec).toInt();

        QString     AllowedBitRate=FORMATDEF[ui->FileFormatCB->currentIndex()].PossibleVideoCodec;
        QString     BitRate="";
        int         Index=0;
        bool        IsFindBitRate=false;
        AllowedBitRate=AUDIOCODECDEF[CurrentCodec].PossibleBitrate2CH;
        BitRate="";
        Index=0;
        IsFindBitRate=false;
        QStringList List;
        while (AllowedBitRate.length()>0) {
            Index=AllowedBitRate.indexOf("#");
            if (Index>0) {
                BitRate=AllowedBitRate.left(Index);
                AllowedBitRate=AllowedBitRate.right(AllowedBitRate.length()-Index-1);
            } else {
                BitRate=AllowedBitRate;
                AllowedBitRate="";
            }
            List.append(BitRate);
        }
        for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) {
            QString NameA=List[j];      if (NameA.endsWith("k")) NameA=NameA.left(NameA.length()-1);
            int     NumA=NameA.toInt();
            QString NameB=List[j+1];    if (NameB.endsWith("k")) NameB=NameB.left(NameB.length()-1);
            int     NumB=NameB.toInt();
            if (NumA>NumB) List.swap(j,j+1);
        }
        for (int i=0;i<List.count();i++) {
            ui->AudioBitRateCB->addItem(List[i]);
            if ((ChangeIndex==-1)&&(List[i]==QString("%1k").arg(ApplicationConfig->DefaultAudioBitRate))) {
                ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->count()-1);
                IsFindBitRate=true;
            }
        }
        if (!IsFindBitRate) ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->findText(AUDIOCODECDEF[CurrentCodec].Default));
        ui->AudioBitRateCB->setEnabled(ui->AudioBitRateCB->count()>1);
    } else ui->AudioBitRateCB->setEnabled(false);
}

//====================================================================================================================

void DlgApplicationSettings::ChangeSmartphoneTypeCB(int) {
    int         ExportMode=EXPORTMODE_SMARTPHONE;
    ui->SmartphoneModelCB->clear();
    int ItemData=ui->SmartphoneTypeCB->currentIndex();
    if (ItemData>=0) ItemData=ui->SmartphoneTypeCB->itemData(ItemData).toInt();
    QStringList List;
    for (int i=0;i<ApplicationConfig->RenderDeviceModel.count();i++)
        if ((ApplicationConfig->RenderDeviceModel[i].DeviceType==ExportMode)&&(ApplicationConfig->RenderDeviceModel[i].DeviceSubtype==ItemData))
            List.append(ApplicationConfig->RenderDeviceModel[i].DeviceName);
    List.sort();
    ui->SmartphoneModelCB->addItems(List);
    int i=0;
    while ((i<ApplicationConfig->RenderDeviceModel.count())&&(ApplicationConfig->RenderDeviceModel[i].DeviceIndex!=ApplicationConfig->DefaultSmartphoneModel)) i++;
    if (ApplicationConfig->RenderDeviceModel[i].DeviceIndex==ApplicationConfig->DefaultSmartphoneModel)
        ui->SmartphoneModelCB->setCurrentIndex(ui->SmartphoneModelCB->findText(ApplicationConfig->RenderDeviceModel[i].DeviceName));
}

//====================================================================================================================

void DlgApplicationSettings::ChangeMMSystemTypeCB(int) {
    int         ExportMode=EXPORTMODE_MULTIMEDIASYS;
    ui->MMSystemModelCB->clear();
    int ItemData=ui->MMSystemTypeCB->currentIndex();
    if (ItemData>=0) ItemData=ui->MMSystemTypeCB->itemData(ItemData).toInt();
    QStringList List;
    for (int i=0;i<ApplicationConfig->RenderDeviceModel.count();i++)
        if ((ApplicationConfig->RenderDeviceModel[i].DeviceType==ExportMode)&&(ApplicationConfig->RenderDeviceModel[i].DeviceSubtype==ItemData))
            List.append(ApplicationConfig->RenderDeviceModel[i].DeviceName);
    List.sort();
    ui->MMSystemModelCB->addItems(List);
    int i=0;
    while ((i<ApplicationConfig->RenderDeviceModel.count())&&(ApplicationConfig->RenderDeviceModel[i].DeviceIndex!=ApplicationConfig->DefaultMultimediaModel)) i++;
    if (ApplicationConfig->RenderDeviceModel[i].DeviceIndex==ApplicationConfig->DefaultMultimediaModel)
        ui->MMSystemModelCB->setCurrentIndex(ui->MMSystemModelCB->findText(ApplicationConfig->RenderDeviceModel[i].DeviceName));
}

//====================================================================================================================

void DlgApplicationSettings::ChangeForTheWTypeCB(int) {
    int         ExportMode=EXPORTMODE_FORTHEWEB;
    ui->ForTheWModelCB->clear();
    int ItemData=ui->ForTheWTypeCB->currentIndex();
    if (ItemData>=0) ItemData=ui->ForTheWTypeCB->itemData(ItemData).toInt();
    QStringList List;
    for (int i=0;i<ApplicationConfig->RenderDeviceModel.count();i++)
        if ((ApplicationConfig->RenderDeviceModel[i].DeviceType==ExportMode)&&(ApplicationConfig->RenderDeviceModel[i].DeviceSubtype==ItemData))
            List.append(ApplicationConfig->RenderDeviceModel[i].DeviceName);
    List.sort();
    ui->ForTheWModelCB->addItems(List);
    int i=0;
    while ((i<ApplicationConfig->RenderDeviceModel.count())&&(ApplicationConfig->RenderDeviceModel[i].DeviceIndex!=ApplicationConfig->DefaultForTheWEBModel)) i++;
    if (ApplicationConfig->RenderDeviceModel[i].DeviceIndex==ApplicationConfig->DefaultForTheWEBModel)
        ui->ForTheWModelCB->setCurrentIndex(ui->ForTheWModelCB->findText(ApplicationConfig->RenderDeviceModel[i].DeviceName));
}

//====================================================================================================================

void DlgApplicationSettings::DBFillTableDevice(int ChangeIndex) {
    AskApplyDBChange();
    ui->TableDevice->setUpdatesEnabled(false);
    ui->TableDevice->setSortingEnabled(false);
    while (ui->TableDevice->rowCount()>0) ui->TableDevice->removeRow(0);
    for (int i=0;i<ApplicationConfig->RenderDeviceModel.count();i++) if (ApplicationConfig->RenderDeviceModel[i].DeviceType==ChangeIndex+1) {
        int j=ui->TableDevice->rowCount();
        ui->TableDevice->insertRow(j);
        ui->TableDevice->setItem(j,0,new QTableWidgetItem(ApplicationConfig->RenderDeviceModel[i].FromUserConf?QIcon(ICON_USERCONF):QIcon(ICON_GLOBALCONF),
                                                          (ApplicationConfig->RenderDeviceModel[i].DeviceIndex<10?"#0":"#")+QString("%1").arg(ApplicationConfig->RenderDeviceModel[i].DeviceIndex)));
        ui->TableDevice->setItem(j,1,new QTableWidgetItem(ApplicationConfig->TranslatedRenderSubtype[ApplicationConfig->RenderDeviceModel[i].DeviceType][ApplicationConfig->RenderDeviceModel[i].DeviceSubtype]));
        ui->TableDevice->setItem(j,2,new QTableWidgetItem(ApplicationConfig->RenderDeviceModel[i].DeviceName));
    }
    ui->TableDevice->resizeColumnToContents(0);
    ui->TableDevice->resizeColumnToContents(1);
    ui->TableDevice->resizeRowsToContents();
    ui->TableDevice->setSortingEnabled(true);
    ui->TableDevice->setUpdatesEnabled(true);

    QStringList List;
    for (int i=0;i<ApplicationConfig->TranslatedRenderSubtype[ChangeIndex+1].count();i++)
        List.append(ApplicationConfig->TranslatedRenderSubtype[ChangeIndex+1][i]+"#"+QString("%1").arg(i));
    List.sort();
    ui->DBDeviceSubtypeCB->clear();
    for (int i=0;i<List.count();i++) {
        QString Item=List[i];
        int     ItemData=Item.mid(Item.lastIndexOf("#")+1).toInt();
        Item=Item.left(Item.lastIndexOf("#"));
        ui->DBDeviceSubtypeCB->addItem(Item,QVariant(ItemData));
    }
    ui->DBDeviceSubtypeCB->setCurrentIndex(-1);
    ui->DBDeviceModelED->setText("");
    ui->DBStandardCB->setCurrentIndex(-1);
    ui->DBFileFormatCB->setCurrentIndex(-1);
    ui->DBImageSizeCombo->setCurrentIndex(-1);
    ui->DBVideoFormatCB->setCurrentIndex(-1);
    ui->DBAudioFormatCB->setCurrentIndex(-1);
    IsDeviceChanged=false;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBSelectionChanged() {
    AskApplyDBChange();
    CurrentDevice=ui->TableDevice->currentRow();
    if (CurrentDevice<0) {
        ui->DBRemoveBT->setEnabled(false);
        ui->DBResetBT->setEnabled(false);
        ui->DBDeviceSubtypeCB->setEnabled(false);
        ui->DBDeviceModelED->setEnabled(false);
        ui->DBStandardCB->setEnabled(false);
        ui->DBFileFormatCB->setEnabled(false);
        ui->DBImageSizeCombo->setEnabled(false);
        ui->DBVideoFormatCB->setEnabled(false);
        ui->DBAudioFormatCB->setEnabled(false);
    } else {
        ui->DBDeviceSubtypeCB->setEnabled(true);
        ui->DBDeviceModelED->setEnabled(true);
        ui->DBStandardCB->setEnabled(true);
        ui->DBFileFormatCB->setEnabled(true);
        ui->DBImageSizeCombo->setEnabled(true);
        ui->DBVideoFormatCB->setEnabled(true);
        ui->DBAudioFormatCB->setEnabled(true);
        QString DeviceNumber=ui->TableDevice->item(CurrentDevice,0)->text();
        if (DeviceNumber.startsWith("#")) DeviceNumber=DeviceNumber.mid(1);
        CurrentDeviceIndex=DeviceNumber.toInt();
        CurrentDevice=0; while (ApplicationConfig->RenderDeviceModel[CurrentDevice].DeviceIndex!=CurrentDeviceIndex) CurrentDevice++;
        cDeviceModelDef *Device=&ApplicationConfig->RenderDeviceModel[CurrentDevice];

        ui->DBRemoveBT->setEnabled(!Device->FromGlobalConf && Device->FromUserConf);
        ui->DBResetBT->setEnabled(Device->FromGlobalConf && Device->FromUserConf);
        ui->DBDeviceSubtypeCB->setCurrentIndex(ui->DBDeviceSubtypeCB->findText(ApplicationConfig->TranslatedRenderSubtype[Device->DeviceType][Device->DeviceSubtype]));
        ui->DBDeviceModelED->setText(Device->DeviceName);
        ui->DBStandardCB->setCurrentIndex(Device->Standard);
        ui->DBFileFormatCB->setCurrentIndex(ui->DBFileFormatCB->findText(FORMATDEF[Device->FileFormat].LongName));
        ui->DBImageSizeCombo->setCurrentIndex(Device->ImageSize);
        ui->DBVideoFormatCB->setCurrentIndex(ui->DBVideoFormatCB->findText(VIDEOCODECDEF[Device->VideoCodec].LongName));
        ui->DBAudioFormatCB->setCurrentIndex(ui->DBAudioFormatCB->findText(AUDIOCODECDEF[Device->AudioCodec].LongName));
        QString VideoBitRateStr=QString("%1").arg(Device->VideoBitrate); if (VideoBitRateStr.endsWith("000")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-3)+"k";
        QString AudioBitRateStr=QString("%1").arg(Device->AudioBitrate); if (AudioBitRateStr.endsWith("000")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-3)+"k";
        ui->DBVideoBitRateCB->setCurrentIndex(ui->DBVideoBitRateCB->findText(VideoBitRateStr));
        ui->DBAudioBitRateCB->setCurrentIndex(ui->DBAudioBitRateCB->findText(AudioBitRateStr));
    }
    IsDeviceChanged=false;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBImageSizeCombo(int) {
    int SaveIndex=ui->DBImageSizeCombo->currentIndex();
    ui->DBImageSizeCombo->clear();
    int Standard=ui->DBStandardCB->currentIndex();
    if (Standard==-1) return;
    for (int i=0;i<NBR_SIZEDEF;i++) ui->DBImageSizeCombo->addItem(IMAGEDEFGENNAME[Standard][i]);
    ui->DBImageSizeCombo->setCurrentIndex(SaveIndex);
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBFileFormatCombo(int /*ChangeIndex*/) {
    ui->DBVideoFormatCB->clear();
    ui->DBAudioFormatCB->clear();
    int CurrentFormat=ui->DBFileFormatCB->currentIndex();
    if (CurrentFormat==-1) return;
    CurrentFormat=ui->DBFileFormatCB->itemData(CurrentFormat).toInt();

    //********* Video codec part
    QString     AllowedCodec=FORMATDEF[CurrentFormat].PossibleVideoCodec;
    QString     Codec="";
    int         Index=0;
    bool        IsFindCodec=false;
    while (AllowedCodec.length()>0) {
        Index=AllowedCodec.indexOf("#");
        if (Index>0) {
            Codec=AllowedCodec.left(Index);
            AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
        } else {
            Codec=AllowedCodec;
            AllowedCodec="";
        }
        // Now find index of this codec in the VIDEOCODECDEF
        Index=0;
        while ((Index<NBR_VIDEOCODECDEF)&&(Codec!=QString(VIDEOCODECDEF[Index].FFD_VCODECST))) Index++;
        if ((Index<NBR_VIDEOCODECDEF)&&(VIDEOCODECDEF[Index].IsFind)) ui->DBVideoFormatCB->addItem(VIDEOCODECDEF[Index].LongName,QVariant(Index));
    }
    ui->DBVideoFormatCB->setEnabled(ui->VideoFormatCB->count()>0);

    //********* Audio codec part
    AllowedCodec=FORMATDEF[CurrentFormat].PossibleAudioCodec;
    Codec="";
    Index=0;
    IsFindCodec=false;
    while (AllowedCodec.length()>0) {
        Index=AllowedCodec.indexOf("#");
        if (Index>0) {
            Codec=AllowedCodec.left(Index);
            AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
        } else {
            Codec=AllowedCodec;
            AllowedCodec="";
        }
        // Now find index of this codec in the AUDIOCODECDEF
        Index=0;
        while ((Index<NBR_AUDIOCODECDEF)&&(Codec!=QString(AUDIOCODECDEF[Index].ShortName))) Index++;
        if ((Index<NBR_AUDIOCODECDEF)&&(AUDIOCODECDEF[Index].IsFind)) ui->DBAudioFormatCB->addItem(AUDIOCODECDEF[Index].LongName,QVariant(Index));
    }
    ui->AudioFormatCB->setEnabled(ui->DBAudioFormatCB->count()>0);
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBChImageSizeCB(int) {
    // Change bitrate values
//TODO:
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBChModel(QString) {
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBDeviceSubtypeCB(int) {
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBChVideoBitRateCB(int) {
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBChAudioBitRateCB(int) {
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBVideoBitRateCB(int) {
    ui->DBVideoBitRateCB->clear();
    int CurrentCodec=ui->DBVideoFormatCB->currentIndex();
    if (CurrentCodec>=0) {
        CurrentCodec=ui->DBVideoFormatCB->itemData(CurrentCodec).toInt();

        QString     AllowedBitRate;
        QString     BitRate="";
        int         Index=0;
        bool        IsFindBitRate=false;
        AllowedBitRate=VIDEOCODECDEF[CurrentCodec].PossibleBitrate;
        BitRate="";
        Index=0;
        IsFindBitRate=false;
        while (AllowedBitRate.length()>0) {
            Index=AllowedBitRate.indexOf("#");
            if (Index>0) {
                BitRate=AllowedBitRate.left(Index);
                AllowedBitRate=AllowedBitRate.right(AllowedBitRate.length()-Index-1);
            } else {
                BitRate=AllowedBitRate;
                AllowedBitRate="";
            }
            ui->DBVideoBitRateCB->addItem(BitRate);
        }
        ui->DBVideoBitRateCB->setEnabled(ui->DBVideoBitRateCB->count()>1);
    } else ui->DBVideoBitRateCB->setEnabled(false);
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBAudioBitRateCB(int) {
    ui->DBAudioBitRateCB->clear();
    int CurrentCodec=ui->DBAudioFormatCB->currentIndex();
    if (CurrentCodec>=0) {
        CurrentCodec=ui->DBAudioFormatCB->itemData(CurrentCodec).toInt();

        QString     AllowedBitRate;;
        QString     BitRate="";
        int         Index=0;
        bool        IsFindBitRate=false;
        AllowedBitRate=AUDIOCODECDEF[CurrentCodec].PossibleBitrate2CH;
        BitRate="";
        Index=0;
        IsFindBitRate=false;
        while (AllowedBitRate.length()>0) {
            Index=AllowedBitRate.indexOf("#");
            if (Index>0) {
                BitRate=AllowedBitRate.left(Index);
                AllowedBitRate=AllowedBitRate.right(AllowedBitRate.length()-Index-1);
            } else {
                BitRate=AllowedBitRate;
                AllowedBitRate="";
            }
            ui->DBAudioBitRateCB->addItem(BitRate);
        }
        ui->DBAudioBitRateCB->setEnabled(ui->DBAudioBitRateCB->count()>1);
    } else ui->DBAudioBitRateCB->setEnabled(false);
    IsDeviceChanged=true;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBResetToDefaultBT() {
    if (CurrentDevice>=0) {
        cDeviceModelDef *Device=&ApplicationConfig->RenderDeviceModel[CurrentDevice];
        Device->DeviceName   =Device->BckDeviceName;                                      // long name for the device model
        Device->DeviceType   =Device->BckDeviceType;                                      // device type
        Device->DeviceSubtype=Device->BckDeviceSubtype;                                   // device Subtype
        Device->Standard     =Device->BckStandard;                                        // standard : PAL/NTSC
        Device->FileFormat   =Device->BckFileFormat;                                      // sFormatDef number
        Device->ImageSize    =Device->BckImageSize;                                       // DefImageFormat number
        Device->VideoCodec   =Device->BckVideoCodec;                                      // sVideoCodecDef number
        Device->VideoBitrate =Device->BckVideoBitrate;                                    // Bitrate number in sVideoCodecDef
        Device->AudioCodec   =Device->BckAudioCodec;                                      // sAudioCodecDef number
        Device->AudioBitrate =Device->BckAudioBitrate;                                    // Bitrate number in sAudioCodecDef

        Device->FromUserConf =false;
        IsDeviceChanged      =false;
        ui->DBApplyBT->setEnabled(IsDeviceChanged);
        int SaveCurrentDevice=CurrentDeviceIndex;
        DBFillTableDevice(ui->DBDeviceTypeCB->currentIndex());
        for (int i=0;i<ui->TableDevice->rowCount();i++) {
            QString DeviceNumber=ui->TableDevice->item(i,0)->text();
            if (DeviceNumber.startsWith("#")) DeviceNumber=DeviceNumber.mid(1);
            if (SaveCurrentDevice==DeviceNumber.toInt()) {
                ui->TableDevice->setCurrentCell(i,0);
                break;
            }
        }
    }
    IsDeviceChanged=false;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

void DlgApplicationSettings::DBApplyChange() {
    if (CurrentDevice>=0) {
        cDeviceModelDef *Device=&ApplicationConfig->RenderDeviceModel[CurrentDevice];

        Device->DeviceSubtype=ui->DBDeviceSubtypeCB->itemData(ui->DBDeviceSubtypeCB->currentIndex()).toInt();
        Device->DeviceName   =ui->DBDeviceModelED->text();
        Device->Standard     =ui->DBStandardCB->currentIndex();
        Device->FileFormat   =ui->DBFileFormatCB->itemData(ui->DBFileFormatCB->currentIndex()).toInt();
        Device->ImageSize    =ui->DBImageSizeCombo->currentIndex();
        Device->VideoCodec   =ui->DBVideoFormatCB->itemData(ui->DBVideoFormatCB->currentIndex()).toInt();
        Device->AudioCodec   =ui->DBAudioFormatCB->itemData(ui->DBAudioFormatCB->currentIndex()).toInt();

        QString VideoBitRateStr=ui->DBVideoBitRateCB->currentText(); if (VideoBitRateStr.endsWith("k")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-1)+"000";
        QString AudioBitRateStr=ui->DBAudioBitRateCB->currentText(); if (AudioBitRateStr.endsWith("k")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-1)+"000";
        Device->VideoBitrate =VideoBitRateStr.toInt();
        Device->AudioBitrate =AudioBitRateStr.toInt();

        Device->FromUserConf =true;
        IsDeviceChanged      =false;
        ui->DBApplyBT->setEnabled(IsDeviceChanged);
        int SaveCurrentDevice=CurrentDeviceIndex;
        DBFillTableDevice(ui->DBDeviceTypeCB->currentIndex());
        for (int i=0;i<ui->TableDevice->rowCount();i++) {
            QString DeviceNumber=ui->TableDevice->item(i,0)->text();
            if (DeviceNumber.startsWith("#")) DeviceNumber=DeviceNumber.mid(1);
            if (SaveCurrentDevice==DeviceNumber.toInt()) {
                ui->TableDevice->setCurrentCell(i,0);
                break;
            }
        }
    }
    IsDeviceChanged=false;
    ui->DBApplyBT->setEnabled(IsDeviceChanged);
}

//====================================================================================================================

void DlgApplicationSettings::DBAddDevice() {
    int DeviceIndex=0;
    for (int i=0;i<ApplicationConfig->RenderDeviceModel.count();i++) if (ApplicationConfig->RenderDeviceModel[i].DeviceIndex>DeviceIndex) DeviceIndex=ApplicationConfig->RenderDeviceModel[i].DeviceIndex;
    DeviceIndex++;
    int index=ApplicationConfig->RenderDeviceModel.count();
    ApplicationConfig->RenderDeviceModel.append(cDeviceModelDef(false,index));
    ApplicationConfig->RenderDeviceModel[index].DeviceIndex  =DeviceIndex;
    ApplicationConfig->RenderDeviceModel[index].DeviceName   =QApplication::translate("DlgApplicationSettings","<new device>");
    ApplicationConfig->RenderDeviceModel[index].DeviceType   =ui->DBDeviceTypeCB->currentIndex()+1;
    DBFillTableDevice(ui->DBDeviceTypeCB->currentIndex());
    for (int i=0;i<ui->TableDevice->rowCount();i++) {
        QString DeviceNumber=ui->TableDevice->item(i,0)->text();
        if (DeviceNumber.startsWith("#")) DeviceNumber=DeviceNumber.mid(1);
        if (DeviceIndex==DeviceNumber.toInt()) {
            ui->TableDevice->setCurrentCell(i,0);
            break;
        }
    }
}

//====================================================================================================================

void DlgApplicationSettings::DBRemoveBT() {
    if ((CurrentDevice<0)||(CurrentDevice>=ApplicationConfig->RenderDeviceModel.count())) return;
    if (ApplicationConfig->RenderDeviceModel[CurrentDevice].FromGlobalConf) return;

    int toDelete=CurrentDevice;
    for (int i=0;i<ui->TableDevice->rowCount();i++) {
        QString DeviceNumber=ui->TableDevice->item(i,0)->text();
        if (DeviceNumber.startsWith("#")) DeviceNumber=DeviceNumber.mid(1);
        if (CurrentDeviceIndex==DeviceNumber.toInt()) {
            ui->TableDevice->removeRow(i);
            break;
        }
    }
    ApplicationConfig->RenderDeviceModel.removeAt(toDelete);
    DBFillTableDevice(ui->DBDeviceTypeCB->currentIndex());
}
