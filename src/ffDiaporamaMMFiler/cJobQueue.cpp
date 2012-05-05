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

#include "../sharedfiles/cDeviceModelDef.h"

// Qualification of possible settings for each job
uint32_t PossibleJobsSettings[NBR_JOBTYPE] = {
     0                                                  // #define JOBTYPE_NOTDEFINED
    ,0                                                  // #define JOBTYPE_OPENFILE
    ,0                                                  // #define JOBTYPE_DISPLAYINFO
    ,0                                                  // #define JOBTYPE_REMOVEFILE
    ,JOBQUALIF_IMAGE_CONVERTIMAGE                       // #define JOBTYPE_IMAGE_CONVERT_JPG
    ,JOBQUALIF_AUDIO_CONVERTAUDIO                       // #define JOBTYPE_AUDIO_CONVERTAUDIO
    ,JOBQUALIF_VIDEO_CONVERTVIDEO                       // #define JOBTYPE_VIDEO_CONVERTVIDEO
};

// Help page for each job
int JobHelpPage[NBR_JOBTYPE] = {
     -1                                                 // #define JOBTYPE_NOTDEFINED
    ,-1                                                 // #define JOBTYPE_OPENFILE
    ,-1                                                 // #define JOBTYPE_DISPLAYINFO
    ,-1                                                 // #define JOBTYPE_REMOVEFILE
    ,JOBHELP_IMAGE_CONVERTIMAGE                         // #define JOBTYPE_IMAGE_CONVERT_JPG
    ,JOBHELP_AUDIO_CONVERTAUDIO                         // #define JOBTYPE_AUDIO_CONVERTAUDIO
    ,JOBHELP_VIDEO_CONVERTVIDEO                         // #define JOBTYPE_VIDEO_CONVERTVIDEO
};

/* JOB Command list definition
    Image
        -SSX:Str    Define a size suffix                Str is computed size suffix to be used (This suffix is apply only if JobSettings have JOBQUALIF_DESTNAME_ADDSIZESUFFIX flag)
        -SRS:Str    Define a rescal to size value       Str is the rescal size to be rescal to in string format
        -FRT        Force image rotation                if present in the command list then image was rotated for jpg format
        -DUP        Forbid image upscaling              if present in the command list then image upscalling is forbidden

    Container part
        -FCO:Num    Define a ffmpeg -vcodec value       Num correspond to index of codec in FORMATDEF table

    Video part
        -VCO:Num    Define a ffmpeg -vcodec value       Num correspond to index of codec in VIDEOCODECDEF table
        -VB:Num     Define a ffmpeg -b value            or -minrate -maxrate -bufsize values in int format
        -VI:Num     Define a ffmpeg image size          Num correspond to index in DefImageFormat table

    Audio part
        -ACO:Num    Define a ffmpeg -acodec value       Num correspond to index of codec in AUDIOCODECDEF table
        -AB:Str     Define a ffmpeg -ab value           Str correspond to bitrate value in string format
        -AR:Str     Define a ffmpeg -ar value           Str correspond to rate value in string format
        -ACH:Num    Define a ffmpeg -ac value           Num correspond to chanels value in int format
*/


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
    CurrentAction    ="";

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
    Element.setAttribute("DestinationExtension",DestinationExtension);

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
        if (Element.hasAttribute("JobQualif"))              JobQualif           =Element.attribute("JobQualif").toInt();
        if (Element.hasAttribute("JobSettings"))            JobSettings         =Element.attribute("JobSettings").toInt();
        if (Element.hasAttribute("SourceSuffix"))           SourceSuffix        =Element.attribute("SourceSuffix");
        if (Element.hasAttribute("SourceFolder"))           SourceFolder        =Element.attribute("SourceFolder");
        if (Element.hasAttribute("DestinationSuffix"))      DestinationSuffix   =Element.attribute("DestinationSuffix");
        if (Element.hasAttribute("DestinationFolder"))      DestinationFolder   =Element.attribute("DestinationFolder");
        if (Element.hasAttribute("DestinationExtension"))   DestinationExtension=Element.attribute("DestinationExtension");

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

QString cJob::ComputeDestinationName(QString Source,QString SizeSuffix) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJob::ComputeDestinationName");

    QString SourceExt   ="."+QFileInfo(Source).suffix();
    QString ShortName   =QFileInfo(Source).fileName();          ShortName.replace(SourceExt,"");
    QString SourcePath  =QFileInfo(Source).absolutePath();

    QString DestSuffix  =((JobSettings&JOBQUALIF_DESTNAME_ADDSUFFIX)!=0)?"_"+DestinationSuffix+"."+DestinationExtension:"."+DestinationExtension;
    QString DestPath    =((JobSettings&JOBQUALIF_DESTPLACE_INFOLDER)==0)?SourcePath:DestinationFolder;
    QString DestName    =ShortName;

    if (((JobSettings&JOBQUALIF_DESTNAME_ADDSIZESUFFIX)!=0)&&(SizeSuffix!="")) {
        SizeSuffix.replace("/","-");
        DestSuffix="_"+SizeSuffix+DestSuffix;
    }

    if (!SourcePath.endsWith(QDir::separator()))            SourcePath=SourcePath+QDir::separator();
    if (!DestPath.endsWith(QDir::separator()))              DestPath=DestPath+QDir::separator();

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
    JobTypeText.append(QApplication::translate("QCustomJobTable","Convert"));                           // JOBTYPE_AUDIO_CONVERTAUDIO
    JobTypeText.append(QApplication::translate("QCustomJobTable","Convert"));                           // JOBTYPE_VIDEO_CONVERTVIDEO
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
    for (int i=0;i<List.count();i++) List[i]->SaveToXML(Element,"Job-"+QString("%1").arg(i));
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

void cJobQueue::PostJobStatusChanged(cJob *Job,QString Action,int Percent) {
    Job->CurrentAction=Action;
    Job->PercentDone  =Percent;
    PostEvent(EVENT_JobStatusChanged);
}

//====================================================================================================================

QString cJobQueue::ComputeFFMPEGCommand(cJob *Job) {
    QString Source      =Job->SourcesAndDests[Job->CurrentIndex];
    QString Destination =Job->ComputeDestinationName(Source,"");
    QString ffmpegCmd   =QString("%1 -i \"%2\"").arg(ENCODERNAME).arg(Source);
    if ((Job->IsCommandListContain("-VCO"))&&(Job->IsCommandListContain("-VB")))  {
        int     VideoCodecIndex =Job->CommandListValue("-VCO");
        int     VideoBitRate    =Job->CommandListValue("-VB");
        QString vCodec          ="";
        QString AudioCodec      =(Job->IsCommandListContain("-ACO")?AUDIOCODECDEF[Job->CommandListValue("-ACO")].ShortName:"");
        QString Preset          ="";

        switch (VIDEOCODECDEF[VideoCodecIndex].FFD_VCODEC) {
            case VCODEC_MPEG    :   vCodec=QString("-vcodec mpeg2video -minrate %1 -maxrate %2 -bufsize %3 -b:0 %4 -bf 3")
                                       .arg(VideoBitRate-VideoBitRate/10)
                                       .arg(VideoBitRate+VideoBitRate/10)
                                       .arg(VideoBitRate*2)
                                       .arg(VideoBitRate);
                                    break;
            case VCODEC_MPEG4   :   if (AudioCodec=="libopencore_amrnb") {
                                            vCodec=QString("-f 3gp -vcodec mpeg4 -b:0 %1").arg(VideoBitRate);
                                    } else {
                                        if (QString(VIDEOCODECDEF[VideoCodecIndex].ShortName)==QString("mpeg4"))
                                            vCodec=QString("-vcodec mpeg4 -vtag xvid -b:0 %1").arg(VideoBitRate);
                                            else vCodec=QString("-vcodec libxvid -b:0 %1").arg(VideoBitRate);
                                    }
                                    break;
            case VCODEC_H264HQ  :   //Preset=AdjustDirForOS(QDir::currentPath()); if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();
                                    #ifdef OLDFFMPEGPRESET
                                    Preset="-fpre \""+Preset+"libx264-hq.ffpreset\"";
                                    #else
                                    Preset="-preset veryfast -x264opts ref=3";
                                    #endif
                                    vCodec=QString("-vcodec libx264 -pix_fmt yuv420p ")+Preset+QString(" -minrate %1 -maxrate %2 -bufsize %3 -b:0 %4")
                                        .arg(VideoBitRate-VideoBitRate/10)
                                        .arg(VideoBitRate+VideoBitRate/10)
                                        .arg(VideoBitRate*2)
                                        .arg(VideoBitRate);
                                    break;
            case VCODEC_H264PQ  :   //Preset=AdjustDirForOS(QDir::currentPath()); if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();
                                    #ifdef OLDFFMPEGPRESET
                                    Preset="-fpre \""+Preset+"libx264-pq.ffpreset\"";
                                    #else
                                    Preset="-preset veryfast -x264opts level=1.3:no-cabac:vbv-bufsize=768:vbv-maxrate=768";
                                    #endif
                                    vCodec=QString("-vcodec libx264 -pix_fmt yuv420p ")+Preset+QString(" -minrate %1 -maxrate %2 -bufsize %3 -b:0 %4")
                                        .arg(VideoBitRate-VideoBitRate/10)
                                        .arg(VideoBitRate+VideoBitRate/10)
                                        .arg(VideoBitRate*2)
                                        .arg(VideoBitRate);
                                    break;
            case VCODEC_X264LL  :   Preset=AdjustDirForOS(QDir::currentPath()); if (!Preset.endsWith(QDir::separator())) Preset=Preset+QDir::separator();
                                    #ifdef OLDFFMPEGPRESET
                                    Preset="-fpre \""+Preset+"libx264-lossless.ffpreset\"";
                                    #else
                                    Preset="-preset veryfast -qp 0";
                                    #endif
                                    vCodec=QString("-vcodec libx264 -pix_fmt yuv420p ")+Preset;
                                    break;
            case VCODEC_MJPEG   :   vCodec="-vcodec mjpeg -qscale 2 -qmin 2 -qmax 2";   break;
            case VCODEC_VP8     :   vCodec=QString("-vcodec libvpx -minrate %1 -maxrate %2 -bufsize %3 -b:0 %4 -bf 3")
                                        .arg(VideoBitRate-VideoBitRate/10)
                                        .arg(VideoBitRate+VideoBitRate/10)
                                        .arg(VideoBitRate*2)
                                        .arg(VideoBitRate);
                                    break;
            case VCODEC_H263    :   vCodec=QString("-vcodec flv -b:0 %1").arg(VideoBitRate);
                                    break;
            case VCODEC_THEORA  :   vCodec=QString("-vcodec libtheora -b:0 %1").arg(VideoBitRate);
                                    break;
        }
        #ifdef OLDFFMPEG
        vCodec.replace(" -b:0 "," -b "); // switch to old syntax
        #endif

        ffmpegCmd=ffmpegCmd+" "+vCodec;
    }
    if (Job->IsCommandListContain("-ACO"))  {
        ffmpegCmd=ffmpegCmd+" -acodec "+AUDIOCODECDEF[Job->CommandListValue("-ACO")].ShortName;
        if (Job->IsCommandListContain("-AB"))   ffmpegCmd=ffmpegCmd+" -ab "+Job->CommandListValueString("-AB");
        if (Job->IsCommandListContain("-AR"))   ffmpegCmd=ffmpegCmd+" -ar "+Job->CommandListValueString("-AR").left(Job->CommandListValueString("-AR").indexOf(" "));
        if (Job->IsCommandListContain("-ACH"))  ffmpegCmd=ffmpegCmd+" -ac %1"+Job->CommandListValue("-ACH");
    }
    ffmpegCmd=ffmpegCmd+" \""+Destination+"\"";
    return ffmpegCmd;
}

//====================================================================================================================

bool cJobQueue::ApplySourceTransformation(QString /*Source*/,QString /*NewSource*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::ApplySourceTransformation");

    ToLog(LOGMSG_WARNING,"    "+QString(QApplication::translate("cJobQueue","Applying source transformation : Sorry no yet developped !")),JOBQUEUESRC);
    return true;
}

//====================================================================================================================

bool cJobQueue::ApplyDestinationOverWriting(QString Destination,uint32_t JobSettings) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::ApplyDestinationOverWriting");

    bool IsOkForWriting=true;
    if ((QFileInfo(Destination).exists())&&((JobSettings & JOBQUALIF_DESTINATION_OVERWRITE)==0)) {
        ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("cJobQueue","Destination file (%1) already exist").arg(Destination),JOBQUEUESRC);
        IsOkForWriting=false;
    } else if ((QFileInfo(Destination).exists())&&((JobSettings & JOBQUALIF_DESTINATION_OVERWRITE)!=0)) {
        if (!QFile(Destination).remove()) {
            IsOkForWriting=false;
            ToLog(LOGMSG_CRITICAL,   "    "+QApplication::translate("cJobQueue","Failed to remove existing destination file (%1)").arg(Destination),JOBQUEUESRC);
        } else
            ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cJobQueue","Successfully remove existing destination file (%1)").arg(Destination),JOBQUEUESRC);
    }
    return IsOkForWriting;
}

//====================================================================================================================

void cJobQueue::ConvertIMG(cJob *Job) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::ConvertIMG");

    if (Job->JobType!=JOBTYPE_IMAGE_CONVERTIMAGE) {
        Job->JobStatus=JOBSTATUS_CANCELED;
        ToLog(LOGMSG_CRITICAL,QString(QApplication::translate("cJobQueue","Jobtype error")),JOBQUEUESRC);
        return;
    }
    if ((Job->JobStatus!=JOBSTATUS_READYTOSTART)&&(Job->JobStatus!=JOBSTATUS_PAUSED)) return;

    int StepInJob=6;                                                                            // Load+Rescal+Modif.Dest+Save+Modif Source+Finish
    if ((Job->DestinationExtension=="tiff")||(Job->DestinationExtension=="jpg")) StepInJob++;   // Add minimal exif values

    Job->JobStatus=JOBSTATUS_STARTED;
    for (;Job->CurrentIndex<Job->SourcesAndDests.count();) {
        bool    Rescal      =Job->IsCommandListContain("-SRS");
        int     ImgRotation =1;
        QString SizeSuffix  =(Job->IsCommandListContain("-SSX")?Job->CommandListValueString("-SSX"):"ns");
        QString Source      =Job->SourcesAndDests[Job->CurrentIndex];
        QString Destination =Job->ComputeDestinationName(Source,SizeSuffix);
        QString NewSource   =Job->ComputeNewSourceName(Source);
        int     Quality     =Job->IsCommandListContain("-Q")?Job->CommandListValue("-Q"):90;
        int     InStep      =0;
        bool    ForceRotate =(ImgRotation!=1)&&((Job->DestinationExtension!="jpg")||(Job->IsCommandListContain("-FRT")));
        QImage  Img;

        ToLog(LOGMSG_INFORMATION,QApplication::translate("cJobQueue","Start image conversion of %1 to %2").arg(Source).arg(Destination),JOBQUEUESRC);

        // Step 1 : Load image
        PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Open file"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
        Img.load(Source);
        if (Img.isNull()) ToLog(LOGMSG_CRITICAL,   "    "+QApplication::translate("cJobQueue","Failed to open file %1").arg(Source),JOBQUEUESRC);
            else          ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cJobQueue","Successfully open file %1").arg(Source),JOBQUEUESRC);

        // Get rotation information from exif value
        if (!Img.isNull()) {
            // Restart same job with -pva option to get binary value of orientation
            QString Commande=AdjustDirForOS("exiv2 print -pva \""+Source+"\"");
            QString  Info,Part;
            QProcess Process;
            bool     ExifOk=true;
            Process.setProcessChannelMode(QProcess::MergedChannels);
            Process.start(Commande);
            if (!Process.waitForStarted()) {
                ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Impossible to start exiv2 - no exif informations will be decode for %1").arg(Source));
                ExifOk=false;
            }
            if (ExifOk && !Process.waitForFinished()) {
                Process.kill();
                ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Error during exiv2 process - no exif informations will be decode for %1").arg(Source));
                ExifOk=false;
            }
            if (ExifOk && (Process.exitStatus()<0)) {
                ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","exiv2 return error %1 - no exif informations will be decode for %2").arg(Process.exitStatus()).arg(Source));
                ExifOk=false;
            }
            if (ExifOk) {
                Info=QString(Process.readAllStandardOutput());

                while (Info.length()>0) {
                    if (Info.contains("\n")) {
                        Part=Info.left(Info.indexOf("\n"));
                        Info=Info.mid(Info.indexOf("\n")+QString("\n").length());
                    } else {
                        Part=Info;
                        Info="";
                    }
                    QString Designation,Value;
                    if (Part.contains(" ")) {
                        Designation=Part.left(Part.indexOf(" "));
                        while (Designation.contains(".")) Designation=(Designation.mid(Designation.indexOf(".")+QString(".").length())).trimmed();
                        if (Designation=="0x0112") {
                            Value=(Part.mid(Part.lastIndexOf(" ")+QString(" ").length())).trimmed();
                            ImgRotation=Value.toInt();
                        }
                    }
                }
            }
            Process.terminate();
            Process.close();

        }

        // Step 2 : Rescal
        PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Rescal image"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
        if ((!Img.isNull())&&(Rescal)) {

            int     ImageWidth  =0;
            int     ImageHeight =0;
            int     NewWidth    =0;
            int     NewHeight   =0;
            QString Value       =Job->CommandListValueString("-SRS");

            if (Value.indexOf("MPix")!=-1) {
                // Image rotation
                if ((ForceRotate)&&(ImgRotation!=1)) {
                    int Rotation=0;
                    if (ImgRotation==8)         Rotation=-90;       // Rotating image anti-clockwise by 90 degrees...'
                    else if (ImgRotation==3)    Rotation=180;       // Rotating image clockwise by 180 degrees...'
                    else if (ImgRotation==6)    Rotation=90;        // Rotating image clockwise by 90 degrees...'

                    if (Rotation!=0) {
                        QMatrix matrix;
                        matrix.rotate(Rotation);
                        Img=Img.transformed(matrix,Qt::SmoothTransformation);
                        ImgRotation=1;
                        if (Img.isNull()) ToLog(LOGMSG_CRITICAL,   "    "+QApplication::translate("cJobQueue","Failed to rotate image"),JOBQUEUESRC);
                            else          ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cJobQueue","Succesfully rotate image"),JOBQUEUESRC);
                    }
                }

                // if image is rotated then swap ImageWidth and ImageHeight
                ImageWidth  =Img.width();
                ImageHeight =Img.height();

                double  iValue      =Value.left(Value.indexOf("MPix")).trimmed().toDouble()*1000*1000;
                double  ActualValue =ImageWidth*ImageHeight;
                // + or - 10%
                if (((iValue-iValue*0.1>ActualValue)&&(!Job->IsCommandListContain("-DUP")))||(iValue+iValue*0.1<ActualValue)) {
                    double Transfo=sqrt(iValue/ActualValue);
                    NewWidth =int(Transfo*double(ImageWidth));
                    NewHeight=int(Transfo*double(ImageHeight));
                }
            } else {
                int Rotation=0;
                if (ImgRotation==8)         Rotation=-90;       // Rotating image anti-clockwise by 90 degrees...'
                else if (ImgRotation==3)    Rotation=180;       // Rotating image clockwise by 180 degrees...'
                else if (ImgRotation==6)    Rotation=90;        // Rotating image clockwise by 90 degrees...'

                if (Rotation!=0) {
                    QMatrix matrix;
                    matrix.rotate(Rotation);
                    Img=Img.transformed(matrix,Qt::SmoothTransformation);
                    ImgRotation=1;
                    if (Img.isNull()) ToLog(LOGMSG_CRITICAL,   "    "+QApplication::translate("cJobQueue","Failed to rotate image"),JOBQUEUESRC);
                        else          ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cJobQueue","Succesfully rotate image"),JOBQUEUESRC);
                }

                if (!Img.isNull()) {
                    ImageWidth  =Img.width();
                    ImageHeight =Img.height();

                    int MaxRows=0;
                    if      (Value.indexOf("QVGA")!=-1)     MaxRows=240;
                    else if (Value.indexOf("HVGA")!=-1)     MaxRows=320;
                    else if (Value.indexOf("WVGA")!=-1)     MaxRows=480;
                    else if (Value.indexOf("DVD")!=-1)      MaxRows=576;
                    else if (Value.indexOf("720p")!=-1)     MaxRows=720;
                    else if (Value.indexOf("XGA")!=-1)      MaxRows=768;
                    else if (Value.indexOf("1080p")!=-1)    MaxRows=1080;
                    // + or - 10%
                    if ((MaxRows!=0)&&(((MaxRows-MaxRows*0.1>ImageHeight)&&(!Job->IsCommandListContain("-DUP")))||(MaxRows+MaxRows*0.1<ImageHeight))) {
                        double AspectR=double(ImageWidth)/double(ImageHeight);
                        NewHeight=MaxRows;
                        NewWidth =int(double(MaxRows)*AspectR);
                    }
                }
            }
            if ((!Img.isNull())&&(NewWidth!=0)&&(NewHeight!=0)) {
                Img=Img.scaled(NewWidth,NewHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
                if (Img.isNull()) ToLog(LOGMSG_CRITICAL,   "    "+QApplication::translate("cJobQueue","Failed to rescal image %1 to %2x%3").arg(Source).arg(NewWidth).arg(NewHeight),JOBQUEUESRC);
                    else          ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cJobQueue","Succesfully rescal image %1 to %2x%3").arg(Source).arg(NewWidth).arg(NewHeight),JOBQUEUESRC);
                PostEvent(EVENT_JobStatusChanged);
            }
        } else if (ForceRotate) {
            int Rotation=0;
            if (ImgRotation==8)         Rotation=-90;       // Rotating image anti-clockwise by 90 degrees...'
            else if (ImgRotation==3)    Rotation=180;       // Rotating image clockwise by 180 degrees...'
            else if (ImgRotation==6)    Rotation=90;        // Rotating image clockwise by 90 degrees...'

            if (Rotation!=0) {
                QMatrix matrix;
                matrix.rotate(Rotation);
                Img=Img.transformed(matrix,Qt::SmoothTransformation);
                ImgRotation=1;
                if (Img.isNull()) ToLog(LOGMSG_CRITICAL,   "    "+QApplication::translate("cJobQueue","Failed to rotate image"),JOBQUEUESRC);
                    else          ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cJobQueue","Succesfully rotate image"),JOBQUEUESRC);
            }
        }

        // Step 3 : Ensure destination image could be write (depends on overwrite !)
        PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Apply destination overwriting strategy"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
        if ((!Img.isNull())&&(ApplyDestinationOverWriting(Destination,Job->JobSettings))) {

            // Step 4 : Save image
            PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Save image file"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
            if (!Img.save(Destination,Job->DestinationExtension.toUtf8().data(),Quality))
                ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("cJobQueue","Failed to write destination file %1").arg(Destination),JOBQUEUESRC);
            else {
                ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cJobQueue","Successfully writing destination file %1").arg(Destination),JOBQUEUESRC);

                bool ExifOk=true;
                if ((Job->DestinationExtension=="tiff")||(Job->DestinationExtension=="jpg")) {
                    // Step 5 : Add exif values to destination file
                    // Command to add minimal exif information into file :
                    //      exiv2 IMG_3682_new.jpg -M"add Exif.Photo.PixelYDimension Short 3000" -M"add Exif.Photo.PixelXDimension Short 2000" -M"add Exif.Image.Orientation Short 8"
                    PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Add minimal exif value to image file"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
                    QString  Commande;
                    QProcess Process;

                    if (Job->DestinationExtension=="tiff") Commande=QString("exiv2 \"%1\" -M\"set Exif.Image.ImageHeight Short %2\" -M\"set Exif.Image.ImageWidth Short %3\"").arg(Destination).arg(Img.height()).arg(Img.width());
                        else Commande=QString("exiv2 \"%1\" -M\"add Exif.Photo.PixelYDimension Short %2\" -M\"add Exif.Photo.PixelXDimension Short %3\" -M\"add Exif.Image.Orientation Short %4\"").arg(Destination).arg(Img.height()).arg(Img.width()).arg(ImgRotation);

                    Process.setProcessChannelMode(QProcess::MergedChannels);
                    Process.start(Commande);
                    if (!Process.waitForStarted()) {
                        ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("cBaseMediaFile","Impossible to start exiv2 - no exif informations will be set for %1").arg(Destination),JOBQUEUESRC);
                        ExifOk=false;
                    }
                    if (ExifOk && !Process.waitForFinished()) {
                        Process.kill();
                        ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("cBaseMediaFile","Error during exiv2 process - no exif informations will be set for %1").arg(Destination),JOBQUEUESRC);
                        ExifOk=false;
                    }
                    if (ExifOk && (Process.exitStatus()<0)) {
                        ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("cBaseMediaFile","exiv2 return error %1 - no exif informations will be set for %2").arg(Process.exitStatus()).arg(Destination),JOBQUEUESRC);
                        ExifOk=false;
                    }
                }

                if (ExifOk) {
                    ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cBaseMediaFile","Successfully add minimal exif values to %1").arg(Destination),JOBQUEUESRC);
                    // Step 5 : Transform source file
                    PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Apply source file strategy"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
                    if (ApplySourceTransformation(Source,NewSource)) {
                        Job->Succeded++;
                    }
                }
                PostEvent(EVENT_FileListChanged,QFileInfo(Destination).absolutePath());
            }

        }
        // Go to next file
        Job->CurrentIndex++;
    }
    if (Job->Succeded==Job->SourcesAndDests.count()) Job->JobStatus=JOBSTATUS_SUCCEEDED; else Job->JobStatus=JOBSTATUS_ENDEDWITHERROR;
    PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Job finished"),100);
}

//====================================================================================================================

void cJobQueue::ProcessFFMPEGJob(cJob *Job) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cJobQueue::ProcessFFMPEGJob");

    int StepInJob=4;      // Modif.Dest and Convert+Modif Source

    if (Job->JobType!=JOBTYPE_AUDIO_CONVERTAUDIO) {
        Job->JobStatus=JOBSTATUS_CANCELED;
        ToLog(LOGMSG_CRITICAL,QString(QApplication::translate("cJobQueue","Jobtype error")),JOBQUEUESRC);
        return;
    }
    if ((Job->JobStatus!=JOBSTATUS_READYTOSTART)&&(Job->JobStatus!=JOBSTATUS_PAUSED)) return;

    Job->JobStatus=JOBSTATUS_STARTED;
    for (;Job->CurrentIndex<Job->SourcesAndDests.count();) {

        QString SizeSuffix  ="ns";
        QString Source      =Job->SourcesAndDests[Job->CurrentIndex];
        QString Destination =Job->ComputeDestinationName(Source,SizeSuffix);
        QString NewSource   =Job->ComputeNewSourceName(Source);
        QString ffmpegCmd   =ComputeFFMPEGCommand(Job);
        int     InStep      =0;

        ToLog(LOGMSG_INFORMATION,QApplication::translate("cJobQueue","Start audio conversion of %1 to %2").arg(Source).arg(Destination),JOBQUEUESRC);

        // Step 1 : Ensure destination file could be write (depends on overwrite !)
        PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Apply destination overwriting strategy"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
        if (Source==Destination) {
            ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("cJobQueue","Error : for sound conversion, source and destination must be different !"),JOBQUEUESRC);
        } else if (ApplyDestinationOverWriting(Destination,Job->JobSettings)) {

            // Step 2 : Do conversion
            ToLog(/*LOGMSG_DEBUGTRACE*/LOGMSG_INFORMATION,"    Send command:"+ffmpegCmd,JOBQUEUESRC);
            QProcess    Process;
            QString     ffmpegOutput="";
            QString     ffmpegLine="";
            bool        ffmpegJobStarted=false;
            PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Process avconv job"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
            Process.setProcessChannelMode(QProcess::MergedChannels);                        // Mix standard and error chanels
            //Process.setWorkingDirectory(Diaporama->ApplicationConfig->UserConfigPath);    // Set working directory to user folder (for log generation)
            Process.start(ffmpegCmd,QIODevice::Append|QIODevice::ReadWrite);                // Start command
            if (!Process.waitForStarted()) {
                ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("DlgRenderVideo","Error starting avconv"),JOBQUEUESRC);
            } else {
                while (!Process.waitForFinished(1000)) { // 1 sec
                    ffmpegOutput=ffmpegLine+QString().fromLocal8Bit(Process.readAllStandardOutput());
                    while (ffmpegOutput!="") {
                        if (ffmpegOutput.indexOf("\n")>0) {
                            ffmpegLine  =ffmpegOutput.left(ffmpegOutput.indexOf("\n"));
                            ffmpegOutput=ffmpegOutput.mid(ffmpegOutput.indexOf("\n")+QString("\n").length());
                            ToLog(LOGMSG_DEBUGTRACE,ffmpegLine,JOBQUEUESRC);
                        } else {
                            ffmpegLine  =ffmpegOutput;
                            ffmpegOutput="";
                        }
                        if (ffmpegLine=="Press ctrl-c to stop encoding") ffmpegJobStarted=true;
                    }
                }
                ffmpegOutput=ffmpegLine+QString().fromLocal8Bit(Process.readAllStandardOutput());
                while (ffmpegOutput!="") {
                    if (ffmpegOutput.indexOf("\n")>0) {
                        ffmpegLine  =ffmpegOutput.left(ffmpegOutput.indexOf("\n"));
                        ffmpegOutput=ffmpegOutput.mid(ffmpegOutput.indexOf("\n")+QString("\n").length());
                        ToLog(LOGMSG_DEBUGTRACE,ffmpegLine,JOBQUEUESRC);
                    } else {
                        ffmpegLine  =ffmpegOutput;
                        ffmpegOutput="";
                    }
                    if (ffmpegLine=="Press ctrl-c to stop encoding") ffmpegJobStarted=true;
                }

                if ((Process.exitStatus()<0)||(!ffmpegJobStarted)) {
                    if (!ffmpegJobStarted) ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("cJobQueue","Error : avconv job can't start : %1").arg(ffmpegLine),JOBQUEUESRC);
                        else               ToLog(LOGMSG_CRITICAL,"    "+QApplication::translate("cJobQueue","Error : avconv return error %1").arg(Process.exitStatus()),JOBQUEUESRC);
                } else {
                    ToLog(LOGMSG_INFORMATION,"    "+QApplication::translate("cJobQueue","Succesfully process avconv job"),JOBQUEUESRC);
                    PostEvent(EVENT_FileListChanged,QFileInfo(Destination).absolutePath());

                    // Step 3 : Transform source file
                    PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Apply source file strategy"),(double(Job->CurrentIndex*StepInJob+InStep++)/double(Job->SourcesAndDests.count()*StepInJob))*100);
                    if (ApplySourceTransformation(Source,NewSource)) {
                        Job->Succeded++;
                    }
                }
            }
        }
        // Go to next file
        Job->CurrentIndex++;
    }
    if (Job->Succeded==Job->SourcesAndDests.count()) Job->JobStatus=JOBSTATUS_SUCCEEDED; else Job->JobStatus=JOBSTATUS_ENDEDWITHERROR;
    PostJobStatusChanged(Job,QApplication::translate("cJobQueue","Job finished"),100);
}
