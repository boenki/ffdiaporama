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

#include "cSoundBlockList.h"

// Specific defines for this file
#define MAXSOUNDPACKETSIZE      4096

//#define DEBUGMODE

//*********************************************************************************************************************************************
// Base object for sound manipulation
//*********************************************************************************************************************************************

cSoundBlockList::cSoundBlockList() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::cSoundBlockList";
    #endif
    TempData            =(uint8_t *)av_malloc(MAXSOUNDPACKETSIZE+4);                        // Buffer for stocking temporary data (when decoding data are less than a packet)
    CurrentTempSize     =0;                                                                 // Amount of data in the TempData buffer
    SoundPacketSize     =MAXSOUNDPACKETSIZE;                                                // Size of a packet (depending on FPS)
    Channels            =2;                                                                 // Number of channels
    SamplingRate        =48000;                                                             // Sampling rate (frequency)
    SampleBytes         =2;                                                                 // 16 bits : Size of a sample
    dDuration           =double(SoundPacketSize)/double(Channels*SampleBytes*SamplingRate); // Duration of a packet
    NbrPacketForFPS     =1;                                                                 // Number of packet for FPS
}

//====================================================================================================================

cSoundBlockList::~cSoundBlockList() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::~cSoundBlockList";
    #endif
    ClearList();
    av_free(TempData);
    TempData=NULL;
}

//====================================================================================================================
// Prepare and calculate values for a frame rate
//====================================================================================================================
void cSoundBlockList::SetFPS(double TheFPS) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::SetFPS";
    #endif
    FPS            =TheFPS;
    WantedDuration =double(1)/FPS;
    SoundPacketSize=int(WantedDuration*double(SamplingRate))*SampleBytes*Channels;
    if (SoundPacketSize==31996) SoundPacketSize=32000; // Fix a rounded error for 6 FPS
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::SetFrameSize";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::ClearList";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::DetachFirstPacket";
    #endif
    int16_t *Ret=NULL;
    if (List.count()>0) Ret=(int16_t *)List.takeFirst();
    return Ret;
}

//====================================================================================================================
// Append a packet to the end of the list
//====================================================================================================================
void cSoundBlockList::AppendPacket(int16_t *PacketToAdd) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::AppendPacket";
    #endif
    List.append(PacketToAdd);
}

//====================================================================================================================
// Append a packet of null sound to the end of the list
//====================================================================================================================
void cSoundBlockList::AppendNullSoundPacket() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::AppendNullSoundPacket";
    #endif
    AppendPacket(NULL);
}

//====================================================================================================================
// Append data to the list creating packet as necessary and filling TempData
//====================================================================================================================
void cSoundBlockList::AppendData(int16_t *Data,int64_t DataLen) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::AppendData";
    #endif
    uint8_t *CurData=(uint8_t *)Data;
    // Cut data to Packet
    while ((DataLen+CurrentTempSize>=SoundPacketSize)) {
        uint8_t *Packet=(uint8_t *)av_malloc(SoundPacketSize+4);
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
            AppendPacket((int16_t *)Packet);
        }
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::MixAppendPacket";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cSoundBlockList::ApplyVolume";
    #endif
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
