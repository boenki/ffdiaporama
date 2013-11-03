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

#include "cSoundBlockList.h"

//*********************************************************************************************************************************************
// Base object for sound manipulation
//*********************************************************************************************************************************************

cSoundBlockList::cSoundBlockList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::cSoundBlockList");

    CurrentTempSize     =0;                                                                 // Amount of data in the TempData buffer
    CurrentPosition     =-1;
    SoundPacketSize     =MAXSOUNDPACKETSIZE;                                                // Size of a packet (depending on FPS)
    TempData            =(u_int8_t *)av_malloc(SoundPacketSize+8);                           // Buffer for stocking temporary data (when decoding data are less than a packet)
    Channels            =2;                                                                 // Number of channels
    SamplingRate        =48000;                                                             // Sampling rate (frequency)
    SampleBytes         =2;                                                                 // 16 bits : Size of a sample
    dDuration           =double(SoundPacketSize)/double(Channels*SampleBytes*SamplingRate); // Duration of a packet
    NbrPacketForFPS     =1;                                                                 // Number of packet for FPS
    Adjusted            =false;
}

//====================================================================================================================

cSoundBlockList::~cSoundBlockList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::~cSoundBlockList");

    ClearList();
    if (TempData) {
        av_free(TempData);
        TempData=NULL;
    }
}

//====================================================================================================================
// Prepare and calculate values for a frame rate
//====================================================================================================================
void cSoundBlockList::SetFPS(double TheWantedDuration,int TheChannels,int64_t TheSamplingRate,enum AVSampleFormat TheSampleFormat) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::SetFPS");

    // Adjust value for 6 FPS (because rounded value make seek of audio files)
    if ((TheWantedDuration>166)&&(TheWantedDuration<167)) TheWantedDuration=166;

    ClearList();
    SamplingRate   =TheSamplingRate;
    SampleFormat   =TheSampleFormat;
    Channels       =TheChannels;
    WantedDuration =TheWantedDuration;
    NbrPacketForFPS=1;
    dDuration      =double(WantedDuration)/1000;
    SoundPacketSize=int(dDuration*double(SamplingRate))*SampleBytes*Channels;

    while (SoundPacketSize>MAXSOUNDPACKETSIZE) {
        SoundPacketSize =SoundPacketSize/2;
        dDuration       =dDuration/2;
        NbrPacketForFPS*=2;
    }
    if (TempData) {
        av_free(TempData);
        TempData=NULL;
    }
    TempData=(u_int8_t *)av_malloc(SoundPacketSize+8);
}

//====================================================================================================================
// Prepare and calculate values for a frame size
//====================================================================================================================
void cSoundBlockList::SetFrameSize(int FrameSize,int TheChannels,int64_t TheSamplingRate,enum AVSampleFormat TheSampleFormat) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::SetFrameSize");

    ClearList();
    SamplingRate   =TheSamplingRate;
    SampleFormat   =TheSampleFormat;
    SoundPacketSize=FrameSize;
    Channels       =TheChannels;
    NbrPacketForFPS=1;
    dDuration      =double(SoundPacketSize)/(double(SamplingRate)*double(SampleBytes)*double(Channels));
    WantedDuration =dDuration*1000;
    if (TempData) {
        av_free(TempData);
        TempData=NULL;
    }
    TempData=(u_int8_t *)av_malloc(SoundPacketSize+8);
}

//====================================================================================================================
// Clear the list (make av_free of each packet)
//====================================================================================================================
void cSoundBlockList::ClearList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::ClearList");
    while (List.count()>0) {
        int16_t *Packet=DetachFirstPacket();
        if (Packet) av_free(Packet);
    }
    CurrentTempSize=0;
    CurrentPosition=-1;
}

//====================================================================================================================
// Detach the first packet of the list (do not make av_free)
//====================================================================================================================
int16_t *cSoundBlockList::DetachFirstPacket() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::DetachFirstPacket");
    Mutex.lock();
    int16_t *Ret=NULL;
    if (List.count()>0) {
        Ret=(int16_t *)List.takeFirst();
        CurrentPosition+=(double(SoundPacketSize)/(SampleBytes*Channels*SamplingRate))*AV_TIME_BASE;
    }
    Mutex.unlock();
    return Ret;
}

//====================================================================================================================
// Append a packet to the end of the list
//====================================================================================================================
void cSoundBlockList::AppendPacket(int64_t Position,int16_t *PacketToAdd) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::AppendPacket");
    Mutex.lock();
    if (CurrentPosition==-1)
        CurrentPosition=Position;
    List.append(PacketToAdd);
    Mutex.unlock();
}

//====================================================================================================================
// Append a packet of null sound to the end of the list
//====================================================================================================================
void cSoundBlockList::AppendNullSoundPacket(int64_t Position) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::AppendNullSoundPacket");
    AppendPacket(Position,NULL);
}

//====================================================================================================================
// Synchronise sound and video by adding null sound to catch VideoPosition
//====================================================================================================================
void cSoundBlockList::AdjustSoundPosition(int64_t SoundPosition,int64_t VideoPosition) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::AdjustSoundPosition");
    if ((CurrentTempSize+SoundPacketSize*List.count())==0) {
        if (SoundPosition>VideoPosition) {
            int64_t Delay=int64_t((qreal(SoundPosition-VideoPosition)/1000000)*SamplingRate)*SampleBytes*Channels;
            u_int8_t *NullData=(u_int8_t *)av_malloc(Delay+8);
            memset(NullData,0,Delay);
            AppendData(VideoPosition,(int16_t *)NullData,Delay);
            av_free(NullData);
        }
    } else if (CurrentPosition>VideoPosition) {
        u_int8_t *OldPacket=(u_int8_t *)av_malloc(List.count()*SoundPacketSize+CurrentTempSize+8);
        u_int8_t *CurOldPacket=OldPacket;
        int64_t  CurSize=0;
        int64_t  OldPosition=CurrentPosition;
        while (!List.isEmpty()) {
            u_int8_t *Packet=(u_int8_t *)List.takeFirst();
            memcpy(CurOldPacket,Packet,SoundPacketSize);
            av_free(Packet);
            CurOldPacket+=SoundPacketSize;
            CurSize+=SoundPacketSize;
        }
        if (CurrentTempSize) {
            memcpy(CurOldPacket,TempData,CurrentTempSize);
            CurSize+=CurrentTempSize;
            CurrentTempSize=0;
        }
        CurrentPosition=-1;
        AdjustSoundPosition(OldPosition,VideoPosition);
        AppendData(SoundPosition,(int16_t *)OldPacket,CurSize);
        av_free(OldPacket);
    }
    Adjusted=true;
}

//====================================================================================================================
// Append data to the list creating packet as necessary and filling TempData
//====================================================================================================================
void cSoundBlockList::AppendData(int64_t Position,int16_t *Data,int64_t DataLen) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::AppendData");

    u_int8_t *CurData=(u_int8_t *)Data;
    // Cut data to Packet
    while ((DataLen+CurrentTempSize>=SoundPacketSize)) {
        #if defined(LIBAV) && (LIBAVVERSIONINT<=8)
            u_int8_t *Packet=(u_int8_t *)av_malloc(SoundPacketSize+8);
        #else
            u_int8_t *Packet=NULL;
            int     out_linesize=0;
            av_samples_alloc(&Packet,&out_linesize,Channels,SoundPacketSize+8,SampleFormat,1);
        #endif
        if (Packet) {
            if (CurrentTempSize>0) {                                // Use previously data store in TempData
                int DataToUse=SoundPacketSize-CurrentTempSize;
                memcpy(Packet,TempData,CurrentTempSize);
                memcpy(Packet+CurrentTempSize,CurData,DataToUse);
                DataLen        -=DataToUse;
                CurData        +=DataToUse;
                CurrentTempSize=0;
            } else {                                                // Construct a full packet
                memcpy(Packet,CurData,SoundPacketSize);
                DataLen-=SoundPacketSize;
                CurData+=SoundPacketSize;
            }
            AppendPacket(Position,(int16_t *)Packet);
            Position+=(double(SoundPacketSize/2*Channels)/double(SamplingRate))*AV_TIME_BASE;
        }
    }
    if (DataLen>0) {                                            // Store a partial packet in temp buffer
        // Store data left to TempData
        memcpy(TempData+CurrentTempSize,CurData,DataLen);
        CurrentTempSize+=DataLen;
    }
}

//====================================================================================================================
// Append a packet to the end of the list by mixing 2 packet
// Note : the 2 packet are free during process
//====================================================================================================================
void cSoundBlockList::MixAppendPacket(int64_t Position,int16_t *PacketA,int16_t *PacketB) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::MixAppendPacket");

    int32_t mix;
    if ((PacketA==NULL)&&(PacketB==NULL))           AppendNullSoundPacket(Position);
        else if ((PacketA!=NULL)&&(PacketB==NULL))  AppendPacket(Position,PacketA);
        else if ((PacketA==NULL)&&(PacketB!=NULL))  AppendPacket(Position,PacketB);
        else {
            // Mix the 2 sources buffer using the first buffer as destination
            int16_t *Buf1=PacketA;
            int16_t *Buf2=PacketB;
            for (int j=0;j<SoundPacketSize/(SampleBytes*Channels);j++) {
                // Left channel : Adjust if necessary (16 bits)
                mix=*(Buf1)+*(Buf2++);  if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;     *(Buf1++)=int16_t(mix);
                // Right channel : Adjust if necessary (16 bits)
                mix=*(Buf1)+*(Buf2++);  if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;     *(Buf1++)=int16_t(mix);
            }
            av_free(PacketB); // Free the second buffer
            AppendPacket(Position,PacketA);
    }
}

//====================================================================================================================
// Change volume of a packet
//====================================================================================================================
void cSoundBlockList::ApplyVolume(int PacketNumber,double VolumeFactor) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSoundBlockList::ApplyVolume");

    if (PacketNumber>=List.count()) return;
    int16_t *Buf1=List[PacketNumber];
    if (Buf1==NULL) return;
    int32_t mix;
    for (int j=0;j<SoundPacketSize/(SampleBytes*Channels);j++) {
        // Left channel : Adjust if necessary (16 bits)
        mix=int32_t(double(*(Buf1))*VolumeFactor); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
        // Right channel : Adjust if necessary (16 bits)
        mix=int32_t(double(*(Buf1))*VolumeFactor); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
    }
}
