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

// Basic inclusions (common to all files)
#include "SubProjects/VariousClass/_GlobalDefines.h"

// Include some additional standard class

#include <QAction>
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
#include <QWidget>
#include <QWindowsStyle>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

//============================================
// Global defines
//============================================
//#define BUILDVERSION_WEBURL                 "http://download.tuxfamily.org/ffdiaporama/Stable/BUILDVERSION.txt"
#define BUILDVERSION_WEBURL                 "http://download.tuxfamily.org/ffdiaporama/Devel/BUILDVERSION.txt"

#define APPLICATION_ROOTNAME                "Project"                             // Name of root node in the project xml file

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
#define ICON_GEOMETRY_LOCKED                "img/Geometry_Lock.png"
#define ICON_GEOMETRY_UNLOCKED              "img/Geometry_Unlock.png"
#define ICON_GEOMETRY_PROJECT               "img/Geometry_ProjectLock.png"
#define ICON_FRAMING_CUSTOM                 "img/action_cancel.png"
#define ICON_GREEN                          "img/Green.png"
#define ICON_RED                            "img/Red.png"

#define ICON_GEOMETRY_IMAGE                 "img/Geometry_ImageLock.png"
#define ICON_FRAMING_FULL                   "img/AdjustWH.png"
#define ICON_FRAMING_WIDTH                  "img/AdjustW.png"
#define ICON_FRAMING_HEIGHT                 "img/AdjustH.png"
#define ICON_GLOBALCONF                     "img/db.png"
#define ICON_USERCONF                       "img/db_update.png"

// Object type definition
#define DIAPORAMAOBJECTTYPE_EMPTY           0
#define DIAPORAMAOBJECTTYPE_IMAGE           1
#define DIAPORAMAOBJECTTYPE_VIDEO           2

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

#endif // GLOBALDEFINES_H
