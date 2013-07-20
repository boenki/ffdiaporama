/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <levray.dominique@bbox.fr>

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

// Specific inclusions
#include "_Model.h"
#include "cBaseApplicationConfig.h"
#include "_Diaporama.h"

//=============================================================================================================================

cModelListItem::cModelListItem(cModelList *Parent,QString FileName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cModelListItem::cModelListItem");
    this->Parent  =Parent;
    this->FileName=FileName;
    Name          =QFileInfo(FileName).baseName();
    IsCustom      =!FileName.startsWith(ModelFolder);

    cDiaporama           *Diaporama=new cDiaporama(Parent->ApplicationConfig,false);
    cDiaporamaObjectInfo *Frame    =new cDiaporamaObjectInfo(NULL,0,Diaporama,1,false);
    Diaporama->ProjectInfo->Title  =QApplication::translate("cModelList","Project title");
    Diaporama->ProjectInfo->Album  =QApplication::translate("cModelList","Project album");
    Diaporama->ProjectInfo->Author =QApplication::translate("cModelList","Project author");
    Diaporama->ProjectInfo->Comment=QApplication::translate("cModelList","Project comment");

    switch (Parent->ModelType) {
        case ffd_MODELTYPE_THUMBNAIL:
            Diaporama->ProjectThumbnail->LoadThumbnail(FileName);
            Frame->CurrentObject                    =Diaporama->ProjectThumbnail;
            Frame->CurrentObject_CurrentShot        =Diaporama->ProjectThumbnail->List[0];
            Frame->CurrentObject_BackgroundBrush    =new QBrush(Qt::black,Qt::SolidPattern);
            Frame->CurrentObject_FreeBackgroundBrush=true;
            Diaporama->PrepareImage(Frame,THUMBWITH,THUMBHEIGHT,true,false,false);
            Thumbnail=Frame->CurrentObject_PreparedImage->copy();
            break;
    }

    delete Frame;
    delete Diaporama;
}

//=============================================================================================================================

cModelListItem::~cModelListItem() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cModelListItem::~cModelListItem");
}

//=============================================================================================================================

cModelList::cModelList(cBaseApplicationConfig *ApplicationConfig,ffd_MODELTYPE ModelType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cModelList::cModelList");
    this->ApplicationConfig=ApplicationConfig;
    FillModelType(ModelType);
}

//=============================================================================================================================

cModelList::~cModelList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cModelList::~cModelList");
}

//=============================================================================================================================

void cModelList::FillModelType(ffd_MODELTYPE ModelType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cModelList::FillModelType");
    this->ModelType=ModelType;
    List.clear();

    StandardModelPath=ModelFolder;
    if (!StandardModelPath.endsWith(QDir::separator())) StandardModelPath=StandardModelPath+QDir::separator();

    CustomModelPath=ApplicationConfig->UserConfigPath;
    if (!CustomModelPath.endsWith(QDir::separator())) CustomModelPath=CustomModelPath+QDir::separator();

    switch (ModelType) {
        case ffd_MODELTYPE_THUMBNAIL:
            ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Register thumbnail models..."));
            StandardModelPath=StandardModelPath+"Thumbnails";
            CustomModelPath  =CustomModelPath+"Thumbnails";
            ModelSuffix ="thb";
            break;
        case ffd_MODELTYPE_TITLESLIDE:
            ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Register title slide models..."));
            StandardModelPath=StandardModelPath+"TitleSlides";
            CustomModelPath  =StandardModelPath+"TitleSlides";
            ModelSuffix ="tss";
            break;
    }

    // Load standard model
    QDir          Folder(StandardModelPath);
    QFileInfoList Files=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Files|QDir::Hidden);
    int           i=0;

    while (i<Files.count()) if ((Files[i].fileName().startsWith("."))||(Files[i].suffix().toLower()!=ModelSuffix)) Files.removeAt(i); else i++;
    for (i=0;i<Files.count();i++) List.append(cModelListItem(this,Files[i].absoluteFilePath()));

    // Load custom model
    Folder=QDir(CustomModelPath);
    if (!Folder.exists()) Folder.mkpath(Folder.absolutePath());
    Files=Folder.entryInfoList(QDir::Dirs|QDir::AllDirs|QDir::Files|QDir::Hidden);
    i=0;
    while (i<Files.count()) if ((Files[i].fileName().startsWith("."))||(Files[i].suffix().toLower()!=ModelSuffix)) Files.removeAt(i); else i++;
    for (i=0;i<Files.count();i++) List.append(cModelListItem(this,Files[i].absoluteFilePath()));
}

//=============================================================================================================================

int cModelList::SearchModel(QString ModelName) {
    int i=0;
    while ((i<List.count())&&(List[i].Name!=ModelName)) i++;
    if ((i<List.count())&&(List[i].Name==ModelName)) return i;
        else return 0;
}
