/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

#include "cCustomSlideTable.h"

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QModelIndex>

#include <QHeaderView>
#include <QScrollBar>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>

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

//********************************************************************************************************
// QCustomThumbItemDelegate
//********************************************************************************************************

class QCustomThumbItemDelegate : public QStyledItemDelegate {
//Q_OBJECT
public:
    cCustomSlideTable  *ParentTable;

    explicit QCustomThumbItemDelegate(QObject *parent);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//========================================================================================================================

QCustomThumbItemDelegate::QCustomThumbItemDelegate(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomThumbItemDelegate::QCustomThumbItemDelegate");

    ParentTable=(cCustomSlideTable *)parent;
}

//===========================================================================================================================

void DrawThumbnailsBox(int Xa,int Ya,int Width,int Height,QPainter *Painter,QImage *Icon) {
    QPen Pen;
    Pen.setWidth(1);
    Pen.setColor(Qt::darkGray);
    Painter->setPen(Pen);
    Painter->setBrush(Qt::NoBrush);
    Painter->drawRect(Xa-1,Ya-1,Width+4-1,Height+4-1);
    Pen.setColor(Qt::white);
    Painter->setPen(Pen);
    Painter->drawRect(Xa-1,Ya-1,Width+2-1,Height+2-1);
    Pen.setColor(Qt::black);
    Painter->setPen(Pen);
    Painter->drawRect(Xa-2,Ya-2,Width+4-1,Height+4-1);

    // -------------------------- if Transition Icon
    if (Icon) {
        Painter->drawImage(QRect(Xa,Ya,32,32),*Icon);
        delete Icon;
    }
}

//========================================================================================================================

void QCustomThumbItemDelegate::paint(QPainter *Painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomThumbItemDelegate::paint");

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    Painter->save();

    int ItemIndex = index.row()*ParentTable->columnCount()+index.column();

    if (ItemIndex>=ParentTable->Diaporama->List.count()) {

        Painter->fillRect(option.rect,QColor(Qt::white));

    } else {

        cDiaporamaObject *Object     =ParentTable->Diaporama->List[ItemIndex];
        int              ThumbWidth  =ParentTable->columnWidth(0);
        int              ThumbHeight =ParentTable->rowHeight(0);
        bool             IsTransition=((Object->TransitionFamilly!=0)||(Object->TransitionSubType!=0));
        QPointF          Table[10];

        Painter->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
        Painter->setClipRect(QRectF(option.rect.x(),option.rect.y(),option.rect.width(),option.rect.height()));
        // Fill background
        Painter->fillRect(option.rect,QColor(WidgetBackground_Color));
        // Translate painter (if needed) so all coordinate are from 0,0
        if ((option.rect.x()!=0)||(option.rect.y()!=0)) Painter->translate(option.rect.x(),option.rect.y());

        // Draw slide separation line for Partition mode
        QPen    Pen;
        Pen.setWidth(1);
        Pen.setStyle(Qt::SolidLine);
        Pen.setColor(WidgetBorder_Color);
        Painter->setPen(Pen);
        Painter->drawLine(0,ThumbHeight-1,ThumbWidth-1,ThumbHeight-1);

        //==========================================================================================================================
        // Track BACKGROUND (first 1/4 height of the slide)
        //==========================================================================================================================

        int BackThumbHeight = ThumbHeight/4-6;
        int BackThumbWidth  = ParentTable->Diaporama->GetWidthForHeight(BackThumbHeight);

        // Fill background track
        Painter->fillRect(QRect(0,(ThumbHeight/4-16)/2,ThumbWidth,16),QColor(ObjectBackground_Ruller));

        // Draw thumb
        ParentTable->Diaporama->PrepareBackground(ItemIndex,BackThumbWidth,BackThumbHeight,Painter,TransitionSize+3,2);

        // Draw frame arround thumb
        DrawThumbnailsBox(TransitionSize+3,2,BackThumbWidth,BackThumbHeight,Painter,NULL);

        // Draw background transition box & icon (if transition exist)
        if (Object->BackgroundType) DrawThumbnailsBox(2,(ThumbHeight/4-32)/2-1,32,32,Painter,IsTransition?IconList.GetIcon(0,1):IconList.GetIcon(0,0));


        //==========================================================================================================================
        // Track OBJECT (second and third 1/4 height of the slide)
        //==========================================================================================================================
        DrawThumbnailsBox(2,ThumbHeight/4+2-1,32,32,Painter,IsTransition?IconList.GetIcon(Object->TransitionFamilly,Object->TransitionSubType):IconList.GetIcon(0,0));

        int     NewThumbWidth       = ThumbWidth-TransitionSize-6;
        int     NewThumbHeight      = ParentTable->Diaporama->GetHeightForWidth(NewThumbWidth);
        int     BarWidth            = (ThumbWidth-NewThumbWidth)/2;
        bool    HaveSound           = false;
        double  SoundVolume         = 0;
        bool    PreviousHaveSound   = false;
        double  PreviousSoundVolume = 0;
        bool    HaveFilter          =false;

        // Search it at least one block have filter
        for (int shot=0;shot<Object->List.count();shot++) for (int obj=0;obj<Object->List.at(shot)->ShotComposition.List.count();obj++)
            if ((Object->List.at(shot)->ShotComposition.List[obj]->BackgroundBrush->GaussBlurSharpenSigma!=0)||
                (Object->List.at(shot)->ShotComposition.List[obj]->BackgroundBrush->QuickBlurSharpenSigma!=0)||
                (Object->List.at(shot)->ShotComposition.List[obj]->BackgroundBrush->Desat!=0)||
                (Object->List.at(shot)->ShotComposition.List[obj]->BackgroundBrush->Swirl!=0)||
                (Object->List.at(shot)->ShotComposition.List[obj]->BackgroundBrush->Implode!=0)||
                (Object->List.at(shot)->ShotComposition.List[obj]->BackgroundBrush->WaveAmp!=0)||
                (Object->List.at(shot)->ShotComposition.List[obj]->BackgroundBrush->OnOffFilter!=0)
               ) HaveFilter=true;

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
        if (ItemIndex>0) {
            for (int i=0;i<ParentTable->Diaporama->List[ItemIndex-1]->ObjectComposition.List.count();i++)
                    if ((ParentTable->Diaporama->List[ItemIndex-1]->ObjectComposition.List[i]->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&
                        (ParentTable->Diaporama->List[ItemIndex-1]->ObjectComposition.List[i]->BackgroundBrush->Video)&&
                        (ParentTable->Diaporama->List[ItemIndex-1]->ObjectComposition.List[i]->BackgroundBrush->SoundVolume!=0)) {

                PreviousHaveSound=true;
                // Parse all object from all shot to determine max volume
                for (int v=0;v<ParentTable->Diaporama->List[ItemIndex-1]->List.count();v++) for (int w=0;w<ParentTable->Diaporama->List[ItemIndex-1]->List[v]->ShotComposition.List.count();w++)
                    if ((ParentTable->Diaporama->List[ItemIndex-1]->List[v]->ShotComposition.List[w]->IndexKey==ParentTable->Diaporama->List[ItemIndex-1]->ObjectComposition.List[i]->IndexKey)&&
                        (ParentTable->Diaporama->List[ItemIndex-1]->List[v]->ShotComposition.List[w]->BackgroundBrush->SoundVolume>PreviousSoundVolume))
                            PreviousSoundVolume=ParentTable->Diaporama->List[ItemIndex-1]->List[v]->ShotComposition.List[w]->BackgroundBrush->SoundVolume;
            }
        }

        // Draw transition out of previous track
        if (IsTransition && PreviousHaveSound && !HaveSound) {
            Pen.setColor(ObjectBackground_Ruller);
            Pen.setWidth(1);
            Pen.setStyle(Qt::SolidLine);
            Painter->setPen(Pen);
            Painter->setBrush(QBrush(QColor(ObjectBackground_Ruller)));
            Table[0]=QPointF(0,3*ThumbHeight/4-TIMELINESOUNDHEIGHT-2);
            Table[1]=QPointF(TransitionSize,3*ThumbHeight/4-2);
            Table[2]=QPointF(0,3*ThumbHeight/4-2);
            Painter->drawPolygon(Table,3);

            // Draw transition out for this (previous) soundtrack
            Pen.setColor(((ItemIndex&0x1)!=0x1)?FirstSound_Color:SecondSound_Color);
            Pen.setWidth(1);
            Pen.setStyle(Qt::SolidLine);
            Painter->setPen(Pen);
            Painter->setBrush(QBrush(QColor(((ItemIndex&0x1)!=0x1)?FirstSound_Color:SecondSound_Color)));
            int RHeightPrevious=int(double(TIMELINESOUNDHEIGHT)*(PreviousSoundVolume/1.5));
            Table[0]=QPointF(0,3*ThumbHeight/4-RHeightPrevious-2);
            Table[1]=QPointF(TransitionSize,3*ThumbHeight/4-2);
            Table[2]=QPointF(0,3*ThumbHeight/4-2);
            Painter->drawPolygon(Table,3);
        }

        if (!HaveSound) {
            // Display a thumb with no sound
            Painter->fillRect(TransitionSize+3,ThumbHeight/4+2-1,NewThumbWidth,NewThumbHeight,Transparent);     // Fill background with transparent image
            Object->DrawThumbnail(NewThumbWidth,NewThumbHeight,Painter,TransitionSize+3,ThumbHeight/4+1);       // Draw thumb
            DrawThumbnailsBox(TransitionSize+3,ThumbHeight/4+2-1,NewThumbWidth,NewThumbHeight,Painter,NULL);    // Draw frame arround thumb
            if (Object->ObjectComposition.List.count()>1)   Painter->drawImage(TransitionSize+3+8,                  2-1+ThumbHeight/4+8,    QImage(ICON_BLOCKPRESENCE));    // Add mark if multiple block
            if (Object->List.count()>1)                     Painter->drawImage(TransitionSize+3+NewThumbWidth-32,   2-1+ThumbHeight/4+8,    QImage(ICON_SHOTPRESENCE));     // Add mark if multiple shot
            if (HaveFilter)                                 Painter->drawImage(TransitionSize+3+NewThumbWidth-32,   2-1+ThumbHeight/4+32,   QImage(ICON_HAVEFILTER));       // Add mark if at least one block have filter

        } else {
            // Display a thumb with sound track
            int     VideoThumbWidth =NewThumbWidth-BarWidth*2;
            int     VideoThumbHeight=ParentTable->Diaporama->GetHeightForWidth(VideoThumbWidth);

            // Draw thumb part
            Painter->fillRect(TransitionSize+3+BarWidth,ThumbHeight/4+2-1,VideoThumbWidth,VideoThumbHeight,Transparent);     // Fill background with transparent image
            Object->DrawThumbnail(VideoThumbWidth,VideoThumbHeight,Painter,TransitionSize+3+BarWidth,ThumbHeight/4+1);       // Draw thumb

            // Draw black bar for cinema decoration at left & right
            Painter->fillRect(TransitionSize+3,                         ThumbHeight/4+2-1, BarWidth,VideoThumbHeight,QBrush(Qt::black));
            Painter->fillRect(TransitionSize+3+BarWidth+VideoThumbWidth,ThumbHeight/4+2-1, BarWidth,VideoThumbHeight,QBrush(Qt::black));

            // Draw cinema decoration at left & right
            for (int HH=0;HH<3;HH++) {
                Painter->fillRect(TransitionSize+3+4,                         ThumbHeight/4+2-1+HH*(VideoThumbHeight/3)+4,BarWidth-9,(VideoThumbHeight/5),QBrush(Qt::lightGray));
                Painter->fillRect(TransitionSize+3+5+BarWidth+VideoThumbWidth,ThumbHeight/4+2-1+HH*(VideoThumbHeight/3)+4,BarWidth-9,(VideoThumbHeight/5),QBrush(Qt::lightGray));
            }

            QPen Pen;
            Pen.setWidth(1);
            Painter->setBrush(Qt::NoBrush);
            Pen.setColor(Qt::black);    Painter->setPen(Pen);    Painter->drawRect(TransitionSize+3-2,ThumbHeight/4+1-2,NewThumbWidth+4,VideoThumbHeight+4);
            Pen.setColor(Qt::darkGray); Painter->setPen(Pen);
            Painter->drawRect(TransitionSize+3-1,ThumbHeight/4+1-1,NewThumbWidth+2,VideoThumbHeight+2);
            Painter->drawRect(TransitionSize+3+BarWidth-2,ThumbHeight/4,VideoThumbWidth+4,VideoThumbHeight);

            Pen.setColor(Qt::white);    Painter->setPen(Pen);
            Painter->drawRect(TransitionSize+3,  ThumbHeight/4+1,  NewThumbWidth,  VideoThumbHeight+1);
            Painter->drawRect(TransitionSize+3+BarWidth-1,ThumbHeight/4+1,VideoThumbWidth+2,VideoThumbHeight);

            if (Object->ObjectComposition.List.count()>1)   Painter->drawImage(TransitionSize+3+BarWidth+8,                  2-1+ThumbHeight/4+8,    QImage(ICON_BLOCKPRESENCE));    // Add mark if multiple block
            if (Object->List.count()>1)                     Painter->drawImage(TransitionSize+3+BarWidth+VideoThumbWidth-32, 2-1+ThumbHeight/4+8,    QImage(ICON_SHOTPRESENCE));     // Add mark if multiple shot
            if (HaveFilter)                                 Painter->drawImage(TransitionSize+3+BarWidth+VideoThumbWidth-32, 2-1+ThumbHeight/4+32,   QImage(ICON_HAVEFILTER));       // Add mark if at least one block have filter

            // Soundtrack part
            int RHeight=int(double(TIMELINESOUNDHEIGHT)*(SoundVolume/1.5));

            Pen.setColor(ObjectBackground_Ruller);
            Pen.setWidth(1);
            Pen.setStyle(Qt::SolidLine);
            Painter->setPen(Pen);
            Painter->setBrush(QBrush(QColor(ObjectBackground_Ruller)));
            Painter->drawRect(0,3*ThumbHeight/4-2-TIMELINESOUNDHEIGHT,ThumbWidth,TIMELINESOUNDHEIGHT);

            // Draw transitions
            if (IsTransition) {
                // Draw transition out for previous soundtrack
                if ((ItemIndex>0)&&(PreviousHaveSound)) {
                    Pen.setColor(((ItemIndex&0x1)!=0x1)?FirstSound_Color:SecondSound_Color);
                    Pen.setWidth(1);
                    Pen.setStyle(Qt::SolidLine);
                    Painter->setPen(Pen);
                    Painter->setBrush(QBrush(QColor(((ItemIndex&0x1)!=0x1)?FirstSound_Color:SecondSound_Color)));
                    int RHeightPrevious=int(double(TIMELINESOUNDHEIGHT)*(PreviousSoundVolume/1.5));
                    Table[0]=QPointF(0,             3*ThumbHeight/4-2-RHeightPrevious);
                    Table[1]=QPointF(TransitionSize,3*ThumbHeight/4-2);
                    Table[2]=QPointF(0,             3*ThumbHeight/4-2);
                    Painter->drawPolygon(Table,3);
                }
                // Draw in transition + soundtrack
                Pen.setColor(((ItemIndex&0x1)==0x1)?FirstSound_Color:SecondSound_Color);
                Pen.setWidth(1);
                Pen.setStyle(Qt::SolidLine);
                Painter->setPen(Pen);
                Painter->setBrush(QBrush(QColor(((ItemIndex&0x1)==0x1)?FirstSound_Color:SecondSound_Color)));
                Table[0]=QPointF(0,             3*ThumbHeight/4-2);
                Table[1]=QPointF(TransitionSize,3*ThumbHeight/4-2-RHeight);
                Table[2]=QPointF(ThumbWidth,    3*ThumbHeight/4-2-RHeight);                          // Draw soundtrack without transition
                Table[3]=QPointF(ThumbWidth,    3*ThumbHeight/4-2);
                Painter->drawPolygon(Table,4);
            } else {
                Pen.setColor(((ItemIndex&0x1)==0x1)?FirstSound_Color:SecondSound_Color);
                Pen.setWidth(1);
                Pen.setStyle(Qt::SolidLine);
                Painter->setPen(Pen);
                Painter->setBrush(QBrush(QColor(((ItemIndex&0x1)==0x1)?FirstSound_Color:SecondSound_Color)));
                Table[0]=QPointF(0,         3*ThumbHeight/4-2);
                Table[1]=QPointF(0,         3*ThumbHeight/4-2-RHeight);
                Table[2]=QPointF(ThumbWidth,3*ThumbHeight/4-2-RHeight);                          // Draw soundtrack without transition
                Table[3]=QPointF(ThumbWidth,3*ThumbHeight/4-2);
                Painter->drawPolygon(Table,4);
            }
        }

        // Draw transition duration, slide duration and slide name
        int FontFactor=((ParentTable->ApplicationConfig->TimelineHeight-TIMELINEMINHEIGH)/20)*10;
        QFont font= QApplication::font();
        Painter->setFont(font);
        #ifdef Q_OS_WIN
        font.setPointSizeF(double(110+FontFactor)/double(Painter->fontMetrics().boundingRect("0").height()));                  // Scale font
        #else
        font.setPointSizeF(double(140+FontFactor)/double(Painter->fontMetrics().boundingRect("0").height()));                  // Scale font
        #endif
        Painter->setFont(font);

        Pen.setWidth(1);
        Pen.setStyle(Qt::SolidLine);
        QString SlideDuration=QTime(0,0,0,0).addMSecs(Object->GetDuration()).toString("hh:mm:ss.zzz");
        QString FileName=Object->SlideName;
        QString TransitionDuration=QTime(0,0,0,0).addMSecs(Object->GetTransitDuration()).toString("ss.z");
        QString SlideNumber=QString("%1").arg(ItemIndex+1);
        // Chapter TAG
        if (ItemIndex==0) Painter->drawImage(2,2,QImage(":/img/Chapter.png"));
            else if (Object->StartNewChapter) Painter->drawImage(TransitionSize+3,2,QImage(":/img/Chapter.png"));
        if (TransitionDuration[0]=='0')             TransitionDuration=TransitionDuration.right(TransitionDuration.length()-1);   // Cut first 0
        while (TransitionDuration.endsWith("0"))    TransitionDuration=TransitionDuration.left(TransitionDuration.length()-1);
        while (TransitionDuration.endsWith("."))    TransitionDuration=TransitionDuration.left(TransitionDuration.length()-1);

        if (Object->TypeObject==DIAPORAMAOBJECTTYPE_VIDEO) {
            Pen.setColor(Qt::black);
            Painter->setPen(Pen);
            Painter->drawText(QRectF(TransitionSize+3+BarWidth+1,ThumbHeight/4+2-1+1,                   NewThumbWidth,              16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(TransitionSize+3+BarWidth+1,ThumbHeight/4+2-1+1+NewThumbHeight-16, NewThumbWidth,              16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(2+1,                        ThumbHeight/4+2-1+34+1,                32,                         16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(BackThumbWidth+2+1,         (ThumbHeight/4-16)/2+1,                ThumbWidth-BackThumbWidth-4,16),SlideNumber,Qt::AlignHCenter|Qt::AlignVCenter);

            Pen.setColor(Qt::white);
            Painter->setPen(Pen);
            Painter->drawText(QRectF(TransitionSize+3+BarWidth, ThumbHeight/4+2-1,                      NewThumbWidth,              16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(TransitionSize+3+BarWidth, ThumbHeight/4+2-1+NewThumbHeight-16,    NewThumbWidth,              16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(2,                         ThumbHeight/4+2-1+34,                  32,                          16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(BackThumbWidth+2,          (ThumbHeight/4-16)/2,                  ThumbWidth-BackThumbWidth-4, 16),SlideNumber,Qt::AlignHCenter|Qt::AlignVCenter);
        } else {
            Pen.setColor(Qt::black);
            Painter->setPen(Pen);
            Painter->drawText(QRectF(TransitionSize+3+1,        ThumbHeight/4+2-1+1,                    NewThumbWidth,              16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(TransitionSize+3+1,        ThumbHeight/4+2-1+1+NewThumbHeight-16,  NewThumbWidth,              16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(2+1,                       ThumbHeight/4+2-1+34+1,                 32,                         16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(BackThumbWidth+2+1,        (ThumbHeight/4-16)/2+1,                 ThumbWidth-BackThumbWidth-4,16),SlideNumber,Qt::AlignHCenter|Qt::AlignVCenter);

            Pen.setColor(Qt::white);
            Painter->setPen(Pen);
            Painter->drawText(QRectF(TransitionSize+3,          ThumbHeight/4+2-1,                      NewThumbWidth,              16),SlideDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(TransitionSize+3,          ThumbHeight/4+2-1+NewThumbHeight-16,    NewThumbWidth,              16),FileName,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(2,                         ThumbHeight/4+2-1+34,                   32,                         16),TransitionDuration,Qt::AlignHCenter|Qt::AlignVCenter);
            Painter->drawText(QRectF(BackThumbWidth+2,          (ThumbHeight/4-16)/2,                   ThumbWidth-BackThumbWidth-4,16),SlideNumber,Qt::AlignHCenter|Qt::AlignVCenter);
        }

        //==========================================================================================================================
        // Track MUSIC (last 1/4 height of the slide)
        //==========================================================================================================================

        int         CurrentCountObjet   =0;
        qlonglong   StartPosition       =0;
        qlonglong   NextStartPosition   =0;
        double      CurrentFactor       =Object->MusicPause?0:Object->MusicReduceVolume?Object->MusicReduceFactor:1;
        double      PreviousFactor      =0;

        if ((ItemIndex>0)&&(Object->Parent->GetMusicObject(ItemIndex-1,StartPosition)!=NULL))
            PreviousFactor=(Object->Parent->List[ItemIndex-1]->MusicPause)?0:(Object->Parent->List[ItemIndex-1]->MusicReduceVolume)?Object->Parent->List[ItemIndex-1]->MusicReduceFactor:1;

        bool        EndMusic            =true;
        bool        DrawVolumeTransition=(PreviousFactor!=CurrentFactor);
        bool        DrawInTransition    =false;
        bool        DrawOutTransition   =false;
        bool        DrawOutCut          =false;
        bool        DrawPause           =false;
        int         RHeight             =int(TIMELINESOUNDHEIGHT*2*(CurrentFactor/1.5));
        int         PHeight             =int(TIMELINESOUNDHEIGHT*2*(PreviousFactor/1.5));

        if (ItemIndex>0) {
            cMusicObject *PrevMusique=Object->Parent->GetMusicObject(ItemIndex-1,StartPosition);
            if ((PrevMusique)&&((QTime(0,0,0,0).msecsTo(PrevMusique->Duration)-StartPosition)>Object->Parent->List[ItemIndex-1]->GetDuration())) DrawOutTransition=true;
        }

        // Calculate wich music will be use for this object and for the next object
        int             OwnerObjectMusic    =0;
        int             OwnerObjectNextMusic=0;
        cMusicObject    *CurMusic           =Object->Parent->GetMusicObject(ItemIndex,StartPosition,&CurrentCountObjet,&OwnerObjectMusic);
        cMusicObject    *NextMusic          =NULL;

        if ((CurMusic!=NULL)&&(StartPosition>=(QTime(0,0,0,0).msecsTo(CurMusic->Duration)-Object->TransitionDuration))) CurMusic=NULL;

        if ((ItemIndex+1)<ParentTable->columnCount()) {
            NextMusic=Object->Parent->GetMusicObject(ItemIndex+1,NextStartPosition,NULL,&OwnerObjectNextMusic);
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
                    if (((ItemIndex>0)&&(Object->Parent->GetMusicObject(ItemIndex-1,StartPosition)!=NULL))&&(IsTransition)) DrawInTransition=true;
                    DrawVolumeTransition=false;
                }
            // continue Playlist from a previous object
            } else if (Object->MusicPause) DrawPause=true;
            if (DrawInTransition && IsTransition) {
                // Draw out transition from a previous object
                if (DrawOutTransition) {
                    if ((CurrentCountObjet & 1)!=1) {
                        Painter->setBrush(QBrush(QColor(FirstMusic_Color)));
                        Pen.setColor(FirstMusic_Color);
                    } else {
                        Painter->setBrush(QBrush(QColor(SecondMusic_Color)));
                        Pen.setColor(SecondMusic_Color);
                    }
                    Pen.setWidth(0);
                    Painter->setPen(Pen);
                    Table[0]=QPointF(-1,ThumbHeight-5-PHeight+2);
                    Table[1]=QPointF(34,ThumbHeight-5+2);
                    Table[2]=QPointF(-1,ThumbHeight-5+2);
                    Painter->drawPolygon(Table,3);
                }
                Table[0]=QPointF(-1,ThumbHeight-5+2);
                Table[1]=QPointF(34,ThumbHeight-5-RHeight+2);
            } else if (DrawVolumeTransition && IsTransition) {
                Table[0]=QPointF(-1,ThumbHeight-5-PHeight+2);
                Table[1]=QPointF(34,ThumbHeight-5-RHeight+2);
            } else {
                Table[0]=QPointF(-1,ThumbHeight-5+2);
                Table[1]=QPointF(-1,ThumbHeight-5-RHeight+2);
            }
            if (DrawOutCut) {
                Table[2]=QPointF(ThumbWidth-34,ThumbHeight-5-RHeight+2);
                Table[3]=QPointF(ThumbWidth-34,ThumbHeight-5+2);
            } else {
                Table[2]=QPointF(ThumbWidth+2,ThumbHeight-5-RHeight+2);
                Table[3]=QPointF(ThumbWidth+2,ThumbHeight-5+2);
            }
            Table[4]=QPointF(-1,ThumbHeight-5+2);

            if ((CurrentCountObjet & 1)==1) {
                Painter->setBrush(QBrush(QColor(FirstMusic_Color)));
                Pen.setColor(FirstMusic_Color);
            } else {
                Painter->setBrush(QBrush(QColor(SecondMusic_Color)));
                Pen.setColor(SecondMusic_Color);
            }
            Pen.setWidth(0);
            Painter->setPen(Pen);
            Painter->drawPolygon(Table,5);

            if (DrawPause) Painter->drawImage((ThumbWidth-24-TransitionSize)/2+TransitionSize,ThumbHeight-5-24,QImage(ICON_PLAYERPAUSE));
        } else if (DrawOutTransition) {
            // Draw out transition from a previous object
            if ((CurrentCountObjet & 1)!=1) {
                Painter->setBrush(QBrush(QColor(FirstMusic_Color)));
                Pen.setColor(FirstMusic_Color);
            } else {
                Painter->setBrush(QBrush(QColor(SecondMusic_Color)));
                Pen.setColor(SecondMusic_Color);
            }
            Pen.setWidth(0);
            Painter->setPen(Pen);
            Table[0]=QPointF(-1,ThumbHeight-5-PHeight+2);
            Table[1]=QPointF(34,ThumbHeight-5+2);
            Table[2]=QPointF(-1,ThumbHeight-5+2);
            Painter->drawPolygon(Table,3);
        }

        // Draw separated line
        Pen.setWidth(1);
        Pen.setStyle(Qt::DotLine);
        Pen.setColor(DotLine_Color);
        Painter->setPen(Pen);
        Painter->drawLine(QPointF(TransitionSize+1,3*ThumbHeight/4),QPointF(TransitionSize+1,ThumbHeight-1));
        Painter->drawLine(QPointF(ThumbWidth-1,    3*ThumbHeight/4),QPointF(ThumbWidth-1,    ThumbHeight-1));

        //==========================================================================================================================
        // Draw current box (if needed)
        //==========================================================================================================================
        if (ItemIndex==ParentTable->Diaporama->CurrentCol) {
            Pen.setColor(WidgetSelection_Color);
            Pen.setStyle(Qt::SolidLine);
            Pen.setWidth(10);
            Painter->setPen(Pen);
            Painter->setBrush(Qt::NoBrush); //QBrush(QColor(WidgetSelection_Color)));
            //Painter->setOpacity(0.5);
            Painter->drawRect(0,0,ThumbWidth,ThumbHeight);
            //Painter->setOpacity(1);
        }

        //==========================================================================================================================
        // Draw Drag & Drop inserting point (if needed)
        //==========================================================================================================================
        bool    DrawDragBefore=false;
        bool    DrawDragAfter =false;
        bool    DrawMusicPart =false;

        if (ParentTable->IsDragOn==DRAGMODE_INTERNALMOVE_SLIDE) {             //  Drag source is timeline
            if ((ItemIndex!=ParentTable->DragItemSource)&&(ItemIndex!=ParentTable->DragItemSource+1)) {
                if (ItemIndex==ParentTable->DragItemDest)                                                           DrawDragBefore=true;
                if ((ItemIndex==ParentTable->DragItemDest-1)&&(ItemIndex==ParentTable->Diaporama->List.count()-1))  DrawDragAfter =true;
            }

        } else if (ParentTable->IsDragOn==DRAGMODE_EXTERNALADD_SLIDE) {      //  Drag source is external to timeline
            if (ItemIndex==ParentTable->DragItemDest)                                                               DrawDragBefore=true;
            if ((ItemIndex==ParentTable->DragItemDest-1)&&(ItemIndex==ParentTable->Diaporama->List.count()-1))      DrawDragAfter =true;

        } else if (ParentTable->IsDragOn==DRAGMODE_EXTERNALADD_MUSIC) {      //  Drag source is music only and external to timeline
            if (ItemIndex==ParentTable->DragItemDest)                                                               DrawMusicPart=true;
        }

        if (DrawDragBefore || DrawDragAfter || DrawMusicPart) {
            Pen.setColor(WidgetDrag_Color);
            Pen.setStyle(Qt::SolidLine);
            Pen.setWidth(6);
            Painter->setPen(Pen);
            Painter->setBrush(Qt::NoBrush); //QBrush(QColor(WidgetSelection_Color)));
            Painter->setOpacity(0.5);
            if (DrawDragBefore) {
                // Before slide
                Painter->drawLine(3,3*(ThumbHeight/8),3,ThumbHeight-3*(ThumbHeight/8));
                Painter->drawLine(3,ThumbHeight/2,32,3*(ThumbHeight/8));
                Painter->drawLine(3,ThumbHeight/2,32,ThumbHeight-3*(ThumbHeight/8));
                Painter->drawLine(3,ThumbHeight/2,64,ThumbHeight/2);
            } else if (DrawDragAfter) {
                // After slide
                Painter->drawLine(ThumbWidth-3,3*(ThumbHeight/8),ThumbWidth-3,ThumbHeight-3*(ThumbHeight/8));
                Painter->drawLine(ThumbWidth-3,ThumbHeight/2,ThumbWidth-3-32,3*(ThumbHeight/8));
                Painter->drawLine(ThumbWidth-3,ThumbHeight/2,ThumbWidth-3-32,ThumbHeight-3*(ThumbHeight/8));
                Painter->drawLine(ThumbWidth-3,ThumbHeight/2,ThumbWidth-3-64,ThumbHeight/2);
            } else if (DrawMusicPart) {
                // Music part
                Painter->drawRect(3,ThumbHeight-ThumbHeight/4+3,ThumbWidth-3,ThumbHeight/4-6);
            }
            Painter->setOpacity(1);
        }

        //==========================================================================================================================
        // Draw selection
        //==========================================================================================================================
        if (option.state & QStyle::State_Selected) {
            Painter->setPen(QPen(Qt::NoPen));
            Painter->setBrush(QBrush(Qt::blue));
            Painter->setOpacity(0.25);
            Painter->drawRect(0,0,ThumbWidth,ThumbHeight);
            Painter->setOpacity(1);
        }
    }
    Painter->restore();
    QApplication::restoreOverrideCursor();
}

//********************************************************************************************************************
// cCustomSlideTable
//********************************************************************************************************************

cCustomSlideTable::cCustomSlideTable(QWidget *parent):QTableWidget(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::cCustomSlideTable");

    Diaporama        =NULL;             // Link to current diaporama
    ApplicationConfig=NULL;             // Link to current application config
    PartitionMode    =false;            // True if multiple line
    DragItemSource   =-1;
    DragItemDest     =-1;
    IsDragOn         =DRAGMODE_NOACTION;

    setItemDelegate(new QCustomThumbItemDelegate(this));

    horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    verticalHeader()->setResizeMode(QHeaderView::Fixed);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    setDragDropMode(QAbstractItemView::DragDrop);
    setDragDropOverwriteMode(false);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

//====================================================================================================================

void cCustomSlideTable::dragEnterEvent(QDragEnterEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::dragEnterEvent");

    // Construct file list
    QList<QUrl> urlList;
    QString     fName;
    QFileInfo   info;
    QStringList FileList;

    if (event->mimeData()->hasUrls()) {
        urlList = event->mimeData()->urls();                                // returns list of QUrls
        for (int i=0;i<urlList.count();i++) {
            fName = urlList[i].toLocalFile();                               // convert first QUrl to local path
            info.setFile(fName);                                            // information about file
            if (info.isFile()) FileList.append(fName);                      // append file
        }
        // Now, parse all files to find music files (and put them in MusicFileList)
        QStringList MusicFileList;
        int i=0;
        while (i<FileList.count()) {
            if (ApplicationConfig->AllowMusicExtension.contains(QFileInfo(FileList.at(i)).suffix().toLower())) {
                // check if file contains video track
                cVideoFile  *MediaFile=new cVideoFile(OBJECTTYPE_VIDEOFILE,ApplicationConfig);
                if (((MediaFile)&&(MediaFile->GetInformationFromFile(QFileInfo(FileList.at(i)).absoluteFilePath(),NULL,NULL)))&&
                     (MediaFile->OpenCodecAndFile())&&(MediaFile->VideoStreamNumber<0)&&(MediaFile->AudioStreamNumber>=0)) {
                    MusicFileList.append(FileList.at(i));
                    FileList.removeAt(i);
                } else i++;
            } else i++;
        }
        if ((MusicFileList.count()>0)&&(FileList.count()==0)&&(Diaporama->List.count()>0)) {
            // Drag & drop with Music only
            IsDragOn      =DRAGMODE_EXTERNALADD_MUSIC;
            DragItemSource=-1;
            DragItemDest  =-1;
            setCursor(Qt::ClosedHandCursor);
            event->acceptProposedAction();
        } else if ((MusicFileList.count()==0)&&(FileList.count()>0)) {
            // Drag & drop with no music file(s)
            IsDragOn      =DRAGMODE_EXTERNALADD_SLIDE;
            DragItemSource=-1;
            DragItemDest  =-1;
            setCursor(Qt::ClosedHandCursor);
            event->acceptProposedAction();
        } else {
            // Reject action
            DragItemSource   =-1;
            DragItemDest     =-1;
            IsDragOn         =DRAGMODE_NOACTION;
        }
    } else {
        // Reject action
        DragItemSource   =-1;
        DragItemDest     =-1;
        IsDragOn         =DRAGMODE_NOACTION;
    }
}

//====================================================================================================================

void cCustomSlideTable::dragLeaveEvent(QDragLeaveEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::dragLeaveEvent");
    if (IsDragOn==DRAGMODE_EXTERNALADD_SLIDE) {
        // Clear previous selected slide
        int NbrX=columnCount();
        if ((NbrX>0)&&(DragItemDest>=0)) {
            int ToUse   =DragItemDest;
            DragItemDest=-1;
            if (ToUse<Diaporama->List.count())  update(model()->index(ToUse/NbrX,ToUse-(ToUse/NbrX)*NbrX));
                else                            update(model()->index((ToUse-1)/NbrX,(ToUse-1)-((ToUse-1)/NbrX)*NbrX));
        }
        IsDragOn=DRAGMODE_NOACTION;
    }
    event->accept();
    setCursor(Qt::ArrowCursor);
}

//====================================================================================================================

void cCustomSlideTable::dropEvent(QDropEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::dropEvent");

    IsDragOn=DRAGMODE_NOACTION;
    GlobalMainWindow->FileList.clear();

    QList<QUrl> urlList;
    QString     fName;
    QFileInfo   info;

    if (event->mimeData()->hasUrls()) {
        urlList = event->mimeData()->urls();                                // returns list of QUrls
        for (int i=0;i<urlList.count();i++) {
            fName = urlList[i].toLocalFile();                               // convert first QUrl to local path
            info.setFile(fName);                                            // information about file
            if (info.isFile()) GlobalMainWindow->FileList.append(fName);    // append file
        }
    }
    event->acceptProposedAction();
    if (GlobalMainWindow->FileList.count()>0) emit DoAddDragAndDropFile();
}

//====================================================================================================================

void cCustomSlideTable::dragMoveEvent(QDragMoveEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::dragMoveEvent");

    if ((IsDragOn==DRAGMODE_EXTERNALADD_SLIDE)||(IsDragOn==DRAGMODE_EXTERNALADD_MUSIC)) {

        int ThumbWidth  =columnWidth(0);
        int ThumbHeight =rowHeight(0);
        int NbrX        =columnCount();

        // Get item number under mouse
        int newrow=ThumbHeight>0?(event->pos().y()+verticalOffset())/ThumbHeight:0;
        int newcol=ThumbWidth>0?(event->pos().x()+horizontalOffset())/ThumbWidth:0;
        int Selected=newrow*NbrX+newcol;

        // Adjust selected
        if (IsDragOn==DRAGMODE_EXTERNALADD_SLIDE) {
            if (Selected>Diaporama->List.count()) Selected=Diaporama->List.count();
            else if ((Selected==Diaporama->List.count()-1)&&(event->pos().x()>0)&&(event->pos().x()<width())) {
                int NewX=event->pos().x()-(event->pos().x()/ThumbWidth)*ThumbWidth;
                if (NewX>=3*ThumbWidth/4) Selected=Diaporama->List.count();
            }
        } else if (IsDragOn==DRAGMODE_EXTERNALADD_MUSIC) {
            if (Selected>=Diaporama->List.count()) Selected=Diaporama->List.count()-1;
        }
        if (Selected<0) Selected=0;

        if (Selected!=DragItemDest) {

            // Clear previous selected slide
            if ((NbrX>0)&&(DragItemDest>=0)) {
                int ToUse   =DragItemDest;
                DragItemDest=-1;
                if (ToUse<Diaporama->List.count())  update(model()->index(ToUse/NbrX,ToUse-(ToUse/NbrX)*NbrX));
                    else                            update(model()->index((ToUse-1)/NbrX,(ToUse-1)-((ToUse-1)/NbrX)*NbrX));
            }

            // Display new selected slide
            DragItemDest=Selected;
            if ((NbrX>0)&&(Selected>=0)) {
                if (Selected<Diaporama->List.count())   update(model()->index(Selected/NbrX,Selected-(Selected/NbrX)*NbrX));
                    else                                update(model()->index((Selected-1)/NbrX,(Selected-1)-((Selected-1)/NbrX)*NbrX));
            }
        }
        event->acceptProposedAction();
        setCursor(Qt::ClosedHandCursor);
    }
}

//====================================================================================================================

void cCustomSlideTable::mouseMoveEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::mouseMoveEvent");

    if (IsDragOn!=DRAGMODE_INTERNALMOVE_SLIDE) {
        setCursor(Qt::ArrowCursor);
        QTableWidget::mouseMoveEvent(event);
    } else {
        if      ((!PartitionMode)&&(event->pos().x()<0)&&(horizontalScrollBar()->value()>0))                                        horizontalScrollBar()->setValue(horizontalScrollBar()->value()-1);  // Try to scroll left if not partition mode
        else if ((!PartitionMode)&&(event->pos().x()>width())&&(horizontalScrollBar()->value()<horizontalScrollBar()->maximum()))   horizontalScrollBar()->setValue(horizontalScrollBar()->value()+1);  // Try to scroll right if not partition mode
        else if (( PartitionMode)&&(event->pos().y()<0)&&(verticalScrollBar()->value()>0))                                          verticalScrollBar()->setValue(verticalScrollBar()->value()-1);      // Try to scroll up if partition mode
        else if (( PartitionMode)&&(event->pos().y()>height())&&(verticalScrollBar()->value()<verticalScrollBar()->maximum()))      verticalScrollBar()->setValue(verticalScrollBar()->value()+1);      // Try to scroll down if partition mode

        int ThumbWidth  =columnWidth(0);
        int ThumbHeight =rowHeight(0);
        int NbrX        =columnCount();

        int newrow=ThumbHeight>0?(event->pos().y()+verticalOffset())/ThumbHeight:0;
        int newcol=ThumbWidth>0?(event->pos().x()+horizontalOffset())/ThumbWidth:0;
        int Selected=newrow*NbrX+newcol;
        if (Selected>Diaporama->List.count()) Selected=Diaporama->List.count();
        if (Selected<0) Selected=0;

        if (Selected!=DragItemDest) {

            // Clear previous selected slide
            if ((NbrX>0)&&(DragItemDest>=0)) {
                int ToUse   =DragItemDest;
                DragItemDest=-1;
                if (ToUse<Diaporama->List.count())  update(model()->index(ToUse/NbrX,ToUse-(ToUse/NbrX)*NbrX));
                    else                            update(model()->index((ToUse-1)/NbrX,(ToUse-1)-((ToUse-1)/NbrX)*NbrX));
            }

            // Display new selected slide
            DragItemDest=Selected;
            if ((NbrX>0)&&(Selected>=0)) {
                if (Selected<Diaporama->List.count())   update(model()->index(Selected/NbrX,Selected-(Selected/NbrX)*NbrX));
                    else                                update(model()->index((Selected-1)/NbrX,(Selected-1)-((Selected-1)/NbrX)*NbrX));
            }
        }
        if ((DragItemDest!=-1)&&(event->pos().x()>=-columnWidth(0))&&(event->pos().x()<=width()+columnWidth(0))&&
            (event->pos().y()>=0-(PartitionMode?rowHeight(0):0))&&(event->pos().y()<=height()+(PartitionMode?rowHeight(0):0))) {
            if ((DragItemDest!=Diaporama->CurrentCol)&&(DragItemDest!=Diaporama->CurrentCol+1)) setCursor(Qt::ClosedHandCursor);
                else setCursor(Qt::ForbiddenCursor);
        } else setCursor(Qt::ForbiddenCursor);
    }
}

//====================================================================================================================

void cCustomSlideTable::mousePressEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::mousePressEvent");

    if (event->button()!=Qt::LeftButton) {
        QTableWidget::mousePressEvent(event);
    } else {
        if ((Diaporama->List.count()==0)||(IsDragOn==DRAGMODE_INTERNALMOVE_SLIDE)) return;
        setCursor(Qt::ArrowCursor);

        // Get item number under mouse
        int ThumbWidth  =columnWidth(0);
        int ThumbHeight =rowHeight(0);

        int row         =(event->pos().y()+verticalOffset())/ThumbHeight;
        int col         =(event->pos().x()+horizontalOffset())/ThumbWidth;
        int Current     =currentRow()*columnCount()+currentColumn();
        int Selected    =row*columnCount()+col;

        if (event->modifiers()==Qt::ShiftModifier) {
            // Shift : Add all items from current to item
            if (Current<Selected) for (int i=Current+1;i<=Selected;i++) selectionModel()->select(model()->index(i/columnCount(),i-(i/columnCount())*columnCount(),QModelIndex()),QItemSelectionModel::Select);
                else              for (int i=Current-1;i>=Selected;i--) selectionModel()->select(model()->index(i/columnCount(),i-(i/columnCount())*columnCount(),QModelIndex()),QItemSelectionModel::Select);
        } else if (event->modifiers()==Qt::ControlModifier) {
            // Control : toggle selection for item (if is not current item)
            if (Selected!=Diaporama->CurrentCol) selectionModel()->select(model()->index(row,col,QModelIndex()),QItemSelectionModel::Toggle);
        } else {
            // Other : clear selection, then add item to selection
            ResetDisplay(Selected);
        }

        IsDragOn=DRAGMODE_NOACTION;
        DragItemSource=row*columnCount()+col;

        if ((Selected>=0)&&(Selected<Diaporama->List.count())) {
            // if it was previously selected then start a drag & drop operation
            IsDragOn=DRAGMODE_INTERNALMOVE_SLIDE;
            DragItemSource=Selected;
            DragItemDest  =Selected;
            setCursor(Qt::ClosedHandCursor);
        }
    }
}

//====================================================================================================================

void cCustomSlideTable::mouseReleaseEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::mouseReleaseEvent");

    setCursor(Qt::ArrowCursor);
    if (event->button()==Qt::RightButton) {
        emit RightClickEvent(event);
    } else if (IsDragOn!=DRAGMODE_INTERNALMOVE_SLIDE) {
        QTableWidget::mouseReleaseEvent(event);
    } else {
        setCursor(Qt::ArrowCursor);
        IsDragOn=DRAGMODE_NOACTION;
        if ((DragItemDest!=-1)&&(event->pos().x()>=-columnWidth(0))&&(event->pos().x()<=width()+columnWidth(0))&&
            (event->pos().y()>=0-(PartitionMode?rowHeight(0):0))&&(event->pos().y()<=height()+(PartitionMode?rowHeight(0):0)))
            emit DragMoveItem(); else {setUpdatesEnabled(false); setUpdatesEnabled(true);}
    }

}

//====================================================================================================================

void cCustomSlideTable::mouseDoubleClickEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::mouseDoubleClickEvent");

    if (columnCount()==0) return;
    int ThumbWidth =columnWidth(0);
    int ThumbHeight=rowHeight(0);
    int row=(event->pos().y()+verticalOffset())/ThumbHeight;
    int col=(event->pos().x()+horizontalOffset())/ThumbWidth;
    int NbrX       =width()/ThumbWidth;

    int Selected=(PartitionMode?row*NbrX+col:col);
    int x=event->pos().x()+horizontalOffset()-col*ThumbWidth;
    int y=event->pos().y()+verticalOffset()-row*ThumbHeight;

    if ((Selected>=0)&&(Selected<Diaporama->List.count())) {
        if (x<=TransitionSize) {
            // In transition column
            if (y<ThumbHeight/4)                        ; //emit EditBackGTransition()
            else if (y<ThumbHeight/4+TransitionSize)    emit EditTransition();
            else if (y>=3*ThumbHeight/4)                emit EditSoundTrack();
        } else {
            // In object column
            if (y<ThumbHeight/4)                        emit EditBackground();
            else if (y<3*ThumbHeight/4)                 emit EditMediaObject();
            else if (y>=3*ThumbHeight/4)                emit EditMusicTrack();
        }
    }
}

//====================================================================================================================

void cCustomSlideTable::AddObjectToTimeLine(int CurIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::AddObjectToTimeLine");

    int ThumbWidth =Diaporama->GetWidthForHeight(ApplicationConfig->TimelineHeight/2-4)+36+5;
    int ThumbHeight=ApplicationConfig->TimelineHeight;
    int NbrX       =viewport()->width()/ThumbWidth;
    int NbrY       =NbrX>0?Diaporama->List.count()/NbrX:0;

    if (NbrX*NbrY<Diaporama->List.count()) NbrY++;

    // Remove empty cell with unselecable item (if needed)
    for (int Row=0;Row<rowCount();Row++) for (int Col=0;Col<columnCount();Col++) if (cellWidget(Row,Col)!=NULL) removeCellWidget(Row,Col);

    if (PartitionMode) {
        // Partition mode
        setColumnCount(NbrX);   for (int Col=0;Col<columnCount();Col++) setColumnWidth(Col,ThumbWidth);
        setRowCount(NbrY);      for (int Row=0;Row<rowCount();Row++)    setRowHeight(Row,ThumbHeight);
    } else {
        setRowCount(1);                             setRowHeight(0,ThumbHeight);
        setColumnCount(Diaporama->List.count());    for (int i=0;i<columnCount();i++) setColumnWidth(i,ThumbWidth);
    }

    // Adjust current sel
    if ((Diaporama->CurrentCol<0)||(Diaporama->CurrentCol==CurIndex)) Diaporama->CurrentCol=CurIndex;
    GlobalMainWindow->AdjustRuller();
}

//====================================================================================================================

void cCustomSlideTable::ResetDisplay(int Selected) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::ResetDisplay");
    int ThumbWidth =Diaporama->GetWidthForHeight(ApplicationConfig->TimelineHeight/2-4)+36+5;
    int ThumbHeight=ApplicationConfig->TimelineHeight;
    int NbrX       =viewport()->width()/ThumbWidth;
    int NbrY       =NbrX>0?Diaporama->List.count()/NbrX:0;

    if (NbrX*NbrY<Diaporama->List.count()) NbrY++;

    GlobalMainWindow->FLAGSTOPITEMSELECTION=true;
    setUpdatesEnabled(false);

    // Clear selection
    selectionModel()->clear();

    // Remove empty cell with unselecable item (if needed)
    for (int Row=0;Row<rowCount();Row++) for (int Col=0;Col<columnCount();Col++) if (cellWidget(Row,Col)!=NULL) removeCellWidget(Row,Col);
    // Set new col and row count
    if (PartitionMode) {
        // Partition mode
        setColumnCount(NbrX);   for (int Col=0;Col<columnCount();Col++) setColumnWidth(Col,ThumbWidth);
        setRowCount(NbrY);      for (int Row=0;Row<rowCount();Row++)    setRowHeight(Row,ThumbHeight);
    } else {
        setRowCount(1);                             setRowHeight(0,ThumbHeight);
        setColumnCount(Diaporama->List.count());    for (int i=0;i<columnCount();i++) setColumnWidth(i,ThumbWidth);
    }
    GlobalMainWindow->FLAGSTOPITEMSELECTION=false;
    if (Selected>=0) {
        int row=columnCount()>0?Selected/columnCount():0;
        int col=Selected-row*columnCount();
        setCurrentCell(row,col,QItemSelectionModel::Select|QItemSelectionModel::Current);
    }
    setUpdatesEnabled(true);  // Reset timeline painting
}

//====================================================================================================================

void cCustomSlideTable::SetTimelineHeight(bool NewPartitionMode) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::SetTimelineHeight");
    PartitionMode=NewPartitionMode;
    int Selected=CurrentSelected();
    if (!PartitionMode) {
        setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setFixedHeight(ApplicationConfig->TimelineHeight+(height()-viewport()->height()));
    } else {
        setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        setMaximumHeight(QWIDGETSIZE_MAX);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    updateGeometry();
    QApplication::processEvents(); // Give time to Qt to redefine position of each control and timeline height !
    ResetDisplay(Selected);
}

//====================================================================================================================

int cCustomSlideTable::CurrentSelected() {
    return currentRow()*columnCount()+currentColumn();
}

bool cCustomSlideTable::IsMultipleSelection() {
    QModelIndexList SelList=selectionModel()->selectedIndexes();
    return (SelList.count()>1);
}

void cCustomSlideTable::CurrentSelectionList(QList<int> *List) {
    QModelIndexList SelList=selectionModel()->selectedIndexes();
    QList<bool>     IsSelected;
    for (int i=0;i<rowCount()*columnCount();i++) IsSelected.append(false);
    for (int i=0;i<SelList.count();i++) {
        int Index=SelList.at(i).row()*columnCount()+SelList.at(i).column();
        IsSelected[Index]=true;
    }
    List->clear();
    for (int i=0;i<IsSelected.count();i++) if ((i<Diaporama->List.count())&&(IsSelected[i])) List->append(i);
}

//====================================================================================================================

void cCustomSlideTable::SetCurrentCell(int Index) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::SetCurrentCell");
    if (columnCount()==0) return;
    setUpdatesEnabled(false);
    selectionModel()->clear();      // Clear selection
    int row=Index/columnCount();
    int col=Index-row*columnCount();
    setCurrentCell(row,col,QItemSelectionModel::Select|QItemSelectionModel::Current);    
    setUpdatesEnabled(true);
}

//====================================================================================================================

void cCustomSlideTable::CleanAll() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomSlideTable::CleanAll");
    setUpdatesEnabled(false);
    for (int Row=0;Row<rowCount();Row++) for (int Col=0;Col<columnCount();Col++) if (cellWidget(Row,Col)!=NULL) removeCellWidget(Row,Col);
    while (columnCount()>0) removeColumn(columnCount()-1);
    while (rowCount())      removeRow(rowCount()-1);
    setUpdatesEnabled(true);
}

