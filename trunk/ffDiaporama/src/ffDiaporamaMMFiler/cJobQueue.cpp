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

//#define DEBUGMODE

//====================================================================================================================

cJob::cJob() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJob::cJob";
    #endif
    JobType     =0;
    JobStatus   =0;
    PercentDone =0;
    Source      ="";
    Destination ="";
    Command     ="";
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

    Element.setAttribute("JobType",JobType);
    Element.setAttribute("JobStatus",JobStatus);
    Element.setAttribute("Source",Source);
    Element.setAttribute("Destination",Destination);
    Element.setAttribute("Command",Command);

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

        if (Element.hasAttribute("JobType"))        JobType     =Element.attribute("JobType").toInt();
        if (Element.hasAttribute("JobStatus"))      JobStatus   =Element.attribute("JobStatus").toInt();
        if (Element.hasAttribute("Source"))         Source      =Element.attribute("Source");
        if (Element.hasAttribute("Destination"))    Destination =Element.attribute("Destination");
        if (Element.hasAttribute("Command"))        Command     =Element.attribute("Command");

        return IsOk;
    }
    return false;
}

//*********************************************************************************************************************************************

cJobQueue::cJobQueue() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cJobQueue::cJobQueue";
    #endif
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
