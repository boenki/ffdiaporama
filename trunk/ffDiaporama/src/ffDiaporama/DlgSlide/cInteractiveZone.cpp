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

#include "cInteractiveZone.h"

#define HANDLESIZEX     8
#define HANDLESIZEY     8
#define HANDLEMAGNETX   14
#define HANDLEMAGNETY   10

#define RULER_HORIZ_SCREENBORDER    0x0001
#define RULER_HORIZ_TVMARGIN        0x0002
#define RULER_HORIZ_SCREENCENTER    0x0004
#define RULER_HORIZ_UNSELECTED      0x0008
#define RULER_VERT_SCREENBORDER     0x0010
#define RULER_VERT_TVMARGIN         0x0020
#define RULER_VERT_SCREENCENTER     0x0040
#define RULER_VERT_UNSELECTED       0x0080

//====================================================================================================================

cInteractiveZone::cInteractiveZone(QWidget *parent):QWidget(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::cInteractiveZone");
    BlockTable      =NULL;
    DiaporamaObject =NULL;
    BackgroundImage =NULL;
    ForegroundImage =NULL;
    IsCapture       =false;
    TransfoType     =NOTYETDEFINED;
    setMouseTracking(true);
}

//====================================================================================================================

cInteractiveZone::~cInteractiveZone() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::~cInteractiveZone");
    if (BackgroundImage!=NULL) {
        delete BackgroundImage;
        BackgroundImage=NULL;
    }
    if (ForegroundImage!=NULL) {
        delete ForegroundImage;
        ForegroundImage=NULL;
    }
}

//====================================================================================================================

void cInteractiveZone::GetForDisplayUnit(double &DisplayW,double &DisplayH) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::GetForDisplayUnit");

    if (DiaporamaObject->Parent->ImageGeometry==GEOMETRY_4_3)        { DisplayW=1440; DisplayH=1080; }
    else if (DiaporamaObject->Parent->ImageGeometry==GEOMETRY_16_9)  { DisplayW=1920; DisplayH=1080; }
    else if (DiaporamaObject->Parent->ImageGeometry==GEOMETRY_40_17) { DisplayW=1920; DisplayH=816;  }
    else { DisplayW=0; DisplayH=0; }
}

//====================================================================================================================

void cInteractiveZone::DifferedEmitRightClickEvent() {
    emit RightClickEvent(NULL);
}

//====================================================================================================================

void cInteractiveZone::DifferedEmitDoubleClickEvent() {
    emit DoubleClickEvent(NULL);
}

//====================================================================================================================

void cInteractiveZone::paintEvent(QPaintEvent *) {
    if (!ForegroundImage) return;
    QPainter Painter(this);
    Painter.save();
    Painter.translate(SceneRect.left(),SceneRect.top());
    Painter.drawImage(0,0,*ForegroundImage);

    //Painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    Painter.setBrush(Qt::NoBrush);

    UpdateIsSelected();

    int CurSelect=0;


    // Draw blue frame borders when multi-select
    for (int i=0;i<BlockTable->CompositionList->List.count();i++) if ((BlockTable->CompositionList->List[i]->IsVisible)&&(IsSelected[i])) {

        QRect  NewRect;
        double NewX=BlockTable->CompositionList->List[i]->x;
        double NewY=BlockTable->CompositionList->List[i]->y;
        double NewW=BlockTable->CompositionList->List[i]->w;
        double NewH=BlockTable->CompositionList->List[i]->h;

        // Apply transformation
        if ((IsCapture)&&(TransfoType!=NOTYETDEFINED)) {
            NewX=NewX+Move_X-(NewX-Sel_X)+(Sel_W!=0?((NewX-Sel_X)/Sel_W)*(Sel_W+Scale_X):0);
            NewY=NewY+Move_Y-(NewY-Sel_Y)+(Sel_H!=0?((NewY-Sel_Y)/Sel_H)*(Sel_H+Scale_Y):0);
            NewW=(Sel_W!=0?(NewW/Sel_W)*(Sel_W+Scale_X):Scale_X);
            NewH=(Sel_H!=0?(NewH/Sel_H)*(Sel_H+Scale_Y):Scale_Y);
        }

        NewRect=QRect(NewX*SceneRect.width(),NewY*SceneRect.height(),NewW*SceneRect.width(),NewH*SceneRect.height());
        if (NbrSelected>1) {
            QPen pen(Qt::white);
            pen.setJoinStyle(Qt::RoundJoin);
            pen.setWidth(1);
            pen.setStyle(Qt::DashLine);
            Painter.setPen(pen);
            DrawSelect(Painter,QRectF(NewRect.left()+1,NewRect.top()+1,NewRect.width(),NewRect.height()),false);
            pen.setColor(Qt::blue);
            Painter.setPen(pen);
            DrawSelect(Painter,NewRect,false);
        }

        if (CurSelect==0) {
            CurSelRect=NewRect;
            if (!IsCapture) {
                Sel_X=BlockTable->CompositionList->List[i]->x;
                Sel_Y=BlockTable->CompositionList->List[i]->y;
                Sel_W=BlockTable->CompositionList->List[i]->w;
                Sel_H=BlockTable->CompositionList->List[i]->h;
            }
        } else {
            if (NewRect.left()  <CurSelRect.left())   {
                CurSelRect.setLeft(NewRect.left());
                if (!IsCapture) {
                    Sel_W=Sel_X+Sel_W-BlockTable->CompositionList->List[i]->x;
                    Sel_X=BlockTable->CompositionList->List[i]->x;
                }
            }
            if (NewRect.top()   <CurSelRect.top())    {
                CurSelRect.setTop(NewRect.top());
                if (!IsCapture) {
                    Sel_H=Sel_Y+Sel_H-BlockTable->CompositionList->List[i]->y;
                    Sel_Y=BlockTable->CompositionList->List[i]->y;
                }
            }
            if (NewRect.right() >CurSelRect.right())  {
                CurSelRect.setRight(NewRect.right());
                if (!IsCapture) Sel_W=BlockTable->CompositionList->List[i]->x+BlockTable->CompositionList->List[i]->w-Sel_X;
            }
            if (NewRect.bottom()>CurSelRect.bottom()) {
                CurSelRect.setBottom(NewRect.bottom());
                if (!IsCapture) Sel_H=BlockTable->CompositionList->List[i]->y+BlockTable->CompositionList->List[i]->h-Sel_Y;
            }
        }
        CurSelect++;

    }

    if ((!IsCapture)&&(NbrSelected>0)) {
        if ((CurSelRect.width()==0)||(CurSelRect.height()==0)) {
            Sel_W=0.02;
            Sel_H=0.02;
            CurSelRect.setWidth(2);
            CurSelRect.setHeight(2);
        }
        AspectRatio=double(CurSelRect.height())/double(CurSelRect.width());
    }

    // Draw rullers if they was enabled
    if (MagneticRuler!=0) {
        QList<double>   MagnetVert;
        QList<double>   MagnetHoriz;

        //if ((MagneticRuler&RULER_VERT_SCREENBORDER)!=0)    MagnetVert.append(0);                                                  // Left screen
        //if ((MagneticRuler&RULER_VERT_SCREENBORDER)!=0)    MagnetVert.append(SceneRect.width());                                  // Right screen
        if ((MagneticRuler&RULER_VERT_TVMARGIN)!=0)          MagnetVert.append(SceneRect.width()*0.05);                             // 5% Left TV Margins
        if ((MagneticRuler&RULER_VERT_TVMARGIN)!=0)          MagnetVert.append(SceneRect.width()-SceneRect.width()*0.05);           // 5% Right TV Margins
        if ((MagneticRuler&RULER_VERT_SCREENCENTER)!=0)      MagnetVert.append(SceneRect.width()*0.5);                              // Center or screen
        //if ((MagneticRuler&RULER_HORIZ_SCREENBORDER)!=0)   MagnetHoriz.append(0);                                                 // Top screen
        //if ((MagneticRuler&RULER_HORIZ_SCREENBORDER)!=0)   MagnetHoriz.append(SceneRect.height());                                // Bottom screen
        if ((MagneticRuler&RULER_HORIZ_TVMARGIN)!=0)         MagnetHoriz.append(SceneRect.height()*0.05);                           // 5% Up TV Margins
        if ((MagneticRuler&RULER_HORIZ_TVMARGIN)!=0)         MagnetHoriz.append(SceneRect.height()-SceneRect.height()*0.05);        // 5% Bottom TV Margins
        if ((MagneticRuler&RULER_HORIZ_SCREENCENTER)!=0)     MagnetHoriz.append(SceneRect.height()*0.5);                            // Center or screen
        // Display base rulers

        QPen pen=QPen(Qt::white);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setWidth(2);
        pen.setStyle(Qt::DotLine);
        Painter.setPen(pen);
        for (int i=0;i<MagnetVert.count();i++)  Painter.drawLine(MagnetVert[i]+1,0,MagnetVert[i]+1,SceneRect.height());
        for (int i=0;i<MagnetHoriz.count();i++) Painter.drawLine(0,MagnetHoriz[i]+1,SceneRect.width(),MagnetHoriz[i]+1);
        pen.setColor(QColor(0,255,0));
        Painter.setPen(pen);
        for (int i=0;i<MagnetVert.count();i++)  Painter.drawLine(MagnetVert[i],0,MagnetVert[i],SceneRect.height());
        for (int i=0;i<MagnetHoriz.count();i++) Painter.drawLine(0,MagnetHoriz[i],SceneRect.width(),MagnetHoriz[i]);

        if (IsCapture) {
            pen.setWidth(1);
            pen.setStyle(Qt::DashDotDotLine);
            MagnetVert.clear();
            MagnetHoriz.clear();
            double Ecart=0.005;
            // Unselected object
            if ((MagneticRuler&RULER_VERT_UNSELECTED)!=0) for (int i=BlockTable->CompositionList->List.count()-1;i>=0;i--) if ((!IsSelected[i])&&(BlockTable->CompositionList->List[i]->IsVisible)) {
                double a1=double(CurSelRect.left())/SceneRect.width();
                double a2=(double(CurSelRect.left())+double(CurSelRect.width())/2)/SceneRect.width();
                double a3=double(CurSelRect.right())/SceneRect.width();
                double x1=BlockTable->CompositionList->List[i]->x;
                double x2=BlockTable->CompositionList->List[i]->x+BlockTable->CompositionList->List[i]->w/2;
                double x3=BlockTable->CompositionList->List[i]->x+BlockTable->CompositionList->List[i]->w;
                if (((x1>(a1-Ecart))&&(x1<(a1+Ecart)))||((x1>(a2-Ecart))&&(x1<(a2+Ecart)))||((x1>(a3-Ecart))&&(x1<(a3+Ecart)))) MagnetVert.append(x1*SceneRect.width());
                if (((x2>(a1-Ecart))&&(x2<(a1+Ecart)))||((x2>(a2-Ecart))&&(x2<(a2+Ecart)))||((x2>(a3-Ecart))&&(x2<(a3+Ecart)))) MagnetVert.append(x2*SceneRect.width());
                if (((x3>(a1-Ecart))&&(x3<(a1+Ecart)))||((x3>(a2-Ecart))&&(x3<(a2+Ecart)))||((x3>(a3-Ecart))&&(x3<(a3+Ecart)))) MagnetVert.append(x3*SceneRect.width());
            }
            if ((MagneticRuler&RULER_HORIZ_UNSELECTED)!=0) for (int i=BlockTable->CompositionList->List.count()-1;i>=0;i--) if ((!IsSelected[i])&&(BlockTable->CompositionList->List[i]->IsVisible)) {
                double a1=double(CurSelRect.top())/SceneRect.height();
                double a2=(double(CurSelRect.top())+double(CurSelRect.height())/2)/SceneRect.height();
                double a3=double(CurSelRect.bottom())/SceneRect.height();
                double y1=BlockTable->CompositionList->List[i]->y;
                double y2=BlockTable->CompositionList->List[i]->y+BlockTable->CompositionList->List[i]->h/2;
                double y3=BlockTable->CompositionList->List[i]->y+BlockTable->CompositionList->List[i]->h;
                if (((y1>(a1-Ecart))&&(y1<(a1+Ecart)))||((y1>(a2-Ecart))&&(y1<(a2+Ecart)))||((y1>(a3-Ecart))&&(y1<(a3+Ecart)))) MagnetHoriz.append(y1*SceneRect.height());
                if (((y2>(a1-Ecart))&&(y2<(a1+Ecart)))||((y2>(a2-Ecart))&&(y2<(a2+Ecart)))||((y2>(a3-Ecart))&&(y2<(a3+Ecart)))) MagnetHoriz.append(y2*SceneRect.height());
                if (((y3>(a1-Ecart))&&(y3<(a1+Ecart)))||((y3>(a2-Ecart))&&(y3<(a2+Ecart)))||((y3>(a3-Ecart))&&(y3<(a3+Ecart)))) MagnetHoriz.append(y3*SceneRect.height());
            }
            // Display small rulers
            pen.setColor(Qt::white);
            Painter.setPen(pen);
            for (int i=0;i<MagnetVert.count();i++)  Painter.drawLine(MagnetVert[i]+1,0,MagnetVert[i]+1,SceneRect.height());
            for (int i=0;i<MagnetHoriz.count();i++) Painter.drawLine(0,MagnetHoriz[i]+1,SceneRect.width(),MagnetHoriz[i]+1);
            pen.setColor(QColor(0,255,0));
            Painter.setPen(pen);
            for (int i=0;i<MagnetVert.count();i++)  Painter.drawLine(MagnetVert[i],0,MagnetVert[i],SceneRect.height());
            for (int i=0;i<MagnetHoriz.count();i++) Painter.drawLine(0,MagnetHoriz[i],SceneRect.width(),MagnetHoriz[i]);
        }
    }

    // Draw select frame border
    if (NbrSelected>0) {
        QPen pen(Qt::white);
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);
        Painter.setPen(pen);
        DrawSelect(Painter,QRectF(CurSelRect.left()+1,CurSelRect.top()+1,CurSelRect.width(),CurSelRect.height()),true);
        pen.setColor(Qt::red);
        Painter.setPen(pen);
        DrawSelect(Painter,CurSelRect,true);
    }

    Painter.restore();
}

void cInteractiveZone::DrawSelect(QPainter &Painter,QRectF Rect,bool WithHandles) {
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

void cInteractiveZone::RefreshDisplay() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::RefreshDisplay");
    if ((!BlockTable)||(!BlockTable->CompositionList)) return;

    if (BlockTable->updatesEnabled()) {
        // Prepare BackgroundImage if not exist
        if (!BackgroundImage) {
            QPainter Painter;
            double   xmax,ymax;
            GetForDisplayUnit(xmax,ymax);
            BackgroundImage=new QImage(xmax,ymax,QImage::Format_ARGB32_Premultiplied);
            Painter.begin(BackgroundImage);
            DiaporamaObject->Parent->PrepareBackground(DiaporamaObject->Parent->GetObjectIndex(DiaporamaObject),xmax,ymax,&Painter,0,0);
            Painter.end();
        }

        if (ForegroundImage!=NULL) {
            delete ForegroundImage;
            ForegroundImage=NULL;
        }

        ForegroundImage =new QImage(BackgroundImage->scaled(QSize(this->width(),this->height()),Qt::KeepAspectRatio,Qt::SmoothTransformation));
        SceneRect       =QRect((this->width()-ForegroundImage->width())/2,(this->height()-ForegroundImage->height())/2,ForegroundImage->width(),ForegroundImage->height());

        UpdateIsSelected();

        // Draw image of the scene under the background
        QPainter P;
        P.begin(ForegroundImage);
        P.save();

        int StartVideoPos=0;

        for (int i=0;i<BlockTable->CompositionList->List.count();i++) if (BlockTable->CompositionList->List[i]->IsVisible) {
            // If it's a video block, calc video position
            if (BlockTable->CompositionList->List[i]->BackgroundBrush->Video) {
                StartVideoPos=QTime(0,0,0,0).msecsTo(BlockTable->CompositionList->List[i]->BackgroundBrush->Video->StartPos);
                for (int k=0;k<CurrentShotNbr;k++) {
                    for (int l=0;l<DiaporamaObject->List[k]->ShotComposition.List.count();l++) {
                        if (DiaporamaObject->List[k]->ShotComposition.List[l]->IndexKey==BlockTable->CompositionList->List[i]->IndexKey) {
                            if (DiaporamaObject->List[k]->ShotComposition.List[l]->IsVisible) StartVideoPos+=DiaporamaObject->List[k]->StaticDuration;
                            l=DiaporamaObject->List[k]->ShotComposition.List.count();    // stop loop
                        }
                    }
                }
            } else StartVideoPos=0;

            // Draw composition block
            double NewX=BlockTable->CompositionList->List[i]->x;
            double NewY=BlockTable->CompositionList->List[i]->y;
            double NewW=BlockTable->CompositionList->List[i]->w;
            double NewH=BlockTable->CompositionList->List[i]->h;

            // Apply transformation
            if ((IsSelected[i])&&(IsCapture)&&(TransfoType!=NOTYETDEFINED)) {
                NewX=NewX+Move_X-(NewX-Sel_X)+(Sel_W!=0?((NewX-Sel_X)/Sel_W)*(Sel_W+Scale_X):0);
                NewY=NewY+Move_Y-(NewY-Sel_Y)+(Sel_H!=0?((NewY-Sel_Y)/Sel_H)*(Sel_H+Scale_Y):0);
                NewW=(Sel_W!=0?(NewW/Sel_W)*(Sel_W+Scale_X):Scale_X);
                NewH=(Sel_H!=0?(NewH/Sel_H)*(Sel_H+Scale_Y):Scale_Y);
            }

            BlockTable->CompositionList->List[i]->DrawCompositionObject(&P,double(ForegroundImage->height())/double(1080),0,0,ForegroundImage->width(),ForegroundImage->height(),true,0,StartVideoPos,
                                                                        NULL,1,NULL,true,DiaporamaObject->List[CurrentShotNbr]->StaticDuration,false,
                                                                        (IsCapture)&&(TransfoType!=NOTYETDEFINED),NewX,NewY,NewW,NewH);

            if ((CurrentShotNbr>0)&&(BlockTable->CompositionList->List[i]->SameAsPrevShot))
                P.drawImage((NewX+NewW/2)*ForegroundImage->width()-12,(NewY+NewH/2)*ForegroundImage->height()-12,QImage(":/img/Lock24.png"));
        }

        P.restore();
        P.end();
    }

    repaint();
}

//====================================================================================================================

void cInteractiveZone::UpdateIsSelected() {
    QModelIndexList SelList=BlockTable->selectionModel()->selectedIndexes();
    IsSelected.clear();
    NbrSelected           =0;
    LockGeometry          =false;
    SelectionHaveLockBlock=false;

    for (int i=0;i<BlockTable->rowCount();i++)  IsSelected.append(false);
    for (int i=0;i<SelList.count();i++)         IsSelected[SelList.at(i).row()]=BlockTable->CompositionList->List[SelList.at(i).row()]->IsVisible;

    for (int i=0;i<IsSelected.count();i++) if (IsSelected[i]) {
        NbrSelected++;
        if ((BlockTable->CompositionList->List[i]->BackgroundBrush->LockGeometry)||
            (BlockTable->CompositionList->List[i]->BackgroundBrush->Image!=NULL)||
            (BlockTable->CompositionList->List[i]->BackgroundBrush->Video!=NULL)) LockGeometry=true;
        if ((CurrentShotNbr>0)&&(BlockTable->CompositionList->List[i]->SameAsPrevShot)) SelectionHaveLockBlock=true;
    }
}

//====================================================================================================================

bool cInteractiveZone::IsInRect(QPoint Pos,QRect Rect) {
    return ((Pos.x()-SceneRect.left()>=Rect.left())&&(Pos.x()-SceneRect.left()<=Rect.right())&&(Pos.y()-SceneRect.top()>=Rect.top())&&(Pos.y()-SceneRect.top()<=Rect.bottom()));
}

//====================================================================================================================

bool cInteractiveZone::IsInSelectedRect(QPoint Pos) {
    for (int i=0;i<BlockTable->CompositionList->List.count();i++) if ((BlockTable->CompositionList->List[i]->IsVisible)&&(IsSelected[i])) {
        QRect ObjRect=QRect(BlockTable->CompositionList->List[i]->x*SceneRect.width(),BlockTable->CompositionList->List[i]->y*SceneRect.height(),
                            BlockTable->CompositionList->List[i]->w*SceneRect.width(),BlockTable->CompositionList->List[i]->h*SceneRect.height());
        if ((Pos.x()-SceneRect.left()>=ObjRect.left())&&(Pos.x()-SceneRect.left()<=ObjRect.right())&&(Pos.y()-SceneRect.top()>=ObjRect.top())&&(Pos.y()-SceneRect.top()<=ObjRect.bottom())) return true;
    }
    return false;
}

//====================================================================================================================

void cInteractiveZone::ManageCursor(QPoint Pos,Qt::KeyboardModifiers Modifiers) {
    if      (IsInRect(Pos,QRect(CurSelRect.left()-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))                          setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::SizeFDiagCursor);  // Top left
    else if (IsInRect(Pos,QRect(CurSelRect.left()-HANDLESIZEX/2, CurSelRect.top()+CurSelRect.height()/2-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))   setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::SizeHorCursor);    // Left
    else if (IsInRect(Pos,QRect(CurSelRect.left()-HANDLESIZEX/2, CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))                      setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::SizeBDiagCursor);  // Bottom left
    else if (IsInRect(Pos,QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))                         setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::SizeBDiagCursor);  // Top right
    else if (IsInRect(Pos,QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.top()+CurSelRect.height()/2-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))   setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::SizeHorCursor);    // Right
    else if (IsInRect(Pos,QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))                      setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::SizeFDiagCursor);  // Bottom right
    else if (IsInRect(Pos,QRect(CurSelRect.left()+CurSelRect.width()/2-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))     setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::SizeVerCursor);    // Top
    else if (IsInRect(Pos,QRect(CurSelRect.left()+CurSelRect.width()/2-HANDLESIZEX/2,CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY)))  setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::SizeVerCursor);    // Bottom
    else if (IsInSelectedRect(Pos)) {
        if      (Modifiers==Qt::NoModifier)                                         setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::OpenHandCursor);
        else if ((Modifiers==Qt::ControlModifier)||(Modifiers==Qt::ShiftModifier))  setCursor(Qt::PointingHandCursor);
        else if (Modifiers==(Qt::ControlModifier|Qt::ShiftModifier))                setCursor(Qt::CrossCursor);
        else                                                                        setCursor(SelectionHaveLockBlock?Qt::ForbiddenCursor:Qt::ArrowCursor);
    } else setCursor(Qt::ArrowCursor);   // standard
}

//====================================================================================================================

QRect cInteractiveZone::ComputeNewCurSelRect() {
    QRect NewCurSelRect;
    int CurSelect=0;
    for (int i=0;i<BlockTable->CompositionList->List.count();i++) if ((BlockTable->CompositionList->List[i]->IsVisible)&&(IsSelected[i])) {
        double NewX=BlockTable->CompositionList->List[i]->x;
        double NewY=BlockTable->CompositionList->List[i]->y;
        double NewW=BlockTable->CompositionList->List[i]->w;
        double NewH=BlockTable->CompositionList->List[i]->h;

        NewX=NewX+Move_X-(NewX-Sel_X)+(Sel_W!=0?((NewX-Sel_X)/Sel_W)*(Sel_W+Scale_X):0);
        NewY=NewY+Move_Y-(NewY-Sel_Y)+(Sel_H!=0?((NewY-Sel_Y)/Sel_H)*(Sel_H+Scale_Y):0);
        NewW=(Sel_W!=0?(NewW/Sel_W)*(Sel_W+Scale_X):Scale_X);
        NewH=(Sel_H!=0?(NewH/Sel_H)*(Sel_H+Scale_Y):Scale_Y);

        QRect NewRect=QRect(NewX*SceneRect.width(),NewY*SceneRect.height(),NewW*SceneRect.width(),NewH*SceneRect.height());
        if (CurSelect==0) NewCurSelRect=NewRect; else {
            if (NewRect.left()  <NewCurSelRect.left())   NewCurSelRect.setLeft(NewRect.left());
            if (NewRect.top()   <NewCurSelRect.top())    NewCurSelRect.setTop(NewRect.top());
            if (NewRect.right() >NewCurSelRect.right())  NewCurSelRect.setRight(NewRect.right());
            if (NewRect.bottom()>NewCurSelRect.bottom()) NewCurSelRect.setBottom(NewRect.bottom());
        }
        CurSelect++;
    }
    return NewCurSelRect;
}

//====================================================================================================================

void cInteractiveZone::keyPressEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::keyPressEvent");
    if ((!BlockTable)||(!BlockTable->CompositionList)) return;

    ManageCursor(mapFromGlobal(QCursor::pos()),event->modifiers());
    if ((!SelectionHaveLockBlock)&&(NbrSelected>0)) {
        double StepX=double(1)/SceneRect.width();
        double StepY=double(1)/SceneRect.height();

        Move_X =0;
        Scale_X=0;
        Move_Y =0;
        Scale_Y=0;

        switch (event->key()) {
            case Qt::Key_Left:
                if (event->modifiers()==Qt::ShiftModifier)          {Move_X =-StepX; Scale_X=StepX; Move_Y=LockGeometry?(AspectRatio*Move_X*SceneRect.width())/SceneRect.height():0; Scale_Y=LockGeometry?(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height():0;}
                else if (event->modifiers()==Qt::ControlModifier)   {Scale_X=-StepX; Scale_Y=LockGeometry?(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height():0; }
                else                                                 Move_X=-StepX;
                break;
            case Qt::Key_Right:
                if (event->modifiers()==Qt::ShiftModifier)          {Move_X =StepX; Scale_X=-StepX; Move_Y=LockGeometry?(AspectRatio*Move_X*SceneRect.width())/SceneRect.height():0; Scale_Y=LockGeometry?(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height():0;}
                else if (event->modifiers()==Qt::ControlModifier)   {Scale_X=StepX; Scale_Y=LockGeometry?(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height():0; }
                else                                                 Move_X=StepX;
                break;
            case Qt::Key_Up:
                if (event->modifiers()==Qt::ShiftModifier)          {Move_Y =-StepY; Scale_Y=StepY; Move_X=LockGeometry?((Move_Y*SceneRect.height())/AspectRatio)/SceneRect.width():0; Scale_X=LockGeometry?((Scale_Y*SceneRect.height())/AspectRatio)/SceneRect.width():0;}
                else if (event->modifiers()==Qt::ControlModifier)   {Scale_Y=-StepY; Scale_X=LockGeometry?((Scale_Y*SceneRect.height())/AspectRatio)/SceneRect.width():0; }
                else                                                 Move_Y=-StepY;
                break;
            case Qt::Key_Down:
                if (event->modifiers()==Qt::ShiftModifier)          {Move_Y =StepY; Scale_Y=-StepY; Move_X=LockGeometry?((Move_Y*SceneRect.height())/AspectRatio)/SceneRect.width():0; Scale_X=LockGeometry?((Scale_Y*SceneRect.height())/AspectRatio)/SceneRect.width():0;}
                else if (event->modifiers()==Qt::ControlModifier)   {Scale_Y=StepY; Scale_X=LockGeometry?((Scale_Y*SceneRect.height())/AspectRatio)/SceneRect.width():0; }
                else                                                 Move_Y=StepY;
                break;
            default :
                QWidget::keyPressEvent(event);
                break;
        }
        if ((Move_X!=0)||(Move_Y!=0)||(Scale_X!=0)||(Scale_Y!=0)) emit TransformBlock(Move_X,Move_Y,Scale_X,Scale_Y,Sel_X,Sel_Y,Sel_W,Sel_H);
    } else QWidget::keyPressEvent(event);
}

//====================================================================================================================

void cInteractiveZone::keyReleaseEvent(QKeyEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::keyReleaseEvent");
    if ((!BlockTable)||(!BlockTable->CompositionList)) return;

    ManageCursor(mapFromGlobal(QCursor::pos()),event->modifiers());
    QWidget::keyReleaseEvent(event);
}

//====================================================================================================================

void cInteractiveZone::mouseMoveEvent(QMouseEvent *event) {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::mouseMoveEvent");   // Remove : too much
    if ((!BlockTable)||(!BlockTable->CompositionList)) return;

    if ((!IsCapture)||(SelectionHaveLockBlock)) {

        ManageCursor(event->pos(),event->modifiers());

    } else {

        // *************************************************************************
        // Create rulers
        // *************************************************************************

        QList<double>   MagnetVert;
        QList<double>   MagnetHoriz;
        if ((MagneticRuler&RULER_VERT_SCREENBORDER)!=0)    MagnetVert.append(0);                                                  // Left screen
        if ((MagneticRuler&RULER_VERT_SCREENBORDER)!=0)    MagnetVert.append(SceneRect.width());                                  // Right screen
        if ((MagneticRuler&RULER_VERT_TVMARGIN)!=0)        MagnetVert.append(SceneRect.width()*0.05);                             // 5% Left TV Margins
        if ((MagneticRuler&RULER_VERT_TVMARGIN)!=0)        MagnetVert.append(SceneRect.width()-SceneRect.width()*0.05);           // 5% Right TV Margins
        if ((MagneticRuler&RULER_VERT_SCREENCENTER)!=0)    MagnetVert.append(SceneRect.width()*0.5);                              // Center or screen
        // Unselected object
        if ((MagneticRuler&RULER_VERT_UNSELECTED)!=0) for (int i=BlockTable->CompositionList->List.count()-1;i>=0;i--) if ((!IsSelected[i])&&(BlockTable->CompositionList->List[i]->IsVisible)) {
            MagnetVert.append(BlockTable->CompositionList->List[i]->x*SceneRect.width());
            MagnetVert.append(BlockTable->CompositionList->List[i]->x*SceneRect.width()+BlockTable->CompositionList->List[i]->w*SceneRect.width()/2);
            MagnetVert.append(BlockTable->CompositionList->List[i]->x*SceneRect.width()+BlockTable->CompositionList->List[i]->w*SceneRect.width());
        }
        // Clean collection
        for (int i=MagnetVert.count()-1;i>=0;i--) for (int j=0;j<i;j++) if (MagnetVert[j]==MagnetVert[i]) { MagnetVert.removeAt(i); break; }

        if ((MagneticRuler&RULER_HORIZ_SCREENBORDER)!=0)     MagnetHoriz.append(0);                                                  // Top screen
        if ((MagneticRuler&RULER_HORIZ_SCREENBORDER)!=0)     MagnetHoriz.append(SceneRect.height());                                 // Bottom screen
        if ((MagneticRuler&RULER_HORIZ_TVMARGIN)!=0)         MagnetHoriz.append(SceneRect.height()*0.05);                            // 5% Up TV Margins
        if ((MagneticRuler&RULER_HORIZ_TVMARGIN)!=0)         MagnetHoriz.append(SceneRect.height()-SceneRect.height()*0.05);         // 5% Bottom TV Margins
        if ((MagneticRuler&RULER_HORIZ_SCREENCENTER)!=0)     MagnetHoriz.append(SceneRect.height()*0.5);                             // Center or screen
        // Unselected object
        if ((MagneticRuler&RULER_HORIZ_UNSELECTED)!=0) for (int i=BlockTable->CompositionList->List.count()-1;i>=0;i--) if ((!IsSelected[i])&&(BlockTable->CompositionList->List[i]->IsVisible)) {
            MagnetHoriz.append(BlockTable->CompositionList->List[i]->y*SceneRect.height());
            MagnetHoriz.append(BlockTable->CompositionList->List[i]->y*SceneRect.height()+BlockTable->CompositionList->List[i]->h*SceneRect.height()/2);
            MagnetHoriz.append(BlockTable->CompositionList->List[i]->y*SceneRect.height()+BlockTable->CompositionList->List[i]->h*SceneRect.height());
        }
        // Clean collection
        for (int i=MagnetHoriz.count()-1;i>=0;i--) for (int j=0;j<i;j++) if (MagnetHoriz[j]==MagnetHoriz[i]) { MagnetHoriz.removeAt(i); break; }

        // *************************************************************************
        // Calc transformation
        // *************************************************************************

        double DX=double(event->pos().x()-CapturePos.x())/SceneRect.width();
        double DY=double(event->pos().y()-CapturePos.y())/SceneRect.height();
        QRect  NewCurSelRect;

        // Top left
        if  (TransfoType==RESIZEUPLEFT) {
            // Adjust DX and DY for resize not less than 0
            if (DX>=Sel_W)  DX=Sel_W;
            if (DY>=Sel_H)  DY=Sel_H;
            Move_X       =DX;
            Move_Y       =LockGeometry?(AspectRatio*Move_X*SceneRect.width())/SceneRect.height():DY;
            Scale_X      =-Move_X;
            Scale_Y      =-Move_Y;
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]<NewCurSelRect.right())&&(NewCurSelRect.left()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.left()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Move_X =(MagnetVert[Ruller]-Sel_X*SceneRect.width())/SceneRect.width();
                if (LockGeometry) Move_Y=(AspectRatio*Move_X*SceneRect.width())/SceneRect.height();
                Scale_Y=-Move_Y;
                Scale_X=-Move_X;
                break;
            }

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]<NewCurSelRect.bottom())&&(NewCurSelRect.top()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.top()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Move_Y =(MagnetHoriz[Ruller]-Sel_Y*SceneRect.height())/SceneRect.height();
                if (LockGeometry) Move_X=(Move_Y*SceneRect.height()/AspectRatio)/SceneRect.width();
                Scale_Y=-Move_Y;
                Scale_X=-Move_X;
                break;
            }

        // Left
        } else if (TransfoType==RESIZELEFT) {
            // Adjust DX and DY for resize not less than 0
            if (DX>=Sel_W)  DX=Sel_W;
            if (DY>=Sel_H)  DY=Sel_H;
            Move_X =DX;
            Scale_X=-Move_X;
            if (LockGeometry) {
                Scale_Y=(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height();
                Move_Y =-Scale_Y/2;
            }
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]<NewCurSelRect.right())&&(NewCurSelRect.left()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.left()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Move_X =(MagnetVert[Ruller]-Sel_X*SceneRect.width())/SceneRect.width();
                Scale_X=-Move_X;
                if (LockGeometry) {
                    Scale_Y=(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height();
                    Move_Y =-Scale_Y/2;
                }
                break;
            }

        // Bottom left
        } else if (TransfoType==RESIZEDOWNLEFT) {
            // Adjust DX and DY for resize not less than 0
            if (Sel_W!=0) {
                if (DX>=Sel_W)  DX=Sel_W;
                if (DY<=-Sel_H) DY=-Sel_H;
            }
            Move_X       =DX;
            Scale_X      =-Move_X;
            Move_Y       =0;
            Scale_Y      =LockGeometry?(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height():DY;

            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]<NewCurSelRect.right())&&(NewCurSelRect.left()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.left()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Move_X =(MagnetVert[Ruller]-Sel_X*SceneRect.width())/SceneRect.width();
                Scale_X=-Move_X;
                if (LockGeometry) Scale_Y=-(AspectRatio*Move_X*SceneRect.width())/SceneRect.height();
                break;
            }

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]>NewCurSelRect.top())&&(NewCurSelRect.bottom()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.bottom()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Scale_Y =(MagnetHoriz[Ruller]-(Sel_Y+Sel_H)*SceneRect.height())/SceneRect.height();
                if (LockGeometry) {
                    Move_X =-(Scale_Y*SceneRect.height()/AspectRatio)/SceneRect.width();
                    Scale_X=-Move_X;
                }
                break;
            }

        // Top right
        } else if (TransfoType==RESIZEUPRIGHT) {
            // Adjust DX and DY for resize not less than 0
            if (DX<=-Sel_W) DX=-Sel_W;
            if (DY>=Sel_H)  DY=Sel_H;
            Move_X       =0;
            Scale_X      =DX;
            Move_Y       =LockGeometry?(-AspectRatio*Scale_X*SceneRect.width())/SceneRect.height():DY;
            Scale_Y      =-Move_Y;
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]>NewCurSelRect.left())&&(NewCurSelRect.right()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.right()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Scale_X=(MagnetVert[Ruller]-(Sel_X+Sel_W)*SceneRect.width())/SceneRect.width();
                if (LockGeometry) Move_Y=-(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height();
                Scale_Y=-Move_Y;
                break;
            }

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]<NewCurSelRect.bottom())&&(NewCurSelRect.top()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.top()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Move_Y =(MagnetHoriz[Ruller]-Sel_Y*SceneRect.height())/SceneRect.height();
                if (LockGeometry) Scale_X=-(Move_Y*SceneRect.height()/AspectRatio)/SceneRect.width();
                Scale_Y=-Move_Y;
                break;
            }
        // Right
        } else if (TransfoType==RESIZERIGHT) {
            // Adjust DX and DY for resize not less than 0
            if (DX<=-Sel_W) DX=-Sel_W;
            if (DY<=-Sel_H) DY=-Sel_H;
            Move_X =0;
            Scale_X=DX;
            if (LockGeometry) {
                Scale_Y=(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height();
                Move_Y =-Scale_Y/2;
            }
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]>NewCurSelRect.left())&&(NewCurSelRect.right()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.right()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Scale_X=(MagnetVert[Ruller]-(Sel_X+Sel_W)*SceneRect.width())/SceneRect.width();
                if (LockGeometry) {
                    Scale_Y=(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height();
                    Move_Y =-Scale_Y/2;
                }
                break;
            }
        // Bottom right
        } else if (TransfoType==RESIZEDOWNRIGHT) {
            // Adjust DX and DY for resize not less than 0
            if (DX<=-Sel_W) DX=-Sel_W;
            if (DY<=-Sel_H) DY=-Sel_H;
            Move_X       =0;
            Scale_X      =DX;
            Move_Y       =0;
            Scale_Y      =LockGeometry?(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height():DY;
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules vertical
            for (int Ruller=0;Ruller<MagnetVert.count();Ruller++) if ((MagnetVert[Ruller]>NewCurSelRect.left())&&(NewCurSelRect.right()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.right()<=MagnetVert[Ruller]+HANDLEMAGNETX)) {
                Scale_X=(MagnetVert[Ruller]-(Sel_X+Sel_W)*SceneRect.width())/SceneRect.width();
                if (LockGeometry) Scale_Y=(AspectRatio*Scale_X*SceneRect.width())/SceneRect.height();
                break;
            }

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]>NewCurSelRect.top())&&(NewCurSelRect.bottom()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.bottom()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Scale_Y =(MagnetHoriz[Ruller]-(Sel_Y+Sel_H)*SceneRect.height())/SceneRect.height();
                if (LockGeometry) Scale_X=(Scale_Y*SceneRect.height()/AspectRatio)/SceneRect.width();
                break;
            }
        // Top
        } else if (TransfoType==RESIZEUP) {
            // Adjust DX and DY for resize not less than 0
            if (DY>=Sel_H)  DY=Sel_H;
            Move_Y =DY;
            Scale_Y=-Move_Y;
            if (LockGeometry) {
                Scale_X=(Scale_Y*SceneRect.height()/AspectRatio)/SceneRect.width();
                Move_X =-Scale_X/2;
            }
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]<NewCurSelRect.bottom())&&(NewCurSelRect.top()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.top()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Move_Y =(MagnetHoriz[Ruller]-Sel_Y*SceneRect.height())/SceneRect.height();
                Scale_Y=-Move_Y;
                if (LockGeometry) {
                    Scale_X=(Scale_Y*SceneRect.height()/AspectRatio)/SceneRect.width();
                    Move_X =-Scale_X/2;
                }
                break;
            }
        // Bottom
        } else if (TransfoType==RESIZEDOWN) {
            // Adjust DX and DY for resize not less than 0
            if (DY<=-Sel_H) DY=-Sel_H;
            Move_Y =0;
            Scale_Y=DY;
            if (LockGeometry) {
                Scale_X=(Scale_Y*SceneRect.height()/AspectRatio)/SceneRect.width();
                Move_X =-Scale_X/2;
            }
            NewCurSelRect=ComputeNewCurSelRect();

            // Apply magnetic rules horizontal
            for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) if ((MagnetHoriz[Ruller]>NewCurSelRect.top())&&(NewCurSelRect.bottom()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.bottom()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) {
                Scale_Y =(MagnetHoriz[Ruller]-(Sel_Y+Sel_H)*SceneRect.height())/SceneRect.height();
                if (LockGeometry) {
                    Scale_X=(Scale_Y*SceneRect.height()/AspectRatio)/SceneRect.width();
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
                    else if ((NewCurSelRect.right()     >=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.right()     <=MagnetVert[Ruller]+HANDLEMAGNETX)) { DX=NewCurSelRect.right()     -MagnetVert[Ruller];  break; }
                    else if ((NewCurSelRect.center().x()>=MagnetVert[Ruller]-HANDLEMAGNETX)&&(NewCurSelRect.center().x()<=MagnetVert[Ruller]+HANDLEMAGNETX)) { DX=NewCurSelRect.center().x()-MagnetVert[Ruller];  break; }
                }

                // Apply magnetic rules horizontal
                for (int Ruller=0;Ruller<MagnetHoriz.count();Ruller++) {
                    if      ((NewCurSelRect.top()       >=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.top()       <=MagnetHoriz[Ruller]+HANDLEMAGNETY)) { DY=NewCurSelRect.top()       -MagnetHoriz[Ruller];  break; }
                    else if ((NewCurSelRect.bottom()    >=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.bottom()    <=MagnetHoriz[Ruller]+HANDLEMAGNETY)) { DY=NewCurSelRect.bottom()    -MagnetHoriz[Ruller];  break; }
                    else if ((NewCurSelRect.center().y()>=MagnetHoriz[Ruller]-HANDLEMAGNETY)&&(NewCurSelRect.center().y()<=MagnetHoriz[Ruller]+HANDLEMAGNETY)) { DY=NewCurSelRect.center().y()-MagnetHoriz[Ruller];  break; }
                }

                Move_X=double(event->pos().x()-DX-CapturePos.x())/SceneRect.width();
                Move_Y=double(event->pos().y()-DY-CapturePos.y())/SceneRect.height();
            }
        }
        if ((Move_X!=0)||(Move_Y!=0)||(Scale_X!=0)||(Scale_Y!=0)) {
            RefreshDisplay();
            if (NbrSelected==1) emit DisplayTransformBlock(Move_X,Move_Y,Scale_X,Scale_Y,Sel_X,Sel_Y,Sel_W,Sel_H);
        }
    }
}

//====================================================================================================================

void cInteractiveZone::mouseDoubleClickEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::mouseDoubleClickEvent");
    if ((!BlockTable)||(!BlockTable->CompositionList)) return;

    if (event->button()==Qt::LeftButton) {
        if (!((NbrSelected==1)&&(IsInSelectedRect(event->pos())))) {
            if (NbrSelected>=0) BlockTable->clearSelection();
            // Try to select another block
            int i=BlockTable->CompositionList->List.count()-1;
            while (i>=0) {
                if (BlockTable->CompositionList->List[i]->IsVisible) {
                    QRect ObjRect=QRect(BlockTable->CompositionList->List[i]->x*SceneRect.width(),BlockTable->CompositionList->List[i]->y*SceneRect.height(),
                                        BlockTable->CompositionList->List[i]->w*SceneRect.width(),BlockTable->CompositionList->List[i]->h*SceneRect.height());
                    if (IsInRect(event->pos(),ObjRect)) {
                        BlockTable->clearSelection();
                        BlockTable->setCurrentCell(i,0,QItemSelectionModel::Current|QItemSelectionModel::Select);
                        break;
                    }
                }
                i--;
            }
            UpdateIsSelected();
        }
        if ((NbrSelected==1)&&(IsInSelectedRect(event->pos()))) QTimer::singleShot(250,this,SLOT(DifferedEmitDoubleClickEvent()));    // Append " emit DoubleClickEvent" to the message queue
    }
}

//====================================================================================================================

void cInteractiveZone::mousePressEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::mousePressEvent");
    if ((!BlockTable)||(!BlockTable->CompositionList)) return;

    ManageCursor(event->pos(),event->modifiers());
    emit StartSelectionChange();
    setFocus();
    if (event->button()==Qt::RightButton) {

        // Reset selection if no block or only one is selected
        if ((!((NbrSelected>0)&&(IsInSelectedRect(event->pos()))))&&(NbrSelected<2)) {
            BlockTable->clearSelection();
            // Try to select another block
            int i=BlockTable->CompositionList->List.count()-1;
            while (i>=0) {
                if (BlockTable->CompositionList->List[i]->IsVisible) {
                    QRect ObjRect=QRect(BlockTable->CompositionList->List[i]->x*SceneRect.width(),BlockTable->CompositionList->List[i]->y*SceneRect.height(),
                                        BlockTable->CompositionList->List[i]->w*SceneRect.width(),BlockTable->CompositionList->List[i]->h*SceneRect.height());
                    if (IsInRect(event->pos(),ObjRect)) {
                        BlockTable->clearSelection();
                        BlockTable->setCurrentCell(i,0,QItemSelectionModel::Current|QItemSelectionModel::Select);
                        break;
                    }
                }
                i--;
            }
            UpdateIsSelected();
        }
        QTimer::singleShot(250,this,SLOT(DifferedEmitRightClickEvent()));    // Append " emit RightClickEvent" to the message queue

    } if (event->button()==Qt::LeftButton) {

        TransfoType=NOTYETDEFINED;

        if ((event->modifiers()==Qt::ControlModifier)||(event->modifiers()==Qt::ShiftModifier)) {

            // Try to toggle block to a multi block selection (one click to add, new click to remove)
            int i=BlockTable->CompositionList->List.count()-1;
            while (i>=0) {
                if (BlockTable->CompositionList->List[i]->IsVisible) {
                    QRect ObjRect=QRect(BlockTable->CompositionList->List[i]->x*SceneRect.width(),BlockTable->CompositionList->List[i]->y*SceneRect.height(),
                                        BlockTable->CompositionList->List[i]->w*SceneRect.width(),BlockTable->CompositionList->List[i]->h*SceneRect.height());
                    if (IsInRect(event->pos(),ObjRect)) {
                        IsSelected[i]=!IsSelected[i];
                        break;
                    }
                }
                i--;
            }

            int FirstSelected=-1;
            for (i=0;i<BlockTable->CompositionList->List.count();i++) if (IsSelected[i]) {
                FirstSelected=i;
                break;
            }
            BlockTable->clearSelection();
            if (FirstSelected==-1) {
                BlockTable->setCurrentCell(i,0,QItemSelectionModel::Current|QItemSelectionModel::Deselect);
            } else {
                BlockTable->setCurrentCell(i,0,QItemSelectionModel::Current|QItemSelectionModel::Select);
                for (i=0;i<BlockTable->CompositionList->List.count();i++) if ((IsSelected[i])&&(i!=FirstSelected))
                    BlockTable->setCurrentCell(i,0,QItemSelectionModel::Select);
            }
        } else if (event->modifiers()==(Qt::ShiftModifier|Qt::ControlModifier)) {

            // Try to select a block under current selected block
            if (NbrSelected==1) {
                int i=IsSelected.count()-1;
                while ((i>=0)&&(!IsSelected[i])) i--;   // Find current selected block
                for (int j=i-1;j>=0;j--) if (BlockTable->CompositionList->List[j]->IsVisible) {
                    QRect ObjRect=QRect(BlockTable->CompositionList->List[j]->x*SceneRect.width(),BlockTable->CompositionList->List[j]->y*SceneRect.height(),
                                        BlockTable->CompositionList->List[j]->w*SceneRect.width(),BlockTable->CompositionList->List[j]->h*SceneRect.height());
                    if (IsInRect(event->pos(),ObjRect)) {
                        BlockTable->clearSelection();
                        BlockTable->setCurrentCell(j,0,QItemSelectionModel::Current|QItemSelectionModel::Select);
                        break;
                    }
                }
            }

        } else if (event->modifiers()==Qt::NoModifier) {
            // Resize
            if ((!SelectionHaveLockBlock)&&(NbrSelected>0)&&(IsInRect(event->pos(),QRect(CurSelRect.left()-HANDLESIZEX/2, CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))                           TransfoType=RESIZEDOWNLEFT; // Bottom left
            else if ((!SelectionHaveLockBlock)&&(NbrSelected>0)&&(IsInRect(event->pos(),QRect(CurSelRect.left()-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))                          TransfoType=RESIZEUPLEFT;   // Top left
            else if ((!SelectionHaveLockBlock)&&(NbrSelected>0)&&(IsInRect(event->pos(),QRect(CurSelRect.left()-HANDLESIZEX/2, CurSelRect.top()+CurSelRect.height()/2-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))   TransfoType=RESIZELEFT;     // Left
            else if ((!SelectionHaveLockBlock)&&(NbrSelected>0)&&(IsInRect(event->pos(),QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))                         TransfoType=RESIZEUPRIGHT;  // Top right
            else if ((!SelectionHaveLockBlock)&&(NbrSelected>0)&&(IsInRect(event->pos(),QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.top()+CurSelRect.height()/2-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))   TransfoType=RESIZERIGHT;    // Right
            else if ((!SelectionHaveLockBlock)&&(NbrSelected>0)&&(IsInRect(event->pos(),QRect(CurSelRect.right()-HANDLESIZEX/2,CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))                      TransfoType=RESIZEDOWNRIGHT;// Bottom right
            else if ((!SelectionHaveLockBlock)&&(NbrSelected>0)&&(IsInRect(event->pos(),QRect(CurSelRect.left()+CurSelRect.width()/2-HANDLESIZEX/2,CurSelRect.top()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))     TransfoType=RESIZEUP;       // Top
            else if ((!SelectionHaveLockBlock)&&(NbrSelected>0)&&(IsInRect(event->pos(),QRect(CurSelRect.left()+CurSelRect.width()/2-HANDLESIZEX/2,CurSelRect.bottom()-HANDLESIZEY/2,HANDLESIZEX,HANDLESIZEY))))  TransfoType=RESIZEDOWN;     // Bottom
            else {
                // Move
                if ((NbrSelected==0)||(!IsInSelectedRect(event->pos()))) {

                    // Replace current selection
                    BlockTable->clearSelection();
                    int i=BlockTable->CompositionList->List.count()-1;
                    while (i>=0) {
                        if (BlockTable->CompositionList->List[i]->IsVisible) {
                            QRect ObjRect=QRect(BlockTable->CompositionList->List[i]->x*SceneRect.width(),BlockTable->CompositionList->List[i]->y*SceneRect.height(),
                                                BlockTable->CompositionList->List[i]->w*SceneRect.width(),BlockTable->CompositionList->List[i]->h*SceneRect.height());
                            if (IsInRect(event->pos(),ObjRect)) {
                                BlockTable->clearSelection();
                                BlockTable->setCurrentCell(i,0,QItemSelectionModel::Current|QItemSelectionModel::Select);
                                break;
                            }
                        }
                        i--;
                    }
                } else if ((IsInSelectedRect(event->pos()))&&(!SelectionHaveLockBlock)) {
                    TransfoType=MOVEBLOCK;
                    setCursor(Qt::ClosedHandCursor);
                }
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
    emit EndSelectionChange();
}

//====================================================================================================================

void cInteractiveZone::mouseReleaseEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cInteractiveZone::mouseReleaseEvent");
    if ((!BlockTable)||(!BlockTable->CompositionList)||(!IsCapture)) return;

    IsCapture=false;

    // Block move
    if ((!SelectionHaveLockBlock)&&((Move_X!=0)||(Move_Y!=0)||(Scale_X!=0)||(Scale_Y!=0))) emit TransformBlock(Move_X,Move_Y,Scale_X,Scale_Y,Sel_X,Sel_Y,Sel_W,Sel_H);
    ManageCursor(event->pos(),event->modifiers());
}
