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
    Diaporama=&TheDiaporama;

    // Init LIBAV values
    IsDestFileOpen   =false;
    VideoStream      =NULL;
    AudioStream      =NULL;
    VideoCodecContext=NULL;
    AudioCodecContext=NULL;
    VideoCodec       =NULL;
    AudioCodec       =NULL;

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
}

//====================================================================================================================

DlgRenderVideo::~DlgRenderVideo() {
    delete ui;

    if (IsDestFileOpen) {
        // close each codec
        if (VideoStream!=NULL) {
            avcodec_close(VideoStream->codec);
        }

        // free the streams
        for(int i = 0; i < (int)OutputFormatContext->nb_streams; i++) {
            av_freep(&OutputFormatContext->streams[i]->codec);
            av_freep(&OutputFormatContext->streams[i]);
            OutputFormatContext->streams[i]=NULL;
        }
        VideoStream=NULL;
        AudioStream=NULL;

        // close the file
        if (OutputFormatContext->pb!=NULL) url_fclose(OutputFormatContext->pb);

        // free the stream
        if (OutputFormatContext!=NULL) {
            av_free(OutputFormatContext);
            OutputFormatContext=NULL;
        }
    }
}

//====================================================================================================================

void DlgRenderVideo::InitImageSizeCombo(int) {
    int Geometry=ui->GeometryCombo->currentIndex();
    int Standard=ui->StandardCombo->currentIndex();
    int ImageSize=ui->ImageSizeCombo->currentIndex();
    ui->ImageSizeCombo->clear();
    for (int i=0;i<4;i++) ui->ImageSizeCombo->addItem(DefImageFormat[Standard][Geometry][i].Name);
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
    OutputFileName=OutputFileName+"."+FileFormat;
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
    if (IsDestFileOpen) {
        StopProcessWanted=true;
    } else {
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
        Diaporama->VideoFrameRate   =DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].FPS;
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

        ui->InformationLabel1->setText(Diaporama->OutputFileName);
        ui->InformationLabel2->setText(DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Name);
        ui->InformationLabel3->setText(QString(VIDEOCODECDEF[VideoCodecIndex].LongName)+" - "+QString("%1").arg(Diaporama->VideoBitRate)+" kb/s");
        ui->InformationLabel4->setText(QString(AUDIOCODECDEF[AudioCodecIndex].LongName)+" - 48000 Hz - "+QString("%1").arg(Diaporama->AudioBitRate)+" kb/s");

        if (!OpenDestFile(VideoCodecIndex,AudioCodecIndex)) {
            // Save Window size and position
            Diaporama->ApplicationConfig->DlgRenderVideoWSP->SaveWindowState(this);
            done(1); // close box with error !
            return;
        }
        FPS     =(uint64_t)AV_TIME_BASE/Diaporama->VideoFrameRate;                      // Time duration of a frame
        NbrFrame=int(double(Diaporama->GetDuration()*1000)/double(FPS));                // Number of frame to generate

        int                     W=VideoStream->codec->width;                            // Width to generate
        int                     H=VideoStream->codec->height-Extend-Extend;             // Height to generate
        AVFrame                 *VideoFramePicture=NULL;                                                // Image in YUV mode
        bool                    IsImageUpdated  =false;                                 // True if image was modified (then YUV buffer must be update)
        bool                    Continue        =true;                                  // Loop control
        bool                    RefreshDisplay  =true;                                  // Display control : true if display need to ne refresh
        QImage                  *Image=NULL;
        int                     CurrentFrameNumber;

        cDiaporamaObjectInfo    *PreviousFrame  =NULL;
        cDiaporamaObjectInfo    *Frame   =NULL;

        Position                =0;                                                     // Current position
        ColumnStart             =-1;                                                    // Start position of current object
        Column                  =-1;                                                    // Current object
        StartTime               =QTime::currentTime();                                  // Display control : time the process start
        LastCheckTime           =StartTime;                                             // Display control : last time the loop start
        IsThreadWriteVideoFrame =false;
        IsThreadWriteAudioFrame =false;

        // Init sound objects
        if (AudioStream!=NULL) {
            int audio_input_frame_size=AudioStream->codec->frame_size; // frame size in samples
            if ((audio_input_frame_size<=1)&&((AudioStream->codec->codec_id==CODEC_ID_PCM_S16LE)||(AudioStream->codec->codec_id==CODEC_ID_PCM_S16BE)||
                                              (AudioStream->codec->codec_id==CODEC_ID_PCM_S16LE)||(AudioStream->codec->codec_id==CODEC_ID_PCM_S16BE)))
                audio_input_frame_size=MixedMusic.SoundPacketSize; else audio_input_frame_size*=MixedMusic.SampleBytes*MixedMusic.Channels;

            MixedMusic.ClearList();
            MixedMusic.SetFPS(Diaporama->VideoFrameRate);
            EncodedAudio.SetFrameSize(audio_input_frame_size);
        }

        ui->TotalProgressBar->setMaximum(NbrFrame);

        // write the header
        av_write_header(OutputFormatContext);

        RenderedFrame    =0;

        for (CurrentFrameNumber=0;CurrentFrameNumber<NbrFrame;CurrentFrameNumber++) {

            //============================================
            // Calculate position & column
            //============================================
            if ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+Diaporama->List[Column].GetDuration()-Diaporama->GetTransitionDuration(Column+1))<=Position))) {
                Column++;
                ColumnStart=Position;
                if (Column<Diaporama->List.count()) {
                    int ForSlider=int(double(Diaporama->List[Column].GetDuration()-Diaporama->GetTransitionDuration(Column+1))/(FPS/double(1000)))-1;
                    ui->SlideProgressBar->setMaximum(ForSlider);
                }
                RefreshDisplay =true;
            } else {
                int DurationProcess=LastCheckTime.msecsTo(QTime::currentTime());
                RefreshDisplay =(DurationProcess>=1000);    // Refresh display only one time per second
            }

            // Refresh Display (if needed)
            if (RefreshDisplay) RefreshDisplayControl();

            // Give time to interface !
            QCoreApplication::processEvents();

            //==================================================
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
            if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)&&(Frame->CurrentObject->TypeObject==DIAPORAMAOBJECTTYPE_VIDEO)) {
                Frame->CurrentObject_SoundTrackMontage=new cSoundBlockList();
                Frame->CurrentObject_SoundTrackMontage->SetFPS(Diaporama->VideoFrameRate);
            }
            if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)&&(Frame->TransitObject->TypeObject==DIAPORAMAOBJECTTYPE_VIDEO)) {
                Frame->TransitObject_SoundTrackMontage=new cSoundBlockList();
                Frame->TransitObject_SoundTrackMontage->SetFPS(Diaporama->VideoFrameRate);
            }

            IsImageUpdated=true;

            // Ensure background, image and soundtrack is ready (in thread mode)
            Diaporama->LoadSources(Frame,W,H,false);

            // Give time to interface !
            QCoreApplication::processEvents();

            //============================================
            // Prepare image
            //============================================
            Diaporama->PrepareImage(Frame,W,H,Extend,true);                                                 // Current Object
            if (Frame->IsTransition) Diaporama->PrepareImage(Frame,W,H,Extend,false);                // Transition Object
            ThreadDoAssembly=QtConcurrent::run(Diaporama,&cDiaporama::DoAssembly,Frame,W,H+Extend);

            // Give time to interface !
            QCoreApplication::processEvents();

            //============================================
            // Prepare mixed buffer for sound & music
            //============================================
            if (AudioStream!=NULL) {
                // Calc number of packet to mix
                int MaxPacket=Frame->CurrentObject_MusicTrack->List.count();
                if ((Frame->CurrentObject_SoundTrackMontage!=NULL)&&(MaxPacket>Frame->CurrentObject_SoundTrackMontage->List.count())) MaxPacket=Frame->CurrentObject_SoundTrackMontage->List.count();
                if (MaxPacket>MixedMusic.NbrPacketForFPS) MaxPacket=MixedMusic.NbrPacketForFPS;
                // Transfert and mix audio data
                for (int j=0;j<MaxPacket;j++) MixedMusic.MixAppendPacket(Frame->CurrentObject_MusicTrack->DetachFirstPacket(),(Frame->CurrentObject_SoundTrackMontage!=NULL)?Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket():NULL);
                // Give time to interface !
                QCoreApplication::processEvents();
            }

            //***************************************************************************************************************************
            // Encode and write this frame
            //***************************************************************************************************************************

            if (VideoStream!=NULL) {
                ThreadDoAssembly.waitForFinished();

                if (IsThreadWriteVideoFrame) {
                    ThreadWriteVideoFrame.waitForFinished();
                    Continue=Continue && (bool)ThreadWriteVideoFrame.result();
                }
                // Update YUV buffer if image was modified since last frame
                if (IsImageUpdated) {
                    if (VideoFramePicture!=NULL) {
                        if (VideoFramePicture->data[0]!=NULL) av_free(VideoFramePicture->data[0]);
                        av_free(VideoFramePicture);
                        VideoFramePicture=NULL;
                    }
                    VideoFramePicture=QImageToYUVStream(Frame->RenderedImage);
                }
                ThreadWriteVideoFrame=QtConcurrent::run(this,&DlgRenderVideo::WriteVideoFrame,CurrentFrameNumber,VideoFramePicture,W,H);
                IsThreadWriteVideoFrame=true;
                // Give time to interface !
                QCoreApplication::processEvents();
            }

            if (AudioStream!=NULL) for (int audiof=0;Continue && audiof<MixedMusic.NbrPacketForFPS;audiof++) {//while ((Continue)&&(MixedMusic.List.count()>0)) {
                if (IsThreadWriteAudioFrame) {
                    ThreadWriteAudioFrame.waitForFinished();
                    Continue=Continue && (bool)ThreadWriteAudioFrame.result();
                }
                ThreadWriteAudioFrame=QtConcurrent::run(this,&DlgRenderVideo::WriteAudioFrame,MixedMusic.DetachFirstPacket(),MixedMusic.SoundPacketSize);
                IsThreadWriteAudioFrame=true;
                // Give time to interface !
                QCoreApplication::processEvents();
            }
            //***************************************************************************************************************************
            // Go to next frame
            //***************************************************************************************************************************
            QCoreApplication::processEvents();

            // Free Image buffer
            if (Image!=NULL) {
                delete Image;
                Image=NULL;
            }

            // Calculate next position
            Position     +=(FPS/1000);
            if (PreviousFrame!=NULL) delete PreviousFrame;
            PreviousFrame=Frame;
            Frame =NULL;

            if ((Continue==false)||(StopProcessWanted==true)) break;   // Stop the process if error occur or user ask to stop
        }

        // Give time to interface !
        QCoreApplication::processEvents();

        if (Continue && !StopProcessWanted) ui->SlideProgressBar->setValue(ui->SlideProgressBar->maximum());

        // wait for the ThreadWriteVideoFrame is finished
        if (IsThreadWriteVideoFrame) {
            ThreadWriteVideoFrame.waitForFinished();
            Continue=Continue && (bool)ThreadWriteVideoFrame.result();
        }
        // wait for the ThreadWriteAudioFrame is finished
        if (IsThreadWriteAudioFrame) {
            ThreadWriteAudioFrame.waitForFinished();
            Continue=Continue && (bool)ThreadWriteAudioFrame.result();
        }

        // write the trailer
        if (Continue && !StopProcessWanted) {
            flush_ffmpeg_VideoStream(W,H);              // Flush delayed video frame
            QCoreApplication::processEvents();          // Give time to interface !

            // Flush audio frame
            if (AudioStream!=NULL) while ((Continue)&&(MixedMusic.List.count()>0)) {
                if (IsThreadWriteAudioFrame) {
                    ThreadWriteAudioFrame.waitForFinished();
                    Continue=Continue && (bool)ThreadWriteAudioFrame.result();
                }
                ThreadWriteAudioFrame=QtConcurrent::run(this,&DlgRenderVideo::WriteAudioFrame,MixedMusic.DetachFirstPacket(),MixedMusic.SoundPacketSize);
                IsThreadWriteAudioFrame=true;
                // Give time to interface !
                QCoreApplication::processEvents();
            }

            av_write_trailer(OutputFormatContext);      // Write de trailer
        }

        // Delete image buffer
        if (Image!=NULL) {
            delete Image;
            Image=NULL;
        }
        if (VideoFramePicture!=NULL) {
            if (VideoFramePicture->data[0]!=NULL) av_free(VideoFramePicture->data[0]);
            av_free(VideoFramePicture);
            VideoFramePicture=NULL;
        }
        if (PreviousFrame!=NULL) {
            delete PreviousFrame;
            PreviousFrame=NULL;
        }

        // Last information update
        RefreshDisplayControl();

        // Inform user of success
        if (Continue && !StopProcessWanted) QMessageBox::information(this,QCoreApplication::translate("DlgRenderVideo","Render video"),QCoreApplication::translate("DlgRenderVideo","Job completed succesfully !"));
        // Save Window size and position
        Diaporama->ApplicationConfig->DlgRenderVideoWSP->SaveWindowState(this);
        // Close the dialog box
        done(0);
    }
}

//============================================================================================
//============================================================================================
bool DlgRenderVideo::OpenDestFile(int VideoCodecIndex,int AudioCodecIndex) {
    //=====================================
    // Create the container
    //=====================================
    AVOutputFormat *Fmt=av_guess_format(Diaporama->VideoCodec.toLocal8Bit(),Diaporama->OutputFileName.toLocal8Bit(),NULL);
    if ((Fmt==NULL)||(Fmt->video_codec==CODEC_ID_NONE)) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error creating output format !");
        return false;
    }
    // Force codec ID
    Fmt->video_codec=(CodecID)VIDEOCODECDEF[VideoCodecIndex].Codec_id;
    Fmt->audio_codec=(CodecID)AUDIOCODECDEF[AudioCodecIndex].Codec_id;

    // allocate and format the output media context
    OutputFormatContext = avformat_alloc_context();
    if (!OutputFormatContext) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : Unable to allocate OutputFormatContext !");
        return false;
    }

    memcpy(OutputFormatContext->filename,Diaporama->OutputFileName.toLocal8Bit(),strlen(Diaporama->OutputFileName.toLocal8Bit())+1);
    OutputFormatContext->oformat  =Fmt;
    OutputFormatContext->timestamp=0;
    OutputFormatContext->bit_rate =Diaporama->AudioBitRate+Diaporama->VideoBitRate;      // Video bitrate+audio bitrate ?

    AVFormatParameters	fpOutFile;
    memset(&fpOutFile,0,sizeof(AVFormatParameters));

    // set the output parameters
    if (av_set_parameters(OutputFormatContext,&fpOutFile)<0) {
        av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Invalid output format parameters !");
        return false;
    }

    OutputFormatContext->preload   = (int)(0.5 * AV_TIME_BASE);
    OutputFormatContext->max_delay = (int)(0.7 * AV_TIME_BASE);

    if (!CreateVideoStream()) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error creating video stream !");
        return false;
    }

    if (!CreateAudioStream()) { // Audio stream must be create after video stream
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error creating audio stream !");
        return false;
    }

    // open the file for writing
    if (url_fopen(&OutputFormatContext->pb,Diaporama->OutputFileName.toLocal8Bit(),URL_WRONLY)<0) {
        av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error creating output video file !");
        return false;
    }

    // File is correctly open
    return true;
}

//============================================================================================
// Create the audio streams
//============================================================================================

bool DlgRenderVideo::CreateAudioStream() {
    AudioStream = av_new_stream(OutputFormatContext,0);
    if (AudioStream==NULL) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio stream !");
        av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
        return false;
    } else {
        AudioCodecContext=AudioStream->codec;
        // Fill stream with default values
        avcodec_get_context_defaults2(AudioCodecContext,CODEC_TYPE_AUDIO);

        //===== Open Audio encoder
        AudioCodec=avcodec_find_encoder((CodecID)AUDIOCODECDEF[AudioCodecIndex].Codec_id);
        if (!AudioCodec) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Audio codec not found !");
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            return false;
        }

        // Set standard custom values
        AudioCodecContext->codec_id=(CodecID)AUDIOCODECDEF[AudioCodecIndex].Codec_id;
        AudioCodecContext->codec_type    = AVMEDIA_TYPE_AUDIO;
        AudioCodecContext->sample_fmt    = SAMPLE_FMT_S16;
        AudioCodecContext->sample_rate   = Diaporama->AudioFrequency;
        AudioStream->stream_copy         = 1;

        // Set codec specific
        AudioCodecContext->bit_rate             = Diaporama->AudioBitRate*1000;
        AudioCodecContext->rc_max_rate          = Diaporama->AudioBitRate*1000;
        AudioCodecContext->rc_min_rate          = Diaporama->AudioBitRate*1000;
        AudioCodecContext->bit_rate_tolerance   = Diaporama->AudioBitRate*100;
        AudioCodecContext->rc_buffer_size       = 0;
        AudioCodecContext->channels             = 2;
        AudioCodecContext->channel_layout= CH_LAYOUT_STEREO_DOWNMIX;    //CH_LAYOUT_STEREO;

        // Ensure the container stream uses the same aspect ratio & frame rate
        AudioCodecContext->time_base    =(AVRational){1,AudioCodecContext->sample_rate};
        AudioStream->r_frame_rate       =VideoCodecContext->time_base;
        AudioStream->time_base          =AudioCodecContext->time_base;


        if (OutputFormatContext->oformat->flags & AVFMT_GLOBALHEADER) AudioCodecContext->flags|=CODEC_FLAG_GLOBAL_HEADER;

        switch (AudioCodecContext->codec_id) {
        case CODEC_ID_PCM_S16LE:
            //AudioCodecContext->codec_tag=MKTAG('s','o','w','t');
            break;
        case CODEC_ID_MP2:
        case CODEC_ID_MP3:
            //AudioCodecContext->codec_tag=MKTAG('.','m','p','3');
            if(AudioCodecContext->block_align==1) AudioCodecContext->block_align= 0;
            break;
        case CODEC_ID_AAC:
            //AudioCodecContext->profile  =FF_PROFILE_AAC_MAIN; // Not working with internal aac encoder
            //AudioCodecContext->codec_tag=MKTAG('m','p','4','a');
            break;
        case CODEC_ID_AC3:
            //AudioCodecContext->codec_tag    =MKTAG('m','s',0x20,0x00); /* Dolby AC-3 */
            AudioCodecContext->block_align  =0;
            AudioCodecContext->flags        |=AVFMT_NOTIMESTAMPS;
            AudioCodecContext->flags        |=CODEC_FLAG_GLOBAL_HEADER;
            break;
        case CODEC_ID_VORBIS:
            //AudioCodecContext->codec_tag=MKTAG('O','g','g','S');
            break;
        default:
            break;
        }

        // open the codec
        if (avcodec_open(AudioCodecContext,AudioCodec)<0) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"could not open audio codec !");
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            return false;
        }
    }
    return true;
}

//============================================================================================
// Create the video streams
//============================================================================================
bool DlgRenderVideo::CreateVideoStream() {
    VideoStream = av_new_stream(OutputFormatContext,0);
    if (VideoStream==NULL) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate video stream !");
        av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
        return false;
    } else {
        VideoCodecContext=VideoStream->codec;

        // Fill stream with default values
        avcodec_get_context_defaults(VideoCodecContext);
        VideoCodecContext->codec_id   = OutputFormatContext->oformat->video_codec;
        VideoCodecContext->codec_type = CODEC_TYPE_VIDEO;

        // Set image size and pixel aspect ratio (PAR)
        Extend                                      = DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Extend;
        VideoCodecContext->sample_aspect_ratio.num  = DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].PARNUM;
        VideoCodecContext->sample_aspect_ratio.den  = DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].PARDEN;
        VideoCodecContext->width                    = DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Width;
        VideoCodecContext->height                   = DefImageFormat[Diaporama->LastStandard][Diaporama->ImageGeometry][Diaporama->LastImageSize].Height+Extend+Extend;

        // Activate multithreading support if getCpuCount()>1
        if (getCpuCount()>1) VideoCodecContext->thread_count=getCpuCount();

        // Set frame rate
        if (Diaporama->VideoFrameRate==(double)24000/1001) {
            VideoCodecContext->time_base.den=24000;
            VideoCodecContext->time_base.num=1001;
            //av_set_pts_info(VideoStream,33,1001,24000);
        } else if (Diaporama->VideoFrameRate==25) {
            VideoCodecContext->time_base.den=25;
            VideoCodecContext->time_base.num=1;
            //av_set_pts_info(VideoStream,33,1,90000);
        } else if (Diaporama->VideoFrameRate==(double)30000/1001) {
            VideoCodecContext->time_base.den=30000;
            VideoCodecContext->time_base.num=1001;
            //av_set_pts_info(VideoStream,33,1001,30000);
        }

        // Ensure the container stream uses the same aspect ratio & frame rate
        VideoStream->sample_aspect_ratio=VideoCodecContext->sample_aspect_ratio;
        VideoStream->time_base          =VideoCodecContext->time_base;
        VideoStream->r_frame_rate       =VideoCodecContext->time_base;

        // Specific values depending on codec
        switch (VideoCodecContext->codec_id) {
        case CODEC_ID_MPEG2VIDEO :
            VideoCodecContext->bit_rate                 = Diaporama->VideoBitRate*1000;
            VideoCodecContext->codec_id                 = CODEC_ID_MPEG2VIDEO;
            VideoCodecContext->codec_tag                = MKTAG('M','P','E','G');
            VideoCodecContext->pix_fmt                  = PIX_FMT_YUV420P;
            VideoCodecContext->me_method                = ME_EPZS;                              // Motion estimation algorithm used for video coding.
            VideoCodecContext->qcompress                = 0.5;                                  // amount of qscale change between easy & hard scenes (0.0-1.0)
            VideoCodecContext->qblur                    = 0.5;                                  // amount of qscale smoothing over time (0.0-1.0)
            VideoCodecContext->qmin                     = 2;                                    // minimum quantizer (def:2)
            VideoCodecContext->qmax                     = 16;                                   // maximum quantizer (def:31)
            VideoCodecContext->max_qdiff                = 3;                                    // maximum quantizer difference between frames (def:3)
            VideoCodecContext->mpeg_quant               = 0;                                    // 0 -> h263 quant, 1 -> mpeg quant. (def:0)
            VideoCodecContext->strict_std_compliance    = -1;                                   // strictly follow the standard (MPEG4, ...)
            VideoCodecContext->luma_elim_threshold      = 0;                                    // luma single coefficient elimination threshold
            VideoCodecContext->chroma_elim_threshold    = 0;                                    // chroma single coeff elimination threshold
            VideoCodecContext->lumi_masking             = 0.0;
            VideoCodecContext->dark_masking             = 0.0;
            VideoCodecContext->gop_size                 = Diaporama->VideoFrameRate/2;          // Ref frame number : emit one intra frame every 1/2 sec
            VideoCodecContext->rc_buffer_size           = 1835000;                              // DVD standard !
            VideoCodecContext->bit_rate_tolerance       = VideoCodecContext->bit_rate/10;
            VideoCodecContext->rc_max_rate              = VideoCodecContext->bit_rate;
            VideoCodecContext->rc_min_rate              = VideoCodecContext->bit_rate;

            VideoCodecContext->max_b_frames             = 2;
            break;

        case CODEC_ID_MPEG4 :
            VideoCodecContext->bit_rate                 = Diaporama->VideoBitRate*1000;
            VideoCodecContext->codec_id                 = CODEC_ID_MPEG4;
            VideoCodecContext->codec_tag                = MKTAG('D','I','V','X');               // By default the fourcc is 'FMP4' but Windows Media Player doesn't recognize it. We'll force to 'XVID' fourcc. (similar as -vtag XVID) even if it wasn't the XviD codec that encoded the video :-(
            VideoCodecContext->pix_fmt                  = PIX_FMT_YUV420P;
            VideoCodecContext->me_method                = ME_EPZS;                              // Motion estimation algorithm used for video coding.
            VideoCodecContext->qcompress                = 0.5;                                  // amount of qscale change between easy & hard scenes (0.0-1.0)
            VideoCodecContext->qblur                    = 0.5;                                  // amount of qscale smoothing over time (0.0-1.0)
            VideoCodecContext->qmin                     = 2;                                    // minimum quantizer (def:2)
            VideoCodecContext->qmax                     = 16;                                   // maximum quantizer (def:31)
            VideoCodecContext->max_qdiff                = 3;                                    // maximum quantizer difference between frames (def:3)
            VideoCodecContext->mpeg_quant               = 0;                                    // 0 -> h263 quant, 1 -> mpeg quant. (def:0)
            VideoCodecContext->strict_std_compliance    = -1;                                   // strictly follow the standard (MPEG4, ...)
            VideoCodecContext->luma_elim_threshold      = 0;                                    // luma single coefficient elimination threshold
            VideoCodecContext->chroma_elim_threshold    = 0;                                    // chroma single coeff elimination threshold
            VideoCodecContext->lumi_masking             = 0.0;
            VideoCodecContext->dark_masking             = 0.0;
            VideoCodecContext->gop_size                 = Diaporama->VideoFrameRate/2;          // Ref frame number : emit one intra frame every 1/2 sec
            VideoCodecContext->rc_buffer_size           = 1835000;                              // DVD standard !
            VideoCodecContext->bit_rate_tolerance       = VideoCodecContext->bit_rate/10;
            VideoCodecContext->rc_max_rate              = VideoCodecContext->bit_rate;
            VideoCodecContext->rc_min_rate              = VideoCodecContext->bit_rate;
            break;

        case CODEC_ID_H264 :
            VideoCodecContext->codec_id                 = CODEC_ID_H264;
            VideoCodecContext->pix_fmt                  = PIX_FMT_YUV420P;
            VideoCodecContext->bit_rate                 = Diaporama->VideoBitRate*1000;
            VideoCodecContext->bit_rate_tolerance       = Diaporama->VideoBitRate*100;
            VideoCodecContext->rc_max_rate              = 0;
            VideoCodecContext->rc_buffer_size           = 0;

            VideoCodecContext->gop_size = 40;
            VideoCodecContext->max_b_frames = 3;
            VideoCodecContext->b_frame_strategy = 1;
            VideoCodecContext->coder_type = 1;
            VideoCodecContext->me_cmp = 1;
            VideoCodecContext->me_range = 16;
            VideoCodecContext->qmin = 10;
            VideoCodecContext->qmax = 51;
            VideoCodecContext->scenechange_threshold = 40;
            VideoCodecContext->flags |= CODEC_FLAG_LOOP_FILTER;
            VideoCodecContext->me_method = ME_HEX;
            VideoCodecContext->me_subpel_quality = 5;
            VideoCodecContext->i_quant_factor = 0.71;
            VideoCodecContext->qcompress = 0.6;
            VideoCodecContext->max_qdiff = 4;
            VideoCodecContext->directpred = 1;
            VideoCodecContext->flags2 |= CODEC_FLAG2_FASTPSKIP;

            /*
            VideoCodecContext->ticks_per_frame          = 2;
            VideoCodecContext->rc_buffer_size           = VideoCodecContext->bit_rate*2;        // vbv_buf_size
            VideoCodecContext->rc_min_vbv_overflow_use  = VideoCodecContext->bit_rate;          // vbv_maxrate ?
            VideoCodecContext->rc_max_available_vbv_use = VideoCodecContext->bit_rate;
            VideoCodecContext->bit_rate_tolerance       = VideoCodecContext->bit_rate/10;
            VideoCodecContext->rc_max_rate              = VideoCodecContext->bit_rate;
            VideoCodecContext->rc_min_rate              = VideoCodecContext->bit_rate;

            VideoCodecContext->codec_tag                = MKTAG('a','v','c','1');

            // values from HQ preset !
            VideoCodecContext->me_method                = ME_UMH;                               // Motion estimation algorithm used for video coding.
            VideoCodecContext->qcompress                = 0.6;                                  // amount of qscale change between easy & hard scenes (0.0-1.0)
            VideoCodecContext->me_range                 = 16;                                   // maximum motion estimation search range in subpel units
            VideoCodecContext->me_subpel_quality        = 8;                                    // subpel ME quality
                                                                                            //      - 1:Fastest, but extremely low quality
                                                                                            //      - 2-5:Progressively better and slower
                                                                                            //      - 6-7:6 is the default. Activates rate-distortion optimization for partition decision. This can considerably improve efficiency, though it has a notable speed cost. 6 activates it in I/P frames, and subme7 activates it in B frames.
                                                                                            //      - 8-9: Activates rate-distortion refinement, which uses RDO to refine both motion vectors and intra prediction modes. Slower than subme 6, but again, more efficient.
            VideoCodecContext->qmin                     = 10;                                   // minimum quantizer
            VideoCodecContext->qmax                     = 51;                                   // maximum quantizer
            VideoCodecContext->max_qdiff                = 4;                                    // maximum quantizer difference between frames
            VideoCodecContext->i_quant_factor           = 0.71;                                 // qscale factor between P and I-frames
            VideoCodecContext->b_frame_strategy         = 2;                                    // 0: Very fast 1: Fast, default mode in x264 2: A much slower but more accurate B-frame
            VideoCodecContext->max_b_frames             = 3;                                    // maximum number of B-frames between non-B-frames
            VideoCodecContext->refs                     = 4;                                    // number of reference frames
            VideoCodecContext->scenechange_threshold    = 40;                                   // scene change detection threshold
            VideoCodecContext->directpred               = 3;                                    // direct MV prediction mode - 0 (none), 1 (spatial), 2 (temporal), 3 (auto)
            VideoCodecContext->trellis                  = 1;                                    // trellis RD quantization
            VideoCodecContext->weighted_p_pred          = 2;                                    // explicit P-frame weighted prediction analysis method
            VideoCodecContext->me_cmp                   |= 1;                                   // cmp=+chroma, where CHROMA = 1
            VideoCodecContext->flags                    |=CODEC_FLAG_LOOP_FILTER;               // flags=+loop
            VideoCodecContext->coder_type               = 1;                                    // coder = 1
            VideoCodecContext->partitions               |=X264_PART_I8X8+X264_PART_I4X4+X264_PART_P8X8+X264_PART_B8X8;                          // partitions=+parti8x8+parti4x4+partp8x8+partb8x8
            VideoCodecContext->flags2                   |=CODEC_FLAG2_MIXED_REFS+CODEC_FLAG2_WPRED+CODEC_FLAG2_8X8DCT+CODEC_FLAG2_FASTPSKIP;    // flags2=+bpyramid+mixed_refs+wpred+dct8x8+fastpskip
            VideoCodecContext->keyint_min               = 25;                                   // keyint_min=25
            VideoCodecContext->gop_size                 = 250;                                  // Ref frame number : emit one intra frame every 1/4 sec
            */
/*            // libx264-medium.ffpreset preset
            VideoCodecContext->coder_type = 1;  // coder = 1
            VideoCodecContext->flags|=CODEC_FLAG_LOOP_FILTER;   // flags=+loop
            VideoCodecContext->me_cmp|= 1;  // cmp=+chroma, where CHROMA = 1
            VideoCodecContext->partitions|=X264_PART_I8X8+X264_PART_I4X4+X264_PART_P8X8+X264_PART_B8X8; // partitions=+parti8x8+parti4x4+partp8x8+partb8x8
            VideoCodecContext->me_method=ME_HEX;    // me_method=hex
            VideoCodecContext->me_subpel_quality = 7;   // subq=7
            VideoCodecContext->me_range = 16;   // me_range=16
            VideoCodecContext->gop_size = 250;  // g=250
            VideoCodecContext->keyint_min = 25; // keyint_min=25
            VideoCodecContext->scenechange_threshold = 40;  // sc_threshold=40
            VideoCodecContext->i_quant_factor = 0.71; // i_qfactor=0.71
            VideoCodecContext->b_frame_strategy = 1;  // b_strategy=1
            VideoCodecContext->qcompress = 0.6; // qcomp=0.6
            VideoCodecContext->qmin = 10;   // qmin=10
            VideoCodecContext->qmax = 51;   // qmax=51
            VideoCodecContext->max_qdiff = 4;   // qdiff=4
            VideoCodecContext->max_b_frames = 3;    // bf=3
            VideoCodecContext->refs = 3;    // refs=3
            VideoCodecContext->directpred = 1;  // directpred=1
            VideoCodecContext->trellis = 1; // trellis=1
            VideoCodecContext->flags2|=CODEC_FLAG2_BPYRAMID+CODEC_FLAG2_MIXED_REFS+CODEC_FLAG2_WPRED+CODEC_FLAG2_8X8DCT+CODEC_FLAG2_FASTPSKIP;  // flags2=+bpyramid+mixed_refs+wpred+dct8x8+fastpskip
            VideoCodecContext->weighted_p_pred = 2; // wpredp=2
            // libx264-main.ffpreset preset
            VideoCodecContext->flags2|=CODEC_FLAG2_8X8DCT;
            //VideoCodecContext->flags2^=CODEC_FLAG2_8X8DCT;    // flags2=-dct8x8
*/
            break;

        case CODEC_ID_MJPEG:
            VideoCodecContext->bit_rate                 = Diaporama->VideoBitRate*1000;
            VideoCodecContext->codec_tag                = MKTAG('m','j','p','b');
            VideoCodecContext->mb_lmin                  = VideoCodecContext->qmin*FF_QP2LAMBDA;
            VideoCodecContext->lmin                     = VideoCodecContext->qmin*FF_QP2LAMBDA;
            VideoCodecContext->mb_lmax                  = VideoCodecContext->qmax*FF_QP2LAMBDA;
            VideoCodecContext->lmax                     = VideoCodecContext->qmax*FF_QP2LAMBDA;
            VideoCodecContext->global_quality           = VideoCodecContext->qmin*FF_QP2LAMBDA;
            VideoCodecContext->flags                    |= CODEC_FLAG_QSCALE;
            VideoCodecContext->pix_fmt                  = PIX_FMT_YUVJ420P;
            VideoCodecContext->gop_size                 = 1000;                                     // Ref frame number : emit one intra frame every 1 sec
            VideoCodecContext->rc_buffer_size           = 1835000;                                  // DVD standard !
            VideoCodecContext->bit_rate_tolerance       = VideoCodecContext->bit_rate/10;
            VideoCodecContext->rc_max_rate              = VideoCodecContext->bit_rate;
            VideoCodecContext->rc_min_rate              = VideoCodecContext->bit_rate;
            break;

        default:
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Unknown codec");
            return false;
            break;
        }

        // Some formats want stream headers to be separate
        if (OutputFormatContext->oformat->flags & AVFMT_GLOBALHEADER) VideoCodecContext->flags|=CODEC_FLAG_GLOBAL_HEADER;

        //===== Open Video encoder

        VideoCodec=avcodec_find_encoder(VideoCodecContext->codec_id);
        if (!VideoCodec) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Video codec not found !");
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            return false;
        }
        // open the codec
        if (avcodec_open(VideoCodecContext,VideoCodec)<0) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"could not open video codec !");
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            return false;
        }
    }
    return true;
}

//============================================================================================
// Convert QImage to a YUV AVFrame
//============================================================================================
AVFrame *DlgRenderVideo::QImageToYUVStream(QImage *Image) {
    // Calc destination size
    int W=Image->width();
    int H=Image->height();

    AVFrame *InputFrame     =NULL;
    int     SizeInputBuf    =0;
    uint8_t *InputFrameBuf  =NULL;

    AVFrame *OutputFrame    =NULL;
    int     SizeOutputBuf   =0;
    uint8_t *OutputFrameBuf =NULL;

    bool    IsOk            = true;

    // Allocate the input frame that we will fill up with the bitmap.
    InputFrame=avcodec_alloc_frame();
    if (InputFrame!=NULL) {
        // Allocate the buffer holding actual frame data.
        SizeInputBuf = avpicture_get_size(PIX_FMT_BGR24,Image->width(),Image->height());
        InputFrameBuf= (uint8_t*)av_malloc(SizeInputBuf);
        if (InputFrameBuf!=NULL) {
            // Setting up various pointers between the buffers.
            avpicture_fill((AVPicture *)InputFrame,InputFrameBuf,PIX_FMT_BGRA,W,H);
            // Associate the Bitmap to the AVFrame
            InputFrame->data[0]    =(uint8_t*)Image->bits();
            InputFrame->linesize[0]=W*4;
        } else {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Input frame buffer not allocated !");
            av_free(InputFrame);
        }
    } else QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Input frame not allocated !");

    // if error at this point : end function
    if (InputFrame==NULL) return NULL;

    // Allocate the buffer holding future frame data
    OutputFrame = avcodec_alloc_frame();
    if (OutputFrame!=NULL) {
        if (VideoCodecContext->codec_id==CODEC_ID_MJPEG) OutputFrame->quality=VideoCodecContext->global_quality;

        SizeOutputBuf =avpicture_get_size(VideoCodecContext->pix_fmt,W,H);
        OutputFrameBuf=(uint8_t*)av_malloc(SizeOutputBuf);
        if (OutputFrameBuf!=NULL) {
            // Setting up various pointers between the buffers.
            avpicture_fill((AVPicture *)OutputFrame,OutputFrameBuf,VideoCodecContext->pix_fmt,W,H);
            // Create a scaling context for conversion
            SwsContext *scalingContext=sws_getContext(W,H,PIX_FMT_BGRA,W,H,VideoCodecContext->pix_fmt,SWS_BICUBIC,NULL,NULL,NULL);
            if (scalingContext!=NULL) {
                // Convert image
                if (sws_scale(scalingContext,InputFrame->data,InputFrame->linesize,0,H,OutputFrame->data,OutputFrame->linesize)<0)  {
                    QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"scaling failed !");
                    IsOk=false;
                }
                sws_freeContext(scalingContext);
            } else {
                QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error creating scaling context !");
                IsOk=false;
            }
        } else {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Output frame buffer not allocated !");
            av_free(OutputFrame);   OutputFrame=NULL;
        }
    } else QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Output frame not allocated !");

    // Cleanup input buffer (no longer need it)
    av_free(InputFrameBuf); InputFrameBuf=NULL;
    av_free(InputFrame);    InputFrame   =NULL;

    // Cleanup output buffer if error occur
    if (!IsOk) {
        if (OutputFrameBuf!=NULL) {
            av_free(OutputFrameBuf);
            OutputFrameBuf=NULL;
        }
        if (OutputFrame!=NULL) {
            av_free(OutputFrame);
            OutputFrame=NULL;
        }
    }

    return OutputFrame;
}

//============================================================================================
//============================================================================================
bool DlgRenderVideo::WriteAudioFrame(int16_t *Packet,int AudioLen) {
    //uint8_t         *Buffer=(uint8_t *)Packet;
    int64_t         AVNOPTSVALUE=0x8000000000000000; // to solve type error with Qt
    bool            IsOk=true;
    AVPacket        pkt;
    int             audio_outbuf_size=FF_MIN_BUFFER_SIZE; //AudioLen
    uint8_t         *audio_outbuf=(uint8_t *)av_malloc(audio_outbuf_size);

    if (audio_outbuf==NULL) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate audio buffer !");
        return false;
    }
    EncodedAudio.AppendData(Packet,AudioLen);
    while (EncodedAudio.List.count()>0) {
        int16_t *PacketSound=EncodedAudio.DetachFirstPacket();
        int out_size= avcodec_encode_audio(AudioCodecContext,audio_outbuf,EncodedAudio.SoundPacketSize,(short int *)PacketSound);
        if (out_size>0) {
            av_init_packet(&pkt);

            if ((AudioCodecContext->coded_frame!=NULL)&&(AudioCodecContext->coded_frame->pts!=AVNOPTSVALUE))
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
                IsOk=false;
            }
        } else if (out_size<0) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error encoding sound !");
            IsOk=false;
        }
        av_free(PacketSound);
    }
    // Cleanup buffer
    av_free(audio_outbuf);

    av_free(Packet);

    return IsOk;
}

//============================================================================================
// Write video frame
//============================================================================================
bool DlgRenderVideo::WriteVideoFrame(int FrameNumber,AVFrame *VideoFramePicture,int Width,int Height) {
    int64_t     AVNOPTSVALUE=0x8000000000000000; // to solve type error with Qt
    bool        IsOk=true;
    AVPacket    pkt;
    int         video_outbuf_size =4*Width*Height;  if (video_outbuf_size<FF_MIN_BUFFER_SIZE) video_outbuf_size=FF_MIN_BUFFER_SIZE;
    uint8_t     *video_outbuf     =(uint8_t*)av_malloc(video_outbuf_size);

    if (video_outbuf==NULL) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate video buffer !");
        return false;
    }

    if (VideoFramePicture==NULL) return false;

    memset(video_outbuf,0,video_outbuf_size);

    // encode the image
    VideoFramePicture->pts      =FrameNumber;
    VideoFramePicture->quality  =VideoCodecContext->global_quality;
    int out_size=avcodec_encode_video(VideoCodecContext,video_outbuf,video_outbuf_size,VideoFramePicture);

    if (out_size>0) {
        av_init_packet(&pkt);

        if ((VideoCodecContext->coded_frame!=NULL)&&(VideoCodecContext->coded_frame->pts!=AVNOPTSVALUE))
            pkt.pts=av_rescale_q(VideoCodecContext->coded_frame->pts,VideoCodecContext->time_base,VideoStream->time_base);

        if ((VideoCodecContext->coded_frame!=NULL)&&(VideoCodecContext->coded_frame->key_frame)) pkt.flags|=AV_PKT_FLAG_KEY;

        pkt.stream_index=VideoStream->index;
        pkt.data        =video_outbuf;
        pkt.size        =out_size;

        // write the compressed frame in the media file
        if (av_interleaved_write_frame(OutputFormatContext,&pkt)!=0) {
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error while writing video frame !");
            IsOk=false;
        } else RenderedFrame++;

    } else if (out_size<0) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error encoding image !");
        IsOk=false;
    }

    // Cleanup buffer
    av_free(video_outbuf);

    return IsOk;
}

//============================================================================================
// Write video frame previously delayed
//============================================================================================
void DlgRenderVideo::flush_ffmpeg_VideoStream(int Width,int Height) {
    int64_t     AVNOPTSVALUE=0x8000000000000000; // to solve type error with Qt
    int         video_outbuf_size =4*Width*Height;  if (video_outbuf_size<FF_MIN_BUFFER_SIZE) video_outbuf_size=FF_MIN_BUFFER_SIZE;
    uint8_t     *video_outbuf     =(uint8_t*)av_malloc(video_outbuf_size);
    AVPacket    pkt;

    if (video_outbuf==NULL) {
        QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Memory error : could not allocate video buffer !");
        return;
    }

    memset(video_outbuf,0,video_outbuf_size);

    // get the delayed frames
    while (1) {
        AVPacket packet;
        av_init_packet(&packet);

        int out_size = avcodec_encode_video(VideoCodecContext,video_outbuf,video_outbuf_size,NULL);
        if (out_size < 0) {
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error encoding delayed frame !");
            break;
        }
        if (out_size==0) break;

        av_init_packet(&pkt);

        if ((VideoCodecContext->coded_frame!=NULL)&&(VideoCodecContext->coded_frame->pts!=AVNOPTSVALUE))
            pkt.pts=av_rescale_q(VideoCodecContext->coded_frame->pts,VideoCodecContext->time_base,VideoStream->time_base);

        if ((VideoCodecContext->coded_frame!=NULL)&&(VideoCodecContext->coded_frame->key_frame)) pkt.flags|=AV_PKT_FLAG_KEY;

        pkt.stream_index=VideoStream->index;
        pkt.data        =video_outbuf;
        pkt.size        =out_size;

        // write the compressed frame in the media file
        if (av_interleaved_write_frame(OutputFormatContext,&pkt)!=0) {
            av_log(OutputFormatContext,AV_LOG_DEBUG,"AVLOG:");
            QMessageBox::critical(this,QCoreApplication::translate("DlgRenderVideo","Render video"),"Error while writing video frame !");
        } else {
            RenderedFrame++;
            RefreshDisplayControl();            // Refresh Display
            QCoreApplication::processEvents();  // Give time to interface !
        }
    }
    avcodec_flush_buffers(VideoCodecContext);
    // Cleanup buffer
    av_free(video_outbuf);
}

//============================================
// Refresh Display controls
//============================================
void DlgRenderVideo::RefreshDisplayControl() {
    int         DurationProcess =0;
    QString     DisplayText     ="";

    DurationProcess=StartTime.msecsTo(QTime::currentTime());
    DisplayText=QString("%1").arg((QTime(0,0,0,0).addMSecs(DurationProcess)).toString("hh:mm:ss"));     ui->ElapsedTimeLabel->setText(DisplayText);
    DisplayText=QString("%1").arg(double(RenderedFrame)/(double(DurationProcess)/1000),0,'f',1);        ui->FPSLabel->setText(DisplayText);
    DisplayText=QString("%1/%2").arg(Column+1).arg(Diaporama->List.count());                            ui->SlideNumberLabel->setText(DisplayText);
    DisplayText=QString("%1/%2").arg(RenderedFrame).arg(NbrFrame);                                      ui->FrameNumberLabel->setText(DisplayText);
    ui->SlideProgressBar->setValue(int(double(Position-ColumnStart)/(FPS/double(1000))));
    ui->TotalProgressBar->setValue(RenderedFrame);
    LastCheckTime=QTime::currentTime();
}
