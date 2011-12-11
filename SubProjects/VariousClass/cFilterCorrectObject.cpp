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

#include "cFilterCorrectObject.h"
#include "../fmt_filters/fmt_filters.h"

//#define DEBUGMODE

//*********************************************************************************************************************************************
// Base object for filters correction image
//*********************************************************************************************************************************************

cFilterCorrectObject::cFilterCorrectObject() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFilterCorrectObject::cFilterCorrectObject";
    #endif
    ImageRotation           = 0;                // Image rotation
    X                       = 0;                // X position (in %) relative to up/left corner
    Y                       = 0;                // Y position (in %) relative to up/left corner
    ZoomFactor              = 1;                // Zoom factor (in %)
    Brightness              = 0;
    Contrast                = 0;
    Gamma                   = 1;
    Red                     = 0;
    Green                   = 0;
    Blue                    = 0;
    LockGeometry            = false;
    FullFilling             = false;

}

//====================================================================================================================

QImage *cFilterCorrectObject::GetImage(QImage *LastLoadedImage,int Width,int Height,double PctDone,cFilterCorrectObject *PreviousFilter,bool Smoothing) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFilterCorrectObject::GetImage";
    #endif
    if (!LastLoadedImage) return NULL;

    QImage  *SourceImage    =NULL;
    double  TheXFactor      =X;
    double  TheYFactor      =Y;
    double  TheZoomFactor   =ZoomFactor;
    double  TheRotateFactor =ImageRotation;
    double  TheBrightness   =Brightness;
    double  TheContrast     =Contrast;
    double  TheGamma        =Gamma;
    double  TheRed          =Red;
    double  TheGreen        =Green;
    double  TheBlue         =Blue;
    double  TheAspectRatio  =AspectRatio;

    // Adjust values depending on PctDone and previous Filter (if exist)
    if (PreviousFilter) {
        if (PreviousFilter->X!=TheXFactor)                  TheXFactor      =PreviousFilter->X+(TheXFactor-PreviousFilter->X)*PctDone;
        if (PreviousFilter->Y!=TheYFactor)                  TheYFactor      =PreviousFilter->Y+(TheYFactor-PreviousFilter->Y)*PctDone;
        if (PreviousFilter->ZoomFactor!=TheZoomFactor)      TheZoomFactor   =PreviousFilter->ZoomFactor+(TheZoomFactor-PreviousFilter->ZoomFactor)*PctDone;
        if (PreviousFilter->ImageRotation!=TheRotateFactor) TheRotateFactor =PreviousFilter->ImageRotation+(TheRotateFactor-PreviousFilter->ImageRotation)*PctDone;
        if (PreviousFilter->Brightness!=TheBrightness)      TheBrightness   =PreviousFilter->Brightness+(TheBrightness-PreviousFilter->Brightness)*PctDone;
        if (PreviousFilter->Contrast!=TheContrast)          TheContrast     =PreviousFilter->Contrast+(TheContrast-PreviousFilter->Contrast)*PctDone;
        if (PreviousFilter->Gamma!=TheGamma)                TheGamma        =PreviousFilter->Gamma+(TheGamma-PreviousFilter->Gamma)*PctDone;
        if (PreviousFilter->Red!=TheRed)                    TheRed          =PreviousFilter->Red+(TheRed-PreviousFilter->Red)*PctDone;
        if (PreviousFilter->Green!=TheGreen)                TheGreen        =PreviousFilter->Green+(TheGreen-PreviousFilter->Green)*PctDone;
        if (PreviousFilter->Blue!=TheBlue)                  TheBlue         =PreviousFilter->Blue+(TheBlue-PreviousFilter->Blue)*PctDone;
        if (PreviousFilter->AspectRatio!=TheAspectRatio)    TheAspectRatio  =PreviousFilter->AspectRatio+(TheAspectRatio-PreviousFilter->AspectRatio)*PctDone;
    }

    // Prepare values from sourceimage size
    double   RealImageW=double(LastLoadedImage->width());               // Get real image widht
    double   RealImageH=double(LastLoadedImage->height());              // Get real image height
    double   Hyp=sqrt(RealImageW*RealImageW+RealImageH*RealImageH);     // Calc hypothenuse of the image to define full canvas

    // Rotate image if needed and create a SourceImage
    if (TheRotateFactor!=0) {
        QTransform matrix;
        matrix.rotate(TheRotateFactor,Qt::ZAxis);
        SourceImage=new QImage(LastLoadedImage->transformed(matrix,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation));
        // update real image size
        RealImageW=double(SourceImage->width());
        RealImageH=double(SourceImage->height());

    // If no rotation then SourceImage=LastLoadedImage
    } else SourceImage=LastLoadedImage;

    // Calc coordinates of the part in the source image
    double  SrcX            =Hyp*TheXFactor;
    double  SrcY            =Hyp*TheYFactor;
    double  SrcW            =Hyp*TheZoomFactor;
    double  SrcH            =SrcW*TheAspectRatio;
    double  DstX            =0;
    double  DstY            =0;
    double  DstW            =Width;
    double  DstH            =DstW*TheAspectRatio;

    // Prepare RetImage Composition with transparent background
    QImage      *RetImage=new QImage(Width,Height,QImage::Format_ARGB32_Premultiplied);
    QPainter    PB;
    PB.begin(RetImage);
    PB.setCompositionMode(QPainter::CompositionMode_Source);
    PB.fillRect(QRect(0,0,Width+1,Height+1),Qt::transparent);
    PB.setCompositionMode(QPainter::CompositionMode_SourceOver);

    if (Smoothing) PB.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
        else       PB.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);

    // Expand Source Image to Canvas size (hyp)
    QImage   *TempImage=new QImage(Hyp,Hyp,QImage::Format_ARGB32_Premultiplied);
    QPainter PC;
    PC.begin(TempImage);
    PC.setCompositionMode(QPainter::CompositionMode_Source);
    PC.fillRect(QRect(0,0,Hyp,Hyp),Qt::transparent);
    PC.setCompositionMode(QPainter::CompositionMode_SourceOver);
    PC.drawImage((Hyp-RealImageW)/2,(Hyp-RealImageH)/2,*SourceImage);
    PC.end();

    // Adjust TempImage to wanted size and pos
    QImage NewSourceImage=(TempImage->copy(int(SrcX),int(SrcY),SrcW,SrcH)).scaled(int(DstW),int(DstH),Qt::IgnoreAspectRatio,Smoothing?Qt::SmoothTransformation:Qt::FastTransformation);
    delete TempImage;

    // Smoothing is not correctly used here !
    // Then force smoothing by reduce source image before draw image
    PB.drawImage(QRectF(DstX,DstY,DstW,DstH),NewSourceImage);
    PB.end();

    // Delete SourceImage if we have created it
    if (SourceImage!=LastLoadedImage) delete SourceImage;

    // Apply correction filters to DestImage
    fmt_filters::image img(RetImage->bits(),RetImage->width(),RetImage->height());
    if (TheBrightness!=0)                           fmt_filters::brightness(img,TheBrightness);
    if (TheContrast!=0)                             fmt_filters::contrast(img,TheContrast);
    if (TheGamma!=1)                                fmt_filters::gamma(img,TheGamma);
    if ((TheRed!=0)||(TheGreen!=0)||(TheBlue!=0))   fmt_filters::colorize(img,TheRed,TheGreen,TheBlue);

    return RetImage;
}

//====================================================================================================================

void cFilterCorrectObject::ApplyFilter(QImage *Image) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFilterCorrectObject::ApplyFilter";
    #endif
    if (Image==NULL) return;
    fmt_filters::image img(Image->bits(),Image->width(),Image->height());
    if (Brightness!=0)                      fmt_filters::brightness(img,Brightness);
    if (Contrast!=0)                        fmt_filters::contrast(img,Contrast);
    if (Gamma!=1)                           fmt_filters::gamma(img,Gamma);
    if ((Red!=0)||(Green!=0)||(Blue!=0))    fmt_filters::colorize(img,Red,Green,Blue);
}

//====================================================================================================================

void cFilterCorrectObject::SaveToXML(QDomElement &domDocument,QString ElementName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFilterCorrectObject::SaveToXML";
    #endif
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    // Attribut of the object
    Element.setAttribute("X",               X);                 // X position (in %) relative to up/left corner
    Element.setAttribute("Y",               Y);                 // Y position (in %) relative to up/left corner
    Element.setAttribute("ZoomFactor",      ZoomFactor);        // Zoom factor (in %)
    Element.setAttribute("ImageRotation",   ImageRotation);     // Image rotation (in °)
    Element.setAttribute("Brightness",      Brightness);
    Element.setAttribute("Contrast",        Contrast);
    Element.setAttribute("Gamma",           Gamma);
    Element.setAttribute("Red",             Red);
    Element.setAttribute("Green",           Green);
    Element.setAttribute("Blue",            Blue);
    Element.setAttribute("LockGeometry",    LockGeometry?1:0);
    Element.setAttribute("AspectRatio",     AspectRatio);
    Element.setAttribute("FullFilling",     FullFilling?1:0);

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cFilterCorrectObject::LoadFromXML(QDomElement domDocument,QString ElementName) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cFilterCorrectObject::LoadFromXML";
    #endif
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        if (Element.hasAttribute("X"))              X               =Element.attribute("X").toDouble();                      // X position (in %) relative to up/left corner
        if (Element.hasAttribute("Y"))              Y               =Element.attribute("Y").toDouble();                      // Y position (in %) relative to up/left corner
        if (Element.hasAttribute("ZoomFactor"))     ZoomFactor      =Element.attribute("ZoomFactor").toDouble();             // Zoom factor (in %)
        if (Element.hasAttribute("ImageRotation"))  ImageRotation   =Element.attribute("ImageRotation").toDouble();          // Image rotation (in °)
        if (Element.hasAttribute("Brightness"))     Brightness      =Element.attribute("Brightness").toInt();
        if (Element.hasAttribute("Contrast"))       Contrast        =Element.attribute("Contrast").toInt();
        if (Element.hasAttribute("Gamma"))          Gamma           =Element.attribute("Gamma").toDouble();
        if (Element.hasAttribute("Red"))            Red             =Element.attribute("Red").toInt();
        if (Element.hasAttribute("Green"))          Green           =Element.attribute("Green").toInt();
        if (Element.hasAttribute("Blue"))           Blue            =Element.attribute("Blue").toInt();
        if (Element.hasAttribute("AspectRatio"))    AspectRatio     =Element.attribute("AspectRatio").toDouble();
        if (Element.hasAttribute("FullFilling"))    FullFilling     =Element.attribute("FullFilling").toInt()==1;

        // If old ImageGeometry value in project file then compute LockGeometry
        if (Element.hasAttribute("ImageGeometry")) LockGeometry=(Element.attribute("ImageGeometry").toInt()!=2);
            // Else load saved value
            else if (Element.hasAttribute("LockGeometry")) LockGeometry=Element.attribute("LockGeometry").toInt()==1;

        return true;
    }
    return false;
}
