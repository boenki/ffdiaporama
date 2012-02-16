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

#include "wgt_JobConvertImage.h"
#include "DlgJobSettings.h"
#include "ui_DlgJobSettings.h"

#include "../sharedfiles/QCustomGetFolder.h"
#include "cApplicationConfig.h"

//====================================================================================================================

DlgJobSettings::DlgJobSettings(cJob *TheJob,cJobQueue *TheJobQueue,QString HelpURL,cBaseApplicationConfig *ApplicationConfig,cSaveWindowPosition *DlgWSP,QWidget *parent)
    :QCustomDialog(HelpURL,ApplicationConfig,DlgWSP,parent),ui(new Ui::DlgJobSettings) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::DlgJobSettings");

    ui->setupUi(this);
    CancelBt=ui->CancelBt;
    OkBt    =ui->OkBt;
    HelpBt  =ui->HelpBT;

    Job         =TheJob;
    JobQueue    =TheJobQueue;
    wgt_Job     =NULL;
    JobSummary  ="";
}

//====================================================================================================================

DlgJobSettings::~DlgJobSettings() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::~DlgJobSettings");

    delete ui;
    delete wgt_Job;
}

//====================================================================================================================
// Initialise dialog

void DlgJobSettings::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::DoInitDialog");

    switch (Job->JobType) {
        case JOBTYPE_IMAGE_CONVERTIMAGE : wgt_Job=new wgt_JobConvertImage(this,ui->JobWidget);  break;
    }
    if (wgt_Job) {
        wgt_Job->Job=Job;
        ui->JobWidget->setFixedHeight(wgt_Job->height());
        connect(wgt_Job,SIGNAL(NeedRefreshControls()),this,SLOT(RefreshControls()));
    }

    connect(ui->Source_Nothing_RD,              SIGNAL(released()),this,SLOT(s_Source_Nothing_RD()));
    connect(ui->Source_Delete_RD,               SIGNAL(released()),this,SLOT(s_Source_Delete_RD()));
    connect(ui->Source_Rename_RD,               SIGNAL(released()),this,SLOT(s_Source_Rename_RD()));
    connect(ui->Source_Move_RD,                 SIGNAL(released()),this,SLOT(s_Source_Move_RD()));
    connect(ui->Destination_AddSuffix_RD,       SIGNAL(released()),this,SLOT(s_Destination_AddSuffix_RD()));
    connect(ui->Destination_InFolder_RD,        SIGNAL(released()),this,SLOT(s_Destination_InFolder_RD()));
    connect(ui->Source_Suffix_ED,               SIGNAL(textChanged(const QString)),this,SLOT(s_Source_Suffix_ED(const QString)));
    connect(ui->Destination_Suffix_ED,          SIGNAL(textChanged(const QString)),this,SLOT(s_Destination_Suffix_ED(const QString)));
    connect(ui->Destination_Overwrite_RD,       SIGNAL(released()),this,SLOT(s_Destination_Overwrite_RD()));
    connect(ui->Source_Folder_BT,               SIGNAL(released()),this,SLOT(s_Source_Folder_BT()));
    connect(ui->Destination_Folder_BT,          SIGNAL(released()),this,SLOT(s_Destination_Folder_BT()));

    if (wgt_Job) wgt_Job->DoInitDialog();
    RefreshControls();

}

//====================================================================================================================

void DlgJobSettings::RefreshControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::RefreshControls");

    bool DisplaySourcePart=(Job->JobQualif&JOBQUALIF_SOURCE_ALL)!=0;
    ui->Source_Nothing_RD->setChecked((Job->JobSettings&JOBQUALIF_SOURCE_KEEP)!=0);
    ui->Source_Delete_RD->setChecked((Job->JobSettings&JOBQUALIF_SOURCE_REMOVE)!=0);
    ui->Source_Rename_RD->setChecked((Job->JobSettings&JOBQUALIF_SOURCE_ADDSUFFIX)!=0);
    ui->Source_Suffix_ED->setText(Job->SourceSuffix);
    ui->Source_Move_RD->setChecked((Job->JobSettings&JOBQUALIF_SOURCE_MOVE)!=0);
    ui->Source_Folder_ED->setText(Job->SourceFolder);
    ui->Source_Label->setVisible(DisplaySourcePart);
    ui->Source_Nothing_RD->setVisible(DisplaySourcePart);       ui->Source_Nothing_RD->setEnabled((Job->JobQualif&JOBQUALIF_SOURCE_KEEP)!=0);
    ui->Source_Delete_RD->setVisible(DisplaySourcePart);        ui->Source_Delete_RD->setEnabled((Job->JobQualif&JOBQUALIF_SOURCE_REMOVE)!=0);
    ui->Source_Rename_RD->setVisible(DisplaySourcePart);        ui->Source_Rename_RD->setEnabled((Job->JobQualif&JOBQUALIF_SOURCE_ADDSUFFIX)!=0);
    ui->Source_Suffix_ED->setVisible(DisplaySourcePart);        ui->Source_Suffix_ED->setEnabled(ui->Source_Rename_RD->isChecked()&&((Job->JobQualif&JOBQUALIF_SOURCE_ADDSUFFIX)!=0));
    ui->Source_Move_RD->setVisible(DisplaySourcePart);          ui->Source_Move_RD->setEnabled((Job->JobQualif&JOBQUALIF_SOURCE_MOVE)!=0);
    ui->Source_Folder_ED->setVisible(DisplaySourcePart);        ui->Source_Folder_ED->setEnabled(ui->Source_Move_RD->isChecked()&&((Job->JobQualif&JOBQUALIF_SOURCE_MOVE)!=0));
    ui->Source_Folder_BT->setVisible(DisplaySourcePart);        ui->Source_Folder_BT->setEnabled(ui->Source_Move_RD->isChecked()&&((Job->JobQualif&JOBQUALIF_SOURCE_MOVE)!=0));
    ui->Source_Folder_Spacer->setVisible(DisplaySourcePart);

    bool DisplayDestinationNamePart=(Job->JobQualif&JOBQUALIF_DESTNAME_ADDSUFFIX)!=0;
    bool DisplayDestinationPlacePart=(Job->JobQualif&JOBQUALIF_DESTPLACE_INFOLDER)!=0;
    bool DisplayOverWrite=(Job->JobQualif&JOBQUALIF_DESTINATION_OVERWRITE)!=0;
    ui->DestinationName_Label->setVisible(DisplayDestinationNamePart||DisplayDestinationPlacePart||DisplayOverWrite);
    ui->Destination_AddSuffix_RD->setChecked((Job->JobSettings&JOBQUALIF_DESTNAME_ADDSUFFIX)!=0);
    ui->Destination_Suffix_ED->setText(Job->DestinationSuffix);
    ui->Destination_AddSuffix_RD->setVisible(DisplayDestinationNamePart);       ui->Destination_AddSuffix_RD->setEnabled((Job->JobQualif&JOBQUALIF_DESTNAME_ADDSUFFIX)!=0);
    ui->Destination_Suffix_ED->setVisible(DisplayDestinationNamePart);          ui->Destination_Suffix_ED->setEnabled(ui->Destination_AddSuffix_RD->isChecked()&&DisplayDestinationNamePart);
    ui->Destination_InFolder_RD->setChecked((Job->JobSettings&JOBQUALIF_DESTPLACE_INFOLDER)!=0);
    ui->Destination_Folder_ED->setText(Job->DestinationFolder);
    ui->DestinationPlace_Label_spacer0->setVisible(DisplayDestinationPlacePart);
    ui->Destination_InFolder_RD->setVisible(DisplayDestinationPlacePart);       ui->Destination_InFolder_RD->setEnabled((Job->JobQualif&JOBQUALIF_DESTPLACE_INFOLDER)!=0);
    ui->Destination_Folder_ED->setVisible(DisplayDestinationPlacePart);         ui->Destination_Folder_ED->setEnabled(ui->Destination_InFolder_RD->isChecked()&&DisplayDestinationPlacePart);
    ui->Destination_Folder_BT->setVisible(DisplayDestinationPlacePart);         ui->Destination_Folder_BT->setEnabled(ui->Destination_InFolder_RD->isChecked()&&DisplayDestinationPlacePart);
    ui->Destination_Folder_Spacer->setVisible(DisplayDestinationPlacePart);
    ui->Destination_Overwrite_RD->setVisible(DisplayOverWrite);                 ui->Destination_Overwrite_RD->setChecked((Job->JobSettings&JOBQUALIF_DESTINATION_OVERWRITE)!=0);

/*
    ui->Thumbnails_Label
    ui->Thumbnails_Label_spacer0
    ui->Thumbnails_Ignore_RD
    ui->Thumbnails_Copy_RD
*/

    if (wgt_Job) wgt_Job->RefreshControls();

    RefreshJobSummary();
    ui->JobSummary_ED->setText(JobSummary);
}

//====================================================================================================================

void DlgJobSettings::s_Source_Nothing_RD() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Source_Nothing_RD");

    Job->JobSettings=(Job->JobSettings & ~JOBQUALIF_SOURCE_ALL)|JOBQUALIF_SOURCE_KEEP;
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Source_Delete_RD() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Source_Delete_RD");

    Job->JobSettings=(Job->JobSettings & ~JOBQUALIF_SOURCE_ALL)|JOBQUALIF_SOURCE_REMOVE;
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Source_Rename_RD() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Source_Rename_RD");

    Job->JobSettings=(Job->JobSettings & ~JOBQUALIF_SOURCE_ALL)|JOBQUALIF_SOURCE_ADDSUFFIX;
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Source_Move_RD() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Source_Move_RD");

    Job->JobSettings=(Job->JobSettings & ~JOBQUALIF_SOURCE_ALL)|JOBQUALIF_SOURCE_MOVE;
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Destination_AddSuffix_RD() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Destination_AddSuffix_RD");

    if (ui->Destination_AddSuffix_RD->isChecked()) Job->JobSettings=(Job->JobSettings|JOBQUALIF_DESTNAME_ADDSUFFIX);
        else Job->JobSettings=(Job->JobSettings & ~JOBQUALIF_DESTNAME_ADDSUFFIX);
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Destination_InFolder_RD() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Destination_InFolder_RD");

    if (ui->Destination_InFolder_RD->isChecked()) Job->JobSettings=(Job->JobSettings|JOBQUALIF_DESTPLACE_INFOLDER);
        else Job->JobSettings=(Job->JobSettings & ~JOBQUALIF_DESTPLACE_INFOLDER);
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Destination_Overwrite_RD() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Destination_Overwrite_RD");

    if (ui->Destination_Overwrite_RD->isChecked()) Job->JobSettings=(Job->JobSettings|JOBQUALIF_DESTINATION_OVERWRITE);
        else Job->JobSettings=(Job->JobSettings & ~JOBQUALIF_DESTINATION_OVERWRITE);
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Source_Suffix_ED(const QString Suffix) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Destination_Overwrite_RD");

    Job->SourceSuffix=Suffix;
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Destination_Suffix_ED(const QString Suffix) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Destination_Overwrite_RD");

    Job->DestinationSuffix=Suffix;
    RefreshControls();
}

//====================================================================================================================

void DlgJobSettings::s_Source_Folder_BT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Source_Folder_BT");

    QString NewPath=GetDestinationFolder(Job->SourceFolder,"",BaseApplicationConfig,((cApplicationConfig *)BaseApplicationConfig)->DlgGetFolderWSP,this);
    if (NewPath!="") {
        Job->SourceFolder=NewPath;
        RefreshControls();
    }
}

//====================================================================================================================

void DlgJobSettings::s_Destination_Folder_BT() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::s_Destination_Folder_BT");

    QString NewPath=GetDestinationFolder(Job->DestinationFolder,"",BaseApplicationConfig,((cApplicationConfig *)BaseApplicationConfig)->DlgGetFolderWSP,this);
    if (NewPath!="") {
        Job->DestinationFolder=NewPath;
        RefreshControls();
    }
}

//====================================================================================================================

void DlgJobSettings::RefreshJobSummary() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:DlgJobSettings::RefreshJobSummary");

    JobSummary  ="";

    for (int i=0;i<Job->SourcesAndDests.count();i++) {
        QString Source      =Job->SourcesAndDests[i];
        QString Destination =Job->ComputeDestinationName(Source);
        QString NewSource   =Job->ComputeNewSourceName(Source);

        if (JobSummary!="") JobSummary=JobSummary+"\n";

        JobSummary=JobSummary+JobQueue->JobTypeText[Job->JobType]+" ("+QFileInfo(Source).fileName()+" => "+(QFileInfo(Source).absolutePath()==QFileInfo(Destination).absolutePath()?QFileInfo(Destination).fileName():Destination)+")";
        if ((Job->JobSettings&JOBQUALIF_DESTINATION_OVERWRITE)!=0)  JobSummary=JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","overwrite destination file as needed"));
        if ((Job->JobSettings&JOBQUALIF_SOURCE_REMOVE)!=0)          JobSummary=JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and remove %1")).arg(Source);
        else if ((Job->JobSettings&JOBQUALIF_SOURCE_ADDSUFFIX)!=0)  JobSummary=JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and rename %1 to %2")).arg(Source).arg(NewSource);
        else if ((Job->JobSettings&JOBQUALIF_SOURCE_MOVE)!=0)       JobSummary=JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and move %1 to %2")).arg(Source).arg(NewSource);
        if (wgt_Job) wgt_Job->AppendJobSummary(i,&JobSummary,JobQueue);
    }
}
