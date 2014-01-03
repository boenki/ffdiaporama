/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2014 Dominique Levray <domledom@laposte.net>

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

#ifndef _TRANSITION_H
#define _TRANSITION_H

    // Basic inclusions (common to all files)
    #include "_GlobalDefines.h"
    #include "cBaseApplicationConfig.h"

    #include <QImage>
    #include <QPainter>

    // Transition familly definition
    #define TRANSITIONFAMILLY_BASE              0
    #define TRANSITIONFAMILLY_ZOOMINOUT         1
    #define TRANSITIONFAMILLY_SLIDE             2
    #define TRANSITIONFAMILLY_PUSH              3
    #define TRANSITIONFAMILLY_LUMA_BAR          4
    #define TRANSITIONFAMILLY_LUMA_BOX          5
    #define TRANSITIONFAMILLY_LUMA_CENTER       6
    #define TRANSITIONFAMILLY_LUMA_CHECKER      7
    #define TRANSITIONFAMILLY_LUMA_CLOCK        8
    #define TRANSITIONFAMILLY_LUMA_SNAKE        9
    #define TRANSITIONFAMILLY_DEFORM            10

    // No luma transition : number of sub type
    #define TRANSITIONMAXSUBTYPE_BASE           5
    #define TRANSITIONMAXSUBTYPE_ZOOMINOUT      18
    #define TRANSITIONMAXSUBTYPE_SLIDE          22
    #define TRANSITIONMAXSUBTYPE_PUSH           16
    #define TRANSITIONMAXSUBTYPE_DEFORM         4

    // static local values use to work with luma images
    #define     LUMADLG_HEIGHT  80
    extern int  LUMADLG_WIDTH;

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
        void    SetGeometry(ffd_GEOMETRY Geometry);
    };

    // static global values
    extern  cLumaList       LumaList_Bar;
    extern  cLumaList       LumaList_Box;
    extern  cLumaList       LumaList_Center;
    extern  cLumaList       LumaList_Checker;
    extern  cLumaList       LumaList_Clock;
    extern  cLumaList       LumaList_Snake;
    extern  cIconList       IconList;

    int RegisterNoLumaTransition();
    int RegisterLumaTransition();

    //*********************************************************************************************************************

    void DoTransition(int TransitionFamilly,int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight);

    void Transition_Basic(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight);
    void Transition_Zoom(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight);
    void Transition_Slide(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight);
    void Transition_Push(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight);
    void Transition_Deform(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight);
    void Transition_Luma(cLumaList *LumaList,int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight);

#endif // _TRANSITION_H
