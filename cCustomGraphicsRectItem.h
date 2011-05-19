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

#ifndef CCUSTOMGRAPHICSRECTITEM_H
#define CCUSTOMGRAPHICSRECTITEM_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_Diaporama.h"

//======================================
// Specific defines for this dialog box
//======================================
#define TYPE_wgt_QCustomScene           0
#define TYPE_wgt_QCompositionWidget     1
#define TYPE_DlgSlideProperties         2

struct  sMagneticRuller {
    bool     MagneticRuler;
    double   MagnetX1;
    double   MagnetY1;
    double   MagnetX2;
    double   MagnetY2;
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
// class use to add interractive resize to QGraphicsRectItem object
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
class cCustomGraphicsRectItem;
class cResizeGraphicsRectItem : public QGraphicsRectItem {
public:
    cCustomGraphicsRectItem *RectItem;
    int                     ZValue;
    int                     TypeItem;
    bool                    IsCapture;
    bool                    BlockRecursion;

    explicit cResizeGraphicsRectItem(QGraphicsScene *scene,cCustomGraphicsRectItem *RectItem,int ZValue,int TypeItem,QGraphicsItem *parent=NULL);
    virtual ~cResizeGraphicsRectItem();

    void                CalcPosition();

protected:
    virtual void        mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void        mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual QVariant    itemChange(GraphicsItemChange change,const QVariant &value);
    virtual void        paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget=NULL);

private:
    void                ResizeUpperLeft(QPointF &newpos);
    void                ResizeUpperRight(QPointF &newpos);
    void                ResizeBottomLeft(QPointF &newpos);
    void                ResizeBottomRight(QPointF &newpos);
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
// class use to crop rectangle into the image
//---------------------------------------------------------------------------------------------------------------------------------------------------------------

class cCustomGraphicsRectItem : public QGraphicsRectItem {
public:
    QWidget             *ParentWidget;
    int                 ParentWidgetType;
    double               *x,*y,*w,*h,*zoom;
    sMagneticRuller     *MagneticRuller;    // Link to MagneticRuller parent structure
    bool                BlockZoomChange;    // flag to block zoom changing during change % to pixel
    bool                IsCapture;
    bool                KeepAspectRatio;    // true if aspect ratio is keep during resize
    double              AspectRatio;
    QGraphicsScene      *scene;

    cResizeGraphicsRectItem *UpperLeft;
    cResizeGraphicsRectItem *UpperRight;
    cResizeGraphicsRectItem *BottomLeft;
    cResizeGraphicsRectItem *BottomRight;

    explicit cCustomGraphicsRectItem(QGraphicsScene *scene,int ZValue,double *x,double *y,double *zoom,double *w,double *h,double xmax,double ymax,
                                     bool KeepAspectRatio,double AspectRatio,sMagneticRuller *MagneticRuller,QWidget *ParentWidget,int ParentWidgetType);
    virtual ~cCustomGraphicsRectItem();

    void                SendRefreshBackgroundImage();
    void                RecalcEmbededResizeRectItem();

protected:
    virtual void        paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget=0);
    virtual void        mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void        mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual QVariant    itemChange(GraphicsItemChange change,const QVariant &value);

private:
    double               StockW,StockH;  // Stock value when zoom mode is use
};

#endif // CCUSTOMGRAPHICSRECTITEM_H
