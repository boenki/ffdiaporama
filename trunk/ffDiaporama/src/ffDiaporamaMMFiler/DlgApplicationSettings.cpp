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

#include <QMessageBox>
#include "../sharedfiles/DlgCheckConfig.h"
#include "../sharedfiles/DlgManageDevices.h"

#include "DlgApplicationSettings.h"
#include "ui_DlgApplicationSettings.h"

//====================================================================================================================

DlgApplicationSettings::DlgApplicationSettings(QString HelpURL,cApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgApplicationSettings) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgApplicationSettings::DlgApplicationSettings");

    this->ApplicationConfig=ApplicationConfig;
    this->HelpURLChkConfig =HelpURLChkConfig;

    ui->setupUi(this);
    CancelBt=ui->CancelBt;
    OkBt    =ui->OkBt;
    HelpBt  =ui->HelpBT;

    IsDeviceChanged=false;
}

//====================================================================================================================

DlgApplicationSettings::~DlgApplicationSettings() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgApplicationSettings::~DlgApplicationSettings");

    delete ui;
}

//====================================================================================================================
// Initialise dialog

void DlgApplicationSettings::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgApplicationSettings::DoInitDialog");

    ui->RememberLastDirectoriesCH->setChecked(ApplicationConfig->RememberLastDirectories);
    ui->RestoreWindowCH->setChecked(ApplicationConfig->RestoreWindow);
    ui->HiddenCB->setChecked(ApplicationConfig->ShowHiddenFilesAndDir);
    ui->FoldersFirstCB->setChecked(ApplicationConfig->ShowFoldersFirst);
    ui->Crop1088To1080CB->setChecked(ApplicationConfig->Crop1088To1080);
    #ifdef Q_OS_WIN
        ui->RasterModeCB->setVisible(false);
        ui->OnlyMediaDrivesCB->setVisible(false);
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        ui->RasterModeCB->setChecked(ApplicationConfig->RasterMode);
        ui->OnlyMediaDrivesCB->setChecked(!ApplicationConfig->ShowMntDrive);
    #endif
    ui->FileNameIconViewCB->setChecked(ApplicationConfig->DisplayFileName);
    ui->MiniHeightEXIFCB->setCurrentIndex(ApplicationConfig->MinimumEXIFHeight/100);

    QString ThumbSize;
    ThumbSize=QString("%1x%2").arg(ApplicationConfig->Image_ThumbWidth).arg(ApplicationConfig->Image_ThumbHeight); ui->ThumbSImageCB->setCurrentIndex(ui->ThumbSImageCB->findText(ThumbSize));
    ThumbSize=QString("%1x%2").arg(ApplicationConfig->Music_ThumbWidth).arg(ApplicationConfig->Music_ThumbHeight); ui->ThumbSMusicCB->setCurrentIndex(ui->ThumbSMusicCB->findText(ThumbSize));
    ThumbSize=QString("%1x%2").arg(ApplicationConfig->Video_ThumbWidth).arg(ApplicationConfig->Video_ThumbHeight); ui->ThumbSVideoCB->setCurrentIndex(ui->ThumbSVideoCB->findText(ThumbSize));

    connect(ui->CheckConfigBT,SIGNAL(clicked()),this,SLOT(s_CheckConfig()));
    connect(ui->DBManageDevicesBT,SIGNAL(clicked()),this,SLOT(s_ManageDevices()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(TabChanged(int)));

    ui->tabWidget->setCurrentIndex(0);
}

//====================================================================================================================
// Call when user click on Ok button

void DlgApplicationSettings::DoAccept() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgApplicationSettings::DoAccept");

    ApplicationConfig->RememberLastDirectories  =ui->RememberLastDirectoriesCH->isChecked();
    ApplicationConfig->RestoreWindow            =ui->RestoreWindowCH->isChecked();
    ApplicationConfig->ShowHiddenFilesAndDir    =ui->HiddenCB->isChecked();
    ApplicationConfig->ShowFoldersFirst         =ui->FoldersFirstCB->isChecked();
    ApplicationConfig->Crop1088To1080           =ui->Crop1088To1080CB->isChecked();
    ApplicationConfig->DisplayFileName          =ui->FileNameIconViewCB->isChecked();
    ApplicationConfig->MinimumEXIFHeight        =ui->MiniHeightEXIFCB->currentIndex()*100;

    QString ThumbSize;
    ThumbSize=ui->ThumbSImageCB->currentText();   ApplicationConfig->Image_ThumbWidth=ThumbSize.left(ThumbSize.indexOf("x")).toInt();    ApplicationConfig->Image_ThumbHeight=ThumbSize.mid(ThumbSize.indexOf("x")+1).toInt();
    ThumbSize=ui->ThumbSMusicCB->currentText();   ApplicationConfig->Music_ThumbWidth=ThumbSize.left(ThumbSize.indexOf("x")).toInt();    ApplicationConfig->Music_ThumbHeight=ThumbSize.mid(ThumbSize.indexOf("x")+1).toInt();
    ThumbSize=ui->ThumbSVideoCB->currentText();   ApplicationConfig->Video_ThumbWidth=ThumbSize.left(ThumbSize.indexOf("x")).toInt();    ApplicationConfig->Video_ThumbHeight=ThumbSize.mid(ThumbSize.indexOf("x")+1).toInt();

    #ifdef Q_OS_LINUX
        ApplicationConfig->RasterMode           =ui->RasterModeCB->isChecked();
        ApplicationConfig->ShowMntDrive         =!ui->OnlyMediaDrivesCB->isChecked();
    #endif
}

//====================================================================================================================

void DlgApplicationSettings::s_CheckConfig() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgApplicationSettings::s_CheckConfig");
    DlgCheckConfig Dlg(HELPFILE_DlgCheckConfig,ApplicationConfig,ApplicationConfig->DlgCheckConfigWSP,this);
    Dlg.InitDialog();
    Dlg.exec();
}

//====================================================================================================================

void DlgApplicationSettings::s_ManageDevices() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgApplicationSettings::s_ManageDevices");
    DlgManageDevices Dlg(&ApplicationConfig->DeviceModelList,HELPFILE_DlgManageDevices,ApplicationConfig,ApplicationConfig->DlgManageDevicesWSP,this);
    Dlg.InitDialog();
    Dlg.exec();
}

//====================================================================================================================

void DlgApplicationSettings::TabChanged(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgApplicationSettings::TabChanged");
}
