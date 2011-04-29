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
#include "_ImagesDefinitions.h"
#include "wgt_QBackgroundWidget.h"

// static global values
cBackgroundList BackgroundList;
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
// Base object for filters transformation image
//*********************************************************************************************************************************************

cFilterTransformObject::cFilterTransformObject() {
    BlurSigma               = 0;
    BlurRadius              = 5;
    OnOffFilter             = 0;
}

//====================================================================================================================

void cFilterTransformObject::ApplyFilter(QImage *Image) {
    if (Image==NULL) return;
    fmt_filters::image img(Image->bits(),Image->width(),Image->height());
    if ((OnOffFilter & FilterDespeckle)==FilterDespeckle)   fmt_filters::despeckle(img);
    if ((OnOffFilter & FilterEqualize)==FilterEqualize)     fmt_filters::equalize(img);
    if ((OnOffFilter & FilterGray)==FilterGray)             fmt_filters::gray(img);
    if (BlurSigma<0)                                        fmt_filters::blur(img,BlurRadius,-BlurSigma);
    if (BlurSigma>0)                                        fmt_filters::sharpen(img,BlurRadius,BlurSigma);
}

//====================================================================================================================

void cFilterTransformObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString /*PathForRelativPath*/) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    // Attribut of the object
    Element.setAttribute("BlurSigma",    BlurSigma);
    Element.setAttribute("BlurRadius",   BlurRadius);
    Element.setAttribute("OnOffFilter",  OnOffFilter);

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cFilterTransformObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString /*PathForRelativPath*/) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        BlurSigma=  Element.attribute("BlurSigma").toDouble();
        BlurRadius= Element.attribute("BlurRadius").toDouble();
        OnOffFilter=Element.attribute("OnOffFilter").toInt();

        return true;
    }
    return false;
}

//*********************************************************************************************************************************************
// Base object for filters correction image
//*********************************************************************************************************************************************

cFilterCorrectObject::cFilterCorrectObject() {
    Brightness  = 0;
    Contrast    = 0;
    Gamma       = 1;
    Red         = 0;
    Green       = 0;
    Blue        = 0;
}

//====================================================================================================================

void cFilterCorrectObject::ApplyFilter(QImage *Image) {
    if (Image==NULL) return;
    fmt_filters::image img(Image->bits(),Image->width(),Image->height());
    if (Brightness!=0)                      fmt_filters::brightness(img,Brightness);
    if (Contrast!=0)                        fmt_filters::contrast(img,Contrast);
    if (Gamma!=1)                           fmt_filters::gamma(img,Gamma);
    if ((Red!=0)||(Green!=0)||(Blue!=0))    fmt_filters::colorize(img,Red,Green,Blue);
}

//====================================================================================================================

void cFilterCorrectObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString /*PathForRelativPath*/) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    // Attribut of the object
    Element.setAttribute("Brightness",  Brightness);
    Element.setAttribute("Contrast",    Contrast);
    Element.setAttribute("Gamma",       Gamma);
    Element.setAttribute("Red",         Red);
    Element.setAttribute("Green",       Green);
    Element.setAttribute("Blue",        Blue);

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cFilterCorrectObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString /*PathForRelativPath*/) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        Brightness= Element.attribute("Brightness").toInt();
        Contrast=   Element.attribute("Contrast").toInt();
        Gamma=      Element.attribute("Gamma").toDouble();
        Red=        Element.attribute("Red").toInt();
        Green=      Element.attribute("Green").toInt();
        Blue=       Element.attribute("Blue").toInt();

        return true;
    }
    return false;
}

//*********************************************************************************************************************************************
// Object for Brush definition
//*********************************************************************************************************************************************

cBrushDefinition::cBrushDefinition() {
    BrushType           =BRUSHTYPE_SOLID;       // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors, 5=brush library, 6=image disk
    PatternType         =Qt::Dense4Pattern;     // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    GradientColors      =3;                     // Number of colors in gradient mode (2 or 3)
    ColorD              ="#ffffff";             // First Color
    ColorF              ="#000000";             // Last Color
    ColorIntermed       ="#777777";             // Intermediate Color
    Intermediate        =0.1;                   // Intermediate position of 2nd color (in %)
    GradientOrientation =6;                     // 1=Up-Left, 2=Up, 3=Up-right, ...
    BrushImage          ="";                    // Image name if brush library or brush disk
}

QBrush *cBrushDefinition::GetBrush(QRectF Rect) {
    switch (BrushType) {
        case BRUSHTYPE_NOBRUSH :        return new QBrush(Qt::NoBrush);
        case BRUSHTYPE_SOLID :          return new QBrush(QColor(ColorD),Qt::SolidPattern);
        case BRUSHTYPE_PATTERN :        return new QBrush(QColor(ColorD),(Qt::BrushStyle)(PatternType+3));
        case BRUSHTYPE_GRADIENT2 :      return GetGradientBrush(Rect);
        case BRUSHTYPE_GRADIENT3 :      return GetGradientBrush(Rect);
        case BRUSHTYPE_IMAGELIBRARY :   return GetLibraryBrush(Rect);
//        case BRUSHTYPE_IMAGEDISK :
    }
    return new QBrush(Qt::NoBrush);
}

QBrush *cBrushDefinition::GetLibraryBrush(QRectF Rect) {
    if ((LastLoadedBackgroundImage!=NULL)&&(LastLoadedBackgroundImageName!=BrushImage)) {
        delete LastLoadedBackgroundImage;
        LastLoadedBackgroundImage=NULL;
        LastLoadedBackgroundImageName="";
    }
    if (LastLoadedBackgroundImage==NULL) {
        for (int j=0;j<BackgroundList.List.count();j++) if (BrushImage==BackgroundList.List[j].Name) {
            LastLoadedBackgroundImageName=BrushImage;
            LastLoadedBackgroundImage    =new QImage(BackgroundList.List[j].FilePath);
        }
    }
    if (LastLoadedBackgroundImage!=NULL) {
        QImage  NewImg1=QImage(LastLoadedBackgroundImage->scaledToHeight(Rect.height()+1));
        int W=NewImg1.width();
        int H=GetHeightForWidth(W,Rect);
        if (H<NewImg1.height()) {
            H=NewImg1.height();
            W=GetWidthForHeight(H,Rect);
        }
        if ((W!=NewImg1.width())||(H!=NewImg1.height())) {
            QImage  NewImg2=QImage(NewImg1.copy(0,0,W,H));
            return new QBrush(NewImg2);
        } else return new QBrush(NewImg1);
    } else return new QBrush(Qt::NoBrush);
}

QBrush *cBrushDefinition::GetGradientBrush(QRectF Rect) {
    QGradient Gradient;
    switch (GradientOrientation) {
        case 0: Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()),QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()));                     break;          // Up-Left
        case 1: Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()),QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()));    break;          // Up
        case 2: Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()),QPointF(Rect.x(),Rect.y()+Rect.height()));                     break;          // Up-right
        case 3: Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()+Rect.height()/2),QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()/2));   break;          // Left
        case 4: Gradient=QRadialGradient(
                    QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()/2),
                    Rect.width()>Rect.height()?Rect.width():Rect.height(),
                    QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()/2)
                );  break;                                                                                                                                      // Radial
        case 5: Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()/2),QPointF(Rect.x(),Rect.y()+Rect.height()/2));   break;          // Right
        case 6: Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()+Rect.height()),QPointF(Rect.x()+Rect.width(),Rect.y()));                     break;          // bt-Left
        case 7: Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()),QPointF(Rect.x()+Rect.width()/2,Rect.y()));    break;          // bottom
        case 8: Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()),QPointF(Rect.x(),Rect.y()));                     break;          // bt-right
    }
    Gradient.setColorAt(0,QColor(ColorD));
    Gradient.setColorAt(1,QColor(ColorF));
    if (BrushType==BRUSHTYPE_GRADIENT3) Gradient.setColorAt(Intermediate,QColor(ColorIntermed));
    return new QBrush(Gradient);
}

// Return height for width depending on Rect geometry
int cBrushDefinition::GetHeightForWidth(int WantedWith,QRectF Rect) {
    double   Ratio=Rect.width()/Rect.height();
    return int(double(double(WantedWith)/Ratio));
}

// Return width for height depending on Rect geometry
int cBrushDefinition::GetWidthForHeight(int WantedHeight,QRectF Rect) {
    double   Ratio=Rect.height()/Rect.width();
    return int(double(double(WantedHeight)/Ratio));
}

//====================================================================================================================

void cBrushDefinition::SaveToXML(QDomElement &domDocument,QString ElementName,QString /*PathForRelativPath*/) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    // Attribut of the object
    Element.setAttribute("BrushType",BrushType);                        // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
    Element.setAttribute("PatternType",PatternType);                    // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    Element.setAttribute("GradientColors",GradientColors);              // Number of colors in gradient mode (2 or 3)
    Element.setAttribute("ColorD",ColorD);                              // First Color
    Element.setAttribute("ColorF",ColorF);                              // Last Color
    Element.setAttribute("ColorIntermed",ColorIntermed);                // Intermediate Color
    Element.setAttribute("Intermediate",Intermediate);                  // Intermediate position of 2nd color (in %)
    Element.setAttribute("GradientOrientation",GradientOrientation);    // 0=Radial, 1=Up-Left, 2=Up, 3=Up-right, 4=Right, 5=bt-right, 6=bottom, 7=bt-Left, 8=Left
    Element.setAttribute("BrushImage",BrushImage);                      // Image name if brush library or brush disk

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cBrushDefinition::LoadFromXML(QDomElement domDocument,QString ElementName,QString /*PathForRelativPath*/) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        // Attribut of the object
        BrushType=          Element.attribute("BrushType").toInt();             // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
        PatternType=        Element.attribute("PatternType").toInt();           // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
        GradientColors=     Element.attribute("GradientColors").toInt();        // Number of colors in gradient mode (2 or 3)
        ColorD=             Element.attribute("ColorD");                        // First Color
        ColorF=             Element.attribute("ColorF");                        // Last Color
        ColorIntermed=      Element.attribute("ColorIntermed");                 // Intermediate Color
        Intermediate=       Element.attribute("Intermediate").toDouble();       // Intermediate position of 2nd color (in %)
        GradientOrientation=Element.attribute("GradientOrientation").toInt();   // 0=Radial, 1=Up-Left, 2=Up, 3=Up-right, 4=Right, 5=bt-right, 6=bottom, 7=bt-Left, 8=Left
        BrushImage         =Element.attribute("BrushImage");                    // Image name if brush library or brush disk
        return true;
    }
    return false;
}

//*********************************************************************************************************************************************
// Base object for composition definition
//*********************************************************************************************************************************************

cBackgroundObject::cBackgroundObject(QString FileName,int TheGeometry) {
    IsValide    = false;
    FilePath    = QFileInfo(FileName).path()+QDir::separator()+QFileInfo(FileName).baseName()+".jpg";
    Name        = "";
    WebSite     = "";
    Licence     = "";

    // Make Icon
    QImage *BrushImage=new QImage();
    BrushImage->load(AdjustDirForOS(FilePath));
    if (!BrushImage->isNull()) {
        Geometry = TheGeometry;
        int     H,W;
        QImage  *NewImg;
        switch (Geometry) {
            case GEOMETRY_4_3 :
                H=BrushImage->height();
                W=int(double(4)*(double(H)/double(3)));
                NewImg=new QImage(BrushImage->copy((BrushImage->width()-W)/2,(BrushImage->height()-H)/2,W,H));
                delete BrushImage;
                BrushImage=NewImg;
                break;
            case GEOMETRY_40_17 :
                W=BrushImage->width();
                H=int(double(17)*(double(W)/double(40)));
                NewImg=new QImage(BrushImage->copy((BrushImage->width()-W)/2,(BrushImage->height()-H)/2,W,H));
                delete BrushImage;
                BrushImage=NewImg;
                break;
        }

        Icon=QPixmap(QPixmap::fromImage(BrushImage->scaledToHeight(64)));
        delete BrushImage;
    }
    IsValide=!Icon.isNull() && LoadInfo(QFileInfo(FileName).path()+QDir::separator()+QFileInfo(FileName).baseName()+".txt");
}

//====================================================================================================================

bool cBackgroundObject::LoadInfo(QString FileName) {
    QFile   file(AdjustDirForOS(FileName));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString Line=file.readLine();
            Line.replace("\n","");
            if (Name=="")            Name=Line;
            else if (WebSite=="")    WebSite=Line;
            else if (Licence=="")    Licence=Line;
        }
        file.close();
    }
    return (Name!="")&&(WebSite!="")&&(Licence!="");
}

//*********************************************************************************************************************************************
// Global class containing background library
//*********************************************************************************************************************************************
cBackgroundList::cBackgroundList() {
    Geometry=-1;
}

//====================================================================================================================

cBackgroundList::~cBackgroundList() {
    List.clear();
}

//====================================================================================================================

void cBackgroundList::ScanDisk(QString Path,int TheGeometry) {
    if (Geometry==TheGeometry) return;
    Geometry=TheGeometry;

    QDir                Folder(Path);
    QFileInfoList       Files=Folder.entryInfoList();;

    List.clear();
    for (int i=0;i<Files.count();i++) if (Files[i].isFile() && QString(Files[i].suffix()).toLower()=="txt") {
        QString FileName=AdjustDirForOS(QFileInfo(Files[i]).path()+QDir::separator()+QFileInfo(Files[i]).baseName()+".jpg");
        if (QFileInfo(QString(FileName)).isFile())
            List.append(cBackgroundObject(Files[i].absoluteFilePath(),Geometry));
    }
}

//====================================================================================================================

void cBackgroundList::PopulateTable(QTableWidget *Table) {
    for (int i=0;i<List.count();i++) if (List[i].IsValide==true) {
        wgt_QBackgroundWidget *Widget=new wgt_QBackgroundWidget();
        Widget->SetupWidget(&List[i]);
        Table->insertRow(Table->rowCount());
        Table->setRowHeight(Table->rowCount()-1,64);
        Table->setCellWidget(Table->rowCount()-1,0,Widget);
    }
}

int cBackgroundList::SearchImage(QString NameToFind) {
    int Ret=-1;
    int j=0;
    while ((j<List.count())&&(Ret==-1)) if (List[j].Name==NameToFind) Ret=j; else j++;
    return Ret;
}

//*********************************************************************************************************************************************
// Global class containing icons of transitions
//*********************************************************************************************************************************************

cIconObject::cIconObject(int TheTransitionFamilly,int TheTransitionSubType) {
    TransitionFamilly=TheTransitionFamilly;
    TransitionSubType=TheTransitionSubType;
    QString Familly=QString("%1").arg(TransitionFamilly);   if (Familly.length()<2) Familly="0"+Familly;
    QString SubType=QString("%1").arg(TransitionSubType);   if (SubType.length()<2) SubType="0"+SubType;
    QString FileName="transitions-icons/tr-"+Familly+"-"+SubType+".png";
    Icon=QImage(AdjustDirForOS(FileName));
    if (Icon.isNull()) {
        Icon=QImage(AdjustDirForOS("transitions-icons/tr-icon-error.png"));
        qDebug()<<"Icon not found:"<<AdjustDirForOS(QDir(FileName).absolutePath());
    }
}

//====================================================================================================================

cIconObject::cIconObject(int TheTransitionFamilly,int TheTransitionSubType,QImage LumaImage) {
    TransitionFamilly=TheTransitionFamilly;
    TransitionSubType=TheTransitionSubType;
    Icon=LumaImage.scaled(QSize(32,32),Qt::IgnoreAspectRatio/*,Qt::SmoothTransformation*/);
}

//====================================================================================================================

cIconObject::~cIconObject() {

}

//*********************************************************************************************************************************************
// Global class containing icons library
//*********************************************************************************************************************************************

cIconList::cIconList() {

}

//====================================================================================================================

cIconList::~cIconList() {
    List.clear();
}

//====================================================================================================================

QImage *cIconList::GetIcon(int TransitionFamilly,int TransitionSubType) {
    int i=0;
    while ((i<List.count())&&((List[i].TransitionFamilly!=TransitionFamilly)||(List[i].TransitionSubType!=TransitionSubType))) i++;
    if (i<List.count()) return new QImage(List[i].Icon);
        else return new QImage("transitions-icons/tr-icon-error.png");
}

//*********************************************************************************************************************************************
// Global class for luma object
//*********************************************************************************************************************************************
cLumaListObject::cLumaListObject(QString FileName) {
    OriginalLuma=QImage(FileName);
    DlgLumaImage=QImage(OriginalLuma.scaled(LUMADLG_WIDTH,LUMADLG_HEIGHT,Qt::IgnoreAspectRatio/*,Qt::SmoothTransformation*/)).convertToFormat(QImage::Format_ARGB32_Premultiplied);
    Name        =QFileInfo(FileName).baseName();
}

//====================================================================================================================

cLumaListObject::~cLumaListObject() {
}

//*********************************************************************************************************************************************
// Global class containing luma library
//*********************************************************************************************************************************************

cLumaList::cLumaList() {
    Geometry=GEOMETRY_16_9;
    LUMADLG_WIDTH=int((double(LUMADLG_HEIGHT)/double(9))*double(16));
}

//====================================================================================================================

cLumaList::~cLumaList() {
    List.clear();
}

//====================================================================================================================

void cLumaList::ScanDisk(QString Path,int TransitionFamilly) {
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
    if (Geometry==TheGeometry) return;
    Geometry=TheGeometry;
    switch (Geometry) {
    case GEOMETRY_4_3   : LUMADLG_WIDTH=int((double(LUMADLG_HEIGHT)/double(3))*double(4));    break;
    case GEOMETRY_16_9  : LUMADLG_WIDTH=int((double(LUMADLG_HEIGHT)/double(9))*double(16));   break;
    case GEOMETRY_40_17 :
    default             : LUMADLG_WIDTH=int((double(LUMADLG_HEIGHT)/double(17))*double(40));  break;
    }
    for (int i=0;i<List.count();i++)
        List[i].DlgLumaImage=QImage(List[i].OriginalLuma.scaled(LUMADLG_WIDTH,LUMADLG_HEIGHT,Qt::IgnoreAspectRatio/*,Qt::SmoothTransformation*/)).convertToFormat(QImage::Format_ARGB32_Premultiplied);
}
