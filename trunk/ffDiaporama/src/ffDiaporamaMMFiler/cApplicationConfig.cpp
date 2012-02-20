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

//====================================================================================================================

cApplicationConfig::cApplicationConfig(QMainWindow *TopLevelWindow):cBaseApplicationConfig(TopLevelWindow,ALLOWEDWEBLANGUAGE,APPLICATION_GROUPNAME,APPLICATION_NAME,APPLICATION_VERSION,CONFIGFILEEXT,CONFIGFILE_ROOTNAME) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::cApplicationConfig");

    MemCacheMaxValue=1024*1024*512;
}

//====================================================================================================================

cApplicationConfig::~cApplicationConfig() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::~cApplicationConfig");

    delete DlgApplicationSettingsWSP;
    delete DlgJobSettingsWSP;
    delete DlgCheckConfigWSP;
    delete DlgManageDevicesWSP;
    delete DlgInfoFileWSP;
    delete DlgAboutWSP;
    delete DlgGetFolderWSP;
}

//====================================================================================================================

bool cApplicationConfig::LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::LoadConfigurationFile");

    return cBaseApplicationConfig::LoadConfigurationFile(TypeConfigFile,App) && DeviceModelList.LoadConfigurationFile(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile,TypeConfigFile);
}

//====================================================================================================================

bool cApplicationConfig::SaveConfigurationFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::SaveConfigurationValues");

    return cBaseApplicationConfig::SaveConfigurationFile() && DeviceModelList.SaveConfigurationFile(UserConfigFile);
}

//====================================================================================================================

void cApplicationConfig::InitValues() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::InitValues");

    SplitterHSizeAndPos     ="";
    SplitterVSizeAndPos     ="";

    JobDefault[JOBTYPE_NOTDEFINED]          =0;                                 DefaultSourceSuffix[JOBTYPE_NOTDEFINED]         ="";        DefaultDestinationSuffix[JOBTYPE_NOTDEFINED]        ="";
    JobDefault[JOBTYPE_OPENFILE]            =0;                                 DefaultSourceSuffix[JOBTYPE_OPENFILE]           ="";        DefaultDestinationSuffix[JOBTYPE_OPENFILE]          ="";
    JobDefault[JOBTYPE_DISPLAYINFO]         =0;                                 DefaultSourceSuffix[JOBTYPE_DISPLAYINFO]        ="";        DefaultDestinationSuffix[JOBTYPE_DISPLAYINFO]       ="";
    JobDefault[JOBTYPE_IMAGE_CONVERTIMAGE]  =JOBDEFAULT_IMAGE_CONVERTIMAGE;     DefaultSourceSuffix[JOBTYPE_IMAGE_CONVERTIMAGE] ="old";     DefaultDestinationSuffix[JOBTYPE_IMAGE_CONVERTIMAGE]="new";     DefaultOptions[JOBTYPE_IMAGE_CONVERTIMAGE]="jpg;";
    JobDefault[JOBTYPE_AUDIO_CONVERTAUDIO]  =JOBDEFAULT_AUDIO_CONVERTAUDIO;     DefaultSourceSuffix[JOBTYPE_AUDIO_CONVERTAUDIO] ="old";     DefaultDestinationSuffix[JOBTYPE_AUDIO_CONVERTAUDIO]="new";     DefaultOptions[JOBTYPE_AUDIO_CONVERTAUDIO]="mp3;";
    JobDefault[JOBTYPE_VIDEO_CONVERTVIDEO]  =JOBDEFAULT_VIDEO_CONVERTVIDEO;     DefaultSourceSuffix[JOBTYPE_VIDEO_CONVERTVIDEO] ="old";     DefaultDestinationSuffix[JOBTYPE_VIDEO_CONVERTVIDEO]="new";     DefaultOptions[JOBTYPE_VIDEO_CONVERTVIDEO]="mp4;";

    DefaultSourceFolder     ="";
    DefaultDestinationFolder="";

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
    DlgJobSettingsWSP        =new cSaveWindowPosition("DlgJobSettingsWSP",RestoreWindow,false);         // Dialog box "Job settings" - Window size and position
    DlgGetFolderWSP          =new cSaveWindowPosition("DlgGetFolderWSP",RestoreWindow,false);           // Dialog box "Select a folder" - Window size and position
}

//====================================================================================================================

void cApplicationConfig::SaveValueToXML(QDomElement &domDocument) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::SaveValueToXML");

    QDomDocument    Document;
    QDomElement     Element;

    Element=Document.createElement("Options");
    Element.setAttribute("SplitterHSizeAndPos",         SplitterHSizeAndPos);
    Element.setAttribute("SplitterVSizeAndPos",         SplitterVSizeAndPos);
    Element.setAttribute("CurrentPath",                 CurrentPath);
    Element.setAttribute("DefaultSourceFolder",         DefaultSourceFolder);
    Element.setAttribute("DefaultDestinationFolder",    DefaultDestinationFolder);
    domDocument.appendChild(Element);

    for (int i=0;i<NBR_JOBTYPE;i++) {
        Element=Document.createElement(QString("DefaultJobSettings_%1").arg(i));
        Element.setAttribute("JobDefault",              JobDefault[i]);
        Element.setAttribute("DefaultOptions",          DefaultOptions[i]);
        Element.setAttribute("DefaultSourceSuffix",     DefaultSourceSuffix[i]);
        Element.setAttribute("DefaultDestinationSuffix",DefaultDestinationSuffix[i]);
        domDocument.appendChild(Element);
    }

    DlgApplicationSettingsWSP->SaveToXML(domDocument);
    DlgCheckConfigWSP->SaveToXML(domDocument);
    DlgAboutWSP->SaveToXML(domDocument);
    DlgManageDevicesWSP->SaveToXML(domDocument);
    DlgInfoFileWSP->SaveToXML(domDocument);
    DlgJobSettingsWSP->SaveToXML(domDocument);
    DlgGetFolderWSP->SaveToXML(domDocument);
    JobQueue.SaveToXML(domDocument,"JobList");
}

//====================================================================================================================

bool cApplicationConfig::LoadValueFromXML(QDomElement domDocument,LoadConfigFileType /*TypeConfigFile*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cApplicationConfig::LoadValueFromXML");

    if ((domDocument.elementsByTagName("Options").length()>0)&&(domDocument.elementsByTagName("Options").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("Options").item(0).toElement();
        if (Element.hasAttribute("SplitterHSizeAndPos"))        SplitterHSizeAndPos         =Element.attribute("SplitterHSizeAndPos");
        if (Element.hasAttribute("SplitterVSizeAndPos"))        SplitterVSizeAndPos         =Element.attribute("SplitterVSizeAndPos");
        if (Element.hasAttribute("CurrentPath"))                CurrentPath                 =Element.attribute("CurrentPath");
        if (Element.hasAttribute("DefaultSourceFolder"))        DefaultSourceFolder         =Element.attribute("DefaultSourceFolder");
        if (Element.hasAttribute("DefaultDestinationFolder"))   DefaultDestinationFolder    =Element.attribute("DefaultDestinationFolder");
    }

    for (int i=0;i<NBR_JOBTYPE;i++) if ((domDocument.elementsByTagName(QString("DefaultJobSettings_%1").arg(i)).length()>0)&&(domDocument.elementsByTagName(QString("DefaultJobSettings_%1").arg(i)).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(QString("DefaultJobSettings_%1").arg(i)).item(0).toElement();
        if (Element.hasAttribute("JobDefault"))                 JobDefault[i]               =Element.attribute("JobDefault").toInt();
        if (Element.hasAttribute("DefaultOptions"))             DefaultOptions[i]           =Element.attribute("DefaultOptions");
        if (Element.hasAttribute("DefaultSourceSuffix"))        DefaultSourceSuffix[i]      =Element.attribute("DefaultSourceSuffix");
        if (Element.hasAttribute("DefaultDestinationSuffix"))   DefaultDestinationSuffix[i] =Element.attribute("DefaultDestinationSuffix");
        domDocument.appendChild(Element);
    }

    DlgApplicationSettingsWSP->LoadFromXML(domDocument);
    DlgCheckConfigWSP->LoadFromXML(domDocument);
    DlgAboutWSP->LoadFromXML(domDocument);
    DlgManageDevicesWSP->LoadFromXML(domDocument);
    DlgInfoFileWSP->LoadFromXML(domDocument);
    DlgJobSettingsWSP->LoadFromXML(domDocument);
    DlgGetFolderWSP->LoadFromXML(domDocument);
    JobQueue.LoadFromXML(domDocument,"JobList");
    return true;
}
