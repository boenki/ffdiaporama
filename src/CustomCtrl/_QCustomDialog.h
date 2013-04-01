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

#ifndef _QCUSTOMDIALOG_H
#define _QCUSTOMDIALOG_H

// Basic inclusions (common to all files)
#include "../engine/_GlobalDefines.h"

// Include some additional standard class
#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QDomDocument>
#include <QDesktopServices>
#include <QHeaderView>
#include <QIcon>
#include <QImage>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QShowEvent>
#include <QString>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTreeWidget>
#include <QWidget>

// Include some common various class
#include "../engine/cBaseApplicationConfig.h"

//**********************************************************************************
// Dialogbox help file number
//**********************************************************************************

#define HELPFILE_NEWS                       5
#define HELPFILE_WIKIINDEX                  8386
#define HELPFILE_PRINCIPLESINDEX            8388    // How ffDiaporama works
#define HELPFILE_RENDERINDEX                8466    // Rendering videos
#define HELPFILE_DlgApplicationSettings     8406    // Change ffDiaporama's settings
#define HELPFILE_DlgCheckConfig             8409    // Check configuration
#define HELPFILE_DlgSlideProperties         8486    // Modifying slides
#define HELPFILE_DlgManageDevices           8411    // Manage device's profiles
#define HELPFILE_DlgImageCorrection         8413    // Reframe and correct images or videos
#define HELPFILE_DlgBackgroundProperties    8415    // Define slide's background
#define HELPFILE_DlgMusicProperties         8417    // Define music track
#define HELPFILE_DlgTransitionProperties    8419    // Define transitions
#define HELPFILE_DlgVideoEdit               8421    // Cut a video
#define HELPFILE_DlgTextEdit                8423    // Defining the properties of a text block
#define HELPFILE_DlgManageStyle             8425    // Managing styles
#define HELPFILE_DlgffDPjrProperties        8427    // Defining the project properties
#define HELPFILE_DlgRenderVideo             8429    // Rendering the video
#define HELPFILE_DlgInfoFile                8443    // Display the properties of a multimedia file
#define HELPFILE_DlgSlideDuration           0       // No page for that !
#define HELPFILE_DlgTransitionDuration      0       // No page for that !
#define HELPFILE_DlgRulerDef                0       // No page for that !
#define HELPFILE_DlgManageFavorite          0       // No page for that !

//**********************************************************************************
// name of default icons
//**********************************************************************************

#define ICON_GEOMETRY_LOCKED                ":/img/Geometry_Lock.png"
#define ICON_GEOMETRY_UNLOCKED              ":/img/Geometry_Unlock.png"
#define ICON_GEOMETRY_PROJECT               ":/img/Geometry_ProjectLock.png"

//**********************************************************************************
// Display coordinates unit
//**********************************************************************************

#define DISPLAYUNIT_PERCENT                 0
#define DISPLAYUNIT_PIXELS                  1

//**********************************************************************************

class QCustomDialog : public QDialog {
Q_OBJECT
public:
    struct sUndoData {
        int         ActionType;
        QString     Data;
        QWidget     *FocusWindow;
    };

    QList<sUndoData>  UndoDataList;

    QString                 HelpURL;
    cBaseApplicationConfig  *BaseApplicationConfig;
    cSaveWindowPosition     *DlgWSP;
    QDomDocument            *Undo;                          // Save object before modification for cancel button
    QStringList             UndoData;
    QAbstractButton         *OkBt;
    QAbstractButton         *CancelBt;
    QAbstractButton         *HelpBt;
    QAbstractButton         *UndoBt;

    explicit        QCustomDialog(int HelpURLIndex,cBaseApplicationConfig *BaseApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent = 0);
                    ~QCustomDialog();

    virtual void    InitDialog();

    // Abstract function : need to be overloaded
    virtual void    DoInitDialog()=0;           // Initialise dialog
    virtual bool    DoAccept()=0;               // Call when user click on Ok button
    virtual void    DoRejet()=0;                // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo()=0;      // Initiale Undo
    virtual void    DoGlobalUndo()=0;           // Apply Undo : call when user click on Cancel button

    virtual void    SaveWindowState();
    virtual void    RestoreWindowState();

    virtual void    AppendPartialUndo(int ActionType,QWidget *FocusWindow,bool ForceAdd);
    virtual void    RemoveLastPartialUndo();

    virtual void    PreparePartialUndo(int,QDomElement) {}     // To be overloaded
    virtual void    ApplyPartialUndo(int,QDomElement)   {}     // To be overloaded

protected:
    virtual void    accept();
    virtual void    reject();

    // Utility functions
    virtual void                DoInitTableWidget(QTableWidget *Table,QString TableColumns);
    virtual QTableWidgetItem    *CreateItem(QString ItemText,int Alignment,QBrush Background);
    virtual void                DoResizeColumnsTableWidget(QTableWidget *Table);
    virtual void                SetCBIndex(QComboBox *CB,int ItemData);                             // Utility function use to set current index in a combobox base on an itemdata

private slots:
    void            doHelp();
    void            DoPartialUndo();
};

//**********************************************************************************

class QNullTableWidgetItem : public QTableWidgetItem {
public:
    explicit QNullTableWidgetItem(const QIcon &icon, const QString &text, int type):QTableWidgetItem(icon,text,type=Type)   {setFlags(Qt::NoItemFlags);}
    explicit QNullTableWidgetItem(const QTableWidgetItem &other):QTableWidgetItem(other)                                    {setFlags(Qt::NoItemFlags);}
    explicit QNullTableWidgetItem(int type):QTableWidgetItem(type=Type)                                                     {setFlags(Qt::NoItemFlags);}
    explicit QNullTableWidgetItem(const QString &text,int type=Type):QTableWidgetItem(text,type=Type)                       {setFlags(Qt::NoItemFlags);}
};


// New definition for QInputDialog::getText
QString CustomInputDialog(QWidget *parent,const QString &title,const QString &label,int mode,const QString &text,bool *ok,Qt::InputMethodHints inputMethodHints=Qt::ImhNone);
int     CustomMessageBox (QWidget *parent,QMessageBox::Icon icon,const QString& title,const QString& text,QMessageBox::StandardButtons buttons=QMessageBox::Ok,QMessageBox::StandardButton defaultButton=QMessageBox::NoButton);

#endif // _QCUSTOMDIALOG_H
