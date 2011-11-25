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

#ifndef DLGRENDERVIDEO_H
#define DLGRENDERVIDEO_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

namespace Ui {
    class DlgRenderVideo;
}

class DlgRenderVideo : public QDialog {
Q_OBJECT
public:
    cDiaporama      *Diaporama;
    int             ExportMode;                     // Export mode (smartphone, advanced, etc...)
    bool            StopSpinboxRecursion;
    bool            StopProcessWanted;              // True if user click on cancel or close during encoding process
    int             Extend;                         // amout of padding (top and bottom) for cinema mode with DVD
    int             VideoCodecIndex;                // Index of video codec
    int             AudioCodecIndex;                // Index of audio codec
    double          FPS;
    int             NbrFrame;                       // Number of frame to generate
    QTime           StartTime;                      // Time the process start
    QTime           LastCheckTime;                  // Last time the loop start
    bool            IsDestFileOpen;                 // true if encoding is started

    QString         OutputFileName;                 // filename with path for the last rendering file
    int             OutputFileFormat;               // Container format
    QString         VideoCodec;                     // Last video codec used
    double          VideoFrameRate;                 // Last video frame rate used
    int             VideoBitRate;                   // Last video bit rate used
    QString         AudioCodec;                     // Last audio codec used
    int             AudioFrequency;                 // Last audio frequency used
    int             AudioBitRate;                   // Last audio bit rate used
    int             ImageSize;                      // Last image size use for rendering
    int             Standard;                       // Last standard use for rendering

    explicit DlgRenderVideo(cDiaporama &Diaporama,int ExportMode,QWidget *parent = 0);
    ~DlgRenderVideo();

    void            DisplayInformations(QString Text);
    bool            WriteTempAudioFile(QString TempWAVFileName,int FromSlide);

protected:
    virtual void    showEvent(QShowEvent *);
    virtual void    reject();
    virtual void    accept();

private slots:
    void            Help();
    void            SetSavedWindowGeometry();
    void            InitImageSizeCombo(int);
    void            SelectDestinationFile();
    void            AdjustDestinationFile();
    void            FileFormatCombo(int);
    void            InitVideoBitRateCB(int);
    void            InitAudioBitRateCB(int);
    void            s_DeviceTypeCB(int);
    void            s_DeviceModelCB(int);
    void            SetZoneToAll();
    void            SetZoneToPartial();

private:
    Ui::DlgRenderVideo *ui;
};

#endif // DLGRENDERVIDEO_H
