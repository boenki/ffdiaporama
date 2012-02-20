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

#include "wgt_JobConvertAudio.h"
#include "ui_wgt_JobConvertAudio.h"
#include "../sharedfiles/cBaseMediaFile.h"
#include "../sharedfiles/cDeviceModelDef.h"

//====================================================================================================================

wgt_JobConvertAudio::wgt_JobConvertAudio(QCustomDialog *Dialog,QWidget *parent):wgt_JobBase(Dialog,parent),ui(new Ui::wgt_JobConvertAudio) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::DoInitDialog");

    StopRefreshCB=false;
    ui->setupUi(this);
}

//====================================================================================================================

wgt_JobConvertAudio::~wgt_JobConvertAudio() {
    delete ui;
}

//====================================================================================================================

void wgt_JobConvertAudio::DoInitDialog() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::DoInitDialog");

    // Start init by mono and stereo CB
    ui->ForceToMonoCB->setChecked(Job->IsCommandListContain("-C1"));
    ui->ForceToStereoCB->setChecked(Job->IsCommandListContain("-C2"));

    // Init format container combo
    for (int i=0;i<NBR_AUDIOFORMATDEF;i++) if (AUDIOFORMATDEF[i].IsFind) ui->AudioFormatCB->addItem(AUDIOFORMATDEF[i].LongName,QVariant(i));
    connect(ui->AudioFormatCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_AudioFormatCB(int)));
    connect(ui->AudioBitRateCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_AudioBitRateCB(int)));
    connect(ui->ChAudioFreqCB,SIGNAL(released()),this,SLOT(s_ChAudioFreqCB()));
    connect(ui->AudioFreqCB,SIGNAL(currentIndexChanged(int)),this,SLOT(s_AudioFreqCB(int)));
    connect(ui->ForceToMonoCB,SIGNAL(released()),this,SLOT(s_ForceToMonoCB()));
    connect(ui->ForceToStereoCB,SIGNAL(released()),this,SLOT(s_ForceToStereoCB()));
}

//====================================================================================================================

void wgt_JobConvertAudio::RefreshControls() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::RefreshControls");

    bool IsFind=false;
    for (int i=0;i<NBR_AUDIOFORMATDEF;i++) if (Job->DestinationExtension==AUDIOFORMATDEF[i].FileExtension) {
        ui->AudioFormatCB->setCurrentIndex(ui->AudioFormatCB->findText(AUDIOFORMATDEF[i].LongName));
        IsFind=true;
    }
    if (!IsFind) ui->AudioFormatCB->setCurrentIndex(0);

    int CurrentCodec=ui->AudioFormatCB->currentIndex();
    if (CurrentCodec>=0) {
        bool        IsFind=false;
        Job->DestinationExtension=AUDIOFORMATDEF[ui->AudioFormatCB->itemData(CurrentCodec).toInt()].FileExtension;
        QStringList AudioCodecs=QString(AUDIOFORMATDEF[ui->AudioFormatCB->itemData(CurrentCodec).toInt()].PossibleAudioCodec).split("#");
        CurrentCodec=0;
        while ((!IsFind)&&(CurrentCodec<NBR_AUDIOCODECDEF)&&(!AudioCodecs.contains(AUDIOCODECDEF[CurrentCodec].ShortName))) CurrentCodec++;
    }

    if (CurrentCodec<NBR_AUDIOCODECDEF) {

        // If this format need bitrate but no bitrate is in command list then add default
        if ((ui->AudioBitRateCB->isEnabled())&&(!Job->IsCommandListContain("-AB"))) {
            Job->Command=(Job->Command!=""?Job->Command+"##":"")+"-AB:"+AUDIOCODECDEF[CurrentCodec].Default;
        }

        // Try to set bitrate
        if ((ui->AudioBitRateCB->isEnabled())&&(Job->IsCommandListContain("-AB"))) {
            ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->findText(Job->CommandListValueString("-AB")));
            //if failed, switch to default value
            if (ui->AudioBitRateCB->currentIndex()<0) ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->findText(AUDIOCODECDEF[CurrentCodec].Default));

        // If this format not allow bitrate but a bitrate is in command list then remove it
        } else if ((!ui->AudioBitRateCB->isEnabled())&&(Job->IsCommandListContain("-AB"))) {
            QStringList CommandList=Job->Command.split("##");
            Job->Command="";
            int i=0;
            while (i<CommandList.count()) if (CommandList[i].startsWith("-AB")) CommandList.removeAt(i); else i++;
            for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
        }

        // Set frequency (if selected)
        if (Job->IsCommandListContain("-AR")) {
            ui->ChAudioFreqCB->setChecked(true);
            ui->AudioFreqCB->setEnabled(true);
            ui->AudioFreqCB->setCurrentIndex(ui->AudioFreqCB->findText(Job->CommandListValueString("-AR")));
        } else {
            ui->ChAudioFreqCB->setChecked(false);
            ui->AudioFreqCB->setEnabled(false);
        }
    }
}

//====================================================================================================================

void wgt_JobConvertAudio::AppendJobSummary(int index,QString *JobSummary,cJobQueue *JobQueue) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::AppendJobSummary");

    QString         SourceFile=Job->SourcesAndDests[index];
    cBaseMediaFile  *MediaFile=NULL;

    // Search MediaFile in list
    for (int i=0;i<JobQueue->MediaList.count();i++) if (((cBaseMediaFile *)JobQueue->MediaList.at(i))->FileName==SourceFile) {
        MediaFile=JobQueue->MediaList.at(i);
        break;
    }

    if ((MediaFile!=NULL)&&(Job->IsCommandListContain("-ACO")))  {
        *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set codec to %1")).arg(AUDIOCODECDEF[Job->CommandListValue("-ACO")].ShortName);
        if (Job->IsCommandListContain("-AB"))   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and set audio compression to %1")).arg(Job->CommandListValueString("-AB"));
        if (Job->IsCommandListContain("-AR"))   *JobSummary=*JobSummary+"\n    "+QString(QApplication::translate("QCustomJobTable","and force audio frequency to %1")).arg(Job->CommandListValueString("-AR").left(Job->CommandListValueString("-AR").indexOf(" ")));
        if (Job->IsCommandListContain("-ACH"))  *JobSummary=*JobSummary+"\n    "+(Job->CommandListValue("-ACH")==1?QString(QApplication::translate("QCustomJobTable","and force to mono")):QString(QApplication::translate("QCustomJobTable","and force to stereo")));
        *JobSummary=*JobSummary+"\n    "+JobQueue->ComputeFFMPEGCommand(Job);
    }
}

//====================================================================================================================

void wgt_JobConvertAudio::s_AudioFormatCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::s_AudioFormatCB");

    StopRefreshCB=true;

    // Prepare CommandList
    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-ACO")) CommandList.removeAt(i); else i++;

    // Fill combo box
    ui->AudioBitRateCB->clear();
    int CurrentCodec=ui->AudioFormatCB->currentIndex();
    if (CurrentCodec>=0) {
        bool        IsFind=false;
        Job->DestinationExtension=AUDIOFORMATDEF[ui->AudioFormatCB->itemData(CurrentCodec).toInt()].FileExtension;
        QStringList AudioCodecs=QString(AUDIOFORMATDEF[ui->AudioFormatCB->itemData(CurrentCodec).toInt()].PossibleAudioCodec).split("#");
        CurrentCodec=0;
        while ((!IsFind)&&(CurrentCodec<NBR_AUDIOCODECDEF)&&(!AudioCodecs.contains(AUDIOCODECDEF[CurrentCodec].ShortName))) CurrentCodec++;
    }
    if (CurrentCodec<NBR_AUDIOCODECDEF) {
        CommandList.append(QString("-ACO:%1").arg(CurrentCodec));

        QString     AllowedBitRate;
        QString     BitRate="";
        int         Index=0;
        bool        IsFindBitRate=false;
        AllowedBitRate=AUDIOCODECDEF[CurrentCodec].PossibleBitrate2CH;
        BitRate="";
        Index=0;
        IsFindBitRate=false;
        QStringList List;
        while (AllowedBitRate.length()>0) {
            Index=AllowedBitRate.indexOf("#");
            if (Index>0) {
                BitRate=AllowedBitRate.left(Index);
                AllowedBitRate=AllowedBitRate.right(AllowedBitRate.length()-Index-1);
            } else {
                BitRate=AllowedBitRate;
                AllowedBitRate="";
            }
            List.append(BitRate);
        }
        for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) {
            QString NameA=List[j];      if (NameA.endsWith("k")) NameA=NameA.left(NameA.length()-1);
            int     NumA=NameA.toInt();
            QString NameB=List[j+1];    if (NameB.endsWith("k")) NameB=NameB.left(NameB.length()-1);
            int     NumB=NameB.toInt();
            if (NumA>NumB) List.swap(j,j+1);
        }
        for (int i=0;i<List.count();i++) ui->AudioBitRateCB->addItem(List[i]);
        if (!IsFindBitRate) ui->AudioBitRateCB->setCurrentIndex(ui->AudioBitRateCB->findText(AUDIOCODECDEF[CurrentCodec].Default));
        ui->AudioBitRateCB->setEnabled(ui->AudioBitRateCB->count()>1);
    } else ui->AudioBitRateCB->setEnabled(false);
    StopRefreshCB=false;
    // Construct CommandList
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_JobConvertAudio::s_AudioBitRateCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::s_AudioBitRateCB");

    if (StopRefreshCB) return;
    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-AB")) CommandList.removeAt(i); else i++;
    if (ui->AudioBitRateCB->isEnabled()) CommandList.append(QString("-AB:%1").arg(ui->AudioBitRateCB->currentText()));
    // Construct CommandList
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_JobConvertAudio::s_AudioFreqCB(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::s_AudioFreqCB");

    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-AR")) CommandList.removeAt(i); else i++;
    if (ui->AudioFreqCB->isEnabled()) CommandList.append(QString("-AR:%1").arg(ui->AudioFreqCB->currentText()));
    // Construct CommandList
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_JobConvertAudio::s_ChAudioFreqCB() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::s_ChAudioFreqCB");

    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-AR")) CommandList.removeAt(i); else i++;
    if (ui->ChAudioFreqCB->isChecked()) CommandList.append(QString("-AR:%1").arg(ui->AudioFreqCB->currentText()));
    // Construct CommandList
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_JobConvertAudio::s_ForceToMonoCB() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::s_ForceToMonoCB");

    if (StopRefreshCB) return;
    StopRefreshCB=true;
    if (ui->ForceToStereoCB->isChecked()) ui->ForceToStereoCB->setChecked(false);
    StopRefreshCB=false;

    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-ACH")) CommandList.removeAt(i); else i++;
    if (ui->ForceToMonoCB->isChecked()) CommandList.append(QString("-ACH:1"));
    // Construct CommandList
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}

//====================================================================================================================

void wgt_JobConvertAudio::s_ForceToStereoCB() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:wgt_JobConvertAudio::s_ForceToStereoCB");

    if (StopRefreshCB) return;
    StopRefreshCB=true;
    if (ui->ForceToMonoCB->isChecked()) ui->ForceToMonoCB->setChecked(false);
    StopRefreshCB=false;

    int         i=0;
    QStringList CommandList=Job->Command.split("##");
    Job->Command="";
    while (i<CommandList.count()) if (CommandList[i].startsWith("-ACH")) CommandList.removeAt(i); else i++;
    if (ui->ForceToStereoCB->isChecked()) CommandList.append(QString("-ACH:2"));
    // Construct CommandList
    for (int i=0;i<CommandList.count();i++) Job->Command=(Job->Command!=""?Job->Command+"##":"")+CommandList[i];
    emit NeedRefreshControls();
}
