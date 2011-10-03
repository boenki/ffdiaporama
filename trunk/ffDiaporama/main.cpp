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
    // Initialise configuration options
    cApplicationConfig *ApplicationConfig=new cApplicationConfig();
    ApplicationConfig->ParentWindow=NULL;
    ApplicationConfig->InitConfigurationValues();                                                               // Init all values
    ApplicationConfig->LoadConfigurationFile(GLOBALCONFIGFILE);                                                 // Get values from global configuration file (overwrite previously initialized values)
    if (!ApplicationConfig->LoadConfigurationFile(USERCONFIGFILE)) ApplicationConfig->SaveConfigurationFile();  // Load values from user configuration file (overwrite previously initialized values)

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        if (ApplicationConfig->RasterMode) QApplication::setGraphicsSystem("raster");
    #endif

    QApplication app(argc, argv);

    app.setApplicationName(QString(APPLICATION_NAME)+QString(" ")+QString(APPLICATION_VERSION));

    // Search system language
    QTranslator translator;     // translator for the application
    QTranslator QTtranslator;   // translator for QT default text
    CurrentLanguage=QLocale::system().name().left(2);
    AddToSystemProperties(QString(SYSTEMLOCAL_STR)+CurrentLanguage);

    QString AutoLoad="";

    // Parse parameters
    for (int i=1;i<argc;i++) {
        QString Param=QString(argv[i]).toLower();
        if (Param.startsWith("-lang=")) CurrentLanguage=Param.mid(QString("-lang=").length());
        else AutoLoad=QString().fromLocal8Bit(argv[i]);
    }

    // Validate if system locale is supported and if not force use of "en"
    if ((CurrentLanguage!="fr")&&(CurrentLanguage!="es")&&(CurrentLanguage!="it")&&(CurrentLanguage!="en")&&(CurrentLanguage!="ru")) CurrentLanguage="en";

    // Install translation (if needed)
    if (CurrentLanguage!="en") {
        if (!translator.load(AdjustDirForOS(QString("locale")+QDir::separator()+QString("locale_")+CurrentLanguage+".qm")))
            ExitApplicationWithFatalError("Error loading translation file ...");
        if (!QTtranslator.load(AdjustDirForOS(QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+".qm")))
            AddToSystemProperties("Error loading QT translation file ...");

        app.installTranslator(&translator);
        app.installTranslator(&QTtranslator);
        AddToSystemProperties(QString(LOADEDLOCAL_STR)+AdjustDirForOS(QDir().absoluteFilePath(QString("locale")+QDir::separator()+QString("locale_")+CurrentLanguage+".qm")));
    }
    AddSeparatorToSystemProperties();
    Transparent.setTextureImage(QImage("img/transparent.png"));  // Load transparent brush

    MainWindow w(ApplicationConfig);

    if (w.ApplicationConfig->RestoreWindow && w.ApplicationConfig->MainWinState) w.showMaximized(); else w.show();
    if (AutoLoad!="") w.OpenFile(AutoLoad);
    return app.exec();
}
