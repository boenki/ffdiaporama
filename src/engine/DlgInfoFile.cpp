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

#include "DlgInfoFile.h"
#include "ui_DlgInfoFile.h"

//====================================================================================================================

DlgInfoFile::DlgInfoFile(cBaseMediaFile *MediaFile,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgInfoFile) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgInfoFile::DlgInfoFile");

    this->MediaFile=MediaFile;

    ui->setupUi(this);
    //CancelBt=ui->CancelBt;
    OkBt    =ui->OkBt;
    HelpBt  =ui->HelpBT;
}

//====================================================================================================================

DlgInfoFile::~DlgInfoFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgInfoFile::~DlgInfoFile");

    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgInfoFile::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgInfoFile::DoInitDialog");

    DoInitTableWidget(ui->tableWidget,"Propertie;Value");

    if (MediaFile) {
        // General file information
        ui->FileIconLabel->setPixmap(QPixmap().fromImage(MediaFile->Icon100));
        ui->FileNameValue->setText(MediaFile->ShortName);
        ui->FileTypeValue->setText(MediaFile->GetFileTypeStr()+QString("(%1)").arg(MediaFile->GetInformationValue("Long Format")));
        ui->FileSizeValue->setText(MediaFile->GetFileSizeStr());
        ui->FileCreatedValue->setText(MediaFile->GetFileDateTimeStr(true));
        ui->FileModifyValue->setText(MediaFile->GetFileDateTimeStr(false));

        //**************** Video
        if ((MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)&&(((cVideoFile *)MediaFile)->VideoTrackNbr>0)) {
            ui->VideoTable->setUpdatesEnabled(false);
            ui->VideoTitleLabel->setVisible(true);
            ui->VideoTable->setVisible(true);
            DoInitTableWidget(ui->VideoTable,"#;Image Size;Image Format;Image Geometry;Codec;Frame Rate;Bitrate");
            for (int i=0;i<((cVideoFile *)MediaFile)->VideoTrackNbr;i++) {
                QString TrackNum=QString("%1").arg(i); while (TrackNum.length()<3) TrackNum="0"+TrackNum;
                TrackNum="Video_"+TrackNum+":";
                QColor Background=((i & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
                ui->VideoTable->insertRow(ui->VideoTable->rowCount());
                ui->VideoTable->setItem(ui->VideoTable->rowCount()-1,0,CreateItem(QString("%1").arg(i+1),Qt::AlignLeft|Qt::AlignVCenter,Background));
                ui->VideoTable->setItem(ui->VideoTable->rowCount()-1,1,CreateItem(MediaFile->GetImageSizeStr(cBaseMediaFile::SIZEONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->VideoTable->setItem(ui->VideoTable->rowCount()-1,2,CreateItem(MediaFile->GetImageSizeStr(cBaseMediaFile::FMTONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->VideoTable->setItem(ui->VideoTable->rowCount()-1,3,CreateItem(MediaFile->GetImageSizeStr(cBaseMediaFile::GEOONLY),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->VideoTable->setItem(ui->VideoTable->rowCount()-1,4,CreateItem(MediaFile->GetInformationValue(TrackNum+"Codec"),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->VideoTable->setItem(ui->VideoTable->rowCount()-1,5,CreateItem(MediaFile->GetInformationValue(TrackNum+"Frame rate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->VideoTable->setItem(ui->VideoTable->rowCount()-1,6,CreateItem(MediaFile->GetInformationValue(TrackNum+"Bitrate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
            }
            DoResizeColumnsTableWidget(ui->VideoTable);
            ui->VideoTable->setUpdatesEnabled(true);
        } else {
            ui->VideoTitleLabel->setVisible(false);
            ui->VideoTable->setVisible(false);
        }

        //**************** Chapters
        if (((MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)&&(((cVideoFile *)MediaFile)->NbrChapters>0))||((MediaFile->ObjectType==OBJECTTYPE_FFDFILE)&&(((cffDProjectFile *)MediaFile)->NbrChapters>0))) {
            ui->ChapterTable->setUpdatesEnabled(false);
            ui->ChapterTitleLabel->setVisible(true);
            ui->ChapterTable->setVisible(true);
            int NbrChapter=(MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)?((cVideoFile *)MediaFile)->NbrChapters:((MediaFile->ObjectType==OBJECTTYPE_FFDFILE)?((cffDProjectFile *)MediaFile)->NbrChapters:0);
            if (MediaFile->ObjectType==OBJECTTYPE_FFDFILE) DoInitTableWidget(ui->ChapterTable,"#;Slide;Title;Start;End;Duration");
                else DoInitTableWidget(ui->ChapterTable,"#;Title;Start;End;Duration");
            for (int i=0;i<NbrChapter;i++) {
                int     Col=0;
                QString ChapterNum=QString("%1").arg(i); while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
                ChapterNum="Chapter_"+ChapterNum+":";
                QColor Background=((i & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
                ui->ChapterTable->insertRow(ui->ChapterTable->rowCount());
                ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,Col++,CreateItem(QString("%1").arg(i+1),Qt::AlignLeft|Qt::AlignVCenter,Background));
                if (MediaFile->ObjectType==OBJECTTYPE_FFDFILE) ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,Col++,CreateItem(MediaFile->GetInformationValue(ChapterNum+"InSlide"),Qt::AlignLeft|Qt::AlignVCenter,Background));
                ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,Col++,CreateItem(MediaFile->GetInformationValue(ChapterNum+"title"),Qt::AlignLeft|Qt::AlignVCenter,Background));
                ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,Col++,CreateItem(MediaFile->GetInformationValue(ChapterNum+"Start"),Qt::AlignLeft|Qt::AlignVCenter,Background));
                ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,Col++,CreateItem(MediaFile->GetInformationValue(ChapterNum+"End"),Qt::AlignLeft|Qt::AlignVCenter,Background));
                ui->ChapterTable->setItem(ui->ChapterTable->rowCount()-1,Col++,CreateItem(MediaFile->GetInformationValue(ChapterNum+"Duration"),Qt::AlignLeft|Qt::AlignVCenter,Background));
            }
            DoResizeColumnsTableWidget(ui->ChapterTable);
            ui->ChapterTable->setUpdatesEnabled(true);
        } else {
            ui->ChapterTitleLabel->setVisible(false);
            ui->ChapterTable->setVisible(false);
        }

        //**************** Audio
        if (((MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)||(MediaFile->ObjectType==OBJECTTYPE_MUSICFILE))&&(((cVideoFile *)MediaFile)->AudioTrackNbr>0)) {
            ui->AudioTable->setUpdatesEnabled(false);
            ui->AudioTitleLabel->setVisible(true);
            ui->AudioTable->setVisible(true);
            if (MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE) DoInitTableWidget(ui->AudioTable,"#;Language;Codec;Channels;Bitrate;Frequency;Title");
                else DoInitTableWidget(ui->AudioTable,"#;Language;Codec;Channels;Bitrate;Frequency");
            for (int i=0;i<((cVideoFile *)MediaFile)->AudioTrackNbr;i++) {
                QString TrackNum=QString("%1").arg(i); while (TrackNum.length()<3) TrackNum="0"+TrackNum;
                TrackNum="Audio_"+TrackNum+":";
                QColor Background=((i & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
                ui->AudioTable->insertRow(ui->AudioTable->rowCount());
                ui->AudioTable->setItem(ui->AudioTable->rowCount()-1,0,CreateItem(QString("%1").arg(i+1),Qt::AlignLeft|Qt::AlignVCenter,Background));
                ui->AudioTable->setItem(ui->AudioTable->rowCount()-1,1,CreateItem(MediaFile->GetInformationValue(TrackNum+"language"),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->AudioTable->setItem(ui->AudioTable->rowCount()-1,2,CreateItem(MediaFile->GetInformationValue(TrackNum+"Codec"),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->AudioTable->setItem(ui->AudioTable->rowCount()-1,3,CreateItem(MediaFile->GetInformationValue(TrackNum+"Channels"),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->AudioTable->setItem(ui->AudioTable->rowCount()-1,4,CreateItem(MediaFile->GetInformationValue(TrackNum+"Bitrate"),Qt::AlignCenter|Qt::AlignVCenter,Background));
                ui->AudioTable->setItem(ui->AudioTable->rowCount()-1,5,CreateItem(MediaFile->GetInformationValue(TrackNum+"Frequency"),Qt::AlignCenter|Qt::AlignVCenter,Background));
                if (MediaFile->ObjectType==OBJECTTYPE_VIDEOFILE)
                    ui->AudioTable->setItem(ui->AudioTable->rowCount()-1,6,CreateItem(MediaFile->GetInformationValue(TrackNum+"title"),Qt::AlignLeft|Qt::AlignVCenter,Background));
            }
            DoResizeColumnsTableWidget(ui->AudioTable);
            ui->AudioTable->setUpdatesEnabled(true);
        } else {
            ui->AudioTitleLabel->setVisible(false);
            ui->AudioTable->setVisible(false);
        }

        //**************** Additionnals
        ui->tableWidget->setUpdatesEnabled(false);
        ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::QHeaderView::ResizeToContents);
        for (int i=0;i<MediaFile->InformationList.count();i++)
          if ((!((QString)MediaFile->InformationList[i]).startsWith("Chapter_"))
              &&(!((QString)MediaFile->InformationList[i]).startsWith("Video_"))
              &&(!((QString)MediaFile->InformationList[i]).startsWith("Audio_"))
              &&(!((QString)MediaFile->InformationList[i]).startsWith("Short Format"))
              &&(!((QString)MediaFile->InformationList[i]).startsWith("Long Format"))
          ) {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());
            QStringList Value=MediaFile->InformationList[i].split("##");
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(Value[0]));
            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(Value[1]));
        }
        DoResizeColumnsTableWidget(ui->tableWidget);
        ui->tableWidget->setUpdatesEnabled(true);
    } else {
        ui->VideoTitleLabel->setVisible(false);
        ui->VideoTable->setVisible(false);
        ui->ChapterTitleLabel->setVisible(false);
        ui->ChapterTable->setVisible(false);
        ui->AudioTitleLabel->setVisible(false);
        ui->AudioTable->setVisible(false);
    }
}
