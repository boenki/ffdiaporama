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

#ifndef APPLICATIONDEFINITIONS_H
#define APPLICATIONDEFINITIONS_H

// Basic inclusions (common to all files)
#include "../engine/_GlobalDefines.h"
#include "../engine/cDriveList.h"

// Include some common various class
#include "../engine/cBaseApplicationConfig.h"
#include "../engine/cDeviceModelDef.h"
#include "../engine/cBrushDefinition.h"

// Specific inclusions
#include "_StyleDefinitions.h"

// Include some additional standard class
#include <QSplitter>
#include <QMainWindow>

// Application definition
// Note : Application version and revision are in BUILDVERSION.txt
// Syntax for BUILDVERSION.txt is : <Version MAJOR.MINOR[.SUB|_beta_VERSION|_devel]>

#define APPLICATION_NAME                    "ffDiaporama"
#define CONFIGFILEEXT                       ".xml"                                // File extension of configuration files
#define CONFIGFILE_ROOTNAME                 "Configuration"                       // Name of root node in the config xml file
#define APPLICATION_ROOTNAME                "Project"                             // Name of root node in the project xml file

//#define BUILDVERSION_WEBURL                 "http://ffdiaporama.tuxfamily.org/Stable/BUILDVERSION.txt"
#define BUILDVERSION_WEBURL                 "http://ffdiaporama.tuxfamily.org/Devel/BUILDVERSION.txt"

// Dialogbox help file
#define WIKI_CMS_PATH                       "http://ffdiaporama.tuxfamily.org/?page_id="
#define HELPFILE_WIKIINDEX                  7550
#define HELPFILE_PRINCIPLESINDEX            7554
#define HELPFILE_RENDERINDEX                7596
#define HELPFILE_INDEX                      "http://ffdiaporama.tuxfamily.org/?page_id=7550&lang=<local>"
#define HELPFILE_PRINCIPLES                 "http://ffdiaporama.tuxfamily.org/?page_id=7554&lang=<local>"
#define HELPFILE_DEVICES                    "http://ffdiaporama.tuxfamily.org/?page_id=7598&lang=<local>"
#define HELPFILE_DlgTextEdit                "http://ffdiaporama.tuxfamily.org/?page_id=7621&lang=<local>"
#define HELPFILE_DlgSlideProperties         "http://ffdiaporama.tuxfamily.org/?page_id=7606&lang=<local>"
#define HELPFILE_DlgBackgroundProperties    "http://ffdiaporama.tuxfamily.org/?page_id=7613&lang=<local>"
#define HELPFILE_DlgImageCorrection         "http://ffdiaporama.tuxfamily.org/?page_id=7611&lang=<local>"
#define HELPFILE_DlgManageStyle             "http://ffdiaporama.tuxfamily.org/?page_id=7623&lang=<local>"
#define HELPFILE_DlgMusicProperties         "http://ffdiaporama.tuxfamily.org/?page_id=7615&lang=<local>"
#define HELPFILE_DlgRenderVideo             "http://ffdiaporama.tuxfamily.org/?page_id=7627&lang=<local>"
#define HELPFILE_DlgTransitionProperties    "http://ffdiaporama.tuxfamily.org/?page_id=7617&lang=<local>"
#define HELPFILE_DlgCheckConfig             "http://ffdiaporama.tuxfamily.org/?page_id=7604&lang=<local>"
#define HELPFILE_DlgApplicationSettings     "http://ffdiaporama.tuxfamily.org/?page_id=7602&lang=<local>"
#define HELPFILE_DlgManageDevices           "http://ffdiaporama.tuxfamily.org/?page_id=7609&lang=<local>"
#define HELPFILE_DlgffDPjrProperties        "http://ffdiaporama.tuxfamily.org/?page_id=7625&lang=<local>"
#define HELPFILE_DlgInfoFile                "http://ffdiaporama.tuxfamily.org/?page_id=7641&lang=<local>"

#define HELPFILE_DlgVideoEdit               "http://ffdiaporama.tuxfamily.org/?page_id=7619&lang=<local>"
#define HELPFILE_DlgSlideDuration           ""
#define HELPFILE_DlgTransitionDuration      ""
#define HELPFILE_DlgRulerDef                ""                                                                  // No page for that !
#define HELPFILE_DlgManageFavorite          ""                                                                  // No page for that !
#define HELPFILE_NEWS                       "http://ffdiaporama.tuxfamily.org/?cat=5&lang=<local>"

// Define styles name used in xml file
#define STYLENAME_TEXTSTYLE                 "TextStyleCollection"
#define STYLENAME_BACKGROUNDSTYLE           "TextBackgroundStyleCollection"
#define STYLENAME_COORDINATESTYLE           "StyleCoordinateCollection"
#define STYLENAME_BLOCKSHAPESTYLE           "StyleBlockShapeCollection"

#define ICON_GEOMETRY_LOCKED                ":/img/Geometry_Lock.png"
#define ICON_GEOMETRY_UNLOCKED              ":/img/Geometry_Unlock.png"
#define ICON_GEOMETRY_PROJECT               ":/img/Geometry_ProjectLock.png"

// Display coordinates unit
#define DISPLAYUNIT_PERCENT                 0
#define DISPLAYUNIT_PIXELS                  1

// Mainwindow display mode
#define DISPLAYWINDOWMODE_PLAYER            0
#define DISPLAYWINDOWMODE_PARTITION         1
#define DISPLAYWINDOWMODE_BROWSER           2

#define TIMELINEMINHEIGH                    144                 // min heigh of montage track =(32+4)*4
#define TIMELINEMAXHEIGH                    224                 // max heigh of montage track (min height+4x20 pixels)

#define ALLOWEDWEBLANGUAGE                  "en;fr;it;es;el;de;nl"

//====================================================================================================================

class cSaveWinWithSplitterPos : public cSaveWindowPosition {
public:
    QString SplitterTop;

    explicit        cSaveWinWithSplitterPos(QString WindowName,bool &RestoreWindow,bool IsMainWindow);
    virtual         ~cSaveWinWithSplitterPos() {}
    virtual void    ApplyToWindow(QWidget *Window,QSplitter *Top);
    virtual void    SaveWindowState(QWidget *Window,QSplitter *Top);
    virtual void    OverloadedSaveToXML(QDomElement &domDocument);
    virtual void    OverloadedLoadFromXML(QDomElement domDocument);
};

//====================================================================================================================

struct sDefaultBlockCoord {
    int     AutoCompo;
    int     AutoFraming;
};

class cApplicationConfig : public cBaseApplicationConfig {
public:
    // Device database
    cDeviceModelList        DeviceModelList;                            // List of known rendering device model

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

    // Drive list for browser
    cDriveList              *DriveList;

    // Last directories
    QString                 LastProjectPath;                            // Last folder use for project
    QString                 LastRenderVideoPath;                        // Last folder use for render video
    QString                 LastCaptureImage;                           // Last folder use for captured image
    QString                 CurrentPath;                                // Currently shown path in browser widget

    // Preferences
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
    int                     DefaultImageSize;                           // Default image size
    int                     DefaultStandard;                            // Default standard (PAL/NTSC)
    QString                 DefaultLanguage;                            // Default Language (ISO 639 language code)

    int                     DefaultSmartphoneType;                      // Default Smartphone Type
    int                     DefaultSmartphoneModel;                     // Default Smartphone Model
    int                     DefaultMultimediaType;                      // Default Multimedia Type
    int                     DefaultMultimediaModel;                     // Default Multimedia Model
    int                     DefaultForTheWEBType;                       // Default ForTheWEB Type
    int                     DefaultForTheWEBModel;                      // Default ForTheWEB Model
    int                     DefaultLossLess;                            // Default Lossless imagesize

    QStringList             RecentFile;                                 // Recent project files

    // Main Window Size & Position
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

    explicit                cApplicationConfig(QMainWindow *TheTopLevelWindow);
    virtual                 ~cApplicationConfig();

    // Abstract functions in cBaseApplicationConfig
    virtual bool            LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App);
    virtual bool            SaveConfigurationFile();
    virtual void            InitValues();
    virtual void            SaveValueToXML(QDomElement &domDocument);
    virtual bool            LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile);
};

#endif // APPLICATIONDEFINITIONS_H
