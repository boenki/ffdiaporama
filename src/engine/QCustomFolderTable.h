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
#include <QFutureWatcher>

// Include some common various class
#include "cBaseApplicationConfig.h"
#include "cBaseMediaFile.h"

class QCustomFolderTable : public QTableWidget {
Q_OBJECT
public:
    QStringList             BrowsePathList;
    QList<cBaseMediaFile*>  MediaList;
    cBaseApplicationConfig  *ApplicationConfig;

    QString                 CurrentPath;

    int                     CurrentShowFolderNumber;
    int                     CurrentTotalFilesNumber;
    int                     CurrentShowFilesNumber;
    int                     CurrentDisplayItem;
    qlonglong               CurrentShowFolderSize;
    qlonglong               CurrentTotalFolderSize;
    qlonglong               CurrentShowDuration;

    // Thread controls
    QFutureWatcher<void>    ScanMediaList;
    bool                    StopScanMediaList;
    bool                    StopAllEvent;
    bool                    ScanMediaListProgress;
    bool                    InScanMediaFunction;

    QAbstractItemModel      *DefaultModel;
    QAbstractItemDelegate   *DefaultDelegate,*IconDelegate;

    explicit                QCustomFolderTable(QWidget *parent = 0);
                            ~QCustomFolderTable();

    virtual void            EnsureThreadIsStopped();

    virtual void            resizeEvent(QResizeEvent *);
    virtual void            mouseDoubleClickEvent(QMouseEvent *);
    virtual void            mouseReleaseEvent(QMouseEvent *);

    virtual void            SetMode(int Mode,int Filter);
    virtual void            FillListFolder(QString Path);
    virtual void            RefreshListFolder();

    virtual void            AppendMediaToTable(cBaseMediaFile *MediaObject);

    virtual QImage          *GetImageForColumn(int Col,cBaseMediaFile *MediaObject);
    virtual QString         GetTextForColumn(int Col,cBaseMediaFile *MediaObject);
    virtual int             GetAlignmentForColumn(int Col);

    virtual cBaseMediaFile          *GetCurrentMediaFile();
    virtual QList<cBaseMediaFile*>  GetCurrentSelectedMediaFile() const;

    virtual bool            CanBrowseToPreviousPath();
    virtual QString         BrowseToPreviousPath();
    virtual bool            CanBrowseToUpperPath();
    virtual QString         BrowseToUpperPath();

protected:
    virtual QMimeData       *mimeData(const QList <QTableWidgetItem *>) const;
    virtual void            keyReleaseEvent(QKeyEvent *event);

signals:
    void    DoubleClickEvent(QMouseEvent *ev);
    void    RightClickEvent(QMouseEvent *ev);
    void    RefreshFolderInfo();
    void    NeedResizeColumns();
    void    RemoveFiles();
    void    InsertFiles();
    void    OpenFiles();
    void    RenameFiles();
    void    Refresh();

public slots:
    void    DoResizeColumns();

private:
    void                DoScanMediaList();
    int                 GetWidthForIcon();
    int                 GetHeightForIcon();
    QTableWidgetItem    *CreateItem(QString ItemText,int Alignment,QBrush Background);
};

#endif // QCUSTOMFOLDERTABLE_H
