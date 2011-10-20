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

    #if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
        setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));
    #endif

    switch (ExportMode) {
        case EXPORTMODE_ADVANCED     :  setWindowTitle(QApplication::translate("DlgRenderVideo","Advanced render video"));                          break;
        case EXPORTMODE_SMARTPHONE   :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for mobil player and smartphone"));   break;
        case EXPORTMODE_MULTIMEDIASYS:  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for multimedia system"));             break;
        case EXPORTMODE_FORTHEWEB    :  setWindowTitle(QApplication::translate("DlgRenderVideo","Render video for the WEB"));                       break;
    }

    // For the first step, only SelectDestWidget is display and enable
    ui->SelectDestScroll->setEnabled(true);     ui->SelectDestScroll->setVisible(true);
    ui->ProcessWidget->setEnabled(false);       ui->ProcessWidget->setVisible(false);

    // Output file
    if (Diaporama->OutputFileName=="")
        Diaporama->OutputFileName=Diaporama->ApplicationConfig->LastRenderVideoPath+
        (Diaporama->ApplicationConfig->LastRenderVideoPath.endsWith(QDir::separator())?"":QString(QDir::separator()))+
        (Diaporama->ProjectFileName!=""?QFileInfo(Diaporama->ProjectFileName).baseName():"movie");

    if (ExportMode==EXPORTMODE_ADVANCED) {
        ui->DeviceTypeLabel->setVisible(false);
        ui->DeviceTypeCB->setVisible(false);
        ui->DeviceModelCB->setVisible(false);
        ui->DeviceModelLabel->setVisible(false);
        ui->RenderFormatText->setVisible(false);

        // Init format container combo
        for (int i=0;i<NBR_FORMATDEF;i++) if (FORMATDEF[i].IsFind) {
            ui->FileFormatCB->addItem(FORMATDEF[i].LongName,QVariant(i));
            if (i==Diaporama->OutputFileFormat) {
                ui->FileFormatCB->setCurrentIndex(i);
                FileFormatCombo(i);
            }
        }

        connect(ui->FileFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(FileFormatCombo(int)));

        // Video standard & geometry
        ui->GeometryCombo->setEnabled(false);
        ui->GeometryCombo->setCurrentIndex(Diaporama->ImageGeometry);
        ui->StandardCombo->setCurrentIndex(Diaporama->LastStandard);
        connect(ui->GeometryCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitImageSizeCombo(int)));
        connect(ui->StandardCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitImageSizeCombo(int)));

        // Image size
        InitImageSizeCombo(0);
        ui->ImageSizeCombo->setCurrentIndex(Diaporama->LastImageSize);
        connect(ui->ImageSizeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));

        // codec(s) & bitrate(s)
        FileFormatCombo(0);
        InitVideoBitRateCB(-1);
        InitAudioBitRateCB(-1);
        connect(ui->VideoFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitVideoBitRateCB(int)));
        connect(ui->AudioFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(InitAudioBitRateCB(int)));

    } else {
        ui->FileFormatLabel->setVisible(false);         ui->FileFormatCB->setVisible(false);
        ui->GeometryLabel->setVisible(false);           ui->GeometryCombo->setVisible(false);
        ui->StandardLabel->setVisible(false);           ui->StandardCombo->setVisible(false);
        ui->ImageSizeLabel->setVisible(false);          ui->ImageSizeCombo->setVisible(false);
        ui->VideoFormatLabel->setVisible(false);        ui->VideoFormatCB->setVisible(false);
        ui->VideoBitRateLabel->setVisible(false);       ui->VideoBitRateCB->setVisible(false);
        ui->AudioFormatLabel->setVisible(false);        ui->AudioFormatCB->setVisible(false);
        ui->AudioBitRateLabel->setVisible(false);       ui->AudioBitRateCB->setVisible(false);

        QStringList List;
        int         Default=ExportMode==EXPORTMODE_SMARTPHONE?Diaporama->ApplicationConfig->DefaultSmartphoneType:
                            ExportMode==EXPORTMODE_MULTIMEDIASYS?Diaporama->ApplicationConfig->DefaultMultimediaType:
                            Diaporama->ApplicationConfig->DefaultForTheWEBType;

        for (int i=0;i<Diaporama->ApplicationConfig->TranslatedRenderSubtype[ExportMode].count();i++) List.append(Diaporama->ApplicationConfig->TranslatedRenderSubtype[ExportMode][i]+"#"+QString("%1").arg(i));
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
    connect(ui->RenderZoneFromED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRenderZoneFromED(int)));
    connect(ui->RenderZoneToED,SIGNAL(valueChanged(int)),this,SLOT(s_ChgRenderZoneToED(int)));

    ui->DestinationFilePath->setText(QDir::cleanPath(QDir(QFileInfo(Diaporama->ProjectFileName).dir().absolutePath()).absoluteFilePath(Diaporama->OutputFileName)));
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

void DlgRenderVideo::s_ChgRenderZoneFromED(int NewValue) {
    if (StopSpinboxRecursion) return;
    StopSpinboxRecursion=true;
    if (ui->RenderZoneToED->value()<NewValue) ui->RenderZoneToED->setValue(NewValue);
    StopSpinboxRecursion=false;
}

//====================================================================================================================

void DlgRenderVideo::s_ChgRenderZoneToED(int NewValue) {
    if (StopSpinboxRecursion) return;
    StopSpinboxRecursion=true;
    if (ui->RenderZoneFromED->value()>NewValue) ui->RenderZoneFromED->setValue(NewValue);
    StopSpinboxRecursion=false;
}

//====================================================================================================================

void DlgRenderVideo::s_DeviceTypeCB(int) {
    ui->DeviceModelCB->clear();
    int ItemData=ui->DeviceTypeCB->currentIndex();
    if (ItemData>=0) ItemData=ui->DeviceTypeCB->itemData(ItemData).toInt();
    QStringList List;
    for (int i=0;i<Diaporama->ApplicationConfig->RenderDeviceModel.count();i++)
        if ((Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceType==ExportMode)&&(Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceSubtype==ItemData))
            List.append(Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceName);
    List.sort();
    ui->DeviceModelCB->addItems(List);
    int i=0;
    int ToFind=(ExportMode==EXPORTMODE_SMARTPHONE)?Diaporama->ApplicationConfig->DefaultSmartphoneModel:
               (ExportMode==EXPORTMODE_MULTIMEDIASYS)?Diaporama->ApplicationConfig->DefaultMultimediaModel:
               Diaporama->ApplicationConfig->DefaultForTheWEBModel;
    while ((i<Diaporama->ApplicationConfig->RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceIndex!=ToFind)) i++;
    if (Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceIndex==ToFind)
        ui->DeviceModelCB->setCurrentIndex(ui->DeviceModelCB->findText(Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceName));
}

//====================================================================================================================

void DlgRenderVideo::InitImageSizeCombo(int) {
    int Geometry=ui->GeometryCombo->currentIndex();
    int Standard=ui->StandardCombo->currentIndex();
    int ImageSize=ui->ImageSizeCombo->currentIndex();
    ui->ImageSizeCombo->clear();
    for (int i=0;i<NBR_SIZEDEF;i++) ui->ImageSizeCombo->addItem(DefImageFormat[Standard][Geometry][i].Name);
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
        while ((i<Diaporama->ApplicationConfig->RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceName!=Device)) i++;
        if (i<Diaporama->ApplicationConfig->RenderDeviceModel.count()) {
            FileFormat=QString(FORMATDEF[Diaporama->ApplicationConfig->RenderDeviceModel[i].FileFormat].LongName)+" (*."+
                    QString(FORMATDEF[Diaporama->ApplicationConfig->RenderDeviceModel[i].FileFormat].FileExtension)+")";
        }
    }
    QString Path=QFileInfo(ui->DestinationFilePath->text()).absolutePath();
    QString OutputFileName  =QFileDialog::getSaveFileName(this,QApplication::translate("MainWindow","Select destination file"),Path,FileFormat);
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
            if (Codec==QString(Diaporama->VideoCodec)) {
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
            if (Codec==QString(Diaporama->AudioCodec)) {
                ui->AudioFormatCB->setCurrentIndex(ui->AudioFormatCB->count()-1);
                IsFindCodec=true;
            }
        }
    }
    if (!IsFindCodec) ui->AudioFormatCB->setCurrentIndex(0);
    ui->AudioFormatCB->setEnabled(ui->AudioFormatCB->count()>0);
}

//====================================================================================================================

void DlgRenderVideo::InitVideoBitRateCB(int ChangeIndex) {
    ui->VideoBitRateCB->clear();
    int CurrentCodec=ui->VideoFormatCB->currentIndex();
    int CurrentSize =ui->ImageSizeCombo->currentIndex();
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
            if ((ChangeIndex==-1)&&(List[i]==QString("%1k").arg(Diaporama->VideoBitRate))) {
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
            if ((ChangeIndex==-1)&&(List[i]==QString("%1k").arg(Diaporama->AudioBitRate))) {
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
        while ((i<Diaporama->ApplicationConfig->RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceName!=Device)) i++;
        if (i<Diaporama->ApplicationConfig->RenderDeviceModel.count()) {
            FileFormat=QString(FORMATDEF[Diaporama->ApplicationConfig->RenderDeviceModel[i].FileFormat].FileExtension);
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
    } else {
        // Save Window size and position
        Diaporama->ApplicationConfig->DlgRenderVideoWSP->SaveWindowState(this);
        done(1);
    }
}

//====================================================================================================================

void DlgRenderVideo::s_DeviceModelCB(int) {
    QString Device=ui->DeviceModelCB->currentText();
    int i=0;
    while ((i<Diaporama->ApplicationConfig->RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceName!=Device)) i++;
    if (i<Diaporama->ApplicationConfig->RenderDeviceModel.count()) {
        int Standard=Diaporama->ApplicationConfig->RenderDeviceModel[i].Standard;
        QString Text="Format=\t"+QString(FORMATDEF[Diaporama->ApplicationConfig->RenderDeviceModel[i].FileFormat].LongName)+"\nVideo=\t";
        Text=Text+VIDEOCODECDEF[Diaporama->ApplicationConfig->RenderDeviceModel[i].VideoCodec].LongName;
        int ImgSize=Diaporama->ApplicationConfig->RenderDeviceModel[i].ImageSize;
        int ExtendH   =0;
        int ExtendV   =0;
        int W=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Width;
        int H=DefImageFormat[Standard][Diaporama->ImageGeometry][ImgSize].Height;
        Text=Text+QString("-%1").arg(W)+"x"+QString("%1").arg(H);
        if (ExtendH>0) Text=Text+"+PADLEFT:"+QString("%1").arg(ExtendH/2)+"+PADRIGHT:"+QString("%1").arg(ExtendH-ExtendH/2);
        if (ExtendV>0) Text=Text+"+PADTOP:"+QString("%1").arg(ExtendV/2)+"+PADBOTTOM:"+QString("%1").arg(ExtendV-ExtendV/2);

        QString VideoBitRateStr=QString("%1").arg(Diaporama->ApplicationConfig->RenderDeviceModel[i].VideoBitrate); if (VideoBitRateStr.endsWith("000")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-3)+"k";
        QString AudioBitRateStr=QString("%1").arg(Diaporama->ApplicationConfig->RenderDeviceModel[i].AudioBitrate); if (AudioBitRateStr.endsWith("000")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-3)+"k";

        Text=Text+"-"+VideoBitRateStr+"b/s\nAudio=\t"+AUDIOCODECDEF[Diaporama->ApplicationConfig->RenderDeviceModel[i].AudioCodec].LongName+"-"+AudioBitRateStr+"b/s";
        ui->RenderFormatText->setText(Text);

    } else ui->RenderFormatText->setText("");
    AdjustDestinationFile();
}

//====================================================================================================================

void DlgRenderVideo::accept() {
    bool                    IsModify        =false;
    cDiaporamaObjectInfo    *PreviousFrame  =NULL;
    cDiaporamaObjectInfo    *Frame          =NULL;
    QString                 TempWAVFileName;
    QString                 vCodec,aCodec;
    QString                 ffmpegCommand;
    int                     W,H;
    QProcess                Process;
    bool                    RefreshDisplay;
    int                     DurationProcess;        // Display informations
    QString                 DisplayText;            // Display informations

    if (IsDestFileOpen) {
        StopProcessWanted=true;
    } else {
        if (Diaporama->OutputFileName !=ui->DestinationFilePath->text()) {
            Diaporama->OutputFileName=ui->DestinationFilePath->text();
            IsModify=true;
        }

        if ((QFileInfo(Diaporama->OutputFileName).exists())&&(QMessageBox::question(this,QApplication::translate("DlgRenderVideo","Overwrite file ?"),
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
        double      VideoFrameRate=0;
        int         OutputFileFormat=0;
        int         AudioFrequency=48000;
        QString     VideoCodec="";
        QString     AudioCodec="";
        QString     Preset="";
        int         VideoBitRate=Diaporama->VideoBitRate;
        int         AudioBitRate=Diaporama->AudioBitRate;
        int         ExtendH=0;
        int         ExtendV=0;
        int         Channels=2;
        int         FromSlide=(ui->RenderZoneFromBt->isChecked())?ui->RenderZoneFromED->value()-1:0;
        int         ToSlide  =(ui->RenderZoneFromBt->isChecked())?ui->RenderZoneToED->value()-1:Diaporama->List.count()-1;

        if (ExportMode==EXPORTMODE_ADVANCED) {
            if (Diaporama->LastStandard !=ui->StandardCombo->currentIndex())   { Diaporama->LastStandard =ui->StandardCombo->currentIndex();   IsModify=true; }
            if (Diaporama->LastImageSize!=ui->ImageSizeCombo->currentIndex())  { Diaporama->LastImageSize=ui->ImageSizeCombo->currentIndex();  IsModify=true; }

            OutputFileFormat=ui->FileFormatCB->currentIndex();
            VideoFrameRate  =DefImageFormat[ui->StandardCombo->currentIndex()][ui->GeometryCombo->currentIndex()][ui->ImageSizeCombo->currentIndex()].dFPS;
            // Video codec
            VideoCodecIndex=ui->VideoFormatCB->currentIndex();
            if (VideoCodecIndex<0) {
                QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Video codec error !");
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
                QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Audio codec error !");
                done(0);
                return;
            }
            AudioCodecIndex=ui->AudioFormatCB->itemData(AudioCodecIndex).toInt();
            BitRate=ui->AudioBitRateCB->currentText();
            if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1)+"000";
            AudioBitRate=BitRate.toInt();
            ExtendV =DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Extend*2;

        } else {

            QString Device=ui->DeviceModelCB->currentText();
            int i=0;
            while ((i<Diaporama->ApplicationConfig->RenderDeviceModel.count())&&(Diaporama->ApplicationConfig->RenderDeviceModel[i].DeviceName!=Device)) i++;
            if (i<Diaporama->ApplicationConfig->RenderDeviceModel.count()) {
                Diaporama->LastStandard=Diaporama->ApplicationConfig->RenderDeviceModel[i].Standard;
                OutputFileFormat=Diaporama->ApplicationConfig->RenderDeviceModel[i].FileFormat;

                VideoCodecIndex =Diaporama->ApplicationConfig->RenderDeviceModel[i].VideoCodec;
                AudioCodecIndex =Diaporama->ApplicationConfig->RenderDeviceModel[i].AudioCodec;

                int ImgSize=Diaporama->ApplicationConfig->RenderDeviceModel[i].ImageSize;
                //int W=DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][ImgSize].Width;
                //int H=DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][ImgSize].Height;
                VideoFrameRate=DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][ImgSize].dFPS;
                Diaporama->LastImageSize=ImgSize;
                VideoBitRate=Diaporama->ApplicationConfig->RenderDeviceModel[i].VideoBitrate;
                AudioBitRate=Diaporama->ApplicationConfig->RenderDeviceModel[i].AudioBitrate;
            }
        }

        if (OutputFileFormat>=0) {
            if (Diaporama->OutputFileFormat!=ui->FileFormatCB->itemData(Diaporama->OutputFileFormat).toInt()) {
                Diaporama->OutputFileFormat=ui->FileFormatCB->itemData(Diaporama->OutputFileFormat).toInt();
                IsModify=true;
            }
        }

        FileFormat  =FORMATDEF[Diaporama->OutputFileFormat].FileExtension;
        VideoCodec  =VIDEOCODECDEF[VideoCodecIndex].FFD_VCODECST;
        AudioCodec  =AUDIOCODECDEF[AudioCodecIndex].ShortName;

        // Special case adjustment
        if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==CODEC_ID_PCM_S16LE)    AudioBitRate=1536;      // Special case for WAV
        if (QString(FORMATDEF[OutputFileFormat].ShortName)==QString("flv")) AudioFrequency=44100;   // Special case for FLV
        if (QString(FORMATDEF[OutputFileFormat].ShortName)==QString("3gp")) AudioFrequency=8000;    // Special case for AMRNB

        if (Diaporama->VideoFrameRate !=VideoFrameRate)                  { Diaporama->VideoFrameRate   =VideoFrameRate;                     IsModify=true; }
        if (Diaporama->AudioFrequency !=AudioFrequency)                  { Diaporama->AudioFrequency   =AudioFrequency;                     IsModify=true; }
        if (Diaporama->VideoCodec     !=VideoCodec)                      { Diaporama->VideoCodec       =VideoCodec;                         IsModify=true; }
        if (Diaporama->AudioCodec     !=AudioCodec)                      { Diaporama->AudioCodec       =AudioCodec;                         IsModify=true; }
        if (Diaporama->VideoBitRate   !=VideoBitRate)                    { Diaporama->VideoBitRate     =VideoBitRate;                       IsModify=true; }
        if (Diaporama->AudioBitRate   !=AudioBitRate)                    { Diaporama->AudioBitRate     =AudioBitRate;                       IsModify=true; }

        if (IsModify) GlobalMainWindow->SetModifyFlag(true);

        //*****************
        QString VideoBitRateStr=QString("%1").arg(VideoBitRate); if (VideoBitRateStr.endsWith("000")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-3)+"k";
        QString AudioBitRateStr=QString("%1").arg(AudioBitRate); if (AudioBitRateStr.endsWith("000")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-3)+"k";

        ui->InformationLabel1->setText(Diaporama->OutputFileName);
        ui->InformationLabel2->setText(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Name);
        ui->InformationLabel3->setText(QString(VIDEOCODECDEF[VideoCodecIndex].LongName)+" - "+VideoBitRateStr+"b/s");
        ui->InformationLabel4->setText(QString(AUDIOCODECDEF[AudioCodecIndex].LongName)+QString(" - %1 Hz - ").arg(Diaporama->AudioFrequency)+AudioBitRateStr+"b/s");

        //**********************************************************************************************************************************

        FPS     =(1/double(25)/*Diaporama->VideoFrameRate*/)*double(AV_TIME_BASE);  // Time duration of a frame (pour éviter les problèmes d'arrondi, génère le son en PAL)
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
        // 1st step encoding : produce WAV file with sound
        //**********************************************************************************************************************************

        // Create tempwavefile in the same directory as destination file
        TempWAVFileName=AdjustDirForOS(QFileInfo(Diaporama->OutputFileName).absolutePath());
        if (!TempWAVFileName.endsWith(QDir::separator())) TempWAVFileName=TempWAVFileName+QDir::separator();
        TempWAVFileName=TempWAVFileName+"temp.wav";

        StartTime=QTime::currentTime();                                  // Display control : time the process start
        Continue=WriteTempAudioFile(TempWAVFileName,FromSlide);

        //**********************************************************************************************************************************
        // 2nd step encoding : produce final file using temporary WAV file with sound
        //**********************************************************************************************************************************
        StartTime=QTime::currentTime();                                     // Display control : time the process start
        FPS     =(1/Diaporama->VideoFrameRate)*double(AV_TIME_BASE);        // Time duration of a frame
        NbrFrame=int(double(Diaporama->GetPartialDuration(FromSlide,ToSlide)*1000)/double(FPS));    // Number of frame to generate

        ui->SlideProgressBar->setValue(0);
        ui->TotalProgressBar->setValue(0);
        ui->TotalProgressBar->setMaximum(NbrFrame);
        ui->SlideNumberLabel->setText("");
        ui->FrameNumberLabel->setText("");

        // Construct ffmpeg command line
        if (Continue) {
            W       =DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Width;
            H       =DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Height;

            // Video codec part
            switch (VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC) {
                case VCODEC_MPEG    :   vCodec=QString("-vcodec mpeg2video -minrate %1 -maxrate %2 -bufsize %3 -b %4 -bf 3")
                                           .arg(Diaporama->VideoBitRate-Diaporama->VideoBitRate/10)
                                           .arg(Diaporama->VideoBitRate+Diaporama->VideoBitRate/10)
                                           .arg(Diaporama->VideoBitRate*2)
                                           .arg(Diaporama->VideoBitRate);
                                        break;
                case VCODEC_MPEG4   :   if (AudioCodec=="libopencore_amrnb") {
                                                vCodec=QString("-f 3gp -vcodec mpeg4 -b %1").arg(Diaporama->VideoBitRate);
                                        } else {
                                            if (QString(VIDEOCODECDEF[VideoCodecIndex].ShortName)==QString("mpeg4"))
                                                vCodec=QString("-vcodec mpeg4 -vtag xvid -b %1").arg(Diaporama->VideoBitRate);
                                                else vCodec=QString("-vcodec libxvid -b %1").arg(Diaporama->VideoBitRate);
                                        }
                                        break;
                case VCODEC_H264HQ  :   Preset=AdjustDirForOS(QDir::currentPath()); if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();
                                        Preset="-fpre \""+Preset+"libx264-hq.ffpreset\"";
                                        vCodec=QString("-vcodec libx264 ")+Preset+QString(" -minrate %1 -maxrate %2 -bufsize %3 -b %4")
                                            .arg(Diaporama->VideoBitRate-Diaporama->VideoBitRate/10)
                                            .arg(Diaporama->VideoBitRate+Diaporama->VideoBitRate/10)
                                            .arg(Diaporama->VideoBitRate*2)
                                            .arg(Diaporama->VideoBitRate);
                                        break;
                case VCODEC_H264PQ  :   Preset=AdjustDirForOS(QDir::currentPath()); if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();
                                        Preset="-fpre \""+Preset+"libx264-pq.ffpreset\"";
                                        vCodec=QString("-vcodec libx264 ")+Preset+QString(" -minrate %1 -maxrate %2 -bufsize %3 -b %4")
                                            .arg(Diaporama->VideoBitRate-Diaporama->VideoBitRate/10)
                                            .arg(Diaporama->VideoBitRate+Diaporama->VideoBitRate/10)
                                            .arg(Diaporama->VideoBitRate*2)
                                            .arg(Diaporama->VideoBitRate);
                                        break;
                case VCODEC_MJPEG   :   vCodec="-vcodec mjpeg -qscale 2 -qmin 2 -qmax 2";   break;
                case VCODEC_VP8     :   vCodec=QString("-vcodec libvpx -minrate %1 -maxrate %2 -bufsize %3 -b %4 -bf 3")
                                            .arg(Diaporama->VideoBitRate-Diaporama->VideoBitRate/10)
                                            .arg(Diaporama->VideoBitRate+Diaporama->VideoBitRate/10)
                                            .arg(Diaporama->VideoBitRate*2)
                                            .arg(Diaporama->VideoBitRate);
                                        break;
                case VCODEC_H263    :   vCodec=QString("-vcodec flv -b %1").arg(Diaporama->VideoBitRate);
                                        break;
                case VCODEC_THEORA  :   vCodec=QString("-vcodec libtheora -b %1").arg(Diaporama->VideoBitRate);
                                        break;
                default:
                    QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Unknown video codec");
                    Continue=false;
                    break;
            }

            // Audio codec part
            switch (AUDIOCODECDEF[AudioCodecIndex].Codec_id) {
                case CODEC_ID_PCM_S16LE:    aCodec=QString("-acodec pcm_s16le -ab %1").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_MP2:          aCodec=QString("-acodec mp2 -ab %1").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_MP3:          aCodec=QString("-acodec libmp3lame -ab %1").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_AAC:          if (QString(AUDIOCODECDEF[AudioCodecIndex].ShortName)==QString("aac"))
                                                aCodec=QString("-acodec aac -strict experimental -ab %1").arg(Diaporama->AudioBitRate);
                                                else aCodec=QString("-acodec libfaac -ab %1").arg(Diaporama->AudioBitRate);
                                            break;
                case CODEC_ID_AC3:          aCodec=QString("-acodec ac3 -ab %1").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_VORBIS:       aCodec=QString("-acodec libvorbis -ab %1").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_AMR_NB:       aCodec=QString("-acodec libopencore_amrnb -ab %1").arg(Diaporama->AudioBitRate); Channels=1; break;

                default:
                    QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Unknown audio codec");
                    Continue=false;
                    break;
            }

            if (Continue) {
                #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
                ffmpegCommand="\""+Diaporama->ApplicationConfig->PathFFMPEG+"\"";
                #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
                ffmpegCommand=Diaporama->ApplicationConfig->PathFFMPEG;
                #endif
                QString AddSizestr="";
                if (W==720) {
                    AddSizestr=QString(" -s %1x%2").
                            arg(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Width).
                            arg(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Height);

                    switch (Diaporama->ImageGeometry) {
                        case GEOMETRY_4_3:      W=(double(H)/3)*4;      break;
                        case GEOMETRY_16_9:     W=(double(H)/9)*16;     break;
                        case GEOMETRY_40_17:    W=(double(H)/17)*40;    break;
                    }

                }
                ffmpegCommand=ffmpegCommand+QString(" -y -f image2pipe -vcodec ppm -r "+QString(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].FPS)+" -i -"+
                      " -i \"")+TempWAVFileName+"\" -dframes "+QString("%1").arg(NbrFrame)+" "+vCodec+AddSizestr+" -r "+
                      QString(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].FPS)+                        
                      " "+aCodec+QString(" -ar %1 -ac %2 -aspect %3:%4")
                      .arg(Diaporama->AudioFrequency)
                      .arg(Channels)
                      .arg(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].PARNUM)
                      .arg(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].PARDEN);
                if (ExtendV>0) ffmpegCommand=ffmpegCommand+QString(" -padtop %1 -padbottom %2").arg(ExtendV/2).arg(ExtendV-ExtendV/2);
                if (ExtendH>0) ffmpegCommand=ffmpegCommand+QString(" -padleft %1 -padright %2").arg(ExtendH/2).arg(ExtendH-ExtendH/2);

                // Activate multithreading support if getCpuCount()>1 and codec is h264 or VP8
                if ((getCpuCount()>1)&&(
                        (VIDEOCODECDEF[VideoCodecIndex].Codec_id==CODEC_ID_H264)||
                        (VIDEOCODECDEF[VideoCodecIndex].Codec_id==CODEC_ID_VP8)
                        )) ffmpegCommand=ffmpegCommand+" -threads "+QString("%1").arg(getCpuCount());

                ffmpegCommand=ffmpegCommand+" \""+Diaporama->OutputFileName+"\"";
                qDebug()<<ffmpegCommand;

                ffmpegCommand=AdjustDirForOS(ffmpegCommand);
            }
        }

        // Start ffmpegCommand
        if (Continue) {
            Process.setProcessChannelMode(QProcess::ForwardedChannels);                     // Forward standard and error message to the ffdiaporama console
            //Process.setWorkingDirectory(Diaporama->ApplicationConfig->UserConfigPath);      // Set working directory to user folder (for log generation)
            Process.start(ffmpegCommand,QIODevice::Append|QIODevice::ReadWrite);            // Start command
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

            for (int RenderedFrame=0;Continue && (RenderedFrame<NbrFrame);RenderedFrame++) {
                if ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+Diaporama->List[Column].GetDuration()-Diaporama->GetTransitionDuration(Column+1))<=Position))) {
                    while ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+Diaporama->List[Column].GetDuration()-Diaporama->GetTransitionDuration(Column+1))<=Position))) {
                        Column++;
                        ColumnStart=Diaporama->GetObjectStartPosition(Column);
                        if (Column<Diaporama->List.count()) ui->SlideProgressBar->setMaximum(int(double(Diaporama->List[Column].GetDuration()-Diaporama->GetTransitionDuration(Column+1))/(FPS/double(1000)))-1);
                    }
                    RefreshDisplay =true;
                    if (Column>0) Diaporama->FreeUnusedMemory(Column-1);
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
                Diaporama->LoadSources(Frame,double(H)/double(1080),W,H,false,true);                                     // Load source images
                Diaporama->DoAssembly(Frame,W,H);                                            // Make final assembly

                // Give time to interface !
                QApplication::processEvents();

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
                    // Give time to interface !
                    QApplication::processEvents();
                    // Stop the process if error occur or user ask to stop
                    Continue=Continue && !StopProcessWanted;;
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

        QFile::remove(TempWAVFileName);

        // Free unused CacheFullImage
        Diaporama->FreeUnusedMemory(-1);

        // Inform user of success
        if (Continue) QMessageBox::information(this,QApplication::translate("DlgRenderVideo","Render video"),QApplication::translate("DlgRenderVideo","Job completed succesfully !"));

        // Save Window size and position
        Diaporama->ApplicationConfig->DlgRenderVideoWSP->SaveWindowState(this);

        // Close the dialog box
        done(0);
    }
}

//============================================================================================
// Make audio temp file
//============================================================================================

bool DlgRenderVideo::WriteTempAudioFile(QString TempWAVFileName,int FromSlide) {
    bool                    Continue            =true;      // true if no error occur
    cDiaporamaObjectInfo    *PreviousFrame      =NULL;
    cDiaporamaObjectInfo    *Frame              =NULL;
    AVOutputFormat          *Fmt                =NULL;      // No delete needed !
    AVFormatContext         *OutputFormatContext=NULL;
    AVStream                *AudioStream        =NULL;
    AVCodecContext          *AudioCodecContext  =NULL;
    AVCodec                 *AudioCodec         =NULL;
    uint8_t                 *audio_outbuf       =NULL;
    AVFormatParameters      fpOutFile;          memset(&fpOutFile,0,sizeof(AVFormatParameters));
    cSoundBlockList         RenderMusic;
    cSoundBlockList         EncodedAudio;

    //    AVStream        *VideoStream        =NULL;
    //    AVCodecContext  *VideoCodecContext  =NULL;
    //    AVCodec         *VideoCodec         =NULL;

    ui->SoundProgressBar->setMaximum(NbrFrame);

    // Get the container format
    Fmt=av_guess_format(NULL,TempWAVFileName.toUtf8(),NULL);
    if (Fmt==NULL) {
        QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error creating temporary wav file !");
        Continue=false;
    } else Fmt->audio_codec=CODEC_ID_PCM_S16LE;

    // allocate the output media context
    if (Continue) {

        OutputFormatContext = avformat_alloc_context();
        if (!OutputFormatContext) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : Unable to allocate OutputFormatContext !");
            Continue=false;
        } else {
            memcpy(OutputFormatContext->filename,TempWAVFileName.toUtf8(),strlen(TempWAVFileName.toUtf8())+1);
            OutputFormatContext->oformat  =Fmt;
            OutputFormatContext->timestamp=0;
            OutputFormatContext->bit_rate =1536;
            //OutputFormatContext->preload  = (int)(0.5 * AV_TIME_BASE);
            //OutputFormatContext->max_delay= (int)(0.7 * AV_TIME_BASE);
            // set the output parameters
            if (av_set_parameters(OutputFormatContext,&fpOutFile)<0) {
                av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
                QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Invalid output format parameters !");
                Continue=false;
            }
        }
    }

    // Allocate AudioStream
    if (Continue) {
        AudioStream=av_new_stream(OutputFormatContext,0);
        if (AudioStream==NULL) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio stream !");
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
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Audio codec not found !");
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            Continue=false;
        } else {
            AudioCodecContext->codec_id             = CODEC_ID_PCM_S16LE;
            AudioCodecContext->codec_type           = AVMEDIA_TYPE_AUDIO;
            AudioCodecContext->sample_fmt           = SAMPLE_FMT_S16;
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
            if (avcodec_open(AudioCodecContext,AudioCodec)<0) {
                QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"could not open audio codec !");
                av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
                Continue=false;
            } else {
                // Init sound blocks
                int audio_input_frame_size=AudioStream->codec->frame_size;                          // frame size in samples
                if (audio_input_frame_size<=1) audio_input_frame_size=RenderMusic.SoundPacketSize; else audio_input_frame_size*=RenderMusic.SampleBytes*RenderMusic.Channels;
                RenderMusic.SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
                EncodedAudio.SetFrameSize(audio_input_frame_size);
            }
        }
    }

    // open the file for writing
    if (Continue) {
        #if FF_API_OLD_AVIO
            #if AVIO_WRONLY
            if (avio_open(&OutputFormatContext->pb,TempWAVFileName.toUtf8(),AVIO_WRONLY)<0) {
            #else
                if (avio_open(&OutputFormatContext->pb,TempWAVFileName.toUtf8(),URL_WRONLY)<0) {
            #endif
        #else
            if (url_fopen(&OutputFormatContext->pb,TempWAVFileName.toUtf8(),URL_WRONLY)<0) {
        #endif
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error creating temporary audio file !");
            Continue=false;
        }
    }

    // Allocate buffer to encode
    if (Continue) {
        audio_outbuf=(uint8_t *)av_malloc(FF_MIN_BUFFER_SIZE);
        if (audio_outbuf==NULL) {
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio buffer !");
            Continue=false;
        }
    }

    // write the header
    if ((Continue)&&(av_write_header(OutputFormatContext)!=0)) {
        av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
        QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error writing the header of the temporary audio file !");
        Continue=false;
    }

    // Encode the file
    if (Continue) {
        LastCheckTime   =StartTime;                                     // Display control : last time the loop start
        int Position    =Diaporama->GetObjectStartPosition(FromSlide);  // Render current position
        int ColumnStart =-1;                                            // Render start position of current object
        int Column      =-1;                                            // Render current object
        for (int RenderedFrame=0;Continue && (RenderedFrame<NbrFrame);RenderedFrame++) {
            // Calculate position & column
            if ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+Diaporama->List[Column].GetDuration()-Diaporama->GetTransitionDuration(Column+1))<=Position))) {
                Column++;
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
                Frame->CurrentObject_MusicTrack=new cSoundBlockList();
                Frame->CurrentObject_MusicTrack->SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
                Frame->TransitObject_MusicTrack=new cSoundBlockList();
                Frame->TransitObject_MusicTrack->SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
            }

            // Ensure SoundTracks are ready
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
                Frame->CurrentObject_SoundTrackMontage=new cSoundBlockList();
                Frame->CurrentObject_SoundTrackMontage->SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
                Frame->TransitObject_SoundTrackMontage=new cSoundBlockList();
                Frame->TransitObject_SoundTrackMontage->SetFPS(25/*Diaporama->VideoFrameRate*/);        // Pour la generation du sond, force en PAL pour eviter les problèmes d'arrondi
            }

            // Prepare frame with W and H =0 to force SoundMusicOnly ! (thread mode is not necessary here)
            Diaporama->LoadSources(Frame,0,0,0,false,true);

            // Give time to interface !
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
                            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error while writing audio frame !");
                            Continue=false;
                        }
                    } else if (out_size<0) {
                        QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error encoding sound !");
                        Continue=false;
                    }
                    av_free(PacketSound);
                }

                av_free(Packet);

            }
            QApplication::processEvents();  // Give time to interface !

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
            QMessageBox::critical(this,QApplication::translate("DlgRenderVideo","Render video"),"Error writing the trailer of the temporary audio file !");
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
