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

#define ALLOWEDWEBLANGUAGE                  "en;fr;it;es;el;de"

class cApplicationConfig : public cBaseApplicationConfig {
public:
    // Device database
    cDeviceModelList        DeviceModelList;                            // List of known rendering device model
    cJobQueue               JobQueue;

    QString                 SplitterSizeAndPos;                         // Splitter size save
    bool                    ShowHiddenFilesAndDir;                      // If true, hidden files will be show
    bool                    ShowMntDrive;                               // If true, show drives under /mnt/ [Linux only]
    bool                    ShowFoldersFirst;                           // If true, display folders at first in table list
    QString                 CurrentPath;                                // Currently shown path
    int                     CurrentFilter;                              // Currently selected filter
    int                     CurrentMode;                                // Currently selected display mode
    cSaveWindowPosition     *DlgApplicationSettingsWSP;                 // Dialog box "Application settings" - Window size and position
    cSaveWindowPosition     *DlgCheckConfigWSP;                         // Dialog box "Check configuration" - Window size and position
    cSaveWindowPosition     *DlgAboutWSP;                               // Dialog box "About" - Window size and position
    cSaveWindowPosition     *DlgManageDevicesWSP;                       // Dialog box "Manage Devices" - Window size and position
    cSaveWindowPosition     *DlgInfoFileWSP;                            // Dialog box "File Information" - Window size and position

    explicit cApplicationConfig(QMainWindow *TopLevelWindow);
    ~cApplicationConfig();

    virtual bool            LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App);
    virtual bool            SaveConfigurationFile();
    virtual void            InitValues();
    virtual void            SaveValueToXML(QDomElement &domDocument);
    virtual bool            LoadValueFromXML(QDomElement domDocument,LoadConfigFileType TypeConfigFile);
};

#endif // CAPPLICATIONCONFIG_H
