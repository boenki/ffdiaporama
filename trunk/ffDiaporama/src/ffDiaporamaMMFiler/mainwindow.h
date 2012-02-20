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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QString>
#include <QStringList>
#include <QMainWindow>
#include <QFutureWatcher>

#include "../sharedfiles/cBaseApplicationConfig.h"
#include "../sharedfiles/cDriveList.h"
#include "../sharedfiles/QCustomFolderTree.h"
#include "../sharedfiles/QCustomFolderTable.h"

#include "cApplicationConfig.h"

namespace Ui {
    class MainWindow;
}

//**************************************

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    cApplicationConfig      *ApplicationConfig;
    cDriveList              *DriveList;
    bool                    IsFirstInitDone;
    cJobQueue               JobQueue;
    QTimer                  Timer;
    QFutureWatcher<void>    Thread;
    int                     CurrentJobThread;

    explicit            MainWindow(QWidget *parent = 0);
                        ~MainWindow();
    void                InitWindow(QString ForceLanguage,QApplication *App);
    void                RefreshControls();

protected:
    virtual void        resizeEvent(QResizeEvent *);
    virtual void        showEvent(QShowEvent *);
    virtual void        closeEvent(QCloseEvent *);
    virtual void        customEvent(QEvent *);

private slots:

    void                DoMaximized();                                  // Use on init to force window maximized
    void                DoRefreshFolderInfo();

    void                s_TimerEvent();
    void                ThreadJob();

    void                s_currentTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *);
    void                DoRefreshSelectedFileInfo();

    void                s_DlgCheckConfig();
    void                s_Refresh();
    void                s_Action_OpenFile();
    void                s_Action_InfoFile();
    void                s_Action_RemoveFile();
    void                s_Action_RemoveFolder();
    void                s_Action_WizardOnFile();
    void                s_itemDoubleClicked(QMouseEvent *ev);   // do a s_Action_OpenFile
    void                s_itemRightClicked(QMouseEvent *ev);    // do a s_Action_WizardOnFile

    void                s_Config();
    void                s_Action_Exit();

    void                s_Action_Mode();
    void                s_Action_Filter();

    // Help menu
    void                s_About();
    void                s_Documentation();
    void                s_NewFunctions();

    // Actions
    bool                DoDirectJob_RemoveFolder(QString FolderPath);
    void                DoAddJob_Convert(QList<cBaseMediaFile*>*MediaList,int JobType);

private:
    QAction             *CreateMenuAction(QImage *Icon,QString Text,int Data,bool Checkable,bool IsCheck);
    QAction             *CreateMenuAction(QIcon Icon,QString Text,int Data,bool Checkable,bool IsCheck);
    QIcon               *GetIconMode();
    QIcon               *GetIconFilter();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
