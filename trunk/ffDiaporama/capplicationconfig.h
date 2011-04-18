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

#include <QString>
#include <QWidget>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QByteArray>
#include <QString>
#include <QWidget>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define APPLICATION_NAME                    "ffDiaporama"
#define CONFIGFILEEXT                       ".xml"                                  // File extension of configuration files
#define CONFIGFILE_ROOTNAME                 "Configuration"                         // Name of root node in the config xml file
#define APPLICATION_ROOTNAME                "Project"                               // Name of root node in the project xml file
#define GLOBALCONFIGFILE                    2                                       // Type of config file : Global
#define USERCONFIGFILE                      3                                       // Type of config file : USER

/****************************************************************************
  Definition of image format supported by the application
****************************************************************************/

// Image geometry definition
#define GEOMETRY_4_3                        0
#define GEOMETRY_16_9                       1
#define GEOMETRY_40_17                      2

// Standard definition
#define STANDARD_PAL                        0
#define STANDARD_NTSC                       1

// Image size definition
#define NBR_SIZEDEF                         4
#define SIZE_VGA                            0
#define SIZE_DVD                            1
#define SIZE_720P                           2
#define SIZE_1080p                          3

struct sIMAGEDEF {
    int     Width;          // Width
    int     Height;         // Height
    int     PARNUM;         // Pixel aspect ratio (num)
    int     PARDEN;         // Pixel aspect ratio (den)
    double  FPS;            // Frame per second
    char    Name[100];      // Display name
    int     Extend;         // Padding for cinema mode with DVD
};
extern sIMAGEDEF DefImageFormat [2][3][4];

/****************************************************************************
 audio/video codec and file format definition
****************************************************************************/

struct sVideoCodecDef {
    bool    IsFind;
    int     Codec_id;
    char    ShortName[50];
    char    LongName[200];
    char    PossibleBitrate[200];
    char    DefaultBitrate[NBR_SIZEDEF][10];
};
#define NBR_VIDEOCODECDEF   5
extern struct sVideoCodecDef VIDEOCODECDEF[NBR_VIDEOCODECDEF];

struct sAudioCodecDef {
    bool    IsFind;
    int     Codec_id;
    char    ShortName[50];
    char    LongName[200];
    char    PossibleBitrate2CH[200];
    bool    Possibly6CH;
    char    PossibleBitrate6CH[200];
    char    Default[10];
};
#define NBR_AUDIOCODECDEF   6
extern struct sAudioCodecDef AUDIOCODECDEF[NBR_AUDIOCODECDEF];

struct sFormatDef {
    bool    IsFind;
    char    ShortName[50];
    char    FileExtension[10];
    char    LongName[200];
    char    PossibleVideoCodec[200];
    char    PossibleAudioCodec[200];
};
#define NBR_FORMATDEF   6
extern struct sFormatDef FORMATDEF[NBR_FORMATDEF];

//****************************************************************************

// Object type
#define DIAPORAMAOBJECTTYPE_EMPTY           0
#define DIAPORAMAOBJECTTYPE_IMAGE           1
#define DIAPORAMAOBJECTTYPE_VIDEO           2

// Speed wave forme
#define SPEEDWAVE_LINEAR                    0
#define SPEEDWAVE_SINQUARTER                1

// Track definition
#define TRACKBACKGROUND                     0
#define TRACKMONTAGE                        1
#define TRACKMUSIC                          2

// Thubnails definitions
#define ADJUSTXCOLUMN                       10      // width to add to object image for display transition
#define TIMELINEMINHEIGH                    100     // min heigh of montage track
#define TIMELINEMAXHEIGH                    140     // max heigh of montage track
#define TIMELINESOUNDHEIGHT                 18      // Height of sound & music track

// Utility use to retrieve number of processor
int getCpuCount();

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

    QString             Plateforme;                                 // Operating system in use
    QWidget             *ParentWindow;                              // Link to the top window
    QString             CurrentFolder;                              // Current folder

    #if defined(Q_WS_WIN)
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
    QString             UserConfigFile;                             // Path and filename to user configuration file
    QString             GlobalConfigFile;                           // Path and filename to global configuration file (in binary directory)

    // Last directories
    bool                RememberLastDirectories;                    // If true, Remember all directories for future use
    QString             LastMediaPath;                              // Last folder use for image/video
    QString             LastProjectPath;                            // Last folder use for project
    QString             LastMusicPath;                              // Last folder use for music
    QString             LastRenderVideoPath;                        // Last folder use for render video

    // Preferences
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

#endif // CAPPLICATIONCONFIG_H
