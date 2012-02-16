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

#include <QtGui/QApplication>
#include <QString>
#include <QDir>
#include <stdio.h>

#include "../sharedfiles/cBaseApplicationConfig.h"
#include "mainwindow.h"
#include <iostream>

int main(int argc, char *argv[]) {

    #ifdef Q_OS_WIN
    // Check Windows version and :
    //      If <Windows/XP then exit application
    //      If Windows/XP then swith to low fragmentation heap mode
    //      If >Windows/XP then it's OK
    // And attach stdio to console if application was started from a console
    SetLFHeap();
    #endif

    SetWorkingPath(argv,APPLICATION_GROUPNAME,APPLICATION_NAME,CONFIGFILEEXT);
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        if (SearchRasterMode(APPLICATION_NAME,APPLICATION_NAME,CONFIGFILEEXT,CONFIGFILE_ROOTNAME)) QApplication::setGraphicsSystem("raster");
    #endif

    QApplication::setStyle("Cleanlooks");

    QApplication app(argc, argv);

    QString ForceLanguage="";

    // Parse parameters
    for (int i=1;i<argc;i++) {
        QString Param=QString(argv[i]).toLower();
        if (Param.startsWith("-lang="))             ForceLanguage=Param.mid(QString("-lang=").length());
        else if (Param.startsWith("-loglevel="))    LogMsgLevel  =Param.mid(QString("-loglevel=").length()).toInt();
    }

    // Log Level part
    switch (LogMsgLevel) {
        case 1 : std::cout << QString("Set LogLevel to DEBUGTRACE\n").toLocal8Bit().constData();    break;
        case 2 : std::cout << QString("Set LogLevel to INFORMATION\n").toLocal8Bit().constData();   break;
        case 3 : std::cout << QString("Set LogLevel to WARNING\n").toLocal8Bit().constData();       break;
        case 4 : std::cout << QString("Set LogLevel to CRITICAL\n").toLocal8Bit().constData();      break;
        default :std::cout << QString("Incorrect LogLevel\n").toLocal8Bit().constData();            LogMsgLevel=2;   exit(1);    break;
    }

    // Start GUI
    MainWindow w;
    w.InitWindow(ForceLanguage,&app);

    if (w.ApplicationConfig->RestoreWindow && w.ApplicationConfig->MainWinState) w.showMaximized(); else w.show();

    return app.exec();
}
