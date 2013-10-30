/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

// Somethings needed by libav
#ifdef _STDINT_H
    #undef _STDINT_H            // Remove previous inclusion (if exist)
#endif
#define __STDC_CONSTANT_MACROS  // Activate macro for stdint
#include <stdint.h>             // Include stdint with macro activated
#include <iostream>

#include "mainwindow.h"
#include <QTranslator>
#include <QtDebug>

int main(int argc, char* argv[]) {
    ToLog(LOGMSG_INFORMATION,QString("QT Version:%1").arg(qVersion()));

    SetWorkingPath(argv,APPLICATION_NAME,CONFIGFILEEXT);
    int     zero=1;
    char    *WM_NAME[]={(char *)"ffDiaporama"};
    QString AutoLoad="";
    QString ForceLanguage="";
    int     FuturLogMsgLevel=LogMsgLevel;

    ToLog(LOGMSG_INFORMATION,"Start application ...");

    #if defined(Q_OS_LINUX) || defined(Q_OS_SOLARIS)
        #if QT_VERSION >= 0x050000
        #else
            if (SearchRasterMode(APPLICATION_NAME,CONFIGFILEEXT,CONFIGFILE_ROOTNAME)) QApplication::setGraphicsSystem("raster");
        #endif
        QApplication app(zero,WM_NAME);
    #elif defined(Q_OS_WIN)
        #if QT_VERSION >= 0x050000
            QApplication app(zero,WM_NAME);
        #else
            // Check Windows version and :
            //      If <Windows/XP then exit application
            //      If Windows/XP then swith to low fragmentation heap mode
            //      If >Windows/XP then it's OK
            // And attach stdio to console if application was started from a console
            SetLFHeap();
            QThread::currentThread()->setPriority(QThread::HighestPriority);
            QApplication::setStyle("Cleanlooks");
            QApplication app(zero,WM_NAME);
        #endif
    #endif

    // Parse parameters to find ForceLanguage and AutoLoad
    ToLog(LOGMSG_INFORMATION,"Parse command line ...");
    for (int i=1;i<argc;i++) {
        QString Param=QString(argv[i]).toLower();
        if (Param.startsWith("-lang="))             ForceLanguage=Param.mid(QString("-lang=").length());
        else if (Param.startsWith("-loglevel="))    FuturLogMsgLevel=Param.mid(QString("-loglevel=").length()).toInt();
        else                                        AutoLoad=QString().fromLocal8Bit(argv[i]);
    }

    // Log Level part
    if ((FuturLogMsgLevel<1)||(FuturLogMsgLevel>4)) FuturLogMsgLevel=2;

    switch (FuturLogMsgLevel) {
        case 1 : ToLog(LOGMSG_INFORMATION,"Set LogLevel to DEBUGTRACE");    break;
        case 2 : ToLog(LOGMSG_INFORMATION,"Set LogLevel to INFORMATION");   break;
        case 3 : ToLog(LOGMSG_INFORMATION,"Set LogLevel to WARNING");       break;
        case 4 :
        default: ToLog(LOGMSG_INFORMATION,"Set LogLevel to CRITICAL");      break;
    }

    // Start GUI
    ToLog(LOGMSG_INFORMATION,"Start GUI ...");
    MainWindow w;
    w.InitWindow(ForceLanguage,&app);
    LogMsgLevel=FuturLogMsgLevel;
    if (w.ApplicationConfig->RestoreWindow && w.ApplicationConfig->MainWinState) w.showMaximized(); else w.show();

    if (AutoLoad!="") {
        w.FileForIO=AutoLoad;
        w.DoOpenFileParam();
    }

    ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Start ..."));

    return app.exec();
}
