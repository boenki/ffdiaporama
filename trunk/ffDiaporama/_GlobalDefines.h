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

/*
    This file contains :
        - All standard library inclusions
        - All QT inclusions
        - Third party library inclusions
        - All global defines
        - Some global static values

    Note : Because this file is included in all other files, it is necessary to limit modification in this file !
*/

#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

//============================================
// Basic inclusions (common to all files)
//============================================

#ifdef _STDINT_H
    #undef _STDINT_H            // Remove previous inclusion (if exist)
#endif

#define __STDC_CONSTANT_MACROS  // Activate macro for stdint
#include <stdint.h>             // Include stdint with macro activated
#include <math.h>

// SDL Library must be include before any other
extern "C" {
    #include <SDL/SDL.h>
}

#ifdef __MINGW32__
    #undef main // We don't want SDL to override our main()
#endif

//============================================
// Standard QT inclusions needed by all files
//============================================
#include <QtCore>
#include <QtDebug>
#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QByteArray>
#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QCursor>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QHeaderView>
#include <QIcon>
#include <QImage>
#include <QInputDialog>
#include <QLabel>
#include <QList>
#include <QListView>
#include <QMainWindow>
#include <QMatrix>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QMouseEvent>
#include <QtNetwork/QNetworkReply>
#include <QObject>
#include <QPaintDevice>
#include <QPainter>
#include <QProcess>
#include <QProgressBar>
#include <QScrollBar>
#include <QShowEvent>
#include <QSize>
#include <QSizeF>
#include <QSlider>
#include <QSplashScreen>
#include <QString>
#include <QStringList>
#include <QStyledItemDelegate>
#include <QStyleOptionGraphicsItem>
#include <QStyleOptionViewItem>
#include <QTableWidget>
#include <QTextOption>
#include <QTextStream>
#include <QTime>
#include <QTimeEdit>
#include <QTimer>
#include <QTranslator>
//#include <QtWebKit/QWebView>
//#include <QtWebKit/QWebHistory>
//#include <QtWebKit/QWebPage>
#include <QWidget>
#include <QWindowsStyle>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

//============================================
// Other third party library inclusions
//============================================

// ffmpeg
extern "C" {
    #include <libavutil/common.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

// FMT_FILTERS
#include <fmt_filters.h>

//============================================
// Global defines
//============================================
#define APPLICATION_NAME                    "ffDiaporama"
#define APPLICATION_VERSION                 "1.1 beta 1"
//#define BUILDVERSION_WEBURL                 "http://download.tuxfamily.org/ffdiaporama/Stable/BUILDVERSION.txt"
#define BUILDVERSION_WEBURL                 "http://download.tuxfamily.org/ffdiaporama/Devel/BUILDVERSION.txt"

#define CONFIGFILEEXT                       ".xml"                                // File extension of configuration files
#define CONFIGFILE_ROOTNAME                 "Configuration"                       // Name of root node in the config xml file
#define APPLICATION_ROOTNAME                "Project"                             // Name of root node in the project xml file

#define GLOBALCONFIGFILE                    2                                     // Type of config file : Global
#define USERCONFIGFILE                      3                                     // Type of config file : USER
#define BUFFERING_NBR_FRAME                 5                                     // Number of frame wanted in the playing buffer

#define ICON_PLAYERPLAY                     "img/player_play.png"                 // FileName of play icon
#define ICON_PLAYERPAUSE                    "img/player_pause.png"                // FileName of pause icon
#define ICON_SHOTPRESENCE                   "img/TimelineShots.png"               // FileName of icon representing shots in the timeline
#define ICON_BLOCKPRESENCE                  "img/TimelineBlocks.png"              // FileName of icon representing blocks in the timeline
#define ICON_VISIBLE_OK                     "img/Visible_OK.png"
#define ICON_VISIBLE_KO                     "img/Visible_KO.png"
#define ICON_SOUND_OK                       "img/object_sound.png"
#define ICON_SOUND_KO                       "img/sound_KO.png"

#define ICON_OBJECT_TEXT                    "img/object_text.png"
#define ICON_OBJECT_SOUND                   "img/object_sound.png"
#define ICON_OBJECT_MOVIE                   "img/object_movie.png"
#define ICON_OBJECT_IMAGE                   "img/object_image.png"
#define ICON_OBJECT_TEXTHIDE                "img/object_textHide.png"
#define ICON_OBJECT_MOVIEHIDE               "img/object_movieHide.png"
#define ICON_OBJECT_IMAGEHIDE               "img/object_imageHide.png"
#define ICON_RULER_ON                       "img/ruler_ok.png"
#define ICON_RULER_OFF                      "img/ruler_ko.png"
#define ICON_GLOBALCONF                     "img/db.png"
#define ICON_USERCONF                       "img/db_update.png"
#define ICON_GEOMETRY_LOCKED                "img/Geometry_Lock.png"
#define ICON_GEOMETRY_UNLOCKED              "img/Geometry_Unlock.png"
#define ICON_GEOMETRY_PROJECT               "img/Geometry_ProjectLock.png"
#define ICON_GEOMETRY_IMAGE                 "img/Geometry_ImageLock.png"
#define ICON_FRAMING_WIDTH                  "img/AdjustW.png"
#define ICON_FRAMING_HEIGHT                 "img/AdjustH.png"
#define ICON_FRAMING_FULL                   "img/AdjustWH.png"
#define ICON_FRAMING_CUSTOM                 "img/action_cancel.png"
#define ICON_GREEN                          "img/Green.png"
#define ICON_RED                            "img/Red.png"

#define STYLENAME_TEXTSTYLE                 "TextStyleCollection"
#define STYLENAME_BACKGROUNDSTYLE           "TextBackgroundStyleCollection"
#define STYLENAME_COORDINATESTYLE           "StyleCoordinateCollection"
#define STYLENAME_BLOCKSHAPESTYLE           "StyleBlockShapeCollection"
#define STYLENAME_FRAMINGSTYLE              "StyleImageFramingCollection"

// Project geometry definition
#define GEOMETRY_4_3                        0
#define GEOMETRY_16_9                       1
#define GEOMETRY_40_17                      2
#define NBR_GEOMETRY_DEF                    3

#define IMAGE_GEOMETRY_UNKNOWN              0   // undefined image geometry
#define IMAGE_GEOMETRY_3_2                  1   // Standard 3:2 landscape image
#define IMAGE_GEOMETRY_2_3                  2   // Standard 3:2 portrait image
#define IMAGE_GEOMETRY_4_3                  3   // Standard 4:3 landscape image
#define IMAGE_GEOMETRY_3_4                  4   // Standard 4:3 portrait image
#define IMAGE_GEOMETRY_16_9                 5   // Standard 16:9 landscape image
#define IMAGE_GEOMETRY_9_16                 6   // Standard 16:9 portrait image
#define IMAGE_GEOMETRY_40_17                7   // Standard cinema landscape image
#define IMAGE_GEOMETRY_17_40                8   // Standard cinema portrait image

// Export mode definition
#define EXPORTMODE_ADVANCED                 0
#define ICON_ADVANCED                       "img/render.png"
#define EXPORTMODE_SMARTPHONE               1
#define ICON_SMARTPHONE                     "img/Smartphone.png"
#define EXPORTMODE_MULTIMEDIASYS            2
#define ICON_MULTIMEDIASYS                  "img/tv.png"
#define EXPORTMODE_FORTHEWEB                3
#define ICON_FORTHEWEB                      "img/Internet.png"

// Standard definition
#define STANDARD_PAL                        0
#define STANDARD_NTSC                       1

// Image size definition
#define NBR_SIZEDEF                         9
#define SIZE_QVGA                           0
#define SIZE_HVGA                           1
#define SIZE_VGA                            2
#define SIZE_DVD                            3
#define SIZE_WVGA                           4
#define SIZE_XGA                            5
#define SIZE_720P                           6
#define SIZE_1080p                          7
#define SIZE_RIM240                         8

// Object type definition
#define DIAPORAMAOBJECTTYPE_EMPTY           0
#define DIAPORAMAOBJECTTYPE_IMAGE           1
#define DIAPORAMAOBJECTTYPE_VIDEO           2

// Composition type definition
#define COMPOSITIONTYPE_BACKGROUND          0
#define COMPOSITIONTYPE_OBJECT              1
#define COMPOSITIONTYPE_SHOT                2

// Shot type definition
#define SHOTTYPE_STATIC                     0
#define SHOTTYPE_MOBIL                      1
#define SHOTTYPE_VIDEO                      2

// Thumbnail type definition
#define THUMBNAILTYPE_OBJECT                0
#define THUMBNAILTYPE_NULL                  1
#define THUMBNAILTYPE_SHOT                  2

// Display coordinates unit
#define DISPLAYUNIT_PERCENT                 0
#define DISPLAYUNIT_PIXELS                  1

// Speed wave forme definition
#define SPEEDWAVE_LINEAR                    0
#define SPEEDWAVE_SINQUARTER                1

// Thumbnails definitions
#define ADJUSTXCOLUMN                       10      // width to add to object image for display transition
#define TIMELINEMINHEIGH                    80      // min heigh of montage track
#define TIMELINEMAXHEIGH                    160     // max heigh of montage track
#define TIMELINESOUNDHEIGHT                 18      // Height of sound & music track

// Composition parameters
#define SCALINGTEXTFACTOR                   400

// Brush type definition
#define BRUSHTYPE_NOBRUSH                   0
#define BRUSHTYPE_SOLID                     1
#define BRUSHTYPE_PATTERN                   2
#define BRUSHTYPE_GRADIENT2                 3
#define BRUSHTYPE_GRADIENT3                 4
#define BRUSHTYPE_IMAGELIBRARY              5
#define BRUSHTYPE_IMAGEDISK                 6

// Brush Gradient orientation
#define GRADIENTORIENTATION_UPLEFT          0
#define GRADIENTORIENTATION_UP              1
#define GRADIENTORIENTATION_UPRIGHT         2
#define GRADIENTORIENTATION_LEFT            3
#define GRADIENTORIENTATION_RADIAL          4
#define GRADIENTORIENTATION_RIGHT           5
#define GRADIENTORIENTATION_BOTTOMLEFT      6
#define GRADIENTORIENTATION_BOTTOM          7
#define GRADIENTORIENTATION_BOTTOMRIGHT     8

// OnOffFilter mask definition
#define FilterEqualize                      0x01
#define FilterDespeckle                     0x02
#define FilterGray                          0x04

// Transition familly definition
#define TRANSITIONFAMILLY_BASE              0
#define TRANSITIONFAMILLY_ZOOMINOUT         1
#define TRANSITIONFAMILLY_SLIDE             2
#define TRANSITIONFAMILLY_PUSH              3
#define TRANSITIONFAMILLY_LUMA_BAR          4
#define TRANSITIONFAMILLY_LUMA_BOX          5
#define TRANSITIONFAMILLY_LUMA_CENTER       6
#define TRANSITIONFAMILLY_LUMA_CHECKER      7
#define TRANSITIONFAMILLY_LUMA_CLOCK        8
#define TRANSITIONFAMILLY_LUMA_SNAKE        9

// No luma transition : number of sub type
#define TRANSITIONMAXSUBTYPE_BASE           4
#define TRANSITIONMAXSUBTYPE_ZOOMINOUT      18
#define TRANSITIONMAXSUBTYPE_SLIDE          16
#define TRANSITIONMAXSUBTYPE_PUSH           16

//============================================
// Dialogbox help file
//============================================
#define WIKI_CMS_PATH                       "http://ffdiaporama.tuxfamily.org/ffdiaporama/?page_id="
#define HELPFILE_INDEX                      "1567"                                                              //fct_001
#define HELPFILE_PRINCIPLES                 "1306"                                                              //fct_002
#define HELPFILE_DEVICES                    "1322"                                                              //fct_009
#define HELPFILE_NEWS                       "http://ffdiaporama.tuxfamily.org/<local>/News.php"
#define HELPFILE_WEBSITEINDEX               "http://ffdiaporama.tuxfamily.org/<local>/index.php"
#define HELPFILE_SUPPORT                    "1567"                                                              //fct_001"
#define HELPFILE_DlgTextEdit                "1282"                                                              //dlg_001"
#define HELPFILE_DlgApplicationSettings     "1286"                                                              //dlg_002"
#define HELPFILE_DlgSlideProperties         "1288"                                                              //dlg_003"
#define HELPFILE_DlgBackgroundProperties    "1290"                                                              //dlg_004"
#define HELPFILE_DlgImageCorrection         "1292"                                                              //dlg_005"
#define HELPFILE_DlgManageStyle             "1762"                                                              //dlg_006"
#define HELPFILE_DlgMusicProperties         "1296"                                                              //dlg_007"
#define HELPFILE_DlgRenderVideo             "1298"                                                              //dlg_008"
#define HELPFILE_DlgTransitionProperties    "1300"                                                              //dlg_009"
#define HELPFILE_DlgVideoEdit               "1302"                                                              //dlg_010"
#define HELPFILE_DlgCheckConfig             "3086"

//============================================
// Default values
//============================================
#define DEFAULT_FONT_FAMILLY                "Arial"
#define DEFAULT_FONT_SIZE                   12
#define DEFAULT_FONT_COLOR                  "#ffffaa"
#define DEFAULT_FONT_SHADOWCOLOR            "#333333"
#define DEFAULT_FONT_ISBOLD                 false
#define DEFAULT_FONT_ISITALIC               false
#define DEFAULT_FONT_ISUNDERLINE            false
#define DEFAULT_FONT_HALIGN                 1
#define DEFAULT_FONT_VALIGN                 1
#define DEFAULT_FONT_TEXTEFFECT             5

#define DEFAULT_SHAPE_OPACITY               0
#define DEFAULT_SHAPE_BORDERSIZE            0
#define DEFAULT_SHAPE_BORDERCOLOR           "#333333"
#define DEFAULT_SHAPE_SHADOWCOLOR           "#000000"
#define DEFAULT_SHAPE_INTERNALBORDERSIZE    6
#define DEFAULT_SHAPE_INTERNALBORDERCOLOR1  "#808080"
#define DEFAULT_SHAPE_INTERNALBORDERCOLOR2  "#c0c0c0"
#define DEFAULT_SHAPE_BRUSHCOLORD           "#808080"

//============================================
// String to include in the system properties
//============================================

#define OPERATINGSYSTEM_STR                 "Operating system version\t: "
#define STARTINGPATH_STR                    "Starting path\t: "
#define WORKINGPATH_STR                     "Working path set to\t: "
#define SYSTEMLOCAL_STR                     "Detected system locale\t: "
#define LOADEDLOCAL_STR                     "Translation file loaded\t: "
#define COMMONPARTVERSION_STR               "Common part version\t: "
#define VERSIONQT_STR                       "QT version\t\t: "
#define FMTFILTERVERSION_STR                "fmt_filters version\t: "
#define SDLVERSION_STR                      "SDL version\t\t: "
#define LIBAVCODECVERSION_STR               "LIBAVCODEC version\t: "
#define LIBAVFORMATVERSION_STR              "LIBAVFORMAT version\t: "
#define LIBSWSCALEVERSION_STR               "LIBSWSCALE version\t: "
#define TEMPDIR_STR                         "Temp directory to be use\t: "
#define USERCONFIGFILE_STR                  "User config file to be use\t: "

//============================================
// Image format definition
//============================================
struct sIMAGEDEF {
    int     Width;                                              // Width
    int     Height;                                             // Height
    int     PARNUM;                                             // Pixel aspect ratio (num)
    int     PARDEN;                                             // Pixel aspect ratio (den)
    double  dFPS;                                               // Frame per second
    char    FPS[20];                                            // Frame per second
    char    Name[100];                                          // Display name
    int     Extend;                                             // Padding for cinema mode with DVD
};
extern sIMAGEDEF DefImageFormat [2][3][NBR_SIZEDEF];            // Image format definition (_applicationconfig.cpp)
extern QString IMAGEDEFGENNAME[2][NBR_SIZEDEF];                 // Image format generic name (_applicationconfig.cpp)
extern int ORDERIMAGENAME[NBR_SIZEDEF];                         // Display order of image size

//============================================
// Video codec definitions
//============================================
#define     VCODEC_MJPEG        0                               // Motion JPEG
#define     VCODECST_MJPEG      "MJPEG"                         // String Motion JPEG
#define     VCODEC_MPEG         1                               // MPEG-2 video
#define     VCODECST_MPEG       "MPEG"                          // String MPEG-2 video
#define     VCODEC_MPEG4        2                               // DivX/XVid/MPEG-4
#define     VCODECST_MPEG4      "MPEG4"                         // String DivX/XVid/MPEG-4
#define     VCODEC_H264HQ       3                               // H.264 AVC/MPEG-4 AVC + VPRE libx264-hq.ffpreset
#define     VCODECST_H264HQ     "H264HQ"                        // String H.264 AVC/MPEG-4 AVC + VPRE libx264-hq.ffpreset
#define     VCODEC_H264PQ       4                               // H.264 AVC/MPEG-4 AVC + VPRE libx264-hq.ffpreset ********
#define     VCODECST_H264PQ     "H264PQ"                        // String H.264 AVC/MPEG-4 AVC + VPRE libx264-hq.ffpreset ********
#define     VCODEC_VP8          5                               // WebM-VP8
#define     VCODECST_VP8        "VP8"                           // String WebM-VP8
#define     VCODEC_H263         6                               // Flash Video / Sorenson H.263
#define     VCODECST_H263       "H263"                          // String Flash Video / Sorenson H.263
#define     VCODEC_THEORA       7                               // Theora
#define     VCODECST_THEORA     "THEORA"                        // String Theora

struct sVideoCodecDef {
    bool    IsFind;                                             // true if codec is supported by installed version of ffmpeg
    int     Codec_id;                                           // ffmpeg codec id
    int     FFD_VCODEC;                                         // ffDiaporama video codec id
    char    FFD_VCODECST[10];                                   // ffDiaporama video codec id string
    char    ShortName[50];                                      // short name of the codec (copy of the ffmpeg value)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleBitrate[200];                               // list of possible compression bit rate (define by this application)
    char    DefaultBitrate[NBR_SIZEDEF][10];                    // prefered compression bit rate for each possible size
};
#define NBR_VIDEOCODECDEF   8
extern struct sVideoCodecDef VIDEOCODECDEF[NBR_VIDEOCODECDEF];  // Real definition in capplicationconfig.cpp

//============================================
// Audio codec definitions
//============================================
struct sAudioCodecDef {
    bool    IsFind;                                             // true if codec is supported by installed version of ffmpeg
    int     Codec_id;                                           // ffmpeg codec id
    char    ShortName[50];                                      // short name of the codec (copy of the ffmpeg value)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleBitrate2CH[200];                            // list of possible compression bit rate in stereo mode (define by this application)
    bool    Possibly6CH;                                        // true if this codec support 5.1/6 chanels mode
    char    PossibleBitrate6CH[200];                            // list of possible compression bit rate in 5.1/6 chanels mode (define by this application)
    char    Default[10];                                        // prefered compression bit rate
};
#define NBR_AUDIOCODECDEF   7
extern struct sAudioCodecDef AUDIOCODECDEF[NBR_AUDIOCODECDEF];  // Real definition in capplicationconfig.cpp

//============================================
// Format container definitions
//============================================
struct sFormatDef {
    bool    IsFind;                                             // true if format container is supported by installed version of ffmpeg
    char    ShortName[50];                                      // short name of the format container (copy of the ffmpeg value)
    char    FileExtension[10];                                  // prefered file extension for the format container (define by this application)
    char    LongName[200];                                      // long name of the codec (define by this application)
    char    PossibleVideoCodec[200];                            // list of possible video codec for this format container (using VCODECST String define)
    char    PossibleAudioCodec[200];                            // list of possible audio codec for this format container (define by this application)
};
#define NBR_FORMATDEF   10
extern struct sFormatDef FORMATDEF[NBR_FORMATDEF];              // Real definition in capplicationconfig.cpp

//============================================
// Basic class from this project
//============================================
#include "_StyleDefinitions.h"
#include "_SoundDefinitions.h"
#include "_ImagesDefinitions.h"
#include "_cLuLoImageCache.h"
#include "_ImageFileWrapper.h"
#include "_VideoFileWrapper.h"
#include "_ApplicationDefinitions.h"

#endif // GLOBALDEFINES_H
