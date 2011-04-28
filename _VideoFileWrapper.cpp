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

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_ApplicationDefinitions.h"
#include "_SoundDefinitions.h"
#include "_VideoFileWrapper.h"

//*************************************************************************************************************************************

DecodeVideoObject::DecodeVideoObject(AVPacket *aStreamPacket,double aFramePosition,AVStream *aVideoStream,double adPosition,bool aIsKeyFrame) {
    StreamPacket    =aStreamPacket;
    FramePosition   =aFramePosition;
    VideoStream     =aVideoStream;
    dPosition       =adPosition;
    IsKeyFrame      =aIsKeyFrame;
}

DecodeVideoObject::~DecodeVideoObject() {
    // Free packet
    av_free_packet(StreamPacket);          // Free the StreamPacket that was allocated by previous call to av_read_frame
    delete StreamPacket;
}

//*************************************************************************************************************************************

DecodeVideoObjectList::DecodeVideoObjectList() {
}

DecodeVideoObjectList::~DecodeVideoObjectList() {
    List.clear();
}

DecodeVideoObject *DecodeVideoObjectList::DetachFirstPacket() {
    if (List.count()>0) return (DecodeVideoObject *)List.takeFirst(); else return NULL;
}

/*************************************************************************************************************************************
    CLASS cvideofilewrapper
*************************************************************************************************************************************/

cvideofilewrapper::cvideofilewrapper() {
    CacheFirstImage     = NULL;             // Cache image of first image of the video

    // LibAVFormat/Codec/SWScale part
    ffmpegVideoFile     = NULL;
    VideoDecoderCodec   = NULL;
    AudioDecoderCodec   = NULL;
    VideoStreamNumber   = 0;
    NextPacketPosition  = -1;
    AdjustTimeStamp     = 0;
}

//====================================================================================================================

cvideofilewrapper::~cvideofilewrapper() {
    if (CacheFirstImage!=NULL) {
        delete CacheFirstImage;
        CacheFirstImage=NULL;
    }

    // Close LibAVFormat and LibAVCodec contexte for the file
    CloseVideoFileReader();
}

//====================================================================================================================
// Close LibAVFormat and LibAVCodec contexte for the file
//====================================================================================================================

void cvideofilewrapper::CloseVideoFileReader() {
    // Close the codecs
    if (VideoDecoderCodec!=NULL) {
        avcodec_close(ffmpegVideoFile->streams[VideoStreamNumber]->codec);
        VideoDecoderCodec=NULL;
    }
    if (AudioDecoderCodec!=NULL) {
        avcodec_close(ffmpegVideoFile->streams[AudioStreamNumber]->codec);
        AudioDecoderCodec=NULL;
    }

    // Close the video file
    if (ffmpegVideoFile!=NULL) {
        av_close_input_file(ffmpegVideoFile);
        ffmpegVideoFile=NULL;
    }
}

//====================================================================================================================
// Read a video frame from current stream
//====================================================================================================================

QImage *cvideofilewrapper::ReadVideoFrame(int Position,cSoundBlockList *SoundTrackBloc,double Volume,bool ForceSoundOnly) {
    // Ensure file was previously open
    if ((ffmpegVideoFile==NULL)||((MusicOnly==false)&&(ForceSoundOnly==false)&&(VideoDecoderCodec==NULL))||(((MusicOnly==true)||(ForceSoundOnly==true))&&(AudioDecoderCodec==NULL))) return NULL;

    int64_t         AVNOPTSVALUE        =INT64_C(0x8000000000000000); // to solve type error with Qt
    QImage          *RetImage           =NULL;
    AVStream        *AudioStream        =ffmpegVideoFile->streams[AudioStreamNumber];
    AVStream        *VideoStream        =ffmpegVideoFile->streams[VideoStreamNumber];

    #if FF_API_OLD_SAMPLE_FMT
    int64_t         SrcSampleSize       =(av_get_bits_per_sample_fmt(AudioStream->codec->sample_fmt)>>3)*int64_t(AudioStream->codec->channels);
    #else
    int64_t         SrcSampleSize       =(av_get_bits_per_sample_format(AudioStream->codec->sample_fmt)>>3)*int64_t(AudioStream->codec->channels);
    #endif

    int64_t         DstSampleSize       =((SoundTrackBloc!=NULL)?(SoundTrackBloc->SampleBytes*SoundTrackBloc->Channels):0);
    AVPacket        *StreamPacket       =NULL;
    uint8_t         *BufferToDecode     =NULL;
    uint8_t         *BufferForDecoded   =NULL;
    int64_t         MaxAudioLenDecoded  =AVCODEC_MAX_AUDIO_FRAME_SIZE*10;
    int64_t         AudioLenDecoded     =0;
    double          dPosition           =double(Position)/1000;     // Position in double format
    double          EndPosition         =dPosition+(SoundTrackBloc==NULL?0:SoundTrackBloc->WantedDuration);
    double          AudioDataWanted     =((AudioStream)&&(SoundTrackBloc))?SoundTrackBloc->WantedDuration*double(AudioStream->codec->sample_rate)*SrcSampleSize:0;

    // Cac difftime between asked position and previous end decoded position
    int DiffTimePosition=NextPacketPosition-Position;
    if (DiffTimePosition<0) DiffTimePosition=-DiffTimePosition;

    // Adjust position if input file have a start_time value
    if (ffmpegVideoFile->start_time!=AVNOPTSVALUE)  dPosition+=double(ffmpegVideoFile->start_time)/double(AV_TIME_BASE);

    // Prepare a buffer for sound decoding
    if (SoundTrackBloc!=NULL) {
        BufferToDecode      =(uint8_t *)av_malloc((AVCODEC_MAX_AUDIO_FRAME_SIZE*3)/2);
        BufferForDecoded    =(uint8_t *)av_malloc(MaxAudioLenDecoded);
    }
    // Calc if we need to seek to a position
    if ((Position==0)
//        ||(SoundTrackBloc==NULL)                    // If no SoundTrackBloc then it's a dialog box or seek preview without playing
        ||(DiffTimePosition>2)                      // Allow 2 msec diff (rounded double !)
        ) {
         //qDebug()<<"SEEK Ancien="<<NextPacketPosition<<"Nouveau="<<Position<<"Diff="<<DiffTimePosition<<"AdjustTimeStamp="<<AdjustTimeStamp;
        // Flush all buffers
        for (unsigned int i=0;i<ffmpegVideoFile->nb_streams;i++)  {
            AVCodecContext *codec_context = ffmpegVideoFile->streams[i]->codec;
            if (codec_context && codec_context->codec) avcodec_flush_buffers(codec_context);
        }
        VideoObjectList.List.clear();                               // Clear spool buffer
        if (SoundTrackBloc!=NULL) SoundTrackBloc->ClearList();      // Clear soundtrack list

        // Seek to nearest previous key frame
        int64_t seek_target=av_rescale_q(int64_t((dPosition-double(AdjustTimeStamp)/1000)*AV_TIME_BASE),AV_TIME_BASE_Q,ffmpegVideoFile->streams[AudioStream!=NULL?AudioStreamNumber:VideoStreamNumber]->time_base);
        av_seek_frame(ffmpegVideoFile,AudioStream!=NULL?AudioStreamNumber:VideoStreamNumber,seek_target,AVSEEK_FLAG_BACKWARD);      // If possible prefere audio stream
    }

    //*************************************************************************************************************************************
    // Decoding process : Get StreamPacket until endposition is reach (if sound is wanted) or until image is ok (if image only is wanted)
    //*************************************************************************************************************************************
    bool    Continue        =true;
    bool    IsVideoFind     =false;
    double  FrameTimeBase   =0;
    double  FramePosition   =0;
    double  FrameDuration   =0;
    double  FrameEndPosition=0;

    if (MusicOnly || ForceSoundOnly) IsVideoFind=true;

    // Parse VideoObjectList to find if we already have the packet for image
    for (int j=0;j<VideoObjectList.List.count();j++) {
        if ((!IsVideoFind)&&(VideoObjectList.List[j]->FramePosition>=dPosition)) {
            // Now construct RetImage
            RetImage=YUVStreamToQImage(dPosition,false);
            IsVideoFind=(RetImage!=NULL);
        }
    }

//****************************************
// Ajouter un skip de frame inutile !
//  => Ref-Frame<Position dans la liste !
//****************************************

    while (Continue) {
        StreamPacket=new AVPacket();
        av_init_packet(StreamPacket);
        StreamPacket->flags|=AV_PKT_FLAG_KEY;  // HACK for CorePNG to decode as normal PNG by default

        if (av_read_frame(ffmpegVideoFile,StreamPacket)==0) {

            //qDebug()<<"=>"<<((StreamPacket->stream_index==VideoStreamNumber)?"Video":(StreamPacket->stream_index==AudioStreamNumber)?"Audio":"Unknown")
            //        <<"packet at"<<FramePosition<<"Is Keyframe"<<(((StreamPacket->flags & PKT_FLAG_KEY)>0)?"Yes":"No")<<"Flag"<<StreamPacket->flags
            //        <<"[PTS="<<StreamPacket->dts<<"-DTS="<<StreamPacket->pts<<"]";

            if ((StreamPacket->stream_index==VideoStreamNumber)||(StreamPacket->stream_index==AudioStreamNumber)) {
                FrameTimeBase   =av_q2d(StreamPacket->stream_index==VideoStreamNumber?VideoStream->time_base:AudioStream->time_base);
                if (CodecUsePTS) FramePosition=double((StreamPacket->pts!=AVNOPTSVALUE)?StreamPacket->pts:0)*FrameTimeBase;   // pts instead of dts
                    else         FramePosition=double((StreamPacket->dts!=AVNOPTSVALUE)?StreamPacket->dts:0)*FrameTimeBase;   // dts instead of pts
                FrameDuration   =double(StreamPacket->duration)*FrameTimeBase;;
                FrameEndPosition=FramePosition+FrameDuration;

                //===================================================================================
                // Decode video
                //===================================================================================
                if ((!MusicOnly)&&(!ForceSoundOnly)&&(StreamPacket->stream_index==VideoStreamNumber)) {
                    VideoObjectList.List.append(new DecodeVideoObject(StreamPacket,FramePosition,VideoStream,dPosition,(StreamPacket->flags & PKT_FLAG_KEY)>0));

                    if ((!IsVideoFind)&&(FramePosition>=dPosition)) {
                        // Now construct RetImage
                        RetImage=YUVStreamToQImage(dPosition,(SoundTrackBloc==NULL));
                        IsVideoFind=(RetImage!=NULL);
                    }
                    StreamPacket=NULL;                 // StreamPacket is deleted by the VideoObjectList

                //===================================================================================
                // Decode audio
                //===================================================================================
                } else if ((StreamPacket->stream_index==AudioStreamNumber)&&(SoundTrackBloc!=NULL)&&(StreamPacket->size>0)) {
                    AVPacket PacketTemp;
                    av_init_packet(&PacketTemp);
                    PacketTemp.data=StreamPacket->data;
                    PacketTemp.size=StreamPacket->size;

                    // NOTE: the audio packet can contain several frames
                    while (PacketTemp.size>0) {

                        // Decode audio data
                        int SizeDecoded=(AVCODEC_MAX_AUDIO_FRAME_SIZE*3)/2;
                        int Len=avcodec_decode_audio3(AudioStream->codec,(int16_t *)BufferToDecode,&SizeDecoded,&PacketTemp);
                        if (Len<0) {
                            // if decode error then data are not good : replace them with null sound
                            //SizeDecoded=int64_t(LastAudioFrameDuration*double(SoundTrackBloc->SamplingRate))*DstSampleSize;
                            //memset(BufferForDecoded+AudioLenDecoded,0,SizeDecoded);
                            //AudioLenDecoded+=SizeDecoded;
                            //qDebug()<<"    =>Make NULL Audio frame"<<SizeDecoded<<"bytes added - Buffer:"<<AudioLenDecoded<<"/"<<MaxAudioLenDecoded;
                            // if error, we skip the frame and exit the while loop
                            PacketTemp.size=0;
                        } else {
                            // If wanted position <= CurrentPosition+Packet duration then add this packet to the queue
                            if ((FrameEndPosition>=dPosition)&&(SizeDecoded>0)) {

                                int64_t Delta=0;
                                // if dPosition start in the midle of the pack, then calculate delta
                                if (dPosition>FramePosition) {
                                    Delta=int64_t(double(dPosition-FramePosition)*double(AudioStream->codec->sample_rate));
                                    Delta*=SrcSampleSize;
                                }
                                // Append decoded data to BufferForDecoded buffer
                                memcpy(BufferForDecoded+AudioLenDecoded,BufferToDecode+Delta,SizeDecoded-Delta);
                                AudioLenDecoded+=(SizeDecoded-Delta);

                                //double RealDuration=(double(SizeDecoded-Delta)/double(SrcSampleSize))/double(AudioStream->codec->sample_rate);
                            } else {
                                qDebug()<<"Skip packet :"<<FramePosition<<"-"<<FrameEndPosition<<"["<<dPosition<<"-"<<EndPosition<<"]";
                            }
                            PacketTemp.data+=Len;
                            PacketTemp.size-=Len;
                        }
                    }
                }
            }

            // Continue with a new one
            if (StreamPacket!=NULL) {
                av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
                delete StreamPacket;
                StreamPacket=NULL;
            }

            // Check if we need to continue loop
            Continue=(((AudioLenDecoded<AudioDataWanted)||(IsVideoFind==false))&&(VideoObjectList.List.count()<50));
            //qDebug()<<"??? AudioLenDecoded"<<AudioLenDecoded<<"/AudioDataWanted"<<AudioDataWanted<<"IsVideoFind"<<(IsVideoFind?"Yes":"No");
        } else {
            // if error in av_read_frame(...) then may be we have reach the end of file !
            Continue=false;
            // if no image then use the spool to construct one
            if (RetImage==NULL) RetImage=YUVStreamToQImage(dPosition,true);
        }
    }
    // Keep NextPacketPosition for determine next time if we need to seek
    NextPacketPosition=int(EndPosition*1000);

    //**********************************************************************
    // Transfert data from BufferForDecoded to Buffer using audio_resample
    //**********************************************************************
    if ((SoundTrackBloc!=NULL)&&(AudioLenDecoded>0)) {
        // Create a context for resampling audio data
        ReSampleContext *RSC=NULL;  // Context for resampling audio data

        if ((SoundTrackBloc->Channels!=AudioStream->codec->channels)||(SoundTrackBloc->SamplingRate!=AudioStream->codec->sample_rate)) RSC=av_audio_resample_init(
                SoundTrackBloc->Channels,AudioStream->codec->channels,          // output_channels, input_channels
                SoundTrackBloc->SamplingRate,AudioStream->codec->sample_rate,   // output_rate, input_rate
                SAMPLE_FMT_S16,AudioStream->codec->sample_fmt,                  // sample_fmt_out, sample_fmt_in
                16,                                                             // filter_length
                10,                                                             // log2_phase_count
                0,                                                              // linear
                0.8);                                                           // cutoff

        if (RSC!=NULL) {
            short int*BufSampled=(short int*)av_malloc((AVCODEC_MAX_AUDIO_FRAME_SIZE*3)/2);
            // Resample sound to wanted freq. and channels
            try {
                int64_t SizeSampled=audio_resample(RSC,BufSampled,(short int*)BufferForDecoded,AudioLenDecoded/SrcSampleSize)*DstSampleSize;
                // Adjust volume
                if (Volume!=1) {
                    int16_t *Buf1=(int16_t*)BufSampled;
                    int32_t mix;
                    for (int j=0;j<SizeSampled/(SoundTrackBloc->SampleBytes*SoundTrackBloc->Channels);j++) {
                        // Left channel : Adjust if necessary (16 bits)
                        mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
                        // Right channel : Adjust if necessary (16 bits)
                        mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
                    }
                }
                // Append data to SoundTrackBloc
                SoundTrackBloc->AppendData((int16_t*)BufSampled,SizeSampled);
            } catch (...) {
                qDebug()<<"Plantage audio_resample ????";
            }

            // Close the resampling audio context
            audio_resample_close(RSC);
            // Free allocated buffers
            av_free(BufSampled);
        } else {
            // Adjust volume
            if (Volume!=1) {
                int16_t *Buf1=(int16_t*)BufferForDecoded;
                int32_t mix;
                for (int j=0;j<AudioLenDecoded/(SoundTrackBloc->SampleBytes*SoundTrackBloc->Channels);j++) {
                    // Left channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
                    // Right channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
                }
            }
            // Append data to SoundTrackBloc
            SoundTrackBloc->AppendData((int16_t*)BufferForDecoded,AudioLenDecoded);
        }
    }

    if (SoundTrackBloc!=NULL) {
        // Now ensure SoundTrackBloc have correct wanted packet (if no then add nullsound)
        while (SoundTrackBloc->List.count()<SoundTrackBloc->NbrPacketForFPS) {
            SoundTrackBloc->AppendNullSoundPacket();
            qDebug()<<"Allongement SoundTrackBloc :"
                    <<SoundTrackBloc->List.count()<<"/"<<SoundTrackBloc->NbrPacketForFPS
                    <<"TempSize:"<<SoundTrackBloc->CurrentTempSize;
        }
    }

    if (BufferToDecode)   av_free(BufferToDecode);
    if (BufferForDecoded) av_free(BufferForDecoded);

    return RetImage;
}

//====================================================================================================================
// Convert actual data in a QImage
//====================================================================================================================

QImage *cvideofilewrapper::YUVStreamToQImage(double dPosition,bool GetFirstValide) {
    // Allocate structure for YUV image
    AVFrame *FrameBufferYUV=avcodec_alloc_frame();
    if (FrameBufferYUV==NULL) return NULL;

    // Allocate structure for RGB image
    AVFrame *FrameBufferRGB=avcodec_alloc_frame();
    if (FrameBufferRGB==NULL) {
        av_free(FrameBufferYUV);
        return NULL;
    }

    // Decode each frame in the spool until we reach dPosition
    QImage  *RetImage    =NULL;
    bool    IsFrameValide=false;
    if (GetFirstValide) {
        while ((VideoObjectList.List.count()>0)&&(!IsFrameValide)) {
            DecodeVideoObject *Packet=VideoObjectList.DetachFirstPacket();
            int FrameDecoded=-1;
            avcodec_decode_video2(Packet->VideoStream->codec,FrameBufferYUV,&FrameDecoded,Packet->StreamPacket);
            IsFrameValide=(FrameDecoded>0);
            delete Packet;
        }
    } else {
        while ((VideoObjectList.List.count()>0)&&(VideoObjectList.List[0]->FramePosition<=dPosition)) {
            DecodeVideoObject *Packet=VideoObjectList.DetachFirstPacket();
            int FrameDecoded=-1;
            avcodec_decode_video2(Packet->VideoStream->codec,FrameBufferYUV,&FrameDecoded,Packet->StreamPacket);
            if (!IsFrameValide) IsFrameValide=(FrameDecoded>0);
            delete Packet;
        }
    }

    if (IsFrameValide) {
        // Calc destination size
        int W=ffmpegVideoFile->streams[VideoStreamNumber]->codec->width;
        int H=ffmpegVideoFile->streams[VideoStreamNumber]->codec->height;

        // Create QImage
        RetImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);

        // Assign appropriate parts of ImageBuffer to image planes in FrameBufferRGB
        avpicture_fill(
                (AVPicture *)FrameBufferRGB,        // Buffer to prepare
                RetImage->bits(),                   // Buffer which will contain the image data
                PIX_FMT_BGRA,                       // The format in which the picture data is stored (see http://wiki.aasimon.org/doku.php?id=ffmpeg:pixelformat)
                W,                                  // The width of the image in pixels
                H                                   // The height of the image in pixels
        );

        // Get a converter from libswscale
        struct SwsContext *img_convert_ctx=sws_getContext(
            W,H,ffmpegVideoFile->streams[VideoStreamNumber]->codec->pix_fmt,        // Src Widht,Height,Format
            W,H,PIX_FMT_BGRA,                                                       // Destination Width,Height,Format
            SWS_FAST_BILINEAR/*SWS_BICUBIC*/,                                       // flags
            NULL,NULL,NULL);                                                        // src Filter,dst Filter,param

        if (img_convert_ctx!=NULL) {
            int ret = sws_scale(
                img_convert_ctx,                                                    // libswscale converter
                FrameBufferYUV->data,                                               // Source buffer
                FrameBufferYUV->linesize,                                           // Source Stride ?
                0,                                                                  // Source SliceY:the position in the source image of the slice to process, that is the number (counted starting from zero) in the image of the first row of the slice
                H,                                                                  // Source SliceH:the height of the source slice, that is the number of rows in the slice
                FrameBufferRGB->data,                                               // Destination buffer
                FrameBufferRGB->linesize                                            // Destination Stride
            );
            if (ret<=0) {
                delete RetImage;
                RetImage=NULL;
            }
            sws_freeContext(img_convert_ctx);
        }
    }

    // Free the 2 AVFrame buffers
    av_free(FrameBufferYUV);
    av_free(FrameBufferRGB);

    return RetImage;
}

//====================================================================================================================

bool cvideofilewrapper::GetInformationFromFile(QString GivenFileName,bool aMusicOnly) {
    // Clean memory if a previous file was loaded
    CloseVideoFileReader();

    //Reset SourceFile values
    MusicOnly    =aMusicOnly;
    FileName     =GivenFileName;

    CreatDateTime=QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime=QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    Duration     =QTime(0,0,0,0);

    //====================================================================================================================
    // Open video file and get a LibAVFormat context and an associated LibAVCodec decoder
    //====================================================================================================================

    // Open video file and retrieve stream information
    if (av_open_input_file(&ffmpegVideoFile,FileName.toLocal8Bit(),NULL,0,NULL)!=0) return false;

    // Setup AVFormatContext options
    ffmpegVideoFile->flags|=AVFMT_FLAG_GENPTS;      // Generate missing pts even if it requires parsing future frames.

    if (av_find_stream_info(ffmpegVideoFile)<0) return false;

    // Get informations about duration
    int hh,mm,ss,ms;
    ms=ffmpegVideoFile->duration/1000;
    ss=ms/1000;
    mm=ss/60;
    hh=mm/60;

    mm=mm-(hh*60);
    ss=ss-(ss/60)*60;
    ms=ms-(ms/1000)*1000;
    Duration=QTime(hh,mm,ss,ms);

    // Find the first audio stream
    AudioStreamNumber=0;
    while ((AudioStreamNumber<(int)ffmpegVideoFile->nb_streams)&&(ffmpegVideoFile->streams[AudioStreamNumber]->codec->codec_type!=CODEC_TYPE_AUDIO)) AudioStreamNumber++;
    if (AudioStreamNumber>=(int)ffmpegVideoFile->nb_streams) return false;

    // Setup STREAM options
    ffmpegVideoFile->streams[AudioStreamNumber]->discard=AVDISCARD_DEFAULT;

    // Find the decoder for the audio stream and open it
    AudioDecoderCodec=avcodec_find_decoder(ffmpegVideoFile->streams[AudioStreamNumber]->codec->codec_id);

    // Setup decoder options
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->debug_mv         =0;                    // Debug level (0=nothing)
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->debug            =0;                    // Debug level (0=nothing)
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->workaround_bugs  =1;                    // Work around bugs in encoders which sometimes cannot be detected automatically : 1=autodetection
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->lowres           =0;                    // low resolution decoding, 1-> 1/2 size, 2->1/4 size
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->idct_algo        =FF_IDCT_AUTO;         // IDCT algorithm, 0=auto
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->skip_frame       =AVDISCARD_DEFAULT;    // ???????
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->skip_idct        =AVDISCARD_DEFAULT;    // ???????
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->skip_loop_filter =AVDISCARD_DEFAULT;    // ???????
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
    ffmpegVideoFile->streams[AudioStreamNumber]->codec->error_concealment=3;

    if ((AudioDecoderCodec==NULL)||(avcodec_open(ffmpegVideoFile->streams[AudioStreamNumber]->codec,AudioDecoderCodec)<0)) return false;

    // Find the first video stream
    VideoStreamNumber=0;
    VideoDecoderCodec=NULL;
    if (!MusicOnly) {
        while ((VideoStreamNumber<(int)ffmpegVideoFile->nb_streams)&&(ffmpegVideoFile->streams[VideoStreamNumber]->codec->codec_type!=CODEC_TYPE_VIDEO)) VideoStreamNumber++;
        if (VideoStreamNumber>=(int)ffmpegVideoFile->nb_streams) return false;

        // Setup STREAM options
        ffmpegVideoFile->streams[VideoStreamNumber]->discard=AVDISCARD_DEFAULT;

        // Find the decoder for the video stream and open it
        VideoDecoderCodec=avcodec_find_decoder(ffmpegVideoFile->streams[VideoStreamNumber]->codec->codec_id);

        // Setup decoder options
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->debug_mv         =0;                    // Debug level (0=nothing)
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->debug            =0;                    // Debug level (0=nothing)
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->workaround_bugs  =1;                    // Work around bugs in encoders which sometimes cannot be detected automatically : 1=autodetection
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->lowres           =0;                    // low resolution decoding, 1-> 1/2 size, 2->1/4 size
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->idct_algo        =FF_IDCT_AUTO;         // IDCT algorithm, 0=auto
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_frame       =AVDISCARD_DEFAULT;    // ???????
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_idct        =AVDISCARD_DEFAULT;    // ???????
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_loop_filter =AVDISCARD_DEFAULT;    // ???????
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->error_concealment=3;

        // h264 specific
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->thread_count     =getCpuCount();        // Not sure it work with old ffmpeg version !!!!!
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_loop_filter =AVDISCARD_ALL;        // Reduce quality but speed reading !

        if ((VideoDecoderCodec==NULL)||(avcodec_open(ffmpegVideoFile->streams[VideoStreamNumber]->codec,VideoDecoderCodec)<0)) return false;

        // Hack to correct wrong frame rates that seem to be generated by some codecs
        if (ffmpegVideoFile->streams[VideoStreamNumber]->codec->time_base.num>1000 && ffmpegVideoFile->streams[VideoStreamNumber]->codec->time_base.den==1) ffmpegVideoFile->streams[VideoStreamNumber]->codec->time_base.den=1000;

        CodecUsePTS=ffmpegVideoFile->streams[VideoStreamNumber]->codec->codec_id==CODEC_ID_H264;

        // Get informations about size image
        ImageWidth =ffmpegVideoFile->streams[VideoStreamNumber]->codec->width;
        ImageHeight=ffmpegVideoFile->streams[VideoStreamNumber]->codec->height;
        // Get Aspect Ratio
        AspectRatio=double(ffmpegVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.den);
        if (ffmpegVideoFile->streams[VideoStreamNumber]->sample_aspect_ratio.num!=0)
            AspectRatio=double(ffmpegVideoFile->streams[VideoStreamNumber]->sample_aspect_ratio.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->sample_aspect_ratio.den);
        if (AspectRatio==0) AspectRatio=1;

        // Try to load one image to be sure we can make something with this file
        IsValide    =true; // Disable IsValide test for ImageAt
        QImage *Img =ImageAt(true,720,0,false,true,NULL,1,false);
        if ((Img==NULL)&&(VideoObjectList.List.count()>0)) {

            // Allocate structure for YUV image
            AVFrame *FrameBufferYUV=avcodec_alloc_frame();
            if (FrameBufferYUV==NULL) return false;

            int FrameDecoded=-1;
            while ((VideoObjectList.List.count()>0)&&(FrameDecoded<=0)) {
                DecodeVideoObject *Packet=VideoObjectList.DetachFirstPacket();
                avcodec_decode_video2(Packet->VideoStream->codec,FrameBufferYUV,&FrameDecoded,Packet->StreamPacket);
                if (FrameDecoded>0) AdjustTimeStamp=int(Packet->FramePosition*1000)+1;
                delete Packet;
            }
            if (FrameDecoded>0) Img =ImageAt(true,720,0,false,true,NULL,1,false);
            av_free(FrameBufferYUV);
        }
        IsValide    =(Img!=NULL);
        delete Img;
    } else IsValide=true;

    return IsValide;
}

//====================================================================================================================

QImage *cvideofilewrapper::ImageAt(bool PreviewMode,int PreviewMaxHeight,int Position,bool CachedMode,bool ForceLoadDisk,cSoundBlockList *SoundTrackBloc,double Volume,bool ForceSoundOnly) {
    if (!IsValide) return NULL;

    // If ForceLoadDisk then ensure CacheImage is null
    if ((ForceLoadDisk)&&(CacheFirstImage!=NULL)) {
        delete CacheFirstImage;
        CacheFirstImage=NULL;
    }

    if (PreviewMode && CacheFirstImage && CachedMode) return new QImage(CacheFirstImage->copy());

    // Load a video frame
    QImage *LoadedImage=ReadVideoFrame(Position+AdjustTimeStamp,SoundTrackBloc,Volume,ForceSoundOnly);

    if ((!MusicOnly)&&(!ForceSoundOnly)&&(LoadedImage)) {
        // Scale image if anamorphous codec
        if (AspectRatio!=1) {
            QImage *NewLoadedImage=new QImage(LoadedImage->scaled(int(double(ImageWidth)*AspectRatio),ImageHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            delete LoadedImage;
            LoadedImage=NewLoadedImage;
        }
        // If preview mode and image size > PreviewMaxHeight, reduce Cache Image
        if ((PreviewMode)&&(ImageHeight>PreviewMaxHeight)) {
            QImage *NewImage=new QImage(LoadedImage->scaledToHeight(PreviewMaxHeight));
            delete LoadedImage;
            LoadedImage =NewImage;
        }
        if ((PreviewMode)&&(CachedMode==true)) {
            if (CacheFirstImage!=NULL) delete CacheFirstImage;
            CacheFirstImage=LoadedImage;
            LoadedImage=new QImage(CacheFirstImage->copy());
        }

    } else {
        // This case append when sound file is a video file : we don't want image !
        if (LoadedImage) {
            delete LoadedImage;
            LoadedImage=NULL;
        }
    }

    return LoadedImage;
}
