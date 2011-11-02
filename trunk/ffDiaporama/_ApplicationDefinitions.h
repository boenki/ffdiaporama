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
extern QString CurrentAppVersion;                               // Application version read from BUILDVERSION.txt

// Utility functions
int     getCpuCount();                                          // Retrieve number of processor
QString AdjustDirForOS(QString Dir);                            // Adjust separator in pathname depending on operating system
void    AddToSystemProperties(QString StringToAdd);             // Add a string to the system properties log
void    AddSeparatorToSystemProperties();                       // Add a separator line to the system properties log
void    ExitApplicationWithFatalError(QString StringToAdd);     // Exit application with error code 1 if error adding a string to the system properties log and and display it

//====================================================================================================================

//============================================
// Class to handle window size & position
//============================================

class cSaveWindowPosition {
public:
    QString     WindowName;     // Name of the Window
    bool        *RestoreWindow; // Link to RestoreWindow boolean variable
    bool        IsMainWindow;   // true if window is a QDockWidget
    QString     WindowGeo;      // Array for saveGeometry (All windows)
    QString     MainWinSS;      // Array for saveState (QMainWindow only)

    cSaveWindowPosition(QString WindowName,bool &RestoreWindow,bool IsMainWindow);
    void    ApplyToWindow(QWidget *Window);
    void    SaveWindowState(QWidget *Window);
    void    SaveToXML(QDomElement &domDocument);
    void    LoadFromXML(QDomElement domDocument);
};

//============================================
// Device model definition
//============================================

class cDeviceModelDef {
public:
    bool    FromGlobalConf;                                     // true if device model is defined in global config file
    bool    FromUserConf;                                       // true if device model is defined in user config file
    bool    IsFind;                                             // true if device model format is supported by installed version of ffmpeg
    int     DeviceIndex;                                        // Device number index key
    QString DeviceName;                                         // long name for the device model
    int     DeviceType;                                         // device type
    int     DeviceSubtype;                                      // device Subtype
    int     Standard;                                           // standard : PAL/NTSC
    int     FileFormat;                                         // sFormatDef number
    int     ImageSize;                                          // DefImageFormat number
    int     VideoCodec;                                         // sVideoCodecDef number
    int     VideoBitrate;                                       // Bitrate number in sVideoCodecDef
    int     AudioCodec;                                         // sAudioCodecDef number
    int     AudioBitrate;                                       // Bitrate number in sAudioCodecDef

    // Save value to be able to reset to default
    QString BckDeviceName;                                      // long name for the device model
    int     BckDeviceType;                                      // device type
    int     BckDeviceSubtype;                                   // device Subtype
    int     BckStandard;                                        // standard : PAL/NTSC
    int     BckFileFormat;                                      // sFormatDef number
    int     BckImageSize;                                       // DefImageFormat number
    int     BckVideoCodec;                                      // sVideoCodecDef number
    int     BckVideoBitrate;                                    // Bitrate number in sVideoCodecDef
    int     BckAudioCodec;                                      // sAudioCodecDef number
    int     BckAudioBitrate;                                    // Bitrate number in sAudioCodecDef

    cDeviceModelDef(bool IsGlobalConf,int IndexKey);
    ~cDeviceModelDef();

    void    SaveToXML(QDomElement &domDocument,QString ElementName);
    bool    LoadFromXML(QDomElement domDocument,QString ElementName,bool IsUserConfigFile);
};

//====================================================================================================================

class cApplicationConfig {
public:
    QString                 PathEXIV2;                                  // Filename with path to exiv2 binary
    QString                 PathFFMPEG;                                 // Filename with path to ffmpeg binary

    QString                 Plateforme;                                 // Operating system in use
    QWidget                 *ParentWindow;                              // Link to the top window
    QString                 CurrentFolder;                              // Current folder

    #if defined(Q_OS_WIN)
        // registry value for specific Windows Folder
        QString WINDOWS_APPDATA;                                        // specific Windows Folder : AppData
        QString WINDOWS_MUSIC;                                          // specific Windows Folder : My Music
        QString WINDOWS_PICTURES;                                       // specific Windows Folder : My Pictures
        QString WINDOWS_VIDEO;                                          // specific Windows Folder : My Video
        QString WINDOWS_DOCUMENTS;                                      // specific Windows Folder : Personal
    #endif

    QStringList             AllowVideoExtension;                        // List of all file extension allowed for video
    QStringList             AllowImageExtension;                        // List of all file extension allowed for image
    QStringList             AllowMusicExtension;                        // List of all file extension allowed for music

    // Collections
    QList<cDeviceModelDef>  RenderDeviceModel;                          // List of known rendering device model
    cStyleCollection        StyleTextCollection;                        // List of known text style
    cStyleCollection        StyleTextBackgroundCollection;              // List of known background text style
    cStyleCollection        StyleCoordinateCollection;                  // List of known Coordinate style
    cStyleCollection        StyleBlockShapeCollection;                  // List of known BlockShapestyle
    cStyleCollection        StyleImageFramingCollection;                // List of known Framing style

    // Default new text block options
    QString                 DefaultBlock_Text_TextST;
    QString                 DefaultBlock_Text_BackGST;
    QString                 DefaultBlock_Text_CoordST[3];
    QString                 DefaultBlock_Text_ShapeST;

    // Default new image block option (when slide creation)
    QString                 DefaultBlockSL_IMG_TextST;
    QString                 DefaultBlockSL_IMG_ShapeST;
    QString                 DefaultBlockSL_IMG_CoordST[9][3];

    // Default new image block option (when block add in slide dialog)
    QString                 DefaultBlockBA_IMG_TextST;
    QString                 DefaultBlockBA_IMG_ShapeST;
    QString                 DefaultBlockBA_IMG_CoordST[9][3];

    // User contexte
    QString                 UserConfigPath;                             // Path and filename to user profil path
    QString                 UserConfigFile;                             // Path and filename to user configuration file
    QString                 GlobalConfigFile;                           // Path and filename to global configuration file (in binary directory)

    // Last directories
    bool                    RememberLastDirectories;                    // If true, Remember all directories for future use
    QString                 LastMediaPath;                              // Last folder use for image/video
    QString                 LastProjectPath;                            // Last folder use for project
    QString                 LastMusicPath;                              // Last folder use for music
    QString                 LastRenderVideoPath;                        // Last folder use for render video

    // Preferences
    bool                    RasterMode;                                 // Enable or disable raster mode [Linux only]
    bool                    RestoreWindow;                              // If true, restore window state and position at startup
    bool                    AskUserToRemove;                            // If true, user must answer to a confirmation dialog box to remove slide
    bool                    PartitionMode;                              // If true, partition mode is on

    // Editor options
    bool                    AppendObject;                               // If true, new object will be append at the end of the diaporama, if false, new object will be insert after current position
    bool                    SortFile;                                   // if true sort file by (last) number when multiple file insertion
    int                     DisplayUnit;                                // Display coordinates unit
    int                     DefaultFraming;                             // 0=Width, 1=Height
    int                     TimelineHeight;                             // Height of the timeline
    bool                    SlideRuler;                                 // if true, ruler is on in slide properties dialog box
    bool                    FramingRuler;                               // if true, ruler is on in framing/correction dialog box
    bool                    Crop1088To1080;                             // Automaticaly crop video from 1088 lines to 1080 (CANON)

    // Preview setting
    double                  PreviewFPS;                                 // Preview FrameRate
    int                     PreviewMaxHeight;                           // Specific for preview : Maximum height of image
    bool                    ApplyTransfoPreview;                        // True if image transformation are apply during preview

    // Default project settings
    int                     ImageGeometry;                              // Project image geometry for image rendering
    int                     NoShotDuration;                             // Default duration for fixed image when is alone (no shot)
    int                     FixedDuration;                              // Default duration for fixed image
    int                     SpeedWave;                                  // Default Speed wave methode

    // Default transition
    bool                    RandomTransition;                           // if true randomize a transition
    int                     DefaultTransitionFamilly;                   // Transition familly
    int                     DefaultTransitionSubType;                   // Transition type in the familly
    int                     DefaultTransitionDuration;                  // Transition duration (in msec)

    // Default rendering options
    int                     DefaultFormat;                              // Default output format container
    QString                 DefaultVideoCodec;                          // Default video codec
    int                     DefaultVideoBitRate;                        // Default video bit rate
    QString                 DefaultAudioCodec;                          // Default audio codec
    int                     DefaultAudioBitRate;                        // Default audio bit rate
    int                     DefaultImageSize;                           // Default image size
    int                     DefaultStandard;                            // Default standard (PAL/NTSC)

    int                     DefaultSmartphoneType;                      // Default Smartphone Type
    int                     DefaultSmartphoneModel;                     // Default Smartphone Model
    int                     DefaultMultimediaType;                      // Default Multimedia Type
    int                     DefaultMultimediaModel;                     // Default Multimedia Model
    int                     DefaultForTheWEBType;                       // Default ForTheWEB Type
    int                     DefaultForTheWEBModel;                      // Default ForTheWEB Model

    QStringList             RecentFile;                                 // Recent project files
    QStringList             TranslatedRenderType;                       // Translated render device type
    QStringList             TranslatedRenderSubtype[4];                 // Translated render device subtype

    // Main Window Size & Position
    bool                    MainWinState;                               // WindowsSettings-ismaximized
    cSaveWindowPosition     *MainWinWSP;                                // MainWindow - Window size and position
    cSaveWindowPosition     *DlgBackgroundPropertiesWSP;                // Dialog box "Background properties" - Window size and position
    cSaveWindowPosition     *DlgMusicPropertiesWSP;                     // Dialog box "Music properties" - Window size and position
    cSaveWindowPosition     *DlgApplicationSettingsWSP;                 // Dialog box "Application settings" - Window size and position
    cSaveWindowPosition     *DlgRenderVideoWSP;                         // Dialog box "Render Video" - Window size and position
    cSaveWindowPosition     *DlgTransitionPropertiesWSP;                // Dialog box "Transition properties" - Window size and position
    cSaveWindowPosition     *DlgSlidePropertiesWSP;                     // Dialog box "Slide properties" - Window size and position
    cSaveWindowPosition     *DlgImageTransformationWSP;                 // Dialog box "Image Transformation" - Window size and position
    cSaveWindowPosition     *DlgImageCorrectionWSP;                     // Dialog box "Image Correction" - Window size and position
    cSaveWindowPosition     *DlgVideoEditWSP;                           // Dialog box "Edit video" - Window size and position
    cSaveWindowPosition     *DlgTextEditWSP;                            // Dialog box "Text editor" - Window size and position
    cSaveWindowPosition     *DlgManageStyleWSP;                         // Dialog box "Manage style" - Window size and position

    cApplicationConfig();
    ~cApplicationConfig();

    bool        InitConfigurationValues();
    bool        LoadConfigurationFile(int TypeConfigFile);
    bool        SaveConfigurationFile();

    enum FilterFile {ALLFILE,IMAGEFILE,VIDEOFILE,MUSICFILE};
    QString     GetFilterForMediaFile(FilterFile type);
};

#endif // APPLICATIONDEFINITIONS_H
