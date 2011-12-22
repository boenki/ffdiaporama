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

#include <QtDebug>
#include <QtGui/QApplication>
#include <QFileInfoList>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QHeaderView>
#include <QTableWidgetItem>
#include "QCustomFolderTable.h"

#define FILETABLESTATE_FILETOCHEK   1
#define FileToCheckIcon             "img/player_time.png"

QCustomFolderTable::QCustomFolderTable(QWidget *parent):QTableWidget(parent) {
}

//====================================================================================================================

void QCustomFolderTable::SetMode(QCustomFolderTree *AssociatedTree,int Mode) {
    Tree=AssociatedTree;

    // Reset content
    clearContents();
    while (rowCount()>0)    removeRow(0);
    while (columnCount()>0) removeColumn(0);

    switch (Mode) {
        case DISPLAY_DATA :
            setRowCount(0);
            setColumnCount(3);
            setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            horizontalHeader()->show();
            setHorizontalHeaderLabels(QString("File;Size;Date").split(";"));
            horizontalHeader()->setStretchLastSection(false);
            horizontalHeader()->setSortIndicatorShown(false);
            horizontalHeader()->setCascadingSectionResizes(false);
            horizontalHeader()->setClickable(false);
            horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
            horizontalHeader()->setMovable(false);
            horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
            horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
            verticalHeader()->hide();
            verticalHeader()->setStretchLastSection(false);
            verticalHeader()->setSortIndicatorShown(false);
            verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
            break;
    }
}

//====================================================================================================================

int QCustomFolderTable::FillListFolder(QString Path) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    // Reset content
    clearContents();
    while (rowCount()>0) removeRow(0);

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (Path.startsWith("~")) Path=QDir::homePath()+Path.mid(2);
    #endif

    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();

    // Scan files and add them to table
    QDir                Folder(Path);
    QFileInfoList       Files=Folder.entryInfoList();;
    QTableWidgetItem    *Item;
    int                 Row,Col;

    foreach(QFileInfo File,Files) if ((File.fileName()!=".")&&(File.fileName()!="..")) {
        Row=rowCount();
        insertRow(Row);
        verticalHeader()->setResizeMode(Row,QHeaderView::Fixed);
        Col=0;
        Item=new QTableWidgetItem(File.fileName());
        Item->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        Item->setData(Qt::UserRole,QVariant(int(FILETABLESTATE_FILETOCHEK)));
        if (File.isDir()) Item->setIcon(Tree->GetIcon(File.absoluteFilePath())); else Item->setIcon(QIcon(FileToCheckIcon));
        setItem(Row,Col++,Item);

        Item=new QTableWidgetItem(GetTextSize(File.size()));
        Item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        setItem(Row,Col++,Item);
    }
    //resizeRowsToContents();
    //resizeColumnsToContents();
    QApplication::restoreOverrideCursor();

    return Files.count();
}
