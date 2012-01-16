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
    cApplicationConfig  *ApplicationConfig;
    cDriveList          *DriveList;
    bool                IsFirstInitDone;

    explicit            MainWindow(QWidget *parent = 0);
                        ~MainWindow();
    void                InitWindow(QString ForceLanguage,QApplication *App);
    void                RefreshControls();

protected:
    virtual void        resizeEvent(QResizeEvent *);
    virtual void        closeEvent(QCloseEvent *);

private slots:
    void                DoRefreshFolderInfo();

    void                s_currentTreeItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *);
    void                s_currentTableItemChanged();

    void                s_DlgCheckConfig();
    void                s_Refresh();
    void                s_OpenFile();
    void                s_InfoFile();
    void                s_ActionFile();
    void                s_itemDoubleClicked(QTableWidgetItem *);
    void                s_itemDoubleClicked();

    void                s_Config();
    void                s_action_Exit();

    void                s_action_Mode();
    void                s_action_Filter();

    // Help menu
    void                s_About();
    void                s_Documentation();
    void                s_NewFunctions();

private:
    QAction             *CreateMenuAction(QIcon Icon,QString Text,int Data,bool Checkable,bool IsCheck);
    QIcon               *GetIconMode();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
