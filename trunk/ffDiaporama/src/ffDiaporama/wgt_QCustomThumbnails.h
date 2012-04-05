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

#ifndef WGT_QCUSTOMTHUMBNAILS_H
#define WGT_QCUSTOMTHUMBNAILS_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"
#include "cCustomTableWidget.h"

#include <QLabel>
#include <QMouseEvent>

// Thumbnails definitions
#define ADJUSTXCOLUMN                       10      // width to add to object image for display transition
#define TIMELINEMINHEIGH                    80      // min heigh of montage track
#define TIMELINEMAXHEIGH                    160     // max heigh of montage track
#define TIMELINESOUNDHEIGHT                 18      // Height of sound & music track

// Thumbnail type definition
#define THUMBNAILTYPE_OBJECT                0
#define THUMBNAILTYPE_NULL                  1
#define THUMBNAILTYPE_SHOT                  2

class wgt_QCustomThumbnails : public QLabel {
Q_OBJECT
public:
    QTableWidget        *Timeline;
    cDiaporamaObject    *DiaporamaObject;   // Use only for THUMBNAILTYPE_SHOT
    int                 Type;
    bool                HasBackGTransition;
    QRect               BackGTransitionRect;
    bool                HasTransition;
    QRect               TransitionRect;
    bool                HasSoundTrack;
    QRect               SoundTrackRect;
    QRect               BackgroundRect;
    QRect               MediaObjectRect;
    QRect               MusicTrackRect;

    explicit wgt_QCustomThumbnails(QTableWidget *Timeline,int Type);
    ~wgt_QCustomThumbnails();

signals:
    void        EditTransition();
    void        EditSoundTrack();
    void        EditMediaObject();
    void        EditBackGTransition();
    void        EditBackground();
    void        EditMusicTrack();
    void        RightClick();

public slots:

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
    virtual void paintEvent (QPaintEvent *);

private:
    void    DrawThumbnailsBox(int Xa,int Ya,int Width,int Height,QPainter &Painter,QImage *Icon);
};

#endif // WGT_QCUSTOMTHUMBNAILS_H
