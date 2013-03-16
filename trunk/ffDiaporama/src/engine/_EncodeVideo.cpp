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
        case VFORMAT_3GP:
            if (((VideoCodec==AV_CODEC_ID_MPEG4)||(VideoCodec==AV_CODEC_ID_H264))&&((AudioCodec==AV_CODEC_ID_AMR_NB)||(AudioCodec==AV_CODEC_ID_AMR_WB)))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_MJPEG:
            if ((VideoCodec==AV_CODEC_ID_MJPEG)&&(AudioCodec==AV_CODEC_ID_PCM_S16LE))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_OGV:
            if ((VideoCodec==AV_CODEC_ID_THEORA)&&(AudioCodec==AV_CODEC_ID_VORBIS))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_WEBM:
            if ((VideoCodec==AV_CODEC_ID_VP8)&&(AudioCodec==AV_CODEC_ID_VORBIS))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_OLDFLV:
            if ((VideoCodec==AV_CODEC_ID_FLV1)&&(AudioCodec==AV_CODEC_ID_MP3))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_FLV:
            if ((VideoCodec==AV_CODEC_ID_H264)&&(AudioCodec==AV_CODEC_ID_AAC))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_MPEG:
            if ((VideoCodec==AV_CODEC_ID_MPEG2VIDEO)&&((AudioCodec==AV_CODEC_ID_AC3)||(AudioCodec==AV_CODEC_ID_MP2)))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_AVI:
            if (((VideoCodec==AV_CODEC_ID_MPEG4)||(VideoCodec==AV_CODEC_ID_MJPEG)||(VideoCodec==AV_CODEC_ID_H264))||
                ((VideoCodec==AV_CODEC_ID_MPEG2VIDEO)&&((AudioCodec==AV_CODEC_ID_MP2)||(AudioCodec==AV_CODEC_ID_AC3)||(AudioCodec==AV_CODEC_ID_MP3)||(AudioCodec==AV_CODEC_ID_PCM_S16LE))))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_MP4:
            if (((VideoCodec==AV_CODEC_ID_MPEG4)||(VideoCodec==AV_CODEC_ID_H264))&&((AudioCodec==AV_CODEC_ID_MP3)||(AudioCodec==AV_CODEC_ID_AAC)))
                Ret=SUPPORTED_COMBINATION;
            break;
        case VFORMAT_MKV:
            Ret=SUPPORTED_COMBINATION;
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
                pkt.pts =AudioFrameNbr++;
                pkt.dts =AV_NOPTS_VALUE;
                avcodec_encode_audio2(AudioStream->codec,&pkt,NULL,&got_packet);
                avcodec_flush_buffers(AudioStream->codec);
            }
            /*
            if (VideoStream) {
                #ifdef LIBAV_09
                pkt.data=NULL;
                pkt.size=0;
                pkt.pts =VideoFrameNbr++;
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
            #if defined(USELIBAVRESAMPLE)
                avresample_close(AudioResampler);
                avresample_free(&AudioResampler);
            #elif defined(USELIBSWRESAMPLE)
                swr_free(&AudioResampler);
            #endif
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

int cEncodeVideo::getThreadFlags(CodecID codecId) {
    int Ret=0;
    switch (codecId) {
        case CODEC_ID_PRORES:
        case CODEC_ID_MPEG1VIDEO:
        case CODEC_ID_DVVIDEO:
        case CODEC_ID_MPEG2VIDEO:   Ret=FF_THREAD_SLICE;                    break;
        case CODEC_ID_H264 :        Ret=FF_THREAD_FRAME|FF_THREAD_SLICE;    break;
        default:                    Ret=FF_THREAD_FRAME;                    break;
    }
    return Ret;
}

//*************************************************************************************************************************************************

bool cEncodeVideo::OpenEncoder(cDiaporama *Diaporama,QString OutputFileName,int FromSlide,int ToSlide,
                    bool EncodeVideo,int VideoCodecSubId,sIMAGEDEF *ImageDef,int ImageWidth,int ImageHeight,int ExtendV,int InternalWidth,int InternalHeight,AVRational PixelAspectRatio,int VideoBitrate,
                    bool EncodeAudio,int AudioCodecSubId,int AudioChannels,int AudioBitrate,int AudioSampleRate,QString Language) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::OpenEncoder");

    sFormatDef *FormatDef   =NULL;
    this->Diaporama         =Diaporama;
    this->OutputFileName    =OutputFileName;
    this->FromSlide         =FromSlide;
    this->ToSlide           =ToSlide;
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
        this->VideoBitrate   =VideoBitrate;
        this->ImageDef       =ImageDef;
        this->VideoFrameRate =ImageDef->AVFPS;
        this->VideoCodecSubId=VideoCodecSubId;
        this->InternalWidth  =InternalWidth;
        this->InternalHeight =InternalHeight;
        this->ExtendV        =ExtendV;

        // Add stream
        if (!OpenVideoStream(&VIDEOCODECDEF[VideoCodecSubId],VideoCodecSubId,VideoFrameRate,ImageWidth,ImageHeight+ExtendV,PixelAspectRatio,VideoBitrate))
            return false;

    } else {
        // If sound only, ensure FrameRate have a value
        VideoFrameRate=(AVRational){1,25};
    }

    //=======================================
    // Open Audio stream
    //=======================================

    if (EncodeAudio) {
        // Audio parameters
        this->AudioChannels  =AudioChannels;
        this->AudioBitrate   =AudioBitrate;
        this->AudioSampleRate=AudioSampleRate;
        this->AudioCodecSubId=AudioCodecSubId;

        // Add stream
        if (!OpenAudioStream(&AUDIOCODECDEF[AudioCodecSubId],AudioChannels,AudioBitrate,AudioSampleRate,Language))
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
    int mux_preload=int(0.5*AV_TIME_BASE);
    int mux_max_delay=int(0.7*AV_TIME_BASE);
    int mux_rate=0;
    int packet_size=-1;

    if (QString(Container->oformat->name)==QString("mpegts")) {
        packet_size =188;
        mux_rate    =int(VideoStream->codec->bit_rate*1.1);
    } else if (QString(Container->oformat->name)==QString("matroska")) {
        mux_rate     =10080*1000;
        mux_preload  =AV_TIME_BASE/10;  // 100 ms preloading
        mux_max_delay=200*1000;         // 500 ms
    } else if (QString(Container->oformat->name)==QString("webm")) {
        mux_rate     =10080*1000;
        mux_preload  =AV_TIME_BASE/10;  // 100 ms preloading
        mux_max_delay=200*1000;         // 500 ms
    }
    Container->flags   |=AVFMT_FLAG_NONBLOCK;
    Container->max_delay=mux_max_delay;
    av_opt_set_int(Container,"preload",mux_preload,AV_OPT_SEARCH_CHILDREN);
    av_opt_set_int(Container,"muxrate",mux_rate,AV_OPT_SEARCH_CHILDREN);
    if (packet_size!=-1) Container->packet_size=packet_size;

    if (avformat_write_header(Container,NULL)<0) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-OpenEncoder: avformat_write_header() failed");
        return false;
    }

    //********************************************
    // Log output format
    //********************************************
    av_dump_format(Container,0,OutputFileName.toUtf8().constData(),1);
    IsOpen=true;

    //=======================================
    // Init counter
    //=======================================

    dFPS    =double(VideoFrameRate.den)/double(VideoFrameRate.num);
    NbrFrame=int(double(Diaporama->GetPartialDuration(FromSlide,ToSlide))*dFPS/1000);    // Number of frame to generate

    //=======================================
    // Prepare Display
    //=======================================
    InitDisplay();

    return true;
}

//*************************************************************************************************************************************************
// Create a stream
//*************************************************************************************************************************************************

bool cEncodeVideo::AddStream(AVStream **Stream,AVCodec **codec,const char *CodecName,AVMediaType Type) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::AddStream");

    *codec=avcodec_find_encoder_by_name(CodecName);
    if (!(*codec)) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-AddStream: Unable to find codec %1").arg(CodecName));
        return false;
    }
    if ((*codec)->id==AV_CODEC_ID_NONE) {
        ToLog(LOGMSG_CRITICAL,QString("EncodeVideo-AddStream: codec->id==AV_CODEC_ID_NONE"));
        return false;
    }

    // Create stream
    *Stream=avformat_new_stream(Container,*codec);
    if (!(*Stream)) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-AddStream: avformat_new_stream() failed");
        return false;
    }
    (*Stream)->codec->codec_type=Type;
    (*Stream)->codec->codec_id  =(*codec)->id;

    // Setup encoder context for stream
    if (avcodec_get_context_defaults3((*Stream)->codec,*codec)<0) {
        ToLog(LOGMSG_CRITICAL,"EncodeVideo-AddStream: avcodec_get_context_defaults3() failed");
        return false;
    }

    if (Type==AVMEDIA_TYPE_VIDEO) Container->oformat->video_codec=(*codec)->id;
        else if (Type==AVMEDIA_TYPE_AUDIO) Container->oformat->audio_codec=(*codec)->id;

    (*Stream)->codec->codec_type=Type;          // set again
    (*Stream)->codec->codec_id  =(*codec)->id;  // set again

    if ((Container->oformat->flags & AVFMT_GLOBALHEADER)
            ||(!strcmp(Container->oformat->name,"mp4"))
            //||(!strcmp(Container->oformat->name,"mov"))
            ||(!strcmp(Container->oformat->name,"mpegts"))
            ||(!strcmp(Container->oformat->name,"3gp"))
        )
        (*Stream)->codec->flags|=CODEC_FLAG_GLOBAL_HEADER;

    int ThreadC =((getCpuCount()-1)>1)?(getCpuCount()-1):1;
    if (ThreadC>0) (*Stream)->codec->thread_count=ThreadC;
    (*Stream)->codec->thread_type=getThreadFlags((*codec)->id);

    return true;
}

//*************************************************************************************************************************************************
// Create video streams
//*************************************************************************************************************************************************

bool cEncodeVideo::OpenVideoStream(sVideoCodecDef *VideoCodecDef,int VideoCodecSubId,AVRational VideoFrameRate,
                                   int ImageWidth,int ImageHeight,AVRational PixelAspectRatio,int VideoBitrate) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::OpenVideoStream");

    AVCodec *codec;
    if (!AddStream(&VideoStream,&codec,VideoCodecDef->ShortName,AVMEDIA_TYPE_VIDEO)) return false;

    AVDictionary *opts=NULL;
    int MinRate=-1;
    int MaxRate=-1;
    int BufSize=-1;
    int BFrames=-1;

    // Setup codec parameters
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
        VideoStream->codec->qmin   =2;
        VideoStream->codec->qmax   =2;

    } else if (codec->id==AV_CODEC_ID_VP8) {

        BFrames=3;
        VideoStream->codec->gop_size=120;                                   av_dict_set(&opts,"g",QString("%1").arg(VideoStream->codec->gop_size).toUtf8(),0);
        VideoStream->codec->qmax    =ImageHeight<=576?63:51;                av_dict_set(&opts,"qmax",QString("%1").arg(VideoStream->codec->qmax).toUtf8(),0);
        VideoStream->codec->qmin    =ImageHeight<=576?1:11;                 av_dict_set(&opts,"qmin",QString("%1").arg(VideoStream->codec->qmin).toUtf8(),0);
        VideoStream->codec->mb_lmin =VideoStream->codec->qmin*FF_QP2LAMBDA;
        VideoStream->codec->lmin    =VideoStream->codec->qmin*FF_QP2LAMBDA;
        VideoStream->codec->mb_lmax =VideoStream->codec->qmax*FF_QP2LAMBDA;
        VideoStream->codec->lmax    =VideoStream->codec->qmax*FF_QP2LAMBDA;

        if (ImageHeight<=720) av_dict_set(&opts,"profile","0",0); else av_dict_set(&opts,"profile","1",0);
        if (ImageHeight>576)  av_dict_set(&opts,"slices","4",0);

        av_dict_set(&opts,"lag-in-frames","16",0);
        av_dict_set(&opts,"deadline","good",0);
        if (VideoStream->codec->thread_count>0) av_dict_set(&opts,"cpu-used",QString("%1").arg(VideoStream->codec->thread_count).toUtf8(),0);

        #if defined(LIBAV_08)
            MinRate=VideoBitrate;
            MaxRate=VideoBitrate;
            BufSize=VideoBitrate;
        #endif

    } else if (codec->id==AV_CODEC_ID_H264) {
        // -vcodec libx264 -vsync vfr -pix_fmt yuv420p -b:0 1500000 -minrate 1350000 -maxrate 1650000 -bufsize 3000000
        // -preset veryfast -refs:0 3 -r 25  -sws_flags bicubic
        // Stream #0.0: Video: libx264, yuv420p, 640x360 [PAR 1:1 DAR 16:9], q=-1--1, 1500 kb/s, 1k tbn, 25 tbc
        // Stream #0.1: Audio: libfaac, 44100 Hz, stereo, 159 kb/s
        // cabac=1 ref=3 deblock=1:0:0 analyse=0x1:0x111 me=hex subme=2 psy=1 psy_rd=1.00:0.00 mixed_ref=0
        // me_range=16 chroma_me=1 trellis=0 8x8dct=0 cqm=0 deadzone=21,11 fast_pskip=1 chroma_qp_offset=0
        // threads=3 sliced_threads=0 nr=0 decimate=1 interlaced=0 bluray_compat=0 constrained_intra=0 bframes=3
        // b_pyramid=0 b_adapt=1 b_bias=0 direct=1 weightb=0 open_gop=1 weightp=1 keyint=250 keyint_min=25
        // scenecut=40 intra_refresh=0 rc_lookahead=10 rc=abr mbtree=1 bitrate=1500 ratetol=1.0 qcomp=0.60
        // qpmin=0 qpmax=69 qpstep=4 vbv_maxrate=1650 vbv_bufsize=3000 nal_hrd=none ip_ratio=1.25 aq=1:1.00
        // profile Main, level 3.0
        av_dict_set(&opts,"preset","veryfast",0);
        VideoStream->codec->gop_size=250;           av_dict_set(&opts,"g",QString("%1").arg(VideoStream->codec->gop_size).toUtf8(),0);
        VideoStream->codec->qmin    =0;             av_dict_set(&opts,"qmin",QString("%1").arg(VideoStream->codec->qmin).toUtf8(),0);
        VideoStream->codec->qmax    =69;            av_dict_set(&opts,"qmax",QString("%1").arg(VideoStream->codec->qmax).toUtf8(),0);
        if (VideoStream->codec->thread_count>0)     av_dict_set(&opts,"threads",QString("%1").arg(VideoStream->codec->thread_count).toUtf8(),0);

        switch (VideoCodecSubId) {
            case VCODEC_H264HQ:     // High Quality H.264 AVC/MPEG-4 AVC
                av_dict_set(&opts,"refs",    "3",          0);
                av_dict_set(&opts,"vprofile","main",       0);
                av_dict_set(&opts,"tune",    "zerolatency",0);
                BFrames=3;
                MinRate=int(double(VideoBitrate)*0.9);
                MaxRate=int(double(VideoBitrate)*1.1);
                BufSize=int(double(VideoBitrate)*2);
                break;

            case VCODEC_H264PQ:     // Phone Quality H.264 AVC/MPEG-4 AVC
                av_dict_set(&opts,"refs","3",           0);
                av_dict_set(&opts,"vprofile","baseline",0);
                av_dict_set(&opts,"tune","fastdecode",  0);
                MinRate=int(double(VideoBitrate)*0.9);
                MaxRate=int(double(VideoBitrate)*1.1);
                BufSize=int(double(VideoBitrate)*2);
                break;

            case VCODEC_X264LL:     // x264 lossless
                av_dict_set(&opts,"refs","3",0);
                av_dict_set(&opts,"qp",  "0",0);
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

bool cEncodeVideo::OpenAudioStream(sAudioCodecDef *AudioCodecDef,int &AudioChannels,int &AudioBitrate,int &AudioSampleRate,QString Language) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::OpenAudioStream");

    AVCodec *codec;
    if (!AddStream(&AudioStream,&codec,AudioCodecDef->ShortName,AVMEDIA_TYPE_AUDIO)) return false;

    AVDictionary    *opts   =NULL;

    // Setup codec parameters
    AudioStream->codec->sample_fmt  =AV_SAMPLE_FMT_S16;
    AudioStream->codec->channels    =AudioChannels;
    AudioStream->codec->sample_rate =AudioSampleRate;

    av_dict_set(&AudioStream->metadata,"language",Language.toUtf8().constData(),0);

    if (codec->id==AV_CODEC_ID_PCM_S16LE) {

        AudioBitrate=AudioSampleRate*16*AudioChannels;

    } else if (codec->id==AV_CODEC_ID_FLAC) {
        av_dict_set(&opts,"lpc_coeff_precision","15",0);
        av_dict_set(&opts,"lpc_type","2",0);
        av_dict_set(&opts,"lpc_passes","1",0);
        av_dict_set(&opts,"min_partition_order","0",0);
        av_dict_set(&opts,"max_partition_order","8",0);
        av_dict_set(&opts,"prediction_order_method","0",0);
        av_dict_set(&opts,"ch_mode","-1",0);

    } else if (codec->id==AV_CODEC_ID_AAC) {
        //VideoStream->codec->profile=FF_PROFILE_AAC_MAIN;
        if (QString(AUDIOCODECDEF[2].ShortName)=="aac")
            AudioStream->codec->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;

    } else if (codec->id==AV_CODEC_ID_MP2) {

    } else if (codec->id==AV_CODEC_ID_MP3) {
        #ifdef LIBAV_09
        AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_S16P;
        #endif
        av_dict_set(&opts,"reservoir","1",0);

    } else if (codec->id==AV_CODEC_ID_VORBIS) {
        #ifdef LIBAV_09
        AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLTP;
        #endif

    } else if (codec->id==AV_CODEC_ID_AC3) {
        #ifdef LIBAV_09
        AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLTP;
        #else
        AudioStream->codec->sample_fmt =AV_SAMPLE_FMT_FLT;
        #endif
    } else if (codec->id==AV_CODEC_ID_AMR_NB) {
        AudioStream->codec->channels=1;
        AudioChannels=1;
    } else if (codec->id==AV_CODEC_ID_AMR_WB) {
        AudioStream->codec->channels=1;
        AudioChannels=1;
    }
    AudioStream->codec->bit_rate    =AudioBitrate;
    av_dict_set(&opts,"ab",QString("%1").arg(AudioBitrate).toUtf8(),0);
    AudioStream->codec->channel_layout=(AudioStream->codec->channels==2?AV_CH_LAYOUT_STEREO:AV_CH_LAYOUT_MONO);

    int errcode=avcodec_open2(AudioStream->codec,codec,&opts);
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

    // Set Chapters (only if video stream)
    if (VideoStream) {
        for (int i=FromSlide;i<=ToSlide;i++) if ((i==FromSlide)||(Diaporama->List[i]->StartNewChapter)) {
            AVChapter *Chapter=(AVChapter *)av_mallocz(sizeof(AVChapter));
            int64_t   Start   =Diaporama->GetObjectStartPosition(i)+(i>FromSlide?Diaporama->List[i]->GetTransitDuration():0)-Diaporama->GetObjectStartPosition(FromSlide);
            int64_t   Duration=Diaporama->List[i]->GetDuration()-(i>FromSlide?Diaporama->List[i]->GetTransitDuration():0);
            int       NextC   =i+1;
            while ((NextC<ToSlide)&&(!Diaporama->List[NextC]->StartNewChapter)) {
                Duration=Duration+Diaporama->List[NextC]->GetDuration();
                NextC++;
                if (NextC<=ToSlide) Duration=Duration-Diaporama->List[NextC-1]->GetTransitDuration();
            }
            int64_t   End     =Start+Duration;
            int64_t   ts_off  =av_rescale_q(Container->start_time,AV_TIME_BASE_Q,VideoStream->time_base);
            Chapter->id       =Container->nb_chapters;
            Chapter->time_base=VideoStream->time_base;
            Chapter->start    =av_rescale_q((Start-ts_off)*1000,AV_TIME_BASE_Q,VideoStream->time_base);
            Chapter->end      =av_rescale_q((End-ts_off)*1000,AV_TIME_BASE_Q,VideoStream->time_base);
            av_dict_set(&Chapter->metadata,"title",Diaporama->List[i]->SlideName.toUtf8(),0);
            Container->chapters=(AVChapter **)av_realloc(Container->chapters,sizeof(AVChapter)*(Container->nb_chapters+1));
            Container->chapters[Container->nb_chapters]=Chapter;
            Container->nb_chapters++;
        }
    }

    return true;
}

//*************************************************************************************************************************************************

void cEncodeVideo::InitDisplay() {
    SlideNumberLabel->setText("");
    SlideProgressBar->setValue(0);
    FrameNumberLabel->setText("");
    TotalProgressBar->setValue(0);
    ElapsedTimeLabel->setText("");
    FPSLabel->setText("");

    // Filename
    InfoLabelB1->setText(OutputFileName);

    // Video part
    if (VideoStream) {
        InfoLabelB2->setText(ImageDef->Name);
        QString VideoBitRateStr=QString("%1").arg(VideoBitrate); if (VideoBitRateStr.endsWith("000")) VideoBitRateStr=VideoBitRateStr.left(VideoBitRateStr.length()-3)+"k";
        InfoLabelB3->setText(QString(VIDEOCODECDEF[VideoCodecSubId].LongName)+" - "+(VideoBitRateStr!="0"?VideoBitRateStr+"b/s":"lossless"));
    } else {
        InfoLabelA2->setVisible(false);
        InfoLabelB2->setVisible(false);
        InfoLabelA3->setVisible(false);
        InfoLabelB3->setVisible(false);
        SlideProgressBarLabel->setVisible(false);
        SlideNumberLabel->setVisible(false);
        SlideProgressBar->setVisible(false);
    }

    // Audio part
    if (AudioStream) {
        QString AudioBitRateStr=QString("%1").arg(AudioBitrate); if (AudioBitRateStr.endsWith("000")) AudioBitRateStr=AudioBitRateStr.left(AudioBitRateStr.length()-3)+"k";
        InfoLabelB4->setText(QString(AUDIOCODECDEF[AudioCodecSubId].LongName)+QString(" - %1 Hz - ").arg(AudioSampleRate)+(AudioBitRateStr!="0"?AudioBitRateStr+"b/s":"lossless"));
    } else {
        InfoLabelA4->setVisible(false);
        InfoLabelB4->setVisible(false);
    }
}

//*************************************************************************************************************************************************

void cEncodeVideo::DisplayProgress(qlonglong RenderedFrame,qlonglong Position,int Column,int ColumnStart) {
    int     DurationProcess =StartTime.msecsTo(QTime::currentTime());
    double  CalcFPS         =(double(RenderedFrame)/(double(DurationProcess)/1000));
    double  EstimDur        =double(NbrFrame-RenderedFrame)/CalcFPS;

    ElapsedTimeLabel->setText(QString("%1").arg((QTime(0,0,0,0).addMSecs(DurationProcess)).toString("hh:mm:ss"))+
                            QApplication::translate("DlgRenderVideo"," - Estimated time left : ")+
                            QString("%1").arg(QTime(0,0,0,0).addMSecs(EstimDur*1000).toString("hh:mm:ss")));
    FPSLabel->setText(QString("%1").arg(double(RenderedFrame)/(double(DurationProcess)/1000),0,'f',1));
    if (VideoStream) {
        SlideNumberLabel->setText(QString("%1/%2").arg(Column-FromSlide+1).arg(ToSlide-FromSlide+1));
        FrameNumberLabel->setText(QString("%1/%2").arg(RenderedFrame).arg(NbrFrame));
    }
    SlideProgressBar->setValue(Position!=-1?int(double(Position-ColumnStart)/(double(AV_TIME_BASE)/dFPS/double(1000))):SlideProgressBar->maximum());
    TotalProgressBar->setValue(RenderedFrame);

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
        #if defined(LIBAV_08)
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

    AudioFrameNbr       =0;
    VideoFrameNbr       =0;
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
        #if defined(LIBAV_08)
            if ((AudioStream->codec->sample_fmt!=RenderMusic.SampleFormat)||(AudioStream->codec->channels!=RenderMusic.Channels)||(AudioSampleRate!=RenderMusic.SamplingRate)) {
                if (!AudioResamplerBuffer) {
                    AudioResamplerBufferSize=AVCODEC_MAX_AUDIO_FRAME_SIZE*ToEncodeMusic.Channels;
                    AudioResamplerBuffer=(uint8_t *)av_malloc(AudioResamplerBufferSize);
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
        #elif defined(USELIBSWRESAMPLE)
            if ((AudioStream->codec->sample_fmt!=RenderMusic.SampleFormat)||(AudioStream->codec->channels!=RenderMusic.Channels)||(AudioSampleRate!=RenderMusic.SamplingRate)) {
/*                if (!AudioResamplerBuffer) {
//                    AudioResamplerBufferSize=AVCODEC_MAX_AUDIO_FRAME_SIZE*ToEncodeMusic.Channels;
//                    AudioResamplerBuffer=(uint8_t *)av_malloc(AudioResamplerBufferSize);
                    int out_linesize=0;
                    AudioResamplerBufferSize=av_samples_alloc(&AudioResamplerBuffer,&out_linesize,ToEncodeMusic.Channels,AVCODEC_MAX_AUDIO_FRAME_SIZE,ToEncodeMusic.SampleFormat,1);
                    if (AudioResamplerBufferSize<=0) {
                        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: AudioResamplerBuffer allocation failed"));
                        Continue=false;
                    }
                }*/
                if (!AudioResampler) {
                    AudioResampler=swr_alloc();
                    av_opt_set_int(AudioResampler,"in_channel_layout",     av_get_default_channel_layout(ToEncodeMusic.Channels),0);
                    av_opt_set_int(AudioResampler,"in_sample_rate",        ToEncodeMusic.SamplingRate,0);
                    av_opt_set_sample_fmt(AudioResampler,"in_sample_fmt",  ToEncodeMusic.SampleFormat,0);
                    av_opt_set_int(AudioResampler,"out_channel_layout",    AudioStream->codec->channel_layout,0);
                    av_opt_set_int(AudioResampler,"out_sample_rate",       AudioStream->codec->sample_rate,0);
                    av_opt_set_sample_fmt(AudioResampler,"out_sample_fmt", AudioStream->codec->sample_fmt,0);
                    if (swr_init(AudioResampler)<0) {
                        ToLog(LOGMSG_CRITICAL,QString("DoEncode: swr_init failed"));
                        Continue=false;
                    }
                }
            }
        #elif defined(USELIBAVRESAMPLE)
            if ((AudioStream->codec->sample_fmt!=ToEncodeMusic.SampleFormat)||(AudioStream->codec->channels!=ToEncodeMusic.Channels)||(AudioSampleRate!=ToEncodeMusic.SamplingRate)) {
                if (!AudioResamplerBuffer) {
                    int out_linesize=0;
                    AudioResamplerBufferSize=av_samples_alloc(&AudioResamplerBuffer,&out_linesize,ToEncodeMusic.Channels,AVCODEC_MAX_AUDIO_FRAME_SIZE,ToEncodeMusic.SampleFormat,1);
                    if (AudioResamplerBufferSize<=0) {
                        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: AudioResamplerBuffer allocation failed"));
                        Continue=false;
                    }
                }
                if (!AudioResampler) {
                    AudioResampler=avresample_alloc_context();
                    if (AudioResampler) {
                        av_opt_set_int(AudioResampler,"in_channel_layout", av_get_default_channel_layout(ToEncodeMusic.Channels),0);
                        av_opt_set_int(AudioResampler,"in_sample_rate",    ToEncodeMusic.SamplingRate,0);
                        av_opt_set_int(AudioResampler,"out_channel_layout",AudioStream->codec->channel_layout,0);
                        av_opt_set_int(AudioResampler,"out_sample_rate",   AudioSampleRate,0);
                        av_opt_set_int(AudioResampler,"in_sample_fmt",     ToEncodeMusic.SampleFormat,0);
                        av_opt_set_int(AudioResampler,"out_sample_fmt",    AudioStream->codec->sample_fmt,0);
                            if (avresample_open(AudioResampler)<0) {
                                ToLog(LOGMSG_CRITICAL,QString("DoEncode: avresample_open failed"));
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

    if (SlideProgressBar)   SlideProgressBar->setMaximum(ToSlide-FromSlide);
    if (TotalProgressBar)   TotalProgressBar->setMaximum(NbrFrame);

    //QFutureWatcher<void> ThreadEncodeVideo;
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

        // Ensure previous threaded encoding was ended before continuing
        //if (ThreadEncodeVideo.isRunning()) ThreadEncodeVideo.waitForFinished();

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
                #if defined(LIBAV_08)
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
            //ThreadEncodeVideo.setFuture(QtConcurrent::run(this,&cEncodeVideo::EncodeVideo,&ToEncodeMusic,&ImageList,&Continue));
            EncodeVideo(&ToEncodeMusic,&ImageList,Continue);
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

    //if (ThreadEncodeVideo.isRunning()) ThreadEncodeVideo.waitForFinished();

    // refresh display
    if (!StopProcessWanted) DisplayProgress(RenderedFrame,-1,Column,0);  // Set All to 100%

    // Cleaning
    if (PreviousFrame!=NULL) delete PreviousFrame;
    if (Frame!=NULL)         delete Frame;

    CloseEncoder();

    return Continue;
}

//*************************************************************************************************************************************************

void cEncodeVideo::EncodeMusic(cSoundBlockList *ToEncodeMusic,bool &Continue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::EncodeMusic");
    Mutex.lock();

    int     errcode;
    int64_t DestNbrSamples=ToEncodeMusic->SoundPacketSize/(ToEncodeMusic->Channels*av_get_bytes_per_sample(ToEncodeMusic->SampleFormat));
    int     DestSampleSize=AudioStream->codec->channels*av_get_bytes_per_sample(AudioStream->codec->sample_fmt);
    uint8_t *DestPacket   =NULL;
    int16_t *PacketSound  =NULL;
    int64_t DestPacketSize=DestNbrSamples*DestSampleSize;

    // Flush audio frame of ToEncodeMusic
    while ((Continue)&&(ToEncodeMusic->List.count()>0)&&(!StopProcessWanted)&&((!VideoStream)||(LastAudioPts<(LastVideoPts+IncreasingVideoPts)))) {
        PacketSound=ToEncodeMusic->DetachFirstPacket();
        if (PacketSound==NULL) {
            ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: PacketSound==NULL"));
            Continue=false;
        } else {
            #if defined(LIBAV_08)
                DestPacket=(uint8_t *)PacketSound;
            #elif defined(USELIBAVRESAMPLE)
                // LIBAV 9 => Convert sample format (is needed)
                if ((AudioResampler!=NULL)&&(AudioResamplerBuffer!=NULL)) {
                    DestPacket=AudioResamplerBuffer;
                    uint8_t *in_data[RESAMPLE_MAX_CHANNELS]={0},*out_data[RESAMPLE_MAX_CHANNELS]={0};
                    int     in_linesize=0,out_linesize=0;
                    int     out_samples=avresample_available(AudioResampler)+av_rescale_rnd(avresample_get_delay(AudioResampler)+DestNbrSamples,AudioStream->codec->sample_rate,ToEncodeMusic->SamplingRate,AV_ROUND_UP);
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
            #elif defined(USELIBSWRESAMPLE)
                if (AudioResampler!=NULL) {
                    int out_samples=av_rescale_rnd(swr_get_delay(AudioResampler,ToEncodeMusic->SamplingRate)+DestNbrSamples,AudioStream->codec->sample_rate,ToEncodeMusic->SamplingRate,AV_ROUND_UP);
                    av_samples_alloc(&AudioResamplerBuffer,NULL,AudioStream->codec->channels,out_samples,AudioStream->codec->sample_fmt,0);
                    if (!AudioResamplerBuffer) {
                        ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: AudioResamplerBuffer allocation failed"));
                        Continue=false;
                    }
                    uint8_t *in_data[RESAMPLE_MAX_CHANNELS]={0},*out_data[RESAMPLE_MAX_CHANNELS]={0};
                    int     in_linesize=0,out_linesize=0;
                    if (av_samples_fill_arrays(in_data,&in_linesize,(uint8_t *)PacketSound,ToEncodeMusic->Channels,DestNbrSamples,ToEncodeMusic->SampleFormat,0)<0) {
                        ToLog(LOGMSG_CRITICAL,QString("failed in_data fill arrays"));
                        Continue=false;
                    } else {
                        if (av_samples_fill_arrays(out_data,&out_linesize,AudioResamplerBuffer,AudioStream->codec->channels,out_samples,AudioStream->codec->sample_fmt,0)<0) {
                            ToLog(LOGMSG_CRITICAL,QString("failed out_data fill arrays"));
                            Continue=false;
                        } else {
                            DestPacketSize=swr_convert(AudioResampler,out_data,out_samples,(const uint8_t **)in_data,DestNbrSamples)*DestSampleSize;
                            if (DestPacketSize<=0) {
                                ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: swr_convert failed"));
                                Continue=false;
                            }
                            DestPacket=(uint8_t *)out_data[0];
                        }
                    }
                } else DestPacket=(uint8_t *)PacketSound;
            #else
                DestPacket=(uint8_t *)PacketSound;
            #endif

            if (Continue) {
                // Init AudioFrame
                avcodec_get_frame_defaults(AudioFrame);
                AudioFrame->nb_samples      =DestPacketSize/DestSampleSize;
                AudioFrame->pts             =AudioFrame->nb_samples*AudioFrameNbr;

                #ifdef LIBAV_09
                AudioFrame->format          =AudioStream->codec->sample_fmt;
                AudioFrame->channel_layout  =AudioStream->codec->channel_layout;
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
                        Continue=false;
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
                            Continue=false;
                        }
                    }
                    LastAudioPts+=IncreasingAudioPts;
                    AudioFrameNbr++;
                    //ToLog(LOGMSG_INFORMATION,QString("Audio:  Stream:%1 - Frame:%2 - PTS:%3").arg(AudioStream->index).arg(AudioFrameNbr).arg(LastAudioPts));
                } else {
                    char Buf[2048];
                    av_strerror(errcode,Buf,2048);
                    ToLog(LOGMSG_CRITICAL,QString("EncodeMusic: avcodec_fill_audio_frame() failed: ")+QString(Buf));
                    Continue=false;
                }
            }

            if ((AudioFrame->extended_data)&&(AudioFrame->extended_data!=AudioFrame->data)) {
                av_free(AudioFrame->extended_data);
                AudioFrame->extended_data=NULL;
            }
        }
        av_free(PacketSound);

        #if defined(USELIBSWRESAMPLE)
        av_free(AudioResamplerBuffer);
        AudioResamplerBuffer=NULL;
        #endif
    }
    avcodec_flush_buffers(AudioStream->codec);
    Mutex.unlock();
}

//*************************************************************************************************************************************************

void cEncodeVideo::EncodeVideo(cSoundBlockList *ToEncodeMusic,QList<QImage *>*ImageList,bool &Continue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cEncodeVideo::EncodeVideo");
    if ((AudioStream)&&(AudioFrame)) EncodeMusic(ToEncodeMusic,Continue);

    Mutex.lock();
    if ((VideoStream)&&(ImageList->count()>0)&&(VideoFrameConverter)&&(VideoFrame)) {
        QImage *Image=ImageList->takeFirst();
        avcodec_get_frame_defaults(VideoFrame);
        if (avpicture_fill(
            (AVPicture *)VideoFrame,            // Frame to prepare
            VideoFrameBuf,                      // Buffer which will contain the image data
            VideoStream->codec->pix_fmt,        // The format in which the picture data is stored
            VideoStream->codec->width,          // The width of the image in pixels
            VideoStream->codec->height          // The height of the image in pixels
        )!=VideoFrameBufSize) {
            ToLog(LOGMSG_CRITICAL,"EncodeVideo-EncodeVideo: avpicture_fill() failed for VideoFrameBuf");
            Mutex.unlock();
            return;
        }

        if ((VideoFrameNbr%VideoStream->codec->gop_size)==0) VideoFrame->pict_type=AV_PICTURE_TYPE_I;
        VideoFrame->pts=VideoFrameNbr;

        // Apply ExtendV
        if ((Continue)&&(!StopProcessWanted)&&(Image->height()!=VideoStream->codec->height)) {
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
        if ((Continue)&&(!StopProcessWanted)) {
            #ifdef USELIBSWRESAMPLE
            uint8_t *data={(uint8_t *)Image->bits()};
            #else
            uint8_t *data=(uint8_t *)Image->bits();
            #endif
            int LineSize=Image->bytesPerLine();
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
                Continue=false;
            }
        }

        if (Image) delete Image;

        if ((Continue)&&(!StopProcessWanted)) {

            AVPacket pkt;
            av_init_packet(&pkt);
            pkt.size=0;
            pkt.data=NULL;

            #ifdef LIBAV_08
            int out_size=avcodec_encode_video(VideoStream->codec,VideoEncodeBuffer,VideoEncodeBufferSize,VideoFrame);
            if (out_size<0) {
                ToLog(LOGMSG_CRITICAL,QString("EncodeVideo: avcodec_encode_video failed"));
                Continue=false;
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
                Continue=false;
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
                    Continue=false;
                }
            }

            LastVideoPts+=IncreasingVideoPts;
            VideoFrameNbr++;
            //ToLog(LOGMSG_INFORMATION,QString("Video:  Stream:%1 - Frame:%2 - PTS:%3").arg(VideoStream->index).arg(VideoFrameNbr).arg(LastVideoPts));

            if ((VideoFrame->extended_data)&&(VideoFrame->extended_data!=VideoFrame->data)) {
                av_free(VideoFrame->extended_data);
                VideoFrame->extended_data=NULL;
            }
        }
        avcodec_flush_buffers(VideoStream->codec);
    }
    Mutex.unlock();
}
