/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

#include "_EncodeVideo.h"

cEncodeVideo::cEncodeVideo(cDiaporama *Diaporama) {
    this->Diaporama =Diaporama;
    FormatDef       =NULL;
    VideoCodecDef   =NULL;
    AudioCodecDef   =NULL;
    Container       =NULL;
    VideoCodec      =NULL;
    AudioCodec      =NULL;
    VideoStream     =NULL;
    AudioStream     =NULL;
    m_pkt           =NULL;
    m_videoOutBuf   =NULL;
    Picture         =NULL;
    TmpPicture      =NULL;
    IsOpen          =false;

    // link to control for progression display
    ElapsedTimeLabel=NULL;
    SlideNumberLabel=NULL;
    FrameNumberLabel=NULL;
    FPSLabel        =NULL;
    SlideProgressBar=NULL;
    TotalProgressBar=NULL;
}

//*************************************************************************************************************************************************

cEncodeVideo::~cEncodeVideo() {
    CloseEncoder();
}

//*************************************************************************************************************************************************

void cEncodeVideo::CloseEncoder() {
    if (Container) {
        if (IsOpen) {
            if ((AudioStream)&&(AudioStream->codec->codec_id==CODEC_ID_FLAC)) {
                // Finalize with NULL input needed by FLAC to generate md5sum in context extradata
                AVPacket pkt;
                int got_packet;
                av_init_packet(&pkt);
                pkt.data=NULL;
                pkt.size=0;
                pkt.pts =CurAudioPts++;
                pkt.dts =AV_NOPTS_VALUE;
                avcodec_encode_audio2(AudioStream->codec,&pkt,NULL,&got_packet);
                avcodec_flush_buffers(AudioStream->codec);
            }
            av_write_trailer(Container);
            avio_close(Container->pb);
        }
        for (unsigned int i=0;i<Container->nb_streams;i++) {
            if (VideoStream==Container->streams[i]) VideoStream=NULL;
            if (AudioStream==Container->streams[i]) AudioStream=NULL;
            av_freep(&Container->streams[i]);
        }
        av_free(Container);
        Container=NULL;
    }
    if (VideoStream) {
        av_freep(VideoStream);
        VideoStream=NULL;
    }
    if (AudioStream) {
        av_freep(AudioStream);
        AudioStream=NULL;
    }
    if (m_pkt) {
        av_free_packet(m_pkt);
        delete m_pkt;
        m_pkt = NULL;
    }
    if (m_videoOutBuf) {
        av_free(m_videoOutBuf);
        m_videoOutBuf=NULL;
    }
    if (Picture)     {
        av_free(Picture);
        Picture=NULL;
    }

    if (TmpPicture) {
        av_free(TmpPicture);
        TmpPicture=NULL;
    }
}

//*************************************************************************************************************************************************

bool cEncodeVideo::OpenEncoder(QString OutputFileName,int FromSlide,int ToSlide,
                    bool EncodeVideo,int VideoCodecId,double VideoFrameRate,int ImageWidth,int ImageHeight,enum PixelFormat PixelFmt,int VideoBitrate,int KeyFrameDist,
                    bool EncodeAudio,int AudioCodecId,int m_audioChannels,int m_audioBitrate,int m_audioSampleRate,QString Language) {

    this->OutputFileName=OutputFileName;
    this->VideoFrameRate=VideoFrameRate;
    this->FromSlide     =FromSlide;
    this->ToSlide       =ToSlide;
    NbrFrame            =int(double(Diaporama->GetPartialDuration(FromSlide,ToSlide))*VideoFrameRate/1000);    // Number of frame to generate
    int StreamNbr       =0;

    // Search container format from OutputFileName
    QString FMT=QFileInfo(OutputFileName).suffix();

    // Search FMT from FROMATDEF
    int i=0;
    while ((i<NBR_FORMATDEF)&&(QString(FORMATDEF[i].FileExtension)!=FMT)) i++;

    // if FMT not found, search it from AUDIOFORMATDEF
    if (i>=NBR_FORMATDEF) {
        int i=0;
        while ((i<NBR_AUDIOFORMATDEF)&&(QString(AUDIOFORMATDEF[i].FileExtension)!=FMT)) i++;
        if (i>=NBR_AUDIOFORMATDEF) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: Unknown format %1").arg(FMT));
            return false;
        } else FormatDef=&AUDIOFORMATDEF[i];
    } else FormatDef=&FORMATDEF[i];

    // Search Video codec from VideoCodecId
    if (EncodeVideo) {
        VideoStreamNbr=StreamNbr++;
        int i=0;
        while ((i<NBR_VIDEOCODECDEF)&&(VIDEOCODECDEF[i].Codec_id!=VideoCodecId)) i++;
        if ((i>=NBR_VIDEOCODECDEF)||(!VIDEOCODECDEF[i].IsFind)) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: video codec not available"));
            return false;
        }
        VideoCodecDef=&VIDEOCODECDEF[i];
        //=========> TODO : Check parameters !
    }

    // Search Audio codec from VideoCodecId
    if (EncodeAudio) {
        AudioStreamNbr=StreamNbr++;
        int i=0;
        while ((i<NBR_AUDIOCODECDEF)&&(AUDIOCODECDEF[i].Codec_id!=AudioCodecId)) i++;
        if ((i>=NBR_AUDIOCODECDEF)||(!AUDIOCODECDEF[i].IsFind)) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: audio codec not available"));
            return false;
        }
        AudioCodecDef=&AUDIOCODECDEF[i];
        //=========> TODO : Check parameters !
    }

    //=======================================
    // Prepare container
    //=======================================

    // Prepare AVOutputFormat struct
    AVOutputFormat *fmt=av_guess_format(QString(FormatDef->ShortName).toLocal8Bit().constData(),NULL,NULL);
    if (!fmt) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: Unable to guess AVOutputFormat from container %1").arg(QString(FormatDef->ShortName)));
        return false;
    }
    OutputFormat=*fmt;
    if (OutputFormat.flags & AVFMT_NOFILE) return false;

    // Check if video codec is available
    if (VideoCodecDef) {
        VideoStreamNbr=StreamNbr++;
        VideoCodec=avcodec_find_encoder_by_name(VideoCodecDef->ShortName);
        if (!VideoCodec) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: Unable to find video codec %1").arg(VideoCodecId));
            return false;
        } else if (VideoCodec->id==CODEC_ID_NONE) return false;
        OutputFormat.video_codec=VideoCodec->id;
    }

    // Check if audio codec is available
    if (AudioCodecDef) {
        AudioStreamNbr=StreamNbr++;
        AudioCodec=avcodec_find_encoder_by_name(AudioCodecDef->ShortName);
        if (!AudioCodec) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: Unable to find audio codec %1").arg(AudioCodecId));
            return false;
        } else if (AudioCodec->id==CODEC_ID_NONE) return false;
        OutputFormat.audio_codec=AudioCodec->id;
    }

    //=======================================
    // Prepare streams
    //=======================================

    // Alloc container
    Container=avformat_alloc_context();
    if (!Container) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenEncoder: Unable to allocate AVFormatContext");
        return false;
    }
    Container->oformat=&OutputFormat;
    //if (m_container == "mpegts") Container->packet_size = 2324;
    snprintf(Container->filename,sizeof(Container->filename),"%s",OutputFileName.toLocal8Bit().constData());

    if ((VideoCodecDef)&&(!OpenVideoStream(VideoFrameRate,ImageWidth,ImageHeight,PixelFmt,VideoBitrate,KeyFrameDist))) return false;
    if ((AudioCodecDef)&&(!OpenAudioStream(m_audioChannels,m_audioBitrate,m_audioSampleRate,Language))) return false;
    if (!PrepareTAG(Language)) return false;

    //********************************************
    // Open file and header
    //********************************************

    if (avio_open(&Container->pb,OutputFileName.toUtf8().constData(),AVIO_FLAG_WRITE)<0) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenEncoder: avio_open() failed");
        return false;
    }
    avformat_write_header(Container,NULL);
    IsOpen=true;

    //********************************************
    // Log output format
    //********************************************
    av_dump_format(Container,0,OutputFileName.toLocal8Bit().constData(),1);

    return true;
}


//*************************************************************************************************************************************************

bool cEncodeVideo::OpenVideoStream(double VideoFrameRate,int ImageWidth,int ImageHeight,enum PixelFormat PixelFmt,int VideoBitrate,int KeyFrameDist) {
    // Create video streams
    VideoStream=avformat_new_stream(Container,NULL);
    if (!VideoStream) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: avformat_new_stream() failed");
        return false;
    }

    AVCodec *codec=avcodec_find_encoder(Container->oformat->video_codec);
    if (!codec) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: avcodec_find_encoder() failed");
        return false;
    }
    avcodec_get_context_defaults3(VideoStream->codec,codec);
    VideoStream->id                               = VideoStreamNbr;
    VideoStream->codec->codec                     = codec;
    VideoStream->codec->codec_id                  = Container->oformat->video_codec;
    VideoStream->codec->codec_type                = AVMEDIA_TYPE_VIDEO;
    VideoStream->codec->bit_rate                  = VideoBitrate;
    VideoStream->codec->width                     = ImageWidth;
    VideoStream->codec->height                    = ImageHeight;
    // VideoStream->codec->sample_aspect_ratio.num= (int)floor(m_aspect * 10000);
    // VideoStream->codec->sample_aspect_ratio.den = 10000;
    VideoStream->codec->time_base                 = GetCodecTimeBase(VideoStream->codec->codec,VideoFrameRate);
    //VideoStream->time_base.den                    = 90000;
    //VideoStream->time_base.num                    = 1;
    VideoStream->r_frame_rate.num                 = 0;
    VideoStream->r_frame_rate.den                 = 0;
    VideoStream->codec->gop_size                  = KeyFrameDist;
    VideoStream->codec->pix_fmt                   = PIX_FMT_YUV420P;
    VideoStream->codec->thread_count              = ((getCpuCount()-1)>1)?(getCpuCount()-1):1;
    VideoStream->codec->thread_type               = FF_THREAD_SLICE;

    if (VideoStream->codec->codec_id==CODEC_ID_MPEG2VIDEO) {
        VideoStream->codec->max_b_frames=2;
    } else if (VideoStream->codec->codec_id==CODEC_ID_MPEG1VIDEO) {
        VideoStream->codec->mb_decision =2;
    } else if (VideoStream->codec->codec_id==CODEC_ID_H264) {
        if ((VideoStream->codec->width > 480)||(VideoStream->codec->bit_rate > 600000)) {
            VideoStream->codec->level = 31;
            av_opt_set(VideoStream->codec->priv_data,"profile","main",0);
        } else {
            VideoStream->codec->level = 30;
            av_opt_set(VideoStream->codec->priv_data,"profile","baseline",0);
        }

        VideoStream->codec->coder_type            = 0;
        VideoStream->codec->max_b_frames          = 0;
        VideoStream->codec->slices                = 8;
        VideoStream->codec->flags                |= CODEC_FLAG_LOOP_FILTER;
        VideoStream->codec->me_cmp               |= 1;
        VideoStream->codec->me_method             = ME_HEX;
        VideoStream->codec->me_subpel_quality     = 6;
        VideoStream->codec->me_range              = 16;
        VideoStream->codec->keyint_min            = 25;
        VideoStream->codec->scenechange_threshold = 40;
        VideoStream->codec->i_quant_factor        = 0.71;
        VideoStream->codec->b_frame_strategy      = 1;
        VideoStream->codec->qcompress             = 0.6;
        VideoStream->codec->qmin                  = 10;
        VideoStream->codec->qmax                  = 51;
        VideoStream->codec->max_qdiff             = 4;
        VideoStream->codec->refs                  = 3;
        VideoStream->codec->trellis               = 0;
        av_opt_set(VideoStream->codec,"partitions","i8x8,i4x4,p8x8,b8x8",0);
        av_opt_set_int(VideoStream->codec,"direct-pred",  1,0);
        av_opt_set_int(VideoStream->codec,"rc-lookahead", 0,0);
        av_opt_set_int(VideoStream->codec,"fast-pskip",   1,0);
        av_opt_set_int(VideoStream->codec,"mixed-refs",   1,0);
        av_opt_set_int(VideoStream->codec,"8x8dct",       0,0);
        av_opt_set_int(VideoStream->codec,"weightb",      0,0);
    }
    if (Container->oformat->flags&AVFMT_GLOBALHEADER) VideoStream->codec->flags|=CODEC_FLAG_GLOBAL_HEADER;

    // Alloc AVPacket for video
    m_pkt=new AVPacket;
    if (!m_pkt) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: error allocating AVPacket for video");
        return false;
    }
    av_new_packet(m_pkt,Container->packet_size);

    // Open video codec
    m_videoOutBuf = (unsigned char *)av_malloc(ImageWidth*ImageHeight*2+10);
    if (avcodec_open2(VideoStream->codec,NULL,NULL)<0) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-Init: avcodec_open() failed for video");
        return false;
    }

    // Allocate picture
    Picture=avcodec_alloc_frame();
    if (!Picture) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-Init: avcodec_alloc_frame() failed for picture");
        return false;
    }
    unsigned char *picture_buf = (unsigned char *)av_malloc(avpicture_get_size(PixelFmt,ImageWidth,ImageHeight));
    if (!picture_buf) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-Init: av_malloc() failed for picture");
        return false;
    }
    avpicture_fill((AVPicture *)Picture,picture_buf,PixelFmt,ImageWidth,ImageHeight);

    // Allocate TmpPicture (if needed)
    if (VideoStream->codec->pix_fmt!=PIX_FMT_YUV420P) {
        TmpPicture=avcodec_alloc_frame();
        if (!TmpPicture) {
            ToLog(LOGMSG_CRITICAL,"EncodeVideo-Init: avcodec_alloc_frame() failed for tmpPicture");
            return false;
        }
        unsigned char *picture_buf = (unsigned char *)av_malloc(avpicture_get_size(PixelFmt,ImageWidth,ImageHeight));
        if (!picture_buf) {
            ToLog(LOGMSG_CRITICAL,"EncodeVideo-Init: av_malloc() failed for tmpPicture");
            return false;
        }
        avpicture_fill((AVPicture *)TmpPicture, picture_buf,PixelFmt, ImageWidth, ImageHeight);
    }
    return true;
}

//*************************************************************************************************************************************************

bool cEncodeVideo::OpenAudioStream(int m_audioChannels,int m_audioBitrate,int m_audioSampleRate,QString Language) {
    // Open codec
    AudioStream=avformat_new_stream(Container,NULL);
    if (!AudioStream) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenAudioStream: avformat_new_stream() failed");
        return false;
    }

    AudioStream->id                       = AudioStreamNbr;
    AudioStream->codec->codec_id          = Container->oformat->audio_codec;
    AudioStream->codec->codec_type        = AVMEDIA_TYPE_AUDIO;
    AudioStream->codec->bit_rate          = m_audioBitrate;
    AudioStream->codec->sample_fmt        = AV_SAMPLE_FMT_S16;
    AudioStream->codec->channels          = m_audioChannels;
    AudioStream->codec->sample_rate       = m_audioSampleRate;
    AudioStream->codec->time_base         = (AVRational){1,m_audioSampleRate};

    //AudioStream->codec->rc_min_rate       = m_audioBitrate;
    //AudioStream->codec->rc_max_rate       = m_audioBitrate;
    //AudioStream->codec->bit_rate_tolerance= 0;
    //AudioStream->codec->rc_buffer_size    = 0;
    //AudioStream->codec->flags |= CODEC_FLAG_QSCALE; // VBR
    //AudioStream->codec->global_quality = blah;

    if ((Container->oformat->flags & AVFMT_GLOBALHEADER)||(!strcmp(Container->oformat->name,"mp4"))||(!strcmp(Container->oformat->name,"mov"))||(!strcmp(Container->oformat->name,"3gp")))
        AudioStream->codec->flags|=CODEC_FLAG_GLOBAL_HEADER;

    switch (AudioStream->codec->codec_id) {
        case CODEC_ID_MP3:
            #ifdef USELIBAVRESAMPLE
            AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_S16P;
            #endif
            break;
        case CODEC_ID_VORBIS:
            #ifdef USELIBAVRESAMPLE
            AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLTP;
            AudioStream->codec->flags|=CODEC_FLAG_QSCALE;
            #endif
            break;
        case CODEC_ID_AC3:
            #ifdef USELIBAVRESAMPLE
            AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLTP;                     break;
            #else
            AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLT;
            #endif
            break;
        case CODEC_ID_AMR_NB:
        case CODEC_ID_AMR_WB:
            AudioStream->codec->channels=1;
            break;
        default:                                                                                        break;
    }
    #ifdef USELIBAVRESAMPLE
        AudioStream->codec->channel_layout= av_get_default_channel_layout(AudioStream->codec->channels);
    #else
        AudioStream->codec->channel_layout= AudioStream->codec->channels==2?AV_CH_LAYOUT_STEREO:AV_CH_LAYOUT_MONO;
    #endif

    //AudioStream->codec->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    AVCodec *codec=avcodec_find_encoder(AudioStream->codec->codec_id);
    if (!codec) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenAudioStream: avcodec_find_encoder() failed");
        return false;
    } else {
        int errcode=avcodec_open2(AudioStream->codec,codec,NULL);
        if (errcode<0) {
            char Buf[2048];
            av_strerror(errcode,Buf,2048);
            ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenAudioStream: avcodec_open2() failed: "+QString(Buf));
            return false;
        }
        av_dict_set(&AudioStream->metadata,"language",Language.toLocal8Bit().constData(),0);
    }
    return true;
}

//*************************************************************************************************************************************************

bool cEncodeVideo::PrepareTAG(QString Language) {
    // Set TAGS
    av_dict_set(&Container->metadata,"language",Language.toLocal8Bit().constData(),0);
    av_dict_set(&Container->metadata,"title",AdjustMETA(Diaporama->ProjectInfo->Title==""?QFileInfo(OutputFileName).baseName():Diaporama->ProjectInfo->Title).toLocal8Bit().constData(),0);
    av_dict_set(&Container->metadata,"artist",AdjustMETA(Diaporama->ProjectInfo->Author).toLocal8Bit().constData(),0);
    av_dict_set(&Container->metadata,"album",AdjustMETA(Diaporama->ProjectInfo->Album).toLocal8Bit().constData(),0);
    av_dict_set(&Container->metadata,"comment",AdjustMETA(Diaporama->ProjectInfo->Comment).toLocal8Bit().constData(),0);
    av_dict_set(&Container->metadata,"date",QString("%1").arg(Diaporama->ProjectInfo->Year).toLocal8Bit().constData(),0);
    av_dict_set(&Container->metadata,"composer",QString(Diaporama->ApplicationConfig->ApplicationName+QString(" ")+Diaporama->ApplicationConfig->ApplicationVersion).toLocal8Bit().constData(),0);
    av_dict_set(&Container->metadata,"creation_time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLocal8Bit().constData(),0); // ISO 8601 format

    //===========> TODO: Add chapters !

    return true;
}

//*************************************************************************************************************************************************

void cEncodeVideo::DisplayProgress(qlonglong RenderedFrame,qlonglong Position,int Column,int ColumnStart) {
    int     DurationProcess =StartTime.msecsTo(QTime::currentTime());
    double  CalcFPS         =(double(RenderedFrame)/(double(DurationProcess)/1000));
    double  EstimDur        =double(NbrFrame-RenderedFrame)/CalcFPS;

    if (ElapsedTimeLabel)   ElapsedTimeLabel->setText(QString("%1").arg((QTime(0,0,0,0).addMSecs(DurationProcess)).toString("hh:mm:ss"))+
                                                    QApplication::translate("DlgRenderVideo"," - Estimated time left : ")+
                                                    QString("%1").arg(QTime(0,0,0,0).addMSecs(EstimDur*1000).toString("hh:mm:ss")));
    if (FPSLabel)           FPSLabel->setText(QString("%1").arg(double(RenderedFrame)/(double(DurationProcess)/1000),0,'f',1));
    if (SlideNumberLabel)   SlideNumberLabel->setText(QString("%1/%2").arg(Column-FromSlide+1).arg(ToSlide-FromSlide+1));
    if (FrameNumberLabel)   FrameNumberLabel->setText(QString("%1/%2").arg(RenderedFrame).arg(NbrFrame));
    if (SlideProgressBar)   SlideProgressBar->setValue(int(double(Position-ColumnStart)/(double(AV_TIME_BASE)/VideoFrameRate/double(1000))));
    if (TotalProgressBar)   TotalProgressBar->setValue(RenderedFrame);

    LastCheckTime=QTime::currentTime();
    QApplication::processEvents();  // Give time to interface!
}

//*************************************************************************************************************************************************

QString cEncodeVideo::AdjustMETA(QString Text) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::AdjustMETA");

    //Metadata keys or values containing special characters (’=’, ’;’, ’#’, ’\’ and a newline) must be escaped with a backslash ’\’.
    Text.replace("=","\\=");
    Text.replace(";","\\;");
    Text.replace("#","\\#");
    //Text.replace("\\","\\\\");
    Text.replace("\n","\\\n");
    #ifdef Q_OS_WIN
        return Text.toUtf8();
    #else
        return Text;
    #endif
}

//*************************************************************************************************************************************************

AVRational cEncodeVideo::GetCodecTimeBase(const AVCodec *VideoCodec,double FPS) {
    AVRational result;
    result.den = (int)floor(FPS * 100);
    result.num = 100;
    if (VideoCodec && VideoCodec->supported_framerates) {
        const AVRational *p= VideoCodec->supported_framerates;
        AVRational      req =(AVRational){result.den, result.num};
        const AVRational *best = NULL;
        AVRational best_error= (AVRational){INT_MAX, 1};
        for(; p->den!=0; p++) {
            AVRational error = av_sub_q(req, *p);
            if (error.num <0)   error.num *= -1;
            if (av_cmp_q(error, best_error) < 0) {
                best_error = error;
                best = p;
            }
        }
        if (best && best->num && best->den) {
            result.den = best->num;
            result.num = best->den;
        }
    }
    if (result.den == 2997) {
        result.den = 30000;
        result.num = 1001;
    } else if (result.den == 5994) {
        result.den = 60000;
        result.num = 1001;
    }
    return result;
}

//*************************************************************************************************************************************************

bool cEncodeVideo::EncodeVideo() {
    bool                    Continue=true;
    cSoundBlockList         RenderMusic,ToEncodeMusic;
    QFutureWatcher<void>    ThreadWrite;
    cDiaporamaObjectInfo    *PreviousFrame=NULL;
    cDiaporamaObjectInfo    *Frame        =NULL;
    qlonglong               RenderedFrame =0;

    CurAudioPts         =0;
    StartTime           =QTime::currentTime();
    LastCheckTime       =StartTime;                                     // Display control : last time the loop start
    qlonglong Position  =Diaporama->GetObjectStartPosition(FromSlide);  // Render current position
    int ColumnStart     =-1;                                            // Render start position of current object
    int Column          =-1;                                            // Render current object

    // Init RenderMusic and ToEncodeMusic
    RenderMusic.SetFPS(VideoFrameRate,2,AudioStream->codec->sample_rate,AV_SAMPLE_FMT_S16);
    #ifndef USELIBAVRESAMPLE
        // LIBAV 0.8 => ToEncodeMusic use AudioStream->codec->sample_fmt format
        int ComputedFrameSize=AudioStream->codec->channels*av_get_bytes_per_sample(AudioStream->codec->sample_fmt)*AudioStream->codec->frame_size;
        if (ComputedFrameSize==0) ComputedFrameSize=RenderMusic.SoundPacketSize;
        ToEncodeMusic.SetFrameSize(ComputedFrameSize,AudioStream->codec->channels,AudioStream->codec->sample_rate,AudioStream->codec->sample_fmt);
    #else
        // LIBAV 9 => ToEncodeMusic use AV_SAMPLE_FMT_S16 format
        int ComputedFrameSize=AudioStream->codec->channels*av_get_bytes_per_sample(AV_SAMPLE_FMT_S16)*AudioStream->codec->frame_size;
        if (ComputedFrameSize==0) ComputedFrameSize=RenderMusic.SoundPacketSize;
        ToEncodeMusic.SetFrameSize(ComputedFrameSize,RenderMusic.Channels,AudioStream->codec->sample_rate,AV_SAMPLE_FMT_S16);
    #endif

    // Init Resampler (if needed)
    RPCK=NULL;
    RSC =NULL;
    #ifndef USELIBAVRESAMPLE
        if ((AudioStream->codec->sample_fmt!=RenderMusic.SampleFormat)||(AudioStream->codec->channels!=RenderMusic.Channels)||(AudioStream->codec->sample_rate!=RenderMusic.SamplingRate)) {
            if (!RPCK) {
                RPCK=(uint8_t *)av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE*ToEncodeMusic.Channels);
                if (!RPCK) {
                    ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: RPCK allocation failed"));
                    Continue=false;
                }
            }
            if (!RSC) RSC=av_audio_resample_init(                               // Context for resampling audio data
                AudioStream->codec->channels,RenderMusic.Channels,              // output_channels, input_channels
                AudioStream->codec->sample_rate,RenderMusic.SamplingRate,       // output_rate, input_rate
                AudioStream->codec->sample_fmt,RenderMusic.SampleFormat,        // sample_fmt_out, sample_fmt_in
                0,                                                              // filter_length
                0,                                                              // log2_phase_count
                1,                                                              // linear
                0);                                                             // cutoff
            if (!RSC) {
                ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: av_audio_resample_init failed"));
                Continue=false;
            }
        }
    #else
        if ((AudioStream->codec->sample_fmt!=ToEncodeMusic.SampleFormat)||(AudioStream->codec->channels!=ToEncodeMusic.Channels)||(AudioStream->codec->sample_rate!=ToEncodeMusic.SamplingRate)) {
            if (!RPCK) {
                int out_linesize=0;
                if (av_samples_alloc(&RPCK,&out_linesize,ToEncodeMusic.Channels,AVCODEC_MAX_AUDIO_FRAME_SIZE,ToEncodeMusic.SampleFormat,1)<0) {
                    ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: RPCK allocation failed"));
                    Continue=false;
                }
            }
            if (!RSC) {
                RSC=avresample_alloc_context();
                if (RSC) {
                    av_opt_set_int(RSC,"in_channel_layout",  av_get_default_channel_layout(ToEncodeMusic.Channels), 0);
                    av_opt_set_int(RSC,"in_sample_fmt",      ToEncodeMusic.SampleFormat,                            0);
                    av_opt_set_int(RSC,"in_sample_rate",     ToEncodeMusic.SamplingRate,                            0);
                    av_opt_set_int(RSC,"out_channel_layout", AudioStream->codec->channel_layout,                    0);
                    av_opt_set_int(RSC,"out_sample_fmt",     AudioStream->codec->sample_fmt,                        0);
                    av_opt_set_int(RSC,"out_sample_rate",    AudioStream->codec->sample_rate,                       0);
                    if (avresample_open(RSC)<0) {
                        ToLog(LOGMSG_CRITICAL,QString("Error opening context"));
                        Continue=false;
                    }
                } else {
                    ToLog(LOGMSG_CRITICAL,QString("Error allocating AVAudioResampleContext"));
                    Continue=false;
                }
            }
        #endif
    }

    for (RenderedFrame=0;Continue && (RenderedFrame<NbrFrame);RenderedFrame++) {

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
        if (LastCheckTime.msecsTo(QTime::currentTime())>=1000) DisplayProgress(RenderedFrame,Position,Column,ColumnStart);  // Refresh display only one time per second

        // Get current frame
        Frame=new cDiaporamaObjectInfo(PreviousFrame,Position,Diaporama,double(1000)/double(VideoFrameRate));

        // Ensure MusicTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_MusicTrack==NULL)) {
            Frame->CurrentObject_MusicTrack=new cSoundBlockList();
            Frame->CurrentObject_MusicTrack->SetFPS(VideoFrameRate,2,AudioStream->codec->sample_rate,AV_SAMPLE_FMT_S16);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
            Frame->TransitObject_MusicTrack=new cSoundBlockList();
            Frame->TransitObject_MusicTrack->SetFPS(VideoFrameRate,2,AudioStream->codec->sample_rate,AV_SAMPLE_FMT_S16);
        }

        // Ensure SoundTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
            Frame->CurrentObject_SoundTrackMontage=new cSoundBlockList();
            Frame->CurrentObject_SoundTrackMontage->SetFPS(VideoFrameRate,2,AudioStream->codec->sample_rate,AV_SAMPLE_FMT_S16);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
            Frame->TransitObject_SoundTrackMontage=new cSoundBlockList();
            Frame->TransitObject_SoundTrackMontage->SetFPS(VideoFrameRate,2,AudioStream->codec->sample_rate,AV_SAMPLE_FMT_S16);
        }

        // Prepare frame with W and H =0 to force SoundMusicOnly! (thread mode is not necessary here)
        Diaporama->LoadSources(Frame,0,0,false,0);

        // Calc number of packet to mix
        int MaxPacket=Frame->CurrentObject_MusicTrack->List.count();
        if ((Frame->CurrentObject_SoundTrackMontage!=NULL)&&
            (Frame->CurrentObject_SoundTrackMontage->List.count()>0)&&
            (MaxPacket>Frame->CurrentObject_SoundTrackMontage->List.count())) MaxPacket=Frame->CurrentObject_SoundTrackMontage->List.count();
        if (MaxPacket>RenderMusic.NbrPacketForFPS) MaxPacket=RenderMusic.NbrPacketForFPS;

        // mix audio data
        for (int j=0;j<MaxPacket;j++)
            RenderMusic.MixAppendPacket(Frame->CurrentObject_MusicTrack->DetachFirstPacket(),Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket());

        // Give time to interface!
        //while ((ThreadWrite.isRunning())&&(!StopProcessWanted)) QApplication::processEvents();
        if (ThreadWrite.isRunning()) ThreadWrite.waitForFinished();

        //************************** Unthreaded part

        // Transfert RenderMusic data to EncodeMusic data
        while ((Continue)&&(RenderMusic.List.count()>0)) {
            uint8_t *PacketSound=(uint8_t *)RenderMusic.DetachFirstPacket();
            if (PacketSound==NULL) {
                PacketSound=(uint8_t *)av_malloc(RenderMusic.SoundPacketSize+4);
                memset(PacketSound,0,RenderMusic.SoundPacketSize);
            }
            #ifndef USELIBAVRESAMPLE
                // LIBAV 0.8 => ToEncodeMusic must have exactly AudioStream->codec->frame_size data
                if ((RSC!=NULL)&&(RPCK!=NULL)) {
                    int64_t DestNbrSamples=RenderMusic.SoundPacketSize/(RenderMusic.Channels*av_get_bytes_per_sample(RenderMusic.SampleFormat));
                    int64_t DestPacketSize=audio_resample(RSC,(short int*)RPCK,(short int*)PacketSound,DestNbrSamples)*AudioStream->codec->channels*av_get_bytes_per_sample(AudioStream->codec->sample_fmt);
                    if (DestPacketSize<=0) {
                        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: audio_resample failed"));
                        Continue=false;
                    } else ToEncodeMusic.AppendData((int16_t *)RPCK,DestPacketSize);
                } else ToEncodeMusic.AppendData((int16_t *)PacketSound,RenderMusic.SoundPacketSize);
            #else
                // LIBAV 9 => ToEncodeMusic is converted during encoding process
                ToEncodeMusic.AppendData((int16_t *)PacketSound,RenderMusic.SoundPacketSize);
            #endif
            av_free(PacketSound);
        }

        //************************** End of unthreaded part

        // Write audio frame to disk
        if (Continue) ThreadWrite.setFuture(QtConcurrent::run(this,&cEncodeVideo::EncodeMusic,&ToEncodeMusic,&Continue));
//        if (Continue) EncodeMusic(&ToEncodeMusic,&Continue);

        // Calculate next position
        if (Continue) {
            Position     +=double(1000)/VideoFrameRate;
            if (PreviousFrame!=NULL) delete PreviousFrame;
            PreviousFrame=Frame;
            Frame =NULL;
        }

        // Stop the process if error occur or user ask to stop
        Continue=Continue && !StopProcessWanted;
    }

    //while ((ThreadWrite.isRunning())&&(!StopProcessWanted)) QApplication::processEvents();
    if (ThreadWrite.isRunning()) ThreadWrite.waitForFinished();

    // refresh display
    DisplayProgress(RenderedFrame,Position,Column,ColumnStart);

    // Close Resampler (if needed)
    if (RSC) {
        #ifndef USELIBAVRESAMPLE
            audio_resample_close(RSC);
            RSC=NULL;
        #else
            avresample_close(RSC);
            avresample_free(&RSC);
            RSC=NULL;
        #endif
    }
    if (RPCK) av_free(RPCK);

    // Cleaning
    if (PreviousFrame!=NULL) delete PreviousFrame;
    if (Frame!=NULL)         delete Frame;

    return Continue;
}

//*************************************************************************************************************************************************

void cEncodeVideo::EncodeMusic(cSoundBlockList *ToEncodeMusic,bool *Continue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::EncodeMusic");

    AVFrame *frame=avcodec_alloc_frame();
    if (frame==NULL) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: avcodec_alloc_frame failed"));
        *Continue=false;
        return;
    }

    int     errcode;
    int64_t DestNbrSamples=ToEncodeMusic->SoundPacketSize/(ToEncodeMusic->Channels*av_get_bytes_per_sample(ToEncodeMusic->SampleFormat));
    int     DestSampleSize=AudioStream->codec->channels*av_get_bytes_per_sample(AudioStream->codec->sample_fmt);
    uint8_t *DestPacket   =NULL;
    int16_t *PacketSound  =NULL;
    int64_t DestPacketSize=DestNbrSamples*DestSampleSize;

    // Flush audio frame of ToEncodeMusic
    while (ToEncodeMusic->List.count()>0) {
        PacketSound=ToEncodeMusic->DetachFirstPacket();
        if (PacketSound==NULL) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: PacketSound==NULL"));
            *Continue=false;
        } else {
            #ifdef USELIBAVRESAMPLE
                // LIBAV 9 => Convert sample format (is needed)
                if ((RSC!=NULL)&&(RPCK!=NULL)) {
                    DestPacket=RPCK;
                    uint8_t *in_data[AVRESAMPLE_MAX_CHANNELS]={0},*out_data[AVRESAMPLE_MAX_CHANNELS]={0};
                    int     in_linesize=0,out_linesize=0;
                    int     out_samples=avresample_available(RSC)+av_rescale_rnd(avresample_get_delay(RSC)+DestNbrSamples,AudioStream->codec->sample_rate,ToEncodeMusic->SamplingRate,AV_ROUND_UP);
                    if (av_samples_fill_arrays(in_data,&in_linesize,(uint8_t *)PacketSound,ToEncodeMusic->Channels,DestNbrSamples,ToEncodeMusic->SampleFormat,1)<0) {
                        ToLog(LOGMSG_CRITICAL,QString("failed in_data fill arrays"));
                        Continue=false;
                    } else {
                        if (av_samples_fill_arrays(out_data,&out_linesize,RPCK,AudioStream->codec->channels,out_samples,AudioStream->codec->sample_fmt,1)<0) {
                            ToLog(LOGMSG_CRITICAL,QString("failed out_data fill arrays"));
                            Continue=false;
                        } else {
                            DestPacketSize=avresample_convert(RSC,out_data,out_linesize,out_samples,in_data,in_linesize,DestNbrSamples)*DestSampleSize;
                            if (DestPacketSize<=0) {
                                ToLog(LOGMSG_CRITICAL,QString("Error in avresample_convert"));
                                Continue=false;
                            }
                        }
                    }
                    DestPacket=out_data[0];
                } else DestPacket=(uint8_t *)PacketSound;
            #else
                DestPacket=(uint8_t *)PacketSound;
            #endif

            // Init frame
            avcodec_get_frame_defaults(frame);
            frame->nb_samples    =DestPacketSize/DestSampleSize;
            #ifdef USELIBAVRESAMPLE
            frame->format        =AudioStream->codec->sample_fmt;
            frame->channel_layout=AudioStream->codec->channel_layout;
            frame->sample_rate   =AudioStream->codec->sample_rate;
            #endif

            // fill buffer
            errcode=avcodec_fill_audio_frame(frame,AudioStream->codec->channels,AudioStream->codec->sample_fmt,(const uint8_t*)DestPacket,DestPacketSize,1);
            if (errcode>=0) {
                // Init packet
                AVPacket pkt;
                av_init_packet(&pkt);
                pkt.data    =NULL;
                pkt.size    =0;
                pkt.pts     =CurAudioPts++;
                pkt.dts     =AV_NOPTS_VALUE;

                int got_packet=0;
                errcode=avcodec_encode_audio2(AudioStream->codec,&pkt,frame,&got_packet);
                if (errcode<0) {
                    char Buf[2048];
                    av_strerror(errcode,Buf,2048);
                    ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: avcodec_encode_audio2() failed: ")+QString(Buf));
                    *Continue=false;
                } else {
                    pkt.flags       |= AV_PKT_FLAG_KEY;
                    pkt.stream_index =AudioStream->index;

                    if (pkt.pts!=(int64_t)AV_NOPTS_VALUE)   pkt.pts     =av_rescale_q(pkt.pts,AudioStream->codec->time_base,AudioStream->time_base);
                    if (pkt.dts!=(int64_t)AV_NOPTS_VALUE)   pkt.dts     =av_rescale_q(pkt.dts,AudioStream->codec->time_base,AudioStream->time_base);
                    if (pkt.duration>0)                     pkt.duration=av_rescale_q(pkt.duration,AudioStream->codec->time_base,AudioStream->time_base);

                    // write the compressed frame in the media file
                    errcode=av_interleaved_write_frame(Container,&pkt);
                    if (errcode!=0) {
                        char Buf[2048];
                        av_strerror(errcode,Buf,2048);
                        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: av_interleaved_write_frame failed: ")+QString(Buf));
                        *Continue=false;
                    }
                }
            } else {
                char Buf[2048];
                av_strerror(errcode,Buf,2048);
                ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: avcodec_fill_audio_frame() failed: ")+QString(Buf));
                *Continue=false;
            }
            if ((frame->extended_data)&&(frame->extended_data!=frame->data)) {
                av_free(frame->extended_data);
                frame->extended_data=NULL;
            }
        }
        av_free(PacketSound);
    }
    av_free(frame);
    avcodec_flush_buffers(AudioStream->codec);
}
