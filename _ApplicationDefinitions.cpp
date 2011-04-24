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

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_ApplicationDefinitions.h"

//====================================================================================================================
#if defined(Q_OS_WIN32)
#   include <windows.h>
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
#   include <unistd.h>
#elif defined(Q_OS_MACX)
#   include <mach/mach.h>
#   include <mach/machine.h>
#endif

/****************************************************************************
  Definition of image format supported by the application
****************************************************************************/

sIMAGEDEF DefImageFormat [2][3][4] = {
    {   // STANDARD_PAL
        {   // GEOMETRY_4_3
            {640,480,1,1,25,"VGA - 640x480 - 25 FPS",0},                        // SIZE_VGA
            {720,576,16,15,25,"DVD - 720x576 - 25 FPS",0},                      // SIZE_DVD
            {960,720,1,1,25,"720p - 960x720 - 25 FPS",0},                       // SIZE_720P
            {1440,1080,1,1,25,"1080p - 1440x1080 - 25 FPS",0}                   // SIZE_1080p
        },{     // GEOMETRY_16_9
            {640,360,1,1,25,"VGA - 640x360 - 25 FPS",0},                        // SIZE_VGA
            {720,576,64,45,25,"DVD - 720x576 WIDE - 25 FPS",0},                 // SIZE_DVD
            {1280,720,1,1,25,"720p - 1280x720 - 25 FPS",0},                     // SIZE_720P
            {1920,1080,1,1,25,"1080p - 1920x1080 - 25 FPS",0}                   // SIZE_1080p
        },{     // GEOMETRY_40_17
            {640,272,1,1,25,"VGA - 640x272 - 25 FPS",0},                        // SIZE_VGA
            {720,436,64,45,25,"DVD - 720x436 WIDE+PAD - 25 FPS",70},            // SIZE_DVD
            {1280,544,1,1,25,"720p - 1280x544 - 25 FPS",0},                     // SIZE_720P
            {1920,816,1,1,25,"1080p - 1920x816 - 25 FPS",0}                     // SIZE_1080p
    }},{// STANDARD_NTSC
        {   // GEOMETRY_4_3
            {640,480,1,1,(double)30000/1001,"VGA - 640x480 - 29.97 FPS",0},             // SIZE_VGA
            {720,480,8,9,(double)30000/1001,"DVD - 720x480 WIDE - 29.97 FPS",0},        // SIZE_DVD
            {960,720,1,1,(double)24000/1001,"720p - 960x720 - 23.976 FPS",0},           // SIZE_720P
            {1440,1080,1,1,(double)24000/1001,"1080p - 1440x1080 - 23.976 FPS",0}       // SIZE_1080p
        },{     // GEOMETRY_16_9
            {640,360,1,1,(double)30000/1001,"VGA - 640x272 - 29.97 FPS",0},             // SIZE_VGA
            {720,480,32,27,(double)30000/1001,"DVD - 720x480 WIDE - 29.97 FPS",0},      // SIZE_DVD
            {1280,720,1,1,(double)24000/1001,"720p - 1280x720 - 23.976 FPS",0},         // SIZE_720P
            {1920,1080,1,1,(double)24000/1001,"1080p - 1920x1080 - 23.976 FPS",0}       // SIZE_1080p
        },{     // GEOMETRY_40_17
            {640,272,1,1,(double)30000/1001,"VGA - 640x272 - 29.97 FPS",0},             // SIZE_VGA
            {720,362,32,27,(double)30000/1001,"DVD - 720x362 WIDE+PAD - 29.97 FPS",59}, // SIZE_DVD
            {1280,544,1,1,(double)24000/1001,"720p - 1280x544 - 23.976 FPS",0},         // SIZE_720P
            {1920,816,1,1,(double)24000/1001,"1080p - 1920x816 - 23.976 FPS",0}         // SIZE_1080p
    }}
};

/****************************************************************************
 audio/video codec and file format definition
****************************************************************************/

struct sVideoCodecDef VIDEOCODECDEF[NBR_VIDEOCODECDEF]={
    {false, CODEC_ID_MJPEG,     "mjpeg",        "Motion JPEG",
     "250k#500k#750k#1000k#1500k#2000k",                                                {"500k","750k","1000k","2000k"}},
    {false, CODEC_ID_MPEG2VIDEO,"mpeg2video",   "MPEG-2 video",
     "2000k#3000k#4000k#6000k#8000k#10000k#12000k#15000k#20000k",                       {"3000k","6000k","12000k","20000k"}},
    {false, CODEC_ID_MPEG4,    "libxvid",       "DivX/XVid/MPEG-4",
     "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k",                {"2000k","3000k","6000k","8000k"}},
    {false, CODEC_ID_H264,     "libx264",       "H.264 AVC/MPEG-4 AVC",
     "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k#10000k#12000k",  {"1500k","2500k","5000k","8000k"}},
    {false, CODEC_ID_VP8,      "libvpx",        "WebM-VP8",
     "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k",                {"1500k","2500k","5000k","8000k"}}
};

struct sAudioCodecDef AUDIOCODECDEF[NBR_AUDIOCODECDEF]={
    {false, CODEC_ID_PCM_S16LE,"pcm_s16le",     "WAV (PCM signed 16-bit little-endian)","",
     false,"",""},
    {false, CODEC_ID_MP3,      "libmp3lame",    "MP3 (MPEG audio layer 3)",             "8k#16k#24k#32k#40k#48k#56k#64k#80k#96k#112k#128k#144k#160k#192k#224k#256k#320k",
     false,"","160k"},
    {false, CODEC_ID_AAC,      "libfaac",       "AAC (Advanced Audio Codec)",           "64k#80k#96k#112k#128k#144k#160k#192k#224k#256k#320k#384k",
     true,"224k#256k#320k#384k#448k#500k#512k#576k#640k","160k"},
    {false, CODEC_ID_AC3,      "ac3",           "AC3 (ATSC A/52A)",                     "64k#80k#96k#112k#128k#144k#160k#192k#224k#256k#320k#384k",
     true,"224k#256k#320k#384k#448k#500k#512k#576k#640k","160k"},
    {false, CODEC_ID_VORBIS,   "libvorbis",     "OGG (Vorbis)",                         "64k#96k#128k#192k#256k#500k",
     false,"","128k"},
    {false, CODEC_ID_MP2,      "mp2",           "MP2 (MPEG audio layer 2)",             "64k#96k#128k#192k#256k#500k",
     false,"","128k"},
};

struct sFormatDef FORMATDEF[NBR_FORMATDEF]={
    {false, "3gp",      "3gp",  "3GP file format",              "libx264#h264",                                "libfaac#aac"},
    {false, "avi",      "avi",  "AVI file format",              "mjpeg#mpeg2video#libxvid#mpeg4#libx264#h264", "pcm_s16le#mp2#libmp3lame#mp3#libfaac#aac#ac3"},
    {false, "matroska", "mkv",  "MKV (Matroska) file format",   "libxvid#mpeg4#libx264#h264",                  "pcm_s16le#libmp3lame#mp3#libfaac#aac#ac3#libvorbis#vorbis"},
    {false, "mjpeg",    "avi",  "MJPEG video",                  "mjpeg",                                       "pcm_s16le"},
    {false, "mp4",      "mp4",  "MP4 file format",              "libxvid#mpeg4#libx264#h264",                  "libmp3lame#mp3#libfaac#aac"},
    {false, "webm",     "webm", "WEBM file format",             "libvpx",                                      "libvorbis#vorbis"}
};
// Note : depending on ffmpeg version :
//       => aac codec is libfaac or aac

/****************************************************************************
  Other
****************************************************************************/
QString CurrentLanguage;                                        // Current language code (en, fr, ...)
QString SystemProperties="";                                    // System properties log

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
    puts(SystemProperties.toLocal8Bit());     // Print out SystemProperties
    exit(1);
}

/****************************************************************************
functions used to retrieve number of processor
Thanks to : Stuart Nixon
See : http://lists.trolltech.com/qt-interest/2006-05/thread00922-0.html
*****************************************************************************/

int getCpuCount() {
    //qDebug() << "IN:getCpuCount";
    int cpuCount=1;

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
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

cSaveWindowPosition::cSaveWindowPosition(QString TheWindowName,bool &TheRestoreWindow,bool TheIsMainWindow) {
    RestoreWindow   =&TheRestoreWindow;
    WindowName      =TheWindowName;
    IsMainWindow    =TheIsMainWindow;
    WinRx           =-1;          // WindowsSettings-x
    WinRy           =-1;          // WindowsSettings-y
    WinRw           =-1;          // WindowsSettings-width
    WinRh           =-1;          // WindowsSettings-height
}

//***********************************************

void cSaveWindowPosition::ApplyToWindow(QWidget *Window) {
    if ((Window==NULL)||(*RestoreWindow==false)) return;

    // Restore window size and position
    if (WinRx>0) {
        Window->setGeometry(WinRx,WinRy,WinRw,WinRh);
        if (IsMainWindow) {
            QByteArray MainWinBA=QByteArray::fromHex(MainWinSS.toUtf8());
            ((QMainWindow *)Window)->restoreState(MainWinBA);
        }
    }
}

//***********************************************

void cSaveWindowPosition::SaveWindowState(QWidget *Window) {
    if ((Window==NULL)||(*RestoreWindow==false)) return;
    // Save window size & position (if needed)
    QRect size=Window->geometry();
    WinRx = size.x();
    WinRy = size.y();
    WinRw = size.width();
    WinRh = size.height();
    if (IsMainWindow) {
        QByteArray MainWinBA=QByteArray(((QMainWindow *)Window)->saveState());
        MainWinSS=QString(MainWinBA.toHex());
    }
}

//***********************************************

void cSaveWindowPosition::SaveToXML(QDomElement &domDocument) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(WindowName);
    Element.setAttribute("Rx",WinRx);
    Element.setAttribute("Ry",WinRy);
    Element.setAttribute("Rw",WinRw);
    Element.setAttribute("Rh",WinRh);
    if (IsMainWindow) Element.setAttribute("saveState",MainWinSS);
    domDocument.appendChild(Element);
}

//***********************************************

void cSaveWindowPosition::LoadFromXML(QDomElement domDocument) {
    if ((domDocument.elementsByTagName(WindowName).length()>0)&&(domDocument.elementsByTagName(WindowName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(WindowName).item(0).toElement();
        WinRx=Element.attribute("Rx","-1").toInt();
        WinRy=Element.attribute("Ry","-1").toInt();
        WinRw=Element.attribute("Rw","-1").toInt();
        WinRh=Element.attribute("Rh","-1").toInt();
        if (IsMainWindow) MainWinSS=Element.attribute("saveState");
    }
}

//====================================================================================================================

cApplicationConfig::cApplicationConfig() {
    //qDebug() << "IN:cApplicationConfig::cApplicationConfig";
    ParentWindow=NULL;

#ifdef Q_OS_WIN
    // Search plateforme and define specific value depending on plateforme
    switch (QSysInfo().WindowsVersion) {
        case 0x0010 : Plateforme="Windows NT (operating system version 4.0)";   break;
        case 0x0020 : Plateforme="Windows 2000 (operating system version 5.0)"; break;
        case 0x0030 : Plateforme="Windows XP (operating system version 5.1)";   break;
        case 0x0040 : Plateforme="Windows Server 2003, Windows Server 2003 R2, Windows Home Server, Windows XP Professional x64 Edition (operating system version 5.2)";    break;
        case 0x0080 : Plateforme="Windows Vista, Windows Server 2008 (operating system version 6.0)";   break;
        case 0x0090 : Plateforme="Windows 7, Windows Server 2008 R2 (operating system version 6.1)";    break;
        default     : Plateforme="Unknown version"; break;
    }

    // AERO Flag for MPlayer
    if (QSysInfo().WindowsVersion>=0x0080) AEROCOMPATIBILITY=true;     // True pour prendre en charge AERO (Vista/Windows 7)
    else                                   AEROCOMPATIBILITY=false;    // False pour ne pas prendre en charge AERO (Vista/Windows 7)

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

    AddToSystemProperties(QString(OPERATINGSYSTEM_STR)+Plateforme+" - "+QString("%1").arg(getCpuCount())+" Core/CPU");

    // Prepare lists of allowed extension
    //************************************
    // List of all file extension allowed for video
    AllowVideoExtension.append("avi");     AllowVideoExtension.append("AVI");
    AllowVideoExtension.append("mov");     AllowVideoExtension.append("MOV");
    AllowVideoExtension.append("mpg");     AllowVideoExtension.append("MPG");
    AllowVideoExtension.append("mpeg");    AllowVideoExtension.append("MPEG");
    AllowVideoExtension.append("m4v");     AllowVideoExtension.append("M4V");
    AllowVideoExtension.append("mkv");     AllowVideoExtension.append("MKV");
    AllowVideoExtension.append("mp4");     AllowVideoExtension.append("MP4");
    // List of all file extension allowed for image
    AllowImageExtension.append("bmp");     AllowImageExtension.append("BMP");
    AllowImageExtension.append("gif");     AllowImageExtension.append("GIF");
    AllowImageExtension.append("jpg");     AllowImageExtension.append("JPG");
    AllowImageExtension.append("jpeg");    AllowImageExtension.append("JPEG");
    AllowImageExtension.append("png");     AllowImageExtension.append("PNG");
    AllowImageExtension.append("pbm");     AllowImageExtension.append("PBM");
    AllowImageExtension.append("pgm");     AllowImageExtension.append("PGM");
    AllowImageExtension.append("ppm");     AllowImageExtension.append("PPM");
    AllowImageExtension.append("tiff");    AllowImageExtension.append("TIFF");
    AllowImageExtension.append("xbm");     AllowImageExtension.append("XBM");
    AllowImageExtension.append("xpm");     AllowImageExtension.append("XPM");

    GlobalConfigFile=QString(APPLICATION_NAME)+QString(CONFIGFILEEXT);

    // set UserConfigFile value (depending on operating system)
#ifdef Q_OS_WIN
    UserConfigFile=WINDOWS_APPDATA;
    if (UserConfigFile[UserConfigFile.length()-1]!=QDir::separator()) UserConfigFile=UserConfigFile+QDir::separator();
    UserConfigFile=UserConfigFile+APPLICATION_NAME+QDir::separator()+APPLICATION_NAME+CONFIGFILEEXT;
    PathEXIV2 = "exiv2\\exiv2.exe";             // FileName of exiv2 (with path) : Windows version
#endif
#ifdef Q_WS_X11
    UserConfigFile=QDir::homePath();
    if (UserConfigFile[UserConfigFile.length()-1]!=QDir::separator()) UserConfigFile=UserConfigFile+QDir::separator();
    UserConfigFile=UserConfigFile+"."+APPLICATION_NAME+QDir::separator()+APPLICATION_NAME+CONFIGFILEEXT;
    PathEXIV2="/usr/bin/exiv2";               // FileName of exiv2 (with path) : Linux version
#endif

    MainWinWSP                  =new cSaveWindowPosition("MainWindow",RestoreWindow,true);                  // MainWindow - Window size and position
    DlgImagePropertiesWSP       =new cSaveWindowPosition("DlgImageProperties",RestoreWindow,false);         // Dialog box "Image properties" - Window size and position
    DlgVideoPropertiesWSP       =new cSaveWindowPosition("DlgVideoProperties",RestoreWindow,false);         // Dialog box "Video properties" - Window size and position
    DlgBackgroundPropertiesWSP  =new cSaveWindowPosition("DlgBackgroundProperties",RestoreWindow,false);    // Dialog box "Background properties" - Window size and position
    DlgMusicPropertiesWSP       =new cSaveWindowPosition("DlgMusicProperties",RestoreWindow,false);         // Dialog box "Music properties" - Window size and position
    DlgProjectSettingsWSP       =new cSaveWindowPosition("DlgProjectSettings",RestoreWindow,false);         // Dialog box "Project settings" - Window size and position
    DlgApplicationSettingsWSP   =new cSaveWindowPosition("DlgApplicationSettings",RestoreWindow,false);     // Dialog box "Application settings" - Window size and position
    DlgRenderVideoWSP           =new cSaveWindowPosition("DlgRenderVideoWSP",RestoreWindow,false);          // Dialog box "Render Video" - Window size and position
    DlgTransitionPropertiesWSP  =new cSaveWindowPosition("DlgTransitionPropertiesWSP",RestoreWindow,false); // Dialog box "Transition properties" - Window size and position
    DisableSSE2                 =0;
 }

//====================================================================================================================
cApplicationConfig::~cApplicationConfig() {
    delete MainWinWSP;
    delete DlgImagePropertiesWSP;
    delete DlgVideoPropertiesWSP;
    delete DlgMusicPropertiesWSP;
    delete DlgBackgroundPropertiesWSP;
    delete DlgProjectSettingsWSP;
    delete DlgApplicationSettingsWSP;
    delete DlgRenderVideoWSP;
    delete DlgTransitionPropertiesWSP;
}

//====================================================================================================================

QString cApplicationConfig::GetFilterForMediaFile(FilterFile type) {
    // enum FilterFile {ALLFILE,IMAGEFILE,VIDEOFILE};
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
    return ReturnFile;
}

//====================================================================================================================

bool cApplicationConfig::InitConfigurationValues() {
    //qDebug() << "IN:cApplicationConfig::InitConfigurationValues";

    CurrentFolder   =QDir::currentPath();

    // Initialise all variables and set them default value
    DisableSSE2                 = 0;
    RememberLastDirectories     = true;                     // If true, Remember all directories for future use
    RestoreWindow               = true;                     // If true, restore window state and position at startup
    AppendObject                = false;                    // If true, new object will be append at the end of the diaporama, if false, new object will be insert after current position
    DefaultFraming              = 2;                        // 0=Width, 1=Height, 2=Full
    MainWinState                = false;                    // Windows State
    TimelineHeight              = 120;                      // Initial height of the timeline
    PreviewFPS                  = 12.5;                     // Preview FrameRate
    NoShotDuration              = 6000;                     // Default duration for fixed image when is alone (no shot)
    FixedDuration               = 3000;                     // Default duration for fixed image (msec)
    MobilDuration               = 3000;                     // Default duration for mobil image (msec)
    SpeedWave                   = SPEEDWAVE_LINEAR;         // Default speed wave methode
    ImageGeometry               = GEOMETRY_16_9;            // Project image geometry for image rendering

    DefaultFormat               = 1;                        // Default format = avi
    DefaultImageSize            = SIZE_720P;                // Default image size
    DefaultStandard             = STANDARD_PAL;             // Default standard (PAL/NTSC)

    RandomTransition            =true;                      // if true randomize a transition
    DefaultTransitionFamilly    =TRANSITIONFAMILLY_BASE;    // Transition familly
    DefaultTransitionSubType    =1;                         // Transition type in the familly
    DefaultTransitionDuration   =1000;                      // Transition duration (in msec)

#ifdef Q_OS_WIN
    LastMediaPath           = WINDOWS_PICTURES;             // Last folder use for image/video
    LastProjectPath         = WINDOWS_DOCUMENTS;            // Last folder use for project
    LastMusicPath           = WINDOWS_MUSIC;                // Last folder use for music
    LastRenderVideoPath     = WINDOWS_VIDEO;                // Last folder use for render video
#endif
#ifdef Q_WS_X11
    LastMediaPath           = QDir::home().absolutePath();  // Last folder use for image/video
    LastProjectPath         = QDir::home().absolutePath();  // Last folder use for project
    LastMusicPath           = QDir::home().absolutePath();  // Last folder use for music
    LastRenderVideoPath     = QDir::home().absolutePath();  // Last folder use for render video
#endif

    return true;
}

//====================================================================================================================

bool cApplicationConfig::LoadConfigurationFile(int TypeConfigFile) {
    //qDebug() << "IN:cApplicationConfig::LoadConfigurationValues";

    //Load all option from configuration file except FFMPEGPart

    QFile           file(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile);
    QDomDocument    domDocument;
    QDomElement     root;
    QString         errorStr;
    int             errorLine,errorColumn;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error reading configuration file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","Error reading configuration file","Error message").toLocal8Bit().constData());
        return false;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn)) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error reading content of configuration file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","Error reading content of configuration file","Error message").toLocal8Bit().constData());
        return false;
    }

    root = domDocument.documentElement();
    if (root.tagName()!=CONFIGFILE_ROOTNAME) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","The file is not a valid configuration file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","The file is not a valid configuration file","Error message").toLocal8Bit().constData());
        return false;
    }

    if (TypeConfigFile!=USERCONFIGFILE) return true;    // It's finish for GlobalConfigFile

    QDomNodeList    NodeList;

    // Load preferences
    if ((root.elementsByTagName("LastDirectories").length()>0)&&(root.elementsByTagName("LastDirectories").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("LastDirectories").item(0).toElement();
        RememberLastDirectories =Element.attribute("RememberLastDirectories")=="1";
        LastMediaPath           =Element.attribute("LastMediaPath");
        LastProjectPath         =Element.attribute("LastProjectPath");
        LastMusicPath           =Element.attribute("LastMusicPath");
        LastRenderVideoPath     =Element.attribute("LastRenderVideoPath");
    }

    if ((root.elementsByTagName("EditorOptions").length()>0)&&(root.elementsByTagName("EditorOptions").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("EditorOptions").item(0).toElement();
        AppendObject             =Element.attribute("AppendObject")=="1";
        TimelineHeight           =Element.attribute("TimelineHeight").toInt();
        DefaultFraming           =Element.attribute("DefaultFraming").toInt();
        PreviewFPS               =Element.attribute("PreviewFPS").toDouble();
        DisableSSE2              =Element.attribute("DisableSSE2")=="1";
        RandomTransition         =Element.attribute("RandomTransition")=="1";
        DefaultTransitionFamilly =Element.attribute("DefaultTransitionFamilly").toInt();
        DefaultTransitionSubType =Element.attribute("DefaultTransitionSubType").toInt();
        DefaultTransitionDuration=Element.attribute("DefaultTransitionDuration").toInt();
    }

    if ((root.elementsByTagName("ProjectDefault").length()>0)&&(root.elementsByTagName("ProjectDefault").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("ProjectDefault").item(0).toElement();
        ImageGeometry           =Element.attribute("ImageGeometry").toInt();
        NoShotDuration          =Element.attribute("NoShotDuration").toInt();
        FixedDuration           =Element.attribute("FixedDuration").toInt();
        MobilDuration           =Element.attribute("MobilDuration").toInt();
        SpeedWave               =Element.attribute("SpeedWave").toInt();
    }

    if ((root.elementsByTagName("RenderDefault").length()>0)&&(root.elementsByTagName("RenderDefault").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("RenderDefault").item(0).toElement();
        DefaultFormat           =Element.attribute("Format").toInt();
        DefaultVideoCodec       =Element.attribute("VideoCodec");
        DefaultVideoBitRate     =Element.attribute("VideoBitRate").toInt();
        DefaultAudioCodec       =Element.attribute("AudioCodec");
        DefaultAudioBitRate     =Element.attribute("AudioBitRate").toInt();
        DefaultImageSize        =Element.attribute("ImageSize").toInt();
        DefaultStandard         =Element.attribute("Standard").toInt();
    }

    NodeList=root.elementsByTagName("RestoreWindow");               if ((NodeList.length()>0)&&(NodeList.at(0).childNodes().length()>0)) RestoreWindow=NodeList.at(0).childNodes().at(0).nodeValue()=="1";
    // Load windows size and position
    MainWinWSP->LoadFromXML(root);                                  // MainWindow - Window size and position
    DlgImagePropertiesWSP->LoadFromXML(root);                       // Dialog box "Image properties" - Window size and position
    DlgVideoPropertiesWSP->LoadFromXML(root);                       // Dialog box "Video properties" - Window size and position
    DlgBackgroundPropertiesWSP->LoadFromXML(root);                  // Dialog box "Background properties" - Window size and position
    DlgMusicPropertiesWSP->LoadFromXML(root);                       // Dialog box "Music properties" - Window size and position
    DlgProjectSettingsWSP->LoadFromXML(root);                       // Dialog box "Project settings" - Window size and position
    DlgApplicationSettingsWSP->LoadFromXML(root);                   // Dialog box "Application settings" - Window size and position
    DlgRenderVideoWSP->LoadFromXML(root);                           // Dialog box "Render video" - Window size and position
    DlgTransitionPropertiesWSP->LoadFromXML(root);                  // Dialog box "Transition properties" - Window size and position

    if (DisableSSE2) qputenv("QT_NO_SSE2","1");
    return true;
}

//====================================================================================================================

bool cApplicationConfig::SaveConfigurationFile() {
    //qDebug() << "IN:cApplicationConfig::SaveConfigurationValues";

    // Save all option to the configuration file
    QFile           file(UserConfigFile);
    QDomDocument    domDocument(APPLICATION_NAME);
    QDomElement     Element;
    QDomElement     root;

    // Ensure destination exist
    QFileInfo       ConfPath(UserConfigFile);
    QDir            ConfDir;
    ConfDir.mkdir(ConfPath.path());

    // Create xml document and root
    root=domDocument.createElement(CONFIGFILE_ROOTNAME);
    domDocument.appendChild(root);

    // Save preferences
    Element=domDocument.createElement("LastDirectories");
    Element.setAttribute("RememberLastDirectories", RememberLastDirectories?"1":"0");
    Element.setAttribute("LastMediaPath",           LastMediaPath);
    Element.setAttribute("LastProjectPath",         LastProjectPath);
    Element.setAttribute("LastMusicPath",           LastMusicPath);
    Element.setAttribute("LastRenderVideoPath",     LastRenderVideoPath);
    root.appendChild(Element);

    Element=domDocument.createElement("EditorOptions");
    Element.setAttribute("AppendObject",             AppendObject?"1":"0");
    Element.setAttribute("TimelineHeight",           (QString("%1").arg(TimelineHeight)));
    Element.setAttribute("DefaultFraming",           (QString("%1").arg(DefaultFraming)));
    Element.setAttribute("DisableSSE2",              DisableSSE2?"1":"0");
    Element.setAttribute("PreviewFPS",               (QString("%1").arg(PreviewFPS,0,'f')));
    Element.setAttribute("RandomTransition",         RandomTransition?"1":"0");
    Element.setAttribute("DefaultTransitionFamilly", (QString("%1").arg(DefaultTransitionFamilly)));
    Element.setAttribute("DefaultTransitionSubType", (QString("%1").arg(DefaultTransitionSubType)));
    Element.setAttribute("DefaultTransitionDuration",(QString("%1").arg(DefaultTransitionDuration)));
    root.appendChild(Element);

    Element=domDocument.createElement("ProjectDefault");
    Element.setAttribute("ImageGeometry",           (QString("%1").arg(ImageGeometry)));
    Element.setAttribute("NoShotDuration",          (QString("%1").arg(NoShotDuration)));
    Element.setAttribute("FixedDuration",           (QString("%1").arg(FixedDuration)));
    Element.setAttribute("MobilDuration",           (QString("%1").arg(MobilDuration)));
    Element.setAttribute("SpeedWave",               (QString("%1").arg(SpeedWave)));
    root.appendChild(Element);

    Element=domDocument.createElement("RenderDefault");
    Element.setAttribute("Format",                  (QString("%1").arg(DefaultFormat)));
    Element.setAttribute("VideoCodec",              DefaultVideoCodec);
    Element.setAttribute("VideoBitRate",            (QString("%1").arg(DefaultVideoBitRate)));
    Element.setAttribute("AudioCodec",              DefaultAudioCodec);
    Element.setAttribute("AudioBitRate",            (QString("%1").arg(DefaultAudioBitRate)));
    Element.setAttribute("Standard",                (QString("%1").arg(DefaultStandard)));
    Element.setAttribute("ImageSize",               (QString("%1").arg(DefaultImageSize)));
    root.appendChild(Element);

    Element=domDocument.createElement("RestoreWindow");             Element.appendChild(domDocument.createTextNode(RestoreWindow?"1":"0"));                             root.appendChild(Element);

    // Save windows size and position
    MainWinWSP->SaveToXML(root);                                // MainWindow - Window size and position
    DlgImagePropertiesWSP->SaveToXML(root);                     // Dialog box "Image properties" - Window size and position
    DlgVideoPropertiesWSP->SaveToXML(root);                     // Dialog box "Video properties" - Window size and position
    DlgBackgroundPropertiesWSP->SaveToXML(root);                // Dialog box "Background properties" - Window size and position
    DlgMusicPropertiesWSP->SaveToXML(root);                     // Dialog box "Music properties" - Window size and position
    DlgProjectSettingsWSP->SaveToXML(root);                     // Dialog box "Project settings" - Window size and position
    DlgApplicationSettingsWSP->SaveToXML(root);                 // Dialog box "Application settings" - Window size and position
    DlgRenderVideoWSP->SaveToXML(root);                         // Dialog box "Render video" - Window size and position
    DlgTransitionPropertiesWSP->SaveToXML(root);                // Dialog box "Transition properties" - Window size and position

    // Write file to disk
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error creating configuration file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","Error creating configuration file","Error message").toLocal8Bit().constData());
        return false;
    }
    QTextStream out(&file);
    domDocument.save(out,4);
    file.close();
    return true;
}
