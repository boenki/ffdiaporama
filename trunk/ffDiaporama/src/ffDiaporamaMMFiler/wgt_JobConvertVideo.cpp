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

#include "wgt_JobConvertVideo.h"
#include "ui_wgt_JobConvertVideo.h"
#include "../sharedfiles/cBaseMediaFile.h"
#include "cApplicationConfig.h"

//====================================================================================================================

wgt_JobConvertVideo::wgt_JobConvertVideo(QCustomDialog *Dialog,QWidget *parent):wgt_JobBase(Dialog,parent),ui(new Ui::wgt_JobConvertVideo) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertVideo::DoInitDialog");

    ui->setupUi(this);
}

//====================================================================================================================

wgt_JobConvertVideo::~wgt_JobConvertVideo() {
    delete ui;
}

//====================================================================================================================

void wgt_JobConvertVideo::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertVideo::DoInitDialog");

    QStringList List;
    for (int i=1;i<((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.TranslatedRenderType.count();i++)
        List.append(((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.TranslatedRenderType[i]+"#"+QString("%1").arg(i));
    List.sort();
    for (int i=0;i<List.count();i++) {
        QString Item=List[i];
        int     ItemData=Item.mid(Item.lastIndexOf("#")+1).toInt();
        Item=Item.left(Item.lastIndexOf("#"));
        ui->DeviceCB->addItem(Item,QVariant(ItemData));
    }
    ui->DeviceCB->setCurrentIndex(0);
    s_DeviceCB(0);
    s_DeviceTypeCB(0);
    s_DeviceModelCB(0);

    connect(ui->DeviceCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_DeviceCB(int)));
    connect(ui->DeviceTypeCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_DeviceTypeCB(int)));
    connect(ui->DeviceModelCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_DeviceModelCB(int)));
}

//====================================================================================================================

void wgt_JobConvertVideo::RefreshControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertVideo::RefreshControls");

}

//====================================================================================================================

void wgt_JobConvertVideo::AppendJobSummary(int index,QString *JobSummary,cJobQueue *JobQueue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertVideo::AppendJobSummary");

    QString         SourceFile=Job->SourcesAndDests[index];
    cBaseMediaFile  *MediaFile=NULL;

    // Search MediaFile in list
    for (int i=0;i<JobQueue->MediaList.count();i++) if (((cBaseMediaFile *)JobQueue->MediaList.at(i))->FileName==SourceFile) {
        MediaFile=JobQueue->MediaList.at(i);
        break;
    }

    if (MediaFile!=NULL) {
        if (Job->IsCommandListContain("-VI"))  {
            int     MaxWidth=0,MaxHeight=0;
            int     NewWidth=0,NewHeight=0;
            int     Index   =Job->CommandListValueString("-VI").split("/")[0].toInt();
            int     Standard=Job->CommandListValueString("-VI").split("/")[1].toInt();
            double  Ratio   =double(MediaFile->ImageWidth)/double(MediaFile->ImageHeight);
            for (int j=0;j<3;j++) {
                double RatioPAR=double(DefImageFormat[Standard][j][Index].PARDEN)/double(DefImageFormat[Standard][j][Index].PARNUM);
                if (double(DefImageFormat[Standard][j][Index].Width)/RatioPAR==double(DefImageFormat[Standard][j][Index].Height)) RatioPAR=1;
                if (MaxWidth <DefImageFormat[Standard][j][Index].Width)             MaxWidth =DefImageFormat[Standard][j][Index].Width;
                if (MaxHeight<DefImageFormat[Standard][j][Index].Height*RatioPAR)   MaxHeight=DefImageFormat[Standard][j][Index].Height*RatioPAR;
            }
            if (int(Ratio*double(MaxHeight))<=MaxWidth) {
                NewWidth =int(Ratio*double(MaxHeight));
                NewHeight=MaxHeight;
            } else {
                NewWidth =MaxWidth;
                NewHeight=int(double(MaxWidth)/Ratio);
            }
            *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and resize to %1x%2")).arg(NewWidth).arg(NewHeight);
        }
        if (Job->IsCommandListContain("-VCO"))  {
            int     VideoCodecIndex =Job->CommandListValue("-VCO");
            int     VideoBitRate    =Job->CommandListValue("-VB");
            QString AudioCodec      =(Job->IsCommandListContain("-ACO")?AUDIOCODECDEF[Job->CommandListValue("-ACO")].ShortName:"");
            switch (VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC) {
                case VCODEC_MPEG    :   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("mpeg2video");
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1 with + and - 10%")).arg(VideoBitRate);
                                        break;
                case VCODEC_MPEG4   :   if (AudioCodec=="libopencore_amrnb") {
                                            *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("mpeg4");
                                            *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1")).arg(VideoBitRate);
                                        } else {
                                            if (QString(VIDEOCODECDEF[VideoCodecIndex].ShortName)==QString("mpeg4")) {
                                                *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("mpeg4");
                                                *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1")).arg(VideoBitRate);
                                            } else {
                                                *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("libxvid");
                                                *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1")).arg(VideoBitRate);
                                            }
                                        }
                                        break;
                case VCODEC_H264HQ  :   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("libx264");
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1 with + and - 10%")).arg(VideoBitRate);
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video preset %1")).arg(
                                        #ifdef OLDFFMPEGPRESET
                                        "libx264-hq.ffpreset"
                                        #else
                                        "veryfast (ref=3)"
                                        #endif
                                        );
                                        break;
                case VCODEC_H264PQ  :   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("libx264");
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1 with + and - 10%")).arg(VideoBitRate);
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video preset %1")).arg(
                                        #ifdef OLDFFMPEGPRESET
                                        "libx264-pq.ffpreset"
                                        #else
                                        "veryfast (level=1.3:no-cabac:vbv-bufsize=768:vbv-maxrate=768)"
                                        #endif
                                        );
                                        break;
                case VCODEC_MJPEG   :   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("mjpeg");
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video quality to 2"));
                                        break;
                case VCODEC_VP8     :   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("libvpx");
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1 with + and - 10%")).arg(VideoBitRate);
                                        break;
                case VCODEC_H263    :   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("flv");
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1")).arg(VideoBitRate);
                                        break;
                case VCODEC_THEORA  :   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video codec to %1")).arg("libtheora");
                                        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set video compression to %1")).arg(VideoBitRate);
                                        break;
            }
        }
        if (Job->IsCommandListContain("-ACO"))  {
            *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set codec to %1")).arg(AUDIOCODECDEF[Job->CommandListValue("-ACO")].ShortName);
            if (Job->IsCommandListContain("-AB"))   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set audio compression to %1")).arg(Job->CommandListValueString("-AB"));
            if (Job->IsCommandListContain("-AR"))   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and force audio frequency to %1")).arg(Job->CommandListValueString("-AR").left(Job->CommandListValueString("-AR").indexOf(" ")));
            if (Job->IsCommandListContain("-ACH"))  *JobSummary=*JobSummary+"\n    "+(Job->CommandListValue("-ACH")==1?QString(QApplication::translate("QCustomJobTable","and force to mono")):QString(QApplication::translate("QCustomJobTable","and force to stereo")));
        }
        *JobSummary=*JobSummary+"\n    "+JobQueue->ComputeFFMPEGCommand(Job);
    }
}

//====================================================================================================================

void wgt_JobConvertVideo::s_DeviceCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertVideo::s_DeviceCB");

    ui->DeviceTypeCB->clear();
    int  ExportMode=ui->DeviceCB->currentIndex();
    if (ExportMode>=0) {
        ExportMode=ui->DeviceCB->itemData(ExportMode).toInt();
        QStringList List;
        for (int i=0;i<((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.TranslatedRenderSubtype[ExportMode].count();i++)
            List.append(((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.TranslatedRenderSubtype[ExportMode][i]+"#"+QString("%1").arg(i));
        List.sort();
        for (int i=0;i<List.count();i++) {
            QString Item=List[i];
            int     ItemData=Item.mid(Item.lastIndexOf("#")+1).toInt();
            Item=Item.left(Item.lastIndexOf("#"));
            ui->DeviceTypeCB->addItem(Item,QVariant(ItemData));
        }
        ui->DeviceTypeCB->setCurrentIndex(0);
    }
}

//====================================================================================================================

void wgt_JobConvertVideo::s_DeviceTypeCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertVideo::s_DeviceTypeCB");

    ui->DeviceModelCB->clear();
    int  ExportMode=ui->DeviceCB->currentIndex();
    if (ExportMode>=0) {
        ExportMode=ui->DeviceCB->itemData(ExportMode).toInt();
        int ItemData=ui->DeviceTypeCB->currentIndex();
        if (ItemData>=0) ItemData=ui->DeviceTypeCB->itemData(ItemData).toInt();
        QStringList List;
        for (int i=0;i<((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel.count();i++)
            if ((((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->DeviceType==ExportMode)&&(((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->DeviceSubtype==ItemData))
                List.append(((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->DeviceName);
        List.sort();
        ui->DeviceModelCB->addItems(List);
        ui->DeviceModelCB->setCurrentIndex(0);
    }
}

//====================================================================================================================

void wgt_JobConvertVideo::s_DeviceModelCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertVideo::s_DeviceModelCB");

    Job->Command="";
    QStringList CommandList;
    int         ExportMode=ui->DeviceCB->currentIndex();

    if ((ExportMode>=0)&&(ui->DeviceModelCB->currentText()!="")) {
        ExportMode=ui->DeviceCB->itemData(ExportMode).toInt();
        QString Device=ui->DeviceModelCB->currentText();
        int i=0;
        while ((i<((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel.count())&&(((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
        if (i<((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel.count()) {

            int     OutputFileFormat;               // Container format
            int     Standard;
            int     VideoBitRate;                   // Last video bit rate used
            int     AudioFrequency;                 // Last audio frequency used
            int     AudioBitRate;                   // Last audio bit rate used
            int     ImageSize;                      // Last image size use for rendering
            int     VideoCodecIndex;                // Index of video codec
            int     AudioCodecIndex;                // Index of audio codec

            OutputFileFormat=0;
            QString Device=ui->DeviceModelCB->currentText();
            int i=0;
            while ((i<((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel.count())&&(((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->DeviceName!=Device)) i++;
            if (i<((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel.count()) {
                Standard        =((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->Standard;
                OutputFileFormat=((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->FileFormat;
                VideoCodecIndex =((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->VideoCodec;
                AudioCodecIndex =((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->AudioCodec;
                ImageSize       =((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->ImageSize;
                VideoBitRate    =((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->VideoBitrate;
                AudioBitRate    =((cApplicationConfig *)Dialog->BaseApplicationConfig)->DeviceModelList.RenderDeviceModel[i]->AudioBitrate;

                if      (QString(FORMATDEF[OutputFileFormat].ShortName)==QString("flv")) AudioFrequency=44100;   // Special case for FLV
                else if (QString(FORMATDEF[OutputFileFormat].ShortName)==QString("3gp")) AudioFrequency=8000;    // Special case for AMRNB
                else                                                                     AudioFrequency=48000;   // Standard audio frequency for video


                // Container part
                CommandList.append(QString("-FCO:%1").arg(OutputFileFormat));
                CommandList.append(QString("-VB:%1").arg(VideoBitRate));

                // Video part
                CommandList.append(QString("-VCO:%1").arg(VideoCodecIndex));
                CommandList.append(QString("-VB:%1").arg(VideoBitRate));
                CommandList.append(QString("-VI:%1/%2").arg(ImageSize).arg(Standard));

                // Audio part
                CommandList.append(QString("-ACO:%1").arg(AudioCodecIndex));
                CommandList.append(QString("-AB:%1k").arg(AudioBitRate));
                CommandList.append(QString("-AR:%1").arg(AudioFrequency));
            }

        }
    }
    // Construct CommandList
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}

