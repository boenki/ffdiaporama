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

#ifndef CAPPLICATIONCONFIG_H
#define CAPPLICATIONCONFIG_H

#include <QDomElement>
#include <QDomDocument>
#include <QString>
#include "../sharedfiles/cBaseApplicationConfig.h"
#include "../sharedfiles/cBaseMediaFile.h"
#include "../sharedfiles/cDeviceModelDef.h"
#include "cJobQueue.h"

#define APPLICATION_GROUPNAME               "ffDiaporama"
#define APPLICATION_NAME                    "ffDiaporamaMMFiler"
#define APPLICATION_VERSION                 "1.2 devel"
#define CONFIGFILEEXT                       ".xml"                                // File extension of configuration files
#define CONFIGFILE_ROOTNAME                 "Configuration"                       // Name of root node in the config xml file

#define HELPFILE_DlgCheckConfig             "http://ffdiaporama.tuxfamily.org/?page_id=3968&lang=<local>"
#define HELPFILE_SUPPORT                    "http://ffdiaporama.tuxfamily.org/?page_id=3854&lang=<local>"
#define HELPFILE_NEWS                       "http://ffdiaporama.tuxfamily.org/?cat=5&lang=<local>"
#define HELPFILE_DlgApplicationSettings     "http://ffdiaporama.tuxfamily.org/?page_id=4205&lang=<local>"
#define HELPFILE_DlgManageDevices           "http://ffdiaporama.tuxfamily.org/?page_id=4023&lang=<local>"
#define HELPFILE_DlgInfoFile                "http://ffdiaporama.tuxfamily.org/?page_id=4206&lang=<local>"
#define HELPFILE_DlgJob                     "http://ffdiaporama.tuxfamily.org/?page_id=%1&lang=<local>"

#define ALLOWEDWEBLANGUAGE                  "en;fr;it;es;el;de"

class cApplicationConfig : public cBaseApplicationConfig {
public:
    // Device database
    cDeviceModelList        DeviceModelList;                            // List of known rendering device model
    cJobQueue               JobQueue;

    qlonglong               MemCacheMaxValue;                           // Maximum value for image cache
    QString                 CurrentPath;                                // Currently shown path

    // Default settings for jobs
    uint32_t                JobDefault[NBR_JOBTYPE];
    QString                 DefaultSourceSuffix[NBR_JOBTYPE];
    QString                 DefaultDestinationSuffix[NBR_JOBTYPE];
    QString                 DefaultOptions[NBR_JOBTYPE];
    QString                 DefaultSourceFolder;
    QString                 DefaultDestinationFolder;

    // Size & pos
    QString                 SplitterHSizeAndPos;                        // Splitter H between tree and table (size save)
    QString                 SplitterVSizeAndPos;                        // Splitter V between browser and job table (size save)
    cSaveWindowPosition     *DlgApplicationSettingsWSP;                 // Dialog box "Application settings" - Window size and position
    cSaveWindowPosition     *DlgCheckConfigWSP;                         // Dialog box "Check configuration" - Window size and position
    cSaveWindowPosition     *DlgAboutWSP;                               // Dialog box "About" - Window size and position
    cSaveWindowPosition     *DlgManageDevicesWSP;                       // Dialog box "Manage Devices" - Window size and position
    cSaveWindowPosition     *DlgInfoFileWSP;                            // Dialog box "File Information" - Window size and position
    cSaveWindowPosition     *DlgJobSettingsWSP;                         // Dialog box "Job settings" - Window size and position
    cSaveWindowPosition     *DlgGetFolderWSP;                           // Dialog box "Select a folder" - Window size and position

    explicit cApplicationConfig(QMainWindow *TopLevelWindow);
    ~cApplicationConfig();

    virtual bool            LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App);
    virtual bool            SaveConfigurationFile();
    virtual void            InitValues();
    virtual void            SaveValueToXML(QDomElement &domDocument);
    virtual bool            LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile);
};

#endif // CAPPLICATIONCONFIG_H
