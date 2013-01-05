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

#include "_SDL_Support.h"
#include "_QCustomDialog.h"
#include <QMessageBox>

//*********************************************************************************************************************************************
// SDL global define values
//*********************************************************************************************************************************************

bool                SDLIsAudioOpen=false;   // true if SDL work at least one time
double              SDLCurrentFPS =-1;      // Current FPS setting for SDL
SDL_AudioSpec       AudioSpec;              // SDL param bloc
cSDLSoundBlockList  MixedMusic;             // Sound to play
Uint8               SDLBuf[64000];
int32_t             SDLBufSize=0;

//*********************************************************************************************************************************************
// SDL Audio Call Back
//*********************************************************************************************************************************************

void SDLAudioCallback(void *,Uint8 *stream,int len) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:SDLAudioCallback");

    SDLIsAudioOpen=true;
    if (len!=MixedMusic.SoundPacketSize) {
        ToLog(LOGMSG_CRITICAL,QString("Error in SDLAudioCallback : Wanted len(%1)<>MixedMusic.SoundPacketSize(%2)").arg(len).arg(MixedMusic.SoundPacketSize));
        return;
    }
    int32_t CurPos=0;
    if (SDLBufSize>0) {
        memcpy(stream,&SDLBuf,SDLBufSize);
        CurPos=SDLBufSize;
        SDLBufSize=0;
    }
    while (len>0) {
        int16_t *Packet=MixedMusic.DetachFirstPacket();
        if (Packet!=NULL) {
            if (len>=MixedMusic.SoundPacketSize) {
                memcpy(stream+CurPos,(Uint8 *)Packet,MixedMusic.SoundPacketSize);
                CurPos+=MixedMusic.SoundPacketSize;
                len   -=MixedMusic.SoundPacketSize;
            } else {
                memcpy(stream+CurPos,(Uint8 *)Packet,len);
                SDLBufSize=MixedMusic.SoundPacketSize-len;
                memcpy(&SDLBuf,(Uint8 *)Packet+len,SDLBufSize);
                CurPos+=len;
                len   -=MixedMusic.SoundPacketSize;
            }
            av_free(Packet);
        } else {
            memset(stream+CurPos,0,len);
            len=0;
            CurPos=0;
            SDLBufSize=0;
        }
    }
}

//*********************************************************************************************************************************************
// SDL Init/Reinit function
//*********************************************************************************************************************************************

void SDLFirstInit(double WantedFPS,bool SDLAncMode) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:SDLFirstInit");

    // Start SDL
    if (SDL_Init(SDL_INIT_AUDIO)) {
        CustomMessageBox(NULL,QMessageBox::Critical,QApplication::translate("MainWindow","Error during startup"),
                QApplication::translate("MainWindow","Error during initialisation of sound system. Check your configuration and try again"),
                QMessageBox::Close,QMessageBox::Close);
        ToLog(LOGMSG_CRITICAL,QString("SDLFirstInit=Could not initialize SDL :%1").arg(SDL_GetError()));
        exit(1);    // ExitApplicationWithFatalError
    }
    SDLSetFPS(WantedFPS,SDLAncMode);
}

//*********************************************************************************************************************************************
// SDL Close function
//*********************************************************************************************************************************************

void SDLLastClose() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:SDLLastClose");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:SDLSetFPS");

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
    Desired.size    =Desired.samples*2;
    Desired.padding =0;
    Desired.silence =0;
    if (!SDLAncMode) Desired.samples/=MixedMusic.SampleBytes;           // New SDL use byte instead of sample
    AudioSpec=Desired;
    bool Error=false;
    if (SDL_OpenAudio(&Desired,&AudioSpec)<0) Error=true;
    if (Error) {
        CustomMessageBox(NULL,QMessageBox::Critical,QApplication::translate("MainWindow","Error during startup"),
                QApplication::translate("MainWindow","Error during initialisation of sound system. Check your configuration and try again"),
                QMessageBox::Close,QMessageBox::Close);
        ToLog(LOGMSG_CRITICAL,QString("SDLFirstInit=Error in SDL_OpenAudio:%1").arg(SDL_GetError()));
        exit(1);    // ExitApplicationWithFatalError
    }
}

//====================================================================================================================
// Construct block list -> SDL Version
//====================================================================================================================
cSDLSoundBlockList::cSDLSoundBlockList():cSoundBlockList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSDLSoundBlockList::cSDLSoundBlockList");
}

//====================================================================================================================
// Detach the first packet of the list (do not make av_free) -> SDL Version
//====================================================================================================================
int16_t *cSDLSoundBlockList::DetachFirstPacket() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSDLSoundBlockList::DetachFirstPacket");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSDLSoundBlockList::AppendPacket");

    SDL_LockAudio();
    List.append(PacketToAdd);
    SDL_UnlockAudio();
}
