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

#ifndef QCUSTOMFOLDERTREE_H
#define QCUSTOMFOLDERTREE_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QIcon>

extern QString IconsPath;

// Utility function
QString GetTextSize(qlonglong Size);

class cHardDriveDescriptor {
public:
    QString     Path;
    QString     Label;
    qlonglong   Size;
    qlonglong   Used;
    qlonglong   Avail;
    QIcon       IconDrive;
    QString     Device;         // Linux only : associated device path (/dev/...)
    bool        IsReadOnly;

    cHardDriveDescriptor(QString Path,QString Alias);
};

class QCustomFolderTree : public QTreeWidget {
Q_OBJECT
public:
    bool                            ShowHidden;                         // If true, hidden files will be show
    bool                            ShowMntDrive;                       // Show drives under /mnt/ [Linux only]
    QList<cHardDriveDescriptor>     HardDrive;                          // Table of alias for drives

    explicit        QCustomFolderTree(QWidget *parent=0);

    // Public utility functions
    virtual QString                 GetFolderPath(const QTreeWidgetItem *current,bool TreeMode);
    virtual QString                 GetCurrentFolderPath();
    virtual void                    SetSelectItemByPath(QString Path);
    virtual void                    RefreshItemByPath(QString Path,int Level=0);
    virtual void                    CreateHardDriveList();
    virtual QIcon                   GetIcon(QString FilePath);
    virtual QString                 RealPathToTreePath(QString Path);
    virtual cHardDriveDescriptor    *SearchRealDrive(QString Path);

private:
    QTreeWidgetItem *CreateItem(QString Text,QString FilePath,QIcon Icon);
    bool            IsFolderHaveChild(QString Folder);
    bool            IsReadOnlyDrive(QString Folder);

signals:

public slots:
    void    s_itemExpanded(QTreeWidgetItem *item);

};

#endif // QCUSTOMFOLDERTREE_H
