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

//#define DEBUGMODE

//====================================================================================================================

cJob::cJob() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJob::cJob";
    #endif
    JobType     =0;
    JobStatus   =0;
    PercentDone =0;
    Command     ="";
    Result      ="";
    CurrentIndex=0;
    Overwrite   =false;
    Succeded    =0;
}

//====================================================================================================================

cJob::~cJob() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJob::~cJob";
    #endif

}

//====================================================================================================================

void cJob::SaveToXML(QDomElement &domDocument,QString ElementName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJob::SaveToXML";
    #endif
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    Element.setAttribute("DateTime",DateTime.toString("dd/MM/yyy hh:mm:ss"));
    Element.setAttribute("JobType",JobType);
    Element.setAttribute("JobStatus",JobStatus);
    Element.setAttribute("Command",Command);
    Element.setAttribute("Result",Result);
    Element.setAttribute("CurrentIndex",CurrentIndex);
    Element.setAttribute("Succeded",Succeded);
    Element.setAttribute("Overwrite",Overwrite?"1":"0");

    QString MixSourcesAndDests;
    for (int i=0;i<SourcesAndDests.count();i++) MixSourcesAndDests=MixSourcesAndDests+"##*##"+SourcesAndDests[i];
    Element.setAttribute("SourcesAndDests",MixSourcesAndDests);
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cJob::LoadFromXML(QDomElement domDocument,QString ElementName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJob::LoadFromXML";
    #endif
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        bool IsOk=true;

        if (Element.hasAttribute("DateTime"))           DateTime    =QDateTime().fromString(Element.attribute("DateTime"),"dd/MM/yyy hh:mm:ss");
        if (Element.hasAttribute("JobType"))            JobType     =Element.attribute("JobType").toInt();
        if (Element.hasAttribute("JobStatus"))          JobStatus   =Element.attribute("JobStatus").toInt();
        if (Element.hasAttribute("Command"))            Command     =Element.attribute("Command");
        if (Element.hasAttribute("Result"))             Result      =Element.attribute("Result");
        if (Element.hasAttribute("CurrentIndex"))       CurrentIndex=Element.attribute("CurrentIndex").toInt();
        if (Element.hasAttribute("Succeded"))           Succeded    =Element.attribute("Succeded").toInt();
        if (Element.hasAttribute("Overwrite"))          Overwrite   =Element.attribute("Overwrite")=="1";

        QString MixSourcesAndDests;
        if (Element.hasAttribute("SourcesAndDests"))    MixSourcesAndDests=Element.attribute("SourcesAndDests",MixSourcesAndDests);
        SourcesAndDests=MixSourcesAndDests.split("##*##");

        return IsOk;
    }
    return false;
}

//*********************************************************************************************************************************************

cJobQueue::cJobQueue() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJobQueue::cJobQueue";
    #endif

    StatusText.append(QApplication::translate("QCustomJobTable","Waiting"));                            // JOBSTATUS_READYTOSTART
    StatusText.append(QApplication::translate("QCustomJobTable","Started"));                            // JOBSTATUS_STARTED
    StatusText.append(QApplication::translate("QCustomJobTable","Paused"));                             // JOBSTATUS_PAUSED
    StatusText.append(QApplication::translate("QCustomJobTable","Canceled"));                           // JOBSTATUS_CANCELED
    StatusText.append(QApplication::translate("QCustomJobTable","Succeeded"));                          // JOBSTATUS_SUCCEEDED
    StatusText.append(QApplication::translate("QCustomJobTable","Ended with error"));                   // JOBSTATUS_ENDEDWITHERROR

    JobTypeText.append(QApplication::translate("QCustomJobTable","Not defined"));                       // JOBTYPE_NOTDEFINED
    JobTypeText.append(QApplication::translate("QCustomJobTable","Open media file"));                   // JOBTYPE_OPENFILE
    JobTypeText.append(QApplication::translate("QCustomJobTable","Display information on file"));       // JOBTYPE_DISPLAYINFO
    JobTypeText.append(QApplication::translate("QCustomJobTable","Convert image to jpg"));              // JOBTYPE_IMAGE_CONVERT_JPG
    JobTypeText.append(QApplication::translate("QCustomJobTable","Convert multiple images to jpg"));    // JOBTYPE_IMAGE_CONVERT_MULTJPG
    JobTypeText.append(QApplication::translate("QCustomJobTable","Convert image to jpg"));              // JOBTYPE_IMAGE_CONVERT_PNG
    JobTypeText.append(QApplication::translate("QCustomJobTable","Convert multiple images to png"));    // JOBTYPE_IMAGE_CONVERT_MULTPNG
}

//====================================================================================================================

cJobQueue::~cJobQueue() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJobQueue::~cJobQueue";
    #endif
    while (List.count()>0) delete List.takeLast();
}

//====================================================================================================================

void cJobQueue::SaveToXML(QDomElement &domDocument,QString ElementName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJobQueue::SaveToXML";
    #endif
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    // Save job list
    Element.setAttribute("JobNumber",List.count());
    for (int i=0;i<List.count();i++) List[i]->SaveToXML(Element,"Composition-"+QString("%1").arg(i));
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cJobQueue::LoadFromXML(QDomElement domDocument,QString ElementName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJobQueue::LoadFromXML";
    #endif
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

void cJobQueue::RefreshJobStatus(QWidget *Window) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJobQueue::RefreshJobStatus";
    #endif

}

//====================================================================================================================

void cJobQueue::ConvertIMG(cJob *Job,QWidget *Window) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJobQueue::ConvertIMG";
    #endif

    if ((Job->JobType!=JOBTYPE_IMAGE_CONVERT_JPG)&&(Job->JobType!=JOBTYPE_IMAGE_CONVERT_MULTJPG)&&
        (Job->JobType!=JOBTYPE_IMAGE_CONVERT_PNG)&&(Job->JobType!=JOBTYPE_IMAGE_CONVERT_MULTPNG)) {
        Job->JobStatus=JOBSTATUS_CANCELED;
        Job->Result=QApplication::translate("cJobQueue","Jobtype error");
        return;
    }
    if ((Job->JobStatus!=JOBSTATUS_READYTOSTART)&&(Job->JobStatus!=JOBSTATUS_PAUSED)) return;

    Job->JobStatus=JOBSTATUS_STARTED;
    for (;Job->CurrentIndex<Job->SourcesAndDests.count();) {
        QImage  Img;
        QString Source      =Job->SourcesAndDests[Job->CurrentIndex].split("##-##")[0];
        QString Destination =Job->SourcesAndDests[Job->CurrentIndex].split("##-##")[1];
        Img.load(Source);
        Job->PercentDone=(double(Job->CurrentIndex*2+1)/double(Job->SourcesAndDests.count()*2))*100;
        QApplication::postEvent(Window,new QEvent(JobStatusChanged));

        if (Img.isNull()) {
            Job->Result=Job->Result+"\n"+QApplication::translate("cJobQueue","Failed to open source file")+" "+Source;
        } else {
            if ((QFileInfo(Destination).exists())&&(!Job->Overwrite)) {
                Job->Result=Job->Result+"\n"+QApplication::translate("cJobQueue","Destination file (%1) already exist").arg(Destination);
            } else if ((QFileInfo(Destination).exists())&&(Job->Overwrite)&&(!QFile(Destination).remove())) {
                Job->Result=Job->Result+"\n"+QApplication::translate("cJobQueue","Failed to overwrite destination file (%1)").arg(Destination);
            } else if (!Img.save(Destination,Job->Command.toUtf8().data(),-1)) {
                Job->Result=Job->Result+"\n"+QApplication::translate("cJobQueue","Failed to write destination file (%1)").arg(Destination);
            } else {
                Job->Result=Job->Result+"\n"+QString(QApplication::translate("cJobQueue","Successfully convert %1 to %2")).arg(Source).arg(Destination);
                QApplication::postEvent(Window,new QEvent(FileListChanged));
                Job->Succeded++;
            }
        }
        Job->CurrentIndex++;
        Job->PercentDone=(double(Job->CurrentIndex*2)/double(Job->SourcesAndDests.count()*2))*100;
        if (Job->CurrentIndex<Job->SourcesAndDests.count()) QApplication::postEvent(Window,new QEvent(JobStatusChanged));
    }
    if (Job->Succeded==Job->SourcesAndDests.count()) Job->JobStatus=JOBSTATUS_SUCCEEDED; else Job->JobStatus=JOBSTATUS_ENDEDWITHERROR;
    QApplication::postEvent(Window,new QEvent(JobStatusChanged));
}
