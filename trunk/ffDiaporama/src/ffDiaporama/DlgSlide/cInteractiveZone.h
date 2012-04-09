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

#ifndef CINTERACTIVEZONE_H
#define CINTERACTIVEZONE_H

#include "../../sharedfiles/_GlobalDefines.h"
#include "../_Diaporama.h"
#include "../_ApplicationDefinitions.h"
#include "cCustomBlockTable.h"

#include <QWidget>

enum TRANSFOTYPE {NOTYETDEFINED,MOVEBLOCK,RESIZEUPLEFT,RESIZEDOWNLEFT,RESIZEUPRIGHT,RESIZEDOWNRIGHT,RESIZELEFT,RESIZERIGHT,RESIZEUP,RESIZEDOWN};

class cInteractiveZone : public QWidget {
Q_OBJECT
public:
    int                 MagneticRuler;
    cCustomBlockTable   *BlockTable;
    cDiaporamaObject    *DiaporamaObject;
    int                 CurrentShotNbr;                // Current shot number
    QImage              *BackgroundImage;
    QImage              *ForegroundImage;
    QRectF              SceneRect;
    QRectF              CurSelRect;
    bool                IsCapture;                      // True if there is an active capture
    QPoint              CapturePos;
    QList<bool>         IsSelected;
    int                 NbrSelected;
    bool                LockGeometry;
    bool                SelectionHaveLockBlock;         // True if selection contains block with SameAsPreviousShot propertie

    // Selection rectangle properties
    double              AspectRatio;                    // Aspect ratio of the (multi)selection rectangle
    double              Sel_X,Sel_Y,Sel_W,Sel_H;        // Position and size of the current (multi)selection rectangle

    // Transformations
    TRANSFOTYPE         TransfoType;
    double              Move_X,Move_Y;                  // Blocks move
    double              Scale_X,Scale_Y;                // Blocks resize

    explicit cInteractiveZone(QWidget *parent = 0);
    ~cInteractiveZone();

    void    RefreshDisplay();

protected:
    virtual void        paintEvent(QPaintEvent *event);

    virtual void	mouseDoubleClickEvent(QMouseEvent *event);
    virtual void	mouseMoveEvent(QMouseEvent *event);
    virtual void	mousePressEvent(QMouseEvent *event);
    virtual void	mouseReleaseEvent(QMouseEvent *event);
    virtual void	keyPressEvent(QKeyEvent *event);
    virtual void	keyReleaseEvent(QKeyEvent *event);

signals:
    void    StartSelectionChange();
    void    EndSelectionChange();
    void    RightClickEvent(QMouseEvent *ev);
    void    DoubleClickEvent(QMouseEvent *ev);
    void    TransformBlock(double Move_X,double Move_Y,double Scale_X,double Scale_Y,double Sel_X,double Sel_Y,double Sel_W,double Sel_H);
    void    DisplayTransformBlock(double Move_X,double Move_Y,double Scale_X,double Scale_Y,double Sel_X,double Sel_Y,double Sel_W,double Sel_H);

public slots:
    void    DifferedEmitRightClickEvent();
    void    DifferedEmitDoubleClickEvent();

private:
    bool    IsInRect(QPoint Pos,QRect Rect);
    bool    IsInSelectedRect(QPoint Pos);
    void    GetForDisplayUnit(double &DisplayW,double &DisplayH);
    void    UpdateIsSelected();
    void    ManageCursor(QPoint Pos,Qt::KeyboardModifiers Modifiers);
    QRect   ComputeNewCurSelRect();
    void    DrawSelect(QPainter &Painter,QRectF Rect,bool WithHandles);
};

#endif // CINTERACTIVEZONE_H
