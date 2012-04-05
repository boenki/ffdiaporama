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

#include "mainwindow.h"
#include "wgt_QCustomThumbnails.h"
#include "cCustomTableWidget.h"


#include <QHeaderView>
#include <QScrollBar>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>

//********************************************************************************************************************
// cCustomTableWidget
//********************************************************************************************************************

cCustomTableWidget::cCustomTableWidget(QWidget *parent):QTableWidget(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::cCustomTableWidget");
    PartitionMode=false;
    horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    verticalHeader()->setResizeMode(QHeaderView::Fixed);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setDragDropOverwriteMode(false);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

//====================================================================================================================

void cCustomTableWidget::dragEnterEvent(QDragEnterEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::dragEnterEvent");
    GlobalMainWindow->IsDragOn=2;
    GlobalMainWindow->DragItemSource=-1;
    GlobalMainWindow->DragItemDest  =-1;
    setCursor(Qt::ClosedHandCursor);
    event->acceptProposedAction();
}

void cCustomTableWidget::dropEvent(QDropEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::dropEvent");
    GlobalMainWindow->IsDragOn=0;

    QList<QUrl> urlList;
    QString     fName;
    QFileInfo   info;

    if (event->mimeData()->hasUrls()) {
        urlList = event->mimeData()->urls(); // returns list of QUrls
        for (int i=0;i<urlList.count();i++) {
            fName = urlList[i].toLocalFile();           // convert first QUrl to local path
            info.setFile(fName);                        // information about file
            if (info.isFile()) GlobalMainWindow->FileList.append(fName);     // append file
        }
    }
    event->acceptProposedAction();
    if (GlobalMainWindow->FileList.count()>0) {
        GlobalMainWindow->SavedCurIndex =GlobalMainWindow->DragItemDest>0?GlobalMainWindow->DragItemDest-1:0;
        GlobalMainWindow->CurIndex      =GlobalMainWindow->DragItemDest;
        GlobalMainWindow->ToStatusBar(QApplication::translate("MainWindow","Add file to project :")+QFileInfo(GlobalMainWindow->FileList[0]).fileName());
        GlobalMainWindow->DoAddDragAndDropFile();
    }
}

void cCustomTableWidget::dragMoveEvent(QDragMoveEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::dragMoveEvent");
    int ThumbWidth  =GlobalMainWindow->Diaporama->GetWidthForHeight(GlobalMainWindow->ApplicationConfig->TimelineHeight-5)+32+ADJUSTXCOLUMN;
    int ThumbHeight =GlobalMainWindow->ApplicationConfig->TimelineHeight/2+GlobalMainWindow->ApplicationConfig->TimelineHeight+TIMELINESOUNDHEIGHT*2;
    int NbrX        =width()/ThumbWidth;
    int NbrY        =height()/ThumbHeight;  if (NbrY>rowCount()) NbrY=rowCount();
    int ToUse       =GlobalMainWindow->DragItemDest; if (GlobalMainWindow->DragItemDest==GlobalMainWindow->Diaporama->List.count()) ToUse--;
    int row         =PartitionMode?ToUse/NbrX:0;
    int col         =PartitionMode?ToUse-(ToUse/NbrX)*NbrX:ToUse;

    // Get item number under mouse
    int newrow=(event->pos().y()+verticalOffset())/ThumbHeight;
    int newcol=(event->pos().x()+horizontalOffset())/ThumbWidth;
    int Selected=(PartitionMode?newrow*NbrX+newcol:newcol);
    if (Selected>GlobalMainWindow->Diaporama->List.count()) Selected=GlobalMainWindow->Diaporama->List.count();
    if (Selected<0) Selected=0;

    if (Selected!=GlobalMainWindow->DragItemDest) {
        GlobalMainWindow->DragItemDest=-1;
        wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
        if (ItemToPaint) ItemToPaint->repaint();

        GlobalMainWindow->DragItemDest=Selected;
        int ToUse=GlobalMainWindow->DragItemDest;
        if (GlobalMainWindow->DragItemDest==GlobalMainWindow->Diaporama->List.count()) ToUse--;
        row=PartitionMode?ToUse/NbrX:0;
        col=PartitionMode?ToUse-(ToUse/NbrX)*NbrX:ToUse;
        ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
        if (ItemToPaint) ItemToPaint->repaint();
    }
    event->acceptProposedAction();
    setCursor(Qt::ClosedHandCursor);
}

//====================================================================================================================

void cCustomTableWidget::mousePressEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::mousePressEvent");
    QTableWidget::mousePressEvent(event);
    if (GlobalMainWindow->IsDragOn==1) return;
    setCursor(Qt::ArrowCursor);
    GlobalMainWindow->IsDragOn=0;

    // Get item number under mouse
    int ThumbWidth =GlobalMainWindow->Diaporama->GetWidthForHeight(GlobalMainWindow->ApplicationConfig->TimelineHeight-5)+32+ADJUSTXCOLUMN;
    int ThumbHeight=GlobalMainWindow->ApplicationConfig->TimelineHeight/2+GlobalMainWindow->ApplicationConfig->TimelineHeight+TIMELINESOUNDHEIGHT*2;
    int row=(event->pos().y()+verticalOffset())/ThumbHeight;
    int col=(event->pos().x()+horizontalOffset())/ThumbWidth;
    int NbrX       =width()/ThumbWidth;

    int Selected=(PartitionMode?row*NbrX+col:col);

    if ((Selected>=0)&&(Selected<NbrItem())) {
        // if item is correct, check if it was previously selected. Then if not select it
        if (Selected!=CurrentSelected()) SetCurrentCell(Selected); else {
            // if it was previously selected then start a drag & drop operation
            GlobalMainWindow->IsDragOn=1;
            GlobalMainWindow->DragItemSource=Selected;
            GlobalMainWindow->DragItemDest  =Selected;
            wgt_QCustomThumbnails *Previous=(wgt_QCustomThumbnails *)cellWidget(row,col); if (Previous) Previous->repaint();
            setCursor(Qt::ClosedHandCursor);
        }
    }
}

void cCustomTableWidget::mouseMoveEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::mouseMoveEvent");
    if (GlobalMainWindow->IsDragOn!=1) {
        QTableWidget::mouseMoveEvent(event);
    } else {
        int ThumbWidth  =GlobalMainWindow->Diaporama->GetWidthForHeight(GlobalMainWindow->ApplicationConfig->TimelineHeight-5)+32+ADJUSTXCOLUMN;
        int ThumbHeight =GlobalMainWindow->ApplicationConfig->TimelineHeight/2+GlobalMainWindow->ApplicationConfig->TimelineHeight+TIMELINESOUNDHEIGHT*2;
        int NbrX        =width()/ThumbWidth;
        int NbrY        =height()/ThumbHeight;  if (NbrY>rowCount()) NbrY=rowCount();
        int ToUse       =GlobalMainWindow->DragItemDest; if (GlobalMainWindow->DragItemDest==GlobalMainWindow->Diaporama->List.count()) ToUse--;
        int row         =PartitionMode?ToUse/NbrX:0;
        int col         =PartitionMode?ToUse-(ToUse/NbrX)*NbrX:ToUse;

        if (event->pos().x()<0) {
            if (GlobalMainWindow->DragItemDest!=-1) {
                wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
                GlobalMainWindow->DragItemDest=-1;
                if (ItemToPaint) ItemToPaint->repaint();
            }
            setCursor(Qt::ForbiddenCursor);
            // Try to scroll left if not partition mode
            if ((!PartitionMode)&&(horizontalScrollBar()->value()>0)) horizontalScrollBar()->setValue(horizontalScrollBar()->value()-1);

        } else if (event->pos().x()>=NbrX*ThumbWidth) {
            if (GlobalMainWindow->DragItemDest!=-1) {
                wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
                GlobalMainWindow->DragItemDest=-1;
                if (ItemToPaint) ItemToPaint->repaint();
            }
            setCursor(Qt::ForbiddenCursor);
            // Try to scroll right if not partition mode
            if ((!PartitionMode)&&(horizontalScrollBar()->value()<horizontalScrollBar()->maximum())) horizontalScrollBar()->setValue(horizontalScrollBar()->value()+1);

        } else if (event->pos().y()<0) {
            if (GlobalMainWindow->DragItemDest!=-1) {
                wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
                GlobalMainWindow->DragItemDest=-1;
                if (ItemToPaint) ItemToPaint->repaint();
                // Try to scroll up if partition mode
                if ((PartitionMode)&&(verticalScrollBar()->value()>0)) verticalScrollBar()->setValue(verticalScrollBar()->value()-1);
            }
            setCursor(Qt::ForbiddenCursor);

        } else if (event->pos().y()>=NbrY*ThumbHeight) {
            if (GlobalMainWindow->DragItemDest!=-1) {
                wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
                GlobalMainWindow->DragItemDest=-1;
                if (ItemToPaint) ItemToPaint->repaint();
            }
            setCursor(Qt::ForbiddenCursor);
            // Try to scroll down if partition mode
            if ((PartitionMode)&&(verticalScrollBar()->value()<verticalScrollBar()->maximum())) verticalScrollBar()->setValue(verticalScrollBar()->value()+1);

        } else {
            // Get item number under mouse
            int newrow  =(event->pos().y()+verticalOffset())/ThumbHeight;
            int newcol  =(event->pos().x()+horizontalOffset())/ThumbWidth;
            int Selected=(PartitionMode?newrow*NbrX+newcol:newcol);
            if ((Selected>NbrItem())||(Selected==GlobalMainWindow->DragItemSource)||(Selected==GlobalMainWindow->DragItemSource+1)) {
                if (GlobalMainWindow->DragItemDest!=-1) {
                    wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
                    GlobalMainWindow->DragItemDest=-1;
                    if (ItemToPaint) ItemToPaint->repaint();
                }
                setCursor(Qt::ForbiddenCursor);
            } else {
                setCursor(Qt::ClosedHandCursor);
                if (Selected!=GlobalMainWindow->DragItemDest) {
                    wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
                    GlobalMainWindow->DragItemDest=Selected;
                    if (ItemToPaint) ItemToPaint->repaint();
                    int ToUse=GlobalMainWindow->DragItemDest;
                    if (GlobalMainWindow->DragItemDest>=GlobalMainWindow->Diaporama->List.count()) ToUse--;
                    row=PartitionMode?ToUse/NbrX:0;
                    col=PartitionMode?ToUse-(ToUse/NbrX)*NbrX:ToUse;
                    ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(row,col);
                    if (ItemToPaint) ItemToPaint->repaint();
                }
            }
        }
    }
}

void cCustomTableWidget::mouseReleaseEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::mouseReleaseEvent");
    setCursor(Qt::ArrowCursor);
    if (event->button()==Qt::RightButton) {
        emit RightClickEvent(event);
    } else if (GlobalMainWindow->IsDragOn!=1) {
        QTableWidget::mouseReleaseEvent(event);
    } else {
        setCursor(Qt::ArrowCursor);
        GlobalMainWindow->IsDragOn=false;
        if (GlobalMainWindow->DragItemDest!=-1) emit DragMoveItem();
    }
}

//====================================================================================================================

void cCustomTableWidget::AddObjectToTimeLine(int CurIndex) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::AddObjectToTimeLine");
    int ThumbWidth =GlobalMainWindow->Diaporama->GetWidthForHeight(GlobalMainWindow->ApplicationConfig->TimelineHeight-5)+32+ADJUSTXCOLUMN;
    int ThumbHeight=GlobalMainWindow->ApplicationConfig->TimelineHeight/2+GlobalMainWindow->ApplicationConfig->TimelineHeight+TIMELINESOUNDHEIGHT*2;
    int NbrX       =viewport()->width()/ThumbWidth;

    wgt_QCustomThumbnails *ObjectBackground=new wgt_QCustomThumbnails(this,THUMBNAILTYPE_OBJECT);
    connect(ObjectBackground,SIGNAL(EditBackground()),      GlobalMainWindow,SLOT(s_Event_DoubleClickedOnBackground()));
    connect(ObjectBackground,SIGNAL(EditMediaObject()),     GlobalMainWindow,SLOT(s_Event_DoubleClickedOnObject()));
    connect(ObjectBackground,SIGNAL(EditTransition()),      GlobalMainWindow,SLOT(s_Event_DoubleClickedOnTransition()));
    connect(ObjectBackground,SIGNAL(EditSoundTrack()),      GlobalMainWindow,SLOT(s_Event_DoubleClickedOnVideoSound()));
    connect(ObjectBackground,SIGNAL(EditMusicTrack()),      GlobalMainWindow,SLOT(s_Event_DoubleClickedOnMusic()));

    if (PartitionMode) {
        // Partition mode
        CurIndex        =NbrItem();
        int CurrentRow  =CurIndex/NbrX;
        int CurrentCol  =CurIndex-CurrentRow*NbrX;
        if (CurrentRow>=rowCount()) {
            insertRow(CurrentRow);
            setRowHeight(CurrentRow,ThumbHeight);
            for (int i=0;i<columnCount();i++) setCellWidget(CurrentRow,i,new wgt_QCustomThumbnails(this,THUMBNAILTYPE_NULL));
        }
        if (CurrentCol>=columnCount()) {
            insertColumn(CurrentCol);
            setColumnWidth(CurrentCol,ThumbWidth);
        }
        setCellWidget(CurrentRow,CurrentCol,ObjectBackground);

    } else {
        // Preview mode
        if (rowCount()==0) {
            insertRow(0);
            setRowHeight(0,ThumbHeight);
        }
        insertColumn(CurIndex);
        setColumnWidth(CurIndex,ThumbWidth);
        setCellWidget(0,CurIndex,ObjectBackground);
    }

    if ((GlobalMainWindow->Diaporama->CurrentCol<0)||(GlobalMainWindow->Diaporama->CurrentCol==CurIndex)) {
        GlobalMainWindow->Diaporama->CurrentCol=CurIndex;
        GlobalMainWindow->AdjustRuller();
    }
}

//====================================================================================================================

void cCustomTableWidget::SetTimelineHeight(bool NewPartitionMode) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::SetTimelineHeight");
    int Selected=CurrentSelected();
    if (!NewPartitionMode) {
        setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        int FixedHeight=15+                                                         // Horizontal slider and marges
                        GlobalMainWindow->ApplicationConfig->TimelineHeight/2+      // Background
                        GlobalMainWindow->ApplicationConfig->TimelineHeight+        // Montage
                        TIMELINESOUNDHEIGHT*2+4;                                    // Music
        setFixedHeight(FixedHeight);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else {
        setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        setMaximumHeight(QWIDGETSIZE_MAX);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    QApplication::processEvents(); // Give time to Qt to redefine position of each control and timeline height !
    PartitionMode=NewPartitionMode;
    ResetDisplay(Selected);
}

//====================================================================================================================

void cCustomTableWidget::ResetDisplay(int Selected) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::ResetDisplay");
    GlobalMainWindow->FLAGSTOPITEMSELECTION=true;
    setUpdatesEnabled(false);
    CleanAll();
    for (int i=0;i<GlobalMainWindow->Diaporama->List.count();i++) AddObjectToTimeLine(i);
    GlobalMainWindow->FLAGSTOPITEMSELECTION=false;
    SetCurrentCell(Selected);
    setUpdatesEnabled(true);  // Reset timeline painting
}

//====================================================================================================================

int cCustomTableWidget::CurrentSelected() {
    int NbrX=width()/(GlobalMainWindow->Diaporama->GetWidthForHeight(GlobalMainWindow->ApplicationConfig->TimelineHeight-5)+32+ADJUSTXCOLUMN);
    int Selected=(PartitionMode?currentRow()*NbrX+currentColumn():currentColumn());
    return Selected;
}

//====================================================================================================================

int cCustomTableWidget::NbrItem() {
    if (PartitionMode) {
        int NbrCol=width()/(GlobalMainWindow->Diaporama->GetWidthForHeight(GlobalMainWindow->ApplicationConfig->TimelineHeight-5)+32+ADJUSTXCOLUMN);
        int NbrRow=rowCount();
        if (NbrRow>0) {
            NbrRow--;
            int z=0;
            while ((cellWidget(NbrRow,z)!=NULL)&&(((wgt_QCustomThumbnails *)cellWidget(NbrRow,z))->Type!=THUMBNAILTYPE_NULL)) z++;
            return NbrRow*NbrCol+z;
        } else return 0;
    } else return columnCount();
}

//====================================================================================================================

void cCustomTableWidget::SetCurrentCell(int Index) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::SetCurrentCell");
    setUpdatesEnabled(false);
    if (GlobalMainWindow->ApplicationConfig->PartitionMode) {
        int NbrCol=width()/(GlobalMainWindow->Diaporama->GetWidthForHeight(GlobalMainWindow->ApplicationConfig->TimelineHeight-5)+32+ADJUSTXCOLUMN);
        if (NbrCol) setCurrentCell(Index/NbrCol,Index-(Index/NbrCol)*NbrCol);
     } else setCurrentCell(0,Index);
    setUpdatesEnabled(true);
}

//====================================================================================================================

void cCustomTableWidget::CleanAll() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget::CleanAll");
    setUpdatesEnabled(false);
    for (int i=0;i<rowCount();i++) for (int j=0;j<columnCount();j++) if (cellWidget(i,j)!=NULL) removeCellWidget(i,j);
    while (columnCount()>0) removeColumn(columnCount()-1);
    while (rowCount())      removeRow(rowCount()-1);
    setUpdatesEnabled(true);
}

//********************************************************************************************************************
// cCustomTableWidget2
//********************************************************************************************************************

cCustomTableWidget2::cCustomTableWidget2(QWidget *parent):QTableWidget(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget2::cCustomTableWidget2");
    horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    verticalHeader()->setResizeMode(QHeaderView::Fixed);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setDragDropOverwriteMode(false);
    setAcceptDrops(false);
    setDropIndicatorShown(false);
}

//====================================================================================================================

void cCustomTableWidget2::mousePressEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget2::mousePressEvent");
    QTableWidget::mousePressEvent(event);
    if (GlobalMainWindow->IsDragOn==1) return;
    setCursor(Qt::ArrowCursor);
    GlobalMainWindow->IsDragOn=0;

    // Get item number under mouse
    int ThumbHeight=rowHeight(0);
    int ThumbWidth =GlobalMainWindow->Diaporama->GetWidthForHeight(ThumbHeight);
    int row        =(event->pos().y()+verticalOffset())/ThumbHeight;
    int col        =(event->pos().x()+horizontalOffset())/ThumbWidth;
    int Selected   =col;

    if ((Selected>=0)&&(Selected<NbrItem())) {
        // if item is correct, check if it was previously selected. Then if not select it
        if (Selected!=CurrentSelected()) SetCurrentCell(Selected); else {
            // if it was previously selected then start a drag & drop operation
            GlobalMainWindow->IsDragOn=1;
            GlobalMainWindow->DragItemSource=Selected;
            GlobalMainWindow->DragItemDest  =Selected;
            wgt_QCustomThumbnails *Previous=(wgt_QCustomThumbnails *)cellWidget(row,col); if (Previous) Previous->repaint();
            setCursor(Qt::ClosedHandCursor);
        }
    }
}

void cCustomTableWidget2::mouseMoveEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget2::mouseMoveEvent");
    if (GlobalMainWindow->IsDragOn!=1) {
        setCursor(Qt::ArrowCursor);
        QTableWidget::mouseMoveEvent(event);
    } else {
        int ThumbHeight=rowHeight(0);
        int ThumbWidth =GlobalMainWindow->Diaporama->GetWidthForHeight(ThumbHeight);
        int NbrX       =width()/ThumbWidth;
        int NbrY       =height()/ThumbHeight;  if (NbrY>rowCount()) NbrY=rowCount();
        int ToUse      =GlobalMainWindow->DragItemDest; if (GlobalMainWindow->DragItemDest==columnCount()) ToUse--;
        int col        =ToUse;

        if (event->pos().x()<0) {
            if (GlobalMainWindow->DragItemDest!=-1) {
                wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(0,col);
                GlobalMainWindow->DragItemDest=-1;
                if (ItemToPaint) ItemToPaint->repaint();
            }
            setCursor(Qt::ForbiddenCursor);
            // Try to scroll left if not partition mode
            if ((horizontalScrollBar()->value()>0)) horizontalScrollBar()->setValue(horizontalScrollBar()->value()-1);

        } else if (event->pos().x()>=NbrX*ThumbWidth) {
            if (GlobalMainWindow->DragItemDest!=-1) {
                wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(0,col);
                GlobalMainWindow->DragItemDest=-1;
                if (ItemToPaint) ItemToPaint->repaint();
            }
            setCursor(Qt::ForbiddenCursor);
            // Try to scroll right if not partition mode
            if ((horizontalScrollBar()->value()<horizontalScrollBar()->maximum())) horizontalScrollBar()->setValue(horizontalScrollBar()->value()+1);

        } else {
            // Get item number under mouse
            int Selected=(event->pos().x()+horizontalOffset())/ThumbWidth;
            if ((Selected>NbrItem())||(Selected==GlobalMainWindow->DragItemSource)||((Selected==GlobalMainWindow->DragItemSource+1)&&(Selected!=NbrItem()))) {
                if (GlobalMainWindow->DragItemDest!=-1) {
                    wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(0,col);
                    GlobalMainWindow->DragItemDest=-1;
                    if (ItemToPaint) ItemToPaint->repaint();
                }
                setCursor(Qt::ForbiddenCursor);
            } else {
                setCursor(Qt::ClosedHandCursor);
                if (Selected!=GlobalMainWindow->DragItemDest) {
                    wgt_QCustomThumbnails *ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(0,col);
                    GlobalMainWindow->DragItemDest=Selected;
                    if (ItemToPaint) ItemToPaint->repaint();
                    col=GlobalMainWindow->DragItemDest;
                    if (GlobalMainWindow->DragItemDest>=columnCount()) col--;
                    ItemToPaint=(wgt_QCustomThumbnails *)cellWidget(0,col);
                    if (ItemToPaint) ItemToPaint->repaint();
                }
            }
        }
    }
}

void cCustomTableWidget2::mouseReleaseEvent(QMouseEvent *event) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget2::mouseReleaseEvent");
    setCursor(Qt::ArrowCursor);
    if (event->button()==Qt::RightButton) {
        emit RightClickEvent(event);
    } else if (GlobalMainWindow->IsDragOn!=1) {
        QTableWidget::mouseReleaseEvent(event);
    } else {
        GlobalMainWindow->IsDragOn=false;
        if ((GlobalMainWindow->DragItemDest!=-1)&&(GlobalMainWindow->DragItemDest!=GlobalMainWindow->DragItemSource)&&
            ((GlobalMainWindow->DragItemDest<columnCount())||(GlobalMainWindow->DragItemSource!=columnCount()-1)))
            emit DragMoveItem();
    }
}

//====================================================================================================================

int cCustomTableWidget2::CurrentSelected() {
    return currentColumn();
}

//====================================================================================================================

int cCustomTableWidget2::NbrItem() {
    return columnCount();
}

//====================================================================================================================

void cCustomTableWidget2::SetCurrentCell(int Index) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCustomTableWidget2::SetCurrentCell");
    setUpdatesEnabled(false);
    setCurrentCell(0,Index,QItemSelectionModel::Select|QItemSelectionModel::Current);
    setUpdatesEnabled(true);
}

