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

#include "cCustomShotTable.h"
#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QScrollBar>
#include <QPainter>

#define SCALINGTEXTFACTOR 400

//********************************************************************************************************************************
// cShotTableItemDelegate : Item delegate for block table
//********************************************************************************************************************************

class cShotTableItemDelegate : public QStyledItemDelegate {
//Q_OBJECT
public:
    cCustomShotTable  *ParentTable;

    explicit cShotTableItemDelegate(QObject *parent);

    virtual void    paint(QPainter *painter,const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

//========================================================================================================================

cShotTableItemDelegate::cShotTableItemDelegate(QObject *parent):QStyledItemDelegate(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cShotTableItemDelegate::cShotTableItemDelegate");

    ParentTable=(cCustomShotTable *)parent;
}

//========================================================================================================================

void cShotTableItemDelegate::paint(QPainter *Painter,const QStyleOptionViewItem &option,const QModelIndex &index) const {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cShotTableItemDelegate::paint");
    int Height=ParentTable->rowHeight(0);
    int Width =ParentTable->columnWidth(0);

    if (ParentTable->DiaporamaObject==NULL) return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QImage    TempImg(Width,Height,QImage::Format_ARGB32_Premultiplied);
    QPainter  TempPainter;
    TempPainter.begin(&TempImg);

    // Fill background
    TempPainter.fillRect(QRectF(0,0,Width,Height),Transparent);

    // Calc start position of this shot
    qlonglong Position=0; for (int i=1;i<=index.column();i++) Position=Position+ParentTable->DiaporamaObject->List[i-1]->StaticDuration;

    // Calc duration of this shot
    qlonglong Duration=ParentTable->DiaporamaObject->List[index.column()]->StaticDuration;

    // Define display color of duration (Red only if shot is the last and Position+Duration < DiaporamaObject's duration
    bool RedColor=((index.column()==ParentTable->DiaporamaObject->List.count()-1)&&(Position+Duration<ParentTable->DiaporamaObject->GetDuration()));
    if (RedColor) Duration=ParentTable->DiaporamaObject->GetDuration()-Position;

    // draw shot
    if (ParentTable->DiaporamaObject->Thumbnail) {
        delete ParentTable->DiaporamaObject->Thumbnail;
        ParentTable->DiaporamaObject->Thumbnail=NULL;
    }
    ParentTable->DiaporamaObject->DrawThumbnail(Width,Height,&TempPainter,0,0,index.column());

    // Draw selected box (if needed)

    if (index.column()==ParentTable->currentColumn()) {
        QPen Pen;
        Pen.setColor(Qt::blue);
        Pen.setWidth(6);
        TempPainter.setPen(Pen);
        TempPainter.setBrush(Qt::NoBrush);
        TempPainter.drawRect(3,3,Width-1-6,Height-1-6);
    }

    // Draw Drag & Drop inserting point (if needed)
    if ((ParentTable->IsDragOn==1)&&(index.column()!=ParentTable->DragItemSource)&&((index.column()==ParentTable->DragItemDest)||((index.column()==ParentTable->DragItemDest-1)&&(ParentTable->DragItemDest==ParentTable->DiaporamaObject->List.count())))) {
        TempPainter.save();
        QPen Pen;
        Pen.setColor(Qt::red);
        Pen.setStyle(Qt::SolidLine);
        Pen.setWidth(6);
        TempPainter.setPen(Pen);
        TempPainter.setBrush(Qt::NoBrush); //QBrush(QColor(WidgetSelection_Color)));
        TempPainter.setOpacity(0.5);
        TempPainter.setOpacity(0.5);
        if (index.column()==ParentTable->DragItemDest)  TempPainter.drawLine(3,      0,3,      Height);
            else                                        TempPainter.drawLine(Width-3,0,Width-3,Height);
        TempPainter.setOpacity(1);
        TempPainter.restore();
    }

    // -------------------------- Draw shot duration
    QPen  Pen;
    QFont font= QApplication::font();
    int   FontFactor=((ParentTable->DiaporamaObject->Parent->ApplicationConfig->TimelineHeight-TIMELINEMINHEIGH)/20)*10;
    TempPainter.setFont(font);
    #ifdef Q_OS_WIN
    font.setPointSizeF(double(110+FontFactor)/double(TempPainter.fontMetrics().boundingRect("0").height()));                  // Scale font
    #else
    font.setPointSizeF((double(140+FontFactor)/double(TempPainter.fontMetrics().boundingRect("0").height()))*ScreenFontAdjust);// Scale font
    #endif
    TempPainter.setFont(font);
    Pen.setWidth(1);
    Pen.setStyle(Qt::SolidLine);
    QString ShotDuration=QTime(0,0,0,0).addMSecs(Duration).toString("hh:mm:ss.zzz");
    Pen.setColor(Qt::black);
    TempPainter.setPen(Pen);
    TempPainter.drawText(QRectF(6+1,6+1,Width-12,Height-12),ShotDuration,Qt::AlignHCenter|Qt::AlignTop);
    Pen.setColor(RedColor?Qt::red:Qt::white);
    TempPainter.setPen(Pen);
    TempPainter.drawText(QRectF(6,6,Width-12,Height-12),ShotDuration,Qt::AlignHCenter|Qt::AlignTop);

    // -------------------------- Draw shot position
    ShotDuration=QTime(0,0,0,0).addMSecs(Position+Duration).toString("hh:mm:ss.zzz");
    Pen.setColor(Qt::black);
    TempPainter.setPen(Pen);
    TempPainter.drawText(QRectF(6+1,6+1,Width-12,Height-12),ShotDuration,Qt::AlignRight|Qt::AlignBottom);
    Pen.setColor(RedColor?Qt::red:Qt::white);
    TempPainter.setPen(Pen);
    TempPainter.drawText(QRectF(6,6,Width-12,Height-12),ShotDuration,Qt::AlignRight|Qt::AlignBottom);

    TempPainter.end();
    Painter->drawImage(option.rect.x(),option.rect.y(),TempImg);

    QApplication::restoreOverrideCursor();
}

//********************************************************************************************************************************
// cCustomShotTable : block table
//********************************************************************************************************************************

cCustomShotTable::cCustomShotTable(QWidget *parent):QTableWidget(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomShotTable::cCustomShotTable");

    DragItemSource  =-1;
    DragItemDest    =-1;
    IsDragOn        =0;
    DiaporamaObject =NULL;

    #if QT_VERSION >= 0x050000
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    #else
    horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    verticalHeader()->setResizeMode(QHeaderView::Fixed);
    #endif
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setDragDropOverwriteMode(false);
    setAcceptDrops(false);
    setDropIndicatorShown(false);

    setItemDelegate(new cShotTableItemDelegate(this));
    setRowCount(1);
}

//====================================================================================================================

void cCustomShotTable::mousePressEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomShotTable::mousePressEvent");
    QTableWidget::mousePressEvent(event);
    if ((IsDragOn==1)||(columnCount()==0)||(DiaporamaObject==NULL)) return;
    setCursor(Qt::ArrowCursor);
    IsDragOn=0;

    // Get item number under mouse
    int ThumbWidth =columnWidth(0);
    int Selected   =(event->pos().x()+horizontalOffset())/ThumbWidth;

    if ((Selected>=0)&&(Selected<NbrItem())) {
        // if item is correct, check if it was previously selected. Then if not select it
        if (Selected!=CurrentSelected()) SetCurrentCell(Selected); else {
            // if it was previously selected then start a drag & drop operation
            IsDragOn=1;
            DragItemSource=Selected;
            DragItemDest  =Selected;
            //RepaintCell(Selected);
            setCursor(Qt::ClosedHandCursor);
        }
    }
}

void cCustomShotTable::mouseMoveEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomShotTable::mouseMoveEvent");
    if (IsDragOn!=1) {
        setCursor(Qt::ArrowCursor);
        QTableWidget::mouseMoveEvent(event);
    } else {
        if ((columnCount()==0)||(DiaporamaObject==NULL)) return;

        int ThumbHeight=rowHeight(0);
        int ThumbWidth =columnWidth(0);
        int NbrX       =width()/ThumbWidth;
        int NbrY       =height()/ThumbHeight;  if (NbrY>rowCount()) NbrY=rowCount();
        int ToUse      =DragItemDest; if (DragItemDest==columnCount()) ToUse--;

        // Try to scroll left
        if ((event->pos().x()<0)&&(horizontalScrollBar()->value()>0)) horizontalScrollBar()->setValue(horizontalScrollBar()->value()-1);
        // Try to scroll right
        else if ((event->pos().x()>=NbrX*ThumbWidth)&&(horizontalScrollBar()->value()<horizontalScrollBar()->maximum())) horizontalScrollBar()->setValue(horizontalScrollBar()->value()+1);
        else {
            // Get item number under mouse
            int Selected=(event->pos().x()+horizontalOffset())/ThumbWidth;
            if ((Selected>NbrItem())||(Selected==DragItemSource)||((Selected==DragItemSource+1)&&(Selected!=NbrItem()))) {
                DragItemDest=-1;
                setCursor(Qt::ForbiddenCursor);
            } else {
                setCursor(Qt::ClosedHandCursor);
                DragItemDest=Selected;
            }
        }
        // Force a repaint
        setUpdatesEnabled(false);
        setUpdatesEnabled(true);
    }
}

void cCustomShotTable::mouseReleaseEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomShotTable::mouseReleaseEvent");
    setCursor(Qt::ArrowCursor);
    if (event->button()==Qt::RightButton) {
        emit RightClickEvent(event);
    } else if (IsDragOn!=1) {
        QTableWidget::mouseReleaseEvent(event);
    } else {
        if ((columnCount()==0)||(DiaporamaObject==NULL)) return;
        IsDragOn=false;
        if ((DragItemDest!=-1)&&(DragItemDest!=DragItemSource)&&
            ((DragItemDest<columnCount())||(DragItemSource!=columnCount()-1)))
            emit DragMoveItem();
    }
}

//====================================================================================================================

int cCustomShotTable::CurrentSelected() {
    return currentColumn();
}

//====================================================================================================================

int cCustomShotTable::NbrItem() {
    return columnCount();
}

//====================================================================================================================

void cCustomShotTable::SetCurrentCell(int Index) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomShotTable::SetCurrentCell");
    if ((Index<0)||(Index>=columnCount())) return;
    setCurrentCell(0,Index,QItemSelectionModel::Select|QItemSelectionModel::Current);
}

//====================================================================================================================

void cCustomShotTable::RepaintCell(int Index) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomShotTable::RepaintCell");
    if ((Index<0)||(Index>=columnCount())) return;

    update(model()->index(0,Index));
}
