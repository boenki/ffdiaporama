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

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Specific inclusions
#include "_ImageFileWrapper.h"
#include "mainwindow.h"

//*********************************************************************************************************************************************
// Global class containing image sequences
//*********************************************************************************************************************************************

cimagefilewrapper::cimagefilewrapper() {
    IsValide            = false;                    // if true then object if fuly initialise
    ObjectGeometry      = IMAGE_GEOMETRY_UNKNOWN;
    FileName            = "";                       // filename
    ImageWidth          = 0;                        // Widht of normal image
    ImageHeight         = 0;                        // Height of normal image
}

//====================================================================================================================

cimagefilewrapper::~cimagefilewrapper() {
}

//====================================================================================================================

bool cimagefilewrapper::CallEXIF() {
    cLuLoImageCacheObject *ImageObject=GlobalMainWindow->ImagesCache.FindObject(FileName,&BrushFileTransform,true,false);
    if (!ImageObject) return false;

    //ImageObject->ImageOrientation=1; // Set default image orientation

    QString   Commande;
    QString   Info,Part;
    bool      ExifOK=true;

    // start exiv2
    qDebug()<<QApplication::translate("MainWindow","Analyse file with EXIV2 :")+QFileInfo(FileName).fileName();

    #ifdef Q_OS_WIN
    Commande = "\""+GlobalMainWindow->ApplicationConfig->PathEXIV2+"\" print -pa \""+FileName+"\"";
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    Commande = GlobalMainWindow->ApplicationConfig->PathEXIV2+" print -pa \""+FileName+"\"";
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
                ExivValue.append(Part+"##"+Value);
            }
        }
        ExivValue.append("Exif status##ok");
    }
    Process.terminate();
    Process.close();

    // Restart same job with -pv option to know binary value of orientation
    #ifdef Q_OS_WIN
    Commande = "\""+GlobalMainWindow->ApplicationConfig->PathEXIV2+"\" print -pva \""+FileName+"\"";
    #elif defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
    Commande = GlobalMainWindow->ApplicationConfig->PathEXIV2+" print -pva \""+FileName+"\"";
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
                    ImageObject->ImageOrientation=Value.toInt();
                }
            }
        }
    }
    if (ImageObject->ImageOrientation==-1) ImageObject->ImageOrientation=1;

    Process.terminate();
    Process.close();

    return ExifOK;
}

//====================================================================================================================

bool cimagefilewrapper::GetInformationFromFile(QString GivenFileName,QStringList &AliasList) {
    FileName=QFileInfo(GivenFileName).absoluteFilePath();

    // Use aliaslist
    int i;
    for (i=0;(i<AliasList.count())&&(!AliasList.at(i).startsWith(FileName));i++);
    if ((i<AliasList.count())&&(AliasList.at(i).startsWith(FileName))) {
        FileName=AliasList.at(i);
        if (FileName.indexOf("####")>0) FileName=FileName.mid(FileName.indexOf("####")+QString("####").length());
    }

    bool Continue=true;
    while ((Continue)&&(!QFileInfo(FileName).exists())) {
        if (QMessageBox::question(GlobalMainWindow,QApplication::translate("MainWindow","Open image file"),
            QApplication::translate("MainWindow","Impossible to open file ")+FileName+"\n"+QApplication::translate("MainWindow","Do you want to select another file ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes)
            Continue=false;
        else {
            QString NewFileName=QFileDialog::getOpenFileName(GlobalMainWindow,QApplication::translate("MainWindow","Select another file for ")+QFileInfo(FileName).fileName(),
               GlobalMainWindow->ApplicationConfig->RememberLastDirectories?GlobalMainWindow->ApplicationConfig->LastMediaPath:"",
               GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(IMAGEFILE));
            if (NewFileName!="") {
                AliasList.append(FileName+"####"+NewFileName);
                FileName=NewFileName;
                if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(FileName).absolutePath();     // Keep folder for next use
                GlobalMainWindow->SetModifyFlag(true);
            } else Continue=false;
        }
    }
    if (!Continue) {
        qDebug()<<"Impossible to open file"<<FileName;
        return false;
    }

    CreatDateTime   =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime   =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    IsValide        =true;

    cLuLoImageCacheObject *ImageObject=GlobalMainWindow->ImagesCache.FindObject(FileName,&BrushFileTransform,true,false);
    if (ImageObject) {
        ImageObject->ClearAll();    // Clear all cached images
        if (ImageObject->ImageOrientation==-1) CallEXIF();
    }
    return IsValide;
}

//====================================================================================================================

QImage *cimagefilewrapper::ImageAt(bool PreviewMode,bool ForceLoadDisk,cFilterTransformObject *Filter,bool Smoothing) {
    if (!IsValide) return NULL;

    cLuLoImageCacheObject *ImageObject=GlobalMainWindow->ImagesCache.FindObject(FileName,&BrushFileTransform,Smoothing,true);
    if (!ImageObject) {
        qDebug()<<"ImagesCache.FindObject return NULL !";
        return NULL;  // There is an error !!!!!
    }
    if (ImageObject->ImageOrientation==-1) CallEXIF();

    // Unfiltered image
    if (PreviewMode && ForceLoadDisk && (!Filter)) {
        QImage *UnfilteredImage=ImageObject->ValidateUnfilteredImage(GlobalMainWindow->ApplicationConfig->PreviewMaxHeight);
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

        QImage *CachePreviewImage=ImageObject->ValidateCachePreviewImage(GlobalMainWindow->ApplicationConfig->PreviewMaxHeight,Filter);

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
