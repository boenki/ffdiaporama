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

#ifndef cDriveList_H
#define cDriveList_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QString>
#include <QIcon>
#include <QList>

// Include some common various class
#include "cBaseApplicationConfig.h"
#include "cDriveList.h"

// class to handle one drive
class cDriveDesc {
public:
    QString     Path;
    QString     Label;
    qlonglong   Size;
    qlonglong   Used;
    qlonglong   Avail;
    QIcon       IconDrive;
    QString     Device;         // Linux only : associated device path (/dev/...)
    bool        IsReadOnly;
    int         Flag;           // Use by cDriveList::UpdateDriveList = 0=No longer exist, 1=Already, 2=New drive

    cDriveDesc(QString Path,QString Alias);
};

// class to handle drive list
class cDriveList {
public:
    QList<cDriveDesc>   List;                      // Table of alias for drives

    cDriveList();

    virtual void        UpdateDriveList();
    virtual QIcon       GetFolderIcon(QString FilePath);

private:
    bool                SearchDrive(QString Path);
};

#endif // cDriveList_H
