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

#ifndef QCUSTOMFILEINFOLABEL_H
#define QCUSTOMFILEINFOLABEL_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QLabel>
#include <QWidget>
#include <QPaintEvent>
#include <QString>
#include <QIcon>

#include "cBaseMediaFile.h"

// defines allowed display mode
#define DISPLAY_DATA        0
#define DISPLAY_WEBSHORT    1
#define DISPLAY_WEBLONG     2
#define DISPLAY_ICON48      3
#define DISPLAY_ICON100     4
#define DISPLAY_ICONWALL    5

class QCustomFileInfoLabel : public QLabel {
Q_OBJECT
public:
    int             DisplayMode;
    QString         ShortName;
    QString         Text[3][2];
    QImage          *IconType;
    QImage          *Icon16;
    QImage          *Icon32;
    QImage          *Icon48;
    QImage          *Icon100;

    explicit        QCustomFileInfoLabel(QWidget *parent = 0);
    virtual void    paintEvent(QPaintEvent *event);
    virtual void    mouseDoubleClickEvent(QMouseEvent *);

    virtual void    SetupFileInfoLabel(cBaseMediaFile *MediaObject);

signals:
    void    DoubleClickEvent();

public slots:

};

#endif // QCUSTOMFILEINFOLABEL_H
