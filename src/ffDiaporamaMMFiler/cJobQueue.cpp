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

#include <QApplication>
#include <QtDebug>

#include "cJobQueue.h"
#include <QCustomEvent>
#include <QPainter>

// Qualification of possible settings for each job
uint32_t PossibleJobsSettings[NBR_JOBTYPE] = {
     0                                                  // #define JOBTYPE_NOTDEFINED
    ,0                                                  // #define JOBTYPE_OPENFILE
    ,0                                                  // #define JOBTYPE_DISPLAYINFO
    ,0                                                  // #define JOBTYPE_REMOVEFILE
    ,JOBQUALIF_IMAGE_CONVERTIMAGE                       // #define JOBTYPE_IMAGE_CONVERT_JPG
};

#define ICON_GREEN      ":/img/Green.png"
#define ICON_RED        ":/img/Red.png"

//====================================================================================================================

cJob::cJob() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::cJob");

    // Job qualification & settings
    JobQualif        =0;
    JobSettings      =0;
    SourceSuffix     ="";
    SourceFolder     ="";
    DestinationSuffix="";
    DestinationFolder="";

    // Job controls
    JobType          =0;
    JobStatus        =0;
    PercentDone      =0;
    Succeded         =0;
    CurrentIndex     =0;

    // Job specific
    Command          ="";
}

//====================================================================================================================

cJob::~cJob() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::~cJob");
}

//====================================================================================================================

void cJob::SaveToXML(QDomElement &domDocument,QString ElementName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    // Job qualification & settings
    Element.setAttribute("JobQualif",JobQualif);
    Element.setAttribute("JobSettings",JobSettings);
    Element.setAttribute("SourceSuffix",SourceSuffix);
    Element.setAttribute("SourceFolder",SourceFolder);
    Element.setAttribute("DestinationSuffix",DestinationSuffix);
    Element.setAttribute("DestinationFolder",DestinationFolder);

    // Job controls
    Element.setAttribute("DateTime",DateTime.toString("dd/MM/yyy hh:mm:ss"));
    Element.setAttribute("JobType",JobType);
    Element.setAttribute("JobStatus",JobStatus);
    Element.setAttribute("CurrentIndex",CurrentIndex);
    Element.setAttribute("Succeded",Succeded);

    // Job specific
    Element.setAttribute("Command",Command);

    QString MixSourcesAndDests;
    for (int i=0;i<SourcesAndDests.count();i++) MixSourcesAndDests=MixSourcesAndDests+"##*##"+SourcesAndDests[i];
    Element.setAttribute("SourcesAndDests",MixSourcesAndDests);
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cJob::LoadFromXML(QDomElement domDocument,QString ElementName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        bool IsOk=true;

        // Job qualification & settings
        if (Element.hasAttribute("JobQualif"))          JobQualif           =Element.attribute("JobQualif").toInt();
        if (Element.hasAttribute("JobSettings"))        JobSettings         =Element.attribute("JobSettings").toInt();
        if (Element.hasAttribute("SourceSuffix"))       SourceSuffix        =Element.attribute("SourceSuffix");
        if (Element.hasAttribute("SourceFolder"))       SourceFolder        =Element.attribute("SourceFolder");
        if (Element.hasAttribute("DestinationSuffix"))  DestinationSuffix   =Element.attribute("DestinationSuffix");
        if (Element.hasAttribute("DestinationFolder"))  DestinationFolder   =Element.attribute("DestinationFolder");

        // Job controls
        if (Element.hasAttribute("DateTime"))           DateTime            =QDateTime().fromString(Element.attribute("DateTime"),"dd/MM/yyy hh:mm:ss");
        if (Element.hasAttribute("JobType"))            JobType             =Element.attribute("JobType").toInt();
        if (Element.hasAttribute("JobStatus"))          JobStatus           =Element.attribute("JobStatus").toInt();
        if (Element.hasAttribute("CurrentIndex"))       CurrentIndex        =Element.attribute("CurrentIndex").toInt();
        if (Element.hasAttribute("Succeded"))           Succeded            =Element.attribute("Succeded").toInt();

        // Job specific
        if (Element.hasAttribute("Command"))            Command             =Element.attribute("Command");

        QString MixSourcesAndDests;
        if (Element.hasAttribute("SourcesAndDests"))    MixSourcesAndDests=Element.attribute("SourcesAndDests",MixSourcesAndDests);
        SourcesAndDests=MixSourcesAndDests.split("##*##");

        return IsOk;
    }
    return false;
}

//====================================================================================================================

QString cJob::ComputeDestinationName(QString Source) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::ComputeDestinationName");

    QString SourceExt   ="."+QFileInfo(Source).suffix();
    QString ShortName   =QFileInfo(Source).fileName();          ShortName.replace(SourceExt,"");
    QString SourcePath  =QFileInfo(Source).absolutePath();

    QString DestSuffix  =((JobSettings&JOBQUALIF_DESTNAME_ADDSUFFIX)!=0)?DestinationSuffix+"."+DestinationExtension:"."+DestinationExtension;
    QString DestPath    =((JobSettings&JOBQUALIF_DESTPLACE_INFOLDER)==0)?SourcePath:DestinationFolder;
    QString DestName    =ShortName;

    if (!SourcePath.endsWith(QDir::separator())) SourcePath=SourcePath+QDir::separator();
    if (!DestPath.endsWith(QDir::separator()))   DestPath=DestPath+QDir::separator();

    return DestPath+DestName+DestSuffix;
}

//====================================================================================================================

QString cJob::ComputeNewSourceName(QString Source) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::ComputeNewSourceName");

    QString SourceExt       ="."+QFileInfo(Source).suffix();
    QString ShortName       =QFileInfo(Source).fileName();          ShortName.replace(SourceExt,"");
    QString SourcePath      =QFileInfo(Source).absolutePath();

    QString NewSourceSuffix  =((JobSettings&JOBQUALIF_SOURCE_ADDSUFFIX)!=0)?SourceSuffix+SourceExt:SourceExt;
    QString NewSourcePath    =((JobSettings&JOBQUALIF_SOURCE_MOVE)==0)?SourcePath:SourceFolder;
    QString NewSourceName    =ShortName;

    if (!SourcePath.endsWith(QDir::separator()))    SourcePath=SourcePath+QDir::separator();
    if (!NewSourcePath.endsWith(QDir::separator())) NewSourcePath=NewSourcePath+QDir::separator();

    return NewSourcePath+NewSourceName+NewSourceSuffix;
}

//====================================================================================================================

bool cJob::IsCommandListContain(QString ToFind) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::IsCommandListContain");

    bool        Ret=false;
    QStringList CommandList=Command.split("##");
    for (int i=0;i<CommandList.count();i++) if (CommandList[i].left(ToFind.length())==ToFind) Ret=true;
    return Ret;
}

//====================================================================================================================

QString cJob::CommandListValueString(QString ToFind) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::CommandListValue");

    QString Ret="";
    QStringList CommandList=Command.split("##");
    for (int i=0;i<CommandList.count();i++) if (CommandList[i].left(ToFind.length())==ToFind) Ret=CommandList[i].split(":")[1];
    return Ret;
}

//====================================================================================================================

int cJob::CommandListValue(QString ToFind) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::CommandListValue");

    int         Ret=0;
    QStringList CommandList=Command.split("##");
    for (int i=0;i<CommandList.count();i++) if (CommandList[i].left(ToFind.length())==ToFind) Ret=CommandList[i].split(":")[1].toInt();
    return Ret;
}

//*********************************************************************************************************************************************
// CJOBQUEUE
//*********************************************************************************************************************************************

cJobQueue::cJobQueue() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::cJobQueue");

    StatusText.append(QApplication::translate("QCustomJobTable","Waiting"));                            // JOBSTATUS_READYTOSTART
    StatusText.append(QApplication::translate("QCustomJobTable","Started"));                            // JOBSTATUS_STARTED
    StatusText.append(QApplication::translate("QCustomJobTable","Paused"));                             // JOBSTATUS_PAUSED
    StatusText.append(QApplication::translate("QCustomJobTable","Canceled"));                           // JOBSTATUS_CANCELED
    StatusText.append(QApplication::translate("QCustomJobTable","Succeeded"));                          // JOBSTATUS_SUCCEEDED
    StatusText.append(QApplication::translate("QCustomJobTable","Ended with error"));                   // JOBSTATUS_ENDEDWITHERROR

    JobTypeText.append(QApplication::translate("QCustomJobTable","Not defined"));                       // JOBTYPE_NOTDEFINED
    JobTypeText.append(QApplication::translate("QCustomJobTable","Open"));                              // JOBTYPE_OPENFILE
    JobTypeText.append(QApplication::translate("QCustomJobTable","Display information"));               // JOBTYPE_DISPLAYINFO
    JobTypeText.append(QApplication::translate("QCustomJobTable","Remove"));                            // JOBTYPE_REMOVEFILE
    JobTypeText.append(QApplication::translate("QCustomJobTable","Convert"));                           // JOBTYPE_IMAGE_CONVERTIMAGE
}

//====================================================================================================================

cJobQueue::~cJobQueue() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::~cJobQueue");

    while (List.count()>0) delete List.takeLast();
}

//====================================================================================================================

void cJobQueue::SaveToXML(QDomElement &domDocument,QString ElementName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    // Save job list
    Element.setAttribute("JobNumber",List.count());
    for (int i=0;i<List.count();i++) List[i]->SaveToXML(Element,"Composition-"+QString("%1").arg(i));
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cJobQueue::LoadFromXML(QDomElement domDocument,QString ElementName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        bool IsOk=true;
        // Load job list
        List.clear();
        int JobNumber=Element.attribute("JobNumber").toInt();
        for (int i=0;i<JobNumber;i++) {
            cJob *JobObject=new cJob();
            if (!JobObject->LoadFromXML(Element,"Job-"+QString("%1").arg(i))) {
                IsOk=false;
                delete JobObject;
            } else List.append(JobObject);
        }
        return IsOk;
    } else return false;
}

//====================================================================================================================

void cJobQueue::RefreshJobStatus(QWidget */*Window*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::RefreshJobStatus");
}

//====================================================================================================================

bool cJobQueue::ApplySourceTransformation(QString /*Source*/,QString /*NewSource*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::ApplySourceTransformation");

    ToLog(LOGMSG_CRITICAL,QString(QApplication::translate("cJobQueue","Applying source transformation : Sorry no yet developped !")));
    return true;
}

//====================================================================================================================

bool cJobQueue::ApplyDestinationOverWriting(QString Destination,uint32_t JobSettings) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::ApplyDestinationOverWriting");

    bool IsOkForWriting=true;
    if ((QFileInfo(Destination).exists())&&((JobSettings & JOBQUALIF_DESTINATION_OVERWRITE)==0)) {
        ToLog(LOGMSG_CRITICAL,QApplication::translate("cJobQueue","Destination file (%1) already exist").arg(Destination));
        IsOkForWriting=false;
    } else if ((QFileInfo(Destination).exists())&&((JobSettings & JOBQUALIF_DESTINATION_OVERWRITE)!=0)&&(!QFile(Destination).remove())) {
        ToLog(LOGMSG_CRITICAL,QApplication::translate("cJobQueue","Failed to remove existing destination file (%1)").arg(Destination));
        IsOkForWriting=false;
    }
    return IsOkForWriting;
}

//====================================================================================================================

void cJobQueue::ConvertIMG(cJob *Job) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::ConvertIMG");

    if (Job->JobType!=JOBTYPE_IMAGE_CONVERTIMAGE) {
        Job->JobStatus=JOBSTATUS_CANCELED;
        ToLog(LOGMSG_CRITICAL,QString(QApplication::translate("cJobQueue","Jobtype error")));
        return;
    }
    if ((Job->JobStatus!=JOBSTATUS_READYTOSTART)&&(Job->JobStatus!=JOBSTATUS_PAUSED)) return;

    Job->JobStatus=JOBSTATUS_STARTED;
    for (;Job->CurrentIndex<Job->SourcesAndDests.count();) {
        QImage  Img;
        QString Source      =Job->SourcesAndDests[Job->CurrentIndex];
        QString Destination =Job->ComputeDestinationName(Source);
        QString NewSource   =Job->ComputeNewSourceName(Source);
        bool    Rescal      =Job->IsCommandListContain("-S");
        int     Quality     =Job->IsCommandListContain("-Q")?Job->CommandListValue("-Q"):90;
        int     StepInJob   =4;      // Load+Rescal+Save+Modif Source


        ToLog(LOGMSG_INFORMATION,QApplication::translate("cJobQueue","Convert image")+" "+Source);
        Img.load(Source);
        Job->PercentDone=(double(Job->CurrentIndex*StepInJob+1)/double(Job->SourcesAndDests.count()*StepInJob))*100;
        PostEvent(EVENT_JobStatusChanged);

        // Step 1 : Load image
        if (Img.isNull()) {
            ToLog(LOGMSG_CRITICAL,QApplication::translate("cJobQueue","Failed to open file %1").arg(Source));
        }

        // Step 2 : Rescal
        if ((!Img.isNull())&&(Rescal)) {
            int     ImageWidth  =Img.width();
            int     ImageHeight =Img.height();
            int     NewWidth    =0;
            int     NewHeight   =0;
            QString Value       =Job->CommandListValueString("-S");

            if (Value.indexOf("MPix")!=-1) {
                double  iValue      =Value.left(Value.indexOf("MPix")).trimmed().toDouble()*1000*1000;
                double  ActualValue =ImageWidth*ImageHeight;
                // + or - 10%
                if (((iValue-iValue*0.1>ActualValue)&&(!Job->IsCommandListContain("-D")))||(iValue+iValue*0.1<ActualValue)) {
                    double Transfo=sqrt(iValue/ActualValue);
                    NewWidth =int(Transfo*double(ImageWidth));
                    NewHeight=int(Transfo*double(ImageHeight));
                }
            } else {
                int MaxRows=0;
                if      (Value.indexOf("QVGA")!=-1)     MaxRows=240;
                else if (Value.indexOf("HVGA")!=-1)     MaxRows=320;
                else if (Value.indexOf("WVGA")!=-1)     MaxRows=480;
                else if (Value.indexOf("DVD")!=-1)      MaxRows=576;
                else if (Value.indexOf("720p")!=-1)     MaxRows=720;
                else if (Value.indexOf("XGA")!=-1)      MaxRows=768;
                else if (Value.indexOf("1080p")!=-1)    MaxRows=1080;
                // + or - 10%
                if ((MaxRows!=0)&&(((MaxRows-MaxRows*0.1>ImageHeight)&&(!Job->IsCommandListContain("-D")))||(MaxRows+MaxRows*0.1<ImageHeight))) {
                    double AspectR=double(ImageWidth)/double(ImageHeight);
                    NewHeight=MaxRows;
                    NewWidth =int(double(MaxRows)*AspectR);
                }
            }
            if ((NewWidth!=0)&&(NewHeight!=0)) {
                Img=Img.scaledToWidth(NewWidth,Qt::SmoothTransformation);
                if (Img.isNull()) ToLog(LOGMSG_CRITICAL,QApplication::translate("cJobQueue","failed to rescal image %1 to %2x%3").arg(Source).arg(NewWidth).arg(NewHeight));
                    else ToLog(LOGMSG_INFORMATION,QApplication::translate("cJobQueue","succesfully rescal image %1 to %2x%3").arg(Source).arg(NewWidth).arg(NewHeight));
                PostEvent(EVENT_JobStatusChanged);
            }
        }

        // Step 3 : Ensure destination image could be write (depends on overwrite !)
        if ((!Img.isNull())&&(ApplyDestinationOverWriting(Destination,Job->JobSettings))) {

            // Step 4 : Save image
            if (!Img.save(Destination,Job->DestinationExtension.toUtf8().data(),Quality)) {
                ToLog(LOGMSG_CRITICAL,QApplication::translate("cJobQueue","Failed to write destination file %1").arg(Destination));
            } else {
                ToLog(LOGMSG_INFORMATION,QApplication::translate("cJobQueue","Successfully writing destination file %1").arg(Destination));
                PostEvent(EVENT_FileListChanged,QFileInfo(Destination).absolutePath());

                // Step 5 : Transform source file
                if (ApplySourceTransformation(Source,NewSource)) Job->Succeded++;
            }

        }
        // Go to next file
        Job->CurrentIndex++;
        Job->PercentDone=(double(Job->CurrentIndex*2)/double(Job->SourcesAndDests.count()*2))*100;
        if (Job->CurrentIndex<Job->SourcesAndDests.count()) PostEvent(EVENT_JobStatusChanged);
    }
    if (Job->Succeded==Job->SourcesAndDests.count()) Job->JobStatus=JOBSTATUS_SUCCEEDED; else Job->JobStatus=JOBSTATUS_ENDEDWITHERROR;
    PostEvent(EVENT_JobStatusChanged);
}
