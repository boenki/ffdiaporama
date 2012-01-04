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

#ifndef wgt_QCustomRuller_H
#define wgt_QCustomRuller_H

// Basic inclusions (common to all files)
#include "SubProjects/VariousClass/_GlobalDefines.h"

// Include some additional standard class

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

class wgt_QCustomRuller : public QLabel {
Q_OBJECT
public:
    int     TotalDuration;      // Total duration in MSec of the movie
    int     StartPos;           // Start position of the marked zone
    int     EndPos;             // End position of the marked zone
    int     PreviousStartPos;   // Start position of the previous slide
    int     PrevisousEndPos;    // End position of the previous slide
    int     NextStartPos;       // Start position of the next slide
    int     NextEndPos;         // End position of the next slide

    QSlider *Slider;            // Embeded QSlider object

    explicit wgt_QCustomRuller(QWidget *parent = 0);
    ~wgt_QCustomRuller();

    void    ActiveSlider(int TotalDuration);

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent (QPaintEvent *);

signals:

public slots:

};

#endif // wgt_QCustomRuller_H
