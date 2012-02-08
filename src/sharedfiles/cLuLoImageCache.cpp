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

//#define DEBUGMODE

//*********************************************************************************************************************************************
// Base object for image cache manipulation
//*********************************************************************************************************************************************
// Constructor for image file

cLuLoImageCacheObject::cLuLoImageCacheObject(QString TheFileName,int TheImageOrientation,QString TheFilterString,bool TheSmoothing,cLuLoImageCache *Parent) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCacheObject::cLuLoImageCacheObject";
    #endif
    FileName            =TheFileName;       // Full filename
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCacheObject::cLuLoImageCacheObject";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCacheObject::~cLuLoImageCacheObject";
    #endif

    if (CacheRenderImage!=NULL) {
        delete CacheRenderImage;
        CacheRenderImage=NULL;
    }
    if (CachePreviewImage!=NULL) {
        delete CachePreviewImage;
        CachePreviewImage=NULL;
    }
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateCacheRenderImage() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCacheObject::ValidateCacheRenderImage";
    #endif

    if (CacheRenderImage==NULL) {

        if (FilterString=="") {

            // Image object
            if (TypeObject==LULOOBJECT_IMAGE) {
                // Load image from disk
                qDebug()<<QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName();
                CacheRenderImage=new QImage(FileName);

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

        } else {

            // Search LuLoImageCache collection to find image without filter
            cLuLoImageCacheObject *UnfilteredObject=LuLoImageCache->FindObject(FileName,ImageOrientation,NULL,Smoothing,true);

            if (UnfilteredObject) {

                // Get a copy of render image without filter
                QImage *UnfilteredImage=UnfilteredObject->ValidateCacheRenderImage();
                if ((UnfilteredImage==NULL)||(UnfilteredImage->isNull()))
                    qDebug()<<"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : UnfilteredObject->ValidateCacheRenderImage() return NULL";
                    else CacheRenderImage=new QImage(UnfilteredImage->copy());

            } else {
                qDebug()<<"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : LuLoImageCache->FindObject return NULL";
            }
        }

        // If image is ok then apply filter if exist
         if ((FilterString!="")&&(CacheRenderImage)&&(!CacheRenderImage->isNull())) {
            cFilterTransformObject Filter(FilterString);
            Filter.ApplyFilter(CacheRenderImage);
        }

        // If error
        if ((CacheRenderImage)&&(CacheRenderImage->isNull())) {
            delete CacheRenderImage;
            CacheRenderImage=NULL;
        }

    }
    LuLoImageCache->FreeMemoryToMaxValue();
    if (CacheRenderImage==NULL) qDebug()<<"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : return NULL";
    return CacheRenderImage;
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateCachePreviewImage() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCacheObject::ValidateCachePreviewImage";
    #endif

    if (CachePreviewImage==NULL) {

        if (FilterString=="") {

            ValidateCacheRenderImage();

            // if CacheRenderImage exist then copy it
            if ((CacheRenderImage)&&(!CacheRenderImage->isNull())) CachePreviewImage=new QImage(CacheRenderImage->copy());

            // If image size>PREVIEWMAXHEIGHT, reduce Cache Image
            if ((CachePreviewImage)&&(!CachePreviewImage->isNull())&&(CachePreviewImage->height()>PREVIEWMAXHEIGHT*2))  {
                QImage *NewImage=new QImage(CachePreviewImage->scaledToHeight(PREVIEWMAXHEIGHT,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                delete CachePreviewImage;
                CachePreviewImage=NewImage;
            }

        } else {

            // Search LuLoImageCache collection to find image without filter
            cLuLoImageCacheObject *UnfilteredObject=LuLoImageCache->FindObject(FileName,ImageOrientation,NULL,Smoothing,true);

            if (UnfilteredObject) {

                if ((UnfilteredObject->CachePreviewImage)&&(!UnfilteredObject->CachePreviewImage->isNull())) {

                    // Get a copy of image without filter
                    CachePreviewImage=new QImage(UnfilteredObject->CachePreviewImage->copy());

                } else {

                    // Get a copy of render image without filter
                    QImage *UnfilteredImage=UnfilteredObject->ValidateCacheRenderImage();
                    if ((UnfilteredImage==NULL)||(UnfilteredImage->isNull()))
                        qDebug()<<"Error in cLuLoImageCacheObject::ValidateCachePreviewImage() : UnfilteredObject->ValidateCacheRenderImage() return NULL";
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
                    Filter.ApplyFilter(CachePreviewImage);
                }

            } else {
                qDebug()<<"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : LuLoImageCache->FindObject return NULL";
            }

        }

        // If error
        if ((CachePreviewImage)&&(CachePreviewImage->isNull())) {
            delete CachePreviewImage;
            CachePreviewImage=NULL;
        }

    }

    LuLoImageCache->FreeMemoryToMaxValue();
    if (CachePreviewImage==NULL) qDebug()<<"Error in cLuLoImageCacheObject::CachePreviewImage() : return NULL";
    return CachePreviewImage;
}

//*********************************************************************************************************************************************
// List object for image cache manipulation
//*********************************************************************************************************************************************

cLuLoImageCache::cLuLoImageCache() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCache::cLuLoImageCache";
    #endif
    MaxValue=1024*1024*1024;
}

//===============================================================================

cLuLoImageCache::~cLuLoImageCache() {
    while (List.count()>0) delete List.takeLast();
}

//===============================================================================
// Find object corresponding to FileName - if object not found then create one
//===============================================================================

// Image version
cLuLoImageCacheObject *cLuLoImageCache::FindObject(QString FileName,int ImageOrientation,cFilterTransformObject *Filter,bool Smoothing,bool SetAtTop) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCache::FindObject";
    #endif

    QString FilterString=(Filter!=NULL)?Filter->FilterToString():"";
    int i=0;
    while ((i<List.count())&&((List[i]->TypeObject!=LULOOBJECT_IMAGE)||(List[i]->FileName!=FileName)||(List[i]->FilterString!=FilterString)||(List[i]->Smoothing!=Smoothing))) i++;

    if ((i<List.count())&&(List[i]->TypeObject==LULOOBJECT_IMAGE)&&(List[i]->FileName==FileName)&&(List[i]->FilterString==FilterString)&&(List[i]->Smoothing==Smoothing)) {
        // if wanted and image found then set it to the top of the list
        if ((SetAtTop)&&(i>0)) { // If item is not the first
            cLuLoImageCacheObject *Object=List.takeAt(i);   // Detach item from the list
            List.prepend(Object);                           // Re-attach item at first position
            i=0;
        }
    } else {
        // Image not found then create it at top of the list
        List.prepend(new cLuLoImageCacheObject(FileName,ImageOrientation,FilterString,Smoothing,this));     // Append a new object at first position
        i=0;
    }
    return List[i]; // return first object
}

// Video version
cLuLoImageCacheObject *cLuLoImageCache::FindObject(cCustomIcon *Video,int Position,bool Smoothing,bool SetAtTop) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCache::FindObject";
    #endif

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

int cLuLoImageCache::MemoryUsed() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCache::MemoryUsed";
    #endif
    int MemUsed=0;
    for (int i=0;i<List.count();i++) {
        if (List[i]->CacheRenderImage)    MemUsed=MemUsed+List[i]->CacheRenderImage->byteCount();
        if (List[i]->CachePreviewImage)   MemUsed=MemUsed+List[i]->CachePreviewImage->byteCount();
    }
    return MemUsed;
}

//===============================================================================

void cLuLoImageCache::FreeMemoryToMaxValue() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cLuLoImageCache::FreeMemoryToMaxValue";
    #endif
    int ToFree=MemoryUsed()-MaxValue;
    int i=List.count()-1;
    while ((ToFree>0)&&(i>0)) {
        if (List[i]->CacheRenderImage) {
            ToFree=ToFree-List[i]->CacheRenderImage->byteCount();
            delete List[i]->CacheRenderImage;
            List[i]->CacheRenderImage=NULL;
        }
        if ((ToFree>0)&&(List[i]->CachePreviewImage)) {
            ToFree=ToFree-List[i]->CachePreviewImage->byteCount();
            delete List[i]->CachePreviewImage;
            List[i]->CachePreviewImage=NULL;
        }
        i--;
    }
}