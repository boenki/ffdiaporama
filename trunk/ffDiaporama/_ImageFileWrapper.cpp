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
    CacheImage          = NULL;     // Cache image for preview mode
    CacheFullImage      = NULL;     // Cache image for Full image mode
    UnfilteredImage     = NULL;     // Cache image (Preview image with no filter)
    ImageOrientation    = 0;        // Image orientation (EXIF)
}

//====================================================================================================================

cimagefilewrapper::~cimagefilewrapper() {
    if (CacheFullImage!=NULL) {
        if (CacheFullImage!=CacheImage) delete CacheFullImage;
        CacheFullImage=NULL;
    }
    if (CacheImage!=NULL) {
        delete CacheImage;
        CacheImage=NULL;
    }
    if (UnfilteredImage!=NULL) {
        delete UnfilteredImage;
        UnfilteredImage=NULL;
    }
}

//====================================================================================================================

bool cimagefilewrapper::GetInformationFromFile(QString GivenFileName) {
    if (CacheFullImage!=NULL) {
        if (CacheFullImage!=CacheImage) delete CacheFullImage;
        CacheFullImage=NULL;
    }
    if (CacheImage!=NULL) {
        delete CacheImage;
        CacheImage=NULL;
    }

    FileName=QFileInfo(GivenFileName).absoluteFilePath();

    bool Continue=true;
    while ((Continue)&&(!QFileInfo(FileName).exists())) {
        if (QMessageBox::question(GlobalMainWindow,QApplication::translate("MainWindow","Open image file"),
            QApplication::translate("MainWindow","Impossible to open file ")+FileName+"\n"+QApplication::translate("MainWindow","Do you want to select another file ?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)!=QMessageBox::Yes) Continue=false; else {

            QString NewFileName=QFileDialog::getOpenFileName(GlobalMainWindow,QApplication::translate("MainWindow","Select another file for ")+QFileInfo(FileName).fileName(),
               GlobalMainWindow->ApplicationConfig->RememberLastDirectories?GlobalMainWindow->ApplicationConfig->LastMediaPath:"",
               GlobalMainWindow->ApplicationConfig->GetFilterForMediaFile(cApplicationConfig::IMAGEFILE));
            if (NewFileName!="") {
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

    if (ImageOrientation==0) CallEXIF();

    return IsValide;
}

bool cimagefilewrapper::CallEXIF() {

    ImageOrientation=1; // Set default image orientation

    QString   Commande;
    QString   Info,Part;
    bool      ExifOK=true;

    // start exiv2
    GlobalMainWindow->StatusBarList.append(QApplication::translate("MainWindow","Analyse file with EXIV2 :")+QFileInfo(FileName).fileName());
    Commande = GlobalMainWindow->ApplicationConfig->PathEXIV2+" print -pa \""+FileName+"\"";
    Commande = AdjustDirForOS(Commande);
    QProcess Process;
    Process.setProcessChannelMode(QProcess::MergedChannels);
    Process.start(Commande);
    if (!Process.waitForStarted()) {
        qDebug()<<"Impossible to start exiv2 - no exif informations will be decode for"<<FileName;
        ExifOK=false;
    }
    if (ExifOK && !Process.waitForFinished()) {
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
    }
    // Restart same job with -pv option to know binary value of orientation
    Commande = GlobalMainWindow->ApplicationConfig->PathEXIV2+" print -pva -g Exif.Image.Orientation \""+FileName+"\"";
    Commande = AdjustDirForOS(Commande);
    Process.start(Commande);
    if (!Process.waitForStarted()) {
        qDebug()<<"Impossible to start exiv2 - no exif informations will be decode for"<<FileName;
        ExifOK=false;
    }
    if (ExifOK && !Process.waitForFinished()) {
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
    return ExifOK;
}

//====================================================================================================================

QImage *cimagefilewrapper::ImageAt(bool PreviewMode,bool ForceLoadDisk,cFilterTransformObject *Filter) {
    if (!IsValide) return NULL;


    // Unfiltered image
    if (PreviewMode && ForceLoadDisk && (!Filter)) {
        // Use cached UnfilteredImage (if exist)
        if (UnfilteredImage!=NULL) return new QImage(UnfilteredImage->copy());
        // else : create UnfilteredImage
        GlobalMainWindow->StatusBarList.append(QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName());
        QImage *LoadedImg=new QImage(FileName);
        if (LoadedImg) {
            UnfilteredImage=new QImage(LoadedImg->scaledToHeight(GlobalMainWindow->ApplicationConfig->PreviewMaxHeight,Qt::SmoothTransformation));
            // If image is ok then apply exif orientation (if needed)
            // A good explanation of the exif orientation tag is available at http://jpegclub.org/exif_orientation.html
            QMatrix matrix;
            if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                  matrix.rotate(-90);
                  QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                  delete UnfilteredImage;
                  UnfilteredImage=NewImage;
            } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                  matrix.rotate(180);
                  QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                  delete UnfilteredImage;
                  UnfilteredImage=NewImage;
            } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                  matrix.rotate(90);
                  QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                  delete UnfilteredImage;
                  UnfilteredImage=NewImage;
            }
            delete LoadedImg;
            return new QImage(UnfilteredImage->copy());
        }
        return NULL;
    }

    // If ForceLoadDisk then ensure CacheImage is null
    if (ForceLoadDisk) {
        if (CacheFullImage!=NULL) {
            if (CacheFullImage!=CacheImage) delete CacheFullImage;
            CacheFullImage=NULL;
        }
        if (CacheImage!=NULL) {
            delete CacheImage;
            CacheImage=NULL;
        }
    }

    // Try to load image from FileName
    if (((!PreviewMode)&&(!CacheFullImage))||((PreviewMode)&&(!CacheImage))) {
        // Make sure we have nothing !
        if (CacheFullImage!=NULL) {
            if (CacheFullImage!=CacheImage) delete CacheFullImage;
            CacheFullImage=NULL;
        }
        if (CacheImage!=NULL) {
            delete CacheImage;
            CacheImage=NULL;
        }
        // Make cache image
        GlobalMainWindow->StatusBarList.append(QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName());
        CacheFullImage=new QImage(FileName);

        if (CacheFullImage->isNull()) {
            // If error then free CacheFullImage
            IsValide=false;
            delete CacheFullImage;
            CacheFullImage=NULL;
        } else {
            // If image is ok then apply exif orientation (if needed)
            // A good explanation of the exif orientation tag is available at http://jpegclub.org/exif_orientation.html
            QMatrix matrix;
            if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                  matrix.rotate(-90);
                  QImage *NewImage=new QImage(CacheFullImage->transformed(matrix));
                  delete CacheFullImage;
                  CacheFullImage=NewImage;
            } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                  matrix.rotate(180);
                  QImage *NewImage=new QImage(CacheFullImage->transformed(matrix));
                  delete CacheFullImage;
                  CacheFullImage=NewImage;
            } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                  matrix.rotate(90);
                  QImage *NewImage=new QImage(CacheFullImage->transformed(matrix));
                  delete CacheFullImage;
                  CacheFullImage=NewImage;
            }
            // if filter then apply filter
            if (Filter && !PreviewMode) Filter->ApplyFilter(CacheFullImage);

            // If preview mode and image size > PreviewMaxHeight, reduce Cache Image
            if (CacheFullImage->height()<=GlobalMainWindow->ApplicationConfig->PreviewMaxHeight*2)  CacheImage=CacheFullImage;
                else CacheImage=new QImage(CacheFullImage->scaledToHeight(GlobalMainWindow->ApplicationConfig->PreviewMaxHeight,Qt::SmoothTransformation));

            if (Filter && PreviewMode) Filter->ApplyFilter(CacheImage);

            // Get preview image size
            ImageHeight=CacheImage->height();
            ImageWidth =CacheImage->width();
        }
    }
    // For memory usage reduction : free CacheFullImage if preview mode
    if ((PreviewMode)&&(CacheFullImage!=NULL)&&(CacheFullImage!=CacheImage)) {
        delete CacheFullImage;
        CacheFullImage=NULL;
    }

    // return wanted image
    if ((PreviewMode)&&(CacheImage))      return new QImage(CacheImage->copy());
    if ((!PreviewMode)&&(CacheFullImage)) return new QImage(CacheFullImage->copy());
    return NULL; // if image is not valide
}
