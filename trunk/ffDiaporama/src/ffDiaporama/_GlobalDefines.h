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
#include "../sharedfiles/_GlobalDefines.h"

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


#define ICON_GREEN                          ":/img/Green.png"
#define ICON_RED                            ":/img/Red.png"

#define ICON_GLOBALCONF                     ":/img/db.png"
#define ICON_USERCONF                       ":/img/db_update.png"


#endif // GLOBALDEFINES_H
