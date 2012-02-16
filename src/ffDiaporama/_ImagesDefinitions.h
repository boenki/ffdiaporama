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

#ifndef IMAGESDEFINITIONS_H
#define IMAGESDEFINITIONS_H

// Basic inclusions (common to all files)
#include "../sharedfiles/_GlobalDefines.h"

// Include some common various class
#include "../sharedfiles/cFilterTransformObject.h"
#include "../sharedfiles/cLuLoImageCache.h"
#include "../sharedfiles/cBaseMediaFile.h"
#include "../sharedfiles/cSoundBlockList.h"
#include "../sharedfiles/cBrushDefinition.h"

//*********************************************************************************************************************************************
// Global class containing icons of transitions
//*********************************************************************************************************************************************

class cIconObject {
public:
    QImage  Icon;                       // The icon
    int     TransitionFamilly;          // Transition familly
    int     TransitionSubType;          // Transition type in the familly

    cIconObject(int TransitionFamilly,int TransitionSubType);
    cIconObject(int TransitionFamilly,int TransitionSubType,QImage LumaImage);
};

//*********************************************************************************************************************************************
// Global class containing icons library
//*********************************************************************************************************************************************

class   cIconList {
public:
    QList<cIconObject>  List;                       // list of icons

    cIconList();
    ~cIconList();

    QImage *GetIcon(int TransitionFamilly,int TransitionSubType);
};

//*********************************************************************************************************************************************
// Global class containing luma library
//*********************************************************************************************************************************************

class   cLumaListObject {
public:
    QImage  OriginalLuma;
    QImage  DlgLumaImage;
    QString Name;

    cLumaListObject(QString FileName);
};

class   cLumaList {
public:
    int                     Geometry;
    QList<cLumaListObject>  List;                       // list of Luma

    cLumaList();
    ~cLumaList();

    void    ScanDisk(QString Path,int TransitionFamilly);
    void    SetGeometry(int Geometry);
};

// static global values
extern  cBackgroundList BackgroundList;                         // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Bar;                           // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Box;                           // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Center;                        // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Checker;                       // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Clock;                         // Real definition in _ImagesDefinitions.cpp
extern  cLumaList       LumaList_Snake;                         // Real definition in _ImagesDefinitions.cpp
extern  cIconList       IconList;                               // Real definition in _ImagesDefinitions.cpp

// static local values use to work with luma images
#define     LUMADLG_HEIGHT  80
extern int  LUMADLG_WIDTH;                                      // Real definition in _ImagesDefinitions.cpp

#endif // IMAGESDEFINITIONS_H
