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

#include "_Transition.h"
#include "cBrushDefinition.h"

// static global values
cLumaList       LumaList_Bar;
cLumaList       LumaList_Box;
cLumaList       LumaList_Center;
cLumaList       LumaList_Checker;
cLumaList       LumaList_Clock;
cLumaList       LumaList_Snake;
cIconList       IconList;

// static local values use to speed background image loading (cache)
QImage          *LastLoadedBackgroundImage      =NULL;
QString         LastLoadedBackgroundImageName   ="";

// static local values use to work with luma images
int  LUMADLG_WIDTH=0;

//*********************************************************************************************************************************************
// Global class containing icons of transitions
//*********************************************************************************************************************************************

cIconObject::cIconObject(int TheTransitionFamilly,int TheTransitionSubType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cIconObject::cIconObject");

    TransitionFamilly=TheTransitionFamilly;
    TransitionSubType=TheTransitionSubType;
    QString Familly=QString("%1").arg(TransitionFamilly);   if (Familly.length()<2) Familly="0"+Familly;
    QString SubType=QString("%1").arg(TransitionSubType);   if (SubType.length()<2) SubType="0"+SubType;
    QString FileName=QString(":/img/Transitions")+QDir().separator()+QString("tr-")+Familly+QString("-")+SubType+QString(".png");
    Icon=QImage(FileName);
    if (Icon.isNull()) {
        Icon=QImage(QString(":/img/Transitions")+QDir().separator()+QString("tr-icon-error.png"));
        ToLog(LOGMSG_WARNING,"Icon not found:"+QDir(FileName).absolutePath());
    }
}

//====================================================================================================================

cIconObject::cIconObject(int TheTransitionFamilly,int TheTransitionSubType,QImage LumaImage) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cIconObject::cIconObject");

    TransitionFamilly=TheTransitionFamilly;
    TransitionSubType=TheTransitionSubType;
    Icon=LumaImage.scaled(QSize(32,32),Qt::IgnoreAspectRatio/*,Qt::SmoothTransformation*/);
}

//*********************************************************************************************************************************************
// Global class containing icons library
//*********************************************************************************************************************************************

cIconList::cIconList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cIconList::cIconList");
}

//====================================================================================================================

cIconList::~cIconList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cIconList::~cIconList");
    List.clear();
}

//====================================================================================================================

QImage *cIconList::GetIcon(int TransitionFamilly,int TransitionSubType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cIconList::GetIcon");

    int i=0;
    while ((i<List.count())&&((List[i].TransitionFamilly!=TransitionFamilly)||(List[i].TransitionSubType!=TransitionSubType))) i++;
    if (i<List.count()) return new QImage(List[i].Icon);
        else return new QImage(":/img/Transitions/tr-icon-error.png");
}

//*********************************************************************************************************************************************
// Global class for luma object
//*********************************************************************************************************************************************
cLumaListObject::cLumaListObject(QString FileName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLumaListObject::cLumaListObject");

    OriginalLuma=QImage(FileName);
    DlgLumaImage=QImage(OriginalLuma.scaled(LUMADLG_WIDTH,LUMADLG_HEIGHT,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)).convertToFormat(QImage::Format_ARGB32_Premultiplied);
    Name        =QFileInfo(FileName).baseName();
}

//*********************************************************************************************************************************************
// Global class containing luma library
//*********************************************************************************************************************************************

cLumaList::cLumaList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLumaList::cLumaList");

    Geometry=GEOMETRY_16_9;
    LUMADLG_WIDTH=int((double(LUMADLG_HEIGHT)/double(9))*double(16));
}

//====================================================================================================================

cLumaList::~cLumaList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLumaList::~cLumaList");

    List.clear();
}

//====================================================================================================================

void cLumaList::ScanDisk(QString Path,int TransitionFamilly) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLumaList::ScanDisk");

    QDir                Folder(Path);
    QFileInfoList       Files=Folder.entryInfoList();;

    List.clear();
    for (int i=0;i<Files.count();i++) if (Files[i].isFile() && QString(Files[i].suffix()).toLower()=="png") List.append(cLumaListObject(Files[i].absoluteFilePath()));
    // Sort list by name
    for (int i=0;i<List.count();i++) for (int j=0;j<List.count()-1;j++) if (List[j].Name>List[j+1].Name) List.swap(j,j+1);
    // Register icons for this list
    for (int i=0;i<List.count();i++) IconList.List.append(cIconObject(TransitionFamilly,i,List[i].OriginalLuma));
}

//====================================================================================================================

void cLumaList::SetGeometry(int TheGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cLumaList::SetGeometry");

    if (Geometry==TheGeometry) return;
    Geometry=TheGeometry;
    switch (Geometry) {
    case GEOMETRY_4_3   : LUMADLG_WIDTH=int((double(LUMADLG_HEIGHT)/double(3))*double(4));    break;
    case GEOMETRY_16_9  : LUMADLG_WIDTH=int((double(LUMADLG_HEIGHT)/double(9))*double(16));   break;
    case GEOMETRY_40_17 :
    default             : LUMADLG_WIDTH=int((double(LUMADLG_HEIGHT)/double(17))*double(40));  break;
    }
    for (int i=0;i<List.count();i++)
        List[i].DlgLumaImage=QImage(List[i].OriginalLuma.scaled(LUMADLG_WIDTH,LUMADLG_HEIGHT,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)).convertToFormat(QImage::Format_ARGB32_Premultiplied);
}

//============================================================================================
// Public utility function use to register transitions
//============================================================================================

int RegisterNoLumaTransition() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::RegisterNoLumaTransition");

    for (int i=0;i<TRANSITIONMAXSUBTYPE_BASE;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_BASE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_ZOOMINOUT;i++)  IconList.List.append(cIconObject(TRANSITIONFAMILLY_ZOOMINOUT,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_SLIDE;i++)      IconList.List.append(cIconObject(TRANSITIONFAMILLY_SLIDE,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_PUSH;i++)       IconList.List.append(cIconObject(TRANSITIONFAMILLY_PUSH,i));
    for (int i=0;i<TRANSITIONMAXSUBTYPE_DEFORM;i++)     IconList.List.append(cIconObject(TRANSITIONFAMILLY_DEFORM,i));
    return IconList.List.count();
}

int RegisterLumaTransition() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::RegisterLumaTransition");
    int     PreviousListNumber=IconList.List.count();
    QString Path;

    Path="luma/Bar";        LumaList_Bar.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BAR);
    Path="luma/Box";        LumaList_Box.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_BOX);
    Path="luma/Center";     LumaList_Center.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CENTER);
    Path="luma/Checker";    LumaList_Checker.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CHECKER);
    Path="luma/Clock";      LumaList_Clock.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_CLOCK);
    Path="luma/Snake";      LumaList_Snake.ScanDisk(Path,TRANSITIONFAMILLY_LUMA_SNAKE);
    return IconList.List.count()-PreviousListNumber;
}

//============================================================================================
// Private utility function use to rotate an image
//============================================================================================

QImage RotateImage(double TheRotateXAxis,double TheRotateYAxis,double TheRotateZAxis,QImage *OldImg) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::RotateImage");

    double dw=double(OldImg->width());
    double dh=double(OldImg->height());
    double hyp=sqrt(dw*dw+dh*dh);

    QImage   Img(hyp,hyp,QImage::Format_ARGB32_Premultiplied);
    QPainter Painter;
    Painter.begin(&Img);
    Painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
    Painter.setCompositionMode(QPainter::CompositionMode_Source);
    Painter.fillRect(QRect(0,0,hyp,hyp),Qt::transparent);
    Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // All coordonates from center
    QTransform  Matrix;
    Matrix.translate(hyp/2,hyp/2);
    if (TheRotateZAxis!=0) Matrix.rotate(TheRotateZAxis,Qt::ZAxis);   // Standard axis
    if (TheRotateXAxis!=0) Matrix.rotate(TheRotateXAxis,Qt::XAxis);   // Rotate from X axis
    if (TheRotateYAxis!=0) Matrix.rotate(TheRotateYAxis,Qt::YAxis);   // Rotate from Y axis
    Painter.setWorldTransform(Matrix,false);
    Painter.drawImage(-(dw)/2,-(dh)/2,*OldImg);

    Painter.end();
    return Img;
}

//============================================================================================
//  Basic transition
//      0       No transition
//      1       Dissolve with gradual disappearance of the image A
//      2       Dissolve with no modification of the image A
//      3       Fade to black
//      4       Fade with blur
//============================================================================================

void Transition_Basic(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int,int) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::Transition_Basic");
    QImage  ImgA,ImgB;
    int     DestImageWith  =ImageA->width();
    int     DestImageHeight=ImageA->height();
    int     MaxRA,MaxRB;

    switch (TransitionSubType) {
        case 0:
            WorkingPainter->drawImage(0,0,*ImageB);
            break;
        case 1:     // Dissolve with gradual disappearance of the image A
            WorkingPainter->setOpacity(1-PCT);
            WorkingPainter->drawImage(0,0,*ImageA);
            WorkingPainter->setOpacity(PCT);
            WorkingPainter->drawImage(0,0,*ImageB);
            WorkingPainter->setOpacity(1);
            break;
        case 2:     // Dissolve with no modification of the image A
            //WorkingPainter->setOpacity(1-PCT);
            WorkingPainter->drawImage(0,0,*ImageA);
            WorkingPainter->setOpacity(PCT);
            WorkingPainter->drawImage(0,0,*ImageB);
            WorkingPainter->setOpacity(1);
            break;
        case 3:     // Fade to black
            if (PCT<0.5) {
                WorkingPainter->setOpacity(1-PCT*2);
                WorkingPainter->drawImage(0,0,*ImageA);
            } else {
                WorkingPainter->setOpacity((PCT-0.5)*2);
                WorkingPainter->drawImage(0,0,*ImageB);
            }
            WorkingPainter->setOpacity(1);
            break;
        case 4 :    // Blur
            if (PCT<0.5) {
                ImgA =ImageA->scaledToHeight(DestImageHeight/4);
                MaxRA=ImgA.width()/4; if (MaxRA>ImgA.height()/4) MaxRA=ImgA.height()/4;
                WorkingPainter->drawImage(QRect(0,0,DestImageWith,DestImageHeight),Blitz::blur(ImgA,int(PCT*MaxRA)),QRect(0,0,ImgA.width(),ImgA.height()));
                if (PCT>0.4) {
                    WorkingPainter->setOpacity((PCT-0.4)*5);
                    ImgB =ImageB->scaledToHeight(DestImageHeight/4);
                    MaxRB=ImgB.width()/4; if (MaxRB>ImgB.height()/4) MaxRB=ImgB.height()/4;
                    WorkingPainter->drawImage(QRect(0,0,DestImageWith,DestImageHeight),Blitz::blur(ImgB,int((0.5-(PCT/2))*MaxRB)),QRect(0,0,ImgB.width(),ImgB.height()));
                    WorkingPainter->setOpacity(1);
                }
            } else {
                ImgB=ImageB->scaledToHeight(DestImageHeight/4);
                MaxRB=ImgB.width()/4; if (MaxRB>ImgB.height()/4) MaxRB=ImgB.height()/4;
                WorkingPainter->drawImage(QRect(0,0,DestImageWith,DestImageHeight),Blitz::blur(ImgB,int((0.5-(PCT/2))*MaxRB)),QRect(0,0,ImgB.width(),ImgB.height()));
                if (PCT>0.6) WorkingPainter->setOpacity(1);
            }
            break;
    }
}

//============================================================================================
//  Zoom transition
//      0       ImageA is reduced to Border Left Center
//      1       ImageB is enlarged from Border Left Center
//      2       ImageA is reduced to Border Right Center
//      3       ImageB is enlarged from Border Right Center
//      4       ImageA is reduced to Border Top Center
//      5       ImageB is enlarged from Border Top Center
//      6       ImageA is reduced to Border Bottom Center
//      7       ImageB is enlarged from Border Bottom Center
//      8       ImageA is reduced to Upper Left Corner
//      9       ImageB is enlarged from Upper Left Corner
//      10      ImageA is reduced to Upper Right Corner
//      11      ImageB is enlarged from Upper Right Corner
//      12      ImageA is reduced to Bottom Left Corner
//      13      ImageB is enlarged from Bottom Left Corner
//      14      ImageA is reduced to Bottom Right Corner
//      15      ImageB is enlarged from Bottom Right Corner
//      16      ImageA is reduced to Center
//      17      ImageB is enlarged from Center
//============================================================================================

void Transition_Zoom(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::Transition_Zoom");

    bool    Reverse=(TransitionSubType & 0x1)==1;
    QPoint  box;
    int     wt= int(double(DestImageWith)*(Reverse?(1-PCT):PCT));
    int     ht= int(double(DestImageHeight)*(Reverse?(1-PCT):PCT));

    switch (TransitionSubType) {
        case 0 :
        case 1 : box=QPoint(0,(DestImageHeight-ht)/2);                      break;  // Border Left Center
        case 2 :
        case 3 : box=QPoint(DestImageWith-wt,(DestImageHeight-ht)/2);       break;  // Border Right Center
        case 4 :
        case 5 : box=QPoint((DestImageWith-wt)/2,0);                        break;  // Border Top Center
        case 6 :
        case 7 : box=QPoint((DestImageWith-wt)/2,DestImageHeight-ht);       break;  // Border Bottom Center
        case 8 :
        case 9 : box=QPoint(0,0);                                           break;  // Upper Left Corner
        case 10:
        case 11: box=QPoint(DestImageWith-wt,0);                            break;  // Upper Right Corner
        case 12:
        case 13: box=QPoint(0,DestImageHeight-ht);                          break;  // Bottom Left Corner
        case 14:
        case 15: box=QPoint(DestImageWith-wt,DestImageHeight-ht);           break;  // Bottom Right Corner
        case 16:
        case 17: box=QPoint((DestImageWith-wt)/2,(DestImageHeight-ht)/2);   break;  // Center
    }

    // Draw transformed image
    if (!Reverse) {
        // Old image will desapear progressively during the second half time of the transition
        if (PCT<0.5) WorkingPainter->drawImage(0,0,*ImageA); else {
            WorkingPainter->setOpacity(1-(PCT-0.5)*2);
            WorkingPainter->drawImage(0,0,*ImageA);
            WorkingPainter->setOpacity(1);
        }
    } else {
        // New image will apear immediatly during the old image is moving out
        WorkingPainter->drawImage(0,0,*ImageB);
    }
    WorkingPainter->drawImage(box,(Reverse?ImageA:ImageB)->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
}

//============================================================================================
//  Slide transition
//      0       ImageB appear moving since left to right
//      1       ImageB appear moving since right to left
//      2       ImageB appear moving since up to down
//      3       ImageB appear moving since down to up
//      4       ImageB appear moving since the upper left corner
//      5       ImageB appear moving since the upper right corner
//      6       ImageB appear moving since the lower left corner
//      7       ImageB appear moving since the lower right corner
//      8       ImageA disappear moving from left to right
//      9       ImageA disappear moving from right to left
//      10      ImageA disappear moving from up to down
//      11      ImageA disappear moving from down to up
//      12      ImageA disappear moving from the upper left corner
//      13      ImageA disappear moving from the upper right corner
//      14      ImageA disappear moving from the lower left corner
//      15      ImageA disappear moving from the lower right corner
//      16      ImageB is cut into two and each part moves to the sides left and right
//      17      ImageA is cut into two and each part moves from the sides left and right
//      18      ImageB is cut into two and each part moves to the sides top and bottom
//      19      ImageA is cut into two and each part moves from the sides top and bottom
//      20      ImageB is cut into four and each part moves to the sides left and right and top and bottom
//      21      ImageA is cut into four and each part moves from the sides left and right and top and bottom
//============================================================================================

void Transition_Slide(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::Transition_Slide");

    bool    Reverse=((TransitionSubType<16) &&(TransitionSubType>=8))||
                    ((TransitionSubType>=16)&&((TransitionSubType & 0x1)>0));
    if (Reverse) PCT=(1-PCT);

    QRect   box1,box2,box3,box4,box5,box6,box7,box8;
    int     BoxNum =(TransitionSubType<16)?1:(TransitionSubType<20)?2:3;
    int     PCTW   =int(PCT*double(DestImageWith));
    int     PCTH   =int(PCT*double(DestImageHeight));


    switch (TransitionSubType) {
        case 0 :
        case 8 :    box1=QRect(DestImageWith-PCTW,0,PCTW,DestImageHeight);                      box2=QRect(0,0,PCTW,DestImageHeight);                           break;      // Since left to right
        case 1 :
        case 9 :    box1=QRect(0,0,PCTW,DestImageHeight);                                       box2=QRect(DestImageWith-PCTW,0,PCTW,DestImageHeight);          break;      // Since right to left
        case 2 :
        case 10:    box1=QRect(0,DestImageHeight-PCTH,DestImageWith,PCTH);                      box2=QRect(0,0,DestImageWith,PCTH);                             break;      // Since up to down
        case 3 :
        case 11:    box1=QRect(0,0,DestImageWith,PCTH);                                         box2=QRect(0,DestImageHeight-PCTH,DestImageWith,PCTH);          break;      // Since down to up
        case 4 :
        case 12:    box1=QRect(DestImageWith-PCTW,DestImageHeight-PCTH,PCTW,PCTH);              box2=QRect(0,0,PCTW,PCTH);                                      break;      // Since the upper left corner
        case 5 :
        case 13:    box1=QRect(0,DestImageHeight-PCTH,PCTW,PCTH);                               box2=QRect(DestImageWith-PCTW,0,PCTW,PCTH);                     break;      // Since the upper right corner
        case 6 :
        case 14:    box1=QRect(DestImageWith-PCTW,0,PCTW,PCTH);                                 box2=QRect(0,DestImageHeight-PCTH,PCTW,PCTH);                   break;      // Since the lower left corner
        case 7 :
        case 15:    box1=QRect(0,0,PCTW,PCTH);                                                  box2=QRect(DestImageWith-PCTW,DestImageHeight-PCTH,PCTW,PCTH);  break;      // Since the lower right corner

        // Cut image and slide each part : 2 parts image
        case 16:
        case 17:    PCTW=int(PCT*double(DestImageWith/2));
                    box1=QRect((DestImageWith/2)-PCTW,0,PCTW,DestImageHeight);                  box2=QRect(0,0,PCTW,DestImageHeight);                                       // left part
                    box3=QRect((DestImageWith/2),0,PCTW,DestImageHeight);                       box4=QRect(DestImageWith-PCTW,0,PCTW,DestImageHeight);                      // right part
                    break;      // Since left and right
        case 18:
        case 19:    PCTH=int(PCT*double(DestImageHeight/2));
                    box1=QRect(0,(DestImageHeight/2)-PCTH,DestImageWith,PCTH);                  box2=QRect(0,0,DestImageWith,PCTH);                                         // top part
                    box3=QRect(0,(DestImageHeight/2),DestImageWith,PCTH);                       box4=QRect(0,DestImageHeight-PCTH,DestImageWith,PCTH);                      // bottom part
                    break;      // Since top and bottom

        // Cut image and slide each part : 4 parts image
        case 20:
        case 21:    PCTW=int(PCT*double(DestImageWith/2));
                    PCTH=int(PCT*double(DestImageHeight/2));
                    box1=QRect((DestImageWith/2)-PCTW,(DestImageHeight/2)-PCTH,PCTW,PCTH);      box2=QRect(0,                 0,PCTW,PCTH);                                 // left-top part
                    box3=QRect((DestImageWith/2),     (DestImageHeight/2)-PCTH,PCTW,PCTH);      box4=QRect(DestImageWith-PCTW,0,PCTW,PCTH);                                 // right part

                    box5=QRect((DestImageWith/2)-PCTW,(DestImageHeight/2),     PCTW,PCTH);      box6=QRect(0,                 DestImageHeight-PCTH,PCTW,PCTH);              // left-top part
                    box7=QRect((DestImageWith/2),     (DestImageHeight/2),     PCTW,PCTH);      box8=QRect(DestImageWith-PCTW,DestImageHeight-PCTH,PCTW,PCTH);              // right part
                    break;      // Since top and bottom
    }
    // Draw transformed image
    if (!Reverse) {
        // Old image will desapear progressively during the second half time of the transition
        if (PCT<0.5) WorkingPainter->drawImage(0,0,*ImageA); else {
            WorkingPainter->setOpacity(1-(PCT-0.5)*2);
            WorkingPainter->drawImage(0,0,*ImageA);
            WorkingPainter->setOpacity(1);
        }
    } else {
        // New image will apear immediatly during the old image is moving out
        WorkingPainter->drawImage(0,0,*ImageB);
    }
    WorkingPainter->drawImage(box2,Reverse?*ImageA:*ImageB,box1);
    if (BoxNum>1) WorkingPainter->drawImage(box4,Reverse?*ImageA:*ImageB,box3);
    if (BoxNum>2) {
        WorkingPainter->drawImage(box6,Reverse?*ImageA:*ImageB,box5);
        WorkingPainter->drawImage(box8,Reverse?*ImageA:*ImageB,box7);
    }
}

//============================================================================================
//  Push transition
//      0       ImageB push ImageA Since left to right
//      1       ImageB push ImageA Since right to left
//      2       ImageB push ImageA Since up to down
//      3       ImageB push ImageA Since down to up
//      4       ImageB zoom in from border Left Center   + ImageA zoom out to border Right Center
//      5       ImageB zoom in from border Right Center  + ImageA zoom out to border Left Center
//      6       ImageB zoom in from border Top Center    + ImageA zoom out to border bottom Center
//      7       ImageB zoom in from border bottom Center + ImageA zoom out to border Top Center
//      8       Rotating from y axis
//      9       Rotating from y axis
//      10      Rotating from x axis
//      11      Rotating from x axis
//      12      1/2 Rotating from y axis (flip)
//      13      1/2 Rotating from y axis (flip)
//      14      1/2 Rotating from x axis (flip)
//      15      1/2 Rotating from x axis (flip)
//============================================================================================

void Transition_Push(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::Transition_Push");

    QRect       box1,box2;
    QRect       box3,box4;
    QPoint      box;
    int         wt,ht;
    int         PCTW=int(PCT*double(DestImageWith));
    int         PCTH=int(PCT*double(DestImageHeight));
    int         PCTWB=int((1-PCT)*double(DestImageWith));
    int         PCTHB=int((1-PCT)*double(DestImageHeight));
    double      Rotate,dw,dh;
    QImage      Img;

    switch (TransitionSubType) {
    case 0 :    // Since left to right
        box1=QRect(DestImageWith-PCTW,0,PCTW,DestImageHeight);
        box2=QRect(0,0,PCTW,DestImageHeight);
        box3=QRect(0,0,PCTWB,DestImageHeight);
        box4=QRect(DestImageWith-PCTWB,0,PCTWB,DestImageHeight);
        WorkingPainter->drawImage(box4,*ImageA,box3);
        WorkingPainter->drawImage(box2,*ImageB,box1);
        break;
    case 1 :    // Since right to left
        box1=QRect(0,0,PCTW,DestImageHeight);
        box2=QRect(DestImageWith-PCTW,0,PCTW,DestImageHeight);
        box3=QRect(DestImageWith-PCTWB,0,PCTWB,DestImageHeight);
        box4=QRect(0,0,PCTWB,DestImageHeight);
        WorkingPainter->drawImage(box4,*ImageA,box3);
        WorkingPainter->drawImage(box2,*ImageB,box1);
        break;
    case 2 :    // Since up to down
        box1=QRect(0,DestImageHeight-PCTH,DestImageWith,PCTH);
        box2=QRect(0,0,DestImageWith,PCTH);
        box3=QRect(0,0,DestImageWith,PCTHB);
        box4=QRect(0,DestImageHeight-PCTHB,DestImageWith,PCTHB);
        WorkingPainter->drawImage(box4,*ImageA,box3);
        WorkingPainter->drawImage(box2,*ImageB,box1);
        break;
    case 3 :    // Since down to up
        box1=QRect(0,0,DestImageWith,PCTH);
        box2=QRect(0,DestImageHeight-PCTH,DestImageWith,PCTH);
        box3=QRect(0,DestImageHeight-PCTHB,DestImageWith,PCTHB);
        box4=QRect(0,0,DestImageWith,PCTHB);
        WorkingPainter->drawImage(box4,*ImageA,box3);
        WorkingPainter->drawImage(box2,*ImageB,box1);
        break;
    case 4 :    // Enterring : zoom in from border Left Center - Previous image : zoom out to border Right Center
        wt=int(double(DestImageWith)*(1-PCT));
        ht=int(double(DestImageHeight)*(1-PCT));
        box=QPoint(DestImageWith-wt,(DestImageHeight-ht)/2);
        WorkingPainter->drawImage(box,ImageA->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        wt=int(double(DestImageWith)*PCT);
        ht=int(double(DestImageHeight)*PCT);
        box=QPoint(0,(DestImageHeight-ht)/2);
        WorkingPainter->drawImage(box,ImageB->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        break;
    case 5 :    // Enterring : zoom in from border Right Center - Previous image : zoom out to border Left Center
        wt=int(double(DestImageWith)*(1-PCT));
        ht=int(double(DestImageHeight)*(1-PCT));
        box=QPoint(0,(DestImageHeight-ht)/2);
        WorkingPainter->drawImage(box,ImageA->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        wt=int(double(DestImageWith)*PCT);
        ht=int(double(DestImageHeight)*PCT);
        box=QPoint(DestImageWith-wt,(DestImageHeight-ht)/2);
        WorkingPainter->drawImage(box,ImageB->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        break;
    case 6 :    // Enterring : zoom in from border Top Center - Previous image : zoom out to border bottom Center
        wt=int(double(DestImageWith)*(1-PCT));
        ht=int(double(DestImageHeight)*(1-PCT));
        box=QPoint((DestImageWith-wt)/2,DestImageHeight-ht);
        WorkingPainter->drawImage(box,ImageA->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        wt=int(double(DestImageWith)*PCT);
        ht=int(double(DestImageHeight)*PCT);
        box=QPoint((DestImageWith-wt)/2,0);
        WorkingPainter->drawImage(box,ImageB->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        break;
    case 7 :    // Enterring : zoom in from border bottom Center - Previous image : zoom out to border Top Center
        wt=int(double(DestImageWith)*(1-PCT));
        ht=int(double(DestImageHeight)*(1-PCT));
        box=QPoint((DestImageWith-wt)/2,0);
        WorkingPainter->drawImage(box,ImageA->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        wt=int(double(DestImageWith)*PCT);
        ht=int(double(DestImageHeight)*PCT);
        box=QPoint((DestImageWith-wt)/2,DestImageHeight-ht);
        WorkingPainter->drawImage(box,ImageB->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        break;
    case 8 :    // Rotating from y axis
        if (PCT<0.5) {
            Rotate=double(90)*(PCT*2);
            Img=RotateImage(0,Rotate,0,ImageA);
        } else {
            Rotate=double(-90)*((1-PCT)*2);
            Img=RotateImage(0,Rotate,0,ImageB);
        }
        dw=(double(DestImageWith)-double(Img.width()))/2;
        dh=(double(DestImageHeight)-double(Img.height()))/2;
        WorkingPainter->drawImage(QPointF(dw,dh),Img);
        break;
    case 9 :    // Rotating from y axis
        if (PCT<0.5) {
            Rotate=double(-90)*(PCT*2);
            Img=RotateImage(0,Rotate,0,ImageA);
        } else {
            Rotate=double(90)*((1-PCT)*2);
            Img=RotateImage(0,Rotate,0,ImageB);
        }
        dw=(double(DestImageWith)-double(Img.width()))/2;
        dh=(double(DestImageHeight)-double(Img.height()))/2;
        WorkingPainter->drawImage(QPointF(dw,dh),Img);
        break;
    case 10 :    // Rotating from x axis
        if (PCT<0.5) {
            Rotate=double(90)*(PCT*2);
            Img=RotateImage(Rotate,0,0,ImageA);
        } else {
            Rotate=double(-90)*((1-PCT)*2);
            Img=RotateImage(Rotate,0,0,ImageB);
        }
        dw=(double(DestImageWith)-double(Img.width()))/2;
        dh=(double(DestImageHeight)-double(Img.height()))/2;
        WorkingPainter->drawImage(QPointF(dw,dh),Img);
        break;
    case 11 :    // Rotating from x axis
        if (PCT<0.5) {
            Rotate=double(-90)*(PCT*2);
            Img=RotateImage(Rotate,0,0,ImageA);
        } else {
            Rotate=double(90)*((1-PCT)*2);
            Img=RotateImage(Rotate,0,0,ImageB);
        }
        dw=(double(DestImageWith)-double(Img.width()))/2;
        dh=(double(DestImageHeight)-double(Img.height()))/2;
        WorkingPainter->drawImage(QPointF(dw,dh),Img);
        break;
    case 12 :    // 1/2 Rotating from y axis (flip)
        dw=DestImageWith/2;
        WorkingPainter->drawImage(QRectF(0,0,dw,DestImageHeight),*ImageA,QRectF(0,0,dw,DestImageHeight));
        WorkingPainter->drawImage(QRectF(dw,0,dw,DestImageHeight),*ImageB,QRectF(dw,0,dw,DestImageHeight));
        if (PCT<0.5) {
            Rotate=double(90)*(PCT*2);
            Img=RotateImage(0,Rotate,0,ImageA);
            dw=(double(DestImageWith)-double(Img.width()))/2;
            dh=(double(DestImageHeight)-double(Img.height()))/2;
            WorkingPainter->drawImage(QRectF(DestImageWith/2,dh,Img.width()/2,Img.height()),Img,QRectF(Img.width()/2,0,Img.width()/2,Img.height()));
        } else {
            Rotate=double(-90)*((1-PCT)*2);
            Img=RotateImage(0,Rotate,0,ImageB);
            dw=(double(DestImageWith)-double(Img.width()))/2;
            dh=(double(DestImageHeight)-double(Img.height()))/2;
            WorkingPainter->drawImage(QRectF(dw,dh,Img.width()/2,Img.height()),Img,QRectF(0,0,Img.width()/2,Img.height()));
        }
        break;
    case 13 :    // 1/2 Rotating from y axis (flip)
        dw=DestImageWith/2;
        WorkingPainter->drawImage(QRectF(0,0,dw,DestImageHeight),*ImageB,QRectF(0,0,dw,DestImageHeight));
        WorkingPainter->drawImage(QRectF(dw,0,dw,DestImageHeight),*ImageA,QRectF(dw,0,dw,DestImageHeight));
        if (PCT<0.5) {
            Rotate=double(-90)*(PCT*2);
            Img=RotateImage(0,Rotate,0,ImageA);
            dw=(double(DestImageWith)-double(Img.width()))/2;
            dh=(double(DestImageHeight)-double(Img.height()))/2;
            WorkingPainter->drawImage(QRectF(dw,dh,Img.width()/2,Img.height()),Img,QRectF(0,0,Img.width()/2,Img.height()));
        } else {
            Rotate=double(90)*((1-PCT)*2);
            Img=RotateImage(0,Rotate,0,ImageB);
            dw=(double(DestImageWith)-double(Img.width()))/2;
            dh=(double(DestImageHeight)-double(Img.height()))/2;
            WorkingPainter->drawImage(QRectF(DestImageWith/2,dh,Img.width()/2,Img.height()),Img,QRectF(Img.width()/2,0,Img.width()/2,Img.height()));
        }
        break;
    case 14 :    // 1/2 Rotating from x axis (flip)
        dh=DestImageHeight/2;
        WorkingPainter->drawImage(QRectF(0,0,DestImageWith,dh),*ImageA,QRectF(0,0,DestImageWith,dh));
        WorkingPainter->drawImage(QRectF(0,dh,DestImageWith,dh),*ImageB,QRectF(0,dh,DestImageWith,dh));
        if (PCT<0.5) {
            Rotate=double(90)*(PCT*2);
            Img=RotateImage(Rotate,0,0,ImageA);
            dw=(double(DestImageWith)-double(Img.width()))/2;
            dh=(double(DestImageHeight)-double(Img.height()))/2;
            WorkingPainter->drawImage(QRectF(dw,DestImageHeight/2,Img.width(),Img.height()/2),Img,QRectF(0,Img.height()/2,Img.width(),Img.height()/2));
        } else {
            Rotate=double(-90)*((1-PCT)*2);
            Img=RotateImage(Rotate,0,0,ImageB);
            dw=(double(DestImageWith)-double(Img.width()))/2;
            dh=(double(DestImageHeight)-double(Img.height()))/2;
            WorkingPainter->drawImage(QRectF(dw,dh,Img.width(),Img.height()/2),Img,QRectF(0,0,Img.width(),Img.height()/2));
        }
        break;
    case 15 :    // 1/2 Rotating from x axis (flip)
        dh=DestImageHeight/2;
        WorkingPainter->drawImage(QRectF(0,0,DestImageWith,dh),*ImageB,QRectF(0,0,DestImageWith,dh));
        WorkingPainter->drawImage(QRectF(0,dh,DestImageWith,dh),*ImageA,QRectF(0,dh,DestImageWith,dh));
        if (PCT<0.5) {
            Rotate=double(-90)*(PCT*2);
            Img=RotateImage(Rotate,0,0,ImageA);
            dw=(double(DestImageWith)-double(Img.width()))/2;
            dh=(double(DestImageHeight)-double(Img.height()))/2;
            WorkingPainter->drawImage(QRectF(dw,dh,Img.width(),Img.height()/2),Img,QRectF(0,0,Img.width(),Img.height()/2));
        } else {
            Rotate=double(90)*((1-PCT)*2);
            Img=RotateImage(Rotate,0,0,ImageB);
            dw=(double(DestImageWith)-double(Img.width()))/2;
            dh=(double(DestImageHeight)-double(Img.height()))/2;
            WorkingPainter->drawImage(QRectF(dw,DestImageHeight/2,Img.width(),Img.height()/2),Img,QRectF(0,Img.height()/2,Img.width(),Img.height()/2));
        }
        break;
    }
}

//============================================================================================
//  Deform transition
//      0       Reduces image A by enlarging the image B Since left to right
//      1       Reduces image A by enlarging the image B Since right to left
//      2       Reduces image A by enlarging the image B Since up to down
//      3       Reduces image A by enlarging the image B Since down to up
//============================================================================================

void Transition_Deform(int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::Transition_Deform");

    int         PCTW=int(PCT*double(DestImageWith));
    int         PCTH=int(PCT*double(DestImageHeight));
    int         PCTWB=int((1-PCT)*double(DestImageWith));
    int         PCTHB=int((1-PCT)*double(DestImageHeight));

    switch (TransitionSubType) {
        case 0 :    // Since left to right
            WorkingPainter->drawImage(QRect(PCTW,0,DestImageWith-PCTW,DestImageHeight),*ImageA,QRect(0,0,DestImageWith,DestImageHeight));
            WorkingPainter->drawImage(QRect(0,0,PCTW,DestImageHeight),*ImageB,QRect(0,0,DestImageWith,DestImageHeight));
            break;
        case 1 :    // Since right to left
            WorkingPainter->drawImage(QRect(0,0,PCTWB,DestImageHeight),*ImageA,QRect(0,0,DestImageWith,DestImageHeight));
            WorkingPainter->drawImage(QRect(PCTWB,0,DestImageWith-PCTWB,DestImageHeight),*ImageB,QRect(0,0,DestImageWith,DestImageHeight));
            break;
        case 2 :    // Since up to down
            WorkingPainter->drawImage(QRect(0,PCTH,DestImageWith,DestImageHeight-PCTH),*ImageA,QRect(0,0,DestImageWith,DestImageHeight));
            WorkingPainter->drawImage(QRect(0,0,DestImageWith,PCTH),*ImageB,QRect(0,0,DestImageWith,DestImageHeight));
            break;
        case 3 :    // Since down to up
            WorkingPainter->drawImage(QRect(0,0,DestImageWith,PCTHB),*ImageA,QRect(0,0,DestImageWith,DestImageHeight));
            WorkingPainter->drawImage(QRect(0,PCTHB,DestImageWith,DestImageHeight-PCTHB),*ImageB,QRect(0,0,DestImageWith,DestImageHeight));
            break;
    }
}

//============================================================================================
//  Luma transition
//============================================================================================

void Transition_Luma(cLumaList *LumaList,int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:_Transition::Transition_Luma");

    QImage  Img=ImageB->copy();
    if (TransitionSubType<LumaList->List.count()) {
        // Get a copy of luma image scaled to correct size
        QImage  Luma=((DestImageWith==LUMADLG_WIDTH)&&(DestImageHeight==LUMADLG_HEIGHT))?LumaList->List[TransitionSubType].DlgLumaImage:
                        LumaList->List[TransitionSubType].OriginalLuma.scaled(ImageB->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32_Premultiplied);

        // Apply PCTDone to luma mask
        u_int8_t  limit    =u_int8_t(PCT*double(0xff))+1;
        u_int32_t *LumaData=(u_int32_t *)Luma.bits();
        u_int32_t *ImgData =(u_int32_t *)Img.bits();
        u_int32_t *ImgData2=(u_int32_t *)ImageA->bits();

        for (int i=0;i<DestImageWith*DestImageHeight;i++) {
            if (((*LumaData++)& 0xff)>limit) *ImgData=*ImgData2;
            ImgData++;
            ImgData2++;
        }
    }
    // Draw transformed image
    WorkingPainter->drawImage(0,0,Img);
}

//============================================================================================
// Generic public function to do a transition
//============================================================================================

void DoTransition(int TransitionFamilly,int TransitionSubType,double PCT,QImage *ImageA,QImage *ImageB,QPainter *WorkingPainter,int DestImageWith,int DestImageHeight) {
    switch (TransitionFamilly) {
        case TRANSITIONFAMILLY_BASE        : Transition_Basic(                  TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_ZOOMINOUT   : Transition_Zoom(                   TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_PUSH        : Transition_Push(                   TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_SLIDE       : Transition_Slide(                  TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_DEFORM      : Transition_Deform(                 TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_LUMA_BAR    : Transition_Luma(&LumaList_Bar,     TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_LUMA_BOX    : Transition_Luma(&LumaList_Box,     TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_LUMA_CENTER : Transition_Luma(&LumaList_Center,  TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_LUMA_CHECKER: Transition_Luma(&LumaList_Checker, TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_LUMA_CLOCK  : Transition_Luma(&LumaList_Clock,   TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
        case TRANSITIONFAMILLY_LUMA_SNAKE  : Transition_Luma(&LumaList_Snake,   TransitionSubType,PCT,ImageA,ImageB,WorkingPainter,DestImageWith,DestImageHeight);  break;
    }
}