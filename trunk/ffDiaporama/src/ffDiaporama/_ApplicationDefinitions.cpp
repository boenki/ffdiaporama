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

// Specific inclusions
#include "_ApplicationDefinitions.h"
#include "_Diaporama.h"

#define RULER_HORIZ_SCREENBORDER    0x0001
#define RULER_HORIZ_TVMARGIN        0x0002
#define RULER_HORIZ_SCREENCENTER    0x0004
#define RULER_HORIZ_UNSELECTED      0x0008
#define RULER_VERT_SCREENBORDER     0x0010
#define RULER_VERT_TVMARGIN         0x0020
#define RULER_VERT_SCREENCENTER     0x0040
#define RULER_VERT_UNSELECTED       0x0080
#define RULER_DEFAULT               RULER_HORIZ_SCREENBORDER|RULER_HORIZ_TVMARGIN|RULER_HORIZ_SCREENCENTER|RULER_HORIZ_UNSELECTED|RULER_VERT_SCREENBORDER|RULER_VERT_TVMARGIN|RULER_VERT_SCREENCENTER|RULER_VERT_UNSELECTED

//====================================================================================================================

cSaveWinWithSplitterPos::cSaveWinWithSplitterPos(QString WindowName,bool &RestoreWindow,bool IsMainWindow):cSaveWindowPosition(WindowName,RestoreWindow,IsMainWindow) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSaveWinWithSplitterPos::cSaveWinWithSplitterPos");
    SplitterTop="";
}

void cSaveWinWithSplitterPos::ApplyToWindow(QWidget *Window,QSplitter *Top) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSaveWinWithSplitterPos::ApplyToWindow");
    cSaveWindowPosition::ApplyToWindow(Window);
    if (SplitterTop!="")    Top->restoreState(QByteArray::fromHex(SplitterTop.toUtf8()));
}

void cSaveWinWithSplitterPos::SaveWindowState(QWidget *Window,QSplitter *Top) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSaveWinWithSplitterPos::SaveWindowState");
    cSaveWindowPosition::SaveWindowState(Window);
    SplitterTop   =QString(QByteArray(Top->saveState()).toHex());
}

void cSaveWinWithSplitterPos::OverloadedSaveToXML(QDomElement &Element) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSaveWinWithSplitterPos::OverloadedSaveToXML");
    Element.setAttribute("SplitterTop",SplitterTop);
}

void cSaveWinWithSplitterPos::OverloadedLoadFromXML(QDomElement Element) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cSaveWinWithSplitterPos::OverloadedLoadFromXML");
    if (Element.hasAttribute("SplitterTop"))    SplitterTop=Element.attribute("SplitterTop");
}

//====================================================================================================================

cApplicationConfig::cApplicationConfig(QMainWindow *TheTopLevelWindow):cBaseApplicationConfig(TheTopLevelWindow,ALLOWEDWEBLANGUAGE,APPLICATION_NAME,APPLICATION_NAME,CurrentAppName,CONFIGFILEEXT,CONFIGFILE_ROOTNAME) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::cApplicationConfig");
}

//====================================================================================================================
cApplicationConfig::~cApplicationConfig() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::~cApplicationConfig");

    delete DlgMusicPropertiesWSP;
    delete DlgBackgroundPropertiesWSP;
    delete DlgApplicationSettingsWSP;
    delete DlgRenderVideoWSP;
    delete DlgTransitionPropertiesWSP;
    delete DlgTransitionDurationWSP;
    delete DlgSlidePropertiesWSP;
    delete DlgSlideDurationWSP;
    delete DlgImageTransformationWSP;
    delete DlgImageCorrectionWSP;
    delete DlgTextEditWSP;
    delete DlgManageStyleWSP;
    delete DlgCheckConfigWSP;
    delete DlgManageDevicesWSP;
    delete DlgAboutWSP;
    delete DlgffDPjrPropertiesWSP;
    delete DlgInfoFileWSP;
    delete DlgRulerDef;
    delete DlgManageFavoriteWSP;
    delete DlgFileExplorerWSP;
}

//====================================================================================================================

void cApplicationConfig::InitValues() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::InitValues");

    // Initialise all variables and set them default value
    WindowDisplayMode           = DISPLAYWINDOWMODE_PLAYER; // Mainwindow display mode
    PartitionMode               = false;                    // If true, partition mode is on (timeline with multiple row)
    MemCacheMaxValue            = qlonglong(512*1024*1024);
    AskUserToRemove             = true;                     // If true, user must answer to a confirmation dialog box to remove slide
    SortFile                    = true;                     // if true sort file by (last) number when multiple file insertion
    AppendObject                = false;                    // If true, new object will be append at the end of the diaporama, if false, new object will be insert after current position
    DisplayUnit                 = 1;                        // Display coordinates unit
    DefaultFraming              = 2;                        // 0=Width, 1=Height, 2=Full
    TimelineHeight              = TIMELINEMINHEIGH;         // Initial height of the timeline
    PreviewFPS                  = 12.5;                     // Preview FrameRate
    NoShotDuration              = 6000;                     // Default duration for fixed image when is alone (no shot)
    FixedDuration               = 3000;                     // Default duration for fixed image (msec)
    ImageGeometry               = GEOMETRY_16_9;            // Project image geometry for image rendering
    SlideRuler                  = RULER_DEFAULT;            // if true, ruler is on in slide properties dialog box
    FramingRuler                = true;                     // if true, ruler is on in framing/correction dialog box
    DefaultTitleFilling         = 0;                        // Default Title filling mode
    DefaultAuthor               = "";                       // Default Author name
    DefaultTransitionSpeedWave  = SPEEDWAVE_SINQUARTER;     // Default Speed wave for transition
    DefaultBlockAnimSpeedWave   = SPEEDWAVE_LINEAR;         // Default Speed wave for block animation
    DefaultImageAnimSpeedWave   = SPEEDWAVE_LINEAR;         // Default Speed wave for image framing and correction animation

    DefaultFormat               = 1;                        // Default format = avi
    DefaultNameProjectName      =true;                      // Use project name as default name for rendering
    DefaultImageSize            = SIZE_720P;                // Default image size
    DefaultStandard             = STANDARD_PAL;             // Default standard (PAL/NTSC)
    DefaultLanguage             ="und";                     // Default Language (ISO 639 language code)

    RandomTransition            =true;                      // if true randomize a transition
    DefaultTransitionFamilly    =TRANSITIONFAMILLY_BASE;    // Transition familly
    DefaultTransitionSubType    =1;                         // Transition type in the familly
    DefaultTransitionDuration   =1000;                      // Transition duration (in msec)

    DefaultSmartphoneType       =0;                         // Default Smartphone Type
    DefaultSmartphoneModel      =0;                         // Default Smartphone Model
    DefaultMultimediaType       =0;                         // Default Multimedia Type
    DefaultMultimediaModel      =0;                         // Default Multimedia Model
    DefaultForTheWEBType        =0;                         // Default ForTheWEB Type
    DefaultForTheWEBModel       =0;                         // Default ForTheWEB Model
    DefaultLossLess             =0;                         // Default Lossless imagesize

    #ifdef Q_OS_WIN
        LastProjectPath         = WINDOWS_DOCUMENTS;            // Last folder use for project
        LastRenderVideoPath     = WINDOWS_VIDEO;                // Last folder use for render video
        LastCaptureImage        = WINDOWS_PICTURES;             // Last folder use for captured image
        if (LastRenderVideoPath=="") LastRenderVideoPath=WINDOWS_DOCUMENTS;
        SDLAudioOldMode         = false;                        // If true SDL audio use old mode sample instead byte
        QSettings Settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",QSettings::NativeFormat);
        CurrentPath=Settings.value("Personal").toString();
    #endif
    #ifdef Q_WS_X11
        LastProjectPath         = QDir::home().absolutePath();  // Last folder use for project
        LastRenderVideoPath     = QDir::home().absolutePath();  // Last folder use for render video
        LastCaptureImage        = QDir::home().absolutePath();  // Last folder use for captured image
        SDLAudioOldMode         = true;                         // If true SDL audio use old mode sample instead byte
        CurrentPath="~";   // User home folder
    #endif

    // Init collections
    StyleTextCollection.CollectionName          =QString(STYLENAME_TEXTSTYLE);
    StyleTextBackgroundCollection.CollectionName=QString(STYLENAME_BACKGROUNDSTYLE);
    StyleBlockShapeCollection.CollectionName    =QString(STYLENAME_BLOCKSHAPESTYLE);

    DlgBackgroundPropertiesWSP  =new cSaveWindowPosition("DlgBackgroundProperties",RestoreWindow,false);    // Dialog box "Background properties" - Window size and position
    DlgMusicPropertiesWSP       =new cSaveWindowPosition("DlgMusicProperties",RestoreWindow,false);         // Dialog box "Music properties" - Window size and position
    DlgApplicationSettingsWSP   =new cSaveWindowPosition("DlgApplicationSettings",RestoreWindow,false);     // Dialog box "Application settings" - Window size and position
    DlgRenderVideoWSP           =new cSaveWindowPosition("DlgRenderVideoWSP",RestoreWindow,false);          // Dialog box "Render Video" - Window size and position
    DlgTransitionPropertiesWSP  =new cSaveWindowPosition("DlgTransitionPropertiesWSP",RestoreWindow,false); // Dialog box "Transition properties" - Window size and position
    DlgTransitionDurationWSP    =new cSaveWindowPosition("DlgTransitionDurationWSP",RestoreWindow,false);   // Dialog box "Transition duration" - Window size and position
    DlgSlidePropertiesWSP       =new cSaveWinWithSplitterPos("DlgSlidePropertiesWSP",RestoreWindow,false);  // Dialog box "Slide properties" - Window size and position
    DlgSlideDurationWSP         =new cSaveWinWithSplitterPos("DlgSlideDurationWSP",RestoreWindow,false);    // Dialog box "Slide duration" - Window size and position
    DlgImageTransformationWSP   =new cSaveWindowPosition("DlgImageTransformationWSP",RestoreWindow,false);  // Dialog box "Image transformation" - Window size and position
    DlgImageCorrectionWSP       =new cSaveWindowPosition("DlgImageCorrectionWSP",RestoreWindow,false);      // Dialog box "Image correction" - Window size and position
    DlgTextEditWSP              =new cSaveWindowPosition("DlgTextEditWSP",RestoreWindow,false);             // Dialog box "Text editor" - Window size and position
    DlgManageStyleWSP           =new cSaveWindowPosition("DlgManageStyleWSP",RestoreWindow,false);          // Dialog box "Manage style" - Window size and position
    DlgCheckConfigWSP           =new cSaveWindowPosition("DlgCheckConfigWSP",RestoreWindow,false);          // Dialog box "Check configuration" - Window size and position
    DlgManageDevicesWSP         =new cSaveWindowPosition("DlgManageDevicesWSP",RestoreWindow,false);        // Dialog box "Manage Devices" - Window size and position
    DlgAboutWSP                 =new cSaveWindowPosition("DlgAboutWSP",RestoreWindow,false);                // Dialog box "About" - Window size and position
    DlgffDPjrPropertiesWSP      =new cSaveWindowPosition("DlgffDPjrPropertiesWSP",RestoreWindow,false);     // Dialog box "Project properties" - Window size and position
    DlgInfoFileWSP              =new cSaveWindowPosition("DlgInfoFileWSP",RestoreWindow,false);             // Dialog box "File Information" - Window size and position
    DlgRulerDef                 =new cSaveWindowPosition("DlgRulerDef",RestoreWindow,false);                // Dialog box "Ruler properties" - Window size and position
    DlgManageFavoriteWSP        =new cSaveWindowPosition("DlgManageFavoriteWSP",RestoreWindow,false);       // Dialog box "Manage favorite" - Window size and position
    DlgFileExplorerWSP          =new cSaveWinWithSplitterPos("DlgFileExplorerWSP",RestoreWindow,false);     // Dialog box "File explorer" - Window size and position

    // Default new text block options
    DefaultBlock_Text_TextST    ="###GLOBALSTYLE###:0";
    DefaultBlock_Text_BackGST   ="###GLOBALSTYLE###:0";
    DefaultBlock_Text_ShapeST   ="###GLOBALSTYLE###:0";
    DefaultBlock_AutoSizePos    =AUTOCOMPOSIZE_HALFSCREEN;
    DefaultBlock_AutoLocking    =AUTOFRAMING_CUSTOMUNLOCK;

    // Default new image block option (when slide creation)
    DefaultBlockSL_IMG_TextST   ="###GLOBALSTYLE###:0";
    DefaultBlockSL_IMG_ShapeST  ="###GLOBALSTYLE###:0";
    DefaultBlockSL[IMAGETYPE_UNKNOWN         ].AutoCompo=DefaultBlock_AutoSizePos;      DefaultBlockSL[IMAGETYPE_UNKNOWN         ].AutoFraming=DefaultBlock_AutoLocking;
    DefaultBlockSL[IMAGETYPE_PHOTOLANDSCAPE  ].AutoCompo=AUTOCOMPOSIZE_FULLSCREEN;      DefaultBlockSL[IMAGETYPE_PHOTOLANDSCAPE  ].AutoFraming=AUTOFRAMING_WIDTHMIDLEMAX;
    DefaultBlockSL[IMAGETYPE_PHOTOPORTRAIT   ].AutoCompo=AUTOCOMPOSIZE_FULLSCREEN;      DefaultBlockSL[IMAGETYPE_PHOTOPORTRAIT   ].AutoFraming=AUTOFRAMING_HEIGHTMIDLEMAX;
    DefaultBlockSL[IMAGETYPE_CLIPARTLANDSCAPE].AutoCompo=AUTOCOMPOSIZE_REALSIZE;        DefaultBlockSL[IMAGETYPE_CLIPARTLANDSCAPE].AutoFraming=AUTOFRAMING_FULLMAX;
    DefaultBlockSL[IMAGETYPE_CLIPARTPORTRAIT ].AutoCompo=AUTOCOMPOSIZE_REALSIZE;        DefaultBlockSL[IMAGETYPE_CLIPARTPORTRAIT ].AutoFraming=AUTOFRAMING_FULLMAX;
    DefaultBlockSL[IMAGETYPE_VIDEOLANDSCAPE  ].AutoCompo=AUTOCOMPOSIZE_REALSIZE;        DefaultBlockSL[IMAGETYPE_VIDEOLANDSCAPE  ].AutoFraming=AUTOFRAMING_FULLMAX;
    DefaultBlockSL[IMAGETYPE_VIDEOPORTRAIT   ].AutoCompo=AUTOCOMPOSIZE_REALSIZE;        DefaultBlockSL[IMAGETYPE_VIDEOPORTRAIT   ].AutoFraming=AUTOFRAMING_FULLMAX;

    // Default new image block option (when block add in slide dialog)
    DefaultBlockBA_IMG_TextST   ="###GLOBALSTYLE###:0";
    DefaultBlockBA_IMG_ShapeST  ="###GLOBALSTYLE###:0";
    DefaultBlockBA[IMAGETYPE_UNKNOWN         ].AutoCompo=DefaultBlock_AutoSizePos;      DefaultBlockBA[IMAGETYPE_UNKNOWN         ].AutoFraming=DefaultBlock_AutoLocking;
    DefaultBlockBA[IMAGETYPE_PHOTOLANDSCAPE  ].AutoCompo=AUTOCOMPOSIZE_HALFSCREEN;      DefaultBlockBA[IMAGETYPE_PHOTOLANDSCAPE  ].AutoFraming=AUTOFRAMING_WIDTHMIDLEMAX;
    DefaultBlockBA[IMAGETYPE_PHOTOPORTRAIT   ].AutoCompo=AUTOCOMPOSIZE_HALFSCREEN;      DefaultBlockBA[IMAGETYPE_PHOTOPORTRAIT   ].AutoFraming=AUTOFRAMING_HEIGHTMIDLEMAX;
    DefaultBlockBA[IMAGETYPE_CLIPARTLANDSCAPE].AutoCompo=AUTOCOMPOSIZE_REALSIZE;        DefaultBlockBA[IMAGETYPE_CLIPARTLANDSCAPE].AutoFraming=AUTOFRAMING_FULLMAX;
    DefaultBlockBA[IMAGETYPE_CLIPARTPORTRAIT ].AutoCompo=AUTOCOMPOSIZE_REALSIZE;        DefaultBlockBA[IMAGETYPE_CLIPARTPORTRAIT ].AutoFraming=AUTOFRAMING_FULLMAX;
    DefaultBlockBA[IMAGETYPE_VIDEOLANDSCAPE  ].AutoCompo=AUTOCOMPOSIZE_REALSIZE;        DefaultBlockBA[IMAGETYPE_VIDEOLANDSCAPE  ].AutoFraming=AUTOFRAMING_FULLMAX;
    DefaultBlockBA[IMAGETYPE_VIDEOPORTRAIT   ].AutoCompo=AUTOCOMPOSIZE_REALSIZE;        DefaultBlockBA[IMAGETYPE_VIDEOPORTRAIT   ].AutoFraming=AUTOFRAMING_FULLMAX;
}

//====================================================================================================================

bool cApplicationConfig::LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::LoadConfigurationFile");

    if (!cBaseApplicationConfig::LoadConfigurationFile(TypeConfigFile,App)) {
        if (TypeConfigFile==USERCONFIGFILE) DeviceModelList.TranslatRenderType();
        return false;
    } else if (!DeviceModelList.LoadConfigurationFile(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile,TypeConfigFile)) {
        if (TypeConfigFile==USERCONFIGFILE) DeviceModelList.TranslatRenderType();
        return false;
    } else return true;
}

//====================================================================================================================

bool cApplicationConfig::SaveConfigurationFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::SaveConfigurationValues");

    return cBaseApplicationConfig::SaveConfigurationFile() && DeviceModelList.SaveConfigurationFile(UserConfigFile);
}

//====================================================================================================================

void cApplicationConfig::SaveValueToXML(QDomElement &domDocument) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::SaveValueToXML");

    QDomElement     Element,SubElement,SubSubElement;
    QDomDocument    Document;

    // Save preferences
    if ((domDocument.elementsByTagName("LastDirectories").length()>0)&&(domDocument.elementsByTagName("LastDirectories").item(0).isElement()==true)) {
        Element=domDocument.elementsByTagName("LastDirectories").item(0).toElement();
        Element.setAttribute("LastProjectPath",         LastProjectPath);
        Element.setAttribute("LastRenderVideoPath",     LastRenderVideoPath);
        Element.setAttribute("LastCaptureImage",        LastCaptureImage);
        Element.setAttribute("CurrentBrowserPath",      CurrentPath);
    }

    Element=Document.createElement("EditorOptions");
    Element.setAttribute("SDLAudioOldMode",             SDLAudioOldMode?"1":"0");
    Element.setAttribute("AppendObject",                AppendObject?"1":"0");
    Element.setAttribute("DisplayUnit",                 DisplayUnit);
    Element.setAttribute("PartitionMode",               PartitionMode?"1":"0");
    Element.setAttribute("WindowDisplayMode",           WindowDisplayMode);
    Element.setAttribute("BrowserWidgetSplitter",       BrowserWidgetSplitter);
    Element.setAttribute("SortFile",                    SortFile?"1":"0");
    Element.setAttribute("NewTimelineHeight",           TimelineHeight);
    Element.setAttribute("DefaultFraming",              DefaultFraming);
    Element.setAttribute("PreviewFPS",                  (QString("%1").arg(PreviewFPS,0,'f')));
    Element.setAttribute("RandomTransition",            RandomTransition?"1":"0");
    Element.setAttribute("DefaultTransitionFamilly",    DefaultTransitionFamilly);
    Element.setAttribute("DefaultTransitionSubType",    DefaultTransitionSubType);
    Element.setAttribute("DefaultTransitionDuration",   DefaultTransitionDuration);
    Element.setAttribute("AskUserToRemove",             AskUserToRemove?"1":"0");
    Element.setAttribute("DlgSlideRuler",               SlideRuler);
    Element.setAttribute("FramingRuler",                FramingRuler?"1":"0");
    domDocument.appendChild(Element);

    Element=Document.createElement("BrowserFavorites");
    Element.setAttribute("FavoritesNumber",             BrowserFavorites.count());
    for (int i=0;i<BrowserFavorites.count();i++) Element.setAttribute(QString("BrowserFavorites_%1").arg(i),BrowserFavorites[i]);
    domDocument.appendChild(Element);

    Element=Document.createElement("ProjectDefault");
    Element.setAttribute("ImageGeometry",               ImageGeometry);
    Element.setAttribute("NoShotDuration",              NoShotDuration);
    Element.setAttribute("FixedDuration",               FixedDuration);
    Element.setAttribute("DefaultAuthor",               DefaultAuthor);
    Element.setAttribute("DefaultTitleFilling",         DefaultTitleFilling);
    Element.setAttribute("DefaultTransitionSpeedWave",  DefaultTransitionSpeedWave);
    Element.setAttribute("DefaultBlockAnimSpeedWave",   DefaultBlockAnimSpeedWave);
    Element.setAttribute("DefaultImageAnimSpeedWave",   DefaultImageAnimSpeedWave);

    SubElement=Document.createElement("DefaultBlock_Text");
    SubElement.setAttribute("TextST",                   DefaultBlock_Text_TextST);
    SubElement.setAttribute("BackGST",                  DefaultBlock_Text_BackGST);
    SubElement.setAttribute("ShapeST",                  DefaultBlock_Text_ShapeST);
    SubElement.setAttribute("AutoSizePos",              DefaultBlock_AutoSizePos);
    SubElement.setAttribute("AutoLocking",              DefaultBlock_AutoLocking);
    Element.appendChild(SubElement);

    SubElement=Document.createElement("DefaultBlockSL_IMG");
    SubElement.setAttribute("TextST",                   DefaultBlockSL_IMG_TextST);
    SubElement.setAttribute("ShapeST",                  DefaultBlockSL_IMG_ShapeST);
    for (int i=1;i<NBR_IMAGETYPE;i++) {
        SubSubElement=Document.createElement(QString("IMG_GEO_%1").arg(i));
        SubSubElement.setAttribute("AutoCompo",         DefaultBlockSL[i].AutoCompo);
        SubSubElement.setAttribute("AutoFraming",       DefaultBlockSL[i].AutoFraming);
        SubElement.appendChild(SubSubElement);
    }
    Element.appendChild(SubElement);

    SubElement=Document.createElement("DefaultBlockBA_IMG");
    SubElement.setAttribute("TextST",                   DefaultBlockBA_IMG_TextST);
    SubElement.setAttribute("ShapeST",                  DefaultBlockBA_IMG_ShapeST);
    for (int i=1;i<NBR_IMAGETYPE;i++) {
        SubSubElement=Document.createElement(QString("IMG_GEO_%1").arg(i));
        SubSubElement.setAttribute("AutoCompo",         DefaultBlockBA[i].AutoCompo);
        SubSubElement.setAttribute("AutoFraming",       DefaultBlockBA[i].AutoFraming);
        SubElement.appendChild(SubSubElement);
    }
    Element.appendChild(SubElement);
    domDocument.appendChild(Element);

    Element=Document.createElement("RenderDefault");
    Element.setAttribute("DefaultNameProjectName",      DefaultNameProjectName?"1":"0");
    Element.setAttribute("Format",                      DefaultFormat);
    Element.setAttribute("VideoCodec",                  DefaultVideoCodec);
    Element.setAttribute("VideoBitRate",                DefaultVideoBitRate);
    Element.setAttribute("AudioCodec",                  DefaultAudioCodec);
    Element.setAttribute("AudioBitRate",                DefaultAudioBitRate);
    Element.setAttribute("Standard",                    DefaultStandard);
    Element.setAttribute("Language",                    DefaultLanguage);
    Element.setAttribute("ImageSize",                   DefaultImageSize);
    Element.setAttribute("DefaultSmartphoneType",       DefaultSmartphoneType);
    Element.setAttribute("DefaultSmartphoneModel",      DefaultSmartphoneModel);
    Element.setAttribute("DefaultMultimediaType",       DefaultMultimediaType);
    Element.setAttribute("DefaultMultimediaModel",      DefaultMultimediaModel);
    Element.setAttribute("DefaultForTheWEBType",        DefaultForTheWEBType);
    Element.setAttribute("DefaultForTheWEBModel",       DefaultForTheWEBModel);
    Element.setAttribute("DefaultLossLess",             DefaultLossLess);
    domDocument.appendChild(Element);

    Element=Document.createElement("RecentFiles");
    for (int i=0;i<RecentFile.count();i++) {
        SubElement=Document.createElement("Recent-"+QString("%1").arg(i));
        SubElement.setAttribute("File",RecentFile.at(i));
        Element.appendChild(SubElement);
    }
    domDocument.appendChild(Element);

    // Save other collections
    StyleTextCollection.SaveToXML(domDocument);
    StyleTextBackgroundCollection.SaveToXML(domDocument);
    StyleBlockShapeCollection.SaveToXML(domDocument);

    // Save windows size and position
    DlgBackgroundPropertiesWSP->SaveToXML(domDocument);
    DlgMusicPropertiesWSP->SaveToXML(domDocument);
    DlgApplicationSettingsWSP->SaveToXML(domDocument);
    DlgRenderVideoWSP->SaveToXML(domDocument);
    DlgTransitionPropertiesWSP->SaveToXML(domDocument);
    DlgTransitionDurationWSP->SaveToXML(domDocument);
    DlgSlidePropertiesWSP->SaveToXML(domDocument);
    DlgSlideDurationWSP->SaveToXML(domDocument);
    DlgImageTransformationWSP->SaveToXML(domDocument);
    DlgImageCorrectionWSP->SaveToXML(domDocument);
    DlgTextEditWSP->SaveToXML(domDocument);
    DlgManageStyleWSP->SaveToXML(domDocument);
    DlgCheckConfigWSP->SaveToXML(domDocument);
    DlgManageDevicesWSP->SaveToXML(domDocument);
    DlgAboutWSP->SaveToXML(domDocument);
    DlgffDPjrPropertiesWSP->SaveToXML(domDocument);
    DlgInfoFileWSP->SaveToXML(domDocument);
    DlgRulerDef->SaveToXML(domDocument);
    DlgManageFavoriteWSP->SaveToXML(domDocument);
    DlgFileExplorerWSP->SaveToXML(domDocument);
}

//====================================================================================================================

bool cApplicationConfig::LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::LoadValueFromXML");

    // Load preferences
    if ((domDocument.elementsByTagName("LastDirectories").length()>0)&&(domDocument.elementsByTagName("LastDirectories").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("LastDirectories").item(0).toElement();
        if (Element.hasAttribute("LastProjectPath"))            LastProjectPath             =Element.attribute("LastProjectPath");
        if (Element.hasAttribute("LastRenderVideoPath"))        LastRenderVideoPath         =Element.attribute("LastRenderVideoPath");
        if (Element.hasAttribute("LastCaptureImage"))           LastCaptureImage            =Element.attribute("LastCaptureImage");
        if (Element.hasAttribute("CurrentBrowserPath"))         CurrentPath                 =Element.attribute("CurrentBrowserPath");
    }

    if ((domDocument.elementsByTagName("EditorOptions").length()>0)&&(domDocument.elementsByTagName("EditorOptions").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("EditorOptions").item(0).toElement();
        if (Element.hasAttribute("MemCacheMaxValue"))           MemCacheMaxValue            =Element.attribute("MemCacheMaxValue").toLongLong();
        #ifdef Q_OS_WIN
        if ((!IsWindowsXP)&&(MemCacheMaxValue>qlonglong(512*1024*1024))) MemCacheMaxValue=qlonglong(512*1024*1024);
        #endif
        if (Element.hasAttribute("SDLAudioOldMode"))            SDLAudioOldMode             =Element.attribute("SDLAudioOldMode")=="1";
        if (Element.hasAttribute("AppendObject"))               AppendObject                =Element.attribute("AppendObject")=="1";
        if (Element.hasAttribute("PartitionMode"))              PartitionMode               =Element.attribute("PartitionMode")=="1";
        if (Element.hasAttribute("WindowDisplayMode"))          WindowDisplayMode           =Element.attribute("WindowDisplayMode").toInt();
        if (Element.hasAttribute("BrowserWidgetSplitter"))      BrowserWidgetSplitter       =Element.attribute("BrowserWidgetSplitter");
        if (Element.hasAttribute("DisplayUnit"))                DisplayUnit                 =Element.attribute("DisplayUnit").toInt();
        if (Element.hasAttribute("SortFile"))                   SortFile                    =Element.attribute("SortFile")=="1";
        if (Element.hasAttribute("NewTimelineHeight"))          TimelineHeight              =Element.attribute("NewTimelineHeight").toInt();
        if (TimelineHeight<TIMELINEMINHEIGH) TimelineHeight=TIMELINEMINHEIGH;
        if (TimelineHeight>TIMELINEMAXHEIGH) TimelineHeight=TIMELINEMAXHEIGH;
        if (Element.hasAttribute("DefaultFraming"))             DefaultFraming              =Element.attribute("DefaultFraming").toInt();
        if (Element.hasAttribute("PreviewFPS"))                 PreviewFPS                  =Element.attribute("PreviewFPS").toDouble();
        if (Element.hasAttribute("RandomTransition"))           RandomTransition            =Element.attribute("RandomTransition")=="1";
        if (Element.hasAttribute("DefaultTransitionFamilly"))   DefaultTransitionFamilly    =Element.attribute("DefaultTransitionFamilly").toInt();
        if (Element.hasAttribute("DefaultTransitionSubType"))   DefaultTransitionSubType    =Element.attribute("DefaultTransitionSubType").toInt();
        if (Element.hasAttribute("DefaultTransitionDuration"))  DefaultTransitionDuration   =Element.attribute("DefaultTransitionDuration").toInt();
        if (Element.hasAttribute("AskUserToRemove"))            AskUserToRemove             =Element.attribute("AskUserToRemove")!="0";
        if ((Element.hasAttribute("SlideRuler"))&&(Element.attribute("SlideRuler")!="0"))   SlideRuler=RULER_DEFAULT;
        if (Element.hasAttribute("DlgSlideRuler"))              SlideRuler                  =Element.attribute("DlgSlideRuler").toInt();
        if (Element.hasAttribute("FramingRuler"))               FramingRuler                =Element.attribute("FramingRuler")!="0";
    }

    if ((domDocument.elementsByTagName("BrowserFavorites").length()>0)&&(domDocument.elementsByTagName("BrowserFavorites").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("BrowserFavorites").item(0).toElement();
        int FavToLoad=0;
        if (Element.hasAttribute("FavoritesNumber")) FavToLoad=Element.attribute("FavoritesNumber").toInt();
        BrowserFavorites.clear();
        for (int i=0;i<FavToLoad;i++) if (Element.hasAttribute(QString("BrowserFavorites_%1").arg(i))) BrowserFavorites.append(Element.attribute(QString("BrowserFavorites_%1").arg(i)));
    }

    if ((domDocument.elementsByTagName("ProjectDefault").length()>0)&&(domDocument.elementsByTagName("ProjectDefault").item(0).isElement()==true)) {
        QDomElement  Element=domDocument.elementsByTagName("ProjectDefault").item(0).toElement();
        if (Element.hasAttribute("ImageGeometry"))              ImageGeometry               =Element.attribute("ImageGeometry").toInt();
        if (Element.hasAttribute("NoShotDuration"))             NoShotDuration              =Element.attribute("NoShotDuration").toInt();
        if (Element.hasAttribute("FixedDuration"))              FixedDuration               =Element.attribute("FixedDuration").toInt();
        if (Element.hasAttribute("DefaultTitleFilling"))        DefaultTitleFilling         =Element.attribute("DefaultTitleFilling").toInt();
        if (Element.hasAttribute("DefaultAuthor"))              DefaultAuthor               =Element.attribute("DefaultAuthor");
        if (Element.hasAttribute("DefaultTransitionSpeedWave")) DefaultTransitionSpeedWave  =Element.attribute("DefaultTransitionSpeedWave").toInt();
        if (Element.hasAttribute("DefaultBlockAnimSpeedWave"))  DefaultBlockAnimSpeedWave   =Element.attribute("DefaultBlockAnimSpeedWave").toInt();
        if (Element.hasAttribute("DefaultImageAnimSpeedWave"))  DefaultImageAnimSpeedWave   =Element.attribute("DefaultImageAnimSpeedWave").toInt();

        // Compatibility with version prior to 1.5
        if (Element.hasAttribute("SpeedWave")) {
            DefaultTransitionSpeedWave=Element.attribute("SpeedWave").toInt();
            DefaultBlockAnimSpeedWave =Element.attribute("SpeedWave").toInt();
            DefaultImageAnimSpeedWave =Element.attribute("SpeedWave").toInt();
        }

        if ((Element.elementsByTagName("DefaultBlock_Text").length()>0)&&(Element.elementsByTagName("DefaultBlock_Text").item(0).isElement()==true)) {
            QDomElement SubElement=Element.elementsByTagName("DefaultBlock_Text").item(0).toElement();
            if (SubElement.hasAttribute("TextST"))              DefaultBlock_Text_TextST    =SubElement.attribute("TextST");
            if (SubElement.hasAttribute("BackGST"))             DefaultBlock_Text_BackGST   =SubElement.attribute("BackGST");
            if (SubElement.hasAttribute("ShapeST"))             DefaultBlock_Text_ShapeST   =SubElement.attribute("ShapeST");
            if (SubElement.hasAttribute("AutoSizePos"))         DefaultBlock_AutoSizePos    =SubElement.attribute("AutoSizePos").toInt();
            if (SubElement.hasAttribute("AutoLocking"))         DefaultBlock_AutoLocking    =SubElement.attribute("AutoLocking").toInt();
        }
        if ((Element.elementsByTagName("DefaultBlockSL_IMG").length()>0)&&(Element.elementsByTagName("DefaultBlockSL_IMG").item(0).isElement()==true)) {
            QDomElement SubElement=Element.elementsByTagName("DefaultBlockSL_IMG").item(0).toElement();
            if (SubElement.hasAttribute("TextST"))   DefaultBlockSL_IMG_TextST    =SubElement.attribute("TextST");
            if (SubElement.hasAttribute("ShapeST"))  DefaultBlockSL_IMG_ShapeST   =SubElement.attribute("ShapeST");
            for (int i=1;i<NBR_IMAGETYPE;i++) if ((SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).length()>0)&&(SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).item(0).isElement()==true)) {
                QDomElement SubSubElement=SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).item(0).toElement();
                if (SubSubElement.hasAttribute("AutoCompo"))   DefaultBlockSL[i].AutoCompo  =SubSubElement.attribute("AutoCompo").toInt();
                if (SubSubElement.hasAttribute("AutoFraming")) DefaultBlockSL[i].AutoFraming=SubSubElement.attribute("AutoFraming").toInt();
            }
        }

        if ((Element.elementsByTagName("DefaultBlockBA_IMG").length()>0)&&(Element.elementsByTagName("DefaultBlockBA_IMG").item(0).isElement()==true)) {
            QDomElement SubElement=Element.elementsByTagName("DefaultBlockBA_IMG").item(0).toElement();
            if (SubElement.hasAttribute("TextST"))   DefaultBlockBA_IMG_TextST    =SubElement.attribute("TextST");
            if (SubElement.hasAttribute("ShapeST"))  DefaultBlockBA_IMG_ShapeST   =SubElement.attribute("ShapeST");
            for (int i=1;i<NBR_IMAGETYPE;i++) if ((SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).length()>0)&&(SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).item(0).isElement()==true)) {
                QDomElement SubSubElement=SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).item(0).toElement();
                if (SubSubElement.hasAttribute("AutoCompo"))   DefaultBlockBA[i].AutoCompo  =SubSubElement.attribute("AutoCompo").toInt();
                if (SubSubElement.hasAttribute("AutoFraming")) DefaultBlockBA[i].AutoFraming=SubSubElement.attribute("AutoFraming").toInt();
            }
        }
    }
    if ((domDocument.elementsByTagName("RenderDefault").length()>0)&&(domDocument.elementsByTagName("RenderDefault").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("RenderDefault").item(0).toElement();
        if (Element.hasAttribute("DefaultNameProjectName")) DefaultNameProjectName  =Element.attribute("DefaultNameProjectName")=="1";
        if (Element.hasAttribute("Format"))                 DefaultFormat           =Element.attribute("Format").toInt();
        if (Element.hasAttribute("VideoCodec"))             DefaultVideoCodec       =Element.attribute("VideoCodec");
        if (Element.hasAttribute("VideoBitRate"))           DefaultVideoBitRate     =Element.attribute("VideoBitRate").toInt();
        if (Element.hasAttribute("AudioCodec"))             DefaultAudioCodec       =Element.attribute("AudioCodec");
        if (Element.hasAttribute("AudioBitRate"))           DefaultAudioBitRate     =Element.attribute("AudioBitRate").toInt();
        if (Element.hasAttribute("ImageSize"))              DefaultImageSize        =Element.attribute("ImageSize").toInt();
        if (Element.hasAttribute("Standard"))               DefaultStandard         =Element.attribute("Standard").toInt();
        if (Element.hasAttribute("Language"))               DefaultLanguage         =Element.attribute("Language");
        if (Element.hasAttribute("DefaultSmartphoneType"))  DefaultSmartphoneType   =Element.attribute("DefaultSmartphoneType").toInt();
        if (Element.hasAttribute("DefaultSmartphoneModel")) DefaultSmartphoneModel  =Element.attribute("DefaultSmartphoneModel").toInt();
        if (Element.hasAttribute("DefaultMultimediaType"))  DefaultMultimediaType   =Element.attribute("DefaultMultimediaType").toInt();
        if (Element.hasAttribute("DefaultMultimediaModel")) DefaultMultimediaModel  =Element.attribute("DefaultMultimediaModel").toInt();
        if (Element.hasAttribute("DefaultForTheWEBType"))   DefaultForTheWEBType    =Element.attribute("DefaultForTheWEBType").toInt();
        if (Element.hasAttribute("DefaultForTheWEBModel"))  DefaultForTheWEBModel   =Element.attribute("DefaultForTheWEBModel").toInt();
        if (Element.hasAttribute("DefaultLossLess"))        DefaultLossLess         =Element.attribute("DefaultLossLess").toInt();
    }

    if ((domDocument.elementsByTagName("RecentFiles").length()>0)&&(domDocument.elementsByTagName("RecentFiles").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("RecentFiles").item(0).toElement();
        int i=0;
        while ((Element.elementsByTagName("Recent-"+QString("%1").arg(i)).length()>0)&&(domDocument.elementsByTagName("Recent-"+QString("%1").arg(i)).item(0).isElement()==true)) {
            QDomElement SubElement=domDocument.elementsByTagName("Recent-"+QString("%1").arg(i)).item(0).toElement();
            QString     File=SubElement.attribute("File");
            RecentFile.append(File);
            i++;
        }
    }

    //***************************************
    // Load RenderDeviceModel collection
    //***************************************
    // For compatibility with old version, try to load this list from ffDiaporama user config file
    DeviceModelList.LoadFromXML(domDocument,TypeConfigFile);

    // Load other collections
    StyleTextCollection.LoadFromXML(domDocument,TypeConfigFile);
    StyleTextBackgroundCollection.LoadFromXML(domDocument,TypeConfigFile);
    StyleBlockShapeCollection.LoadFromXML(domDocument,TypeConfigFile);

    // Load windows size and position
    DlgBackgroundPropertiesWSP->LoadFromXML(domDocument);
    DlgMusicPropertiesWSP->LoadFromXML(domDocument);
    DlgApplicationSettingsWSP->LoadFromXML(domDocument);
    DlgRenderVideoWSP->LoadFromXML(domDocument);
    DlgTransitionPropertiesWSP->LoadFromXML(domDocument);
    DlgTransitionDurationWSP->LoadFromXML(domDocument);
    DlgSlidePropertiesWSP->LoadFromXML(domDocument);
    DlgSlideDurationWSP->LoadFromXML(domDocument);
    DlgImageTransformationWSP->LoadFromXML(domDocument);
    DlgImageCorrectionWSP->LoadFromXML(domDocument);
    DlgTextEditWSP->LoadFromXML(domDocument);
    DlgManageStyleWSP->LoadFromXML(domDocument);
    DlgCheckConfigWSP->LoadFromXML(domDocument);
    DlgManageDevicesWSP->LoadFromXML(domDocument);
    DlgAboutWSP->LoadFromXML(domDocument);
    DlgffDPjrPropertiesWSP->LoadFromXML(domDocument);
    DlgInfoFileWSP->LoadFromXML(domDocument);
    DlgRulerDef->LoadFromXML(domDocument);
    DlgManageFavoriteWSP->LoadFromXML(domDocument);
    DlgFileExplorerWSP->LoadFromXML(domDocument);

    return true;
}
