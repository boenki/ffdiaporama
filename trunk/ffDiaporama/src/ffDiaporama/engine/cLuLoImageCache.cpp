/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <domledom@laposte.net>

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
#include <QImageReader>

QMutex MemoryMutex;

//*********************************************************************************************************************************************
// Base object for image cache manipulation
//*********************************************************************************************************************************************
// Constructor for image file

cLuLoImageCacheObject::cLuLoImageCacheObject(qlonglong TheFileKey,QDateTime TheModifDateTime,int TheImageOrientation,QString TheFilterString,bool TheSmoothing,cLuLoImageCache *Parent) {
    FileKey             =TheFileKey;       // Full filename
    ModifDateTime       =TheModifDateTime;
    FilterString        =TheFilterString;
    Smoothing           =TheSmoothing;
    CacheRenderImage    =NULL;
    CachePreviewImage   =NULL;
    ImageOrientation    =TheImageOrientation;                 // Image orientation (EXIF)
    LuLoImageCache      =Parent;
    Position            =0;
    ByteCount           =0;
}

//===============================================================================

cLuLoImageCacheObject::~cLuLoImageCacheObject() {
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

QImage *cLuLoImageCacheObject::ValidateCacheRenderImage() {
    LuLoImageCache->FreeMemoryToMaxValue(this);
    MemoryMutex.lock();
    if (CacheRenderImage==NULL) {
        QString FileName=LuLoImageCache->FilesTable->GetFileName(FileKey);

        // Load image from disk
        ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName());
        if (QFileInfo(FileName).suffix().toLower()=="svg") {
            //qDebug()<<"SVG";
        }
        QImageReader Img(FileName);
        CacheRenderImage=new QImage(Img.read());

        if (!CacheRenderImage)
            ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error allocating memory for render image"));

        if ((CacheRenderImage)&&(CacheRenderImage->isNull())) {
            ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error loading file :")+FileName);
            delete CacheRenderImage;
            CacheRenderImage=NULL;
        }

        // If image is ok then apply exif orientation (if needed)
        if (CacheRenderImage) {
            if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                QMatrix matrix;
                matrix.rotate(-90);
                QImage *NewImage=new QImage(CacheRenderImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                if (NewImage) {
                    if (NewImage->isNull()) delete NewImage; else {
                        delete CacheRenderImage;
                        CacheRenderImage=NewImage;
                    }
                }
            } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                QMatrix matrix;
                matrix.rotate(180);
                QImage *NewImage=new QImage(CacheRenderImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                if (NewImage) {
                    if (NewImage->isNull()) delete NewImage; else {
                        delete CacheRenderImage;
                        CacheRenderImage=NewImage;
                    }
                }
            } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                QMatrix matrix;
                matrix.rotate(90);
                QImage *NewImage=new QImage(CacheRenderImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                if (NewImage) {
                    if (NewImage->isNull()) delete NewImage; else {
                        delete CacheRenderImage;
                        CacheRenderImage=NewImage;
                    }
                }
            }
        }

        // If error
        if ((CacheRenderImage)&&(CacheRenderImage->isNull())) {
            delete CacheRenderImage;
            CacheRenderImage=NULL;
        }

    }
    if (CacheRenderImage==NULL) ToLog(LOGMSG_CRITICAL,"Error in cLuLoImageCacheObject::ValidateCacheRenderImage() : return NULL");
    ByteCount=((CacheRenderImage)?CacheRenderImage->byteCount():0)+(((CachePreviewImage)&&(CachePreviewImage!=CacheRenderImage))?CachePreviewImage->byteCount():0);
    QImage *Ret=(CacheRenderImage)?new QImage(CacheRenderImage->copy()):NULL;
    MemoryMutex.unlock();
    return Ret;
}

//===============================================================================

QImage *cLuLoImageCacheObject::ValidateCachePreviewImage() {
    LuLoImageCache->FreeMemoryToMaxValue(this);
    MemoryMutex.lock();
    if (CachePreviewImage==NULL) {
        // ValidateCacheRenderImage();

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
        } else {
            QString FileName=LuLoImageCache->FilesTable->GetFileName(FileKey);
            ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Loading file :")+QFileInfo(FileName).fileName());
            // if no CacheRenderImage then load image directly at correct size
            QImageReader Img(FileName);
            if (Img.canRead()) {
                QSize Size =Img.size();
                if (((ImageOrientation==8)||(ImageOrientation==6))&&(Size.width()>PREVIEWMAXHEIGHT)) {
                    Size.setHeight((qreal(Size.height())/qreal(Size.width()))*PREVIEWMAXHEIGHT);
                    Size.setWidth(PREVIEWMAXHEIGHT);
                    Img.setScaledSize(Size);
                } else if ((ImageOrientation!=8)&&(ImageOrientation!=6)&&(Size.height()>PREVIEWMAXHEIGHT)) {
                    Size.setWidth((qreal(Size.width())/qreal(Size.height()))*PREVIEWMAXHEIGHT);
                    Size.setHeight(PREVIEWMAXHEIGHT);
                    Img.setScaledSize(Size);
                }
                CachePreviewImage=new QImage(Img.read());
                if ((CachePreviewImage)&&(CachePreviewImage->isNull())) {
                    ToLog(LOGMSG_CRITICAL,QApplication::translate("MainWindow","Error loading file :")+FileName);
                    delete CachePreviewImage;
                    CachePreviewImage=NULL;
                }
                if (CachePreviewImage) {
                    if (ImageOrientation==8) {          // Rotating image anti-clockwise by 90 degrees...'
                        QMatrix matrix;
                        matrix.rotate(-90);
                        QImage *NewImage=new QImage(CachePreviewImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                        if (NewImage) {
                            if (NewImage->isNull()) delete NewImage; else {
                                delete CachePreviewImage;
                                CachePreviewImage=NewImage;
                            }
                        }
                    } else if (ImageOrientation==3) {   // Rotating image clockwise by 180 degrees...'
                        QMatrix matrix;
                        matrix.rotate(180);
                        QImage *NewImage=new QImage(CachePreviewImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                        if (NewImage) {
                            if (NewImage->isNull()) delete NewImage; else {
                                delete CachePreviewImage;
                                CachePreviewImage=NewImage;
                            }
                        }
                    } else if (ImageOrientation==6) {   // Rotating image clockwise by 90 degrees...'
                        QMatrix matrix;
                        matrix.rotate(90);
                        QImage *NewImage=new QImage(CachePreviewImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
                        if (NewImage) {
                            if (NewImage->isNull()) delete NewImage; else {
                                delete CachePreviewImage;
                                CachePreviewImage=NewImage;
                            }
                        }
                    }
                }
            }
        }

        // If error
        if ((CachePreviewImage)&&(CachePreviewImage->isNull())) {
            delete CachePreviewImage;
            CachePreviewImage=NULL;
        }

    }
    if (CachePreviewImage==NULL) ToLog(LOGMSG_CRITICAL,"Error in cLuLoImageCacheObject::CachePreviewImage() : return NULL");
    ByteCount=((CacheRenderImage)?CacheRenderImage->byteCount():0)+(((CachePreviewImage)&&(CachePreviewImage!=CacheRenderImage))?CachePreviewImage->byteCount():0);
    QImage *Ret=(CachePreviewImage)?new QImage(CachePreviewImage->copy()):NULL;
    MemoryMutex.unlock();
    return Ret;
}

//*********************************************************************************************************************************************
// List object for image cache manipulation
//*********************************************************************************************************************************************

cLuLoImageCache::cLuLoImageCache() {
    MaxValue=1024*1024*1024;
    FilesTable=NULL;
}

//===============================================================================

cLuLoImageCache::~cLuLoImageCache() {
    clear();
}

//===============================================================================

void cLuLoImageCache::clear() {
    while (List.count()>0) delete List.takeLast();
}

//===============================================================================
// Find object corresponding to FileName - if object not found then create one
//===============================================================================

// Image version
cLuLoImageCacheObject *cLuLoImageCache::FindObject(qlonglong FileKey,QDateTime ModifDateTime,int ImageOrientation,bool Smoothing,bool SetAtTop) {
    int i=0;
    while ((i<List.count())&&((List[i]->FileKey!=FileKey)||(List[i]->Smoothing!=Smoothing))) i++;

    MemoryMutex.lock();
    if ((i<List.count())&&(List[i]->FileKey==FileKey)&&(List[i]->ModifDateTime==ModifDateTime)&&(List[i]->Smoothing==Smoothing)) {
        // if wanted and image found then set it to the top of the list
        if ((SetAtTop)&&(i>0)) { // If item is not the first
            cLuLoImageCacheObject *Object=List.takeAt(i);   // Detach item from the list
            List.prepend(Object);                           // Re-attach item at first position
            i=0;
        }
    } else {
        // Image not found then create it at top of the list
        List.prepend(new cLuLoImageCacheObject(FileKey,ModifDateTime,ImageOrientation,"",Smoothing,this));     // Append a new object at first position
        i=0;
    }
    MemoryMutex.unlock();
    return List[i]; // return first object
}

//===============================================================================
// Special case for Image object : Remove all image object of this key
void cLuLoImageCache::RemoveImageObject(qlonglong FileKey) {
    MemoryMutex.lock();
    int i=List.count()-1;
    while (i>=0) {
        if (List[i]->FileKey==FileKey) delete List.takeAt(i);
        i--;
    }
    MemoryMutex.unlock();
}

//===============================================================================

int64_t cLuLoImageCache::MemoryUsed() {
    int64_t MemUsed=0;
    for (int i=0;i<List.count();i++) MemUsed+=List[i]->ByteCount;
    return MemUsed;
}

//===============================================================================

void cLuLoImageCache::FreeMemoryToMaxValue(cLuLoImageCacheObject *DontFree) {
    if (DontFree) {
        // Ensure DontFree object is at top of list
        int i=0;
        while ((i<List.count())&&(List[i]!=DontFree)) i++;
        if ((i<List.count())&&(List[i]==DontFree)) {
            cLuLoImageCacheObject *Object=List.takeAt(i);   // Detach item from the list
            List.prepend(Object);                           // Re-attach item at first position
        }
    }
    MemoryMutex.lock();
    // 1st step : ensure used memory is less than max allowed
    int64_t Memory    =MemoryUsed();
    int64_t MaxMemory =MaxValue;
    if (Memory>MaxMemory) {
        QString DisplayLog=QString("Free memory for max value (%1 Mb) : Before=%2 cached objects for %3 Mb").arg(MaxMemory/(1024*1024)).arg(List.count()).arg(MaxMemory/(1024*1024));
        while (((Memory=MemoryUsed())>MaxMemory)&&(List.count()>1)) delete List.takeLast();    // Never delete first object
        ToLog(LOGMSG_INFORMATION,DisplayLog+QString(" - After=%1 cached objects for %2 Mb").arg(List.count()).arg(Memory/(1024*1024)));
    }
    // 2st step : ensure we are able to allocate a 128 Mb block
    void *block=NULL;
    while ((block==NULL)&&(List.count()>1)) {
        block=malloc(128*1024*1024);
        if ((block==NULL)&&(List.count()>1)) {  // Never delete first object
            delete List.takeLast();
            ToLog(LOGMSG_INFORMATION,QString("Free memory to ensure enough space to work - After=%1 cached objects for %2 Mb").arg(List.count()).arg(Memory/(1024*1024)));
        }
    }
    if (block) free(block);
    MemoryMutex.unlock();
}
