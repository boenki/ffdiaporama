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

#include "DlgCheckConfig.h"
#include "ui_DlgCheckConfig.h"
#include "../../engine/cBaseMediaFile.h"
#include "../../engine/_Transition.h"
#include "../../engine/cBrushDefinition.h"
#include "../../engine/_SDL_Support.h"

#define ICON_GREEN      ":/img/SmallGreen.png"
#define ICON_RED        ":/img/SmallRed.png"
#define ICON_YELLOW     ":/img/SmallYellow.png"

//====================================================================================================================

DlgCheckConfig::DlgCheckConfig(QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgCheckConfig) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgCheckConfig::DlgCheckConfig");

    ui->setupUi(this);
    OkBt    =ui->OKBT;
    //CancelBt=;
    HelpBt  =ui->HelpBT;
}

//====================================================================================================================

DlgCheckConfig::~DlgCheckConfig() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgCheckConfig::~DlgCheckConfig");

    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgCheckConfig::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgCheckConfig::DoInitDialog");

    QString StatusStr;
    bool    Status;
    int     LumaListNbr=LumaList_Bar.List.count()+LumaList_Box.List.count()+LumaList_Center.List.count()+LumaList_Checker.List.count()+LumaList_Clock.List.count()+LumaList_Snake.List.count();

    // ffDiaporama
    ui->ListWidget->addItem(new QListWidgetItem(QString("ffDiaporama:%1").arg(BaseApplicationConfig->ApplicationVersion)));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Operating system version: %1 - %2 Core/CPU").arg(BaseApplicationConfig->Plateforme).arg(getCpuCount())));
    #if defined(Q_OS_WIN32) || defined(Q_OS_LINUX32)
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Application architecture: 32 bits")));
    #else
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Application architecture: 64 bits")));
    #endif
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","User config file: %1").arg(BaseApplicationConfig->UserConfigFile)));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Global config file: %1").arg(BaseApplicationConfig->GlobalConfigFile)));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Starting path: %1").arg(BaseApplicationConfig->StartingPath)));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Working path set to: %1").arg(AdjustDirForOS(QDir::currentPath()))));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Search path set to: %1").arg(getenv("PATH"))));
    // Ressources libraries
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","The background library contains %1 pictures").arg(BackgroundList.List.count())));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","The non-luma library contains %1 transitions").arg(IconList.List.count()-LumaListNbr)));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","The luma library contains %1 transitions").arg(LumaListNbr)));
    // SDL
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","SDL version:%1.%2.%3-Licence=GPL version 2.1 or later").arg(SDL_MAJOR_VERSION).arg(SDL_MINOR_VERSION).arg(SDL_PATCHLEVEL)));
    // fmt_filters
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","fmt_filters version:0.6.4-Licence=LGPL")));
    // QT
    ui->ListWidget->addItem(new QListWidgetItem(""));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","QT version: %1").arg(qVersion())));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Detected system locale: %1").arg(BaseApplicationConfig->CurrentLanguage)));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Translation file loaded: %1").arg(AdjustDirForOS(QDir().absoluteFilePath(QString("locale")+QDir::separator()+QString("locale_")+BaseApplicationConfig->CurrentLanguage+".qm")))));
    ui->ListWidget->addItem(new QListWidgetItem(""));

    // exiv2
    StatusStr=QApplication::translate("DlgCheckConfig","LibExiv2 version:")+QString("%1.%2.%3").arg(EXIV2_MAJOR_VERSION).arg(EXIV2_MINOR_VERSION).arg(EXIV2_PATCH_VERSION);
    ui->ListWidget->addItem(new QListWidgetItem(StatusStr));
    #ifdef EXIV2WITHPREVIEW
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","LibExiv2 support for preview image")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_YELLOW),StatusStr));
    ui->ListWidget->addItem(new QListWidgetItem(""));

    // taglib
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","TAGLib version:")+QString("%1.%2.%3").arg(TAGLIB_MAJOR_VERSION).arg(TAGLIB_MINOR_VERSION).arg(TAGLIB_PATCH_VERSION)));
    #ifdef TAGLIBWITHFLAC
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","TAGLib support for FLAC")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_YELLOW),StatusStr));
    #ifdef TAGLIBWITHASF
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","TAGLib support for ASF/WMA")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_YELLOW),StatusStr));
    #ifdef TAGLIB_WITH_MP4
    Status=true;
    #else
    Status=false;
    #endif
    StatusStr=QApplication::translate("DlgCheckConfig","TAGLib support for M4A/MP4")+" "+(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"));
    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_YELLOW),StatusStr));
    ui->ListWidget->addItem(new QListWidgetItem(""));

    // QImageBlitz
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","QImageBlitz")));
    BlitzCPUInfo    Info;
    Status=Info.haveExtension(BlitzCPUInfo::MMX);        ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","MMX extension %1").arg(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));
    Status=Info.haveExtension(BlitzCPUInfo::IntegerSSE); ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","IntegerSSE extension %1").arg(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));
    Status=Info.haveExtension(BlitzCPUInfo::SSE);        ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","SSE extension %1").arg(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));
    Status=Info.haveExtension(BlitzCPUInfo::SSE2);       ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","SSE2 extension %1").arg(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));
    Status=Info.haveExtension(BlitzCPUInfo::AMD3DNOW);   ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","AMD3DNOW extension %1").arg(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));
    Status=Info.haveExtension(BlitzCPUInfo::AMD3DNOW2);  ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","AMD3DNOW2 extension %1").arg(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));
    Status=Info.haveExtension(BlitzCPUInfo::AltiVec);    ui->ListWidget->addItem(new QListWidgetItem(Status?QIcon(ICON_GREEN):QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","AltiVec extension %1").arg(Status?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));
    ui->ListWidget->addItem(new QListWidgetItem(""));

    // libav
    ui->ListWidget->addItem(new QListWidgetItem("libav"));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","LIBAV general version:")+
                                            #if defined(LIBAV_08)
                                                "0.8.x"
                                            #elif defined(LIBAV_09)
                                                "9.x"
                                            #else
                                                "error"
                                            #endif
                                                ));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","LIBAVCODEC version:%1.%2.%3.%4") .arg(LIBAVCODEC_VERSION_MAJOR) .arg(LIBAVCODEC_VERSION_MINOR) .arg(LIBAVCODEC_VERSION_MICRO) .arg(avcodec_version())));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","LIBAVFORMAT version:%1.%2.%3.%4").arg(LIBAVFORMAT_VERSION_MAJOR).arg(LIBAVFORMAT_VERSION_MINOR).arg(LIBAVFORMAT_VERSION_MICRO).arg(avformat_version())));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","LIBSWSCALE version:%1.%2.%3.%4") .arg(LIBSWSCALE_VERSION_MAJOR) .arg(LIBSWSCALE_VERSION_MINOR) .arg(LIBSWSCALE_VERSION_MICRO) .arg(swscale_version())));
    ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","LIBAVFILTER version:%1.%2.%3.%4").arg(LIBAVFILTER_VERSION_MAJOR).arg(LIBAVFILTER_VERSION_MINOR).arg(LIBAVFILTER_VERSION_MICRO).arg(avfilter_version())));

    if ((AUDIOCODECDEF[7].IsFind)&&(VIDEOCODECDEF[8].IsFind)&&(FORMATDEF[2].IsFind)) ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_GREEN),QApplication::translate("DlgCheckConfig","Lossless rendering")+" "+QApplication::translate("DlgCheckConfig","available")));
        else ui->ListWidget->addItem(new QListWidgetItem(QIcon(ICON_RED),QApplication::translate("DlgCheckConfig","Lossless rendering")+" "+QApplication::translate("DlgCheckConfig","not available")));

    ui->ListWidget->addItem(new QListWidgetItem(""));

    // Codecs
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","Audio Codecs")));
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(AUDIOCODECDEF[i].IsFind?QIcon(ICON_GREEN):AUDIOCODECDEF[i].IsRead?QIcon(ICON_YELLOW):QIcon(ICON_RED),QString(AUDIOCODECDEF[i].LongName)+" "+(AUDIOCODECDEF[i].IsFind||AUDIOCODECDEF[i].IsRead?QApplication::translate("DlgCheckConfig","available")+QString(" - Codec=%1").arg(QString(AUDIOCODECDEF[i].ShortName)):QApplication::translate("DlgCheckConfig","not available"))));

    ui->ListWidget->addItem(new QListWidgetItem(""));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","Video Codecs")));
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) ui->ListWidget->addItem(new QListWidgetItem(VIDEOCODECDEF[i].IsFind?QIcon(ICON_GREEN):VIDEOCODECDEF[i].IsRead?QIcon(ICON_YELLOW):QIcon(ICON_RED),QString(VIDEOCODECDEF[i].LongName)+" "+(VIDEOCODECDEF[i].IsFind||VIDEOCODECDEF[i].IsRead?QApplication::translate("DlgCheckConfig","available")+QString(" - Codec=%1").arg(QString(VIDEOCODECDEF[i].ShortName)):QApplication::translate("DlgCheckConfig","not available"))));

    ui->ListWidget->addItem(new QListWidgetItem(""));
    ui->ListWidget->addItem(new QListWidgetItem(QApplication::translate("DlgCheckConfig","Container Formats")));
    for (int i=0;i<VFORMAT_NBR;i++) ui->ListWidget->addItem(new QListWidgetItem(FORMATDEF[i].IsFind?QIcon(ICON_GREEN):FORMATDEF[i].IsRead?QIcon(ICON_YELLOW):QIcon(ICON_RED),QString(FORMATDEF[i].LongName)+" "+(FORMATDEF[i].IsFind||FORMATDEF[i].IsRead?QApplication::translate("DlgCheckConfig","available"):QApplication::translate("DlgCheckConfig","not available"))));

    ui->CheckConfigAtStartupCB->setChecked(BaseApplicationConfig->CheckConfigAtStartup);
}

//====================================================================================================================
// Call when user click on Ok button

bool DlgCheckConfig::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgCheckConfig::DoAccept");

    BaseApplicationConfig->CheckConfigAtStartup=ui->CheckConfigAtStartupCB->isChecked();
    return true;
}
