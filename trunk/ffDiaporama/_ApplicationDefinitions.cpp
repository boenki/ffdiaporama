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

// Specific inclusions
#include "_ApplicationDefinitions.h"
#include "_Diaporama.h"

//#define DEBUGMODE

/****************************************************************************
  Other
****************************************************************************/
QString SystemProperties="";                                    // System properties log
QString CurrentAppVersion="";                                   // Application version read from BUILDVERSION.txt

void AddToSystemProperties(QString StringToAdd) {
    if (SystemProperties.length()>0) SystemProperties=SystemProperties+"\n";
    SystemProperties=SystemProperties+StringToAdd;
}

void AddSeparatorToSystemProperties() {
    AddToSystemProperties("------------------------------------------------------------------------------");
}

void ExitApplicationWithFatalError(QString StringToAdd) {
    AddSeparatorToSystemProperties();
    AddToSystemProperties(StringToAdd);
    fprintf(stderr,"%s\n",SystemProperties.toLocal8Bit().data());     // Print out SystemProperties
    exit(1);
}

//====================================================================================================================

cSaveDlgSlideProperties::cSaveDlgSlideProperties(QString WindowName,bool &RestoreWindow,bool IsMainWindow):cSaveWindowPosition(WindowName,RestoreWindow,IsMainWindow) {
    SplitterTop="";
    SplitterBottom="";
}

void cSaveDlgSlideProperties::ApplyToWindow(QWidget *Window,QSplitter *Top,QSplitter *Bottom) {
    cSaveWindowPosition::ApplyToWindow(Window);
    if (SplitterTop!="")    Top->restoreState(QByteArray::fromHex(SplitterTop.toUtf8()));
    if (SplitterBottom!="") Bottom->restoreState(QByteArray::fromHex(SplitterBottom.toUtf8()));
}

void cSaveDlgSlideProperties::SaveWindowState(QWidget *Window,QSplitter *Top,QSplitter *Bottom) {
    cSaveWindowPosition::SaveWindowState(Window);
    SplitterTop   =QString(QByteArray(Top->saveState()).toHex());
    SplitterBottom=QString(QByteArray(Bottom->saveState()).toHex());
}

void cSaveDlgSlideProperties::OverloadedSaveToXML(QDomElement &Element) {
    Element.setAttribute("SplitterTop",SplitterTop);
    Element.setAttribute("SplitterBottom",SplitterBottom);
}

void cSaveDlgSlideProperties::OverloadedLoadFromXML(QDomElement Element) {
    if (Element.hasAttribute("SplitterTop"))    SplitterTop=Element.attribute("SplitterTop");
    if (Element.hasAttribute("SplitterBottom")) SplitterBottom=Element.attribute("SplitterBottom");
}


//====================================================================================================================

cApplicationConfig::cApplicationConfig(QMainWindow *TheTopLevelWindow):cBaseApplicationConfig(TheTopLevelWindow,ALLOWEDWEBLANGUAGE,APPLICATION_NAME,APPLICATION_NAME,APPLICATION_VERSION,CONFIGFILEEXT,CONFIGFILE_ROOTNAME) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::cApplicationConfig";
    #endif
}

//====================================================================================================================
cApplicationConfig::~cApplicationConfig() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::~cApplicationConfig";
    #endif
    delete DlgMusicPropertiesWSP;
    delete DlgBackgroundPropertiesWSP;
    delete DlgApplicationSettingsWSP;
    delete DlgRenderVideoWSP;
    delete DlgTransitionPropertiesWSP;
    delete DlgSlidePropertiesWSP;
    delete DlgImageTransformationWSP;
    delete DlgImageCorrectionWSP;
    delete DlgVideoEditWSP;
    delete DlgTextEditWSP;
    delete DlgManageStyleWSP;
    delete DlgCheckConfigWSP;
    delete DlgManageDevicesWSP;
    delete DlgAboutWSP;
    delete DlgffDPjrPropertiesWSP;
}

//====================================================================================================================

void cApplicationConfig::InitValues() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::InitValues";
    #endif

    // Initialise all variables and set them default value
    NbrSlideInCache             = 5;                        // Number of slide in cache (Real compute is NbrSlideInCache*2+1)
    MemCacheMaxValue            = 256*1024*1024;            // 256 Mb for image cache
    AskUserToRemove             = true;                     // If true, user must answer to a confirmation dialog box to remove slide
    SortFile                    = true;                     // if true sort file by (last) number when multiple file insertion
    AppendObject                = false;                    // If true, new object will be append at the end of the diaporama, if false, new object will be insert after current position
    PartitionMode               = false;                    // If true, partition mode is on
    DisplayUnit                 = 1;                        // Display coordinates unit
    DefaultFraming              = 2;                        // 0=Width, 1=Height, 2=Full
    TimelineHeight              = 120;                      // Initial height of the timeline
    PreviewFPS                  = 12.5;                     // Preview FrameRate
    NoShotDuration              = 6000;                     // Default duration for fixed image when is alone (no shot)
    FixedDuration               = 3000;                     // Default duration for fixed image (msec)
    SpeedWave                   = SPEEDWAVE_LINEAR;         // Default speed wave methode
    ImageGeometry               = GEOMETRY_16_9;            // Project image geometry for image rendering
    SlideRuler                  = true;                     // if true, ruler is on in slide properties dialog box
    FramingRuler                = true;                     // if true, ruler is on in framing/correction dialog box
    DefaultTitleFilling         = 0;                        // Default Title filling mode
    DefaultAuthor               = "";                       // Default Author name

    DefaultFormat               = 1;                        // Default format = avi
    DefaultNameProjectName      =true;                      // Use project name as default name for rendering
    DefaultImageSize            = SIZE_720P;                // Default image size
    DefaultStandard             = STANDARD_PAL;             // Default standard (PAL/NTSC)

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

    #ifdef Q_OS_WIN
        LastProjectPath         = WINDOWS_DOCUMENTS;            // Last folder use for project
        LastRenderVideoPath     = WINDOWS_VIDEO;                // Last folder use for render video
        if (LastRenderVideoPath=="") LastRenderVideoPath=WINDOWS_DOCUMENTS;
        SDLAudioOldMode         = false;                        // If true SDL audio use old mode sample instead byte
    #endif
    #ifdef Q_WS_X11
        LastProjectPath         = QDir::home().absolutePath();  // Last folder use for project
        LastRenderVideoPath     = QDir::home().absolutePath();  // Last folder use for render video
        SDLAudioOldMode         = true;                         // If true SDL audio use old mode sample instead byte
    #endif

    AddToSystemProperties(QString(OPERATINGSYSTEM_STR)+Plateforme+" - "+QString("%1").arg(getCpuCount())+" Core/CPU");
    AddToSystemProperties(QString(USERCONFIGFILE_STR)+UserConfigFile);
    AddToSystemProperties(QString(GLOBALCONFIGFILE_STR)+GlobalConfigFile);

    // Init collections
    StyleTextCollection.CollectionName          =QString(STYLENAME_TEXTSTYLE);
    StyleTextBackgroundCollection.CollectionName=QString(STYLENAME_BACKGROUNDSTYLE);
    StyleCoordinateCollection.CollectionName    =QString(STYLENAME_COORDINATESTYLE);
    StyleBlockShapeCollection.CollectionName    =QString(STYLENAME_BLOCKSHAPESTYLE);
    StyleImageFramingCollection.CollectionName  =QString(STYLENAME_FRAMINGSTYLE);

    DlgBackgroundPropertiesWSP  =new cSaveWindowPosition("DlgBackgroundProperties",RestoreWindow,false);    // Dialog box "Background properties" - Window size and position
    DlgMusicPropertiesWSP       =new cSaveWindowPosition("DlgMusicProperties",RestoreWindow,false);         // Dialog box "Music properties" - Window size and position
    DlgApplicationSettingsWSP   =new cSaveWindowPosition("DlgApplicationSettings",RestoreWindow,false);     // Dialog box "Application settings" - Window size and position
    DlgRenderVideoWSP           =new cSaveWindowPosition("DlgRenderVideoWSP",RestoreWindow,false);          // Dialog box "Render Video" - Window size and position
    DlgTransitionPropertiesWSP  =new cSaveWindowPosition("DlgTransitionPropertiesWSP",RestoreWindow,false); // Dialog box "Transition properties" - Window size and position
    DlgSlidePropertiesWSP       =new cSaveDlgSlideProperties("DlgSlidePropertiesWSP",RestoreWindow,false);  // Dialog box "Slide properties" - Window size and position
    DlgImageTransformationWSP   =new cSaveWindowPosition("DlgImageTransformationWSP",RestoreWindow,false);  // Dialog box "Image transformation" - Window size and position
    DlgImageCorrectionWSP       =new cSaveWindowPosition("DlgImageCorrectionWSP",RestoreWindow,false);      // Dialog box "Image correction" - Window size and position
    DlgVideoEditWSP             =new cSaveWindowPosition("DlgVideoEditWSP",RestoreWindow,false);            // Dialog box "Edit video" - Window size and position
    DlgTextEditWSP              =new cSaveWindowPosition("DlgTextEditWSP",RestoreWindow,false);             // Dialog box "Text editor" - Window size and position
    DlgManageStyleWSP           =new cSaveWindowPosition("DlgManageStyleWSP",RestoreWindow,false);          // Dialog box "Manage style" - Window size and position
    DlgCheckConfigWSP           =new cSaveWindowPosition("DlgCheckConfigWSP",RestoreWindow,false);          // Dialog box "Check configuration" - Window size and position
    DlgManageDevicesWSP         =new cSaveWindowPosition("DlgManageDevicesWSP",RestoreWindow,false);        // Dialog box "Manage Devices" - Window size and position
    DlgAboutWSP                 =new cSaveWindowPosition("DlgAboutWSP",RestoreWindow,false);                // Dialog box "About" - Window size and position
    DlgffDPjrPropertiesWSP      =new cSaveWindowPosition("DlgffDPjrPropertiesWSP",RestoreWindow,false);     // Dialog box "Project properties" - Window size and position

    // Default new text block options
    DefaultBlock_Text_TextST    ="###GLOBALSTYLE###:0";
    DefaultBlock_Text_BackGST   ="###GLOBALSTYLE###:0";
    DefaultBlock_Text_ShapeST   ="###GLOBALSTYLE###:0";
    for (int i=0;i<3;i++) DefaultBlock_Text_CoordST[i]="###GLOBALSTYLE###:0";

    // Default new image block option (when slide creation)
    DefaultBlockSL_IMG_TextST   ="###GLOBALSTYLE###:0";
    DefaultBlockSL_IMG_ShapeST  ="###GLOBALSTYLE###:0";
    for (int i=0;i<9;i++) for (int j=0;j<3;j++) DefaultBlockSL_IMG_CoordST[i][j]="###GLOBALSTYLE###:0";

    // Default new image block option (when block add in slide dialog)
    DefaultBlockBA_IMG_TextST   ="###GLOBALSTYLE###:0";
    DefaultBlockBA_IMG_ShapeST  ="###GLOBALSTYLE###:0";
    for (int i=0;i<9;i++) for (int j=0;j<3;j++) DefaultBlockBA_IMG_CoordST[i][j]="###GLOBALSTYLE###:0";
    for (int i=0;i<3;i++) {
        DefaultBlockSL_CLIPARTLOCK[i]=0;
        DefaultBlockBA_CLIPARTLOCK[i]=0;
    }
}

//====================================================================================================================

bool cApplicationConfig::LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::LoadConfigurationFile";
    #endif
    return cBaseApplicationConfig::LoadConfigurationFile(TypeConfigFile,App) && DeviceModelList.LoadConfigurationFile(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile,TypeConfigFile);
}

//====================================================================================================================

bool cApplicationConfig::SaveConfigurationFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::SaveConfigurationValues";
    #endif
    return cBaseApplicationConfig::SaveConfigurationFile() && DeviceModelList.SaveConfigurationFile(UserConfigFile);
}

//====================================================================================================================

void cApplicationConfig::SaveValueToXML(QDomElement &domDocument) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::SaveValueToXML";
    #endif

    QDomElement     Element,SubElement,SubSubElement;
    QDomDocument    Document;

    // Save preferences
    if ((domDocument.elementsByTagName("LastDirectories").length()>0)&&(domDocument.elementsByTagName("LastDirectories").item(0).isElement()==true)) {
        Element=domDocument.elementsByTagName("LastDirectories").item(0).toElement();
        Element.setAttribute("LastProjectPath",         LastProjectPath);
        Element.setAttribute("LastRenderVideoPath",     LastRenderVideoPath);
    }

    Element=Document.createElement("EditorOptions");
    Element.setAttribute("NbrSlideInCache",             NbrSlideInCache);
    Element.setAttribute("MemCacheMaxValue",            MemCacheMaxValue);
    Element.setAttribute("SDLAudioOldMode",             SDLAudioOldMode?"1":"0");
    Element.setAttribute("AppendObject",                AppendObject?"1":"0");
    Element.setAttribute("DisplayUnit",                 DisplayUnit);
    Element.setAttribute("PartitionMode",               PartitionMode?"1":"0");
    Element.setAttribute("SortFile",                    SortFile?"1":"0");
    Element.setAttribute("TimelineHeight",              TimelineHeight);
    Element.setAttribute("DefaultFraming",              DefaultFraming);
    Element.setAttribute("PreviewFPS",                  (QString("%1").arg(PreviewFPS,0,'f')));
    Element.setAttribute("RandomTransition",            RandomTransition?"1":"0");
    Element.setAttribute("DefaultTransitionFamilly",    DefaultTransitionFamilly);
    Element.setAttribute("DefaultTransitionSubType",    DefaultTransitionSubType);
    Element.setAttribute("DefaultTransitionDuration",   DefaultTransitionDuration);
    Element.setAttribute("AskUserToRemove",             AskUserToRemove?"1":"0");
    Element.setAttribute("SlideRuler",                  SlideRuler?"1":"0");
    Element.setAttribute("FramingRuler",                FramingRuler?"1":"0");
    domDocument.appendChild(Element);

    Element=Document.createElement("ProjectDefault");
    Element.setAttribute("ImageGeometry",               ImageGeometry);
    Element.setAttribute("NoShotDuration",              NoShotDuration);
    Element.setAttribute("FixedDuration",               FixedDuration);
    Element.setAttribute("SpeedWave",                   SpeedWave);
    Element.setAttribute("DefaultAuthor",               DefaultAuthor);
    Element.setAttribute("DefaultTitleFilling",         DefaultTitleFilling);

    SubElement=Document.createElement("DefaultBlock_Text");
    SubElement.setAttribute("TextST",    DefaultBlock_Text_TextST);
    SubElement.setAttribute("BackGST",   DefaultBlock_Text_BackGST);
    SubElement.setAttribute("Coord0ST",  DefaultBlock_Text_CoordST[0]);
    SubElement.setAttribute("Coord1ST",  DefaultBlock_Text_CoordST[1]);
    SubElement.setAttribute("Coord2ST",  DefaultBlock_Text_CoordST[2]);
    SubElement.setAttribute("ShapeST",   DefaultBlock_Text_ShapeST);
    Element.appendChild(SubElement);

    SubElement=Document.createElement("DefaultBlockSL_IMG");
    SubElement.setAttribute("TextST",    DefaultBlockSL_IMG_TextST);
    SubElement.setAttribute("ShapeST",   DefaultBlockSL_IMG_ShapeST);
    for (int i=0;i<9;i++) {
        SubSubElement=Document.createElement(QString("IMG_GEO_%1").arg(i));
        for (int j=0;j<3;j++) SubSubElement.setAttribute(QString("CoordST_%1").arg(j),DefaultBlockSL_IMG_CoordST[i][j]);
        SubElement.appendChild(SubSubElement);
        for (int j=0;j<3;j++) SubElement.setAttribute(QString("LockLS_%1").arg(j),DefaultBlockSL_CLIPARTLOCK[j]);
    }
    Element.appendChild(SubElement);

    SubElement=Document.createElement("DefaultBlockBA_IMG");
    SubElement.setAttribute("TextST",    DefaultBlockBA_IMG_TextST);
    SubElement.setAttribute("ShapeST",   DefaultBlockBA_IMG_ShapeST);
    for (int i=0;i<9;i++) {
        SubSubElement=Document.createElement(QString("IMG_GEO_%1").arg(i));
        for (int j=0;j<3;j++) SubSubElement.setAttribute(QString("CoordST_%1").arg(j),DefaultBlockBA_IMG_CoordST[i][j]);
        SubElement.appendChild(SubSubElement);
    }
    for (int j=0;j<3;j++) SubElement.setAttribute(QString("LockBA_%1").arg(j),DefaultBlockBA_CLIPARTLOCK[j]);
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
    Element.setAttribute("ImageSize",                   DefaultImageSize);
    Element.setAttribute("DefaultSmartphoneType",       DefaultSmartphoneType);
    Element.setAttribute("DefaultSmartphoneModel",      DefaultSmartphoneModel);
    Element.setAttribute("DefaultMultimediaType",       DefaultMultimediaType);
    Element.setAttribute("DefaultMultimediaModel",      DefaultMultimediaModel);
    Element.setAttribute("DefaultForTheWEBType",        DefaultForTheWEBType);
    Element.setAttribute("DefaultForTheWEBModel",       DefaultForTheWEBModel);
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
    StyleCoordinateCollection.SaveToXML(domDocument);
    StyleBlockShapeCollection.SaveToXML(domDocument);
    StyleImageFramingCollection.SaveToXML(domDocument);

    // Save windows size and position
    DlgBackgroundPropertiesWSP->SaveToXML(domDocument);
    DlgMusicPropertiesWSP->SaveToXML(domDocument);
    DlgApplicationSettingsWSP->SaveToXML(domDocument);
    DlgRenderVideoWSP->SaveToXML(domDocument);
    DlgTransitionPropertiesWSP->SaveToXML(domDocument);
    DlgSlidePropertiesWSP->SaveToXML(domDocument);
    DlgImageTransformationWSP->SaveToXML(domDocument);
    DlgImageCorrectionWSP->SaveToXML(domDocument);
    DlgVideoEditWSP->SaveToXML(domDocument);
    DlgTextEditWSP->SaveToXML(domDocument);
    DlgManageStyleWSP->SaveToXML(domDocument);
    DlgCheckConfigWSP->SaveToXML(domDocument);
    DlgManageDevicesWSP->SaveToXML(domDocument);
    DlgAboutWSP->SaveToXML(domDocument);
    DlgffDPjrPropertiesWSP->SaveToXML(domDocument);
}

//====================================================================================================================

bool cApplicationConfig::LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::LoadValueFromXML";
    #endif

    // Load preferences
    if ((domDocument.elementsByTagName("LastDirectories").length()>0)&&(domDocument.elementsByTagName("LastDirectories").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("LastDirectories").item(0).toElement();
        if (Element.hasAttribute("LastProjectPath"))            LastProjectPath             =Element.attribute("LastProjectPath");
        if (Element.hasAttribute("LastRenderVideoPath"))        LastRenderVideoPath         =Element.attribute("LastRenderVideoPath");
    }

    if ((domDocument.elementsByTagName("EditorOptions").length()>0)&&(domDocument.elementsByTagName("EditorOptions").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("EditorOptions").item(0).toElement();
        if (Element.hasAttribute("NbrSlideInCache"))            NbrSlideInCache             =Element.attribute("NbrSlideInCache").toInt();
        if (Element.hasAttribute("MemCacheMaxValue"))           MemCacheMaxValue            =Element.attribute("MemCacheMaxValue").toLongLong();
        if (Element.hasAttribute("SDLAudioOldMode"))            SDLAudioOldMode             =Element.attribute("SDLAudioOldMode")=="1";
        if (Element.hasAttribute("AppendObject"))               AppendObject                =Element.attribute("AppendObject")=="1";
        if (Element.hasAttribute("PartitionMode"))              PartitionMode               =Element.attribute("PartitionMode")=="1";
        if (Element.hasAttribute("DisplayUnit"))                DisplayUnit                 =Element.attribute("DisplayUnit").toInt();
        if (Element.hasAttribute("SortFile"))                   SortFile                    =Element.attribute("SortFile")=="1";
        if (Element.hasAttribute("TimelineHeight"))             TimelineHeight              =Element.attribute("TimelineHeight").toInt();
        if (Element.hasAttribute("DefaultFraming"))             DefaultFraming              =Element.attribute("DefaultFraming").toInt();
        if (Element.hasAttribute("PreviewFPS"))                 PreviewFPS                  =Element.attribute("PreviewFPS").toDouble();
        if (Element.hasAttribute("RandomTransition"))           RandomTransition            =Element.attribute("RandomTransition")=="1";
        if (Element.hasAttribute("DefaultTransitionFamilly"))   DefaultTransitionFamilly    =Element.attribute("DefaultTransitionFamilly").toInt();
        if (Element.hasAttribute("DefaultTransitionSubType"))   DefaultTransitionSubType    =Element.attribute("DefaultTransitionSubType").toInt();
        if (Element.hasAttribute("DefaultTransitionDuration"))  DefaultTransitionDuration   =Element.attribute("DefaultTransitionDuration").toInt();
        if (Element.hasAttribute("AskUserToRemove"))            AskUserToRemove             =Element.attribute("AskUserToRemove")!="0";
        if (Element.hasAttribute("SlideRuler"))                 SlideRuler                  =Element.attribute("SlideRuler")!="0";
        if (Element.hasAttribute("FramingRuler"))               FramingRuler                =Element.attribute("FramingRuler")!="0";
    }

    if ((domDocument.elementsByTagName("ProjectDefault").length()>0)&&(domDocument.elementsByTagName("ProjectDefault").item(0).isElement()==true)) {
        QDomElement  Element=domDocument.elementsByTagName("ProjectDefault").item(0).toElement();
        if (Element.hasAttribute("ImageGeometry"))              ImageGeometry               =Element.attribute("ImageGeometry").toInt();
        if (Element.hasAttribute("NoShotDuration"))             NoShotDuration              =Element.attribute("NoShotDuration").toInt();
        if (Element.hasAttribute("FixedDuration"))              FixedDuration               =Element.attribute("FixedDuration").toInt();
        if (Element.hasAttribute("SpeedWave"))                  SpeedWave                   =Element.attribute("SpeedWave").toInt();
        if (Element.hasAttribute("DefaultTitleFilling"))        DefaultTitleFilling         =Element.attribute("DefaultTitleFilling").toInt();
        if (Element.hasAttribute("DefaultAuthor"))              DefaultAuthor               =Element.attribute("DefaultAuthor");

        if ((Element.elementsByTagName("DefaultBlock_Text").length()>0)&&(Element.elementsByTagName("DefaultBlock_Text").item(0).isElement()==true)) {
            QDomElement SubElement=Element.elementsByTagName("DefaultBlock_Text").item(0).toElement();
            if (SubElement.hasAttribute("TextST"))   DefaultBlock_Text_TextST    =SubElement.attribute("TextST");
            if (SubElement.hasAttribute("BackGST"))  DefaultBlock_Text_BackGST   =SubElement.attribute("BackGST");
            if (SubElement.hasAttribute("Coord0ST")) DefaultBlock_Text_CoordST[0]=SubElement.attribute("Coord0ST");
            if (SubElement.hasAttribute("Coord1ST")) DefaultBlock_Text_CoordST[1]=SubElement.attribute("Coord1ST");
            if (SubElement.hasAttribute("Coord2ST")) DefaultBlock_Text_CoordST[2]=SubElement.attribute("Coord2ST");
            if (SubElement.hasAttribute("ShapeST"))  DefaultBlock_Text_ShapeST   =SubElement.attribute("ShapeST");
        }
        if ((Element.elementsByTagName("DefaultBlockSL_IMG").length()>0)&&(Element.elementsByTagName("DefaultBlockSL_IMG").item(0).isElement()==true)) {
            QDomElement SubElement=Element.elementsByTagName("DefaultBlockSL_IMG").item(0).toElement();
            if (SubElement.hasAttribute("TextST"))   DefaultBlockSL_IMG_TextST    =SubElement.attribute("TextST");
            if (SubElement.hasAttribute("ShapeST"))  DefaultBlockSL_IMG_ShapeST   =SubElement.attribute("ShapeST");
            for (int i=0;i<9;i++) if ((SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).length()>0)&&(SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).item(0).isElement()==true)) {
                QDomElement SubSubElement=SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).item(0).toElement();
                for (int j=0;j<3;j++) if (SubSubElement.hasAttribute(QString("CoordST_%1").arg(j))) DefaultBlockSL_IMG_CoordST[i][j]=SubSubElement.attribute(QString("CoordST_%1").arg(j));
            }
            for (int j=0;j<3;j++) if (SubElement.hasAttribute(QString("LockLS_%1").arg(j))) DefaultBlockSL_CLIPARTLOCK[j]=SubElement.attribute(QString("LockLS_%1").arg(j)).toInt();
        }

        if ((Element.elementsByTagName("DefaultBlockBA_IMG").length()>0)&&(Element.elementsByTagName("DefaultBlockBA_IMG").item(0).isElement()==true)) {
            QDomElement SubElement=Element.elementsByTagName("DefaultBlockBA_IMG").item(0).toElement();
            if (SubElement.hasAttribute("TextST"))   DefaultBlockBA_IMG_TextST    =SubElement.attribute("TextST");
            if (SubElement.hasAttribute("ShapeST"))  DefaultBlockBA_IMG_ShapeST   =SubElement.attribute("ShapeST");
            for (int i=0;i<9;i++) if ((SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).length()>0)&&(SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).item(0).isElement()==true)) {
                QDomElement SubSubElement=SubElement.elementsByTagName(QString("IMG_GEO_%1").arg(i)).item(0).toElement();
                for (int j=0;j<3;j++) if (SubSubElement.hasAttribute(QString("CoordST_%1").arg(j))) DefaultBlockBA_IMG_CoordST[i][j]=SubSubElement.attribute(QString("CoordST_%1").arg(j));
            }
            for (int j=0;j<3;j++) if (SubElement.hasAttribute(QString("LockBA_%1").arg(j))) DefaultBlockBA_CLIPARTLOCK[j]=SubElement.attribute(QString("LockBA_%1").arg(j)).toInt();
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
        if (Element.hasAttribute("DefaultSmartphoneType"))  DefaultSmartphoneType   =Element.attribute("DefaultSmartphoneType").toInt();
        if (Element.hasAttribute("DefaultSmartphoneModel")) DefaultSmartphoneModel  =Element.attribute("DefaultSmartphoneModel").toInt();
        if (Element.hasAttribute("DefaultMultimediaType"))  DefaultMultimediaType   =Element.attribute("DefaultMultimediaType").toInt();
        if (Element.hasAttribute("DefaultMultimediaModel")) DefaultMultimediaModel  =Element.attribute("DefaultMultimediaModel").toInt();
        if (Element.hasAttribute("DefaultForTheWEBType"))   DefaultForTheWEBType    =Element.attribute("DefaultForTheWEBType").toInt();
        if (Element.hasAttribute("DefaultForTheWEBModel"))  DefaultForTheWEBModel   =Element.attribute("DefaultForTheWEBModel").toInt();
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
    StyleCoordinateCollection.LoadFromXML(domDocument,TypeConfigFile);
    StyleBlockShapeCollection.LoadFromXML(domDocument,TypeConfigFile);
    StyleImageFramingCollection.LoadFromXML(domDocument,TypeConfigFile);

    // Load windows size and position
    DlgBackgroundPropertiesWSP->LoadFromXML(domDocument);
    DlgMusicPropertiesWSP->LoadFromXML(domDocument);
    DlgApplicationSettingsWSP->LoadFromXML(domDocument);
    DlgRenderVideoWSP->LoadFromXML(domDocument);
    DlgTransitionPropertiesWSP->LoadFromXML(domDocument);
    DlgSlidePropertiesWSP->LoadFromXML(domDocument);
    DlgImageTransformationWSP->LoadFromXML(domDocument);
    DlgImageCorrectionWSP->LoadFromXML(domDocument);
    DlgVideoEditWSP->LoadFromXML(domDocument);
    DlgTextEditWSP->LoadFromXML(domDocument);
    DlgManageStyleWSP->LoadFromXML(domDocument);
    DlgCheckConfigWSP->LoadFromXML(domDocument);
    DlgManageDevicesWSP->LoadFromXML(domDocument);
    DlgAboutWSP->LoadFromXML(domDocument);
    DlgffDPjrPropertiesWSP->LoadFromXML(domDocument);

    return true;
}
