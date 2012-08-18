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
#include "cLuLoImageCache.h"
#include "cBaseMediaFile.h"

// Include some additional standard class
#include <QFileInfo>

//*********************************************************************************************************************************************
// Base object for image cache manipulation
//*********************************************************************************************************************************************
// Constructor for image file

cLuLoImageCacheObject::cLuLoImageCacheObject(QString TheFileName,QDateTime TheModifDateTime,int TheImageOrientation,QString TheFilterString,bool TheSmoothing,cLuLoImageCache *Parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCacheObject::cLuLoImageCacheObject");

    FileName            =TheFileName;       // Full filename
    ModifDateTime       =TheModifDateTime;
    FilterString        =TheFilterString;
    Smoothing           =TheSmoothing;
    CacheRenderImage    =NULL;
    CachePreviewImage   =NULL;
    ImageOrientation    =TheImageOrientation;                 // Image orientation (EXIF)
    LuLoImageCache      =Parent;
    Position            =0;
    Video               =NULL;
    TypeObject          =LULOOBJECT_IMAGE;
}

//===============================================================================
// Constructor for video image

cLuLoImageCacheObject::cLuLoImageCacheObject(cCustomIcon *TheVideo,int ThePosition,bool TheSmoothing,cLuLoImageCache *Parent) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCacheObject::cLuLoImageCacheObject");

    FilterString        ="";
    Smoothing           =TheSmoothing;
    CacheRenderImage    =NULL;
    CachePreviewImage   =NULL;
    ImageOrientation    =-1;
    LuLoImageCache      =Parent;
    Position            =ThePosition;
    Video               =TheVideo;
    FileName            =((cVideoFile *)Video)->FileName;
    TypeObject          =LULOOBJECT_VIDEO;
}

//===============================================================================

cLuLoImageCacheObject::~cLuLoImageCacheObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCacheObject::~cLuLoImageCacheObject");

    if (CachePreviewImage!=NULL) {
        if (CachePreviewImage!=CacheRenderImage) delete CachePreviewImage;
        CachePreviewImage=NULL;
    }
    if (CacheRenderImage!=NULL) {
        delete CacheRenderImage;
        CacheRenderImage=NULL;
    }
}

//===============================================================================

QString cLuLoImageCacheObject::CachedFilteredImage() {
    QString CachedFile=FileName;
    return CachedFile.replace("."+QFileInfo(CachedFile).suffix(),"_ffd.jpg");
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateCacheRenderImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCacheObject::ValidateCacheRenderImage");
    LuLoImageCache->FreeMemoryToMaxValue();

    if (CacheRenderImage==NULL) {

        if ((FilterString!="")&&(QFileInfo(CachedFilteredImage()).exists())) {

            ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Loading cached filtered file :")+QFileInfo(CachedFilteredImage()).fileName());
            CacheRenderImage=new QImage(CachedFilteredImage());

            #ifdef Q_OS_WIN
            // On Windows : reduce image size to 8 MPix max
            double  MaxValue=8000000;
            if ((IsWindowsXP)&&(CacheRenderImage!=NULL)&&(!CacheRenderImage->isNull())&&((CacheRenderImage->width()*CacheRenderImage->height())>MaxValue)) {
                double  ActualValue =CacheRenderImage->width()*CacheRenderImage->height();
                double  Transfo     =sqrt(MaxValue/ActualValue);;
                int     ImageWidth  =int(Transfo*double(CacheRenderImage->width()));
                int     ImageHeight =int(Transfo*double(CacheRenderImage->height()));
                ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Rescale image to 8MPix"));
                QImage *NewCacheRenderImage=new QImage(CacheRenderImage->scaled(ImageWidth,ImageHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                delete CacheRenderImage;
                CacheRenderImage=NewCacheRenderImage;
            }
            #endif

            if (!CacheRenderImage)
                ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error allocating memory for cached filtered file"));
            if ((CacheRenderImage)&&(CacheRenderImage->isNull()))
                ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error loading cached filtered file :")+QFileInfo(CachedFilteredImage()).fileName());

        } else if (FilterString=="") {

            // Image object
            if (TypeObject==LULOOBJECT_IMAGE) {
                // Load image from disk
                ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName());
                CacheRenderImage=new QImage(FileName);

                #ifdef Q_OS_WIN
                // On Windows : reduce image size to 8 MPix max
                double  MaxValue=8000000;
                if ((IsWindowsXP)&&(CacheRenderImage!=NULL)&&(!CacheRenderImage->isNull())&&((CacheRenderImage->width()*CacheRenderImage->height())>MaxValue)) {
                    double  ActualValue =CacheRenderImage->width()*CacheRenderImage->height();
                    double  Transfo     =sqrt(MaxValue/ActualValue);;
                    int     ImageWidth  =int(Transfo*double(CacheRenderImage->width()));
                    int     ImageHeight =int(Transfo*double(CacheRenderImage->height()));
                    ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Rescale image to 8MPix"));
                    QImage *NewCacheRenderImage=new QImage(CacheRenderImage->scaled(ImageWidth,ImageHeight,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                    delete CacheRenderImage;
                    CacheRenderImage=NewCacheRenderImage;
                }
                #endif

                if (!CacheRenderImage)
                    ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error allocating memory for render image"));
                if ((CacheRenderImage)&&(CacheRenderImage->isNull()))
                    ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error loading file :")+FileName);

                // If image is ok then apply exif orientation (if needed)
                if ((CacheRenderImage)&&(!CacheRenderImage->isNull())) {
                    if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                        QMatrix matrix;
                        matrix.rotate(-90);
                        QImage *NewImage=new QImage(CacheRenderImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                        delete CacheRenderImage;
                        CacheRenderImage=NewImage;
                    } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                        QMatrix matrix;
                        matrix.rotate(180);
                        QImage *NewImage=new QImage(CacheRenderImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                        delete CacheRenderImage;
                        CacheRenderImage=NewImage;
                    } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                        QMatrix matrix;
                        matrix.rotate(90);
                        QImage *NewImage=new QImage(CacheRenderImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                        delete CacheRenderImage;
                        CacheRenderImage=NewImage;
                    }
                }

            // Video object
            } else {

            }

            // If image is ok then apply filter if exist
            if ((FilterString!="")&&(CacheRenderImage)&&(!CacheRenderImage->isNull())) {
                cFilterTransformObject Filter(FilterString);
                Filter.ApplyFilter(CacheRenderImage);
            }

        } else {

            // Search LuLoImageCache collection to find image without filter
            cLuLoImageCacheObject *UnfilteredObject=LuLoImageCache->FindObject(FileName,ModifDateTime,ImageOrientation,NULL,Smoothing,true);

            if (UnfilteredObject) {

                // Get a copy of render image without filter
                QImage *UnfilteredImage=UnfilteredObject->ValidateCacheRenderImage();
                if ((UnfilteredImage==NULL)||(UnfilteredImage->isNull()))
                    ToLog(LOGMSG_CRITICAL,"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : UnfilteredObject->ValidateCacheRenderImage() return NULL");
                    else CacheRenderImage=new QImage(UnfilteredImage->copy());

            } else {
                ToLog(LOGMSG_CRITICAL,"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : LuLoImageCache->FindObject return NULL");
            }

            // If image is ok then apply filter if exist
             if ((FilterString!="")&&(CacheRenderImage)&&(!CacheRenderImage->isNull())) {
                cFilterTransformObject Filter(FilterString);
                if (CacheRenderImage->format()!=QImage::Format_ARGB32_Premultiplied) {
                    QImage *NewImage=new QImage(CacheRenderImage->convertToFormat(QImage::Format_ARGB32_Premultiplied));
                    delete CacheRenderImage;
                    CacheRenderImage=NewImage;
                }
                Filter.ApplyFilter(CacheRenderImage);
            }
        }

        // If error
        if ((CacheRenderImage)&&(CacheRenderImage->isNull())) {
            delete CacheRenderImage;
            CacheRenderImage=NULL;
        }

    }
    LuLoImageCache->FreeMemoryToMaxValue();
    if (CacheRenderImage==NULL) ToLog(LOGMSG_CRITICAL,"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : return NULL");
    return CacheRenderImage;
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateCachePreviewImage() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCacheObject::ValidateCachePreviewImage");
    LuLoImageCache->FreeMemoryToMaxValue();

    if (CachePreviewImage==NULL) {

        if ((FilterString=="")||((FilterString!="")&&(QFileInfo(CachedFilteredImage()).exists()))) {

            ValidateCacheRenderImage();

            // if CacheRenderImage exist then copy it
            if ((CacheRenderImage)&&(!CacheRenderImage->isNull())) {
                if (CacheRenderImage->height()<=PREVIEWMAXHEIGHT) CachePreviewImage=CacheRenderImage; else {
                    CachePreviewImage=new QImage(CacheRenderImage->copy());
                    // If image size>PREVIEWMAXHEIGHT, reduce Cache Image
                    if ((CachePreviewImage)&&(!CachePreviewImage->isNull())&&(CachePreviewImage->height()>PREVIEWMAXHEIGHT*2))  {
                        QImage *NewImage=new QImage(CachePreviewImage->scaledToHeight(PREVIEWMAXHEIGHT,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                        delete CachePreviewImage;
                        CachePreviewImage=NewImage;
                    }
                }
            }

        } else {

            // Search LuLoImageCache collection to find image without filter
            cLuLoImageCacheObject *UnfilteredObject=LuLoImageCache->FindObject(FileName,ModifDateTime,ImageOrientation,NULL,Smoothing,true);

            if (UnfilteredObject) {

                if ((UnfilteredObject->CachePreviewImage)&&(!UnfilteredObject->CachePreviewImage->isNull())) {

                    // Get a copy of image without filter
                    CachePreviewImage=new QImage(UnfilteredObject->CachePreviewImage->copy());

                } else {

                    // Get a copy of render image without filter
                    QImage *UnfilteredImage=UnfilteredObject->ValidateCacheRenderImage();
                    if ((UnfilteredImage==NULL)||(UnfilteredImage->isNull()))
                        ToLog(LOGMSG_CRITICAL,"Error in cLuLoImageCacheObject::ValidateCachePreviewImage() : UnfilteredObject->ValidateCacheRenderImage() return NULL");
                        else CachePreviewImage=new QImage(UnfilteredImage->copy());

                    // If image size>PREVIEWMAXHEIGHT, reduce Cache Image
                    if ((CachePreviewImage)&&(!CachePreviewImage->isNull())&&(CachePreviewImage->height()>PREVIEWMAXHEIGHT*2))  {
                        QImage *NewImage=new QImage(CachePreviewImage->scaledToHeight(PREVIEWMAXHEIGHT,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                        delete CachePreviewImage;
                        CachePreviewImage=NewImage;
                    }

                }

                // If image is ok then apply then apply filter if exist
                if ((FilterString!="")&&(CachePreviewImage)&&(!CachePreviewImage->isNull())) {
                    cFilterTransformObject Filter(FilterString);
                    if (CachePreviewImage->format()!=QImage::Format_ARGB32_Premultiplied) {
                        QImage *NewImage=new QImage(CachePreviewImage->convertToFormat(QImage::Format_ARGB32_Premultiplied));
                        delete CachePreviewImage;
                        CachePreviewImage=NewImage;
                    }
                    Filter.ApplyFilter(CachePreviewImage);
                }

            } else {
                ToLog(LOGMSG_CRITICAL,"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : LuLoImageCache->FindObject return NULL");
            }

        }

        // If error
        if ((CachePreviewImage)&&(CachePreviewImage->isNull())) {
            delete CachePreviewImage;
            CachePreviewImage=NULL;
        }

    }

    LuLoImageCache->FreeMemoryToMaxValue();
    if (CachePreviewImage==NULL) ToLog(LOGMSG_CRITICAL,"Error in cLuLoImageCacheObject::CachePreviewImage() : return NULL");
    return CachePreviewImage;
}

//*********************************************************************************************************************************************
// List object for image cache manipulation
//*********************************************************************************************************************************************

cLuLoImageCache::cLuLoImageCache() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCache::cLuLoImageCache");

    MaxValue=1024*1024*1024;
}

//===============================================================================

cLuLoImageCache::~cLuLoImageCache() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCache::~cLuLoImageCache");
    while (List.count()>0) delete List.takeLast();
}

//===============================================================================
// Find object corresponding to FileName - if object not found then create one
//===============================================================================

// Image version
cLuLoImageCacheObject *cLuLoImageCache::FindObject(QString FileName,QDateTime ModifDateTime,int ImageOrientation,cFilterTransformObject *Filter,bool Smoothing,bool SetAtTop) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCache::FindObject");


    QString FilterString=(Filter!=NULL)?Filter->FilterToString():"";
    int i=0;
    while ((i<List.count())&&((List[i]->TypeObject!=LULOOBJECT_IMAGE)||(List[i]->FileName!=FileName)||(List[i]->FilterString!=FilterString)||(List[i]->Smoothing!=Smoothing))) i++;

    if ((i<List.count())&&(List[i]->TypeObject==LULOOBJECT_IMAGE)&&(List[i]->FileName==FileName)&&(List[i]->ModifDateTime==ModifDateTime)&&(List[i]->FilterString==FilterString)&&(List[i]->Smoothing==Smoothing)) {
        // if wanted and image found then set it to the top of the list
        if ((SetAtTop)&&(i>0)) { // If item is not the first
            cLuLoImageCacheObject *Object=List.takeAt(i);   // Detach item from the list
            List.prepend(Object);                           // Re-attach item at first position
            i=0;
        }
    } else {
        // Image not found then create it at top of the list
        List.prepend(new cLuLoImageCacheObject(FileName,ModifDateTime,ImageOrientation,FilterString,Smoothing,this));     // Append a new object at first position
        i=0;
    }
    return List[i]; // return first object
}

//===============================================================================

// Video version
cLuLoImageCacheObject *cLuLoImageCache::FindObject(cCustomIcon *Video,int Position,bool Smoothing,bool SetAtTop) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCache::FindObject");

    int i=0;
    while ((i<List.count())&&((List[i]->TypeObject!=LULOOBJECT_VIDEO)||(List[i]->Video!=Video)||(List[i]->Position!=Position)||(List[i]->Smoothing!=Smoothing))) i++;

    if ((i<List.count())&&(List[i]->TypeObject==LULOOBJECT_VIDEO)&&(List[i]->Video==Video)&&(List[i]->Position==Position)&&(List[i]->Smoothing==Smoothing)) {
        // if wanted and image found then set it to the top of the list
        if ((SetAtTop)&&(i>0)) { // If item is not the first
            cLuLoImageCacheObject *Object=List.takeAt(i);   // Detach item from the list
            List.prepend(Object);                           // Re-attach item at first position
            i=0;
        }
    } else {
        // Image not found then create it at top of the list
        List.prepend(new cLuLoImageCacheObject(Video,Position,Smoothing,this));     // Append a new object at first position
        i=0;
    }
    return List[i]; // return first object
}

//===============================================================================
// Special case for video object : Remove all video object  of this name
void cLuLoImageCache::RemoveVideoObject(QString FileName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCache::RemoveVideoObject");

    int i=List.count()-1;
    while (i>=0) {
        if ((List[i]->TypeObject==LULOOBJECT_VIDEO)&&(List[i]->FileName==FileName)) {
            if (List[i]->CachePreviewImage!=List[i]->CacheRenderImage) delete List[i]->CachePreviewImage;
            List[i]->CachePreviewImage=NULL;
            if (List[i]->CacheRenderImage) delete List[i]->CacheRenderImage;
            List[i]->CacheRenderImage=NULL;
            delete List.takeAt(i);
        }
        i--;
    }
}

//===============================================================================
// Special case for Image object : Remove all image object  of this name
void cLuLoImageCache::RemoveImageObject(QString FileName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCache::RemoveImageObject");

    int i=List.count()-1;
    while (i>=0) {
        if ((List[i]->TypeObject==LULOOBJECT_IMAGE)&&(List[i]->FileName==FileName)) {
            if (List[i]->CachePreviewImage!=List[i]->CacheRenderImage) delete List[i]->CachePreviewImage;
            List[i]->CachePreviewImage=NULL;
            if (List[i]->CacheRenderImage) delete List[i]->CacheRenderImage;
            List[i]->CacheRenderImage=NULL;
            delete List.takeAt(i);
        }
        i--;
    }
}

//===============================================================================

qlonglong cLuLoImageCache::MemoryUsed() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCache::MemoryUsed");

    qlonglong MemUsed=0;
    for (int i=0;i<List.count();i++) {
        if (List[i]->CacheRenderImage)                                                              MemUsed=MemUsed+List[i]->CacheRenderImage->byteCount();
        if ((List[i]->CachePreviewImage)&&(List[i]->CachePreviewImage!=List[i]->CacheRenderImage))  MemUsed=MemUsed+List[i]->CachePreviewImage->byteCount();
    }
    return MemUsed;
}

//===============================================================================

void cLuLoImageCache::FreeMemoryToMaxValue() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLuLoImageCache::FreeMemoryToMaxValue");

    // 1st step : ensure used memory is less than max allowed
    qlonglong Memory    =MemoryUsed();
    qlonglong MaxMemory =MaxValue;
    #ifdef Q_OS_WIN
    if (IsWindowsXP) MaxMemory=180*1024*1024;
    #endif
    if (Memory>MaxMemory) {
        QString DisplayLog=QString("Free memory for max value (%1 Mb) : Before=%2 cached objects for %3 Mb").arg(MaxMemory/(1024*1024)).arg(List.count()).arg(MaxMemory/(1024*1024));
        int i=List.count()-1;
        while ((Memory>MaxMemory)&&(i>0)) {
            if ((Memory>MaxMemory)&&(List[i]->CachePreviewImage)) {
                if (List[i]->CachePreviewImage!=List[i]->CacheRenderImage) {
                    Memory=Memory-List[i]->CachePreviewImage->byteCount();
                    delete List[i]->CachePreviewImage;
                }
                List[i]->CachePreviewImage=NULL;
            }
            if (List[i]->CacheRenderImage) {
                Memory=Memory-List[i]->CacheRenderImage->byteCount();
                delete List[i]->CacheRenderImage;
                List[i]->CacheRenderImage=NULL;
            }
            i--;
        }
        while ((List.count()>0)&&(List[List.count()-1]->CachePreviewImage==NULL)&&(List[List.count()-1]->CacheRenderImage==NULL)) delete List.takeLast();
        ToLog(LOGMSG_INFORMATION,DisplayLog+QString(" - After=%1 cached objects for %2 Mb").arg(List.count()).arg(Memory/(1024*1024)));
    }
    // 2st step : ensure we are able to allocate a 128 Mb block
    void *block=NULL;
    while ((block==NULL)&&(List.count()>1)) {
        block=malloc(128*1024*1024);
        if ((block==NULL)&&(List.count()>1)) {
            // Search if we can remove at least one CacheRenderImage
            int i=List.count()-1;
            while ((i>=1)&&(List[i]->CacheRenderImage==NULL)) i--;
            if (i>=1) {
                delete List[i]->CacheRenderImage;
                List[i]->CacheRenderImage=NULL;
            // if no CacheRenderImage was found, then delete latest cached image (render & preview)
            } else delete List.takeLast();
            ToLog(LOGMSG_INFORMATION,QString("Free memory to ensure enough space to work - After=%1 cached objects for %2 Mb").arg(List.count()).arg(Memory/(1024*1024)));
        }
    }
    if (block) free(block);
}
