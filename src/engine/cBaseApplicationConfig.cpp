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

// Include some common various class
#include "cBaseApplicationConfig.h"
#include "cDeviceModelDef.h"

// Include some additional standard class
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QTranslator>
#include <QTextDocument>

//*****************************************************************************************************************************************

#ifdef Q_OS_LINUX
    bool SearchRasterMode(QString ApplicationName,QString ConfigFileExt,QString ConfigFileRootName) {
        ToLog(LOGMSG_DEBUGTRACE,"IN:SearchRasterMode");

        QString UserConfigPath;    // Path and filename to user profil path
        QString UserConfigFile;    // Path and filename to user configuration file
        QString GlobalConfigFile;  // Path and filename to global configuration file (in binary directory)
        bool    RasterMode=true;

        UserConfigPath=QDir::homePath();
        if (UserConfigPath[UserConfigPath.length()-1]!=QDir::separator()) UserConfigPath=UserConfigPath+QDir::separator();
        UserConfigPath  = UserConfigPath+"."+ApplicationName+QDir::separator();
        GlobalConfigFile=QFileInfo(ApplicationName+ConfigFileExt).absoluteFilePath();
        UserConfigFile  =QFileInfo(UserConfigPath+ApplicationName+ConfigFileExt).absoluteFilePath();


        QFile           file(GlobalConfigFile);
        QDomDocument    domDocument;
        QDomElement     root;
        QString         errorStr;
        int             errorLine,errorColumn;

        // Load Global preferences
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            if (domDocument.setContent(&file,true,&errorStr,&errorLine,&errorColumn)) {
                root = domDocument.documentElement();
                if ((root.tagName()==ConfigFileRootName)&&(root.elementsByTagName("GlobalPreferences").length()>0)&&(root.elementsByTagName("GlobalPreferences").item(0).isElement()==true)) {
                    ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Search Raster mode in configuration file")+GlobalConfigFile);
                    QDomElement Element=root.elementsByTagName("GlobalPreferences").item(0).toElement();
                    if (Element.hasAttribute("RasterMode")) RasterMode=Element.attribute("RasterMode")=="1";
                }
            }
            file.close();
        }

        // Load user preferences
        file.setFileName(UserConfigFile);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            if (domDocument.setContent(&file,true,&errorStr,&errorLine,&errorColumn)) {
                root = domDocument.documentElement();
                if ((root.tagName()==ConfigFileRootName)&&(root.elementsByTagName("GlobalPreferences").length()>0)&&(root.elementsByTagName("GlobalPreferences").item(0).isElement()==true)) {
                    ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Search Raster mode in configuration file")+GlobalConfigFile);
                    QDomElement Element=root.elementsByTagName("GlobalPreferences").item(0).toElement();
                    if (Element.hasAttribute("RasterMode")) RasterMode=Element.attribute("RasterMode")=="1";
                }
            }
            file.close();
        }

        return RasterMode;
    }
#endif

//====================================================================================================================

QString GetTextSize(qlonglong Size) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:GetTextSize");

    QString UnitStr="";
    int     Unit   =0;

    while ((Size>1024*1024)&&(Unit<2)) {
        Unit++;
        Size=Size/1024;
    }
    switch (Unit) {
        case 0 : UnitStr=QApplication::translate("QCustomFolderTree","Kb","Unit Kb");   break;
        case 1 : UnitStr=QApplication::translate("QCustomFolderTree","Mb","Unit Mb");   break;
        case 2 : UnitStr=QApplication::translate("QCustomFolderTree","Gb","Unit Gb");   break;
        case 3 : UnitStr=QApplication::translate("QCustomFolderTree","Tb","Unit Tb");   break;
    }
    if (Size==0) return "0";
    else if (double(Size)/double(1024)>0.1) return QString("%1").arg(double(Size)/double(1024),8,'f',1).trimmed()+" "+UnitStr;
    else return "<0.1"+UnitStr;
}

//====================================================================================================================

bool CheckFolder(QString FileToTest,QString PathToTest) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:CheckFolder");

    QString Path=QDir(PathToTest).absolutePath();
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
    bool IsFound=QFileInfo(Path+FileToTest).exists();
    if (IsFound) QDir::setCurrent(Path);
    ToLog(LOGMSG_INFORMATION,QString("Try to find datas in %1 %2").arg(Path+FileToTest).arg(IsFound));
    return IsFound;
}


//**************************************************
// First thing to do : ensure correct current path
// At program startup : CurrentPath is set to data folder (we search GlobalConfig file) that could be :
//      in current path
//      or in ../ApplicationGroupName
//      or in ../ApplicationName
//      or in $$PREFIX/share/ApplicationGroupName
//      or in $$PREFIX/share/ApplicationName
//**************************************************

bool SetWorkingPath(char * const argv[],QString ApplicationName,QString ConfigFileExt) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:SetWorkingPath");
    QString StartupDir=QFileInfo(argv[0]).absolutePath();
    ToLog(LOGMSG_INFORMATION,"StartupDir "+StartupDir);
    QDir::setCurrent(StartupDir);

    QString FileToTest  =QString("%1%2").arg(ApplicationName).arg(ConfigFileExt);
    #ifdef Q_OS_WIN
    QString ShareDir="..";
    #else
    QString ShareDir=QString(SHARE_DIR)+QDir().separator()+QString("share");
    #endif

    if (!CheckFolder(FileToTest,QDir::currentPath())
        //&&(!CheckFolder(FileToTest,QString("..")+QDir().separator()+ApplicationGroupName))
        &&(!CheckFolder(FileToTest,QString("..")+QDir().separator()+ApplicationName))
        &&(!CheckFolder(FileToTest,QString("..")+QDir().separator()))
        //&&(!CheckFolder(FileToTest,ShareDir+QDir().separator()+ApplicationGroupName))
        &&(!CheckFolder(FileToTest,ShareDir+QDir().separator()+ApplicationName))
       ) {
        ToLog(LOGMSG_INFORMATION,QString("Critical error : Impossible to find global configuration file (%1%2)").arg(ApplicationName).arg(ConfigFileExt));
        exit(1);
    }
    ToLog(LOGMSG_INFORMATION,"Set working path to "+QDir::currentPath());

    return true;
}

//====================================================================================================================

//functions used to retrieve number of processor
//Thanks to : Stuart Nixon
//See : http://lists.trolltech.com/qt-interest/2006-05/thread00922-0.html
int getCpuCount() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:getCpuCount");
    int cpuCount=1;

#ifdef Q_OS_WIN
    SYSTEM_INFO    si;
    GetSystemInfo(&si);
    cpuCount = si.dwNumberOfProcessors;
#elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    cpuCount = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(Q_OS_MACX)
   kern_return_t            kr;
   struct host_basic_info   hostinfo;
   unsigned int             count;

   count=HOST_BASIC_INFO_COUNT;
   kr   =host_info(mach_host_self(),HOST_BASIC_INFO,(host_info_t)&hostinfo,&count);

   if (kr==KERN_SUCCESS) cpuCount=hostinfo.avail_cpus;
#endif
    if(cpuCount<1) cpuCount=1;
    return cpuCount;
}

//====================================================================================================================

//functions used to adjust folder name depending on operating system
QString AdjustDirForOS(QString Dir) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:AdjustDirForOS");

    #ifdef Q_OS_WIN
    Dir.replace("/","\\");
    bool DoubleSlashBegin=Dir.startsWith("\\\\");
    Dir.replace("\\\\","\\");
    if (DoubleSlashBegin) Dir="\\"+Dir;
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    Dir.replace("\\","/");
    #endif
    return Dir;
}

//**********************************************************************************************************************
// cBaseApplicationConfig
//**********************************************************************************************************************

cBaseApplicationConfig::cBaseApplicationConfig(QMainWindow *TheTopLevelWindow,QString TheAllowedWEBLanguage,QString TheApplicationGroupName,QString TheApplicationName,QString TheApplicationVersion,QString TheConfigFileExt,QString TheConfigFileRootName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseApplicationConfig::cBaseApplicationConfig");

    AllowedWEBLanguage      =TheAllowedWEBLanguage;
    TopLevelWindow          =TheTopLevelWindow;            // Link to MainWindow of the application
    ApplicationGroupName    =TheApplicationGroupName;      // Private folder name to save user configuration file
    ApplicationName         =TheApplicationName;           // Application name
    ApplicationVersion      =TheApplicationVersion;        // Application version
    ConfigFileExt           =TheConfigFileExt;             // File extension of configuration files
    ConfigFileRootName      =TheConfigFileRootName;        // Name of root node in the config xml file

    if (ApplicationVersion.startsWith(ApplicationName)) ApplicationVersion.mid(ApplicationVersion.indexOf(ApplicationName)+ApplicationName.length()+1);
    if (ApplicationGroupName!=ApplicationName) ToLog(LOGMSG_INFORMATION,QString("Starting %1 %2 %3").arg(ApplicationGroupName).arg(ApplicationName).arg(ApplicationVersion));
        else ToLog(LOGMSG_INFORMATION,QString("Starting %1 %2").arg(ApplicationGroupName).arg(ApplicationVersion));
}

//====================================================================================================================

cBaseApplicationConfig::~cBaseApplicationConfig() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseApplicationConfig::~cBaseApplicationConfig");

    if (MainWinWSP) delete MainWinWSP;      MainWinWSP=NULL;
}

//====================================================================================================================

QString cBaseApplicationConfig::GetValideWEBLanguage(QString Language) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseApplicationConfig::GetValideWEBLanguage");

    if (!AllowedWEBLanguage.contains(Language)) Language="en";
    return Language;
}

//====================================================================================================================
// Preload system icon images
void cBaseApplicationConfig::PreloadSystemIcons() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseApplicationConfig::PreloadSystemIcons");

    ToLog(LOGMSG_INFORMATION,"Loading system icons...");
    #ifdef Q_OS_WIN
    if (QSysInfo().WindowsVersion>=0x0090) { // At least Windows 7
        DefaultUSERIcon.LoadIcons(GetIconForFileOrDir("%SystemRoot%\\system32\\imageres.dll",117));
        DefaultCDROMIcon.LoadIcons(GetIconForFileOrDir("%SystemRoot%\\system32\\shell32.dll",11));
        DefaultHDDIcon.LoadIcons(GetIconForFileOrDir("%SystemRoot%\\system32\\shell32.dll",8));
        DefaultFOLDERIcon.LoadIcons(GetIconForFileOrDir("%SystemRoot%\\system32\\shell32.dll",3));
    }
    #endif
    if (DefaultCDROMIcon.Icon16.isNull())   DefaultCDROMIcon.LoadIconsFromIMG(  "cdrom.png");
    if (DefaultHDDIcon.Icon16.isNull())     DefaultHDDIcon.LoadIconsFromIMG(    "hdd.png");
    if (DefaultUSBIcon.Icon16.isNull())     DefaultUSBIcon.LoadIconsFromIMG(    "usb.png");
    if (DefaultREMOTEIcon.Icon16.isNull())  DefaultREMOTEIcon.LoadIconsFromIMG( "hdd-lan.png");
    if (DefaultFOLDERIcon.Icon16.isNull())  DefaultFOLDERIcon.LoadIconsFromIMG( "directory.png");
    if (DefaultDelayedIcon.Icon16.isNull()) DefaultDelayedIcon.LoadIconsFromIMG("delayed.png");
    if (DefaultFFDIcon.Icon16.isNull())     DefaultFFDIcon.LoadIconsFromIMG(    "ffDiaporama.png");
    if (DefaultThumbIcon.Icon16.isNull())   DefaultThumbIcon.LoadIconsFromIMG(  "Thumbnails.png");
    if (DefaultIMAGEIcon.Icon16.isNull())   DefaultIMAGEIcon.LoadIconsFromIMG(  "image.png");
    if (DefaultVIDEOIcon.Icon16.isNull())   DefaultVIDEOIcon.LoadIconsFromIMG(  "video.png");
    if (DefaultMUSICIcon.Icon16.isNull())   DefaultMUSICIcon.LoadIconsFromIMG(  "audio.png");
    if (DefaultUSERIcon.Icon16.isNull())    DefaultUSERIcon.LoadIconsFromIMG(   "folder_home.png");
    if (DefaultFILEIcon.Icon16.isNull())    DefaultFILEIcon.LoadIconsFromIMG(   "file.png");
    VideoMask_120=QImage(":/img/VideoMask_120x180.png");
    VideoMask_150=QImage(":/img/VideoMask_150x200.png");
    VideoMask_162=QImage(":/img/VideoMask_162x216.png");
}

//====================================================================================================================

QString cBaseApplicationConfig::GetFilterForMediaFile(FilterFile type) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseApplicationConfig::GetFilterForMediaFile");

    // enum FilterFile {ALLFILE,IMAGEFILE,VIDEOFILE,MUSICFILE};
    QString ReturnFile="";
    if (type==ALLFILE) {
        ReturnFile=ReturnFile+QApplication::translate("MainWindow","All suported file (");
        for (int i=0;i<AllowImageExtension.count();i++) ReturnFile=ReturnFile+(i>0?" *":"*.")+AllowImageExtension[i];
        for (int i=0;i<AllowVideoExtension.count();i++) ReturnFile=ReturnFile+(i>0?" *":"*.")+AllowVideoExtension[i];
        ReturnFile=ReturnFile+")";
    }
    if ((type==ALLFILE)||(type==IMAGEFILE)) {
        if (ReturnFile!="") ReturnFile=ReturnFile+";;";
        ReturnFile=ReturnFile+QApplication::translate("MainWindow","Image file (");
        for (int i=0;i<AllowImageExtension.count();i++) ReturnFile=ReturnFile+(i>0?" *":"*.")+AllowImageExtension[i];
        ReturnFile=ReturnFile+")";
    }
    if ((type==ALLFILE)||(type==VIDEOFILE)) {
        if (ReturnFile!="") ReturnFile=ReturnFile+";;";
        ReturnFile=ReturnFile+QApplication::translate("MainWindow","Video file (");
        for (int i=0;i<AllowVideoExtension.count();i++) ReturnFile=ReturnFile+(i>0?" *":"*.")+AllowVideoExtension[i];
        ReturnFile=ReturnFile+")";
    }
    if (type==MUSICFILE) {
        if (ReturnFile!="") ReturnFile=ReturnFile+";;";
        ReturnFile=ReturnFile+QApplication::translate("MainWindow","Music file (");
        for (int i=0;i<AllowMusicExtension.count();i++) ReturnFile=ReturnFile+(i>0?" *":"*.")+AllowMusicExtension[i];
        ReturnFile=ReturnFile+")";
    }
    return ReturnFile;
}

//====================================================================================================================

bool cBaseApplicationConfig::InitConfigurationValues(QString ForceLanguage,QApplication *App) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseApplicationConfig::InitConfigurationValues");

    // Initialise all variables and set them default value
    ParentWindow            = NULL;
    StartingPath            = AdjustDirForOS(QDir::currentPath());
    this->ForceLanguage     = ForceLanguage;
    MainWinState            = false;                                                        // WindowsSettings-ismaximized
    RestoreWindow           = true;                                                         // if true then restore windows size and position
    MainWinWSP              = new cSaveWindowPosition("MainWindow",RestoreWindow,true);     // MainWindow - Window size and position
    #ifdef Q_OS_LINUX
        RasterMode          = true;                                                         // Enable or disable raster mode [Linux only]
        CheckConfigAtStartup= true;
    #endif
    #ifdef Q_OS_WIN
        CheckConfigAtStartup= false;
    #endif
    MemCacheMaxValue        = 512*1024*1024;                                                // 512 Mb for image cache
    Crop1088To1080          = true;                                                         // Automaticaly crop video from 1088 lines to 1080 (CANON)
    Deinterlace             = false;
    Smoothing               = true;                                                         // True do smoothing in preview
    RememberLastDirectories =true;
    ShowHiddenFilesAndDir   =false;
    ShowMntDrive            =false;
    ShowFoldersFirst        =true;
    CurrentFilter           =OBJECTTYPE_MANAGED;
    CurrentMode             =DISPLAY_ICON100;
    DisplayFileName         =true;
    MinimumEXIFHeight       =100;
    Image_ThumbWidth        =300;
    Image_ThumbHeight       =200;
    Music_ThumbWidth        =200;
    Music_ThumbHeight       =200;
    Video_ThumbWidth        =162;
    Video_ThumbHeight       =216;

    //*********************************************************************
    // Search plateforme and define specific value depending on plateforme
    //*********************************************************************
    #ifdef Q_OS_WIN
        switch (QSysInfo().WindowsVersion) {
        case 0x0010 : Plateforme="Windows NT (operating system version 4.0)";   break;
        case 0x0020 : Plateforme="Windows 2000 (operating system version 5.0)"; break;
        case 0x0030 : Plateforme="Windows XP (operating system version 5.1)";   break;
        case 0x0040 : Plateforme="Windows Server 2003, Windows Server 2003 R2, Windows Home Server, Windows XP Professional x64 Edition (operating system version 5.2)";    break;
        case 0x0080 : Plateforme="Windows Vista, Windows Server 2008 (operating system version 6.0)";   break;
        case 0x0090 : Plateforme="Windows 7, Windows Server 2008 R2 (operating system version 6.1)";    break;
        default     : Plateforme="Unknown version"; break;
        }

        // Load registry value for specific Windows Folder
        QSettings Settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",QSettings::NativeFormat);
        WINDOWS_APPDATA  =Settings.value("AppData").toString();
        WINDOWS_MUSIC    =Settings.value("My Music").toString();
        WINDOWS_PICTURES =Settings.value("My Pictures").toString();
        WINDOWS_VIDEO    =Settings.value("My Video").toString();
        WINDOWS_DOCUMENTS=Settings.value("Personal").toString();
    #endif
    #ifdef Q_WS_X11
        Plateforme="Unix/Linux";
    #endif

    //*******************************************************
    // Now we can construct Global and User ConfigFile value
    //*******************************************************
    #ifdef Q_OS_WIN
        UserConfigPath=WINDOWS_APPDATA;
        if (UserConfigPath[UserConfigPath.length()-1]!=QDir::separator()) UserConfigPath=UserConfigPath+QDir::separator();
        UserConfigPath  = UserConfigPath+ApplicationGroupName+QDir::separator();
    #else
        UserConfigPath=QDir::homePath();
        if (UserConfigPath[UserConfigPath.length()-1]!=QDir::separator()) UserConfigPath=UserConfigPath+QDir::separator();
        UserConfigPath  = UserConfigPath+"."+ApplicationGroupName+QDir::separator();
    #endif
    GlobalConfigFile=QFileInfo(ApplicationName+ConfigFileExt).absoluteFilePath();
    UserConfigFile  =QFileInfo(UserConfigPath+ApplicationName+ConfigFileExt).absoluteFilePath();

    // Define application name
    if (ApplicationGroupName==ApplicationName) App->setApplicationName(ApplicationName+QString(" ")+ApplicationVersion);
        else App->setApplicationName(ApplicationGroupName+QString(" ")+ApplicationName+QString(" ")+ApplicationVersion);

    //************************************
    // Prepare lists of allowed extension
    //************************************
    int c;
    AllowVideoExtension=AllowVideoExtensions.split("#"); c=AllowVideoExtension.count();  for (int i=0;i<c;i++) AllowVideoExtension.append(AllowVideoExtension[i].toUpper());
    AllowImageExtension=AllowImageExtensions.split("#"); c=AllowImageExtension.count();  for (int i=0;i<c;i++) AllowImageExtension.append(AllowImageExtension[i].toUpper());
    AllowMusicExtension=AllowMusicExtensions.split("#"); c=AllowMusicExtension.count();  for (int i=0;i<c;i++) AllowMusicExtension.append(AllowMusicExtension[i].toUpper());

    //************************************
    // set default values for path
    //************************************
    #ifdef Q_OS_WIN
        LastMediaPath           = WINDOWS_PICTURES;             // Last folder use for image/video
        LastMusicPath           = WINDOWS_MUSIC;                // Last folder use for music
        LastProjectPath         = WINDOWS_DOCUMENTS;            // Last folder use for project
        LastRenderVideoPath     = WINDOWS_VIDEO;                // Last folder use for render video
        LastCaptureImage        = WINDOWS_PICTURES;             // Last folder use for captured image
        CurrentPath             = WINDOWS_DOCUMENTS;
        if (LastRenderVideoPath=="") LastRenderVideoPath=WINDOWS_DOCUMENTS;
    #endif
    #ifdef Q_WS_X11
        LastMediaPath           = QDir::home().absolutePath();  // Last folder use for image/video
        LastMusicPath           = QDir::home().absolutePath();  // Last folder use for music
        LastProjectPath         = QDir::home().absolutePath();  // Last folder use for project
        LastRenderVideoPath     = QDir::home().absolutePath();  // Last folder use for render video
        LastCaptureImage        = QDir::home().absolutePath();  // Last folder use for captured image
        CurrentPath             ="~";   // User home folder
    #endif

    //************************************
    // set language
    //************************************
    // First thing to do is to load ForceLanguage from USERCONFIGFILE and install translator to fix text codec !
    QFile           file(UserConfigFile);
    QDomDocument    domDocument;
    QDomElement     root;
    QString         errorStr;
    int             errorLine,errorColumn;

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&file);
        if (domDocument.setContent(in.readAll(),true,&errorStr,&errorLine,&errorColumn)) {
            domDocument.setContent(domDocument.toString(4),true,&errorStr,&errorLine,&errorColumn);
            if ((root.elementsByTagName("GlobalPreferences").length()>0)&&(root.elementsByTagName("GlobalPreferences").item(0).isElement()==true)) {
                QDomElement Element=root.elementsByTagName("GlobalPreferences").item(0).toElement();
                if ((Element.hasAttribute("ForceLanguage"))&&(ForceLanguage=="")) ForceLanguage=Element.attribute("ForceLanguage");
            }
        }
    }
    // Search system language
    QString CurrentSubLanguage;
    if (ForceLanguage!="") {
        CurrentSubLanguage=ForceLanguage.toLower();
        CurrentLanguage   =ForceLanguage.left(2).toLower();
    } else {
        CurrentLanguage   =QLocale::system().name().left(2).toLower();
        CurrentSubLanguage=QLocale::system().name().toLower();
    }
    // Search if language user sub-language code
    if (QFileInfo(QString("locale")+QDir().separator()+ApplicationName+QString("_")+CurrentSubLanguage+QString(".qm")).exists())
        CurrentLanguage=CurrentSubLanguage;

    // Validate if system locale is supported and if not : force use of "en"
    if ((CurrentLanguage!="en")&&(!QFileInfo(QString("locale")+QDir().separator()+ApplicationName+QString("_")+CurrentLanguage+QString(".qm")).exists())) {
        ToLog(LOGMSG_INFORMATION,QString("Language \"%1\" not found : switch to english").arg(CurrentLanguage));
        CurrentLanguage="en";
    }

    // Install translation (if needed)
    if (CurrentLanguage!="en") {
        // Load translation
        if (!translator.load(QString("locale")+QDir().separator()+ApplicationName+QString("_")+CurrentLanguage+QString(".qm"))) {
            ToLog(LOGMSG_WARNING,QString("Error loading application translation file ... locale")+QDir().separator()+ApplicationName+QString("_")+CurrentLanguage+QString(".qm"));
            exit(1);
        } else ToLog(LOGMSG_INFORMATION,QString("Loading application translation file ... locale")+QDir().separator()+ApplicationName+QString("_")+CurrentLanguage+QString(".qm"));

        // Try to load QT system translation file in current project local folder
        if (QFileInfo(QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm")).exists()) {
            if (!QTtranslator.load(QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm"))) {
                ToLog(LOGMSG_WARNING,QString("Error loading QT system translation file ... locale")+QDir::separator()+QString("qt_")+CurrentLanguage+".qm");
            } else {
                ToLog(LOGMSG_INFORMATION,QString("Loading QT system translation file ... locale")+QDir::separator()+QString("qt_")+CurrentLanguage+".qm");
            }
        } else if (QFileInfo(QString("..")+QDir::separator()+QString("..")+QDir::separator()+QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm")).exists()) {
            // If not then try to load QT system translation file in parrent project local folder
            if (!QTtranslator.load(QString("..")+QDir::separator()+QString("..")+QDir::separator()+QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm"))) {
                ToLog(LOGMSG_WARNING,QString("Error loading QT system translation file ... ..")+QDir::separator()+QString("..")+QDir::separator()+QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm"));
            } else {
                ToLog(LOGMSG_INFORMATION,QString("Loading QT system translation file ... ..")+QDir::separator()+QString("..")+QDir::separator()+QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm"));
            }
        }
        App->installTranslator(&translator);
        App->installTranslator(&QTtranslator);
    }

    // Now translator are loaded, then we can load configuration files

    // Init all others values by call subclassing function
    InitValues();

    LoadConfigurationFile(GLOBALCONFIGFILE,App);                                                 // Get values from global configuration file (overwrite previously initialized values)
    if (!LoadConfigurationFile(USERCONFIGFILE,App)) SaveConfigurationFile();                     // Load values from user configuration file (overwrite previously initialized values)

    return true;
}

//====================================================================================================================

bool cBaseApplicationConfig::LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication */*App*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseApplicationConfig::LoadConfigurationValues"+(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile));

    QFile           file(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile);
    QDomDocument    domDocument;
    QDomElement     root;
    QString         errorStr;
    int             errorLine,errorColumn;
    bool            IsOk=true;

    ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Read configuration file")+" "+(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile));
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        ToLog(LOGMSG_WARNING,QApplication::translate("MainWindow","Error reading configuration file","Error message")+" "+(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile));
        IsOk=false;
    }

    if (IsOk) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        if (!domDocument.setContent(in.readAll(),true,&errorStr,&errorLine,&errorColumn)) {
            ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error reading content of configuration file","Error message")+" "+(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile));
            IsOk=false;
        }

        domDocument.setContent(domDocument.toString(4),true,&errorStr,&errorLine,&errorColumn);
    }

    if (IsOk) {
        root = domDocument.documentElement();
        if (root.tagName()!=ConfigFileRootName) {
            ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","The file is not a valid configuration file","Error message")+" "+(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile));
            IsOk=false;
        }
    }

    if (IsOk) {
        // Load Global preferences
        if ((root.elementsByTagName("GlobalPreferences").length()>0)&&(root.elementsByTagName("GlobalPreferences").item(0).isElement()==true)) {
            QDomElement Element=root.elementsByTagName("GlobalPreferences").item(0).toElement();
            #ifdef Q_OS_LINUX
            if (Element.hasAttribute("RasterMode"))                             RasterMode              =Element.attribute("RasterMode")=="1";
            #endif
            if (Element.hasAttribute("RestoreWindow"))                          RestoreWindow           =Element.attribute("RestoreWindow")=="1";
            if (Element.hasAttribute("Crop1088To1080"))                         Crop1088To1080          =Element.attribute("Crop1088To1080")!="0";
            if (Element.hasAttribute("Deinterlace"))                            Deinterlace             =Element.attribute("Deinterlace")!="0";
            if (Element.hasAttribute("Smoothing"))                              Smoothing               =Element.attribute("Smoothing")=="1";
            if (Element.hasAttribute("CheckConfigAtStartup"))                   CheckConfigAtStartup    =Element.attribute("CheckConfigAtStartup")=="1";
        }

        if ((domDocument.elementsByTagName("LastDirectories").length()>0)&&(domDocument.elementsByTagName("LastDirectories").item(0).isElement()==true)) {
            QDomElement Element=domDocument.elementsByTagName("LastDirectories").item(0).toElement();
            if (Element.hasAttribute("RememberLastDirectories"))    RememberLastDirectories=Element.attribute("RememberLastDirectories")=="1";
            if (Element.hasAttribute("LastMediaPath"))              LastMediaPath          =Element.attribute("LastMediaPath");
            if (Element.hasAttribute("LastMusicPath"))              LastMusicPath          =Element.attribute("LastMusicPath");
        }

        // Load windows size and position
        MainWinWSP->LoadFromXML(root);                                  // MainWindow - Window size and position

        if ((domDocument.elementsByTagName("MMFiler").length()>0)&&(domDocument.elementsByTagName("MMFiler").item(0).isElement()==true)) {
            QDomElement Element=domDocument.elementsByTagName("MMFiler").item(0).toElement();
            if (Element.hasAttribute("ShowHiddenFilesAndDir"))      ShowHiddenFilesAndDir=Element.attribute("ShowHiddenFilesAndDir")=="1";
            if (Element.hasAttribute("ShowMntDrive"))               ShowMntDrive=Element.attribute("ShowMntDrive")=="1";
            if (Element.hasAttribute("ShowFoldersFirst"))           ShowFoldersFirst=Element.attribute("ShowFoldersFirst")=="1";
            if (Element.hasAttribute("CurrentFilter"))              CurrentFilter=Element.attribute("CurrentFilter").toInt();
            if (Element.hasAttribute("CurrentMode"))                CurrentMode=Element.attribute("CurrentMode").toInt();
            if (Element.hasAttribute("DisplayFileName"))            DisplayFileName=Element.attribute("DisplayFileName")=="1";
            if (Element.hasAttribute("MinimumEXIFHeight"))          MinimumEXIFHeight=Element.attribute("MinimumEXIFHeight").toInt();
            if (Element.hasAttribute("Image_ThumbWidth"))           Image_ThumbWidth=Element.attribute("Image_ThumbWidth").toInt();
            if (Element.hasAttribute("Image_ThumbHeight"))          Image_ThumbHeight=Element.attribute("Image_ThumbHeight").toInt();
            if (Element.hasAttribute("Music_ThumbWidth"))           Music_ThumbWidth=Element.attribute("Music_ThumbWidth").toInt();
            if (Element.hasAttribute("Music_ThumbHeight"))          Music_ThumbHeight=Element.attribute("Music_ThumbHeight").toInt();
            if (Element.hasAttribute("Video_ThumbWidth"))           Video_ThumbWidth=Element.attribute("Video_ThumbWidth").toInt();
            if (Element.hasAttribute("Video_ThumbHeight"))          Video_ThumbHeight=Element.attribute("Video_ThumbHeight").toInt();
        }

        if ((domDocument.elementsByTagName("CacheMemory").length()>0)&&(domDocument.elementsByTagName("CacheMemory").item(0).isElement()==true)) {
            QDomElement Element=domDocument.elementsByTagName("CacheMemory").item(0).toElement();
            if (Element.hasAttribute("MemCacheMaxValue"))           MemCacheMaxValue=qlonglong(Element.attribute("MemCacheMaxValue").toInt())*qlonglong(1024*1024);
        }
    }

    return IsOk && LoadValueFromXML(root,TypeConfigFile);
}

//====================================================================================================================

bool cBaseApplicationConfig::SaveConfigurationFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseApplicationConfig::SaveConfigurationValues");

    // Save all option to the configuration file
    QFile           file(UserConfigFile);
    QDomDocument    domDocument(ApplicationName);   domDocument.appendChild(domDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement     root;

    // Ensure destination exist
    QFileInfo       ConfPath(UserConfigFile);
    QDir            ConfDir;
    ConfDir.mkdir(ConfPath.path());

    // Create xml document and root
    root=domDocument.createElement(ConfigFileRootName);
    domDocument.appendChild(root);

    // Save preferences
    QDomElement     Element;
    Element=domDocument.createElement("GlobalPreferences");
    #ifdef Q_OS_LINUX
    Element.setAttribute("RasterMode",              RasterMode?"1":"0");
    #endif
    Element.setAttribute("RestoreWindow",           RestoreWindow?"1":"0");
    Element.setAttribute("ForceLanguage",           ForceLanguage);
    Element.setAttribute("Crop1088To1080",          Crop1088To1080?"1":"0");
    Element.setAttribute("Deinterlace",             Deinterlace?"1":"0");
    Element.setAttribute("Smoothing",               Smoothing?"1":0);
    Element.setAttribute("CheckConfigAtStartup",    CheckConfigAtStartup?"1":"0");
    root.appendChild(Element);

    Element=domDocument.createElement("LastDirectories");
    Element.setAttribute("RememberLastDirectories", RememberLastDirectories?"1":"0");
    Element.setAttribute("LastMediaPath",           LastMediaPath);
    Element.setAttribute("LastMusicPath",           LastMusicPath);
    root.appendChild(Element);

    Element=domDocument.createElement("MMFiler");
    Element.setAttribute("ShowHiddenFilesAndDir",   ShowHiddenFilesAndDir?"1":"0");
    Element.setAttribute("ShowMntDrive",            ShowMntDrive?"1":"0");
    Element.setAttribute("ShowFoldersFirst",        ShowFoldersFirst?"1":"0");
    Element.setAttribute("CurrentFilter",           CurrentFilter);
    Element.setAttribute("CurrentMode",             CurrentMode);
    Element.setAttribute("DisplayFileName",         DisplayFileName?"1":"0");
    Element.setAttribute("MinimumEXIFHeight",       MinimumEXIFHeight);
    Element.setAttribute("Image_ThumbWidth",        Image_ThumbWidth);
    Element.setAttribute("Image_ThumbHeight",       Image_ThumbHeight);
    Element.setAttribute("Music_ThumbWidth",        Music_ThumbWidth);
    Element.setAttribute("Music_ThumbHeight",       Music_ThumbHeight);
    Element.setAttribute("Video_ThumbWidth",        Video_ThumbWidth);
    Element.setAttribute("Video_ThumbHeight",       Video_ThumbHeight);
    root.appendChild(Element);

    Element=domDocument.createElement("CacheMemory");
    Element.setAttribute("MemCacheMaxValue",        MemCacheMaxValue/(1024*1024));
    root.appendChild(Element);

    // Save windows size and position
    MainWinWSP->SaveToXML(root);

    SaveValueToXML(root);

    // Write file to disk
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error creating configuration file","Error message")+" "+UserConfigFile);
        return false;
    }
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    domDocument.save(out,4);
    file.close();
    return true;
}
