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

//====================================================================================================================
// LibAVFormat/Codec/SWScale inclusion
//====================================================================================================================

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
    #include <libavutil/common.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}
#endif

//====================================================================================================================
// Standard QT inclusion
//====================================================================================================================

#include <QTranslator>
#include <QDir>
#include <QtDebug>
#include <QtGui/QApplication>
#include <QWindowsStyle>

//====================================================================================================================
// This project inclusion
//====================================================================================================================

#include "capplicationconfig.h"
#include "cSoundDefinition.h"
#include "cdiaporama.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication::setGraphicsSystem("raster");
    //QApplication::setStyle(new QWindowsStyle);
    QApplication app(argc, argv);

    app.setApplicationName(APPLICATION_NAME);

    QString CurrentPath=QDir::currentPath();
    if (!CurrentPath.endsWith(QDir::separator())) CurrentPath=CurrentPath+QDir::separator();

    // Ensure correct path
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    qDebug() << "Previous path=" << QDir::currentPath();
    if (!QFileInfo("ffDiaporama.xml").exists()) QDir::setCurrent(QString("../")+QString(APPLICATION_NAME));
    if (!QFileInfo("ffDiaporama.xml").exists()) QDir::setCurrent(QString("/usr/share/")+QString(APPLICATION_NAME));
    qDebug() << "Current path=" << QDir::currentPath();
    #endif

    // Install translation
    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("locale")+QDir::separator()+QString("locale_")+locale.left(2));
    app.installTranslator(&translator);

    // Register all formats and codecs for libavformat/libavcodec/etc ...
    qDebug()<<"Init Libavformat and registered codecs";
    avcodec_init();
    av_register_all();

    // Check codec to know if they was finded
    AVCodec *p=NULL;
    while ((p=av_codec_next(p))) {
        if (p->type==AVMEDIA_TYPE_AUDIO) for (int i=0;i<NBR_AUDIOCODECDEF;i++) if ((p->id==AUDIOCODECDEF[i].Codec_id)&&(p->encode!=NULL)&&(!AUDIOCODECDEF[i].IsFind)) {
            AUDIOCODECDEF[i].IsFind=true;
            strcpy(AUDIOCODECDEF[i].ShortName,p->name);
        }
        if (p->type==AVMEDIA_TYPE_VIDEO) for (int i=0;i<NBR_VIDEOCODECDEF;i++) if ((p->id==VIDEOCODECDEF[i].Codec_id)&&(p->encode!=NULL)&&(!VIDEOCODECDEF[i].IsFind)) {
            VIDEOCODECDEF[i].IsFind=true;
            strcpy(VIDEOCODECDEF[i].ShortName,p->name);
        }
    }

    // Supprime les codecs pas encore programmé !
    VIDEOCODECDEF[0].IsFind=false;  //CODEC_ID_MJPEG        => Ecrit un jpeg au lieu d'une vidéo
    VIDEOCODECDEF[1].IsFind=false;  //CODEC_ID_MPEG2VIDEO   => Erreur en fin de fichier vidéo
    //VIDEOCODECDEF[2].IsFind=false;    //CODEC_ID_MPEG4    => OK
    //VIDEOCODECDEF[3].IsFind=false;    //CODEC_ID_H264     => OK
    VIDEOCODECDEF[4].IsFind=false;  //CODEC_ID_VP8          => pas commencé

    //AUDIOCODECDEF[0].IsFind=false;  //CODEC_ID_PCM_S16LE  => OK
    //AUDIOCODECDEF[1].IsFind=false;  //CODEC_ID_MP3        => OK
    //AUDIOCODECDEF[2].IsFind=false;  //CODEC_ID_AAC        => OK
    AUDIOCODECDEF[3].IsFind=false;  //CODEC_ID_AC3          => Erreur de vitesse !!!!!!!!
    //AUDIOCODECDEF[4].IsFind=false;  //CODEC_ID_VORBIS     => OK
    //AUDIOCODECDEF[5].IsFind=false;  //CODEC_ID_MP2        => OK

    // Check format to know if they was finded
    AVOutputFormat *ofmt=NULL;
    while ((ofmt=av_oformat_next(ofmt))) {
        for (int i=0;i<NBR_FORMATDEF;i++) if (strcmp(ofmt->name,FORMATDEF[i].ShortName)==0) {
            QString     AllowedCodec=FORMATDEF[i].PossibleVideoCodec;
            QString     Codec="";
            int         Index=0;
            bool        IsFindVideoCodec=false;
            bool        IsFindAudioCodec=false;

            while (AllowedCodec.length()>0) {
                Index=AllowedCodec.indexOf("#");
                if (Index>0) {
                    Codec=AllowedCodec.left(Index);
                    AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
                } else {
                    Codec=AllowedCodec;
                    AllowedCodec="";
                }
                // Now find index of this codec in the VIDEOCODECDEF
                Index=0;
                while ((Index<NBR_VIDEOCODECDEF)&&(Codec!=QString(VIDEOCODECDEF[Index].ShortName))) Index++;
                if ((Index<NBR_VIDEOCODECDEF)&&(VIDEOCODECDEF[Index].IsFind)) IsFindVideoCodec=true;
            }
            AllowedCodec=FORMATDEF[i].PossibleAudioCodec;
            Codec="";
            Index=0;
            while (AllowedCodec.length()>0) {
                Index=AllowedCodec.indexOf("#");
                if (Index>0) {
                    Codec=AllowedCodec.left(Index);
                    AllowedCodec=AllowedCodec.right(AllowedCodec.length()-Index-1);
                } else {
                    Codec=AllowedCodec;
                    AllowedCodec="";
                }
                // Now find index of this codec in the AUDIOCODECDEF
                Index=0;
                while ((Index<NBR_AUDIOCODECDEF)&&(Codec!=QString(AUDIOCODECDEF[Index].ShortName))) Index++;
                if ((Index<NBR_AUDIOCODECDEF)&&(AUDIOCODECDEF[Index].IsFind)) IsFindAudioCodec=true;
            }
            FORMATDEF[i].IsFind=IsFindAudioCodec && IsFindVideoCodec;
        }
    }

    // Display finding codecs & formats
    qDebug()<<" Video codecs :";
    for (int i=0;i<NBR_VIDEOCODECDEF;i++) if (VIDEOCODECDEF[i].IsFind) qDebug()<<"  =>"<<VIDEOCODECDEF[i].LongName<<"Codec:"<<VIDEOCODECDEF[i].ShortName;
    qDebug()<<" Audio codecs :";
    for (int i=0;i<NBR_AUDIOCODECDEF;i++) if (AUDIOCODECDEF[i].IsFind) qDebug()<<"  =>"<<AUDIOCODECDEF[i].LongName<<"Codec:"<<AUDIOCODECDEF[i].ShortName;
    qDebug()<<" Container formats :";
    for (int i=0;i<NBR_FORMATDEF;i++)     if (FORMATDEF[i].IsFind) qDebug()<<"  =>"<<FORMATDEF[i].LongName;

    BackgroundList.ScanDisk("background",GEOMETRY_16_9);

    qDebug()<<"Loading transitions and transition icons";
    for (int i=0;i<TRANSITIONMAXSUBTYPE_BASE;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_BASE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_ZOOMINOUT;i++)  IconList.List.append(cIconObject(TRANSITIONFAMILLY_ZOOMINOUT,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_SLIDE;i++)      IconList.List.append(cIconObject(TRANSITIONFAMILLY_SLIDE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_PUSH;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_PUSH,i));
    LumaList_Bar.ScanDisk("luma/Bar",           TRANSITIONFAMILLY_LUMA_BAR);
    LumaList_Box.ScanDisk("luma/Box",           TRANSITIONFAMILLY_LUMA_BOX);
    LumaList_Center.ScanDisk("luma/Center",     TRANSITIONFAMILLY_LUMA_CENTER);
    LumaList_Checker.ScanDisk("luma/Checker",   TRANSITIONFAMILLY_LUMA_CHECKER);
    LumaList_Clock.ScanDisk("luma/Clock",       TRANSITIONFAMILLY_LUMA_CLOCK);
    LumaList_Snake.ScanDisk("luma/Snake",       TRANSITIONFAMILLY_LUMA_SNAKE);
    qDebug()<<"  =>"<<IconList.List.count()<<"transitions loaded";

    MainWindow w(CurrentPath);

    if (w.ApplicationConfig->RestoreWindow && w.ApplicationConfig->MainWinState) w.showMaximized(); else w.show();

    return app.exec();
}
