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

#ifndef WGT_QCUSTOMTHUMBNAILS_H
#define WGT_QCUSTOMTHUMBNAILS_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

class wgt_QCustomThumbnails : public QLabel {
Q_OBJECT
public:
    QTableWidget        *Timeline;
    cDiaporamaObject    *DiaporamaObject;   // Use only for THUMBNAILTYPE_SHOT
    int                 Type;
    bool                HasTransition;
    QRect               TransitionRect;
    bool                HasSoundTrack;
    QRect               SoundTrackRect;
    QRect               MediaObjectRect;

    explicit wgt_QCustomThumbnails(QTableWidget *Timeline,int Type);
    ~wgt_QCustomThumbnails();

signals:
    void        EditTransition();
    void        EditSoundTrack();
    void        EditMediaObject();

public slots:

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent * e);
    virtual void paintEvent (QPaintEvent *);

private:
    void    DrawThumbnailsBox(int Xa,int Ya,int Width,int Height,QPainter &Painter,QImage *Icon);
};

#endif // WGT_QCUSTOMTHUMBNAILS_H
