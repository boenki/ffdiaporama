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

#ifndef CBASEAPPLICATIONCONFIG_H
#define CBASEAPPLICATIONCONFIG_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QTranslator>
#include <QDomElement>
#include <QDomDocument>
#include <QMainWindow>

// Include some common various class
#include "cSaveWindowPosition.h"
#include "cLuLoImageCache.h"

// Standard geometry definition
#define GEOMETRY_4_3                        0
#define GEOMETRY_16_9                       1
#define GEOMETRY_40_17                      2
#define NBR_GEOMETRY_DEF                    3

// Default Icon
extern QString IconsPath;                      // Linux path where system icons where found (/usr/share/icons/gnome or /usr/share/icons/default.kde4
extern QIcon   DefaultCDROMIcon;
extern QIcon   DefaultHDDIcon;
extern QIcon   DefaultUSBIcon;
extern QIcon   DefaultREMOTEIcon;
extern QIcon   DefaultUSERIcon;
extern QIcon   DefaultFOLDERIcon;
extern QIcon   DefaultFILEIcon;
extern QIcon   DefaultIMAGEIcon;
extern QIcon   DefaultVIDEOIcon;
extern QIcon   DefaultMUSICIcon;
extern QIcon   DefaultFFDIcon;

// Utility functions
void    PreloadSystemIcons();
int     getCpuCount();                                                                                              // Retrieve number of processor
QString AdjustDirForOS(QString Dir);                                                                                // Adjust separator in pathname depending on operating system
QString GetTextSize(qlonglong Size);                                                                                // transform a size (_int64) in a string with apropriate unit (Gb/Tb...)
bool    CheckFolder(QString FileToTest,QString PathToTest);                                                         // Check if FileToTest exist in PathToTest and if yes the change current folder to PathToTest
bool    SetWorkingPath(char *argv[],QString ApplicationGroupName,QString ApplicationName,QString ConfigFileExt);    // Adjust current folder

// enum types definitions
enum    FilterFile {ALLFILE,IMAGEFILE,VIDEOFILE,MUSICFILE};
enum    LoadConfigFileType {USERCONFIGFILE,GLOBALCONFIGFILE};

// Application config class
class cBaseApplicationConfig {
public:
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        bool                RasterMode;                                 // Enable or disable raster mode [Linux only]
    #endif
    bool                    RestoreWindow;                              // If true, restore window state and position at startup
    QMainWindow             *TopLevelWindow;                            // Link to MainWindow of the application
    QString                 AllowedWEBLanguage;

    // Image cache
    cLuLoImageCache         ImagesCache;                                // cLuLoImageCache List Object

    // Last directories
    bool                    RememberLastDirectories;                    // If true, Remember all directories for future use
    QString                 LastMediaPath;                              // Last folder use for image/video
    QString                 LastMusicPath;                              // Last folder use for music

    // Other
    bool                    Smoothing;                                  // True do smoothing in preview
    bool                    Crop1088To1080;                             // Automaticaly crop video from 1088 lines to 1080 (CANON)
    bool                    QuickResamplingPreview;                     // If true then use quick resampling during preview
    bool                    ApplyTransfoPreview;                        // True if image transformation are apply during preview
    bool                    CheckConfigAtStartup;                       // If true, check config at startup

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
    QString                 PathEXIV2;                                  // Filename with path to exiv2 binary
    QString                 PathFFMPEG;                                 // Filename with path to ffmpeg binary
    bool                    MainWinState;                               // WindowsSettings-ismaximized
    cSaveWindowPosition     *MainWinWSP;                                // MainWindow - Window size and position
    QTranslator             translator;                                 // translator for the application
    QTranslator             QTtranslator;                               // translator for QT default text

    cBaseApplicationConfig(QMainWindow *TopLevelWindow,QString AllowedWEBLanguage,QString ApplicationGroupName,QString ApplicationName,QString ApplicationVersion,QString ConfigFileExt,QString ConfigFileRootName);
    ~cBaseApplicationConfig();

    virtual QString         GetFilterForMediaFile(FilterFile type);
    virtual bool            InitConfigurationValues(QString ForceLanguage,QApplication *App);
    virtual bool            LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App);
    virtual bool            SaveConfigurationFile();

    virtual QString         GetValideWEBLanguage(QString Language);

    // Abstract functions
    virtual void            InitValues()                                                                =0;
    virtual void            SaveValueToXML(QDomElement &domDocument)                                    =0;
    virtual bool            LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile) =0;
};

#endif // CBASEAPPLICATIONCONFIG_H
