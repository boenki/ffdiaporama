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

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#include <winbase.h>
#endif

// Specific inclusions
#include "DlgManageStyle.h"
#include "ui_DlgManageStyle.h"

#include "mainwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>

#define ICON_FRAMING_CUSTOM                 ":/img/action_cancel.png"
#define ICON_FRAMING_FULL                   ":/img/AdjustWH.png"
#define ICON_FRAMING_WIDTH                  ":/img/AdjustW.png"
#define ICON_FRAMING_HEIGHT                 ":/img/AdjustH.png"
#define ICON_GLOBALCONF                     ":/img/db.png"
#define ICON_USERCONF                       ":/img/db_update.png"

//============================================================================================================================
// Manage style dialog
//============================================================================================================================

DlgManageStyle::DlgManageStyle(cStyleCollection *TheCollection,QWidget *parent):QDialog(parent),ui(new Ui::DlgManageStyle) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::DlgManageStyle");
    ui->setupUi(this);
    Collection    =TheCollection;
    UndoCollection=Collection->PrepUndo();

    setWindowFlags((windowFlags()|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMaximizeButtonHint)&(~Qt::WindowMinimizeButtonHint));

    PopulateList("");

    // Define handler
    connect(ui->CancelBt,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->OkBt,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ui->HelpBT,SIGNAL(clicked()),this,SLOT(Help()));

    connect(ui->ListStyle,SIGNAL(currentRowChanged(int)),this,SLOT(s_currentRowChanged(int)));

    connect(ui->DBRenameBT,SIGNAL(clicked()),this,SLOT(s_DBRename()));
    connect(ui->DBRemoveBT,SIGNAL(clicked()),this,SLOT(s_DBRemove()));
    connect(ui->DBResetBT,SIGNAL(clicked()),this,SLOT(s_DBReset()));

    ui->ListStyle->setCurrentRow(0);
}

//====================================================================================================================

DlgManageStyle::~DlgManageStyle() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::~DlgManageStyle");
    delete ui;
    delete UndoCollection;
}

//====================================================================================================================

void DlgManageStyle::Help() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::Help");
    GlobalMainWindow->OpenHelp(HELPFILE_DlgManageStyle);
}

//====================================================================================================================

void DlgManageStyle::SetSavedWindowGeometry() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::SetSavedWindowGeometry");
    GlobalMainWindow->ApplicationConfig->DlgManageStyleWSP->ApplyToWindow(this);
}

//====================================================================================================================

void DlgManageStyle::showEvent(QShowEvent *ev) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::showEvent");
    QDialog::showEvent(ev);
    QTimer::singleShot(0,this,SLOT(SetSavedWindowGeometry()));
}

//====================================================================================================================

void DlgManageStyle::reject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::reject");
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgManageStyleWSP->SaveWindowState(this);
    UndoCollection->SourceCollection->ApplyUndo(UndoCollection);
    done(1);
}

//====================================================================================================================

void DlgManageStyle::accept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::accept");
    // Save Window size and position
    GlobalMainWindow->ApplicationConfig->DlgManageStyleWSP->SaveWindowState(this);
    done(0);
}

//====================================================================================================================

void DlgManageStyle::PopulateList(QString StyleToActivate) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::PopulateList");
    ui->ListStyle->setUpdatesEnabled(false);
    ui->ListStyle->clear();
    QString Item;
    for (int i=0;i<Collection->Collection.count();i++)
        if (((!Collection->GeometryFilter)&&(Collection->Collection[i].GetFilteredPart()==""))||
            (((Collection->GeometryFilter)&&(Collection->Collection[i].GetFilteredPart()==Collection->ActiveFilter)))
        ) {
        Item=Collection->Collection[i].StyleName.mid(Collection->Collection[i].GetFilteredPart().length());
        ui->ListStyle->addItem(new QListWidgetItem(Collection->Collection[i].FromUserConf?QIcon(ICON_USERCONF):QIcon(ICON_GLOBALCONF),Item));
        if (StyleToActivate==(Collection->GeometryFilter?Collection->ActiveFilter:"")+Item) ui->ListStyle->setCurrentRow(ui->ListStyle->count()-1);
    }
    ui->ListStyle->setUpdatesEnabled(true);
}

//====================================================================================================================

void DlgManageStyle::s_currentRowChanged(int NewRow) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::s_currentRowChanged");
    QListWidgetItem *Item=ui->ListStyle->item(NewRow);
    if (Item) {
        QString StyleName=(Collection->GeometryFilter?Collection->ActiveFilter:"")+Item->text();
        int i=0;
        while ((i<Collection->Collection.count())&&(Collection->Collection[i].StyleName!=StyleName)) i++;
        if ((i<Collection->Collection.count())&&(Collection->Collection[i].StyleName==StyleName)) {
            ui->DBRenameBT->setEnabled(true);
            ui->DBRemoveBT->setEnabled(Collection->Collection[i].FromGlobalConf==false);
            ui->DBResetBT->setEnabled(Collection->Collection[i].FromGlobalConf==true);
        } else {
            ui->DBRenameBT->setEnabled(false);
            ui->DBRemoveBT->setEnabled(false);
            ui->DBResetBT->setEnabled(false);
        }
    } else {
        ui->DBRenameBT->setEnabled(false);
        ui->DBRemoveBT->setEnabled(false);
        ui->DBResetBT->setEnabled(false);
    }
}

//====================================================================================================================

void DlgManageStyle::s_DBRename() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::s_DBRename");
    QListWidgetItem *Item=ui->ListStyle->item(ui->ListStyle->currentRow());
    if (Item) {
        QString StyleName=(Collection->GeometryFilter?Collection->ActiveFilter:"")+Item->text();
        int i=0;
        while ((i<Collection->Collection.count())&&(Collection->Collection[i].StyleName!=StyleName)) i++;
        if ((i<Collection->Collection.count())&&(Collection->Collection[i].StyleName==StyleName)) {
            bool    Ok,Continue=true;
            QString Text=Item->text();
            while (Continue) {
                Continue=false;
                Text=QInputDialog::getText(this,QApplication::translate("DlgManageStyle","Rename style"),QApplication::translate("DlgManageStyle","New style name:"),QLineEdit::Normal,Text,&Ok);
                if (Ok && !Text.isEmpty()) {
                    // Ensure Style is not use by another style
                    int j=0;
                    while ((j<Collection->Collection.count())&&((j==i)||(Collection->Collection[j].StyleName!=(Collection->GeometryFilter?Collection->ActiveFilter:"")+Text))) j++;
                    if ((j<Collection->Collection.count())&&(Collection->Collection[j].StyleName==(Collection->GeometryFilter?Collection->ActiveFilter:"")+Text)) {
                        CustomMessageBox(this,QMessageBox::Critical,QApplication::translate("DlgManageStyle","Rename style"),
                                              QApplication::translate("DlgManageStyle","A style with this name already exist.\nPlease select another name!"));
                        Continue=true;
                    } else {
                        // If all is ok then apply new name
                        Collection->Collection[i].StyleName=(Collection->GeometryFilter?Collection->ActiveFilter:"")+Text;
                        Collection->Collection[i].FromUserConf=true;
                    }
                }
            }
            QString StyleName=Collection->Collection[i].StyleName;
            Collection->SortList();
            PopulateList(StyleName);
        }
    }
}

//====================================================================================================================

void DlgManageStyle::s_DBRemove() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::s_DBRemove");
    QListWidgetItem *Item=ui->ListStyle->item(ui->ListStyle->currentRow());
    if (Item) {
        QString StyleName=(Collection->GeometryFilter?Collection->ActiveFilter:"")+Item->text();
        int i=0;
        while ((i<Collection->Collection.count())&&(Collection->Collection[i].StyleName!=StyleName)) i++;
        if ((i<Collection->Collection.count())&&(Collection->Collection[i].StyleName==StyleName)) {
            if (ui->ListStyle->currentRow()<ui->ListStyle->count()-1) Item=ui->ListStyle->item(ui->ListStyle->currentRow()+1);
            else if (ui->ListStyle->currentRow()>0) Item=ui->ListStyle->item(ui->ListStyle->currentRow()-1);
            else Item=NULL;
            Collection->Collection.removeAt(i);
            PopulateList(Item==NULL?"":(Collection->GeometryFilter?Collection->ActiveFilter:"")+Item->text());
        }
    }
}

//====================================================================================================================

void DlgManageStyle::s_DBReset() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgManageStyle::s_DBReset");
    QListWidgetItem *Item=ui->ListStyle->item(ui->ListStyle->currentRow());
    if (Item) {
        QString StyleName=(Collection->GeometryFilter?Collection->ActiveFilter:"")+Item->text();
        int i=0;
        while ((i<Collection->Collection.count())&&(Collection->Collection[i].StyleName!=StyleName)) i++;
        if ((i<Collection->Collection.count())&&(Collection->Collection[i].StyleName==StyleName)) {
            Collection->Collection[i].StyleName=Collection->Collection[i].BckStyleName;
            Collection->Collection[i].StyleDef =Collection->Collection[i].BckStyleDef;
            Collection->Collection[i].FromUserConf=false;
            QString StyleName=Collection->Collection[i].StyleName;
            Collection->SortList();
            PopulateList(StyleName);
        }
    }
}
