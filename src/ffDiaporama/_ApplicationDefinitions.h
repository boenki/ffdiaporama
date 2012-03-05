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
#include "../sharedfiles/_GlobalDefines.h"

// Include some common various class
#include "../sharedfiles/cBaseApplicationConfig.h"
#include "../sharedfiles/cDeviceModelDef.h"

// Specific inclusions
#include "_StyleDefinitions.h"

// Include some additional standard class
#include <QSplitter>
#include <QMainWindow>

// Application definition
#define APPLICATION_NAME                    "ffDiaporama"
#define APPLICATION_VERSION                 "1.2.1"
#define CONFIGFILEEXT                       ".xml"                                // File extension of configuration files
#define CONFIGFILE_ROOTNAME                 "Configuration"                       // Name of root node in the config xml file
#define APPLICATION_ROOTNAME                "Project"                             // Name of root node in the project xml file

#define BUILDVERSION_WEBURL                 "http://download.tuxfamily.org/ffdiaporama/Stable/BUILDVERSION.txt"
//#define BUILDVERSION_WEBURL                 "http://download.tuxfamily.org/ffdiaporama/Devel/BUILDVERSION.txt"

// Dialogbox help file
#define WIKI_CMS_PATH                       "http://ffdiaporama.tuxfamily.org/?page_id="
#define HELPFILE_INDEX                      "http://ffdiaporama.tuxfamily.org/?page_id=3854&lang=<local>"
#define HELPFILE_PRINCIPLES                 "http://ffdiaporama.tuxfamily.org/?page_id=3870&lang=<local>"
#define HELPFILE_DEVICES                    "http://ffdiaporama.tuxfamily.org/?page_id=3909&lang=<local>"
#define HELPFILE_DlgTextEdit                "http://ffdiaporama.tuxfamily.org/?page_id=3883&lang=<local>"
#define HELPFILE_DlgSlideProperties         "http://ffdiaporama.tuxfamily.org/?page_id=3919&lang=<local>"
#define HELPFILE_DlgBackgroundProperties    "http://ffdiaporama.tuxfamily.org/?page_id=3924&lang=<local>"
#define HELPFILE_DlgImageCorrection         "http://ffdiaporama.tuxfamily.org/?page_id=3927&lang=<local>"
#define HELPFILE_DlgManageStyle             "http://ffdiaporama.tuxfamily.org/?page_id=3880&lang=<local>"
#define HELPFILE_DlgMusicProperties         "http://ffdiaporama.tuxfamily.org/?page_id=3931&lang=<local>"
#define HELPFILE_DlgRenderVideo             "http://ffdiaporama.tuxfamily.org/?page_id=3935&lang=<local>"
#define HELPFILE_DlgTransitionProperties    "http://ffdiaporama.tuxfamily.org/?page_id=3941&lang=<local>"
#define HELPFILE_DlgVideoEdit               "http://ffdiaporama.tuxfamily.org/?page_id=3945&lang=<local>"
#define HELPFILE_DlgCheckConfig             "http://ffdiaporama.tuxfamily.org/?page_id=3968&lang=<local>"
#define HELPFILE_SUPPORT                    "http://ffdiaporama.tuxfamily.org/?page_id=3854&lang=<local>"
#define HELPFILE_NEWS                       "http://ffdiaporama.tuxfamily.org/?cat=5&lang=<local>"
#define HELPFILE_DlgApplicationSettings     "http://ffdiaporama.tuxfamily.org/?page_id=3915&lang=<local>"
#define HELPFILE_DlgManageDevices           "http://ffdiaporama.tuxfamily.org/?page_id=4023&lang=<local>"
#define HELPFILE_DlgffDPjrProperties        "http://ffdiaporama.tuxfamily.org/?page_id=4021&lang=<local>"

// Define styles name used in xml file
#define STYLENAME_TEXTSTYLE                 "TextStyleCollection"
#define STYLENAME_BACKGROUNDSTYLE           "TextBackgroundStyleCollection"
#define STYLENAME_COORDINATESTYLE           "StyleCoordinateCollection"
#define STYLENAME_BLOCKSHAPESTYLE           "StyleBlockShapeCollection"
#define STYLENAME_FRAMINGSTYLE              "StyleImageFramingCollection"

// String to include in the system properties
#define OPERATINGSYSTEM_STR                 "Operating system version\t: "
#define STARTINGPATH_STR                    "Starting path\t: "
#define WORKINGPATH_STR                     "Working path set to\t: "
#define SYSTEMLOCAL_STR                     "Detected system locale\t: "
#define LOADEDLOCAL_STR                     "Translation file loaded\t: "
#define COMMONPARTVERSION_STR               "Common part version\t: "
#define VERSIONQT_STR                       "QT version\t\t: "
#define FMTFILTERVERSION_STR                "fmt_filters version\t: "
#define SDLVERSION_STR                      "SDL version\t\t: "
#define LIBAVCODECVERSION_STR               "LIBAVCODEC version\t: "
#define LIBAVFORMATVERSION_STR              "LIBAVFORMAT version\t: "
#define LIBSWSCALEVERSION_STR               "LIBSWSCALE version\t: "
#define USERCONFIGFILE_STR                  "User config file\t: "
#define GLOBALCONFIGFILE_STR                "Global config file\t: "

// Display coordinates unit
#define DISPLAYUNIT_PERCENT                 0
#define DISPLAYUNIT_PIXELS                  1

// Global values
extern QString SystemProperties;                                // System properties log
extern QString CurrentAppVersion;                               // Application version read from BUILDVERSION.txt

#define ALLOWEDWEBLANGUAGE                  "en;fr;it;es;el;de"

// Utility functions
void    AddToSystemProperties(QString StringToAdd);             // Add a string to the system properties log
void    AddSeparatorToSystemProperties();                       // Add a separator line to the system properties log
void    ExitApplicationWithFatalError(QString StringToAdd);     // Exit application with error code 1 if error adding a string to the system properties log and and display it

//====================================================================================================================

class cSaveDlgSlideProperties : public cSaveWindowPosition {
public:
    QString SplitterTop;
    QString SplitterBottom;

    explicit        cSaveDlgSlideProperties(QString WindowName,bool &RestoreWindow,bool IsMainWindow);
    virtual void    ApplyToWindow(QWidget *Window,QSplitter *Top,QSplitter *Bottom);
    virtual void    SaveWindowState(QWidget *Window,QSplitter *Top,QSplitter *Bottom);
    virtual void    OverloadedSaveToXML(QDomElement &domDocument);
    virtual void    OverloadedLoadFromXML(QDomElement domDocument);
};

//====================================================================================================================

class cApplicationConfig : public cBaseApplicationConfig {
public:
    // Device database
    cDeviceModelList        DeviceModelList;                            // List of known rendering device model

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
    int                     DefaultBlockSL_CLIPARTLOCK[3];

    // Default new image block option (when block add in slide dialog)
    QString                 DefaultBlockBA_IMG_TextST;
    QString                 DefaultBlockBA_IMG_ShapeST;
    QString                 DefaultBlockBA_IMG_CoordST[9][3];
    int                     DefaultBlockBA_CLIPARTLOCK[3];

    // Last directories
    QString                 LastProjectPath;                            // Last folder use for project
    QString                 LastRenderVideoPath;                        // Last folder use for render video

    // Preferences
    bool                    SDLAudioOldMode;                            // If true SDL audio use old mode sample instead byte
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

    // Preview setting
    double                  PreviewFPS;                                 // Preview FrameRate

    // Default project settings
    int                     ImageGeometry;                              // Project image geometry for image rendering
    int                     NoShotDuration;                             // Default duration for fixed image when is alone (no shot)
    int                     FixedDuration;                              // Default duration for fixed image
    int                     SpeedWave;                                  // Default Speed wave methode
    QString                 DefaultAuthor;                              // Default Author name
    int                     DefaultTitleFilling;                        // Default Title filling mode

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
    cSaveDlgSlideProperties *DlgSlidePropertiesWSP;                     // Dialog box "Slide properties" - Window size and position
    cSaveWindowPosition     *DlgImageTransformationWSP;                 // Dialog box "Image Transformation" - Window size and position
    cSaveWindowPosition     *DlgImageCorrectionWSP;                     // Dialog box "Image Correction" - Window size and position
    cSaveWindowPosition     *DlgVideoEditWSP;                           // Dialog box "Edit video" - Window size and position
    cSaveWindowPosition     *DlgTextEditWSP;                            // Dialog box "Text editor" - Window size and position
    cSaveWindowPosition     *DlgManageStyleWSP;                         // Dialog box "Manage style" - Window size and position
    cSaveWindowPosition     *DlgCheckConfigWSP;                         // Dialog box "Check configuration" - Window size and position
    cSaveWindowPosition     *DlgManageDevicesWSP;                       // Dialog box "Manage Devices" - Window size and position
    cSaveWindowPosition     *DlgAboutWSP;                               // Dialog box "About" - Window size and position
    cSaveWindowPosition     *DlgffDPjrPropertiesWSP;                    // Dialog box "Project properties" - Window size and position

    explicit                cApplicationConfig(QMainWindow *TheTopLevelWindow);
                            ~cApplicationConfig();

    // Abstract functions in cBaseApplicationConfig
    virtual bool            LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App);
    virtual bool            SaveConfigurationFile();
    virtual void            InitValues();
    virtual void            SaveValueToXML(QDomElement &domDocument);
    virtual bool            LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile);
};

#endif // APPLICATIONDEFINITIONS_H
