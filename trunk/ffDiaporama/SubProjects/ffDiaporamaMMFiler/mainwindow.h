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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QStringList>
#include <QMainWindow>
#include "../VariousClass/cBaseApplicationConfig.h"
#include "../VariousClass/QCustomFolderTree.h"
#include "../VariousClass/QCustomFolderTable.h"

#define APPLICATION_GROUPNAME               "ffDiaporama"
#define APPLICATION_NAME                    "ffDiaporamaMMFiler"
#define APPLICATION_VERSION                 "1.0 devel"
#define CONFIGFILEEXT                       ".xml"                                // File extension of configuration files
#define CONFIGFILE_ROOTNAME                 "Configuration"                       // Name of root node in the config xml file

namespace Ui {
    class MainWindow;
}

//**************************************

class cApplicationConfig : public cBaseApplicationConfig {
public:
    QString     SplitterSizeAndPos;                             // Splitter size save
    bool        ShowHiddenFilesAndDir;                          // If true, hidden files will be show
    bool        ShowMntDrive;                                   // If true, show drives under /mnt/ [Linux only]
    QString     CurrentPath;                                    // Currently shown path

    explicit cApplicationConfig();
    ~cApplicationConfig();

    virtual void    InitValues();
    virtual void    SaveValueToXML(QDomElement &domDocument);
    virtual bool    LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile);
};

//**************************************

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    cApplicationConfig  *ApplicationConfig;
    bool                IsFirstInitDone;

    explicit            MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void                InitWindow(QString ForceLanguage,QApplication *App);

protected:
    virtual void        resizeEvent(QResizeEvent *);
    virtual void        closeEvent(QCloseEvent *);

private slots:
    void                s_currentItemChanged(QTreeWidgetItem *current,QTreeWidgetItem *);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
