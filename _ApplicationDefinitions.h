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

#ifndef APPLICATIONDEFINITIONS_H
#define APPLICATIONDEFINITIONS_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Global values
extern QString CurrentLanguage;                                 // Current language code (en, fr, ...)
extern QString SystemProperties;                                // System properties log

// Utility functions
int     getCpuCount();                                          // Retrieve number of processor
QString AdjustDirForOS(QString Dir);                            // Adjust separator in pathname depending on operating system
void    AddToSystemProperties(QString StringToAdd);             // Add a string to the system properties log
void    AddSeparatorToSystemProperties();                       // Add a separator line to the system properties log
void    ExitApplicationWithFatalError(QString StringToAdd);     // Exit application with error code 1 if error adding a string to the system properties log and and display it

//====================================================================================================================

class cSaveWindowPosition {
public:
    QString     WindowName;     // Name of the Window
    int         WinRx;          // WindowsSettings-x
    int         WinRy;          // WindowsSettings-y
    int         WinRw;          // WindowsSettings-width
    int         WinRh;          // WindowsSettings-height
    bool        *RestoreWindow; // Link to RestoreWindow boolean variable
    bool        IsMainWindow;   // true if window is a QDockWidget
    QString     MainWinSS;      // if QMainWindow, array for saveState

    cSaveWindowPosition(QString WindowName,bool &RestoreWindow,bool IsMainWindow);
    void    ApplyToWindow(QWidget *Window);
    void    SaveWindowState(QWidget *Window);
    void    SaveToXML(QDomElement &domDocument);
    void    LoadFromXML(QDomElement domDocument);
};

//====================================================================================================================

class cApplicationConfig {
public:
    QString             PathEXIV2;                                  // Filename with path to exiv2 binary
    QString             PathFFMPEG;                                 // Filename with path to ffmpeg binary

    QString             Plateforme;                                 // Operating system in use
    QWidget             *ParentWindow;                              // Link to the top window
    QString             CurrentFolder;                              // Current folder

    #if defined(Q_OS_WIN)
        bool    AEROCOMPATIBILITY;                                  // True pour prendre en charge AERO (Vista/Windows 7)
        // registry value for specific Windows Folder
        QString WINDOWS_APPDATA;                                    // specific Windows Folder : AppData
        QString WINDOWS_MUSIC;                                      // specific Windows Folder : My Music
        QString WINDOWS_PICTURES;                                   // specific Windows Folder : My Pictures
        QString WINDOWS_VIDEO;                                      // specific Windows Folder : My Video
        QString WINDOWS_DOCUMENTS;                                  // specific Windows Folder : Personal
    #endif

    QStringList         AllowVideoExtension;                        // List of all file extension allowed for video
    QStringList         AllowImageExtension;                        // List of all file extension allowed for image

    // User contexte
    QString             UserConfigPath;                             // Path and filename to user profil path
    QString             UserConfigFile;                             // Path and filename to user configuration file
    QString             GlobalConfigFile;                           // Path and filename to global configuration file (in binary directory)

    // Last directories
    bool                RememberLastDirectories;                    // If true, Remember all directories for future use
    QString             LastMediaPath;                              // Last folder use for image/video
    QString             LastProjectPath;                            // Last folder use for project
    QString             LastMusicPath;                              // Last folder use for music
    QString             LastRenderVideoPath;                        // Last folder use for render video

    // Preferences
    bool                DisableSSE2;                                // Disable SSE2 processor extension if error with hardware acceleration
    bool                RestoreWindow;                              // If true, restore window state and position at startup
    bool                AppendObject;                               // If true, new object will be append at the end of the diaporama, if false, new object will be insert after current position
    int                 DefaultFraming;                             // 0=Width, 1=Height
    int                 TimelineHeight;                             // Height of the timeline
    double              PreviewFPS;                                 // Preview FrameRate
    int                 PreviewMaxHeight;                           // Specific for preview : Maximum height of image

    // Default project settings
    int                 ImageGeometry;                              // Project image geometry for image rendering
    int                 NoShotDuration;                             // Default duration for fixed image when is alone (no shot)
    int                 FixedDuration;                              // Default duration for fixed image
    int                 MobilDuration;                              // Default duration for mobil image
    int                 SpeedWave;                                  // Default Speed wave methode

    // Default transition
    bool                RandomTransition;                           // if true randomize a transition
    int                 DefaultTransitionFamilly;                   // Transition familly
    int                 DefaultTransitionSubType;                   // Transition type in the familly
    int                 DefaultTransitionDuration;                  // Transition duration (in msec)

    // Default rendering options
    int                 DefaultFormat;                              // Default output format container
    QString             DefaultVideoCodec;                          // Default video codec
    int                 DefaultVideoBitRate;                        // Default video bit rate
    QString             DefaultAudioCodec;                          // Default audio codec
    int                 DefaultAudioBitRate;                        // Default audio bit rate
    int                 DefaultImageSize;                           // Default image size
    int                 DefaultStandard;                            // Default standard (PAL/NTSC)

    // Main Window Size & Position
    bool                MainWinState;                               // WindowsSettings-ismaximized
    cSaveWindowPosition *MainWinWSP;                                // MainWindow - Window size and position
    cSaveWindowPosition *DlgImagePropertiesWSP;                     // Dialog box "Image properties" - Window size and position
    cSaveWindowPosition *DlgVideoPropertiesWSP;                     // Dialog box "Video properties" - Window size and position
    cSaveWindowPosition *DlgBackgroundPropertiesWSP;                // Dialog box "Background properties" - Window size and position
    cSaveWindowPosition *DlgMusicPropertiesWSP;                     // Dialog box "Music properties" - Window size and position
    cSaveWindowPosition *DlgProjectSettingsWSP;                     // Dialog box "Project settings" - Window size and position
    cSaveWindowPosition *DlgApplicationSettingsWSP;                 // Dialog box "Application settings" - Window size and position
    cSaveWindowPosition *DlgRenderVideoWSP;                         // Dialog box "Render Video" - Window size and position
    cSaveWindowPosition *DlgTransitionPropertiesWSP;                // Dialog box "Transition properties" - Window size and position

    cApplicationConfig();
    ~cApplicationConfig();

    virtual bool        InitConfigurationValues();
    virtual bool        LoadConfigurationFile(int TypeConfigFile);
    virtual bool        SaveConfigurationFile();

    enum FilterFile {ALLFILE,IMAGEFILE,VIDEOFILE};
    virtual QString     GetFilterForMediaFile(FilterFile type);
};

#endif // APPLICATIONDEFINITIONS_H
