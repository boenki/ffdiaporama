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
#include "_cLuLoImageCache.h"
#include "_ImageFileWrapper.h"
#include "mainwindow.h"

//*********************************************************************************************************************************************
// Base object for image cache manipulation
//*********************************************************************************************************************************************

cLuLoImageCacheObject::cLuLoImageCacheObject(QString TheFileName) {
    FileName        =TheFileName;   // Full filename
    CacheFullImage  =NULL;
    CacheImage      =NULL;
    UnfilteredImage =NULL;
    ImageOrientation=0;             // Image orientation (EXIF)
}

//===============================================================================

cLuLoImageCacheObject::~cLuLoImageCacheObject() {
    ClearAll();
}

//===============================================================================

void cLuLoImageCacheObject::ClearAll() {
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

//===============================================================================

void cLuLoImageCacheObject::ClearCacheAndCacheFull() {
    if (CacheFullImage!=NULL) {
        if (CacheFullImage!=CacheImage) delete CacheFullImage;
        CacheFullImage=NULL;
    }
    if (CacheImage!=NULL) {
        delete CacheImage;
        CacheImage=NULL;
    }
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateCacheFullImage(cFilterTransformObject *Filter) {
    if (CacheFullImage==NULL) {
        GlobalMainWindow->SetTempStatusText(QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName());

        // Load image from disk
        CacheFullImage=new QImage(FileName);

        // If image is ok then apply exif orientation (if needed)
        if ((CacheFullImage)&&(!CacheFullImage->isNull())) {
            if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                QMatrix matrix;
                matrix.rotate(-90);
                QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                delete UnfilteredImage;
                UnfilteredImage=NewImage;
            } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                QMatrix matrix;
                matrix.rotate(180);
                QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                delete UnfilteredImage;
                UnfilteredImage=NewImage;
            } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                QMatrix matrix;
                matrix.rotate(90);
                QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                delete UnfilteredImage;
                UnfilteredImage=NewImage;
            }
        }

        // if Filter then apply filter
        if (Filter) Filter->ApplyFilter(CacheFullImage);

    }
    return CacheFullImage;
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateCacheImage(cFilterTransformObject *Filter) {
    if (CacheImage==NULL) {

        // Validate CacheFullImage
        if (!CacheFullImage) ValidateCacheFullImage(Filter);

        // If image size>PreviewMaxHeight, reduce Cache Image
        if (CacheFullImage->height()<=GlobalMainWindow->ApplicationConfig->PreviewMaxHeight*2)  CacheImage=CacheFullImage;
            else CacheImage=new QImage(CacheFullImage->scaledToHeight(GlobalMainWindow->ApplicationConfig->PreviewMaxHeight,Qt::SmoothTransformation));

        // if Filter then apply filter
        if (Filter) Filter->ApplyFilter(CacheImage);
    }
    return CacheImage;
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateUnfilteredImage() {
    if (UnfilteredImage==NULL) {
        // Load image from disk
        UnfilteredImage=new QImage(FileName);

        // If image is ok then apply exif orientation (if needed)
        if ((UnfilteredImage)&&(!UnfilteredImage->isNull())) {
            if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                QMatrix matrix;
                matrix.rotate(-90);
                QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                delete UnfilteredImage;
                UnfilteredImage=NewImage;
            } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                QMatrix matrix;
                matrix.rotate(180);
                QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                delete UnfilteredImage;
                UnfilteredImage=NewImage;
            } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                QMatrix matrix;
                matrix.rotate(90);
                QImage *NewImage=new QImage(UnfilteredImage->transformed(matrix));
                delete UnfilteredImage;
                UnfilteredImage=NewImage;
            }
        }

        // Ensure image is at correct height (to speed correction image dialog !)
        if ((UnfilteredImage)&&(!UnfilteredImage->isNull())&&(UnfilteredImage->height()!=GlobalMainWindow->ApplicationConfig->PreviewMaxHeight)) {
            QImage *NewImage=new QImage(UnfilteredImage->scaledToHeight(GlobalMainWindow->ApplicationConfig->PreviewMaxHeight,Qt::SmoothTransformation));
            delete UnfilteredImage;
            UnfilteredImage=NewImage;
        }

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
cLuLoImageCacheObject *cLuLoImageCache::FindObject(QString FileName,bool SetAtTop) {
    int i=0;
    while ((i<List.count())&&(List[i]->FileName!=FileName)) i++;

    if ((i<List.count())&&(List[i]->FileName==FileName)) {
        // if wanted and image found then set it to the top of the list
        if ((SetAtTop)&&(i>0)) { // If item is not the first
            cLuLoImageCacheObject *Object=List.takeAt(i);   // Detach item from the list
            List.insert(0,Object);                          // Re-attach item at first position
            i=0;
        }
    } else {
        // Image not found then create it at top of the list
        List.insert(0,new cLuLoImageCacheObject(FileName));     // Append a new object at first position
        i=0;
    }
    return List[i]; // return first object
}

//===============================================================================

int cLuLoImageCache::MemoryUsed() {
    int MemUsed=0;
    for (int i=0;i<List.count();i++) {
        if (List[i]->CacheFullImage)    MemUsed=MemUsed+List[i]->CacheFullImage->byteCount();
        if (List[i]->CacheImage)        MemUsed=MemUsed+List[i]->CacheImage->byteCount();
        if (List[i]->UnfilteredImage)   MemUsed=MemUsed+List[i]->UnfilteredImage->byteCount();
    }
    return MemUsed;
}

//===============================================================================

void cLuLoImageCache::FreeMemoryToMaxValue(int MaxValue) {
    int ToFree=MemoryUsed()-MaxValue;
    int i=List.count()-1;
    while ((ToFree>0)&&(i>0)) {
        if (List[i]->UnfilteredImage)   ToFree=ToFree-List[i]->UnfilteredImage->byteCount();
        if (List[i]->CacheFullImage)    ToFree=ToFree-List[i]->CacheFullImage->byteCount();
        if (List[i]->CacheImage)        ToFree=ToFree-List[i]->CacheImage->byteCount();
        List[i]->ClearAll();
        i--;
    }
}

//===============================================================================
// Cache image (Preview mode)
QImage *cLuLoImageCache::GetCacheImage(QString FileName) {
    cLuLoImageCacheObject *Object=FindObject(FileName);
    return Object->ValidateCacheImage();
}

//===============================================================================
// Cache image (Full image mode)
QImage *cLuLoImageCache::GetCacheFullImage(QString FileName) {
    cLuLoImageCacheObject *Object=FindObject(FileName);
    return Object->ValidateCacheFullImage();
}

//===============================================================================
// Cache image (Preview image with no filter)
QImage *cLuLoImageCache::GetUnfilteredImage(QString FileName) {
    cLuLoImageCacheObject *Object=FindObject(FileName);
    return Object->ValidateUnfilteredImage();
}
