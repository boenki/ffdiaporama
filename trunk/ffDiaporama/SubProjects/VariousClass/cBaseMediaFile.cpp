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

// Include some common various class
#include "cBaseApplicationConfig.h"

// Include some additional standard class
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>

// Include some additional standard class
#include "cDeviceModelDef.h"
#include "cBaseMediaFile.h"

extern "C" {
    //#include <libavutil/opt.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/avutil.h>
}

//#define DEBUGMODE

//****************************************************************************************************************************************************************

#include "../TAGLib/fileref.h"
#include "../TAGLib/tbytevector.h"

//---- MP3 Files
#include "../TAGLib/id3v2tag.h"
#include "../TAGLib/id3v2frame.h"
#include "../TAGLib/id3v2header.h"
#include "../TAGLib/id3v2framefactory.h"
#include "../TAGLib/attachedpictureframe.h"
#include "../TAGLib/mpegfile.h"
#include "../TAGLib/flacfile.h"
#include "../TAGLib/mp4file.h"
#include "../TAGLib/vorbisfile.h"
#include "../TAGLib/oggflacfile.h"
#include "../TAGLib/asffile.h"
#include "../TAGLib/mp4tag.h"
#include "../TAGLib/mp4item.h"
#include "../TAGLib/mp4coverart.h"

// from Google music manager (see:http://code.google.com/p/gogglesmm/source/browse/src/gmutils.cpp?spec=svn6c3dbecbad40ee49736b9ff7fe3f1bfa6ca18c13&r=6c3dbecbad40ee49736b9ff7fe3f1bfa6ca18c13)
bool gm_decode_base64(uchar *buffer,uint &len) {
    static const char base64[256]={
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

QImage *GetEmbededImage(QString FileName) {

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
    //*********** OGG
    if ((Image->isNull())&&((QFileInfo(FileName).suffix().toLower()=="ogg")||(QFileInfo(FileName).suffix().toLower()=="oga"))) {
        TagLib::Vorbis::File        OggFile(TagLib::FileName(FileName.toLocal8Bit()));
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
    if ((Image->isNull())&&(/*(QFileInfo(FileName).suffix().toLower()=="mp4")||*/(QFileInfo(FileName).suffix().toLower()=="m4a")||(QFileInfo(FileName).suffix().toLower()=="m4v"))) {
        TagLib::MP4::File MP4File(TagLib::FileName(FileName.toLocal8Bit()));

        if (MP4File.tag()) {
            if (MP4File.tag()->itemListMap().contains("covr")) {
                TagLib::MP4::CoverArtList coverArtList = MP4File.tag()->itemListMap()["covr"].toCoverArtList();
                if (coverArtList.size()!= 0) {
                    TagLib::MP4::CoverArt ca = coverArtList.front();
                    Image->loadFromData((const uchar *) ca.data().data(),ca.data().size());
                }
            }
        }
    }
    //*********** ASF/WMA //////////////////// A FINIR !
    /*
    if ((Image->isNull())&&(QFileInfo(FileName).suffix().toLower()=="wma")) {
        TagLib::ASF::File ASFFile(TagLib::FileName(TagLib::FileName(FileName.toLocal8Bit())));
        if ((ASFFile.tag())&&(!ASFFile.tag()->attributeListMap()["WM/Picture"].isEmpty())) {
            TagLib::ID3v2::FrameList l=ASFFile.tag()->attributeListMap()["WM/Picture"];
            if (!l.isEmpty()) {
                TagLib::ID3v2::AttachedPictureFrame *pic=static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l.front());
                if (pic) Image->loadFromData((const uchar *)pic->picture().data(),pic->picture().size());
            }
            qDebug()<<"ICI";
        }
    }*/
    //***********
    if (!Image->isNull()) return Image; else {
        delete Image;
        return NULL;
    }
}

//*********************************************************************************************************************************************
// Base class object
//*********************************************************************************************************************************************

cBaseMediaFile::cBaseMediaFile(cBaseApplicationConfig *TheApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::cBaseMediaFile";
    #endif
    ObjectType          = OBJECTTYPE_UNMANAGED;
    IsValide            = false;                                    // if true then object if initialise
    IsInformationValide = false;                                    // if true then information list if fuly initialise
    ObjectGeometry      = IMAGE_GEOMETRY_UNKNOWN;                   // Image geometry
    FileName            = "";                                       // filename
    ShortName           = "";
    FileSize            = 0;
    FileSizeText        = "";
    ImageWidth          = 0;                                        // Widht of normal image
    ImageHeight         = 0;                                        // Height of normal image
    CreatDateTime       = QDateTime(QDate(0,0,0),QTime(0,0,0));     // Original date/time
    ModifDateTime       = QDateTime(QDate(0,0,0),QTime(0,0,0));     // Last modified date/time
    ApplicationConfig   = TheApplicationConfig;
    WEBInfo             = "";
    AspectRatio         = 1;
}

//====================================================================================================================

cBaseMediaFile::~cBaseMediaFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::~cBaseMediaFile";
    #endif
}

//====================================================================================================================

bool cBaseMediaFile::GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::GetInformationFromFile";
    #endif
    FileName            =QFileInfo(GivenFileName).absoluteFilePath();
    ShortName           =QFileInfo(FileName).fileName();
    FileSize            =QFileInfo(FileName).size();
    FileSizeText        =GetTextSize(FileSize);
    CreatDateTime       =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime       =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !

    if (ModifyFlag) *ModifyFlag=false;

    // Use aliaslist
    if (AliasList) {
        int i;
        for (i=0;(i<AliasList->count())&&(!AliasList->at(i).startsWith(FileName));i++);
        if ((i<AliasList->count())&&(AliasList->at(i).startsWith(FileName))) {
            FileName=AliasList->at(i);
            if (FileName.indexOf("####")>0) FileName=FileName.mid(FileName.indexOf("####")+QString("####").length());
        }
    }

    bool Continue=true;
    while ((Continue)&&(!QFileInfo(FileName).exists())) {
        if (QMessageBox::question(ApplicationConfig->TopLevelWindow,QApplication::translate("cBaseMediaFile","Open file"),
            QApplication::translate("cBaseMediaFile","Impossible to open file ")+FileName+"\n"+QApplication::translate("cBaseMediaFile","Do you want to select another file ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes)
            Continue=false;
        else {
            QString NewFileName=QFileDialog::getOpenFileName(ApplicationConfig->TopLevelWindow,QApplication::translate("cBaseMediaFile","Select another file for ")+QFileInfo(FileName).fileName(),
               ApplicationConfig->RememberLastDirectories?ApplicationConfig->LastMediaPath:"",
               ApplicationConfig->GetFilterForMediaFile(IMAGEFILE));
            if (NewFileName!="") {
                if (AliasList) AliasList->append(FileName+"####"+NewFileName);
                FileName=NewFileName;
                if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(FileName).absolutePath();     // Keep folder for next use
                if (ModifyFlag) *ModifyFlag=true;
            } else Continue=false;
        }
    }
    if (!Continue) {
        qDebug()<<"Impossible to open file"<<FileName;
        return false;
    }

    IsValide=true;

    return IsValide;
}

//====================================================================================================================

QString cBaseMediaFile::GetInformationValue(QString ValueToSearch) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::GetInformationValue";
    #endif
    int i=0;
    while ((i<InformationList.count())&&(!((QString )InformationList[i]).startsWith(ValueToSearch+"##"))) i++;
    if ((i<InformationList.count())&&(((QString )InformationList[i]).startsWith(ValueToSearch))) {
        QStringList Values=((QString)InformationList[i]).split("##");
        if (Values.count()==2) return ((QString)Values[1]).trimmed();
    }
    return "";
}

//====================================================================================================================

void cBaseMediaFile::AddIcons(QString FileName) {
    QImage Img(FileName);
    if (Img.width()>Img.height()) {
        Icon16 =Img.scaledToWidth(16,Qt::SmoothTransformation);
        Icon32 =Img.scaledToWidth(32,Qt::SmoothTransformation);
        Icon48 =Img.scaledToWidth(48,Qt::SmoothTransformation);
        Icon100=Img.scaledToWidth(100,Qt::SmoothTransformation);
    } else {
        Icon16 =Img.scaledToHeight(16,Qt::SmoothTransformation);
        Icon32 =Img.scaledToHeight(32,Qt::SmoothTransformation);
        Icon48 =Img.scaledToHeight(48,Qt::SmoothTransformation);
        Icon100=Img.scaledToHeight(100,Qt::SmoothTransformation);
    }
}

//====================================================================================================================

void cBaseMediaFile::AddIcons(QImage *Image) {
    if (Image->width()>Image->height()) {
        Icon16 =Image->scaledToWidth(16,Qt::SmoothTransformation);
        Icon32 =Image->scaledToWidth(32,Qt::SmoothTransformation);
        Icon48 =Image->scaledToWidth(48,Qt::SmoothTransformation);
        Icon100=Image->scaledToWidth(100,Qt::SmoothTransformation);
    } else {
        Icon16 =Image->scaledToHeight(16,Qt::SmoothTransformation);
        Icon32 =Image->scaledToHeight(32,Qt::SmoothTransformation);
        Icon48 =Image->scaledToHeight(48,Qt::SmoothTransformation);
        Icon100=Image->scaledToHeight(100,Qt::SmoothTransformation);
    }
}

//====================================================================================================================

void cBaseMediaFile::AddIcons(QIcon Icon) {
    Icon16 =Icon.pixmap(16,16).toImage();
    Icon32 =Icon.pixmap(32,32).toImage();
    Icon48 =Icon.pixmap(48,48).toImage();
    Icon100=Icon.pixmap(100,100).toImage();
}

//====================================================================================================================

QString cBaseMediaFile::GetImageGeometryStr() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::GetImageGeometryStr";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::GetFileSizeStr";
    #endif
    return FileSizeText;
}

//====================================================================================================================

QString cBaseMediaFile::GetFileDateTimeStr(bool Created) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::GetFileDateTimeStr";
    #endif
    if (Created) return CreatDateTime.toString();
        else return ModifDateTime.toString();
}

//====================================================================================================================

QString cBaseMediaFile::GetImageSizeStr(ImageSizeFmt Fmt) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::GetImageSizeStr";
    #endif
    QString SizeInfo="";
    QString FmtInfo ="";
    QString GeoInfo ="";

    if ((ImageWidth>0)&&(ImageHeight>0)) {
        // Compute MPix
        double MPix=double(double(ImageWidth)*double(ImageHeight))/double(1000000);
        int    RealImageWidth=ImageWidth;
        if (AspectRatio!=1) RealImageWidth=int(double(ImageWidth)/AspectRatio);
        SizeInfo=QString("%1x%2").arg(RealImageWidth).arg(ImageHeight);

        // now search if size is referenced in DefImageFormat
        for (int i=0;i<2;i++) for (int j=0;j<3;j++) for (int k=0;k<NBR_SIZEDEF;k++) if ((DefImageFormat[i][j][k].Width==RealImageWidth)&&(DefImageFormat[i][j][k].Height==ImageHeight)) {
            FmtInfo=QString(DefImageFormat[i][j][k].Name).left(QString(DefImageFormat[i][j][k].Name).indexOf(" -"));
            break;
        }
        if ((FmtInfo=="")&&(MPix>=1)) FmtInfo=QString("%1").arg(MPix,8,'f',1).trimmed()+QApplication::translate("cBaseMediaFile","MPix");
    }
    GeoInfo=GetImageGeometryStr();
    switch (Fmt) {
        case FULLWEB  : return SizeInfo+" ("+FmtInfo+(FmtInfo!=""?"-":"")+GeoInfo+")";
        case SIZEONLY : return SizeInfo;
        case FMTONLY  : return FmtInfo;
        case GEOONLY  : return GeoInfo;
        default       : return "";
    }
}

//====================================================================================================================

QString cBaseMediaFile::GetCumulInfoStr(QString Key1,QString Key2) {
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cUnmanagedFile::cUnmanagedFile";
    #endif
    AddIcons(DefaultFILEIcon);
    ObjectType  =OBJECTTYPE_UNMANAGED;
    IsInformationValide=true;
}

//====================================================================================================================

QString cUnmanagedFile::GetFileTypeStr() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cUnmanagedFile::GetFileTypeStr";
    #endif
    return QApplication::translate("cBaseMediaFile","Unmanaged","File type");
}

//====================================================================================================================

bool cUnmanagedFile::IsFilteredFile(int RequireObjectType) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cUnmanagedFile::IsFilteredFile";
    #endif
    return RequireObjectType==OBJECTTYPE_UNMANAGED;
}

//*********************************************************************************************************************************************
// Folder
//*********************************************************************************************************************************************

cFolder::cFolder(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFolder::cFolder";
    #endif
    AddIcons(DefaultFOLDERIcon);
    ObjectType  =OBJECTTYPE_FOLDER;
    IsInformationValide=true;
}

//====================================================================================================================

bool cFolder::GetInformationFromFile(QString GivenFileName,QStringList */*AliasList*/,bool */*ModifyFlag*/) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFolder::GetInformationFromFile";
    #endif
    FileName            =QFileInfo(GivenFileName).absoluteFilePath();
    ShortName           =QFileInfo(GivenFileName).fileName();
    CreatDateTime       =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime       =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    return true;
}

//====================================================================================================================

bool cFolder::IsFilteredFile(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFolder::IsFilteredFile";
    #endif
    return true;    // always valide
}

//====================================================================================================================

QString cFolder::GetFileTypeStr() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFolder::GetFileTypeStr";
    #endif
    return QApplication::translate("cBaseMediaFile","Folder","File type");
}

//*********************************************************************************************************************************************
// ffDiaporama project file
//*********************************************************************************************************************************************

cffDProjectFile::cffDProjectFile(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::cffDProjectFile";
    #endif
    AddIcons(DefaultFFDIcon);
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

bool cffDProjectFile::GetInformationFromFile(QString GivenFileName,QStringList */*AliasList*/,bool */*ModifyFlag*/) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::GetInformationFromFile";
    #endif
    FileName            =QFileInfo(GivenFileName).absoluteFilePath();
    ShortName           =QFileInfo(GivenFileName).fileName();
    FileSize            =QFileInfo(GivenFileName).size();
    FileSizeText        =GetTextSize(FileSize);
    CreatDateTime       =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime       =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    return true;
}

//====================================================================================================================

void cffDProjectFile::SaveToXML(QDomElement &domDocument) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::SaveToXML";
    #endif

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
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::LoadFromXML";
    #endif

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

#define FFD_APPLICATION_ROOTNAME    "Project"   // Name of root node in the project xml file

void cffDProjectFile::GetFullInformationFromFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::GetFullInformationFromFile";
    #endif

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
    WEBInfo=Title;
    if (Album!="")      WEBInfo=WEBInfo+(WEBInfo!=""?" - ":"")+Album;
    if (WEBInfo!="")    WEBInfo=WEBInfo+(WEBInfo!=""?" - ":"")+QString("%1").arg(Year);
    if (Author!="")     WEBInfo=WEBInfo+(WEBInfo!=""?" - ":"")+Author;
    if (Composer!="")   WEBInfo=WEBInfo+(WEBInfo!=""?" - ":"")+Composer;
    IsInformationValide=true;
}

//====================================================================================================================

bool cffDProjectFile::IsFilteredFile(int RequireObjectType) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::IsFilteredFile";
    #endif
    return (RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(RequireObjectType==OBJECTTYPE_FFDFILE);
}

//====================================================================================================================

QString cffDProjectFile::GetFileTypeStr() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::GetFileTypeStr";
    #endif
    return QApplication::translate("cBaseMediaFile","ffDiaporama","File type");
}

//*********************************************************************************************************************************************
// Image file
//*********************************************************************************************************************************************

cImageFile::cImageFile(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cImageFile::cImageFile";
    #endif
    AddIcons(DefaultIMAGEIcon);
    ObjectType  =OBJECTTYPE_IMAGEFILE;  // coul be turn later to OBJECTTYPE_THUMBNAIL
}

//====================================================================================================================

bool cImageFile::IsFilteredFile(int RequireObjectType) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cImageFile::IsFilteredFile";
    #endif
    if (ObjectType==OBJECTTYPE_IMAGEFILE) return (RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(RequireObjectType==OBJECTTYPE_IMAGEFILE);
        else return (RequireObjectType==OBJECTTYPE_UNMANAGED);
}

//====================================================================================================================

QString cImageFile::GetFileTypeStr() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cImageFile::GetFileTypeStr";
    #endif
    if (ObjectType==OBJECTTYPE_IMAGEFILE) return QApplication::translate("cBaseMediaFile","Image","File type");
        else return QApplication::translate("cBaseMediaFile","Thumbnail","File type");
}

//====================================================================================================================

void cImageFile::GetFullInformationFromFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cImageFile::GetFullInformationFromFile";
    #endif
    int ImageOrientation=-1;
    CallEXIF(ImageOrientation);
    ImageWidth =GetInformationValue("Photo.PixelXDimension").toInt();
    ImageHeight=GetInformationValue("Photo.PixelYDimension").toInt();
    WEBInfo    =GetImageSizeStr();

    QString Info;
    QString GeneralInfo;
    Info=GetInformationValue("Image.Model");            if (Info!="") GeneralInfo=Info;
    Info=GetInformationValue("Image.Orientation");      if (Info!="") GeneralInfo=GeneralInfo+" ("+Info+")";
    if (GeneralInfo!="") {
        if (WEBInfo!="") WEBInfo=WEBInfo+" - ";
        WEBInfo=WEBInfo+GeneralInfo;
    }

    QString CameraInfo;
    Info=GetInformationValue("Photo.ExposureTime");     if (Info!="") CameraInfo=CameraInfo+((CameraInfo!="")?"-":"")+Info;
    Info=GetInformationValue("Photo.ApertureValue");    if (Info=="") Info=GetInformationValue("Image.ApertureValue");    if (Info!="") CameraInfo=CameraInfo+((CameraInfo!="")?"-":"")+Info;
    Info=GetInformationValue("Photo.ISOSpeedRatings");  if (Info!="") CameraInfo=CameraInfo+"-"+Info+" ISO";

    Info=GetInformationValue("CanonCs.LensType");       if (Info!="") CameraInfo=CameraInfo+"-"+Info;           // Canon version
    Info=GetInformationValue("NikonLd3.LensIDNumber");  if (Info!="") CameraInfo=CameraInfo+"-"+Info;           // Nikon version

    Info=GetInformationValue("Photo.Flash");            if (Info!="") {
        CameraInfo=CameraInfo+((CameraInfo!="")?"-":"")+Info;
        //Info=GetInformationValue("Fujifilm.FlashMode"); if (Info!="") CameraInfo=CameraInfo+" ("+Info+")";       // Fujifilm version
        Info=GetInformationValue("CanonCs.FlashMode");  if (Info!="") CameraInfo=CameraInfo+" ("+Info+")";       // Canon version
        Info=GetInformationValue("Nikon3.FlashMode");   if (Info!="") CameraInfo=CameraInfo+" ("+Info+")";       // Nikon version
    }
    if (CameraInfo!="") WEBInfo=WEBInfo+" - "+CameraInfo;

    // Append InformationList
    if (GetInformationValue("Image.Artist")!="") InformationList.append(QString("artist")+QString("##")+GetInformationValue("Image.Artist"));
    if (GetInformationValue("Image.Model")!="")  InformationList.append(QString("composer")+QString("##")+GetInformationValue("Image.Model"));

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
    IsInformationValide=true;
}

//====================================================================================================================

bool cImageFile::CallEXIF(int &ImageOrientation) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cImageFile::CallEXIF";
    #endif
    QString   Commande;
    QString   Info,Part;
    bool      ExifOK=true;

    // start exiv2
    qDebug()<<QApplication::translate("cBaseMediaFile","Analyse file with EXIV2 :")+QFileInfo(FileName).fileName();

    #ifdef Q_OS_WIN
    Commande = "\""+ApplicationConfig->PathEXIV2+"\" print -pa \""+FileName+"\"";
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    Commande = ApplicationConfig->PathEXIV2+" print -pa \""+FileName+"\"";
    #endif
    Commande = AdjustDirForOS(Commande);
    QProcess Process;
    Process.setProcessChannelMode(QProcess::MergedChannels);
    Process.start(Commande);
    if (!Process.waitForStarted()) {
        qDebug()<<"Impossible to start exiv2 - no exif informations will be decode for"<<FileName;
        ExifOK=false;
    }
    if (ExifOK && !Process.waitForFinished()) {
        Process.kill();
        qDebug()<<"Error during exiv2 process - no exif informations will be decode for"<<FileName;
        ExifOK=false;
    }
    if (ExifOK && (Process.exitStatus()<0)) {
        qDebug()<<"Exiv2 return error"<<Process.exitStatus()<<"- no exif informations will be decode for"<<FileName;
        ExifOK=false;
    }
    if (ExifOK) {
        Info=QString().fromLocal8Bit(Process.readAllStandardOutput());

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
                Value=(Part.mid(Part.indexOf(" ")+QString(" ").length())).trimmed();
                if (Value.contains(" ")) Value=(Value.mid(Value.indexOf(" ")+QString(" ").length())).trimmed();
                if (Value.contains(" ")) Value=(Value.mid(Value.indexOf(" ")+QString(" ").length())).trimmed();
                if (Part.contains(" ")) Part=Part.left(Part.indexOf(" "));
                if (Part.startsWith("Exif.")) Part=Part.mid(QString("Exif.").length());
                InformationList.append(Part+"##"+Value);
            }
        }
        InformationList.append("Exif status##ok");
    }
    Process.terminate();
    Process.close();

    // Restart same job with -pv option to know binary value of orientation if (ImageOrientation==-1)
    if (ImageOrientation==-1) {
        #ifdef Q_OS_WIN
        Commande = "\""+ApplicationConfig->PathEXIV2+"\" print -pva \""+FileName+"\"";
        #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        Commande = ApplicationConfig->PathEXIV2+" print -pva \""+FileName+"\"";
        #endif
        Commande = AdjustDirForOS(Commande);
        Process.start(Commande);
        if (!Process.waitForStarted()) {
            qDebug()<<"Impossible to start exiv2 - no exif informations will be decode for"<<FileName;
            ExifOK=false;
        }
        if (ExifOK && !Process.waitForFinished()) {
            Process.kill();
            qDebug()<<"Error during exiv2 process - no exif informations will be decode for"<<FileName;
            ExifOK=false;
        }
        if (ExifOK && (Process.exitStatus()<0)) {
            qDebug()<<"Exiv2 return error"<<Process.exitStatus()<<"- no exif informations will be decode for"<<FileName;
            ExifOK=false;
        }
        if (ExifOK) {
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
                        ImageOrientation=Value.toInt();
                    }
                }
            }
        }
        if (ImageOrientation==-1) ImageOrientation=1;

        Process.terminate();
        Process.close();
    }
    return ExifOK;
}

//====================================================================================================================

QImage *cImageFile::ImageAt(bool PreviewMode,bool ForceLoadDisk,cFilterTransformObject *Filter) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cImageFile::ImageAt";
    #endif
    if (!IsValide) return NULL;

    bool                    Smoothing   =(!PreviewMode || ApplicationConfig->Smoothing);
    cLuLoImageCacheObject   *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileName,&BrushFileTransform,Smoothing,true);
    if (!ImageObject) {
        qDebug()<<"ImagesCache.FindObject return NULL !";
        return NULL;  // There is an error !!!!!
    }
    if (ImageObject->ImageOrientation==-1) CallEXIF(ImageObject->ImageOrientation);

    // Unfiltered image
    if (PreviewMode && ForceLoadDisk && (!Filter)) {
        QImage *UnfilteredImage=ImageObject->ValidateUnfilteredImage();
        if (UnfilteredImage && !UnfilteredImage->isNull()) {
            // Compute image geometry
            ObjectGeometry=IMAGE_GEOMETRY_UNKNOWN;
            double RatioHW=double(UnfilteredImage->width())/double(UnfilteredImage->height());
            if ((RatioHW>=1.45)&&(RatioHW<=1.55))           ObjectGeometry=IMAGE_GEOMETRY_3_2;
            else if ((RatioHW>=0.65)&&(RatioHW<=0.67))      ObjectGeometry=IMAGE_GEOMETRY_2_3;
            else if ((RatioHW>=1.32)&&(RatioHW<=1.34))      ObjectGeometry=IMAGE_GEOMETRY_4_3;
            else if ((RatioHW>=0.74)&&(RatioHW<=0.76))      ObjectGeometry=IMAGE_GEOMETRY_3_4;
            else if ((RatioHW>=1.77)&&(RatioHW<=1.79))      ObjectGeometry=IMAGE_GEOMETRY_16_9;
            else if ((RatioHW>=0.56)&&(RatioHW<=0.58))      ObjectGeometry=IMAGE_GEOMETRY_9_16;
            else if ((RatioHW>=2.34)&&(RatioHW<=2.36))      ObjectGeometry=IMAGE_GEOMETRY_40_17;
            else if ((RatioHW>=0.42)&&(RatioHW<=0.44))      ObjectGeometry=IMAGE_GEOMETRY_17_40;
            return UnfilteredImage;
        }
        // Image is not correct !
        if (UnfilteredImage && UnfilteredImage->isNull()) {
            delete UnfilteredImage;
            UnfilteredImage=NULL;
        }
        return NULL;
    }

    // Ensure ImageObject is at correct state
    if ((ForceLoadDisk)||                                                                           // If full refresh asked
        ((!PreviewMode)&&(ImageObject->CacheRenderImage==NULL)&&(ImageObject->CachePreviewImage!=NULL))||    // if not preview mode and CachePreviewImage not null but CacheRenderImage is null
        ((PreviewMode)&&(ImageObject->CachePreviewImage==NULL)&&(ImageObject->CacheRenderImage!=NULL)))      // if preview mode and CacheRenderImage not null but CachePreviewImage is null       ??????????????
            ImageObject->ClearAll();

    if (PreviewMode) {

        // Stop here if we have wanted image
        if (ImageObject->CachePreviewImage) return new QImage(ImageObject->CachePreviewImage->copy());

        QImage *CachePreviewImage=ImageObject->ValidateCachePreviewImage(Filter);

        // Get preview image size
        ImageHeight=CachePreviewImage->height();
        ImageWidth =CachePreviewImage->width();

        // Compute image geometry
        ObjectGeometry=IMAGE_GEOMETRY_UNKNOWN;
        double RatioHW=double(CachePreviewImage->width())/double(CachePreviewImage->height());
        if ((RatioHW>=1.45)&&(RatioHW<=1.55))           ObjectGeometry=IMAGE_GEOMETRY_3_2;
        else if ((RatioHW>=0.65)&&(RatioHW<=0.67))      ObjectGeometry=IMAGE_GEOMETRY_2_3;
        else if ((RatioHW>=1.32)&&(RatioHW<=1.34))      ObjectGeometry=IMAGE_GEOMETRY_4_3;
        else if ((RatioHW>=0.74)&&(RatioHW<=0.76))      ObjectGeometry=IMAGE_GEOMETRY_3_4;
        else if ((RatioHW>=1.77)&&(RatioHW<=1.79))      ObjectGeometry=IMAGE_GEOMETRY_16_9;
        else if ((RatioHW>=0.56)&&(RatioHW<=0.58))      ObjectGeometry=IMAGE_GEOMETRY_9_16;
        else if ((RatioHW>=2.34)&&(RatioHW<=2.36))      ObjectGeometry=IMAGE_GEOMETRY_40_17;
        else if ((RatioHW>=0.42)&&(RatioHW<=0.44))      ObjectGeometry=IMAGE_GEOMETRY_17_40;

    } else {

        // Stop here if we have wanted image
        if (ImageObject->CacheRenderImage) return new QImage(ImageObject->CacheRenderImage->copy());

        // Ensure CacheRenderImage is valide
        QImage *CacheRenderImage=ImageObject->ValidateCacheRenderImage((Filter && !PreviewMode)?Filter:NULL);
        if (CacheRenderImage && !CacheRenderImage->isNull()) {
            // Compute image geometry
            ObjectGeometry=IMAGE_GEOMETRY_UNKNOWN;
            double RatioHW=double(CacheRenderImage->width())/double(CacheRenderImage->height());
            if ((RatioHW>=1.45)&&(RatioHW<=1.55))           ObjectGeometry=IMAGE_GEOMETRY_3_2;
            else if ((RatioHW>=0.65)&&(RatioHW<=0.67))      ObjectGeometry=IMAGE_GEOMETRY_2_3;
            else if ((RatioHW>=1.32)&&(RatioHW<=1.34))      ObjectGeometry=IMAGE_GEOMETRY_4_3;
            else if ((RatioHW>=0.74)&&(RatioHW<=0.76))      ObjectGeometry=IMAGE_GEOMETRY_3_4;
            else if ((RatioHW>=1.77)&&(RatioHW<=1.79))      ObjectGeometry=IMAGE_GEOMETRY_16_9;
            else if ((RatioHW>=0.56)&&(RatioHW<=0.58))      ObjectGeometry=IMAGE_GEOMETRY_9_16;
            else if ((RatioHW>=2.34)&&(RatioHW<=2.36))      ObjectGeometry=IMAGE_GEOMETRY_40_17;
            else if ((RatioHW>=0.42)&&(RatioHW<=0.44))      ObjectGeometry=IMAGE_GEOMETRY_17_40;
        }

    }
    // return wanted image
    if ((PreviewMode)&&(ImageObject->CachePreviewImage)) return new QImage(ImageObject->CachePreviewImage->copy());
    if ((!PreviewMode)&&(ImageObject->CacheRenderImage)) return new QImage(ImageObject->CacheRenderImage->copy());
    return NULL; // if image is not valide
}

/*************************************************************************************************************************************
    CLASS cVideoFile
*************************************************************************************************************************************/

cVideoFile::cVideoFile(WantedObjectTypeFmt TheWantedObjectType,cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::cVideoFile";
    #endif

    WantedObjectType        = TheWantedObjectType;
    MusicOnly               = (WantedObjectType==MUSICFILE);
    ObjectType              = OBJECTTYPE_MUSICORVIDEO;                  // Not yet determined
    IsOpen                  = false;
    StartPos                = QTime(0,0,0,0);   // Start position
    EndPos                  = QTime(0,0,0,0);   // End position

    // Video part
    ffmpegVideoFile         = NULL;
    VideoDecoderCodec       = NULL;
    VideoStreamNumber       = 0;
    FrameBufferYUV          = NULL;
    FrameBufferYUVReady     = false;
    FrameBufferYUVPosition  = 0;
    CacheFirstImage         = NULL;                     // Cache image of first image of the video
    CacheLastImage          = NULL;                     // Cache image of last image of the video (Preview mode only)
    dEndFileCachePos        = 0;                        // Position of the cache image of last image of the video
    VideoCodecInfo          = "";
    VideoTrackNbr           = 0;
    VideoStreamNumber       =-1;

    // Audio part
    ffmpegAudioFile         = NULL;
    AudioDecoderCodec       = NULL;
    LastAudioReadedPosition = -1;
    IsVorbis                = false;
    AudioCodecInfo          = "";
    AudioTrackNbr           = 0;
    AudioStreamNumber       =-1;
}

//====================================================================================================================

cVideoFile::~cVideoFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::~cVideoFile";
    #endif
    if (CacheFirstImage!=NULL) {
        delete CacheFirstImage;
        CacheFirstImage=NULL;
    }
    if (CacheLastImage!=NULL) {
        delete CacheLastImage;
        CacheLastImage=NULL;
    }

    // Close LibAVFormat and LibAVCodec contexte for the file
    CloseCodecAndFile();
}

//====================================================================================================================

// Overloaded function use to dertermine if media file correspond to WantedObjectType
//      WantedObjectType could be OBJECTTYPE_VIDEOFILE or OBJECTTYPE_MUSICFILE
//      if AudioOnly was set to true in constructor then ignore all video track and set WantedObjectType to OBJECTTYPE_MUSICFILE else set it to OBJECTTYPE_VIDEOFILE
//      return true if WantedObjectType=OBJECTTYPE_VIDEOFILE and at least one video track is present
//      return true if WantedObjectType=OBJECTTYPE_MUSICFILE and at least one audio track is present

bool cVideoFile::GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::RealGetInformationFromFile";
    #endif
    if (!cBaseMediaFile::GetInformationFromFile(GivenFileName,AliasList,ModifyFlag)) return false;

    AVFormatContext *ffmpegFile=NULL;;

    // if file exist then Open video file and get a LibAVFormat context and an associated LibAVCodec decoder
    #if (LIBAVFORMAT_VERSION_MAJOR>=53)
        if (avformat_open_input(&ffmpegFile,FileName.toLocal8Bit(),NULL,NULL)!=0) return false;
    #else
        if (av_open_input_file(&ffmpegFile,FileName.toLocal8Bit(),NULL,0,NULL)!=0) return false;
    #endif

    ffmpegFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future frames.

    #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=28))
        if (avformat_find_stream_info(ffmpegFile,NULL)<0) {
            avformat_close_input(&ffmpegFile);
    #else
        if (av_find_stream_info(ffmpegFile)<0) {    // deprecated : use avformat_find_stream_info instead
            av_close_input_file(ffmpegFile);
    #endif

        return false;
    }

    // Get metadata
    #if (LIBAVFORMAT_VERSION_MAJOR<53)
    AVMetadataTag *tag=NULL;
    while ((tag=av_metadata_get(ffmpegFile->metadata,"",tag,AV_METADATA_IGNORE_SUFFIX))) {
    #else
    AVDictionaryEntry *tag=NULL;
    while ((tag=av_dict_get(ffmpegFile->metadata,"",tag,AV_DICT_IGNORE_SUFFIX))) {
    #endif
        QString Value=QString().fromUtf8(tag->value);
        #if defined(Q_OS_WIN)
        Value.replace(char(13),"\n");
        #endif
        if (Value.endsWith("\n")) Value=Value.left(Value.lastIndexOf("\n"));
        InformationList.append(QString().fromUtf8(tag->key).toLower()+QString("##")+Value);
    }

    // Get chapters
    NbrChapters=ffmpegFile->nb_chapters;
    for (uint i=0;i<ffmpegFile->nb_chapters;i++) {
        AVChapter   *ch=ffmpegFile->chapters[i];
        QString     ChapterNum=QString("%1").arg(i);
        while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
        qlonglong Start=double(ch->start)*(double(av_q2d(ch->time_base))*1000);    // Lib AV use 1/1 000 000 000 sec and we want msec !
        qlonglong End  =double(ch->end)*(double(av_q2d(ch->time_base))*1000);    // Lib AV use 1/1 000 000 000 sec and we want msec !
        InformationList.append("Chapter_"+ChapterNum+":Start"   +QString("##")+QTime(0,0,0,0).addMSecs(Start).toString("hh:mm:ss.zzz"));
        InformationList.append("Chapter_"+ChapterNum+":End"     +QString("##")+QTime(0,0,0,0).addMSecs(End).toString("hh:mm:ss.zzz"));
        InformationList.append("Chapter_"+ChapterNum+":Duration"+QString("##")+QTime(0,0,0,0).addMSecs(End-Start).toString("hh:mm:ss.zzz"));
            // Chapter metadata
            #if (LIBAVFORMAT_VERSION_MAJOR<53)
            while ((tag=av_metadata_get(ch->metadata,"",tag,AV_METADATA_IGNORE_SUFFIX)))
            #else
            while ((tag=av_dict_get(ch->metadata,"",tag,AV_DICT_IGNORE_SUFFIX)))
            #endif
                InformationList.append("Chapter_"+ChapterNum+":"+QString().fromUtf8(tag->key).toLower()+QString("##")+QString().fromUtf8(tag->value));
    }
    // Get informations about duration
    int hh,mm,ss,ms;
    ms=ffmpegFile->duration/1000;
    ss=ms/1000;
    mm=ss/60;
    hh=mm/60;
    mm=mm-(hh*60);
    ss=ss-(ss/60)*60;
    ms=ms-(ms/1000)*1000;
    Duration=QTime(hh,mm,ss,ms);
    EndPos  =Duration;    // By default : EndPos is set to the end of file
    InformationList.append(QString("Duration")+QString("##")+Duration.toString("HH:mm:ss.zzz"));

    // Get information from track
    for (int Track=0;Track<(int)ffmpegFile->nb_streams;Track++) {

        // Find codec
        AVCodec *Codec=avcodec_find_decoder(ffmpegFile->streams[Track]->codec->codec_id);

        if (ffmpegFile->streams[Track]->codec->codec_type==AVMEDIA_TYPE_AUDIO) {
            // Keep this as default track
            if (AudioStreamNumber==-1) AudioStreamNumber=Track;

            // Compute TrackNum
            QString TrackNum=QString("%1").arg(AudioTrackNbr);
            while (TrackNum.length()<3) TrackNum="0"+TrackNum;
            TrackNum="Audio_"+TrackNum+":";

            // General
            InformationList.append(TrackNum+QString("Track")+QString("##")+QString("%1").arg(Track));
            if (Codec) InformationList.append(TrackNum+QString("Codec")+QString("##")+QString(Codec->name));

            // Channels
            if (ffmpegFile->streams[Track]->codec->channels==1)        InformationList.append(TrackNum+QString("Channels")+QString("##")+QApplication::translate("cBaseMediaFile","Mono","Audio channels mode"));
            else if (ffmpegFile->streams[Track]->codec->channels==2)   InformationList.append(TrackNum+QString("Channels")+QString("##")+QApplication::translate("cBaseMediaFile","Stereo","Audio channels mode"));
            else                                                            InformationList.append(TrackNum+QString("Channels")+QString("##")+QString("%1").arg(ffmpegFile->streams[Track]->codec->channels));

            // Frequency
            if (int(ffmpegFile->streams[Track]->codec->sample_rate/1000)*1000>0) {
                if (int(ffmpegFile->streams[Track]->codec->sample_rate/1000)*1000==ffmpegFile->streams[Track]->codec->sample_rate)
                     InformationList.append(TrackNum+QString("Frequency")+QString("##")+QString("%1").arg(int(ffmpegFile->streams[Track]->codec->sample_rate/1000))+"Khz");
                else InformationList.append(TrackNum+QString("Frequency")+QString("##")+QString("%1").arg(double(ffmpegFile->streams[Track]->codec->sample_rate)/1000,8,'f',1).trimmed()+"Khz");
            }

            // Bitrate
            if (int(ffmpegFile->streams[Track]->codec->bit_rate/1000)>0) InformationList.append(TrackNum+QString("Bitrate")+QString("##")+QString("%1").arg(int(ffmpegFile->streams[Track]->codec->bit_rate/1000))+"Kb/s");

            // Sample format
            switch (ffmpegFile->streams[Track]->codec->sample_fmt) {
                case AV_SAMPLE_FMT_U8: 	 InformationList.append(TrackNum+QString("Sample format")+QString("##")+"unsigned 8 bits"); break;
                case AV_SAMPLE_FMT_S16:  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"signed 16 bits"); break;
                case AV_SAMPLE_FMT_S32:  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"signed 32 bits"); break;
                case AV_SAMPLE_FMT_FLT:  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"float"); break;
                case AV_SAMPLE_FMT_DBL:  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"double"); break;
                #ifdef AV_SAMPLE_FMT_U8P
                case AV_SAMPLE_FMT_U8P:  InformationList.append(TrackNum+QString("Sample format")+QString("##")+"unsigned 8 bits, planar"); break;
                case AV_SAMPLE_FMT_S16P: InformationList.append(TrackNum+QString("Sample format")+QString("##")+"signed 16 bits, planar"); break;
                case AV_SAMPLE_FMT_S32P: InformationList.append(TrackNum+QString("Sample format")+QString("##")+"signed 32 bits, planar"); break;
                case AV_SAMPLE_FMT_FLTP: InformationList.append(TrackNum+QString("Sample format")+QString("##")+"float, planar"); break;
                case AV_SAMPLE_FMT_DBLP: InformationList.append(TrackNum+QString("Sample format")+QString("##")+"double, planar"); break;
                #endif
                default:                 InformationList.append(TrackNum+QString("Sample format")+QString("##")+"Unknown"); break;
            }

            // Stream metadata
            #if (LIBAVFORMAT_VERSION_MAJOR<53)
            while ((tag=av_metadata_get(ffmpegFile->streams[Track]->metadata,"",tag,AV_METADATA_IGNORE_SUFFIX))) {
            #else
            while ((tag=av_dict_get(ffmpegFile->streams[Track]->metadata,"",tag,AV_DICT_IGNORE_SUFFIX))) {
            #endif
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

        } else if (!MusicOnly && (ffmpegFile->streams[Track]->codec->codec_type==AVMEDIA_TYPE_VIDEO)) {

            // Keep this as default track
            if (VideoStreamNumber==-1) VideoStreamNumber=Track;

            // Compute TrackNum
            QString TrackNum=QString("%1").arg(VideoTrackNbr);
            while (TrackNum.length()<3) TrackNum="0"+TrackNum;
            TrackNum="Video_"+TrackNum+":";

            // General
            InformationList.append(TrackNum+QString("Track")+QString("##")+QString("%1").arg(Track));
            if (Codec) InformationList.append(TrackNum+QString("Codec")+QString("##")+QString(Codec->name));

            // Bitrate
            if (ffmpegFile->streams[Track]->codec->bit_rate>0) InformationList.append(TrackNum+QString("Bitrate")+QString("##")+QString("%1").arg(int(ffmpegFile->streams[Track]->codec->bit_rate/1000))+"Kb/s");

            // Frame rate
            if (int(double(ffmpegFile->streams[Track]->avg_frame_rate.num)/double(ffmpegFile->streams[Track]->avg_frame_rate.den))>0) {
                if (int(double(ffmpegFile->streams[Track]->avg_frame_rate.num)/double(ffmpegFile->streams[Track]->avg_frame_rate.den))==double(ffmpegFile->streams[Track]->avg_frame_rate.num)/double(ffmpegFile->streams[Track]->avg_frame_rate.den))
                     InformationList.append(TrackNum+QString("Frame rate")+QString("##")+QString("%1").arg(int(double(ffmpegFile->streams[Track]->avg_frame_rate.num)/double(ffmpegFile->streams[Track]->avg_frame_rate.den)))+" FPS");
                else InformationList.append(TrackNum+QString("Frame rate")+QString("##")+QString("%1").arg(double(double(ffmpegFile->streams[Track]->avg_frame_rate.num)/double(ffmpegFile->streams[Track]->avg_frame_rate.den)),8,'f',3).trimmed()+" FPS");
            }

            // Stream metadata
            #if (LIBAVFORMAT_VERSION_MAJOR<53)
            while ((tag=av_metadata_get(ffmpegFile->streams[Track]->metadata,"",tag,AV_METADATA_IGNORE_SUFFIX)))
            #else
            while ((tag=av_dict_get(ffmpegFile->streams[Track]->metadata,"",tag,AV_DICT_IGNORE_SUFFIX)))
            #endif
                InformationList.append(TrackNum+QString(tag->key)+QString("##")+QString().fromUtf8(tag->value));

            // Ensure language exist (Note : AVI ‘AttachedPictureFrame’and FLV container own language at container level instead of track level)
            if (GetInformationValue(TrackNum+"language")=="") {
                QString Lng=GetInformationValue("language");
                InformationList.append(TrackNum+QString("language##")+(Lng==""?"und":Lng));
            }

            // Next
            VideoTrackNbr++;
        }
    }
    // Close file
    #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=28))
        avformat_close_input(&ffmpegFile);
    #else
        av_close_input_file(ffmpegFile);
    #endif

    // Do file qualification

    if (VideoTrackNbr>0)      ObjectType=OBJECTTYPE_VIDEOFILE;
    else if (AudioTrackNbr>0) ObjectType=OBJECTTYPE_MUSICFILE;
    else                      ObjectType=OBJECTTYPE_MUSICORVIDEO;                    // Error !

    // Load correct icon depending on type
    if (Icon16.isNull()) {
        if (ObjectType==OBJECTTYPE_MUSICFILE) AddIcons(DefaultMUSICIcon);
            else                              AddIcons(DefaultVIDEOIcon);
    }
    // Return value depending on type ask
    IsValide=((WantedObjectType==MUSICORVIDEO)&&(VideoTrackNbr+AudioTrackNbr>0))||
            ((WantedObjectType==VIDEOFILE)&&(VideoTrackNbr>0))||
            ((WantedObjectType==MUSICFILE)&&(AudioTrackNbr>0));
    return IsValide;
}

//====================================================================================================================

bool cVideoFile::IsFilteredFile(int RequireObjectType) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::IsFilteredFile";
    #endif
    return (RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(ObjectType==RequireObjectType);
}

//====================================================================================================================

QString cVideoFile::GetFileTypeStr() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::GetFileTypeStr";
    #endif
    if (ObjectType==OBJECTTYPE_VIDEOFILE)   return QApplication::translate("cBaseMediaFile","Video","File type");
        else                                return QApplication::translate("cBaseMediaFile","Music","File type");
}

//====================================================================================================================

void cVideoFile::GetFullInformationFromFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::GetFullInformationFromFile";
    #endif

    QImage *Img=GetEmbededImage(FileName);
    if (Img) {
        Icon16=QImage();
        Icon32=QImage();
        Icon48=QImage();
        AddIcons(Img);
        delete Img;
    }

    // If it's a video then search if an image (jpg) with same name exist
    if (ObjectType==OBJECTTYPE_VIDEOFILE) {
        // Reset Icons to ensure it will be reloaded
        Icon16=QImage();
        Icon32=QImage();
        Icon48=QImage();

        // Search if a jukebox mode thumbnail exist
        QFileInfo   File(FileName);
        QString     JPegFile=File.absolutePath()+(File.absolutePath().endsWith(QDir::separator())?"":QString(QDir::separator()))+File.completeBaseName()+".jpg";
        if (QFileInfo(JPegFile).exists()) AddIcons(JPegFile);
    }

    OpenCodecAndFile();
    WEBInfo=GetImageSizeStr();
    CloseCodecAndFile();
    IsInformationValide=true;
}

//====================================================================================================================
// Close LibAVFormat and LibAVCodec contexte for the file
//====================================================================================================================

void cVideoFile::CloseCodecAndFile() {
#ifdef DEBUGMODE
qDebug() << "IN:cVideoFile::CloseCodecAndFile";
#endif
    // Close the video codec
    if (VideoDecoderCodec!=NULL) {
        avcodec_close(ffmpegVideoFile->streams[VideoStreamNumber]->codec);
        VideoDecoderCodec=NULL;
    }

    // Close the video file
    if (ffmpegVideoFile!=NULL) {
        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=28))
            avformat_close_input(&ffmpegVideoFile);
        #else
            av_close_input_file(ffmpegVideoFile);
        #endif
        ffmpegVideoFile=NULL;
    }

    // Close the audio codec
    if (AudioDecoderCodec!=NULL) {
        avcodec_close(ffmpegAudioFile->streams[AudioStreamNumber]->codec);
        AudioDecoderCodec=NULL;
    }
    // Close the audio file
    if (ffmpegAudioFile!=NULL) {
        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=28))
            avformat_close_input(&ffmpegAudioFile);
        #else
            av_close_input_file(ffmpegAudioFile);
        #endif
        ffmpegAudioFile=NULL;
    }

    if (FrameBufferYUV!=NULL) {
        av_free(FrameBufferYUV);
        FrameBufferYUV=NULL;
    }
    IsOpen=false;
}

//====================================================================================================================
// Read an audio frame from current stream
//====================================================================================================================
void cVideoFile::ReadAudioFrame(bool PreviewMode,qlonglong Position,cSoundBlockList *SoundTrackBloc,double Volume,bool DontUseEndPos) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::ReadAudioFrame";
    #endif
    // Ensure file was previously open and all is ok
    if ((SoundTrackBloc==NULL)||(AudioStreamNumber==-1)||(ffmpegAudioFile->streams[AudioStreamNumber]==NULL)||(ffmpegAudioFile==NULL)||(AudioDecoderCodec==NULL)) return;

    // Ensure Position is not > EndPosition
    if (Position>QTime(0,0,0,0).msecsTo(DontUseEndPos?Duration:EndPos)) return;

    int64_t         AVNOPTSVALUE        =INT64_C(0x8000000000000000); // to solve type error with Qt
    AVStream        *AudioStream        =ffmpegAudioFile->streams[AudioStreamNumber];
    int64_t         SrcSampleSize       =2*int64_t(AudioStream->codec->channels);

    int64_t         DstSampleSize       =(SoundTrackBloc->SampleBytes*SoundTrackBloc->Channels);
    AVPacket        *StreamPacket       =NULL;
    uint8_t         *BufferToDecode     =NULL;
    uint8_t         *BufferForDecoded   =NULL;
    int64_t         MaxAudioLenDecoded  =AVCODEC_MAX_AUDIO_FRAME_SIZE*10;
    int64_t         AudioLenDecoded     =0;
    double          dPosition           =double(Position)/1000;     // Position in double format
    double          EndPosition         =dPosition+SoundTrackBloc->WantedDuration;
    double          AudioDataWanted     =SoundTrackBloc->WantedDuration*double(AudioStream->codec->sample_rate)*SrcSampleSize;

    bool            Continue        =true;
    double          FramePosition   =dPosition;
    double          FrameDuration   =0;

    // Cac difftime between asked position and previous end decoded position
    int DiffTimePosition=LastAudioReadedPosition-Position;
    if (DiffTimePosition<0) DiffTimePosition=-DiffTimePosition;

    // Adjust position if input file have a start_time value
    if (ffmpegAudioFile->start_time!=AVNOPTSVALUE)  dPosition+=double(ffmpegAudioFile->start_time)/double(AV_TIME_BASE);

    // Prepare a buffer for sound decoding
    BufferToDecode      =(uint8_t *)av_malloc(48000*4*2);   // 2 sec buffer
    BufferForDecoded    =(uint8_t *)av_malloc(MaxAudioLenDecoded);

    // Calc if we need to seek to a position
    if ((Position==0)||(DiffTimePosition>2)) {// Allow 2 msec diff (rounded double !)

        // Flush all buffers
        for (unsigned int i=0;i<ffmpegAudioFile->nb_streams;i++)  {
            AVCodecContext *codec_context = ffmpegAudioFile->streams[i]->codec;
            if (codec_context && codec_context->codec) avcodec_flush_buffers(codec_context);
        }
        SoundTrackBloc->ClearList();      // Clear soundtrack list

        // Seek to nearest previous key frame
        int64_t seek_target=av_rescale_q(int64_t((dPosition/1000)*AV_TIME_BASE),AV_TIME_BASE_Q,ffmpegAudioFile->streams[AudioStreamNumber]->time_base);
        if (av_seek_frame(ffmpegAudioFile,AudioStreamNumber,seek_target,AVSEEK_FLAG_BACKWARD)<0) {
            qDebug()<<"Seek error";
        }
        FramePosition=-1;

    }

    //*************************************************************************************************************************************
    // Decoding process : Get StreamPacket until endposition is reach (if sound is wanted) or until image is ok (if image only is wanted)
    //*************************************************************************************************************************************

    while (Continue) {
        StreamPacket=new AVPacket();
        av_init_packet(StreamPacket);
        //StreamPacket->flags|=AV_PKT_FLAG_KEY;  // HACK for CorePNG to decode as normal PNG by default

        if (av_read_frame(ffmpegAudioFile,StreamPacket)==0) {
            if ((StreamPacket->stream_index==AudioStreamNumber)&&(StreamPacket->size>0)) {

                AVPacket PacketTemp;
                av_init_packet(&PacketTemp);
                PacketTemp.data=StreamPacket->data;
                PacketTemp.size=StreamPacket->size;

                if (StreamPacket->pts!=AVNOPTSVALUE) FramePosition=double(StreamPacket->pts)*double(av_q2d(AudioStream->time_base));

                // NOTE: the audio packet can contain several frames
                if (FramePosition!=-1) while (PacketTemp.size>0) {

                    // Decode audio data
                    int SizeDecoded=(AVCODEC_MAX_AUDIO_FRAME_SIZE*3)/2;
                    #if defined (FF_API_OLD_DECODE_AUDIO)
                        int Len=avcodec_decode_audio3(AudioStream->codec,(int16_t *)BufferToDecode,&SizeDecoded,&PacketTemp);
                    #else
                        int Len=avcodec_decode_audio3(AudioStream->codec,(int16_t *)BufferToDecode,&SizeDecoded,&PacketTemp);
                    #endif
                    if (Len<0) {
                        // if decode error then data are not good : replace them with null sound
                        //SizeDecoded=int64_t(LastAudioFrameDuration*double(SoundTrackBloc->SamplingRate))*DstSampleSize;
                        //memset(BufferForDecoded+AudioLenDecoded,0,SizeDecoded);
                        //AudioLenDecoded+=SizeDecoded;
                        //qDebug()<<"    =>Make NULL Audio frame"<<SizeDecoded<<"bytes added - Buffer:"<<AudioLenDecoded<<"/"<<MaxAudioLenDecoded;
                        // if error, we skip the frame and exit the while loop
                        PacketTemp.size=0;
                    } else if (SizeDecoded>0) {
                        FrameDuration=double(SizeDecoded)/(double(SrcSampleSize)*double(AudioStream->codec->sample_rate));
                        // If wanted position <= CurrentPosition+Packet duration then add this packet to the queue
                        if ((FramePosition+FrameDuration)>=dPosition) {

                            int64_t Delta=0;
                            // if dPosition start in the midle of the pack, then calculate delta
                            if (dPosition>FramePosition) {
                                Delta=int64_t(double(dPosition-FramePosition)*double(AudioStream->codec->sample_rate));
                                Delta*=SrcSampleSize;
                            }
                            // Append decoded data to BufferForDecoded buffer
                            memcpy(BufferForDecoded+AudioLenDecoded,BufferToDecode+Delta,SizeDecoded-Delta);
                            AudioLenDecoded+=(SizeDecoded-Delta);

                        } else {
                            //qDebug()<<"Skip packet :"<<FramePosition<<"-"<<FrameEndPosition<<"["<<dPosition<<"-"<<EndPosition<<"]";
                        }
                        PacketTemp.data+=Len;
                        PacketTemp.size-=Len;
                        FramePosition=FramePosition+FrameDuration;
                    }
                }
            }

            // Continue with a new one
            av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
            delete StreamPacket;
            StreamPacket=NULL;

            // Check if we need to continue loop
            Continue=(AudioLenDecoded<AudioDataWanted);
        } else {
            // if error in av_read_frame(...) then may be we have reach the end of file !
            Continue=false;
        }
    }
    // Keep NextPacketPosition for determine next time if we need to seek
    LastAudioReadedPosition=int(EndPosition*1000);

    //**********************************************************************
    // Transfert data from BufferForDecoded to Buffer using audio_resample
    //**********************************************************************
    if (AudioLenDecoded>0) {

        // Adjust volume
        if (Volume!=1) {
            int16_t *Buf1=(int16_t*)BufferForDecoded;
            int32_t mix;
            for (int j=0;j<AudioLenDecoded/(SoundTrackBloc->SampleBytes*SoundTrackBloc->Channels);j++) {
                // Left channel : Adjust if necessary (16 bits)
                mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
                // Right channel : Adjust if necessary (16 bits)
                mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
            }
        }

        bool Continue=true;
        if (AudioStream->codec->channels==1) {
            // Mono to stereo
            int16_t *NewBuf=(short int*)av_malloc(AudioLenDecoded*DstSampleSize+DstSampleSize);
            int16_t *Pa=NewBuf;
            int16_t *Pb=(int16_t*)BufferForDecoded;

            for (int i=0;i<AudioLenDecoded/2;i++) {
                *(Pa++)=*Pb;
                *(Pa++)=*(Pb++);
            }
            // Switch the 2 buffers
            av_free(BufferForDecoded);
            BufferForDecoded=(uint8_t*)NewBuf;
            AudioLenDecoded=AudioLenDecoded*2;

        } else if (AudioStream->codec->channels!=2) {
            // 5.1 to stereo ?????
            Continue=false;
        }

        if (Continue) {

            if (SoundTrackBloc->SamplingRate!=AudioStream->codec->sample_rate) {

                double  NewSize=(double(AudioLenDecoded/DstSampleSize)/double(AudioStream->codec->sample_rate))*double(SoundTrackBloc->SamplingRate);
                double  PasSrc =1/double(AudioStream->codec->sample_rate);
                double  PasDst =1/double(SoundTrackBloc->SamplingRate);
                double  PosSrc =0;
                double  PosDst =0;
                int16_t *NewBuf=(short int*)av_malloc(NewSize*DstSampleSize+DstSampleSize);
                int16_t *PtrSrc=(int16_t*)BufferForDecoded;
                int16_t *PtrDst=NewBuf;
                int     RealNewSize=0;

                if (PreviewMode && ApplicationConfig->QuickResamplingPreview) {
                    // For Preview Mode use a quick resampling linear method without interpolation
                    int16_t Left,Right;

                    for (int i=0;i<AudioLenDecoded/DstSampleSize;i++) {
                        Left=*(PtrSrc++);
                        Right=*(PtrSrc++);
                        *(PtrDst++)=Left;           // First Chanel
                        *(PtrDst++)=Right;          // Second Chanel
                        PosSrc=PosSrc+PasSrc;
                        PosDst=PosDst+PasDst;
                        RealNewSize++;
                        if ((PosSrc-PosDst)>=PasDst) {
                            *(PtrDst++)=Left;           // First Chanel
                            *(PtrDst++)=Right;          // Second Chanel
                            PosDst=PosDst+PasDst;
                            RealNewSize++;
                        }
                    }

                } else {
                    // For Rendering Mode use a resampling linear method with Hermit interpolation (http://en.wikipedia.org/wiki/Hermite_interpolation)
                    int16_t Left_x0,Left_x1,Left_x2,Left_x3;
                    int16_t Right_x0,Right_x1,Right_x2,Right_x3;
                    float   t,c0,c1,c2,c3,Value;

                    // First value
                    Left_x0=*(PtrSrc++);    *(PtrDst++)=Left_x0;
                    Right_x0=*(PtrSrc++);   *(PtrDst++)=Right_x0;
                    PosSrc=PosSrc+PasSrc;
                    PosDst=PosDst+PasDst;
                    RealNewSize++;

                    // Second value
                    Left_x1=*(PtrSrc++);    *(PtrDst++)=Left_x1;
                    Right_x1=*(PtrSrc++);   *(PtrDst++)=Right_x1;
                    //PosSrc=PosSrc+PasSrc;
                    PosDst=PosDst+PasDst;
                    RealNewSize++;

                    // Third value
                    Left_x2=*(PtrSrc++);
                    Right_x2=*(PtrSrc++);

                    // Neutralize first swap
                    Left_x3 =Left_x2;       Left_x2 =Left_x1;       Left_x1 =Left_x0;
                    Right_x3=Right_x2;      Right_x2=Right_x1;      Right_x1=Right_x0;

                    for (int i=2;i<(NewSize-2);i++) {

                        if ((PosDst-PosSrc)>=PasSrc) {
                            // swap all values
                            Left_x0 =Left_x1;       Left_x1 =Left_x2;       Left_x2 =Left_x3;
                            Right_x0=Right_x1;      Right_x1=Right_x2;      Right_x2=Right_x3;

                            // Get fourth value
                            Left_x3=*(PtrSrc++);
                            Right_x3=*(PtrSrc++);
                            PosSrc=PosSrc+PasSrc;
                        }

                        // Calculate distance between PosSrc and next point
                        t=(PosDst-PosSrc)/PasSrc;

                        // Calculate interpolation using Hermite method
                        // Left Chanel
                        c0   =Left_x1;
                        c1   =.5F * (Left_x2 - Left_x0);
                        c2   =Left_x0 - (2.5F * Left_x1) + (2 * Left_x2) - (.5F * Left_x3);
                        c3   =(.5F * (Left_x3 - Left_x0)) + (1.5F * (Left_x1 - Left_x2));
                        Value=(((((c3 * t) + c2) * t) + c1) * t) + c0;
                        *(PtrDst++)=int16_t(Value);
                        // Right Chanel
                        c0   =Right_x1;
                        c1   =.5F * (Right_x2 - Right_x0);
                        c2   =Right_x0 - (2.5F * Right_x1) + (2 * Right_x2) - (.5F * Right_x3);
                        c3   =(.5F * (Right_x3 - Right_x0)) + (1.5F * (Right_x1 - Right_x2));
                        Value=(((((c3 * t) + c2) * t) + c1) * t) + c0;
                        *(PtrDst++)=int16_t(Value);
                        PosDst=PosDst+PasDst;
                        RealNewSize++;
                    }
                    // Last 2 values
                    *(PtrDst++)=Left_x2;
                    *(PtrDst++)=Right_x2;
                    RealNewSize++;
                    *(PtrDst++)=Left_x3;
                    *(PtrDst++)=Right_x3;
                    RealNewSize++;

                }

                // Append data to SoundTrackBloc
                SoundTrackBloc->AppendData((int16_t*)NewBuf,trunc(RealNewSize)*DstSampleSize);
                av_free(NewBuf);                                                    // Free allocated buffers

                /* OLD VERSION USING FFMPEG FUNCTION
                // Resample sound to wanted freq. using ffmpeg audio_resample function
                ReSampleContext *RSC=av_audio_resample_init(                        // Context for resampling audio data
                    SoundTrackBloc->Channels,2,                                     // output_channels, input_channels
                    SoundTrackBloc->SamplingRate,AudioStream->codec->sample_rate,   // output_rate, input_rate
                    SAMPLE_FMT_S16,AudioStream->codec->sample_fmt,                  // sample_fmt_out, sample_fmt_in
                    16,                                                             // filter_length
                    10,                                                             // log2_phase_count
                    0,                                                              // linear
                    0.8);                                                           // cutoff
                if (RSC!=NULL) {
                    short int*BufSampled=(short int*)av_malloc((AVCODEC_MAX_AUDIO_FRAME_SIZE*3)/2);
                    int64_t SizeSampled=audio_resample(RSC,BufSampled,(short int*)BufferForDecoded,AudioLenDecoded/SrcSampleSize)*DstSampleSize;
                    SoundTrackBloc->AppendData((int16_t*)BufSampled,SizeSampled);   // Append data to SoundTrackBloc
                    audio_resample_close(RSC);                                      // Close the resampling audio context
                    av_free(BufSampled);                                            // Free allocated buffers
                }
                */

            } else {
                // Append data to SoundTrackBloc
                SoundTrackBloc->AppendData((int16_t*)BufferForDecoded,AudioLenDecoded);
            }

        }
    }

    // Now ensure SoundTrackBloc have correct wanted packet (if no then add nullsound)
    while (SoundTrackBloc->List.count()<SoundTrackBloc->NbrPacketForFPS) SoundTrackBloc->AppendNullSoundPacket();

    if (BufferToDecode)   av_free(BufferToDecode);
    if (BufferForDecoded) av_free(BufferForDecoded);
}

//====================================================================================================================
// Read a video frame from current stream
//====================================================================================================================

#define MAXELEMENTSINOBJECTLIST 500

QImage *cVideoFile::ReadVideoFrame(qlonglong Position,bool DontUseEndPos) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::ReadVideoFrame";
    #endif
    int64_t AVNOPTSVALUE=INT64_C(0x8000000000000000); // to solve type error with Qt

    // Ensure file was previously open
    if ((ffmpegVideoFile==NULL)||(VideoDecoderCodec==NULL)) return NULL;

    double dEndFile =double(QTime(0,0,0,0).msecsTo(DontUseEndPos?Duration:EndPos))/1000;    // End File Position in double format
    double dPosition=double(Position)/1000;                                                 // Position in double format

    // Ensure Position is not > EndPosition, in that case, change Position to lastposition
    if ((dPosition>0)&&(dPosition>=dEndFile)) {
        Position=QTime(0,0,0,0).msecsTo(EndPos);
        dPosition=double(Position)/1000;
        // if we have the correct last image then return it
        if ((dEndFileCachePos==dEndFile)&&(CacheLastImage)) return new QImage(CacheLastImage->copy());
        // if we have an old last image delete
        if (CacheLastImage) {
            delete CacheLastImage;
            CacheLastImage=NULL;
            dEndFileCachePos=0;
        }
    }

    // Adjust position if input file have a start_time value
    if (ffmpegVideoFile->start_time!=AVNOPTSVALUE)  {
        dPosition+=double(ffmpegVideoFile->start_time)/double(AV_TIME_BASE);
        Position  =int(dPosition*1000);
    }

    // Allocate structure for YUV image
    if (FrameBufferYUV==NULL) FrameBufferYUV=avcodec_alloc_frame();
    if (FrameBufferYUV==NULL) return NULL;

    bool            DataInBuffer        =false;
    QImage          *RetImage           =NULL;
    AVStream        *VideoStream        =ffmpegVideoFile->streams[VideoStreamNumber];
    AVPacket        *StreamPacket       =NULL;

    if ((FrameBufferYUVReady)&&(FrameBufferYUVPosition==Position)) {
        return ConvertYUVToRGB();
    }

    // Cac difftime between asked position and previous end decoded position
    qlonglong DiffTimePosition=-1;
    if (FrameBufferYUVReady) DiffTimePosition=Position-FrameBufferYUVPosition;


    // Calc if we need to seek to a position
    if ((Position==0)||(DiffTimePosition<0)||(DiffTimePosition>100)) { // Allow 0,1 sec diff

        // Flush all buffers
        for (unsigned int i=0;i<ffmpegVideoFile->nb_streams;i++)  {
            AVCodecContext *codec_context = ffmpegVideoFile->streams[i]->codec;
            if (codec_context && codec_context->codec) avcodec_flush_buffers(codec_context);
        }
        FrameBufferYUVReady    = false;
        FrameBufferYUVPosition = 0;

        // Seek to nearest previous key frame
        int64_t seek_target=av_rescale_q(int64_t(Position*1000),AV_TIME_BASE_Q,ffmpegVideoFile->streams[VideoStreamNumber]->time_base);
        if (av_seek_frame(ffmpegVideoFile,VideoStreamNumber,seek_target,AVSEEK_FLAG_BACKWARD)<0) {
            // Try in AVSEEK_FLAG_ANY mode
            if (av_seek_frame(ffmpegVideoFile,VideoStreamNumber,seek_target,AVSEEK_FLAG_ANY)<0) {
                qDebug()<<"Seek error";
            }
        }
    } else {
        DataInBuffer=true;
    }

    //*************************************************************************************************************************************
    // Decoding process : Get StreamPacket until endposition is reach (if sound is wanted) or until image is ok (if image only is wanted)
    //*************************************************************************************************************************************
    bool    Continue        =true;
    bool    IsVideoFind     =false;
    double  FrameTimeBase   =av_q2d(VideoStream->time_base);;
    double  FramePosition   =0;

    while (Continue) {
        StreamPacket=new AVPacket();
        av_init_packet(StreamPacket);
        StreamPacket->flags|=AV_PKT_FLAG_KEY;  // HACK for CorePNG to decode as normal PNG by default

        if (av_read_frame(ffmpegVideoFile,StreamPacket)==0) {

            if (StreamPacket->stream_index==VideoStreamNumber) {
                if (!CodecUsePTS) FramePosition=double((StreamPacket->pts!=AVNOPTSVALUE)?StreamPacket->pts:0)*FrameTimeBase;   // pts instead of dts
                    else          FramePosition=double((StreamPacket->dts!=AVNOPTSVALUE)?StreamPacket->dts:0)*FrameTimeBase;   // dts instead of pts

                int FrameDecoded=0;
                avcodec_decode_video2(VideoStream->codec,FrameBufferYUV,&FrameDecoded,StreamPacket);
                if (FrameDecoded>0) DataInBuffer=true;

                // Create image
                if ((DataInBuffer)&&((FramePosition>=dPosition)||(FramePosition>=dEndFile))) {
                    FrameBufferYUVReady   =true;                        // Keep actual value for FrameBufferYUV
                    FrameBufferYUVPosition=int(FramePosition*1000);     // Keep actual value for FrameBufferYUV
                    RetImage              =ConvertYUVToRGB();           // Create RetImage from YUV Buffer
                    IsVideoFind           =(RetImage!=NULL);
                }

            }

            // Check if we need to continue loop
            Continue=(IsVideoFind==false)&&((dEndFile==0)||(FramePosition<dEndFile));

            if ((IsVideoFind)&&(ApplicationConfig->Crop1088To1080)&&(RetImage->height()==1088)&&(RetImage->width()==1920)) {
                QImage *newRetImage=new QImage(RetImage->copy(0,4,1920,1080));
                delete RetImage;
                RetImage=newRetImage;
            }

        } else {
            // if error in av_read_frame(...) then may be we have reach the end of file !
            Continue=false;
            // Create image
            if (DataInBuffer) {

                FrameBufferYUVReady   =true;                        // Keep actual value for FrameBufferYUV
                FrameBufferYUVPosition=int(FramePosition*1000);     // Keep actual value for FrameBufferYUV
                RetImage              =ConvertYUVToRGB();           // Create RetImage from YUV Buffer
                IsVideoFind           =(RetImage!=NULL);

                if (IsVideoFind) {
                    if ((ApplicationConfig->Crop1088To1080)&&(RetImage->height()==1088)&&(RetImage->width()==1920)) {
                        QImage *newRetImage=new QImage(RetImage->copy(0,4,1920,1080));
                        delete RetImage;
                        RetImage=newRetImage;
                    }
                    if (CacheLastImage) delete CacheLastImage;
                    CacheLastImage  =new QImage(RetImage->copy());
                    dEndFileCachePos=dEndFile;  // keep position for future use
                }

            }
        }

        if (IsVideoFind) {
            ObjectGeometry=IMAGE_GEOMETRY_UNKNOWN;
            double RatioHW=double(RetImage->width())/double(RetImage->height());
            if ((RatioHW>=1.45)&&(RatioHW<=1.55))           ObjectGeometry=IMAGE_GEOMETRY_3_2;
            else if ((RatioHW>=0.65)&&(RatioHW<=0.67))      ObjectGeometry=IMAGE_GEOMETRY_2_3;
            else if ((RatioHW>=1.32)&&(RatioHW<=1.34))      ObjectGeometry=IMAGE_GEOMETRY_4_3;
            else if ((RatioHW>=0.74)&&(RatioHW<=0.76))      ObjectGeometry=IMAGE_GEOMETRY_3_4;
            else if ((RatioHW>=1.77)&&(RatioHW<=1.79))      ObjectGeometry=IMAGE_GEOMETRY_16_9;
            else if ((RatioHW>=0.56)&&(RatioHW<=0.58))      ObjectGeometry=IMAGE_GEOMETRY_9_16;
            else if ((RatioHW>=2.34)&&(RatioHW<=2.36))      ObjectGeometry=IMAGE_GEOMETRY_40_17;
            else if ((RatioHW>=0.42)&&(RatioHW<=0.44))      ObjectGeometry=IMAGE_GEOMETRY_17_40;
        }

        // Continue with a new one
        if (StreamPacket!=NULL) {
            av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
            delete StreamPacket;
            StreamPacket=NULL;
        }
    }

    if (!IsVideoFind) {
        qDebug()<<"No video image return for position "<<Position;
    }

    // Check if it's the last image and if we need to  cache it
    if ((FramePosition>=dEndFile)&&(RetImage)) {
        CacheLastImage  =new QImage(RetImage->copy());
        dEndFileCachePos=dEndFile;  // keep position for future use
    }

    return RetImage;
}

//====================================================================================================================

QImage *cVideoFile::ConvertYUVToRGB() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::ConvertYUVToRGB";
    #endif
    int     W               =ffmpegVideoFile->streams[VideoStreamNumber]->codec->width;
    int     H               =ffmpegVideoFile->streams[VideoStreamNumber]->codec->height;
    QImage  *RetImage       =new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
    AVFrame *FrameBufferRGB =avcodec_alloc_frame();  // Allocate structure for RGB image

    if (FrameBufferRGB!=NULL) {

        avpicture_fill(
                (AVPicture *)FrameBufferRGB,        // Buffer to prepare
                RetImage->bits(),                   // Buffer which will contain the image data
                PIX_FMT_BGRA,                       // The format in which the picture data is stored (see http://wiki.aasimon.org/doku.php?id=ffmpeg:pixelformat)
                W,                                  // The width of the image in pixels
                H                                   // The height of the image in pixels
        );

        // Get a converter from libswscale
        struct SwsContext *img_convert_ctx=sws_getContext(
            W,H,ffmpegVideoFile->streams[VideoStreamNumber]->codec->pix_fmt,        // Src Widht,Height,Format
            W,H,PIX_FMT_BGRA,                                                       // Destination Width,Height,Format
            SWS_FAST_BILINEAR/*SWS_BICUBIC*/,                                       // flags
            NULL,NULL,NULL);                                                        // src Filter,dst Filter,param

        if (img_convert_ctx!=NULL) {
            int ret = sws_scale(
                img_convert_ctx,                                                    // libswscale converter
                FrameBufferYUV->data,                                               // Source buffer
                FrameBufferYUV->linesize,                                           // Source Stride ?
                0,                                                                  // Source SliceY:the position in the source image of the slice to process, that is the number (counted starting from zero) in the image of the first row of the slice
                H,                                                                  // Source SliceH:the height of the source slice, that is the number of rows in the slice
                FrameBufferRGB->data,                                               // Destination buffer
                FrameBufferRGB->linesize                                            // Destination Stride
            );
            if (ret<=0) {
                delete RetImage;
                RetImage=NULL;
            }
            sws_freeContext(img_convert_ctx);
        }

        // free FrameBufferRGB because we don't need it in the future
        av_free(FrameBufferRGB);
    }

    return RetImage;
}

//====================================================================================================================
//DontUseEndPos default=false
QImage *cVideoFile::ImageAt(bool PreviewMode,qlonglong Position,qlonglong StartPosToAdd,bool ForceLoadDisk,cSoundBlockList *SoundTrackBloc,double Volume,
                                   bool ForceSoundOnly,cFilterTransformObject *Filter,bool DontUseEndPos) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::ImageAt";
    #endif
    if (!IsValide) return NULL;
    if (!IsOpen) OpenCodecAndFile();

    // If ForceLoadDisk then ensure CacheImage is null
    if ((ForceLoadDisk)&&(CacheFirstImage!=NULL)) {
        delete CacheFirstImage;
        CacheFirstImage=NULL;
    }

    if ((PreviewMode)&&(CacheFirstImage)&&(Position+StartPosToAdd==0)) return new QImage(CacheFirstImage->copy());

    // Load a video frame
    QImage *LoadedImage=NULL;

    if ((SoundTrackBloc)&&(SoundTrackBloc->NbrPacketForFPS)) ReadAudioFrame(PreviewMode,Position+StartPosToAdd,SoundTrackBloc,Volume,DontUseEndPos);
    if ((!MusicOnly)&&(!ForceSoundOnly)) {
        LoadedImage=ReadVideoFrame(Position+StartPosToAdd,DontUseEndPos);
        if (!LoadedImage) LoadedImage=ReadVideoFrame(Position+StartPosToAdd-100,DontUseEndPos);  // Retry 1/10 sec before
/////////////////// ICI -> 1/10 ce n'est pas bon !!!!!!!!!!!!
    }

    if ((!MusicOnly)&&(!ForceSoundOnly)&&(LoadedImage)) {
        // Scale image if anamorphous codec
        if (AspectRatio!=1) {
            ImageWidth =int(double(LoadedImage->width())*AspectRatio);
            ImageHeight=LoadedImage->height();
            QImage *NewLoadedImage=new QImage(LoadedImage->scaled(ImageWidth,ImageHeight,Qt::IgnoreAspectRatio/*,Qt::SmoothTransformation*/));
            delete LoadedImage;
            LoadedImage=NewLoadedImage;
        }
        // If preview mode and image size > PreviewMaxHeight, reduce Cache Image
        if ((PreviewMode)&&(ImageHeight>PREVIEWMAXHEIGHT*2)) {
            QImage *NewImage=new QImage(LoadedImage->scaledToHeight(PREVIEWMAXHEIGHT));
            delete LoadedImage;
            LoadedImage =NewImage;
        }
        if (Filter && ((!PreviewMode)||(PreviewMode && ApplicationConfig->ApplyTransfoPreview))) Filter->ApplyFilter(LoadedImage);
        if ((PreviewMode)&&(Position+StartPosToAdd==0)) {
            if (CacheFirstImage!=NULL) delete CacheFirstImage;
            CacheFirstImage=LoadedImage;
            LoadedImage=new QImage(CacheFirstImage->copy());
        }

    } else {
        // This case append when sound file is a video file : we don't want image !
        if (LoadedImage) {
            delete LoadedImage;
            LoadedImage=NULL;
        }
    }

    return LoadedImage;
}

//====================================================================================================================

bool cVideoFile::OpenCodecAndFile() {
    // Ensure file was previously checked
    if (!IsValide) return false;

    // Clean memory if a previous file was loaded
    CloseCodecAndFile();
    if (CacheFirstImage!=NULL) {
        delete CacheFirstImage;
        CacheFirstImage=NULL;
    }
    if (CacheLastImage!=NULL) {
        delete CacheLastImage;
        CacheLastImage=NULL;
    }

    //**********************************

    // Open audio stream
    if (AudioStreamNumber!=-1) {
        // if file exist then Open video file and get a LibAVFormat context and an associated LibAVCodec decoder
        #if (LIBAVFORMAT_VERSION_MAJOR>=53)
            if (avformat_open_input(&ffmpegAudioFile,FileName.toLocal8Bit(),NULL,NULL)!=0) return false;
        #else
            if (av_open_input_file(&ffmpegAudioFile,FileName.toLocal8Bit(),NULL,0,NULL)!=0) return false;
        #endif

        ffmpegAudioFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future frames.

        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=28))
            if (avformat_find_stream_info(ffmpegAudioFile,NULL)<0) {
                avformat_close_input(&ffmpegAudioFile);
        #else
            if (av_find_stream_info(ffmpegAudioFile)<0) {    // deprecated : use avformat_find_stream_info instead
                av_close_input_file(ffmpegAudioFile);
        #endif

            return false;
        }

        // Setup STREAM options
        ffmpegAudioFile->streams[AudioStreamNumber]->discard=AVDISCARD_DEFAULT;

        // Find the decoder for the audio stream and open it
        AudioDecoderCodec=avcodec_find_decoder(ffmpegAudioFile->streams[AudioStreamNumber]->codec->codec_id);
        IsVorbis=(strcmp(AudioDecoderCodec->name,"vorbis")==0);

        // Setup decoder options
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->debug_mv         =0;                    // Debug level (0=nothing)
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->debug            =0;                    // Debug level (0=nothing)
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->workaround_bugs  =1;                    // Work around bugs in encoders which sometimes cannot be detected automatically : 1=autodetection
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->lowres           =0;                    // low resolution decoding, 1-> 1/2 size, 2->1/4 size
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->idct_algo        =FF_IDCT_AUTO;         // IDCT algorithm, 0=auto
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->skip_frame       =AVDISCARD_DEFAULT;    // ???????
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->skip_idct        =AVDISCARD_DEFAULT;    // ???????
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->skip_loop_filter =AVDISCARD_DEFAULT;    // ???????
        #if defined(FF_API_ER)
            //ffmpegAudioFile->streams[AudioStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
        #else
            ffmpegAudioFile->streams[AudioStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
        #endif
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->error_concealment=3;

        #if defined(FF_API_AVCODEC_OPEN)
        if ((AudioDecoderCodec==NULL)||(avcodec_open2(ffmpegAudioFile->streams[AudioStreamNumber]->codec,AudioDecoderCodec,NULL)<0)) return false;
        #else
        if ((AudioDecoderCodec==NULL)||(avcodec_open(ffmpegAudioFile->streams[AudioStreamNumber]->codec,AudioDecoderCodec)<0)) return false;
        #endif
        IsOpen=true;
    }

    // Open video stream
    if ((VideoStreamNumber!=-1)&&(!MusicOnly)) {

        // if file exist then Open video file and get a LibAVFormat context and an associated LibAVCodec decoder
        #if (LIBAVFORMAT_VERSION_MAJOR>=53)
            if (avformat_open_input(&ffmpegVideoFile,FileName.toLocal8Bit(),NULL,NULL)!=0) return false;
        #else
            if (av_open_input_file(&ffmpegVideoFile,FileName.toLocal8Bit(),NULL,0,NULL)!=0) return false;
        #endif

        ffmpegVideoFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future frames.

        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=28))
            if (avformat_find_stream_info(ffmpegVideoFile,NULL)<0) {
                avformat_close_input(&ffmpegVideoFile);
        #else
            if (av_find_stream_info(ffmpegVideoFile)<0) {    // deprecated : use avformat_find_stream_info instead
                av_close_input_file(ffmpegVideoFile);
        #endif

            return false;
        }

        // Setup STREAM options
        ffmpegVideoFile->streams[VideoStreamNumber]->discard=AVDISCARD_DEFAULT;

        // Find the decoder for the video stream and open it
        VideoDecoderCodec=avcodec_find_decoder(ffmpegVideoFile->streams[VideoStreamNumber]->codec->codec_id);

        // Setup decoder options
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->debug_mv         =0;                    // Debug level (0=nothing)
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->debug            =0;                    // Debug level (0=nothing)
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->workaround_bugs  =1;                    // Work around bugs in encoders which sometimes cannot be detected automatically : 1=autodetection
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->lowres           =0;                    // low resolution decoding, 1-> 1/2 size, 2->1/4 size
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->idct_algo        =FF_IDCT_AUTO;         // IDCT algorithm, 0=auto
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_frame       =AVDISCARD_DEFAULT;    // ???????
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_idct        =AVDISCARD_DEFAULT;    // ???????
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_loop_filter =AVDISCARD_DEFAULT;    // ???????
        #if defined(FF_API_ER)
            //ffmpegVideoFile->streams[VideoStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
        #else
            ffmpegVideoFile->streams[VideoStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
        #endif
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->error_concealment=3;

        // h264 specific
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->thread_count     =getCpuCount();        // Not sure it work with old ffmpeg version !!!!!
        //ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_loop_filter =AVDISCARD_ALL;        // Reduce quality but speed reading !
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->skip_loop_filter =AVDISCARD_BIDIR;

        #if defined(FF_API_AVCODEC_OPEN)
            if ((VideoDecoderCodec==NULL)||(avcodec_open2(ffmpegVideoFile->streams[VideoStreamNumber]->codec,VideoDecoderCodec,NULL)<0)) {
        #else
            if ((VideoDecoderCodec==NULL)||(avcodec_open(ffmpegVideoFile->streams[VideoStreamNumber]->codec,VideoDecoderCodec)<0)) {
        #endif
            CloseCodecAndFile();
            return false;
        }
        // Hack to correct wrong frame rates that seem to be generated by some codecs
        if (ffmpegVideoFile->streams[VideoStreamNumber]->codec->time_base.num>1000 && ffmpegVideoFile->streams[VideoStreamNumber]->codec->time_base.den==1) ffmpegVideoFile->streams[VideoStreamNumber]->codec->time_base.den=1000;

        CodecUsePTS=ffmpegVideoFile->streams[VideoStreamNumber]->codec->codec_id==CODEC_ID_H264;

        // Get Aspect Ratio
        AspectRatio=double(ffmpegVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->codec->sample_aspect_ratio.den);

        if (ffmpegVideoFile->streams[VideoStreamNumber]->sample_aspect_ratio.num!=0)
            AspectRatio=double(ffmpegVideoFile->streams[VideoStreamNumber]->sample_aspect_ratio.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->sample_aspect_ratio.den);

        if (AspectRatio==0) AspectRatio=1;

        // Special case for DVD mode video without PAR
        if ((AspectRatio==1)&&(ffmpegVideoFile->streams[VideoStreamNumber]->codec->coded_width==720)&&(ffmpegVideoFile->streams[VideoStreamNumber]->codec->coded_height==576))
            AspectRatio=double((576/3)*4)/720;

        // Try to load one image to be sure we can make something with this file
        IsOpen=true;
        QImage *Img =ImageAt(true,0,0,true,NULL,1,false,NULL,false);
        if (Img) {
            // Get informations about size image
            ImageWidth=Img->width();
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
                QImage Final=VideoMask.copy();
                QImage ImgF;
                if (Img->width()>Img->height()) {
                    if (double(Img->width())/double(Img->height())<1.5) ImgF=Img->scaledToWidth(84,Qt::SmoothTransformation);
                    else ImgF=Img->scaledToWidth(88,Qt::SmoothTransformation);
                } else ImgF=Img->scaledToHeight(88,Qt::SmoothTransformation);
                QPainter Painter;
                Painter.begin(&Final);
                Painter.drawImage(QRect((96-ImgF.width())/2,(96-ImgF.height())/2,ImgF.width(),ImgF.height()),ImgF);
                Painter.end();
                AddIcons(&Final);
            }
            delete Img;
        } else {
            CloseCodecAndFile();
            return false;
        }
    }

    return IsOpen;
}
