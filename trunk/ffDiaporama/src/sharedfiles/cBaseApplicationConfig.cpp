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

// Include some common various class
#include "cBaseApplicationConfig.h"

// Include some additional standard class
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QTranslator>

//#define DEBUGMODE

//====================================================================================================================

#if defined(Q_OS_WIN)
    #include <QPixmapCache>
    #include <windows.h>
    #include <QSettings>
    #ifndef SHGFI_ADDOVERLAYS
        #define SHGFI_ADDOVERLAYS 0x000000020
    #endif

    // qt_fromWinHBITMAP From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QImage qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h) {
        #ifdef DEBUGMODE
        qDebug() << "IN:qt_fromWinHBITMAP";
        #endif
        BITMAPINFO bmi;
        memset(&bmi, 0, sizeof(bmi));
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = w;
        bmi.bmiHeader.biHeight      = -h;
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage   = w * h * 4;

        QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
        if (image.isNull())
            return image;

        // Get bitmap bits
        uchar *data = (uchar *) qMalloc(bmi.bmiHeader.biSizeImage);

        if (GetDIBits(hdc, bitmap, 0, h, data, &bmi, DIB_RGB_COLORS)) {
            // Create image and copy data into image.
            for (int y=0; y<h; ++y) {
                void *dest = (void *) image.scanLine(y);
                void *src = data + y * image.bytesPerLine();
                memcpy(dest, src, image.bytesPerLine());
            }
        } else {
            qDebug()<<"qt_fromWinHBITMAP(), failed to get bitmap bits";
        }
        qFree(data);

        return image;
    }

    //====================================================================================================================
    // convertHIconToPixmap From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QPixmap convertHIconToPixmap( const HICON icon) {
        #ifdef DEBUGMODE
        qDebug() << "IN:convertHIconToPixmap";
        #endif

        bool foundAlpha = false;
        HDC screenDevice = GetDC(0);
        HDC hdc = CreateCompatibleDC(screenDevice);
        ReleaseDC(0, screenDevice);

        ICONINFO iconinfo;
        bool result = GetIconInfo(icon, &iconinfo); //x and y Hotspot describes the icon center
        if (!result)
            qDebug()<<"convertHIconToPixmap(), failed to GetIconInfo()";

        int w = iconinfo.xHotspot * 2;
        int h = iconinfo.yHotspot * 2;

        BITMAPINFOHEADER bitmapInfo;
        bitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
        bitmapInfo.biWidth       = w;
        bitmapInfo.biHeight      = h;
        bitmapInfo.biPlanes      = 1;
        bitmapInfo.biBitCount    = 32;
        bitmapInfo.biCompression = BI_RGB;
        bitmapInfo.biSizeImage   = 0;
        bitmapInfo.biXPelsPerMeter = 0;
        bitmapInfo.biYPelsPerMeter = 0;
        bitmapInfo.biClrUsed       = 0;
        bitmapInfo.biClrImportant  = 0;
        DWORD* bits;

        HBITMAP winBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS, (VOID**)&bits, NULL, 0);
        HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);
        DrawIconEx( hdc, 0, 0, icon, iconinfo.xHotspot * 2, iconinfo.yHotspot * 2, 0, 0, DI_NORMAL);
        QImage image = qt_fromWinHBITMAP(hdc, winBitmap, w, h);

        for (int y = 0 ; y < h && !foundAlpha ; y++) {
            QRgb *scanLine= reinterpret_cast<QRgb *>(image.scanLine(y));
            for (int x = 0; x < w ; x++) {
                if (qAlpha(scanLine[x]) != 0) {
                    foundAlpha = true;
                    break;
                }
            }
        }
        if (!foundAlpha) {
            //If no alpha was found, we use the mask to set alpha values
            DrawIconEx( hdc, 0, 0, icon, w, h, 0, 0, DI_MASK);
            QImage mask = qt_fromWinHBITMAP(hdc, winBitmap, w, h);

            for (int y = 0 ; y < h ; y++){
                QRgb *scanlineImage = reinterpret_cast<QRgb *>(image.scanLine(y));
                QRgb *scanlineMask = mask.isNull() ? 0 : reinterpret_cast<QRgb *>(mask.scanLine(y));
                for (int x = 0; x < w ; x++){
                    if (scanlineMask && qRed(scanlineMask[x]) != 0)
                        scanlineImage[x] = 0; //mask out this pixel
                    else
                        scanlineImage[x] |= 0xff000000; // set the alpha channel to 255
                }
            }
        }
        //dispose resources created by iconinfo call
        DeleteObject(iconinfo.hbmMask);
        DeleteObject(iconinfo.hbmColor);

        SelectObject(hdc, oldhdc); //restore state
        DeleteObject(winBitmap);
        DeleteDC(hdc);
        return QPixmap::fromImage(image);
    }

    //====================================================================================================================
    // GetIconForFileOrDir adapted by domledom From Qmmander Filemanager / Copyright (C) Alex Skoruppa 2009 (See:http://qmmander.googlecode.com/svn-history/r93/trunk/winfileinfo.cpp)
    QIcon GetIconForFileOrDir(QString FileName,int IconIndex) {
        #ifdef DEBUGMODE
        qDebug() << "IN:GetIconForFileOrDir";
        #endif
        QIcon RetIcon;
        WCHAR WinFileName[256+1];

        MultiByteToWideChar(CP_ACP,0,FileName.toLocal8Bit(),-1,WinFileName,256+1);

        if (IconIndex!=0) {
            HICON Icon;
            if (ExtractIconEx(WinFileName,IconIndex,&Icon,NULL,1)>0) {
                RetIcon=convertHIconToPixmap(Icon);
                DeleteObject(Icon);
                return RetIcon;
            } else if (ExtractIconEx(WinFileName,IconIndex,NULL,&Icon,1)>0) {
                RetIcon=convertHIconToPixmap(Icon);
                DeleteObject(Icon);
                return RetIcon;
            }
        }

        SHFILEINFO  info;
        QString     key;
        QPixmap     pixmap;
        QString     fileExtension=QFileInfo(FileName).suffix().toUpper();
        fileExtension.prepend(QLatin1String("."));

        if (QFileInfo(FileName).isFile() && !QFileInfo(FileName).isExecutable() && !QFileInfo(FileName).isSymLink()) key=QLatin1String("qt_")+fileExtension;
        if (!key.isEmpty()) QPixmapCache::find(key, pixmap);

        if (!pixmap.isNull()) {
            RetIcon.addPixmap(pixmap);
            if (QPixmapCache::find(key+QLatin1Char('l'),pixmap)) RetIcon.addPixmap(pixmap);
            return RetIcon;
        }

        //Get the small icon
        if (SHGetFileInfo(WinFileName,0,&info,sizeof(SHFILEINFO),SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS)) {
            if (QFileInfo(FileName).isDir() && !QDir(FileName).isRoot()) {
                //using the unique icon index provided by windows save us from duplicate keys
                key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
                QPixmapCache::find(key,pixmap);
                if (!pixmap.isNull()) {
                    RetIcon.addPixmap(pixmap);
                    if (QPixmapCache::find(key + QLatin1Char('l'), pixmap)) RetIcon.addPixmap(pixmap);
                    DestroyIcon(info.hIcon);
                    return RetIcon;
                }
            }
            if (pixmap.isNull()) {
                pixmap = convertHIconToPixmap(info.hIcon);
                if (!pixmap.isNull()) {
                    RetIcon.addPixmap(pixmap);
                    if (!key.isEmpty()) QPixmapCache::insert(key,pixmap);
                } else {
                    qDebug()<<"QCustomFolderTree::getWinIcon() no small icon found";
                }
            }
            DestroyIcon(info.hIcon);
        }
        //Get the big icon
        if (SHGetFileInfo(WinFileName,0,&info,sizeof(SHFILEINFO),SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS)) {
            if (QFileInfo(FileName).isDir() && !QFileInfo(FileName).isRoot()) {
                //using the unique icon index provided by windows save us from duplicate keys
                key=QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
            }
            pixmap=convertHIconToPixmap(info.hIcon);
            if (!pixmap.isNull()) {
                RetIcon.addPixmap(pixmap);
                if (!key.isEmpty()) QPixmapCache::insert(key+QLatin1Char('l'),pixmap);
            } else {
                qDebug()<<"QCustomFolderTree::getWinIcon() no large icon found";
            }
            DestroyIcon(info.hIcon);
        }
        qDebug()<<"Loading icon nbr"<<IconIndex<<"From "+FileName<<(RetIcon.isNull()?"Error":"Ok");
        return RetIcon;
    }

#endif

//====================================================================================================================

QString GetTextSize(qlonglong Size) {
    #ifdef DEBUGMODE
    qDebug() << "IN:GetTextSize";
    #endif

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
    #ifdef DEBUGMODE
    qDebug() << "IN:CheckFolder";
    #endif
    QString Path=QDir(PathToTest).absolutePath();
    if (!Path.endsWith(QDir::separator())) Path=Path+QDir::separator();
    bool IsFound=QFileInfo(Path+FileToTest).exists();
    if (IsFound) QDir::setCurrent(Path);
    qDebug()<<"Try to find datas in"<<Path+FileToTest<<IsFound;
    return IsFound;
}


//**************************************************
// First thing to do : ensure correct current path
// At program startup : CurrentPath is in exe/binarie folder
//      GlobalConfig must be in current path [for Windows only]
//      or in ../ApplicationGroupName path [Qt Creator or Linux console build mode]
//      or in ../ApplicationName path [Qt Creator only or Linux console build mode]
// it could be
//      or in /usr/share/ApplicationGroupName [for Linux]
//      or in /usr/local/share/ApplicationGroupName [for Linux]
//      or in /opt/share/ApplicationGroupName [for Linux]

//**************************************************
bool SetWorkingPath(char *argv[],QString ApplicationGroupName,QString ApplicationName,QString ConfigFileExt) {
    QString StartupDir=QFileInfo(argv[0]).absolutePath();
    qDebug()<<"StartupDir"<<StartupDir;
    QDir::setCurrent(StartupDir);

    QString FileToTest  =QString("%1%2").arg(ApplicationName).arg(ConfigFileExt);

    if (!CheckFolder(FileToTest,QDir::currentPath())
         &&(!CheckFolder(FileToTest,QString("..")+QDir().separator()+ApplicationGroupName))
         &&(!CheckFolder(FileToTest,QString("../share/")+ApplicationGroupName))
       ) {
        qDebug()<<"Critical error : Impossible to find global configuration file ("<<QString("%1%2").arg(ApplicationName).arg(ConfigFileExt)<<")";
        exit(1);
    }
    qDebug()<<"Set working path to"<<QDir::currentPath();

    return true;
}

//====================================================================================================================

//functions used to retrieve number of processor
//Thanks to : Stuart Nixon
//See : http://lists.trolltech.com/qt-interest/2006-05/thread00922-0.html
int getCpuCount() {
    #ifdef DEBUGMODE
    qDebug() << "IN:getCpuCount";
    #endif
    int cpuCount=1;

#if defined(Q_OS_WIN)
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
    #ifdef DEBUGMODE
    qDebug()<<"IN:AdjustDirForOS";
    #endif
    #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
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

cCustomIcon::cCustomIcon() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::cCustomIcon";
    #endif
}

void cCustomIcon::LoadIcons(QString FileName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::LoadIcons as filename";
    #endif
    QImage IconBIG(FileName);
    qDebug()<<"Loading "+FileName<<(IconBIG.isNull()?"Error":"Ok");
    if (IconBIG.width()>IconBIG.height()) {
        Icon16 =IconBIG.scaledToWidth(16,Qt::SmoothTransformation);
        Icon32 =IconBIG.scaledToWidth(32,Qt::SmoothTransformation);
        Icon48 =IconBIG.scaledToWidth(48,Qt::SmoothTransformation);
        Icon100=IconBIG.scaledToWidth(100,Qt::SmoothTransformation);
    } else {
        Icon16 =IconBIG.scaledToHeight(16,Qt::SmoothTransformation);
        Icon32 =IconBIG.scaledToHeight(32,Qt::SmoothTransformation);
        Icon48 =IconBIG.scaledToHeight(48,Qt::SmoothTransformation);
        Icon100=IconBIG.scaledToHeight(100,Qt::SmoothTransformation);
    }
    if (IconBIG.height()>ICONBIG_MAXHEIGHT) IconBIG=IconBIG.scaledToHeight(ICONBIG_MAXHEIGHT,Qt::SmoothTransformation);
}


//====================================================================================================================

void cCustomIcon::LoadIcons(cCustomIcon *CustomIcon) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::LoadIcons as CustomIcon";
    #endif
    Icon16 =CustomIcon->Icon16.copy();
    Icon32 =CustomIcon->Icon32.copy();
    Icon48 =CustomIcon->Icon48.copy();
    Icon100=CustomIcon->Icon100.copy();
    IconBIG=CustomIcon->IconBIG.copy();
}

//====================================================================================================================

void cCustomIcon::LoadIconsFromIMG(QString FileName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::LoadIconsFromIMG";
    #endif
    Icon16.load("img/MediaIcons/16x16/"+FileName);      qDebug()<<"Loading img/MediaIcons/16x16/"+FileName<<(Icon16.isNull()?"Error":"Ok");
    Icon32.load("img/MediaIcons/32x32/"+FileName);      qDebug()<<"Loading img/MediaIcons/32x32/"+FileName<<(Icon32.isNull()?"Error":"Ok");
    Icon48.load("img/MediaIcons/48x48/"+FileName);      qDebug()<<"Loading img/MediaIcons/48x48/"+FileName<<(Icon48.isNull()?"Error":"Ok");
    Icon100.load("img/MediaIcons/100x100/"+FileName);   qDebug()<<"Loading img/MediaIcons/100x100/"+FileName<<(Icon100.isNull()?"Error":"Ok");
    IconBIG.load("img/MediaIcons/200x200/"+FileName);   qDebug()<<"Loading img/MediaIcons/200x200/"+FileName<<(IconBIG.isNull()?"Error":"Ok");
}

//====================================================================================================================

void cCustomIcon::LoadIconsFromLinux(QString LinuxPath,QString FileName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::LoadIconsFromLinux";
    #endif
    Icon16.load(LinuxPath+"16x16/"+FileName);       qDebug()<<"Loading "+LinuxPath+"16x16/"+FileName<<(Icon16.isNull()?"Error":"Ok");
    Icon32.load(LinuxPath+"32x32/"+FileName);       qDebug()<<"Loading "+LinuxPath+"32x32/"+FileName<<(Icon32.isNull()?"Error":"Ok");
    Icon48.load(LinuxPath+"48x48/"+FileName);       qDebug()<<"Loading "+LinuxPath+"48x48/"+FileName<<(Icon48.isNull()?"Error":"Ok");
    Icon100=QImage(LinuxPath+"128x128/"+FileName);  qDebug()<<"Loading "+LinuxPath+"128x128/"+FileName<<(Icon100.isNull()?"Error":"Ok");
    if (!Icon100.isNull()) IconBIG=Icon100.copy();

    if (!Icon100.isNull()) Icon100=Icon100.scaledToHeight(100,Qt::SmoothTransformation);
    if (!IconBIG.isNull()) IconBIG=IconBIG.scaledToHeight(200,Qt::SmoothTransformation);
}

//====================================================================================================================

void cCustomIcon::LoadIcons(QImage *Image) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::LoadIcons as QImage";
    #endif
    if (Image->width()>Image->height()) {
        Icon16 =Image->scaledToWidth(16,Qt::SmoothTransformation);
        Icon32 =Image->scaledToWidth(32,Qt::SmoothTransformation);
        Icon48 =Image->scaledToWidth(48,Qt::SmoothTransformation);
        Icon100=Image->scaledToWidth(100,Qt::SmoothTransformation);
    } else {
        Icon16 =Image->scaledToHeight(16,Qt::SmoothTransformation);
        Icon32 =Image->scaledToHeight(32,Qt::SmoothTransformation);
        Icon48 =Image->scaledToHeight(48,Qt::SmoothTransformation);
        Icon100=Image->scaledToHeight(100,Qt::SmoothTransformation);
    }
    if (Image->height()<=ICONBIG_MAXHEIGHT) IconBIG=Image->copy(); else IconBIG=Image->scaledToHeight(ICONBIG_MAXHEIGHT);
}

//====================================================================================================================

void cCustomIcon::LoadIcons(QIcon Icon) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::LoadIcons as QIcon";
    #endif
    Icon16 =Icon.pixmap(16,16).toImage();
    Icon32 =Icon.pixmap(32,32).toImage();
    Icon48 =Icon.pixmap(48,48).toImage();
    Icon100=Icon.pixmap(100,100).toImage();
    if ((Icon100.height()<100)&&(Icon100.width()<100)) {
        if (Icon100.height()>Icon100.width()) Icon100=Icon100.scaledToHeight(100,Qt::SmoothTransformation);
            else Icon100=Icon100.scaledToWidth(100,Qt::SmoothTransformation);
    }
    IconBIG=Icon.pixmap(ICONBIG_MAXHEIGHT,ICONBIG_MAXHEIGHT).toImage();
}

//====================================================================================================================

QIcon cCustomIcon::GetIcon() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::GetIcon";
    #endif

    QIcon Ret=QIcon(QPixmap().fromImage(Icon16));
    Ret.addPixmap(QPixmap().fromImage(Icon32));
    Ret.addPixmap(QPixmap().fromImage(Icon48));
    Ret.addPixmap(QPixmap().fromImage(Icon100));
    Ret.addPixmap(QPixmap().fromImage(IconBIG));
    return Ret;
}

//====================================================================================================================

QImage  *cCustomIcon::GetIcon(IconSize Size) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cCustomIcon::GetIcon";
    #endif
    switch (Size) {
        case ICON16:  return &Icon16;
        case ICON32:  return &Icon32;
        case ICON48:  return &Icon48;
        case ICON100: return &Icon100;
        case ICONBIG: return &IconBIG;
        default:      return &Icon16;
    }
}

//**********************************************************************************************************************
// cBaseApplicationConfig
//**********************************************************************************************************************

cBaseApplicationConfig::cBaseApplicationConfig(QMainWindow *TheTopLevelWindow,QString TheAllowedWEBLanguage,QString TheApplicationGroupName,QString TheApplicationName,QString TheApplicationVersion,QString TheConfigFileExt,QString TheConfigFileRootName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseApplicationConfig::cBaseApplicationConfig";
    #endif
    AllowedWEBLanguage      =TheAllowedWEBLanguage;
    TopLevelWindow          =TheTopLevelWindow;            // Link to MainWindow of the application
    ApplicationGroupName    =TheApplicationGroupName;      // Private folder name to save user configuration file
    ApplicationName         =TheApplicationName;           // Application name
    ApplicationVersion      =TheApplicationVersion;        // Application version
    ConfigFileExt           =TheConfigFileExt;             // File extension of configuration files
    ConfigFileRootName      =TheConfigFileRootName;        // Name of root node in the config xml file
    if (ApplicationGroupName!=ApplicationName) qDebug()<<QString("Starting ")+ApplicationGroupName+QString(" ")+ApplicationName+QString(" version ")+ApplicationVersion;
        else qDebug()<<QString("Starting ")+ApplicationGroupName+QString(" version ")+ApplicationVersion;
}

//====================================================================================================================

cBaseApplicationConfig::~cBaseApplicationConfig() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseApplicationConfig::~cBaseApplicationConfig";
    #endif
    if (MainWinWSP) delete MainWinWSP;      MainWinWSP=NULL;
}

//====================================================================================================================

QString cBaseApplicationConfig::GetValideWEBLanguage(QString Language) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseApplicationConfig::GetValideWEBLanguage";
    #endif
    if (!AllowedWEBLanguage.contains(Language)) Language="en";
    return Language;
}

//====================================================================================================================
// Preload system icon images
void cBaseApplicationConfig::PreloadSystemIcons() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseApplicationConfig::PreloadSystemIcons";
    #endif

    qDebug()<<QApplication::translate("MainWindow","Load system icons");

    DefaultCDROMIcon.LoadIconsFromIMG(  "cdrom.png");
    DefaultHDDIcon.LoadIconsFromIMG(    "hdd.png");
    DefaultUSBIcon.LoadIconsFromIMG(    "usb.png");
    DefaultREMOTEIcon.LoadIconsFromIMG( "hdd-lan.png");
    DefaultFOLDERIcon.LoadIconsFromIMG( "directory.png");
    DefaultDelayedIcon.LoadIconsFromIMG("delayed.png");
    DefaultFFDIcon.LoadIconsFromIMG(    "ffDiaporama.png");
    DefaultThumbIcon.LoadIconsFromIMG(  "Thumbnails.png");
    DefaultIMAGEIcon.LoadIconsFromIMG(  "image.png");
    DefaultVIDEOIcon.LoadIconsFromIMG(  "video.png");
    DefaultMUSICIcon.LoadIconsFromIMG(  "audio.png");
    VideoMask=QImage("img/VideoMask.png");
    #if defined(Q_OS_WIN)
        DefaultUSERIcon.LoadIcons(GetIconForFileOrDir(QString(getenv("SystemRoot"))+QString("\\system32\\SHELL32.dll"),126));
        DefaultFILEIcon.LoadIcons(GetIconForFileOrDir(QString(getenv("SystemRoot"))+QString("\\system32\\SHELL32.dll"),-67));
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        #define DEV_USER        "places/user-home.png"
        #define FILE_STD        "mimetypes/unknown.png"
        if (QFileInfo("/usr/share/icons/gnome/16x16/"+QString(DEV_CDROM)).exists()) IconsPath="/usr/share/icons/gnome/";
            else if (QFileInfo("/usr/share/icons/default.kde4/16x16/"+QString(DEV_CDROM)).exists()) IconsPath="/usr/share/icons/default.kde4/";
            else IconsPath="";
        if (IconsPath!="") {
            DefaultUSERIcon.LoadIconsFromLinux(IconsPath,DEV_USER);
            DefaultFILEIcon.LoadIconsFromLinux(IconsPath,FILE_STD);
        }
    #endif
}

//====================================================================================================================

QString cBaseApplicationConfig::GetFilterForMediaFile(FilterFile type) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseApplicationConfig::GetFilterForMediaFile";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseApplicationConfig::InitConfigurationValues";
    #endif

    // Initialise all variables and set them default value
    ParentWindow            = NULL;
    this->ForceLanguage     = ForceLanguage;
    MainWinState            = false;                                                        // WindowsSettings-ismaximized
    RestoreWindow           = true;                                                         // if true then restore windows size and position
    MainWinWSP              = new cSaveWindowPosition("MainWindow",RestoreWindow,true);     // MainWindow - Window size and position
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        RasterMode          = true;                                                         // Enable or disable raster mode [Linux only]
        CheckConfigAtStartup= true;
    #endif
    #ifdef Q_OS_WIN
        CheckConfigAtStartup= false;
    #endif
    Crop1088To1080          = true;                                                         // Automaticaly crop video from 1088 lines to 1080 (CANON)
    ApplyTransfoPreview     = true;                                                         // True if image transformation are apply during preview
    QuickResamplingPreview  = false;                                                        // If true then use quick resampling during preview
    Smoothing               = true;                                                         // True do smoothing in preview

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

    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    if (RasterMode) QApplication::setGraphicsSystem("raster");
    #endif

    // Define application name
    if (ApplicationGroupName==ApplicationName) App->setApplicationName(ApplicationName+QString(" ")+ApplicationVersion);
        else App->setApplicationName(ApplicationGroupName+QString(" ")+ApplicationName+QString(" ")+ApplicationVersion);

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
    AllowVideoExtension.append("flv");     AllowVideoExtension.append("FLV");
    AllowVideoExtension.append("3gp");     AllowVideoExtension.append("3GP");
    AllowVideoExtension.append("ogv");     AllowVideoExtension.append("OGV");
    AllowVideoExtension.append("webm");    AllowVideoExtension.append("WEBM");
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
    // List of all file extension allowed for musique
    AllowMusicExtension.append("wav");     AllowMusicExtension.append("WAV");
    AllowMusicExtension.append("mp3");     AllowMusicExtension.append("MP3");
    AllowMusicExtension.append("mp4");     AllowMusicExtension.append("MP4");
    AllowMusicExtension.append("m4a");     AllowMusicExtension.append("M4A");
    AllowMusicExtension.append("m4b");     AllowMusicExtension.append("M4B");
    AllowMusicExtension.append("m4p");     AllowMusicExtension.append("M4P");
    AllowMusicExtension.append("3g2");     AllowMusicExtension.append("3G2");
    AllowMusicExtension.append("ogg");     AllowMusicExtension.append("OGG");
    AllowMusicExtension.append("oga");     AllowMusicExtension.append("OGA");
    AllowMusicExtension.append("spx");     AllowMusicExtension.append("SPX");
    AllowMusicExtension.append("wma");     AllowMusicExtension.append("WMA");
    AllowMusicExtension.append("flac");    AllowMusicExtension.append("FLAC");

    // set value of external tools path (depending on operating system)
/*
    #ifdef Q_OS_WIN
        PathEXIV2       = AdjustDirForOS(QDir::currentPath()+(QDir::currentPath().endsWith(QDir::separator())?"":QString(QDir::separator()))+"exiv2\\exiv2.exe");           // FileName of exiv2 (with path) : Windows version
        //PathFFMPEG      = AdjustDirForOS(QDir::currentPath()+(QDir::currentPath().endsWith(QDir::separator())?"":QString(QDir::separator()))+"ffmpeg\\bin\\ffmpeg.exe");  // FileName of ffmpeg (with path) : Windows version
        PathFFMPEG      = AdjustDirForOS(QDir::currentPath()+(QDir::currentPath().endsWith(QDir::separator())?"":QString(QDir::separator()))+"ffmpeg.exe");                 // FileName of ffmpeg (with path) : Windows version
    #else
*/
        PathEXIV2       = "exiv2";                       // FileName of exiv2 (with path) : Linux version
        PathFFMPEG      = "ffmpeg";                      // FileName of ffmpeg (with path) : Windows version
//    #endif

    RememberLastDirectories     = true;                     // If true, Remember all directories for future use
    #ifdef Q_OS_WIN
        LastMediaPath           = WINDOWS_PICTURES;             // Last folder use for image/video
        LastMusicPath           = WINDOWS_MUSIC;                // Last folder use for music
    #endif
    #ifdef Q_WS_X11
        LastMediaPath           = QDir::home().absolutePath();  // Last folder use for image/video
        LastMusicPath           = QDir::home().absolutePath();  // Last folder use for music
    #endif

    // Init all others values by call subclassing function
    InitValues();

    LoadConfigurationFile(GLOBALCONFIGFILE,App);                                                 // Get values from global configuration file (overwrite previously initialized values)
    if (!LoadConfigurationFile(USERCONFIGFILE,App)) SaveConfigurationFile();                     // Load values from user configuration file (overwrite previously initialized values)

    return true;
}

//====================================================================================================================

bool cBaseApplicationConfig::LoadConfigurationFile(LoadConfigFileType TypeConfigFile,QApplication *App) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseApplicationConfig::LoadConfigurationValues"<<(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile);
    #endif
    QFile           file(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile);
    QDomDocument    domDocument;
    QDomElement     root;
    QString         errorStr;
    int             errorLine,errorColumn;
    bool            IsOk=true;

    qDebug()<<QApplication::translate("MainWindow","Read configuration file")<<(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug()<<QApplication::translate("MainWindow","Error reading configuration file","Error message")<<(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile);
        IsOk=false;
    }

    if (IsOk && (!domDocument.setContent(&file,true,&errorStr,&errorLine,&errorColumn))) {
        qDebug()<<QApplication::translate("MainWindow","Error reading content of configuration file","Error message")<<(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile);
        IsOk=false;
    }

    if (IsOk) {
        root = domDocument.documentElement();
        if (root.tagName()!=ConfigFileRootName) {
            qDebug()<<QApplication::translate("MainWindow","The file is not a valid configuration file","Error message")<<(TypeConfigFile==USERCONFIGFILE?UserConfigFile:GlobalConfigFile);
            IsOk=false;
        }
    }

    if (IsOk) {
        // Load Global preferences
        if ((root.elementsByTagName("GlobalPreferences").length()>0)&&(root.elementsByTagName("GlobalPreferences").item(0).isElement()==true)) {
            QDomElement Element=root.elementsByTagName("GlobalPreferences").item(0).toElement();
            #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
            if (Element.hasAttribute("RasterMode"))                             RasterMode              =Element.attribute("RasterMode")=="1";
            #endif
            if (Element.hasAttribute("RestoreWindow"))                          RestoreWindow           =Element.attribute("RestoreWindow")=="1";
            if ((Element.hasAttribute("ForceLanguage"))&&(ForceLanguage==""))   ForceLanguage           =Element.attribute("ForceLanguage");
            if (Element.hasAttribute("Crop1088To1080"))                         Crop1088To1080          =Element.attribute("Crop1088To1080")!="0";
            if (Element.hasAttribute("ApplyTransfoPreview"))                    ApplyTransfoPreview     =Element.attribute("ApplyTransfoPreview")=="1";
            if (Element.hasAttribute("QuickResamplingPreview"))                 QuickResamplingPreview  =Element.attribute("QuickResamplingPreview")=="1";
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
    }

    if (TypeConfigFile==USERCONFIGFILE) {
        // Search system language

        CurrentLanguage=QLocale::system().name().left(2);
        if (ForceLanguage!="") CurrentLanguage=ForceLanguage;

        // Validate if system locale is supported and if not force use of "en"
        if ((CurrentLanguage!="en")&&(!QFileInfo(QString("locale")+QDir().separator()+ApplicationName+QString("_")+CurrentLanguage+QString(".qm")).exists())) {
            qDebug()<<QString("Language \"")+CurrentLanguage+QString("\" not found : switch to english");
            CurrentLanguage="en";
        }

        // Install translation (if needed)
        if (CurrentLanguage!="en") {
            // Load translation
            if (!translator.load(QString("locale")+QDir().separator()+ApplicationName+QString("_")+CurrentLanguage+QString(".qm"))) {
                qDebug()<<"Error loading application translation file ..."<<QString("locale")+QDir().separator()+ApplicationName+QString("_")+CurrentLanguage+QString(".qm");
                exit(1);
            } else qDebug()<<"Loading application translation file ..."<<QString("locale")+QDir().separator()+ApplicationName+QString("_")+CurrentLanguage+QString(".qm");

            // Try to load QT system translation file in current project local folder
            if (QFileInfo(QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm")).exists()) {
                if (!QTtranslator.load(QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm"))) {
                    qDebug()<<"Error loading QT system translation file ..."<<QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+".qm";
                } else {
                    qDebug()<<"Loading QT system translation file ..."<<QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+".qm";
                }
            } else if (QFileInfo(QString("..")+QDir::separator()+QString("..")+QDir::separator()+QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm")).exists()) {
                // If not then try to load QT system translation file in parrent project local folder
                if (!QTtranslator.load(QString("..")+QDir::separator()+QString("..")+QDir::separator()+QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm"))) {
                    qDebug()<<"Error loading QT system translation file ..."<<QString("..")+QDir::separator()+QString("..")+QDir::separator()+QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm");
                } else {
                    qDebug()<<"Loading QT system translation file ..."<<QString("..")+QDir::separator()+QString("..")+QDir::separator()+QString("locale")+QDir::separator()+QString("qt_")+CurrentLanguage+QString(".qm");
                }
            }
            App->installTranslator(&translator);
            App->installTranslator(&QTtranslator);
        }

    }

    return IsOk && LoadValueFromXML(root,TypeConfigFile);
}

//====================================================================================================================

bool cBaseApplicationConfig::SaveConfigurationFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseApplicationConfig::SaveConfigurationValues";
    #endif
    // Save all option to the configuration file
    QFile           file(UserConfigFile);
    QDomDocument    domDocument(ApplicationName);
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
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    Element.setAttribute("RasterMode",              RasterMode?"1":"0");
    #endif
    Element.setAttribute("RestoreWindow",           RestoreWindow?"1":"0");
    Element.setAttribute("ForceLanguage",           ForceLanguage);
    Element.setAttribute("Crop1088To1080",          Crop1088To1080?"1":"0");
    Element.setAttribute("ApplyTransfoPreview",     ApplyTransfoPreview?"1":0);
    Element.setAttribute("QuickResamplingPreview",  QuickResamplingPreview?"1":0);
    Element.setAttribute("Smoothing",               Smoothing?"1":0);
    Element.setAttribute("CheckConfigAtStartup",    CheckConfigAtStartup?"1":"0");
    root.appendChild(Element);

    Element=domDocument.createElement("LastDirectories");
    Element.setAttribute("RememberLastDirectories", RememberLastDirectories?"1":"0");
    Element.setAttribute("LastMediaPath",           LastMediaPath);
    Element.setAttribute("LastMusicPath",           LastMusicPath);
    root.appendChild(Element);

    // Save windows size and position
    MainWinWSP->SaveToXML(root);

    SaveValueToXML(root);

    // Write file to disk
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        qDebug()<<QApplication::translate("MainWindow","Error creating configuration file","Error message")<<UserConfigFile;
        return false;
    }
    QTextStream out(&file);
    domDocument.save(out,4);
    file.close();
    return true;
}
