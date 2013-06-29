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
#include "cDeviceModelDef.h"
#include "cDriveList.h"
#include "_StyleDefinitions.h"
#include "_SpeedWave.h"
#include "_Transition.h"

//============================================

// Standard geometry definition
#define GEOMETRY_4_3                            0
#define GEOMETRY_16_9                           1
#define GEOMETRY_40_17                          2
#define NBR_GEOMETRY_DEF                        3

// Define possible values for images geometry
#define IMAGE_GEOMETRY_UNKNOWN                  0   // undefined image geometry
#define IMAGE_GEOMETRY_3_2                      1   // Standard 3:2 landscape image
#define IMAGE_GEOMETRY_2_3                      2   // Standard 3:2 portrait image
#define IMAGE_GEOMETRY_4_3                      3   // Standard 4:3 landscape image
#define IMAGE_GEOMETRY_3_4                      4   // Standard 4:3 portrait image
#define IMAGE_GEOMETRY_16_9                     5   // Standard 16:9 landscape image
#define IMAGE_GEOMETRY_9_16                     6   // Standard 16:9 portrait image
#define IMAGE_GEOMETRY_40_17                    7   // Standard cinema landscape image
#define IMAGE_GEOMETRY_17_40                    8   // Standard cinema portrait image

//============================================

// Media object types
#define OBJECTTYPE_UNMANAGED                    0
#define OBJECTTYPE_MANAGED                      1
#define OBJECTTYPE_FOLDER                       2
#define OBJECTTYPE_FFDFILE                      3
#define OBJECTTYPE_IMAGEFILE                    4
#define OBJECTTYPE_VIDEOFILE                    5
#define OBJECTTYPE_MUSICFILE                    6
#define OBJECTTYPE_THUMBNAIL                    7

//============================================

// Media filtering constant
#define FILTERALLOW_OBJECTTYPE_UNMANAGED        0x0001
#define FILTERALLOW_OBJECTTYPE_MANAGED          0x0002
#define FILTERALLOW_OBJECTTYPE_FOLDER           0x0004
#define FILTERALLOW_OBJECTTYPE_FFDFILE          0x0008
#define FILTERALLOW_OBJECTTYPE_IMAGEFILE        0x0010
#define FILTERALLOW_OBJECTTYPE_VIDEOFILE        0x0020
#define FILTERALLOW_OBJECTTYPE_MUSICFILE        0x0040
#define FILTERALLOW_OBJECTTYPE_IMAGEVECTORFILE  0x0080
#define FILTERALLOW_OBJECTTYPE_THUMBNAIL        0x0100
#define FILTERALLOW_OBJECTTYPE_ALL              0xffff

// Multimedia file explorer allowed display mode
#define DISPLAY_DATA                            0
#define DISPLAY_ICON100                         1

//============================================

// Mainwindow display mode
#define DISPLAYWINDOWMODE_PLAYER                0
#define DISPLAYWINDOWMODE_PARTITION             1
#define DISPLAYWINDOWMODE_BROWSER               2

// Mainwindow display size of thumbnails in timeline
#define TIMELINEMINHEIGH                        144                 // min heigh of montage track =(32+4)*4
#define TIMELINEMAXHEIGH                        224                 // max heigh of montage track (min height+4x20 pixels)

//============================================

// Brush type definition
#define BRUSHTYPE_NOBRUSH                   0
#define BRUSHTYPE_SOLID                     1
#define BRUSHTYPE_PATTERN                   2
#define BRUSHTYPE_GRADIENT2                 3
#define BRUSHTYPE_GRADIENT3                 4
#define BRUSHTYPE_IMAGELIBRARY              5
#define BRUSHTYPE_IMAGEDISK                 6

// Brush Gradient orientation
#define GRADIENTORIENTATION_UPLEFT          0
#define GRADIENTORIENTATION_UP              1
#define GRADIENTORIENTATION_UPRIGHT         2
#define GRADIENTORIENTATION_LEFT            3
#define GRADIENTORIENTATION_RADIAL          4
#define GRADIENTORIENTATION_RIGHT           5
#define GRADIENTORIENTATION_BOTTOMLEFT      6
#define GRADIENTORIENTATION_BOTTOM          7
#define GRADIENTORIENTATION_BOTTOMRIGHT     8

//============================================

// Composition type definition
#define COMPOSITIONTYPE_BACKGROUND          0
#define COMPOSITIONTYPE_OBJECT              1
#define COMPOSITIONTYPE_SHOT                2

//============================================

// Define possible values for images type
#define IMAGETYPE_UNKNOWN                   0
#define IMAGETYPE_PHOTOLANDSCAPE            1
#define IMAGETYPE_PHOTOPORTRAIT             2
#define IMAGETYPE_CLIPARTLANDSCAPE          3   // Cliparts are images less than 1920x1080
#define IMAGETYPE_CLIPARTPORTRAIT           4
#define IMAGETYPE_VIDEOLANDSCAPE            5
#define IMAGETYPE_VIDEOPORTRAIT             6
#define NBR_IMAGETYPE                       7

//============================================

// Brush Auto Framing definition
#define AUTOFRAMING_CUSTOMUNLOCK            0
#define AUTOFRAMING_CUSTOMLOCK              1
#define AUTOFRAMING_CUSTOMIMGLOCK           2
#define AUTOFRAMING_CUSTOMPRJLOCK           3
#define AUTOFRAMING_FULLMAX                 4
#define AUTOFRAMING_FULLMIN                 5
#define AUTOFRAMING_HEIGHTLEFTMAX           6
#define AUTOFRAMING_HEIGHTLEFTMIN           7
#define AUTOFRAMING_HEIGHTMIDLEMAX          8
#define AUTOFRAMING_HEIGHTMIDLEMIN          9
#define AUTOFRAMING_HEIGHTRIGHTMAX          10
#define AUTOFRAMING_HEIGHTRIGHTMIN          11
#define AUTOFRAMING_WIDTHTOPMAX             12
#define AUTOFRAMING_WIDTHTOPMIN             13
#define AUTOFRAMING_WIDTHMIDLEMAX           14
#define AUTOFRAMING_WIDTHMIDLEMIN           15
#define AUTOFRAMING_WIDTHBOTTOMMAX          16
#define AUTOFRAMING_WIDTHBOTTOMMIN          17
#define NBR_AUTOFRAMING                     18

#define AUTOFRAMING_ICON_GEOMETRY_LOCKED    ":/img/Geometry_Lock.png"
#define AUTOFRAMING_ICON_GEOMETRY_UNLOCKED  ":/img/Geometry_Unlock.png"
#define AUTOFRAMING_ICON_GEOMETRY_PROJECT   ":/img/Geometry_ProjectLock.png"
#define AUTOFRAMING_ICON_GEOMETRY_IMAGE     ":/img/Geometry_ImageLock.png"

#define AUTOFRAMING_GEOMETRY_CUSTOM         0
#define AUTOFRAMING_GEOMETRY_PROJECT        1
#define AUTOFRAMING_GEOMETRY_IMAGE          2

//============================================

#define RULER_HORIZ_SCREENBORDER    0x0001
#define RULER_HORIZ_TVMARGIN        0x0002
#define RULER_HORIZ_SCREENCENTER    0x0004
#define RULER_HORIZ_UNSELECTED      0x0008
#define RULER_VERT_SCREENBORDER     0x0010
#define RULER_VERT_TVMARGIN         0x0020
#define RULER_VERT_SCREENCENTER     0x0040
#define RULER_VERT_UNSELECTED       0x0080
#define RULER_DEFAULT               RULER_HORIZ_SCREENBORDER|RULER_HORIZ_TVMARGIN|RULER_HORIZ_SCREENCENTER|RULER_HORIZ_UNSELECTED|RULER_VERT_SCREENBORDER|RULER_VERT_TVMARGIN|RULER_VERT_SCREENCENTER|RULER_VERT_UNSELECTED

//============================================

#define AUTOCOMPOSIZE_CUSTOM                0
#define AUTOCOMPOSIZE_REALSIZE              1
#define AUTOCOMPOSIZE_FULLSCREEN            2
#define AUTOCOMPOSIZE_TVMARGINS             3
#define AUTOCOMPOSIZE_TWOTHIRDSSCREEN       4
#define AUTOCOMPOSIZE_HALFSCREEN            5
#define AUTOCOMPOSIZE_THIRDSCREEN           6
#define AUTOCOMPOSIZE_QUARTERSCREEN         7

//============================================
// OnOffFilter mask definition
//============================================

#define FilterEqualize                      0x0001
#define FilterDespeckle                     0x0002
#define FilterGray                          0x0004
#define FilterNegative                      0x0008
#define FilterEmboss                        0x0010
#define FilterEdge                          0x0020
#define FilterAntialias                     0x0040
#define FilterNormalize                     0x0080
#define FilterCharcoal                      0x0100
#define FilterOil                           0x0200

//====================================================================================================================
// Utility functions
//====================================================================================================================

int     getCpuCount();                                                                                              // Retrieve number of processor
QString AdjustDirForOS(QString Dir);                                                                                // Adjust separator in pathname depending on operating system
QString GetTextSize(qlonglong Size);                                                                                // transform a size (_int64) in a string with apropriate unit (Gb/Tb...)
bool    CheckFolder(QString FileToTest,QString PathToTest);                                                         // Check if FileToTest exist in PathToTest and if yes the change current folder to PathToTest
bool    SetWorkingPath(char * const argv[],QString ApplicationName,QString ConfigFileExt);    // Adjust current folder

#ifdef Q_OS_LINUX
    bool SearchRasterMode(QString ApplicationName,QString ConfigFileExt,QString ConfigFileRootName);
#endif

//====================================================================================================================

struct sDefaultBlockCoord {
    int     AutoCompo;
    int     AutoFraming;
};

//====================================================================================================================
// Application config class
//====================================================================================================================

class cBaseApplicationConfig {
public:
    QMainWindow             *TopLevelWindow;                            // Link to MainWindow of the application
    cDeviceModelList        DeviceModelList;                            // List of known rendering device model
    cDriveList              *DriveList;                                 // Drive list for multimedia file explorer
    QString                 AllowedWEBLanguage;

    // Image cache
    cLuLoImageCache         ImagesCache;                                // cLuLoImageCache List Object
    qlonglong               MemCacheMaxValue;                           // Maximum value for image cache

    // Preferences
    bool                    RasterMode;                                 // Enable or disable raster mode [Linux only]
    bool                    RestoreWindow;                              // If true, restore window state and position at startup
    bool                    DisableTooltips;                            // If true, tooltips are disable in dialog boxes
    bool                    SDLAudioOldMode;                            // If true SDL audio use old mode sample instead byte
    bool                    AskUserToRemove;                            // If true, user must answer to a confirmation dialog box to remove slide
    bool                    PartitionMode;                              // If true, partition mode is on
    int                     WindowDisplayMode;
    QString                 BrowserWidgetSplitter;                      // Position of the Browser Widget Splitter
    QStringList             BrowserFavorites;                           // List of favorites path

    // Editor options
    bool                    AppendObject;                               // If true, new object will be append at the end of the diaporama, if false, new object will be insert after current position
    bool                    SortFile;                                   // if true sort file by (last) number when multiple file insertion
    int                     DisplayUnit;                                // Display coordinates unit
    int                     DefaultFraming;                             // 0=Width, 1=Height
    int                     TimelineHeight;                             // Height of the timeline
    int                     SlideRuler;                                 // if true, ruler is on in slide properties dialog box
    bool                    FramingRuler;                               // if true, ruler is on in framing/correction dialog box

    // Preview setting
    double                  PreviewFPS;                                 // Preview FrameRate
    int64_t                 PreviewSamplingRate;                        // Sampling rate (frequency) for preview
    int                     MaxVideoPreviewHeight;

    // Last directories
    bool                    RememberLastDirectories;                    // If true, Remember all directories for future use
    QString                 LastMediaPath;                              // Last folder use for image/video
    QString                 LastMusicPath;                              // Last folder use for music
    QString                 LastProjectPath;                            // Last folder use for project
    QString                 LastRenderVideoPath;                        // Last folder use for render video
    QString                 LastCaptureImage;                           // Last folder use for captured image
    QString                 CurrentPath;                                // Currently shown path in browser widget

    // Other
    bool                    Smoothing;                                  // True do smoothing in preview
    bool                    Crop1088To1080;                             // Automaticaly crop video from 1088 lines to 1080 (CANON)
    bool                    Deinterlace;                                // Automaticaly activate deinterlace filter when add new MTS videos
    bool                    CheckConfigAtStartup;                       // If true, check config at startup

    cStyleCollection        StyleTextCollection;                        // List of known text style
    cStyleCollection        StyleTextBackgroundCollection;              // List of known background text style
    cStyleCollection        StyleBlockShapeCollection;                  // List of known BlockShapestyle

    // Default new text block options
    QString                 DefaultBlock_Text_TextST;
    QString                 DefaultBlock_Text_BackGST;
    QString                 DefaultBlock_Text_ShapeST;
    int                     DefaultBlock_AutoSizePos;
    int                     DefaultBlock_AutoLocking;

    // Default new image block option (when slide creation)
    QString                 DefaultBlockSL_IMG_TextST;
    QString                 DefaultBlockSL_IMG_ShapeST;
    sDefaultBlockCoord      DefaultBlockSL[NBR_IMAGETYPE];

    // Default new image block option (when block add in slide dialog)
    QString                 DefaultBlockBA_IMG_TextST;
    QString                 DefaultBlockBA_IMG_ShapeST;
    sDefaultBlockCoord      DefaultBlockBA[NBR_IMAGETYPE];

    // Default project settings
    int                     ImageGeometry;                              // Project image geometry for image rendering
    int                     NoShotDuration;                             // Default duration for fixed image when is alone (no shot)
    int                     FixedDuration;                              // Default duration for fixed image
    QString                 DefaultAuthor;                              // Default Author name
    int                     DefaultTitleFilling;                        // Default Title filling mode
    int                     DefaultTransitionSpeedWave;                 // Default Speed wave for transition
    int                     DefaultBlockAnimSpeedWave;                  // Default Speed wave for block animation
    int                     DefaultImageAnimSpeedWave;                  // Default Speed wave for image framing and correction animation

    // Default transition
    bool                    RandomTransition;                           // if true randomize a transition
    int                     DefaultTransitionFamilly;                   // Transition familly
    int                     DefaultTransitionSubType;                   // Transition type in the familly
    int                     DefaultTransitionDuration;                  // Transition duration (in msec)

    // Default rendering options
    int                     DefaultNameProjectName;                     // Use project name as default name for rendering
    int                     DefaultFormat;                              // Default output format container
    QString                 DefaultVideoCodec;                          // Default video codec
    int                     DefaultVideoBitRate;                        // Default video bit rate
    QString                 DefaultAudioCodec;                          // Default audio codec
    int                     DefaultAudioBitRate;                        // Default audio bit rate
    int                     DefaultFreq;                                // Default frequency for audio
    int                     DefaultImageSize;                           // Default image size
    int                     DefaultStandard;                            // Default standard (PAL/NTSC)
    int                     DefaultSoundtrackFormat;                    // Default format container for export soundtrack
    QString                 DefaultSoundtrackAudioCodec;                // Default audio codec for export soundtrack
    int                     DefaultSoundtrackBitRate;                   // Default audio bit rate for export soundtrack
    int                     DefaultSoundtrackFreq;                      // Default frequency for export soundtrack
    QString                 DefaultLanguage;                            // Default Language (ISO 639 language code)

    int                     DefaultSmartphoneType;                      // Default Smartphone Type
    int                     DefaultSmartphoneModel;                     // Default Smartphone Model
    int                     DefaultMultimediaType;                      // Default Multimedia Type
    int                     DefaultMultimediaModel;                     // Default Multimedia Model
    int                     DefaultForTheWEBType;                       // Default ForTheWEB Type
    int                     DefaultForTheWEBModel;                      // Default ForTheWEB Model
    int                     DefaultLossLess;                            // Default Lossless imagesize

    QStringList             RecentFile;                                 // Recent project files

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

    // Multimedia file explorer specific options
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

    // Windows Size & Position
    cSaveWindowPosition     *DlgBackgroundPropertiesWSP;                // Dialog box "Background properties" - Window size and position
    cSaveWindowPosition     *DlgMusicPropertiesWSP;                     // Dialog box "Music properties" - Window size and position
    cSaveWindowPosition     *DlgApplicationSettingsWSP;                 // Dialog box "Application settings" - Window size and position
    cSaveWindowPosition     *DlgRenderVideoWSP;                         // Dialog box "Render Video" - Window size and position
    cSaveWindowPosition     *DlgTransitionPropertiesWSP;                // Dialog box "Transition properties" - Window size and position
    cSaveWindowPosition     *DlgTransitionDurationWSP;                  // Dialog box "Transition duration" - Window size and position
    cSaveWinWithSplitterPos *DlgSlidePropertiesWSP;                     // Dialog box "Slide properties" - Window size and position
    cSaveWinWithSplitterPos *DlgSlideDurationWSP;                       // Dialog box "Slide duration" - Window size and position
    cSaveWinWithSplitterPos *DlgFileExplorerWSP;                        // Dialog box "File Explorer" - Window size and position
    cSaveWindowPosition     *DlgImageTransformationWSP;                 // Dialog box "Image Transformation" - Window size and position
    cSaveWindowPosition     *DlgImageCorrectionWSP;                     // Dialog box "Image Correction" - Window size and position
    cSaveWindowPosition     *DlgTextEditWSP;                            // Dialog box "Text editor" - Window size and position
    cSaveWindowPosition     *DlgManageStyleWSP;                         // Dialog box "Manage style" - Window size and position
    cSaveWindowPosition     *DlgCheckConfigWSP;                         // Dialog box "Check configuration" - Window size and position
    cSaveWindowPosition     *DlgManageDevicesWSP;                       // Dialog box "Manage Devices" - Window size and position
    cSaveWindowPosition     *DlgAboutWSP;                               // Dialog box "About" - Window size and position
    cSaveWindowPosition     *DlgffDPjrPropertiesWSP;                    // Dialog box "Project properties" - Window size and position
    cSaveWindowPosition     *DlgInfoFileWSP;                            // Dialog box "File Information" - Window size and position
    cSaveWindowPosition     *DlgRulerDef;                               // Dialog box "Ruler properties" - Window size and position
    cSaveWindowPosition     *DlgManageFavoriteWSP;                      // Dialog box "Manage favorite" - Window size and position
    cSaveWindowPosition     *MainWinWSP;                                // MainWindow - Window size and position

    cBaseApplicationConfig(QMainWindow *TopLevelWindow,QString AllowedWEBLanguage,QString ApplicationGroupName,QString ApplicationName,QString ApplicationVersion,QString ConfigFileExt,QString ConfigFileRootName);
    virtual ~cBaseApplicationConfig();

    virtual QString         GetFilterForMediaFile(FilterFile type);
    virtual bool            InitConfigurationValues(QString ForceLanguage,QApplication *App);
    virtual bool            LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App);
    virtual bool            SaveConfigurationFile();

    virtual QString         GetValideWEBLanguage(QString Language);

    virtual void            PreloadSystemIcons();

    virtual void            InitValues();
    virtual void            SaveValueToXML(QDomElement &domDocument);
    virtual bool            LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile);
};

#endif // CBASEAPPLICATIONCONFIG_H
