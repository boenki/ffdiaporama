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

#include "wgt_JobConvertImage.h"
#include "ui_wgt_JobConvertImage.h"
#include "../sharedfiles/cBaseMediaFile.h"

//====================================================================================================================

wgt_JobConvertImage::wgt_JobConvertImage(QCustomDialog *Dialog,QWidget *parent):wgt_JobBase(Dialog,parent),ui(new Ui::wgt_JobConvertImage) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::wgt_JobConvertImage");

    ui->setupUi(this);
}

//====================================================================================================================

wgt_JobConvertImage::~wgt_JobConvertImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::~wgt_JobConvertImage");

    delete ui;
}

//====================================================================================================================

void wgt_JobConvertImage::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::DoInitDialog");

    connect(ui->DestFormatCB,   SIGNAL(currentIndexChanged(int)),this,SLOT(s_DestFormatCB(int)));
    connect(ui->RescalCB,       SIGNAL(released()),this,SLOT(s_RescalCB()));
    connect(ui->DontUpscaleCB,  SIGNAL(released()),this,SLOT(s_DontUpscaleCB()));
    connect(ui->RescalCombo,    SIGNAL(currentIndexChanged(int)),this,SLOT(s_RescalCombo(int)));
    connect(ui->QualitySL,      SIGNAL(valueChanged(int)),this,SLOT(s_QualitySL(int)));
}

//====================================================================================================================

void wgt_JobConvertImage::RefreshControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::RefreshControls");

    int Quality=90;
    ui->DestFormatCB->setCurrentIndex(ui->DestFormatCB->findText(Job->DestinationExtension));
    ui->RescalCB->setChecked(Job->IsCommandListContain("-SRS"));
    ui->DontUpscaleCB->setChecked(Job->IsCommandListContain("-DUP"));
    ui->RescalCombo->setEnabled(ui->RescalCB->isChecked());
    ui->DontUpscaleCB->setEnabled(ui->RescalCB->isChecked());
    if (Job->IsCommandListContain("-Q")) Quality=Job->CommandListValue("-Q");
    ui->QualitySL->setValue(Quality);
    ui->QualityLabel->setText(QString("%1%").arg(Quality));
    if (Job->IsCommandListContain("-SRS")) ui->RescalCombo->setCurrentIndex(ui->RescalCombo->findText(Job->CommandListValueString("-SRS")));
    ui->QualitySL->setEnabled    ((Job->DestinationExtension!="tiff")&&(Job->DestinationExtension!="bmp")&&(Job->DestinationExtension!="ppm"));
    ui->QualityLabel->setEnabled ((Job->DestinationExtension!="tiff")&&(Job->DestinationExtension!="bmp")&&(Job->DestinationExtension!="ppm"));
    ui->QualityLabel0->setEnabled((Job->DestinationExtension!="tiff")&&(Job->DestinationExtension!="bmp")&&(Job->DestinationExtension!="ppm"));
}

//====================================================================================================================

QString wgt_JobConvertImage::ComputeDestSuffix(cBaseMediaFile *MediaFile) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::ComputeDestSuffix");

    QString SizeSuffix="ns";
    if (Job->IsCommandListContain("-SRS")) {
        QString Value=Job->CommandListValueString("-SRS");
        if (Value.indexOf("MPix")!=-1)  SizeSuffix=Value.left(Value.indexOf(" "))+Value.mid(Value.indexOf(" ")+1);  // Remove space
            else                        SizeSuffix=Value.left(Value.indexOf(" "));  // Get beginning
    } else {
        SizeSuffix=MediaFile->GetImageSizeStr(cBaseMediaFile::FMTONLY);
    }
    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if ((CommandList[i].startsWith("-SSX"))||(CommandList[i].startsWith("-SRT"))) CommandList.removeAt(i); else i++;
    CommandList.append(QString("-SSX:%1").arg(SizeSuffix));
    CommandList.append(QString("-SRT:%1").arg(MediaFile->ImageOrientation));
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    return SizeSuffix;
}

//====================================================================================================================

void wgt_JobConvertImage::AppendJobSummary(int index,QString *JobSummary,cJobQueue *JobQueue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::AppendJobSummary");

    QString         SourceFile=Job->SourcesAndDests[index];
    cBaseMediaFile  *MediaFile=NULL;

    // Search MediaFile in list
    for (int i=0;i<JobQueue->MediaList.count();i++) if (((cBaseMediaFile *)JobQueue->MediaList.at(i))->FileName==SourceFile) {
        MediaFile=JobQueue->MediaList.at(i);
        break;
    }

    if (MediaFile!=NULL) {
        bool ForceRotate=(MediaFile->ImageOrientation!=1)&&((Job->DestinationExtension=="tiff")||(Job->DestinationExtension=="bmp")||(Job->DestinationExtension=="ppm")||(Job->DestinationExtension=="png"));
        int  ImageWidth =MediaFile->ImageWidth;
        int  ImageHeight=MediaFile->ImageHeight;
        if (Job->IsCommandListContain("-SRS")) {
            QString Value       =Job->CommandListValueString("-SRS");
            if (Value.indexOf("MPix")!=-1) {
                // if image is rotated then swap ImageWidth and ImageHeight
                if ((MediaFile->ImageOrientation==3)||(MediaFile->ImageOrientation==6)||(MediaFile->ImageOrientation==8)) {
                    int IW=ImageWidth;
                    ImageWidth=ImageHeight;
                    ImageHeight=IW;
                }
                double  iValue      =Value.left(Value.indexOf("MPix")).trimmed().toDouble()*1000*1000;
                double  ActualValue =ImageWidth*ImageHeight;
                // + or - 10%
                if (((iValue-iValue*0.1>ActualValue)&&(!Job->IsCommandListContain("-DUP")))||(iValue+iValue*0.1<ActualValue)) {
                    double Transfo=sqrt(iValue/ActualValue);
                    ImageWidth =int(Transfo*double(ImageWidth));
                    ImageHeight=int(Transfo*double(ImageHeight));
                    *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and rescal image to %1")).arg(Value);
                    if ((MediaFile->ImageOrientation==3)||(MediaFile->ImageOrientation==6)||(MediaFile->ImageOrientation==8)) {
                        if (ForceRotate)    *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and rotate image"));
                            else            *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","but don't rotate image"));
                    }
                }
            } else {
                int MaxRows=0;
                if      (Value.indexOf("QVGA")!=-1)     MaxRows=240;
                else if (Value.indexOf("HVGA")!=-1)     MaxRows=320;
                else if (Value.indexOf("WVGA")!=-1)     MaxRows=480;
                else if (Value.indexOf("DVD")!=-1)      MaxRows=576;
                else if (Value.indexOf("720p")!=-1)     MaxRows=720;
                else if (Value.indexOf("XGA")!=-1)      MaxRows=768;
                else if (Value.indexOf("1080p")!=-1)    MaxRows=1080;
                if (MaxRows!=0) {
                    // + or - 10%
                    if (((MaxRows-MaxRows*0.1>ImageHeight)&&(!Job->IsCommandListContain("-DUP")))||(MaxRows+MaxRows*0.1<ImageHeight)) {
                        double AspectR=double(ImageWidth)/double(ImageHeight);
                        ImageHeight=MaxRows;
                        ImageWidth =int(double(MaxRows)*AspectR);
                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and rescal image to %1")).arg(Value);
                        if ((MediaFile->ImageOrientation==3)||(MediaFile->ImageOrientation==6)||(MediaFile->ImageOrientation==8))
                            *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and rotate image"));
                    }
                }
            }
        } else if (ForceRotate) *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and rotate image"));
        if ((ImageWidth!=MediaFile->ImageWidth)||(ImageHeight!=MediaFile->ImageHeight)) {
            *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","new image size will become %1x%2")).arg(ImageWidth).arg(ImageHeight);
        }
    }
}

//====================================================================================================================

void wgt_JobConvertImage::s_DestFormatCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::s_DestFormatCB");

    Job->DestinationExtension=ui->DestFormatCB->currentText();
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_JobConvertImage::s_RescalCB() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::s_RescalCB");

    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-SRS")) CommandList.removeAt(i); else i++;
    if (ui->RescalCB->isChecked()) CommandList.append(QString("-SRS:%1").arg(ui->RescalCombo->currentText()));
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_JobConvertImage::s_RescalCombo(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::s_RescalCombo");

    s_RescalCB();
}

//====================================================================================================================

void wgt_JobConvertImage::s_DontUpscaleCB() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::s_DontUpscaleCB");

    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-DUP")) CommandList.removeAt(i); else i++;
    if (ui->DontUpscaleCB->isChecked()) CommandList.append(QString("-DUP"));
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_JobConvertImage::s_QualitySL(int Quality) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertImage::s_DontUpscaleCB");

    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-Q")) CommandList.removeAt(i); else i++;
    CommandList.append(QString("-Q:%1").arg(Quality));
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}
