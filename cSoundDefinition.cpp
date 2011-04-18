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

#include <QtCore>
#include "cSoundDefinition.h"

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
    #include <libavutil/common.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}
#endif

//*********************************************************************************************************************************************
// Base object for sound manipulation
//*********************************************************************************************************************************************

cSoundBlockList::cSoundBlockList() {
    TempData            =(uint8_t *)av_malloc(MAXSOUNDPACKETSIZE+4);                        // Buffer for stocking temporary data (when decoding data are less than a packet)
    CurrentTempSize     =0;                                                                 // Amount of data in the TempData buffer
    SoundPacketSize     =MAXSOUNDPACKETSIZE;                                                // Size of a packet (depending on FPS)
    Channels            =2;                                                                 // Number of channels
    SamplingRate        =48000;                                                             // Sampling rate (frequency)
    SampleBytes         =av_get_bits_per_sample_format(SAMPLE_FMT_S16)>>3;                  // Size of a sample
    dDuration           =double(SoundPacketSize)/double(Channels*SampleBytes*SamplingRate); // Duration of a packet
    NbrPacketForFPS     =1;                                                                 // Number of packet for FPS
}

//====================================================================================================================

cSoundBlockList::~cSoundBlockList() {
    ClearList();
    av_free(TempData);
    TempData=NULL;
}

//====================================================================================================================
// Prepare and calculate values for a frame rate
//====================================================================================================================
void cSoundBlockList::SetFPS(double TheFPS) {
    FPS            =TheFPS;
    WantedDuration =double(1)/FPS;
    SoundPacketSize=int(WantedDuration*double(SamplingRate)*double(SampleBytes)*double(Channels));
    NbrPacketForFPS=1;
    dDuration      =WantedDuration;
    while (SoundPacketSize>MAXSOUNDPACKETSIZE) {
        SoundPacketSize =SoundPacketSize/2;
        dDuration       =dDuration/2;
        NbrPacketForFPS*=2;
    }
}

//====================================================================================================================
// Prepare and calculate values for a frame size
//====================================================================================================================
void cSoundBlockList::SetFrameSize(int FrameSize) {
    SoundPacketSize=FrameSize;
    WantedDuration =double(SoundPacketSize)/(double(SamplingRate)*double(SampleBytes)*double(Channels));
    FPS            =1/WantedDuration;
    NbrPacketForFPS=1;
    dDuration      =WantedDuration;
    while (FPS>30) {
        WantedDuration *=2;
        FPS            /=2;
    }
}

//====================================================================================================================
// Clear the list (make av_free of each packet)
//====================================================================================================================
void cSoundBlockList::ClearList() {
    while (List.count()>0) {
        int16_t *Packet=DetachFirstPacket();
        if (Packet) av_free(Packet);
    }
    CurrentTempSize=0;
}

//====================================================================================================================
// Detach the first packet of the list (do not make av_free)
//====================================================================================================================
int16_t *cSoundBlockList::DetachFirstPacket() {
    if (List.count()>0) return (int16_t *)List.takeFirst(); else return NULL;
}

//====================================================================================================================
// Append a packet to the end of the list
//====================================================================================================================
void cSoundBlockList::AppendPacket(int16_t *PacketToAdd) {
    List.append(PacketToAdd);
}

//====================================================================================================================
// Append a packet of null sound to the end of the list
//====================================================================================================================
void cSoundBlockList::AppendNullSoundPacket() {
    int16_t *Packet=(int16_t *)av_malloc(SoundPacketSize+4);
    memset(Packet,0,SoundPacketSize);
    AppendPacket(Packet);
}

//====================================================================================================================
// Append data to the list creating packet as necessary and filling TempData
//====================================================================================================================
void cSoundBlockList::AppendData(int16_t *Data,int64_t DataLen) {
    uint8_t *CurData=(uint8_t *)Data;
    // Cut data to Packet
    while ((DataLen+CurrentTempSize>=SoundPacketSize)) {
        uint8_t *Packet=(uint8_t *)av_malloc(SoundPacketSize+4);
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
        AppendPacket((int16_t *)Packet);
    }
    if (DataLen>0) {                                            // Store a partial packet in temp buffer
        // Store data left to TempData
        memcpy(TempData,CurData,DataLen);
        CurrentTempSize=DataLen;
    }
}

//====================================================================================================================
// Append a packet to the end of the list by mixing 2 packet
// Note : the 2 packet are free during process
//====================================================================================================================
void cSoundBlockList::MixAppendPacket(int16_t *PacketA,int16_t *PacketB) {
    int32_t mix;
    if ((PacketA==NULL)&&(PacketB==NULL))           AppendNullSoundPacket();
        else if ((PacketA!=NULL)&&(PacketB==NULL))  AppendPacket(PacketA);
        else if ((PacketA==NULL)&&(PacketB!=NULL))  AppendPacket(PacketB);
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
            AppendPacket(PacketA);
    }
}

//====================================================================================================================
// Change volume of a packet
//====================================================================================================================
void cSoundBlockList::ApplyVolume(int PacketNumber,double VolumeFactor) {
    int16_t *Buf1=List[PacketNumber];
    int32_t mix;
    for (int j=0;j<SoundPacketSize/(SampleBytes*Channels);j++) {
        // Left channel : Adjust if necessary (16 bits)
        mix=int32_t(double(*(Buf1))*VolumeFactor); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
        // Right channel : Adjust if necessary (16 bits)
        mix=int32_t(double(*(Buf1))*VolumeFactor); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
    }
}
