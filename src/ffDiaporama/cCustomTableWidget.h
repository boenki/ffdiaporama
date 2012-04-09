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

#ifndef CCUSTOMTABLEWIDGET_H
#define CCUSTOMTABLEWIDGET_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"

#include <QTableWidget>

// Specific inclusions
#include "_Diaporama.h"
//#include "mainwindow.h"

class cCustomTableWidget : public QTableWidget {
Q_OBJECT
public:
    bool        PartitionMode;

    explicit    cCustomTableWidget(QWidget *parent = 0);

    void        AddObjectToTimeLine(int CurIndex);
    void        SetTimelineHeight(bool NewPartitionMode);
    int         CurrentSelected();
    int         NbrItem();
    void        SetCurrentCell(int Index);
    void        CleanAll();
    void        ResetDisplay(int Selected);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private slots:

signals:
    void    DragMoveItem();
    void    RightClickEvent(QMouseEvent *ev);
};

#endif // CCUSTOMTABLEWIDGET_H
