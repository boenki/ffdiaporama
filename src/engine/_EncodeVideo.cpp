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

#define PIXFMT      PIX_FMT_RGB24
#define QTPIXFMT    QImage::Format_RGB888

//*************************************************************************************************************************************************

int CheckEncoderCapabilities(VFORMAT_ID FormatId,CodecID VideoCodec,CodecID AudioCodec) {
    if (VideoCodec==AV_CODEC_ID_NONE) return SUPPORTED_COMBINATION;

    int Ret=INVALID_COMBINATION;

    switch (FormatId) {
        case VFORMAT_MJPEG:     if ((VideoCodec==AV_CODEC_ID_MJPEG)&&(AudioCodec==AV_CODEC_ID_PCM_S16LE))                                   Ret=UNSUPPORTED_COMBINATION;    break;  // Error with WAV in AVI
        #ifdef LIBAV_08
        case VFORMAT_OGV:       if ((VideoCodec==AV_CODEC_ID_THEORA)&&(AudioCodec==AV_CODEC_ID_VORBIS))                                     Ret=SUPPORTED_COMBINATION;      break;
        case VFORMAT_WEBM:      if ((VideoCodec==AV_CODEC_ID_VP8)&&(AudioCodec==AV_CODEC_ID_VORBIS))                                        Ret=SUPPORTED_COMBINATION;      break;
        #else
        case VFORMAT_OGV:       if ((VideoCodec==AV_CODEC_ID_THEORA)&&(AudioCodec==AV_CODEC_ID_VORBIS))                                     Ret=UNSUPPORTED_COMBINATION;      break;
        case VFORMAT_WEBM:      if ((VideoCodec==AV_CODEC_ID_VP8)&&(AudioCodec==AV_CODEC_ID_VORBIS))                                        Ret=UNSUPPORTED_COMBINATION;      break;
        #endif
        case VFORMAT_OLDFLV:    if ((VideoCodec==AV_CODEC_ID_FLV1)&&(AudioCodec==AV_CODEC_ID_MP3))                                          Ret=SUPPORTED_COMBINATION;      break;
        case VFORMAT_FLV:       if ((VideoCodec==AV_CODEC_ID_H264)&&(AudioCodec==AV_CODEC_ID_AAC))                                          Ret=UNSUPPORTED_COMBINATION;    break;  // Error with h264
        case VFORMAT_MPEG:      if ((VideoCodec==AV_CODEC_ID_MPEG2VIDEO)&&((AudioCodec==AV_CODEC_ID_AC3)||(AudioCodec==AV_CODEC_ID_MP2)))   Ret=SUPPORTED_COMBINATION;      break;

        case VFORMAT_AVI:
            switch (VideoCodec) {
                case AV_CODEC_ID_MPEG4:
                case AV_CODEC_ID_MJPEG:
                case AV_CODEC_ID_MPEG2VIDEO:   if ((AudioCodec==AV_CODEC_ID_MP2)||(AudioCodec==AV_CODEC_ID_AC3)||(AudioCodec==AV_CODEC_ID_MP3)) Ret=SUPPORTED_COMBINATION;
                                                else if (AudioCodec==AV_CODEC_ID_PCM_S16LE)                                                 Ret=UNSUPPORTED_COMBINATION;            // Error with WAV in AVI
                                            break;
                case AV_CODEC_ID_H264:                                                                                                      Ret=UNSUPPORTED_COMBINATION;    break;  // Error with h264
                default: break;
            }
            break;

        case VFORMAT_3GP:
            if ((VideoCodec==AV_CODEC_ID_MPEG4)&&((AudioCodec==AV_CODEC_ID_AMR_NB)||(AudioCodec==AV_CODEC_ID_AMR_WB)))                      Ret=SUPPORTED_COMBINATION;
                else if ((VideoCodec==AV_CODEC_ID_H264)&&((AudioCodec==AV_CODEC_ID_AMR_NB)||(AudioCodec==AV_CODEC_ID_AMR_WB)))              Ret=UNSUPPORTED_COMBINATION;            // Error with h264
            break;

        case VFORMAT_MP4:
            if ((VideoCodec==AV_CODEC_ID_MPEG4)&&((AudioCodec==AV_CODEC_ID_MP3)||(AudioCodec==AV_CODEC_ID_AAC)))                            Ret=UNSUPPORTED_COMBINATION;              // unfinished
                else if ((VideoCodec==AV_CODEC_ID_H264)&&((AudioCodec==AV_CODEC_ID_MP3)||(AudioCodec==AV_CODEC_ID_AAC)))                    Ret=UNSUPPORTED_COMBINATION;            // Error with h264
            break;

        case VFORMAT_MKV:
            switch (VideoCodec) {
                case AV_CODEC_ID_MJPEG:
                    switch (AudioCodec) {
                        case AV_CODEC_ID_PCM_S16LE:                                                            Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_MP3:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_AAC:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_AC3:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_VORBIS:                                                               Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_FLAC:                                                                 Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        default: break;
                    }
                    break;
                case AV_CODEC_ID_MPEG4:
                    switch (AudioCodec) {
                        case AV_CODEC_ID_PCM_S16LE:                                                            Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_MP3:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_AAC:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_AC3:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_VORBIS:                                                               Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_FLAC:                                                                 Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        default: break;
                    }
                    break;
                case AV_CODEC_ID_H264:
                    switch (AudioCodec) {
                        case AV_CODEC_ID_PCM_S16LE:                                                            Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_MP3:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_AAC:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_AC3:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_VORBIS:                                                               Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_FLAC:                                                                 Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        default: break;
                    }
                    break;
                case AV_CODEC_ID_THEORA:
                    switch (AudioCodec) {
                        case AV_CODEC_ID_PCM_S16LE:                                                            Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_MP3:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_AAC:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_AC3:                                                                  Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_VORBIS:                                                               Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        case AV_CODEC_ID_FLAC:                                                                 Ret=UNSUPPORTED_COMBINATION;    break;  // unfinished
                        default: break;
                    }
                    break;
                default: break;
            }
            break;

        default: break;
    }
    return Ret;
}

//*************************************************************************************************************************************************

cEncodeVideo::cEncodeVideo() {
    StopProcessWanted       =false;
    Diaporama               =NULL;
    Container               =NULL;
    IsOpen                  =false;

    // Audio buffers
    AudioStream             =NULL;
    AudioFrame              =NULL;
    AudioResampler          =NULL;
    AudioResamplerBuffer    =NULL;

    //Video buffers
    VideoStream             =NULL;
    VideoEncodeBuffer       =NULL;
    VideoEncodeBufferSize   =40*1024*1024;
    VideoFrameConverter     =NULL;
    VideoFrame              =NULL;
    InternalWidth           =0;
    InternalHeight          =0;
    ExtendV                 =0;
    VideoFrameBufSize       =0;
    VideoFrameBuf           =NULL;

    // link to control for progression display
    ElapsedTimeLabel        =NULL;
    SlideNumberLabel        =NULL;
    FrameNumberLabel        =NULL;
    FPSLabel                =NULL;
    SlideProgressBar        =NULL;
    TotalProgressBar        =NULL;
}

//*************************************************************************************************************************************************

cEncodeVideo::~cEncodeVideo() {
    CloseEncoder();
}

//*************************************************************************************************************************************************

void cEncodeVideo::CloseEncoder() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::CloseEncoder");

    if (Container) {
        if (IsOpen) {
            if ((AudioStream)&&(AudioStream->codec->codec_id==AV_CODEC_ID_FLAC)) {
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
            /*
            if (VideoStream) {
                #ifdef LIBAV_09
                pkt.data=NULL;
                pkt.size=0;
                pkt.pts =CurVideoPts++;
                pkt.dts =AV_NOPTS_VALUE;
                avcodec_encode_video2(VideoStream->codec,&pkt,NULL,&got_packet);
                avcodec_flush_buffers(VideoStream->codec);
                #endif
            }
            */
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
    // Audio buffers
    if (AudioFrame) {
        av_free(AudioFrame);
        AudioFrame=NULL;
    }
    if (AudioResampler) {
        #ifdef LIBAV_08
            audio_resample_close(AudioResampler);
            AudioResampler=NULL;
        #else
            avresample_close(AudioResampler);
            avresample_free(&AudioResampler);
            AudioResampler=NULL;
        #endif
    }
    if (AudioResamplerBuffer) {
        av_free(AudioResamplerBuffer);
        AudioResamplerBuffer=NULL;
    }

    // Video buffers
    if (VideoEncodeBuffer) {
        av_free(VideoEncodeBuffer);
        VideoEncodeBuffer=NULL;
    }
    if (VideoFrameConverter) {
        sws_freeContext(VideoFrameConverter);
        VideoFrameConverter=NULL;
    }
    if (VideoFrame) {
        if ((VideoFrame->extended_data)&&(VideoFrame->extended_data!=VideoFrame->data)) {
            av_free(VideoFrame->extended_data);
            VideoFrame->extended_data=NULL;
        }
        if (VideoFrame->data[0]) {
            av_free(VideoFrame->data[0]);
            VideoFrame->data[0]=NULL;
        }
        av_free(VideoFrame);
        VideoFrame=NULL;
    }
}

//*************************************************************************************************************************************************

bool cEncodeVideo::OpenEncoder(cDiaporama *Diaporama,QString OutputFileName,int FromSlide,int ToSlide,
                    bool EncodeVideo,int VideoCodecId,int VideoCodecSubId,AVRational VideoFrameRate,int ImageWidth,int ImageHeight,int ExtendV,int InternalWidth,int InternalHeight,AVRational PixelAspectRatio,int VideoBitrate,
                    bool EncodeAudio,int AudioCodecId,int AudioChannels,int AudioBitrate,int AudioSampleRate,QString Language) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::OpenEncoder");

    sFormatDef *FormatDef   =NULL;
    this->Diaporama         =Diaporama;
    this->OutputFileName    =OutputFileName;
    this->FromSlide         =FromSlide;
    this->ToSlide           =ToSlide;
    this->dFPS              =double(VideoFrameRate.den)/double(VideoFrameRate.num);
    NbrFrame                =int(double(Diaporama->GetPartialDuration(FromSlide,ToSlide))*dFPS/1000);    // Number of frame to generate
    QString FMT             =QFileInfo(OutputFileName).suffix();

    //=======================================
    // Prepare container
    //=======================================

    // Search FMT from FROMATDEF
    int i=0;
    while ((i<VFORMAT_NBR)&&(QString(FORMATDEF[i].FileExtension)!=FMT)) i++;

    // if FMT not found, search it from AUDIOFORMATDEF
    if (i>=VFORMAT_NBR) {
        int i=0;
        while ((i<NBR_AFORMAT)&&(QString(AUDIOFORMATDEF[i].FileExtension)!=FMT)) i++;
        if (i>=NBR_AFORMAT) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: Unknown format %1").arg(FMT));
            return false;
        } else FormatDef=&AUDIOFORMATDEF[i];
    } else FormatDef=&FORMATDEF[i];

    // Alloc container
    Container=avformat_alloc_context();
    if (!Container) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenEncoder: Unable to allocate AVFormatContext");
        return false;
    }

    // Prepare container struct
    snprintf(Container->filename,sizeof(Container->filename),"%s",OutputFileName.toUtf8().constData());

    Container->oformat=av_guess_format(QString(FormatDef->ShortName).toUtf8().constData(),NULL,NULL);
    if (!Container->oformat) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: Unable to guess av_guess_format from container %1").arg(QString(FormatDef->ShortName)));
        return false;
    }
    if (Container->oformat->flags & AVFMT_NOFILE) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: Container->oformat->flags==AVFMT_NOFILE"));
        return false;
    }

    //=======================================
    // Open video stream
    //=======================================

    if (EncodeVideo) {

        // Video parameters
        this->VideoFrameRate=VideoFrameRate;
        this->InternalWidth =InternalWidth;
        this->InternalHeight=InternalHeight;
        this->ExtendV       =ExtendV;

        // Search Video codec from VideoCodecId
        int i=0;
        while ((i<NBR_VIDEOCODECDEF)&&(VIDEOCODECDEF[i].Codec_id!=VideoCodecId)) i++;
        if ((i>=NBR_VIDEOCODECDEF)||(!VIDEOCODECDEF[i].IsFind)) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: video codec not available"));
            return false;
        }

        // Add stream
        if (!OpenVideoStream(&VIDEOCODECDEF[i],VideoCodecSubId,VideoFrameRate,ImageWidth,ImageHeight+ExtendV,PixelAspectRatio,VideoBitrate))
            return false;
    }

    //=======================================
    // Open Audio stream
    //=======================================

    if (EncodeAudio) {
        // Audio parameters
        this->AudioChannels     =AudioChannels;
        this->AudioBitrate      =AudioBitrate;
        this->AudioSampleRate   =AudioSampleRate;

        // Search Video codec from AudioCodecId
        int i=0;
        while ((i<NBR_AUDIOCODECDEF)&&(AUDIOCODECDEF[i].Codec_id!=AudioCodecId)) i++;
        if ((i>=NBR_AUDIOCODECDEF)||(!AUDIOCODECDEF[i].IsFind)) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenEncoder: audio codec not available"));
            return false;
        }

        // Add stream
        if (!OpenAudioStream(&AUDIOCODECDEF[i],AudioChannels,AudioBitrate,AudioSampleRate,Language))
            return false;
    }

    //********************************************
    // Open file and header
    //********************************************

    if (!PrepareTAG(Language)) return false;

    if (avio_open(&Container->pb,Container->filename,AVIO_FLAG_WRITE)<0) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenEncoder: avio_open() failed");
        return false;
    }
    AVDictionary *opts=NULL;
    if (QString(Container->oformat->name)==QString("mpegts")) {
        Container->packet_size=188;
        Container->max_delay  =(int)(double(0.7)*double(AV_TIME_BASE));
        if (VideoStream) av_dict_set(&opts,"muxrate",QString("%1").arg(int(VideoStream->codec->bit_rate*1.1)).toUtf8(),0);
    }

    if (avformat_write_header(Container,&opts)<0) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenEncoder: avformat_write_header() failed");
        return false;
    }

    //********************************************
    // Log output format
    //********************************************
    av_dump_format(Container,0,OutputFileName.toUtf8().constData(),1);

    IsOpen=true;
    return true;
}

//*************************************************************************************************************************************************
// Create video streams
//*************************************************************************************************************************************************

bool cEncodeVideo::OpenVideoStream(sVideoCodecDef *VideoCodecDef,int VideoCodecSubId,AVRational VideoFrameRate,
                                   int ImageWidth,int ImageHeight,AVRational PixelAspectRatio,int VideoBitrate) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::OpenVideoStream");
    AVDictionary *opts=NULL;
    int MinRate=-1;
    int MaxRate=-1;
    int BufSize=-1;
    int ThreadC=-1;
    int BFrames=-1;

    AVCodec *codec=avcodec_find_encoder_by_name(VideoCodecDef->ShortName);
    if (!codec) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenVideoStream: Unable to find video codec %1").arg(VideoCodecDef->ShortName));
        return false;
    }
    if (codec->id==AV_CODEC_ID_NONE) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenVideoStream: VideoCodec->id==AV_CODEC_ID_NONE"));
        return false;
    }

    Container->oformat->video_codec=codec->id;

    VideoStream=avformat_new_stream(Container,NULL);
    if (!VideoStream) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: avformat_new_stream() failed");
        return false;
    }
    if (avcodec_get_context_defaults3(VideoStream->codec,codec)<0) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: avcodec_get_context_defaults3() failed");
        return false;
    }

    VideoStream->codec->width               =ImageWidth;
    VideoStream->codec->height              =ImageHeight;
    VideoStream->codec->pix_fmt             =PIX_FMT_YUV420P;
    VideoStream->codec->time_base           =VideoFrameRate;
    VideoStream->codec->sample_aspect_ratio =PixelAspectRatio;
    VideoStream->sample_aspect_ratio        =PixelAspectRatio;
    VideoStream->codec->bit_rate            =VideoBitrate;
    av_dict_set(&opts,"b",QString("%1").arg(VideoBitrate).toUtf8(),0);

    if (codec->id==AV_CODEC_ID_MPEG2VIDEO) {
        BFrames=2;
        ThreadC=((getCpuCount()-1)>1)?(getCpuCount()-1):1;
        #ifdef LIBAV_08
        MinRate=VideoBitrate;
        MaxRate=VideoBitrate;
        BufSize=1000000;
        VideoStream->codec->flags                |=CODEC_FLAG_CLOSED_GOP;
        VideoStream->codec->flags2               |=CODEC_FLAG2_STRICT_GOP;
        VideoStream->codec->scenechange_threshold =1000000000;
        VideoStream->codec->error_concealment    |=FF_EC_DEBLOCK;
        #endif
        VideoStream->codec->gop_size              =12;

    } else if (codec->id==AV_CODEC_ID_MJPEG) {
        //-qscale 2 -qmin 2 -qmax 2
        VideoStream->codec->pix_fmt=PIX_FMT_YUVJ420P;
        VideoStream->codec->qmax   =2;
        VideoStream->codec->qmin   =2;

    } else if (codec->id==AV_CODEC_ID_VP8) {

        BFrames=3;
        ThreadC=((getCpuCount()-1)>1)?(getCpuCount()-1):1;
        VideoStream->codec->gop_size=120;
        VideoStream->codec->qmax    =ImageHeight<=576?63:51;
        VideoStream->codec->qmin    =ImageHeight<=576?1:11;
        VideoStream->codec->mb_lmin =VideoStream->codec->qmin*FF_QP2LAMBDA;
        VideoStream->codec->lmin    =VideoStream->codec->qmin*FF_QP2LAMBDA;
        VideoStream->codec->mb_lmax =VideoStream->codec->qmax*FF_QP2LAMBDA;
        VideoStream->codec->lmax    =VideoStream->codec->qmax*FF_QP2LAMBDA;

        if (ImageHeight<=720) av_dict_set(&opts,"profile","0",0); else av_dict_set(&opts,"profile","1",0);
        if (ImageHeight>576) av_dict_set(&opts,"slices","4",0);

        av_dict_set(&opts,"g",QString("%1").arg(VideoStream->codec->gop_size).toUtf8(),0);
        av_dict_set(&opts,"qmax",QString("%1").arg(VideoStream->codec->qmax).toUtf8(),0);
        av_dict_set(&opts,"qmin",QString("%1").arg(VideoStream->codec->qmin).toUtf8(),0);
        av_dict_set(&opts,"lag-in-frames","16",0);
        av_dict_set(&opts,"deadline","good",0);
        av_dict_set(&opts,"cpu-used",QString("%1").arg(ThreadC).toUtf8(),0);

        #if defined(LIBAV_08)
            MinRate=VideoBitrate;
            MaxRate=VideoBitrate;
            BufSize=VideoBitrate;
        #endif

    } else if (codec->id==AV_CODEC_ID_H264) {
        ThreadC=((getCpuCount()-1)>1)?(getCpuCount()-1):1;
        MinRate=0;
        MaxRate=int(double(VideoBitrate)*1.1);
        BufSize=int(double(VideoBitrate)*2);
        av_dict_set(&opts,"preset","veryfast",0);

        switch (VideoCodecSubId) {
            case VCODEC_H264HQ:     // High Quality H.264 AVC/MPEG-4 AVC
                // "-preset veryfast -refs:0 3 | -preset veryfast -x264opts ref=3
                av_dict_set(&opts,"refs","3",0);
                //VideoStream->codec->refs=3;
                //VideoStream->codec->qmin=0;
                //VideoStream->codec->qmax=69;
                break;

            case VCODEC_H264PQ:     // Phone Quality H.264 AVC/MPEG-4 AVC
                av_opt_set(VideoStream->codec->priv_data,"tune","zerolatency",0);
                av_dict_set(&opts,"refs","3",0);
                av_dict_set(&opts,"profile","baseline",0);
                av_dict_set(&opts,"tune","fastdecode",0);
                break;

            case VCODEC_X264LL:     // x264 lossless
                av_dict_set(&opts,"qp","0",0);
                break;
        }
    }

    if (MinRate!=-1) {
        av_dict_set(&opts,"minrate",QString("%1").arg(MinRate).toUtf8(),0);
        av_dict_set(&opts,"maxrate",QString("%1").arg(MaxRate).toUtf8(),0);
        av_dict_set(&opts,"bufsize",QString("%1").arg(BufSize).toUtf8(),0);
    }
    if (BFrames!=-1) {
        VideoStream->codec->max_b_frames=BFrames;
        av_dict_set(&opts,"bf",QString("%1").arg(BFrames).toUtf8(),0);
    }
    VideoStream->codec->has_b_frames=(VideoStream->codec->max_b_frames>0)?1:0;
    if (ThreadC>0) {
        VideoStream->codec->thread_count=ThreadC;
        av_dict_set(&opts,"threads",QString("%1").arg(ThreadC).toUtf8(),0);
    }

    if ((Container->oformat->flags & AVFMT_GLOBALHEADER)||
        (!strcmp(Container->oformat->name,"mp4"))||
        (!strcmp(Container->oformat->name,"mov"))||
        (!strcmp(Container->oformat->name,"mpegts"))||
        (!strcmp(Container->oformat->name,"3gp")))
        VideoStream->codec->flags|=CODEC_FLAG_GLOBAL_HEADER;

    // Open encoder
    int errcode=avcodec_open2(VideoStream->codec,codec,&opts);
    if (errcode<0) {
        char Buf[2048];
        av_strerror(errcode,Buf,2048);
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: avcodec_open2() failed: "+QString(Buf));
        return false;
    }

    // Create VideoFrameConverter
    VideoFrameConverter=sws_getContext(
        VideoStream->codec->width,VideoStream->codec->height,PIXFMT,                        // Src Widht,Height,Format
        VideoStream->codec->width,VideoStream->codec->height,VideoStream->codec->pix_fmt,   // Destination Width,Height,Format
        SWS_BICUBIC,                                                                        // flags
        NULL,NULL,NULL);                                                                    // src Filter,dst Filter,param
    if (!VideoFrameConverter) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: sws_getContext() failed");
        return false;
    }

    #ifdef LIBAV_08
    // Create VideoEncodeBuffer
    VideoEncodeBuffer=(uint8_t *)av_malloc(VideoEncodeBufferSize);
    if (!VideoEncodeBuffer) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: av_malloc() failed for VideoEncodeBuffer");
        return false;
    }
    #endif

    // Create and prepare VideoFrame and VideoFrameBuf
    VideoFrame=avcodec_alloc_frame();  // Allocate structure for RGB image
    if (!VideoFrame) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: avcodec_alloc_frame() failed");
        return false;
    } else {
        VideoFrameBufSize=avpicture_get_size(VideoStream->codec->pix_fmt,VideoStream->codec->width,VideoStream->codec->height);
        VideoFrameBuf   =(uint8_t *)av_malloc(VideoFrameBufSize);
        if ((!VideoFrameBufSize)||(!VideoFrameBuf)) {
            ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenVideoStream: av_malloc() failed for VideoFrameBuf");
            return false;
        }
    }

    return true;
}

//*************************************************************************************************************************************************
// Create audio streams
//*************************************************************************************************************************************************

bool cEncodeVideo::OpenAudioStream(sAudioCodecDef *AudioCodecDef,int AudioChannels,int AudioBitrate,int AudioSampleRate,QString Language) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::OpenAudioStream");
    AVDictionary    *opts   =NULL;
    int             ThreadC =((getCpuCount()-1)>1)?(getCpuCount()-1):1;

    AVCodec *Codec=avcodec_find_encoder_by_name(AudioCodecDef->ShortName);
    if (!Codec) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenAudioStream: Unable to find audio codec %1").arg(AudioCodecDef->ShortName));
        return false;
    }

    if (Codec->id==AV_CODEC_ID_NONE) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenAudioStream: AudioCodec->id==AV_CODEC_ID_NONE"));
        return false;
    }

    Container->oformat->audio_codec=Codec->id;

    AudioStream=avformat_new_stream(Container,NULL);
    if (!AudioStream) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenAudioStream: avformat_new_stream() failed");
        return false;
    }

    //AudioStream->codec->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    // Open codec
    av_dict_set(&AudioStream->metadata,"language",Language.toUtf8().constData(),0);

    AudioStream->codec->sample_fmt  =AV_SAMPLE_FMT_S16;
    AudioStream->codec->channels    =AudioChannels;
    AudioStream->codec->sample_rate =AudioSampleRate;
    AudioStream->codec->bit_rate    =AudioBitrate;
    av_dict_set(&opts,"ab",QString("%1").arg(AudioBitrate).toUtf8(),0);
    //av_opt_set_int(AudioStream->codec,"b",AudioBitrate,0);

    AudioStream->codec->thread_count=ThreadC;
    av_dict_set(&opts,"threads",QString("%1").arg(ThreadC).toUtf8(),0);
    //av_opt_set_int(AudioStream->codec,"threads",ThreadC,0);

    if ((Container->oformat->flags & AVFMT_GLOBALHEADER)
            ||(!strcmp(Container->oformat->name,"mp4"))
            ||(!strcmp(Container->oformat->name,"mov"))
            ||(!strcmp(Container->oformat->name,"3gp"))
            //||(!strcmp(Container->oformat->name,"ogg"))
        )
            AudioStream->codec->flags|=CODEC_FLAG_GLOBAL_HEADER;

    if (Codec->id==AV_CODEC_ID_PCM_S16LE) {

        // Nothing to do

    } else if (Codec->id==AV_CODEC_ID_FLAC) {
        av_opt_set_int(AudioStream->codec,"lpc_coeff_precision",15,0);
        av_opt_set_int(AudioStream->codec,"lpc_type",2,0);
        av_opt_set_int(AudioStream->codec,"lpc_passes",1,0);
        av_opt_set_int(AudioStream->codec,"min_partition_order",0,0);
        av_opt_set_int(AudioStream->codec,"max_partition_order",8,0);
        av_opt_set_int(AudioStream->codec,"prediction_order_method",0,0);
        av_opt_set_int(AudioStream->codec,"ch_mode",-1,0);

    } else if (Codec->id==AV_CODEC_ID_AAC) {

    } else if (Codec->id==AV_CODEC_ID_MP2) {

    } else if (Codec->id==AV_CODEC_ID_MP3) {
        #ifdef LIBAV_09
        AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_S16P;
        #endif
        av_opt_set_int(AudioStream->codec,"reservoir",1,0);

    } else if (Codec->id==AV_CODEC_ID_VORBIS) {
        #ifdef LIBAV_09
        AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLTP;
        #endif

    } else if (Codec->id==AV_CODEC_ID_AC3) {
        #ifdef LIBAV_09
        AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLTP;
        #else
        AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLT;
        #endif
    } else if (Codec->id==AV_CODEC_ID_AMR_NB) {
        AudioStream->codec->channels=1;
        AudioChannels=1;
    } else if (Codec->id==AV_CODEC_ID_AMR_WB) {
        AudioStream->codec->channels=1;
        AudioChannels=1;
    }
    #ifdef LIBAV_09
        AudioStream->codec->channel_layout= av_get_default_channel_layout(AudioStream->codec->channels);
    #else
        AudioStream->codec->channel_layout= AudioStream->codec->channels==2?AV_CH_LAYOUT_STEREO:AV_CH_LAYOUT_MONO;
    #endif

    int errcode=avcodec_open2(AudioStream->codec,Codec,&opts);
    if (errcode<0) {
        char Buf[2048];
        av_strerror(errcode,Buf,2048);
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenAudioStream: avcodec_open2() failed: "+QString(Buf));
        return false;
    }

    AudioFrame=avcodec_alloc_frame();
    if (AudioFrame==NULL) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-OpenAudioStream:: avcodec_alloc_frame failed"));
        return false;
    }

    return true;
}

//*************************************************************************************************************************************************

bool cEncodeVideo::PrepareTAG(QString Language) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::PrepareTAG");

    // Set TAGS
    av_dict_set(&Container->metadata,"language",Language.toUtf8().constData(),0);
    av_dict_set(&Container->metadata,"title",AdjustMETA(Diaporama->ProjectInfo->Title==""?QFileInfo(OutputFileName).baseName():Diaporama->ProjectInfo->Title).toUtf8().constData(),0);
    av_dict_set(&Container->metadata,"artist",AdjustMETA(Diaporama->ProjectInfo->Author).toUtf8().constData(),0);
    av_dict_set(&Container->metadata,"album",AdjustMETA(Diaporama->ProjectInfo->Album).toUtf8().constData(),0);
    av_dict_set(&Container->metadata,"comment",AdjustMETA(Diaporama->ProjectInfo->Comment).toUtf8().constData(),0);
    av_dict_set(&Container->metadata,"date",QString("%1").arg(Diaporama->ProjectInfo->Year).toUtf8().constData(),0);
    av_dict_set(&Container->metadata,"composer",QString(Diaporama->ApplicationConfig->ApplicationName+QString(" ")+Diaporama->ApplicationConfig->ApplicationVersion).toUtf8().constData(),0);
    av_dict_set(&Container->metadata,"creation_time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toUtf8().constData(),0); // ISO 8601 format

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
    if (SlideProgressBar)   SlideProgressBar->setValue(Position!=-1?int(double(Position-ColumnStart)/(double(AV_TIME_BASE)/dFPS/double(1000))):SlideProgressBar->maximum());
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

bool cEncodeVideo::DoEncode() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::DoEncode");

    bool                    Continue=true;
    cSoundBlockList         RenderMusic,ToEncodeMusic;
    QList<QImage *>         ImageList;
    cDiaporamaObjectInfo    *PreviousFrame=NULL;
    cDiaporamaObjectInfo    *Frame        =NULL;
    qlonglong               RenderedFrame =0;
    int                     FrameSize     =0;

    // Init RenderMusic and ToEncodeMusic
    RenderMusic.SetFPS(double(1000)/double(dFPS),2,AudioSampleRate,AV_SAMPLE_FMT_S16);
    if (AudioStream) {
        FrameSize=AudioStream->codec->frame_size;
        if ((!FrameSize)&&(AudioStream->codec->codec_id==AV_CODEC_ID_PCM_S16LE)) FrameSize=1024;
        if ((FrameSize==0)&&(VideoStream)) FrameSize=(AudioStream->codec->sample_rate*AudioStream->time_base.num)/AudioStream->time_base.den;
        #ifdef LIBAV_08
            // LIBAV 0.8 => ToEncodeMusic use AudioStream->codec->sample_fmt format
            int ComputedFrameSize=AudioStream->codec->channels*av_get_bytes_per_sample(AudioStream->codec->sample_fmt)*FrameSize;
            if (ComputedFrameSize==0) ComputedFrameSize=RenderMusic.SoundPacketSize;
            ToEncodeMusic.SetFrameSize(ComputedFrameSize,AudioStream->codec->channels,AudioSampleRate,AudioStream->codec->sample_fmt);
        #else
            // LIBAV 9 => ToEncodeMusic use AV_SAMPLE_FMT_S16 format
            int ComputedFrameSize=AudioChannels*av_get_bytes_per_sample(AV_SAMPLE_FMT_S16)*FrameSize;
            if (ComputedFrameSize==0) ComputedFrameSize=RenderMusic.SoundPacketSize;
            ToEncodeMusic.SetFrameSize(ComputedFrameSize,RenderMusic.Channels,AudioSampleRate,AV_SAMPLE_FMT_S16);
        #endif
    }

    CurAudioPts         =0;
    CurVideoPts         =0;
    LastAudioPts        =0;
    LastVideoPts        =0;
    IncreasingVideoPts  =double(1000)/double(dFPS);
    IncreasingAudioPts  =AudioStream?FrameSize*1000*AudioStream->codec->time_base.num/AudioStream->codec->time_base.den:0;
    StartTime           =QTime::currentTime();
    LastCheckTime       =StartTime;                                     // Display control : last time the loop start
    qlonglong Position  =Diaporama->GetObjectStartPosition(FromSlide);  // Render current position
    int ColumnStart     =-1;                                            // Render start position of current object
    int Column          =-1;                                            // Render current object

    // Init Resampler (if needed)
    if (AudioStream) {
        #ifdef LIBAV_08
            if ((AudioStream->codec->sample_fmt!=RenderMusic.SampleFormat)||(AudioStream->codec->channels!=RenderMusic.Channels)||(AudioSampleRate!=RenderMusic.SamplingRate)) {
                if (!AudioResamplerBuffer) {
                    AudioResamplerBuffer=(uint8_t *)av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE*ToEncodeMusic.Channels);
                    if (!AudioResamplerBuffer) {
                        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: AudioResamplerBuffer allocation failed"));
                        Continue=false;
                    }
                }
                if (!AudioResampler) AudioResampler=av_audio_resample_init(         // Context for resampling audio data
                    AudioStream->codec->channels,RenderMusic.Channels,              // output_channels, input_channels
                    AudioSampleRate,RenderMusic.SamplingRate,                       // output_rate, input_rate
                    AudioStream->codec->sample_fmt,RenderMusic.SampleFormat,        // sample_fmt_out, sample_fmt_in
                    0,                                                              // filter_length
                    0,                                                              // log2_phase_count
                    1,                                                              // linear
                    0);                                                             // cutoff
                if (!AudioResampler) {
                    ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: av_audio_resample_init failed"));
                    Continue=false;
                }
            }
        #else
            if ((AudioStream->codec->sample_fmt!=ToEncodeMusic.SampleFormat)||(AudioStream->codec->channels!=ToEncodeMusic.Channels)||(AudioSampleRate!=ToEncodeMusic.SamplingRate)) {
                if (!AudioResamplerBuffer) {
                    int out_linesize=0;
                    if (av_samples_alloc(&AudioResamplerBuffer,&out_linesize,ToEncodeMusic.Channels,AVCODEC_MAX_AUDIO_FRAME_SIZE,ToEncodeMusic.SampleFormat,1)<0) {
                        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: AudioResamplerBuffer allocation failed"));
                        Continue=false;
                    }
                }
                if (!AudioResampler) {
                    AudioResampler=avresample_alloc_context();
                    if (AudioResampler) {
                        av_opt_set_int(AudioResampler,"in_channel_layout",  av_get_default_channel_layout(ToEncodeMusic.Channels), 0);
                        av_opt_set_int(AudioResampler,"in_sample_fmt",      ToEncodeMusic.SampleFormat,                            0);
                        av_opt_set_int(AudioResampler,"in_sample_rate",     ToEncodeMusic.SamplingRate,                            0);
                        av_opt_set_int(AudioResampler,"out_channel_layout", AudioStream->codec->channel_layout,                    0);
                        av_opt_set_int(AudioResampler,"out_sample_fmt",     AudioStream->codec->sample_fmt,                        0);
                        av_opt_set_int(AudioResampler,"out_sample_rate",    AudioSampleRate,                       0);
                        if (avresample_open(AudioResampler)<0) {
                            ToLog(LOGMSG_CRITICAL,QString("Error opening context"));
                            Continue=false;
                        }
                    } else {
                        ToLog(LOGMSG_CRITICAL,QString("Error allocating AVAudioResampleContext"));
                        Continue=false;
                    }
                }
            }
        #endif
    }

    if (SlideProgressBar)   SlideProgressBar->setMaximum(100);
    if (TotalProgressBar)   TotalProgressBar->setMaximum(NbrFrame);

    QFutureWatcher<void> ThreadEncodeVideo;
    for (RenderedFrame=0;Continue && (RenderedFrame<NbrFrame);RenderedFrame++) {
        // Give time to interface!
        QApplication::processEvents();

        // Calculate position & column
        int AdjustedDuration=((Column>=0)&&(Column<Diaporama->List.count()))?Diaporama->List[Column]->GetDuration()-Diaporama->GetTransitionDuration(Column+1):0;
        if (AdjustedDuration<33) AdjustedDuration=33; // Not less than 1/30 sec

        if ((ColumnStart==-1)||(Column==-1)||((Column<Diaporama->List.count())&&((ColumnStart+AdjustedDuration)<=Position))) {
            Column++;
            AdjustedDuration=((Column>=0)&&(Column<Diaporama->List.count()))?Diaporama->List[Column]->GetDuration()-Diaporama->GetTransitionDuration(Column+1):0;
            if (AdjustedDuration<33) AdjustedDuration=33; // Not less than 1/30 sec
            ColumnStart=Diaporama->GetObjectStartPosition(Column);
            if ((Column<Diaporama->List.count())&&(SlideProgressBar)) SlideProgressBar->setMaximum(int(double(AdjustedDuration)/((double(AV_TIME_BASE)/dFPS)/double(1000)))-1);
            Diaporama->CloseUnusedLibAv(Column);
            DisplayProgress(RenderedFrame,Position,Column,ColumnStart);

        // Refresh Display (if needed)
        } else if (LastCheckTime.msecsTo(QTime::currentTime())>=1000) DisplayProgress(RenderedFrame,Position,Column,ColumnStart);  // Refresh display only one time per second

        // Get current frame
        Frame=new cDiaporamaObjectInfo(PreviousFrame,Position,Diaporama,double(1000)/double(dFPS));

        // Ensure MusicTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_MusicTrack==NULL)) {
            Frame->CurrentObject_MusicTrack=new cSoundBlockList();
            Frame->CurrentObject_MusicTrack->SetFPS(Frame->FrameDuration,2,AudioSampleRate,AV_SAMPLE_FMT_S16);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_MusicTrack==NULL)&&(Frame->TransitObject_MusicObject!=NULL)&&(Frame->TransitObject_MusicObject!=Frame->CurrentObject_MusicObject)) {
            Frame->TransitObject_MusicTrack=new cSoundBlockList();
            Frame->TransitObject_MusicTrack->SetFPS(Frame->FrameDuration,2,AudioSampleRate,AV_SAMPLE_FMT_S16);
        }

        // Ensure SoundTracks are ready
        if ((Frame->CurrentObject)&&(Frame->CurrentObject_SoundTrackMontage==NULL)) {
            Frame->CurrentObject_SoundTrackMontage=new cSoundBlockList();
            Frame->CurrentObject_SoundTrackMontage->SetFPS(Frame->FrameDuration,2,AudioSampleRate,AV_SAMPLE_FMT_S16);
        }
        if ((Frame->TransitObject)&&(Frame->TransitObject_SoundTrackMontage==NULL)) {
            Frame->TransitObject_SoundTrackMontage=new cSoundBlockList();
            Frame->TransitObject_SoundTrackMontage->SetFPS(Frame->FrameDuration,2,AudioSampleRate,AV_SAMPLE_FMT_S16);
        }

        // Prepare frame (if W=H=0 then soundonly)
        Diaporama->LoadSources(Frame,InternalWidth,InternalHeight,false,true);                                        // Load source images
        Diaporama->DoAssembly(ComputePCT(Frame->CurrentObject?Frame->CurrentObject->GetSpeedWave():0,Frame->TransitionPCTDone),Frame,InternalWidth,InternalHeight); // Make final assembly

        // Prepare data
        if ((Continue)&&(AudioStream)) {
            // Calc number of packet to mix
            int MaxPacket=Frame->CurrentObject_MusicTrack->List.count();
            if ((Frame->CurrentObject_SoundTrackMontage!=NULL)&&
                (Frame->CurrentObject_SoundTrackMontage->List.count()>0)&&
                (MaxPacket>Frame->CurrentObject_SoundTrackMontage->List.count())) MaxPacket=Frame->CurrentObject_SoundTrackMontage->List.count();
            if (MaxPacket>RenderMusic.NbrPacketForFPS) MaxPacket=RenderMusic.NbrPacketForFPS;

            // mix audio data
            for (int j=0;j<MaxPacket;j++)
                RenderMusic.MixAppendPacket(Frame->CurrentObject_MusicTrack->DetachFirstPacket(),Frame->CurrentObject_SoundTrackMontage->DetachFirstPacket());

            // Transfert RenderMusic data to EncodeMusic data
            while ((Continue)&&(RenderMusic.List.count()>0)) {
                uint8_t *PacketSound=(uint8_t *)RenderMusic.DetachFirstPacket();
                if (PacketSound==NULL) {
                    PacketSound=(uint8_t *)av_malloc(RenderMusic.SoundPacketSize+4);
                    memset(PacketSound,0,RenderMusic.SoundPacketSize);
                }
                #ifdef LIBAV_08
                    // LIBAV 0.8 => ToEncodeMusic must have exactly AudioStream->codec->frame_size data
                    if ((AudioResampler!=NULL)&&(AudioResamplerBuffer!=NULL)) {
                        int64_t DestNbrSamples=RenderMusic.SoundPacketSize/(RenderMusic.Channels*av_get_bytes_per_sample(RenderMusic.SampleFormat));
                        int64_t DestPacketSize=audio_resample(AudioResampler,(short int*)AudioResamplerBuffer,(short int*)PacketSound,DestNbrSamples)*AudioStream->codec->channels*av_get_bytes_per_sample(AudioStream->codec->sample_fmt);
                        if (DestPacketSize<=0) {
                            ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: audio_resample failed"));
                            Continue=false;
                        } else ToEncodeMusic.AppendData((int16_t *)AudioResamplerBuffer,DestPacketSize);
                    } else ToEncodeMusic.AppendData((int16_t *)PacketSound,RenderMusic.SoundPacketSize);
                #else
                    // LIBAV 9 => ToEncodeMusic is converted during encoding process
                    ToEncodeMusic.AppendData((int16_t *)PacketSound,RenderMusic.SoundPacketSize);
                #endif
                av_free(PacketSound);
            }
        }

        // Write data to disk
        if (Continue) {
            if (VideoStream) ImageList.append(new QImage(Frame->RenderedImage->convertToFormat(QTPIXFMT)));
            //QImage *Image=(VideoStream?new QImage(Frame->RenderedImage->convertToFormat(QTPIXFMT)):NULL);
            if (ThreadEncodeVideo.isRunning()) ThreadEncodeVideo.waitForFinished();
            ThreadEncodeVideo.setFuture(QtConcurrent::run(this,&cEncodeVideo::EncodeVideo,&ToEncodeMusic,&ImageList,&Continue));
        }

        // Calculate next position
        if (Continue) {
            Position+=double(1000)/dFPS;
            if (PreviousFrame!=NULL) delete PreviousFrame;
            PreviousFrame=Frame;
            Frame =NULL;
        }

        // Stop the process if error occur or user ask to stop
        Continue=Continue && !StopProcessWanted;
    }

    if (ThreadEncodeVideo.isRunning()) ThreadEncodeVideo.waitForFinished();

    // refresh display
    if (!StopProcessWanted) DisplayProgress(RenderedFrame,-1,Column,0);  // Set All to 100%

    // Cleaning
    if (PreviousFrame!=NULL) delete PreviousFrame;
    if (Frame!=NULL)         delete Frame;

    CloseEncoder();

    return Continue;
}

//*************************************************************************************************************************************************

void cEncodeVideo::EncodeMusic(cSoundBlockList *ToEncodeMusic,bool *Continue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::EncodeMusic");

    int     errcode;
    int64_t DestNbrSamples=ToEncodeMusic->SoundPacketSize/(ToEncodeMusic->Channels*av_get_bytes_per_sample(ToEncodeMusic->SampleFormat));
    int     DestSampleSize=AudioStream->codec->channels*av_get_bytes_per_sample(AudioStream->codec->sample_fmt);
    uint8_t *DestPacket   =NULL;
    int16_t *PacketSound  =NULL;
    int64_t DestPacketSize=DestNbrSamples*DestSampleSize;

    // Flush audio frame of ToEncodeMusic
    while ((Continue)&&(ToEncodeMusic->List.count()>0)&&(!StopProcessWanted)&&((!VideoStream)||(LastAudioPts<(LastVideoPts+IncreasingVideoPts)))) {
        int out_samples=DestNbrSamples;
        PacketSound=ToEncodeMusic->DetachFirstPacket();
        if (PacketSound==NULL) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: PacketSound==NULL"));
            *Continue=false;
        } else {
            #ifdef LIBAV_09
                // LIBAV 9 => Convert sample format (is needed)
                if ((AudioResampler!=NULL)&&(AudioResamplerBuffer!=NULL)) {
                    DestPacket=AudioResamplerBuffer;
                    uint8_t *in_data[AVRESAMPLE_MAX_CHANNELS]={0},*out_data[AVRESAMPLE_MAX_CHANNELS]={0};
                    int     in_linesize=0,out_linesize=0;
                    out_samples=avresample_available(AudioResampler)+av_rescale_rnd(avresample_get_delay(AudioResampler)+DestNbrSamples,AudioStream->codec->sample_rate,ToEncodeMusic->SamplingRate,AV_ROUND_UP);
                    if (av_samples_fill_arrays(in_data,&in_linesize,(uint8_t *)PacketSound,ToEncodeMusic->Channels,DestNbrSamples,ToEncodeMusic->SampleFormat,1)<0) {
                        ToLog(LOGMSG_CRITICAL,QString("failed in_data fill arrays"));
                        Continue=false;
                    } else {
                        if (av_samples_fill_arrays(out_data,&out_linesize,AudioResamplerBuffer,AudioStream->codec->channels,out_samples,AudioStream->codec->sample_fmt,1)<0) {
                            ToLog(LOGMSG_CRITICAL,QString("failed out_data fill arrays"));
                            Continue=false;
                        } else {
                            DestPacketSize=avresample_convert(AudioResampler,out_data,out_linesize,out_samples,in_data,in_linesize,DestNbrSamples)*DestSampleSize;
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

            // Init AudioFrame
            avcodec_get_frame_defaults(AudioFrame);
            AudioFrame->nb_samples      =DestPacketSize/DestSampleSize;
            AudioFrame->pts             =AudioFrame->nb_samples*CurAudioPts;

            #ifdef LIBAV_09
            AudioFrame->format          =AudioStream->codec->sample_fmt;
            AudioFrame->channel_layout  =AudioStream->codec->channel_layout;
            AudioFrame->sample_rate     =AudioStream->codec->sample_rate;
            #endif

            // fill buffer
            errcode=avcodec_fill_audio_frame(AudioFrame,AudioStream->codec->channels,AudioStream->codec->sample_fmt,(const uint8_t*)DestPacket,DestPacketSize,1);
            if (errcode>=0) {
                // Init packet
                AVPacket pkt;
                av_init_packet(&pkt);
                pkt.size=0;
                pkt.data=NULL;

                int got_packet=0;
                errcode=avcodec_encode_audio2(AudioStream->codec,&pkt,AudioFrame,&got_packet);
                if (errcode<0) {
                    char Buf[2048];
                    av_strerror(errcode,Buf,2048);
                    ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: avcodec_encode_audio2() failed: ")+QString(Buf));
                    *Continue=false;
                } else if (got_packet) {
                    pkt.flags|=AV_PKT_FLAG_KEY;
                    #ifdef LIBAV_08
                    if (AudioStream->codec->coded_frame && AudioStream->codec->coded_frame->pts!=(int64_t)AV_NOPTS_VALUE)
                        pkt.pts=av_rescale_q(AudioStream->codec->coded_frame->pts,AudioStream->codec->time_base,AudioStream->time_base);
                    #else
                    if (pkt.dts!=(int64_t)AV_NOPTS_VALUE) pkt.dts=av_rescale_q(pkt.dts,AudioStream->codec->time_base,AudioStream->time_base);
                    if (pkt.pts!=(int64_t)AV_NOPTS_VALUE) pkt.pts=av_rescale_q(pkt.pts,AudioStream->codec->time_base,AudioStream->time_base);
                    #endif

                    // write the compressed frame in the media file
                    pkt.stream_index=AudioStream->index;
                    errcode=av_interleaved_write_frame(Container,&pkt);
                    if (errcode!=0) {
                        char Buf[2048];
                        av_strerror(errcode,Buf,2048);
                        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: av_interleaved_write_frame failed: ")+QString(Buf));
                        *Continue=false;
                    }
                }
                LastAudioPts+=IncreasingAudioPts;
                CurAudioPts++;
                //ToLog(LOGMSG_INFORMATION,QString("Audio:  Stream:%1 - Frame:%2 - PTS:%3").arg(AudioStream->index).arg(CurAudioPts).arg(LastAudioPts));
            } else {
                char Buf[2048];
                av_strerror(errcode,Buf,2048);
                ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: avcodec_fill_audio_frame() failed: ")+QString(Buf));
                *Continue=false;
            }
            if ((AudioFrame->extended_data)&&(AudioFrame->extended_data!=AudioFrame->data)) {
                av_free(AudioFrame->extended_data);
                AudioFrame->extended_data=NULL;
            }
        }
        av_free(PacketSound);
    }
    avcodec_flush_buffers(AudioStream->codec);
}

//*************************************************************************************************************************************************

void cEncodeVideo::EncodeVideo(cSoundBlockList *ToEncodeMusic,QList<QImage *>*ImageList,bool *Continue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::EncodeVideo");
    if ((AudioStream)&&(AudioFrame)) EncodeMusic(ToEncodeMusic,Continue);

    if ((VideoStream)&&(ImageList->count()>0)&&(VideoFrameConverter)&&(VideoFrame)) {
        QImage *Image=ImageList->takeFirst();
        avcodec_get_frame_defaults(VideoFrame);
        if (avpicture_fill(
            (AVPicture *)VideoFrame,            // Frame to prepare
            VideoFrameBuf,                      // Buffer which will contain the image data
            VideoStream->codec->pix_fmt,        // The format in which the picture data is stored (see http://wiki.aasimon.org/doku.php?id=ffmpeg:pixelformat)
            VideoStream->codec->width,          // The width of the image in pixels
            VideoStream->codec->height          // The height of the image in pixels
        )!=VideoFrameBufSize) {
            ToLog(LOGMSG_CRITICAL,"EncodeVideo-EncodeVideo: avpicture_fill() failed for VideoFrameBuf");
            return;
        }

        if ((CurVideoPts%VideoStream->codec->gop_size)==0) VideoFrame->pict_type=AV_PICTURE_TYPE_I;
        VideoFrame->pts=CurVideoPts;

        // Apply ExtendV
        if ((*Continue)&&(!StopProcessWanted)&&(Image->height()!=VideoStream->codec->height)) {
            QImage *NewImage=new QImage(VideoStream->codec->width,VideoStream->codec->height,QTPIXFMT);
            QPainter P;
            P.begin(NewImage);
            P.fillRect(QRect(0,0,NewImage->width(),NewImage->height()),Qt::black);
            P.drawImage(0,(NewImage->height()-Image->height())/2,*Image);
            P.end();
            delete Image;
            Image=NewImage;
        }

        // Now, convert image
        if ((*Continue)&&(!StopProcessWanted)) {
            uint8_t *data   =(uint8_t *)Image->bits();
            int     LineSize=Image->bytesPerLine();
            int Ret=sws_scale(
                VideoFrameConverter,    // libswscale converter
                &data,                  // Source buffer
                &LineSize,              // Source Stride ?
                0,                      // Source SliceY:the position in the source image of the slice to process, that is the number (counted starting from zero) in the image of the first row of the slice
                Image->height(),        // Source SliceH:the height of the source slice, that is the number of rows in the slice
                VideoFrame->data,       // Destination buffer
                VideoFrame->linesize    // Destination Stride
            );
            if (Ret!=Image->height()) {
                ToLog(LOGMSG_CRITICAL,"EncodeVideo-ConvertRGBToYUV: sws_scale() failed");
                *Continue=false;
            }
        }

        if (Image) delete Image;

        if ((*Continue)&&(!StopProcessWanted)) {

            AVPacket pkt;
            av_init_packet(&pkt);
            pkt.size=0;
            pkt.data=NULL;

            #ifdef LIBAV_08
            int out_size=avcodec_encode_video(VideoStream->codec,VideoEncodeBuffer,VideoEncodeBufferSize,VideoFrame);
            if (out_size<0) {
                ToLog(LOGMSG_CRITICAL,QString("EncodeVideo: avcodec_encode_video failed"));
                *Continue=false;
            } else if (out_size>0) {
                pkt.data=VideoEncodeBuffer;
                pkt.size=out_size;
                if (VideoStream->codec->coded_frame && VideoStream->codec->coded_frame->pts!=(int64_t)AV_NOPTS_VALUE)
                    pkt.pts=av_rescale_q(VideoStream->codec->coded_frame->pts,VideoStream->codec->time_base,VideoStream->time_base);
            #else
            int got_packet=0;
            int errcode=avcodec_encode_video2(VideoStream->codec,&pkt,VideoFrame,&got_packet);
            if (errcode!=0) {
                char Buf[2048];
                av_strerror(errcode,Buf,2048);
                ToLog(LOGMSG_CRITICAL,QString("EncodeVideo: avcodec_encode_video2 failed")+QString(Buf));
                *Continue=false;
            } else if (got_packet) {
                if (pkt.dts!=(int64_t)AV_NOPTS_VALUE) pkt.dts=av_rescale_q(pkt.dts,VideoStream->codec->time_base,VideoStream->time_base);
                if (pkt.pts!=(int64_t)AV_NOPTS_VALUE) pkt.pts=av_rescale_q(pkt.pts,VideoStream->codec->time_base,VideoStream->time_base);
            #endif

                pkt.stream_index=VideoStream->index;
                if (VideoStream->codec->coded_frame && VideoStream->codec->coded_frame->key_frame) pkt.flags|=AV_PKT_FLAG_KEY;

                // write the compressed frame in the media file
                int errcode=av_interleaved_write_frame(Container,&pkt);
                if (errcode!=0) {
                    char Buf[2048];
                    av_strerror(errcode,Buf,2048);
                    ToLog(LOGMSG_CRITICAL,QString("EncodeVideo: av_interleaved_write_frame failed: ")+QString(Buf));
                    *Continue=false;
                }
            }

            LastVideoPts+=IncreasingVideoPts;
            CurVideoPts++;
            //ToLog(LOGMSG_INFORMATION,QString("Video:  Stream:%1 - Frame:%2 - PTS:%3").arg(VideoStream->index).arg(CurVideoPts).arg(LastVideoPts));

            if ((VideoFrame->extended_data)&&(VideoFrame->extended_data!=VideoFrame->data)) {
                av_free(VideoFrame->extended_data);
                VideoFrame->extended_data=NULL;
            }
        }
        avcodec_flush_buffers(VideoStream->codec);
    }
}
