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

#ifndef CJOBQUEUE_H
#define CJOBQUEUE_H

#include <QDomElement>
#include <QDomDocument>
#include <QString>
#include "../sharedfiles/cBaseApplicationConfig.h"

#define JOBTYPE_NOTDEFINED                  0
#define JOBTYPE_OPENFILE                    1
#define JOBTYPE_DISPLAYINFO                 2
#define JOBTYPE_IMAGE_CONVERT_JPG           3
#define JOBTYPE_IMAGE_CONVERT_MULTJPG       4
#define JOBTYPE_IMAGE_CONVERT_PNG           5
#define JOBTYPE_IMAGE_CONVERT_MULTPNG       6
/*
#define JOBTYPE_FFMPEGCONVERT_AUDIO         7
#define JOBTYPE_FFMPEGCONVERT_AUDIOFFD      8
#define JOBTYPE_FFMPEGCONVERT_VIDEO         9
#define JOBTYPE_FFMPEGCONVERT_VIDEOFFD      6
#define JOBTYPE_FFMPEGEXTRACT_AUDIO         7
#define JOBTYPE_FFMPEGEXTRACT_SUBTITLES     8
#define JOBTYPE_FFMPEGEXTRACT_CHAPTER       9
#define JOBTYPE_FFMPEGEXTRACT_ALLCHAPTERS   10
#define JOBTYPE_FFDIAPORAMA_RENDER          11
#define JOBTYPE_FFDIAPORAMA_ARCHIVE         12
#define JOBTYPE_PLAYLIST_CREATEARTIST       13
#define JOBTYPE_PLAYLIST_CREATEALBUM        14
#define JOBTYPE_THUMBNAIL_CREATEARTIST      15
#define JOBTYPE_THUMBNAIL_CREATEALBUM       16
#define JOBTYPE_THUMBNAIL_CREATEFOLDER      17
#define JOBTYPE_THUMBNAIL_EXTRACTCOVER      18
#define JOBTYPE_TAG_AUDIO                   19
#define JOBTYPE_TAG_VIDEO                   20
#define JOBTYPE_TAG_FFD                     21
*/
#define JOBSTATUS_READYTOSTART              0
#define JOBSTATUS_STARTED                   1
#define JOBSTATUS_PAUSED                    2
#define JOBSTATUS_CANCELED                  3
#define JOBSTATUS_SUCCEEDED                 4
#define JOBSTATUS_ENDEDWITHERROR            5

const QEvent::Type JobStatusChanged = (QEvent::Type)2000;
const QEvent::Type FileListChanged  = (QEvent::Type)2001;

class cJob {
public:
    QDateTime   DateTime;
    int         JobType;
    int         JobStatus;
    int         PercentDone;
    bool        Overwrite;
    QStringList SourcesAndDests;
    QString     Command;
    QString     Result;
    int         Succeded;
    int         CurrentIndex;

    cJob();
    ~cJob();

    void        SaveToXML(QDomElement &domDocument,QString ElementName);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName);
};

//***************************

class cJobQueue {
public:
    QList<cJob*>    List;                       // list of cJobObject
    QStringList     StatusText;
    QStringList     JobTypeText;

    cJobQueue();
    ~cJobQueue();

    void        SaveToXML(QDomElement &domDocument,QString ElementName);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName);
    void        RefreshJobStatus(QWidget *Window);

    // Jobs
    void        ConvertIMG(cJob *Job,QWidget *Window);
};

#endif // CJOBQUEUE_H
