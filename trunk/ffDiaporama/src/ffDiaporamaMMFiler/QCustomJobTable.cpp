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

// Basic inclusions (common to all files)
#include "../sharedfiles/_QCustomDialog.h"

#include "QCustomJobTable.h"

#include <QHeaderView>

//====================================================================================================================

QCustomJobTable::QCustomJobTable(QWidget *parent):QTableWidget(parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomJobTable::QCustomJobTable");

    JobQueue=NULL;
    ApplicationConfig=NULL;

    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setWordWrap(false);                 // Ensure no word wrap
    setTextElideMode(Qt::ElideNone);    // Ensure no line ellipsis (...)
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setShowGrid(false);

    horizontalHeader()->setSortIndicatorShown(false);
    horizontalHeader()->setCascadingSectionResizes(false);
    horizontalHeader()->show();
    horizontalHeader()->setClickable(false);
    horizontalHeader()->setMovable(false);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    horizontalHeader()->show();
    horizontalHeader()->setStretchLastSection(false);

    verticalHeader()->setStretchLastSection(false);
    verticalHeader()->setSortIndicatorShown(false);
    verticalHeader()->hide();
    verticalHeader()->setResizeMode(QHeaderView::Fixed);            // Fixed because ResizeToContents will be done after table filling

    QString ColumnDef=  QApplication::translate("QCustomJobTable","Date/Time")+";" \
                       +QApplication::translate("QCustomJobTable","Status")+";" \
                       +QApplication::translate("QCustomJobTable","Progress")+";" \
                       +QApplication::translate("QCustomJobTable","Job Type")+";" \
                       +QApplication::translate("QCustomJobTable","Current action");
    setColumnCount(ColumnDef.split(";").count());
    setHorizontalHeaderLabels(ColumnDef.split(";"));
}

//====================================================================================================================

void QCustomJobTable::DoResizeColumns() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomJobTable::DoResizeColumns");

    setUpdatesEnabled(false);
    setVisible(false);                      // To ensure all items of all columns are used to compute size
    resizeColumnsToContents();              // Resize column widht
    resizeRowsToContents();                 // Resize row height
    setVisible(true);                       // To allow display
    setUpdatesEnabled(true);
    this->viewport()->update();
}

//====================================================================================================================

QTableWidgetItem *QCustomJobTable::CreateItem(QString ItemText,int Alignment,QBrush Background) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomJobTable::CreateItem");

    QTableWidgetItem *Item=new QTableWidgetItem(ItemText);
    Item->setTextAlignment(Alignment);
    Item->setBackground(Background);
    return Item;
}

//====================================================================================================================

void QCustomJobTable::DoRefreshList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomJobTable::DoRefreshList");

    setUpdatesEnabled(false);
    while (rowCount()>0) removeRow(rowCount()-1);

    for (int Row=0;Row<JobQueue->List.count();Row++) {
        QColor  Background=((Row & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
        int     iCol=0;
        insertRow(Row);
        setItem(Row,iCol++,CreateItem(JobQueue->List[Row]->DateTime.toString("dd/MM/yyyy hh:mm:ss"),Qt::AlignLeft|Qt::AlignVCenter,Background));
        setItem(Row,iCol++,CreateItem(JobQueue->StatusText[JobQueue->List[Row]->JobStatus],         Qt::AlignLeft|Qt::AlignVCenter,Background));
        setItem(Row,iCol++,CreateItem(QString("%1%").arg(JobQueue->List[Row]->PercentDone),         Qt::AlignLeft|Qt::AlignVCenter,Background));
        setItem(Row,iCol++,CreateItem(JobQueue->JobTypeText[JobQueue->List[Row]->JobType],          Qt::AlignLeft|Qt::AlignVCenter,Background));
        setItem(Row,iCol++,CreateItem(JobQueue->List[Row]->CurrentAction,                           Qt::AlignLeft|Qt::AlignVCenter,Background));
    }
    DoResizeColumns(); // this do a setUpdatesEnabled(true);
}

//====================================================================================================================

void QCustomJobTable::DoRefreshAJob(int Row) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:QCustomJobTable::DoRefreshList");

    QColor  Background=((Row & 0x01)==0x01)?Qt::white:QColor(0xE0,0xE0,0xE0);
    setItem(Row,1,CreateItem(JobQueue->StatusText[JobQueue->List[Row]->JobStatus],  Qt::AlignLeft|Qt::AlignVCenter,Background));
    setItem(Row,2,CreateItem(QString("%1%").arg(JobQueue->List[Row]->PercentDone),  Qt::AlignLeft|Qt::AlignVCenter,Background));
    setItem(Row,4,CreateItem(JobQueue->List[Row]->CurrentAction,                    Qt::AlignLeft|Qt::AlignVCenter,Background));
}
