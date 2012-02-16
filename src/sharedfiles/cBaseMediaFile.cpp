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
#include "_QCustomDialog.h"
#include <QFileDialog>
#include <QPainter>

// Include some additional standard class
#include "cDeviceModelDef.h"
#include "cBaseMediaFile.h"
#include "cLuLoImageCache.h"

#define FFD_APPLICATION_ROOTNAME    "Project"           // Name of root node in the project xml file

cThumbCache::cThumbCache(QString Path) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbCache::cThumbCache");

    ThumbCacheFile    =AdjustDirForOS(Path+QString(THUMBNAILCACHEFILE));
    ThumbCacheDocument=QDomDocument(THUMBCACHE_APPNAME);

    QFile   SourceFile(ThumbCacheFile);
    QString errorStr;
    int     errorLine,errorColumn;

    bool IsThumbCacheReady=false;
    if (SourceFile.open(QFile::ReadOnly | QFile::Text)) {
        IsThumbCacheReady=(ThumbCacheDocument.setContent(&SourceFile,true,&errorStr,&errorLine,&errorColumn))&&(ThumbCacheDocument.documentElement().tagName()==THUMBCACHE_ROOTNAME);
        SourceFile.close();
        // Scan all entries to delete thumb for file no longer exist and thumb for file wich timestamp is not the same
        QDomNodeList    nodeList=ThumbCacheDocument.elementsByTagName("Thumbnails");
        for (int i=0;i<nodeList.count();i++) {
            bool        ToDelete=true;
            QDomElement Element=nodeList.at(i).toElement();
            QString     ShortFileName="";
            if (Element.hasAttribute("ShortFileName")) {
                ShortFileName=Element.attribute("ShortFileName");
                if (Element.hasAttribute("TimeStamp")) {
                    QFileInfo FileInfo(AdjustDirForOS(Path+ShortFileName));
                    QString   TimeStamp=Element.attribute("TimeStamp");
                    QString   fTimeStamp=FileInfo.lastModified().toString("dd/MM/yyyy hh:mm:ss.zzz");
                    ToDelete=(!FileInfo.exists());
                    ToDelete=ToDelete||(fTimeStamp!=TimeStamp);
                }
            }
            if (ToDelete)
                ThumbCacheDocument.documentElement().removeChild(Element); else i++;
        }
    }
    if (!IsThumbCacheReady) {
        ThumbCacheDocument=QDomDocument(THUMBCACHE_APPNAME);
        ThumbCacheDocument.appendChild(ThumbCacheDocument.createElement(THUMBCACHE_ROOTNAME));
        IsThumbCacheReady=true;
    }
}

//****************************************************************************************************************************************************************

cThumbCache::~cThumbCache() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cThumbCache::~cThumbCache");

    if (IsModify) {
        if ((QFileInfo(ThumbCacheFile).exists())&&(!QFile(ThumbCacheFile).remove())) {
            ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Error overwritting %1").arg(ThumbCacheFile));
        } else if (ThumbCacheDocument.elementsByTagName("Thumbnails").count()>0) {
            QFile DestinationFile(ThumbCacheFile);
            if (DestinationFile.open(QFile::WriteOnly | QFile::Text)) {
                QTextStream out(&DestinationFile);
                ThumbCacheDocument.save(out,4);
                DestinationFile.close();
            }
        }
    }
}

//****************************************************************************************************************************************************************

// from Google music manager (see:http://code.google.com/p/gogglesmm/source/browse/src/gmutils.cpp?spec=svn6c3dbecbad40ee49736b9ff7fe3f1bfa6ca18c13&r=6c3dbecbad40ee49736b9ff7fe3f1bfa6ca18c13)
bool gm_decode_base64(uchar *buffer,uint &len) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:gm_decode_base64");

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
        if ((ASFFile.tag())&&(ASFFile.tag()->attributeListMap().contains("WM/Picture"))) {
            const TagLib::ASF::AttributeList &attrList=ASFFile.tag()->attributeListMap()["WM/Picture"];
            if (!attrList.isEmpty()) {
                    TagLib::ASF::Picture pic = attrList[0].toPicture();
                    if (pic.isValid()) Image->loadFromData((const uchar *)pic.picture().data(),pic.picture().size());
            }
        }
    }
    #endif
    //***********
    if (!Image->isNull()) return Image; else {
        delete Image;
        return NULL;
    }
}

//*********************************************************************************************************************************************
// Base class object
//*********************************************************************************************************************************************

cBaseMediaFile::cBaseMediaFile(cBaseApplicationConfig *TheApplicationConfig):cCustomIcon() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::cBaseMediaFile");

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
    ApplicationConfig   = TheApplicationConfig;
    AspectRatio         = 1;
    ImageOrientation    = -1;
}

//====================================================================================================================

cBaseMediaFile::~cBaseMediaFile() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cBaseMediaFile::~cBaseMediaFile for object %1").arg(FileName));
}

//*****************************************************************************************************************************************

bool cBaseMediaFile::GetThumbnailFromCache(cThumbCache *ThumbCache) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetThumbnailFromCache");

    if (!ThumbCache) return false;
    bool    IsOk=false;

    QString ShortFileName=ShortName;
    if (ObjectType==OBJECTTYPE_FOLDER) ShortFileName="###FOLDER###"; else ShortFileName =QFileInfo(FileName).fileName();

    QDomNodeList    nodeList=ThumbCache->ThumbCacheDocument.elementsByTagName("Thumbnails");
    bool            IsFind=false;
    for (int i=0;(i<nodeList.count())&&(!IsFind);i++) {
        QDomElement Element=nodeList.at(i).toElement();
        if ((Element.hasAttribute("Thumbnail"))&&(Element.hasAttribute("ShortFileName"))&&(Element.hasAttribute("TimeStamp"))&&(Element.attribute("ShortFileName")==ShortFileName)) {
            if ((Element.attribute("TimeStamp")==ModifDateTime.toString("dd/MM/yyyy hh:mm:ss.zzz"))) {
                IsFind=true;
                QImage Image;
                if (Image.loadFromData(qUncompress(QByteArray::fromHex(Element.attribute("Thumbnail").toUtf8())),"PNG")) {
                    LoadIcons(&Image);
                    if (Element.hasAttribute("PixelXDimension")) ImageWidth=Element.attribute("PixelXDimension").toInt();
                    if (Element.hasAttribute("PixelYDimension")) ImageHeight=Element.attribute("PixelYDimension").toInt();
                    if (GetInformationValue("Photo.PixelXDimension")=="") InformationList.append(QString("Photo.PixelXDimension")+QString("##")+QString("%1").arg(ImageWidth));
                    if (GetInformationValue("Photo.PixelYDimension")=="") InformationList.append(QString("Photo.PixelYDimension")+QString("##")+QString("%1").arg(ImageHeight));
                    IsOk=true;
                } else {
                    ToLog(LOGMSG_CRITICAL,QString("GetThumbnailFromCache Error reading %1").arg(FileName));
                }
            } else {
                ThumbCache->ThumbCacheDocument.documentElement().removeChild(Element);
            }
        }
    }
    return IsOk;
}

//*****************************************************************************************************************************************

void cBaseMediaFile::AddThumbnailToCache(cThumbCache *ThumbCache,QDateTime TimeStamp) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::AddThumbnailToCache");

    if (!ThumbCache)                    return;
    if (ImageWidth*ImageHeight<1000000) return; // No cache for image <1M pixel

    ThumbCache->IsModify=true;

    QByteArray  Compressed,ImageHexed;
    QString     ShortFileName=ShortName;

    if (ObjectType==OBJECTTYPE_FOLDER) ShortFileName="###FOLDER###"; else ShortFileName =QFileInfo(FileName).fileName();

    QByteArray  ba;
    QBuffer     buf(&ba);
    bool        IsSaved   =false;

    QDomNodeList nodeList=ThumbCache->ThumbCacheDocument.elementsByTagName("Thumbnails");
    for (int i=0;i<nodeList.count();i++) {
        QDomElement Element=nodeList.at(i).toElement();
        if ((Element.hasAttribute("ShortFileName"))&&(Element.attribute("ShortFileName")==ShortFileName)) {
            IconBIG.save(&buf,"PNG");
            Compressed=qCompress(ba,1);
            ImageHexed=Compressed.toHex();
            Element.setAttribute("Thumbnail",QString(ImageHexed));
            Element.setAttribute("Width",IconBIG.width());
            Element.setAttribute("Height",IconBIG.height());
            Element.setAttribute("PixelXDimension",ImageWidth);
            Element.setAttribute("PixelYDimension",ImageHeight);
            Element.setAttribute("TimeStamp",TimeStamp.toString("dd/MM/yyyy hh:mm:ss.zzz"));
            IsSaved=true;
            i=nodeList.count();
        }
    }
    if (!IsSaved) {
        QDomElement SubElement=ThumbCache->ThumbCacheDocument.createElement("Thumbnails");
        SubElement.setAttribute("ShortFileName",ShortFileName);
        IconBIG.save(&buf,"PNG");
        Compressed=qCompress(ba,1);
        ImageHexed=Compressed.toHex();
        SubElement.setAttribute("Thumbnail",QString(ImageHexed));
        SubElement.setAttribute("Width",IconBIG.width());
        SubElement.setAttribute("Height",IconBIG.height());
        SubElement.setAttribute("PixelXDimension",ImageWidth);
        SubElement.setAttribute("PixelYDimension",ImageHeight);
        SubElement.setAttribute("TimeStamp",TimeStamp.toString("dd/MM/yyyy hh:mm:ss.zzz"));
        ThumbCache->ThumbCacheDocument.documentElement().appendChild(SubElement);
    }
}

//====================================================================================================================

bool cBaseMediaFile::GetInformationFromFile(QString GivenFileName,QStringList *AliasList,bool *ModifyFlag) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBaseMediaFile::GetInformationFromFile");

    FileName            =QFileInfo(GivenFileName).absoluteFilePath();

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
        if (CustomMessageBox(ApplicationConfig->TopLevelWindow,QMessageBox::Question,QApplication::translate("cBaseMediaFile","Open file"),
            QApplication::translate("cBaseMediaFile","Impossible to open file ")+FileName+"\n"+QApplication::translate("cBaseMediaFile","Do you want to select another file ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes)
            Continue=false;
        else {
            QString NewFileName=QFileDialog::getOpenFileName(ApplicationConfig->TopLevelWindow,QApplication::translate("cBaseMediaFile","Select another file for ")+QFileInfo(FileName).fileName(),
               ApplicationConfig->RememberLastDirectories?ApplicationConfig->LastMediaPath:"",
               ApplicationConfig->GetFilterForMediaFile(ObjectType==OBJECTTYPE_IMAGEFILE?cBaseApplicationConfig::IMAGEFILE:ObjectType==OBJECTTYPE_VIDEOFILE?cBaseApplicationConfig::VIDEOFILE:cBaseApplicationConfig::MUSICFILE));
            if (NewFileName!="") {
                if (AliasList) AliasList->append(FileName+"####"+NewFileName);
                FileName=NewFileName;
                if (ApplicationConfig->RememberLastDirectories) ApplicationConfig->LastMediaPath=QFileInfo(FileName).absolutePath();     // Keep folder for next use
                if (ModifyFlag) *ModifyFlag=true;
            } else Continue=false;
        }
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

bool cUnmanagedFile::IsFilteredFile(int RequireObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cUnmanagedFile::IsFilteredFile");

    return RequireObjectType==OBJECTTYPE_UNMANAGED;
}

//*********************************************************************************************************************************************
// Folder
//*********************************************************************************************************************************************

cFolder::cFolder(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::cFolder");

    ObjectType  =OBJECTTYPE_FOLDER;
}

//====================================================================================================================

bool cFolder::GetInformationFromFile(QString GivenFileName,QStringList */*AliasList*/,bool */*ModifyFlag*/) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::GetInformationFromFile");

    FileName            =QFileInfo(GivenFileName).absoluteFilePath();
    ShortName           =QFileInfo(GivenFileName).fileName();
    CreatDateTime       =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime       =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    return true;
}

//====================================================================================================================

bool cFolder::IsFilteredFile(int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFolder::IsFilteredFile");

    return true;    // always valide
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
            #if defined(Q_OS_WIN)
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
                    #if defined(Q_OS_WIN)
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
            #if defined(Q_OS_WIN)
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

bool cffDProjectFile::GetInformationFromFile(QString GivenFileName,QStringList */*AliasList*/,bool */*ModifyFlag*/) {
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

bool cffDProjectFile::IsFilteredFile(int RequireObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cffDProjectFile::IsFilteredFile");

    return (RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(RequireObjectType==OBJECTTYPE_FFDFILE);
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
}

//====================================================================================================================

bool cImageFile::IsFilteredFile(int RequireObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::IsFilteredFile");

    if (ObjectType==OBJECTTYPE_IMAGEFILE) return (RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(RequireObjectType==OBJECTTYPE_IMAGEFILE);
        else return (RequireObjectType==OBJECTTYPE_UNMANAGED);
}

//====================================================================================================================

QString cImageFile::GetFileTypeStr() {
    ToLog(LOGMSG_DEBUGTRACE,QString("IN:cImageFile::GetFileTypeStr for %1").arg(FileName));

    if (ObjectType==OBJECTTYPE_IMAGEFILE) return QApplication::translate("cBaseMediaFile","Image","File type");
        else return QApplication::translate("cBaseMediaFile","Thumbnail","File type");
}

//====================================================================================================================

void cImageFile::GetFullInformationFromFile(cThumbCache *ThumbCache) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::GetFullInformationFromFile");

    ImageOrientation    =-1;
    IsInformationValide =false;
    bool ExifOk=false;

    // ******************************************************************************************************
    // Try to load EXIF information using library exiv2 [Linux version]
    // ******************************************************************************************************
    #if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
        Exiv2::Image::AutoPtr ImageFile;
        try {
            #if defined(Q_OS_WIN)
                ImageFile=Exiv2::ImageFactory::open(FileName.toLocal8Bit().data());
            #else
                ImageFile=Exiv2::ImageFactory::open(FileName.toUtf8().data());
            #endif
            ExifOk=true;
        }
        catch( Exiv2::Error& e ) {
            ToLog(LOGMSG_INFORMATION,QApplication::translate("cBaseMediaFile","Image don't have EXIF metadata %1").arg(FileName));
        }
        if (ExifOk) {
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
                        #if defined(Q_OS_WIN)
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
            if (GetInformationValue("Image.Model")!="")  InformationList.append(QString("composer")+QString("##")+GetInformationValue("Image.Model"));

            // Get size information
            if (GetInformationValue("Photo.PixelXDimension")!="") ImageWidth =GetInformationValue("Photo.PixelXDimension").toInt();
            if (GetInformationValue("Photo.PixelYDimension")!="") ImageHeight=GetInformationValue("Photo.PixelYDimension").toInt();

            // Read preview image
            #ifdef EXIV2WITHPREVIEW
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
                        // if preview Icon have a really small size, then don't use it
                        if (Icon->height()>=ApplicationConfig->MinimumEXIFHeight) LoadIcons(Icon);
                    }
                    delete Icon;
                }
                delete Manager;
            }
            #endif
        }

    #elif defined(Q_OS_WIN)

        // ******************************************************************************************************
        // Try to load EXIF information using binary exiv2 [Windows version]
        // ******************************************************************************************************
        QString  Commande=AdjustDirForOS("exiv2 print -pa \""+FileName+"\"");
        QString  Info,Part;
        QProcess Process;
        ExifOk=true;
        Process.setProcessChannelMode(QProcess::MergedChannels);
        Process.start(Commande);
        if (!Process.waitForStarted()) {
            ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Impossible to start exiv2 - no exif informations will be decode for %1").arg(FileName));
            ExifOk=false;
        }
        if (ExifOk && !Process.waitForFinished()) {
            Process.kill();
            ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Error during exiv2 process - no exif informations will be decode for %1").arg(FileName));
            ExifOk=false;
        }
        if (ExifOk && (Process.exitStatus()<0)) {
            ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","exiv2 return error %1 - no exif informations will be decode for %2").arg(Process.exitStatus()).arg(FileName));
            ExifOk=false;
        }
        if (ExifOk) {
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
                    if (Part=="Photo.PixelXDimension") ImageWidth =Value.toInt();
                    if (Part=="Photo.PixelYDimension") ImageHeight=Value.toInt();
                }
            }
            Process.terminate();
            Process.close();

            // Restart same job with -pva option to get binary value of orientation
            Commande=AdjustDirForOS("exiv2 print -pva \""+FileName+"\"");
            Process.start(Commande);
            if (!Process.waitForStarted()) {
                ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Impossible to start exiv2 - no exif informations will be decode for %1").arg(FileName));
                ExifOk=false;
            }
            if (ExifOk && !Process.waitForFinished()) {
                Process.kill();
                ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","Error during exiv2 process - no exif informations will be decode for %1").arg(FileName));
                ExifOk=false;
            }
            if (ExifOk && (Process.exitStatus()<0)) {
                ToLog(LOGMSG_CRITICAL,QApplication::translate("cBaseMediaFile","exiv2 return error %1 - no exif informations will be decode for %2").arg(Process.exitStatus()).arg(FileName));
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
                            ImageOrientation=Value.toInt();
                        }
                    }
                }
            }
        }
        Process.terminate();
        Process.close();
    #endif

    //************************************************************************************
    // If no exif preview image (of image too small) then load/create thumbnail
    //************************************************************************************
    if ((IsIconNeeded)&&(Icon16.isNull())) {

        // Try to load thumb from thumbcache (if exist)
        if (ThumbCache) GetThumbnailFromCache(ThumbCache);

        if (Icon16.isNull()) {
            cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileName,ModifDateTime,ImageOrientation,NULL,ApplicationConfig->Smoothing,true);
            if (ImageObject==NULL) {
                ToLog(LOGMSG_CRITICAL,"Error in cImageFile::GetFullInformationFromFile : FindObject return NULL for thumbnail creation !");
            } else {
                QImage *LN_Image=ImageObject->ValidateCacheRenderImage();   // Get a link to render image in LuLoImageCache collection
                if ((LN_Image==NULL)||(LN_Image->isNull())) {
                    ToLog(LOGMSG_CRITICAL,"Error in cImageFile::GetFullInformationFromFile : ValidateCacheRenderImage return NULL for thumbnail creation !");
                } else {
                    LoadIcons(LN_Image);
                    if (ThumbCache) AddThumbnailToCache(ThumbCache,ModifDateTime);
                }
            }
        }
    }

    //************************************************************************************
    // if no information about size then load image
    //************************************************************************************
    if ((ImageWidth==0)||(ImageHeight==0)) {
        cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileName,ModifDateTime,ImageOrientation,NULL,ApplicationConfig->Smoothing,true);
        if (ImageObject==NULL) {
            ToLog(LOGMSG_CRITICAL,"Error in cImageFile::GetFullInformationFromFile : FindObject return NULL for size computation !");
        } else {
            QImage *LN_Image=ImageObject->ValidateCacheRenderImage();   // Get a link to render image in LuLoImageCache collection
            if ((LN_Image==NULL)||(LN_Image->isNull())) {
                ToLog(LOGMSG_CRITICAL,"Error in cImageFile::GetFullInformationFromFile : ValidateCacheRenderImage return NULL for size computation !");
            } else {
                ImageWidth =LN_Image->width();
                ImageHeight=LN_Image->height();
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
    if (Icon16.isNull()) LoadIcons(&ApplicationConfig->DefaultIMAGEIcon);

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

QImage *cImageFile::ImageAt(bool PreviewMode,cFilterTransformObject *Filter) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cImageFile::ImageAt");

    if (!IsValide)            return NULL;
    if (!IsInformationValide) GetFullInformationFromFile();

    QImage                *LN_Image   =NULL;
    QImage                *RetImage   =NULL;
    cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(FileName,ModifDateTime,ImageOrientation,Filter,(!PreviewMode || ApplicationConfig->Smoothing),true);

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

cVideoFile::cVideoFile(int TheWantedObjectType,cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::cVideoFile");

    MusicOnly               = (TheWantedObjectType==OBJECTTYPE_MUSICFILE);
    ObjectType              = TheWantedObjectType;
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
    dEndFileCachePos        = 0;                        // Position of the cache image of last image of the video
    VideoCodecInfo          = "";
    VideoTrackNbr           = 0;
    VideoStreamNumber       =-1;
    NbrChapters             = 0;

    // Audio part
    ffmpegAudioFile         = NULL;
    AudioDecoderCodec       = NULL;
    LastAudioReadedPosition =-1;
    IsVorbis                = false;
    AudioCodecInfo          = "";
    AudioTrackNbr           = 0;
    AudioStreamNumber       =-1;
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

    int64_t AVNOPTSVALUE=INT64_C(0x8000000000000000); // to solve type error with Qt

    AVFormatContext *ffmpegFile=NULL;;

    //*********************************************************************************************************
    // Open file and get a LibAVFormat context and an associated LibAVCodec decoder
    //*********************************************************************************************************
    #if (LIBAVFORMAT_VERSION_MAJOR>=53)
    if (avformat_open_input(&ffmpegFile,FileName.toLocal8Bit(),NULL,NULL)!=0) return;
    #else
    if (av_open_input_file(&ffmpegFile,FileName.toLocal8Bit(),NULL,0,NULL)!=0) return;
    #endif

    ffmpegFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future frames.

    //*********************************************************************************************************
    // Search stream in file
    //*********************************************************************************************************
    #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=23))
    if (avformat_find_stream_info(ffmpegFile,NULL)<0) {
        avformat_close_input(&ffmpegFile);
        return;
    }
    #else
    if (av_find_stream_info(ffmpegFile)<0) {
        av_close_input_file(ffmpegFile);// deprecated : use avformat_find_stream_info instead
        return;
    }
    #endif

    //*********************************************************************************************************
    // Get metadata
    //*********************************************************************************************************
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

    //*********************************************************************************************************
    // Get chapters
    //*********************************************************************************************************
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

    //*********************************************************************************************************
    // Get informations about duration
    //*********************************************************************************************************
    int         hh,mm,ss;
    qlonglong   ms;

    ms=ffmpegFile->duration;
    if (ffmpegFile->start_time!=AVNOPTSVALUE)  ms-=ffmpegFile->start_time;
    ms=ms/1000;

    ss=ms/1000;
    mm=ss/60;
    hh=mm/60;
    mm=mm-(hh*60);
    ss=ss-(ss/60)*60;
    ms=ms-(ms/1000)*1000;
    Duration=QTime(hh,mm,ss,ms);

    EndPos  =Duration;    // By default : EndPos is set to the end of file
    InformationList.append(QString("Duration")+QString("##")+Duration.toString("HH:mm:ss.zzz"));

    //*********************************************************************************************************
    // Get information from track
    //*********************************************************************************************************
    for (int Track=0;Track<(int)ffmpegFile->nb_streams;Track++) {

        // Find codec
        AVCodec *Codec=avcodec_find_decoder(ffmpegFile->streams[Track]->codec->codec_id);

        //*********************************************************************************************************
        // Audio track
        //*********************************************************************************************************
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
            QString SampleFMT="";
            switch (ffmpegFile->streams[Track]->codec->sample_fmt) {
                case AV_SAMPLE_FMT_U8 : SampleFMT="-U8";    break;
                case AV_SAMPLE_FMT_S16: SampleFMT="-S16";    break;
                case AV_SAMPLE_FMT_S32: SampleFMT="-S32";    break;
                default               : SampleFMT="-?";      break;
            }
            if (ffmpegFile->streams[Track]->codec->channels==1)        InformationList.append(TrackNum+QString("Channels")+QString("##")+QApplication::translate("cBaseMediaFile","Mono","Audio channels mode")+SampleFMT);
            else if (ffmpegFile->streams[Track]->codec->channels==2)   InformationList.append(TrackNum+QString("Channels")+QString("##")+QApplication::translate("cBaseMediaFile","Stereo","Audio channels mode")+SampleFMT);
            else                                                       InformationList.append(TrackNum+QString("Channels")+QString("##")+QString("%1").arg(ffmpegFile->streams[Track]->codec->channels)+SampleFMT);

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

        //*********************************************************************************************************
        // Video track
        //*********************************************************************************************************
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

    //*********************************************************************************************************
    // Close file
    //*********************************************************************************************************
    #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=23))
    avformat_close_input(&ffmpegFile);
    #else
    av_close_input_file(ffmpegFile);
    #endif

    //*********************************************************************************************************
    // Produce thumbnail
    //*********************************************************************************************************

    IsInformationValide=true;

    // If it's an audio file, try to get embeded image
    if (ObjectType==OBJECTTYPE_MUSICFILE) {
        QImage *Img=GetEmbededImage(FileName);
        if (Img) {
            LoadIcons(Img);
            delete Img;
        }

    // If it's a video then search if an image (jpg) with same name exist
    } else if (ObjectType==OBJECTTYPE_VIDEOFILE) {

        // Search if a jukebox mode thumbnail (jpg file with same name as video) exist
        QFileInfo   File(FileName);
        QString     JPegFile=File.absolutePath()+(File.absolutePath().endsWith(QDir::separator())?"":QString(QDir::separator()))+File.completeBaseName()+".jpg";
        if (QFileInfo(JPegFile).exists()) LoadIcons(JPegFile);

        if (Icon16.isNull()) {
            // Open file
            OpenCodecAndFile();
            CloseCodecAndFile();
        }
    }

    // if no icon then load default for type
    if (Icon16.isNull()) LoadIcons(ObjectType==OBJECTTYPE_VIDEOFILE?&ApplicationConfig->DefaultVIDEOIcon:&ApplicationConfig->DefaultMUSICIcon);

}

//====================================================================================================================

bool cVideoFile::IsFilteredFile(int RequireObjectType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::IsFilteredFile");

    return (RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(ObjectType==RequireObjectType);
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

    // Close the video codec
    if (VideoDecoderCodec!=NULL) {
        avcodec_close(ffmpegVideoFile->streams[VideoStreamNumber]->codec);
        VideoDecoderCodec=NULL;
    }

    // Close the video file
    if (ffmpegVideoFile!=NULL) {
        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=23))
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
        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=23))
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
    FrameBufferYUVReady=false;
    IsOpen=false;
}

//====================================================================================================================
// Read an audio frame from current stream
//====================================================================================================================
void cVideoFile::ReadAudioFrame(bool PreviewMode,qlonglong Position,cSoundBlockList *SoundTrackBloc,double Volume,bool DontUseEndPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ReadAudioFrame");

    // Ensure file was previously open and all is ok
    if ((SoundTrackBloc==NULL)||(AudioStreamNumber==-1)||(ffmpegAudioFile->streams[AudioStreamNumber]==NULL)||(ffmpegAudioFile==NULL)||(AudioDecoderCodec==NULL)) return;

    // Ensure Position is not > EndPosition
    if (Position>QTime(0,0,0,0).msecsTo(DontUseEndPos?Duration:EndPos)) return;

    int64_t         AVNOPTSVALUE        =INT64_C(0x8000000000000000); // to solve type error with Qt
    AVStream        *AudioStream        =ffmpegAudioFile->streams[AudioStreamNumber];
    int64_t         SrcSampleSize       =(AudioStream->codec->sample_fmt==AV_SAMPLE_FMT_S16?2:1)*int64_t(AudioStream->codec->channels);
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
    if (ffmpegAudioFile->start_time!=AVNOPTSVALUE)
        dPosition+=double(ffmpegAudioFile->start_time)/double(AV_TIME_BASE);

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
            // Try in AVSEEK_FLAG_ANY mode
            if (av_seek_frame(ffmpegAudioFile,AudioStreamNumber,seek_target,AVSEEK_FLAG_ANY)<0) {
                ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::ReadAudioFrame : Seek error");
            }
        }
        FramePosition=-1;

    }

    //*************************************************************************************************************************************
    // Decoding process : Get StreamPacket until endposition is reach (if sound is wanted) or until image is ok (if image only is wanted)
    //*************************************************************************************************************************************

    while (Continue) {
        StreamPacket=new AVPacket();
        av_init_packet(StreamPacket);

        int err=av_read_frame(ffmpegAudioFile,StreamPacket);
        if (err<0) {
            // if error in av_read_frame(...) then may be we have reach the end of file !
            Continue=false;
            ToLog(LOGMSG_CRITICAL,QString("Error in cVideoFile::ReadAudioFrame : av_read_frame error %1").arg(err));
        } else {
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
                    int Len=avcodec_decode_audio3(AudioStream->codec,(int16_t *)BufferToDecode,&SizeDecoded,&PacketTemp);
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

                        } //else qDebug()<<"Skip packet :"<<FramePosition<<"-"<<FramePosition+FrameDuration<<"["<<dPosition<<"-"<<EndPosition<<"]";

                        PacketTemp.data+=Len;
                        PacketTemp.size-=Len;
                        FramePosition=FramePosition+FrameDuration;

                        //qDebug()<<"Bloc lu : data="<<AudioLenDecoded<<"/"<<AudioDataWanted;
                    }
                }
            }

            // Continue with a new one
            av_free_packet(StreamPacket); // Free the StreamPacket that was allocated by previous call to av_read_frame
            delete StreamPacket;
            StreamPacket=NULL;

            // Check if we need to continue loop
            Continue=(AudioLenDecoded<AudioDataWanted);
        }
    }
    // Keep NextPacketPosition for determine next time if we need to seek
    LastAudioReadedPosition=int(EndPosition*1000);

    //**********************************************************************
    // Transfert data from BufferForDecoded to Buffer using audio_resample
    //**********************************************************************
    if (AudioLenDecoded>0) {

        // u8 mono or stereo TO Stereo-i16
        if (AudioStream->codec->sample_fmt==AV_SAMPLE_FMT_U8) {
            uint8_t *NewBuffer=(uint8_t *)av_malloc(MaxAudioLenDecoded);
            uint8_t *Buf1=(uint8_t*)BufferForDecoded;
            int16_t *Buf2=(int16_t*)NewBuffer;
            int16_t Val;
            for (int j=0;j<AudioLenDecoded;j++) {
                Val=((double(*(Buf1++))-double(128))/double(128))*double(32768);
                *(Buf2++)=Val;                                                      // to i16
                if (AudioStream->codec->channels==1) *(Buf2++)=Val;                 // to stereo
            }
            av_free(BufferForDecoded);
            BufferForDecoded=NewBuffer;
            AudioLenDecoded*=2;                                                     // to i16
            if (AudioStream->codec->channels==1) AudioLenDecoded*=2;                // to stereo

        // Mono-i16 to stereo-i16
        } else if (AudioStream->codec->channels==1) {
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
        }

        // Now, we are sure we have stereo-i16 samples

        // Adjust volume
        if (Volume!=1) {
            int16_t *Buf1=(int16_t*)BufferForDecoded;
            int32_t mix;
            for (int j=0;j<AudioLenDecoded/4;j++) {
                // Left channel : Adjust if necessary (16 bits)
                mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
                // Right channel : Adjust if necessary (16 bits)
                mix=int32_t(double(*(Buf1))*Volume); if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;  *(Buf1++)=int16_t(mix);
            }
        }

        if (SoundTrackBloc->SamplingRate!=AudioStream->codec->sample_rate) {

            double  NewSize=((double(AudioLenDecoded)/double(DstSampleSize))/double(AudioStream->codec->sample_rate))*double(SoundTrackBloc->SamplingRate);
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
                *(PtrDst++)=Left_x2;    //Left_x3;
                *(PtrDst++)=Right_x2;   //Right_x3;
                RealNewSize++;
            }

            // Append data to SoundTrackBloc
            SoundTrackBloc->AppendData((int16_t*)NewBuf,RealNewSize*DstSampleSize);
            av_free(NewBuf); // Free allocated buffers

        } else {
            // Append data to SoundTrackBloc
            SoundTrackBloc->AppendData((int16_t*)BufferForDecoded,AudioLenDecoded);
        }

    }

    // Now ensure SoundTrackBloc have correct wanted packet (if no then add nullsound)
    //while (SoundTrackBloc->List.count()<SoundTrackBloc->NbrPacketForFPS) SoundTrackBloc->AppendNullSoundPacket();

    if (BufferToDecode)   av_free(BufferToDecode);
    if (BufferForDecoded) av_free(BufferForDecoded);
}

//====================================================================================================================
// Read a video frame from current stream
//====================================================================================================================

#define MAXELEMENTSINOBJECTLIST 500

QImage *cVideoFile::ReadVideoFrame(qlonglong Position,bool DontUseEndPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ReadVideoFrame");

    int64_t AVNOPTSVALUE=INT64_C(0x8000000000000000); // to solve type error with Qt

    // Ensure file was previously open
    if ((ffmpegVideoFile==NULL)||(VideoDecoderCodec==NULL)) return NULL;

    double dEndFile =double(QTime(0,0,0,0).msecsTo(DontUseEndPos?Duration:EndPos))/1000;    // End File Position in double format
    double dPosition=double(Position)/1000;                                                 // Position in double format

    if (dEndFile==0) {
        ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::ReadVideoFrame : dEndFile=0 ?????");
        return NULL;
    }

    // Ensure Position is not > EndPosition, in that case, change Position to lastposition
    if ((dPosition>0)&&(dPosition>=dEndFile)) {
        Position=QTime(0,0,0,0).msecsTo(EndPos);
        dPosition=double(Position)/1000;
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
                ToLog(LOGMSG_CRITICAL,"Error in cVideoFile::ReadVideoFrame : Seek error");
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
            Continue=(IsVideoFind==false)&&(FramePosition<dEndFile);

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

    if ((!IsVideoFind)&&(!RetImage)) {
        ToLog(LOGMSG_CRITICAL,QString("No video image return for position %1 => return black frame").arg(Position));
        RetImage =new QImage(ffmpegVideoFile->streams[VideoStreamNumber]->codec->width,ffmpegVideoFile->streams[VideoStreamNumber]->codec->height,QImage::Format_ARGB32_Premultiplied);
        RetImage->fill(0);
    }

    dEndFileCachePos=dEndFile;  // keep position for future use

    return RetImage;
}

//====================================================================================================================
//#define PIXFMT      PIX_FMT_BGRA
//#define QTPIXFMT    QImage::Format_ARGB32_Premultiplied
#define PIXFMT      PIX_FMT_RGB24
#define QTPIXFMT    QImage::Format_RGB888

QImage *cVideoFile::ConvertYUVToRGB() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ConvertYUVToRGB");

    int     W               =ffmpegVideoFile->streams[VideoStreamNumber]->codec->width;
    int     H               =ffmpegVideoFile->streams[VideoStreamNumber]->codec->height;

    QImage  *RetImage       =new QImage(W,H,QTPIXFMT);
    AVFrame *FrameBufferRGB =avcodec_alloc_frame();  // Allocate structure for RGB image

    if (FrameBufferRGB!=NULL) {

        avpicture_fill(
                (AVPicture *)FrameBufferRGB,        // Buffer to prepare
                RetImage->bits(),                   // Buffer which will contain the image data
                PIXFMT,                             // The format in which the picture data is stored (see http://wiki.aasimon.org/doku.php?id=ffmpeg:pixelformat)
                W,                                  // The width of the image in pixels
                H                                   // The height of the image in pixels
        );

        // Get a converter from libswscale
        struct SwsContext *img_convert_ctx=sws_getContext(
            W,H,ffmpegVideoFile->streams[VideoStreamNumber]->codec->pix_fmt,        // Src Widht,Height,Format
            W,H,PIXFMT,                                                             // Destination Width,Height,Format
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
QImage *cVideoFile::ImageAt(bool PreviewMode,qlonglong Position,qlonglong StartPosToAdd,cSoundBlockList *SoundTrackBloc,double Volume,
                                   bool ForceSoundOnly,cFilterTransformObject *Filter,bool DontUseEndPos) {

    ToLog(LOGMSG_DEBUGTRACE,"IN:cVideoFile::ImageAt");

    if (!IsValide) return NULL;
    if (!IsOpen) OpenCodecAndFile();

    // Load a video frame
    QImage *LoadedImage=NULL;

    if ((SoundTrackBloc)&&(SoundTrackBloc->NbrPacketForFPS)&&(SoundTrackBloc->List.count()<SoundTrackBloc->NbrPacketForFPS)) {
        ReadAudioFrame(PreviewMode,Position+StartPosToAdd,SoundTrackBloc,Volume,DontUseEndPos);
    }

    if ((!MusicOnly)&&(!ForceSoundOnly)) {
        LoadedImage=ReadVideoFrame(Position+StartPosToAdd,DontUseEndPos);
        if (LoadedImage) {

            // If preview mode and image size > PreviewMaxHeight, reduce Cache Image
            if ((PreviewMode)&&(ImageHeight>PREVIEWMAXHEIGHT)) {
                QImage *NewImage=new QImage(LoadedImage->scaledToHeight(PREVIEWMAXHEIGHT));
                delete LoadedImage;
                LoadedImage =NewImage;
            }

            // Scale image if anamorphous
            if (AspectRatio!=1) {
                ImageWidth =int(double(LoadedImage->width())*AspectRatio);
                ImageHeight=LoadedImage->height();
                QImage *NewLoadedImage=new QImage(LoadedImage->scaled(ImageWidth,ImageHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                delete LoadedImage;
                LoadedImage=NewLoadedImage;
            }

            if (Filter && ((!PreviewMode)||(PreviewMode && ApplicationConfig->ApplyTransfoPreview))) Filter->ApplyFilter(LoadedImage);
        }

    }

    return LoadedImage;
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
    if (AudioStreamNumber!=-1) {
        // if file exist then Open video file and get a LibAVFormat context and an associated LibAVCodec decoder
        #if (LIBAVFORMAT_VERSION_MAJOR>=53)
            if (avformat_open_input(&ffmpegAudioFile,FileName.toLocal8Bit(),NULL,NULL)!=0) return false;
        #else
            if (av_open_input_file(&ffmpegAudioFile,FileName.toLocal8Bit(),NULL,0,NULL)!=0) return false;
        #endif

        ffmpegAudioFile->flags|=AVFMT_FLAG_GENPTS;       // Generate missing pts even if it requires parsing future frames.

        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=23))
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
            //ffmpegAudioFile->streams[AudioStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
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

        #if (LIBAVFORMAT_VERSION_MAJOR>53) || ((LIBAVFORMAT_VERSION_MAJOR==53)&&(LIBAVFORMAT_VERSION_MINOR>=23))
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
            //ffmpegVideoFile->streams[VideoStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
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
        qlonglong Position=QTime(0,0,0,0).msecsTo(Duration)/2;
        QImage *Img =ImageAt(true,Position,0,NULL,1,false,NULL,false);
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
        } else {
            CloseCodecAndFile();
            return false;
        }
    }

    return IsOpen;
}
