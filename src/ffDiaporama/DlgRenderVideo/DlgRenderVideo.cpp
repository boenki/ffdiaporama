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

#include "../_ApplicationDefinitions.h"
#include "../DlgffDPjrProperties/DlgffDPjrProperties.h"
#include "DlgRenderVideo.h"
#include "ui_DlgRenderVideo.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFutureWatcher>

//====================================================================================================================

DlgRenderVideo::DlgRenderVideo(cDiaporama &TheDiaporama,int TheExportMode,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent):
    QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgRenderVideo) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::DlgRenderVideo");

    ui->setupUi(this);
    OkBt                =ui->OkBt;
    CancelBt            =ui->CancelBt;
    HelpBt              =ui->HelpBT;
    Diaporama           =&TheDiaporama;
    ExportMode          =TheExportMode;
    IsDestFileOpen      =false;
    StopSpinboxRecursion=false;
    Language            =Diaporama->ProjectInfo->DefaultLanguage;
}

//====================================================================================================================

DlgRenderVideo::~DlgRenderVideo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::~DlgRenderVideo");

    delete ui;
}

//====================================================================================================================

void DlgRenderVideo::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::DoInitDialog");

    switch (ExportMode) {
        case EXPORTMODE_ADVANCED :  setWindowTitle(QApplication::translate("DlgRenderVideo","Advanced render video"));                          break;
        case MODE_SMARTPHONE     :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for mobil player and smartphone"));   break;
        case MODE_MULTIMEDIASYS  :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for multimedia system"));             break;
        case MODE_FORTHEWEB      :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for the WEB"));                       break;
    }

    // For the first step, only SelectDestWidget is display and enable
    ui->SelectDestScroll->setEnabled(true);     ui->SelectDestScroll->setVisible(true);
    ui->ProcessWidget->setEnabled(false);       ui->ProcessWidget->setVisible(false);

    QString FolderProject=Diaporama->ProjectFileName;
    if (FolderProject!="") {
        FolderProject=QFileInfo(FolderProject).absolutePath();
        if (FolderProject.endsWith(QDir::separator())) {
            FolderProject=QFileInfo(Diaporama->ProjectFileName).baseName();
        } else if (FolderProject.lastIndexOf(QDir::separator())>0) FolderProject=FolderProject.mid(FolderProject.lastIndexOf(QDir::separator())+1);
    }

    // Output file
    OutputFileName=Diaporama->ApplicationConfig->LastRenderVideoPath+
        (Diaporama->ApplicationConfig->LastRenderVideoPath.endsWith(QDir::separator())?"":QString(QDir::separator()))+
        (Diaporama->ProjectFileName!=""?
             (Diaporama->ApplicationConfig->DefaultNameProjectName==1?QFileInfo(Diaporama->ProjectFileName).baseName():
             (((Diaporama->ApplicationConfig->DefaultNameProjectName==2)&&(Diaporama->ProjectInfo->Title!=""))?Diaporama->ProjectInfo->Title:FolderProject)):
             QApplication::translate("DlgRenderVideo","movie","Default name for rendering"));

    OutputFileFormat    = Diaporama->ApplicationConfig->DefaultFormat;
    VideoCodec          = Diaporama->ApplicationConfig->DefaultVideoCodec;
    VideoFrameRate      = 25;
    VideoBitRate        = Diaporama->ApplicationConfig->DefaultVideoBitRate;
    AudioCodec          = Diaporama->ApplicationConfig->DefaultAudioCodec;
    AudioFrequency      = 48000;
    AudioBitRate        = Diaporama->ApplicationConfig->DefaultAudioBitRate;
    ImageSize           = Diaporama->ApplicationConfig->DefaultImageSize;
    Standard            = Diaporama->ApplicationConfig->DefaultStandard;

    ui->IncludeSoundCB->setChecked(true);
    connect(ui->IncludeSoundCB,SIGNAL(clicked()),this,SLOT(s_IncludeSound()));

    if (ExportMode==EXPORTMODE_ADVANCED) {

        ui->DeviceTypeLabel->setVisible(false);
        ui->DeviceTypeCB->setVisible(false);
        ui->DeviceModelCB->setVisible(false);
        ui->DeviceModelLabel->setVisible(false);
        ui->RenderFormatText->setVisible(false);
        //ui->VideoFormatCB->view()->setFixedWidth(400);
        //ui->AudioFormatCB->view()->setFixedWidth(400);

        // Init format container combo
        for (int i=0;i<NBR_FORMATDEF;i++) if (FORMATDEF[i].IsFind) {
            ui->FileFormatCB->addItem(FORMATDEF[i].LongName,QVariant(i));
            if (i==OutputFileFormat) {
                ui->FileFormatCB->setCurrentIndex(ui->FileFormatCB->count()-1);
                FileFormatCombo(i);
            }
        }

        connect(ui->FileFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(FileFormatCombo(int)));

        // Video standard & geometry
        ui->GeometryCombo->setEnabled(false);
        ui->GeometryCombo->setCurrentIndex(Diaporama->ImageGeometry);
        ui->StandardCombo->setCurrentIndex(Diaporama->ApplicationConfig->DefaultStandard);
        connect(ui->GeometryCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitImageSizeCombo(int)));
        connect(ui->StandardCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitImageSizeCombo(int)));

        // Image size
        InitImageSizeCombo(0);
        ui->ImageSizeCombo->setCurrentIndex(ui->ImageSizeCombo->findText(DefImageFormat[Diaporama->ApplicationConfig->DefaultStandard][Diaporama->ImageGeometry][Diaporama->ApplicationConfig->DefaultImageSize].Name));
        connect(ui->ImageSizeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));

        // codec(s) & bitrate(s)
        FileFormatCombo(0);
        InitVideoBitRateCB(-1);
        InitAudioBitRateCB(-1);

        ui->AudioFormatLabel->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioFormatCB->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioBitRateLabel->setEnabled(ui->IncludeSoundCB->isChecked());
        ui->AudioBitRateCB->setEnabled(ui->IncludeSoundCB->isChecked());

        connect(ui->VideoFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));
        connect(ui->AudioFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitAudioBitRateCB(int)));

    } else {
        ui->FileFormatLabel->setVisible(false);                         ui->FileFormatCB->setVisible(false);
        ui->GeometryLabel->setVisible(false);                           ui->GeometryCombo->setVisible(false);
        ui->StandardLabel->setVisible(false);                           ui->StandardCombo->setVisible(false);
        ui->VideoFormatLabel->setVisible(false);                        ui->VideoFormatCB->setVisible(false);
        ui->VideoBitRateLabel->setVisible(false);                       ui->VideoBitRateCB->setVisible(false);

        ui->AudioFormatLabel->setVisible(false);                        ui->AudioFormatLabel->setEnabled(false);
        ui->AudioFormatCB->setVisible(false);                           ui->AudioFormatCB->setEnabled(false);
        ui->AudioBitRateLabel->setVisible(false);                       ui->AudioBitRateLabel->setEnabled(false);
        ui->AudioBitRateCB->setVisible(false);                          ui->AudioBitRateCB->setEnabled(false);

        QStringList List;
        int         Default=ExportMode==MODE_SMARTPHONE?Diaporama->ApplicationConfig->DefaultSmartphoneType:
                            ExportMode==MODE_MULTIMEDIASYS?Diaporama->ApplicationConfig->DefaultMultimediaType:
                            ExportMode==MODE_FORTHEWEB?Diaporama->ApplicationConfig->DefaultForTheWEBType:
                            Diaporama->ApplicationConfig->DefaultLossLess;
        if (ExportMode!=MODE_LOSSLESS) {
            ui->ImageSizeLabel->setVisible(false);
            ui->ImageSizeCombo->setVisible(false);
            ui->DeviceTypeLabel->setVisible(true);
            ui->DeviceTypeCB->setVisible(true);
            ui->DeviceModelCB->setVisible(true);
            ui->DeviceModelLabel->setVisible(true);
            for (int i=0;i<Diaporama->ApplicationConfig->DeviceModelList.TranslatedRenderSubtype[ExportMode].count();i++) List.append(Diaporama->ApplicationConfig->DeviceModelList.TranslatedRenderSubtype[ExportMode][i]+"#"+QString("%1").arg(i));
            List.sort();
            for (int i=0;i<List.count();i++) {
                QString Item=List[i];
                int     ItemData=Item.mid(Item.lastIndexOf("#")+1).toInt();
                Item=Item.left(Item.lastIndexOf("#"));
                ui->DeviceTypeCB->addItem(Item,QVariant(ItemData));
                if (Default==ItemData)  ui->DeviceTypeCB->setCurrentIndex(i);
            }

            s_DeviceTypeCB(0);
            s_DeviceModelCB(0);
            connect(ui->DeviceTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_DeviceTypeCB(int)));
            connect(ui->DeviceModelCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_DeviceModelCB(int)));
        } else {
            ui->ImageSizeLabel->setVisible(true);
            ui->ImageSizeCombo->setVisible(true);
            ui->DeviceTypeLabel->setVisible(false);
            ui->DeviceTypeCB->setVisible(false);
            ui->DeviceModelCB->setVisible(false);
            ui->DeviceModelLabel->setVisible(false);
            InitImageSizeCombo(0);
            ui->ImageSizeCombo->setCurrentIndex(ui->ImageSizeCombo->findText(DefImageFormat[Diaporama->ApplicationConfig->DefaultStandard][Diaporama->ImageGeometry][Diaporama->ApplicationConfig->DefaultLossLess].Name));
            InitVideoBitRateCB(0);
            connect(ui->ImageSizeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));
        }
    }

    ui->LanguageED->setText(Language);
    ui->RenderZoneAllBt->setChecked(true);
    ui->RenderZoneFromBt->setChecked(false);
    ui->RenderZoneFromED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneToED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneTo->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneFromED->setRange(1,Diaporama->List.count());
    ui->RenderZoneFromED->setValue(1);
    ui->RenderZoneToED->setRange(1,Diaporama->List.count());
    ui->RenderZoneToED->setValue(Diaporama->List.count());
    connect(ui->RenderZoneAllBt,SIGNAL(clicked()),this,SLOT(SetZoneToAll()));
    connect(ui->RenderZoneFromBt,SIGNAL(clicked()),this,SLOT(SetZoneToPartial()));

    ui->DestinationFilePath->setText(OutputFileName);
    AdjustDestinationFile();
    connect(ui->DestinationFilePathBT,SIGNAL(clicked()),this,SLOT(SelectDestinationFile()));
    connect(ui->DestinationFilePath,SIGNAL(editingFinished()),this,SLOT(AdjustDestinationFile()));

    connect(ui->ProjectPropertiesBt,SIGNAL(clicked()),this,SLOT(ProjectProperties()));
}

//====================================================================================================================

void DlgRenderVideo::ProjectProperties() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::ProjectProperties");

    DlgffDPjrProperties Dlg(Diaporama->ProjectInfo,HELPFILE_DlgffDPjrProperties,BaseApplicationConfig,((cApplicationConfig *)BaseApplicationConfig)->DlgffDPjrPropertiesWSP,this);
    Dlg.InitDialog();
    if (Dlg.exec()==0) emit SetModifyFlag();
}

//====================================================================================================================

void DlgRenderVideo::SetZoneToAll() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::SetZoneToAll");

    ui->RenderZoneAllBt->setChecked(true);
    ui->RenderZoneFromBt->setChecked(false);
    ui->RenderZoneFromED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneToED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneTo->setEnabled(ui->RenderZoneFromBt->isChecked());
}

//====================================================================================================================

void DlgRenderVideo::SetZoneToPartial() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::SetZoneToPartial");

    ui->RenderZoneAllBt->setChecked(false);
    ui->RenderZoneFromBt->setChecked(true);
    ui->RenderZoneFromED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneToED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneTo->setEnabled(ui->RenderZoneFromBt->isChecked());
}

//====================================================================================================================

void DlgRenderVideo::s_DeviceTypeCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::s_DeviceTypeCB");

    ui->DeviceModelCB->clear();
    int ItemData=ui->DeviceTypeCB->currentIndex();
    if (ItemData>=0) ItemData=ui->DeviceTypeCB->itemData(ItemData).toInt();
    QStringList List;
    for (int i=0;i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count();i++)
        if ((Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceType==ExportMode)&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceSubtype==ItemData))
            List.append(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName);
    List.sort();
    ui->DeviceModelCB->addItems(List);
    int i=0;
    int ToFind=(ExportMode==MODE_SMARTPHONE)?Diaporama->ApplicationConfig->DefaultSmartphoneModel:
               (ExportMode==MODE_MULTIMEDIASYS)?Diaporama->ApplicationConfig->DefaultMultimediaModel:
               Diaporama->ApplicationConfig->DefaultForTheWEBModel;
    while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceIndex!=ToFind)) i++;
    if (Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceIndex==ToFind)
        ui->DeviceModelCB->setCurrentIndex(ui->DeviceModelCB->findText(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName));
}

//====================================================================================================================

void DlgRenderVideo::InitImageSizeCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::InitImageSizeCombo");
    int Geometry=(ExportMode!=MODE_LOSSLESS?ui->GeometryCombo->currentIndex():Diaporama->ImageGeometry);
    int Standard=(ExportMode!=MODE_LOSSLESS?ui->StandardCombo->currentIndex():Diaporama->ApplicationConfig->DefaultStandard);
    int ImageSize=ui->ImageSizeCombo->currentIndex();
    ui->ImageSizeCombo->clear();
    QStringList List;
    for (int i=0;i<NBR_SIZEDEF;i++) if (ORDERIMAGENAME[Standard][i]!=0) List.append(QString("%1:%2#####%3").arg(DefImageFormat[Standard][Geometry][i].Name).arg(ORDERIMAGENAME[Standard][i]).arg(i));
    // Sort List
    for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) {
        QString StrA=List[j].mid(List[j].lastIndexOf(":")+1);       StrA=StrA.left(StrA.indexOf("#"));
        QString StrB=List[j+1].mid(List[j+1].lastIndexOf(":")+1);   StrB=StrB.left(StrB.indexOf("#"));
        if (StrA.toInt()>StrB.toInt()) List.swap(j,j+1);
    }
    // Fill combo
    for (int i=0;i<List.count();i++) {
        QString Codec=List[i].left(List[i].indexOf("#####")); Codec=Codec.left(Codec.lastIndexOf(":"));
        int Index=List[i].mid(List[i].indexOf("#####")+QString("#####").length()).toInt();
        ui->ImageSizeCombo->addItem(Codec,QVariant(Index));
    }
    ui->ImageSizeCombo->setCurrentIndex(ImageSize);
}

//====================================================================================================================

void DlgRenderVideo::SelectDestinationFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::SelectDestinationFile");

    QString FileFormat;
    if (ExportMode==EXPORTMODE_ADVANCED) {
        int OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
        FileFormat=QString(FORMATDEF[OutputFileFormat].LongName)+" (*."+QString(FORMATDEF[OutputFileFormat].FileExtension)+")";
    } else {
        QString Device=ui->DeviceModelCB->currentText();
        int i=0;
        while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
        if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
            FileFormat=QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].LongName)+" (*."+
                    QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].FileExtension)+")";
        }
    }
    //QString Path=QFileInfo(ui->DestinationFilePath->text()).absolutePath();
    //QString OutputFileName  =QFileDialog::getSaveFileName(this,QApplication::translate("DlgRenderVideo","Select destination file"),Path,FileFormat);
    QString OutputFileName  =QFileDialog::getSaveFileName(this,QApplication::translate("DlgRenderVideo","Select destination file"),ui->DestinationFilePath->text(),FileFormat);
    if (OutputFileName!="") {
        Diaporama->ApplicationConfig->LastRenderVideoPath=QFileInfo(OutputFileName).dir().absolutePath();
        ui->DestinationFilePath->setText(OutputFileName);
        AdjustDestinationFile();
    }
}

//====================================================================================================================

void DlgRenderVideo::FileFormatCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::FileFormatCombo");

    AdjustDestinationFile();

    ui->VideoFormatCB->clear();
    ui->AudioFormatCB->clear();

    //********* Video codec part
    int         CurrentFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
    QString     AllowedCodec=FORMATDEF[CurrentFormat].PossibleVideoCodec;
    QString     Codec="";
    QString     ToSelect="";
    QStringList List;
    int         Index=0;
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
            List.append(QString("%1#####%2").arg(VIDEOCODECDEF[Index].LongName).arg(Index));
            if (Codec==QString(VideoCodec)) ToSelect=QString(VIDEOCODECDEF[Index].LongName);
        }
    }
    // Sort List
    for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) if (List[j]>List[j+1]) List.swap(j,j+1);
    // Fill combo
    for (int i=0;i<List.count();i++) {
        Codec=List[i].left(List[i].indexOf("#####"));
        Index=List[i].mid(List[i].indexOf("#####")+QString("#####").length()).toInt();
        ui->VideoFormatCB->addItem(Codec,QVariant(Index));
    }
    // Set current selection
    if (ui->VideoFormatCB->findText(ToSelect)!=-1) ui->VideoFormatCB->setCurrentIndex(ui->VideoFormatCB->findText(ToSelect));
        else ui->VideoFormatCB->setCurrentIndex(0);
    ui->VideoFormatCB->setEnabled(ui->VideoFormatCB->count()>0);

    //********* Audio codec part
    AllowedCodec=FORMATDEF[CurrentFormat].PossibleAudioCodec;
    Codec="";
    Index=0;
    ToSelect="";
    List.clear();
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
            List.append(QString("%1#####%2").arg(AUDIOCODECDEF[Index].LongName).arg(Index));
            if (Codec==QString(AudioCodec)) ToSelect=QString(AUDIOCODECDEF[Index].LongName);
        }
    }
    // Sort List
    for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) if (List[j]>List[j+1]) List.swap(j,j+1);
    // Fill combo
    for (int i=0;i<List.count();i++) {
        Codec=List[i].left(List[i].indexOf("#####"));
        Index=List[i].mid(List[i].indexOf("#####")+QString("#####").length()).toInt();
        ui->AudioFormatCB->addItem(Codec,QVariant(Index));
    }
    // Set current selection
    if (ui->AudioFormatCB->findText(ToSelect)!=-1) ui->AudioFormatCB->setCurrentIndex(ui->AudioFormatCB->findText(ToSelect));
        else ui->AudioFormatCB->setCurrentIndex(0);
    ui->AudioFormatCB->setEnabled(ui->AudioFormatCB->count()>0);
}

//====================================================================================================================

void DlgRenderVideo::InitVideoBitRateCB(int ChangeIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::InitVideoBitRateCB");

    if (ExportMode==MODE_LOSSLESS) {
        QString Text="Format=\tMKV\nVideo=\tx264 lossless";
        int ImgSize=ImageSize=ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();
        int ExtendH   =0;
        int ExtendV   =0;
        int W=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Width;
        int H=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Height;
        Text=Text+QString("-%1").arg(W)+"x"+QString("%1").arg(H);
        if (ExtendH>0) Text=Text+"+PADLEFT:"+QString("%1").arg(ExtendH/2)+"+PADRIGHT:"+QString("%1").arg(ExtendH-ExtendH/2);
        if (ExtendV>0) Text=Text+"+PADTOP:"+QString("%1").arg(ExtendV/2)+"+PADBOTTOM:"+QString("%1").arg(ExtendV-ExtendV/2);
        if (ui->IncludeSoundCB->isChecked()) Text=Text+"\nAudio=\tFLAC";
        ui->RenderFormatText->setText(Text);
        AdjustDestinationFile();
    } else {
        ui->VideoBitRateCB->clear();
        int Standard    =ui->StandardCombo->currentIndex();
        int CurrentCodec=ui->VideoFormatCB->currentIndex();
        int CurrentSize =ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();
        if (CurrentCodec>=0) {
            CurrentCodec=ui->VideoFormatCB->itemData(CurrentCodec).toInt();

            QString     AllowedBitRate;
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
                if ((ChangeIndex==-1)&&(List[i]==QString("%1k").arg(VideoBitRate))) {
                    ui->VideoBitRateCB->setCurrentIndex(ui->VideoBitRateCB->count()-1);
                    IsFindBitRate=true;
                }
            }
            if (!IsFindBitRate) ui->VideoBitRateCB->setCurrentIndex(ui->VideoBitRateCB->findText(VIDEOCODECDEF[CurrentCodec].DefaultBitrate[Standard][CurrentSize]));
            ui->VideoBitRateCB->setEnabled(ui->VideoBitRateCB->count()>1);
        } else ui->VideoBitRateCB->setEnabled(false);
    }
}

//====================================================================================================================

void DlgRenderVideo::InitAudioBitRateCB(int ChangeIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::InitAudioBitRateCB");

    ui->AudioBitRateCB->clear();
    int CurrentCodec=ui->AudioFormatCB->currentIndex();
    if (CurrentCodec>=0) {
        CurrentCodec=ui->AudioFormatCB->itemData(CurrentCodec).toInt();

        QString     AllowedBitRate;
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
            if ((ChangeIndex==-1)&&(List[i]==QString("%1k").arg(AudioBitRate))) {
                ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->count()-1);
                IsFindBitRate=true;
            }
        }
        if (!IsFindBitRate) ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->findText(AUDIOCODECDEF[CurrentCodec].Default));
        ui->AudioBitRateCB->setEnabled(ui->AudioBitRateCB->count()>1);
    } else ui->AudioBitRateCB->setEnabled(false);
}

//====================================================================================================================

void DlgRenderVideo::AdjustDestinationFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::AdjustDestinationFile");

    QString FileFormat;
    if (ExportMode==EXPORTMODE_ADVANCED) {
        int OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
        FileFormat=FORMATDEF[OutputFileFormat].FileExtension;
    } else {
        if (ExportMode!=MODE_LOSSLESS) {
            QString Device=ui->DeviceModelCB->currentText();
            int i=0;
            while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
            if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
                FileFormat=QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].FileExtension);
            }
        } else {
            FileFormat="mkv";
        }
    }
    QString OutputFileName   =ui->DestinationFilePath->text();

    OutputFileName=QFileInfo(OutputFileName).absoluteFilePath();
    if (QFileInfo(OutputFileName).suffix().length()>0) OutputFileName=OutputFileName.left(OutputFileName.length()-QFileInfo(OutputFileName).suffix().length()-1);
    if (OutputFileName.endsWith(".")) OutputFileName=OutputFileName.left(OutputFileName.length()-1);
    OutputFileName=AdjustDirForOS(OutputFileName+"."+FileFormat);
    ui->DestinationFilePath->setText(OutputFileName);
}

//====================================================================================================================

void DlgRenderVideo::reject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::reject");

    if (IsDestFileOpen) {
        StopProcessWanted=true;
        ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Stop rendering"));
    } else QCustomDialog::reject();
}

//====================================================================================================================

void DlgRenderVideo::s_IncludeSound() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::s_IncludeSound");

    ui->AudioFormatLabel->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioFormatCB->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioBitRateLabel->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioBitRateCB->setEnabled(ui->IncludeSoundCB->isChecked());
    if (ExportMode!=EXPORTMODE_ADVANCED) s_DeviceModelCB(ui->DeviceModelCB->currentIndex());
}

//====================================================================================================================

void DlgRenderVideo::s_DeviceModelCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::s_DeviceModelCB");

    QString Device=ui->DeviceModelCB->currentText();
    int i=0;
    while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
    if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
        int Standard=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->Standard;
        QString Text="Format=\t"+QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].LongName)+"\nVideo=\t";
        Text=Text+VIDEOCODECDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoCodec].LongName;
        int ImgSize=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->ImageSize;
        int ExtendH   =0;
        int ExtendV   =0;
        int W=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Width;
        int H=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Height;
        Text=Text+QString("-%1").arg(W)+"x"+QString("%1").arg(H);
        if (ExtendH>0) Text=Text+"+PADLEFT:"+QString("%1").arg(ExtendH/2)+"+PADRIGHT:"+QString("%1").arg(ExtendH-ExtendH/2);
        if (ExtendV>0) Text=Text+"+PADTOP:"+QString("%1").arg(ExtendV/2)+"+PADBOTTOM:"+QString("%1").arg(ExtendV-ExtendV/2);

        QString FPS=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS==24?"24 FPS":
                    DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS==25?"25 FPS":
                    DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS==24000L/1001L?"23.98 FPS":
                    DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS==30000L/1001L?"29.97 FPS":
                    "";
                                                                                          ;
        QString VideoBitRateStr=QString("%1").arg(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoBitrate); if (VideoBitRateStr.endsWith("000")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-3)+"k";
        QString AudioBitRateStr=QString("%1").arg(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioBitrate); if (AudioBitRateStr.endsWith("000")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-3)+"k";

        Text=Text+"-"+FPS+"-"+VideoBitRateStr+"b/s";
        if (ui->IncludeSoundCB->isChecked()) Text=Text+"\nAudio=\t"+AUDIOCODECDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioCodec].LongName+"-"+AudioBitRateStr+"b/s";
        ui->RenderFormatText->setText(Text);

    } else ui->RenderFormatText->setText("");
    AdjustDestinationFile();
}

//====================================================================================================================

// Video codec part
bool DlgRenderVideo::ComputeVideoPart(QString &vCodec) {
    bool    Continue=true;
    #ifdef LIBAV_08
    bool    isAVCONV=(Diaporama->ApplicationConfig->BinaryEncoderPath=="avconv");
    #endif

    switch (VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC) {

        case VCODEC_H263    :
        case VCODEC_THEORA  :
        case VCODEC_MPEG    :
        case VCODEC_VP8     :
        case VCODEC_H264HQ  :
        case VCODEC_H264PQ  :
        case VCODEC_X264LL  :
            vCodec=QString("-vcodec %1 -pix_fmt yuv420p -b:0 %2").arg(VIDEOCODECDEF[VideoCodecIndex].ShortName).arg(VideoBitRate);
            break;

        case VCODEC_MJPEG   :
            vCodec=QString("-vcodec %1 -qscale 2 -qmin 2 -qmax 2").arg(VIDEOCODECDEF[VideoCodecIndex].ShortName);
            break;

        case VCODEC_MPEG4   :
            if (AudioCodec.contains("amr",Qt::CaseInsensitive)) vCodec=QString("-f 3gp -vcodec mpeg4 -pix_fmt yuv420p -b:0 %1").arg(VideoBitRate);
                else                                            vCodec=QString("-vcodec %1 -pix_fmt yuv420p -b:0 %2").arg(VIDEOCODECDEF[VideoCodecIndex].ShortName).arg(VideoBitRate);
            break;

        default:
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Unknown video codec");
            Continue=false;
            break;
    }
    if ((VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_MPEG)||(VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_VP8))
        vCodec=vCodec+" -bf 3";

    if ((VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_VP8)||(VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_H264PQ)||(VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_H264HQ))
        vCodec=vCodec+QString(" -minrate %1 -maxrate %2 -bufsize %3").arg(VideoBitRate-VideoBitRate/10).arg(VideoBitRate+VideoBitRate/10).arg(VideoBitRate*2);

    if ((VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_X264LL)||(VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_H264PQ)||(VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_H264HQ)) {
        QString Preset=AdjustDirForOS(QDir::currentPath()); if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();

        if (VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_H264HQ) {
            #ifdef LIBAV_07
            Preset="-fpre \""+Preset+"libx264-hq.ffpreset\"";
            #endif
            #ifdef LIBAV_08
            Preset="-preset veryfast -refs:0 3";
            //if (isAVCONV)   Preset="-preset veryfast -refs:0 3";
            //    else        Preset="-preset veryfast -x264opts ref=3";
            #endif
        } else if (VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_H264PQ) {
            #ifdef LIBAV_07
            Preset="-fpre \""+Preset+"libx264-pq.ffpreset\"";
            #endif
            #ifdef LIBAV_08
            Preset="-preset veryfast -profile:v baseline -tune:v fastdecode";
            //if (isAVCONV)   Preset="-preset veryfast -profile:v baseline -tune:v fastdecode";
            //    else        Preset="-preset veryfast -x264opts level=1.3:no-cabac:vbv-bufsize=768:vbv-maxrate=768";
            #endif
        } else if (VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC==VCODEC_X264LL) {
            #ifdef LIBAV_07
            Preset="-fpre \""+Preset+"libx264-lossless.ffpreset\"";
            #endif
            #ifdef LIBAV_08
            Preset="-preset veryfast -qp 0 ";
            #endif
        }
        vCodec=vCodec+" "+Preset;
    }

    #ifdef LIBAV_07
    vCodec.replace(" -b:0 "," -b "); // switch to old syntax
    #endif
    #ifdef LIBAV_08
    if (isAVCONV) vCodec.replace("-vcodec libx264","-vcodec libx264 -vsync vfr"); // add vsync vfr for h264 (AVCONV only)
    #endif

    return Continue;
}

//====================================================================================================================

// Audio codec part
bool DlgRenderVideo::ComputeAudioPart(QString &aCodec) {
    bool    Continue=true;

    if (!ui->IncludeSoundCB->isChecked()) {
        aCodec="-an";
        return true;
    } else {
        int AudioChannels =2;

        switch (AUDIOCODECDEF[AudioCodecIndex].Codec_id) {

            case CODEC_ID_PCM_S16LE:
                aCodec=QString("-acodec copy");             // Use copy instead of -acodec pcm_s16le because we generate temp.wav as pcm_s16le !
                break;

            case CODEC_ID_FLAC:
                aCodec=QString("-acodec flac");
                break;

            case CODEC_ID_AC3:
                aCodec=QString("-acodec %1 -sample_fmt:1 flt -ab %2").arg(AUDIOCODECDEF[AudioCodecIndex].ShortName).arg(AudioBitRate);
                break;

            case CODEC_ID_MP2:
            case CODEC_ID_MP3:
            case CODEC_ID_AAC:
            case CODEC_ID_VORBIS:
            case CODEC_ID_AMR_WB:
            case CODEC_ID_AMR_NB:
                aCodec=QString("-acodec %1 -ab %2").arg(AUDIOCODECDEF[AudioCodecIndex].ShortName).arg(AudioBitRate);
                break;

            default:
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Unknown audio codec");
                Continue=false;
                break;
        }
        if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==CODEC_ID_AMR_WB) {
            AudioChannels=1;
            AudioFrequency=16000;
        } else if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==CODEC_ID_AMR_NB) {
            AudioChannels=1;
            AudioFrequency=8000;
        }
        if (QString(AUDIOCODECDEF[AudioCodecIndex].ShortName)==QString("aac"))          aCodec=aCodec+" -strict experimental -absf aac_adtstoasc";
        if (AudioFrequency!=48000)                                                      aCodec=aCodec+QString(" -ar %1").arg(AudioFrequency);
        if (AudioChannels !=2)                                                          aCodec=aCodec+QString(" -ac %1").arg(AudioChannels);

        #ifdef LIBAV_07
            aCodec=aCodec+" -alang "+Language;
        #endif
        #ifdef LIBAV_08
            aCodec=aCodec+" -metadata:s:1 language="+Language;
        #endif

        return Continue;
    }
}

//====================================================================================================================

// Audio codec part
bool DlgRenderVideo::ComputeTAGPart(QString &aTAG,bool WithChapters) {
    bool    isAVCONV=(Diaporama->ApplicationConfig->BinaryEncoderPath=="avconv");

    // Create metadata temp file
    TempMETAFileName=AdjustDirForOS(QFileInfo(OutputFileName).absolutePath());
    if (!TempMETAFileName.endsWith(QDir::separator())) TempMETAFileName=TempMETAFileName+QDir::separator();
    TempMETAFileName=TempMETAFileName+"TAG.txt";

    QFile   File(TempMETAFileName);
    if (File.open(QFile::WriteOnly|QFile::Truncate|QFile::Text)) {
        QTextStream out(&File);
        /*              M4V/MKV/MP4/MOV/FLV/OGV     AVI     WEBM    MPG/3GP
            title               X                   X       X          -
            author              X                   X       -          -
            album               X                   X       -          -
            year                X                   X       -          -
            comment             X                   X       -          -
            composer            X                   -       -          -


            language    Ok=3GP/MKV/MP4/M4V/MOV/OGV/WEBM/AVI/FLV         Ko=MPG
        */
        out<<";FFMETADATA1\n";    // Write header
        out<<QString("title="+AdjustMETA(Diaporama->ProjectInfo->Title==""?QFileInfo(OutputFileName).baseName():Diaporama->ProjectInfo->Title));
        out<<QString("artist="+AdjustMETA(Diaporama->ProjectInfo->Author));
        out<<QString("album="+AdjustMETA(Diaporama->ProjectInfo->Album));
        out<<QString("comment="+AdjustMETA(Diaporama->ProjectInfo->Comment));
        out<<QString("date="+QString("%1").arg(Diaporama->ProjectInfo->Year)+"\n");
        out<<QString("composer="+AdjustMETA(Diaporama->ProjectInfo->Composer));
        out<<QString("language="+Language+"\n");
        out<<QString("creation_time="+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\n");   // ISO 8601 format

        if (WithChapters) for (int i=FromSlide;i<=ToSlide;i++) if ((i==FromSlide)||(Diaporama->List[i]->StartNewChapter)) {
            int NextChapter=i+1;
            qlonglong Start   =Diaporama->GetObjectStartPosition(i)+(i>FromSlide?Diaporama->List[i]->GetTransitDuration():0)-Diaporama->GetObjectStartPosition(FromSlide);
            qlonglong Duration=Diaporama->List[i]->GetDuration()-(i>FromSlide?Diaporama->List[i]->GetTransitDuration():0);
            while ((NextChapter<ToSlide)&&(!Diaporama->List[NextChapter]->StartNewChapter)) {
                Duration=Duration+Diaporama->List[NextChapter]->GetDuration();
                NextChapter++;
                if (NextChapter<ToSlide) Duration=Duration-Diaporama->List[NextChapter]->GetTransitDuration();
            }
            out<<"[CHAPTER]\n";                                                         // Write Chapter header
            out<<"TIMEBASE=1/1000000\n";                                                // Timebase
            out<<"START="+QString("%1").arg(Start*1000)+"\n";                           // Start position in ms
            out<<"END="+QString("%1").arg((Start+Duration)*1000)+"\n";                  // End position in ms
            if (isAVCONV)   out<<"TITLE="+Diaporama->List[i]->SlideName+"\n";           // Title
                else        out<<"title="+Diaporama->List[i]->SlideName+"\n";           // Title
        }

        File.close();

        #ifdef LIBAV_07
        aTAG=" -i \""+TempMETAFileName+"\"  -map_metadata g:0 -map_chapters 0";
        #endif
        #ifdef LIBAV_08
        if (isAVCONV) aTAG=" -i \""+TempMETAFileName+"\" -map_metadata "+(ui->IncludeSoundCB->isChecked()?"2":"1")+":g -map_chapters "+(ui->IncludeSoundCB->isChecked()?"2":"1");
            else      aTAG=" -i \""+TempMETAFileName+"\" -map_metadata:g 0:g -map_chapters "+(ui->IncludeSoundCB->isChecked()?"2":"1"); // For ffmpeg, mapping is to destination file (for avconv is to source !)
        #endif
        return true;
    } else return false;

}

//====================================================================================================================

void DlgRenderVideo::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::DoAccept");

    cDiaporamaObjectInfo    *PreviousFrame  =NULL;
    cDiaporamaObjectInfo    *Frame          =NULL;
    QString                 vCodec="";
    QString                 aCodec="";
    QString                 StreamFormat="";
    QString                 TAG="";
    QString                 ffmpegCommand;
    QProcess                Process;
    bool                    RefreshDisplay;
    int                     DurationProcess;        // Display information
    QString                 DisplayText;            // Display information

    if (IsDestFileOpen) {
        StopProcessWanted=true;
        ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Stop rendering"));
    } else {
        TempWAVFileName ="";
        TempMETAFileName="";
        AudioFrequency  =48000;
        RenderedFrame   =0;
        FromSlide       =(ui->RenderZoneFromBt->isChecked())?ui->RenderZoneFromED->value()-1:0;
        ToSlide         =(ui->RenderZoneFromBt->isChecked())?ui->RenderZoneToED->value()-1:Diaporama->List.count()-1;
        int ExtendH     =0;
        int ExtendV     =0;

        if (FromSlide>ToSlide) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Range selection"),
                QApplication::translate("DlgRenderVideo","Slide range is defined to incorrect values"));
            ui->RenderZoneToED->setFocus();
            return;
        }

        Language=ui->LanguageED->text();
        if (Language=="") Language="und";
        if (Language.length()!=3) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Language selection"),
                QApplication::translate("DlgRenderVideo","Language must be empty or an ISO 639 language code (3 characters)\nSee help for more details!"));
            ui->LanguageED->setFocus();
            return;
        }

        if (OutputFileName !=ui->DestinationFilePath->text()) OutputFileName=ui->DestinationFilePath->text();

        if ((QFileInfo(OutputFileName).exists())&&(CustomMessageBox(this,QMessageBox::Question,QApplication::translate("DlgRenderVideo","Overwrite file ?"),
            QApplication::translate("DlgRenderVideo","The file you selected already exist.\nDo you want to overwrite it ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes)) return;

        // Get values from controls

        if (ExportMode==EXPORTMODE_ADVANCED) {
            Standard =ui->StandardCombo->currentIndex();
            ImageSize=ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();

            OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
            VideoFrameRate  =DefImageFormat[ui->StandardCombo->currentIndex()][ui->GeometryCombo->currentIndex()][ImageSize].dFPS;
            // Video codec
            VideoCodecIndex=ui->VideoFormatCB->currentIndex();
            if (VideoCodecIndex<0) {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Video codec error!");
                done(0);
                return;
            }
            VideoCodecIndex=ui->VideoFormatCB->itemData(VideoCodecIndex).toInt();
            QString BitRate=ui->VideoBitRateCB->currentText();
            if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1)+"000";
            VideoBitRate=BitRate.toInt();

            // Audio codec
            AudioCodecIndex=ui->AudioFormatCB->currentIndex();
            if (AudioCodecIndex<0) {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Audio codec error!");
                done(0);
                return;
            }
            AudioCodecIndex=ui->AudioFormatCB->itemData(AudioCodecIndex).toInt();
            BitRate=ui->AudioBitRateCB->currentText();
            if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1)+"000";
            AudioBitRate=BitRate.toInt();
            ExtendV =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Extend*2;

        } else if (ExportMode==MODE_LOSSLESS) {

            Standard        =Diaporama->ApplicationConfig->DefaultStandard;
            ImageSize       =ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();
            VideoFrameRate  =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].dFPS;
            OutputFileFormat=2;             // MKV
            VideoCodecIndex =8;             // x264 lossless
            AudioCodecIndex =7;             // FLAC
            VideoBitRate    =0;             // Not use
            AudioBitRate    =0;             // Not use
            ExtendV         =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Extend*2;

        } else {

            OutputFileFormat=0;
            QString Device=ui->DeviceModelCB->currentText();
            if (Device=="") {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Device model selection"),
                    QApplication::translate("DlgRenderVideo","A device model is require!"));
                ui->DeviceModelCB->setFocus();
                return;
            }
            int i=0;
            while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
            if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
                Standard        =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->Standard;
                OutputFileFormat=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat;
                VideoCodecIndex =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoCodec;
                AudioCodecIndex =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioCodec;
                ImageSize       =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->ImageSize;
                VideoFrameRate  =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].dFPS;
                VideoBitRate    =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoBitrate;
                AudioBitRate    =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioBitrate;
                ExtendV         =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Extend*2;
            }
        }

        //QString FileFormat  =FORMATDEF[OutputFileFormat].FileExtension;
        VideoCodec=VIDEOCODECDEF[VideoCodecIndex].FFD_VCODECST;
        AudioCodec=AUDIOCODECDEF[AudioCodecIndex].ShortName;

        // Special case adjustment
        if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==CODEC_ID_PCM_S16LE)    AudioBitRate=1536;      // Special case for WAV
        if (QString(FORMATDEF[OutputFileFormat].ShortName)==QString("flv")) AudioFrequency=44100;   // Special case for FLV
        if (QString(FORMATDEF[OutputFileFormat].ShortName)==QString("3gp")) AudioFrequency=8000;    // Special case for AMRNB

        //*****************
        QString VideoBitRateStr=QString("%1").arg(VideoBitRate); if (VideoBitRateStr.endsWith("000")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-3)+"k";
        QString AudioBitRateStr=QString("%1").arg(AudioBitRate); if (AudioBitRateStr.endsWith("000")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-3)+"k";

        ui->InformationLabel1->setText(OutputFileName);
        ui->InformationLabel2->setText(DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Name);
        ui->InformationLabel3->setText(QString(VIDEOCODECDEF[VideoCodecIndex].LongName)+" - "+(VideoBitRateStr!="0"?VideoBitRateStr+"b/s":"lossless"));
        ui->InformationLabel4->setText(QString(AUDIOCODECDEF[AudioCodecIndex].LongName)+QString(" - %1 Hz - ").arg(AudioFrequency)+(AudioBitRateStr!="0"?AudioBitRateStr+"b/s":"lossless"));

        //**********************************************************************************************************************************
        if ((VideoFrameRate>=29.96)&&(VideoFrameRate<=29.98))    VideoFrameRate=29.97;              // Manual rounded

        FPS             =double(AV_TIME_BASE)/25;                                                   // For sound generation, use only 25 FPS to avoid rounded issue (instead of VideoFrameRate)
        NbrFrame        =int(double(Diaporama->GetPartialDuration(FromSlide,ToSlide)*1000)/FPS);    // Number of frame to generate
        StartTime       =QTime::currentTime();                                                      // Display control : time the process start

        ui->SoundProgressBar->setValue(0);
        ui->SoundProgressBar->setMaximum(NbrFrame);
        ui->SlideProgressBar->setValue(0);
        ui->SlideProgressBar->setMaximum(0);
        ui->TotalProgressBar->setValue(0);
        ui->TotalProgressBar->setMaximum(0);
        ui->SlideNumberLabel->setText("");
        ui->FrameNumberLabel->setText("");

        //**********************************************************************************************************************************
        // Switch dialog to rendering mode
        //**********************************************************************************************************************************

        bool Continue=true;                                  // Loop control
        IsDestFileOpen      =true;
        StopProcessWanted   =false;

        // if process encoding was not started then start it

        // Only ProcessWidget must be display and enable
        ui->SelectDestScroll->setEnabled(false);    ui->SelectDestScroll->setVisible(false);
        ui->ProcessWidget->setEnabled(true);        ui->ProcessWidget->setVisible(true);
        ui->SelectDestScroll->setVisible(false);

        // Disable unwanted buttons
        ui->OkBt->setEnabled(false);
        ui->ProjectPropertiesBt->setEnabled(false);
        ui->HelpBT->setEnabled(false);

        ui->CancelBt->setToolTip(QApplication::translate("DlgRenderVideo","Stop rendering process and exit dialog box"));

        //**********************************************************************************************************************************
        // 1st step encoding : produce WAV file with sound
        //**********************************************************************************************************************************

        if (ui->IncludeSoundCB->isChecked()) {
            // Create tempwavefile in the same directory as destination file
            TempWAVFileName=AdjustDirForOS(QFileInfo(OutputFileName).absolutePath());
            if (!TempWAVFileName.endsWith(QDir::separator())) TempWAVFileName=TempWAVFileName+QDir::separator();
            TempWAVFileName=TempWAVFileName+"temp.wav";
            ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Encoding sound"));
            Continue=WriteTempAudioFile(TempWAVFileName,FromSlide);
        } else {
            ui->SoundProgressLabel->setEnabled(false);
            ui->SoundProgressBar->setEnabled(false);
        }

        //**********************************************************************************************************************************
        // 2nd step encoding : produce final file using temporary WAV file with sound
        //**********************************************************************************************************************************
        //StartTime=QTime::currentTime();                                                             // Display control : time the process start
        FPS             =double(AV_TIME_BASE)/VideoFrameRate;
        NbrFrame        =int(double(Diaporama->GetPartialDuration(FromSlide,ToSlide)*1000)/FPS);    // Number of frame to generate

        ui->SlideProgressBar->setValue(0);
        ui->TotalProgressBar->setValue(0);
        ui->TotalProgressBar->setMaximum(NbrFrame);
        ui->SlideNumberLabel->setText("");
        ui->FrameNumberLabel->setText("");

        int GeoW        =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].PARNUM;
        int GeoH        =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].PARDEN;
        int W           =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Width;
        int H           =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Height;
        int UpdateWidth =W;

        // Special case for SD-DVD format (anamorphous)
        if (W==720) switch (Diaporama->ImageGeometry) {
            case GEOMETRY_4_3:      W=(Standard==STANDARD_PAL?768:640);     GeoW=4;     GeoH=3;  break;
            case GEOMETRY_16_9:     W=(Standard==STANDARD_PAL?1024:852);    GeoW=16;    GeoH=9;  break;
            case GEOMETRY_40_17:    W=(Standard==STANDARD_PAL?1356:1130);   GeoW=40;    GeoH=17; break;
        }

        Continue=Continue && ComputeVideoPart(vCodec);
        Continue=Continue && ComputeAudioPart(aCodec);
        if (QString(FORMATDEF[OutputFileFormat].ShortName)=="mpeg") StreamFormat=" -f mpegts";

        #ifdef LIBAV_TAGCHAPTERS
        Continue=Continue && ComputeTAGPart(TAG,(OutputFileFormat!=2)||(ExportMode==MODE_LOSSLESS));
        #endif

        // Construct ffmpeg command line
        if (Continue) {
            ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Start avconv encoder"));

            #ifdef Q_OS_WIN
            ffmpegCommand="\""+Diaporama->ApplicationConfig->BinaryEncoderPath+"\"";
            #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
            ffmpegCommand=Diaporama->ApplicationConfig->BinaryEncoderPath;
            #endif

            ffmpegCommand=ffmpegCommand+QString(" -y -f image2pipe -vcodec ppm -r %1 -i -").arg(DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].FPS)+
                (ui->IncludeSoundCB->isChecked()?" -i \""+TempWAVFileName+"\"":"")+
                TAG+
                #ifdef LIBAV_07
                " -timestamp now"+
                #endif
                QString(" -dframes %1%2 %3 -r %4 ").arg(NbrFrame).arg(StreamFormat).arg(vCodec).arg(DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].FPS)+
                //(UpdateWidth!=W?QString(" -s %1x%2").arg(UpdateWidth).arg(H+ExtendV):"")+
                #ifdef LIBAV_08
                    #if (LIBAVCODEC_VERSION_MAJOR>=54)
                        ((Diaporama->ApplicationConfig->BinaryEncoderPath=="avconv")?QString(" -filter:v scale=%1:%2:flags=bicubic ").arg(DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Width).arg(DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Height):
                    #else
                        (
                    #endif
                      " -sws_flags bicubic ")+
                #endif
                QString("%1 -aspect %2:%3").arg(aCodec).arg(GeoW).arg(GeoH);

            // Activate multithreading support if getCpuCount()>1 and codec is h264 or VP8
            if (((getCpuCount()-1)>1)&&((VIDEOCODECDEF[VideoCodecIndex].Codec_id==CODEC_ID_H264)||(VIDEOCODECDEF[VideoCodecIndex].Codec_id==CODEC_ID_VP8)))
                ffmpegCommand=ffmpegCommand+" -threads "+QString("%1").arg(getCpuCount()-1);

            ffmpegCommand=ffmpegCommand+" \""+OutputFileName+"\"";

            #ifdef Q_OS_WIN
            ffmpegCommand=ffmpegCommand.replace("30000/1001","29.97");
            ffmpegCommand=ffmpegCommand.replace("24000/1001","23.976");
            #endif

            ToLog(LOGMSG_INFORMATION,ffmpegCommand);

            ffmpegCommand=AdjustDirForOS(ffmpegCommand);
        }

        // Start ffmpegCommand
        if (Continue) {
            Process.setProcessChannelMode(QProcess::/*SeparateChannels*/ForwardedChannels);      // Forward standard and error message to the ffdiaporama console
            Process.start(ffmpegCommand,QIODevice::Append|QIODevice::ReadWrite);                 // Start command
            if (!Process.waitForStarted()) {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Error","Error message"),
                                      QApplication::translate("DlgRenderVideo","Error starting avconv","Error message")+"\n"+ffmpegCommand,
                                      QMessageBox::Close);
                Continue=false;
            }
        }

        // Encode video
        if (Continue) {
            LastCheckTime   =StartTime;                                     // Display control : last time the loop start
            int Position    =Diaporama->GetObjectStartPosition(FromSlide);  // Render current position
            int ColumnStart =-1;                                            // Render start position of current object
            int Column      =-1;                                            // Render current object

            for (RenderedFrame=0;Continue && (RenderedFrame<NbrFrame);RenderedFrame++) {
                int AdjustedDuration=((Column>=0)&&(Column<Diaporama->List.count()))?Diaporama->List[Column]->GetDuration()-Diaporama->GetTransitionDuration(Column+1):0;
                if (AdjustedDuration<33) AdjustedDuration=33; // Not less than 1/30 sec

                if ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+AdjustedDuration)<=Position))) {
                    while ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+AdjustedDuration)<=Position))) {
                        Column++;
                        AdjustedDuration=(Column<Diaporama->List.count())?Diaporama->List[Column]->GetDuration()-Diaporama->GetTransitionDuration(Column+1):0;
                        if (AdjustedDuration<33) AdjustedDuration=33;   // Not less than 1/30 sec
                        ColumnStart=Diaporama->GetObjectStartPosition(Column);
                        if (Column<Diaporama->List.count()) ui->SlideProgressBar->setMaximum(int(double(AdjustedDuration)/(FPS/double(1000)))-1);
                        Diaporama->CloseUnusedLibAv(Column);
                    }
                    RefreshDisplay =true;
                } else RefreshDisplay =(LastCheckTime.msecsTo(QTime::currentTime())>=1000);    // Refresh display only one time per second

                // Refresh Display (if needed)
                if (RefreshDisplay) {
                    DurationProcess=StartTime.msecsTo(QTime::currentTime());
                    double CalcFPS =(double(RenderedFrame)/(double(DurationProcess)/1000));
                    double EstimDur=double(NbrFrame-RenderedFrame)/CalcFPS;
                    DisplayText=QString("%1").arg((QTime(0,0,0,0).addMSecs(DurationProcess)).toString("hh:mm:ss"))+
                            QApplication::translate("DlgRenderVideo"," - Estimated time left : ")+
                            QString("%1").arg(QTime(0,0,0,0).addMSecs(EstimDur*1000).toString("hh:mm:ss"));
                    ui->ElapsedTimeLabel->setText(DisplayText);
                    DisplayText=QString("%1").arg(double(RenderedFrame)/(double(DurationProcess)/1000),0,'f',1);        ui->FPSLabel->setText(DisplayText);
                    DisplayText=QString("%1/%2").arg(Column-FromSlide+1).arg(ToSlide-FromSlide+1);                      ui->SlideNumberLabel->setText(DisplayText);
                    DisplayText=QString("%1/%2").arg(RenderedFrame).arg(NbrFrame);                                      ui->FrameNumberLabel->setText(DisplayText);
                    ui->SlideProgressBar->setValue(int(double(Position-ColumnStart)/(FPS/double(1000))));
                    ui->TotalProgressBar->setValue(RenderedFrame);
                    LastCheckTime=QTime::currentTime();
                }

                // Get current frame
                Frame=new cDiaporamaObjectInfo(PreviousFrame,Position,Diaporama,(FPS/1000));

                // Prepare frame with correct W and H
                Diaporama->LoadSources(Frame,double(H)/double(1080),W,H,false,true);        // Load source images
                Diaporama->DoAssembly(Frame,W,H);                                           // Make final assembly

                // Give time to interface!
                QApplication::processEvents();

                // Apply anamorphous
                if ((UpdateWidth!=W)&&(Frame->RenderedImage->width()!=UpdateWidth)) {
                    QImage *NewImage=new QImage(Frame->RenderedImage->scaled(QSize(UpdateWidth,Frame->RenderedImage->height()),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                    delete Frame->RenderedImage;
                    Frame->RenderedImage=NewImage;
                }

                QImage *ToSave=Frame->RenderedImage;
                if ((ExtendV>0)||(ExtendH>0)) {
                    ToSave=new QImage(W+ExtendH,H+ExtendV,QImage::Format_RGB32);
                    ToSave->fill(0);
                    QPainter P;
                    P.begin(ToSave);
                    P.drawImage(ExtendH/2,ExtendV/2,*Frame->RenderedImage);
                    P.end();
                }

                // Save image to the pipe
                if (!ToSave->save(&Process,"PPM",-1)) {
                    CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Error","Error message"),QApplication::translate("DlgRenderVideo","Error sending image to ffmpeg","Error message"),QMessageBox::Close);
                    Continue=false;
                }

                if (ToSave!=Frame->RenderedImage) delete ToSave;

                // Wait until ffmpeg processed the frame
                while (Continue &&(Process.bytesToWrite()>0)) {
                    if (!Process.waitForBytesWritten()) {
                        CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Error","Error message"),QApplication::translate("DlgRenderVideo","avconv error","Error message"),QMessageBox::Close);
                        Continue=false;
                    }
                    // Give time to interface!
                    QApplication::processEvents();
                    // Stop the process if error occur or user ask to stop
                    Continue=Continue && !StopProcessWanted;
                }

                // Calculate next position
                Position+=(FPS/1000);

                if (PreviousFrame!=NULL) delete PreviousFrame;
                PreviousFrame=Frame;
                Frame =NULL;

                // Stop the process if error occur or user ask to stop
                Continue=Continue && !StopProcessWanted;;
            }
            // Clean PreviousFrame
            if (PreviousFrame!=NULL) delete PreviousFrame;

            ToLog(LOGMSG_INFORMATION,QApplication::translate("DlgRenderVideo","Closing encoder"));

            // Close the pipe to stop ffmpeg process
            Process.closeWriteChannel();

            // Last information update
            DurationProcess=StartTime.msecsTo(QTime::currentTime());
            DisplayText=QString("%1").arg((QTime(0,0,0,0).addMSecs(DurationProcess)).toString("hh:mm:ss"));     ui->ElapsedTimeLabel->setText(DisplayText);
            DisplayText=QString("%1").arg(double(NbrFrame)/(double(DurationProcess)/1000),0,'f',1);             ui->FPSLabel->setText(DisplayText);
            DisplayText=QString("%1/%2").arg(Column-FromSlide+1).arg(ToSlide-FromSlide+1);                      ui->SlideNumberLabel->setText(DisplayText);
            DisplayText=QString("%1/%2").arg(NbrFrame).arg(NbrFrame);                                           ui->FrameNumberLabel->setText(DisplayText);
            ui->SlideProgressBar->setValue(ui->SlideProgressBar->maximum());
            ui->TotalProgressBar->setValue(NbrFrame);

            if (!Process.waitForFinished(30000)) { // 30 sec max to close ffmpeg
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Error","Error message"),QApplication::translate("DlgRenderVideo","Error during avconv process","Error message"),QMessageBox::Close);
                Process.terminate();
                Continue=false;
            } else if (Process.exitStatus()!=QProcess::NormalExit) {
              CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Error","Error message"),QApplication::translate("DlgRenderVideo","Error exiting avconv","Error message"),QMessageBox::Close);
              Continue=false;
            }
        }

        if (TempWAVFileName!="")  QFile::remove(TempWAVFileName);
        if (TempMETAFileName!="") QFile::remove(TempMETAFileName);

        Process.terminate();
        Process.close();

        // Inform user of success
        if (Continue) CustomMessageBox(this,QMessageBox::Information,QApplication::translate("DlgRenderVideo","Render video"),QApplication::translate("DlgRenderVideo","Job completed successfully!"));

        // Save Window size and position
        Diaporama->ApplicationConfig->DlgRenderVideoWSP->SaveWindowState(this);

        // Close the dialog box
        done(0);
    }
}

//============================================================================================

QString DlgRenderVideo::AdjustMETA(QString Text) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::AdjustMETA");

    //Metadata keys or values containing special characters (’=’, ’;’, ’#’, ’\’ and a newline) must be escaped with a backslash ’\’.
    Text.replace("=","\\=");
    Text.replace(";","\\;");
    Text.replace("#","\\#");
    //Text.replace("\\","\\\\");
    Text.replace("\n","\\\n");
    Text=Text+"\n";
    #ifdef Q_OS_WIN
        return Text.toUtf8();
    #else
        return Text;
    #endif
}

//============================================================================================
// Make audio temp file
//============================================================================================

bool DlgRenderVideo::WriteTempAudioFile(QString TempWAVFileName,int FromSlide) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::WriteTempAudioFile");

    bool                    Continue            =true;      // true if no error occur
    cDiaporamaObjectInfo    *PreviousFrame      =NULL;
    cDiaporamaObjectInfo    *Frame              =NULL;
    AVOutputFormat          *Fmt                =NULL;      // No delete needed!
    AVCodec                 *AudioCodec         =NULL;
    QFutureWatcher<void>    ThreadWrite;
    sWriteWAV               WriteWAV;
    double                  RenderFPS           =25;        // For sound generation, use only 25 FPS to avoid rounded issue (instead of VideoFrameRate)

    WriteWAV.AudioCodecContext  =NULL;
    WriteWAV.audio_outbuf       =NULL;
    WriteWAV.AudioStream        =NULL;
    WriteWAV.OutputFormatContext=NULL;

    ui->SoundProgressBar->setMaximum(NbrFrame);

    // Get the container format
    Fmt=av_guess_format(NULL,TempWAVFileName.toUtf8(),NULL);
    if (Fmt==NULL) {
        CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error creating temporary wav file!");
        Continue=false;
    } else Fmt->audio_codec=CODEC_ID_PCM_S16LE;

    // allocate the output media context
    if (Continue) {

        WriteWAV.OutputFormatContext = avformat_alloc_context();
        if (!WriteWAV.OutputFormatContext) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : Unable to allocate OutputFormatContext!");
            Continue=false;
        } else {
            memcpy(WriteWAV.OutputFormatContext->filename,TempWAVFileName.toUtf8(),strlen(TempWAVFileName.toUtf8())+1);
            WriteWAV.OutputFormatContext->oformat  =Fmt;
            WriteWAV.OutputFormatContext->bit_rate =1536;
        }
    }

    // Allocate AudioStream
    if (Continue) {
        #ifdef LIBAV_07
        WriteWAV.AudioStream=av_new_stream(WriteWAV.OutputFormatContext,0);
        #else
        WriteWAV.AudioStream=avformat_new_stream(WriteWAV.OutputFormatContext,0);
        #endif
        if (WriteWAV.AudioStream==NULL) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio stream!");
            Continue=false;
        }
    }

    // Open audio codec
    if (Continue) {
        WriteWAV.AudioCodecContext=WriteWAV.AudioStream->codec;
        #ifdef LIBAV_07
        avcodec_get_context_defaults2(WriteWAV.AudioCodecContext,AVMEDIA_TYPE_AUDIO);    // Fill stream with default values
        #else
        avcodec_get_context_defaults3(WriteWAV.AudioCodecContext,NULL);     // Fill stream with default values
        #endif
        AudioCodec=avcodec_find_encoder(CODEC_ID_PCM_S16LE);                    // Open Audio encoder
        if (!AudioCodec) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Audio codec not found!");
            Continue=false;
        } else {
            WriteWAV.AudioCodecContext->codec_id             = CODEC_ID_PCM_S16LE;
            WriteWAV.AudioCodecContext->codec_type           = AVMEDIA_TYPE_AUDIO;
            WriteWAV.AudioCodecContext->sample_fmt           = AV_SAMPLE_FMT_S16;
            WriteWAV.AudioCodecContext->sample_rate          = 48000;
            WriteWAV.AudioCodecContext->bit_rate             = 48000;
            WriteWAV.AudioCodecContext->rc_max_rate          = 0;
            WriteWAV.AudioCodecContext->rc_min_rate          = 0;
            WriteWAV.AudioCodecContext->bit_rate_tolerance   = 0;
            WriteWAV.AudioCodecContext->rc_buffer_size       = 0;
            WriteWAV.AudioCodecContext->channels             = 2;
            WriteWAV.AudioCodecContext->channel_layout       = AV_CH_STEREO_LEFT|AV_CH_STEREO_RIGHT;    //CH_LAYOUT_STEREO;
            WriteWAV.AudioCodecContext->time_base            = (AVRational){1,WriteWAV.AudioCodecContext->sample_rate};
            WriteWAV.AudioStream->r_frame_rate               = WriteWAV.AudioCodecContext->time_base;
            WriteWAV.AudioStream->time_base                  = WriteWAV.AudioCodecContext->time_base;
            WriteWAV.AudioCodecContext->flags               |= CODEC_FLAG_GLOBAL_HEADER;

            // open the codec
            if (avcodec_open2(WriteWAV.AudioCodecContext,AudioCodec,NULL)<0) {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"could not open audio codec!");
                Continue=false;
            } else {
                // Init sound blocks
                WriteWAV.RenderMusic.SetFPS(RenderFPS);
            }
        }
    }

    // open the file for writing
    if (Continue) {
        int Err=0;
        if ((Err=avio_open(&WriteWAV.OutputFormatContext->pb,TempWAVFileName.toUtf8(),AVIO_FLAG_WRITE))<0) {
            char Buf[500];
            av_strerror(Err,Buf,500);
            ToLog(LOGMSG_CRITICAL,Buf);
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error creating temporary audio file!");
            Continue=false;
        }
    }

    // Allocate buffer to encode
    if (Continue) {
        WriteWAV.audio_outbuf=(uint8_t *)av_malloc(FF_MIN_BUFFER_SIZE);
        if (WriteWAV.audio_outbuf==NULL) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio buffer!");
            Continue=false;
        }
    }

    // write the header
    if ((Continue)&&(avformat_write_header(WriteWAV.OutputFormatContext,NULL)<0)) {
        CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error writing the header of the temporary audio file!");
        Continue=false;
    }

    // Encode the file
    if (Continue) {
        LastCheckTime       =StartTime;                                     // Display control : last time the loop start
        qlonglong Position  =Diaporama->GetObjectStartPosition(FromSlide);  // Render current position
        int ColumnStart     =-1;                                            // Render start position of current object
        int Column          =-1;                                            // Render current object
        for (qlonglong RenderedFrame=0;Continue && (RenderedFrame<NbrFrame);RenderedFrame++) {

            // Calculate position & column
            int AdjustedDuration=((Column>=0)&&(Column<Diaporama->List.count()))?Diaporama->List[Column]->GetDuration()-Diaporama->GetTransitionDuration(Column+1):0;
            if (AdjustedDuration<33) AdjustedDuration=33; // Not less than 1/30 sec
            if ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+AdjustedDuration)<=Position))) {
                Column++;
                AdjustedDuration=((Column>=0)&&(Column<Diaporama->List.count()))?Diaporama->List[Column]->GetDuration()-Diaporama->GetTransitionDuration(Column+1):0;
                if (AdjustedDuration<33) AdjustedDuration=33; // Not less than 1/30 sec
                ColumnStart=Position;
            }
            // Refresh Display (if needed)
            if (LastCheckTime.msecsTo(QTime::currentTime())>=1000) {    // Refresh display only one time per second
                int         DurationProcess =0;
                QString     DisplayText     ="";

                DurationProcess=StartTime.msecsTo(QTime::currentTime());
                DisplayText=QString("%1").arg((QTime(0,0,0,0).addMSecs(DurationProcess)).toString("hh:mm:ss"));     ui->ElapsedTimeLabel->setText(DisplayText);
                DisplayText=QString("%1").arg(double(RenderedFrame)/(double(DurationProcess)/1000),0,'f',1);        ui->FPSLabel->setText(DisplayText);
                LastCheckTime=QTime::currentTime();
                ui->SoundProgressBar->setValue(RenderedFrame);
            }

            // Get current frame
            Frame=new cDiaporamaObjectInfo(PreviousFrame,Position,Diaporama,(FPS/1000)/*((double(AV_TIME_BASE)/RenderFPS)/1000)*/);

            // Ensure MusicTracks are ready
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_MusicTrack==NULL)) {
                Frame->CurrentObject_MusicTrack=new cSoundBlockList();
                Frame->CurrentObject_MusicTrack->SetFPS(RenderFPS);
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
                Frame->TransitObject_MusicTrack=new cSoundBlockList();
                Frame->TransitObject_MusicTrack->SetFPS(RenderFPS);
            }

            // Ensure SoundTracks are ready
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
                Frame->CurrentObject_SoundTrackMontage=new cSoundBlockList();
                Frame->CurrentObject_SoundTrackMontage->SetFPS(RenderFPS);
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
                Frame->TransitObject_SoundTrackMontage=new cSoundBlockList();
                Frame->TransitObject_SoundTrackMontage->SetFPS(RenderFPS);
            }

            // Prepare frame with W and H =0 to force SoundMusicOnly! (thread mode is not necessary here)
            Diaporama->LoadSources(Frame,0,0,0,false,true);

            // Give time to interface!
            QApplication::processEvents();

            // Calc number of packet to mix
            int MaxPacket=Frame->CurrentObject_MusicTrack->List.count();
            if ((Frame->CurrentObject_SoundTrackMontage!=NULL)&&
                (Frame->CurrentObject_SoundTrackMontage->List.count()>0)&&
                (MaxPacket>Frame->CurrentObject_SoundTrackMontage->List.count())) MaxPacket=Frame->CurrentObject_SoundTrackMontage->List.count();
            if (MaxPacket>WriteWAV.RenderMusic.NbrPacketForFPS) MaxPacket=WriteWAV.RenderMusic.NbrPacketForFPS;

            // mix audio data
            for (int j=0;j<MaxPacket;j++)
                WriteWAV.RenderMusic.MixAppendPacket(Frame->CurrentObject_MusicTrack->DetachFirstPacket(),(Frame->CurrentObject_SoundTrackMontage!=NULL)?Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket():NULL);

            // Write audio frame to disk
            if (ThreadWrite.isRunning()) ThreadWrite.waitForFinished();
            ThreadWrite.setFuture(QtConcurrent::run(this,&DlgRenderVideo::WriteRenderedMusicToDisk,&WriteWAV,&Continue));

            QApplication::processEvents();  // Give time to interface!

            // Calculate next position
            Position     +=(FPS/1000);
            if (PreviousFrame!=NULL) delete PreviousFrame;
            PreviousFrame=Frame;
            Frame =NULL;

            // Stop the process if error occur or user ask to stop
            Continue=Continue && !StopProcessWanted;;
        }

        if (ThreadWrite.isRunning()) ThreadWrite.waitForFinished();

        // Write de trailer
        if ((Continue)&&(av_write_trailer(WriteWAV.OutputFormatContext)!=0)) {
            CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error writing the trailer of the temporary audio file!");
            Continue=false;
        }
    }

    // Ensure sound progress bar is at 100%
    if (Continue) ui->SoundProgressBar->setValue(ui->SoundProgressBar->maximum());

    // Clean all

    if (WriteWAV.audio_outbuf)   av_free(WriteWAV.audio_outbuf);
    if (PreviousFrame)  delete PreviousFrame;
    if (Frame)          delete Frame;
    if (WriteWAV.OutputFormatContext) {
        if (WriteWAV.OutputFormatContext->pb) avio_close(WriteWAV.OutputFormatContext->pb);                                   // close the file
        if (WriteWAV.OutputFormatContext->streams[0]) {
            avcodec_close(WriteWAV.AudioStream->codec);                                                              // close codec
            if (WriteWAV.OutputFormatContext->streams[0]->codec) av_freep(&WriteWAV.OutputFormatContext->streams[0]->codec);  // free the audiostream
        }
        av_free(WriteWAV.OutputFormatContext);                                                                       // free the container
    }

    return Continue;
}

void DlgRenderVideo::WriteRenderedMusicToDisk(sWriteWAV *WriteWAV,bool *Continue) {
   ToLog(LOGMSG_DEBUGTRACE,"IN:DlgRenderVideo::WriteTempAudioFile");

    // Flush audio frame
    while (WriteWAV->RenderMusic.List.count()>0) {
        int16_t *PacketSound=WriteWAV->RenderMusic.DetachFirstPacket();
        if (PacketSound==NULL) {
            PacketSound=(int16_t *)av_malloc(WriteWAV->RenderMusic.SoundPacketSize+4);
            memset(PacketSound,0,WriteWAV->RenderMusic.SoundPacketSize);
        }

        #if FF_API_OLD_ENCODE_AUDIO

            // Init frame
            AVFrame *frame=avcodec_alloc_frame();
            if (frame->extended_data!=frame->data) av_freep(&frame->extended_data);
            avcodec_get_frame_defaults(frame);
            frame->nb_samples=WriteWAV->RenderMusic.SoundPacketSize/(WriteWAV->AudioCodecContext->channels*av_get_bytes_per_sample(WriteWAV->AudioCodecContext->sample_fmt));

            // fill buffer
            if (avcodec_fill_audio_frame(frame,WriteWAV->AudioCodecContext->channels,WriteWAV->AudioCodecContext->sample_fmt,(const uint8_t*)PacketSound,WriteWAV->RenderMusic.SoundPacketSize,1)>=0) {

                // Init packet
                AVPacket pkt;
                av_init_packet(&pkt);
                pkt.data = NULL;
                pkt.size = 0;

                int got_packet=0;
                if (avcodec_encode_audio2(WriteWAV->AudioCodecContext,&pkt,frame,&got_packet)<0) {

                    CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error encoding sound!");
                    *Continue=false;

                } else {

                    if ((WriteWAV->AudioCodecContext->coded_frame!=NULL)&&(WriteWAV->AudioCodecContext->coded_frame->pts!=int64_t(INT64_C(0x8000000000000000))))
                        pkt.pts=av_rescale_q(WriteWAV->AudioCodecContext->coded_frame->pts,WriteWAV->AudioCodecContext->time_base,WriteWAV->AudioStream->time_base);

                    if ((WriteWAV->AudioCodecContext->coded_frame!=NULL)&&(WriteWAV->AudioCodecContext->coded_frame->key_frame))
                        pkt.flags|=AV_PKT_FLAG_KEY;

                    pkt.stream_index=WriteWAV->AudioStream->index;

                    // write the compressed frame in the media file
                    if (av_interleaved_write_frame(WriteWAV->OutputFormatContext,&pkt)!=0) {
                        CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error while writing audio frame!");
                        *Continue=false;
                    }
                }
            } else {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error encoding sound!");
                *Continue=false;
            }
            av_free(frame);

        #else
            int out_size= avcodec_encode_audio(WriteWAV->AudioCodecContext,WriteWAV->audio_outbuf,WriteWAV->RenderMusic.SoundPacketSize,(short int *)PacketSound);
            if (out_size>0) {
                AVPacket pkt;
                av_init_packet(&pkt);

                if ((WriteWAV->AudioCodecContext->coded_frame!=NULL)&&(WriteWAV->AudioCodecContext->coded_frame->pts!=int64_t(INT64_C(0x8000000000000000))))
                    pkt.pts=av_rescale_q(WriteWAV->AudioCodecContext->coded_frame->pts,WriteWAV->AudioCodecContext->time_base,WriteWAV->AudioStream->time_base);

                if ((WriteWAV->AudioCodecContext->coded_frame!=NULL)&&(WriteWAV->AudioCodecContext->coded_frame->key_frame))
                    pkt.flags|=AV_PKT_FLAG_KEY;

                pkt.stream_index=WriteWAV->AudioStream->index;
                pkt.data        =WriteWAV->audio_outbuf;
                pkt.size        =out_size;

                // write the compressed frame in the media file
                if (av_interleaved_write_frame(WriteWAV->OutputFormatContext,&pkt)!=0) {
                    CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error while writing audio frame!");
                    *Continue=false;
                }
            } else if (out_size<0) {
                CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgRenderVideo","Render video"),"Error encoding sound!");
                *Continue=false;
            }
        #endif
        av_free(PacketSound);
    }
}
