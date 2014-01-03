/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2014 Dominique Levray <domledom@laposte.net>

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

#ifndef QCustomRuler_H
#define QCustomRuler_H

// Basic inclusions (common to all files)
#include "engine/_GlobalDefines.h"

#include <QLabel>
#include <QSlider>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>

class QCustomRuler : public QSlider {
Q_OBJECT
public:
    int     TotalDuration;      // Total duration in MSec of the movie
    int     StartPos;           // Start position of the marked zone
    int     EndPos;             // End position of the marked zone
    int     PreviousStartPos;   // Start position of the previous slide
    int     PrevisousEndPos;    // End position of the previous slide
    int     NextStartPos;       // Start position of the next slide
    int     NextEndPos;         // End position of the next slide

    explicit QCustomRuler(QWidget *parent = 0);

    void    ActiveSlider(int TotalDuration);

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent (QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);

signals:
    void    PositionChangeByUser();

public slots:

};

#endif // QCustomRuler_H
