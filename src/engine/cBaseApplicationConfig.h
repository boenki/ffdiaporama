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

#ifndef CBASEAPPLICATIONCONFIG_H
#define CBASEAPPLICATIONCONFIG_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#ifndef Q_OS_WIN
    #include <unistd.h>
#endif
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QTranslator>
#include <QDomElement>
#include <QDomDocument>
#include <QMainWindow>

// Include some common various class
#include "cSaveWindowPosition.h"
#include "cCustomIcon.h"
#include "cLuLoImageCache.h"

// Standard geometry definition
#define GEOMETRY_4_3                        0
#define GEOMETRY_16_9                       1
#define GEOMETRY_40_17                      2
#define NBR_GEOMETRY_DEF                    3

// Define possible values for images geometry
#define IMAGE_GEOMETRY_UNKNOWN              0   // undefined image geometry
#define IMAGE_GEOMETRY_3_2                  1   // Standard 3:2 landscape image
#define IMAGE_GEOMETRY_2_3                  2   // Standard 3:2 portrait image
#define IMAGE_GEOMETRY_4_3                  3   // Standard 4:3 landscape image
#define IMAGE_GEOMETRY_3_4                  4   // Standard 4:3 portrait image
#define IMAGE_GEOMETRY_16_9                 5   // Standard 16:9 landscape image
#define IMAGE_GEOMETRY_9_16                 6   // Standard 16:9 portrait image
#define IMAGE_GEOMETRY_40_17                7   // Standard cinema landscape image
#define IMAGE_GEOMETRY_17_40                8   // Standard cinema portrait image

// Media object types
#define OBJECTTYPE_UNMANAGED                0
#define OBJECTTYPE_MANAGED                  1
#define OBJECTTYPE_FOLDER                   2
#define OBJECTTYPE_FFDFILE                  3
#define OBJECTTYPE_IMAGEFILE                4
#define OBJECTTYPE_VIDEOFILE                5
#define OBJECTTYPE_MUSICFILE                6
#define OBJECTTYPE_THUMBNAIL                7

// MMFiler allowed display mode
#define DISPLAY_DATA                        0
#define DISPLAY_ICON100                     1

//****************************************************************************************************************************************************************

// Utility functions
int     getCpuCount();                                                                                              // Retrieve number of processor
QString AdjustDirForOS(QString Dir);                                                                                // Adjust separator in pathname depending on operating system
QString GetTextSize(qlonglong Size);                                                                                // transform a size (_int64) in a string with apropriate unit (Gb/Tb...)
bool    CheckFolder(QString FileToTest,QString PathToTest);                                                         // Check if FileToTest exist in PathToTest and if yes the change current folder to PathToTest
bool    SetWorkingPath(char * const argv[],QString ApplicationGroupName,QString ApplicationName,QString ConfigFileExt);    // Adjust current folder

#ifdef Q_OS_LINUX
    bool SearchRasterMode(QString ApplicationGroupName,QString ApplicationName,QString ConfigFileExt,QString ConfigFileRootName);
#endif

// Application config class
class cBaseApplicationConfig {
public:
    enum    FilterFile          {ALLFILE,IMAGEFILE,VIDEOFILE,MUSICFILE};
    enum    LoadConfigFileType  {USERCONFIGFILE,GLOBALCONFIGFILE};

    QString                 PathEXIV2;                                  // Filename with path to exiv2 binary
    QString                 BinaryEncoderPath;                          // Filename with path to binary encoder ffmpeg or avconv

    bool                    RasterMode;                                 // Enable or disable raster mode [Linux only]
    bool                    RestoreWindow;                              // If true, restore window state and position at startup
    QMainWindow             *TopLevelWindow;                            // Link to MainWindow of the application
    QString                 AllowedWEBLanguage;

    // Image cache
    cLuLoImageCache         ImagesCache;                                // cLuLoImageCache List Object
    qlonglong               MemCacheMaxValue;                           // Maximum value for image cache

    // Last directories
    bool                    RememberLastDirectories;                    // If true, Remember all directories for future use
    QString                 LastMediaPath;                              // Last folder use for image/video
    QString                 LastMusicPath;                              // Last folder use for music

    // Other
    bool                    Smoothing;                                  // True do smoothing in preview
    bool                    Crop1088To1080;                             // Automaticaly crop video from 1088 lines to 1080 (CANON)
    bool                    Deinterlace;                                // Automaticaly activate deinterlace filter when add new MTS videos
    bool                    QuickResamplingPreview;                     // If true then use quick resampling during preview
    bool                    CheckConfigAtStartup;                       // If true, check config at startup

    #ifdef Q_OS_WIN
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
    QString                 StartingPath;
    bool                    MainWinState;                               // WindowsSettings-ismaximized
    cSaveWindowPosition     *MainWinWSP;                                // MainWindow - Window size and position

    QTranslator             translator;                                 // translator for the application
    QTranslator             QTtranslator;                               // translator for QT default text

    // Default systems icons
    cCustomIcon             DefaultCDROMIcon,DefaultHDDIcon,DefaultUSBIcon,DefaultREMOTEIcon;
    cCustomIcon             DefaultUSERIcon,DefaultFOLDERIcon;
    cCustomIcon             DefaultFILEIcon,DefaultDelayedIcon;
    cCustomIcon             DefaultIMAGEIcon,DefaultThumbIcon;
    cCustomIcon             DefaultVIDEOIcon,DefaultMUSICIcon;
    cCustomIcon             DefaultFFDIcon;
    QImage                  VideoMask_120,VideoMask_150,VideoMask_162;

    // MMFiler specific add-on
    bool                    ShowHiddenFilesAndDir;                      // If true, hidden files will be show
    bool                    ShowMntDrive;                               // If true, show drives under /mnt/ [Linux only]
    bool                    ShowFoldersFirst;                           // If true, display folders at first in table list
    int                     CurrentFilter;                              // Currently selected filter
    int                     CurrentMode;                                // Currently selected display mode
    bool                    DisplayFileName;                            // If true Display File Name in icon views
    int                     MinimumEXIFHeight;                          // Minimum height of EXIF preview image for use it
    int                     Image_ThumbWidth,Image_ThumbHeight;         // Thumbnail size in big icon mode for image
    int                     Music_ThumbWidth,Music_ThumbHeight;         // Thumbnail size in big icon mode for music
    int                     Video_ThumbWidth,Video_ThumbHeight;         // Thumbnail size in big icon mode for video

    cBaseApplicationConfig(QMainWindow *TopLevelWindow,QString AllowedWEBLanguage,QString ApplicationGroupName,QString ApplicationName,QString ApplicationVersion,QString ConfigFileExt,QString ConfigFileRootName);
    ~cBaseApplicationConfig();

    virtual QString         GetFilterForMediaFile(FilterFile type);
    virtual bool            InitConfigurationValues(QString ForceLanguage,QApplication *App);
    virtual bool            LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App);
    virtual bool            SaveConfigurationFile();

    virtual QString         GetValideWEBLanguage(QString Language);

    virtual void            PreloadSystemIcons();

    // Abstract functions
    virtual void            InitValues()                                                                =0;
    virtual void            SaveValueToXML(QDomElement &domDocument)                                    =0;
    virtual bool            LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile) =0;
};

#endif // CBASEAPPLICATIONCONFIG_H
