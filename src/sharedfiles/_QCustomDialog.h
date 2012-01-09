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

#ifndef _QCUSTOMDIALOG_H
#define _QCUSTOMDIALOG_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QShowEvent>
#include <QString>
#include <QDialog>
#include <QDomDocument>
#include <QPushButton>
#include <QDesktopServices>
#include <QTableWidgetItem>

// Include some common various class
#include "cBaseApplicationConfig.h"

class QCustomDialog : public QDialog {
Q_OBJECT
public:
    QString                 HelpURL;
    cBaseApplicationConfig  *BaseApplicationConfig;
    cSaveWindowPosition     *DlgWSP;
    QDomDocument            *Undo;                          // Save object before modification for cancel button
    QPushButton             *OkBt;
    QPushButton             *CancelBt;
    QPushButton             *HelpBt;

    explicit        QCustomDialog(QString HelpURL,cBaseApplicationConfig *BaseApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent = 0);
                    ~QCustomDialog();

    virtual void    InitDialog();

    // Abstract function : need to be overloaded
    virtual void    DoInitDialog()=0;           // Initialise dialog
    virtual void    DoAccept()=0;               // Call when user click on Ok button
    virtual void    DoRejet()=0;                // Call when user click on Cancel button
    virtual void    PrepareGlobalUndo()=0;      // Initiale Undo
    virtual void    DoGlobalUndo()=0;           // Apply Undo : call when user click on Cancel button

protected:
    virtual void    accept();
    virtual void    reject();

    // Utility functions
    virtual void                DoInitTableWidget(QTableWidget *Table,QString TableColumns);
    virtual QTableWidgetItem    *CreateItem(QString ItemText,int Alignment,QBrush Background);
    virtual void                DoResizeColumnsTableWidget(QTableWidget *Table);

private slots:
    void            doHelp();
};

#endif // _QCUSTOMDIALOG_H
