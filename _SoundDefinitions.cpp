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
#include "_SoundDefinitions.h"
#include "_VideoFileWrapper.h"
#include "_ApplicationDefinitions.h"

//======================================
// Specific defines for this file
//======================================
#define MAXSOUNDPACKETSIZE      4096

//*********************************************************************************************************************************************
// SDL global define values
//*********************************************************************************************************************************************

bool                SDLIsAudioOpen=false;   // true if SDL work at least one time
double              SDLCurrentFPS =-1;      // Current FPS setting for SDL
SDL_AudioSpec       AudioSpec;              // SDL param bloc
cSoundBlockList     MixedMusic;             // Sound to play

//*********************************************************************************************************************************************
// SDL Audio Call Back
//*********************************************************************************************************************************************

void SDLAudioCallback(void *,Uint8 *stream,int len) {
    SDLIsAudioOpen=true;
    if (len!=MixedMusic.SoundPacketSize) {
        qDebug()<<"Error in SDLAudioCallback : Wanted len("<<len<<")<>MixedMusic.SoundPacketSize("<<MixedMusic.SoundPacketSize<<")";
        return;
    }
    int16_t *Packet=MixedMusic.DetachFirstPacket();

    // Copy data to hardware buffer
    if (Packet!=NULL) {
        memcpy(stream,(Uint8 *)Packet,MixedMusic.SoundPacketSize);
        av_free(Packet);
    }
}

//*********************************************************************************************************************************************
// SDL Init/Reinit function
//*********************************************************************************************************************************************

void SDLFirstInit(double WantedFPS,bool SDLAncMode) {
    // Start SDL
    if (SDL_Init(SDL_INIT_AUDIO)) ExitApplicationWithFatalError("SDLFirstInit=Could not initialize SDL :"+QString("%1").arg(SDL_GetError()));
    SDLSetFPS(WantedFPS,SDLAncMode);
}

void SDLLastClose() {
    if (SDLIsAudioOpen) {
        SDL_CloseAudio();                               // Close audio
        SDLIsAudioOpen=false;
        SDL_Quit();                                     // Close library
    }
}

void SDLSetFPS(double WantedFPS,bool SDLAncMode) {
    if (SDLCurrentFPS==WantedFPS) return;
    SDLCurrentFPS=WantedFPS;

    /*if (SDLIsAudioOpen)*/ SDL_CloseAudio();                               // Close audio

    // Init MixedMusic
    MixedMusic.ClearList();                                             // Free sound buffers
    MixedMusic.SetFPS(WantedFPS);

    // Init SDL
    SDL_AudioSpec Desired;
    Desired.channels=MixedMusic.Channels;                               // Number of chanels
    Desired.format  =AUDIO_S16SYS;                                      // Sound format (pcm16le)
    Desired.freq    =MixedMusic.SamplingRate;                           // Frequency in Hz
    Desired.userdata=NULL;                                              // userdata parameter : not used
    Desired.callback=SDLAudioCallback;                                  // Link to callback function
    Desired.samples =MixedMusic.SoundPacketSize/MixedMusic.Channels;    // In samples unit * chanels number for Linux version

    if (!SDLAncMode) Desired.samples/=MixedMusic.SampleBytes;           // New SDL use byte instead of sample

    Desired.silence =0;
    if (SDL_OpenAudio(&Desired,&AudioSpec)<0) {
        ExitApplicationWithFatalError("SDLSetFPS=Error in SDL_OpenAudio:"+QString(SDL_GetError()));
    }
//    SDLIsAudioOpen=true;
}

//*********************************************************************************************************************************************
// Base object for sound manipulation
//*********************************************************************************************************************************************

cSoundBlockList::cSoundBlockList() {
    TempData            =(uint8_t *)av_malloc(MAXSOUNDPACKETSIZE+4);                        // Buffer for stocking temporary data (when decoding data are less than a packet)
    CurrentTempSize     =0;                                                                 // Amount of data in the TempData buffer
    SoundPacketSize     =MAXSOUNDPACKETSIZE;                                                // Size of a packet (depending on FPS)
    Channels            =2;                                                                 // Number of channels
    SamplingRate        =48000;                                                             // Sampling rate (frequency)
    SampleBytes         =2;                                                                 // 16 bits : Size of a sample
    dDuration           =double(SoundPacketSize)/double(Channels*SampleBytes*SamplingRate); // Duration of a packet
    NbrPacketForFPS     =1;                                                                 // Number of packet for FPS
    NeedLockSDL         =false;
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
    int16_t *Ret=NULL;
    if (NeedLockSDL) SDL_LockAudio();
    if (List.count()>0) Ret=(int16_t *)List.takeFirst();
    if (NeedLockSDL) SDL_UnlockAudio();
    return Ret;
}

//====================================================================================================================
// Append a packet to the end of the list
//====================================================================================================================
void cSoundBlockList::AppendPacket(int16_t *PacketToAdd) {
    if (NeedLockSDL) SDL_LockAudio();
    List.append(PacketToAdd);
    if (NeedLockSDL) SDL_UnlockAudio();
}

//====================================================================================================================
// Append a packet of null sound to the end of the list
//====================================================================================================================
void cSoundBlockList::AppendNullSoundPacket() {
    //int16_t *Packet=(int16_t *)av_malloc(SoundPacketSize+4);
    //memset(Packet,0,SoundPacketSize);
    //AppendPacket(Packet);
    AppendPacket(NULL);
}

//====================================================================================================================
// Append data to the list creating packet as necessary and filling TempData
//====================================================================================================================
void cSoundBlockList::AppendData(int16_t *Data,int64_t DataLen) {
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
    if (Buf1==NULL) return;
    int32_t mix;
    for (int j=0;j<SoundPacketSize/(SampleBytes*Channels);j++) {
        // Left channel : Adjust if necessary (16 bits)
        mix=int32_t(double(*(Buf1))*VolumeFactor); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
        // Right channel : Adjust if necessary (16 bits)
        mix=int32_t(double(*(Buf1))*VolumeFactor); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
    }
}

//*********************************************************************************************************************************************
// Base object for music definition
//*********************************************************************************************************************************************

cMusicObject::cMusicObject() {
    IsValide    =false;
    FilePath    ="";
    StartPos    =QTime(0,0,0,0);                // Start position
    EndPos      =QTime(0,0,0,0);                // End position
    Duration    =QTime(0,0,0,0);                // Duration
    FadeIn      =false;
    FadeOut     =false;
    Volume      =1.0;                           // Volume as % from 1% to 150%
    Music       =NULL;                          // Embeded Object (music is the same as video without video track !)
}

//====================================================================================================================

cMusicObject::~cMusicObject() {
    if (Music!=NULL) {
        delete Music;
        Music=NULL;
    }
}

//====================================================================================================================

void cMusicObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    QString         FileName;

    if (PathForRelativPath!="") {
        if (ForceAbsolutPath) FileName=QDir(PathForRelativPath).absoluteFilePath(FilePath);
            else FileName=QDir(PathForRelativPath).relativeFilePath(FilePath);
    } else FileName=FilePath;

    Element.setAttribute("FilePath",FileName);
    Element.setAttribute("StartPos",StartPos.toString());
    Element.setAttribute("EndPos",  EndPos.toString());
    Element.setAttribute("FadeIn",  FadeIn?"1":"0");
    Element.setAttribute("FadeOut", FadeOut?"1":"0");
    Element.setAttribute("Volume",  QString("%1").arg(Volume,0,'f'));

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cMusicObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList &AliasList) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        FilePath=Element.attribute("FilePath","");
        if (PathForRelativPath!="") FilePath=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(FilePath));
        if (LoadMedia(FilePath,AliasList)) {
            StartPos=QTime().fromString(Element.attribute("StartPos"));
            EndPos  =QTime().fromString(Element.attribute("EndPos"));
            FadeIn  =Element.attribute("FadeIn")=="1";
            FadeOut =Element.attribute("FadeOut")=="1";
            Volume  =Element.attribute("Volume").toDouble();
            return true;
        } else return false;
    } else return false;
}

//====================================================================================================================

bool cMusicObject::LoadMedia(QString &filename,QStringList &AliasList) {
    // Clean all
    if (Music!=NULL) {
        delete Music;
        Music=NULL;
    }

    Music=new cvideofilewrapper();
    IsValide=Music->GetInformationFromFile(filename,true,AliasList);
    FilePath=QFileInfo(Music->FileName).absoluteFilePath();
    StartPos=QTime(0,0,0,0);                // Start position
    EndPos  =Music->Duration;
    Duration=Music->Duration;
    return IsValide;
}
