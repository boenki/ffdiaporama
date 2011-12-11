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

#include <QApplication>
#include <QtDebug>
#include <QFileInfo>
#include "cLuLoImageCache.h"

//*********************************************************************************************************************************************
// Base object for image cache manipulation
//*********************************************************************************************************************************************

cLuLoImageCacheObject::cLuLoImageCacheObject(QString TheFileName,QString TheFilterString,bool TheSmoothing) {
    FileName            =TheFileName;       // Full filename
    FilterString        =TheFilterString;
    Smoothing           =TheSmoothing;
    CacheRenderImage    =NULL;
    CachePreviewImage   =NULL;
    ImageOrientation    =-1;                 // Image orientation (EXIF)
}

//===============================================================================

cLuLoImageCacheObject::~cLuLoImageCacheObject() {
    ClearAll();
}

//===============================================================================

void cLuLoImageCacheObject::ClearAll() {
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

QImage *cLuLoImageCacheObject::ValidateCacheRenderImage(cFilterTransformObject *Filter) {
    if (CacheRenderImage==NULL) {
        qDebug()<<QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName();

        // Load image from disk
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

            // if Filter then apply filter
            if (Filter) Filter->ApplyFilter(CacheRenderImage);
        }

    }
    return CacheRenderImage;
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateCachePreviewImage(int PreviewMaxHeight,cFilterTransformObject *Filter) {
    if (CachePreviewImage==NULL) {

        qDebug()<<QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName();

        // Load image from disk
        CachePreviewImage=new QImage(FileName);

        // If image is ok then apply exif orientation (if needed)
        if ((CachePreviewImage)&&(!CachePreviewImage->isNull())) {
            if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                QMatrix matrix;
                matrix.rotate(-90);
                QImage *NewImage=new QImage(CachePreviewImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                delete CachePreviewImage;
                CachePreviewImage=NewImage;
            } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                QMatrix matrix;
                matrix.rotate(180);
                QImage *NewImage=new QImage(CachePreviewImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                delete CachePreviewImage;
                CachePreviewImage=NewImage;
            } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                QMatrix matrix;
                matrix.rotate(90);
                QImage *NewImage=new QImage(CachePreviewImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                delete CachePreviewImage;
                CachePreviewImage=NewImage;
            }
            // If image size>PreviewMaxHeight, reduce Cache Image
            if (CachePreviewImage->height()>PreviewMaxHeight*2)  {
                QImage *NewImage=new QImage(CachePreviewImage->scaledToHeight(PreviewMaxHeight,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                delete CachePreviewImage;
                CachePreviewImage=NewImage;
            }

            // if Filter then apply filter
            if (Filter) Filter->ApplyFilter(CachePreviewImage);
        }

    }

    return CachePreviewImage;
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateUnfilteredImage(int PreviewMaxHeight) {
    // Load image from disk
    QImage *UnfilteredImage=new QImage(FileName);

    // If image is ok then apply exif orientation (if needed)
    if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
        QMatrix matrix;
        matrix.rotate(-90);
        QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
        delete UnfilteredImage;
        UnfilteredImage=NewImage;
    } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
        QMatrix matrix;
        matrix.rotate(180);
        QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
        delete UnfilteredImage;
        UnfilteredImage=NewImage;
    } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
        QMatrix matrix;
        matrix.rotate(90);
        QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
        delete UnfilteredImage;
        UnfilteredImage=NewImage;
    }

    // Ensure image is at correct height (to speed correction image dialog !)
    if ((UnfilteredImage)&&(!UnfilteredImage->isNull())&&(UnfilteredImage->height()!=PreviewMaxHeight)) {
        QImage *NewImage=new QImage(UnfilteredImage->scaledToHeight(PreviewMaxHeight,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
        delete UnfilteredImage;
        UnfilteredImage=NewImage;
    }

    return UnfilteredImage;
}

//*********************************************************************************************************************************************
// List object for image cache manipulation
//*********************************************************************************************************************************************

cLuLoImageCache::cLuLoImageCache() {
}

//===============================================================================
// Find object corresponding to FileName - if object not found then create one
cLuLoImageCacheObject *cLuLoImageCache::FindObject(QString FileName,cFilterTransformObject *Filter,bool Smoothing,bool SetAtTop) {
    QString FilterString=(Filter!=NULL)?Filter->FilterToString():"";
    int i=0;
    while ((i<List.count())&&((List[i]->FileName!=FileName)||(List[i]->FilterString!=FilterString)||(List[i]->Smoothing!=Smoothing))) i++;

    if ((i<List.count())&&(List[i]->FileName==FileName)&&(List[i]->FilterString==FilterString)&&(List[i]->Smoothing==Smoothing)) {
        // if wanted and image found then set it to the top of the list
        if ((SetAtTop)&&(i>0)) { // If item is not the first
            cLuLoImageCacheObject *Object=List.takeAt(i);   // Detach item from the list
            List.insert(0,Object);                          // Re-attach item at first position
            i=0;
        }
    } else {
        // Image not found then create it at top of the list
        List.insert(0,new cLuLoImageCacheObject(FileName,FilterString,Smoothing));     // Append a new object at first position
        i=0;
    }
    return List[i]; // return first object
}

//===============================================================================

int cLuLoImageCache::MemoryUsed() {
    int MemUsed=0;
    for (int i=0;i<List.count();i++) {
        if (List[i]->CacheRenderImage)    MemUsed=MemUsed+List[i]->CacheRenderImage->byteCount();
        if (List[i]->CachePreviewImage)   MemUsed=MemUsed+List[i]->CachePreviewImage->byteCount();
    }
    return MemUsed;
}

//===============================================================================

void cLuLoImageCache::FreeMemoryToMaxValue(int MaxValue) {
    int ToFree=MemoryUsed()-MaxValue;
    int i=List.count()-1;
    while ((ToFree>0)&&(i>0)) {
        if (List[i]->CacheRenderImage) ToFree=ToFree-List[i]->CacheRenderImage->byteCount();
        if ((ToFree>0)&&(List[i]->CachePreviewImage)) ToFree=ToFree-List[i]->CachePreviewImage->byteCount();
        List[i]->ClearAll();
        i--;
    }
}
