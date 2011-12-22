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

#ifndef CAPPLICATIONCONFIG_H
#define CAPPLICATIONCONFIG_H

// Include some standard class
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QTranslator>

// Include some various class
#include "cSaveWindowPosition.h"

// Utility functions
int     getCpuCount();                                                  // Retrieve number of processor
QString AdjustDirForOS(QString Dir);                                    // Adjust separator in pathname depending on operating system
bool    CheckFolder(QString FileToTest,QString PathToTest);             // Check if FileToTest exist in PathToTest and if yes the change current folder to PathToTest
bool    SetWorkingPath(char *argv[],QString ApplicationGroupName,QString ApplicationName,QString ConfigFileExt);                         // Adjust current folder

// enum types definitions
enum FilterFile {ALLFILE,IMAGEFILE,VIDEOFILE,MUSICFILE};
enum LoadConfigFileType {USERCONFIGFILE,GLOBALCONFIGFILE};

// Application config class
class cBaseApplicationConfig {
public:
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        // Options for linux only
        bool                RasterMode;                                 // Enable or disable raster mode [Linux only]
    #endif
    bool                    RestoreWindow;                              // If true, restore window state and position at startup
    #if defined(Q_OS_WIN)
        // Options for windows only
        // registry value for specific Windows Folder
        QString WINDOWS_APPDATA;                                        // specific Windows Folder : AppData
        QString WINDOWS_MUSIC;                                          // specific Windows Folder : My Music
        QString WINDOWS_PICTURES;                                       // specific Windows Folder : My Pictures
        QString WINDOWS_VIDEO;                                          // specific Windows Folder : My Video
        QString WINDOWS_DOCUMENTS;                                      // specific Windows Folder : Personal
    #endif

    QString                 ApplicationGroupName;                       // Private folder name to save user configuration file
    QString                 ApplicationName;                            // Application name
    QString                 ApplicationVersion;                         // Application version
    QString                 ConfigFileExt;                              // File extension of configuration files
    QString                 ConfigFileRootName;                         // Name of root node in the config xml file
    QStringList             AllowVideoExtension;                        // List of all file extension allowed for video
    QStringList             AllowImageExtension;                        // List of all file extension allowed for image
    QStringList             AllowMusicExtension;                        // List of all file extension allowed for music
    QString                 Plateforme;                                 // Operating system in use
    QWidget                 *ParentWindow;                              // Link to the top window
    QString                 UserConfigPath;                             // Path and filename to user profil path
    QString                 UserConfigFile;                             // Path and filename to user configuration file
    QString                 GlobalConfigFile;                           // Path and filename to global configuration file (in binary directory)
    QString                 CurrentFolder;                              // Current Path
    QString                 CurrentLanguage;                            // Current Language translation
    QString                 ForceLanguage;                              // Empty or forced language
    bool                    MainWinState;                               // WindowsSettings-ismaximized
    cSaveWindowPosition     *MainWinWSP;                                // MainWindow - Window size and position
    QTranslator             translator;                                 // translator for the application
    QTranslator             QTtranslator;                               // translator for QT default text

    cBaseApplicationConfig(QString ApplicationGroupName,QString ApplicationName,QString ApplicationVersion,QString ConfigFileExt,QString ConfigFileRootName);
    ~cBaseApplicationConfig();

    virtual QString         GetFilterForMediaFile(FilterFile type);
    virtual bool            InitConfigurationValues(QString ForceLanguage,QApplication *App);
    virtual bool            LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App);
    virtual bool            SaveConfigurationFile();

    // Abstract functions
    virtual void            InitValues()                                                                =0;
    virtual void            SaveValueToXML(QDomElement &domDocument)                                    =0;
    virtual bool            LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile) =0;
};

#endif // CAPPLICATIONCONFIG_H
