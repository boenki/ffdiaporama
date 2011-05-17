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
    IsValide            = false;    // if true then object if fuly initialise
    FileName            = "";       // filename
    ImageWidth          = 0;        // Widht of normal image
    ImageHeight         = 0;        // Height of normal image
    CacheImage          = NULL;     // Cache image
    ImageOrientation    = 0;        // Image orientation (EXIF)
}

//====================================================================================================================

cimagefilewrapper::~cimagefilewrapper() {
    if (CacheImage!=NULL) {
        delete CacheImage;
        CacheImage=NULL;
    }
}

//====================================================================================================================

bool cimagefilewrapper::GetInformationFromFile(QString &GivenFileName) {
    FileName    =QFileInfo(GivenFileName).absoluteFilePath();

    bool Continue=true;
    while ((Continue)&&(!QFileInfo(FileName).exists())) {
        if (QMessageBox::question(GlobalMainWindow,QCoreApplication::translate("MainWindow","Open image file"),
            QCoreApplication::translate("MainWindow","Impossible to open file ")+FileName+"\n"+QCoreApplication::translate("MainWindow","Do you want to select another file ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes) Continue=false; else {

            QString NewFileName=QFileDialog::getOpenFileName(GlobalMainWindow,QApplication::translate("MainWindow","Select another file"),
               GlobalMainWindow->ApplicationConfig->RememberLastDirectories?GlobalMainWindow->ApplicationConfig->LastMediaPath:"",
               GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(cApplicationConfig::IMAGEFILE));
            if (NewFileName!="") {
                FileName=NewFileName;
                if (GlobalMainWindow->ApplicationConfig->RememberLastDirectories) GlobalMainWindow->ApplicationConfig->LastMediaPath=QFileInfo(FileName).absolutePath();     // Keep folder for next use
            } else Continue=false;
        }
    }
    if (!Continue) {
        qDebug()<<"Impossible to open file"<<FileName;
        return false;
    }

    QString   Commande;
    QString   Info,Part;
    bool      ExifOK=true;

    CreatDateTime   =QFileInfo(FileName).lastModified();       // Keep date/time file was created by the camera !
    ModifDateTime   =QFileInfo(FileName).created();            // Keep date/time file was created on the computer !
    IsValide        =true;
    ImageOrientation=1; // Set default image orientation

    // start exiv2
    Commande = GlobalMainWindow->ApplicationConfig->PathEXIV2+" print -pa \""+GivenFileName+"\"";
    Commande = AdjustDirForOS(Commande);
    QProcess Process;
    Process.setProcessChannelMode(QProcess::MergedChannels);
    Process.start(Commande);
    if (!Process.waitForStarted()) {
        qDebug()<<"Impossible to start exiv2 - no exif informations will be decode for"<<GivenFileName;
        ExifOK=false;
    }
    if (ExifOK && !Process.waitForFinished()) {
        qDebug()<<"Error during exiv2 process - no exif informations will be decode for"<<GivenFileName;
        ExifOK=false;
    }
    if (ExifOK && (Process.exitStatus()<0)) {
        qDebug()<<"Exiv2 return error"<<Process.exitStatus()<<"- no exif informations will be decode for"<<GivenFileName;
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
            bool    GetValueOrientation;
            if (Part.contains(" ")) {
                Designation=Part.left(Part.indexOf(" "));
                GetValueOrientation=(Designation=="Exif.Image.Orientation");
                while (Designation.contains(".")) Designation=(Designation.mid(Designation.indexOf(".")+QString(".").length())).trimmed();
                Value=(Part.mid(Part.indexOf(" ")+QString(" ").length())).trimmed();
                if (Value.contains(" ")) Value=(Value.mid(Value.indexOf(" ")+QString(" ").length())).trimmed();
                if (Value.contains(" ")) Value=(Value.mid(Value.indexOf(" ")+QString(" ").length())).trimmed();
                if (Part.contains(" ")) Part=Part.left(Part.indexOf(" "));
                if (Part.startsWith("Exif.")) Part=Part.mid(QString("Exif.").length());
                ExivValue.append(Part+"##"+Value);
                if (GetValueOrientation) {
                    if ((Value=="top, left")||(Value==QApplication::translate("MainWindow","top, left","EXIV2 translation")))         ImageOrientation=1;
                    if ((Value=="top, right")||(Value==QApplication::translate("MainWindow","top, right","EXIV2 translation")))       ImageOrientation=2;
                    if ((Value=="bottom, right")||(Value==QApplication::translate("MainWindow","bottom, right","EXIV2 translation"))) ImageOrientation=3;
                    if ((Value=="bottom, left")||(Value==QApplication::translate("MainWindow","bottom, left","EXIV2 translation")))   ImageOrientation=4;
                    if ((Value=="left, top")||(Value==QApplication::translate("MainWindow","left, top","EXIV2 translation")))         ImageOrientation=5;
                    if ((Value=="right, top")||(Value==QApplication::translate("MainWindow","right, top","EXIV2 translation")))       ImageOrientation=6;
                    if ((Value=="right, bottom")||(Value==QApplication::translate("MainWindow","right, bottom","EXIV2 translation"))) ImageOrientation=7;
                    if ((Value=="left, bottom")||(Value==QApplication::translate("MainWindow","left, bottom","EXIV2 translation")))   ImageOrientation=8;
                }
            }
        }
    }

    return IsValide;
}

//====================================================================================================================

QImage *cimagefilewrapper::ImageAt(bool PreviewMode,int PreviewMaxHeight,bool ForceLoadDisk) {
    if (!IsValide) return NULL;

    // If ForceLoadDisk then ensure CacheImage is null
    if ((ForceLoadDisk)&&(CacheImage!=NULL)) {
        delete CacheImage;
        CacheImage=NULL;
    }

    // If image already loaded then return it
    if ((PreviewMode)&&(CacheImage)) return new QImage(CacheImage->copy());

    // Try to load image from FileName
    QImage *Image=new QImage(FileName);
    if (Image->isNull()) {

        // If error then free CacheImage
        IsValide=false;
        delete Image;
        Image=NULL;

    } else {

        // A good explanation of the exif orientation tag is available at http://jpegclub.org/exif_orientation.html
        QMatrix matrix;
        if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
              matrix.rotate(-90);
              QImage *NewImage=new QImage(Image->transformed(matrix));
              delete Image;
              Image=NewImage;
        } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
              matrix.rotate(180);
              QImage *NewImage=new QImage(Image->transformed(matrix));
              delete Image;
              Image=NewImage;
        } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
              matrix.rotate(90);
              QImage *NewImage=new QImage(Image->transformed(matrix));
              delete Image;
              Image=NewImage;
        }

        // If ok, get image height
        ImageHeight=Image->height();

        // If preview mode and image size > PreviewMaxHeight, reduce Cache Image
        if ((PreviewMode)&&(ImageHeight>PreviewMaxHeight)) {
            QImage *NewImage=new QImage(Image->scaledToHeight(PreviewMaxHeight,Qt::SmoothTransformation));
            delete Image;
            Image =NewImage;
            ImageHeight=Image->height();
        }

        // Get image width
        ImageWidth =Image->width();

        if (PreviewMode) CacheImage=new QImage(Image->copy());
    }
    return Image;
}
