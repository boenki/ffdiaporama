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

#include "mainwindow.h"
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication::setGraphicsSystem("raster");
    QApplication app(argc, argv);

    AddToSystemProperties(QString(APPLICATION_NAME)+" "+QString(APPLICATION_VERSION));
    AddSeparatorToSystemProperties();
    app.setApplicationName(APPLICATION_NAME);

    QString CurrentPath=QDir::currentPath();
    if (!CurrentPath.endsWith(QDir::separator())) CurrentPath=CurrentPath+QDir::separator();

    AddToSystemProperties("Starting path="+QDir::currentPath());
#if defined(Q_OS_WIN)
    if (!QFileInfo("ffDiaporama.xml").exists()) QDir::setCurrent(QString("..")+QDir().separator()+QString(APPLICATION_NAME));
    if (!QFileInfo("ffDiaporama.xml").exists()) QDir::setCurrent(QString(APPLICATION_NAME));
#endif

    // Ensure correct path
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (!QFileInfo("ffDiaporama.xml").exists()) QDir::setCurrent(QString("..")+QDir().separator()+QString(APPLICATION_NAME));
    if (!QFileInfo("ffDiaporama.xml").exists()) QDir::setCurrent(QString("/usr/share/")+QString(APPLICATION_NAME));
#endif
    AddToSystemProperties("Working path set to "+QDir::currentPath());


    // Search system language
    QTranslator translator;
    CurrentLanguage=QLocale::system().name().left(2);
    AddToSystemProperties("Detected system locale="+CurrentLanguage);

    // Validate if system locale is supported and if not force use of "en"
    if (CurrentLanguage!="fr") CurrentLanguage="en";

    // Install translation (if needed)
    if (CurrentLanguage!="en") {
        if (!translator.load(QString("locale")+QDir::separator()+QString("locale_")+CurrentLanguage+".qm"))
            ExitApplicationWithFatalError("Error loading translation file ...");
        app.installTranslator(&translator);
        AddToSystemProperties("Translation file loaded="+QDir().absoluteFilePath(QString("locale")+QDir::separator()+QString("locale_")+CurrentLanguage+".qm"));
    }
    AddSeparatorToSystemProperties();

    MainWindow w(CurrentPath);

    if (w.ApplicationConfig->RestoreWindow && w.ApplicationConfig->MainWinState) w.showMaximized(); else w.show();

    return app.exec();
}
