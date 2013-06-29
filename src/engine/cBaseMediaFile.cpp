/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

// Include some common various class
#include "cBaseApplicationConfig.h"

// Include some additional standard class
#include "../CustomCtrl/_QCustomDialog.h"
#include <QFileDialog>
#include <QPainter>

// Include some additional standard class
#include "cBaseMediaFile.h"
#include "cLuLoImageCache.h"

#define FFD_APPLICATION_ROOTNAME    "Project"           // Name of root node in the project xml file

#define MaxAudioLenDecoded AVCODEC_MAX_AUDIO_FRAME_SIZE*4

#ifndef INT64_MAX
    #define 	INT64_MAX   0x7fffffffffffffffLL
    #define 	INT64_MIN   (-INT64_MAX - 1LL)
#endif

//#ifdef _MSC_VER
//    #undef AV_TIME_BASE_Q
//    AVRational AV_TIME_BASE_Q={1, AV_TIME_BASE};
//#endif

#define VC_ERROR        0x00000001
#define VC_BUFFER       0x00000002
#define VC_PICTURE      0x00000004
#define VC_USERDATA     0x00000008
#define VC_FLUSHED      0x00000010

//#define PIXFMT      PIX_FMT_BGRA
//#define QTPIXFMT    QImage::Format_ARGB32_Premultiplied
#define PIXFMT      PIX_FMT_RGB24
#define QTPIXFMT    QImage::Format_RGB888

//****************************************************************************************************************************************************************

// from Google music manager (see:http://code.google.com/p/gogglesmm/source/browse/src/gmutils.cpp?spec=svn6c3dbecbad40ee49736b9ff7fe3f1bfa6ca18c13&r=6c3dbecbad40ee49736b9ff7fe3f1bfa6ca18c13)
bool gm_decode_base64(uchar *buffer,uint &len) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:gm_decode_base64");

    static const unsigned char base64[256]={
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x3e,0x80,0x80,0x80,0x3f,
    0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
    0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x80,0x80,0x80,0x80,0x80,
    0x80,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80};

    uint  pos=0;
    uchar v;
    for (uint i=0,b=0;i<len;i++) {
        v=base64[buffer[i]];
        if (v!=0x80) {
          switch(b) {
            case 0: buffer[pos]=(v<<2);
                    b++;
                    break;
            case 1: buffer[pos++]|=(v>>4);
                    buffer[pos]=(v<<4);
                    b++;
                    break;
            case 2: buffer[pos++]|=(v>>2);
                    buffer[pos]=(v<<6);
                    b++;
                    break;
            case 3: buffer[pos++]|=v;
                    b=0;
                    break;
            }
        } else {
            if (buffer[i]=='=' && b>1) {
                len=pos;
                return true;
            } else return false;
        }
    }
    len=pos;
    return true;
}

#ifdef LIBAV_08
    QImage *GetEmbededImage(QString FileName) {
        ToLog(LOGMSG_DEBUGTRACE,"IN:GetEmbededImage");

        // Try to get embeded image
        QImage *Image=new QImage();

        //*********** MP3
        if ((Image->isNull())&&(QFileInfo(FileName).suffix().toLower()=="mp3")) {
            TagLib::MPEG::File MP3File(TagLib::FileName(FileName.toLocal8Bit()));
            if (MP3File.ID3v2Tag()) {
                TagLib::ID3v2::FrameList l=MP3File.ID3v2Tag()->frameListMap()["APIC"];
                if (!l.isEmpty()) {
                    TagLib::ID3v2::AttachedPictureFrame *pic=static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l.front());
                    if (pic) Image->loadFromData((const uchar *)pic->picture().data(),pic->picture().size());
                }
            }
        }
        //*********** FLAC
        #ifdef TAGLIBWITHFLAC
        if ((Image->isNull())&&(QFileInfo(FileName).suffix().toLower()=="flac")) {
            TagLib::FLAC::File                      FLACFile(TagLib::FileName(FileName.toLocal8Bit()));
            TagLib::List<TagLib::FLAC::Picture *>   PictList=FLACFile.pictureList();

            // Search PreferedPic : the one with the type lesser
            TagLib::FLAC::Picture *PreferedPic=NULL;
            if (!PictList.isEmpty()) for (uint i=0;i<PictList.size();i++) {
                TagLib::FLAC::Picture *Pic=PictList[i];
                if ((Pic!=NULL)&&((PreferedPic==NULL)||(PreferedPic->type()>Pic->type()))) PreferedPic=Pic;
            }
            if (PreferedPic) Image->loadFromData((const uchar *)PreferedPic->data().data(),PreferedPic->data().size());
        }
        #endif
        //*********** OGG
        if ((Image->isNull())&&((QFileInfo(FileName).suffix().toLower()=="ogg")||(QFileInfo(FileName).suffix().toLower()=="oga"))) {
            TagLib::Vorbis::File OggFile(TagLib::FileName(FileName.toLocal8Bit()));
            if ((OggFile.tag())&&(OggFile.tag()->contains(TagLib::String("COVERART")))) {
                const TagLib::StringList &CoverList=OggFile.tag()->fieldListMap()["COVERART"];
                for (TagLib::StringList::ConstIterator it=CoverList.begin();it!=CoverList.end();it++) {
                    const TagLib::ByteVector &Vector=(*it).data(TagLib::String::UTF8);
                    if ((Image->isNull())&&(Vector.size())) {
                        uint  len    =Vector.size();
                        uchar *buffer=(uchar *)malloc(len);
                        memcpy(buffer,Vector.data(),len);
                        if (gm_decode_base64(buffer,len))
                            Image->loadFromData((const uchar *)buffer,len);
                        free(buffer);
                    }

                }
            }
        }
        //*********** MP4/M4A => don't work with M4V or MP4 video
        #ifdef TAGLIBWITHMP4
        if ((Image->isNull())&&(/*(QFileInfo(FileName).suffix().toLower()=="mp4")||*/(QFileInfo(FileName).suffix().toLower()=="m4a")||(QFileInfo(FileName).suffix().toLower()=="m4v"))) {
            TagLib::MP4::File MP4File(TagLib::FileName(FileName.toLocal8Bit()));
            if ((MP4File.tag())&&(MP4File.tag()->itemListMap().contains("covr"))) {
                TagLib::MP4::CoverArtList coverArtList = MP4File.tag()->itemListMap()["covr"].toCoverArtList();
                if (coverArtList.size()!= 0) {
                    TagLib::MP4::CoverArt ca = coverArtList.front();
                    Image->loadFromData((const uchar *) ca.data().data(),ca.data().size());
                }
            }
        }
        #endif
        //*********** ASF/WMA //////////////////// A FINIR ! ///////////// CA A PAS L'AIR DE MARCHER !
        #ifdef TAGLIBWITHASF
        if ((Image->isNull())&&(QFileInfo(FileName).suffix().toLower()=="wma")) {
            TagLib::ASF::File ASFFile(TagLib::FileName(TagLib::FileName(FileName.toLocal8Bit())));
            /*
            TagLib::ASF::Tag* asfTag = dynamic_cast<TagLib::ASF::Tag*>(ASFFile.tag());
            TagLib::ASF::AttributeListMap& attrListMap = asfTag->attributeListMap();
            for (TagLib::ASF::AttributeListMap::Iterator it=attrListMap.begin();it!=attrListMap.end();++it) {

                TagLib::ASF::AttributeList& attrList = (*it).second;
                for (TagLib::ASF::AttributeList::Iterator ait = attrList.begin();ait != attrList.end();++ait) {
                    //qDebug()<< QString().fromStdString((*ait).toString().toCString());
                }
            }
            */
            #ifdef TAGLIBWITHASFPICTURE
            if ((ASFFile.tag())&&(ASFFile.tag()->attributeListMap().contains("WM/Picture"))) {
                const TagLib::ASF::AttributeList &attrList=ASFFile.tag()->attributeListMap()["WM/Picture"];
                if (!attrList.isEmpty()) {
                        TagLib::ASF::Picture pic = attrList[0].toPicture();
                        if (pic.isValid()) Image->loadFromData((const uchar *)pic.picture().data(),pic.picture().size());
                }
            }
            #endif
        }
        #endif
        //***********
        if (!Image->isNull()) return Image; else {
            delete Image;
            return NULL;
        }
    }
#endif
//*********************************************************************************************************************************************
// Base class object
//*********************************************************************************************************************************************

cBaseMediaFile::cBaseMediaFile(cBaseApplicationConfig *TheApplicationConfig):cCustomIcon() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::cBaseMediaFile");

    ApplicationConfig   = TheApplicationConfig;
    Reset();
}

void cBaseMediaFile::Reset() {
    ObjectType          = OBJECTTYPE_UNMANAGED;
    IsValide            = false;                                    // if true then object if initialise
    IsInformationValide = false;                                    // if true then information list if fuly initialise
    ObjectGeometry      = IMAGE_GEOMETRY_UNKNOWN;                   // Image geometry
    FileName            = "";                                       // filename
    FileExtension       = "";
    ShortName           = "";
    FileSize            = 0;
    FileSizeText        = "";
    ImageWidth          = 0;                                        // Widht of normal image
    ImageHeight         = 0;                                        // Height of normal image
    CreatDateTime       = QDateTime(QDate(0,0,0),QTime(0,0,0));     // Original date/time
    ModifDateTime       = QDateTime(QDate(0,0,0),QTime(0,0,0));     // Last modified date/time
    AspectRatio         = 1;
    ImageOrientation    = -1;
}

//====================================================================================================================

cBaseMediaFile::~cBaseMediaFile() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cBaseMediaFile::~cBaseMediaFile for object %1").arg(FileName));
}

//====================================================================================================================

bool cBaseMediaFile::GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetInformationFromFile");

    FileName=QFileInfo(GivenFileName).absoluteFilePath();

    if (ModifyFlag) *ModifyFlag=false;

    // Use aliaslist
    if ((AliasList)&&(!QFileInfo(FileName).exists())) {
        // First test : seach for a new path+filename for this filename
        int i;
        for (i=0;(i<AliasList->count())&&(!AliasList->at(i).startsWith(FileName));i++);
        if ((i<AliasList->count())&&(AliasList->at(i).startsWith(FileName))) {
            FileName=AliasList->at(i);
            if (FileName.indexOf("####")>0) FileName=FileName.mid(FileName.indexOf("####")+QString("####").length());
        } else {
            // Second test : use each remplacement folder to try to find find
            i=0;
            QString NewFileName=QFileInfo(GivenFileName).absoluteFilePath();
            while ((i<AliasList->count())&&(!QFileInfo(NewFileName).exists())) {
                QString OldName=AliasList->at(i);
                QString NewName=OldName.mid(OldName.indexOf("####")+QString("####").length());
                OldName=OldName.left(OldName.indexOf("####"));
                OldName=OldName.left(OldName.lastIndexOf(QDir::separator()));
                NewName=NewName.left(NewName.lastIndexOf(QDir::separator()));
                NewFileName=NewName+QDir::separator()+QFileInfo(GivenFileName).fileName();
                i++;
            }
            if (QFileInfo(NewFileName).exists()) {
                FileName=NewFileName;
                if (AliasList) AliasList->append(FileName+"####"+NewFileName);
                if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(FileName).absolutePath();     // Keep folder for next use
                if (ModifyFlag) *ModifyFlag=true;
            }
        }
    }

    bool Continue=true;
    while ((Continue)&&(!QFileInfo(FileName).exists())) {
        QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
        if (CustomMessageBox(ApplicationConfig->TopLevelWindow,QMessageBox::Question,QApplication::translate("cBaseMediaFile","Open file"),
            QApplication::translate("cBaseMediaFile","Impossible to open file ")+FileName+"\n"+QApplication::translate("cBaseMediaFile","Do you want to select another file ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes)
            Continue=false;
        else {
            QString NewFileName=QFileDialog::getOpenFileName(ApplicationConfig->TopLevelWindow,QApplication::translate("cBaseMediaFile","Select another file for ")+QFileInfo(FileName).fileName(),
               ApplicationConfig->RememberLastDirectories?ApplicationConfig->LastMediaPath:"",
               ApplicationConfig->GetFilterForMediaFile(ObjectType==OBJECTTYPE_IMAGEFILE?IMAGEFILE:ObjectType==OBJECTTYPE_VIDEOFILE?VIDEOFILE:MUSICFILE));
            if (NewFileName!="") {
                if (AliasList) AliasList->append(FileName+"####"+NewFileName);
                FileName=NewFileName;
                if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(FileName).absolutePath();     // Keep folder for next use
                if (ModifyFlag) *ModifyFlag=true;
            } else Continue=false;
        }
        QApplication::restoreOverrideCursor();
    }
    if (!Continue) {
        ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Impossible to open file %1").arg(FileName));
        ShortName=QFileInfo(FileName).fileName();
        return false;
    }

    ShortName    =QFileInfo(FileName).fileName();
    FileExtension=QFileInfo(FileName).completeSuffix().toLower();
    FileSize     =QFileInfo(FileName).size();
    FileSizeText =GetTextSize(FileSize);
    ModifDateTime=QFileInfo(FileName).lastModified();
    CreatDateTime=QFileInfo(FileName).created();

    IsValide=true;

    return IsValide;
}

//====================================================================================================================

QString cBaseMediaFile::GetInformationValue(QString ValueToSearch) {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetInformationValue");    // Remove: to much

    int i=0;
    while ((i<InformationList.count())&&(!((QString )InformationList[i]).startsWith(ValueToSearch+"##"))) i++;
    if ((i<InformationList.count())&&(((QString )InformationList[i]).startsWith(ValueToSearch))) {
        QStringList Values=((QString)InformationList[i]).split("##");
        if (Values.count()==2) return ((QString)Values[1]).trimmed();
    }
    return "";
}

//====================================================================================================================

QString cBaseMediaFile::GetImageGeometryStr() {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetImageGeometryStr");    // Remove: to much

    switch (ObjectGeometry) {
        case IMAGE_GEOMETRY_3_2     : return "3:2";
        case IMAGE_GEOMETRY_2_3     : return "2:3";
        case IMAGE_GEOMETRY_4_3     : return "4:3";
        case IMAGE_GEOMETRY_3_4     : return "3:4";
        case IMAGE_GEOMETRY_16_9    : return "16:9";
        case IMAGE_GEOMETRY_9_16    : return "9:16";
        case IMAGE_GEOMETRY_40_17   : return "40:17";
        case IMAGE_GEOMETRY_17_40   : return "17:40";
        default                     : return "";        //QApplication::translate("cBaseMediaFile","ns","Non standard image geometry");
    }
}

//====================================================================================================================

QString cBaseMediaFile::GetFileSizeStr() {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetFileSizeStr");         // Remove: to much

    return FileSizeText;
}

//====================================================================================================================

QString cBaseMediaFile::GetFileDateTimeStr(bool Created) {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetFileDateTimeStr");     // Remove: to much

    if (Created) return CreatDateTime.toString("dd/MM/yyyy hh:mm:ss");
        else return ModifDateTime.toString("dd/MM/yyyy hh:mm:ss");
}

//====================================================================================================================

QString cBaseMediaFile::GetImageSizeStr(ImageSizeFmt Fmt) {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetImageSizeStr");        // Remove: to much

    QString SizeInfo="";
    QString FmtInfo ="";
    QString GeoInfo ="";

    if ((ImageWidth>0)&&(ImageHeight>0)) {
        // Compute MPix
        double MPix=double(double(ImageWidth)*double(ImageHeight))/double(1000000);
        SizeInfo=QString("%1x%2").arg(ImageWidth).arg(ImageHeight);

        // now search if size is referenced in DefImageFormat
        for (int i=0;i<2;i++) for (int j=0;j<3;j++) for (int k=0;k<NBR_SIZEDEF;k++) if ((DefImageFormat[i][j][k].Width==ImageWidth)&&(DefImageFormat[i][j][k].Height==ImageHeight)) {
            FmtInfo=QString(DefImageFormat[i][j][k].Name).left(QString(DefImageFormat[i][j][k].Name).indexOf(" -"));
            break;
        }
        if ((FmtInfo=="")&&(MPix>=1)) FmtInfo=QString("%1").arg(MPix,8,'f',1).trimmed()+QApplication::translate("cBaseMediaFile","MPix");
        else switch (ImageHeight) {
            case 240:   FmtInfo="QVGA";     break;
            case 320:   FmtInfo="HVGA";     break;
            case 480:   FmtInfo="WVGA";     break;
            case 576:   FmtInfo="DVD";      break;
            case 600:   FmtInfo="SVGA";     break;
            case 720:   FmtInfo="720p";     break;
            case 768:   FmtInfo="XGA";      break;
            case 1080:  FmtInfo="1080p";    break;
            default:    FmtInfo="ns";       break;
        }
    }
    GeoInfo=GetImageGeometryStr();
    switch (Fmt) {
        case FULLWEB  : return SizeInfo+((FmtInfo+GeoInfo)!=""?"("+FmtInfo+(FmtInfo!=""?"-":"")+GeoInfo+")":"");
        case SIZEONLY : return SizeInfo;
        case FMTONLY  : return FmtInfo;
        case GEOONLY  : return GeoInfo;
        default       : return "";
    }
}

//====================================================================================================================

QString cBaseMediaFile::GetCumulInfoStr(QString Key1,QString Key2) {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetCumulInfoStr");        // Remove: to much

    int     Num     =0;
    QString TrackNum="";
    QString Value   ="";
    QString Info    ="";
    do {
        TrackNum=QString("%1").arg(Num);
        while (TrackNum.length()<3) TrackNum="0"+TrackNum;
        TrackNum=Key1+"_"+TrackNum+":";
        Value=GetInformationValue(TrackNum+Key2);
        if (Value!="") Info=Info+((Num>0)?",":"")+Value;
        // Next
        Num++;
    } while (Value!="");
    return Info;
}

//*********************************************************************************************************************************************
// Unmanaged File
//*********************************************************************************************************************************************

cUnmanagedFile::cUnmanagedFile(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cUnmanagedFile::cUnmanagedFile");

    LoadIcons(&ApplicationConfig->DefaultFILEIcon);
    ObjectType  =OBJECTTYPE_UNMANAGED;
    IsInformationValide=true;
}

//====================================================================================================================

bool cUnmanagedFile::GetInformationFromFile(QString GivenFileName,QStringList *,bool *) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cUnmanagedFile::GetInformationFromFile");

    FileName            =QFileInfo(GivenFileName).absoluteFilePath();
    ShortName           =QFileInfo(FileName).fileName();

    if (!QFileInfo(FileName).exists()) {
        ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Impossible to open file %1").arg(FileName));
        IsValide=false;
    } else {
        FileSize            =QFileInfo(FileName).size();
        FileSizeText        =GetTextSize(FileSize);
        CreatDateTime       =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
        ModifDateTime       =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !

        IsValide=true;
    }

    return IsValide;
}

//====================================================================================================================

QString cUnmanagedFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cUnmanagedFile::GetFileTypeStr for %1").arg(FileName));

    return QApplication::translate("cBaseMediaFile","Unmanaged","File type");
}

//====================================================================================================================

bool cUnmanagedFile::IsFilteredFile(int RequireObjectType,int AllowedObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cUnmanagedFile::IsFilteredFile");

    return (RequireObjectType==OBJECTTYPE_UNMANAGED)&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_UNMANAGED)!=0);
}

//*********************************************************************************************************************************************
// Folder
//*********************************************************************************************************************************************

cFolder::cFolder(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::cFolder");

    ObjectType  =OBJECTTYPE_FOLDER;
}

//====================================================================================================================

bool cFolder::GetInformationFromFile(QString GivenFileName,QStringList * /*AliasList*/,bool * /*ModifyFlag*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::GetInformationFromFile");

    FileName            =QFileInfo(GivenFileName).absoluteFilePath();
    ShortName           =QFileInfo(GivenFileName).fileName();
    CreatDateTime       =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime       =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    return true;
}

//====================================================================================================================

bool cFolder::IsFilteredFile(int,int AllowedObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::IsFilteredFile");

    return ((AllowedObjectType&FILTERALLOW_OBJECTTYPE_FOLDER)!=0);    // always valide
}

//====================================================================================================================

void cFolder::GetFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::GetFullInformationFromFile");

    IsInformationValide=true;

    QString AdjustedFileName=FileName;  if (!AdjustedFileName.endsWith(QDir::separator())) AdjustedFileName=AdjustedFileName+QDir::separator();

    // Check if a folder.jpg file exist
    if (Icon16.isNull()) {
        QFileInfoList Directorys=QDir(FileName).entryInfoList(QDir::Files);
        for (int j=0;j<Directorys.count();j++) if (Directorys[j].fileName().toLower()=="folder.jpg") {
            QString FileName=AdjustedFileName+Directorys[j].fileName();
            QImage Final(":img/FolderMask_200.png");
            QImage Img(FileName);
            QImage ImgF;
            if (double(Img.height())/double(Img.width())*double(Img.width())<=162) ImgF=Img.scaledToWidth(180,Qt::SmoothTransformation);
                else ImgF=Img.scaledToHeight(162,Qt::SmoothTransformation);
            QPainter Painter;
            Painter.begin(&Final);
            Painter.drawImage(QRect((Final.width()-ImgF.width())/2,195-ImgF.height(),ImgF.width(),ImgF.height()),ImgF);
            Painter.end();
            LoadIcons(&Final);
        }
    }

    // Check if there is an desktop.ini ==========> WINDOWS EXTENSION
    if (Icon16.isNull()) {
        QFileInfoList Directorys=QDir(FileName).entryInfoList(QDir::Files|QDir::Hidden);
        for (int j=0;j<Directorys.count();j++) if (Directorys[j].fileName().toLower()=="desktop.ini") {
            QFile   FileIO(AdjustedFileName+Directorys[j].fileName());
            QString IconFile ="";
            #ifdef Q_OS_WIN
            int     IconIndex=0;
            #endif
            if (FileIO.open(QIODevice::ReadOnly/*|QIODevice::Text*/)) {
                // Sometimes this kind of files have incorrect line terminator : nor \r\n nor \n
                QTextStream FileST(&FileIO);
                QString     AllInfo=FileST.readAll();
                QString     Line="";
                while (AllInfo!="") {
                    int j=0;
                    while ((j<AllInfo.length())&&((AllInfo[j]>=char(32))||(AllInfo[j]==9))) j++;
                    if (j<AllInfo.length()) {
                        Line=AllInfo.left(j);
                        while ((j<AllInfo.length())&&(AllInfo[j]<=char(32))) j++;
                        if (j<AllInfo.length()) AllInfo=AllInfo.mid(j); else AllInfo="";
                    } else {
                        Line=AllInfo;
                        AllInfo="";
                    }
                    #ifdef Q_OS_WIN
                    if ((Line.toUpper().startsWith("ICONINDEX"))&&(Line.indexOf("=")!=-1)) {
                        IconIndex=Line.mid(Line.indexOf("=")+1).toInt();
                    } else
                    #endif
                    if ((Line.toUpper().startsWith("ICONFILE"))&&(Line.indexOf("=")!=-1)) {
                        Line=Line.mid(Line.indexOf("=")+1).trimmed();
                        // Replace all variables like %systemroot%
                        while (Line.indexOf("%")!=-1) {
                            QString Var=Line.mid(Line.indexOf("%")+1);  Var=Var.left(Var.indexOf("%"));
                            QString Value=getenv(Var.toLocal8Bit());
                            Line.replace("%"+Var+"%",Value,Qt::CaseInsensitive);
                        }
                        if (QFileInfo(Line).isRelative()) IconFile=AdjustDirForOS(AdjustedFileName+Line);
                            else IconFile=AdjustDirForOS(QFileInfo(Line).absoluteFilePath());
                    }
                }
                FileIO.close();
            }
            if (IconFile.toLower().endsWith(".jpg") || IconFile.toLower().endsWith(".png") || IconFile.toLower().endsWith(".ico")) LoadIcons(IconFile);
            #ifdef Q_OS_WIN
            else LoadIcons(GetIconForFileOrDir(IconFile,IconIndex));
            #endif
        }
    }

    // if no icon then load default for type
    if (Icon16.isNull()) LoadIcons(&ApplicationConfig->DefaultFOLDERIcon);
}

//====================================================================================================================

QString cFolder::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cFolder::GetFileTypeStr for %1").arg(FileName));

    return QApplication::translate("cBaseMediaFile","Folder","File type");
}

//*********************************************************************************************************************************************
// ffDiaporama project file
//*********************************************************************************************************************************************

cffDProjectFile::cffDProjectFile(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::cffDProjectFile");

    LoadIcons(&ApplicationConfig->DefaultFFDIcon);
    ObjectType      =OBJECTTYPE_FFDFILE;
    Title           ="";
    Author          ="";
    Album           ="";
    Year            =QDate::currentDate().year();
    Comment         ="";
    Composer        ="";
    Duration        =0;
    NbrSlide        =0;
    ffDRevision     ="";
    DefaultLanguage ="und";
    NbrChapters     =0;
}

//====================================================================================================================

bool cffDProjectFile::GetInformationFromFile(QString GivenFileName,QStringList * /*AliasList*/,bool * /*ModifyFlag*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::GetInformationFromFile");

    FileName            =QFileInfo(GivenFileName).absoluteFilePath();
    ShortName           =QFileInfo(GivenFileName).fileName();
    FileSize            =QFileInfo(GivenFileName).size();
    FileSizeText        =GetTextSize(FileSize);
    CreatDateTime       =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime       =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    LoadIcons(&ApplicationConfig->DefaultFFDIcon);
    return true;
}

//====================================================================================================================

void cffDProjectFile::SaveToXML(QDomElement &domDocument) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement("ffDiaporamaProjectProperties");
    Element.setAttribute("Title",Title);
    Element.setAttribute("Author",Author);
    Element.setAttribute("Album",Album);
    Element.setAttribute("Year",Year);
    Element.setAttribute("Comment",Comment);
    Element.setAttribute("Composer",Composer);
    Element.setAttribute("Duration",Duration);
    Element.setAttribute("ffDRevision",ffDRevision);
    Element.setAttribute("DefaultLanguage",DefaultLanguage);
    Element.setAttribute("ChaptersNumber",NbrChapters);
    for (int i=0;i<NbrChapters;i++) {
        QString     ChapterNum=QString("%1").arg(i); while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
        QDomElement SubElement=DomDocument.createElement("Chapter_"+ChapterNum);
        SubElement.setAttribute("Start",GetInformationValue("Chapter_"+ChapterNum+":Start"));
        SubElement.setAttribute("End",GetInformationValue("Chapter_"+ChapterNum+":End"));
        SubElement.setAttribute("Duration",GetInformationValue("Chapter_"+ChapterNum+":Duration"));
        SubElement.setAttribute("title",GetInformationValue("Chapter_"+ChapterNum+":title"));
        SubElement.setAttribute("InSlide",GetInformationValue("Chapter_"+ChapterNum+":InSlide"));
        Element.appendChild(SubElement);
    }
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cffDProjectFile::LoadFromXML(QDomElement domDocument) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::LoadFromXML");

    bool IsOk=false;
    if ((domDocument.elementsByTagName("ffDiaporamaProjectProperties").length()>0)&&(domDocument.elementsByTagName("ffDiaporamaProjectProperties").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("ffDiaporamaProjectProperties").item(0).toElement();
        if (Element.hasAttribute("Title")) {
            Title   =Element.attribute("Title");
            InformationList.append(QString("title")+QString("##")+QString(Title));
        }
        if (Element.hasAttribute("Author")) {
            Author  =Element.attribute("Author");
            InformationList.append(QString("artist")+QString("##")+QString(Author));
        }
        if (Element.hasAttribute("Album")) {
            Album   =Element.attribute("Album");
            InformationList.append(QString("album")+QString("##")+QString(Album));
        }
        if (Element.hasAttribute("Year")) {
            Year    =Element.attribute("Year").toInt();
            InformationList.append(QString("date")+QString("##")+QString("%1").arg(Year));
        }
        if (Element.hasAttribute("Comment")) {
            Comment =Element.attribute("Comment");
            InformationList.append(QString("comment")+QString("##")+QString(Comment));
        }
        if (Element.hasAttribute("ffDRevision")) {
            ffDRevision=Element.attribute("ffDRevision");
            InformationList.append(QString("ffDRevision")+QString("##")+QString(ffDRevision));
        }
        if (Element.hasAttribute("Composer")) {
            Composer=Element.attribute("Composer");
            InformationList.append(QString("composer")+QString("##")+QString(Composer));
        }
        if (Element.hasAttribute("DefaultLanguage")) {
            DefaultLanguage=Element.attribute("DefaultLanguage");
            InformationList.append(QString("Audio_000:language")+QString("##")+QString(DefaultLanguage));
        }
        if (Element.hasAttribute("Duration")) {
            Duration=Element.attribute("Duration").toLongLong();
            if (Duration!=0) {
                int     TimeMSec    =Duration-(Duration/1000)*1000;
                int     TimeSec     =int(Duration/1000);
                int     TimeHour    =TimeSec/(60*60);
                int     TimeMinute  =(TimeSec%(60*60))/60;
                QTime   tDuration;
                tDuration.setHMS(TimeHour,TimeMinute,TimeSec%60,TimeMSec);
                InformationList.append(QString("Duration")+QString("##")+tDuration.toString("HH:mm:ss.zzz"));
            }
        }
        if (Element.hasAttribute("ChaptersNumber")) {
            NbrChapters=Element.attribute("ChaptersNumber").toInt();
            for (int i=0;i<NbrChapters;i++) {
                QString     ChapterNum=QString("%1").arg(i); while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
                if ((domDocument.elementsByTagName("Chapter_"+ChapterNum).length()>0)&&(domDocument.elementsByTagName("Chapter_"+ChapterNum).item(0).isElement()==true)) {
                    QDomElement SubElement=domDocument.elementsByTagName("Chapter_"+ChapterNum).item(0).toElement();
                    QString     Start="";
                    QString     End="";
                    QString     Duration="";
                    QString     Title="";
                    QString     InSlide="";
                    if (SubElement.hasAttribute("Start"))       Start=SubElement.attribute("Start");
                    if (SubElement.hasAttribute("End"))         End=SubElement.attribute("End");
                    if (SubElement.hasAttribute("Duration"))    Duration=SubElement.attribute("Duration");
                    if (SubElement.hasAttribute("title"))       Title=SubElement.attribute("title");
                    if (SubElement.hasAttribute("InSlide"))     InSlide=SubElement.attribute("InSlide");

                    InformationList.append("Chapter_"+ChapterNum+":Start"   +QString("##")+Start);
                    InformationList.append("Chapter_"+ChapterNum+":End"     +QString("##")+End);
                    InformationList.append("Chapter_"+ChapterNum+":Duration"+QString("##")+Duration);
                    InformationList.append("Chapter_"+ChapterNum+":title"   +QString("##")+Title);
                    InformationList.append("Chapter_"+ChapterNum+":InSlide" +QString("##")+InSlide);
                }
            }
        }
        IsOk=true;
    }
    if ((domDocument.elementsByTagName("Project").length()>0)&&(domDocument.elementsByTagName("Project").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("Project").item(0).toElement();
        if (Element.hasAttribute("ImageGeometry")) {
            switch (Element.attribute("ImageGeometry").toInt()) {
                case GEOMETRY_16_9:  ObjectGeometry=IMAGE_GEOMETRY_16_9;   break;
                case GEOMETRY_40_17: ObjectGeometry=IMAGE_GEOMETRY_40_17;  break;
                case GEOMETRY_4_3:
                default:             ObjectGeometry=IMAGE_GEOMETRY_4_3;    break;
            }
        }
        if (Element.hasAttribute("ObjectNumber")) {
            NbrSlide=Element.attribute("ObjectNumber").toInt();
            InformationList.append(QApplication::translate("cBaseMediaFile","Slide number")+QString("##%1").arg(NbrSlide));
        }
    }
    return IsOk;
}

//====================================================================================================================

void cffDProjectFile::GetFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::GetFullInformationFromFile");

    QFile           file(FileName);
    QDomDocument    domDocument;
    QDomElement     root;
    QString         errorStr;
    int             errorLine,errorColumn;
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        if (domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn)) {
            root = domDocument.documentElement();
            // Load project properties
            if (root.tagName()==FFD_APPLICATION_ROOTNAME) LoadFromXML(root);
        }
        file.close();
    }
    IsInformationValide=true;
}

//====================================================================================================================

QString cffDProjectFile::GetTechInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::GetTechInfo");

    QString Info="";
    if (Composer!="")   Info=Info+(Info!=""?" - ":"")+Composer+" ("+ffDRevision+")";
    if (GetImageSizeStr(cBaseMediaFile::GEOONLY)!="")   Info=Info+(Info!=""?" - ":"")+GetImageSizeStr(cBaseMediaFile::GEOONLY);
    if (NbrSlide>0)                                     Info=Info+(Info!=""?" - ":"")+QString("%1").arg(NbrSlide)   +" "+QApplication::translate("cBaseMediaFile","Slides");
    if (NbrChapters>0)                                  Info=Info+(Info!=""?" - ":"")+QString("%1").arg(NbrChapters)+" "+QApplication::translate("cBaseMediaFile","Chapters");
    return Info;
}

//====================================================================================================================

QString cffDProjectFile::GetTAGInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::GetTechInfo");

    QString Info=Title;
    if (Album!="")      Info=Info+(Info!=""?" - ":"")+Album;
    if (Info!="")       Info=Info+(Info!=""?" - ":"")+QString("%1").arg(Year);
    if (Author!="")     Info=Info+(Info!=""?" - ":"")+Author;
    return Info;
}

//====================================================================================================================

bool cffDProjectFile::IsFilteredFile(int RequireObjectType,int AllowedObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::IsFilteredFile");

    return ((RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(RequireObjectType==OBJECTTYPE_FFDFILE))&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_FFDFILE)!=0);
}

//====================================================================================================================

QString cffDProjectFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cffDProjectFile::GetFileTypeStr for %1").arg(FileName));

    return QApplication::translate("cBaseMediaFile","ffDiaporama","File type");
}

//*********************************************************************************************************************************************
// Image file
//*********************************************************************************************************************************************

cImageFile::cImageFile(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::cImageFile");

    ObjectType  =OBJECTTYPE_IMAGEFILE;  // coul be turn later to OBJECTTYPE_THUMBNAIL
    IsVectorImg =false;
    VectorImage =NULL;
}

//====================================================================================================================

cImageFile::~cImageFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::~cImageFile");
    if (VectorImage) {
        delete VectorImage;
        VectorImage=NULL;
    }
}

//====================================================================================================================

bool cImageFile::IsFilteredFile(int RequireObjectType,int AllowedObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::IsFilteredFile");
    if (FileName.endsWith("_ffd.jpg",Qt::CaseInsensitive)) {
        return (RequireObjectType==OBJECTTYPE_UNMANAGED)&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_UNMANAGED)!=0);
    } else if (ObjectType==OBJECTTYPE_IMAGEFILE) {
        if (IsVectorImg) return ((RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(RequireObjectType==OBJECTTYPE_IMAGEFILE))&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_IMAGEVECTORFILE)!=0);
            else         return ((RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(RequireObjectType==OBJECTTYPE_IMAGEFILE))&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_IMAGEFILE)!=0);
    } else return (RequireObjectType==OBJECTTYPE_UNMANAGED)&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_UNMANAGED)!=0);
}

//====================================================================================================================

QString cImageFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cImageFile::GetFileTypeStr for %1").arg(FileName));

    if ((ObjectType==OBJECTTYPE_IMAGEFILE)&&(!IsVectorImg))         return QApplication::translate("cBaseMediaFile","Image","File type");
        else if ((ObjectType==OBJECTTYPE_IMAGEFILE)&&(IsVectorImg)) return QApplication::translate("cBaseMediaFile","Vector image","File type");
        else return QApplication::translate("cBaseMediaFile","Thumbnail","File type");
}

//====================================================================================================================

bool cImageFile::GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetInformationFromFile");
    IsVectorImg=(QFileInfo(GivenFileName).suffix().toLower()=="svg");
    return cBaseMediaFile::GetInformationFromFile(GivenFileName,AliasList,ModifyFlag);
}

//====================================================================================================================

void cImageFile::GetFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetFullInformationFromFile");

    ImageOrientation    =-1;
    IsInformationValide =false;
    bool                ExifOk=false;

    if (!IsVectorImg) {

        // ******************************************************************************************************
        // Try to load EXIF information using library exiv2
        // ******************************************************************************************************
        Exiv2::Image::AutoPtr ImageFile;
        try {
            #ifdef Q_OS_WIN
                ImageFile=Exiv2::ImageFactory::open(FileName.toLocal8Bit().data());
            #else
                ImageFile=Exiv2::ImageFactory::open(FileName.toUtf8().data());
            #endif
            ExifOk=true;
        }
        catch( Exiv2::Error& ) {
            ToLog(LOGMSG_INFORMATION,QApplication::translate("cBaseMediaFile","Image don't have EXIF metadata %1").arg(FileName));
        }
        if ((ExifOk)&&(ImageFile->good())) {
            ImageFile->readMetadata();
            // Read data
            Exiv2::ExifData &exifData = ImageFile->exifData();
            if (!exifData.empty()) {
                Exiv2::ExifData::const_iterator end = exifData.end();
                for (Exiv2::ExifData::const_iterator CurrentData=exifData.begin();CurrentData!=end;++CurrentData) {

                    if ((QString().fromStdString(CurrentData->key())=="Exif.Image.Orientation")&&(CurrentData->tag()==274))
                        ImageOrientation=QString().fromStdString(CurrentData->value().toString()).toInt();

                    if ((CurrentData->typeId()!=Exiv2::undefined)&&
                        (!(((CurrentData->typeId()==Exiv2::unsignedByte)||(CurrentData->typeId()==Exiv2::signedByte))&&(CurrentData->size()>64)))) {
                        QString Key  =QString().fromStdString(CurrentData->key());
                        #ifdef Q_OS_WIN
                        QString Value=QString().fromStdString(CurrentData->print(&exifData).c_str());
                        #else
                        QString Value=QString().fromUtf8(CurrentData->print(&exifData).c_str());
                        #endif
                        if (Key.startsWith("Exif.")) Key=Key.mid(QString("Exif.").length());
                        InformationList.append(Key+QString("##")+Value);
                    }
                }
            }

            // Append InformationList
            if (GetInformationValue("Image.Artist")!="") InformationList.append(QString("artist")+QString("##")+GetInformationValue("Image.Artist"));
            if (GetInformationValue("Image.Model")!="")  {
                if (GetInformationValue("Image.Model").contains(GetInformationValue("Image.Make"),Qt::CaseInsensitive)) InformationList.append(QString("composer")+QString("##")+GetInformationValue("Image.Model"));
                    else InformationList.append(QString("composer")+QString("##")+GetInformationValue("Image.Make")+" "+GetInformationValue("Image.Model"));
            }
            // Get size information
            ImageWidth =ImageFile->pixelWidth();
            ImageHeight=ImageFile->pixelHeight();

            //if (GetInformationValue("Photo.PixelXDimension")!="")       ImageWidth =GetInformationValue("Photo.PixelXDimension").toInt();
            //    else if (GetInformationValue("Image.ImageWidth")!="")   ImageWidth =GetInformationValue("Image.ImageWidth").toInt();            // TIFF Version
            //if (GetInformationValue("Photo.PixelYDimension")!="")       ImageHeight=GetInformationValue("Photo.PixelYDimension").toInt();
            //    else if (GetInformationValue("Image.ImageLength")!="")  ImageHeight=GetInformationValue("Image.ImageLength").toInt();           // TIFF Version

            // switch ImageWidth and ImageHeight if image was rotated
            if ((ImageOrientation==6)||(ImageOrientation==8)) {
                int IW=ImageWidth;
                ImageWidth=ImageHeight;
                ImageHeight=IW;
            }

            // Read preview image
            #ifdef EXIV2WITHPREVIEW
            if (IsIconNeeded) {
                Exiv2::PreviewManager *Manager=new Exiv2::PreviewManager(*ImageFile);
                if (Manager) {
                    Exiv2::PreviewPropertiesList Properties=Manager->getPreviewProperties();
                    if (!Properties.empty()) {
                        Exiv2::PreviewImage Image=Manager->getPreviewImage(Properties[Properties.size()-1]);      // Get the latest image (biggest)
                        QImage *Icon=new QImage();
                        if (Icon->loadFromData(QByteArray((const char*)Image.pData(),Image.size()))) {
                            if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                                QMatrix matrix;
                                matrix.rotate(-90);
                                QImage *NewImage=new QImage(Icon->transformed(matrix,Qt::SmoothTransformation));
                                delete Icon;
                                Icon=NewImage;
                            } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                                QMatrix matrix;
                                matrix.rotate(180);
                                QImage *NewImage=new QImage(Icon->transformed(matrix,Qt::SmoothTransformation));
                                delete Icon;
                                Icon=NewImage;
                            } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                                QMatrix matrix;
                                matrix.rotate(90);
                                QImage *NewImage=new QImage(Icon->transformed(matrix,Qt::SmoothTransformation));
                                delete Icon;
                                Icon=NewImage;
                            }

                            // Sometimes, Icon have black bar : try to remove them
                            if ((double(Icon->width())/double(Icon->height()))!=(double(ImageWidth)/double(ImageHeight))) {
                                if (ImageWidth>ImageHeight) {
                                    int RealHeight=int((double(Icon->width())*double(ImageHeight))/double(ImageWidth));
                                    int Delta     =Icon->height()-RealHeight;
                                    QImage *NewImage=new QImage(Icon->copy(0,Delta/2,Icon->width(),Icon->height()-Delta));
                                    delete Icon;
                                    Icon=NewImage;
                                    // if preview Icon have a really small size, then don't use it
                                    if (Icon->width()>=ApplicationConfig->MinimumEXIFHeight) LoadIcons(Icon);
                                } else {
                                    int RealWidth=int((double(Icon->height())*double(ImageWidth))/double(ImageHeight));
                                    int Delta     =Icon->width()-RealWidth;
                                    QImage *NewImage=new QImage(Icon->copy(Delta/2,0,Icon->width()-Delta,Icon->height()));
                                    delete Icon;
                                    Icon=NewImage;
                                    // if preview Icon have a really small size, then don't use it
                                    if (Icon->height()>=ApplicationConfig->MinimumEXIFHeight) LoadIcons(Icon);
                                }
                            }

                        }
                        delete Icon;
                    }
                    delete Manager;
                }
            }
            #endif
        }

    } else {
        // Vector image file
        QSvgRenderer SVGImg(FileName);
        if (SVGImg.isValid()) {
            ImageOrientation=0;
            ImageWidth      =SVGImg.viewBox().width();
            ImageHeight     =SVGImg.viewBox().height();

            QPainter Painter;
            QImage   Img;
            qreal    RatioX=(ImageWidth>ImageHeight?1:qreal(ImageWidth)/qreal(ImageHeight));
            qreal    RatioY=(ImageWidth<ImageHeight?1:qreal(ImageHeight)/qreal(ImageWidth));

            // 16x16 icon
            Img=QImage(qreal(16)*RatioX,qreal(16)*RatioY,QImage::Format_ARGB32);
            Painter.begin(&Img);
            Painter.setCompositionMode(QPainter::CompositionMode_Source);
            Painter.fillRect(QRect(0,0,Img.width(),Img.height()),Qt::transparent);
            Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            SVGImg.render(&Painter);
            Painter.end();
            Icon16=QImage(16,16,QImage::Format_ARGB32_Premultiplied);
            Painter.begin(&Icon16);
            Painter.setCompositionMode(QPainter::CompositionMode_Source);
            Painter.fillRect(QRect(0,0,Icon16.width(),Icon16.height()),Qt::transparent);
            Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            Painter.drawImage(QPoint((16-Img.width())/2,(16-Img.height())/2),Img);
            Painter.end();

            // 100x100 icon
            Img=QImage(qreal(100)*RatioX,qreal(100)*RatioY,QImage::Format_ARGB32);
            Painter.begin(&Img);
            Painter.setCompositionMode(QPainter::CompositionMode_Source);
            Painter.fillRect(QRect(0,0,Img.width(),Img.height()),Qt::transparent);
            Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            SVGImg.render(&Painter);
            Painter.end();
            Icon100=QImage(100,100,QImage::Format_ARGB32_Premultiplied);
            Painter.begin(&Icon100);
            Painter.setCompositionMode(QPainter::CompositionMode_Source);
            Painter.fillRect(QRect(0,0,Icon100.width(),Icon100.height()),Qt::transparent);
            Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            Painter.drawImage(QPoint((100-Img.width())/2,(100-Img.height())/2),Img);
            Painter.end();

            InformationList.append(QString("Photo.PixelXDimension")+QString("##")+QString("%1").arg(ImageWidth));
            InformationList.append(QString("Photo.PixelYDimension")+QString("##")+QString("%1").arg(ImageHeight));
            IsInformationValide=true;
        }
    }

    //************************************************************************************
    // If no exif preview image (of image too small) then load/create thumbnail
    //************************************************************************************
    if ((!IsVectorImg)&&(IsIconNeeded)&&(Icon16.isNull())) {
        cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileName,ModifDateTime,ImageOrientation,ApplicationConfig->Smoothing,true);
        if (ImageObject==NULL) {
            ToLog(LOGMSG_CRITICAL,"Error in cImageFile::GetFullInformationFromFile : FindObject return NULL for thumbnail creation !");
        } else {
            QImageReader ImgReader(FileName);
            if (ImgReader.canRead()) {
                QSize Size=ImgReader.size();
                if ((Size.width()>=100)||(Size.height()>=100)) {
                    if ((qreal(Size.height())/qreal(Size.width()))*100<=100) {
                        Size.setHeight((qreal(Size.height())/qreal(Size.width()))*100);
                        Size.setWidth(100);
                    } else {
                        Size.setWidth((qreal(Size.width())/qreal(Size.height()))*100);
                        Size.setHeight(100);
                    }
                    ImgReader.setScaledSize(Size);
                }
                QImage Image=ImgReader.read();
                if (Image.isNull()) ToLog(LOGMSG_CRITICAL,"QImageReader.read return error in GetFullInformationFromFile");
                    else LoadIcons(&Image);
            }
        }
    }

    //************************************************************************************
    // if no information about size then load image
    //************************************************************************************
    if ((!IsVectorImg)&&((ImageWidth==0)||(ImageHeight==0))) {
        cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileName,ModifDateTime,ImageOrientation,ApplicationConfig->Smoothing,true);
        if (ImageObject==NULL) {
            ToLog(LOGMSG_CRITICAL,"Error in cImageFile::GetFullInformationFromFile : FindObject return NULL for size computation !");
        } else {
            QImageReader Img(FileName);
            if (Img.canRead()) {
                QSize Size =Img.size();
                ImageWidth =Size.width();
                ImageHeight=Size.height();
                InformationList.append(QString("Photo.PixelXDimension")+QString("##")+QString("%1").arg(ImageWidth));
                InformationList.append(QString("Photo.PixelYDimension")+QString("##")+QString("%1").arg(ImageHeight));
                IsInformationValide=true;
            }
        }
    }

    //************************************************************************************
    // End process by computing some values ....
    //************************************************************************************

    // Sort InformationList
    InformationList.sort();

    // Now we have image size then compute image geometry
    ObjectGeometry=IMAGE_GEOMETRY_UNKNOWN;
    double RatioHW=double(ImageWidth)/double(ImageHeight);
    if ((RatioHW>=1.45)&&(RatioHW<=1.55))           ObjectGeometry=IMAGE_GEOMETRY_3_2;
    else if ((RatioHW>=0.65)&&(RatioHW<=0.67))      ObjectGeometry=IMAGE_GEOMETRY_2_3;
    else if ((RatioHW>=1.32)&&(RatioHW<=1.34))      ObjectGeometry=IMAGE_GEOMETRY_4_3;
    else if ((RatioHW>=0.74)&&(RatioHW<=0.76))      ObjectGeometry=IMAGE_GEOMETRY_3_4;
    else if ((RatioHW>=1.77)&&(RatioHW<=1.79))      ObjectGeometry=IMAGE_GEOMETRY_16_9;
    else if ((RatioHW>=0.56)&&(RatioHW<=0.58))      ObjectGeometry=IMAGE_GEOMETRY_9_16;
    else if ((RatioHW>=2.34)&&(RatioHW<=2.36))      ObjectGeometry=IMAGE_GEOMETRY_40_17;
    else if ((RatioHW>=0.42)&&(RatioHW<=0.44))      ObjectGeometry=IMAGE_GEOMETRY_17_40;

    // if Icon16 stil null then load default icon
    if ((IsIconNeeded)&&(Icon16.isNull())) LoadIcons(&ApplicationConfig->DefaultIMAGEIcon);

    IsInformationValide=true;
}

//====================================================================================================================

QString cImageFile::GetTechInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetTechInfo");

    QString Info=GetImageSizeStr(FULLWEB);
    if (GetInformationValue("artist")!="")              Info=Info+(Info!=""?"-":"")+GetInformationValue("artist");
    if (GetInformationValue("composer")!="")            Info=Info+(Info!=""?"-":"")+GetInformationValue("composer");
    if (GetInformationValue("Image.Orientation")!="")   Info=Info+(Info!=""?"-":"")+GetInformationValue("Image.Orientation");
    return Info;
}

//====================================================================================================================

QString cImageFile::GetTAGInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetTAGInfo");

    QString Info=GetInformationValue("Photo.ExposureTime");
    if (GetInformationValue("Photo.ApertureValue")!="")    Info=Info+(Info!=""?"-":"")+GetInformationValue("Photo.ApertureValue");
    if (GetInformationValue("Photo.ISOSpeedRatings")!="")  Info=Info+(Info!=""?"-":"")+GetInformationValue("Photo.ISOSpeedRatings")+" ISO";
    if (GetInformationValue("CanonCs.LensType")!="")       Info=Info+(Info!=""?"-":"")+GetInformationValue("CanonCs.LensType");                // Canon version
    if (GetInformationValue("NikonLd3.LensIDNumber")!="")  Info=Info+(Info!=""?"-":"")+GetInformationValue("NikonLd3.LensIDNumber");           // Nikon version
    if (GetInformationValue("Photo.Flash")!="")            Info=Info+(Info!=""?"-":"")+GetInformationValue("Photo.Flash");
    if (GetInformationValue("CanonCs.FlashMode")!="")      Info=Info+(Info!=""?"-":"")+GetInformationValue("CanonCs.FlashMode");               // Canon version
    if (GetInformationValue("Nikon3.FlashMode")!="")       Info=Info+(Info!=""?"-":"")+GetInformationValue("Nikon3.FlashMode");                // Nikon version
    return Info;
}

//====================================================================================================================
QImage *cImageFile::LoadVectorImg() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::ImageAt");

    if (!IsValide)            return NULL;
    if (!IsInformationValide) GetFullInformationFromFile();
    if (!IsVectorImg)         return NULL;

    // Vector image file
    QSvgRenderer SVGImg(FileName);
    QImage       *Img=NULL;
    if (SVGImg.isValid()) {
        Img=new QImage(ImageWidth,ImageHeight,QImage::Format_ARGB32_Premultiplied);
        QPainter Painter;
        Painter.begin(Img);
        Painter.setCompositionMode(QPainter::CompositionMode_Source);
        Painter.fillRect(QRect(0,0,Img->width(),Img->height()),Qt::transparent);
        Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        SVGImg.render(&Painter);
        Painter.end();
    }
    return Img;
}

//====================================================================================================================

QImage *cImageFile::ImageAt(bool PreviewMode) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::ImageAt");

    if (!IsValide)            return NULL;
    if (!IsInformationValide) GetFullInformationFromFile();
    if (IsVectorImg)          return LoadVectorImg();

    QImage                *LN_Image   =NULL;
    QImage                *RetImage   =NULL;
    cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileName,ModifDateTime,ImageOrientation,(!PreviewMode || ApplicationConfig->Smoothing),true);

    if (!ImageObject) {
        ToLog(LOGMSG_CRITICAL,"Error in cImageFile::ImageAt : FindObject return NULL !");
        return NULL;  // There is an error !!!!!
    }

    if (PreviewMode) LN_Image=ImageObject->ValidateCachePreviewImage();
        else         LN_Image=ImageObject->ValidateCacheRenderImage();

    if ((LN_Image==NULL)||(LN_Image->isNull())) {
        ToLog(LOGMSG_CRITICAL,"Error in cImageFile::ImageAt : ValidateCacheImage return NULL !");
    } else {
        RetImage=new QImage(LN_Image->copy());
        if ((RetImage==NULL)||(RetImage->isNull()))
            ToLog(LOGMSG_CRITICAL,"Error in cImageFile::ImageAt : LN_Image->copy() return NULL !");
    }

    // return wanted image
    return RetImage;
}

/*************************************************************************************************************************************
    CLASS cVideoFile
*************************************************************************************************************************************/

cImageInCache::cImageInCache(qlonglong Position,QImage *Image) {
    this->Position=Position;
    this->Image   =Image;
}

cImageInCache::~cImageInCache() {
    delete Image;
}

cVideoFile::cVideoFile(int TheWantedObjectType,cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::cVideoFile");
    Reset(TheWantedObjectType);
}

void cVideoFile::Reset(int TheWantedObjectType) {
    cBaseMediaFile::Reset();

    MusicOnly               =(TheWantedObjectType==OBJECTTYPE_MUSICFILE);
    ObjectType              =TheWantedObjectType;
    IsOpen                  =false;
    StartPos                =QTime(0,0,0,0);   // Start position
    EndPos                  =QTime(0,0,0,0);   // End position

    // Video part
    IsMTS                   =false;
    LibavFile               =NULL;
    VideoDecoderCodec       =NULL;
    VideoStreamNumber       =0;
    FrameBufferYUV          =NULL;
    FrameBufferYUVReady     =false;
    FrameBufferYUVPosition  =0;
    VideoCodecInfo          ="";
    VideoTrackNbr           =0;
    VideoStreamNumber       =-1;
    NbrChapters             =0;

    // Audio part
    LibavFile               =NULL;
    AudioDecoderCodec       =NULL;
    LastAudioReadedPosition =-1;
    IsVorbis                =false;
    AudioCodecInfo          ="";
    AudioTrackNbr           =0;
    AudioStreamNumber       =-1;

    // Audio resampling
    RSC                     =NULL;
    RSC_InChannels          =2;
    RSC_OutChannels         =2;
    RSC_InSampleRate        =48000;
    RSC_OutSampleRate       =48000;
    #ifdef LIBAV_09
    RSC_InChannelLayout     =av_get_default_channel_layout(2);
    RSC_OutChannelLayout    =av_get_default_channel_layout(2);
    #endif
    RSC_InSampleFmt         =AV_SAMPLE_FMT_S16;
    RSC_OutSampleFmt        =AV_SAMPLE_FMT_S16;

    // Filter part
    VideoFilterGraph        =NULL;
    VideoFilterIn           =NULL;
    VideoFilterOut          =NULL;
}

//====================================================================================================================

cVideoFile::~cVideoFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::~cVideoFile");

    // Close LibAVFormat and LibAVCodec contexte for the file
    CloseCodecAndFile();
}

//====================================================================================================================

// Overloaded function use to dertermine if media file correspond to WantedObjectType
//      WantedObjectType could be OBJECTTYPE_VIDEOFILE or OBJECTTYPE_MUSICFILE
//      if AudioOnly was set to true in constructor then ignore all video track and set WantedObjectType to OBJECTTYPE_MUSICFILE else set it to OBJECTTYPE_VIDEOFILE
//      return true if WantedObjectType=OBJECTTYPE_VIDEOFILE and at least one video track is present
//      return true if WantedObjectType=OBJECTTYPE_MUSICFILE and at least one audio track is present

void cVideoFile::GetFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::GetFullInformationFromFile");
    Mutex.lock();
    bool Continu=true;

    //*********************************************************************************************************
    // Open file and get a LibAVFormat context and an associated LibAVCodec decoder
    //*********************************************************************************************************
    char filename[512];
    strcpy(filename,FileName.toLocal8Bit());
    if (avformat_open_input(&LibavFile,filename,NULL,NULL)!=0) {
        LibavFile=NULL;
        Mutex.unlock();
        return;
    }
    InformationList.append(QString("Short Format##")+QString(LibavFile->iformat->name));
    InformationList.append(QString("Long Format##")+QString(LibavFile->iformat->long_name));
    LibavFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future NbrFrames.

    //*********************************************************************************************************
    // Search stream in file
    //*********************************************************************************************************
    if (avformat_find_stream_info(LibavFile,NULL)<0) {
        avformat_close_input(&LibavFile);
        LibavFile=NULL;
        Continu=false;
    }

    if (Continu) {
        //*********************************************************************************************************
        // Get metadata
        //*********************************************************************************************************
        AVDictionaryEntry *tag=NULL;
        while ((tag=av_dict_get(LibavFile->metadata,"",tag,AV_DICT_IGNORE_SUFFIX))) {
            QString Value=QString().fromUtf8(tag->value);
            #ifdef Q_OS_WIN
            Value.replace(char(13),"\n");
            #endif
            if (Value.endsWith("\n")) Value=Value.left(Value.lastIndexOf("\n"));
            InformationList.append(QString().fromUtf8(tag->key).toLower()+QString("##")+Value);
        }

        //*********************************************************************************************************
        // Get chapters
        //*********************************************************************************************************
        NbrChapters=0;
        for (uint i=0;i<LibavFile->nb_chapters;i++) {
            AVChapter   *ch=LibavFile->chapters[i];
            QString     ChapterNum=QString("%1").arg(NbrChapters);
            while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
            qlonglong Start=double(ch->start)*(double(av_q2d(ch->time_base))*1000);     // Lib AV use 1/1 000 000 000 sec and we want msec !
            qlonglong End  =double(ch->end)*(double(av_q2d(ch->time_base))*1000);       // Lib AV use 1/1 000 000 000 sec and we want msec !

            // Special case if it's first chapter and start!=0 => add a chapter 0
            if ((NbrChapters==0)&&(LibavFile->chapters[i]->start>0)) {
                InformationList.append("Chapter_"+ChapterNum+":Start"   +QString("##")+QTime(0,0,0,0).toString("hh:mm:ss.zzz"));
                InformationList.append("Chapter_"+ChapterNum+":End"     +QString("##")+QTime(0,0,0,0).addMSecs(Start).toString("hh:mm:ss.zzz"));
                InformationList.append("Chapter_"+ChapterNum+":Duration"+QString("##")+QTime(0,0,0,0).addMSecs(Start).toString("hh:mm:ss.zzz"));
                if (GetInformationValue("title")!="") InformationList.append("Chapter_"+ChapterNum+":title##"+GetInformationValue("title"));
                    else InformationList.append("Chapter_"+ChapterNum+":title##"+QFileInfo(this->FileName).baseName());
                NbrChapters++;
                ChapterNum=QString("%1").arg(NbrChapters);
                while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
            }

            InformationList.append("Chapter_"+ChapterNum+":Start"   +QString("##")+QTime(0,0,0,0).addMSecs(Start).toString("hh:mm:ss.zzz"));
            InformationList.append("Chapter_"+ChapterNum+":End"     +QString("##")+QTime(0,0,0,0).addMSecs(End).toString("hh:mm:ss.zzz"));
            InformationList.append("Chapter_"+ChapterNum+":Duration"+QString("##")+QTime(0,0,0,0).addMSecs(End-Start).toString("hh:mm:ss.zzz"));
            // Chapter metadata
            while ((tag=av_dict_get(ch->metadata,"",tag,AV_DICT_IGNORE_SUFFIX)))
                InformationList.append("Chapter_"+ChapterNum+":"+QString().fromUtf8(tag->key).toLower()+QString("##")+QString().fromUtf8(tag->value));

            NbrChapters++;
        }

        //*********************************************************************************************************
        // Get information about duration
        //*********************************************************************************************************
        int         hh,mm,ss;
        qlonglong   ms;

        ms=LibavFile->duration;//-LibavFile->start_time;
        ms=ms/1000;

        ss=ms/1000;
        mm=ss/60;
        hh=mm/60;
        mm=mm-(hh*60);
        ss=ss-(ss/60)*60;
        ms=ms-(ms/1000)*1000;
        Duration=QTime(hh,mm,ss,ms);

        InformationList.append(QString("Duration")+QString("##")+Duration.toString("HH:mm:ss.zzz"));
        EndPos=Duration;

        //*********************************************************************************************************
        // Get information from track
        //*********************************************************************************************************
        for (int Track=0;Track<(int)LibavFile->nb_streams;Track++) {

            // Find codec
            AVCodec *Codec=avcodec_find_decoder(LibavFile->streams[Track]->codec->codec_id);

            //*********************************************************************************************************
            // Audio track
            //*********************************************************************************************************
            if (LibavFile->streams[Track]->codec->codec_type==AVMEDIA_TYPE_AUDIO) {
                // Keep this as default track
                if (AudioStreamNumber==-1) AudioStreamNumber=Track;

                // Compute TrackNum
                QString TrackNum=QString("%1").arg(AudioTrackNbr);
                while (TrackNum.length()<3) TrackNum="0"+TrackNum;
                TrackNum="Audio_"+TrackNum+":";

                // General
                InformationList.append(TrackNum+QString("Track")+QString("##")+QString("%1").arg(Track));
                if (Codec) InformationList.append(TrackNum+QString("Codec")+QString("##")+QString(Codec->name));

                // Channels and Sample format
                QString SampleFMT="";
                switch (LibavFile->streams[Track]->codec->sample_fmt) {
                    case AV_SAMPLE_FMT_U8  : SampleFMT="-U8";   InformationList.append(TrackNum+QString("Sample format")+QString("##")+"unsigned 8 bits");          break;
                    case AV_SAMPLE_FMT_S16 : SampleFMT="-S16";  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"signed 16 bits");           break;
                    case AV_SAMPLE_FMT_S32 : SampleFMT="-S32";  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"signed 32 bits");           break;
                    case AV_SAMPLE_FMT_FLT : SampleFMT="-FLT";  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"float");                    break;
                    case AV_SAMPLE_FMT_DBL : SampleFMT="-DBL";  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"double");                   break;
                    case AV_SAMPLE_FMT_U8P : SampleFMT="-U8P";  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"unsigned 8 bits, planar");  break;
                    case AV_SAMPLE_FMT_S16P: SampleFMT="-S16P"; InformationList.append(TrackNum+QString("Sample format")+QString("##")+"signed 16 bits, planar");   break;
                    case AV_SAMPLE_FMT_S32P: SampleFMT="-S32P"; InformationList.append(TrackNum+QString("Sample format")+QString("##")+"signed 32 bits, planar");   break;
                    case AV_SAMPLE_FMT_FLTP: SampleFMT="-FLTP"; InformationList.append(TrackNum+QString("Sample format")+QString("##")+"float, planar");            break;
                    case AV_SAMPLE_FMT_DBLP: SampleFMT="-DBLP"; InformationList.append(TrackNum+QString("Sample format")+QString("##")+"double, planar");           break;
                    default                : SampleFMT="-?";    InformationList.append(TrackNum+QString("Sample format")+QString("##")+"Unknown");                  break;
                }
                if (LibavFile->streams[Track]->codec->channels==1)      InformationList.append(TrackNum+QString("Channels")+QString("##")+QApplication::translate("cBaseMediaFile","Mono","Audio channels mode")+SampleFMT);
                else if (LibavFile->streams[Track]->codec->channels==2) InformationList.append(TrackNum+QString("Channels")+QString("##")+QApplication::translate("cBaseMediaFile","Stereo","Audio channels mode")+SampleFMT);
                else                                                         InformationList.append(TrackNum+QString("Channels")+QString("##")+QString("%1").arg(LibavFile->streams[Track]->codec->channels)+SampleFMT);

                // Frequency
                if (int(LibavFile->streams[Track]->codec->sample_rate/1000)*1000>0) {
                    if (int(LibavFile->streams[Track]->codec->sample_rate/1000)*1000==LibavFile->streams[Track]->codec->sample_rate)
                         InformationList.append(TrackNum+QString("Frequency")+QString("##")+QString("%1").arg(int(LibavFile->streams[Track]->codec->sample_rate/1000))+"Khz");
                    else InformationList.append(TrackNum+QString("Frequency")+QString("##")+QString("%1").arg(double(LibavFile->streams[Track]->codec->sample_rate)/1000,8,'f',1).trimmed()+"Khz");
                }

                // Bitrate
                if (int(LibavFile->streams[Track]->codec->bit_rate/1000)>0) InformationList.append(TrackNum+QString("Bitrate")+QString("##")+QString("%1").arg(int(LibavFile->streams[Track]->codec->bit_rate/1000))+"Kb/s");

                // Stream metadata
                while ((tag=av_dict_get(LibavFile->streams[Track]->metadata,"",tag,AV_DICT_IGNORE_SUFFIX))) {
                    // OGV container affect TAG to audio stream !
                    QString Key=QString().fromUtf8(tag->key).toLower();
                    if ((FileName.toLower().endsWith(".ogv"))&&((Key=="title")||(Key=="artist")||(Key=="album")||(Key=="comment")||(Key=="date")||(Key=="composer")||(Key=="encoder")))
                             InformationList.append(Key+QString("##")+QString().fromUtf8(tag->value));
                        else InformationList.append(TrackNum+Key+QString("##")+QString().fromUtf8(tag->value));
                }

                // Ensure language exist (Note : AVI and FLV container own language at container level instead of track level)
                if (GetInformationValue(TrackNum+"language")=="") {
                    QString Lng=GetInformationValue("language");
                    InformationList.append(TrackNum+QString("language##")+(Lng==""?"und":Lng));
                }

                // Next
                AudioTrackNbr++;

            //*********************************************************************************************************
            // Video track
            //*********************************************************************************************************
            } else if (!MusicOnly && (LibavFile->streams[Track]->codec->codec_type==AVMEDIA_TYPE_VIDEO)) {
                // Compute TrackNum
                QString TrackNum=QString("%1").arg(VideoTrackNbr);
                while (TrackNum.length()<3) TrackNum="0"+TrackNum;
                TrackNum="Video_"+TrackNum+":";

                // General
                InformationList.append(TrackNum+QString("Track")+QString("##")+QString("%1").arg(Track));
                if (Codec) InformationList.append(TrackNum+QString("Codec")+QString("##")+QString(Codec->name));

                // Bitrate
                if (LibavFile->streams[Track]->codec->bit_rate>0) InformationList.append(TrackNum+QString("Bitrate")+QString("##")+QString("%1").arg(int(LibavFile->streams[Track]->codec->bit_rate/1000))+"Kb/s");

                // Frame rate
                if (int(double(LibavFile->streams[Track]->avg_frame_rate.num)/double(LibavFile->streams[Track]->avg_frame_rate.den))>0) {
                    if (int(double(LibavFile->streams[Track]->avg_frame_rate.num)/double(LibavFile->streams[Track]->avg_frame_rate.den))==double(LibavFile->streams[Track]->avg_frame_rate.num)/double(LibavFile->streams[Track]->avg_frame_rate.den))
                         InformationList.append(TrackNum+QString("Frame rate")+QString("##")+QString("%1").arg(int(double(LibavFile->streams[Track]->avg_frame_rate.num)/double(LibavFile->streams[Track]->avg_frame_rate.den)))+" FPS");
                    else InformationList.append(TrackNum+QString("Frame rate")+QString("##")+QString("%1").arg(double(double(LibavFile->streams[Track]->avg_frame_rate.num)/double(LibavFile->streams[Track]->avg_frame_rate.den)),8,'f',3).trimmed()+" FPS");
                }

                // Stream metadata
                while ((tag=av_dict_get(LibavFile->streams[Track]->metadata,"",tag,AV_DICT_IGNORE_SUFFIX)))
                    InformationList.append(TrackNum+QString(tag->key)+QString("##")+QString().fromUtf8(tag->value));

                // Ensure language exist (Note : AVI ‘AttachedPictureFrame’and FLV container own language at container level instead of track level)
                if (GetInformationValue(TrackNum+"language")=="") {
                    QString Lng=GetInformationValue("language");
                    InformationList.append(TrackNum+QString("language##")+(Lng==""?"und":Lng));
                }

                // Keep this as default track
                if (VideoStreamNumber==-1) {
                    QImage *Img=NULL;

                    // Search if a jukebox mode thumbnail (jpg file with same name as video) exist
                    QFileInfo   File(FileName);
                    QString     JPegFile=File.absolutePath()+(File.absolutePath().endsWith(QDir::separator())?"":QString(QDir::separator()))+File.completeBaseName()+".jpg";
                    if (QFileInfo(JPegFile).exists()) LoadIcons(JPegFile);

                    VideoStreamNumber=Track;
                    IsMTS=(FileName.endsWith(".mts",Qt::CaseInsensitive) || FileName.endsWith(".m2ts",Qt::CaseInsensitive));
                    LibavFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future NbrFrames.
                    LibavFile->streams[VideoStreamNumber]->discard=AVDISCARD_DEFAULT;  // Setup STREAM options

                    // Find the decoder for the video stream and open it
                    VideoDecoderCodec=avcodec_find_decoder(LibavFile->streams[VideoStreamNumber]->codec->codec_id);

                    // Setup decoder options
                    LibavFile->streams[VideoStreamNumber]->codec->debug_mv         =0;                    // Debug level (0=nothing)
                    LibavFile->streams[VideoStreamNumber]->codec->debug            =0;                    // Debug level (0=nothing)
                    LibavFile->streams[VideoStreamNumber]->codec->workaround_bugs  =1;                    // Work around bugs in encoders which sometimes cannot be detected automatically : 1=autodetection
                    LibavFile->streams[VideoStreamNumber]->codec->idct_algo        =FF_IDCT_AUTO;         // IDCT algorithm, 0=auto
                    LibavFile->streams[VideoStreamNumber]->codec->skip_frame       =AVDISCARD_DEFAULT;    // ???????
                    LibavFile->streams[VideoStreamNumber]->codec->skip_idct        =AVDISCARD_DEFAULT;    // ???????
                    LibavFile->streams[VideoStreamNumber]->codec->skip_loop_filter =AVDISCARD_DEFAULT;    // ???????
                    LibavFile->streams[VideoStreamNumber]->codec->error_concealment=3;
                    LibavFile->streams[VideoStreamNumber]->codec->thread_count     =getCpuCount();
                    LibavFile->streams[VideoStreamNumber]->codec->thread_type      =getThreadFlags(LibavFile->streams[VideoStreamNumber]->codec->codec_id);

                    // Hack to correct wrong frame rates that seem to be generated by some codecs
                    if (LibavFile->streams[VideoStreamNumber]->codec->time_base.num>1000 && LibavFile->streams[VideoStreamNumber]->codec->time_base.den==1) LibavFile->streams[VideoStreamNumber]->codec->time_base.den=1000;

                    if (avcodec_open2(LibavFile->streams[VideoStreamNumber]->codec,VideoDecoderCodec,NULL)>=0) {
                        // Get Aspect Ratio

                        AspectRatio=double(LibavFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.num)/double(LibavFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.den);

                        if (LibavFile->streams[VideoStreamNumber]->sample_aspect_ratio.num!=0)
                            AspectRatio=double(LibavFile->streams[VideoStreamNumber]->sample_aspect_ratio.num)/double(LibavFile->streams[VideoStreamNumber]->sample_aspect_ratio.den);

                        if (AspectRatio==0) AspectRatio=1;

                        // Special case for DVD mode video without PAR
                        if ((AspectRatio==1)&&(LibavFile->streams[VideoStreamNumber]->codec->coded_width==720)&&((LibavFile->streams[VideoStreamNumber]->codec->coded_height==576)||(LibavFile->streams[VideoStreamNumber]->codec->coded_height==480)))
                            AspectRatio=double((LibavFile->streams[VideoStreamNumber]->codec->coded_height/3)*4)/720;

                        // Try to load one image to be sure we can make something with this file
                        // and use this first image as thumbnail (if no jukebox thumbnail)
                        qlonglong   Position =0;
                        double      dEndFile =double(QTime(0,0,0,0).msecsTo(Duration))/1000;    // End File Position in double format
                        if (dEndFile!=0) {
                            // Allocate structure for YUV image
                            if (FrameBufferYUV==NULL) FrameBufferYUV=avcodec_alloc_frame();
                            if (FrameBufferYUV!=NULL) {
                                FrameBufferYUVReady    = false;
                                FrameBufferYUVPosition = 0;

                                AVStream    *VideoStream    =LibavFile->streams[VideoStreamNumber];
                                AVPacket    *StreamPacket   =NULL;
                                bool        Continue        =true;
                                bool        IsVideoFind     =false;
                                double      FrameTimeBase   =av_q2d(VideoStream->time_base);
                                double      FramePosition   =0;

                                while (Continue) {
                                    StreamPacket=new AVPacket();
                                    av_init_packet(StreamPacket);
                                    StreamPacket->flags|=AV_PKT_FLAG_KEY;  // HACK for CorePNG to decode as normal PNG by default
                                    if (av_read_frame(LibavFile,StreamPacket)==0) {
                                        if (StreamPacket->stream_index==VideoStreamNumber) {
                                            int FrameDecoded=0;
                                            if (avcodec_decode_video2(VideoStream->codec,FrameBufferYUV,&FrameDecoded,StreamPacket)<0)
                                                ToLog(LOGMSG_INFORMATION,"IN:cVideoFile::OpenCodecAndFile : avcodec_decode_video2 return an error");
                                            if (FrameDecoded>0) {
                                                int64_t pts=AV_NOPTS_VALUE;
                                                if ((FrameBufferYUV->pkt_dts==(int64_t)AV_NOPTS_VALUE)&&(FrameBufferYUV->pkt_pts!=(int64_t)AV_NOPTS_VALUE)) pts=FrameBufferYUV->pkt_pts; else pts=FrameBufferYUV->pkt_dts;
                                                if (pts==(int64_t)AV_NOPTS_VALUE) pts=0;
                                                FramePosition         =double(pts)*FrameTimeBase;
                                                FrameBufferYUVReady   =true;                                // Keep actual value for FrameBufferYUV
                                                FrameBufferYUVPosition=int(FramePosition*AV_TIME_BASE);     // Keep actual value for FrameBufferYUV
                                                Img                   =ConvertYUVToRGB(false);              // Create Img from YUV Buffer
                                                IsVideoFind           =(Img!=NULL)&&(!Img->isNull());
                                                ObjectGeometry        =IMAGE_GEOMETRY_UNKNOWN;
                                            }
                                        }
                                        // Check if we need to continue loop
                                        Continue=(IsVideoFind==false)&&(FramePosition<dEndFile);
                                    } else {
                                        // if error in av_read_frame(...) then may be we have reach the end of file !
                                        Continue=false;
                                    }
                                    // Continue with a new one
                                    if (StreamPacket!=NULL) {
                                        av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
                                        delete StreamPacket;
                                        StreamPacket=NULL;
                                    }
                                }

                                if ((!IsVideoFind)&&(!Img)) {
                                    ToLog(LOGMSG_CRITICAL,QString("No video image return for position %1 => return black frame").arg(Position));
                                    Img=new QImage(LibavFile->streams[VideoStreamNumber]->codec->width,LibavFile->streams[VideoStreamNumber]->codec->height,QImage::Format_ARGB32_Premultiplied);
                                    Img->fill(0);
                                }
                            } else ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::OpenCodecAndFile : Impossible to allocate FrameBufferYUV");
                        } else ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::OpenCodecAndFile : dEndFile=0 ?????");
                    }
                    if (Img) {
                        IsOpen=true;
                        // Get information about size image
                        ImageWidth =Img->width();
                        ImageHeight=Img->height();
                        // Compute image geometry
                        ObjectGeometry=IMAGE_GEOMETRY_UNKNOWN;
                        double RatioHW=double(ImageWidth)/double(ImageHeight);
                        if ((RatioHW>=1.45)&&(RatioHW<=1.55))           ObjectGeometry=IMAGE_GEOMETRY_3_2;
                        else if ((RatioHW>=0.65)&&(RatioHW<=0.67))      ObjectGeometry=IMAGE_GEOMETRY_2_3;
                        else if ((RatioHW>=1.32)&&(RatioHW<=1.34))      ObjectGeometry=IMAGE_GEOMETRY_4_3;
                        else if ((RatioHW>=0.74)&&(RatioHW<=0.76))      ObjectGeometry=IMAGE_GEOMETRY_3_4;
                        else if ((RatioHW>=1.77)&&(RatioHW<=1.79))      ObjectGeometry=IMAGE_GEOMETRY_16_9;
                        else if ((RatioHW>=0.56)&&(RatioHW<=0.58))      ObjectGeometry=IMAGE_GEOMETRY_9_16;
                        else if ((RatioHW>=2.34)&&(RatioHW<=2.36))      ObjectGeometry=IMAGE_GEOMETRY_40_17;
                        else if ((RatioHW>=0.42)&&(RatioHW<=0.44))      ObjectGeometry=IMAGE_GEOMETRY_17_40;
                        // Icon
                        if (Icon16.isNull()) {
                            QImage Final=(ApplicationConfig->Video_ThumbWidth==162?ApplicationConfig->VideoMask_162:ApplicationConfig->Video_ThumbWidth==150?ApplicationConfig->VideoMask_150:ApplicationConfig->VideoMask_120).copy();
                            QImage ImgF;
                            if (Img->width()>Img->height()) ImgF=Img->scaledToWidth(ApplicationConfig->Video_ThumbWidth-2,Qt::SmoothTransformation);
                                else                        ImgF=Img->scaledToHeight(ApplicationConfig->Video_ThumbHeight*0.7,Qt::SmoothTransformation);
                            QPainter Painter;
                            Painter.begin(&Final);
                            Painter.drawImage(QRect((Final.width()-ImgF.width())/2,(Final.height()-ImgF.height())/2,ImgF.width(),ImgF.height()),ImgF);
                            Painter.end();
                            LoadIcons(&Final);
                        }
                        delete Img;
                    }
                }

                // Next
                VideoTrackNbr++;
            }
            //*********************************************************************************************************
            // Thumbnails (since lavf 54.2.0 - avformat.h)
            //*********************************************************************************************************
            #ifdef LIBAV_09
            if (LibavFile->streams[Track]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
                AVStream *ThumbStream=LibavFile->streams[Track];
                AVPacket pkt         =ThumbStream->attached_pic;
                int      FrameDecoded=0;
                AVFrame  *FrameYUV   =avcodec_alloc_frame();
                if (FrameYUV) {

                    AVCodec *ThumbDecoderCodec=avcodec_find_decoder(ThumbStream->codec->codec_id);

                    // Setup decoder options
                    ThumbStream->codec->debug_mv         =0;                    // Debug level (0=nothing)
                    ThumbStream->codec->debug            =0;                    // Debug level (0=nothing)
                    ThumbStream->codec->workaround_bugs  =1;                    // Work around bugs in encoders which sometimes cannot be detected automatically : 1=autodetection
                    ThumbStream->codec->idct_algo        =FF_IDCT_AUTO;         // IDCT algorithm, 0=auto
                    ThumbStream->codec->skip_frame       =AVDISCARD_DEFAULT;    // ???????
                    ThumbStream->codec->skip_idct        =AVDISCARD_DEFAULT;    // ???????
                    ThumbStream->codec->skip_loop_filter =AVDISCARD_DEFAULT;    // ???????
                    ThumbStream->codec->error_concealment=3;
                    ThumbStream->codec->thread_count     =getCpuCount();
                    ThumbStream->codec->thread_type      =getThreadFlags(ThumbStream->codec->codec_id);
                    if (avcodec_open2(ThumbStream->codec,ThumbDecoderCodec,NULL)>=0) {
                        if ((avcodec_decode_video2(ThumbStream->codec,FrameYUV,&FrameDecoded,&pkt)>=0)&&(FrameDecoded>0)) {
                            int     W       =FrameYUV->width;
                            int     H       =FrameYUV->height;
                            QImage  Thumbnail(W,H,QTPIXFMT);
                            AVFrame *FrameRGB=avcodec_alloc_frame();
                            if ((FrameRGB)&&(!Thumbnail.isNull())) {
                                avpicture_fill((AVPicture *)FrameRGB,Thumbnail.bits(),PIXFMT,W,H);
                                struct SwsContext *img_convert_ctx=sws_getContext(FrameYUV->width,FrameYUV->height,(PixelFormat)FrameYUV->format,W,H,PIXFMT,SWS_FAST_BILINEAR,NULL,NULL,NULL);
                                if (img_convert_ctx!=NULL) {
                                    int ret = sws_scale(img_convert_ctx,FrameYUV->data,FrameYUV->linesize,0,FrameYUV->height,FrameRGB->data,FrameRGB->linesize);
                                    if (ret>0) {
                                        // sws_scaler truncate the width of the images to a multiple of 8. So cut resulting image to comply a multiple of 8
                                        Thumbnail=Thumbnail.copy(0,0,(Thumbnail.width()/8)*8,Thumbnail.height());
                                        LoadIcons(&Thumbnail);
                                    }
                                    sws_freeContext(img_convert_ctx);
                                }
                            }
                            if (FrameRGB) avcodec_free_frame(&FrameRGB);
                        }
                        avcodec_close(ThumbStream->codec);
                    }
                }
                if (FrameYUV) avcodec_free_frame(&FrameYUV);
            }
            #endif
        }

        IsInformationValide=true;

        #ifdef LIBAV_08
        // If it's an audio file, try to get embeded image with taglib
        if ((IsIconNeeded)&&(Icon16.isNull())&&(ObjectType==OBJECTTYPE_MUSICFILE)) {
            QImage *Img=GetEmbededImage(FileName);
            if (Img) {
                LoadIcons(Img);
                delete Img;
            }
        }
        #endif

        // if no icon then load default for type
        if ((IsIconNeeded)&&(Icon16.isNull()))
                LoadIcons(ObjectType==OBJECTTYPE_VIDEOFILE?&ApplicationConfig->DefaultVIDEOIcon:&ApplicationConfig->DefaultMUSICIcon);
    }
    Mutex.unlock();
    CloseCodecAndFile();
}

//====================================================================================================================

bool cVideoFile::IsFilteredFile(int RequireObjectType,int AllowedObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::IsFilteredFile");

    if (ObjectType==OBJECTTYPE_MUSICFILE) return ((RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(ObjectType==RequireObjectType))&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_MUSICFILE)!=0);
        else                              return ((RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(ObjectType==RequireObjectType))&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_VIDEOFILE)!=0);
}

//====================================================================================================================

QString cVideoFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cVideoFile::GetFileTypeStr for %1").arg(FileName));

    if (MusicOnly || (ObjectType==OBJECTTYPE_MUSICFILE)) return QApplication::translate("cBaseMediaFile","Music","File type");
        else return QApplication::translate("cBaseMediaFile","Video","File type");
}

//====================================================================================================================

QImage *cVideoFile::GetDefaultTypeIcon(cCustomIcon::IconSize Size) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::GetDefaultTypeIcon");

    if (MusicOnly || (ObjectType==OBJECTTYPE_MUSICFILE)) return ApplicationConfig->DefaultMUSICIcon.GetIcon(Size);
        else return ApplicationConfig->DefaultVIDEOIcon.GetIcon(Size);
}

//====================================================================================================================

QString cVideoFile::GetTechInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::GetTechInfo");

    QString Info="";
    if (ObjectType==OBJECTTYPE_MUSICFILE) {
        Info=GetCumulInfoStr("Audio","Codec");
        if (GetCumulInfoStr("Audio","Channels")!="")       Info=Info+(Info!=""?"-":"")+GetCumulInfoStr("Audio","Channels");
        if (GetCumulInfoStr("Audio","Bitrate")!="")        Info=Info+(Info!=""?"-":"")+GetCumulInfoStr("Audio","Bitrate");
        if (GetCumulInfoStr("Audio","Frequency")!="")      Info=Info+(Info!=""?"-":"")+GetCumulInfoStr("Audio","Frequency");
    } else {
        Info=GetImageSizeStr();
        if (GetCumulInfoStr("Video","Codec")!="")          Info=Info+(Info!=""?"-":"")+GetCumulInfoStr("Video","Codec");
        if (GetCumulInfoStr("Video","Frame rate")!="")     Info=Info+(Info!=""?"-":"")+GetCumulInfoStr("Video","Frame rate");
        if (GetCumulInfoStr("Video","Bitrate")!="")        Info=Info+(Info!=""?"-":"")+GetCumulInfoStr("Video","Bitrate");

        int     Num     =0;
        QString TrackNum="";
        QString Value   ="";
        QString SubInfo ="";
        do {
            TrackNum=QString("%1").arg(Num);
            while (TrackNum.length()<3) TrackNum="0"+TrackNum;
            TrackNum="Audio_"+TrackNum+":";
            Value=GetInformationValue(TrackNum+"language");
            if (Value!="") {
                if (Num==0) Info=Info+"-"; else Info=Info+"/";
                SubInfo=GetInformationValue(TrackNum+"Codec");
                if (GetInformationValue(TrackNum+"Channels")!="")  SubInfo=SubInfo+(Info!=""?"-":"")+GetInformationValue(TrackNum+"Channels");
                if (GetInformationValue(TrackNum+"Bitrate")!="")   SubInfo=SubInfo+(Info!=""?"-":"")+GetInformationValue(TrackNum+"Bitrate");
                if (GetInformationValue(TrackNum+"Frequency")!="") SubInfo=SubInfo+(Info!=""?"-":"")+GetInformationValue(TrackNum+"Frequency");
                Info=Info+Value+"("+SubInfo+")";
            }
            // Next
            Num++;
        } while (Value!="");
    }
    return Info;
}

//====================================================================================================================

QString cVideoFile::GetTAGInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::GetTAGInfo");

    QString Info=GetInformationValue("track");
    if (GetInformationValue("title")!="")          Info=Info+(Info!=""?"-":"")+GetInformationValue("title");
    if (GetInformationValue("artist")!="")         Info=Info+(Info!=""?"-":"")+GetInformationValue("artist");
    if (GetInformationValue("album")!="")          Info=Info+(Info!=""?"-":"")+GetInformationValue("album");
    if (GetInformationValue("date")!="")           Info=Info+(Info!=""?"-":"")+GetInformationValue("date");
    if (GetInformationValue("genre")!="")          Info=Info+(Info!=""?"-":"")+GetInformationValue("genre");
    return Info;
}

//====================================================================================================================
// Close LibAVFormat and LibAVCodec contexte for the file
//====================================================================================================================

void cVideoFile::CloseCodecAndFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::CloseCodecAndFile");
    Mutex.lock();

    while (CacheImage.count()>0) delete(CacheImage.takeLast());

    // Close the resampling context
    CloseResampler();

    #if !defined(USELIBSWRESAMPLE)
    // Close the filter context
    if (VideoFilterGraph) VideoFilter_Close();
    #endif

    // Close the video codec
    if (VideoDecoderCodec!=NULL) {
        avcodec_close(LibavFile->streams[VideoStreamNumber]->codec);
        VideoDecoderCodec=NULL;
    }

    // Close the audio codec
    if (AudioDecoderCodec!=NULL) {
        avcodec_close(LibavFile->streams[AudioStreamNumber]->codec);
        AudioDecoderCodec=NULL;
    }

    // Close the libav file
    if (LibavFile!=NULL) {
        avformat_close_input(&LibavFile);
        LibavFile=NULL;
    }

    if (FrameBufferYUV!=NULL) {
        if (FrameBufferYUV->opaque) {
            avfilter_unref_buffer((AVFilterBufferRef *)FrameBufferYUV->opaque);
            FrameBufferYUV->opaque=NULL;
        }
        avcodec_free_frame(&FrameBufferYUV);
        FrameBufferYUV=NULL;
    }
    FrameBufferYUVReady=false;
    IsOpen=false;
    Mutex.unlock();
}

//*********************************************************************************************************************

void cVideoFile::CloseResampler() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::CloseResampler");
    if (RSC) {
        #if defined(LIBAV_08)
            audio_resample_close(RSC);
        #elif defined(USELIBAVRESAMPLE)
            avresample_close(RSC);
            avresample_free(&RSC);
        #elif defined(USELIBSWRESAMPLE)
            swr_free(&RSC);
        #else
            audio_resample_close(RSC);
        #endif
        RSC=NULL;
    }
}

//*********************************************************************************************************************

void cVideoFile::CheckResampler(int RSC_InChannels,int RSC_OutChannels,AVSampleFormat RSC_InSampleFmt,AVSampleFormat RSC_OutSampleFmt,int RSC_InSampleRate,int RSC_OutSampleRate
                                           #ifdef LIBAV_09
                                               ,uint64_t RSC_InChannelLayout,uint64_t RSC_OutChannelLayout
                                           #endif
                                      ) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::CheckResampler");
    #ifdef LIBAV_09
    if (RSC_InChannelLayout==0)  RSC_InChannelLayout =av_get_default_channel_layout(RSC_InChannels);
    if (RSC_OutChannelLayout==0) RSC_OutChannelLayout=av_get_default_channel_layout(RSC_OutChannels);
    #endif
    if ((RSC!=NULL)&&
        ( (RSC_InChannels!=this->RSC_InChannels)    ||(RSC_OutChannels!=this->RSC_OutChannels)
        ||(RSC_InSampleFmt!=this->RSC_InSampleFmt)  ||(RSC_OutSampleFmt!=this->RSC_OutSampleFmt)
        ||(RSC_InSampleRate!=this->RSC_InSampleRate)||(RSC_OutSampleRate!=this->RSC_OutSampleRate)
        #ifdef LIBAV_09
        ||(RSC_InChannelLayout!=this->RSC_InChannelLayout)||(RSC_OutChannelLayout!=this->RSC_OutChannelLayout)
        #endif
       )) CloseResampler();
    if (!RSC) {
        this->RSC_InChannels   =RSC_InChannels;
        this->RSC_OutChannels  =RSC_OutChannels;
        this->RSC_InSampleFmt  =RSC_InSampleFmt;
        this->RSC_OutSampleFmt =RSC_OutSampleFmt;
        this->RSC_InSampleRate =RSC_InSampleRate;
        this->RSC_OutSampleRate=RSC_OutSampleRate;

        #if defined(LIBAV_08)
            RSC=av_audio_resample_init(    // Context for resampling audio data
                RSC_OutChannels,RSC_InChannels,             // output_channels, input_channels
                RSC_OutSampleRate,RSC_InSampleRate,         // output_rate, input_rate
                RSC_OutSampleFmt,RSC_InSampleFmt,           // sample_fmt_out, sample_fmt_in
                0,                                          // filter_length
                0,                                          // log2_phase_count
                1,                                          // linear
                0);                                         // cutoff
            if (!RSC) ToLog(LOGMSG_CRITICAL,QString("CheckResampler: av_audio_resample_init failed"));
        #elif defined(USELIBAVRESAMPLE)
            this->RSC_InChannelLayout =RSC_InChannelLayout;
            this->RSC_OutChannelLayout=RSC_OutChannelLayout;
            RSC=avresample_alloc_context();
            if (RSC) {
                av_opt_set_int(RSC,"in_channel_layout",     RSC_InChannelLayout, 0);
                av_opt_set_int(RSC,"in_sample_rate",        RSC_InSampleRate,    0);
                av_opt_set_int(RSC,"out_channel_layout",    RSC_OutChannelLayout,0);
                av_opt_set_int(RSC,"out_sample_rate",       RSC_OutSampleRate,   0);
                av_opt_set_int(RSC,"in_sample_fmt",         RSC_InSampleFmt,     0);
                av_opt_set_int(RSC,"out_sample_fmt",        RSC_OutSampleFmt,    0);
                if (avresample_open(RSC)<0) {
                    ToLog(LOGMSG_CRITICAL,QString("CheckResampler: avresample_open failed"));
                    avresample_free(&RSC);
                    RSC=NULL;
                }
            }
            if (!RSC) ToLog(LOGMSG_CRITICAL,QString("CheckResampler: avresample_alloc_context failed"));
        #elif defined(USELIBSWRESAMPLE)
            this->RSC_InChannelLayout =RSC_InChannelLayout;
            this->RSC_OutChannelLayout=RSC_OutChannelLayout;
            /*RSC=swr_alloc_set_opts(NULL,RSC_OutChannelLayout,RSC_OutSampleFmt,RSC_OutSampleRate,
                                        RSC_InChannelLayout, RSC_InSampleFmt, RSC_InSampleRate,
                                        0, NULL);*/
            RSC=swr_alloc();
            av_opt_set_int(RSC,"in_channel_layout",     RSC_InChannelLayout, 0);
            av_opt_set_int(RSC,"in_sample_rate",        RSC_InSampleRate,    0);
            av_opt_set_int(RSC,"out_channel_layout",    RSC_OutChannelLayout,0);
            av_opt_set_int(RSC,"out_sample_rate",       RSC_OutSampleRate,   0);
            av_opt_set_int(RSC,"in_channel_count",      RSC_InChannels,      0);
            av_opt_set_int(RSC,"out_channel_count",     RSC_OutChannels,     0);
            #if (LIBAVUTIL_VERSION_INT>=AV_VERSION_INT(52,9,100))
            av_opt_set_sample_fmt(RSC,"in_sample_fmt",  RSC_InSampleFmt,     0);
            av_opt_set_sample_fmt(RSC,"out_sample_fmt", RSC_OutSampleFmt,    0);
            #else
            av_opt_set_int(RSC,"in_sample_fmt",         RSC_InSampleFmt,     0);
            av_opt_set_int(RSC,"out_sample_fmt",        RSC_OutSampleFmt,    0);
            #endif
            if ((RSC)&&(swr_init(RSC)<0)) {
                ToLog(LOGMSG_CRITICAL,QString("CheckResampler: swr_init failed"));
                if (RSC) {
                    swr_free(&RSC);
                    RSC=NULL;
                }
            }
            if (!RSC) ToLog(LOGMSG_CRITICAL,QString("CheckResampler: swr_alloc_set_opts failed"));
        #else
            RSC=av_audio_resample_init(    // Context for resampling audio data
                RSC_OutChannels,RSC_InChannels,             // output_channels, input_channels
                RSC_OutSampleRate,RSC_InSampleRate,         // output_rate, input_rate
                RSC_OutSampleFmt,RSC_InSampleFmt,           // sample_fmt_out, sample_fmt_in
                0,                                          // filter_length
                0,                                          // log2_phase_count
                1,                                          // linear
                0);                                         // cutoff
            if (!RSC) ToLog(LOGMSG_CRITICAL,QString("CheckResampler: av_audio_resample_init failed"));
        #endif
    }
}

//*********************************************************************************************************************
// VIDEO FILTER PART : This code was adapt from xbmc sources files
//*********************************************************************************************************************
unsigned int cVideoFile::SetFilters(unsigned int flags) {
    m_filters_next.clear();

    if (flags & FILTER_DEINTERLACE_YADIF) {
        if (flags & FILTER_DEINTERLACE_HALFED)  m_filters_next="yadif=0:-1";
            else                                m_filters_next="yadif=1:-1";

        if (flags & FILTER_DEINTERLACE_FLAGGED) m_filters_next += ":1";
        flags &= ~FILTER_DEINTERLACE_ANY | FILTER_DEINTERLACE_YADIF;
    }
    return flags;
}

int cVideoFile::VideoFilter_Open(QString filters) {
    int result;

    if (VideoFilterGraph) VideoFilter_Close();

    if (filters.isEmpty()) return 0;

    if (!(VideoFilterGraph=avfilter_graph_alloc())) {
        ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : unable to alloc filter graph"));
        return -1;
    }

    VideoFilterGraph->scale_sws_opts = av_strdup("flags=4");

    QString args=QString("%1:%2:%3:%4:%5:%6:%7")
        .arg(LibavFile->streams[VideoStreamNumber]->codec->width)
        .arg(LibavFile->streams[VideoStreamNumber]->codec->height)
        .arg(LibavFile->streams[VideoStreamNumber]->codec->pix_fmt)
        .arg(LibavFile->streams[VideoStreamNumber]->codec->time_base.num)
        .arg(LibavFile->streams[VideoStreamNumber]->codec->time_base.den)
        .arg(LibavFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.num)
        .arg(LibavFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.den);

    #if LIBAVFILTER_VERSION_INT < AV_VERSION_INT(2,60,0)    // from 2.13 to 2.60

        AVFilter *srcFilter=avfilter_get_by_name("buffer");
        AVFilter *outFilter=avfilter_get_by_name("nullsink");

        if ((result=avfilter_graph_create_filter(&VideoFilterIn,srcFilter,"src",args.toLocal8Bit().constData(),NULL,VideoFilterGraph))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_create_filter: src"));
            return result;
        }
        if ((result=avfilter_graph_create_filter(&VideoFilterOut,outFilter,"out",NULL,NULL,VideoFilterGraph))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_create_filter: out"));
            return result;
        }

        outputs = (AVFilterInOut *)av_malloc(sizeof(AVFilterInOut));
        inputs  = (AVFilterInOut *)av_malloc(sizeof(AVFilterInOut));

    #elif LIBAVFILTER_VERSION_INT < AV_VERSION_INT(3,1,0)     // from 2.60 to 3.1

        AVFilter *srcFilter=avfilter_get_by_name("buffer");
        AVFilter *outFilter=avfilter_get_by_name("buffersink");

        if ((result=avfilter_graph_create_filter(&VideoFilterIn,srcFilter,"src",args.toLocal8Bit().constData(),NULL,VideoFilterGraph))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_create_filter: src"));
            return result;
        }
        std::vector<PixelFormat> m_formats;
        m_formats.push_back(PIX_FMT_YUVJ420P);
        m_formats.push_back(PIX_FMT_NONE);      /* always add none to get a terminated list in Libav world */
        AVBufferSinkParams *buffersink_params=av_buffersink_params_alloc();
        buffersink_params->pixel_fmts=&m_formats[0];
        #ifdef FF_API_OLD_VSINK_API
        if ((result=avfilter_graph_create_filter(&VideoFilterOut,outFilter,"out",NULL,(void*)buffersink_params->pixel_fmts,VideoFilterGraph))<0) {
        #else
        if ((result=avfilter_graph_create_filter(&VideoFilterOut,outFilter,"out",NULL,buffersink_params,VideoFilterGraph))<0) {
        #endif
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_create_filter: out"));
            return result;
        }
        av_free(buffersink_params);
        outputs=avfilter_inout_alloc();
        inputs =avfilter_inout_alloc();

    #else                                                   // from 3.1

        AVFilter *srcFilter=avfilter_get_by_name("buffer");
        AVFilter *outFilter=avfilter_get_by_name("buffersink");

        //if ((result=avfilter_graph_create_filter(&VideoFilterIn,srcFilter,"src",args.toLocal8Bit().constData(),NULL,VideoFilterGraph))<0) {
        if ((result=avfilter_graph_create_filter(&VideoFilterIn,srcFilter,"in",args.toLocal8Bit().constData(),NULL,VideoFilterGraph))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_create_filter: src"));
            return result;
        }
        if ((result=avfilter_graph_create_filter(&VideoFilterOut,outFilter,"out",NULL,NULL,VideoFilterGraph))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_create_filter: out"));
            return result;
        }
        outputs=avfilter_inout_alloc();
        inputs =avfilter_inout_alloc();

    #endif

    outputs->name = av_strdup("in");
    outputs->filter_ctx = VideoFilterIn;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = VideoFilterOut;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    #if LIBAVFILTER_VERSION_INT < AV_VERSION_INT(2,23,0)
    if ((result=avfilter_graph_parse(VideoFilterGraph,m_filters.toLocal8Bit().constData(),inputs,outputs,NULL))<0) {
    #elif LIBAVFILTER_VERSION_INT < AV_VERSION_INT(3,1,0)
    if ((result=avfilter_graph_parse(VideoFilterGraph,m_filters.toLocal8Bit().constData(),&inputs,&outputs,NULL))<0) {
    #elif LIBAVFILTER_VERSION_INT < AV_VERSION_INT(3,17,0)
    if ((result=avfilter_graph_parse(VideoFilterGraph,m_filters.toLocal8Bit().constData(),inputs,outputs,NULL))<0) {
    #else
    if ((result=avfilter_graph_parse(VideoFilterGraph,m_filters.toLocal8Bit().constData(),&inputs,&outputs,NULL))<0) {
    #endif
        ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_parse"));
        return result;
    }

    #if ((LIBAVFILTER_VERSION_INT>=AV_VERSION_INT(2,23,0))&&(LIBAVFILTER_VERSION_INT<AV_VERSION_INT(3,1,0)))
    avfilter_inout_free(&outputs);  // since 3.1, do not dispose them
    avfilter_inout_free(&inputs);
    #endif

    if ((result=avfilter_graph_config(VideoFilterGraph,NULL))<0) {
        ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_config"));
        return result;
    }
    return result;
}

void cVideoFile::VideoFilter_Close() {
    if (VideoFilterGraph) avfilter_graph_free(&VideoFilterGraph);
    VideoFilterGraph=NULL;
    VideoFilterIn =NULL;
    VideoFilterOut=NULL;
    outputs=NULL;
    inputs=NULL;
}

int cVideoFile::VideoFilter_Process() {
    #if LIBAVFILTER_VERSION_INT < AV_VERSION_INT(2,60,0)             // from 2.13 to 2.60

        #if LIBAVFILTER_VERSION_INT < AV_VERSION_INT(2,23,0)         // from 2.13 to 2.23
        int Ret=av_vsrc_buffer_add_frame(VideoFilterIn,FrameBufferYUV,FrameBufferYUV->pts,LibavFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio);
        #else                                                       // from 2.23 to 2.60
        int Ret=av_vsrc_buffer_add_frame(VideoFilterIn,FrameBufferYUV,0);
        #endif
        if (Ret<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Process : av_vsrc_buffer_add_frame"));
            return VC_ERROR;
        }
        int NbrFrames;
        while ((NbrFrames=avfilter_poll_frame(VideoFilterOut->inputs[0]))>0) {
            if (VideoFilterOut->inputs[0]->cur_buf) {
                avfilter_unref_buffer(VideoFilterOut->inputs[0]->cur_buf);
                VideoFilterOut->inputs[0]->cur_buf = NULL;
            }

            if ((Ret=avfilter_request_frame(VideoFilterOut->inputs[0]))<0) {
                ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Process : avfilter_request_frame : %1").arg(Ret));
                return VC_ERROR;
            }

            FrameBufferYUV->repeat_pict      = -(NbrFrames - 1);
            FrameBufferYUV->interlaced_frame = VideoFilterOut->inputs[0]->cur_buf->video->interlaced;
            FrameBufferYUV->top_field_first  = VideoFilterOut->inputs[0]->cur_buf->video->top_field_first;

        }

        if (VideoFilterOut->inputs[0]->cur_buf) {
            if (FrameBufferYUV->opaque) avfilter_unref_buffer((AVFilterBufferRef *)FrameBufferYUV->opaque);
            FrameBufferYUV->opaque=(void *)avfilter_ref_buffer(VideoFilterOut->inputs[0]->cur_buf,AV_PERM_READ);
        }

    #elif LIBAVFILTER_VERSION_INT < AV_VERSION_INT(3,0,0)             // from 2.60 to 3.0

        int Ret=av_vsrc_buffer_add_frame(VideoFilterIn,FrameBufferYUV,0);
        if (Ret<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Process : av_vsrc_buffer_add_frame"));
            return VC_ERROR;
        }
        int NbrFrames;
        if ((NbrFrames=av_buffersink_poll_frame(VideoFilterOut))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Process : av_buffersink_poll_frame"));
            return VC_ERROR;
        }
        while (NbrFrames>0) {
            AVFilterBufferRef *m_pBufferRef=NULL;
            Ret=av_buffersink_get_buffer_ref(VideoFilterOut,&m_pBufferRef,0);
            if (!m_pBufferRef) {
                ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Process : cur_buf"));
                return VC_ERROR;
            }
            FrameBufferYUV->repeat_pict     =-(NbrFrames-1);
            FrameBufferYUV->interlaced_frame=m_pBufferRef->video->interlaced;
            FrameBufferYUV->top_field_first =m_pBufferRef->video->top_field_first;
            memcpy(FrameBufferYUV->linesize,m_pBufferRef->linesize,4*sizeof(int));
            memcpy(FrameBufferYUV->data,    m_pBufferRef->data,    4*sizeof(u_int8_t*));
            NbrFrames--;
            if (m_pBufferRef) {
                avfilter_unref_buffer(m_pBufferRef);
                m_pBufferRef = NULL;
            }
        }

    #else
        int Ret=av_buffersrc_add_frame(VideoFilterIn,FrameBufferYUV,0);
        if (Ret<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Process : av_buffersrc_add_frame"));
            return VC_ERROR;
        }
        int NbrFrames;
        if ((NbrFrames=av_buffersink_poll_frame(VideoFilterOut))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Process : av_buffersink_poll_frame"));
            return VC_ERROR;
        }
        while (NbrFrames>0) {
            AVFilterBufferRef *m_pBufferRef=NULL;
            Ret=av_buffersink_get_buffer_ref(VideoFilterOut,&m_pBufferRef,0);
            if (!m_pBufferRef) {
                ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Process : cur_buf"));
                return VC_ERROR;
            }
            FrameBufferYUV->repeat_pict     =-(NbrFrames-1);
            FrameBufferYUV->interlaced_frame=m_pBufferRef->video->interlaced;
            FrameBufferYUV->top_field_first =m_pBufferRef->video->top_field_first;
            NbrFrames--;
            if (m_pBufferRef) {
                if (FrameBufferYUV->opaque) avfilter_unref_buffer((AVFilterBufferRef *)FrameBufferYUV->opaque);
                FrameBufferYUV->opaque=(void *)avfilter_ref_buffer(m_pBufferRef,AV_PERM_READ);
                avfilter_unref_buffer(m_pBufferRef);
                m_pBufferRef = NULL;
            }
        }
    #endif
    return VC_BUFFER;
}

//====================================================================================================================

bool cVideoFile::SeekFile(AVStream *VideoStream,AVStream *AudioStream,int64_t Position,bool Deinterlace) {
    bool ret=true;
    int64_t FPSDuration=int64_t((VideoStream?(double(VideoStream->r_frame_rate.den*AV_TIME_BASE)/double(VideoStream->r_frame_rate.num)):
                                (AudioStream?(double(AudioStream->r_frame_rate.den*AV_TIME_BASE)/double(AudioStream->r_frame_rate.num))*AV_TIME_BASE:
                                 0)));
    if (LibavFile->start_time>0) Position-=LibavFile->start_time; else Position-=2*FPSDuration;
    if (Position<0) Position=0;

    // Seek to nearest previous key frame on video track (if video)
    if (VideoStream) {
        while (CacheImage.count()>0) delete(CacheImage.takeLast());

        int64_t seek_target=av_rescale_q(Position,AV_TIME_BASE_Q,VideoStream->time_base);
        if (seek_target<0) seek_target=0;
        if (avformat_seek_file(LibavFile,VideoStreamNumber,INT64_MIN,seek_target,INT64_MAX,AVSEEK_FLAG_BACKWARD)<0) {
            // Try in AVSEEK_FLAG_ANY mode
            if (avformat_seek_file(LibavFile,VideoStreamNumber,INT64_MIN,seek_target,INT64_MAX,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_ANY)<0) {
                // Try with default stream if exist
                int DefaultStream=av_find_default_stream_index(LibavFile);
                if ((Position>0)||(DefaultStream<0)||(avformat_seek_file(LibavFile,DefaultStream,INT64_MIN,0,INT64_MAX,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_BYTE)<0)) {
                    ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::ReadFrame : Seek error");
                    ret=false;
                }
            }
        }
        // Flush LibAV buffers
        for (unsigned int i=0;i<LibavFile->nb_streams;i++)  {
            AVCodecContext *codec_context = LibavFile->streams[i]->codec;
            if (codec_context && codec_context->codec) avcodec_flush_buffers(codec_context);
        }
        // Close resampler
        if (AudioStream) CloseResampler();
        #if !defined(USELIBSWRESAMPLE)
        // Close video filter
        if (VideoStream) VideoFilter_Close();
        #endif

        bool     Continue        =true;
        AVPacket *StreamPacket   =NULL;
        bool     DontRetryReading=false;

        while (Continue) {
            StreamPacket=new AVPacket();
            av_init_packet(StreamPacket);
            StreamPacket->flags|=AV_PKT_FLAG_KEY;  // HACK for CorePNG to decode as normal PNG by default

            if (av_read_frame(LibavFile,StreamPacket)>=0) {

                double  TimeBase=double(LibavFile->streams[StreamPacket->stream_index]->time_base.den)/double(LibavFile->streams[StreamPacket->stream_index]->time_base.num);
                if (StreamPacket->stream_index==VideoStreamNumber) {

                    int Ret=DecodeVideoFrame(VideoStream,AudioStream,StreamPacket,Deinterlace,&DontRetryReading,Position,FPSDuration);
                    if (Ret>0) {
                        int64_t pts=FrameBufferYUV->pkt_pts;
                        if (pts==(int64_t)AV_NOPTS_VALUE) {
                            if (FrameBufferYUV->pkt_dts!=(int64_t)AV_NOPTS_VALUE) pts=FrameBufferYUV->pkt_dts; else pts=0;
                        }
                        pts=int64_t((double(pts)/TimeBase)*AV_TIME_BASE);
                        Continue=Continue && pts<(Position-FPSDuration);

                    } else if (Ret<0) Continue=false;
                }

            } else Continue=false;

            if (StreamPacket!=NULL) {
                av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
                delete StreamPacket;
                StreamPacket=NULL;
            }

        }

    // else seek au audio track
    } else if (AudioStream) {
        int64_t seek_target=av_rescale_q(Position,AV_TIME_BASE_Q,AudioStream->time_base);
        if (seek_target<0) seek_target=0;
        if (av_seek_frame(LibavFile,AudioStreamNumber,seek_target,AVSEEK_FLAG_BACKWARD)<0) {
            // Try in AVSEEK_FLAG_ANY mode
            if (av_seek_frame(LibavFile,AudioStreamNumber,seek_target,AVSEEK_FLAG_ANY)<0) {
                ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::ReadAudioFrame : Seek error");
                ret=false;
            }
        }
        // Flush LibAV buffers
        for (unsigned int i=0;i<LibavFile->nb_streams;i++)  {
            AVCodecContext *codec_context = LibavFile->streams[i]->codec;
            if (codec_context && codec_context->codec) avcodec_flush_buffers(codec_context);
        }
        // Close resampler
        if (AudioStream) CloseResampler();
        #if !defined(USELIBSWRESAMPLE)
        // Close video filter
        if (VideoStream) VideoFilter_Close();
        #endif
    }
    return ret;
}

//====================================================================================================================

u_int8_t *cVideoFile::Resample(AVFrame *Frame,int64_t *SizeDecoded,int DstSampleSize) {
    u_int8_t *Data=NULL;
    #if defined(LIBAV_08)
        Data=(u_int8_t *)av_malloc(MaxAudioLenDecoded);
        if (Data) *SizeDecoded=audio_resample(RSC,(short int*)Data,(short int*)Frame->data[0],Frame->nb_samples)*DstSampleSize;
    #elif defined(USELIBAVRESAMPLE)
        u_int8_t *in_data[RESAMPLE_MAX_CHANNELS]={0};
        int     in_linesize=0;
        Data=Frame->data[0];
        if (av_samples_fill_arrays(in_data,&in_linesize,(u_int8_t *)Frame->data[0],RSC_InChannels,Frame->nb_samples,RSC_InSampleFmt,1)<0) {
            ToLog(LOGMSG_CRITICAL,QString("failed in_data fill arrays"));
        } else {
            u_int8_t *out_data[RESAMPLE_MAX_CHANNELS]={0};
            int      out_linesize=0;
            int      out_samples=avresample_available(RSC)+av_rescale_rnd(avresample_get_delay(RSC)+Frame->nb_samples,RSC_OutSampleRate,RSC_InSampleRate,AV_ROUND_UP);
            if (av_samples_alloc(&Data,&out_linesize,RSC_OutChannels,out_samples,RSC_OutSampleFmt,1)<0) {
                ToLog(LOGMSG_CRITICAL,QString("av_samples_alloc failed"));
            } else if (av_samples_fill_arrays(out_data,&out_linesize,Data,RSC_OutChannels,out_samples,RSC_OutSampleFmt,1)<0) {
                ToLog(LOGMSG_CRITICAL,QString("failed out_data fill arrays"));
            } else {
                #if (LIBAVRESAMPLE_VERSION_INT<AV_VERSION_INT(1,0,0))
                *SizeDecoded=avresample_convert(RSC,(void **)out_data,out_linesize,out_samples,(void **)in_data,in_linesize,Frame->nb_samples)*DstSampleSize;
                #else
                *SizeDecoded=avresample_convert(RSC,out_data,out_linesize,out_samples,in_data,in_linesize,Frame->nb_samples)*DstSampleSize;
                #endif
            }
        }
    #elif defined(USELIBSWRESAMPLE)
        Data=(u_int8_t *)av_malloc(MaxAudioLenDecoded);
        u_int8_t *out[]={Data};
        if (Data) *SizeDecoded=swr_convert(RSC,out,MaxAudioLenDecoded/DstSampleSize,(const u_int8_t **)Frame->data,Frame->nb_samples)*DstSampleSize;
    #else
        Data=(u_int8_t *)av_malloc(MaxAudioLenDecoded);
        if (Data) *SizeDecoded=audio_resample(RSC,(short int*)Data,(short int*)Frame->data[0],Frame->nb_samples)*DstSampleSize;
    #endif
    return Data;
}

//====================================================================================================================
// Read a frame from current stream
//====================================================================================================================

// Remark: Position must use AV_TIMEBASE Unit
QImage *cVideoFile::ReadFrame(bool PreviewMode,qlonglong Position,bool DontUseEndPos,bool Deinterlace,cSoundBlockList *SoundTrackBloc,double Volume,bool ForceSoundOnly) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ReadFrame");

    // Ensure file was previously open
    if ((!IsOpen)&&(!OpenCodecAndFile())) return NULL;

    // Ensure file have an end file Position
    double dEndFile=double(QTime(0,0,0,0).msecsTo(DontUseEndPos?Duration:EndPos))/1000;
    if (dEndFile==0) {
        ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::ReadFrame : dEndFile=0 ?????");
        return NULL;
    }
    if (Position<0) Position=0;

    AVStream *AudioStream =((AudioStreamNumber>=0)&&SoundTrackBloc?LibavFile->streams[AudioStreamNumber]:NULL);
    AVStream *VideoStream =((!MusicOnly)&&(!ForceSoundOnly)&&(VideoStreamNumber>=0)?LibavFile->streams[VideoStreamNumber]:NULL);

    if ((!AudioStream)&&(!VideoStream)) return NULL;

    int64_t  DstSampleSize=SoundTrackBloc?(SoundTrackBloc->SampleBytes*SoundTrackBloc->Channels):0;
    int64_t  FPSSize      =SoundTrackBloc?SoundTrackBloc->SoundPacketSize*SoundTrackBloc->NbrPacketForFPS:0;
    int64_t  FPSDuration  =FPSSize?(double(FPSSize)/(SoundTrackBloc->Channels*SoundTrackBloc->SampleBytes*SoundTrackBloc->SamplingRate))*AV_TIME_BASE:0;

    if (!FPSDuration) {
        if (PreviewMode) FPSDuration=double(AV_TIME_BASE)/((cBaseApplicationConfig *)ApplicationConfig)->PreviewFPS;
            else if (VideoStream) FPSDuration=double(VideoStream->r_frame_rate.den*AV_TIME_BASE)/double(VideoStream->r_frame_rate.num);
            else FPSDuration=double(AV_TIME_BASE)/double(SoundTrackBloc->SamplingRate);
    }
    // If position >= end of file : disable audio
    double dPosition=double(Position)/AV_TIME_BASE;
    if ((dPosition>0)&&(dPosition>=dEndFile)) {
        AudioStream=NULL; // Disable audio
        // Check if last image is ready and correspond to end of file
        if ((!LastImage.isNull())&&(FrameBufferYUVReady)&&(FrameBufferYUVPosition>=dEndFile*AV_TIME_BASE-FPSDuration)) return new QImage(LastImage.copy());
        // If not then change Position to end file - a FPS to prepare a last image
        Position=dEndFile*AV_TIME_BASE-FPSDuration;
        dPosition=double(Position)/AV_TIME_BASE;
    }

    //================================================

    QImage   *RetImage         =NULL;
    AVPacket *StreamPacket     =NULL;
    bool     Continue          =true;
    double   FrameDuration     =0;
    bool     NeedResampling    =false;
    int64_t  AudioLenWanted    =0;
    int64_t  AudioLenDecoded   =0;
    double   FramePosition     =dPosition;
    bool     ResamplingContinue=(Position!=0);

    // Allocate structures
    if (VideoStream) {
        if (FrameBufferYUV==NULL) FrameBufferYUV=avcodec_alloc_frame();
        if (FrameBufferYUV==NULL) return NULL;
    }
    if (AudioStream) {
        NeedResampling=((AudioStream->codec->sample_fmt!=AV_SAMPLE_FMT_S16)||
                        (AudioStream->codec->channels!=SoundTrackBloc->Channels)||
                        (AudioStream->codec->sample_rate!=SoundTrackBloc->SamplingRate));
    }

    //*************************************************************************************************************************************
    Mutex.lock();
    //*************************************************************************************************************************************
    // SEEK
    //*************************************************************************************************************************************

    // Compute difftime between asked position and previous end decoded position
    int64_t DiffTimePosition=-1000000;
    if (AudioStream) {
        int64_t CurSize=SoundTrackBloc->SoundPacketSize*SoundTrackBloc->List.count()+SoundTrackBloc->CurrentTempSize;
        int64_t CurDur =double(CurSize*AV_TIME_BASE)/(SoundTrackBloc->SamplingRate*DstSampleSize);
        if ((Position>=SoundTrackBloc->CurrentPosition)&&(Position<=SoundTrackBloc->CurrentPosition+CurDur)) {
            int64_t UsableSize=CurSize-((double(Position-SoundTrackBloc->CurrentPosition)/AV_TIME_BASE)*SoundTrackBloc->SamplingRate*DstSampleSize);
            AudioLenWanted  =UsableSize<FPSSize?FPSSize-UsableSize:0;
            DiffTimePosition=0;
        } else {
            AudioLenWanted=FPSSize;
            if (Position<SoundTrackBloc->CurrentPosition) DiffTimePosition=-1;
                else DiffTimePosition=LastAudioReadedPosition-Position;
        }
    } else
        if (FrameBufferYUVReady)
            DiffTimePosition=Position-FrameBufferYUVPosition;

    // Calc if we need to seek to a position
    if ((Position==0)||(DiffTimePosition<0/*-1000*/)||(DiffTimePosition>1500000)) {// Allow 1,5 sec diff (rounded double !)
        if (Position<0) Position=0;
        SeekFile(VideoStream,AudioStream,Position,Deinterlace); // Always seek one FPS before to ensure eventual filter have time to init
        FramePosition=Position/AV_TIME_BASE;
        if (VideoStream) {
            while (CacheImage.count()>0) delete(CacheImage.takeLast());
            FrameBufferYUVReady   =false;
            FrameBufferYUVPosition=0;
        }
        if (AudioStream) {
            SoundTrackBloc->ClearList();      // Clear soundtrack list
            AudioLenWanted    =SoundTrackBloc->SoundPacketSize*SoundTrackBloc->NbrPacketForFPS;
            ResamplingContinue=false;
        }
    }

    // Prepare resampler
    if (NeedResampling) {
        if (!ResamplingContinue) CloseResampler();
        CheckResampler(AudioStream->codec->channels,SoundTrackBloc->Channels,
                       AudioStream->codec->sample_fmt,SoundTrackBloc->SampleFormat,
                       AudioStream->codec->sample_rate,SoundTrackBloc->SamplingRate
                       #ifdef LIBAV_09
                       ,AudioStream->codec->channel_layout
                       ,av_get_default_channel_layout(SoundTrackBloc->Channels)
                       #endif
                       );
    }

    bool IsVideoFind=false;
    int  i=0;
    while (i<CacheImage.count()) {
        if ((CacheImage[i]->Position>=Position)&&(CacheImage[i]->Position<=Position+FPSDuration)) IsVideoFind=true;
        i++;
    }

    //*************************************************************************************************************************************
    // Decoding process : Get StreamPacket until endposition is reach (if sound is wanted) or until image is ok (if image only is wanted)
    //*************************************************************************************************************************************

    bool ByPassFirstImage=(Deinterlace)&&(CacheImage.count()==0);
    bool DontRetryReading=false;

    // Check if we need to continue loop
    Continue=Continue && (
                ((VideoStream)&&(!IsVideoFind))||
                ((AudioStream)&&(AudioLenDecoded<AudioLenWanted))
             );

    while (Continue) {

        StreamPacket=new AVPacket();
        av_init_packet(StreamPacket);
        StreamPacket->flags|=AV_PKT_FLAG_KEY;  // HACK for CorePNG to decode as normal PNG by default

        if (av_read_frame(LibavFile,StreamPacket)<0) {

            //************************
            // If error reading frame
            //************************
            if (AudioLenWanted>AudioLenDecoded) {
                u_int8_t *BufferForDecoded=(u_int8_t *)av_malloc(AudioLenWanted-AudioLenDecoded);
                memset(BufferForDecoded,0,AudioLenWanted-AudioLenDecoded);
                SoundTrackBloc->AppendData(int64_t(dEndFile*AV_TIME_BASE),(int16_t*)BufferForDecoded,AudioLenWanted-AudioLenDecoded);
                av_free(BufferForDecoded);
                AudioLenDecoded=AudioLenWanted;
            }
            if ((!LastImage.isNull())&&(FrameBufferYUVReady)&&(FrameBufferYUVPosition>=(dEndFile-1.5)*AV_TIME_BASE)) {
                RetImage=new QImage(LastImage);
                Continue=false;
            } else {
                if (Position<FPSDuration) {
                    Position=FPSDuration;
                    FramePosition=double(Position)/AV_TIME_BASE;
                }
                // Seek FPSDuration before and modify var to try to loop again
                Continue=SeekFile(VideoStream,AudioStream,Position-FPSDuration,Deinterlace);
                if (dEndFile==double(QTime(0,0,0,0).msecsTo(EndPos))) EndPos=QTime(0,0,0).addMSecs(FramePosition*1000);
                dEndFile               =FramePosition;
                //Duration               =QTime(0,0,0).addMSecs(FramePosition*1000);
                Position               =FramePosition-FPSDuration;
                dPosition              =double(Position)/AV_TIME_BASE;
                FramePosition          =dPosition;
                FrameBufferYUVReady    =false;
                FrameBufferYUVPosition =0;
            }

        } else {

            int64_t FramePts=StreamPacket->pts!=(int64_t)AV_NOPTS_VALUE?StreamPacket->pts:-1;
            double  TimeBase=double(LibavFile->streams[StreamPacket->stream_index]->time_base.den)/double(LibavFile->streams[StreamPacket->stream_index]->time_base.num);
            if (FramePts>=0) FramePosition=(double(FramePts)/TimeBase);

            if ((VideoStream)&&(StreamPacket->stream_index==VideoStreamNumber)) {

                //******************************************************************
                // VIDEO PART
                //******************************************************************

                if ((DecodeVideoFrame(VideoStream,AudioStream,StreamPacket,Deinterlace,&DontRetryReading,Position,FPSDuration)>0)&&(FramePosition>=dPosition)) {
                    if (ByPassFirstImage) {
                        ByPassFirstImage=false;
                    } else {
                        int64_t pts=FrameBufferYUV->pkt_pts;
                        if (pts==(int64_t)AV_NOPTS_VALUE) {
                            if (FrameBufferYUV->pkt_dts!=(int64_t)AV_NOPTS_VALUE) pts=FrameBufferYUV->pkt_dts; else pts=0;
                        }
                        FrameBufferYUVReady   =true;                                            // Keep actual value for FrameBufferYUV
                        FrameBufferYUVPosition=int64_t((double(pts)/TimeBase)*AV_TIME_BASE);    // Keep actual value for FrameBufferYUV
                        CacheImage.append(new cImageInCache(FrameBufferYUVPosition,ConvertYUVToRGB(PreviewMode)));

                        // Count number of image > position
                        int Nbr=0;
                        for (int CNbr=0;CNbr<CacheImage.count();CNbr++) if (CacheImage[CNbr]->Position>=Position) Nbr++;
                        IsVideoFind=Nbr>=4; // For h264 codec, image are not in the good order, so we have to take at least 4 frames to be sure we have all image we want
                    }
                }

            } else if ((AudioStream)&&(StreamPacket->stream_index==AudioStreamNumber)&&(StreamPacket->size>0)) {

                //******************************************************************
                // AUDIO PART
                //******************************************************************

                AVPacket PacketTemp;
                av_init_packet(&PacketTemp);
                PacketTemp.data=StreamPacket->data;
                PacketTemp.size=StreamPacket->size;

                // NOTE: the audio packet can contain several NbrFrames
                while ((Continue)&&(PacketTemp.size>0)) {
                    AVFrame *Frame=avcodec_alloc_frame();
                    int     got_frame;
                    int     Len=avcodec_decode_audio4(AudioStream->codec,Frame,&got_frame,&PacketTemp);
                    if (Len<0) {
                        // if error, we skip the frame and exit the while loop
                        PacketTemp.size=0;
                        Continue=false;
                    } else if (got_frame>0) {

                        // Get data (resample if needed)
                        int64_t  pts=((qreal(AudioStream->cur_pkt.pts)*qreal(AudioStream->time_base.num))/qreal(AudioStream->time_base.den))*1000000;
                        FramePosition=qreal(pts)/1000000;

                        int64_t  SizeDecoded=0;
                        u_int8_t *Data      =NULL;
                        if ((NeedResampling)&&(RSC!=NULL)) {
                            Data=Resample(Frame,&SizeDecoded,DstSampleSize);
                        } else {
                            Data=Frame->data[0];
                            #if defined(LIBAV_08)
                            SizeDecoded=Frame->nb_samples*av_get_bytes_per_sample(AudioStream->codec->sample_fmt)*AudioStream->codec->channels;
                            #elif defined(USELIBAVRESAMPLE)
                            SizeDecoded=av_samples_get_buffer_size(NULL,AudioStream->codec->channels,Frame->nb_samples,AudioStream->codec->sample_fmt,0);
                            #elif defined(USELIBSWRESAMPLE)
                            SizeDecoded=Frame->nb_samples*av_get_bytes_per_sample(AudioStream->codec->sample_fmt)*AudioStream->codec->channels;
                            #endif
                        }
                        Continue=(Data!=NULL);

                        if (Continue) {
                            // Adjust FrameDuration with real Nbr Sample
                            FrameDuration=double(SizeDecoded)/SoundTrackBloc->SamplingRate*DstSampleSize;

                            // Adjust volume if master volume <>1
                            if (Volume!=1) {
                                int16_t *Buf1=(int16_t*)Data;
                                int32_t mix;
                                for (int j=0;j<SizeDecoded/4;j++) {
                                    // Left channel : Adjust if necessary (16 bits)
                                    mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
                                    // Right channel : Adjust if necessary (16 bits)
                                    mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
                                }
                            }
                            // Sync audio/video (if it was not done before)
                            if ((!CacheImage.isEmpty())&&(!SoundTrackBloc->Adjusted))
                                SoundTrackBloc->AdjustSoundPosition(pts>0?pts:Position,CacheImage.first()->Position);
                            // Append decoded data to SoundTrackBloc
                            SoundTrackBloc->AppendData(Position,(int16_t*)Data,SizeDecoded);
                            AudioLenDecoded+=SizeDecoded;
                            PacketTemp.data+=Len;
                            PacketTemp.size-=Len;
                            FramePosition  =FramePosition+FrameDuration;
                        }

                        LastAudioReadedPosition =int64_t(FramePosition*AV_TIME_BASE);    // Keep NextPacketPosition for determine next time if we need to seek
                        if (Data!=Frame->data[0]) av_free(Data);
                    }
                    avcodec_free_frame(&Frame);
                }
            }
        }

        // Check if we need to continue loop
        Continue=Continue && (((VideoStream)&&(!IsVideoFind))||((AudioStream)&&(AudioLenDecoded<AudioLenWanted)));

        // Continue with a new one
        if (StreamPacket!=NULL) {
            av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
            delete StreamPacket;
            StreamPacket=NULL;
        }
    }

    //*************************************************************************************************************************************
    Mutex.unlock();
    //*************************************************************************************************************************************

    if (VideoStream) {
        if (!RetImage) {
            int i=0;
            while ((i<CacheImage.count())&&(CacheImage[i]->Position<Position)) i++;
            if ((i<CacheImage.count())&&(CacheImage[i]->Position>=Position)) RetImage=new QImage(CacheImage[i]->Image->copy());
        }
        if (!RetImage) {
            ToLog(LOGMSG_CRITICAL,QString("No video image return for position %1 => return black frame").arg(Position));
            RetImage =new QImage(LibavFile->streams[VideoStreamNumber]->codec->width,LibavFile->streams[VideoStreamNumber]->codec->height,QImage::Format_ARGB32_Premultiplied);
            RetImage->fill(0);
        }
        int     i=0;
        while (i<CacheImage.count()) {
            if (CacheImage[i]->Position<Position-50000) delete(CacheImage.takeAt(i));
                else i++;
        }
    }

    return RetImage;
}

//==============================================

int cVideoFile::DecodeVideoFrame(AVStream *VideoStream,AVStream *AudioStream,AVPacket *StreamPacket,bool Deinterlace,bool *DontRetryReading,int64_t Position,int64_t FPSDuration) {
    if ((FrameBufferYUV)&&(FrameBufferYUV->opaque)) {
        avfilter_unref_buffer((AVFilterBufferRef *)FrameBufferYUV->opaque);
        FrameBufferYUV->opaque=NULL;
    }

    int FrameDecoded=0;
    LastLibAvMessageLevel=0;    // Clear LastLibAvMessageLevel : some decoder dont return error but display errors messages !
    int Error=avcodec_decode_video2(VideoStream->codec,FrameBufferYUV,&FrameDecoded,StreamPacket);
    if ((Error<0)||(LastLibAvMessageLevel==LOGMSG_CRITICAL)) {
        if ((!*DontRetryReading)&&(Position>FPSDuration)) {
            ToLog(LOGMSG_INFORMATION,"IN:cVideoFile::ReadFrame : error avcodec_decode_video2 return an error. Try to read before.");
            SeekFile(VideoStream,AudioStream,Position-2*FPSDuration,Deinterlace);
            *DontRetryReading=true;
        } else ToLog(LOGMSG_INFORMATION,"IN:cVideoFile::ReadFrame : avcodec_decode_video2 return an error");
        #if !defined(USELIBSWRESAMPLE)
        // Close video filter
        if (VideoStream) VideoFilter_Close();
        #endif
        return -1;
    } else if (FrameDecoded>0) {
        // Video filter part
        if ((Deinterlace)&&(VideoFilterGraph==NULL)) {
            SetFilters(FILTER_DEINTERLACE_YADIF);
            m_filters=m_filters_next;
            VideoFilter_Open(m_filters);
        } else if ((!Deinterlace)&&(VideoFilterGraph!=NULL)) VideoFilter_Close();
        if ((VideoFilterGraph)&&(Deinterlace)) VideoFilter_Process();
        return 1;
    }
    return 0;
}

//====================================================================================================================

QImage *cVideoFile::ConvertYUVToRGB(bool PreviewMode) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ConvertYUVToRGB");

    int W=FrameBufferYUV->width*AspectRatio;    W-=(W%4);  // W must be a multiple of 4 ????
    int H=FrameBufferYUV->height;

    // Reduce image size for preview mode
    /*if (PreviewMode && (H>576)) {
        if ((H==1088)&&(W=1920)) {
            W=960;
            H=542;
        } else {
            W=540*(double(W)/double(H));
            H=540;
        }
    }    // H=540
    */

    LastImage=QImage(W,H,QTPIXFMT);
    AVFrame *FrameBufferRGB =avcodec_alloc_frame();  // Allocate structure for RGB image

    if (FrameBufferRGB!=NULL) {

        avpicture_fill(
                (AVPicture *)FrameBufferRGB,        // Buffer to prepare
                LastImage.bits(),                   // Buffer which will contain the image data
                PIXFMT,                             // The format in which the picture data is stored (see http://wiki.aasimon.org/doku.php?id=Libav:pixelformat)
                W,                                  // The width of the image in pixels
                H                                   // The height of the image in pixels
        );

        // Get a converter from libswscale
        struct SwsContext *img_convert_ctx=sws_getContext(
            FrameBufferYUV->width,                                                  // Src width
            FrameBufferYUV->height,                                                 // Src height
            (PixelFormat)FrameBufferYUV->format,                                    // Src Format
            W,                                                                      // Destination width
            H,                                                                      // Destination height
            PIXFMT,                                                                 // Destination Format
            SWS_BICUBIC,NULL,NULL,NULL);                                            // flags,src Filter,dst Filter,param

        if (img_convert_ctx!=NULL) {
            int ret;
            if (FrameBufferYUV->opaque) {
                AVFilterBufferRef *Buf=(AVFilterBufferRef *)FrameBufferYUV->opaque;
                ret = sws_scale(
                    img_convert_ctx,                                                    // libswscale converter
                    Buf->data,                                                          // Source buffer
                    Buf->linesize,                                                      // Source Stride ?
                    0,                                                                  // Source SliceY:the position in the source image of the slice to process, that is the number (counted starting from zero) in the image of the first row of the slice
                    FrameBufferYUV->height,                                             // Source SliceH:the height of the source slice, that is the number of rows in the slice
                    FrameBufferRGB->data,                                               // Destination buffer
                    FrameBufferRGB->linesize                                            // Destination Stride
                );
            } else {
                ret = sws_scale(
                    img_convert_ctx,                                                    // libswscale converter
                    FrameBufferYUV->data,                                               // Source buffer
                    FrameBufferYUV->linesize,                                           // Source Stride ?
                    0,                                                                  // Source SliceY:the position in the source image of the slice to process, that is the number (counted starting from zero) in the image of the first row of the slice
                    FrameBufferYUV->height,                                             // Source SliceH:the height of the source slice, that is the number of rows in the slice
                    FrameBufferRGB->data,                                               // Destination buffer
                    FrameBufferRGB->linesize                                            // Destination Stride
                );
            }
            if (ret>0) {
                // Auto crop image if 1088 format
                if ((ApplicationConfig->Crop1088To1080)&&(LastImage.height()==1088)&&(LastImage.width()==1920))  LastImage=LastImage.copy(0,4,1920,1080);
                // Reduce image size for preview mode
                if ((PreviewMode)&&(LastImage.height()>ApplicationConfig->MaxVideoPreviewHeight)) LastImage=LastImage.scaledToHeight(ApplicationConfig->MaxVideoPreviewHeight);
            }
            sws_freeContext(img_convert_ctx);
        }

        // free FrameBufferRGB because we don't need it in the future
        avcodec_free_frame(&FrameBufferRGB);
    }


    //return FinalImage;
    return new QImage(LastImage.copy());
}

//====================================================================================================================
//DontUseEndPos default=false
QImage *cVideoFile::ImageAt(bool PreviewMode,qlonglong Position,cSoundBlockList *SoundTrackBloc,bool Deinterlace,
                            double Volume,bool ForceSoundOnly,bool DontUseEndPos) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ImageAt");

    if (!IsValide) return NULL;
    if (!IsOpen) OpenCodecAndFile();

    // Load a video frame
    return ReadFrame(PreviewMode,Position*1000,DontUseEndPos,Deinterlace,SoundTrackBloc,Volume,ForceSoundOnly);
}

//====================================================================================================================

int cVideoFile::getThreadFlags(CodecID codecId) {
    int Ret=0;
    switch (codecId) {
        case CODEC_ID_PRORES:
        case CODEC_ID_MPEG1VIDEO:
        case CODEC_ID_DVVIDEO:
        case CODEC_ID_MPEG2VIDEO:   Ret=FF_THREAD_SLICE;                    break;
        case CODEC_ID_H264 :        Ret=FF_THREAD_FRAME|FF_THREAD_SLICE;    break;
        default:                    Ret=FF_THREAD_FRAME;                    break;
    }
    return Ret;
}

//====================================================================================================================

bool cVideoFile::OpenCodecAndFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::OpenCodecAndFile");

    // Ensure file was previously checked
    if (!IsValide) return false;
    if (!IsInformationValide) GetFullInformationFromFile();

    // Clean memory if a previous file was loaded
    CloseCodecAndFile();

    Mutex.lock();

    //**********************************
    // Open LibavFile
    //**********************************
    // if file exist then Open video file and get a LibAVFormat context and an associated LibAVCodec decoder
    if (avformat_open_input(&LibavFile,FileName.toLocal8Bit(),NULL,NULL)!=0) {
        Mutex.unlock();
        return false;
    }
    LibavFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future NbrFrames.
    if (avformat_find_stream_info(LibavFile,NULL)<0) {
        avformat_close_input(&LibavFile);
        Mutex.unlock();
        return false;
    }

    //**********************************
    // Open audio stream
    //**********************************
    if (AudioStreamNumber!=-1) {

        AVStream *AudioStream=LibavFile->streams[AudioStreamNumber];

        // Setup STREAM options
        AudioStream->discard=AVDISCARD_DEFAULT;

        // Find the decoder for the audio stream and open it
        AudioDecoderCodec=avcodec_find_decoder(AudioStream->codec->codec_id);

        // Setup decoder options
        AudioStream->codec->debug_mv         =0;                    // Debug level (0=nothing)
        AudioStream->codec->debug            =0;                    // Debug level (0=nothing)
        AudioStream->codec->workaround_bugs  =1;                    // Work around bugs in encoders which sometimes cannot be detected automatically : 1=autodetection
        AudioStream->codec->idct_algo        =FF_IDCT_AUTO;         // IDCT algorithm, 0=auto
        AudioStream->codec->skip_frame       =AVDISCARD_DEFAULT;    // ???????
        AudioStream->codec->skip_idct        =AVDISCARD_DEFAULT;    // ???????
        AudioStream->codec->skip_loop_filter =AVDISCARD_DEFAULT;    // ???????
        AudioStream->codec->error_concealment=3;
        AudioStream->codec->thread_count     =getCpuCount();
        AudioStream->codec->thread_type      =getThreadFlags(AudioStream->codec->codec_id);

        if ((AudioDecoderCodec==NULL)||(avcodec_open2(AudioStream->codec,AudioDecoderCodec,NULL)<0)) {
            Mutex.unlock();
            return false;
        }

        IsVorbis=(strcmp(AudioDecoderCodec->name,"vorbis")==0);
    }

    //**********************************
    // Open video stream
    //**********************************
    if ((VideoStreamNumber!=-1)&&(!MusicOnly)) {

        AVStream *VideoStream=LibavFile->streams[VideoStreamNumber];

        // Setup STREAM options
        VideoStream->discard=AVDISCARD_DEFAULT;

        // Find the decoder for the video stream and open it
        VideoDecoderCodec=avcodec_find_decoder(VideoStream->codec->codec_id);

        // Setup decoder options
        VideoStream->codec->debug_mv         =0;                    // Debug level (0=nothing)
        VideoStream->codec->debug            =0;                    // Debug level (0=nothing)
        VideoStream->codec->workaround_bugs  =1;                    // Work around bugs in encoders which sometimes cannot be detected automatically : 1=autodetection
        VideoStream->codec->idct_algo        =FF_IDCT_AUTO;         // IDCT algorithm, 0=auto
        VideoStream->codec->skip_frame       =AVDISCARD_DEFAULT;    // ???????
        VideoStream->codec->skip_idct        =AVDISCARD_DEFAULT;    // ???????
        VideoStream->codec->skip_loop_filter =AVDISCARD_DEFAULT;    // ???????
        VideoStream->codec->error_concealment=3;
        VideoStream->codec->thread_count     =getCpuCount();
        VideoStream->codec->thread_type      =getThreadFlags(VideoStream->codec->codec_id);

        // Hack to correct wrong frame rates that seem to be generated by some codecs
        if (VideoStream->codec->time_base.num>1000 && VideoStream->codec->time_base.den==1)
            VideoStream->codec->time_base.den=1000;

        if ((VideoDecoderCodec==NULL)||(avcodec_open2(VideoStream->codec,VideoDecoderCodec,NULL)<0)) {
            Mutex.unlock();
            return false;
        }
    }
    IsOpen=true;
    Mutex.unlock();
    return IsOpen;
}

//*********************************************************************************************************************************************
// Base object for music definition
//*********************************************************************************************************************************************

cMusicObject::cMusicObject(cBaseApplicationConfig *ApplicationConfig):cVideoFile(OBJECTTYPE_MUSICFILE,ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cMusicObject::cMusicObject");

    Volume=1.0;                           // Volume as % from 1% to 150%
}

//====================================================================================================================

void cMusicObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cMusicObject::SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    QString         TheFileName;

    if (PathForRelativPath!="") {
        if (ForceAbsolutPath) TheFileName=QDir(QFileInfo(PathForRelativPath).absolutePath()).absoluteFilePath(FileName);
            else TheFileName=QDir(QFileInfo(PathForRelativPath).absolutePath()).relativeFilePath(FileName);
    } else TheFileName=FileName;

    Element.setAttribute("FilePath",TheFileName);
    Element.setAttribute("StartPos",StartPos.toString());
    Element.setAttribute("EndPos",  EndPos.toString());
    Element.setAttribute("Volume",  QString("%1").arg(Volume,0,'f'));

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cMusicObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cMusicObject::LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        FileName=Element.attribute("FilePath","");
        if (PathForRelativPath!="") {
            QString PA=QDir(PathForRelativPath).absolutePath();
            if (!PA.endsWith(QDir::separator())) PA=PA+QDir::separator();
            PA=PA+FileName;
            FileName=QDir::cleanPath(PA);
        }
        if (LoadMedia(FileName,AliasList,ModifyFlag)) {
            StartPos=QTime().fromString(Element.attribute("StartPos"));
            EndPos  =QTime().fromString(Element.attribute("EndPos"));
            Volume  =GetDoubleValue(Element,"Volume");
            return true;
        } else return false;
    } else return false;
}

//====================================================================================================================

bool cMusicObject::LoadMedia(QString &TheFilename,QStringList *AliasList,bool *ModifyFlag) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cMusicObject::LoadMedia");

    IsValide=(GetInformationFromFile(TheFilename,AliasList,ModifyFlag))&&(OpenCodecAndFile());
    return IsValide;
}

//====================================================================================================================

bool cMusicObject::IsFilteredFile(int RequireObjectType,int AllowedObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cMusicObject::IsFilteredFile");

    return ((RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(ObjectType==RequireObjectType))&&((AllowedObjectType&FILTERALLOW_OBJECTTYPE_MUSICFILE)!=0);
}
