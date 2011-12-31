/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011 Dominique Levray <levray.dominique@bbox.fr>

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

// Include some additional standard class
#include "cDeviceModelDef.h"
#include "cBaseMediaFile.h"

extern "C" {
    #include <libavutil/opt.h>
    #include <libavutil/pixdesc.h>
    #include <libavutil/avutil.h>
}

//#define DEBUGMODE

//*********************************************************************************************************************************************
// Base class object
//*********************************************************************************************************************************************

cBaseMediaFile::cBaseMediaFile(cBaseApplicationConfig *TheApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBaseMediaFile::cBaseMediaFile";
    #endif
    ObjectType          = OBJECTTYPE_UNMANAGED;
    IsValide            = false;                                    // if true then object if fuly initialise
    ObjectGeometry      = IMAGE_GEOMETRY_UNKNOWN;                   // Image geometry
    FileName            = "";                                       // filename
    ShortName           = "";
    FileSize            = 0;
    FileSizeText        = "";
    ImageWidth          = 0;                                        // Widht of normal image
    ImageHeight         = 0;                                        // Height of normal image
    CreatDateTime       = QDateTime(QDate(0,0,0),QTime(0,0,0));     // Original date/time
    ModifDateTime       = QDateTime(QDate(0,0,0),QTime(0,0,0));     // Last modified date/time
    ModifDateTimeText   = "";
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
    ShortName           =QFileInfo(GivenFileName).fileName();
    FileSize            =QFileInfo(GivenFileName).size();
    FileSizeText        =GetTextSize(FileSize);
    CreatDateTime       =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime       =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    ModifDateTimeText   =ModifDateTime.toString();

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
        if (QMessageBox::question(ApplicationConfig->TopLevelWindow,QApplication::translate("cBaseMediaFile","Open image file"),
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
        if (Values.count()==2) return Values[1];
    }
    return "";
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
        // Compute image geometry
        ObjectGeometry=IMAGE_GEOMETRY_UNKNOWN;

        int RealImageWidth=ImageWidth;

        if (AspectRatio!=1) RealImageWidth=int(double(ImageWidth)/AspectRatio);

        SizeInfo=QString("%1x%2").arg(RealImageWidth).arg(ImageHeight);

        double RatioHW=double(ImageWidth)/double(ImageHeight);
        if ((RatioHW>=1.45)&&(RatioHW<=1.55))           ObjectGeometry=IMAGE_GEOMETRY_3_2;
        else if ((RatioHW>=0.65)&&(RatioHW<=0.67))      ObjectGeometry=IMAGE_GEOMETRY_2_3;
        else if ((RatioHW>=1.32)&&(RatioHW<=1.34))      ObjectGeometry=IMAGE_GEOMETRY_4_3;
        else if ((RatioHW>=0.74)&&(RatioHW<=0.76))      ObjectGeometry=IMAGE_GEOMETRY_3_4;
        else if ((RatioHW>=1.77)&&(RatioHW<=1.79))      ObjectGeometry=IMAGE_GEOMETRY_16_9;
        else if ((RatioHW>=0.56)&&(RatioHW<=0.58))      ObjectGeometry=IMAGE_GEOMETRY_9_16;
        else if ((RatioHW>=2.34)&&(RatioHW<=2.36))      ObjectGeometry=IMAGE_GEOMETRY_40_17;
        else if ((RatioHW>=0.42)&&(RatioHW<=0.44))      ObjectGeometry=IMAGE_GEOMETRY_17_40;

        // now search if size is referenced in DefImageFormat
        for (int i=0;i<2;i++) for (int j=0;j<3;j++) for (int k=0;k<NBR_SIZEDEF;k++) if ((DefImageFormat[i][j][k].Width==RealImageWidth)&&(DefImageFormat[i][j][k].Height==ImageHeight)) {
            FmtInfo=QString(DefImageFormat[i][j][k].Name).left(QString(DefImageFormat[i][j][k].Name).indexOf(" -"));
            break;
        }
        GeoInfo=GetImageGeometryStr();
        if ((FmtInfo=="")&&(MPix>=1)) FmtInfo=QString("%1").arg(MPix,8,'f',1).trimmed()+QApplication::translate("cBaseMediaFile","MPix");
        switch (Fmt) {
            case FULLWEB  : return SizeInfo+" ("+FmtInfo+(FmtInfo!=""?"-":"")+GeoInfo+")";
            case SIZEONLY : return SizeInfo;
            case FMTONLY  : return FmtInfo;
            case GEOONLY  : return GeoInfo;
        }
    }

    return "";
}

//*********************************************************************************************************************************************
// Unmanaged File
//*********************************************************************************************************************************************

cUnmanagedFile::cUnmanagedFile(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cUnmanagedFile::cUnmanagedFile";
    #endif
    Icon        =DefaultFILEIcon;
    ObjectType  =OBJECTTYPE_UNMANAGED;
}

//====================================================================================================================

QString cUnmanagedFile::GetTypeText() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cUnmanagedFile::GetTypeText";
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

//====================================================================================================================

void cUnmanagedFile::GetFullInformationFromFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cUnmanagedFile::GetFullInformationFromFile";
    #endif
    // Nothing to do
}

//*********************************************************************************************************************************************
// Folder
//*********************************************************************************************************************************************

cFolder::cFolder(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFolder::cFolder";
    #endif
    Icon        =DefaultFOLDERIcon;
    ObjectType  =OBJECTTYPE_FOLDER;
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
    ModifDateTimeText   =ModifDateTime.toString();
    return true;
}

//====================================================================================================================

void cFolder::GetFullInformationFromFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFolder::GetFullInformationFromFile";
    #endif
    // Nothing to do
}

//====================================================================================================================

bool cFolder::IsFilteredFile(int) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFolder::IsFilteredFile";
    #endif
    return true;    // always valide
}

//====================================================================================================================

QString cFolder::GetTypeText() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFolder::GetTypeText";
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
    Icon        =DefaultFFDIcon;
    ObjectType  =OBJECTTYPE_FFDFILE;
    Title       ="";
    Author      ="";
    Album       ="";
    Year        =QDate::currentDate().year();
    Comment     ="";
    Composer    ="";
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
    ModifDateTimeText   =ModifDateTime.toString();
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
        if (Element.hasAttribute("Title"))      Title   =Element.attribute("Title");
        if (Element.hasAttribute("Author"))     Author  =Element.attribute("Author");
        if (Element.hasAttribute("Album"))      Album   =Element.attribute("Album");
        if (Element.hasAttribute("Year"))       Year    =Element.attribute("Year").toInt();
        if (Element.hasAttribute("Comment"))    Comment =Element.attribute("Comment");
        if (Element.hasAttribute("Composer"))   Composer=Element.attribute("Composer");
        IsOk=true;
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

    InformationList.append(QString("title")+QString("##")+QString(Title));
    InformationList.append(QString("artist")+QString("##")+QString(Author));
    InformationList.append(QString("album")+QString("##")+QString(Album));
    InformationList.append(QString("date")+QString("##")+QString("%1").arg(Year));
    InformationList.append(QString("comment")+QString("##")+QString(Comment));
    InformationList.append(QString("composer")+QString("##")+QString(Composer));
}

//====================================================================================================================

bool cffDProjectFile::IsFilteredFile(int RequireObjectType) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::IsFilteredFile";
    #endif
    return (RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(RequireObjectType==OBJECTTYPE_FFDFILE);
}

//====================================================================================================================

QString cffDProjectFile::GetTypeText() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cffDProjectFile::GetTypeText";
    #endif
    return QApplication::translate("cBaseMediaFile","ffDiaporama project","File type");
}

//*********************************************************************************************************************************************
// Image file
//*********************************************************************************************************************************************

cImageFile::cImageFile(cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cImageFile::cImageFile";
    #endif
    Icon        =DefaultIMAGEIcon;
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

QString cImageFile::GetTypeText() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cImageFile::GetTypeText";
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
    //Info=GetInformationValue("Image.Artist");           if (Info!="") WEBInfo=WEBInfo+"/"+Info;
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

cVideoFile::cVideoFile(bool IsMusicOnly,cBaseApplicationConfig *ApplicationConfig):cBaseMediaFile(ApplicationConfig) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::cVideoFile";
    #endif
    ObjectType              = OBJECTTYPE_VIDEOFILE;
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

    // Audio part
    ffmpegAudioFile         = NULL;
    AudioDecoderCodec       = NULL;
    LastAudioReadedPosition = -1;
    IsVorbis                = false;
    MusicOnly               = IsMusicOnly;
    AudioCodecInfo          = "";
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

bool cVideoFile::IsFilteredFile(int RequireObjectType) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::IsFilteredFile";
    #endif
    return (RequireObjectType==OBJECTTYPE_UNMANAGED)||(RequireObjectType==OBJECTTYPE_MANAGED)||(!MusicOnly && (RequireObjectType==OBJECTTYPE_VIDEOFILE))||(MusicOnly && (RequireObjectType==OBJECTTYPE_MUSICFILE));
}

//====================================================================================================================

QString cVideoFile::GetTypeText() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::GetTypeText";
    #endif
    if (!MusicOnly) return QApplication::translate("cBaseMediaFile","Video","File type");
        else        return QApplication::translate("cBaseMediaFile","Music","File type");
}

//====================================================================================================================

void cVideoFile::GetFullInformationFromFile() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cVideoFile::GetFullInformationFromFile";
    #endif
    OpenCodecAndFile();
    WEBInfo=GetImageSizeStr();
    CloseCodecAndFile();
    if (Icon.isNull()) {
        if (MusicOnly) Icon=DefaultMUSICIcon; else Icon=DefaultVIDEOIcon;
    }
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
        av_close_input_file(ffmpegVideoFile);
        ffmpegVideoFile=NULL;
    }

    // Close the audio codec
    if (AudioDecoderCodec!=NULL) {
        avcodec_close(ffmpegAudioFile->streams[AudioStreamNumber]->codec);
        AudioDecoderCodec=NULL;
    }
    // Close the audio file
    if (ffmpegAudioFile!=NULL) {
        av_close_input_file(ffmpegAudioFile);
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

    // if file exist then Open video file and get a LibAVFormat context and an associated LibAVCodec decoder
    #if defined(FF_API_FORMAT_PARAMETERS)
        if (avformat_open_input(&ffmpegAudioFile,FileName.toLocal8Bit(),NULL,NULL)!=0) return false;
    #else
        if (av_open_input_file(&ffmpegAudioFile,FileName.toLocal8Bit(),NULL,0,NULL)!=0) return false;
    #endif


    //Reset SourceFile values
    Duration    =QTime(0,0,0,0);
    CodecUsePTS =false;

    // Setup AVFormatContext options
    ffmpegAudioFile->flags|=AVFMT_FLAG_GENPTS;      // Generate missing pts even if it requires parsing future frames.

//      Correct define not yet define
//      #if defined(FF_API_FORMAT_PARAMETERS)
//    if (avformat_find_stream_info(ffmpegAudioFile,NULL)<0) return false;
//    #else
    if (av_find_stream_info(ffmpegAudioFile)<0) return false;
//    #endif

    // Get informations about duration
    int hh,mm,ss,ms;
    ms=ffmpegAudioFile->duration/1000;
    ss=ms/1000;
    mm=ss/60;
    hh=mm/60;

    mm=mm-(hh*60);
    ss=ss-(ss/60)*60;
    ms=ms-(ms/1000)*1000;
    Duration=QTime(hh,mm,ss,ms);
    EndPos  =Duration;    // By default : EndPos is set to the end of file

    InformationList.append(QString("Duration")+QString("##")+Duration.toString("HH:mm:ss"));

    // Find the first audio stream
    AudioStreamNumber=0;
    while ((AudioStreamNumber<(int)ffmpegAudioFile->nb_streams)&&(ffmpegAudioFile->streams[AudioStreamNumber]->codec->codec_type!=AVMEDIA_TYPE_AUDIO)) AudioStreamNumber++;
    if (AudioStreamNumber>=(int)ffmpegAudioFile->nb_streams) {
        //return false;
        AudioStreamNumber=-1;
    } else {
        // Setup STREAM options
        ffmpegAudioFile->streams[AudioStreamNumber]->discard=AVDISCARD_DEFAULT;
        ffmpegAudioFile->flags|=AVFMT_FLAG_GENPTS;      // Generate missing pts even if it requires parsing future frames.

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
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
        ffmpegAudioFile->streams[AudioStreamNumber]->codec->error_concealment=3;

        #if defined(FF_API_AVCODEC_OPEN)
        if ((AudioDecoderCodec==NULL)||(avcodec_open2(ffmpegAudioFile->streams[AudioStreamNumber]->codec,AudioDecoderCodec,NULL)<0)) return false;
        #else
        if ((AudioDecoderCodec==NULL)||(avcodec_open(ffmpegAudioFile->streams[AudioStreamNumber]->codec,AudioDecoderCodec)<0)) return false;
        #endif

        // Check if audio format is AV_SAMPLE_FMT_S16 (the only format supported by ffDiaporama !)
        /*if (ffmpegAudioFile->streams[AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_S16) {
            CloseCodecAndFile();
            return false;
        } else IsOpen=true;*/

        InformationList.append(QString("Audio codec")+QString("##")+QString(AudioDecoderCodec->name));

        if (ffmpegAudioFile->streams[AudioStreamNumber]->codec->channels==1)        InformationList.append(QString("Audio channels")+QString("##")+QApplication::translate("cBaseMediaFile","Mono","Audio channels mode"));
        else if (ffmpegAudioFile->streams[AudioStreamNumber]->codec->channels==2)   InformationList.append(QString("Audio channels")+QString("##")+QApplication::translate("cBaseMediaFile","Stereo","Audio channels mode"));
        else                                                                        InformationList.append(QString("Audio channels")+QString("##")+QString("%1").arg(ffmpegAudioFile->streams[AudioStreamNumber]->codec->channels));

        if (int(ffmpegAudioFile->streams[AudioStreamNumber]->codec->sample_rate/1000)*1000>0) {
            if (int(ffmpegAudioFile->streams[AudioStreamNumber]->codec->sample_rate/1000)*1000==ffmpegAudioFile->streams[AudioStreamNumber]->codec->sample_rate)
                InformationList.append(QString("Audio frequency")+QString("##")+QString("%1").arg(int(ffmpegAudioFile->streams[AudioStreamNumber]->codec->sample_rate/1000))+"Khz");
                else InformationList.append(QString("Audio frequency")+QString("##")+QString("%1").arg(double(ffmpegAudioFile->streams[AudioStreamNumber]->codec->sample_rate)/1000,8,'f',1).trimmed()+"Khz");
        }
        if (int(ffmpegAudioFile->streams[AudioStreamNumber]->codec->bit_rate/1000)>0)
            InformationList.append(QString("Audio bitrate")+QString("##")+QString("%1").arg(int(ffmpegAudioFile->streams[AudioStreamNumber]->codec->bit_rate/1000))+"Kb/s");
    }

    // Find the first video stream
    VideoStreamNumber=0;
    VideoDecoderCodec=NULL;
    if (!MusicOnly) {
        // Reopen file for video
        #if defined(FF_API_FORMAT_PARAMETERS)
        avformat_open_input(&ffmpegVideoFile,FileName.toLocal8Bit(),NULL,NULL);
        #else
        av_open_input_file(&ffmpegVideoFile,FileName.toLocal8Bit(),NULL,0,NULL);
        #endif
        ffmpegVideoFile->flags|=AVFMT_FLAG_GENPTS;      // Generate missing pts even if it requires parsing future frames.

//      Correct define not yet define
//      #if defined(FF_API_FORMAT_PARAMETERS)
//        if (avformat_find_stream_info(ffmpegVideoFile,NULL)<0) return false;
//        #else
        if (av_find_stream_info(ffmpegVideoFile)<0) return false;
//        #endif

        // Get informations about duration
        int hh,mm,ss,ms;
        ms=ffmpegVideoFile->duration/1000;
        ss=ms/1000;
        mm=ss/60;
        hh=mm/60;

        mm=mm-(hh*60);
        ss=ss-(ss/60)*60;
        ms=ms-(ms/1000)*1000;
        Duration=QTime(hh,mm,ss,ms);
        EndPos  =Duration;    // By default : EndPos is set to the end of file

        while ((VideoStreamNumber<(int)ffmpegVideoFile->nb_streams)&&(ffmpegVideoFile->streams[VideoStreamNumber]->codec->codec_type!=AVMEDIA_TYPE_VIDEO)) VideoStreamNumber++;
        if (VideoStreamNumber>=(int)ffmpegVideoFile->nb_streams) return false;

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
        ffmpegVideoFile->streams[VideoStreamNumber]->codec->error_recognition=FF_ER_CAREFUL;        // Error recognization; higher values will detect more errors but may misdetect some more or less valid parts as errors.
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
            if (Icon.isNull()) Icon.addPixmap(QPixmap::fromImage(Img->scaledToHeight(96)));
            delete Img;
        } else {
            CloseCodecAndFile();
            return false;
        }
        InformationList.append(QString("Video codec")+QString("##")+QString(VideoDecoderCodec->name));
        if (ffmpegVideoFile->streams[VideoStreamNumber]->codec->bit_rate>0)
            InformationList.append(QString("Video bitrate")+QString("##")+QString("%1").arg(int(ffmpegVideoFile->streams[VideoStreamNumber]->codec->bit_rate/1000))+"Kb/s");
        if (int(double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.den))>0) {
            if (int(double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.den))==double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.den))
                InformationList.append(QString("Frame rate")+QString("##")+QString("%1").arg(int(double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.den)))+" FPS");
                else InformationList.append(QString("Frame rate")+QString("##")+QString("%1").arg(double(double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.num)/double(ffmpegVideoFile->streams[VideoStreamNumber]->avg_frame_rate.den)),8,'f',3).trimmed()+" FPS");
        }
    }

    // Get metadata
    AVDictionaryEntry *tag     = NULL;
    AVFormatContext   *fmt_ctx = NULL;
    if (ffmpegVideoFile) fmt_ctx=ffmpegVideoFile; else if (ffmpegAudioFile) fmt_ctx=ffmpegAudioFile;
    if (fmt_ctx) while ((tag=av_dict_get(fmt_ctx->metadata,"",tag,AV_DICT_IGNORE_SUFFIX))) InformationList.append(QString(tag->key)+QString("##")+QString(tag->value));

    return IsOpen;
}
