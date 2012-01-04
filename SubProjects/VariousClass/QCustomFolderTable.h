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

#ifndef QCUSTOMFOLDERTABLE_H
#define QCUSTOMFOLDERTABLE_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QWidget>
#include <QTableWidget>
#include <QString>

// Include some common various class
#include "cBaseApplicationConfig.h"
#include "cDriveList.h"
#include "cBaseMediaFile.h"

#define DISPLAY_DATA        0
#define DISPLAY_JUKEBOX     1
#define DISPLAY_WEB         2

class QCustomFolderTable : public QTableWidget {
Q_OBJECT
public:
    cDriveList              *DriveList;
    QList<cBaseMediaFile*>  MediaList;

    bool                    ShowHidden;                         // If true, hidden files will be show
    bool                    ShowMntDrive;                       // Show drives under /mnt/ [Linux only]
    bool                    ShowFoldersFirst;                   // If true, display folders at first in table list
    int                     CurrentMode;                        // Current display mode
    int                     CurrentFilter;                      // Current display mode
    QString                 CurrentPath;

    int                     CurrentShowFolderNumber;
    int                     CurrentTotalFilesNumber;
    int                     CurrentShowFilesNumber;
    qlonglong               CurrentShowFolderSize;
    qlonglong               CurrentTotalFolderSize;
    QTime                   CurrentShowDuration;

    // Thread controls
    QFutureWatcher<void>    ScanMediaList;
    bool                    StopScanMediaList;

    explicit                QCustomFolderTable(QWidget *parent = 0);
                            ~QCustomFolderTable();

    virtual void            SetMode(cDriveList *DriveList,int Mode,int Filter);
    virtual void            FillListFolder(QString Path,cBaseApplicationConfig *ApplicationConfig);

    virtual void            AppendMediaToTable(cBaseMediaFile *MediaObject);
    virtual void            UpdateMediaToTable(int Row,cBaseMediaFile *MediaObject);

    virtual cBaseMediaFile  *GetCurrentMediaFile();

signals:
    void    DoubleClickEvent();
    void    RefreshFolderInfo();
    void    NeedResizeColumns();
    void    UpdateItemIcon(QTableWidgetItem *,cBaseMediaFile *);

public slots:
    void    s_itemDoubleClicked();
    void    DoResizeColumns();
    void    DoUpdateItemIcon(QTableWidgetItem *Item,cBaseMediaFile *Media);

private:
    void                DoScanMediaList();
    QTableWidgetItem    *CreateItem(QString ItemText,int Alignment,QBrush Background);
};

#endif // QCUSTOMFOLDERTABLE_H
