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
#include "_Diaporama.h"

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

AVFrame *ALLOCFRAME() { return avcodec_alloc_frame(); }
void FREEFRAME(AVFrame **Buf) { avcodec_free_frame(Buf); *Buf=NULL; }

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

#ifdef USETAGLIB
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


//====================================================================================================================

cReplaceObjectList::cReplaceObjectList() {
}

void cReplaceObjectList::SearchAppendObject(QString SourceFileName) {
    int i=0;
    while ((i<List.count())&&(List[i].SourceFileName!=SourceFileName)) i++;
    if ((i<List.count())&&(List[i].SourceFileName==SourceFileName)) {
        // Object already define

    } else {
        // Object not yet define
        QString DestFileName=QFileInfo(SourceFileName).baseName()+"."+QFileInfo(SourceFileName).completeSuffix();
        // Search if DestFileName already exist
        bool Cont=true;
        int  Num =0;
        while (Cont) {
            Cont=false;
            int j=0;
            while ((j<List.count())&&(List[j].DestFileName!=DestFileName)) j++;
            if ((j<List.count())&&(List[j].DestFileName==DestFileName)) {
                // DestFileName already define
                Cont=true;
                Num++;
                DestFileName=QFileInfo(SourceFileName).baseName()+QString("-%1").arg(Num)+"."+QFileInfo(SourceFileName).completeSuffix();
            }
        }
        List.append(cReplaceObject(SourceFileName,DestFileName));
    }
}

QString cReplaceObjectList::GetDestinationFileName(QString SourceFileName) {
    for (int i=0;i<List.count();i++) if (List[i].SourceFileName==SourceFileName) return List[i].DestFileName;
    return SourceFileName;
}

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
    FileKey             = -1;
    FolderKey           = -1;
    IsValide            = false;                                    // if true then object if initialise
    IsInformationValide = false;                                    // if true then information list if fuly initialise
    ObjectGeometry      = IMAGE_GEOMETRY_UNKNOWN;                   // Image geometry
    FileSize            = 0;
    ImageWidth          = 0;                                        // Widht of normal image
    ImageHeight         = 0;                                        // Height of normal image
    CreatDateTime       = QDateTime(QDate(0,0,0),QTime(0,0,0));     // Original date/time
    ModifDateTime       = QDateTime(QDate(0,0,0),QTime(0,0,0));     // Last modified date/time
    AspectRatio         = 1;
    ImageOrientation    = -1;
    Duration            = QTime(0,0,0);
}

//====================================================================================================================

cBaseMediaFile::~cBaseMediaFile() {
}

//====================================================================================================================

QString cBaseMediaFile::FileName() {
    return ApplicationConfig->FoldersTable->GetFolderPath(FolderKey)+ShortName();
}

//====================================================================================================================

QString cBaseMediaFile::ShortName() {
    return ApplicationConfig->FilesTable->GetShortName(FileKey);
}

//====================================================================================================================

bool cBaseMediaFile::GetFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetFullInformationFromFile");
    IsInformationValide=ApplicationConfig->FilesTable->GetExtendedProperties(FileKey,&InformationList)&&ApplicationConfig->FilesTable->GetThumbs(FileKey,&Icon16,&Icon100);
    if (!IsInformationValide) IsInformationValide=GetChildFullInformationFromFile() && SaveAllInformationToDatabase();
    if (IsInformationValide) {
    }
    return IsInformationValide;
}

//====================================================================================================================

bool cBaseMediaFile::GetInformationFromFile(QString FileName,QStringList *AliasList,bool *ModifyFlag,qlonglong GivenFolderKey) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetInformationFromFile");

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
            QString NewFileName=QFileInfo(FileName).absoluteFilePath();
            while ((i<AliasList->count())&&(!QFileInfo(NewFileName).exists())) {
                QString OldName=AliasList->at(i);
                QString NewName=OldName.mid(OldName.indexOf("####")+QString("####").length());
                OldName=OldName.left(OldName.indexOf("####"));
                OldName=OldName.left(OldName.lastIndexOf(QDir::separator()));
                NewName=NewName.left(NewName.lastIndexOf(QDir::separator()));
                NewFileName=NewName+QDir::separator()+QFileInfo(FileName).fileName();
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
        return false;
    }

    FileName =QFileInfo(FileName).absoluteFilePath();
    if (FolderKey==-1)  FolderKey=GivenFolderKey>=0?GivenFolderKey:ApplicationConfig->FoldersTable->GetFolderKey(QFileInfo(FileName).absolutePath());
    if (FileKey==-1)    FileKey  =ApplicationConfig->FilesTable->GetFileKey(FolderKey,QFileInfo(FileName).fileName(),ObjectType);

    QString BasicInfo;
    if (ApplicationConfig->FilesTable->GetBasicProperties(FileKey,&BasicInfo,FileName,&FileSize,&CreatDateTime,&ModifDateTime)) {
        QDomDocument    domDocument;
        QString         errorStr;
        int             errorLine,errorColumn;
        if ((domDocument.setContent(BasicInfo,true,&errorStr,&errorLine,&errorColumn))&&
            (domDocument.elementsByTagName("BasicProperties").length()>0)&&
            (domDocument.elementsByTagName("BasicProperties").item(0).isElement()==true)&&
            (LoadBasicInformationFromDatabase(domDocument.elementsByTagName("BasicProperties").item(0).toElement()))) {
            IsValide=true;
            return true;
        }
    }

    FileSize     =QFileInfo(FileName).size();
    ModifDateTime=QFileInfo(FileName).lastModified();
    CreatDateTime=QFileInfo(FileName).created();

    IsValide=true;
    return true;
}

//====================================================================================================================

bool cBaseMediaFile::SaveAllInformationToDatabase() {
    QDomDocument domDocument;
    QDomElement  root=domDocument.createElement("BasicProperties");
    domDocument.appendChild(root);
    SaveBasicInformationToDatabase(&root);
    return ApplicationConfig->FilesTable->SetBasicProperties(FileKey,domDocument.toString())&&
           ApplicationConfig->FilesTable->SetExtendedProperties(FileKey,&InformationList)&&
           ApplicationConfig->FilesTable->SetThumbs(FileKey,&Icon16,&Icon100);
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

    return GetTextSize(FileSize);
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
// return 3 lines to display Summary of media file in dialog box which need them

QStringList cBaseMediaFile::GetSummaryText(QStringList *InformationList) {
    QStringList SummaryText;
    SummaryText.append(ShortName()+"("+GetFileSizeStr()+")");
    SummaryText.append(GetImageSizeStr(cBaseMediaFile::FULLWEB));
    if (ObjectType==OBJECTTYPE_IMAGEFILE) {
        SummaryText.append(GetInformationValue("composer",InformationList));
        if (GetInformationValue("Photo.ExposureTime",   InformationList)!="") SummaryText[2]=SummaryText[2]+(SummaryText[2]!=""?"-":"")+GetInformationValue("Photo.ExposureTime",   InformationList);
        if (GetInformationValue("Photo.ApertureValue",  InformationList)!="") SummaryText[2]=SummaryText[2]+(SummaryText[2]!=""?"-":"")+GetInformationValue("Photo.ApertureValue",  InformationList);
        if (GetInformationValue("Photo.ISOSpeedRatings",InformationList)!="") SummaryText[2]=SummaryText[2]+(SummaryText[2]!=""?"-":"")+GetInformationValue("Photo.ISOSpeedRatings",InformationList)+" ISO";
    } else SummaryText.append(QApplication::translate("DlgSlideProperties","Duration:")+Duration.toString("HH:mm:ss.zzz"));
    return SummaryText;
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

QString cUnmanagedFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cUnmanagedFile::GetFileTypeStr for %1").arg(FileName()));

    return QApplication::translate("cBaseMediaFile","Unmanaged","File type");
}

//*********************************************************************************************************************************************
// Folder
//*********************************************************************************************************************************************

cFolder::cFolder(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::cFolder");

    ObjectType  =OBJECTTYPE_FOLDER;
}

//====================================================================================================================

bool cFolder::GetChildFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::GetChildFullInformationFromFile");

    QString AdjustedFileName=FileName();
    if (!AdjustedFileName.endsWith(QDir::separator())) AdjustedFileName=AdjustedFileName+QDir::separator();

    // Check if a folder.jpg file exist
    if (Icon16.isNull()) {
        QFileInfoList Directorys=QDir(FileName()).entryInfoList(QDir::Files);
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
        QFileInfoList Directorys=QDir(FileName()).entryInfoList(QDir::Files|QDir::Hidden);
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
    return true;
}

//====================================================================================================================

QString cFolder::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cFolder::GetFileTypeStr for %1").arg(FileName()));

    return QApplication::translate("cBaseMediaFile","Folder","File type");
}

//*********************************************************************************************************************************************
// ffDiaporama project file
//*********************************************************************************************************************************************

cffDProjectFile::cffDProjectFile(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::cffDProjectFile");

    ObjectType      =OBJECTTYPE_FFDFILE;
    NbrSlide        =0;
    NbrChapters     =0;

    InitDefaultValues();
}

//====================================================================================================================

void cffDProjectFile::InitDefaultValues() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::InitDefaultValues");

    Title           =QApplication::translate("cModelList","Project title");;
    Author          =QApplication::translate("cModelList","Project author");
    Album           =QApplication::translate("cModelList","Project album");
    OverrideDate    =false;
    EventDate       =QDate::currentDate();
    LongDate        =(OverrideDate?LongDate:FormatLongDate(EventDate));
    Comment         =QApplication::translate("cModelList","Project comment");
    Composer        ="";
    DefaultLanguage =ApplicationConfig->DefaultLanguage;
    ffDRevision     =CurrentAppVersion;;
}

//====================================================================================================================

bool cffDProjectFile::LoadBasicInformationFromDatabase(QDomElement root) {
    return LoadFromXML(root);
}

//====================================================================================================================

void cffDProjectFile::SaveBasicInformationToDatabase(QDomElement *root) {
    SaveToXML(*root);

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement("ffDiaporamaProjectProperties");
    Element.setAttribute("ImageGeometry",ObjectGeometry==IMAGE_GEOMETRY_16_9?GEOMETRY_16_9:ObjectGeometry==IMAGE_GEOMETRY_40_17?GEOMETRY_40_17:GEOMETRY_4_3);
    Element.setAttribute("ObjectNumber",NbrSlide);
    root->appendChild(Element);
}

//====================================================================================================================

void cffDProjectFile::SaveToXML(QDomElement &domDocument) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement("ffDiaporamaProjectProperties");
    Element.setAttribute("Title",Title);
    Element.setAttribute("Author",Author);
    Element.setAttribute("Album",Album);
    Element.setAttribute("LongDate",LongDate);
    Element.setAttribute("EventDate",EventDate.toString(Qt::ISODate));
    Element.setAttribute("OverrideDate",OverrideDate?1:0);
    Element.setAttribute("Comment",Comment);
    Element.setAttribute("Composer",Composer);
    Element.setAttribute("Duration",qlonglong(QTime(0,0,0,0).msecsTo(Duration)));
    Element.setAttribute("ffDRevision",ffDRevision);
    Element.setAttribute("DefaultLanguage",DefaultLanguage);
    Element.setAttribute("ChaptersNumber",NbrChapters);
    for (int i=0;i<NbrChapters;i++) {
        QString     ChapterNum=QString("%1").arg(i); while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
        QDomElement SubElement=DomDocument.createElement("Chapter_"+ChapterNum);
        SubElement.setAttribute("Start",    GetInformationValue("Chapter_"+ChapterNum+":Start",&InformationList));
        SubElement.setAttribute("End",      GetInformationValue("Chapter_"+ChapterNum+":End",&InformationList));
        SubElement.setAttribute("Duration", GetInformationValue("Chapter_"+ChapterNum+":Duration",&InformationList));
        SubElement.setAttribute("title",    GetInformationValue("Chapter_"+ChapterNum+":title",&InformationList));
        SubElement.setAttribute("InSlide",  GetInformationValue("Chapter_"+ChapterNum+":InSlide",&InformationList));
        Element.appendChild(SubElement);
    }
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cffDProjectFile::LoadFromXML(QDomElement domDocument) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::LoadFromXML");

    InitDefaultValues();
    bool IsOk=false;
    if ((domDocument.elementsByTagName("ffDiaporamaProjectProperties").length()>0)&&(domDocument.elementsByTagName("ffDiaporamaProjectProperties").item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName("ffDiaporamaProjectProperties").item(0).toElement();
        if (Element.hasAttribute("Title")) {
            Title=Element.attribute("Title");
            InformationList.append(QString("title")+QString("##")+QString(Title));
        }
        if (Element.hasAttribute("Author")) {
            Author=Element.attribute("Author");
            InformationList.append(QString("artist")+QString("##")+QString(Author));
        }
        if (Element.hasAttribute("Album")) {
            Album=Element.attribute("Album");
            InformationList.append(QString("album")+QString("##")+QString(Album));
        }
        if (Element.hasAttribute("EventDate")) {
            EventDate=EventDate.fromString(Element.attribute("EventDate"),Qt::ISODate);
            InformationList.append(QString("EventDate")+QString("##")+EventDate.toString(ApplicationConfig->ShortDateFormat));
        } else if (Element.hasAttribute("Year")) {
            EventDate.setDate(Element.attribute("Year").toInt(),1,1);
            InformationList.append(QString("EventDate")+QString("##")+EventDate.toString(ApplicationConfig->ShortDateFormat));
        }
        if (Element.hasAttribute("OverrideDate")) OverrideDate=Element.attribute("OverrideDate")=="1";
        if (!OverrideDate) LongDate=FormatLongDate(EventDate); else if (Element.hasAttribute("LongDate")) LongDate=Element.attribute("LongDate");
        InformationList.append(QString("LongDate")+QString("##")+QString(LongDate));

        if (Element.hasAttribute("Comment")) {
            Comment=Element.attribute("Comment");
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
        if (Element.hasAttribute("Duration")) Duration=QTime(0,0,0,0).addMSecs(Element.attribute("Duration").toLongLong());
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

bool cffDProjectFile::GetChildFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::GetChildFullInformationFromFile");

    LoadIcons(&ApplicationConfig->DefaultFFDIcon);

    QFile           file(FileName());
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
    return true;
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
    if (LongDate!="")       Info=Info+(Info!=""?" - ":"")+LongDate; else Info=Info+(Info!=""?" - ":"")+EventDate.toString(ApplicationConfig->ShortDateFormat);
    if (Album!="")          Info=Info+(Info!=""?" - ":"")+Album;
    if (Author!="")         Info=Info+(Info!=""?" - ":"")+Author;
    return Info;
}

//====================================================================================================================

QString cffDProjectFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cffDProjectFile::GetFileTypeStr for %1").arg(FileName()));

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
    NoExifData  =false;
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

QString cImageFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cImageFile::GetFileTypeStr for %1").arg(FileName()));

    if ((ObjectType==OBJECTTYPE_IMAGEFILE)&&(!IsVectorImg))         return QApplication::translate("cBaseMediaFile","Image","File type");
        else if ((ObjectType==OBJECTTYPE_IMAGEFILE)&&(IsVectorImg)) return QApplication::translate("cBaseMediaFile","Vector image","File type");
        else return QApplication::translate("cBaseMediaFile","Thumbnail","File type");
}

//====================================================================================================================

bool cImageFile::LoadBasicInformationFromDatabase(QDomElement root) {
    ImageWidth      =root.attribute("ImageWidth").toInt();
    ImageHeight     =root.attribute("ImageHeight").toInt();
    ImageOrientation=root.attribute("ImageOrientation").toInt();
    ObjectGeometry  =root.attribute("ObjectGeometry").toInt();
    AspectRatio     =GetDoubleValue(root,"AspectRatio");
    return true;
}

//====================================================================================================================

void cImageFile::SaveBasicInformationToDatabase(QDomElement *root) {
    root->setAttribute("ImageWidth",         ImageWidth);
    root->setAttribute("ImageHeight",        ImageHeight);
    root->setAttribute("ImageOrientation",   ImageOrientation);
    root->setAttribute("ObjectGeometry",     ObjectGeometry);
    root->setAttribute("AspectRatio",        QString("%1").arg(AspectRatio,0,'f'));
}

//====================================================================================================================

bool cImageFile::GetInformationFromFile(QString FileName,QStringList *AliasList,bool *ModifyFlag,qlonglong GivenFolderKey) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetInformationFromFile");
    IsVectorImg=(QFileInfo(FileName).suffix().toLower()=="svg");
    return cBaseMediaFile::GetInformationFromFile(FileName,AliasList,ModifyFlag,GivenFolderKey);
}

//====================================================================================================================

bool cImageFile::GetChildFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetChildFullInformationFromFile");

    ImageOrientation    =-1;
    bool                ExifOk=false;

    if ((!IsVectorImg)&&(!NoExifData)) {

        // ******************************************************************************************************
        // Try to load EXIF information using library exiv2
        // ******************************************************************************************************
        Exiv2::Image::AutoPtr ImageFile;
        try {
            #ifdef Q_OS_WIN
                ImageFile=Exiv2::ImageFactory::open(FileName().toLocal8Bit().data());
            #else
                ImageFile=Exiv2::ImageFactory::open(FileName().toUtf8().data());
            #endif
            ExifOk=true;
        }
        catch( Exiv2::Error& ) {
            //ToLog(LOGMSG_INFORMATION,QApplication::translate("cBaseMediaFile","Image don't have EXIF metadata %1").arg(FileName));
            NoExifData=true;
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
            if (GetInformationValue("Image.Artist",&InformationList)!="") InformationList.append(QString("artist")+QString("##")+GetInformationValue("Image.Artist",&InformationList));
            if (GetInformationValue("Image.Model",&InformationList)!="")  {
                if (GetInformationValue("Image.Model",&InformationList).contains(GetInformationValue("Image.Make",&InformationList),Qt::CaseInsensitive)) InformationList.append(QString("composer")+QString("##")+GetInformationValue("Image.Model",&InformationList));
                    else InformationList.append(QString("composer")+QString("##")+GetInformationValue("Image.Make",&InformationList)+" "+GetInformationValue("Image.Model",&InformationList));
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

    } else if (IsVectorImg) {
        // Vector image file
        QSvgRenderer SVGImg(FileName());
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
        }
    }

    //************************************************************************************
    // If no exif preview image (of image too small) then load/create thumbnail
    //************************************************************************************
    if ((!IsVectorImg)&&(IsIconNeeded)&&(Icon16.isNull())) {
        cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileKey,ModifDateTime,ImageOrientation,ApplicationConfig->Smoothing,true);
        if (ImageObject==NULL) {
            ToLog(LOGMSG_CRITICAL,"Error in cImageFile::GetFullInformationFromFile : FindObject return NULL for thumbnail creation !");
        } else {
            QImageReader ImgReader(FileName());
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
        cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileKey,ModifDateTime,ImageOrientation,ApplicationConfig->Smoothing,true);
        if (ImageObject==NULL) {
            ToLog(LOGMSG_CRITICAL,"Error in cImageFile::GetFullInformationFromFile : FindObject return NULL for size computation !");
        } else {
            QImageReader Img(FileName());
            if (Img.canRead()) {
                QSize Size =Img.size();
                ImageWidth =Size.width();
                ImageHeight=Size.height();
                InformationList.append(QString("Photo.PixelXDimension")+QString("##")+QString("%1").arg(ImageWidth));
                InformationList.append(QString("Photo.PixelYDimension")+QString("##")+QString("%1").arg(ImageHeight));
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
    return true;
}

//====================================================================================================================

QString cImageFile::GetTechInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetTechInfo");

    QString Info=GetImageSizeStr(FULLWEB);
    if (GetInformationValue("artist",&InformationList)!="")              Info=Info+(Info!=""?"-":"")+GetInformationValue("artist",&InformationList);
    if (GetInformationValue("composer",&InformationList)!="")            Info=Info+(Info!=""?"-":"")+GetInformationValue("composer",&InformationList);
    if (GetInformationValue("Image.Orientation",&InformationList)!="")   Info=Info+(Info!=""?"-":"")+GetInformationValue("Image.Orientation",&InformationList);
    return Info;
}

//====================================================================================================================

QString cImageFile::GetTAGInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetTAGInfo");

    QString Info=GetInformationValue("Photo.ExposureTime",&InformationList);
    if (GetInformationValue("Photo.ApertureValue",&InformationList)!="")    Info=Info+(Info!=""?"-":"")+GetInformationValue("Photo.ApertureValue",&InformationList);
    if (GetInformationValue("Photo.ISOSpeedRatings",&InformationList)!="")  Info=Info+(Info!=""?"-":"")+GetInformationValue("Photo.ISOSpeedRatings",&InformationList)+" ISO";
    if (GetInformationValue("CanonCs.LensType",&InformationList)!="")       Info=Info+(Info!=""?"-":"")+GetInformationValue("CanonCs.LensType",&InformationList);                // Canon version
    if (GetInformationValue("NikonLd3.LensIDNumber",&InformationList)!="")  Info=Info+(Info!=""?"-":"")+GetInformationValue("NikonLd3.LensIDNumber",&InformationList);           // Nikon version
    if (GetInformationValue("Photo.Flash",&InformationList)!="")            Info=Info+(Info!=""?"-":"")+GetInformationValue("Photo.Flash",&InformationList);
    if (GetInformationValue("CanonCs.FlashMode",&InformationList)!="")      Info=Info+(Info!=""?"-":"")+GetInformationValue("CanonCs.FlashMode",&InformationList);               // Canon version
    if (GetInformationValue("Nikon3.FlashMode",&InformationList)!="")       Info=Info+(Info!=""?"-":"")+GetInformationValue("Nikon3.FlashMode",&InformationList);                // Nikon version
    return Info;
}

//====================================================================================================================

QImage *cImageFile::ImageAt(bool PreviewMode) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::ImageAt");

    if (!IsValide)            return NULL;
    if (!IsInformationValide) GetFullInformationFromFile();

    QImage *RetImage=NULL;
    if (IsVectorImg) {
        // Vector image file
        QSvgRenderer SVGImg(FileName());
        if (SVGImg.isValid()) {
            RetImage=new QImage(ImageWidth,ImageHeight,QImage::Format_ARGB32_Premultiplied);
            QPainter Painter;
            Painter.begin(RetImage);
            Painter.setCompositionMode(QPainter::CompositionMode_Source);
            Painter.fillRect(QRect(0,0,RetImage->width(),RetImage->height()),Qt::transparent);
            Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            Painter.setClipping(true);
            Painter.setClipRect(QRect(0,0,RetImage->width(),RetImage->height()));
            SVGImg.render(&Painter,QRectF(0,0,RetImage->width(),RetImage->height()));
            Painter.end();
        }
    } else {
        QImage                *LN_Image   =NULL;
        cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileKey,ModifDateTime,ImageOrientation,(!PreviewMode || ApplicationConfig->Smoothing),true);

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
    }
    // return wanted image
    return RetImage;
}

/*************************************************************************************************************************************
    CLASS cVideoFile
*************************************************************************************************************************************/

cImageInCache::cImageInCache(int64_t Position,AVFrame *FiltFrame,AVFrame *FrameBufferYUV) {
    this->Position      =Position;
    this->FiltFrame     =FiltFrame;
    this->FrameBufferYUV=FrameBufferYUV;
}

cImageInCache::~cImageInCache() {
    #if defined(LIBAV) || (defined(FFMPEG)&&(FFMPEGVERSIONINT<201))
    if (FrameBufferYUV->opaque) {
        avfilter_unref_buffer((AVFilterBufferRef *)FrameBufferYUV->opaque);
        FrameBufferYUV->opaque=NULL;
    }
    #elif defined(FFMPEG) && (FFMPEGVERSIONINT>=201)
    if (FiltFrame) {
        av_frame_unref(FiltFrame);
        av_frame_free(&FiltFrame);
        //av_frame_unref(FrameBufferYUV);
    }
    #endif
    FREEFRAME(&FrameBufferYUV);
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
    LibavVideoFile          =NULL;
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
    LibavAudioFile          =NULL;
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
    #if (defined(LIBAV) && (LIBAVVERSIONINT>=9)) || defined(FFMPEG)
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

bool cVideoFile::LoadBasicInformationFromDatabase(QDomElement root) {
    ImageWidth       =root.attribute("ImageWidth").toInt();
    ImageHeight      =root.attribute("ImageHeight").toInt();
    ImageOrientation =root.attribute("ImageOrientation").toInt();
    ObjectGeometry   =root.attribute("ObjectGeometry").toInt();
    AspectRatio      =GetDoubleValue(root,"AspectRatio");
    Duration         =QTime(0,0,0,0).addMSecs(root.attribute("Duration").toLongLong());
    NbrChapters      =root.attribute("NbrChapters").toInt();
    VideoStreamNumber=root.attribute("VideoStreamNumber").toInt();
    VideoTrackNbr    =root.attribute("VideoTrackNbr").toInt();
    AudioStreamNumber=root.attribute("AudioStreamNumber").toInt();
    AudioTrackNbr    =root.attribute("AudioTrackNbr").toInt();
    if (EndPos==QTime(0,0,0,0)) EndPos=Duration;
    return true;
}

//====================================================================================================================

void cVideoFile::SaveBasicInformationToDatabase(QDomElement *root) {
    root->setAttribute("ImageWidth",        ImageWidth);
    root->setAttribute("ImageHeight",       ImageHeight);
    root->setAttribute("ImageOrientation",  ImageOrientation);
    root->setAttribute("ObjectGeometry",    ObjectGeometry);
    root->setAttribute("AspectRatio",       QString("%1").arg(AspectRatio,0,'f'));
    root->setAttribute("Duration",          QTime(0,0,0,0).msecsTo(Duration));
    root->setAttribute("NbrChapters",       NbrChapters);
    root->setAttribute("VideoStreamNumber", VideoStreamNumber);
    root->setAttribute("VideoTrackNbr",     VideoTrackNbr);
    root->setAttribute("AudioStreamNumber", AudioStreamNumber);
    root->setAttribute("AudioTrackNbr",     AudioTrackNbr);
}

//====================================================================================================================

// Overloaded function use to dertermine if media file correspond to WantedObjectType
//      WantedObjectType could be OBJECTTYPE_VIDEOFILE or OBJECTTYPE_MUSICFILE
//      if AudioOnly was set to true in constructor then ignore all video track and set WantedObjectType to OBJECTTYPE_MUSICFILE else set it to OBJECTTYPE_VIDEOFILE
//      return true if WantedObjectType=OBJECTTYPE_VIDEOFILE and at least one video track is present
//      return true if WantedObjectType=OBJECTTYPE_MUSICFILE and at least one audio track is present

bool cVideoFile::GetChildFullInformationFromFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::GetChildFullInformationFromFile");

    //Mutex.lock();
    bool            Continu=true;
    AVFormatContext *LibavFile=NULL;
    QString         sFileName=FileName();

    //*********************************************************************************************************
    // Open file and get a LibAVFormat context and an associated LibAVCodec decoder
    //*********************************************************************************************************
    char filename[512];
    strcpy(filename,sFileName.toLocal8Bit());
    if (avformat_open_input(&LibavFile,filename,NULL,NULL)!=0) {
        LibavFile=NULL;
        //Mutex.unlock();
        return false;
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
            int64_t Start=double(ch->start)*(double(av_q2d(ch->time_base))*1000);     // Lib AV use 1/1 000 000 000 sec and we want msec !
            int64_t End  =double(ch->end)*(double(av_q2d(ch->time_base))*1000);       // Lib AV use 1/1 000 000 000 sec and we want msec !

            // Special case if it's first chapter and start!=0 => add a chapter 0
            if ((NbrChapters==0)&&(LibavFile->chapters[i]->start>0)) {
                InformationList.append("Chapter_"+ChapterNum+":Start"   +QString("##")+QTime(0,0,0,0).toString("hh:mm:ss.zzz"));
                InformationList.append("Chapter_"+ChapterNum+":End"     +QString("##")+QTime(0,0,0,0).addMSecs(Start).toString("hh:mm:ss.zzz"));
                InformationList.append("Chapter_"+ChapterNum+":Duration"+QString("##")+QTime(0,0,0,0).addMSecs(Start).toString("hh:mm:ss.zzz"));
                if (GetInformationValue("title",&InformationList)!="") InformationList.append("Chapter_"+ChapterNum+":title##"+GetInformationValue("title",&InformationList));
                    else InformationList.append("Chapter_"+ChapterNum+":title##"+QFileInfo(sFileName).baseName());
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
        int64_t ms=LibavFile->duration/1000;//-LibavFile->start_time;
        int     ss=ms/1000;
        int     mm=ss/60;
        int     hh=mm/60;
        mm      =mm-(hh*60);
        ss      =ss-(ss/60)*60;
        ms      =ms-(ms/1000)*1000;
        Duration=QTime(hh,mm,ss,ms);
        EndPos  =Duration;

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
                else                                                    InformationList.append(TrackNum+QString("Channels")+QString("##")+QString("%1").arg(LibavFile->streams[Track]->codec->channels)+SampleFMT);

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
                    if ((sFileName.toLower().endsWith(".ogv"))&&((Key=="title")||(Key=="artist")||(Key=="album")||(Key=="comment")||(Key=="date")||(Key=="composer")||(Key=="encoder")))
                             InformationList.append(Key+QString("##")+QString().fromUtf8(tag->value));
                        else InformationList.append(TrackNum+Key+QString("##")+QString().fromUtf8(tag->value));
                }

                // Ensure language exist (Note : AVI and FLV container own language at container level instead of track level)
                if (GetInformationValue(TrackNum+"language",&InformationList)=="") {
                    QString Lng=GetInformationValue("language",&InformationList);
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
                if (GetInformationValue(TrackNum+"language",&InformationList)=="") {
                    QString Lng=GetInformationValue("language",&InformationList);
                    InformationList.append(TrackNum+QString("language##")+(Lng==""?"und":Lng));
                }

                // Keep this as default track
                if (VideoStreamNumber==-1) {
                    QImage  *Img=NULL;
                    AVFrame *FrameBufYUV=NULL;


                    // Search if a jukebox mode thumbnail (jpg file with same name as video) exist
                    QFileInfo   File(sFileName);
                    QString     JPegFile=File.absolutePath()+(File.absolutePath().endsWith(QDir::separator())?"":QString(QDir::separator()))+File.completeBaseName()+".jpg";
                    if (QFileInfo(JPegFile).exists()) LoadIcons(JPegFile);

                    VideoStreamNumber=Track;
                    IsMTS=(sFileName.toLower().endsWith(".mts",Qt::CaseInsensitive) || sFileName.toLower().endsWith(".m2ts",Qt::CaseInsensitive) || sFileName.toLower().endsWith(".mod",Qt::CaseInsensitive));
                    LibavFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future NbrFrames.
                    LibavFile->streams[VideoStreamNumber]->discard=AVDISCARD_DEFAULT;  // Setup STREAM options

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

                    if (avcodec_open2(LibavFile->streams[VideoStreamNumber]->codec,Codec,NULL)>=0) {
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
                        int64_t   Position =0;
                        double    dEndFile =double(QTime(0,0,0,0).msecsTo(Duration))/1000;    // End File Position in double format
                        if (dEndFile!=0) {
                            // Allocate structure for YUV image

                            FrameBufYUV=ALLOCFRAME();

                            if (FrameBufYUV!=NULL) {

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
                                            if (avcodec_decode_video2(VideoStream->codec,FrameBufYUV,&FrameDecoded,StreamPacket)<0)
                                                ToLog(LOGMSG_INFORMATION,"IN:cVideoFile::OpenCodecAndFile : avcodec_decode_video2 return an error");
                                            if (FrameDecoded>0) {
                                                int64_t pts=AV_NOPTS_VALUE;
                                                if ((FrameBufYUV->pkt_dts==(int64_t)AV_NOPTS_VALUE)&&(FrameBufYUV->pkt_pts!=(int64_t)AV_NOPTS_VALUE)) pts=FrameBufYUV->pkt_pts; else pts=FrameBufYUV->pkt_dts;
                                                if (pts==(int64_t)AV_NOPTS_VALUE) pts=0;
                                                FramePosition         =double(pts)*FrameTimeBase;
                                                Img                   =ConvertYUVToRGB(false,FrameBufYUV);      // Create Img from YUV Buffer
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
                                FREEFRAME(&FrameBufYUV);

                            } else ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::OpenCodecAndFile : Impossible to allocate FrameBufYUV");
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

            // Close the video codec
            if (Codec!=NULL) {
                avcodec_close(LibavFile->streams[Track]->codec);
                Codec=NULL;
            }

            //*********************************************************************************************************
            // Thumbnails (since lavf 54.2.0 - avformat.h)
            //*********************************************************************************************************
            #ifndef USETAGLIB
            if (LibavFile->streams[Track]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
                AVStream *ThumbStream=LibavFile->streams[Track];
                AVPacket pkt         =ThumbStream->attached_pic;
                int      FrameDecoded=0;
                AVFrame  *FrameYUV=ALLOCFRAME();
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
                            int     W=FrameYUV->width, RealW=(W/8)*8;    if (RealW<W) RealW+=8;
                            int     H=FrameYUV->height,RealH=(H/8)*8;    if (RealH<H) RealH+=8;;
                            QImage  Thumbnail(RealW,RealH,QTPIXFMT);
                            AVFrame *FrameRGB=ALLOCFRAME();
                            if ((FrameRGB)&&(!Thumbnail.isNull())) {
                                avpicture_fill((AVPicture *)FrameRGB,Thumbnail.bits(),PIXFMT,RealW,RealH);
                                struct SwsContext *img_convert_ctx=sws_getContext(FrameYUV->width,FrameYUV->height,(PixelFormat)FrameYUV->format,RealW,RealH,PIXFMT,SWS_FAST_BILINEAR,NULL,NULL,NULL);
                                if (img_convert_ctx!=NULL) {
                                    int ret = sws_scale(img_convert_ctx,FrameYUV->data,FrameYUV->linesize,0,FrameYUV->height,FrameRGB->data,FrameRGB->linesize);
                                    if (ret>0) {
                                        // sws_scaler truncate the width of the images to a multiple of 8. So cut resulting image to comply a multiple of 8
                                        Thumbnail=Thumbnail.copy(0,0,W,H);
                                        LoadIcons(&Thumbnail);
                                    }
                                    sws_freeContext(img_convert_ctx);
                                }
                            }
                            if (FrameRGB) FREEFRAME(&FrameRGB);
                        }
                        avcodec_close(ThumbStream->codec);
                    }
                }
                if (FrameYUV) FREEFRAME(&FrameYUV);
            }
            #endif
        }

        #ifdef USETAGLIB
        // If it's an audio file, try to get embeded image with taglib
        if ((IsIconNeeded)&&(Icon16.isNull())&&(ObjectType==OBJECTTYPE_MUSICFILE)) {
            QImage *Img=GetEmbededImage(sFileName);
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

    // Close the libav file
    if (LibavFile!=NULL) {
        avformat_close_input(&LibavFile);
        LibavFile=NULL;
    }

    IsOpen=false;
    //Mutex.unlock();
    return Continu;
}

//====================================================================================================================

QString cVideoFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cVideoFile::GetFileTypeStr for %1").arg(FileName()));

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
        Info=GetCumulInfoStr(&InformationList,"Audio","Codec");
        if (GetCumulInfoStr(&InformationList,"Audio","Channels")!="")       Info=Info+(Info!=""?"-":"")+GetCumulInfoStr(&InformationList,"Audio","Channels");
        if (GetCumulInfoStr(&InformationList,"Audio","Bitrate")!="")        Info=Info+(Info!=""?"-":"")+GetCumulInfoStr(&InformationList,"Audio","Bitrate");
        if (GetCumulInfoStr(&InformationList,"Audio","Frequency")!="")      Info=Info+(Info!=""?"-":"")+GetCumulInfoStr(&InformationList,"Audio","Frequency");
    } else {
        Info=GetImageSizeStr();
        if (GetCumulInfoStr(&InformationList,"Video","Codec")!="")          Info=Info+(Info!=""?"-":"")+GetCumulInfoStr(&InformationList,"Video","Codec");
        if (GetCumulInfoStr(&InformationList,"Video","Frame rate")!="")     Info=Info+(Info!=""?"-":"")+GetCumulInfoStr(&InformationList,"Video","Frame rate");
        if (GetCumulInfoStr(&InformationList,"Video","Bitrate")!="")        Info=Info+(Info!=""?"-":"")+GetCumulInfoStr(&InformationList,"Video","Bitrate");

        int     Num     =0;
        QString TrackNum="";
        QString Value   ="";
        QString SubInfo ="";
        do {
            TrackNum=QString("%1").arg(Num);
            while (TrackNum.length()<3) TrackNum="0"+TrackNum;
            TrackNum="Audio_"+TrackNum+":";
            Value=GetInformationValue(TrackNum+"language",&InformationList);
            if (Value!="") {
                if (Num==0) Info=Info+"-"; else Info=Info+"/";
                SubInfo=GetInformationValue(TrackNum+"Codec",&InformationList);
                if (GetInformationValue(TrackNum+"Channels",&InformationList)!="")  SubInfo=SubInfo+(Info!=""?"-":"")+GetInformationValue(TrackNum+"Channels",&InformationList);
                if (GetInformationValue(TrackNum+"Bitrate",&InformationList)!="")   SubInfo=SubInfo+(Info!=""?"-":"")+GetInformationValue(TrackNum+"Bitrate",&InformationList);
                if (GetInformationValue(TrackNum+"Frequency",&InformationList)!="") SubInfo=SubInfo+(Info!=""?"-":"")+GetInformationValue(TrackNum+"Frequency",&InformationList);
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

    QString Info=GetInformationValue("track",&InformationList);
    if (GetInformationValue("title",&InformationList)!="")          Info=Info+(Info!=""?"-":"")+GetInformationValue("title",&InformationList);
    if (GetInformationValue("artist",&InformationList)!="")         Info=Info+(Info!=""?"-":"")+GetInformationValue("artist",&InformationList);
    if (GetInformationValue("album",&InformationList)!="")          Info=Info+(Info!=""?"-":"")+GetInformationValue("album",&InformationList);
    if (GetInformationValue("date",&InformationList)!="")           Info=Info+(Info!=""?"-":"")+GetInformationValue("date",&InformationList);
    if (GetInformationValue("genre",&InformationList)!="")          Info=Info+(Info!=""?"-":"")+GetInformationValue("genre",&InformationList);
    return Info;
}

//====================================================================================================================
// Close LibAVFormat and LibAVCodec contexte for the file
//====================================================================================================================

void cVideoFile::CloseCodecAndFile() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::CloseCodecAndFile");
    //Mutex.lock();

    while (CacheImage.count()>0) delete(CacheImage.takeLast());

    // Close the resampling context
    CloseResampler();

    // Close the filter context
    if (VideoFilterGraph) VideoFilter_Close();

    // Close the video codec
    if (VideoDecoderCodec!=NULL) {
        avcodec_close(LibavVideoFile->streams[VideoStreamNumber]->codec);
        VideoDecoderCodec=NULL;
    }

    // Close the audio codec
    if (AudioDecoderCodec!=NULL) {
        avcodec_close(LibavAudioFile->streams[AudioStreamNumber]->codec);
        AudioDecoderCodec=NULL;
    }

    // Close the libav files
    if (LibavAudioFile!=NULL) {
        avformat_close_input(&LibavAudioFile);
        LibavAudioFile=NULL;
    }
    if (LibavVideoFile!=NULL) {
        avformat_close_input(&LibavVideoFile);
        LibavVideoFile=NULL;
    }

    if (FrameBufferYUV!=NULL) {
        #if defined(LIBAV) || (defined(FFMPEG)&&(FFMPEGVERSIONINT<201))
        if (FrameBufferYUV->opaque) {
            avfilter_unref_buffer((AVFilterBufferRef *)FrameBufferYUV->opaque);
            FrameBufferYUV->opaque=NULL;
        }
        #endif
        FREEFRAME(&FrameBufferYUV);
    }
    FrameBufferYUVReady=false;

    IsOpen=false;
    //Mutex.unlock();
}

//*********************************************************************************************************************

void cVideoFile::CloseResampler() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::CloseResampler");
    if (RSC) {
        #if defined(LIBAV) && (LIBAVVERSIONINT<=8)
            audio_resample_close(RSC);
        #elif defined(LIBAV) && (LIBAVVERSIONINT<=9)
            avresample_close(RSC);
            avresample_free(&RSC);
        #elif defined(FFMPEG)
            swr_free(&RSC);
        #endif
        RSC=NULL;
    }
}

//*********************************************************************************************************************

void cVideoFile::CheckResampler(int RSC_InChannels,int RSC_OutChannels,AVSampleFormat RSC_InSampleFmt,AVSampleFormat RSC_OutSampleFmt,int RSC_InSampleRate,int RSC_OutSampleRate
                                            #if (defined(LIBAV)&&(LIBAVVERSIONINT>=9)) || defined(FFMPEG)
                                               ,uint64_t RSC_InChannelLayout,uint64_t RSC_OutChannelLayout
                                           #endif
                                      ) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::CheckResampler");
    #if (defined(LIBAV) && (LIBAVVERSIONINT>=9)) || defined(FFMPEG)
    if (RSC_InChannelLayout==0)  RSC_InChannelLayout =av_get_default_channel_layout(RSC_InChannels);
    if (RSC_OutChannelLayout==0) RSC_OutChannelLayout=av_get_default_channel_layout(RSC_OutChannels);
    #endif
    if ((RSC!=NULL)&&
        ( (RSC_InChannels!=this->RSC_InChannels)    ||(RSC_OutChannels!=this->RSC_OutChannels)
        ||(RSC_InSampleFmt!=this->RSC_InSampleFmt)  ||(RSC_OutSampleFmt!=this->RSC_OutSampleFmt)
        ||(RSC_InSampleRate!=this->RSC_InSampleRate)||(RSC_OutSampleRate!=this->RSC_OutSampleRate)
        #if (defined(LIBAV) && (LIBAVVERSIONINT>=9)) || defined(FFMPEG)
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

        #if defined(LIBAV) && (LIBAVVERSIONINT<=8)
            RSC=av_audio_resample_init(    // Context for resampling audio data
                RSC_OutChannels,RSC_InChannels,             // output_channels, input_channels
                RSC_OutSampleRate,RSC_InSampleRate,         // output_rate, input_rate
                RSC_OutSampleFmt,RSC_InSampleFmt,           // sample_fmt_out, sample_fmt_in
                0,                                          // filter_length
                0,                                          // log2_phase_count
                1,                                          // linear
                0);                                         // cutoff
            if (!RSC) ToLog(LOGMSG_CRITICAL,QString("CheckResampler: av_audio_resample_init failed"));
        #elif defined(LIBAV) && (LIBAVVERSIONINT<=9)
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
        #elif defined(FFMPEG)
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
            av_opt_set_sample_fmt(RSC,"in_sample_fmt",  RSC_InSampleFmt,     0);
            av_opt_set_sample_fmt(RSC,"out_sample_fmt", RSC_OutSampleFmt,    0);
            if ((RSC)&&(swr_init(RSC)<0)) {
                ToLog(LOGMSG_CRITICAL,QString("CheckResampler: swr_init failed"));
                swr_free(&RSC);
                RSC=NULL;
            }
            if (!RSC) ToLog(LOGMSG_CRITICAL,QString("CheckResampler: swr_alloc_set_opts failed"));
        #endif
    }
}

//*********************************************************************************************************************
// VIDEO FILTER PART : This code was adapt from xbmc sources files
//*********************************************************************************************************************

int cVideoFile::VideoFilter_Open() {
    int result;

    if (VideoFilterGraph) VideoFilter_Close();

    if (!(VideoFilterGraph=avfilter_graph_alloc())) {
        ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : unable to alloc filter graph"));
        return -1;
    }

    VideoFilterGraph->scale_sws_opts = av_strdup("flags=4");

    #if defined(LIBAV) && (LIBAVVERSIONINT<=8)

        QString args=QString("%1:%2:%3:%4:%5:%6:%7")
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->width)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->height)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->pix_fmt)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->time_base.num)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->time_base.den)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.num)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.den);

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

        AVFilterInOut *outputs = (AVFilterInOut *)av_malloc(sizeof(AVFilterInOut));
        AVFilterInOut *inputs  = (AVFilterInOut *)av_malloc(sizeof(AVFilterInOut));

    #elif defined(LIBAV) && (LIBAVVERSIONINT<=9)

        QString args=QString("%1:%2:%3:%4:%5:%6:%7")
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->width)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->height)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->pix_fmt)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->time_base.num)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->time_base.den)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.num)
            .arg(LibavVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.den);

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
        AVFilterInOut *outputs=avfilter_inout_alloc();
        AVFilterInOut *inputs =avfilter_inout_alloc();

    #elif defined(FFMPEG)

        char args[512];
        snprintf(args, sizeof(args),"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 LibavVideoFile->streams[VideoStreamNumber]->codec->width,LibavVideoFile->streams[VideoStreamNumber]->codec->height,
                 LibavVideoFile->streams[VideoStreamNumber]->codec->pix_fmt,
                 LibavVideoFile->streams[VideoStreamNumber]->codec->time_base.num,LibavVideoFile->streams[VideoStreamNumber]->codec->time_base.den,
                 LibavVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.num,LibavVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.den
        );

        AVFilter *srcFilter=avfilter_get_by_name("buffer");
        AVFilter *outFilter=avfilter_get_by_name("buffersink");

        if ((result=avfilter_graph_create_filter(&VideoFilterIn,srcFilter,"in",args,NULL,VideoFilterGraph))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_create_filter: src"));
            return result;
        }
        if ((result=avfilter_graph_create_filter(&VideoFilterOut,outFilter,"out",NULL,NULL,VideoFilterGraph))<0) {
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_create_filter: out"));
            return result;
        }
        AVFilterInOut *outputs=avfilter_inout_alloc();
        AVFilterInOut *inputs =avfilter_inout_alloc();

    #endif

    outputs->name = av_strdup("in");
    outputs->filter_ctx = VideoFilterIn;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = VideoFilterOut;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    #if defined(LIBAV) && (LIBAVVERSIONINT<=8)
    if ((result=avfilter_graph_parse(VideoFilterGraph,QString("yadif=1:-1").toLocal8Bit().constData(),inputs,outputs,NULL))<0) {
    #elif (defined(LIBAV) && (LIBAVVERSIONINT<=9)) || (defined(FFMPEG)&&(FFMPEGVERSIONINT<201))
        if ((result=avfilter_graph_parse(VideoFilterGraph,QString("yadif=deint=interlaced:mode=send_frame:parity=auto").toLocal8Bit().constData(),&inputs,&outputs,NULL))<0) {
    #elif defined(FFMPEG)&&(FFMPEGVERSIONINT>=201)
    if ((result=avfilter_graph_parse_ptr(VideoFilterGraph,QString("yadif=deint=interlaced:mode=send_frame:parity=auto").toLocal8Bit().constData(),&inputs,&outputs,NULL))<0) {
    #endif
        ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_parse"));
        return result;
    }

    if ((result=avfilter_graph_config(VideoFilterGraph,NULL))<0) {
        ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::VideoFilter_Open : avfilter_graph_config"));
        return result;
    }
    return result;
}

//====================================================================================================================

void cVideoFile::VideoFilter_Close() {
    if (VideoFilterGraph) avfilter_graph_free(&VideoFilterGraph);
    VideoFilterGraph=NULL;
    VideoFilterIn =NULL;
    VideoFilterOut=NULL;
}

//====================================================================================================================

#if defined(LIBAV) || (FFMPEGVERSIONINT<201)
    int cVideoFile::VideoFilter_Process() {
        #if defined(LIBAV) && (LIBAVVERSIONINT<=8)
            // LIBAV 8
            int Ret=av_vsrc_buffer_add_frame(VideoFilterIn,FrameBufferYUV,FrameBufferYUV->pts,LibavVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio);
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

        #else
            // LIBAV9 AND FFMPEG 1.2
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
                if (!m_pBufferRef) return VC_ERROR; // for first frame ther is no return
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
#endif

//====================================================================================================================

bool cVideoFile::SeekFile(AVStream *VideoStream,AVStream *AudioStream,int64_t Position) {
    bool ret=true;
    AVFormatContext *LibavFile  =NULL;
    int             StreamNumber=0;

    // Reset context variables and buffers
    if (AudioStream) {
        CloseResampler();
        LibavFile=LibavAudioFile;
        StreamNumber=AudioStreamNumber;
    } else if (VideoStream) {
        if (VideoFilterGraph) VideoFilter_Close();
        while (CacheImage.count()>0) delete(CacheImage.takeLast());
        FrameBufferYUVReady   =false;
        FrameBufferYUVPosition=0;
        LibavFile=LibavVideoFile;
        StreamNumber=VideoStreamNumber;
    }

    if (LibavFile->start_time>0) Position-=LibavFile->start_time;
    if (Position<0) Position=0;

    // Flush LibAV buffers
    for (unsigned int i=0;i<LibavFile->nb_streams;i++)  {
        AVCodecContext *codec_context = LibavFile->streams[i]->codec;
        if (codec_context && codec_context->codec) avcodec_flush_buffers(codec_context);
    }

    int64_t seek_target=av_rescale_q(Position,AV_TIME_BASE_Q,LibavFile->streams[StreamNumber]->time_base);
    if (seek_target<0) seek_target=0;
    if (avformat_seek_file(LibavFile,StreamNumber,INT64_MIN,seek_target,INT64_MAX,AVSEEK_FLAG_BACKWARD)<0) {
        // Try in AVSEEK_FLAG_ANY mode
        if (av_seek_frame(LibavFile,StreamNumber,seek_target,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_ANY)<0) {
            // Try with default stream if exist
            int DefaultStream=av_find_default_stream_index(LibavFile);
            if ((DefaultStream==StreamNumber)||(Position>0)||(DefaultStream<0)||(av_seek_frame(LibavFile,DefaultStream,0,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_BYTE)<0)) {
                ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::ReadFrame : Seek error");
                ret=false;
            }
        }
    }
    return ret;
}

//====================================================================================================================

u_int8_t *cVideoFile::Resample(AVFrame *Frame,int64_t *SizeDecoded,int DstSampleSize) {
    u_int8_t *Data=NULL;
    #if defined(LIBAV) && (LIBAVVERSIONINT<=8)
        Data=(u_int8_t *)av_malloc(MaxAudioLenDecoded);
        if (Data) *SizeDecoded=audio_resample(RSC,(short int*)Data,(short int*)Frame->data[0],Frame->nb_samples)*DstSampleSize;
    #elif defined(LIBAV) && (LIBAVVERSIONINT<=9)
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
                *SizeDecoded=avresample_convert(RSC,out_data,out_linesize,out_samples,in_data,in_linesize,Frame->nb_samples)*DstSampleSize;
            }
        }
    #elif defined(FFMPEG)
        Data=(u_int8_t *)av_malloc(MaxAudioLenDecoded);
        u_int8_t *out[]={Data};
        if (Data) *SizeDecoded=swr_convert(RSC,out,MaxAudioLenDecoded/DstSampleSize,(const u_int8_t **)Frame->data,Frame->nb_samples)*DstSampleSize;
    #endif
    return Data;
}

//====================================================================================================================
// Read a frame from current stream
//====================================================================================================================
// maximum diff between asked image position and founded image position
#define ALLOWEDDELTA    250000
// diff between asked image position and current image position before exit loop and return black frame
#define MAXDELTA        2500000

// Remark: Position must use AV_TIMEBASE Unit
QImage *cVideoFile::ReadFrame(bool PreviewMode,int64_t Position,bool DontUseEndPos,bool Deinterlace,cSoundBlockList *SoundTrackBloc,double Volume,bool ForceSoundOnly) {
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

    AVStream *AudioStream =((AudioStreamNumber>=0)&&(SoundTrackBloc)?LibavAudioFile->streams[AudioStreamNumber]:NULL);
    AVStream *VideoStream =((!MusicOnly)&&(!ForceSoundOnly)&&(VideoStreamNumber>=0)?LibavVideoFile->streams[VideoStreamNumber]:NULL);

    if ((!AudioStream)&&(!VideoStream)) return NULL;

    Mutex.lock();

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
        if ((!LastImage.isNull())&&(FrameBufferYUVReady)&&(FrameBufferYUVPosition>=dEndFile*AV_TIME_BASE-FPSDuration)) {
            Mutex.unlock();
            return new QImage(LastImage.copy());
        }
        // If not then change Position to end file - a FPS to prepare a last image
        Position=dEndFile*AV_TIME_BASE-FPSDuration;
        dPosition=double(Position)/AV_TIME_BASE;
    }

    //================================================

    QImage   *RetImage         =NULL;
    AVPacket *StreamPacket     =NULL;
    bool     ContinueAudio     =(AudioStream!=NULL);
    bool     ContinueVideo     =true;
    double   FrameDuration     =0;
    bool     NeedResampling    =false;
    int64_t  AudioLenDecoded   =0;
    double   AudioFramePosition=dPosition;
    double   VideoFramePosition=dPosition;
    bool     ResamplingContinue=(Position!=0);

    if (AudioStream) {
        NeedResampling=((AudioStream->codec->sample_fmt!=AV_SAMPLE_FMT_S16)||
                        (AudioStream->codec->channels!=SoundTrackBloc->Channels)||
                        (AudioStream->codec->sample_rate!=SoundTrackBloc->SamplingRate));

        // Calc if we need to seek to a position
        int64_t DiffTimePosition=-1000000;  // Compute difftime between asked position and previous end decoded position
        if (SoundTrackBloc->CurrentPosition!=-1) DiffTimePosition=0;
        if ((Position==0)||(DiffTimePosition<0)||(DiffTimePosition>1500000)) {// Allow 1,5 sec diff (rounded double !)
            if (Position<0) Position=0;
            SoundTrackBloc->ClearList();                // Clear soundtrack list
            ResamplingContinue=false;
            LastAudioReadedPosition=0;
            SeekFile(NULL,AudioStream,Position);        // Always seek one FPS before to ensure eventual filter have time to init
            AudioFramePosition=Position/AV_TIME_BASE;
        }

        // Prepare resampler
        if (NeedResampling) {
            if (!ResamplingContinue) CloseResampler();
            CheckResampler(AudioStream->codec->channels,SoundTrackBloc->Channels,
                           AudioStream->codec->sample_fmt,SoundTrackBloc->SampleFormat,
                           AudioStream->codec->sample_rate,SoundTrackBloc->SamplingRate
                           #if (defined(LIBAV)&&(LIBAVVERSIONINT>=9)) || defined(FFMPEG)
                           ,AudioStream->codec->channel_layout
                           ,av_get_default_channel_layout(SoundTrackBloc->Channels)
                           #endif
                           );
        }

        // Check if we need to continue loop
        // Note: FPSDuration*(!VideoStream?2:1) is to enhance preview speed
        ContinueAudio=((AudioStream)&&(!(
                        ((LastAudioReadedPosition>=Position+FPSDuration*((PreviewMode&&(!VideoStream))?2:1))&&(SoundTrackBloc->List.count()>=SoundTrackBloc->NbrPacketForFPS))||
                        (LastAudioReadedPosition>=int64_t(dEndFile*AV_TIME_BASE)))
                      ));
        /*if ((!ContinueAudio)&&(AudioStream)) {
            ToLog(LOGMSG_INFORMATION,QString("ContinueAudio=false,Position=%1,LastAudioReadedPosition=%2,SoundTrackBloc->List.count=%3").arg(Position).arg(LastAudioReadedPosition).arg(SoundTrackBloc->List.count()));
        }*/
    }

    // Count number of image > position
    int Nbr=0;
    for (int CNbr=0;CNbr<CacheImage.count();CNbr++) if ((CacheImage[CNbr]->Position>=Position)&&(CacheImage[CNbr]->Position-Position<ALLOWEDDELTA)) Nbr++;
    bool IsVideoFind=Nbr>0;

    ContinueVideo=((VideoStream)&&(!IsVideoFind));
    if (ContinueVideo) {
        int64_t DiffTimePosition=-1000000;  // Compute difftime between asked position and previous end decoded position

        if (FrameBufferYUVReady) {
            DiffTimePosition=Position-FrameBufferYUVPosition;
            //if ((Position==0)||(DiffTimePosition<0)||(DiffTimePosition>1500000))
            //    ToLog(LOGMSG_INFORMATION,QString("VIDEO-SEEK %1 TO %2").arg(ShortName).arg(Position));
        }

        // Calc if we need to seek to a position
        if ((Position==0)||(DiffTimePosition<0)||(DiffTimePosition>1500000)) {// Allow 1,5 sec diff (rounded double !)
            if (Position<0) Position=0;
            SeekFile(VideoStream,NULL,Position);        // Always seek one FPS before to ensure eventual filter have time to init
            VideoFramePosition=Position/AV_TIME_BASE;
        }
    }

    //*************************************************************************************************************************************
    // Decoding process : Get StreamPacket until endposition is reach (if sound is wanted) or until image is ok (if image only is wanted)
    //*************************************************************************************************************************************

    // AUDIO PART
    while (ContinueAudio) {

        StreamPacket=new AVPacket();
        if (!StreamPacket) {
            ContinueAudio=false;
        } else {
            av_init_packet(StreamPacket);
            StreamPacket->flags|=AV_PKT_FLAG_KEY;  // HACK for CorePNG to decode as normal PNG by default

            if (av_read_frame(LibavAudioFile,StreamPacket)<0) {
                // If error reading frame
                if (FPSSize>AudioLenDecoded) {
                    u_int8_t *BufferForDecoded=(u_int8_t *)av_malloc(FPSSize-AudioLenDecoded);
                    memset(BufferForDecoded,0,FPSSize-AudioLenDecoded);
                    SoundTrackBloc->AppendData(int64_t(dEndFile*AV_TIME_BASE),(int16_t*)BufferForDecoded,FPSSize-AudioLenDecoded);
                    av_free(BufferForDecoded);
                    AudioLenDecoded=FPSSize;
                }
                SoundTrackBloc->ClearList();      // Clear soundtrack list
                ResamplingContinue=false;
                LastAudioReadedPosition=0;
                dEndFile=AudioFramePosition;
                if (dEndFile==double(QTime(0,0,0,0).msecsTo(EndPos))) EndPos=QTime(0,0,0).addMSecs(AudioFramePosition*1000);
                ContinueAudio=SeekFile(NULL,AudioStream,Position-2*FPSDuration); // Seek 2*FPSDuration before and modify var to try to loop again
            } else {
                int64_t FramePts=StreamPacket->pts!=(int64_t)AV_NOPTS_VALUE?StreamPacket->pts:-1;
                double  TimeBase=double(LibavAudioFile->streams[StreamPacket->stream_index]->time_base.den)/double(LibavAudioFile->streams[StreamPacket->stream_index]->time_base.num);
                if (FramePts>=0) AudioFramePosition=(double(FramePts)/TimeBase);

                if ((StreamPacket->stream_index==AudioStreamNumber)&&(StreamPacket->size>0)) {
                    AVPacket PacketTemp;
                    av_init_packet(&PacketTemp);
                    PacketTemp.data=StreamPacket->data;
                    PacketTemp.size=StreamPacket->size;

                    // NOTE: the audio packet can contain several NbrFrames
                    while ((ContinueAudio)&&(PacketTemp.size>0)) {
                        AVFrame *Frame=ALLOCFRAME();
                        int     got_frame;
                        int     Len=avcodec_decode_audio4(AudioStream->codec,Frame,&got_frame,&PacketTemp);
                        if (Len<0) {
                            // if error, we skip the frame and exit the while loop
                            PacketTemp.size=0;
                        } else if (got_frame>0) {
                            int64_t  SizeDecoded=0;
                            u_int8_t *Data      =NULL;
                            if ((NeedResampling)&&(RSC!=NULL)) {
                                Data=Resample(Frame,&SizeDecoded,DstSampleSize);
                            } else {
                                Data=Frame->data[0];
                                #if (defined(LIBAV)&&(LIBAVVERSIONINT<=8))
                                SizeDecoded=Frame->nb_samples*av_get_bytes_per_sample(AudioStream->codec->sample_fmt)*AudioStream->codec->channels;
                                #elif (defined(LIBAV)&&(LIBAVVERSIONINT<=9))
                                SizeDecoded=av_samples_get_buffer_size(NULL,AudioStream->codec->channels,Frame->nb_samples,AudioStream->codec->sample_fmt,0);
                                #elif defined(FFMPEG)
                                SizeDecoded=Frame->nb_samples*av_get_bytes_per_sample(AudioStream->codec->sample_fmt)*AudioStream->codec->channels;
                                #endif
                            }
                            ContinueAudio=(Data!=NULL);
                            if (ContinueAudio) {
                                // Adjust FrameDuration with real Nbr Sample
                                FrameDuration=double(SizeDecoded)/(SoundTrackBloc->SamplingRate*DstSampleSize);

                                // Adjust pts and inc FramePts int the case there is multiple blocks
                                int64_t pts=int64_t((double(FramePts)/TimeBase)*AV_TIME_BASE);
                                if (pts<0) pts=int64_t((double(Position+FPSDuration)/TimeBase)*AV_TIME_BASE);
                                FramePts+=FrameDuration*AV_TIME_BASE;
                                AudioFramePosition=qreal(pts)/1000000;

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
    //                                if ((!CacheImage.isEmpty())&&(!SoundTrackBloc->Adjusted))
    //                                    SoundTrackBloc->AdjustSoundPosition(pts>0?pts:Position,CacheImage.first()->Position);

                                // Append decoded data to SoundTrackBloc
                                SoundTrackBloc->AppendData(/*Position*/pts,(int16_t*)Data,SizeDecoded);
                                AudioLenDecoded     +=SizeDecoded;
                                PacketTemp.data     +=Len;
                                PacketTemp.size     -=Len;
                                AudioFramePosition   =AudioFramePosition+FrameDuration;
                            }

                            LastAudioReadedPosition =int64_t(AudioFramePosition*AV_TIME_BASE);    // Keep NextPacketPosition for determine next time if we need to seek
                            if (Data!=Frame->data[0]) av_free(Data);
                        }
                        FREEFRAME(&Frame);
                    }
                }
            }

            // Check if we need to continue loop
            // Note: FPSDuration*(!VideoStream?2:1) is to enhance preview speed
            ContinueAudio=ContinueAudio && ((AudioStream)&&(SoundTrackBloc)&&(!(
                            ((LastAudioReadedPosition>=Position+FPSDuration*((PreviewMode&&(!VideoStream))?2:1))&&(SoundTrackBloc->List.count()>=SoundTrackBloc->NbrPacketForFPS))||
                            (LastAudioReadedPosition>=int64_t(dEndFile*AV_TIME_BASE)))
                        ));
        }
        // Continue with a new one
        if (StreamPacket!=NULL) {
            av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
            delete StreamPacket;
            StreamPacket=NULL;
        }
    }

    // VIDEO PART
    if (VideoStream) {
        if (!ContinueVideo) {
            ToLog(LOGMSG_DEBUGTRACE,QString("Video image for position %1 => use image in cache").arg(Position));
        } else {
            bool ByPassFirstImage=(Deinterlace)&&(CacheImage.count()==0);
            int  MaxErrorCount   =20;
            bool FreeFrames      =false;

            while (ContinueVideo) {
                StreamPacket=new AVPacket();
                if (!StreamPacket) {
                    ContinueVideo=false;
                } else {
                    av_init_packet(StreamPacket);
                    StreamPacket->flags|=AV_PKT_FLAG_KEY;  // HACK for CorePNG to decode as normal PNG by default

                    if (av_read_frame(LibavVideoFile,StreamPacket)<0) {
                        // If error reading frame
                        if ((!LastImage.isNull())&&(FrameBufferYUVReady)&&(FrameBufferYUVPosition>=(dEndFile-1.5)*AV_TIME_BASE)) {
                            if (!RetImage) RetImage=new QImage(LastImage);
                            IsVideoFind=true;
                            ContinueVideo=false;
                        } else {
                            dEndFile=VideoFramePosition;
                            ContinueVideo=SeekFile(VideoStream,NULL,Position-2*FPSDuration);
                            if (dEndFile==double(QTime(0,0,0,0).msecsTo(EndPos))) EndPos=QTime(0,0,0).addMSecs(VideoFramePosition*1000);
                        }
                    } else {

                        int64_t FramePts=StreamPacket->pts!=(int64_t)AV_NOPTS_VALUE?StreamPacket->pts:-1;
                        double  TimeBase=double(LibavVideoFile->streams[StreamPacket->stream_index]->time_base.den)/double(LibavVideoFile->streams[StreamPacket->stream_index]->time_base.num);
                        if (FramePts>=0) VideoFramePosition=(double(FramePts)/TimeBase);

                        if (StreamPacket->stream_index==VideoStreamNumber) {

                            // Allocate structures
                            if (FrameBufferYUV==NULL) FrameBufferYUV=ALLOCFRAME();
                            if (FrameBufferYUV) {

                                #if defined(LIBAV) || (defined(FFMPEG)&&(FFMPEGVERSIONINT<201))
                                if (FrameBufferYUV->opaque) {
                                    avfilter_unref_buffer((AVFilterBufferRef *)FrameBufferYUV->opaque);
                                    FrameBufferYUV->opaque=NULL;
                                }
                                #endif

                                int FrameDecoded=0;
                                LastLibAvMessageLevel=0;    // Clear LastLibAvMessageLevel : some decoder dont return error but display errors messages !
                                int Error=avcodec_decode_video2(VideoStream->codec,FrameBufferYUV,&FrameDecoded,StreamPacket);
                                if ((Error<0)||(LastLibAvMessageLevel==LOGMSG_CRITICAL)) {
                                    if (MaxErrorCount>0) {
                                        if (VideoFramePosition*1000000<Position) {
                                            ToLog(LOGMSG_INFORMATION,QString("IN:cVideoFile::ReadFrame - Error decoding packet: try left %1").arg(MaxErrorCount));
                                        } else {
                                            ToLog(LOGMSG_INFORMATION,QString("IN:cVideoFile::ReadFrame - Error decoding packet: seek to backward and restart reading"));
                                            if (Position>1000000) SeekFile(VideoStream,NULL/*AudioStream*/,Position-1000000); // 1 sec before
                                                else SeekFile(VideoStream,NULL,0);
                                        }
                                        MaxErrorCount--;
                                    } else {
                                        ToLog(LOGMSG_CRITICAL,QString("IN:cVideoFile::ReadFrame - Error decoding packet: and no try left"));
                                        ContinueVideo=false;
                                    }
                                } else if (FrameDecoded>0) {

                                    #if defined(FFMPEG)&&(FFMPEGVERSIONINT>=201)
                                    FrameBufferYUV->pkt_pts = av_frame_get_best_effort_timestamp(FrameBufferYUV);
                                    #endif

                                    // Video filter part
                                    if ((Deinterlace)&&(!VideoFilterGraph))           VideoFilter_Open();
                                        else if ((!Deinterlace)&&(VideoFilterGraph))  VideoFilter_Close();

                                    #if defined(LIBAV) || (defined(FFMPEG)&&(FFMPEGVERSIONINT<201))
                                    if (VideoFilterGraph) VideoFilter_Process();
                                    #elif defined(FFMPEG)&&(FFMPEGVERSIONINT>=201)
                                    AVFrame *FiltFrame=NULL;
                                    if (VideoFilterGraph) {
                                        // FFMPEG 2.0
                                        // push the decoded frame into the filtergraph
                                        if (av_buffersrc_add_frame_flags(VideoFilterIn,FrameBufferYUV,AV_BUFFERSRC_FLAG_KEEP_REF)<0) {
                                            ToLog(LOGMSG_INFORMATION,"IN:cVideoFile::ReadFrame : Error while feeding the filtergraph");
                                        } else {
                                            FiltFrame=av_frame_alloc();
                                            // pull filtered frames from the filtergraph
                                            int ret=av_buffersink_get_frame(VideoFilterOut,FiltFrame);
                                            if ((ret<0)||(ret==AVERROR(EAGAIN))||(ret==AVERROR_EOF)) {
                                                ToLog(LOGMSG_INFORMATION,"IN:cVideoFile::ReadFrame : No image return by filter process");
                                                av_frame_free(&FiltFrame);
                                                FiltFrame=NULL;
                                            }
                                        }
                                    }
                                    #endif
                                    if (ByPassFirstImage) {
                                        ByPassFirstImage=false;
                                        FreeFrames      =true;
                                    } else {
                                        int64_t pts=FrameBufferYUV->pkt_pts;
                                        if (pts==(int64_t)AV_NOPTS_VALUE) {
                                            if (FrameBufferYUV->pkt_dts!=(int64_t)AV_NOPTS_VALUE) {
                                                pts=FrameBufferYUV->pkt_dts;
                                                ToLog(LOGMSG_DEBUGTRACE,QString("IN:cVideoFile::ReadFrame : No PTS so use DTS %1 for position %2").arg(pts).arg(Position));
                                            } else {
                                                pts=0;
                                                ToLog(LOGMSG_DEBUGTRACE,QString("IN:cVideoFile::ReadFrame : No PTS and no DTS for position %1").arg(Position));
                                            }
                                        }
                                        FrameBufferYUVReady   =true;                                            // Keep actual value for FrameBufferYUV
                                        FrameBufferYUVPosition=int64_t((double(pts)/TimeBase)*AV_TIME_BASE);    // Keep actual value for FrameBufferYUV

                                        // Append this frame
                                        cImageInCache *ObjImage=
                                            #if defined(LIBAV) || (defined(FFMPEG)&&(FFMPEGVERSIONINT<201))
                                                new cImageInCache(FrameBufferYUVPosition,NULL,FrameBufferYUV);
                                            #else
                                                new cImageInCache(FrameBufferYUVPosition,FiltFrame,FrameBufferYUV);
                                            #endif
                                        FreeFrames=false;
                                        int ToIns=0;
                                        while ((ToIns<CacheImage.count())&&(CacheImage[ToIns]->Position<ObjImage->Position)) ToIns++;
                                        if (ToIns<CacheImage.count()) {
                                            CacheImage.insert(ToIns,ObjImage);
                                            ToLog(LOGMSG_DEBUGTRACE,QString("IN:cVideoFile::ReadFrame : Insert image %1 for position %2 (FramePosition=%3) - Key:%4 PTS:%5 Num:%6").arg(FrameBufferYUVPosition).arg(Position).arg(VideoFramePosition).arg(FrameBufferYUV->key_frame).arg(FrameBufferYUV->pkt_pts).arg(FrameBufferYUV->coded_picture_number));
                                        } else {
                                            CacheImage.append(ObjImage);
                                            ToLog(LOGMSG_DEBUGTRACE,QString("IN:cVideoFile::ReadFrame : Append image %1 for position %2 (FramePosition=%3) - Key:%4 PTS:%5 Num:%6").arg(FrameBufferYUVPosition).arg(Position).arg(VideoFramePosition).arg(FrameBufferYUV->key_frame).arg(FrameBufferYUV->pkt_pts).arg(FrameBufferYUV->coded_picture_number));
                                        }

                                        // Count number of image > position
                                        int Nbr=0;
                                        for (int CNbr=0;CNbr<CacheImage.count();CNbr++) if ((CacheImage[CNbr]->Position>=Position)&&(CacheImage[CNbr]->Position-Position<ALLOWEDDELTA)) Nbr++;
                                        IsVideoFind=Nbr>0;
                                    }
                                    if (FreeFrames) {
                                        #if defined(LIBAV) || (defined(FFMPEG)&&(FFMPEGVERSIONINT<201))
                                        if (FrameBufferYUV->opaque) {
                                            avfilter_unref_buffer((AVFilterBufferRef *)FrameBufferYUV->opaque);
                                            FrameBufferYUV->opaque=NULL;
                                        }
                                        #else
                                        if (FiltFrame) {
                                            av_frame_unref(FiltFrame);
                                            av_frame_free(&FiltFrame);
                                            av_frame_unref(FrameBufferYUV);
                                        }
                                        #endif
                                        FREEFRAME(&FrameBufferYUV);
                                    } else {
                                        FrameBufferYUV=NULL;
                                        #if defined(FFMPEG)&&(FFMPEGVERSIONINT>=201)
                                        FiltFrame     =NULL;
                                        #endif
                                    }
                                }
                            }
                        }
                    }
                    // Check if we need to continue loop
                    // Note: FPSDuration*(!VideoStream?2:1) is to enhance preview speed
                    ContinueVideo=ContinueVideo && ((VideoStream)&&(!IsVideoFind)&&((VideoFramePosition*1000000<Position)||(VideoFramePosition*1000000-Position<MAXDELTA)));
                }

                // Continue with a new one
                if (StreamPacket!=NULL) {
                    av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
                    delete StreamPacket;
                    StreamPacket=NULL;
                }
            }
        }
    }

    if (VideoStream) {
        if ((!RetImage)&&(CacheImage.count()>0)) {
            // search nearest image (allowed up to MAXDELTA, after return black frame)
            int i=-1,Nearest=MAXDELTA;
            for (int jj=0;jj<CacheImage.count();jj++) if ((CacheImage[jj]->Position>=Position)&&(CacheImage[jj]->Position-Position<MAXDELTA))
                if ((i==-1)||(CacheImage[jj]->Position-Position<Nearest)) {
                    i=jj;
                    Nearest=CacheImage[jj]->Position-Position;
                }
            if ((i>=0)&&(i<CacheImage.count())/*&&(CacheImage[i]->Position>=Position)&&(CacheImage[i]->Position-Position<100000)*/) {
                RetImage=ConvertYUVToRGB(PreviewMode,CacheImage[i]->FiltFrame?CacheImage[i]->FiltFrame:CacheImage[i]->FrameBufferYUV);
                ToLog(LOGMSG_DEBUGTRACE,QString("Video image for position %1 => return image at %2").arg(Position).arg(CacheImage[i]->Position));
            } else {
                //QString A; for (int jj=0;jj<CacheImage.count();jj++) A=A+QString("%1 ").arg(CacheImage[jj]->Position); qDebug()<<A;
                ToLog(LOGMSG_CRITICAL,QString("No video image return for position %1 => return image at %2").arg(Position).arg(CacheImage[0]->Position));
                RetImage=ConvertYUVToRGB(PreviewMode,CacheImage[0]->FiltFrame?CacheImage[0]->FiltFrame:CacheImage[0]->FrameBufferYUV);
            }
        }

        if (!RetImage) {
            ToLog(LOGMSG_CRITICAL,QString("No video image return for position %1 => return black frame").arg(Position));
            RetImage =new QImage(LibavVideoFile->streams[VideoStreamNumber]->codec->width,LibavVideoFile->streams[VideoStreamNumber]->codec->height,QImage::Format_ARGB32_Premultiplied);
            RetImage->fill(0);
        }
        int i=0;
        while (i<CacheImage.count()) {
            if (CacheImage[i]->Position<Position-50000) delete(CacheImage.takeAt(i));
                else i++;
        }
    }

    Mutex.unlock();
    return RetImage;
}

//====================================================================================================================

QImage *cVideoFile::ConvertYUVToRGB(bool PreviewMode,AVFrame *Frame) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ConvertYUVToRGB");

    int W=Frame->width*AspectRatio;    W-=(W%4);   // W must be a multiple of 4 ????
    int H=Frame->height;

    LastImage=QImage(W,H,QTPIXFMT);
    // Allocate structure for RGB image
    AVFrame *FrameBufferRGB=ALLOCFRAME();

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
            Frame->width,                                                     // Src width
            Frame->height,                                                    // Src height
            (PixelFormat)Frame->format,                                       // Src Format
            W,                                                                // Destination width
            H,                                                                // Destination height
            PIXFMT,                                                           // Destination Format
            SWS_BICUBIC,NULL,NULL,NULL);                                      // flags,src Filter,dst Filter,param

        if (img_convert_ctx!=NULL) {
            int ret;
            #if defined(LIBAV) || (defined(FFMPEG)&&(FFMPEGVERSIONINT<201))
            if (Frame->opaque) {
                AVFilterBufferRef *Buf=(AVFilterBufferRef *)Frame->opaque;
                ret = sws_scale(
                    img_convert_ctx,                                           // libswscale converter
                    Buf->data,                                                 // Source buffer
                    Buf->linesize,                                             // Source Stride ?
                    0,                                                         // Source SliceY:the position in the source image of the slice to process, that is the number (counted starting from zero) in the image of the first row of the slice
                    Frame->height,                                             // Source SliceH:the height of the source slice, that is the number of rows in the slice
                    FrameBufferRGB->data,                                      // Destination buffer
                    FrameBufferRGB->linesize                                   // Destination Stride
                );
            } else
            #endif
            {
                ret = sws_scale(
                    img_convert_ctx,                                           // libswscale converter
                    Frame->data,                                               // Source buffer
                    Frame->linesize,                                           // Source Stride ?
                    0,                                                         // Source SliceY:the position in the source image of the slice to process, that is the number (counted starting from zero) in the image of the first row of the slice
                    Frame->height,                                             // Source SliceH:the height of the source slice, that is the number of rows in the slice
                    FrameBufferRGB->data,                                      // Destination buffer
                    FrameBufferRGB->linesize                                   // Destination Stride
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
        FREEFRAME(&FrameBufferRGB);
    }

    //return FinalImage;
    return new QImage(LastImage.copy());
}

//====================================================================================================================
// Load a video frame
// DontUseEndPos default=false
QImage *cVideoFile::ImageAt(bool PreviewMode,int64_t Position,cSoundBlockList *SoundTrackBloc,bool Deinterlace,
                            double Volume,bool ForceSoundOnly,bool DontUseEndPos) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ImageAt");

    if (!IsValide) return NULL;
    if (!IsOpen) OpenCodecAndFile();

    if ((PreviewMode)&&(!SoundTrackBloc)) {
        // for speed improvment, try to find image in cache (only for interface)
        cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileKey,ModifDateTime,ImageOrientation,ApplicationConfig->Smoothing,true);
        if (!ImageObject) return ReadFrame(PreviewMode,Position*1000,DontUseEndPos,Deinterlace,SoundTrackBloc,Volume,ForceSoundOnly);

        if ((ImageObject->Position==Position)&&(ImageObject->CachePreviewImage)) return new QImage(ImageObject->CachePreviewImage->copy());

        if (ImageObject->CachePreviewImage) {
            delete ImageObject->CachePreviewImage;
            ImageObject->CachePreviewImage=NULL;
        }
        ApplicationConfig->ImagesCache.FreeMemoryToMaxValue();
        ImageObject->Position=Position;
        ImageObject->CachePreviewImage=ReadFrame(PreviewMode,Position*1000,DontUseEndPos,Deinterlace,SoundTrackBloc,Volume,ForceSoundOnly);
        if (ImageObject->CachePreviewImage) return new QImage(ImageObject->CachePreviewImage->copy());
            else return NULL;

    } else return ReadFrame(PreviewMode,Position*1000,DontUseEndPos,Deinterlace,SoundTrackBloc,Volume,ForceSoundOnly);
}

//====================================================================================================================

int cVideoFile::getThreadFlags(AVCodecID ID) {
    int Ret=0;
    switch (ID) {
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

    //**********************************
    // Open audio stream
    //**********************************
    if (AudioStreamNumber!=-1) {

        // Open the file and get a LibAVFormat context and an associated LibAVCodec decoder
        if (avformat_open_input(&LibavAudioFile,FileName().toLocal8Bit(),NULL,NULL)!=0) return false;
        LibavAudioFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future NbrFrames.
        if (avformat_find_stream_info(LibavAudioFile,NULL)<0) {
            avformat_close_input(&LibavAudioFile);
            return false;
        }

        AVStream *AudioStream=LibavAudioFile->streams[AudioStreamNumber];

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
            //Mutex.unlock();
            return false;
        }

        IsVorbis=(strcmp(AudioDecoderCodec->name,"vorbis")==0);
    }

    //**********************************
    // Open video stream
    //**********************************
    if ((VideoStreamNumber!=-1)&&(!MusicOnly)) {

        // Open the file and get a LibAVFormat context and an associated LibAVCodec decoder
        if (avformat_open_input(&LibavVideoFile,FileName().toLocal8Bit(),NULL,NULL)!=0) return false;
        LibavVideoFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future NbrFrames.
        if (avformat_find_stream_info(LibavVideoFile,NULL)<0) {
            avformat_close_input(&LibavVideoFile);
            return false;
        }

        AVStream *VideoStream=LibavVideoFile->streams[VideoStreamNumber];

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

        if ((VideoDecoderCodec==NULL)||(avcodec_open2(VideoStream->codec,VideoDecoderCodec,NULL)<0)) return false;
    }
    IsOpen=true;
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

void cMusicObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath,cReplaceObjectList *ReplaceList) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cMusicObject::SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    QString         TheFileName;

    if (ReplaceList) {
        TheFileName=ReplaceList->GetDestinationFileName(FileName());
    } else if (PathForRelativPath!="") {
        if (ForceAbsolutPath) TheFileName=QDir(QFileInfo(PathForRelativPath).absolutePath()).absoluteFilePath(FileName());
            else TheFileName=QDir(QFileInfo(PathForRelativPath).absolutePath()).relativeFilePath(FileName());
    } else TheFileName=FileName();

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

        QString FileName=Element.attribute("FilePath","");
        if ((!QFileInfo(FileName).exists())&&(PathForRelativPath!="")) {
            FileName=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(FileName));
            //QString PA=QDir(PathForRelativPath).absolutePath();
            //if (!PA.endsWith(QDir::separator())) PA=PA+QDir::separator();
            //PA=PA+FileName;
            //FileName=QDir::cleanPath(PA);
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

    IsValide=(GetInformationFromFile(TheFilename,AliasList,ModifyFlag,FolderKey))&&(OpenCodecAndFile());
    return IsValide;
}

//====================================================================================================================

bool cMusicObject::LoadBasicInformationFromDatabase(QDomElement root) {
    Duration=QTime(0,0,0,0).addMSecs(root.attribute("Duration").toLongLong());
    return true;
}

//====================================================================================================================

void cMusicObject::SaveBasicInformationToDatabase(QDomElement *root) {
    root->setAttribute("Duration",QTime(0,0,0,0).msecsTo(Duration));
}
