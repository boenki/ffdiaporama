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

#include "cCustomGraphicsRectItem.h"
#include "wgt_QCompositionWidget.h"
#include "DlgSlideProperties.h"
#include "DlgImageCorrection.h"

#define HANDLESIZEX     8
#define HANDLESIZEY     8
#define HANDLEMAGNETX   18
#define HANDLEMAGNETY   10

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
// class use to add interractive resize to QGraphicsRectItem object
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
cResizeGraphicsRectItem::cResizeGraphicsRectItem(QGraphicsScene *scene,cCustomGraphicsRectItem *TheRectItem,int ZValue,int TheTypeItem,QGraphicsItem *parent)
    :QGraphicsRectItem((*TheRectItem->x)*double(scene->sceneRect().width()),(*TheRectItem->y)*double(scene->sceneRect().height()),
     double(scene->sceneRect().width())*(*((TheRectItem->zoom!=NULL)?TheRectItem->zoom:TheRectItem->w)),
     double(scene->sceneRect().height())*(*((TheRectItem->zoom!=NULL)?TheRectItem->zoom:TheRectItem->h)),parent) {

    RectItem        = TheRectItem;
    TypeItem        = TheTypeItem;
    IsCapture       = false;
    BlockRecursion  = false;            // Flag to stop recursion during resize

    // define a pen for the rectangle
    QColor  Col=QColor(255,0,0);
    QPen    Pen=QPen(Col);
    Pen.setWidth(1);
    setPen(Pen);

    // define Mouse cursor
    switch (TypeItem) {
        case 0 :  setCursor(Qt::SizeFDiagCursor);    break;  // Upper-Left corner
        case 1 :  setCursor(Qt::SizeBDiagCursor);    break;  // Upper-Right corner
        case 2 :  setCursor(Qt::SizeBDiagCursor);    break;  // Bottom-Left corner
        case 3 :  setCursor(Qt::SizeFDiagCursor);    break;  // Bottom-Right corner
        case 4 :  setCursor(Qt::SizeVerCursor);      break;  // Upper
        case 5 :  setCursor(Qt::SizeVerCursor);      break;  // Bottom
        case 6 :  setCursor(Qt::SizeHorCursor);      break;  // Left
        case 7 :  setCursor(Qt::SizeHorCursor);      break;  // Right
    }
    // define Flags
    setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemClipsChildrenToShape|QGraphicsItem::ItemSendsScenePositionChanges|QGraphicsItem::ItemSendsGeometryChanges);
//    setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges|QGraphicsItem::ItemSendsGeometryChanges);
    setZValue(ZValue);
    setData(0,QVariant("ResizeGraphicsRectItem"));

    // Add this item to the scene
    scene->addItem(this);
    CalcPosition();
}

//====================================================================================================================

cResizeGraphicsRectItem::~cResizeGraphicsRectItem() {
    switch (TypeItem) {
        case 0 :  RectItem->UpperLeft=NULL;     break;  // Upper-Left corner
        case 1 :  RectItem->UpperRight=NULL;    break;  // Upper-Right corner
        case 2 :  RectItem->BottomLeft=NULL;    break;  // Bottom-Left corner
        case 3 :  RectItem->BottomRight=NULL;   break;  // Bottom-Right corner
        case 4 :  RectItem->Upper=NULL;         break;  // Upper
        case 5 :  RectItem->Bottom=NULL;        break;  // Bottom
        case 6 :  RectItem->Left=NULL;          break;  // Left
        case 7 :  RectItem->Right=NULL;         break;  // Right
    }
}

//====================================================================================================================

void cResizeGraphicsRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsRectItem::mousePressEvent(event);
      IsCapture = true;
}

//====================================================================================================================

void cResizeGraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
      IsCapture = false;
      QGraphicsRectItem::mouseReleaseEvent(event);
}

//====================================================================================================================

void cResizeGraphicsRectItem::CalcPosition() {
    if (scene()==NULL) return;
    double xmax = scene()->sceneRect().width();
    double ymax = scene()->sceneRect().height();
    double x=0;
    double y=0;
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;
    switch (TypeItem) {
        case 0 :    // Upper-Left corner
            x = (*RectItem->x)*xmax;
            y = (*RectItem->y)*ymax;
            break;
        case 1 :    // Upper-Right corner
            x = (*RectItem->x)*xmax+xmax*(*((RectItem->zoom!=NULL)?RectItem->zoom:RectItem->w));
            y = (*RectItem->y)*ymax;
            break;
        case 2 :    // Bottom-Left corner
            x = (*RectItem->x)*xmax;
            if (RectItem->zoom!=NULL)   y=(*RectItem->y)*ymax+xmax*(*RectItem->zoom)*RectItem->AspectRatio;
                else                    y=(*RectItem->y)*ymax+ymax*(*RectItem->h);
            break;
        case 3 :    // Bottom-Right corner
            x = (*RectItem->x)*xmax+xmax*(*((RectItem->zoom!=NULL)?RectItem->zoom:RectItem->w));
            if (RectItem->zoom!=NULL)   y=(*RectItem->y)*ymax+xmax*(*RectItem->zoom)*RectItem->AspectRatio;
                else                    y=(*RectItem->y)*ymax+ymax*(*RectItem->h);
            break;
        case 4 :  // Upper
            x = (*RectItem->x)*xmax+(xmax*(*((RectItem->zoom!=NULL)?RectItem->zoom:RectItem->w)))/2;
            y = (*RectItem->y)*ymax;
            break;
        case 5 :  // Bottom
            x = (*RectItem->x)*xmax+(xmax*(*((RectItem->zoom!=NULL)?RectItem->zoom:RectItem->w)))/2;
            if (RectItem->zoom!=NULL)   y=(*RectItem->y)*ymax+xmax*(*RectItem->zoom)*RectItem->AspectRatio;
                else                    y=(*RectItem->y)*ymax+ymax*(*RectItem->h);
            break;
        case 6 :  // Left
            x = (*RectItem->x)*xmax;
            if (RectItem->zoom!=NULL)   y=(*RectItem->y)*ymax+(xmax*(*RectItem->zoom)*RectItem->AspectRatio)/2;
                else                    y=(*RectItem->y)*ymax+(ymax*(*RectItem->h))/2;
            break;
        case 7 :  // Right
            x = (*RectItem->x)*xmax+xmax*(*((RectItem->zoom!=NULL)?RectItem->zoom:RectItem->w));
            if (RectItem->zoom!=NULL)   y=(*RectItem->y)*ymax+(xmax*(*RectItem->zoom)*RectItem->AspectRatio)/2;
                else                    y=(*RectItem->y)*ymax+(ymax*(*RectItem->h))/2;
            break;
    }
    x=x-w/2;
    y=y-h/2;
    setPos(x,y);
    QRectF Rect=mapRectFromScene(QRectF(x,y,w,h));
    setRect(Rect);
}

//====================================================================================================================
// function call each time a rectangle is move
//====================================================================================================================

QVariant cResizeGraphicsRectItem::itemChange(GraphicsItemChange change,const QVariant &value) {
    if ((change == ItemPositionChange)&&(scene()!=NULL)) {
        QVariant newpos = value;
        if ((IsCapture==true)&&(BlockRecursion==false)) {
            BlockRecursion = true;
            QPointF   Size=newpos.toPoint();
            switch (TypeItem) {
                case 0 : ResizeUpperLeft(Size);     break;  // Upper-Left corner
                case 1 : ResizeUpperRight(Size);    break;  // Upper-Right corner
                case 2 : ResizeBottomLeft(Size);    break;  // Bottom-Left corner
                case 3 : ResizeBottomRight(Size);   break;  // Bottom-Right corner
                case 4 : ResizeUpper(Size);         break;  // Upper
                case 5 : ResizeBottom(Size);        break;  // Bottom
                case 6 : ResizeLeft(Size);          break;  // Left
                case 7 : ResizeRight(Size);         break;  // Right
            }
            newpos=QVariant(Size);
            RectItem->SendRefreshBackgroundImage();
            BlockRecursion = false;
        }
        return newpos;
    } else return QGraphicsRectItem::itemChange(change,value);
}

//====================================================================================================================
// Redefine paint to don't allow selection rectangle
//====================================================================================================================

void cResizeGraphicsRectItem::paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget) {
    if (RectItem->zoom==NULL) {
        cCustomGraphicsRectItem *CurrentTextItem=NULL;
        for (int i=0;i<scene()->selectedItems().count();i++) {
            QGraphicsItem   *Item=scene()->selectedItems()[i];
            QString         data =Item->data(0).toString();
            if (data=="CustomGraphicsRectItem")      CurrentTextItem=(cCustomGraphicsRectItem *)Item;
            else if (data=="ResizeGraphicsRectItem") CurrentTextItem=((cResizeGraphicsRectItem *)Item)->RectItem;
        }
        if (CurrentTextItem==RectItem) QGraphicsRectItem::paint(painter,option,widget);
    } else QGraphicsRectItem::paint(painter,option,widget);
}

//====================================================================================================================
// Resize from Upper-Left corner
//====================================================================================================================

void cResizeGraphicsRectItem::ResizeUpperLeft(QPointF &newpos) {
    if (scene()==NULL) return;
    double x = double(newpos.x());
    double y = double(newpos.y());
    // get max width and max height
    double xmax = double(scene()->sceneRect().width());
    double ymax = double(scene()->sceneRect().height());
    // calcul width and height
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;

    // Magnetic guide
    if ((RectItem->MagneticRuler!=NULL)&&(RectItem->MagneticRuler->MagneticRuler==true)) {
        double mw = HANDLEMAGNETX;
        double mh = HANDLEMAGNETY;
        double *MagnetRuller=NULL;

        if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX1-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX1+mw))) {
            x=RectItem->MagneticRuler->MagnetX1-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX2-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX2+mw))) {
            x=RectItem->MagneticRuler->MagnetX2-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX3-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX3+mw))) {
            x=RectItem->MagneticRuler->MagnetX3-(w/2);
        }

        if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY1-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY1+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY1;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY2-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY2+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY2;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY3-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY3+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY3;
        if (MagnetRuller) {
            y=*(MagnetRuller)-(h/2);
            double imgh=(RectItem->BottomRight->pos().y()+h/2)-(y+h/2);
            if (RectItem->KeepAspectRatio) {
                double imgw=imgh/RectItem->AspectRatio;
                x=RectItem->BottomRight->pos().x()-imgw;
            }
        }
    }
    // get value of opposite resize corner
    double blockx = RectItem->BottomRight->pos().x();//-w;
    double blocky = RectItem->BottomRight->pos().y();//-h;

    // crop rectangle in the image
    if (x<-(w/2))   x=-w/2;
    if (x>blockx)   x=blockx;
    if (y<-(h/2))   y=-(h/2);
    if (y>blocky)   y=blocky;

    // Respect image geometry (if needed)
    double imgw;
    double imgh;
    if (RectItem->KeepAspectRatio) {
        imgw = blockx-x;
        if (imgw<w) {
            imgw=w;
            x=blockx-imgw;
        }
        imgh = imgw*RectItem->AspectRatio;
        if (blocky-imgh<-(h/2)) {
            imgh = blocky-y;
            if (imgh<h) imgh=h;
            imgw = imgh/RectItem->AspectRatio;
            x    = blockx-imgw;
        } else  y = blocky-imgh;
    } else {
        imgw=(RectItem->BottomRight->pos().x()-x);
        imgh=(RectItem->BottomRight->pos().y()-y);
        if (imgw<w) {
            imgw=w;
            x=blockx-imgw;
        }
        if (imgh<h) {
            imgh=h;
            y=blocky-imgh;
        }
    }

    // Now, update RectItem
    if (RectItem->zoom!=NULL) {
        *(RectItem->zoom)=(imgw/xmax);
        RectItem->AspectRatio=imgh/(ymax*(*(RectItem->zoom)));
    } else {
        (*RectItem->w)=(imgw/xmax);
        (*RectItem->h)=(imgh/ymax);
        RectItem->AspectRatio=imgh/imgw;
    }
    (*RectItem->x)=(x+(w/2))/xmax;
    (*RectItem->y)=(y+(h/2))/ymax;
    RectItem->setPos((*RectItem->x)*xmax,(*RectItem->y)*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF((*RectItem->x)*xmax,(*RectItem->y)*ymax,imgw,imgh));
    RectItem->setRect(Rect);

    // return new value
    newpos.setX(x);
    newpos.setY(y);
}

//====================================================================================================================
// Resize from Upper-Right corner
//====================================================================================================================

void cResizeGraphicsRectItem::ResizeUpperRight(QPointF &newpos) {
    if (scene()==NULL) return;
    double x = newpos.x();
    double y = newpos.y();
    // get max width and max height
    double xmax = scene()->sceneRect().width();
    double ymax = scene()->sceneRect().height();
    //calcul width and height
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;
    //Magnetic guide
    if ((RectItem->MagneticRuler!=NULL)&&(RectItem->MagneticRuler->MagneticRuler==true)) {
        double mw = HANDLEMAGNETX;
        double mh = HANDLEMAGNETY;
        double *MagnetRuller=NULL;

        if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX1-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX1+mw))) {
            x=RectItem->MagneticRuler->MagnetX1-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX2-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX2+mw))) {
            x=RectItem->MagneticRuler->MagnetX2-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX3-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX3+mw))) {
            x=RectItem->MagneticRuler->MagnetX3-(w/2);
        }

        if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY1-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY1+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY1;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY2-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY2+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY2;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY3-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY3+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY3;
        if (MagnetRuller) {
            y=*(MagnetRuller)-(h/2);
            double imgh=(RectItem->BottomLeft->pos().y()+h/2)-(y+h/2);
            if (RectItem->KeepAspectRatio) {
                double imgw=imgh/RectItem->AspectRatio;
                x=RectItem->BottomLeft->pos().x()+imgw;
            }
        }
    }
    // get value of opposite resize corner
    double blockx = RectItem->BottomLeft->pos().x();
    double blocky = RectItem->BottomLeft->pos().y();

    // crop rectangle in the image
    if (x<blockx)       x=blockx;
    if (x>xmax-(w/2))   x=xmax-(w/2);
    if (y<-(h/2))       y=-(h/2);
    if (y>blocky)       y=blocky;

    // Respect image geometry (if needed)
    double imgw;
    double imgh;
    if (RectItem->KeepAspectRatio) {
        imgw = x-blockx;
        if (imgw<w) {
            imgw=w;
            x=blockx+imgw;
        }
        imgh = imgw*RectItem->AspectRatio;
        if (blocky-imgh<-(h/2)) {
            imgh = blocky-y;
            if (imgh<h) imgh=h;
            imgw = imgh/RectItem->AspectRatio;
            x    = blockx+imgw;
        } else y = blocky-imgh;
    } else {
        imgw=(x-RectItem->BottomLeft->pos().x());
        imgh=(RectItem->BottomLeft->pos().y()-y);
        if (imgw<w) {
            imgw=w;
            x=blockx+imgw;
        }
        if (imgh<h) {
            imgh=h;
            y=blocky-imgh;
        }
    }

    // Now, update RectItem
    if (RectItem->zoom!=NULL) {
        *(RectItem->zoom)=(imgw/xmax);
        RectItem->AspectRatio=imgh/(ymax*(*(RectItem->zoom)));
    } else {
        (*RectItem->w)=(imgw/xmax);
        (*RectItem->h)=(imgh/ymax);
        RectItem->AspectRatio=imgh/imgw;
    }
    (*RectItem->y)=(y+(h/2))/ymax;
    RectItem->setPos((*RectItem->x)*xmax,(*RectItem->y)*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF((*RectItem->x)*xmax,(*RectItem->y)*ymax,imgw,imgh));
    RectItem->setRect(Rect);

    // return new value
    newpos.setX(x);
    newpos.setY(y);
}

//====================================================================================================================
// Resize from Bottom-Left corner
//====================================================================================================================

void cResizeGraphicsRectItem::ResizeBottomLeft(QPointF &newpos) {
    if (scene()==NULL) return;
    double x = newpos.x();
    double y = newpos.y();
    // get max width and max height
    double xmax = scene()->sceneRect().width();
    double ymax = scene()->sceneRect().height();
    // calcul width and height
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;

    // Magnetic guide
    if ((RectItem->MagneticRuler!=NULL)&&(RectItem->MagneticRuler->MagneticRuler==true)) {
        double mw = HANDLEMAGNETX;
        double mh = HANDLEMAGNETY;
        double *MagnetRuller=NULL;

        if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX1-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX1+mw))) {
            x=RectItem->MagneticRuler->MagnetX1-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX2-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX2+mw))) {
            x=RectItem->MagneticRuler->MagnetX2-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX3-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX3+mw))) {
            x=RectItem->MagneticRuler->MagnetX3-(w/2);
        }

        if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY1-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY1+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY1;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY2-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY2+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY2;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY3-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY3+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY3;
        if (MagnetRuller) {
            y=*(MagnetRuller)-(h/2);
            double imgh=(y+h/2)-(RectItem->UpperRight->pos().y()+h/2);
            if (RectItem->KeepAspectRatio) {
                double imgw=imgh/RectItem->AspectRatio;
                x=RectItem->UpperRight->pos().x()-imgw;
            }
          }
    }
    // get value of opposite resize corner
    double blockx = RectItem->UpperRight->pos().x();//-w;
    double blocky = RectItem->UpperRight->pos().y();//+h;

    // crop rectangle in the image
    if (x<-(w/2)) x=-w/2;
    if (x>blockx) x=blockx;
    if (y<blocky) y=blocky;
    if (y>ymax-(h/2)) y=ymax-(h/2);

    // Respect image geometry (if needed)
    double imgw;
    double imgh;
    if (RectItem->KeepAspectRatio) {
        imgw = blockx-x;
        if (imgw<w) {
            imgw=w;
            x=blockx-imgw;
        }
        imgh = imgw*RectItem->AspectRatio;
        if (imgh+blocky>ymax-(h/2)) {
            imgh = y-blocky;
            if (imgh<h) imgh=h;
            imgw = imgh/RectItem->AspectRatio;
            x    = blockx-imgw;
        } else y = blocky+imgh;
    } else {
        imgw=(RectItem->UpperRight->pos().x()-x);
        imgh=(y-RectItem->UpperRight->pos().y());
        if (imgw<w) {
            imgw=w;
            x=blockx-imgw;
        }
        if (imgh<h) {
            imgh=h;
            y=blocky+imgh;
        }
    }

    // Now, update RectItem
    if (RectItem->zoom!=NULL) {
        *(RectItem->zoom)=(imgw/xmax);
        RectItem->AspectRatio=imgh/(ymax*(*(RectItem->zoom)));
    } else {
        (*RectItem->w)=(imgw/xmax);
        (*RectItem->h)=(imgh/ymax);
        RectItem->AspectRatio=imgh/imgw;
    }
    (*RectItem->x)=(x+(w/2))/xmax;
    RectItem->setPos((*RectItem->x)*xmax,(*RectItem->y)*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF((*RectItem->x)*xmax,(*RectItem->y)*ymax,imgw,imgh));
    RectItem->setRect(Rect);

    // return new value
    newpos.setX(x);
    newpos.setY(y);
}

//====================================================================================================================
// Resize from Bottom-Right corner
//====================================================================================================================

void cResizeGraphicsRectItem::ResizeBottomRight(QPointF &newpos) {
    if (scene()==NULL) return;
    double x = newpos.x();
    double y = newpos.y();
    // get max width and max height
    double xmax = scene()->sceneRect().width();
    double ymax = scene()->sceneRect().height();
    // calcul width and height
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;
    // Magnetic guide
    if ((RectItem->MagneticRuler!=NULL)&&(RectItem->MagneticRuler->MagneticRuler==true)) {
        double mw = HANDLEMAGNETX;
        double mh = HANDLEMAGNETY;
        double *MagnetRuller=NULL;

        if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX1-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX1+mw))) {
            x=RectItem->MagneticRuler->MagnetX1-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX2-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX2+mw))) {
            x=RectItem->MagneticRuler->MagnetX2-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX3-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX3+mw))) {
            x=RectItem->MagneticRuler->MagnetX3-(w/2);
        }

        if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY1-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY1+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY1;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY2-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY2+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY2;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY3-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY3+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY3;
        if (MagnetRuller) {
            y=*(MagnetRuller)-(h/2);
            double imgh=(y+h/2)-(RectItem->UpperLeft->pos().y()+h/2);
            if (RectItem->KeepAspectRatio) {
                double imgw=imgh/RectItem->AspectRatio;
                x=RectItem->UpperLeft->pos().x()+imgw;
            }
        }
    }
    // get value of opposite resize corner
    double blockx = RectItem->UpperLeft->pos().x();
    double blocky = RectItem->UpperLeft->pos().y();

    // crop rectangle in the image
    if (x<blockx)      x=blockx;
    if (x>xmax-(w/2))  x=xmax-(w/2);
    if (y<blocky)      y=blocky;
    if (y>ymax-(h/2))  y=ymax-(h/2);

    // Respect image geometry (if needed)
    double imgw;
    double imgh;
    if (RectItem->KeepAspectRatio) {
        imgw = x-blockx;
        if (imgw<w) imgw=w;
        imgh = imgw*RectItem->AspectRatio;
        if (imgh+blocky>ymax-(h/2)) {
            imgh = y-blocky;
            if (imgh<h) imgh=h;
            imgw = imgh/RectItem->AspectRatio;
        }
        x = blockx+imgw;
        y = blocky+imgh;
    } else {
        imgw=(x-RectItem->UpperLeft->pos().x());
        imgh=(y-RectItem->UpperLeft->pos().y());
        if (imgw<w) {
            imgw=w;
            x=blockx+imgw;
        }
        if (imgh<h) {
            imgh=h;
            y=blocky+imgh;
        }
    }

    // Now, update RectItem
    if (RectItem->zoom!=NULL) {
        *(RectItem->zoom)=(imgw/xmax);
        RectItem->AspectRatio=imgh/(ymax*(*(RectItem->zoom)));
    } else {
        (*RectItem->w)=(imgw/xmax);
        (*RectItem->h)=(imgh/ymax);
        RectItem->AspectRatio=imgh/imgw;
    }
    RectItem->setPos((*RectItem->x)*xmax,(*RectItem->y)*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF((*RectItem->x)*xmax,(*RectItem->y)*ymax,imgw,imgh));
    RectItem->setRect(Rect);

    // return new value
    newpos.setX(x);
    newpos.setY(y);
}

//====================================================================================================================
// Resize from Upper border
//====================================================================================================================
void cResizeGraphicsRectItem::ResizeUpper(QPointF &newpos) {
    if (scene()==NULL) return;
    double y = double(newpos.y());

    // get max width and max height
    double xmax = double(scene()->sceneRect().width());
    double ymax = double(scene()->sceneRect().height());

    // calcul width and height
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;

    // get value of opposite resize corner
    double blocky = RectItem->Bottom->pos().y();//-h;

    // crop rectangle in the image
    if (y<-(h/2))       y=-(h/2);
    if (y>blocky-(h/2)) y=blocky-(h/2);

    double imgh=blocky-y;
    double imgw=(RectItem->zoom?(*RectItem->zoom):(*RectItem->w))*xmax;

    // Magnetic guide
    if ((RectItem->MagneticRuler!=NULL)&&(RectItem->MagneticRuler->MagneticRuler==true)) {
        double mh = HANDLEMAGNETY;
        double *MagnetRuller=NULL;

        if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY1-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY1+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY1;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY2-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY2+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY2;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY3-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY3+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY3;
        if (MagnetRuller) {
            y=*(MagnetRuller)-(h/2);
            imgh=blocky-y;
            if (RectItem->KeepAspectRatio) imgw=imgh/RectItem->AspectRatio;
        }
    }

    if (imgh<(h/2)) {
        y    = blocky-(h/2);
        imgh = blocky-y;
    }

    // Respect image geometry (if needed)
    if (RectItem->KeepAspectRatio) imgw = imgh/RectItem->AspectRatio;

    // Now, update RectItem
    if (RectItem->zoom!=NULL) {
        *(RectItem->zoom)=(imgw/xmax);
        RectItem->AspectRatio=imgh/(ymax*(*(RectItem->zoom)));
    } else {
        (*RectItem->w)=(imgw/xmax);
        (*RectItem->h)=(imgh/ymax);
        RectItem->AspectRatio=imgh/imgw;
    }
    (*RectItem->y)=(y+h/2)/ymax;
    (*RectItem->x)=(pos().x()+(w/2)-(imgw/2))/xmax;
    RectItem->setPos((*RectItem->x)*xmax,(*RectItem->y)*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF((*RectItem->x)*xmax,(*RectItem->y)*ymax,imgw,imgh));
    RectItem->setRect(Rect);

    // return new value
    newpos.setX(pos().x());
    newpos.setY(y);
}

//====================================================================================================================
// Resize from Bottom border
//====================================================================================================================
void cResizeGraphicsRectItem::ResizeBottom(QPointF &newpos) {
    if (scene()==NULL) return;
    double y = double(newpos.y());

    // get max width and max height
    double xmax = double(scene()->sceneRect().width());
    double ymax = double(scene()->sceneRect().height());

    // calcul width and height
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;

    // get value of opposite resize corner
    double blocky = RectItem->Upper->pos().y();//-h;

    // crop rectangle in the image
    if (y>ymax-(h/2))   y=ymax-(h/2);
    if (y<blocky+(h/2)) y=blocky+(h/2);

    double imgh=y-blocky;
    double imgw=(RectItem->zoom?(*RectItem->zoom):(*RectItem->w))*xmax;

    // Magnetic guide
    if ((RectItem->MagneticRuler!=NULL)&&(RectItem->MagneticRuler->MagneticRuler==true)) {
        double mh = HANDLEMAGNETY;
        double *MagnetRuller=NULL;

        if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY1-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY1+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY1;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY2-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY2+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY2;
        else if ((y+(h/2)>(RectItem->MagneticRuler->MagnetY3-mh))&&(y+(h/2)<(RectItem->MagneticRuler->MagnetY3+mh))) MagnetRuller=&RectItem->MagneticRuler->MagnetY3;
        if (MagnetRuller) {
            y=*(MagnetRuller)-(h/2);
            imgh=y-blocky;
            if (RectItem->KeepAspectRatio) imgw=imgh/RectItem->AspectRatio;
        }
    }

    if (imgh<(h/2)) {
        y    = blocky+(h/2);
        imgh = y-blocky;
    }

    // Respect image geometry (if needed)
    if (RectItem->KeepAspectRatio) imgw = imgh/RectItem->AspectRatio;

    // Now, update RectItem
    if (RectItem->zoom!=NULL) {
        *(RectItem->zoom)=(imgw/xmax);
        RectItem->AspectRatio=imgh/(ymax*(*(RectItem->zoom)));
    } else {
        (*RectItem->w)=(imgw/xmax);
        (*RectItem->h)=(imgh/ymax);
        RectItem->AspectRatio=imgh/imgw;
    }
    (*RectItem->y)=(y+h/2-imgh)/ymax;
    (*RectItem->x)=(pos().x()+(w/2)-(imgw/2))/xmax;
    RectItem->setPos((*RectItem->x)*xmax,(*RectItem->y)*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF((*RectItem->x)*xmax,(*RectItem->y)*ymax,imgw,imgh));
    RectItem->setRect(Rect);

    // return new value
    newpos.setX(pos().x());
    newpos.setY(y);
}

//====================================================================================================================
// Resize from Left border
//====================================================================================================================
void cResizeGraphicsRectItem::ResizeLeft(QPointF &newpos) {
    if (scene()==NULL) return;
    double x = double(newpos.x());

    // get max width and max height
    double xmax = double(scene()->sceneRect().width());
    double ymax = double(scene()->sceneRect().height());

    // calcul width and height
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;

    // get value of opposite resize corner
    double blockx = RectItem->Right->pos().x();//-h;

    // crop rectangle in the image
    if (x<-(w/2))       x=-(w/2);
    if (x>blockx-(w/2)) x=blockx-(w/2);

    double imgw=blockx-x;
    double imgh=(RectItem->zoom?(*RectItem->zoom*RectItem->AspectRatio):(*RectItem->h))*ymax;

    // Magnetic guide
    if ((RectItem->MagneticRuler!=NULL)&&(RectItem->MagneticRuler->MagneticRuler==true)) {
        double mw = HANDLEMAGNETX;
        if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX1-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX1+mw))) {
            x=RectItem->MagneticRuler->MagnetX1-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX2-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX2+mw))) {
            x=RectItem->MagneticRuler->MagnetX2-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX3-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX3+mw))) {
            x=RectItem->MagneticRuler->MagnetX3-(w/2);
        }
        imgw=blockx-x;
    }

    if (imgw<(w/2)) {
        x    = blockx-(w/2);
        imgw = blockx-x;
    }

    // Respect image geometry (if needed)
    if (RectItem->KeepAspectRatio)
        imgh = imgw*RectItem->AspectRatio;

    // Now, update RectItem
    if (RectItem->zoom!=NULL) {
        *(RectItem->zoom)=(imgw/xmax);
        RectItem->AspectRatio=imgh/(ymax*(*(RectItem->zoom)));
    } else {
        (*RectItem->w)=(imgw/xmax);
        (*RectItem->h)=(imgh/ymax);
        RectItem->AspectRatio=imgh/imgw;
    }
    *(RectItem->y)=(pos().y()+(h/2)-(imgh/2))/ymax;
    *(RectItem->x)=(x+(w/2))/xmax;
    RectItem->setPos((*RectItem->x)*xmax,(*RectItem->y)*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF((*RectItem->x)*xmax,(*RectItem->y)*ymax,imgw,imgh));
    RectItem->setRect(Rect);

    // return new value
    newpos.setX(x);
    newpos.setY(pos().y());
}

//====================================================================================================================
// Resize from Right border
//====================================================================================================================
void cResizeGraphicsRectItem::ResizeRight(QPointF &newpos) {
    if (scene()==NULL) return;
    double x = double(newpos.x());

    // get max width and max height
    double xmax = double(scene()->sceneRect().width());
    double ymax = double(scene()->sceneRect().height());

    // calcul width and height
    double w = HANDLESIZEX;
    double h = HANDLESIZEY;

    // get value of opposite resize corner
    double blockx = RectItem->Left->pos().x();//-h;

    // crop rectangle in the image
    if (x>xmax-(w/2))   x=xmax-(w/2);
    if (x<blockx+(w/2)) x=blockx+(w/2);

    double imgw=x-blockx;
    double imgh=(RectItem->zoom?(*RectItem->zoom*RectItem->AspectRatio):(*RectItem->h))*ymax;

    // Magnetic guide
    if ((RectItem->MagneticRuler!=NULL)&&(RectItem->MagneticRuler->MagneticRuler==true)) {
        double mw = HANDLEMAGNETX;
        if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX1-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX1+mw))) {
            x=RectItem->MagneticRuler->MagnetX1-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX2-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX2+mw))) {
            x=RectItem->MagneticRuler->MagnetX2-(w/2);
        } else if ((x+(w/2)>(RectItem->MagneticRuler->MagnetX3-mw))&&(x+(w/2)<(RectItem->MagneticRuler->MagnetX3+mw))) {
            x=RectItem->MagneticRuler->MagnetX3-(w/2);
        }
        imgw=x-blockx;
    }

    if (imgw<(w/2)) {
        x    = blockx+(w/2);
        imgw = x-blockx;
    }

    // Respect image geometry (if needed)
    if (RectItem->KeepAspectRatio)
        imgh = imgw*RectItem->AspectRatio;

    // Now, update RectItem
    if (RectItem->zoom!=NULL) {
        *(RectItem->zoom)=(imgw/xmax);
        RectItem->AspectRatio=imgh/(ymax*(*(RectItem->zoom)));
    } else {
        (*RectItem->w)=(imgw/xmax);
        (*RectItem->h)=(imgh/ymax);
        RectItem->AspectRatio=imgh/imgw;
    }
    *(RectItem->y)=(pos().y()+(h/2)-(imgh/2))/ymax;
    *(RectItem->x)=(x+(w/2)-imgw)/xmax;
    RectItem->setPos((*RectItem->x)*xmax,(*RectItem->y)*ymax);
    QRectF Rect=RectItem->mapRectFromScene(QRectF((*RectItem->x)*xmax,(*RectItem->y)*ymax,imgw,imgh));
    RectItem->setRect(Rect);

    // return new value
    newpos.setX(x);
    newpos.setY(pos().y());
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
// class use to crop rectangle into the image
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
// x,y and zoom or w/h are always give in %
cCustomGraphicsRectItem::cCustomGraphicsRectItem(QGraphicsScene *TheScene,int ZValue,double *Thex,double *They,double *Thezoom,double *Thew,double *Theh,double xmax,double ymax,
                                                 bool TheKeepAspectRatio,double TheAspectRatio,sMagneticRuler *TheMagneticRuler,QWidget *TheParentWidget,int TheParentWidgetType,int TheIndexKey)
                                                :QGraphicsRectItem((*Thex)*xmax,(*They)*ymax,xmax*(*((Thezoom!=NULL)?Thezoom:Thew)),ymax*(*((Thezoom!=NULL)?Thezoom:Theh)),NULL)

{
    IndexKey        = TheIndexKey;
    ParentWidget    = TheParentWidget;
    ParentWidgetType= TheParentWidgetType;
    MagneticRuler   = TheMagneticRuler;
    x               = Thex;
    y               = They;
    zoom            = Thezoom;
    KeepAspectRatio = TheKeepAspectRatio;
    AspectRatio     = TheAspectRatio;

    if (zoom!=NULL) {
        // If zoom mode is use
        w=&StockW;    *(w) = xmax*(*zoom);
        h=&StockH;    *(h) = ymax*(*zoom);
    } else {
        // If zoom mode is not use
        w=Thew;
        h=Theh;
    }
    BlockZoomChange = false;            // flag to block zoom changing during change % to pixel
    IsCapture       = false;
    UpperLeft       = NULL;
    UpperRight      = NULL;
    BottomLeft      = NULL;
    BottomRight     = NULL;
    Upper           = NULL;
    Bottom          = NULL;
    Left            = NULL;
    Right           = NULL;

    // Init item
    setPos((*x)*xmax,(*y)*ymax);
    QRectF Rect=mapRectFromScene(QRectF((*x)*xmax,(*y)*ymax,xmax*(*((zoom!=NULL)?zoom:w)),ymax*(*((zoom!=NULL)?zoom:h))));
    setRect(Rect);

    // define Mouse cursor
    setCursor(Qt::SizeAllCursor);

    // define a pen for the rectangle
    QColor col=QColor(255,0,0);
    QPen   pen=QPen(col);
    pen.setWidth(1);
    setPen(pen);

    // define Flags
    setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemClipsChildrenToShape|QGraphicsItem::ItemSendsScenePositionChanges|QGraphicsItem::ItemSendsGeometryChanges);
//    setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemSendsScenePositionChanges|QGraphicsItem::ItemSendsGeometryChanges);
    setZValue(ZValue);
    setData(0,QVariant("CustomGraphicsRectItem"));

    // Add this item to the scene and create resize box
    TheScene->addItem(this);
    UpperLeft  =new cResizeGraphicsRectItem(TheScene,this,ZValue+1,0);
    UpperRight =new cResizeGraphicsRectItem(TheScene,this,ZValue+1,1);
    BottomLeft =new cResizeGraphicsRectItem(TheScene,this,ZValue+1,2);
    BottomRight=new cResizeGraphicsRectItem(TheScene,this,ZValue+1,3);
    Upper      =new cResizeGraphicsRectItem(TheScene,this,ZValue+1,4);
    Bottom     =new cResizeGraphicsRectItem(TheScene,this,ZValue+1,5);
    Left       =new cResizeGraphicsRectItem(TheScene,this,ZValue+1,6);
    Right      =new cResizeGraphicsRectItem(TheScene,this,ZValue+1,7);
}

//====================================================================================================================

cCustomGraphicsRectItem::~cCustomGraphicsRectItem() {
    if (UpperLeft) {
        if (UpperLeft->scene()) UpperLeft->scene()->removeItem(UpperLeft);
        delete UpperLeft;
        UpperLeft=NULL;
    }
    if (UpperRight) {
        if (UpperRight->scene()) UpperRight->scene()->removeItem(UpperRight);
        delete UpperRight;
        UpperRight=NULL;
    }
    if (BottomLeft) {
        if (BottomLeft->scene()) BottomLeft->scene()->removeItem(BottomLeft);
        delete BottomLeft;
        BottomLeft=NULL;
    }
    if (BottomRight) {
        if (BottomRight->scene()) BottomRight->scene()->removeItem(BottomRight);
        delete BottomRight;
        BottomRight=NULL;
    }
    if (Upper) {
        if (Upper->scene()) Upper->scene()->removeItem(Upper);
        delete Upper;
        Upper=NULL;
    }
    if (Bottom) {
        if (Bottom->scene()) Bottom->scene()->removeItem(Bottom);
        delete Bottom;
        Bottom=NULL;
    }
    if (Left) {
        if (Left->scene()) Left->scene()->removeItem(Left);
        delete Left;
        Left=NULL;
    }
    if (Right) {
        if (Right->scene()) Right->scene()->removeItem(Right);
        delete Right;
        Right=NULL;
    }
    if (scene()) scene()->removeItem(this);
}

//====================================================================================================================

void cCustomGraphicsRectItem::paint(QPainter */*painter*/,const QStyleOptionGraphicsItem */*option*/,QWidget */*widget*/) {
}

//====================================================================================================================

void cCustomGraphicsRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsRectItem::mousePressEvent(event);
    IsCapture = true;
}

//====================================================================================================================

void cCustomGraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    IsCapture = false;
    QGraphicsRectItem::mouseReleaseEvent(event);
}

//====================================================================================================================
// function call each time a rectangle is move
//====================================================================================================================

QVariant cCustomGraphicsRectItem::itemChange(GraphicsItemChange change,const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionChange) {
        QPointF newpos = value.toPointF();
        if (IsCapture==true) {
            double xmax = double(scene()->sceneRect().width());
            double ymax = double(scene()->sceneRect().height());
            *x = newpos.x()/xmax;
            *y = newpos.y()/ymax;
            // calcul width and height;
            double W = xmax*(*((zoom!=NULL)?zoom:w));
            double H = ymax*((zoom!=NULL)?((*zoom)*AspectRatio):(*h));
            // crop rectangle in the image
            if ((*x)*xmax<0)        *x=0;
            if ((*x)*xmax>xmax-W)   *x=(xmax-W)/xmax;
            if ((*y)*ymax<0)        *y=0;
            if ((*y)*ymax>ymax-H)   *y=(ymax-H)/ymax;

            if ((MagneticRuler!=NULL)&&(MagneticRuler->MagneticRuler==true)) {
                double mw = HANDLEMAGNETX;
                double mh = HANDLEMAGNETY;
                if (((*x)*xmax>(MagneticRuler->MagnetX1-mw))&&((*x)*xmax<(MagneticRuler->MagnetX1+mw)))          *x=MagneticRuler->MagnetX1/xmax;
                if ((((*x)*xmax+W)>(MagneticRuler->MagnetX2-mw))&&(((*x)*xmax+W)<(MagneticRuler->MagnetX2+mw)))  *x=(MagneticRuler->MagnetX2-W)/xmax;
                if (((*y)*ymax>(MagneticRuler->MagnetY1-mh))&&((*y)*ymax<(MagneticRuler->MagnetY1+mh)))          *y=MagneticRuler->MagnetY1/ymax;
                if ((((*y)*ymax+H)>(MagneticRuler->MagnetY2-mh))&&(((*y)*ymax+H)<(MagneticRuler->MagnetY2+mh)))  *y=(MagneticRuler->MagnetY2-H)/ymax;

                if ((MagneticRuler->MagnetX3>0)&&(MagneticRuler->MagnetY3>0)) {
                    if (((*x)*xmax>(MagneticRuler->MagnetX3-mw))&&((*x)*xmax<(MagneticRuler->MagnetX3+mw)))          *x=MagneticRuler->MagnetX3/xmax;
                    if ((((*x)*xmax+W)>(MagneticRuler->MagnetX3-mw))&&(((*x)*xmax+W)<(MagneticRuler->MagnetX3+mw)))  *x=(MagneticRuler->MagnetX3-W)/xmax;
                    if (((*y)*ymax>(MagneticRuler->MagnetY3-mh))&&((*y)*ymax<(MagneticRuler->MagnetY3+mh)))          *y=MagneticRuler->MagnetY3/ymax;
                    if ((((*y)*ymax+H)>(MagneticRuler->MagnetY3-mh))&&(((*y)*ymax+H)<(MagneticRuler->MagnetY3+mh)))  *y=(MagneticRuler->MagnetY3-H)/ymax;
                    // Midle attraction
                    if ((((*x)*xmax+W/2)>(MagneticRuler->MagnetX3-mw))&&(((*x)*xmax+W/2)<(MagneticRuler->MagnetX3+mw)))  *x=(MagneticRuler->MagnetX3-W/2)/xmax;
                    if ((((*y)*ymax+H/2)>(MagneticRuler->MagnetY3-mh))&&(((*y)*ymax+H/2)<(MagneticRuler->MagnetY3+mh)))  *y=(MagneticRuler->MagnetY3-H/2)/ymax;
                }

                newpos.setX((*x)*xmax);
                newpos.setY((*y)*ymax);
            }
            SendRefreshBackgroundImage();
        }
        return newpos;
    } else return QGraphicsRectItem::itemChange(change,value);
}

//====================================================================================================================

void cCustomGraphicsRectItem::SendRefreshBackgroundImage() {
    RecalcEmbededResizeRectItem();
    switch (ParentWidgetType) {
        case TYPE_wgt_QCompositionWidget:   ((wgt_QCompositionWidget *)ParentWidget)->StartRefreshControls();   break;
        case TYPE_DlgSlideProperties:       ((DlgSlideProperties *)ParentWidget)->RefreshSceneImage();          break;
        case TYPE_DlgImageCorrection:       ((DlgImageCorrection *)ParentWidget)->RefreshControls();            break;
    }
}

//====================================================================================================================

void cCustomGraphicsRectItem::RecalcEmbededResizeRectItem() {
    // Refresh Resizebox position
    if (UpperLeft)   UpperLeft->CalcPosition();
    if (UpperRight)  UpperRight->CalcPosition();
    if (BottomLeft)  BottomLeft->CalcPosition();
    if (BottomRight) BottomRight->CalcPosition();
    if (Upper)       Upper->CalcPosition();
    if (Bottom)      Bottom->CalcPosition();
    if (Left)        Left->CalcPosition();
    if (Right)       Right->CalcPosition();
}