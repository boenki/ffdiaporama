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

//====================================================================================================================

DlgRenderVideo::DlgRenderVideo(cDiaporama &TheDiaporama,QWidget *parent):QDialog(parent),ui(new Ui::DlgRenderVideo) {
    ui->setupUi(this);
    Diaporama       =&TheDiaporama;
    IsDestFileOpen  =false;

    // For the first step, only SelectDestWidget is display and enable
    ui->SelectDestScroll->setEnabled(true);     ui->SelectDestScroll->setVisible(true);
    ui->ProcessWidget->setEnabled(false);       ui->ProcessWidget->setVisible(false);

    // Output file
    if (Diaporama->OutputFileName=="")
        Diaporama->OutputFileName=Diaporama->ApplicationConfig->LastRenderVideoPath+
        (Diaporama->ApplicationConfig->LastRenderVideoPath.endsWith(QDir::separator())?"":QString(QDir::separator()))+
        (Diaporama->ProjectFileName!=""?QFileInfo(Diaporama->ProjectFileName).baseName():"movie");

    // Init format container combo
    ui->DestinationFilePath->setText(Diaporama->OutputFileName);
    bool IsFind=false;
    for (int i=0;i<NBR_FORMATDEF;i++) if (FORMATDEF[i].IsFind) {
        ui->FileFormatCB->addItem(FORMATDEF[i].LongName,QVariant(i));
        if (i==Diaporama->OutputFileFormat) {
            ui->FileFormatCB->setCurrentIndex(ui->FileFormatCB->count()-1);
            IsFind=true;
        }
    }
    if (!IsFind) ui->FileFormatCB->setCurrentIndex(0);

    AdjustDestinationFile();
    connect(ui->DestinationFilePathBT,SIGNAL(clicked()),this,SLOT(SelectDestinationFile()));
    connect(ui->DestinationFilePath,SIGNAL(editingFinished()),this,SLOT(AdjustDestinationFile()));
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
    QDesktopServices::openUrl(QUrl(QString("http://ffdiaporama.tuxfamily.org/")+CurrentLanguage+QString("/HelpRenderVideo.php")));
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
    int     OutputFileFormat=ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
    QString FileFormat      =QString(FORMATDEF[OutputFileFormat].LongName)+" (*."+QString(FORMATDEF[OutputFileFormat].FileExtension)+")";
    QString OutputFileName  =QFileDialog::getSaveFileName(this,QCoreApplication::translate("MainWindow","Select destination file"),Diaporama->ApplicationConfig->LastRenderVideoPath,FileFormat);
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
        while ((Index<NBR_VIDEOCODECDEF)&&(Codec!=QString(VIDEOCODECDEF[Index].ShortName))) Index++;
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

        QString     AllowedBitRate=FORMATDEF[ui->FileFormatCB->currentIndex()].PossibleVideoCodec;
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
            ui->VideoBitRateCB->addItem(BitRate);
            if ((ChangeIndex==-1)&&(BitRate==QString("%1k").arg(Diaporama->VideoBitRate))) {
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

        QString     AllowedBitRate=FORMATDEF[ui->FileFormatCB->currentIndex()].PossibleVideoCodec;
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
            ui->AudioBitRateCB->addItem(BitRate);
            if ((ChangeIndex==-1)&&(BitRate==QString("%1k").arg(Diaporama->AudioBitRate))) {
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
    int     OutputFileFormat =ui->FileFormatCB->itemData(ui->FileFormatCB->currentIndex()).toInt();
    QString FileFormat       =FORMATDEF[OutputFileFormat].FileExtension;
    QString OutputFileName   =ui->DestinationFilePath->text();

    OutputFileName=QFileInfo(OutputFileName).absoluteFilePath();
    if (QFileInfo(OutputFileName).suffix().length()>0) OutputFileName=OutputFileName.left(OutputFileName.length()-QFileInfo(OutputFileName).suffix().length()-1);
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

void DlgRenderVideo::accept() {
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
        bool Continue=true;                                  // Loop control

        // if process encoding was not started then start it

        // Only ProcessWidget must be display and enable
        ui->SelectDestScroll->setEnabled(false);    ui->SelectDestScroll->setVisible(false);
        ui->ProcessWidget->setEnabled(true);        ui->ProcessWidget->setVisible(true);
        ui->OkBt->setEnabled(false);                ui->SelectDestScroll->setVisible(false);

        IsDestFileOpen      =true;
        StopProcessWanted   =false;

        // Get values from controls
        QString BitRate;

        Diaporama->OutputFileFormat =ui->FileFormatCB->currentIndex(); if (Diaporama->OutputFileFormat>=0) Diaporama->OutputFileFormat=ui->FileFormatCB->itemData(Diaporama->OutputFileFormat).toInt();
        FileFormat                  =FORMATDEF[Diaporama->OutputFileFormat].FileExtension;
        Diaporama->OutputFileName   =ui->DestinationFilePath->text();
        Diaporama->ImageGeometry    =ui->GeometryCombo->currentIndex();
        Diaporama->LastStandard     =ui->StandardCombo->currentIndex();
        Diaporama->LastImageSize    =ui->ImageSizeCombo->currentIndex();
        Diaporama->VideoFrameRate   =DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].dFPS;
        Diaporama->AudioFrequency   =48000;

        VideoCodecIndex             =ui->VideoFormatCB->currentIndex();
        if (VideoCodecIndex<0) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Video codec error !");
            done(0);
            return;
        }
        VideoCodecIndex             =ui->VideoFormatCB->itemData(VideoCodecIndex).toInt();
        Diaporama->VideoCodec       =VIDEOCODECDEF[VideoCodecIndex].ShortName;

        AudioCodecIndex             =ui->AudioFormatCB->currentIndex();
        if (AudioCodecIndex<0) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Audio codec error !");
            done(0);
            return;
        }
        AudioCodecIndex             =ui->AudioFormatCB->itemData(AudioCodecIndex).toInt();
        Diaporama->AudioCodec       =AUDIOCODECDEF[AudioCodecIndex].ShortName;

        BitRate=ui->VideoBitRateCB->currentText();  if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1);    Diaporama->VideoBitRate=BitRate.toInt();
        BitRate=ui->AudioBitRateCB->currentText();  if (BitRate.endsWith("k")) BitRate=BitRate.left(BitRate.length()-1);    Diaporama->AudioBitRate=BitRate.toInt();

        // Special case for WAV
        if (AUDIOCODECDEF[AudioCodecIndex].Codec_id==CODEC_ID_PCM_S16LE) Diaporama->AudioBitRate=1536;

        // Special case for FLV
        if (QString(FORMATDEF[Diaporama->OutputFileFormat].ShortName)==QString("flv")) Diaporama->AudioFrequency=44100;

        ui->InformationLabel1->setText(Diaporama->OutputFileName);
        ui->InformationLabel2->setText(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Name);
        ui->InformationLabel3->setText(QString(VIDEOCODECDEF[VideoCodecIndex].LongName)+" - "+QString("%1").arg(Diaporama->VideoBitRate)+" kb/s");
        ui->InformationLabel4->setText(QString(AUDIOCODECDEF[AudioCodecIndex].LongName)+QString(" - %1 Hz - ").arg(Diaporama->AudioFrequency)+QString("%1").arg(Diaporama->AudioBitRate)+" kb/s");

        //**********************************************************************************************************************************

        FPS     =(uint64_t)AV_TIME_BASE/Diaporama->VideoFrameRate;          // Time duration of a frame
        NbrFrame=int(double(Diaporama->GetDuration()*1000)/double(FPS));    // Number of frame to generate

        ui->SoundProgressBar->setValue(0);
        ui->SoundProgressBar->setMaximum(NbrFrame);
        ui->SlideProgressBar->setValue(0);
        ui->TotalProgressBar->setValue(0);
        ui->TotalProgressBar->setMaximum(NbrFrame);
        ui->SlideNumberLabel->setText("");
        ui->FrameNumberLabel->setText("");

        //**********************************************************************************************************************************
        // 1st step encoding : produce WAV file with sound
        //**********************************************************************************************************************************

        StartTime=QTime::currentTime();                                  // Display control : time the process start

        // Create tempwavefile in the same directory as destination file
        TempWAVFileName=QFileInfo(ui->DestinationFilePath->text()).absolutePath();
        TempWAVFileName=TempWAVFileName+"/temp.wav";
        TempWAVFileName=AdjustDirForOS(TempWAVFileName);

        Continue=WriteTempAudioFile(TempWAVFileName);

        //**********************************************************************************************************************************
        // 2nd step encoding : produce final file using temporary WAV file with sound
        //**********************************************************************************************************************************

        // Construct ffmpeg command line
        if (Continue) {
            W       =DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Width;
            H       =DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Height;
            Extend  =DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Extend;

            // Video codec part
            QString Preset=AdjustDirForOS(QDir::currentPath());
            if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();
            Preset="-fpre \""+Preset+"libx264-hq.ffpreset\"";
            switch (VIDEOCODECDEF[VideoCodecIndex].Codec_id) {
                case CODEC_ID_MPEG2VIDEO :  vCodec=QString("-vcodec mpeg2video -minrate %1k -maxrate %2k -bufsize %3k -b %4k -bf 3")
                                                   .arg(Diaporama->VideoBitRate-Diaporama->VideoBitRate/10)
                                                   .arg(Diaporama->VideoBitRate+Diaporama->VideoBitRate/10)
                                                   .arg(Diaporama->VideoBitRate*2)
                                                   .arg(Diaporama->VideoBitRate);
                                            break;
                case CODEC_ID_MPEG4 :       if (QString(VIDEOCODECDEF[VideoCodecIndex].ShortName)==QString("mpeg4"))
                                                vCodec=QString("-vcodec mpeg4 -vtag xvid -b %1k").arg(Diaporama->VideoBitRate);
                                                else vCodec=QString("-vcodec libxvid -b %1k").arg(Diaporama->VideoBitRate);
                                            break;
                case CODEC_ID_H264 :        vCodec=QString("-vcodec libx264 ")+Preset+QString(" -refs 3 -minrate %1k -maxrate %2k -bufsize %3k -b %4k -bf 3")
                                                .arg(Diaporama->VideoBitRate-Diaporama->VideoBitRate/10)
                                                .arg(Diaporama->VideoBitRate+Diaporama->VideoBitRate/10)
                                                .arg(Diaporama->VideoBitRate*2)
                                                .arg(Diaporama->VideoBitRate);
                                            break;
                case CODEC_ID_MJPEG:        vCodec="-vcodec mjpeg -qscale 2 -qmin 2 -qmax 2";   break;
                case CODEC_ID_VP8:          vCodec=QString("-vcodec libvpx -minrate %1k -maxrate %2k -bufsize %3k -b %4k -bf 3")
                                                .arg(Diaporama->VideoBitRate-Diaporama->VideoBitRate/10)
                                                .arg(Diaporama->VideoBitRate+Diaporama->VideoBitRate/10)
                                                .arg(Diaporama->VideoBitRate*2)
                                                .arg(Diaporama->VideoBitRate);
                                            break;
                case 22 :                   vCodec=QString("-vcodec flv -b %1k").arg(Diaporama->VideoBitRate);
                                            break;
                default:
                    QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Unknown video codec");
                    Continue=false;
                    break;
            }

            // Audio codec part
            switch (AUDIOCODECDEF[AudioCodecIndex].Codec_id) {
                case CODEC_ID_PCM_S16LE:    aCodec=QString("-acodec pcm_s16le -ab %1k").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_MP2:          aCodec=QString("-acodec mp2 -ab %1k").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_MP3:          aCodec=QString("-acodec libmp3lame -ab %1k").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_AAC:          if (QString(AUDIOCODECDEF[AudioCodecIndex].ShortName)==QString("aac"))
                                                aCodec=QString("-acodec aac -strict experimental -ab %1k").arg(Diaporama->AudioBitRate);
                                                else aCodec=QString("-acodec libfaac -ab %1k").arg(Diaporama->AudioBitRate);
                                            break;
                case CODEC_ID_AC3:          aCodec=QString("-acodec ac3 -ab %1k").arg(Diaporama->AudioBitRate); break;
                case CODEC_ID_VORBIS:       aCodec=QString("-acodec libvorbis -ab %1k").arg(Diaporama->AudioBitRate); break;

                default:
                    QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Unknown audio codec");
                    Continue=false;
                    break;
            }

            if (Continue) {
                #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
                ffmpegCommand="\""+Diaporama->ApplicationConfig->PathFFMPEG+"\"";
                #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
                ffmpegCommand=Diaporama->ApplicationConfig->PathFFMPEG;
                #endif
                ffmpegCommand=ffmpegCommand+QString(" -y -f image2pipe -vcodec ppm -i - -i \"")+TempWAVFileName+"\" "+vCodec+" -r "+
                      QString(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].FPS)+                        
                      " "+aCodec+QString(" -ar %1 -ac 2 -aspect %2:%3")
                      .arg(Diaporama->AudioFrequency)
                      .arg(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].PARNUM)
                      .arg(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].PARDEN);
                if (Extend>0) ffmpegCommand=ffmpegCommand+QString(" -padtop %1 -padbottom %2").arg(Extend).arg(Extend);

                // Activate multithreading support if getCpuCount()>1 and codec is h264 or VP8
                if ((getCpuCount()>1)&&(
                        (VIDEOCODECDEF[VideoCodecIndex].Codec_id==CODEC_ID_H264)||
                        (VIDEOCODECDEF[VideoCodecIndex].Codec_id==CODEC_ID_VP8)
                        )) ffmpegCommand=ffmpegCommand+" -threads "+QString("%1").arg(getCpuCount());

                ffmpegCommand=ffmpegCommand+" \""+Diaporama->OutputFileName+"\"";

                ffmpegCommand=AdjustDirForOS(ffmpegCommand);

                qDebug()<<ffmpegCommand;
            }
        }

        // Start ffmpegCommand
        if (Continue) {
            Process.setProcessChannelMode(QProcess::ForwardedChannels);                     // Forward standard and error message to the ffdiaporama console
            //Process.setWorkingDirectory(Diaporama->ApplicationConfig->UserConfigPath);      // Set working directory to user folder (for log generation)
            Process.start(ffmpegCommand,QIODevice::Append|QIODevice::ReadWrite);            // Start command
            if (!Process.waitForStarted()) {
                QMessageBox::critical(NULL,QCoreApplication::translate("DlgRenderVideo","Error","Error message"),
                                      QCoreApplication::translate("DlgRenderVideo","Error starting ffmpeg","Error message")+"\n"+ffmpegCommand,
                                      QMessageBox::Close);
                Continue=false;
            }
        }

        // Encode video
        if (Continue) {
            LastCheckTime   =StartTime;     // Display control : last time the loop start
            int Position    =0;             // Render current position
            int ColumnStart =-1;            // Render start position of current object
            int Column      =-1;            // Render current object

            for (int RenderedFrame=0;Continue && (RenderedFrame<NbrFrame);RenderedFrame++) {
                if ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+Diaporama->List[Column].GetDuration()-Diaporama->GetTransitionDuration(Column+1))<=Position))) {
                    Column++;
                    ColumnStart=Position;
                    if (Column<Diaporama->List.count()) ui->SlideProgressBar->setMaximum(int(double(Diaporama->List[Column].GetDuration()-Diaporama->GetTransitionDuration(Column+1))/(FPS/double(1000)))-1);
                    RefreshDisplay =true;
                    if (Column>0) Diaporama->FreeUnusedMemory(Column-1);
                } else RefreshDisplay =(LastCheckTime.msecsTo(QTime::currentTime())>=1000);    // Refresh display only one time per second

                // Refresh Display (if needed)
                if (RefreshDisplay) {
                    DurationProcess=StartTime.msecsTo(QTime::currentTime());
                    DisplayText=QString("%1").arg((QTime(0,0,0,0).addMSecs(DurationProcess)).toString("hh:mm:ss"));     ui->ElapsedTimeLabel->setText(DisplayText);
                    DisplayText=QString("%1").arg(double(RenderedFrame)/(double(DurationProcess)/1000),0,'f',1);        ui->FPSLabel->setText(DisplayText);
                    DisplayText=QString("%1/%2").arg(Column+1).arg(Diaporama->List.count());                            ui->SlideNumberLabel->setText(DisplayText);
                    DisplayText=QString("%1/%2").arg(RenderedFrame).arg(NbrFrame);                                      ui->FrameNumberLabel->setText(DisplayText);
                    ui->SlideProgressBar->setValue(int(double(Position-ColumnStart)/(FPS/double(1000))));
                    ui->TotalProgressBar->setValue(RenderedFrame);
                    LastCheckTime=QTime::currentTime();
                }

                // Get current frame
                Frame=new cDiaporamaObjectInfo(PreviousFrame,Position,Diaporama,(FPS/1000));

                // Prepare frame with correct W and H
                Diaporama->LoadSources(Frame,W,H,false);                                     // Load source images
                Diaporama->DoAssembly(Frame,W,H);                                            // Make final assembly

                // Give time to interface !
                QCoreApplication::processEvents();

                // Save image to the pipe
                if (!Frame->RenderedImage->save(&Process,"PPM",100)) {
                    QMessageBox::critical(NULL,QCoreApplication::translate("DlgRenderVideo","Error","Error message"),QCoreApplication::translate("DlgRenderVideo","Error sending image to ffmpeg","Error message"),QMessageBox::Close);
                    Continue=false;
                }

                // Wait until ffmpeg processed the frame
                while (Continue &&(Process.bytesToWrite()>0)) {
                    if (!Process.waitForBytesWritten()) {
                        QMessageBox::critical(NULL,QCoreApplication::translate("DlgRenderVideo","Error","Error message"),QCoreApplication::translate("DlgRenderVideo","ffmpeg error","Error message"),QMessageBox::Close);
                        Continue=false;
                    }
                    // Give time to interface !
                    QCoreApplication::processEvents();
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
            DisplayText=QString("%1/%2").arg(Column+1).arg(Diaporama->List.count());                            ui->SlideNumberLabel->setText(DisplayText);
            DisplayText=QString("%1/%2").arg(NbrFrame).arg(NbrFrame);                                           ui->FrameNumberLabel->setText(DisplayText);
            ui->SlideProgressBar->setValue(ui->SlideProgressBar->maximum());
            ui->TotalProgressBar->setValue(NbrFrame);

            if (!Process.waitForFinished(30000)) { // 30 sec max to close ffmpeg
                QMessageBox::critical(NULL,QCoreApplication::translate("DlgRenderVideo","Error","Error message"),QCoreApplication::translate("DlgRenderVideo","Error during ffmpeg process","Error message"),QMessageBox::Close);
                Process.terminate();
                Continue=false;
            } else if (Process.exitStatus()!=QProcess::NormalExit) {
              QMessageBox::critical(NULL,QCoreApplication::translate("DlgRenderVideo","Error","Error message"),QCoreApplication::translate("DlgRenderVideo","Error exiting ffmpeg","Error message"),QMessageBox::Close);
              Continue=false;
            }
        }

        QFile::remove(TempWAVFileName);

        // Free unused CacheFullImage
        Diaporama->FreeUnusedMemory(-1);

        // Inform user of success
        if (Continue) QMessageBox::information(this,QCoreApplication::translate("DlgRenderVideo","Render video"),QCoreApplication::translate("DlgRenderVideo","Job completed succesfully !"));

        // Save Window size and position
        Diaporama->ApplicationConfig->DlgRenderVideoWSP->SaveWindowState(this);

        // Close the dialog box
        done(0);
    }
}

//============================================================================================
// Make audio temp file
//============================================================================================

bool DlgRenderVideo::WriteTempAudioFile(QString TempWAVFileName) {
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
    Fmt=av_guess_format(NULL,TempWAVFileName.toLocal8Bit(),NULL);
    if (Fmt==NULL) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error creating temporary wav file !");
        Continue=false;
    } else Fmt->audio_codec=CODEC_ID_PCM_S16LE;

    // allocate the output media context
    if (Continue) {
        OutputFormatContext = avformat_alloc_context();
        if (!OutputFormatContext) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : Unable to allocate OutputFormatContext !");
            Continue=false;
        } else {
            memcpy(OutputFormatContext->filename,TempWAVFileName.toLocal8Bit(),strlen(TempWAVFileName.toLocal8Bit())+1);
            OutputFormatContext->oformat  =Fmt;
            OutputFormatContext->timestamp=0;
            OutputFormatContext->bit_rate =1536;
            //OutputFormatContext->preload  = (int)(0.5 * AV_TIME_BASE);
            //OutputFormatContext->max_delay= (int)(0.7 * AV_TIME_BASE);
            // set the output parameters
            if (av_set_parameters(OutputFormatContext,&fpOutFile)<0) {
                av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
                QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Invalid output format parameters !");
                Continue=false;
            }
        }
    }

    // Allocate AudioStream
    if (Continue) {
        AudioStream=av_new_stream(OutputFormatContext,0);
        if (AudioStream==NULL) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio stream !");
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
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Audio codec not found !");
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
                QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"could not open audio codec !");
                av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
                Continue=false;
            } else {
                // Init sound blocks
                int audio_input_frame_size=AudioStream->codec->frame_size;                          // frame size in samples
                if (audio_input_frame_size<=1) audio_input_frame_size=RenderMusic.SoundPacketSize; else audio_input_frame_size*=RenderMusic.SampleBytes*RenderMusic.Channels;
                RenderMusic.SetFPS(Diaporama->VideoFrameRate);
                EncodedAudio.SetFrameSize(audio_input_frame_size);
            }
        }
    }

    // open the file for writing
    if (Continue) {
        #if FF_API_OLD_AVIO
        #if AVIO_WRONLY
        if (avio_open(&OutputFormatContext->pb,TempWAVFileName.toLocal8Bit(),AVIO_WRONLY)<0) {
        #else
        if (avio_open(&OutputFormatContext->pb,TempWAVFileName.toLocal8Bit(),URL_WRONLY)<0) {
        #endif
        #else
        if (url_fopen(&OutputFormatContext->pb,TempWAVFileName.toLocal8Bit(),URL_WRONLY)<0) {
        #endif
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error creating temporary audio file !");
            Continue=false;
        }
    }

    // Allocate buffer to encode
    if (Continue) {
        audio_outbuf=(uint8_t *)av_malloc(FF_MIN_BUFFER_SIZE);
        if (audio_outbuf==NULL) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio buffer !");
            Continue=false;
        }
    }

    // write the header
    if ((Continue)&&(av_write_header(OutputFormatContext)!=0)) {
        av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error writing the header of the temporary audio file !");
        Continue=false;
    }

    // Encode the file
    if (Continue) {
        LastCheckTime   =StartTime;     // Display control : last time the loop start
        int Position    =0;             // Render current position
        int ColumnStart =-1;            // Render start position of current object
        int Column      =-1;            // Render current object
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
                Frame->CurrentObject_MusicTrack->SetFPS(Diaporama->VideoFrameRate);
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
                Frame->TransitObject_MusicTrack=new cSoundBlockList();
                Frame->TransitObject_MusicTrack->SetFPS(Diaporama->VideoFrameRate);
            }

            // Ensure SoundTracks are ready
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
                Frame->CurrentObject_SoundTrackMontage=new cSoundBlockList();
                Frame->CurrentObject_SoundTrackMontage->SetFPS(Diaporama->VideoFrameRate);
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
                Frame->TransitObject_SoundTrackMontage=new cSoundBlockList();
                Frame->TransitObject_SoundTrackMontage->SetFPS(Diaporama->VideoFrameRate);
            }

            // Prepare frame with W and H =0 to force SoundMusicOnly ! (thread mode is not necessary here)
            Diaporama->LoadSources(Frame,0,0,false);

            // Give time to interface !
            QCoreApplication::processEvents();

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
                int16_t     *Packet=RenderMusic.DetachFirstPacket();
                AVPacket    pkt;

                EncodedAudio.AppendData(Packet,RenderMusic.SoundPacketSize);
                while (EncodedAudio.List.count()>0) {
                    int16_t *PacketSound=EncodedAudio.DetachFirstPacket();
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
                            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error while writing audio frame !");
                            Continue=false;
                        }
                    } else if (out_size<0) {
                        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error encoding sound !");
                        Continue=false;
                    }
                    av_free(PacketSound);
                }

                av_free(Packet);

            }
            QCoreApplication::processEvents();  // Give time to interface !

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
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error writing the trailer of the temporary audio file !");
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
