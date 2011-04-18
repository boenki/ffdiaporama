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

#ifndef CSOUNDDEFINITION_H
#define CSOUNDDEFINITION_H

#include <QtCore>
#include <QList>

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

#define MAXSOUNDPACKETSIZE      4096

class cSoundBlockList {
public:
    int                 SoundPacketSize;            // Size of a packet (depending on FPS)
    int                 NbrPacketForFPS;            // Number of packet for FPS
    QList<int16_t *>    List;                       // List of sound packet
    uint8_t             *TempData;                  // Buffer for stocking temporary data (when decoding data are less than a packet)
    int64_t             CurrentTempSize;            // Amount of data in the TempData buffer
    double              dDuration;                  // Duration of a packet (in time)

    int64_t             Channels;                   // Number of channels
    int64_t             SamplingRate;               // Sampling rate (frequency)
    int64_t             SampleBytes;                // Size of a sample (16 bits=2 bytes)
    double              WantedDuration;             // Duration wanted depending on FPS
    double              FPS;

    cSoundBlockList();
    ~cSoundBlockList();

    void    ClearList();                                                    // Clear the list (make av_free of each packet)
    int16_t *DetachFirstPacket();                                           // Detach the first packet of the list (do not make av_free)
    void    AppendPacket(int16_t *PacketToAdd);                             // Append a packet to the end of the list
    void    AppendNullSoundPacket();                                        // Append a packet of null sound to the end of the list
    void    MixAppendPacket(int16_t *PacketA,int16_t *PacketB);             // Append a packet to the end of the list by mixing 2 packet
    void    AppendData(int16_t *Data,int64_t DataLen);                      // Append data to the list creating packet as necessary and filling TempData
    void    SetFPS(double FPS);                                             // Prepare and calculate values for a frame rate
    void    SetFrameSize(int FrameSize);                                    // Prepare and calculate values for a frame size
    void    ApplyVolume(int PacketNumber,double VolumeFactor);              // Adjust volume
};

#endif // CSOUNDDEFINITION_H
