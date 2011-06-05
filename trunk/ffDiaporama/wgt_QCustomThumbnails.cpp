/* ======================================================================
    This file is part of ffGlobalMainWindow->Diaporama
    ffGlobalMainWindow->Diaporama is a tools to make GlobalMainWindow->Diaporama as video
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

#include "wgt_QCustomThumbnails.h"
#include "mainwindow.h"

//======================================
// Specific defines for this dialog box
//======================================
#define TransitionSize          36
#define WidgetSelection_Color   Qt::blue
#define WidgetBackground_Color  0xC2C7CB
#define WidgetBorder_Color      Qt::black
#define ObjectBackground_Ruller Qt::darkGray
#define FirstMusic_Color        Qt::green
#define SecondMusic_Color       Qt::darkGreen
#define FirstSound_Color        Qt::cyan
#define SecondSound_Color       Qt::darkCyan
#define DotLine_Color           Qt::white

//===========================================================================================================================

wgt_QCustomThumbnails::wgt_QCustomThumbnails(QTableWidget *TheTimeline,int TheType) : QLabel(TheTimeline) {
    Timeline        =TheTimeline;
    Type            =TheType;
    DiaporamaObject =NULL;
    HasTransition   =false;
    TransitionRect  =QRect(0,0,0,0);
    HasSoundTrack   =false;
    SoundTrackRect  =QRect(0,0,0,0);;
    MediaObjectRect =QRect(0,0,0,0);;

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

//===========================================================================================================================

wgt_QCustomThumbnails::~wgt_QCustomThumbnails() {
}

//===========================================================================================================================
// Double click handler : emit edit signal depending on mouse position
//===========================================================================================================================

void wgt_QCustomThumbnails::mouseDoubleClickEvent(QMouseEvent *Event) {
    int XPos=mapFromGlobal(QPoint(Event->globalX(),Event->globalY())).x();
    int YPos=mapFromGlobal(QPoint(Event->globalX(),Event->globalY())).y();

    if ((HasTransition)&&(XPos>=TransitionRect.left())&&(XPos<=TransitionRect.right())&&(YPos>=TransitionRect.top())&&(YPos<=TransitionRect.bottom()))      emit EditTransition();
    else if ((HasSoundTrack)&&(XPos>=SoundTrackRect.left())&&(XPos<=SoundTrackRect.right())&&(YPos>=SoundTrackRect.top())&&(YPos<=SoundTrackRect.bottom())) emit EditSoundTrack();
    else if ((XPos>=MediaObjectRect.left())&&(XPos<=MediaObjectRect.right())&&(YPos>=MediaObjectRect.top())&&(YPos<=MediaObjectRect.bottom()))              emit EditMediaObject();
}

//===========================================================================================================================

void wgt_QCustomThumbnails::paintEvent(QPaintEvent *) {
    QPainter Painter(this);

    if (Type==THUMBNAILTYPE_SHOT) {

        if (DiaporamaObject) {
            // Draw a standard thumbnail (just the image) at position of the current row
            Painter.save();

            int Col=0;
            int Position = 0;
            while (Col<Timeline->columnCount()) if (Timeline->cellWidget(0,Col)!=this) {
                Position=Position+DiaporamaObject->List[Col].GetStaticDuration();
                Col++;
            } else if (Timeline->cellWidget(0,Col)==this) {
                int Height=Timeline->rowHeight(0);
                int Width =Timeline->columnWidth(Col);
                if (Width!=GlobalMainWindow->Diaporama->GetWidthForHeight(Height)) {
                    Width=GlobalMainWindow->Diaporama->GetWidthForHeight(Height);
                    Timeline->setColumnWidth(Col,Width);
                }

                Painter.fillRect(0,0,Width,Height,Transparent);
                DiaporamaObject->CanvasImageAt(Width,Height,Position,&Painter);

                // -------------------------- Draw selected box (if needed)

                if (Col==Timeline->currentColumn()) {
                    QPen Pen;
                    Pen.setColor(Qt::blue);
                    Pen.setWidth(6);
                    Painter.setPen(Pen);
                    Painter.setBrush(Qt::NoBrush);
                    Painter.drawRect(0,0,this->width()-1,this->height()-1);
                }

                // -------------------------- Draw shot duration
                QPen  Pen;
                QFont font= QApplication::font();
                font.setPointSizeF(double(3500)/double(SCALINGTEXTFACTOR));                  // Scale font
                Painter.setFont(font);
                Pen.setWidth(1);
                Pen.setStyle(Qt::SolidLine);
                QString ShotDuration=QTime(0,0,0,0).addMSecs(DiaporamaObject->List[Col].StaticDuration).toString("hh:mm:ss.zzz");
                Pen.setColor(Qt::black);
                Painter.setPen(Pen);
                Painter.drawText(QRectF(1,4+1,this->width(),this->height()),ShotDuration,Qt::AlignHCenter|Qt::AlignTop);
                Pen.setColor(Qt::white);
                Painter.setPen(Pen);
                Painter.drawText(QRectF(0,4,this->width()-1,this->height()-1),ShotDuration,Qt::AlignHCenter|Qt::AlignTop);

                Col++;
                MediaObjectRect=QRect(0,0,Width,Height);
            }
            Painter.restore();
        }
        //===========================================================================================================================

    } else {
        Painter.save();
        int Col=0;
        while ((Col<Timeline->columnCount())&&(Timeline->cellWidget(TRACKBACKGROUND,Col)!=this)&&(Timeline->cellWidget(TRACKMONTAGE,Col)!=this)&&(Timeline->cellWidget(TRACKMUSIC,Col)!=this)) Col++;
        cDiaporamaObject    *Object         = (Col<Timeline->columnCount())?&GlobalMainWindow->Diaporama->List[Col]:NULL;
        bool                IsTransition    = (Object!=NULL)&&((Object->TransitionFamilly!=0)||(Object->TransitionSubType!=0));
        double              Width           = double(this->width());
        double              Height          = double(this->height());
        int                 ThumbHeight     = Height-5;
        int                 ThumbWidth      = GlobalMainWindow->Diaporama->GetWidthForHeight(ThumbHeight);
        int                 NewThumbHeight  = ThumbHeight-TIMELINESOUNDHEIGHT-2;
        int                 NewThumbWidth   = GlobalMainWindow->Diaporama->GetWidthForHeight(NewThumbHeight);
        int                 BarWidth        = (ThumbWidth-NewThumbWidth)/2;
        QPointF             Table[10];

        // Draw background widget
        QPen    Pen;
        Pen.setWidth(1);
        Pen.setStyle(Qt::SolidLine);
        Pen.setColor(WidgetBorder_Color);
        Painter.setPen(Pen);
        Painter.setBrush(QBrush(QColor(WidgetBackground_Color)));
        Painter.drawRect(-1,(Type!=THUMBNAILTYPE_OBJECTBACKGROUND)?-1:0,Width+2,(Type!=THUMBNAILTYPE_OBJECTBACKGROUND)?Height:Height-1);

        if (Object) {

            if (Type==THUMBNAILTYPE_OBJECTBACKGROUND) {
                // Ruller under thumb
                Pen.setColor(ObjectBackground_Ruller);
                Painter.setPen(Pen);
                Painter.setBrush(QBrush(QColor(ObjectBackground_Ruller)));
                Painter.drawRect(-1,(Height-16)/2,Width+2,16);
            }

            // Draw transition box & icon
            if ((Type==THUMBNAILTYPE_OBJECTSEQUENCE)||((Type==THUMBNAILTYPE_OBJECTBACKGROUND)&&((Object->BackgroundType)))) {
                HasTransition =true;
                TransitionRect=QRect(0,0,TransitionSize,TransitionSize);
                DrawThumbnailsBox(2,(Type==THUMBNAILTYPE_OBJECTBACKGROUND)?(Height-32)/2:2-1,32,32,Painter,IsTransition?(
                        (Type==THUMBNAILTYPE_OBJECTSEQUENCE)?IconList.GetIcon(GlobalMainWindow->Diaporama->List[Col].TransitionFamilly,GlobalMainWindow->Diaporama->List[Col].TransitionSubType):
                        IconList.GetIcon(0,1)
                        ):IconList.GetIcon(0,0));
            }

            if (Type==THUMBNAILTYPE_OBJECTSEQUENCE) {                       // Draw a decorated thumbnail object

                bool    HaveSound   =false;
                double  SoundVolume =0;
                bool    PreviousHaveSound=false;
                double  PreviousSoundVolume=0;

                // Parse current ObjectComposition table to determine if slide have sound
                for (int i=0;i<Object->ObjectComposition.List.count();i++) if ((Object->ObjectComposition.List[i].BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&
                    (Object->ObjectComposition.List[i].BackgroundBrush.Video)&&(Object->ObjectComposition.List[i].BackgroundBrush.SoundVolume!=0)) {
                    HaveSound=true;
                    if (Object->ObjectComposition.List[i].BackgroundBrush.SoundVolume>SoundVolume) SoundVolume=Object->ObjectComposition.List[i].BackgroundBrush.SoundVolume;
                }

                // Parse previous object.ObjectComposition table to determine if prvious slide have sound
                if (Col>0) {
                    for (int i=0;i<GlobalMainWindow->Diaporama->List[Col-1].ObjectComposition.List.count();i++)
                            if ((GlobalMainWindow->Diaporama->List[Col-1].ObjectComposition.List[i].BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK)&&
                                (GlobalMainWindow->Diaporama->List[Col-1].ObjectComposition.List[i].BackgroundBrush.Video)&&
                                (GlobalMainWindow->Diaporama->List[Col-1].ObjectComposition.List[i].BackgroundBrush.SoundVolume!=0)) {

                        PreviousHaveSound=true;
                        if (GlobalMainWindow->Diaporama->List[Col-1].ObjectComposition.List[i].BackgroundBrush.SoundVolume>PreviousSoundVolume)
                            PreviousSoundVolume=GlobalMainWindow->Diaporama->List[Col-1].ObjectComposition.List[i].BackgroundBrush.SoundVolume;
                    }
                }

                // Draw transition out of previous track
                if (IsTransition && PreviousHaveSound && !HaveSound) {
                    Pen.setColor(ObjectBackground_Ruller);
                    Pen.setWidth(1);
                    Pen.setStyle(Qt::SolidLine);
                    Painter.setPen(Pen);
                    Painter.setBrush(QBrush(QColor(ObjectBackground_Ruller)));
                    Table[0]=QPointF(0,Height-TIMELINESOUNDHEIGHT-2);
                    Table[1]=QPointF(TransitionSize,Height-2);
                    Table[2]=QPointF(0,Height-2);
                    Painter.drawPolygon(Table,3);
                    // Draw transition out for this (previous) soundtrack
                    Pen.setColor(((Col&0x1)!=0x1)?FirstSound_Color:SecondSound_Color);
                    Pen.setWidth(1);
                    Pen.setStyle(Qt::SolidLine);
                    Painter.setPen(Pen);
                    Painter.setBrush(QBrush(QColor(((Col&0x1)!=0x1)?FirstSound_Color:SecondSound_Color)));
                    int RHeightPrevious=int(double(TIMELINESOUNDHEIGHT)*(PreviousSoundVolume/1.5));
                    Table[0]=QPointF(0,Height-RHeightPrevious-2);
                    Table[1]=QPointF(TransitionSize,Height-2);
                    Table[2]=QPointF(0,Height-2);
                    Painter.drawPolygon(Table,3);
                }

                if (!HaveSound) {
                    // Display a thumb with no sound
                    Painter.fillRect(TransitionSize+3,2-1,ThumbWidth,ThumbHeight,Transparent);
                    Object->DrawThumbnail(ThumbWidth,ThumbHeight,&Painter,TransitionSize+3,2-1);   // Draw Thumb
                    if (Object->List.count()>1) Painter.drawImage(TransitionSize+3+ThumbWidth-32,2-1+ThumbHeight-32,QImage(ICON_SHOTPRESENCE));
                    DrawThumbnailsBox(TransitionSize+3,2-1,ThumbWidth,ThumbHeight,Painter,NULL);
                    MediaObjectRect=QRect(TransitionSize+3,2-1,ThumbWidth,ThumbHeight);

                } else {
                    // Display a thumb with sound track

                    int H3            =NewThumbHeight/5;
                    int HH3           =(NewThumbHeight-H3*3)/4;
                    int RHeight       =int(double(TIMELINESOUNDHEIGHT)*(SoundVolume/1.5));

                    // Draw thumb part
                    Painter.fillRect(TransitionSize+3,2-1,ThumbWidth,NewThumbHeight,Transparent);
                    Object->DrawThumbnail(NewThumbWidth,NewThumbHeight,&Painter,TransitionSize+3+BarWidth,2-1);   // Draw Thumb
                    if (Object->List.count()>1) Painter.drawImage(TransitionSize+3+BarWidth+NewThumbWidth-32,2-1+NewThumbHeight-32,QImage(ICON_SHOTPRESENCE));
                    DrawThumbnailsBox(TransitionSize+3+BarWidth,2-1,NewThumbWidth,NewThumbHeight,Painter,NULL);
                    DrawThumbnailsBox(TransitionSize+3,2-1,ThumbWidth,NewThumbHeight,Painter,NULL);
                    MediaObjectRect=QRect(TransitionSize+3,2-1,ThumbWidth,NewThumbHeight);

                    // Draw black bar for cinema decoration at left & right
                    Painter.fillRect(TransitionSize+3,2-1,BarWidth-2,NewThumbHeight,QBrush(Qt::black));
                    Painter.fillRect(TransitionSize+3+BarWidth+NewThumbWidth+2,2-1,ThumbWidth-NewThumbWidth-BarWidth-2,NewThumbHeight,QBrush(Qt::black));
                    // Draw cinema decoration at left & right
                    int YPos=(Height-ThumbHeight-2)/2;
                    for (int HH=0;HH<3;HH++) {
                        Painter.fillRect(TransitionSize+3+4,YPos+HH3+(H3+HH3)*HH-1,BarWidth-2-8,H3,QBrush(Qt::lightGray));
                        Painter.fillRect(TransitionSize+3+BarWidth+NewThumbWidth+2+4,YPos+HH3+(H3+HH3)*HH-1,BarWidth-2-8,H3,QBrush(Qt::lightGray));
                    }
                    Height-=2;

                    // Draw background for soundtrack
                    Pen.setColor(ObjectBackground_Ruller);
                    Pen.setWidth(1);
                    Pen.setStyle(Qt::SolidLine);
                    Painter.setPen(Pen);
                    Painter.setBrush(QBrush(QColor(ObjectBackground_Ruller)));
                    Painter.drawRect(0,Height-TIMELINESOUNDHEIGHT,Width,TIMELINESOUNDHEIGHT);
                    SoundTrackRect=QRect(0,Height-TIMELINESOUNDHEIGHT,Width,TIMELINESOUNDHEIGHT);
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
                            Table[0]=QPointF(0,Height-RHeightPrevious);
                            Table[1]=QPointF(TransitionSize,Height);
                            Table[2]=QPointF(0,Height);
                            Painter.drawPolygon(Table,3);
                        }
                        // Draw in transition + soundtrack
                        Pen.setColor(((Col&0x1)==0x1)?FirstSound_Color:SecondSound_Color);
                        Pen.setWidth(1);
                        Pen.setStyle(Qt::SolidLine);
                        Painter.setPen(Pen);
                        Painter.setBrush(QBrush(QColor(((Col&0x1)==0x1)?FirstSound_Color:SecondSound_Color)));
                        Table[0]=QPointF(0,Height);
                        Table[1]=QPointF(TransitionSize,Height-RHeight);
                        Table[2]=QPointF(Width,Height-RHeight);                                            // Draw soundtrack without transition
                        Table[3]=QPointF(Width,Height);
                        Painter.drawPolygon(Table,4);
                    } else {
                        Pen.setColor(((Col&0x1)==0x1)?FirstSound_Color:SecondSound_Color);
                        Pen.setWidth(1);
                        Pen.setStyle(Qt::SolidLine);
                        Painter.setPen(Pen);
                        Painter.setBrush(QBrush(QColor(((Col&0x1)==0x1)?FirstSound_Color:SecondSound_Color)));
                        Table[0]=QPointF(0,Height);
                        Table[1]=QPointF(0,Height-RHeight);
                        Table[2]=QPointF(Width,Height-RHeight);                                            // Draw soundtrack without transition
                        Table[3]=QPointF(Width,Height);
                        Painter.drawPolygon(Table,4);
                    }
                    // Draw separated line
                    Pen.setWidth(1);
                    Pen.setStyle(Qt::DotLine);
                    Pen.setColor(DotLine_Color);
                    Painter.setPen(Pen);
                    Painter.drawLine(QPointF(TransitionSize,Height-TIMELINESOUNDHEIGHT),QPointF(TransitionSize,Height));
                    Painter.drawLine(QPointF(this->width()-1,Height-TIMELINESOUNDHEIGHT),QPointF(this->width()-1,Height));
                }

                // Draw transition duration, slide duration and slide name
                QFont font= QApplication::font();
                font.setPointSizeF(double(3500)/double(SCALINGTEXTFACTOR));                  // Scale font
                Painter.setFont(font);
                Pen.setWidth(1);
                Pen.setStyle(Qt::SolidLine);
                QString SlideDuration=QTime(0,0,0,0).addMSecs(Object->GetDuration()).toString("hh:mm:ss.zzz");
                QString FileName=Object->SlideName;
                QString TransitionDuration=QTime(0,0,0,0).addMSecs(Object->TransitionDuration).toString("ss.zzz");
                TransitionDuration=TransitionDuration.right(TransitionDuration.length()-1);   // Cut first 0

                if (Object->TypeObject==DIAPORAMAOBJECTTYPE_VIDEO) {
                    Pen.setColor(Qt::black);
                    Painter.setPen(Pen);
                    Painter.drawText(QRectF(TransitionSize+3+BarWidth+1,2-1+1,NewThumbWidth,16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
                    Painter.drawText(QRectF(TransitionSize+3+BarWidth+1,2-1+1+NewThumbHeight-16,NewThumbWidth,16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
                    Painter.drawText(QRectF(2+1,2-1+34+1,32,16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);

                    Pen.setColor(Qt::white);
                    Painter.setPen(Pen);
                    Painter.drawText(QRectF(TransitionSize+3+BarWidth,2-1,NewThumbWidth,16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
                    Painter.drawText(QRectF(TransitionSize+3+BarWidth,2-1+NewThumbHeight-16,NewThumbWidth,16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
                    Painter.drawText(QRectF(2,2-1+34,32,16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
                } else {
                    Pen.setColor(Qt::black);
                    Painter.setPen(Pen);
                    Painter.drawText(QRectF(TransitionSize+3+1,2-1+1,ThumbWidth,16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
                    Painter.drawText(QRectF(TransitionSize+3+1,2-1+1+ThumbHeight-16,ThumbWidth,16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
                    Painter.drawText(QRectF(2+1,2-1+34+1,32,16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);

                    Pen.setColor(Qt::white);
                    Painter.setPen(Pen);
                    Painter.drawText(QRectF(TransitionSize+3,2-1,ThumbWidth,16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
                    Painter.drawText(QRectF(TransitionSize+3,2-1+ThumbHeight-16,ThumbWidth,16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
                    Painter.drawText(QRectF(2,2-1+34,32,16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
                }

            } else if (Type==THUMBNAILTYPE_OBJECTBACKGROUND) {              // Draw a decorated thumbnail background

                int         ThumbHeight = Height-6;
                int         ThumbWidth  = GlobalMainWindow->Diaporama->GetWidthForHeight(ThumbHeight);

                Object->Parent->PrepareBackground(Col,ThumbWidth,ThumbHeight,&Painter,TransitionSize+3,2,true);  // Draw Thumb
                DrawThumbnailsBox(TransitionSize+3,2,ThumbWidth,ThumbHeight,Painter,NULL);
                MediaObjectRect=QRect(TransitionSize+3,2,ThumbWidth,ThumbHeight);

                if (Col<Timeline->columnCount()-1) {
                    Pen.setWidth(1);
                    Pen.setStyle(Qt::DotLine);
                    Pen.setColor(DotLine_Color);
                    Painter.setPen(Pen);
                    Painter.drawLine(QPoint(Width-1,2),QPoint(Width-1,Height-2));
                }

            } else if (Type==THUMBNAILTYPE_OBJECTMUSIC) {                   // Draw a decorated thumbnail music
                Height       -=4;
                SoundTrackRect=QRect(-1,2,Width+2,Height);
                HasSoundTrack =true;

                int     CurrentCountObjet   =0;
                int     StartPosition       =0;
                int     NextStartPosition   =0;
                double  CurrentFactor       =Object->MusicPause?0:Object->MusicReduceVolume?Object->MusicReduceFactor:1;
                double  PreviousFactor      =0;
                if ((Col>0)&&(Object->Parent->GetMusicObject(Col-1,StartPosition)!=NULL))
                    PreviousFactor=(Object->Parent->List[Col-1].MusicPause)?0:(Object->Parent->List[Col-1].MusicReduceVolume)?Object->Parent->List[Col-1].MusicReduceFactor:1;

                bool    EndMusic            =true;
                bool    DrawVolumeTransition=(PreviousFactor!=CurrentFactor);
                bool    DrawInTransition    =false;
                bool    DrawOutTransition   =false;
                bool    DrawOutCut          =false;
                bool    DrawPause           =false;
                int     RHeight             =int(Height*(CurrentFactor/1.5));
                int     PHeight             =int(Height*(PreviousFactor/1.5));

                // Calculate wich music will be use for this object and for the next object
                cMusicObject    *CurMusic          =Object->Parent->GetMusicObject(Col,StartPosition,&CurrentCountObjet);
                cMusicObject    *NextMusic         =NULL;

                if ((Col+1)<Timeline->columnCount()) {
                    NextMusic=Object->Parent->GetMusicObject(Col+1,NextStartPosition);
                    if (NextMusic==CurMusic) EndMusic=false;
                    if ((CurMusic)&&((QTime(0,0,0,0).msecsTo(CurMusic->Duration))-StartPosition>=Object->GetDuration())) EndMusic=false;
                } else if (CurMusic) EndMusic=(QTime(0,0,0,0).msecsTo(CurMusic->Duration)-StartPosition)<Object->GetDuration();

                if (CurMusic!=NULL) {
                    // Search if sound end during the slide
                    if ((EndMusic)&&(NextMusic==NULL)) DrawOutCut=true;

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
                        if ((Col>0)&&(Object->Parent->GetMusicObject(Col-1,StartPosition)!=NULL)) {
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
                    if (DrawOutTransition||DrawOutCut) Table[2]=QPointF(Width-34,Height-RHeight+2);   else Table[2]=QPointF(Width+2,Height-RHeight+2);
                    if (DrawOutCut)                    Table[3]=QPointF(Width-34,Height+2);           else Table[3]=QPointF(Width+2,Height+2);
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
                } else {
                    // Draw out transition from a previous object
                    if ((Col>0)&&(Object->Parent->GetMusicObject(Col-1,StartPosition)!=NULL)) {
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
                }

                // Draw separated line
                Pen.setWidth(1);
                Pen.setStyle(Qt::DotLine);
                Pen.setColor(DotLine_Color);
                Painter.setPen(Pen);
                Painter.drawLine(QPointF(TransitionSize,2),QPointF(TransitionSize,Height));
                Painter.drawLine(QPointF(this->width()-1,2),QPointF(this->width()-1,Height));
            }
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
            switch (Type) {
            case THUMBNAILTYPE_OBJECTBACKGROUND :   Painter.drawRect(0,0,this->width(),this->height()+10);       break;
            case THUMBNAILTYPE_OBJECTSEQUENCE   :   Painter.drawRect(0,-10,this->width(),this->height()+20);     break;
            case THUMBNAILTYPE_OBJECTMUSIC      :   Painter.drawRect(0,-10,this->width(),this->height()+10);     break;
            }
            Painter.setOpacity(1);
            Painter.restore();
        }
    }
}

//===========================================================================================================================

void wgt_QCustomThumbnails::DrawThumbnailsBox(int Xa,int Ya,int Width,int Height,QPainter &Painter,QImage *Icon) {
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
