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

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#include <winbase.h>
#endif

/****************************************************************************
  Definition of image format supported by the application
****************************************************************************/

sIMAGEDEF DefImageFormat [2][3][NBR_SIZEDEF] = {
    {   // STANDARD_PAL
        {   // GEOMETRY_4_3
            {320,240,4,3,25,"25","QVGA - 320x240 - 25 FPS",0},                                                  // SIZE_QVGA
            {426,320,4,3,25,"25","HVGA - 426x320 - 25 FPS",0},                                                  // SIZE_HVGA
            {640,480,4,3,25,"25","VGA - 640x480 - 25 FPS",0},                                                   // SIZE_VGA
            {720,576,16,15,25,"25","SD/DVD - 720x576 - 25 FPS",0},                                              // SIZE_DVD
            {640,480,4,3,25,"25","WVGA - 640x480 - 25 FPS",0},                                                  // SIZE_WVGA
            {1024,768,4,3,25,"25","XGA - 1024x768 - 25 FPS",0},                                                 // SIZE_XGA
            {960,720,4,3,25,"25","720p - 960x720 - 25 FPS",0},                                                  // SIZE_720P
            {1440,1080,4,3,25,"25","1080p - 1440x1080 - 25 FPS",0},                                             // SIZE_1080p
            {240,180,4,3,24,"24","RIM 240 - 240x180 - 24 FPS",0}                                               // SIZE_RIM240
        },{ // GEOMETRY_16_9
            {320,180,16,9,25,"25","QVGA - 320x180 - 25 FPS",0},                                                 // SIZE_QVGA
            {480,270,16,9,25,"25","HVGA - 480x270 - 25 FPS",0},                                                 // SIZE_HVGA
            {640,360,16,9,25,"25","VGA - 640x360 - 25 FPS",0},                                                  // SIZE_VGA
            {720,576,64,45,25,"25","SD/DVD - 720x576 WIDE - 25 FPS",0},                                         // SIZE_DVD
            {800,450,16,9,25,"25","WVGA - 800x450 - 25 FPS",0},                                                 // SIZE_WVGA
            {1024,576,16,9,25,"25","XGA - 1024x576 - 25 FPS",0},                                                // SIZE_XGA
            {1280,720,16,9,25,"25","720p - 1280x720 - 25 FPS",0},                                               // SIZE_720P
            {1920,1080,16,9,25,"25","1080p - 1920x1080 - 25 FPS",0},                                            // SIZE_1080p
            {240,135,16,9,24,"24","RIM 240 - 240x135 - 24 FPS",0}                                               // SIZE_RIM240
        },{ // GEOMETRY_40_17
            {320,136,16,9,25,"25","QVGA - 320x136 - 25 FPS",0},                                                 // SIZE_QVGA
            {480,204,40,17,25,"25","HVGA - 480x204 - 25 FPS",0},                                                // SIZE_HVGA
            {640,272,40,17,25,"25","VGA - 640x272 - 25 FPS",0},                                                 // SIZE_VGA
            {720,436,64,45,25,"25","SD/DVD - 720x436 WIDE+PAD - 25 FPS",70},                                    // SIZE_DVD
            {800,340,40,17,25,"25","WVGA - 800x340 - 25 FPS",0},                                                // SIZE_WVGA
            {1024,436,40,17,25,"25","XGA - 1024x436 - 25 FPS",0},                                               // SIZE_XGA
            {1280,544,40,17,25,"25","720p - 1280x544 - 25 FPS",0},                                              // SIZE_720P
            {1920,816,40,17,25,"25","1080p - 1920x816 - 25 FPS",0},                                             // SIZE_1080p
            {240,135,16,9,24,"24","RIM 240 - 240x135+PAD - 24 FPS",16}                                          // SIZE_RIM240
    }},{// STANDARD_NTSC
        {   // GEOMETRY_4_3
            {320,240,1,1,double(30000)/double(1001),"30000/1001",   "SMALL - 320x240 - 29.97 FPS",0},           // SIZE_QVGA
            {426,320,1,1,double(30000)/double(1001),"30000/1001",   "HVGA - 426x320 - 29.97 FPS",0},            // SIZE_HVGA
            {640,480,1,1,double(30000)/double(1001),"30000/1001",   "VGA - 640x480 - 29.97 FPS",0},             // SIZE_VGA
            {720,480,8,9,double(30000)/double(1001),"30000/1001",   "SD/DVD - 720x480 - 29.97 FPS",0},          // SIZE_DVD
            {640,480,1,1,double(30000)/double(1001),"30000/1001",   "WVGA - 640x480 - 29.97 FPS",0},            // SIZE_WVGA
            {1024,768,1,1,double(30000)/double(1001),"30000/1001",  "XGA - 1024x768 - 29.97 FPS",0},            // SIZE_XGA
            {960,720,1,1,double(24000)/double(1001),"24000/1001",   "720p - 960x720 - 23.976 FPS",0},           // SIZE_720P
            {1440,1080,1,1,double(24000)/double(1001),"24000/1001", "1080p - 1440x1080 - 23.976 FPS",0},        // SIZE_1080p
            {240,180,4,3,24,"24","RIM 240 - 240x180 - 24 FPS",0}                                                // SIZE_RIM240
        },{ // GEOMETRY_16_9
            {320,180,1,1,double(30000)/double(1001),"30000/1001",   "SMALL - 320x180 - 29.97 FPS",0},           // SIZE_QVGA
            {480,270,1,1,double(30000)/double(1001),"30000/1001",   "HVGA - 480x270 - 29.97 FPS",0},            // SIZE_HVGA
            {640,360,1,1,double(30000)/double(1001),"30000/1001",   "VGA - 640x272 - 29.97 FPS",0},             // SIZE_VGA
            {720,480,32,27,double(30000)/double(1001),"30000/1001", "SD/DVD - 720x480 WIDE - 29.97 FPS",0},     // SIZE_DVD
            {800,450,1,1,double(30000)/double(1001),"30000/1001",   "WVGA - 800x450 - 29.97 FPS",0},            // SIZE_WVGA
            {1024,576,1,1,double(30000)/double(1001),"30000/1001",  "XGA - 1024x576 - 29.97 FPS",0},            // SIZE_XGA
            {1280,720,1,1,double(24000)/double(1001),"24000/1001",  "720p - 1280x720 - 23.976 FPS",0},          // SIZE_720P
            {1920,1080,1,1,double(24000)/double(1001),"24000/1001", "1080p - 1920x1080 - 23.976 FPS",0},        // SIZE_1080p
            {240,135,16,9,24,"24","RIM 240 - 240x135 - 24 FPS",0}                                               // SIZE_RIM240
        },{ // GEOMETRY_40_17
            {320,136,1,1,double(30000)/double(1001),"30000/1001",   "SMALL - 320x136 - 29.97 FPS",0},           // SIZE_QVGA
            {480,204,1,1,double(30000)/double(1001),"30000/1001",   "HVGA - 480x204 - 29.97 FPS",0},            // SIZE_HVGA
            {640,272,1,1,double(30000)/double(1001),"30000/1001",   "VGA - 640x272 - 29.97 FPS",0},             // SIZE_VGA
            {720,362,32,27,double(30000)/double(1001),"30000/1001", "SD/DVD - 720x362 WIDE+PAD - 29.97 FPS",59},// SIZE_DVD
            {800,340,1,1,double(30000)/double(1001),"30000/1001",   "WVGA - 800x340 - 29.97 FPS",0},            // SIZE_WVGA
            {1024,436,1,1,double(30000)/double(1001),"30000/1001",  "XGA - 1024x436 - 29.97 FPS",0},            // SIZE_XGA
            {1280,544,1,1,double(24000)/double(1001),"24000/1001",  "720p - 1280x544 - 23.976 FPS",0},          // SIZE_720P
            {1920,816,1,1,double(24000)/double(1001),"24000/1001",  "1080p - 1920x816 - 23.976 FPS",0},         // SIZE_1080p
            {240,135,16,9,24,"24","RIM 240 - 240x135+PAD - 24 FPS",16}                                          // SIZE_RIM240
    }}
};

QString IMAGEDEFGENNAME[2][NBR_SIZEDEF]={
    { // PAL
        "QVGA (25 FPS-4:3=320x240-16:9=320x180-40:17=320x136)",                                                 // SIZE_QVGA
        "HVGA (25 FPS-4:3=426x320-16:9=480x270-40:17=480x204)",                                                 // SIZE_HVGA
        "VGA (25 FPS-4:3=640x480-16:9=460x360-40:17=640x272)",                                                  // SIZE_VGA
        "SD/DVD (25 FPS-4:3=720x576-16:9=720x576W-40:17=720x576WP)",                                            // SIZE_DVD
        "WVGA (25 FPS-4:3=640x480-16:9=800x450-40:17=800x340)",                                                 // SIZE_WVGA
        "XGA (25 FPS-4:3=1024x768-16:9=1024x576-40:17=1024x436)",                                               // SIZE_XGA
        "720p (25 FPS-4:3=960x720-16:9=1280x720-40:17=1280x544)",                                               // SIZE_720P
        "1080p (25 FPS-4:3=1440x1080-16:9=1920x1080-40:17=1920x816)",                                           // SIZE_1080p
        "RIM 240 (24 FPS-4:3=240x180-16:9=240x135-40:17=240x135P)"                                              // SIZE_RIM240
    },{ // NTSC
        "QVGA (29.97 FPS-4:3=320x240-16:9=320x180-40:17=320x136)",                                              // SIZE_QVGA
        "HVGA (29.97 FPS-4:3=426x320-16:9=480x270-40:17=480x204)",                                              // SIZE_HVGA
        "VGA (29.97 FPS-4:3=640x480-16:9=460x360-40:17=640x272)",                                               // SIZE_VGA
        "SD/DVD (29.97 FPS-4:3=720x480-16:9=720x480W-40:17=720x480WP)",                                         // SIZE_DVD
        "WVGA (29.97 FPS-4:3=640x480-16:9=800x450-40:17=800x340)",                                              // SIZE_WVGA
        "XGA (29.97 FPS-4:3=1024x768-16:9=1024x576-40:17=1024x436)",                                            // SIZE_XGA
        "720p (23.976 FPS-4:3=960x720-16:9=1280x720-40:17=1280x544)",                                           // SIZE_720P
        "1080p (23.976 FPS-4:3=1440x1080-16:9=1920x1080-40:17=1920x816)",                                       // SIZE_1080p
        "RIM 240 (24 FPS-4:3=240x180-16:9=240x135-40:17=240x135P)"                                              // SIZE_RIM240
    }
};

int ORDERIMAGENAME[NBR_SIZEDEF]={2,3,4,6,5,7,8,9,1};

/****************************************************************************
 audio/video codec and file format definition
****************************************************************************/

struct sVideoCodecDef VIDEOCODECDEF[NBR_VIDEOCODECDEF]={
    {
        false,CODEC_ID_MJPEG,VCODEC_MJPEG,VCODECST_MJPEG,                                   // IsFind,Codec_id,FFD_VCODEC,FFD_VCODECST
        "mjpeg","Motion JPEG",                                                              // ShortName[50], LongName[200]
        "",                                                                                 // PossibleBitrate
        {""}                                                                                // DefaultBitrate[NBR_SIZEDEF]
    },{
        false,CODEC_ID_MPEG2VIDEO,VCODEC_MPEG,VCODECST_MPEG,                                // IsFind,Codec_id,FFD_VCODEC,FFD_VCODECST
        "mpeg2video","MPEG-2 video",                                                        // ShortName[50], LongName[200]
        "2000k#3000k#4000k#6000k#8000k#10000k#12000k#15000k#20000k#400k",                   // PossibleBitrate
        {                                                                                   // DefaultBitrate[NBR_SIZEDEF]
        "1000k",    // QVGA-320x240
        "2000k",    // HVGA-480x320
        "3000k",    // VGA-640x480
        "6000k",    // DVD-720x576
        "6000k",    // WVGA-800x480
        "10000k",   // XGA-1024x768
        "12000k",   // 720p
        "20000k",   // 1080p
        "400k"      // RIM 240
        }
    },{
        false,CODEC_ID_MPEG4,VCODEC_MPEG4,VCODECST_MPEG4,                                   // IsFind,Codec_id,FFD_VCODEC,FFD_VCODECST
        "libxvid","DivX/XVid/MPEG-4",                                                       // ShortName[50], LongName[200]
        "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k#400k#4500k",            // PossibleBitrate
        {                                                                                   // DefaultBitrate[NBR_SIZEDEF]
        "500k",     // QVGA-320x240
        "1000k",    // HVGA-480x320
        "2000k",    // VGA-640x480
        "3000k",    // DVD-720x576
        "4000k",    // WVGA-800x480
        "5000k",    // XGA-1024x768
        "6000k",    // 720p
        "8000k",    // 1080p
        "400k"      // RIM 240
        }
    },{
        false,CODEC_ID_H264,VCODEC_H264HQ,VCODECST_H264HQ,                                  // IsFind,Codec_id,FFD_VCODEC,FFD_VCODECST
        "libx264","High Quality H.264 AVC/MPEG-4 AVC",                                      // ShortName[50], LongName[200]
        "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k#10000k#12000k#400k#3500k",   // PossibleBitrate
        {                                                                                   // DefaultBitrate[NBR_SIZEDEF]
        "500k",     // QVGA-320x240
        "1000k",    // HVGA-480x320
        "1500k",    // VGA-640x480
        "2500k",    // DVD-720x576
        "3000k",    // WVGA-800x480
        "4000k",    // XGA-1024x768
        "5000k",    // 720p
        "8000k",    // 1080p
        "400k"      // RIM 240
        }
    },{
        false,CODEC_ID_H264,VCODEC_H264PQ,VCODECST_H264PQ,                                  // IsFind,Codec_id,FFD_VCODEC,FFD_VCODECST
        "libx264","Phone Quality H.264 AVC/MPEG-4 AVC",                                     // ShortName[50], LongName[200]
        "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k#10000k#400k#3500k",          // PossibleBitrate
        {                                                                                   // DefaultBitrate[NBR_SIZEDEF]
        "500k",     // QVGA-320x240
        "1200k",    // HVGA-480x320
        "1500k",    // VGA-640x480
        "2500k",    // DVD-720x576
        "3000k",    // WVGA-800x480
        "4000k",    // XGA-1024x768
        "5000k",    // 720p
        "8000k",    // 1080p
        "400k"      // RIM 240
        }
    },{
        false,CODEC_ID_VP8,VCODEC_VP8,VCODECST_VP8,                                         // IsFind,Codec_id,FFD_VCODEC,FFD_VCODECST
        "libvpx","WebM-VP8",                                                                // ShortName[50], LongName[200]
        "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k#400k#3500k",                 // PossibleBitrate
        {                                                                                   // DefaultBitrate[NBR_SIZEDEF]
        "500k",     // QVGA-320x240
        "1000k",    // HVGA-480x320
        "1500k",    // VGA-640x480
        "2500k",    // DVD-720x576
        "3000k",    // WVGA-800x480
        "4000k",    // XGA-1024x768
        "5000k",    // 720p
        "8000k",    // 1080p
        "400k"      // RIM 240
        }
    },{
        false, 22,VCODEC_H263,VCODECST_H263,                                                // IsFind,Codec_id,FFD_VCODEC,FFD_VCODECST
        "flv","Flash Video / Sorenson H.263",                                               // ShortName[50], LongName[200]
        "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k#400k#3500k",                 // PossibleBitrate
        {                                                                                   // DefaultBitrate[NBR_SIZEDEF]
        "500k",     // QVGA-320x240
        "1000k",    // HVGA-480x320
        "1500k",    // VGA-640x480
        "2500k",    // DVD-720x576
        "3000k",    // WVGA-800x480
        "4000k",    // XGA-1024x768
        "5000k",    // 720p
        "8000k",    // 1080p
        "400k"      // RIM 240
        }
    },{
        false, CODEC_ID_THEORA,VCODEC_THEORA,VCODECST_THEORA,                               // IsFind,Codec_id,FFD_VCODEC,FFD_VCODECST
        "libtheora","Theora VP3",                                                           // ShortName[50], LongName[200]
        "500k#1000k#1200k#1500k#2000k#2500k#3000k#4000k#5000k#6000k#8000k#400k#3500k",                 // PossibleBitrate
        // QVGA-320x240/HVGA-480x320/VGA-640x480/DVD-720x576/WVGA-800x480/XGA-1024x768/720p/1080p
        {                                                                                   // DefaultBitrate[NBR_SIZEDEF]
        "500k",     // QVGA-320x240
        "1000k",    // HVGA-480x320
        "1500k",    // VGA-640x480
        "2500k",    // DVD-720x576
        "3000k",    // WVGA-800x480
        "4000k",    // XGA-1024x768
        "5000k",    // 720p
        "8000k",    // 1080p
        "400k"      // RIM 240
        }
    }
};

struct sAudioCodecDef AUDIOCODECDEF[NBR_AUDIOCODECDEF]={
    {false, CODEC_ID_PCM_S16LE,"pcm_s16le",         "WAV (PCM signed 16-bit little-endian)","",     false,"",""},
    {false, CODEC_ID_MP3,      "libmp3lame",        "MP3 (MPEG-1/2 Audio Layer III)",       "8k#16k#24k#32k#40k#48k#56k#64k#80k#96k#112k#128k#144k#160k#192k#224k#256k#320k",     false,"","160k"},
    {false, CODEC_ID_AAC,      "libfaac",           "AAC-LC (Advanced Audio Codec)",        "64k#80k#96k#112k#128k#144k#160k#192k#224k#256k#320k#384k",     true,"224k#256k#320k#384k#448k#500k#512k#576k#640k","160k"},
    {false, CODEC_ID_AC3,      "ac3",               "AC3 (Doly Digital)",                   "64k#80k#96k#112k#128k#144k#160k#192k#224k#256k#320k#384k",     true,"224k#256k#320k#384k#448k#500k#512k#576k#640k","160k"},
    {false, CODEC_ID_VORBIS,   "libvorbis",         "OGG (Vorbis)",                         "64k#96k#128k#192k#256k#500k",     false,"","128k"},
    {false, CODEC_ID_MP2,      "mp2",               "MP2 (MPEG-1 Audio Layer II)",          "64k#96k#128k#192k#256k#500k",     false,"","128k"},
    {false, CODEC_ID_AMR_NB,   "libopencore_amrnb", "AMR (Adaptive Multi-Rate)",            "4750#5150#5900#6700#7400#7950#10200#12200",     false,"","6700"}
};

struct sFormatDef FORMATDEF[NBR_FORMATDEF]={
    {false, "3gp",      "3gp",  "3GP file format",              "MPEG4#H264HQ#H264PQ",              "libopencore_amrnb"},
    {false, "avi",      "avi",  "AVI file format",              "MJPEG#MPEG#MPEG4#H264HQ#H264PQ",   "pcm_s16le#mp2#libmp3lame#mp3#libfaac#aac#ac3"},
    {false, "matroska", "mkv",  "MKV Matroska file format",     "MPEG4#H264HQ#H264PQ#THEORA",       "pcm_s16le#libmp3lame#mp3#libfaac#aac#ac3#libvorbis#vorbis"},
    {false, "mjpeg",    "avi",  "MJPEG video",                  "MJPEG",                            "pcm_s16le"},
    {false, "mp4",      "mp4",  "MP4 file format",              "MPEG4#H264HQ#H264PQ",              "libmp3lame#mp3#libfaac#aac"},
    {false, "mpeg",     "mpg",  "MPEG file format",             "MPEG",                             "mp2#ac3"},
    {false, "webm",     "webm", "WEBM file format",             "VP8",                              "libvorbis#vorbis"},
    {false, "flv",      "flv",  "FLV Flash file format 2005",   "H263",                             "libmp3lame#mp3"},
    {false, "flv",      "flv",  "FLV Flash file format 2008",   "H264HQ#H264PQ",                    "libfaac#aac"},
    {false, "ogg",      "ogv",  "OGV Ogg/Theroa file format",   "THEORA",                           "libvorbis#vorbis"}
};
// Note : depending on ffmpeg version :
//       => aac codec is libfaac or aac

/****************************************************************************
  Other
****************************************************************************/
QString CurrentLanguage;                                        // Current language code (en, fr, ...)
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

QString AdjustDirForOS(QString Dir) {
    #if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    Dir.replace("/","\\");
    Dir.replace("\\\\","\\");
    #endif
    return Dir;
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
    WindowGeo       ="";
    MainWinSS       ="";

}

//***********************************************

void cSaveWindowPosition::ApplyToWindow(QWidget *Window) {
    if ((Window==NULL)||(*RestoreWindow==false)) return;

    // Restore window size and position
    if (WindowGeo!="") {
        QByteArray WinBA=QByteArray::fromHex(WindowGeo.toUtf8());
        Window->restoreGeometry(WinBA);
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
    QByteArray WinBA=QByteArray(Window->saveGeometry());
    WindowGeo=QString(WinBA.toHex());
    if (IsMainWindow) {
        QByteArray MainWinBA=QByteArray(((QMainWindow *)Window)->saveState());
        MainWinSS=QString(MainWinBA.toHex());
    }
}

//***********************************************

void cSaveWindowPosition::SaveToXML(QDomElement &domDocument) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(WindowName);
    Element.setAttribute("saveGeometry",WindowGeo);
    if (IsMainWindow) Element.setAttribute("saveState",MainWinSS);
    domDocument.appendChild(Element);
}

//***********************************************

void cSaveWindowPosition::LoadFromXML(QDomElement domDocument) {
    if ((domDocument.elementsByTagName(WindowName).length()>0)&&(domDocument.elementsByTagName(WindowName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(WindowName).item(0).toElement();
        WindowGeo=Element.attribute("saveGeometry");
        if (IsMainWindow) MainWinSS=Element.attribute("saveState");
    }
}

//====================================================================================================================
// Device model definition
//====================================================================================================================

cDeviceModelDef::cDeviceModelDef(bool IsGlobalConf,int IndexKey) {
    FromGlobalConf  =IsGlobalConf;                          // true if device model is defined in global config file
    FromUserConf    =!IsGlobalConf;                         // true if device model is defined in user config file
    IsFind          =false;                                 // true if device model format is supported by installed version of ffmpeg
    DeviceIndex     =IndexKey;                              // Device number index key
    DeviceName      ="";                                    // long name for the device model
    DeviceType      =0;                                     // device type
    DeviceSubtype   =0;
    FileFormat      =0;                                     // sFormatDef number
    VideoCodec      =0;                                     // sVideoCodecDef number
    AudioCodec      =0;                                     // sAudioCodecDef number
    AudioBitrate    =0;                                     // Bitrate number in sAudioCodecDef
    ImageSize       =0;                                     // DefImageFormat number
    VideoBitrate    =0;                                     // Bitrate number in sVideoCodecDef
    Standard        =0;
}

cDeviceModelDef::~cDeviceModelDef() {

}

void cDeviceModelDef::SaveToXML(QDomElement &domDocument,QString ElementName) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    Element.setAttribute("DeviceIndex",DeviceIndex);
    Element.setAttribute("DeviceName",DeviceName);
    Element.setAttribute("DeviceType",DeviceType);
    Element.setAttribute("DeviceSubtype",DeviceSubtype);
    Element.setAttribute("FileFormat",FileFormat);
    Element.setAttribute("VideoCodec",VideoCodec);
    Element.setAttribute("AudioCodec",AudioCodec);
    Element.setAttribute("AudioBitrate",AudioBitrate);
    Element.setAttribute("Standard",Standard);
    Element.setAttribute("ImageSize",ImageSize);
    Element.setAttribute("VideoBitrate",VideoBitrate);
    domDocument.appendChild(Element);
}

bool cDeviceModelDef::LoadFromXML(QDomElement domDocument,QString ElementName,bool IsUserConfigFile) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        if (IsUserConfigFile) FromUserConf=true;
        DeviceName      =Element.attribute("DeviceName");
        DeviceType      =Element.attribute("DeviceType").toInt();
        DeviceSubtype   =Element.attribute("DeviceSubtype").toInt();
        FileFormat      =Element.attribute("FileFormat").toInt();
        VideoCodec      =Element.attribute("VideoCodec").toInt();
        AudioCodec      =Element.attribute("AudioCodec").toInt();
        AudioBitrate    =Element.attribute("AudioBitrate").toInt();
        Standard        =Element.attribute("Standard").toInt();
        ImageSize       =Element.attribute("ImageSize").toInt();
        VideoBitrate    =Element.attribute("VideoBitrate").toInt();

        if (FromUserConf==false) {
            BckDeviceName   =DeviceName;                                      // long name for the device model
            BckDeviceType   =DeviceType;                                      // device type
            BckDeviceSubtype=DeviceSubtype;                                   // device Subtype
            BckStandard     =Standard;                                        // standard : PAL/NTSC
            BckFileFormat   =FileFormat;                                      // sFormatDef number
            BckImageSize    =ImageSize;                                       // DefImageFormat number
            BckVideoCodec   =VideoCodec;                                      // sVideoCodecDef number
            BckVideoBitrate =VideoBitrate;                                    // Bitrate number in sVideoCodecDef
            BckAudioCodec   =AudioCodec;                                      // sAudioCodecDef number
            BckAudioBitrate =AudioBitrate;                                    // Bitrate number in sAudioCodecDef
        }
        return true;
    } else return false;
}

//====================================================================================================================

cApplicationConfig::cApplicationConfig() {

    //qDebug() << "IN:cApplicationConfig::cApplicationConfig";
    ParentWindow=NULL;

    // Init collections
    StyleTextCollection.CollectionName          =QString(STYLENAME_TEXTSTYLE);
    StyleTextBackgroundCollection.CollectionName=QString(STYLENAME_BACKGROUNDSTYLE);
    StyleCoordinateCollection.CollectionName    =QString(STYLENAME_COORDINATESTYLE);
    StyleBlockShapeCollection.CollectionName    =QString(STYLENAME_BLOCKSHAPESTYLE);
    StyleImageFramingCollection.CollectionName  =QString(STYLENAME_FRAMINGSTYLE);

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
    AllowVideoExtension.append("flv");     AllowVideoExtension.append("FLV");
    AllowVideoExtension.append("3gp");     AllowVideoExtension.append("3GP");
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
    AllowMusicExtension.append("mpa");     AllowMusicExtension.append("MPA");
    AllowMusicExtension.append("ogg");     AllowMusicExtension.append("OGG");

    GlobalConfigFile=QString(APPLICATION_NAME)+QString(CONFIGFILEEXT);

    // set UserConfigFile value (depending on operating system)
    #ifdef Q_OS_WIN
        UserConfigPath=WINDOWS_APPDATA;
        if (UserConfigPath[UserConfigPath.length()-1]!=QDir::separator()) UserConfigPath=UserConfigPath+QDir::separator();
        UserConfigPath  = UserConfigPath+APPLICATION_NAME+QDir::separator();
        PathEXIV2       = AdjustDirForOS(QDir::currentPath()+(QDir::currentPath().endsWith(QDir::separator())?"":QString(QDir::separator()))+"exiv2\\exiv2.exe");           // FileName of exiv2 (with path) : Windows version
        PathFFMPEG      = AdjustDirForOS(QDir::currentPath()+(QDir::currentPath().endsWith(QDir::separator())?"":QString(QDir::separator()))+"ffmpeg\\bin\\ffmpeg.exe");    // FileName of ffmpeg (with path) : Windows version
    #else
        UserConfigPath=QDir::homePath();
        if (UserConfigPath[UserConfigPath.length()-1]!=QDir::separator()) UserConfigPath=UserConfigPath+QDir::separator();
        UserConfigPath  = UserConfigPath+"."+APPLICATION_NAME+QDir::separator();
        PathEXIV2       = "exiv2";                       // FileName of exiv2 (with path) : Linux version
        PathFFMPEG      = "ffmpeg";                      // FileName of ffmpeg (with path) : Windows version
    #endif
    UserConfigFile=UserConfigPath+APPLICATION_NAME+CONFIGFILEEXT;
    if (UserConfigPath.endsWith("/")||UserConfigPath.endsWith("\\")) UserConfigPath=UserConfigPath.left(UserConfigPath.length()-QString("/").length());
    UserConfigPath=AdjustDirForOS(UserConfigPath);
    AddToSystemProperties(QString(TEMPDIR_STR)+UserConfigPath);
    AddToSystemProperties(QString(USERCONFIGFILE_STR)+UserConfigFile);

    MainWinWSP                  =new cSaveWindowPosition("MainWindow",RestoreWindow,true);                  // MainWindow - Window size and position
    DlgBackgroundPropertiesWSP  =new cSaveWindowPosition("DlgBackgroundProperties",RestoreWindow,false);    // Dialog box "Background properties" - Window size and position
    DlgMusicPropertiesWSP       =new cSaveWindowPosition("DlgMusicProperties",RestoreWindow,false);         // Dialog box "Music properties" - Window size and position
    DlgApplicationSettingsWSP   =new cSaveWindowPosition("DlgApplicationSettings",RestoreWindow,false);     // Dialog box "Application settings" - Window size and position
    DlgRenderVideoWSP           =new cSaveWindowPosition("DlgRenderVideoWSP",RestoreWindow,false);          // Dialog box "Render Video" - Window size and position
    DlgTransitionPropertiesWSP  =new cSaveWindowPosition("DlgTransitionPropertiesWSP",RestoreWindow,false); // Dialog box "Transition properties" - Window size and position
    DlgSlidePropertiesWSP       =new cSaveWindowPosition("DlgSlidePropertiesWSP",RestoreWindow,false);      // Dialog box "Slide properties" - Window size and position
    DlgImageTransformationWSP   =new cSaveWindowPosition("DlgImageTransformationWSP",RestoreWindow,false);  // Dialog box "Image transformation" - Window size and position
    DlgImageCorrectionWSP       =new cSaveWindowPosition("DlgImageCorrectionWSP",RestoreWindow,false);      // Dialog box "Image correction" - Window size and position
    DlgVideoEditWSP             =new cSaveWindowPosition("DlgVideoEditWSP",RestoreWindow,false);            // Dialog box "Edit video" - Window size and position
    DlgTextEditWSP              =new cSaveWindowPosition("DlgTextEditWSP",RestoreWindow,false);             // Dialog box "Text editor" - Window size and position
    DlgManageStyleWSP           =new cSaveWindowPosition("DlgManageStyleWSP",RestoreWindow,false);          // Dialog box "Manage style" - Window size and position
    DlgCheckConfigWSP           =new cSaveWindowPosition("DlgCheckConfigWSP",RestoreWindow,false);          // Dialog box "Check configuration" - Window size and position
}

//====================================================================================================================
cApplicationConfig::~cApplicationConfig() {
    delete MainWinWSP;
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
}

//====================================================================================================================

QString cApplicationConfig::GetFilterForMediaFile(FilterFile type) {
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

bool cApplicationConfig::InitConfigurationValues() {
    //qDebug() << "IN:cApplicationConfig::InitConfigurationValues";

    CurrentFolder   =QDir::currentPath();

    // Initialise all variables and set them default value
    RasterMode                  = true;                     // Enable or disable raster mode [Linux only]
    NbrSlideInCache             = 5;                        // Number of slide in cache (Real compute is NbrSlideInCache*2+1)
    MemCacheMaxValue            = 256*1024*1024;            // 256 Mb for image cache
    RememberLastDirectories     = true;                     // If true, Remember all directories for future use
    RestoreWindow               = true;                     // If true, restore window state and position at startup
    AskUserToRemove             = true;                     // If true, user must answer to a confirmation dialog box to remove slide
    SortFile                    = true;                     // if true sort file by (last) number when multiple file insertion
    AppendObject                = false;                    // If true, new object will be append at the end of the diaporama, if false, new object will be insert after current position
    PartitionMode               = false;                    // If true, partition mode is on
    DisplayUnit                 = 1;                        // Display coordinates unit
    DefaultFraming              = 2;                        // 0=Width, 1=Height, 2=Full
    MainWinState                = false;                    // Windows State
    TimelineHeight              = 120;                      // Initial height of the timeline
    PreviewFPS                  = 12.5;                     // Preview FrameRate
    ApplyTransfoPreview         = true;                     // True if image transformation are apply during preview
    QuickResamplingPreview      = false;                    // If true then use quick resampling during preview
    Smoothing                   = true;                     // True do smoothing in preview
    NoShotDuration              = 6000;                     // Default duration for fixed image when is alone (no shot)
    FixedDuration               = 3000;                     // Default duration for fixed image (msec)
    SpeedWave                   = SPEEDWAVE_LINEAR;         // Default speed wave methode
    ImageGeometry               = GEOMETRY_16_9;            // Project image geometry for image rendering
    SlideRuler                  = true;                     // if true, ruler is on in slide properties dialog box
    FramingRuler                = true;                     // if true, ruler is on in framing/correction dialog box
    Crop1088To1080              = true;                     // Automaticaly crop video from 1088 lines to 1080 (CANON)

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
    LastMediaPath           = WINDOWS_PICTURES;             // Last folder use for image/video
    LastProjectPath         = WINDOWS_DOCUMENTS;            // Last folder use for project
    LastMusicPath           = WINDOWS_MUSIC;                // Last folder use for music
    LastRenderVideoPath     = WINDOWS_VIDEO;                // Last folder use for render video
    if (LastRenderVideoPath=="") LastRenderVideoPath=WINDOWS_DOCUMENTS;
    SDLAudioOldMode         = false;                        // If true SDL audio use old mode sample instead byte
    CheckConfigAtStartup    = false;
#endif
#ifdef Q_WS_X11
    LastMediaPath           = QDir::home().absolutePath();  // Last folder use for image/video
    LastProjectPath         = QDir::home().absolutePath();  // Last folder use for project
    LastMusicPath           = QDir::home().absolutePath();  // Last folder use for music
    LastRenderVideoPath     = QDir::home().absolutePath();  // Last folder use for render video
    SDLAudioOldMode         = true;                         // If true SDL audio use old mode sample instead byte
    CheckConfigAtStartup    = true;
#endif

    TranslatedRenderType.append(QApplication::translate("DlgRenderVideo","Advanced","Device database type"));           // EXPORTMODE_ADVANCED
    TranslatedRenderType.append(QApplication::translate("DlgRenderVideo","Smartphone","Device database type"));         // EXPORTMODE_SMARTPHONE
    TranslatedRenderType.append(QApplication::translate("DlgRenderVideo","Multimedia system","Device database type"));  // EXPORTMODE_MULTIMEDIASYS
    TranslatedRenderType.append(QApplication::translate("DlgRenderVideo","For the WEB","Device database type"));        // EXPORTMODE_FORTHEWEB

    TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Smartphone","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Portable Player","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Netbook/NetPC","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Handheld game console","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_SMARTPHONE].append(QApplication::translate("DlgRenderVideo","Tablet computer","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].append(QApplication::translate("DlgRenderVideo","Multimedia hard drive and gateway","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].append(QApplication::translate("DlgRenderVideo","Player","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].append(QApplication::translate("DlgRenderVideo","ADSL Box","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_MULTIMEDIASYS].append(QApplication::translate("DlgRenderVideo","Game console","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_FORTHEWEB].append(QApplication::translate("DlgRenderVideo","SWF Flash Player","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_FORTHEWEB].append(QApplication::translate("DlgRenderVideo","Video-sharing and social WebSite","Device database type"));
    TranslatedRenderSubtype[EXPORTMODE_FORTHEWEB].append(QApplication::translate("DlgRenderVideo","HTML 5","Device database type"));

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
        qDebug()<<QApplication::translate("MainWindow","Error reading configuration file","Error message");
        return false;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn)) {
        qDebug()<<QApplication::translate("MainWindow","Error reading content of configuration file","Error message");
        return false;
    }

    root = domDocument.documentElement();
    if (root.tagName()!=CONFIGFILE_ROOTNAME) {
        qDebug()<<QApplication::translate("MainWindow","The file is not a valid configuration file","Error message");
        return false;
    }

    //if (TypeConfigFile!=USERCONFIGFILE) return true;    // It's finish for GlobalConfigFile

    QDomNodeList    NodeList;

    // Load preferences
    if ((root.elementsByTagName("LastDirectories").length()>0)&&(root.elementsByTagName("LastDirectories").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("LastDirectories").item(0).toElement();
        if (Element.hasAttribute("RememberLastDirectories"))    RememberLastDirectories     =Element.attribute("RememberLastDirectories")=="1";
        if (Element.hasAttribute("LastMediaPath"))              LastMediaPath               =Element.attribute("LastMediaPath");
        if (Element.hasAttribute("LastProjectPath"))            LastProjectPath             =Element.attribute("LastProjectPath");
        if (Element.hasAttribute("LastMusicPath"))              LastMusicPath               =Element.attribute("LastMusicPath");
        if (Element.hasAttribute("LastRenderVideoPath"))        LastRenderVideoPath         =Element.attribute("LastRenderVideoPath");
    }

    if ((root.elementsByTagName("EditorOptions").length()>0)&&(root.elementsByTagName("EditorOptions").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("EditorOptions").item(0).toElement();
        if (Element.hasAttribute("CheckConfigAtStartup"))       CheckConfigAtStartup        =Element.attribute("CheckConfigAtStartup")=="1";
        if (Element.hasAttribute("RasterMode"))                 RasterMode                  =Element.attribute("RasterMode")=="1";
        if (Element.hasAttribute("NbrSlideInCache"))            NbrSlideInCache             =Element.attribute("NbrSlideInCache").toInt();
        if (Element.hasAttribute("MemCacheMaxValue"))           MemCacheMaxValue            =Element.attribute("MemCacheMaxValue").toLongLong();
        if (Element.hasAttribute("SDLAudioOldMode"))            SDLAudioOldMode             =Element.attribute("SDLAudioOldMode")=="1";
        if (Element.hasAttribute("AppendObject"))               AppendObject                =Element.attribute("AppendObject")=="1";
        if (Element.hasAttribute("PartitionMode"))              PartitionMode               =Element.attribute("PartitionMode")=="1";
        if (Element.hasAttribute("DisplayUnit"))                DisplayUnit                 =Element.attribute("DisplayUnit").toInt();
        if (Element.hasAttribute("SortFile"))                   SortFile                    =Element.attribute("SortFile")=="1";
        if (Element.hasAttribute("TimelineHeight"))             TimelineHeight              =Element.attribute("TimelineHeight").toInt();
        if (Element.hasAttribute("DefaultFraming"))             DefaultFraming              =Element.attribute("DefaultFraming").toInt();
        if (Element.hasAttribute("ApplyTransfoPreview"))        ApplyTransfoPreview         =Element.attribute("ApplyTransfoPreview")=="1";
        if (Element.hasAttribute("QuickResamplingPreview"))     QuickResamplingPreview      =Element.attribute("QuickResamplingPreview")=="1";
        if (Element.hasAttribute("Smoothing"))                  Smoothing                   =Element.attribute("Smoothing")=="1";
        if (Element.hasAttribute("PreviewFPS"))                 PreviewFPS                  =Element.attribute("PreviewFPS").toDouble();
        if (Element.hasAttribute("RandomTransition"))           RandomTransition            =Element.attribute("RandomTransition")=="1";
        if (Element.hasAttribute("DefaultTransitionFamilly"))   DefaultTransitionFamilly    =Element.attribute("DefaultTransitionFamilly").toInt();
        if (Element.hasAttribute("DefaultTransitionSubType"))   DefaultTransitionSubType    =Element.attribute("DefaultTransitionSubType").toInt();
        if (Element.hasAttribute("DefaultTransitionDuration"))  DefaultTransitionDuration   =Element.attribute("DefaultTransitionDuration").toInt();
        if (Element.hasAttribute("AskUserToRemove"))            AskUserToRemove             =Element.attribute("AskUserToRemove")!="0";
        if (Element.hasAttribute("SlideRuler"))                 SlideRuler                  =Element.attribute("SlideRuler")!="0";
        if (Element.hasAttribute("FramingRuler"))               FramingRuler                =Element.attribute("FramingRuler")!="0";
        if (Element.hasAttribute("Crop1088To1080"))             Crop1088To1080              =Element.attribute("Crop1088To1080")!="0";
    }

    if ((root.elementsByTagName("ProjectDefault").length()>0)&&(root.elementsByTagName("ProjectDefault").item(0).isElement()==true)) {
        QDomElement  Element=root.elementsByTagName("ProjectDefault").item(0).toElement();
        if (Element.hasAttribute("ImageGeometry"))              ImageGeometry               =Element.attribute("ImageGeometry").toInt();
        if (Element.hasAttribute("NoShotDuration"))             NoShotDuration              =Element.attribute("NoShotDuration").toInt();
        if (Element.hasAttribute("FixedDuration"))              FixedDuration               =Element.attribute("FixedDuration").toInt();
        if (Element.hasAttribute("SpeedWave"))                  SpeedWave                   =Element.attribute("SpeedWave").toInt();

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
    if ((root.elementsByTagName("RenderDefault").length()>0)&&(root.elementsByTagName("RenderDefault").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("RenderDefault").item(0).toElement();
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

    if ((root.elementsByTagName("RecentFiles").length()>0)&&(root.elementsByTagName("RecentFiles").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("RecentFiles").item(0).toElement();
        int i=0;
        while ((Element.elementsByTagName("Recent-"+QString("%1").arg(i)).length()>0)&&(root.elementsByTagName("Recent-"+QString("%1").arg(i)).item(0).isElement()==true)) {
            QDomElement SubElement=root.elementsByTagName("Recent-"+QString("%1").arg(i)).item(0).toElement();
            QString     File=SubElement.attribute("File");
            RecentFile.append(File);
            i++;
        }
    }

    //***************************************
    // Load RenderDeviceModel collection
    //***************************************
    if ((root.elementsByTagName("RenderingDeviceModel").length()>0)&&(root.elementsByTagName("RenderingDeviceModel").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("RenderingDeviceModel").item(0).toElement();
        int i=0;
        while ((Element.elementsByTagName("Device_"+QString("%1").arg(i)).length()>0)&&(root.elementsByTagName("Device_"+QString("%1").arg(i)).item(0).isElement()==true)) {
            if (TypeConfigFile==GLOBALCONFIGFILE) {
                // Reading from global config file : append device
                RenderDeviceModel.append(cDeviceModelDef(TypeConfigFile==GLOBALCONFIGFILE,i));
                RenderDeviceModel[i].LoadFromXML(Element,QString("Device_"+QString("%1").arg(i)),false);
            } else {
                // Reading from user config file : search if device already exist, then load it else append a new one
                QString ElementName=QString("Device_"+QString("%1").arg(i));
                if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
                    QDomElement TheElement=domDocument.elementsByTagName(ElementName).item(0).toElement();
                    int IndexKey=TheElement.attribute("DeviceIndex").toInt();
                    int j=0;
                    while ((j<RenderDeviceModel.count())&&(RenderDeviceModel[j].DeviceIndex!=IndexKey)) j++;
                    if (j<RenderDeviceModel.count()) RenderDeviceModel[IndexKey].LoadFromXML(Element,QString("Device_"+QString("%1").arg(i)),true); else {
                        j=RenderDeviceModel.count();
                        RenderDeviceModel.append(cDeviceModelDef(false,IndexKey));
                        RenderDeviceModel[j].LoadFromXML(Element,QString("Device_"+QString("%1").arg(i)),true);
                    }
                }
            }
            i++;
        }
    }

    // Load other collections
    StyleTextCollection.LoadFromXML(domDocument,root,TypeConfigFile);
    StyleTextBackgroundCollection.LoadFromXML(domDocument,root,TypeConfigFile);
    StyleCoordinateCollection.LoadFromXML(domDocument,root,TypeConfigFile);
    StyleBlockShapeCollection.LoadFromXML(domDocument,root,TypeConfigFile);
    StyleImageFramingCollection.LoadFromXML(domDocument,root,TypeConfigFile);

    //***************************************
    // Windows size & position
    //***************************************

    NodeList=root.elementsByTagName("RestoreWindow");
    if ((NodeList.length()>0)&&(NodeList.at(0).childNodes().length()>0)) RestoreWindow=NodeList.at(0).childNodes().at(0).nodeValue()=="1";

    // Load windows size and position
    MainWinWSP->LoadFromXML(root);                                  // MainWindow - Window size and position
    DlgBackgroundPropertiesWSP->LoadFromXML(root);
    DlgMusicPropertiesWSP->LoadFromXML(root);
    DlgApplicationSettingsWSP->LoadFromXML(root);
    DlgRenderVideoWSP->LoadFromXML(root);
    DlgTransitionPropertiesWSP->LoadFromXML(root);
    DlgSlidePropertiesWSP->LoadFromXML(root);
    DlgImageTransformationWSP->LoadFromXML(root);
    DlgImageCorrectionWSP->LoadFromXML(root);
    DlgVideoEditWSP->LoadFromXML(root);
    DlgTextEditWSP->LoadFromXML(root);
    DlgManageStyleWSP->LoadFromXML(root);
    DlgCheckConfigWSP->LoadFromXML(root);
    return true;
}

//====================================================================================================================

bool cApplicationConfig::SaveConfigurationFile() {
    //qDebug() << "IN:cApplicationConfig::SaveConfigurationValues";

    // Save all option to the configuration file
    QFile           file(UserConfigFile);
    QDomDocument    domDocument(APPLICATION_NAME);
    QDomElement     Element,SubElement,SubSubElement;
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
    Element.setAttribute("RememberLastDirectories",     RememberLastDirectories?"1":"0");
    Element.setAttribute("LastMediaPath",               LastMediaPath);
    Element.setAttribute("LastProjectPath",             LastProjectPath);
    Element.setAttribute("LastMusicPath",               LastMusicPath);
    Element.setAttribute("LastRenderVideoPath",         LastRenderVideoPath);
    root.appendChild(Element);

    Element=domDocument.createElement("EditorOptions");
    Element.setAttribute("CheckConfigAtStartup",        CheckConfigAtStartup?"1":"0");
    Element.setAttribute("RasterMode",                  RasterMode?"1":"0");
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
    Element.setAttribute("ApplyTransfoPreview",         ApplyTransfoPreview?"1":0);
    Element.setAttribute("QuickResamplingPreview",      QuickResamplingPreview?"1":0);
    Element.setAttribute("Smoothing",                   Smoothing?"1":0);
    Element.setAttribute("RandomTransition",            RandomTransition?"1":"0");
    Element.setAttribute("DefaultTransitionFamilly",    DefaultTransitionFamilly);
    Element.setAttribute("DefaultTransitionSubType",    DefaultTransitionSubType);
    Element.setAttribute("DefaultTransitionDuration",   DefaultTransitionDuration);
    Element.setAttribute("AskUserToRemove",             AskUserToRemove?"1":"0");
    Element.setAttribute("SlideRuler",                  SlideRuler?"1":"0");
    Element.setAttribute("FramingRuler",                FramingRuler?"1":"0");
    Element.setAttribute("Crop1088To1080",              Crop1088To1080?"1":"0");
    root.appendChild(Element);

    Element=domDocument.createElement("ProjectDefault");
    Element.setAttribute("ImageGeometry",               ImageGeometry);
    Element.setAttribute("NoShotDuration",              NoShotDuration);
    Element.setAttribute("FixedDuration",               FixedDuration);
    Element.setAttribute("SpeedWave",                   SpeedWave);

    SubElement=domDocument.createElement("DefaultBlock_Text");
    SubElement.setAttribute("TextST",    DefaultBlock_Text_TextST);
    SubElement.setAttribute("BackGST",   DefaultBlock_Text_BackGST);
    SubElement.setAttribute("Coord0ST",  DefaultBlock_Text_CoordST[0]);
    SubElement.setAttribute("Coord1ST",  DefaultBlock_Text_CoordST[1]);
    SubElement.setAttribute("Coord2ST",  DefaultBlock_Text_CoordST[2]);
    SubElement.setAttribute("ShapeST",   DefaultBlock_Text_ShapeST);
    Element.appendChild(SubElement);

    SubElement=domDocument.createElement("DefaultBlockSL_IMG");
    SubElement.setAttribute("TextST",    DefaultBlockSL_IMG_TextST);
    SubElement.setAttribute("ShapeST",   DefaultBlockSL_IMG_ShapeST);
    for (int i=0;i<9;i++) {
        SubSubElement=domDocument.createElement(QString("IMG_GEO_%1").arg(i));
        for (int j=0;j<3;j++) SubSubElement.setAttribute(QString("CoordST_%1").arg(j),DefaultBlockSL_IMG_CoordST[i][j]);
        SubElement.appendChild(SubSubElement);
        for (int j=0;j<3;j++) SubElement.setAttribute(QString("LockLS_%1").arg(j),DefaultBlockSL_CLIPARTLOCK[j]);
    }
    Element.appendChild(SubElement);

    SubElement=domDocument.createElement("DefaultBlockBA_IMG");
    SubElement.setAttribute("TextST",    DefaultBlockBA_IMG_TextST);
    SubElement.setAttribute("ShapeST",   DefaultBlockBA_IMG_ShapeST);
    for (int i=0;i<9;i++) {
        SubSubElement=domDocument.createElement(QString("IMG_GEO_%1").arg(i));
        for (int j=0;j<3;j++) SubSubElement.setAttribute(QString("CoordST_%1").arg(j),DefaultBlockBA_IMG_CoordST[i][j]);
        SubElement.appendChild(SubSubElement);
    }
    for (int j=0;j<3;j++) SubElement.setAttribute(QString("LockBA_%1").arg(j),DefaultBlockBA_CLIPARTLOCK[j]);
    Element.appendChild(SubElement);
    root.appendChild(Element);

    Element=domDocument.createElement("RenderDefault");
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
    root.appendChild(Element);

    Element=domDocument.createElement("RecentFiles");
    for (int i=0;i<RecentFile.count();i++) {
        SubElement=domDocument.createElement("Recent-"+QString("%1").arg(i));
        SubElement.setAttribute("File",RecentFile.at(i));
        Element.appendChild(SubElement);
    }
    root.appendChild(Element);

    Element=domDocument.createElement("RestoreWindow");
    Element.appendChild(domDocument.createTextNode(RestoreWindow?"1":"0"));
    root.appendChild(Element);

    int j;

    // Save RenderDeviceModel collection
    j=0;
    Element=domDocument.createElement("RenderingDeviceModel");
    for (int i=0;i<RenderDeviceModel.count();i++) if (RenderDeviceModel[i].FromUserConf) {
        RenderDeviceModel[i].SaveToXML(Element,QString("Device_"+QString("%1").arg(j)));
        j++;
    }
    if (j>0) root.appendChild(Element);

    // Save other collections
    StyleTextCollection.SaveToXML(domDocument,root);
    StyleTextBackgroundCollection.SaveToXML(domDocument,root);
    StyleCoordinateCollection.SaveToXML(domDocument,root);
    StyleBlockShapeCollection.SaveToXML(domDocument,root);
    StyleImageFramingCollection.SaveToXML(domDocument,root);

    // Save windows size and position
    MainWinWSP->SaveToXML(root);                                // MainWindow - Window size and position
    DlgBackgroundPropertiesWSP->SaveToXML(root);
    DlgMusicPropertiesWSP->SaveToXML(root);
    DlgApplicationSettingsWSP->SaveToXML(root);
    DlgRenderVideoWSP->SaveToXML(root);
    DlgTransitionPropertiesWSP->SaveToXML(root);
    DlgSlidePropertiesWSP->SaveToXML(root);
    DlgImageTransformationWSP->SaveToXML(root);
    DlgImageCorrectionWSP->SaveToXML(root);
    DlgVideoEditWSP->SaveToXML(root);
    DlgTextEditWSP->SaveToXML(root);
    DlgManageStyleWSP->SaveToXML(root);
    DlgCheckConfigWSP->SaveToXML(root);

    // Write file to disk
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        ExitApplicationWithFatalError(QApplication::translate("MainWindow","Error creating configuration file","Error message"));
        return false;
    }
    QTextStream out(&file);
    domDocument.save(out,4);
    file.close();
    return true;
}
