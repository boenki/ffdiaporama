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

#include "cImgInteractiveZone.h"

#include <QKeyEvent>
#include <QMouseEvent>

#define HANDLESIZEX     8
#define HANDLESIZEY     8
#define HANDLEMAGNETX   14
#define HANDLEMAGNETY   10

#define MINVALUE        0.002       // Never less than this value for width or height
#define PI              3.14159265

//====================================================================================================================

cImgInteractiveZone::cImgInteractiveZone(QWidget *parent):QWidget(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::cImgInteractiveZone");
    ForegroundImage =NULL;
    CachedImage     =NULL;
    IsCapture       =false;
    TransfoType     =NOTYETDEFINED;

    CompoObject     =NULL;
    BackgroundForm  =0;
    CurrentBrush    =NULL;
    VideoPosition   =0;

    Move_X          =0;
    Scale_X         =0;
    Move_Y          =0;
    Scale_Y         =0;
    SceneRect       =QRectF(0,0,0,0);
    setMouseTracking(true);
}

//====================================================================================================================

cImgInteractiveZone::~cImgInteractiveZone() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::~cImgInteractiveZone");
    if (ForegroundImage!=NULL) {
        delete ForegroundImage;
        ForegroundImage=NULL;
    }
    if (CachedImage!=NULL) {
        delete CachedImage;
        CachedImage=NULL;
    }
}

//====================================================================================================================

void cImgInteractiveZone::InitCachedImage(cCompositionObject *TheCompoObject,int TheBackgroundForm,cBrushDefinition *TheCurrentBrush,int TheVideoPosition) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::InitCachedImage");

    CompoObject     =TheCompoObject;
    BackgroundForm  =TheBackgroundForm;
    CurrentBrush    =TheCurrentBrush;
    VideoPosition   =TheVideoPosition;

    if (CachedImage!=NULL) {
        delete CachedImage;
        CachedImage=NULL;
    }

    if (CurrentBrush->Image) {
        CachedImage=CurrentBrush->Image->ImageAt(true,NULL);
     } else if (CurrentBrush->Video) {
        CachedImage=CurrentBrush->Video->ImageAt(true,VideoPosition,QTime(0,0,0,0).msecsTo(CurrentBrush->Video->StartPos),NULL,CurrentBrush->Deinterlace,1,false,NULL,false);
        if (CachedImage->format()!=QImage::Format_ARGB32_Premultiplied) {
            QImage *NewCachedImage=new QImage(CachedImage->convertToFormat(QImage::Format_ARGB32_Premultiplied));
            delete CachedImage;
            CachedImage=NewCachedImage;
        }
    }

    RefreshDisplay();
}

//====================================================================================================================

void cImgInteractiveZone::paintEvent(QPaintEvent *) {
    if ((!ForegroundImage)||(SceneRect.width()==0)||(SceneRect.height()==0)) return;

    QPainter Painter(this);
    Painter.save();
    Painter.translate(SceneRect.left(),SceneRect.top());
    Painter.drawImage(0,0,*ForegroundImage);

    CurSelRect=QRectF((CurrentBrush->X+Move_X)*dmax,
                      (CurrentBrush->Y+Move_Y)*dmax,
                      (CurrentBrush->ZoomFactor+Scale_X)*dmax,
                      (CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio+Scale_Y)*dmax);

    QImage Image(ForegroundImage->width(),ForegroundImage->height(),QImage::Format_ARGB32_Premultiplied);

    QPainter PainterImg;
    PainterImg.begin(&Image);
    PainterImg.setPen(Qt::NoPen);
    PainterImg.fillRect(QRect(0,0,Image.width(),Image.height()),QBrush(0x555555));
    PainterImg.setBrush(Qt::transparent);
    PainterImg.setCompositionMode(QPainter::CompositionMode_Source);
    DrawShape(PainterImg,BackgroundForm,CurSelRect.left(),CurSelRect.top(),CurSelRect.width(),CurSelRect.height(),CurSelRect.width()/2+CurSelRect.left(),CurSelRect.height()/2+CurSelRect.top());
    PainterImg.setCompositionMode(QPainter::CompositionMode_SourceOver);
    PainterImg.end();

    Painter.setOpacity(0.75);
    Painter.drawImage(0,0,Image);
    Painter.setOpacity(1);

    Painter.setBrush(Qt::NoBrush);
    Painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);

    // Draw rulers if they was enabled
    if (MagneticRuler) {
        QPen pen=QPen(QColor(0,255,0));
        pen.setWidth(1);
        pen.setStyle(Qt::DotLine);
        Painter.setPen(pen);
        for (int i=0;i<MagnetVert.count();i++)  Painter.drawLine(MagnetVert[i],0,MagnetVert[i],SceneRect.height());
        for (int i=0;i<MagnetHoriz.count();i++) Painter.drawLine(0,MagnetHoriz[i],SceneRect.width(),MagnetHoriz[i]);
    }

    QPen pen(Qt::red);
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    Painter.setPen(pen);
    DrawSelect(Painter,QRectF(CurSelRect.left()-1,CurSelRect.top()-1,CurSelRect.width(),CurSelRect.height()),true);

    Painter.restore();
}

void cImgInteractiveZone::DrawSelect(QPainter &Painter,QRectF Rect,bool WithHandles) {
    Painter.drawRect(Rect);
    if (WithHandles) {
        QPen OldPen=Painter.pen();
        QPen pen=OldPen;
        pen.setStyle(Qt::SolidLine);
        Painter.setPen(pen);
        Painter.drawRect(QRectF(Rect.left()-HANDLESIZEX/2, Rect.top()-HANDLESIZEY/2,                 HANDLESIZEX,HANDLESIZEY));    // Top left
        Painter.drawRect(QRectF(Rect.left()-HANDLESIZEX/2, Rect.top()+Rect.height()/2-HANDLESIZEY/2, HANDLESIZEX,HANDLESIZEY));    // Left
        Painter.drawRect(QRectF(Rect.left()-HANDLESIZEX/2, Rect.bottom()-HANDLESIZEY/2,              HANDLESIZEX,HANDLESIZEY));    // Bottom left
        Painter.drawRect(QRectF(Rect.right()-HANDLESIZEX/2,Rect.top()-HANDLESIZEY/2,                 HANDLESIZEX,HANDLESIZEY));    // Top right
        Painter.drawRect(QRectF(Rect.right()-HANDLESIZEX/2,Rect.top()+Rect.height()/2-HANDLESIZEY/2, HANDLESIZEX,HANDLESIZEY));    // right
        Painter.drawRect(QRectF(Rect.right()-HANDLESIZEX/2,Rect.bottom()-HANDLESIZEY/2,              HANDLESIZEX,HANDLESIZEY));    // Bottom right
        Painter.drawRect(QRectF(Rect.left()+Rect.width()/2-HANDLESIZEX/2,Rect.top()-HANDLESIZEY/2,   HANDLESIZEX,HANDLESIZEY));    // Top
        Painter.drawRect(QRectF(Rect.left()+Rect.width()/2-HANDLESIZEX/2,Rect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY));    // Bottom
        Painter.setPen(OldPen);
    }
}

//====================================================================================================================

void cImgInteractiveZone::RefreshDisplay() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::RefreshDisplay");
    if (!CurrentBrush) return;

    if (ForegroundImage!=NULL) {
        delete ForegroundImage;
        ForegroundImage=NULL;
    }

    if (width()<height()) dmax=width(); else dmax=height();

    QImage  *SourceImage=NULL;
    double  Hyp         =sqrt(double(CachedImage->width())*double(CachedImage->width())+double(CachedImage->height())*double(CachedImage->height()));   // Calc hypothenuse of the image to define full canvas

    // calc rectangle before rotation
    double  rx=double(CachedImage->width())*(dmax/Hyp)/2;
    double  ry=double(CachedImage->height())*(dmax/Hyp)/2;

    //RotatePoint.X = ((Pt.X - Centre.X) * Cos(AngCrad) - (Pt.Y - Centre.Y) * Sin(AngCrad) + Centre.X)
    //RotatePoint.Y = ((Pt.X - Centre.X) * Sin(AngCrad) + (Pt.Y - Centre.Y) * Cos(AngCrad) + Centre.Y)

    double  x1=-rx*cos((CurrentBrush->ImageRotation)*PI/180)+ry*sin(CurrentBrush->ImageRotation*PI/180)+dmax/2;
    double  x2=+rx*cos((CurrentBrush->ImageRotation)*PI/180)+ry*sin(CurrentBrush->ImageRotation*PI/180)+dmax/2;
    double  x3=-rx*cos((CurrentBrush->ImageRotation)*PI/180)-ry*sin(CurrentBrush->ImageRotation*PI/180)+dmax/2;
    double  x4=+rx*cos((CurrentBrush->ImageRotation)*PI/180)-ry*sin(CurrentBrush->ImageRotation*PI/180)+dmax/2;
    double  y1=-rx*sin((CurrentBrush->ImageRotation)*PI/180)+ry*cos(CurrentBrush->ImageRotation*PI/180)+dmax/2;
    double  y2=+rx*sin((CurrentBrush->ImageRotation)*PI/180)+ry*cos(CurrentBrush->ImageRotation*PI/180)+dmax/2;
    double  y3=-rx*sin((CurrentBrush->ImageRotation)*PI/180)-ry*cos(CurrentBrush->ImageRotation*PI/180)+dmax/2;
    double  y4=+rx*sin((CurrentBrush->ImageRotation)*PI/180)-ry*cos(CurrentBrush->ImageRotation*PI/180)+dmax/2;

    double  minx=x1;    if (minx>x2) minx=x2;   if (minx>x3) minx=x3;   if (minx>x4) minx=x4;
    double  maxx=x1;    if (maxx<x2) maxx=x2;   if (maxx<x3) maxx=x3;   if (maxx<x4) maxx=x4;
    double  miny=y1;    if (miny>y2) miny=y2;   if (miny>y3) miny=y3;   if (miny>y4) miny=y4;
    double  maxy=y1;    if (maxy<y2) maxy=y2;   if (maxy<y3) maxy=y3;   if (maxy<y4) maxy=y4;

    maxw=maxx-minx;
    maxh=maxy-miny;

    // Rotate image if needed and create a SourceImage
    if (CurrentBrush->ImageRotation!=0) {
        QTransform matrix;
        matrix.rotate(CurrentBrush->ImageRotation,Qt::ZAxis);
        SourceImage=new QImage(CachedImage->transformed(matrix,Qt::SmoothTransformation));

    // If no rotation then SourceImage=CachedImage
    } else SourceImage=CachedImage;

    // Calc coordinates of the part in the source image
    double  RealImageW=double(SourceImage->width());                  // Get real image widht
    double  RealImageH=double(SourceImage->height());                 // Get real image height

    DstX=((Hyp-RealImageW)/2)*(dmax/Hyp);
    DstY=((Hyp-RealImageH)/2)*(dmax/Hyp);
    DstW=RealImageW*(dmax/Hyp);
    DstH=RealImageH*(dmax/Hyp);

    QImage  ToUseImage=SourceImage->scaled(DstW,DstH);
    if (SourceImage!=CachedImage) delete SourceImage;

    if (ToUseImage.format()!=QImage::Format_ARGB32_Premultiplied) ToUseImage=ToUseImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    // On/Off filters and blur/sharpen
    if (CurrentBrush->Image) CurrentBrush->Image->BrushFileTransform.ApplyFilter(&ToUseImage);
        else if (CurrentBrush->Video) CurrentBrush->Video->BrushFileTransform.ApplyFilter(&ToUseImage);

    // Brightness, contrast, gamma and colors adjustments
    CurrentBrush->ApplyFilter(&ToUseImage);

    ForegroundImage=new QImage(dmax,dmax,QImage::Format_ARGB32_Premultiplied);
    QPainter P;
    P.begin(ForegroundImage);
    P.fillRect(QRectF(0,0,dmax,dmax),Transparent);
    P.drawImage(QRectF(DstX,DstY,DstW,DstH),ToUseImage,QRectF(0,0,DstW,DstH));
    P.end();

    SceneRect=QRect((this->width()-ForegroundImage->width())/2,(this->height()-ForegroundImage->height())/2,ForegroundImage->width(),ForegroundImage->height());

    // *************************************************************************
    // Create rulers
    // *************************************************************************

    MagnetHoriz.clear();
    MagnetVert.clear();
    if (MagneticRuler) {
        MagnetVert.append(dmax/2);
        MagnetVert.append(x1);
        MagnetVert.append(x2);
        MagnetVert.append(x3);
        MagnetVert.append(x4);
        MagnetHoriz.append(dmax/2);
        MagnetHoriz.append(y1);
        MagnetHoriz.append(y2);
        MagnetHoriz.append(y3);
        MagnetHoriz.append(y4);
    }
    // Clean collections
    for (int i=MagnetHoriz.count()-1;i>=0;i--) for (int j=0;j<i;j++) if (int(MagnetHoriz[j])==int(MagnetHoriz[i])) { MagnetHoriz.removeAt(i); break; }
    for (int i=MagnetVert.count()-1;i>=0;i--)  for (int j=0;j<i;j++) if (int(MagnetVert[j]) ==int(MagnetVert[i]))  { MagnetVert.removeAt(i);  break; }

    repaint();
}

//====================================================================================================================

bool cImgInteractiveZone::IsInRect(QPoint Pos,QRect Rect) {
    return ((Pos.x()-SceneRect.left()>=Rect.left())&&(Pos.x()-SceneRect.left()<=Rect.right())&&(Pos.y()-SceneRect.top()>=Rect.top())&&(Pos.y()-SceneRect.top()<=Rect.bottom()));
}

//====================================================================================================================

bool cImgInteractiveZone::IsInSelectedRect(QPoint Pos) {
    if ((Pos.x()-SceneRect.left()>=CurSelRect.left())&&(Pos.x()-SceneRect.left()<=CurSelRect.right())&&
        (Pos.y()-SceneRect.top()>=CurSelRect.top())&&(Pos.y()-SceneRect.top()<=CurSelRect.bottom())) return true;
    return false;
}

//====================================================================================================================

void cImgInteractiveZone::ManageCursor(QPoint Pos,Qt::KeyboardModifiers Modifiers) {
    if      (IsInRect(Pos,QRect(CurSelRect.left()-HANDLESIZEX/2, CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))                      setCursor(Qt::SizeBDiagCursor);  // Bottom left
    else if (IsInRect(Pos,QRect(CurSelRect.left()-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))                          setCursor(Qt::SizeFDiagCursor);  // Top left
    else if (IsInRect(Pos,QRect(CurSelRect.left()-HANDLESIZEX/2, CurSelRect.top()+CurSelRect.height()/2-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))   setCursor(Qt::SizeHorCursor);    // Left
    else if (IsInRect(Pos,QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))                         setCursor(Qt::SizeBDiagCursor);  // Top right
    else if (IsInRect(Pos,QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.top()+CurSelRect.height()/2-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))   setCursor(Qt::SizeHorCursor);    // Right
    else if (IsInRect(Pos,QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))                      setCursor(Qt::SizeFDiagCursor);  // Bottom right
    else if (IsInRect(Pos,QRect(CurSelRect.left()+CurSelRect.width()/2-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))     setCursor(Qt::SizeVerCursor);    // Top
    else if (IsInRect(Pos,QRect(CurSelRect.left()+CurSelRect.width()/2-HANDLESIZEX/2,CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))  setCursor(Qt::SizeVerCursor);    // Bottom
    else if ((IsInSelectedRect(Pos))&&(Modifiers==Qt::NoModifier))                                                                                 setCursor(Qt::OpenHandCursor);
    else setCursor(Qt::ArrowCursor);   // standard
}

//====================================================================================================================

QRect cImgInteractiveZone::ComputeNewCurSelRect() {
    if (CurrentBrush->LockGeometry) {
        return QRect((CurrentBrush->X+Move_X)*dmax,
                     (CurrentBrush->Y+Move_Y)*dmax,
                     (CurrentBrush->ZoomFactor+Scale_X)*dmax,
                     (CurrentBrush->ZoomFactor+Scale_X)*CurrentBrush->AspectRatio*dmax);
    } else {
        return QRect((CurrentBrush->X+Move_X)*dmax,
                     (CurrentBrush->Y+Move_Y)*dmax,
                     (CurrentBrush->ZoomFactor+Scale_X)*dmax,
                     ((CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio)+Scale_Y)*dmax);
    }
}


//====================================================================================================================

void cImgInteractiveZone::keyPressEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::keyPressEvent");
    if (!CurrentBrush) return;

    ManageCursor(mapFromGlobal(QCursor::pos()),event->modifiers());
    double StepX=double(1)/SceneRect.width();
    double StepY=double(1)/SceneRect.height();

    Move_X =0;
    Scale_X=0;
    Move_Y =0;
    Scale_Y=0;

    switch (event->key()) {
        case Qt::Key_Left:
            if (event->modifiers()==Qt::ShiftModifier)          {Move_X =-StepX; Scale_X=StepX; Move_Y=CurrentBrush->LockGeometry?(Move_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():0; Scale_Y=CurrentBrush->LockGeometry?(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():0;}
            else if (event->modifiers()==Qt::ControlModifier)   {Scale_X=-StepX; Scale_Y=CurrentBrush->LockGeometry?(CurrentBrush->AspectRatio*Scale_X*SceneRect.width())/SceneRect.height():0; }
            else                                                 Move_X=-StepX;
            break;
        case Qt::Key_Right:
            if (event->modifiers()==Qt::ShiftModifier)          {Move_X =StepX; Scale_X=-StepX; Move_Y=CurrentBrush->LockGeometry?(Move_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():0; Scale_Y=CurrentBrush->LockGeometry?(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():0;}
            else if (event->modifiers()==Qt::ControlModifier)   {Scale_X=StepX; Scale_Y=CurrentBrush->LockGeometry?(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():0; }
            else                                                 Move_X=StepX;
            break;
        case Qt::Key_Up:
            if (event->modifiers()==Qt::ShiftModifier)          {Move_Y =-StepY; Scale_Y=StepY; Move_X=CurrentBrush->LockGeometry?((Move_Y*SceneRect.height())/CurrentBrush->AspectRatio)/SceneRect.width():0; Scale_X=CurrentBrush->LockGeometry?((Scale_Y*SceneRect.height())/CurrentBrush->AspectRatio)/SceneRect.width():0;}
            else if (event->modifiers()==Qt::ControlModifier)   {Scale_Y=-StepY; Scale_X=CurrentBrush->LockGeometry?((Scale_Y*SceneRect.height())*CurrentBrush->AspectRatio)/SceneRect.width():0; }
            else                                                 Move_Y=-StepY;
            break;
        case Qt::Key_Down:
            if (event->modifiers()==Qt::ShiftModifier)          {Move_Y =StepY; Scale_Y=-StepY; Move_X=CurrentBrush->LockGeometry?((Move_Y*SceneRect.height())/CurrentBrush->AspectRatio)/SceneRect.width():0; Scale_X=CurrentBrush->LockGeometry?((Scale_Y*SceneRect.height())/CurrentBrush->AspectRatio)/SceneRect.width():0;}
            else if (event->modifiers()==Qt::ControlModifier)   {Scale_Y=StepY; Scale_X=CurrentBrush->LockGeometry?((Scale_Y*SceneRect.height())/CurrentBrush->AspectRatio)/SceneRect.width():0; }
            else                                                 Move_Y=StepY;
            break;
        default :
            QWidget::keyPressEvent(event);
            break;
    }
    if ((Move_X!=0)||(Move_Y!=0)||(Scale_X!=0)||(Scale_Y!=0)) emit TransformBlock(Move_X,Move_Y,Scale_X,Scale_Y);
}

//====================================================================================================================

void cImgInteractiveZone::keyReleaseEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::keyReleaseEvent");
    if (!CurrentBrush) return;

    ManageCursor(mapFromGlobal(QCursor::pos()),event->modifiers());
    QWidget::keyReleaseEvent(event);
}

//====================================================================================================================

void cImgInteractiveZone::mouseMoveEvent(QMouseEvent *event) {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::mouseMoveEvent");   // Remove : too much
    if (!CurrentBrush) return;

    if (!IsCapture) {

        ManageCursor(event->pos(),event->modifiers());

    } else {

        // *************************************************************************
        // Calc transformation
        // *************************************************************************

        double DX=double(event->pos().x()-CapturePos.x())/SceneRect.width();
        double DY=double(event->pos().y()-CapturePos.y())/SceneRect.height();
        QRect  NewCurSelRect;

        // Top left
        if  (TransfoType==RESIZEUPLEFT) {
            // Adjust DX and DY for resize not less than 0
            if (DX>=CurrentBrush->ZoomFactor-MINVALUE)  DX=CurrentBrush->ZoomFactor-MINVALUE;
            if (DY>=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE)  DY=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE;
            Move_X       =DX;
            Move_Y       =CurrentBrush->LockGeometry?(Move_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():DY;
            Scale_X      =-Move_X;
            Scale_Y      =-Move_Y;
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]<NewCurSelRect.right())&&(NewCurSelRect.left()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.left()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Move_X =(MagnetVert[Ruller]-CurrentBrush->X*SceneRect.width())/SceneRect.width();
                if (CurrentBrush->LockGeometry) Move_Y=(Move_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height();
                Scale_Y=-Move_Y;
                Scale_X=-Move_X;
                break;
            }

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]<NewCurSelRect.bottom())&&(NewCurSelRect.top()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.top()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Move_Y =(MagnetHoriz[Ruller]-CurrentBrush->Y*SceneRect.height())/SceneRect.height();
                if (CurrentBrush->LockGeometry) Move_X=(Move_Y*SceneRect.height()/CurrentBrush->AspectRatio)/SceneRect.width();
                Scale_Y=-Move_Y;
                Scale_X=-Move_X;
                break;
            }

        // Left
        } else if (TransfoType==RESIZELEFT) {
            // Adjust DX and DY for resize not less than 0
            if (DX>=CurrentBrush->ZoomFactor-MINVALUE)  DX=CurrentBrush->ZoomFactor-MINVALUE;
            if (DY>=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE)  DY=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE;
            Move_X =DX;
            Scale_X=-Move_X;
            if (CurrentBrush->LockGeometry) {
                Scale_Y=(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height();
                Move_Y =-Scale_Y/2;
            }
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]<NewCurSelRect.right())&&(NewCurSelRect.left()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.left()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Move_X =(MagnetVert[Ruller]-CurrentBrush->X*SceneRect.width())/SceneRect.width();
                Scale_X=-Move_X;
                if (CurrentBrush->LockGeometry) {
                    Scale_Y=(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height();
                    Move_Y =-Scale_Y/2;
                }
                break;
            }

        // Bottom left
        } else if (TransfoType==RESIZEDOWNLEFT) {
            // Adjust DX and DY for resize not less than 0
            if (CurrentBrush->ZoomFactor!=0) {
                if (DX>=CurrentBrush->ZoomFactor-MINVALUE)     DX=CurrentBrush->ZoomFactor-MINVALUE;
                if (DY<=-(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE))  DY=-(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE);
            }
            Move_X       =DX;
            Scale_X      =-Move_X;
            Move_Y       =0;
            Scale_Y      =CurrentBrush->LockGeometry?(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():DY;

            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]<NewCurSelRect.right())&&(NewCurSelRect.left()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.left()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Move_X =(MagnetVert[Ruller]-CurrentBrush->X*SceneRect.width())/SceneRect.width();
                Scale_X=-Move_X;
                if (CurrentBrush->LockGeometry) Scale_Y=-(Move_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height();
                break;
            }

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]>NewCurSelRect.top())&&(NewCurSelRect.bottom()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.bottom()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Scale_Y =(MagnetHoriz[Ruller]-(CurrentBrush->Y+CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio)*SceneRect.height())/SceneRect.height();
                if (CurrentBrush->LockGeometry) {
                    Move_X =-(Scale_Y*SceneRect.height()/CurrentBrush->AspectRatio)/SceneRect.width();
                    Scale_X=-Move_X;
                }
                break;
            }

        // Top right
        } else if (TransfoType==RESIZEUPRIGHT) {
            // Adjust DX and DY for resize not less than 0
            if (DX<=-(CurrentBrush->ZoomFactor-MINVALUE))  DX=-(CurrentBrush->ZoomFactor-MINVALUE);
            if (DY>=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE)     DY=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE;
            Move_X       =0;
            Scale_X      =DX;
            Move_Y       =CurrentBrush->LockGeometry?(-Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():DY;
            Scale_Y      =-Move_Y;
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]>NewCurSelRect.left())&&(NewCurSelRect.right()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.right()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Scale_X=(MagnetVert[Ruller]-(CurrentBrush->X+CurrentBrush->ZoomFactor)*SceneRect.width())/SceneRect.width();
                if (CurrentBrush->LockGeometry) Move_Y=-(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height();
                Scale_Y=-Move_Y;
                break;
            }

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]<NewCurSelRect.bottom())&&(NewCurSelRect.top()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.top()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Move_Y =(MagnetHoriz[Ruller]-CurrentBrush->Y*SceneRect.height())/SceneRect.height();
                if (CurrentBrush->LockGeometry) Scale_X=-(Move_Y*SceneRect.height()/CurrentBrush->AspectRatio)/SceneRect.width();
                Scale_Y=-Move_Y;
                break;
            }
        // Right
        } else if (TransfoType==RESIZERIGHT) {
            // Adjust DX and DY for resize not less than 0
            if (DX<=-(CurrentBrush->ZoomFactor-MINVALUE)) DX=-(CurrentBrush->ZoomFactor-MINVALUE);
            if (DY<=-(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE)) DY=-(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE);
            Move_X =0;
            Scale_X=DX;
            if (CurrentBrush->LockGeometry) {
                Scale_Y=(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height();
                Move_Y =-Scale_Y/2;
            }
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]>NewCurSelRect.left())&&(NewCurSelRect.right()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.right()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Scale_X=(MagnetVert[Ruller]-(CurrentBrush->X+CurrentBrush->ZoomFactor)*SceneRect.width())/SceneRect.width();
                if (CurrentBrush->LockGeometry) {
                    Scale_Y=(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height();
                    Move_Y =-Scale_Y/2;
                }
                break;
            }
        // Bottom right
        } else if (TransfoType==RESIZEDOWNRIGHT) {
            // Adjust DX and DY for resize not less than 0
            if (DX<=-(CurrentBrush->ZoomFactor-MINVALUE)) DX=-(CurrentBrush->ZoomFactor-MINVALUE);
            if (DY<=-(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE)) DY=-(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE);
            Move_X       =0;
            Scale_X      =DX;
            Move_Y       =0;
            Scale_Y      =CurrentBrush->LockGeometry?(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height():DY;
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]>NewCurSelRect.left())&&(NewCurSelRect.right()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.right()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Scale_X=(MagnetVert[Ruller]-(CurrentBrush->X+CurrentBrush->ZoomFactor)*SceneRect.width())/SceneRect.width();
                if (CurrentBrush->LockGeometry) Scale_Y=(Scale_X*SceneRect.width()*CurrentBrush->AspectRatio)/SceneRect.height();
                break;
            }

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]>NewCurSelRect.top())&&(NewCurSelRect.bottom()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.bottom()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Scale_Y =(MagnetHoriz[Ruller]-(CurrentBrush->Y+CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio)*SceneRect.height())/SceneRect.height();
                if (CurrentBrush->LockGeometry) Scale_X=(Scale_Y*SceneRect.height()/CurrentBrush->AspectRatio)/SceneRect.width();
                break;
            }
        // Top
        } else if (TransfoType==RESIZEUP) {
            // Adjust DX and DY for resize not less than 0
            if (DY>=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE)  DY=CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE;
            Move_Y =DY;
            Scale_Y=-Move_Y;
            if (CurrentBrush->LockGeometry) {
                Scale_X=(Scale_Y*SceneRect.height()/CurrentBrush->AspectRatio)/SceneRect.width();
                Move_X =-Scale_X/2;
            }
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]<NewCurSelRect.bottom())&&(NewCurSelRect.top()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.top()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Move_Y =(MagnetHoriz[Ruller]-CurrentBrush->Y*SceneRect.height())/SceneRect.height();
                Scale_Y=-Move_Y;
                if (CurrentBrush->LockGeometry) {
                    Scale_X=(Scale_Y*SceneRect.height()/CurrentBrush->AspectRatio)/SceneRect.width();
                    Move_X =-Scale_X/2;
                }
                break;
            }
        // Bottom
        } else if (TransfoType==RESIZEDOWN) {
            // Adjust DX and DY for resize not less than 0
            if (DY<=-(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE)) DY=-(CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio-MINVALUE);
            Move_Y =0;
            Scale_Y=DY;
            if (CurrentBrush->LockGeometry) {
                Scale_X=(Scale_Y*SceneRect.height()/CurrentBrush->AspectRatio)/SceneRect.width();
                Move_X =-Scale_X/2;
            }
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]>NewCurSelRect.top())&&(NewCurSelRect.bottom()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.bottom()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Scale_Y =(MagnetHoriz[Ruller]-(CurrentBrush->Y+CurrentBrush->ZoomFactor*CurrentBrush->AspectRatio)*SceneRect.height())/SceneRect.height();
                if (CurrentBrush->LockGeometry) {
                    Scale_X=(Scale_Y*SceneRect.height()/CurrentBrush->AspectRatio)/SceneRect.width();
                    Move_X =-Scale_X/2;
                }
                break;
            }
        // Move
        } else if (TransfoType==MOVEBLOCK) {
            Move_X =DX;
            Move_Y =DY;
            Scale_X=0;
            Scale_Y=0;

            if (MagneticRuler!=0) {
                QRect  NewCurSelRect=ComputeNewCurSelRect();
                DX=0;
                DY=0;

                // Apply magnetic rules vertical
                for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) {
                    if      ((NewCurSelRect.left()      >=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.left()      <=MagnetVert[Ruller]+HANDLEMAGNETX)) { DX=NewCurSelRect.left()      -MagnetVert[Ruller];  break; }
                    else if ((NewCurSelRect.right()     >=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.right()     <=MagnetVert[Ruller]+HANDLEMAGNETX)) { DX=NewCurSelRect.right()+1   -MagnetVert[Ruller];  break; }
                    else if ((NewCurSelRect.center().x()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.center().x()<=MagnetVert[Ruller]+HANDLEMAGNETX)) { DX=NewCurSelRect.center().x()-MagnetVert[Ruller];  break; }
                }

                // Apply magnetic rules horizontal
                for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) {
                    if      ((NewCurSelRect.top()       >=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.top()       <=MagnetHoriz[Ruller]+HANDLEMAGNETY)) { DY=NewCurSelRect.top()       -MagnetHoriz[Ruller];  break; }
                    else if ((NewCurSelRect.bottom()    >=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.bottom()    <=MagnetHoriz[Ruller]+HANDLEMAGNETY)) { DY=NewCurSelRect.bottom()+1  -MagnetHoriz[Ruller];  break; }
                    else if ((NewCurSelRect.center().y()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.center().y()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) { DY=NewCurSelRect.center().y()-MagnetHoriz[Ruller];  break; }
                }

                Move_X=double(event->pos().x()-DX-CapturePos.x())/SceneRect.width();
                Move_Y=double(event->pos().y()-DY-CapturePos.y())/SceneRect.height();
            }
        }
        if ((Move_X!=0)||(Move_Y!=0)||(Scale_X!=0)||(Scale_Y!=0)) {
            emit DisplayTransformBlock(Move_X,Move_Y,Scale_X,Scale_Y);
            repaint();
        }
    }
}

//====================================================================================================================

void cImgInteractiveZone::mousePressEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::mousePressEvent");
    if (!CurrentBrush) return;

    ManageCursor(event->pos(),event->modifiers());
    setFocus();
    if ((event->button()==Qt::LeftButton)&&(event->modifiers()==Qt::NoModifier)) {
        TransfoType=NOTYETDEFINED;

        // Resize
        if ((IsInRect(event->pos(),QRect(CurSelRect.left()-HANDLESIZEX/2, CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))                           TransfoType=RESIZEDOWNLEFT; // Bottom left
        else if ((IsInRect(event->pos(),QRect(CurSelRect.left()-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))                          TransfoType=RESIZEUPLEFT;   // Top left
        else if ((IsInRect(event->pos(),QRect(CurSelRect.left()-HANDLESIZEX/2, CurSelRect.top()+CurSelRect.height()/2-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))   TransfoType=RESIZELEFT;     // Left
        else if ((IsInRect(event->pos(),QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))                         TransfoType=RESIZEUPRIGHT;  // Top right
        else if ((IsInRect(event->pos(),QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.top()+CurSelRect.height()/2-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))   TransfoType=RESIZERIGHT;    // Right
        else if ((IsInRect(event->pos(),QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))                      TransfoType=RESIZEDOWNRIGHT;// Bottom right
        else if ((IsInRect(event->pos(),QRect(CurSelRect.left()+CurSelRect.width()/2-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))     TransfoType=RESIZEUP;       // Top
        else if ((IsInRect(event->pos(),QRect(CurSelRect.left()+CurSelRect.width()/2-HANDLESIZEX/2,CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))  TransfoType=RESIZEDOWN;     // Bottom
        // Move
        else if (IsInSelectedRect(event->pos())) {
            TransfoType=MOVEBLOCK;
            setCursor(Qt::ClosedHandCursor);
        }

        if (TransfoType!=NOTYETDEFINED) {
            IsCapture=true;
            CapturePos=event->pos();
            Move_X =0;
            Move_Y =0;
            Scale_X=0;
            Scale_Y=0;
        }
    }
}

//====================================================================================================================

void cImgInteractiveZone::mouseReleaseEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImgInteractiveZone::mouseReleaseEvent");
    if ((!CurrentBrush)||(!IsCapture)) return;

    IsCapture=false;

    // Block move
    if (((Move_X!=0)||(Move_Y!=0)||(Scale_X!=0)||(Scale_Y!=0))) emit TransformBlock(Move_X,Move_Y,Scale_X,Scale_Y);
    ManageCursor(event->pos(),event->modifiers());
}

