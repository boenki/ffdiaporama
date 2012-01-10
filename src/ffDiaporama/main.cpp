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

// Somethings needed by ffmpeg
#ifdef _STDINT_H
    #undef _STDINT_H            // Remove previous inclusion (if exist)
#endif
#define __STDC_CONSTANT_MACROS  // Activate macro for stdint
#include <stdint.h>             // Include stdint with macro activated

#include "_GlobalDefines.h"
#include "mainwindow.h"
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication app(argc,argv);

    SetWorkingPath(argv,APPLICATION_NAME,APPLICATION_NAME,CONFIGFILEEXT);

    QString AutoLoad="";
    QString ForceLanguage="";

    // Parse parameters to find ForceLanguage and AutoLoad
    for (int i=1;i<argc;i++) {
        QString Param=QString(argv[i]).toLower();
        if (Param.startsWith("-lang=")) ForceLanguage=Param.mid(QString("-lang=").length());
        else AutoLoad=QString().fromLocal8Bit(argv[i]);
    }

    MainWindow w;
    w.InitWindow(ForceLanguage,&app);

    if (w.ApplicationConfig->RestoreWindow && w.ApplicationConfig->MainWinState) w.showMaximized(); else w.show();

    if (AutoLoad!="") {
        w.FileForIO=AutoLoad;
        w.DoOpenFileParam();
    }
    return app.exec();
}
