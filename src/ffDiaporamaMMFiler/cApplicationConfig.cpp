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

#include <QApplication>
#include <QtDebug>

#include "../sharedfiles/QCustomFolderTable.h"
#include "cApplicationConfig.h"

//#define DEBUGMODE

//====================================================================================================================

cApplicationConfig::cApplicationConfig(QMainWindow *TopLevelWindow):cBaseApplicationConfig(TopLevelWindow,ALLOWEDWEBLANGUAGE,APPLICATION_GROUPNAME,APPLICATION_NAME,APPLICATION_VERSION,CONFIGFILEEXT,CONFIGFILE_ROOTNAME) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::cApplicationConfig";
    #endif
}

//====================================================================================================================

cApplicationConfig::~cApplicationConfig() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::~cApplicationConfig";
    #endif

    delete DlgApplicationSettingsWSP;
    delete DlgCheckConfigWSP;
    delete DlgManageDevicesWSP;
    delete DlgInfoFileWSP;
    delete DlgAboutWSP;
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

void cApplicationConfig::InitValues() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::InitValues";
    #endif

    SplitterSizeAndPos      ="";
    ShowHiddenFilesAndDir   =false;
    ShowMntDrive            =false;
    ShowFoldersFirst        =true;
    CurrentFilter           =OBJECTTYPE_UNMANAGED;
    CurrentMode             =DISPLAY_DATA;

    // Thumbnail size in big icon mode
    Image_ThumbWidth =300;
    Image_ThumbHeight=200;
    Music_ThumbWidth =200;
    Music_ThumbHeight=200;
    Video_ThumbWidth =162;
    Video_ThumbHeight=216;

    #if defined(Q_OS_WIN)
    QSettings Settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",QSettings::NativeFormat);
    CurrentPath=Settings.value("Personal").toString();
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    CurrentPath="~";   // User home folder
    #endif

    DlgApplicationSettingsWSP=new cSaveWindowPosition("DlgApplicationSettings",RestoreWindow,false);    // Dialog box "Application settings" - Window size and position
    DlgCheckConfigWSP        =new cSaveWindowPosition("DlgCheckConfigWSP",RestoreWindow,false);         // Dialog box "Check configuration" - Window size and position
    DlgManageDevicesWSP      =new cSaveWindowPosition("DlgManageDevicesWSP",RestoreWindow,false);       // Dialog box "Manage Devices" - Window size and position
    DlgAboutWSP              =new cSaveWindowPosition("DlgAboutWSP",RestoreWindow,false);               // Dialog box "About" - Window size and position
    DlgInfoFileWSP           =new cSaveWindowPosition("DlgInfoFileWSP",RestoreWindow,false);            // Dialog box "File Information" - Window size and position
}

//====================================================================================================================

void cApplicationConfig::SaveValueToXML(QDomElement &domDocument) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::SaveValueToXML";
    #endif

    QDomDocument    Document;
    QDomElement     Element;

    Element=Document.createElement("Options");
    Element.setAttribute("SplitterSizeAndPos",      SplitterSizeAndPos);
    Element.setAttribute("ShowHiddenFilesAndDir",   ShowHiddenFilesAndDir?"1":"0");
    Element.setAttribute("ShowMntDrive",            ShowMntDrive?"1":"0");
    Element.setAttribute("ShowFoldersFirst",        ShowFoldersFirst?"1":"0");
    Element.setAttribute("CurrentPath",             CurrentPath);
    Element.setAttribute("CurrentFilter",           CurrentFilter);
    Element.setAttribute("CurrentMode",             CurrentMode);

    Element.setAttribute("Image_ThumbWidth",        Image_ThumbWidth);
    Element.setAttribute("Image_ThumbHeight",       Image_ThumbHeight);
    Element.setAttribute("Music_ThumbWidth",        Music_ThumbWidth);
    Element.setAttribute("Music_ThumbHeight",       Music_ThumbHeight);
    Element.setAttribute("Video_ThumbWidth",        Video_ThumbWidth);
    Element.setAttribute("Video_ThumbHeight",       Video_ThumbHeight);

    domDocument.appendChild(Element);
    DlgApplicationSettingsWSP->SaveToXML(domDocument);
    DlgCheckConfigWSP->SaveToXML(domDocument);
    DlgAboutWSP->SaveToXML(domDocument);
    DlgManageDevicesWSP->SaveToXML(domDocument);
    DlgInfoFileWSP->SaveToXML(domDocument);
    JobQueue.SaveToXML(domDocument,"JobList");
}

//====================================================================================================================

bool cApplicationConfig::LoadValueFromXML(QDomElement domDocument,LoadConfigFileType /*TypeConfigFile*/) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cApplicationConfig::LoadValueFromXML";
    #endif

    if ((domDocument.elementsByTagName("Options").length()>0)&&(domDocument.elementsByTagName("Options").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("Options").item(0).toElement();
        if (Element.hasAttribute("SplitterSizeAndPos"))     SplitterSizeAndPos=Element.attribute("SplitterSizeAndPos");
        if (Element.hasAttribute("ShowHiddenFilesAndDir"))  ShowHiddenFilesAndDir=Element.attribute("ShowHiddenFilesAndDir")=="1";
        if (Element.hasAttribute("ShowMntDrive"))           ShowMntDrive=Element.attribute("ShowMntDrive")=="1";
        if (Element.hasAttribute("ShowFoldersFirst"))       ShowFoldersFirst=Element.attribute("ShowFoldersFirst")=="1";
        if (Element.hasAttribute("CurrentPath"))            CurrentPath=Element.attribute("CurrentPath");
        if (Element.hasAttribute("CurrentFilter"))          CurrentFilter=Element.attribute("CurrentFilter").toInt();
        if (Element.hasAttribute("CurrentMode"))            CurrentMode=Element.attribute("CurrentMode").toInt();

        if (Element.hasAttribute("Image_ThumbWidth"))       Image_ThumbWidth=Element.attribute("Image_ThumbWidth").toInt();
        if (Element.hasAttribute("Image_ThumbHeight"))      Image_ThumbHeight=Element.attribute("Image_ThumbHeight").toInt();
        if (Element.hasAttribute("Music_ThumbWidth"))       Music_ThumbWidth=Element.attribute("Music_ThumbWidth").toInt();
        if (Element.hasAttribute("Music_ThumbHeight"))      Music_ThumbHeight=Element.attribute("Music_ThumbHeight").toInt();
        if (Element.hasAttribute("Video_ThumbWidth"))       Video_ThumbWidth=Element.attribute("Video_ThumbWidth").toInt();
        if (Element.hasAttribute("Video_ThumbHeight"))      Video_ThumbHeight=Element.attribute("Video_ThumbHeight").toInt();
    }
    DlgApplicationSettingsWSP->LoadFromXML(domDocument);
    DlgCheckConfigWSP->LoadFromXML(domDocument);
    DlgAboutWSP->LoadFromXML(domDocument);
    DlgManageDevicesWSP->LoadFromXML(domDocument);
    DlgInfoFileWSP->LoadFromXML(domDocument);
    JobQueue.LoadFromXML(domDocument,"JobList");
    return true;
}

