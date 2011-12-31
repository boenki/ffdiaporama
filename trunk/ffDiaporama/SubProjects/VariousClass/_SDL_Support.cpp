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

#include "_SDL_Support.h"

//#define DEBUGMODE

//*********************************************************************************************************************************************
// SDL global define values
//*********************************************************************************************************************************************

bool                SDLIsAudioOpen=false;   // true if SDL work at least one time
double              SDLCurrentFPS =-1;      // Current FPS setting for SDL
SDL_AudioSpec       AudioSpec;              // SDL param bloc
cSDLSoundBlockList  MixedMusic;             // Sound to play

//*********************************************************************************************************************************************
// SDL Audio Call Back
//*********************************************************************************************************************************************

void SDLAudioCallback(void *,Uint8 *stream,int len) {
    #ifdef DEBUGMODE
    qDebug() << "IN:SDLAudioCallback";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:SDLFirstInit";
    #endif
    // Start SDL
    if (SDL_Init(SDL_INIT_AUDIO)) {
        qDebug()<<"SDLFirstInit=Could not initialize SDL :"<<SDL_GetError();
        exit(1);    // ExitApplicationWithFatalError
    }
    SDLSetFPS(WantedFPS,SDLAncMode);
}

//*********************************************************************************************************************************************
// SDL Close function
//*********************************************************************************************************************************************

void SDLLastClose() {
    #ifdef DEBUGMODE
    qDebug() << "IN:SDLLastClose";
    #endif
    if (SDLIsAudioOpen) {
        SDL_CloseAudio();                               // Close audio
        SDLIsAudioOpen=false;
        SDL_Quit();                                     // Close library
    }
}

//*********************************************************************************************************************************************
// SDLSetFPS function
//*********************************************************************************************************************************************

void SDLSetFPS(double WantedFPS,bool SDLAncMode) {
    #ifdef DEBUGMODE
    qDebug() << "IN:SDLSetFPS";
    #endif
    //if (SDLCurrentFPS==WantedFPS) return;
    SDLCurrentFPS=WantedFPS;

    SDL_CloseAudio();                               // Close audio

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
        qDebug()<<"SDLFirstInit=Error in SDL_OpenAudio:"<<SDL_GetError();
        exit(1);    // ExitApplicationWithFatalError
    }
//    SDLIsAudioOpen=true;
}

//====================================================================================================================
// Construct block list -> SDL Version
//====================================================================================================================
cSDLSoundBlockList::cSDLSoundBlockList():cSoundBlockList() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSDLSoundBlockList::cSDLSoundBlockList";
    #endif
}

//====================================================================================================================
// Detach the first packet of the list (do not make av_free) -> SDL Version
//====================================================================================================================
int16_t *cSDLSoundBlockList::DetachFirstPacket() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSDLSoundBlockList::DetachFirstPacket";
    #endif
    int16_t *Ret=NULL;
    SDL_LockAudio();
    if (List.count()>0) Ret=(int16_t *)List.takeFirst();
    SDL_UnlockAudio();
    return Ret;
}

//====================================================================================================================
// Append a packet to the end of the list -> SDL Version
//====================================================================================================================
void cSDLSoundBlockList::AppendPacket(int16_t *PacketToAdd) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cSDLSoundBlockList::AppendPacket";
    #endif
    SDL_LockAudio();
    List.append(PacketToAdd);
    SDL_UnlockAudio();
}