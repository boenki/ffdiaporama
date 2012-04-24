/* ======================================================================
    This file is part of ffGlobalMainWindow->Diaporama
    ffGlobalMainWindow->Diaporama is a tools to make GlobalMainWindow->Diaporama as video
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

#include "wgt_QCustomThumbnails.h"
#include "cCustomTableWidget.h"
#include "mainwindow.h"

//======================================
// Specific defines for this dialog box
//======================================
#define TransitionSize          36
#define WidgetSelection_Color   Qt::blue
#define WidgetDrag_Color        Qt::red
#define WidgetBackground_Color  0xC2C7CB
#define WidgetBorder_Color      Qt::black
#define ObjectBackground_Ruller Qt::darkGray
#define FirstMusic_Color        Qt::green
#define SecondMusic_Color       Qt::darkGreen
#define FirstSound_Color        Qt::cyan
#define SecondSound_Color       Qt::darkCyan
#define DotLine_Color           Qt::white

// Icons
#define ICON_SHOTPRESENCE                   ":/img/TimelineShots.png"               // FileName of icon representing shots in the timeline
#define ICON_BLOCKPRESENCE                  ":/img/TimelineBlocks.png"              // FileName of icon representing blocks in the timeline
#define ICON_HAVEFILTER                     ":/img/Transform.png"                   // FileName of icon representing block with filter in the timeline
#define ICON_PLAYERPAUSE                    ":/img/player_pause.png"                // FileName of pause icon

// Composition parameters
#define SCALINGTEXTFACTOR                   400

//===========================================================================================================================

wgt_QCustomThumbnails::wgt_QCustomThumbnails(QTableWidget *TheTimeline,int TheType) : QLabel(TheTimeline) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QCustomThumbnails::wgt_QCustomThumbnails");
    Timeline            =TheTimeline;
    Type                =TheType;
    HasBackGTransition  =false;
    BackGTransitionRect =QRect(0,0,0,0);
    HasTransition       =false;
    TransitionRect      =QRect(0,0,0,0);
    HasSoundTrack       =false;
    SoundTrackRect      =QRect(0,0,0,0);
    BackgroundRect      =QRect(0,0,0,0);
    MediaObjectRect     =QRect(0,0,0,0);
    MusicTrackRect      =QRect(0,0,0,0);

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

//===========================================================================================================================

wgt_QCustomThumbnails::~wgt_QCustomThumbnails() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QCustomThumbnails::wgt_QCustomThumbnails");
}

//===========================================================================================================================
// Double click handler : emit edit signal depending on mouse position
//===========================================================================================================================

void wgt_QCustomThumbnails::mouseDoubleClickEvent(QMouseEvent *Event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QCustomThumbnails::mouseDoubleClickEvent");
    int XPos=mapFromGlobal(QPoint(Event->globalX(),Event->globalY())).x();
    int YPos=mapFromGlobal(QPoint(Event->globalX(),Event->globalY())).y();

    if ((HasBackGTransition)&&(XPos>=BackGTransitionRect.left())&&(XPos<=BackGTransitionRect.right())&&(YPos>=BackGTransitionRect.top())&&(YPos<=BackGTransitionRect.bottom())) emit EditBackGTransition();
    else if ((HasTransition)&&(XPos>=TransitionRect.left())&&(XPos<=TransitionRect.right())&&(YPos>=TransitionRect.top())&&(YPos<=TransitionRect.bottom()))                     emit EditTransition();
    else if ((HasSoundTrack)&&(XPos>=SoundTrackRect.left())&&(XPos<=SoundTrackRect.right())&&(YPos>=SoundTrackRect.top())&&(YPos<=SoundTrackRect.bottom()))                     emit EditSoundTrack();
    else if ((XPos>=BackgroundRect.left())&&(XPos<=BackgroundRect.right())&&(YPos>=BackgroundRect.top())&&(YPos<=BackgroundRect.bottom()))                                      emit EditBackground();
    else if ((XPos>=MediaObjectRect.left())&&(XPos<=MediaObjectRect.right())&&(YPos>=MediaObjectRect.top())&&(YPos<=MediaObjectRect.bottom()))                                  emit EditMediaObject();
    else if ((XPos>=MusicTrackRect.left())&&(XPos<=MusicTrackRect.right())&&(YPos>=MusicTrackRect.top())&&(YPos<=MusicTrackRect.bottom()))                                      emit EditMusicTrack();
}

//===========================================================================================================================

void wgt_QCustomThumbnails::paintEvent(QPaintEvent *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QCustomThumbnails::paintEvent");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QPainter Painter(this);
    Painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);

    Painter.save();
    cCustomTableWidget  *Timeline=(cCustomTableWidget *)this->Timeline;

    // Calc diaporama object number
    int Col=0;
    if (GlobalMainWindow->ApplicationConfig->PartitionMode) {
        int Max=Timeline->NbrItem();
        int NbrCol=Timeline->columnCount();
        while ((Col<Max)&&(Timeline->cellWidget((Col/NbrCol),Col-(Col/NbrCol)*NbrCol)!=this)) Col++;
    } else while ((Col<Timeline->columnCount())&&(Timeline->cellWidget(0,Col)!=this)) Col++;

    cDiaporamaObject    *Object         = (Col<GlobalMainWindow->Diaporama->List.count())?GlobalMainWindow->Diaporama->List[Col]:NULL;

    int                 TimelineHeight  = GlobalMainWindow->ApplicationConfig->TimelineHeight;
    bool                IsTransition    = (Object!=NULL)&&((Object->TransitionFamilly!=0)||(Object->TransitionSubType!=0));
    double              Width           = double(this->width());
    double              Height          = double(GlobalMainWindow->ApplicationConfig->TimelineHeight/2+    // Background
                                                GlobalMainWindow->ApplicationConfig->TimelineHeight+      // Montage
                                                TIMELINESOUNDHEIGHT*2); //this->height());
    QPointF             Table[10];

    // Draw background widget
    QPen    Pen;
    Pen.setWidth(1);
    Pen.setStyle(Qt::SolidLine);
    Pen.setColor(WidgetBorder_Color);
    Painter.setPen(Pen);
    Painter.setBrush(QBrush(QColor(WidgetBackground_Color)));
    Painter.drawRect(-1,0,Width+2,Height-1);

    if (Object) {

        //==========================================================================================================================
        // Track BACKGROUND
        //==========================================================================================================================
        Pen.setColor(ObjectBackground_Ruller);
        Painter.setPen(Pen);
        Painter.setBrush(QBrush(QColor(ObjectBackground_Ruller)));
        Painter.drawRect(-1,(TimelineHeight/2-16)/2,Width+2,16);

        int BackThumbHeight = TimelineHeight/2-6;
        int BackThumbWidth  = GlobalMainWindow->Diaporama->GetWidthForHeight(BackThumbHeight);
        Object->Parent->PrepareBackground(Col,BackThumbWidth,BackThumbHeight,&Painter,TransitionSize+3,2);  // Draw Thumb
        DrawThumbnailsBox(TransitionSize+3,2,BackThumbWidth,BackThumbHeight,Painter,NULL);
        BackgroundRect=QRect(TransitionSize+3,2,BackThumbWidth,BackThumbHeight);

        //==========================================================================================================================
        // Draw background transition box & icon
        //==========================================================================================================================
        if (Object->BackgroundType) {
            HasBackGTransition =true;
            BackGTransitionRect=QRect(0,0,TransitionSize,TransitionSize);
            DrawThumbnailsBox(2,(TimelineHeight/2-32)/2,32,32,Painter,IsTransition?IconList.GetIcon(0,1):IconList.GetIcon(0,0));
        }

        //==========================================================================================================================
        // Draw object transition box & icon
        //==========================================================================================================================
        HasTransition =true;
        TransitionRect=QRect(0,TimelineHeight/2,TransitionSize,TransitionSize);
        DrawThumbnailsBox(2,TimelineHeight/2+2-1,32,32,Painter,IsTransition?IconList.GetIcon(GlobalMainWindow->Diaporama->List[Col]->TransitionFamilly,GlobalMainWindow->Diaporama->List[Col]->TransitionSubType):IconList.GetIcon(0,0));


        //==========================================================================================================================
        // Track OBJECTSEQUENCE
        //==========================================================================================================================
        int     ThumbHeight         = Height-TimelineHeight/2-TIMELINESOUNDHEIGHT*2-5;
        int     ThumbWidth          = GlobalMainWindow->Diaporama->GetWidthForHeight(ThumbHeight);
        int     NewThumbHeight      = ThumbHeight-TIMELINESOUNDHEIGHT-2;
        int     NewThumbWidth       = GlobalMainWindow->Diaporama->GetWidthForHeight(NewThumbHeight);
        int     BarWidth            = (ThumbWidth-NewThumbWidth)/2;
        bool    HaveSound           = false;
        double  SoundVolume         = 0;
        bool    PreviousHaveSound   = false;
        double  PreviousSoundVolume = 0;
        bool    HaveFilter          =false;

        // Search it at least one block have filter
        for (int obj=0;obj<Object->ObjectComposition.List.count();obj++)
            if (((Object->ObjectComposition.List[obj]->BackgroundBrush->Image)&&((Object->ObjectComposition.List[obj]->BackgroundBrush->Image->BrushFileTransform.HaveFilter())))||
                ((Object->ObjectComposition.List[obj]->BackgroundBrush->Video)&&((Object->ObjectComposition.List[obj]->BackgroundBrush->Video->BrushFileTransform.HaveFilter())))) HaveFilter=true;

        // Parse current ObjectComposition table to determine if slide have sound
        for (int i=0;i<Object->ObjectComposition.List.count();i++) if ((Object->ObjectComposition.List[i]->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&
            (Object->ObjectComposition.List[i]->BackgroundBrush->Video)) {
            HaveSound=true;
            // Parse all object from all shot to determine max volume
            for (int v=0;v<Object->List.count();v++) for (int w=0;w<Object->List[v]->ShotComposition.List.count();w++)
                if ((Object->List[v]->ShotComposition.List[w]->IndexKey==Object->ObjectComposition.List[i]->IndexKey)&&
                    (Object->List[v]->ShotComposition.List[w]->BackgroundBrush->SoundVolume>SoundVolume))
                        SoundVolume=Object->List[v]->ShotComposition.List[w]->BackgroundBrush->SoundVolume;
        }

        // Parse previous object.ObjectComposition table to determine if previous slide have sound
        if (Col>0) {
            for (int i=0;i<GlobalMainWindow->Diaporama->List[Col-1]->ObjectComposition.List.count();i++)
                    if ((GlobalMainWindow->Diaporama->List[Col-1]->ObjectComposition.List[i]->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&
                        (GlobalMainWindow->Diaporama->List[Col-1]->ObjectComposition.List[i]->BackgroundBrush->Video)&&
                        (GlobalMainWindow->Diaporama->List[Col-1]->ObjectComposition.List[i]->BackgroundBrush->SoundVolume!=0)) {

                PreviousHaveSound=true;
                // Parse all object from all shot to determine max volume
                for (int v=0;v<GlobalMainWindow->Diaporama->List[Col-1]->List.count();v++) for (int w=0;w<GlobalMainWindow->Diaporama->List[Col-1]->List[v]->ShotComposition.List.count();w++)
                    if ((GlobalMainWindow->Diaporama->List[Col-1]->List[v]->ShotComposition.List[w]->IndexKey==GlobalMainWindow->Diaporama->List[Col-1]->ObjectComposition.List[i]->IndexKey)&&
                        (GlobalMainWindow->Diaporama->List[Col-1]->List[v]->ShotComposition.List[w]->BackgroundBrush->SoundVolume>PreviousSoundVolume))
                            PreviousSoundVolume=GlobalMainWindow->Diaporama->List[Col-1]->List[v]->ShotComposition.List[w]->BackgroundBrush->SoundVolume;
            }
        }

        // Draw transition out of previous track
        if (IsTransition && PreviousHaveSound && !HaveSound) {
            Pen.setColor(ObjectBackground_Ruller);
            Pen.setWidth(1);
            Pen.setStyle(Qt::SolidLine);
            Painter.setPen(Pen);
            Painter.setBrush(QBrush(QColor(ObjectBackground_Ruller)));
            Table[0]=QPointF(0,Height-TIMELINESOUNDHEIGHT*2-TIMELINESOUNDHEIGHT-2);
            Table[1]=QPointF(TransitionSize,Height-TIMELINESOUNDHEIGHT*2-2);
            Table[2]=QPointF(0,Height-TIMELINESOUNDHEIGHT*2-2);
            Painter.drawPolygon(Table,3);
            // Draw transition out for this (previous) soundtrack
            Pen.setColor(((Col&0x1)!=0x1)?FirstSound_Color:SecondSound_Color);
            Pen.setWidth(1);
            Pen.setStyle(Qt::SolidLine);
            Painter.setPen(Pen);
            Painter.setBrush(QBrush(QColor(((Col&0x1)!=0x1)?FirstSound_Color:SecondSound_Color)));
            int RHeightPrevious=int(double(TIMELINESOUNDHEIGHT)*(PreviousSoundVolume/1.5));
            Table[0]=QPointF(0,Height-TIMELINESOUNDHEIGHT*2-RHeightPrevious-2);
            Table[1]=QPointF(TransitionSize,Height-TIMELINESOUNDHEIGHT*2-2);
            Table[2]=QPointF(0,Height-TIMELINESOUNDHEIGHT*2-2);
            Painter.drawPolygon(Table,3);
        }

        if (!HaveSound) {
            // Display a thumb with no sound
            Painter.fillRect(TransitionSize+3,TimelineHeight/2+2-1,ThumbWidth,ThumbHeight,Transparent);
            Object->DrawThumbnail(ThumbWidth+2,ThumbHeight+2,&Painter,TransitionSize+3,TimelineHeight/2+2-1);   // Draw Thumb
            if (Object->ObjectComposition.List.count()>1)   Painter.drawImage(TransitionSize+3+8,2-1+ThumbHeight-32,QImage(ICON_BLOCKPRESENCE));                // Add mark if multiple block
            if (Object->List.count()>1)                     Painter.drawImage(TransitionSize+3+ThumbWidth-32,2-1+ThumbHeight-32,QImage(ICON_SHOTPRESENCE));     // Add mark if multiple shot
            if (HaveFilter)                                 Painter.drawImage(TransitionSize+3+ThumbWidth-32,2-1+ThumbHeight-32+24,QImage(ICON_HAVEFILTER));    // Add mark if at least one block have filter
            DrawThumbnailsBox(TransitionSize+3,TimelineHeight/2+2-1,ThumbWidth,ThumbHeight,Painter,NULL);
            MediaObjectRect=QRect(TransitionSize+3,TimelineHeight/2+2-1,ThumbWidth,ThumbHeight);

        } else {
            // Display a thumb with sound track

            int     H3          =NewThumbHeight/5;
            int     HH3         =(NewThumbHeight-H3*3)/4;
            int     RHeight     =int(double(TIMELINESOUNDHEIGHT)*(SoundVolume/1.5));

            // Draw thumb part
            Painter.fillRect(TransitionSize+3,TimelineHeight/2+2-1,ThumbWidth,NewThumbHeight,Transparent);
            Object->DrawThumbnail(NewThumbWidth+2,NewThumbHeight+4,&Painter,TransitionSize+3+BarWidth-1,TimelineHeight/2-1);   // Draw Thumb

            QPen Pen;
            Pen.setWidth(1);
            Painter.setBrush(Qt::NoBrush);
            Pen.setColor(Qt::black);    Painter.setPen(Pen);    Painter.drawRect(TransitionSize+3-2,TimelineHeight/2+1-2,ThumbWidth+4,NewThumbHeight+4);
            Pen.setColor(Qt::darkGray); Painter.setPen(Pen);    Painter.drawRect(TransitionSize+3-1,TimelineHeight/2+1-1,ThumbWidth+2,NewThumbHeight+2);
            Pen.setColor(Qt::white);    Painter.setPen(Pen);    Painter.drawRect(TransitionSize+3,  TimelineHeight/2+1,  ThumbWidth,  NewThumbHeight);
            Painter.drawRect(TransitionSize+3+BarWidth-1,TimelineHeight/2+1,NewThumbWidth+1,NewThumbHeight);

            MediaObjectRect=QRect(TransitionSize+3,         TimelineHeight/2+2-1,   ThumbWidth,     NewThumbHeight);

            // Draw black bar for cinema decoration at left & right
            Painter.fillRect(TransitionSize+3,                              TimelineHeight/2+2,   BarWidth-2,                             NewThumbHeight-1,QBrush(Qt::black));
            Painter.fillRect(TransitionSize+3+BarWidth+NewThumbWidth+2-1,   TimelineHeight/2+2,   ThumbWidth-NewThumbWidth-BarWidth-2+1,  NewThumbHeight-1,QBrush(Qt::black));

            // Draw cinema decoration at left & right
            int YPos=Height-TimelineHeight/2-TIMELINESOUNDHEIGHT*2-(ThumbHeight-2)/2;
            for (int HH=0;HH<3;HH++) {
                Painter.fillRect(TransitionSize+3+4,                            YPos+HH3+(H3+HH3)*HH-1, BarWidth-2-8,   H3,QBrush(Qt::lightGray));
                Painter.fillRect(TransitionSize+3+BarWidth+NewThumbWidth+2+4,   YPos+HH3+(H3+HH3)*HH-1, BarWidth-2-8,   H3,QBrush(Qt::lightGray));
            }
            if (Object->ObjectComposition.List.count()>1)   Painter.drawImage(TransitionSize+3,TimelineHeight/2+2-1+NewThumbHeight-48-4,QImage(ICON_BLOCKPRESENCE));                   // Add mark if multiple block
            if (Object->List.count()>1)                     Painter.drawImage(TransitionSize+3+BarWidth*2+NewThumbWidth-24-2,TimelineHeight/2+2-1+NewThumbHeight-48-4,QImage(ICON_SHOTPRESENCE));     // Add mark if multiple shot
            if (HaveFilter)                                 Painter.drawImage(TransitionSize+3+BarWidth*2+NewThumbWidth-24-2,TimelineHeight/2+2-1+NewThumbHeight-24-4,QImage(ICON_HAVEFILTER));    // Add mark if at least one block have filter

            // Draw background for soundtrack
            Pen.setColor(ObjectBackground_Ruller);
            Pen.setWidth(1);
            Pen.setStyle(Qt::SolidLine);
            Painter.setPen(Pen);
            Painter.setBrush(QBrush(QColor(ObjectBackground_Ruller)));
            Painter.drawRect(0,Height-2-TIMELINESOUNDHEIGHT*2-TIMELINESOUNDHEIGHT,Width,TIMELINESOUNDHEIGHT);
            SoundTrackRect=QRect(0,Height-2-TIMELINESOUNDHEIGHT*2-TIMELINESOUNDHEIGHT,Width,TIMELINESOUNDHEIGHT);
            HasSoundTrack =true;

            // Draw transitions
            if (IsTransition) {
                // Draw transition out for previous soundtrack
                if ((Col>0)&&(PreviousHaveSound)) {
                    Pen.setColor(((Col&0x1)!=0x1)?FirstSound_Color:SecondSound_Color);
                    Pen.setWidth(1);
                    Pen.setStyle(Qt::SolidLine);
                    Painter.setPen(Pen);
                    Painter.setBrush(QBrush(QColor(((Col&0x1)!=0x1)?FirstSound_Color:SecondSound_Color)));
                    int RHeightPrevious=int(double(TIMELINESOUNDHEIGHT)*(PreviousSoundVolume/1.5));
                    Table[0]=QPointF(0,Height-2-RHeightPrevious-TIMELINESOUNDHEIGHT*2);
                    Table[1]=QPointF(TransitionSize,Height-2-TIMELINESOUNDHEIGHT*2);
                    Table[2]=QPointF(0,Height-2-TIMELINESOUNDHEIGHT*2);
                    Painter.drawPolygon(Table,3);
                }
                // Draw in transition + soundtrack
                Pen.setColor(((Col&0x1)==0x1)?FirstSound_Color:SecondSound_Color);
                Pen.setWidth(1);
                Pen.setStyle(Qt::SolidLine);
                Painter.setPen(Pen);
                Painter.setBrush(QBrush(QColor(((Col&0x1)==0x1)?FirstSound_Color:SecondSound_Color)));
                Table[0]=QPointF(0,Height-2-TIMELINESOUNDHEIGHT*2);
                Table[1]=QPointF(TransitionSize,Height-2-RHeight-TIMELINESOUNDHEIGHT*2);
                Table[2]=QPointF(Width,Height-2-RHeight-TIMELINESOUNDHEIGHT*2);                                            // Draw soundtrack without transition
                Table[3]=QPointF(Width,Height-2-TIMELINESOUNDHEIGHT*2);
                Painter.drawPolygon(Table,4);
            } else {
                Pen.setColor(((Col&0x1)==0x1)?FirstSound_Color:SecondSound_Color);
                Pen.setWidth(1);
                Pen.setStyle(Qt::SolidLine);
                Painter.setPen(Pen);
                Painter.setBrush(QBrush(QColor(((Col&0x1)==0x1)?FirstSound_Color:SecondSound_Color)));
                Table[0]=QPointF(0,Height-2-TIMELINESOUNDHEIGHT*2);
                Table[1]=QPointF(0,Height-2-RHeight-TIMELINESOUNDHEIGHT*2);
                Table[2]=QPointF(Width,Height-2-RHeight-TIMELINESOUNDHEIGHT*2);                                            // Draw soundtrack without transition
                Table[3]=QPointF(Width,Height-2-TIMELINESOUNDHEIGHT*2);
                Painter.drawPolygon(Table,4);
            }
        }

        // Draw transition duration, slide duration and slide name
        QFont font= QApplication::font();
        font.setPointSizeF(double(3500)/double(SCALINGTEXTFACTOR));                  // Scale font
        Painter.setFont(font);
        Pen.setWidth(1);
        Pen.setStyle(Qt::SolidLine);
        QString SlideDuration=QTime(0,0,0,0).addMSecs(Object->GetDuration()).toString("hh:mm:ss.zzz");
        QString FileName=Object->SlideName;
        QString TransitionDuration=QTime(0,0,0,0).addMSecs(Object->GetTransitDuration()).toString("ss.z");
        QString SlideNumber=QString("%1").arg(Col+1);
        // Chapter TAG
        if (Col==0) Painter.drawImage(2,2,QImage(":/img/Chapter.png"));
            else if (Object->StartNewChapter) Painter.drawImage(TransitionSize+3,2,QImage(":/img/Chapter.png"));
        if (TransitionDuration[0]=='0')             TransitionDuration=TransitionDuration.right(TransitionDuration.length()-1);   // Cut first 0
        while (TransitionDuration.endsWith("0"))    TransitionDuration=TransitionDuration.left(TransitionDuration.length()-1);
        while (TransitionDuration.endsWith("."))    TransitionDuration=TransitionDuration.left(TransitionDuration.length()-1);

        if (Object->TypeObject==DIAPORAMAOBJECTTYPE_VIDEO) {
            Pen.setColor(Qt::black);
            Painter.setPen(Pen);
            Painter.drawText(QRectF(TransitionSize+3+BarWidth+1,TimelineHeight/2+2-1+1,NewThumbWidth,16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(TransitionSize+3+BarWidth+1,TimelineHeight/2+2-1+1+NewThumbHeight-16,NewThumbWidth,16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(2+1,TimelineHeight/2+2-1+34+1,32,16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(BackThumbWidth+2+1,(TimelineHeight/2-16)/2+1,Width-BackThumbWidth-4,16),SlideNumber,Qt::AlignHCenter|Qt::AlignVCenter);

            Pen.setColor(Qt::white);
            Painter.setPen(Pen);
            Painter.drawText(QRectF(TransitionSize+3+BarWidth,TimelineHeight/2+2-1,NewThumbWidth,16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(TransitionSize+3+BarWidth,TimelineHeight/2+2-1+NewThumbHeight-16,NewThumbWidth,16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(2,TimelineHeight/2+2-1+34,32,16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(BackThumbWidth+2,(TimelineHeight/2-16)/2,Width-BackThumbWidth-4,16),SlideNumber,Qt::AlignHCenter|Qt::AlignVCenter);
        } else {
            Pen.setColor(Qt::black);
            Painter.setPen(Pen);
            Painter.drawText(QRectF(TransitionSize+3+1,TimelineHeight/2+2-1+1,ThumbWidth,16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(TransitionSize+3+1,TimelineHeight/2+2-1+1+ThumbHeight-16,ThumbWidth,16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(2+1,TimelineHeight/2+2-1+34+1,32,16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(BackThumbWidth+2+1,(TimelineHeight/2-16)/2+1,Width-BackThumbWidth-4,16),SlideNumber,Qt::AlignHCenter|Qt::AlignVCenter);

            Pen.setColor(Qt::white);
            Painter.setPen(Pen);
            Painter.drawText(QRectF(TransitionSize+3,TimelineHeight/2+2-1,ThumbWidth,16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(TransitionSize+3,TimelineHeight/2+2-1+ThumbHeight-16,ThumbWidth,16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(2,TimelineHeight/2+2-1+34,32,16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter.drawText(QRectF(BackThumbWidth+2,(TimelineHeight/2-16)/2,Width-BackThumbWidth-4,16),SlideNumber,Qt::AlignHCenter|Qt::AlignVCenter);
        }

        //==========================================================================================================================
        // Track OBJECTMUSIC
        //==========================================================================================================================
        Height       -=4;
        MusicTrackRect=QRect(-1,Height-TIMELINESOUNDHEIGHT*2,Width+2,TIMELINESOUNDHEIGHT*2);
        HasSoundTrack =true;

        int         CurrentCountObjet   =0;
        qlonglong   StartPosition       =0;
        qlonglong   NextStartPosition   =0;
        double      CurrentFactor       =Object->MusicPause?0:Object->MusicReduceVolume?Object->MusicReduceFactor:1;
        double      PreviousFactor      =0;

        if ((Col>0)&&(Object->Parent->GetMusicObject(Col-1,StartPosition)!=NULL))
            PreviousFactor=(Object->Parent->List[Col-1]->MusicPause)?0:(Object->Parent->List[Col-1]->MusicReduceVolume)?Object->Parent->List[Col-1]->MusicReduceFactor:1;

        bool        EndMusic            =true;
        bool        DrawVolumeTransition=(PreviousFactor!=CurrentFactor);
        bool        DrawInTransition    =false;
        bool        DrawOutTransition   =false;
        bool        DrawOutCut          =false;
        bool        DrawPause           =false;
        int         RHeight             =int(TIMELINESOUNDHEIGHT*2*(CurrentFactor/1.5));
        int         PHeight             =int(TIMELINESOUNDHEIGHT*2*(PreviousFactor/1.5));

        if (Col>0) {
            cMusicObject *PrevMusique=Object->Parent->GetMusicObject(Col-1,StartPosition);
            if ((PrevMusique)&&((QTime(0,0,0,0).msecsTo(PrevMusique->Duration)-StartPosition)>Object->Parent->List[Col-1]->GetDuration())) DrawOutTransition=true;
        }

        // Calculate wich music will be use for this object and for the next object
        int             OwnerObjectMusic    =0;
        int             OwnerObjectNextMusic=0;
        cMusicObject    *CurMusic           =Object->Parent->GetMusicObject(Col,StartPosition,&CurrentCountObjet,&OwnerObjectMusic);
        cMusicObject    *NextMusic          =NULL;

        if ((Col+1)<Timeline->columnCount()) {
            NextMusic=Object->Parent->GetMusicObject(Col+1,NextStartPosition,NULL,&OwnerObjectNextMusic);
            //if (NextMusic==CurMusic) EndMusic=false;
            if ((OwnerObjectMusic==OwnerObjectNextMusic)&&(CurMusic!=NULL)&&(NextMusic!=NULL)) EndMusic=false;
                else if ((CurMusic)&&((QTime(0,0,0,0).msecsTo(CurMusic->Duration))-StartPosition>=Object->GetDuration())) EndMusic=false;
        } else if (CurMusic) EndMusic=(QTime(0,0,0,0).msecsTo(CurMusic->Duration)-StartPosition)<Object->GetDuration();

        if (CurMusic!=NULL) {
            // Search if sound end during the slide
            if (EndMusic) DrawOutCut=true;

            // Start a new Playlist
            if (Object->MusicType) {
                if (Object->MusicList.count()>0) {
                    // Search if previous slide have music
                    if (((Col>0)&&(Object->Parent->GetMusicObject(Col-1,StartPosition)!=NULL))&&(IsTransition)) DrawInTransition=true;
                    DrawVolumeTransition=false;
                }
            // continue Playlist from a previous object
            } else if (Object->MusicPause) DrawPause=true;
            if (DrawInTransition && IsTransition) {
                // Draw out transition from a previous object
                if (DrawOutTransition) {
                    if ((CurrentCountObjet & 1)!=1) {
                        Painter.setBrush(QBrush(QColor(FirstMusic_Color)));
                        Pen.setColor(FirstMusic_Color);
                    } else {
                        Painter.setBrush(QBrush(QColor(SecondMusic_Color)));
                        Pen.setColor(SecondMusic_Color);
                    }
                    Pen.setWidth(0);
                    Painter.setPen(Pen);
                    Table[0]=QPointF(-1,Height-PHeight+2);
                    Table[1]=QPointF(34,Height+2);
                    Table[2]=QPointF(-1,Height+2);
                    Painter.drawPolygon(Table,3);
                }
                Table[0]=QPointF(-1,Height+2);
                Table[1]=QPointF(34,Height-RHeight+2);
            } else if (DrawVolumeTransition && IsTransition) {
                Table[0]=QPointF(-1,Height-PHeight+2);
                Table[1]=QPointF(34,Height-RHeight+2);
            } else {
                Table[0]=QPointF(-1,Height+2);
                Table[1]=QPointF(-1,Height-RHeight+2);
            }
            if (DrawOutCut) {
                Table[2]=QPointF(Width-34,Height-RHeight+2);
                Table[3]=QPointF(Width-34,Height+2);
            } else {
                Table[2]=QPointF(Width+2,Height-RHeight+2);
                Table[3]=QPointF(Width+2,Height+2);
            }
            Table[4]=QPointF(-1,Height+2);

            if ((CurrentCountObjet & 1)==1) {
                Painter.setBrush(QBrush(QColor(FirstMusic_Color)));
                Pen.setColor(FirstMusic_Color);
            } else {
                Painter.setBrush(QBrush(QColor(SecondMusic_Color)));
                Pen.setColor(SecondMusic_Color);
            }
            Pen.setWidth(0);
            Painter.setPen(Pen);
            Painter.drawPolygon(Table,5);

            if (DrawPause) Painter.drawImage((Width-24-TransitionSize)/2+TransitionSize,Height-24,QImage(ICON_PLAYERPAUSE));
        } else if (DrawOutTransition) {
            // Draw out transition from a previous object
            if ((CurrentCountObjet & 1)!=1) {
                Painter.setBrush(QBrush(QColor(FirstMusic_Color)));
                Pen.setColor(FirstMusic_Color);
            } else {
                Painter.setBrush(QBrush(QColor(SecondMusic_Color)));
                Pen.setColor(SecondMusic_Color);
            }
            Pen.setWidth(0);
            Painter.setPen(Pen);
            Table[0]=QPointF(-1,Height-PHeight+2);
            Table[1]=QPointF(34,Height+2);
            Table[2]=QPointF(-1,Height+2);
            Painter.drawPolygon(Table,3);
        }

        // Draw separated line
        Height=double(GlobalMainWindow->ApplicationConfig->TimelineHeight/2+GlobalMainWindow->ApplicationConfig->TimelineHeight+TIMELINESOUNDHEIGHT*2);
        Pen.setWidth(1);
        Pen.setStyle(Qt::DotLine);
        Pen.setColor(DotLine_Color);
        Painter.setPen(Pen);
        Painter.drawLine(QPointF(TransitionSize+1,this->height()-TIMELINESOUNDHEIGHT*2),QPointF(TransitionSize+1,Height-2));
        Painter.drawLine(QPointF(this->width()-2, this->height()-TIMELINESOUNDHEIGHT*2),QPointF(this->width()-2,Height-2));
    }

    // --------------------------

    Painter.restore();

    // Draw selected box (if needed)
    if (Col==GlobalMainWindow->Diaporama->CurrentCol) {
        Painter.save();
        Pen.setColor(WidgetSelection_Color);
        Pen.setStyle(Qt::SolidLine);
        Pen.setWidth(10);
        Painter.setPen(Pen);
        Painter.setBrush(Qt::NoBrush); //QBrush(QColor(WidgetSelection_Color)));
        Painter.setOpacity(0.5);
        Painter.drawRect(0,0,this->width(),Height);
        Painter.setOpacity(1);
        Painter.restore();
    }
    // Draw Drag & Drop inserting point (if needed)
    if (((GlobalMainWindow->IsDragOn==1)&&(Col!=GlobalMainWindow->DragItemSource)&&((Col!=GlobalMainWindow->DragItemSource+1)||(GlobalMainWindow->DragItemSource!=GlobalMainWindow->Diaporama->List.count()-1))&&(Col<GlobalMainWindow->Diaporama->List.count())&&(
         (Col==GlobalMainWindow->DragItemDest)||((Col==GlobalMainWindow->Diaporama->List.count()-1)&&(GlobalMainWindow->DragItemDest>=GlobalMainWindow->Diaporama->List.count()))))||
         ((GlobalMainWindow->IsDragOn==2)&&((Col==GlobalMainWindow->DragItemDest)||((Col+1==GlobalMainWindow->DragItemDest)&&(Col=GlobalMainWindow->Diaporama->List.count()-1)))))
        {
        Painter.save();
        Pen.setColor(WidgetDrag_Color);
        Pen.setStyle(Qt::SolidLine);
        Pen.setWidth(10);
        Painter.setPen(Pen);
        Painter.setBrush(Qt::NoBrush); //QBrush(QColor(WidgetSelection_Color)));
        Painter.setOpacity(0.5);
        if ((Col==GlobalMainWindow->Diaporama->List.count()-1)&&(GlobalMainWindow->DragItemDest>=GlobalMainWindow->Diaporama->List.count()))
            Painter.drawRect(width(),0,width(),Height); else Painter.drawRect(0,0,0,Height);
        Painter.setOpacity(1);
        Painter.restore();
    }
    QApplication::restoreOverrideCursor();
}

//===========================================================================================================================

void wgt_QCustomThumbnails::DrawThumbnailsBox(int Xa,int Ya,int Width,int Height,QPainter &Painter,QImage *Icon) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_QCustomThumbnails::DrawThumbnailsBox");
    QPen Pen;
    Pen.setWidth(1);
    Pen.setColor(Qt::darkGray);
    Painter.setPen(Pen);
    Painter.setBrush(Qt::NoBrush);
    Painter.drawRect(Xa-1,Ya-1,Width+4-1,Height+4-1);
    Pen.setColor(Qt::white);
    Painter.setPen(Pen);
    Painter.drawRect(Xa-1,Ya-1,Width+2-1,Height+2-1);
    Pen.setColor(Qt::black);
    Painter.setPen(Pen);
    Painter.drawRect(Xa-2,Ya-2,Width+4-1,Height+4-1);

    // -------------------------- if Transition Icon
    if (Icon) {
        Painter.drawImage(QRect(Xa,Ya,32,32),*Icon);
        delete Icon;
    }
}
