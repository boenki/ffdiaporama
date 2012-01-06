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

#include "DlgRenderVideo.h"
#include "ui_DlgRenderVideo.h"
#include "mainwindow.h"

//====================================================================================================================

DlgRenderVideo::DlgRenderVideo(cDiaporama &TheDiaporama,int TheExportMode,QWidget *parent):QDialog(parent),ui(new Ui::DlgRenderVideo) {
    ui->setupUi(this);
    Diaporama           =&TheDiaporama;
    ExportMode          =TheExportMode;
    IsDestFileOpen      =false;
    StopSpinboxRecursion=false;
    Language            =Diaporama->ProjectInfo->DefaultLanguage;

    #if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
    #else
    setWindowFlags(Qt::Window|Qt::WindowTitleHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);
    #endif

    switch (ExportMode) {
        case EXPORTMODE_ADVANCED     :  setWindowTitle(QApplication::translate("DlgRenderVideo","Advanced render video"));                          break;
        case MODE_SMARTPHONE   :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for mobil player and smartphone"));   break;
        case MODE_MULTIMEDIASYS:  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for multimedia system"));             break;
        case MODE_FORTHEWEB    :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for the WEB"));                       break;
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
        ui->VideoFormatCB->view()->setFixedWidth(400);
        ui->AudioFormatCB->view()->setFixedWidth(400);

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
        ui->FileFormatLabel->setVisible(false);         ui->FileFormatCB->setVisible(false);
        ui->GeometryLabel->setVisible(false);           ui->GeometryCombo->setVisible(false);
        ui->StandardLabel->setVisible(false);           ui->StandardCombo->setVisible(false);
        ui->ImageSizeLabel->setVisible(false);          ui->ImageSizeCombo->setVisible(false);
        ui->VideoFormatLabel->setVisible(false);        ui->VideoFormatCB->setVisible(false);
        ui->VideoBitRateLabel->setVisible(false);       ui->VideoBitRateCB->setVisible(false);

        ui->AudioFormatLabel->setVisible(false);        ui->AudioFormatLabel->setEnabled(false);
        ui->AudioFormatCB->setVisible(false);           ui->AudioFormatCB->setEnabled(false);
        ui->AudioBitRateLabel->setVisible(false);       ui->AudioBitRateLabel->setEnabled(false);
        ui->AudioBitRateCB->setVisible(false);          ui->AudioBitRateCB->setEnabled(false);

        QStringList List;
        int         Default=ExportMode==MODE_SMARTPHONE?Diaporama->ApplicationConfig->DefaultSmartphoneType:
                            ExportMode==MODE_MULTIMEDIASYS?Diaporama->ApplicationConfig->DefaultMultimediaType:
                            Diaporama->ApplicationConfig->DefaultForTheWEBType;

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

    // Define handler
    connect(ui->CancelBt,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OkBt,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));
}

//====================================================================================================================

DlgRenderVideo::~DlgRenderVideo() {
    delete ui;
}

//====================================================================================================================

void DlgRenderVideo::Help() {
    GlobalMainWindow->OpenHelp(HELPFILE_DlgRenderVideo);
}

//====================================================================================================================

void DlgRenderVideo::SetZoneToAll() {
    ui->RenderZoneAllBt->setChecked(true);
    ui->RenderZoneFromBt->setChecked(false);
    ui->RenderZoneFromED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneToED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneTo->setEnabled(ui->RenderZoneFromBt->isChecked());
}

//====================================================================================================================

void DlgRenderVideo::SetZoneToPartial() {
    ui->RenderZoneAllBt->setChecked(false);
    ui->RenderZoneFromBt->setChecked(true);
    ui->RenderZoneFromED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneToED->setEnabled(ui->RenderZoneFromBt->isChecked());
    ui->RenderZoneTo->setEnabled(ui->RenderZoneFromBt->isChecked());
}

//====================================================================================================================

void DlgRenderVideo::s_DeviceTypeCB(int) {
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
    int Geometry=ui->GeometryCombo->currentIndex();
    int Standard=ui->StandardCombo->currentIndex();
    int ImageSize=ui->ImageSizeCombo->currentIndex();
    ui->ImageSizeCombo->clear();
    QStringList List;
    for (int i=0;i<NBR_SIZEDEF;i++) List.append(QString("%1:%2#####%3").arg(DefImageFormat[Standard][Geometry][i].Name).arg(ORDERIMAGENAME[i]).arg(i));
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
    QString Path=QFileInfo(ui->DestinationFilePath->text()).absolutePath();
    QString OutputFileName  =QFileDialog::getSaveFileName(this,QApplication::translate("DlgRenderVideo","Select destination file"),Path,FileFormat);
    if (OutputFileName!="") {
        Diaporama->ApplicationConfig->LastRenderVideoPath=QFileInfo(OutputFileName).dir().absolutePath();
        ui->DestinationFilePath->setText(OutputFileName);
        AdjustDestinationFile();
    }
}

//====================================================================================================================

void DlgRenderVideo::FileFormatCombo(int) {
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
    ui->VideoBitRateCB->clear();
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
        if (!IsFindBitRate) ui->VideoBitRateCB->setCurrentIndex(ui->VideoBitRateCB->findText(VIDEOCODECDEF[CurrentCodec].DefaultBitrate[CurrentSize]));
        ui->VideoBitRateCB->setEnabled(ui->VideoBitRateCB->count()>1);
    } else ui->VideoBitRateCB->setEnabled(false);
}

//====================================================================================================================

void DlgRenderVideo::InitAudioBitRateCB(int ChangeIndex) {
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
    QString FileFormat;
    if (ExportMode==EXPORTMODE_ADVANCED) {
        int OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
        FileFormat=FORMATDEF[OutputFileFormat].FileExtension;
    } else {
        QString Device=ui->DeviceModelCB->currentText();
        int i=0;
        while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
        if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
            FileFormat=QString(FORMATDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat].FileExtension);
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

void DlgRenderVideo::SetSavedWindowGeometry() {
    Diaporama->ApplicationConfig->DlgRenderVideoWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgRenderVideo::showEvent(QShowEvent *ev) {
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgRenderVideo::reject() {
    if (IsDestFileOpen) {
        StopProcessWanted=true;
        qDebug()<<QApplication::translate("DlgRenderVideo","Stop rendering");
    } else {
        // Save Window size and position
        Diaporama->ApplicationConfig->DlgRenderVideoWSP->SaveWindowState(this);
        done(1);
    }
}

//====================================================================================================================

void DlgRenderVideo::s_IncludeSound() {
    ui->AudioFormatLabel->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioFormatCB->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioBitRateLabel->setEnabled(ui->IncludeSoundCB->isChecked());
    ui->AudioBitRateCB->setEnabled(ui->IncludeSoundCB->isChecked());
    if (ExportMode!=EXPORTMODE_ADVANCED) s_DeviceModelCB(ui->DeviceModelCB->currentIndex());
}

//====================================================================================================================

void DlgRenderVideo::s_DeviceModelCB(int) {
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

        QString VideoBitRateStr=QString("%1").arg(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoBitrate); if (VideoBitRateStr.endsWith("000")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-3)+"k";
        QString AudioBitRateStr=QString("%1").arg(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioBitrate); if (AudioBitRateStr.endsWith("000")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-3)+"k";

        Text=Text+"-"+VideoBitRateStr+"b/s";
        if (ui->IncludeSoundCB->isChecked()) Text=Text+"\nAudio=\t"+AUDIOCODECDEF[Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioCodec].LongName+"-"+AudioBitRateStr+"b/s";
        ui->RenderFormatText->setText(Text);

    } else ui->RenderFormatText->setText("");
    AdjustDestinationFile();
}

//====================================================================================================================

void DlgRenderVideo::accept() {
    cDiaporamaObjectInfo    *PreviousFrame  =NULL;
    cDiaporamaObjectInfo    *Frame          =NULL;
    QString                 TempWAVFileName="",TempMETAFileName="";
    QString                 vCodec="";
    QString                 aCodec="";
    QString                 TAG="";
    QString                 ffmpegCommand;
    int                     W,H;
    QProcess                Process;
    bool                    RefreshDisplay;
    int                     DurationProcess;        // Display informations
    QString                 DisplayText;            // Display informations

    int                     UpdateWidth=0;

    if (IsDestFileOpen) {
        StopProcessWanted=true;
        qDebug()<<QApplication::translate("DlgRenderVideo","Stop rendering");
    } else {
        RenderedFrame=0;
        int FromSlide=(ui->RenderZoneFromBt->isChecked())?ui->RenderZoneFromED->value()-1:0;
        int ToSlide  =(ui->RenderZoneFromBt->isChecked())?ui->RenderZoneToED->value()-1:Diaporama->List.count()-1;

        if (FromSlide>ToSlide) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Range selection"),
                QApplication::translate("DlgRenderVideo","Slide range is defined to incorrect values"));
            ui->RenderZoneToED->setFocus();
            return;
        }

        Language=ui->LanguageED->text();
        if (Language=="") Language="und";
        if (Language.length()!=3) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Language selection"),
                QApplication::translate("DlgRenderVideo","Language must be empty or an ISO 639 language code (3 characters)\nSee help for more details!"));
            ui->LanguageED->setFocus();
            return;
        }

        if (OutputFileName !=ui->DestinationFilePath->text()) OutputFileName=ui->DestinationFilePath->text();

        if ((QFileInfo(OutputFileName).exists())&&(QMessageBox::question(this,QApplication::translate("DlgRenderVideo","Overwrite file ?"),
            QApplication::translate("DlgRenderVideo","The file you selected already exist.\nDo you want to overwrite it ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes)) return;

        bool Continue=true;                                  // Loop control

        // if process encoding was not started then start it

        // Only ProcessWidget must be display and enable
        ui->SelectDestScroll->setEnabled(false);    ui->SelectDestScroll->setVisible(false);
        ui->ProcessWidget->setEnabled(true);        ui->ProcessWidget->setVisible(true);
        ui->OkBt->setEnabled(false);                ui->SelectDestScroll->setVisible(false);

        IsDestFileOpen      =true;
        StopProcessWanted   =false;

        // Get values from controls
        QString     Preset="";
        int         ExtendH=0;
        int         ExtendV=0;
        int         Channels=2;

        if (ExportMode==EXPORTMODE_ADVANCED) {
            Standard =ui->StandardCombo->currentIndex();
            ImageSize=ui->ImageSizeCombo->itemData(ui->ImageSizeCombo->currentIndex()).toInt();

            OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
            VideoFrameRate  =DefImageFormat[ui->StandardCombo->currentIndex()][ui->GeometryCombo->currentIndex()][ImageSize].dFPS;
            // Video codec
            VideoCodecIndex=ui->VideoFormatCB->currentIndex();
            if (VideoCodecIndex<0) {
                QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Video codec error!");
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
                QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Audio codec error!");
                done(0);
                return;
            }
            AudioCodecIndex=ui->AudioFormatCB->itemData(AudioCodecIndex).toInt();
            BitRate=ui->AudioBitRateCB->currentText();
            if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1)+"000";
            AudioBitRate=BitRate.toInt();
            ExtendV =DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Extend*2;

        } else {

            OutputFileFormat=0;
            QString Device=ui->DeviceModelCB->currentText();
            int i=0;
            while ((i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
            if (i<Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel.count()) {
                Standard=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->Standard;
                OutputFileFormat=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->FileFormat;

                VideoCodecIndex =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoCodec;
                AudioCodecIndex =Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioCodec;

                int ImgSize=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->ImageSize;
                VideoFrameRate=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].dFPS;
                ImageSize=ImgSize;
                VideoBitRate=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->VideoBitrate;
                AudioBitRate=Diaporama->ApplicationConfig->DeviceModelList.RenderDeviceModel[i]->AudioBitrate;
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
        ui->InformationLabel3->setText(QString(VIDEOCODECDEF[VideoCodecIndex].LongName)+" - "+VideoBitRateStr+"b/s");
        ui->InformationLabel4->setText(QString(AUDIOCODECDEF[AudioCodecIndex].LongName)+QString(" - %1 Hz - ").arg(AudioFrequency)+AudioBitRateStr+"b/s");

        //**********************************************************************************************************************************

        FPS     =double(AV_TIME_BASE)/double(25);  // Time duration of a frame (pour éviter les problèmes d'arrondi, génère le son en PAL)
        NbrFrame=int(double(Diaporama->GetPartialDuration(FromSlide,ToSlide)*1000)/double(FPS));            // Number of frame to generate

        ui->SoundProgressBar->setValue(0);
        ui->SoundProgressBar->setMaximum(NbrFrame);
        ui->SlideProgressBar->setValue(0);
        ui->SlideProgressBar->setMaximum(0);
        ui->TotalProgressBar->setValue(0);
        ui->TotalProgressBar->setMaximum(0);
        ui->SlideNumberLabel->setText("");
        ui->FrameNumberLabel->setText("");

        //**********************************************************************************************************************************

        GlobalMainWindow->CurrentRenderingDialog=this;

        //**********************************************************************************************************************************
        // 1st step encoding : produce WAV file with sound
        //**********************************************************************************************************************************

        if (ui->IncludeSoundCB->isChecked()) {
            // Create tempwavefile in the same directory as destination file
            TempWAVFileName=AdjustDirForOS(QFileInfo(OutputFileName).absolutePath());
            if (!TempWAVFileName.endsWith(QDir::separator())) TempWAVFileName=TempWAVFileName+QDir::separator();
            TempWAVFileName=TempWAVFileName+"temp.wav";

            StartTime=QTime::currentTime();                                  // Display control : time the process start
            qDebug()<<QApplication::translate("DlgRenderVideo","Encoding sound");
            Continue=WriteTempAudioFile(TempWAVFileName,FromSlide);
        } else {
            ui->SoundProgressLabel->setEnabled(false);
            ui->SoundProgressBar->setEnabled(false);
        }

        //**********************************************************************************************************************************
        // 2nd step encoding : produce final file using temporary WAV file with sound
        //**********************************************************************************************************************************
        StartTime=QTime::currentTime();                                                             // Display control : time the process start
        FPS     =double((uint64_t)AV_TIME_BASE)/double(VideoFrameRate);                             // Time duration of a frame
        NbrFrame=int(double(Diaporama->GetPartialDuration(FromSlide,ToSlide)*1000)/double(FPS));    // Number of frame to generate

        ui->SlideProgressBar->setValue(0);
        ui->TotalProgressBar->setValue(0);
        ui->TotalProgressBar->setMaximum(NbrFrame);
        ui->SlideNumberLabel->setText("");
        ui->FrameNumberLabel->setText("");

        // Construct ffmpeg command line
        if (Continue) {
            W=DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Width;
            H=DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].Height;

            UpdateWidth=W;

            // Video codec part
            switch (VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC) {
                case VCODEC_MPEG    :   vCodec=QString("-vcodec mpeg2video -minrate %1 -maxrate %2 -bufsize %3 -b:0 %4 -bf 3")
                                           .arg(VideoBitRate-VideoBitRate/10)
                                           .arg(VideoBitRate+VideoBitRate/10)
                                           .arg(VideoBitRate*2)
                                           .arg(VideoBitRate);
                                        break;
                case VCODEC_MPEG4   :   if (AudioCodec=="libopencore_amrnb") {
                                                vCodec=QString("-f 3gp -vcodec mpeg4 -b:0 %1").arg(VideoBitRate);
                                        } else {
                                            if (QString(VIDEOCODECDEF[VideoCodecIndex].ShortName)==QString("mpeg4"))
                                                vCodec=QString("-vcodec mpeg4 -vtag xvid -b:0 %1").arg(VideoBitRate);
                                                else vCodec=QString("-vcodec libxvid -b:0 %1").arg(VideoBitRate);
                                        }
                                        break;
                case VCODEC_H264HQ  :   Preset=AdjustDirForOS(QDir::currentPath()); if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();
                                        Preset="-fpre \""+Preset+"libx264-hq.ffpreset\"";
                                        vCodec=QString("-vcodec libx264 ")+Preset+QString(" -minrate %1 -maxrate %2 -bufsize %3 -b:0 %4")
                                            .arg(VideoBitRate-VideoBitRate/10)
                                            .arg(VideoBitRate+VideoBitRate/10)
                                            .arg(VideoBitRate*2)
                                            .arg(VideoBitRate);
                                        break;
                case VCODEC_H264PQ  :   Preset=AdjustDirForOS(QDir::currentPath()); if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();
                                        Preset="-fpre \""+Preset+"libx264-pq.ffpreset\"";
                                        vCodec=QString("-vcodec libx264 ")+Preset+QString(" -minrate %1 -maxrate %2 -bufsize %3 -b:0 %4")
                                            .arg(VideoBitRate-VideoBitRate/10)
                                            .arg(VideoBitRate+VideoBitRate/10)
                                            .arg(VideoBitRate*2)
                                            .arg(VideoBitRate);
                                        break;
                case VCODEC_MJPEG   :   vCodec="-vcodec mjpeg -qscale 2 -qmin 2 -qmax 2";   break;
                case VCODEC_VP8     :   vCodec=QString("-vcodec libvpx -minrate %1 -maxrate %2 -bufsize %3 -b:0 %4 -bf 3")
                                            .arg(VideoBitRate-VideoBitRate/10)
                                            .arg(VideoBitRate+VideoBitRate/10)
                                            .arg(VideoBitRate*2)
                                            .arg(VideoBitRate);
                                        break;
                case VCODEC_H263    :   vCodec=QString("-vcodec flv -b:0 %1").arg(VideoBitRate);
                                        break;
                case VCODEC_THEORA  :   vCodec=QString("-vcodec libtheora -b:0 %1").arg(VideoBitRate);
                                        break;
                default:
                    QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Unknown video codec");
                    Continue=false;
                    break;
            }
            #if (LIBAVFORMAT_VERSION_MAJOR<53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR<28))
            vCodec.replace(" -b:0 "," -b "); // switch to old syntax
            #endif

            // Audio codec part
            if (ui->IncludeSoundCB->isChecked()) {
                switch (AUDIOCODECDEF[AudioCodecIndex].Codec_id) {
                    case CODEC_ID_PCM_S16LE:    aCodec=QString("-acodec pcm_s16le -ab %1").arg(AudioBitRate); break;
                    case CODEC_ID_MP2:          aCodec=QString("-acodec mp2 -ab %1").arg(AudioBitRate); break;
                    case CODEC_ID_MP3:          aCodec=QString("-acodec libmp3lame -ab %1").arg(AudioBitRate); break;
                    case CODEC_ID_AAC:          if (QString(AUDIOCODECDEF[AudioCodecIndex].ShortName)==QString("aac"))
                                                    //aCodec=QString("-acodec aac -strict experimental -ab %1 -absf aac_adtstoasc").arg(AudioBitRate);
                                                    aCodec=QString("-acodec libvo_aacenc -ab %1").arg(AudioBitRate);
                                                    else aCodec=QString("-acodec libfaac -ab %1").arg(AudioBitRate);
                                                break;
                    case CODEC_ID_AC3:          aCodec=QString("-acodec ac3 -ab %1").arg(AudioBitRate); break;
                    case CODEC_ID_VORBIS:       aCodec=QString("-acodec libvorbis -ab %1").arg(AudioBitRate); break;
                    case CODEC_ID_AMR_NB:       aCodec=QString("-acodec libopencore_amrnb -ab %1").arg(AudioBitRate); Channels=1; break;

                    default:
                        QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Unknown audio codec");
                        Continue=false;
                        break;
                }
                #if (LIBAVFORMAT_VERSION_MAJOR<53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR<28))
                aCodec=aCodec+" -alang "+Language;
                #else
                aCodec=aCodec+" -metadata:s:1 language="+Language;
                #endif
            }

            if (Continue) {
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
                    out<<QString("title="+AdjustMETA(Diaporama->ProjectInfo->Title==""?QFileInfo(OutputFileName).baseName():Diaporama->ProjectInfo->Title)).toUtf8();
                    out<<QString("artist="+AdjustMETA(Diaporama->ProjectInfo->Author)).toUtf8();;
                    out<<QString("album="+AdjustMETA(Diaporama->ProjectInfo->Album)).toUtf8();;
                    out<<QString("comment="+AdjustMETA(Diaporama->ProjectInfo->Comment)).toUtf8();;
                    out<<QString("date="+QString("%1").arg(Diaporama->ProjectInfo->Year)+"\n").toUtf8();;
                    out<<QString("composer="+AdjustMETA(Diaporama->ProjectInfo->Composer)).toUtf8();;
                    out<<QString("language="+Language+"\n").toUtf8();;
                    out<<QString("creation_time="+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\n").toUtf8();;   // ISO 8601 format

                    File.close();
                    #if (LIBAVFORMAT_VERSION_MAJOR<53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR<28))
                    TAG=" -i \""+TempMETAFileName+"\"  -map_metadata 0:"+(ui->IncludeSoundCB->isChecked()?"2":"1");
                    #else
                    TAG=" -i \""+TempMETAFileName+"\"  -map_metadata "+(ui->IncludeSoundCB->isChecked()?"2":"1");
                    #endif
                }
            }
            if (Continue) {
                qDebug()<<QApplication::translate("DlgRenderVideo","Start ffmpeg encoder");
                int GeoW=DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].PARNUM;
                int GeoH=DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].PARDEN;
                #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
                ffmpegCommand="\""+Diaporama->ApplicationConfig->PathFFMPEG+"\"";
                #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
                ffmpegCommand=Diaporama->ApplicationConfig->PathFFMPEG;
                #endif
                QString AddSizestr="";
                if (W==720) {
                    switch (Diaporama->ImageGeometry) {
                        case GEOMETRY_4_3:      W=(double(H)/3)*4;      GeoW=4;     GeoH=3; break;
                        case GEOMETRY_16_9:     W=(double(H)/9)*16;     GeoW=16;    GeoH=9; break;
                        case GEOMETRY_40_17:    W=(double(H)/17)*40;    GeoW=16;    GeoH=9; break;
                    }
                }
                ffmpegCommand=ffmpegCommand+QString(" -y -f image2pipe -vcodec ppm -r ")+QString(DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].FPS)+" -i -"+
                        (ui->IncludeSoundCB->isChecked()?" -i \""+TempWAVFileName+"\"":"")+
                        TAG+
                        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=28))
                        " -timestamp now"+
                        #endif
                        " -dframes "+QString("%1").arg(NbrFrame)+" "+vCodec+AddSizestr+" -r "+
                        QString(DefImageFormat[Standard][Diaporama->ImageGeometry][ImageSize].FPS)+
                        " "+aCodec+QString(" -ar %1 -ac %2 -aspect %3:%4")
                        .arg(AudioFrequency)
                        .arg(Channels)
                        .arg(GeoW)
                        .arg(GeoH);
                if (ExtendV>0) ffmpegCommand=ffmpegCommand+QString(" -padtop %1 -padbottom %2").arg(ExtendV/2).arg(ExtendV-ExtendV/2);
                if (ExtendH>0) ffmpegCommand=ffmpegCommand+QString(" -padleft %1 -padright %2").arg(ExtendH/2).arg(ExtendH-ExtendH/2);

                // Activate multithreading support if getCpuCount()>1 and codec is h264 or VP8
                if ((getCpuCount()>1)&&(
                        (VIDEOCODECDEF[VideoCodecIndex].Codec_id==CODEC_ID_H264)||
                        (VIDEOCODECDEF[VideoCodecIndex].Codec_id==CODEC_ID_VP8)
                        )) ffmpegCommand=ffmpegCommand+" -threads "+QString("%1").arg(getCpuCount());

                ffmpegCommand=ffmpegCommand+" \""+OutputFileName+"\"";
                qDebug()<<ffmpegCommand;

                ffmpegCommand=AdjustDirForOS(ffmpegCommand);
            }
        }

        // Start ffmpegCommand
        if (Continue) {
            Process.setProcessChannelMode(QProcess::/*SeparateChannels*/ForwardedChannels);      // Forward standard and error message to the ffdiaporama console
            //Process.setWorkingDirectory(Diaporama->ApplicationConfig->UserConfigPath);         // Set working directory to user folder (for log generation)
            Process.start(ffmpegCommand,QIODevice::Append|QIODevice::ReadWrite);                 // Start command
            if (!Process.waitForStarted()) {
                QMessageBox::critical(NULL,QApplication::translate("DlgRenderVideo","Error","Error message"),
                                      QApplication::translate("DlgRenderVideo","Error starting ffmpeg","Error message")+"\n"+ffmpegCommand,
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
                    }
                    RefreshDisplay =true;
                    if (Column>0) Diaporama->FreeUnusedMemory(Column-1,1);
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

                // Save image to the pipe
                if (!Frame->RenderedImage->save(&Process,"PPM",100)) {
                    QMessageBox::critical(NULL,QApplication::translate("DlgRenderVideo","Error","Error message"),QApplication::translate("DlgRenderVideo","Error sending image to ffmpeg","Error message"),QMessageBox::Close);
                    Continue=false;
                }

                // Wait until ffmpeg processed the frame
                while (Continue &&(Process.bytesToWrite()>0)) {
                    if (!Process.waitForBytesWritten()) {
                        QMessageBox::critical(NULL,QApplication::translate("DlgRenderVideo","Error","Error message"),QApplication::translate("DlgRenderVideo","ffmpeg error","Error message"),QMessageBox::Close);
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

            qDebug()<<QApplication::translate("DlgRenderVideo","Closing encoder");

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
                QMessageBox::critical(NULL,QApplication::translate("DlgRenderVideo","Error","Error message"),QApplication::translate("DlgRenderVideo","Error during ffmpeg process","Error message"),QMessageBox::Close);
                Process.terminate();
                Continue=false;
            } else if (Process.exitStatus()!=QProcess::NormalExit) {
              QMessageBox::critical(NULL,QApplication::translate("DlgRenderVideo","Error","Error message"),QApplication::translate("DlgRenderVideo","Error exiting ffmpeg","Error message"),QMessageBox::Close);
              Continue=false;
            }
        }

        if (TempWAVFileName!="")  QFile::remove(TempWAVFileName);
        if (TempMETAFileName!="") QFile::remove(TempMETAFileName);

        Process.terminate();
        Process.close();

        // Inform user of success
        if (Continue) QMessageBox::information(this,QApplication::translate("DlgRenderVideo","Render video"),QApplication::translate("DlgRenderVideo","Job completed succesfully!"));

        // Save Window size and position
        Diaporama->ApplicationConfig->DlgRenderVideoWSP->SaveWindowState(this);

        // Close the dialog box
        done(0);
    }
}

//============================================================================================

QString DlgRenderVideo::AdjustMETA(QString Text) {
    //Metadata keys or values containing special characters (’=’, ’;’, ’#’, ’\’ and a newline) must be escaped with a backslash ’\’.
    Text.replace("=","\\=");
    Text.replace(";","\\;");
    Text.replace("#","\\#");
    Text.replace("\\","\\\\");
    Text.replace("\n","\\\n");
    Text=Text+"\n";

    return Text;
}

//============================================================================================
// Make audio temp file
//============================================================================================

bool DlgRenderVideo::WriteTempAudioFile(QString TempWAVFileName,int FromSlide) {
    bool                    Continue            =true;      // true if no error occur
    cDiaporamaObjectInfo    *PreviousFrame      =NULL;
    cDiaporamaObjectInfo    *Frame              =NULL;
    AVOutputFormat          *Fmt                =NULL;      // No delete needed!
    AVFormatContext         *OutputFormatContext=NULL;
    AVStream                *AudioStream        =NULL;
    AVCodecContext          *AudioCodecContext  =NULL;
    AVCodec                 *AudioCodec         =NULL;
    uint8_t                 *audio_outbuf       =NULL;
    AVFormatParameters      fpOutFile;          memset(&fpOutFile,0,sizeof(AVFormatParameters));
    cSDLSoundBlockList      RenderMusic;
    cSDLSoundBlockList      EncodedAudio;

    //    AVStream        *VideoStream        =NULL;
    //    AVCodecContext  *VideoCodecContext  =NULL;
    //    AVCodec         *VideoCodec         =NULL;

    ui->SoundProgressBar->setMaximum(NbrFrame);

    // Get the container format
    Fmt=av_guess_format(NULL,TempWAVFileName.toUtf8(),NULL);
    if (Fmt==NULL) {
        QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error creating temporary wav file!");
        Continue=false;
    } else Fmt->audio_codec=CODEC_ID_PCM_S16LE;

    // allocate the output media context
    if (Continue) {

        OutputFormatContext = avformat_alloc_context();
        if (!OutputFormatContext) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : Unable to allocate OutputFormatContext!");
            Continue=false;
        } else {
            memcpy(OutputFormatContext->filename,TempWAVFileName.toUtf8(),strlen(TempWAVFileName.toUtf8())+1);
            OutputFormatContext->oformat  =Fmt;
            //OutputFormatContext->timestamp=0;
            OutputFormatContext->bit_rate =1536;
            #if FF_API_FORMAT_PARAMETERS
            #else
            if (av_set_parameters(OutputFormatContext,&fpOutFile)<0) {
                av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
                QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Invalid output format parameters!");
                Continue=false;
            }
            #endif
        }
    }

    // Allocate AudioStream
    if (Continue) {
        AudioStream=av_new_stream(OutputFormatContext,0);
        if (AudioStream==NULL) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio stream!");
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            Continue=false;
        }
    }

    // Open audio codec
    if (Continue) {
        AudioCodecContext=AudioStream->codec;
        avcodec_get_context_defaults2(AudioCodecContext,AVMEDIA_TYPE_AUDIO);  // Fill stream with default values
        AudioCodec=avcodec_find_encoder(CODEC_ID_PCM_S16LE);                // Open Audio encoder
        if (!AudioCodec) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Audio codec not found!");
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            Continue=false;
        } else {
            AudioCodecContext->codec_id             = CODEC_ID_PCM_S16LE;
            AudioCodecContext->codec_type           = AVMEDIA_TYPE_AUDIO;
            AudioCodecContext->sample_fmt           = AV_SAMPLE_FMT_S16;
            AudioCodecContext->sample_rate          = 48000;
            AudioCodecContext->bit_rate             = 48000;
            AudioCodecContext->rc_max_rate          = 0;
            AudioCodecContext->rc_min_rate          = 0;
            AudioCodecContext->bit_rate_tolerance   = 0;
            AudioCodecContext->rc_buffer_size       = 0;
            AudioCodecContext->channels             = 2;
            AudioCodecContext->channel_layout       = CH_LAYOUT_STEREO_DOWNMIX;    //CH_LAYOUT_STEREO;
            AudioCodecContext->time_base            = (AVRational){1,AudioCodecContext->sample_rate};
            AudioStream->r_frame_rate               = AudioCodecContext->time_base;
            AudioStream->time_base                  = AudioCodecContext->time_base;
            AudioCodecContext->flags               |= CODEC_FLAG_GLOBAL_HEADER;

            // open the codec
            #ifndef FF_API_AVCODEC_OPEN
            if (avcodec_open(AudioCodecContext,AudioCodec)<0) {
            #else
            if (avcodec_open2(AudioCodecContext,AudioCodec,NULL)<0) {
            #endif
                QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"could not open audio codec!");
                av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
                Continue=false;
            } else {
                // Init sound blocks
                int audio_input_frame_size=AudioStream->codec->frame_size;                          // frame size in samples
                if (audio_input_frame_size<=1) audio_input_frame_size=RenderMusic.SoundPacketSize; else audio_input_frame_size*=RenderMusic.SampleBytes*RenderMusic.Channels;
                RenderMusic.SetFPS(25);         // For sound generation, use only 25 FPS to avoir rounded issue
                EncodedAudio.SetFrameSize(audio_input_frame_size);
            }
        }
    }

    // open the file for writing
    if (Continue) {
        int Err=0;
        #if FF_API_OLD_AVIO
            if ((Err=avio_open(&OutputFormatContext->pb,TempWAVFileName.toUtf8(),AVIO_FLAG_WRITE))<0) {
        #else
            if (url_fopen(&OutputFormatContext->pb,TempWAVFileName.toUtf8(),URL_WRONLY)<0) {
        #endif
            char Buf[500];
            av_strerror(Err,Buf,500);
            qDebug()<<Buf;
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error creating temporary audio file!");
            Continue=false;
        }
    }

    // Allocate buffer to encode
    if (Continue) {
        audio_outbuf=(uint8_t *)av_malloc(FF_MIN_BUFFER_SIZE);
        if (audio_outbuf==NULL) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio buffer!");
            Continue=false;
        }
    }

    // write the header
    if ((Continue)&&
        #if FF_API_FORMAT_PARAMETERS
            (avformat_write_header(OutputFormatContext,NULL)<0)) {
        #else
            (av_write_header(OutputFormatContext)!=0)) {
        #endif
        av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
        QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error writing the header of the temporary audio file!");
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
            Frame=new cDiaporamaObjectInfo(PreviousFrame,Position,Diaporama,(FPS/1000));

            // Ensure MusicTracks are ready
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_MusicTrack==NULL)) {
                Frame->CurrentObject_MusicTrack=new cSDLSoundBlockList();
                Frame->CurrentObject_MusicTrack->SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
                Frame->TransitObject_MusicTrack=new cSDLSoundBlockList();
                Frame->TransitObject_MusicTrack->SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
            }

            // Ensure SoundTracks are ready
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
                Frame->CurrentObject_SoundTrackMontage=new cSDLSoundBlockList();
                Frame->CurrentObject_SoundTrackMontage->SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
                Frame->TransitObject_SoundTrackMontage=new cSDLSoundBlockList();
                Frame->TransitObject_SoundTrackMontage->SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
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
            if (MaxPacket>RenderMusic.NbrPacketForFPS) MaxPacket=RenderMusic.NbrPacketForFPS;

            // mix audio data
            for (int j=0;j<MaxPacket;j++)
                RenderMusic.MixAppendPacket(Frame->CurrentObject_MusicTrack->DetachFirstPacket(),(Frame->CurrentObject_SoundTrackMontage!=NULL)?Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket():NULL);

            // Flush audio frame
            while ((Continue)&&(RenderMusic.List.count()>0)) {
                AVPacket    pkt;

                int16_t     *Packet=RenderMusic.DetachFirstPacket();
                if (Packet==NULL) {
                    Packet=(int16_t *)av_malloc(RenderMusic.SoundPacketSize+4);
                    memset(Packet,0,RenderMusic.SoundPacketSize);
                }

                EncodedAudio.AppendData(Packet,RenderMusic.SoundPacketSize);
                while (EncodedAudio.List.count()>0) {
                    int16_t *PacketSound=EncodedAudio.DetachFirstPacket();
                    if (PacketSound==NULL) {
                        PacketSound=(int16_t *)av_malloc(EncodedAudio.SoundPacketSize+4);
                        memset(PacketSound,0,EncodedAudio.SoundPacketSize);
                    }
                    int out_size= avcodec_encode_audio(AudioCodecContext,audio_outbuf,EncodedAudio.SoundPacketSize,(short int *)PacketSound);
                    if (out_size>0) {
                        av_init_packet(&pkt);

                        if ((AudioCodecContext->coded_frame!=NULL)&&(AudioCodecContext->coded_frame->pts!=int64_t(INT64_C(0x8000000000000000))))
                            pkt.pts=av_rescale_q(AudioCodecContext->coded_frame->pts,AudioCodecContext->time_base,AudioStream->time_base);

                        if ((AudioCodecContext->coded_frame!=NULL)&&(AudioCodecContext->coded_frame->key_frame))
                            pkt.flags|=AV_PKT_FLAG_KEY;

                        pkt.stream_index=AudioStream->index;
                        pkt.data        =audio_outbuf;
                        pkt.size        =out_size;

                        // write the compressed frame in the media file
                        if (av_interleaved_write_frame(OutputFormatContext,&pkt)!=0) {
                            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
                            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error while writing audio frame!");
                            Continue=false;
                        }
                    } else if (out_size<0) {
                        QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error encoding sound!");
                        Continue=false;
                    }
                    av_free(PacketSound);
                }

                av_free(Packet);

            }
            QApplication::processEvents();  // Give time to interface!

            // Calculate next position
            Position     +=(FPS/1000);
            if (PreviousFrame!=NULL) delete PreviousFrame;
            PreviousFrame=Frame;
            Frame =NULL;

            // Stop the process if error occur or user ask to stop
            Continue=Continue && !StopProcessWanted;;
        }

        // Write de trailer
        if ((Continue)&&(av_write_trailer(OutputFormatContext)!=0)) {
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error writing the trailer of the temporary audio file!");
            Continue=false;
        }
    }

    // Ensure sound progress bar is at 100%
    if (Continue) ui->SoundProgressBar->setValue(ui->SoundProgressBar->maximum());

    // Clean all

    if (audio_outbuf)   av_free(audio_outbuf);
    if (PreviousFrame)  delete PreviousFrame;
    if (Frame)          delete Frame;
    if (OutputFormatContext) {
        #if FF_API_OLD_AVIO
        if (OutputFormatContext->pb) avio_close(OutputFormatContext->pb);                                   // close the file
        #else
        if (OutputFormatContext->pb) url_fclose(OutputFormatContext->pb);                                   // close the file
        #endif
        if (OutputFormatContext->streams[0]) {
            avcodec_close(AudioStream->codec);                                                              // close codec
            if (OutputFormatContext->streams[0]->codec) av_freep(&OutputFormatContext->streams[0]->codec);  // free the audiostream
        }
        av_free(OutputFormatContext);                                                                       // free the container
    }

    return Continue;
}
