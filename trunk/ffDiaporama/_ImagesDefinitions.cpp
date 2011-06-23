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
#include "mainwindow.h"

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
// Utility function to create a gradient brush
//*********************************************************************************************************************************************

QBrush *GetGradientBrush(QRectF Rect,int BrushType,int GradientOrientation,QString ColorD,QString ColorF,QString ColorIntermed,double Intermediate) {
    QGradient Gradient;
    switch (GradientOrientation) {
        case GRADIENTORIENTATION_UPLEFT:        Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()),QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()));                     break;          // Up-Left
        case GRADIENTORIENTATION_UP:            Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()),QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()));    break;          // Up
        case GRADIENTORIENTATION_UPRIGHT:       Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()),QPointF(Rect.x(),Rect.y()+Rect.height()));                     break;          // Up-right
        case GRADIENTORIENTATION_LEFT:          Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()+Rect.height()/2),QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()/2));   break;          // Left
        case GRADIENTORIENTATION_RADIAL:        Gradient=QRadialGradient(
                    QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()/2),
                    Rect.width()>Rect.height()?Rect.width():Rect.height(),
                    QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()/2)
                );  break;                                                                                                                                      // Radial
        case GRADIENTORIENTATION_RIGHT:         Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()/2),QPointF(Rect.x(),Rect.y()+Rect.height()/2));   break;          // Right
        case GRADIENTORIENTATION_BOTTOMLEFT:    Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()+Rect.height()),QPointF(Rect.x()+Rect.width(),Rect.y()));                     break;          // bt-Left
        case GRADIENTORIENTATION_BOTTOM:        Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()),QPointF(Rect.x()+Rect.width()/2,Rect.y()));    break;          // bottom
        case GRADIENTORIENTATION_BOTTOMRIGHT:   Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()),QPointF(Rect.x(),Rect.y()));                     break;          // bt-right
    }
    Gradient.setColorAt(0,QColor(ColorD));
    Gradient.setColorAt(1,QColor(ColorF));
    if (BrushType==BRUSHTYPE_GRADIENT3) Gradient.setColorAt(Intermediate,QColor(ColorIntermed));
    return new QBrush(Gradient);
}

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
    ImageRotation   = 0;                // Image rotation
    X               = 0;                // X position (in %) relative to up/left corner
    Y               = 0;                // Y position (in %) relative to up/left corner
    ZoomFactor      = 1;                // Zoom factor (in %)
    Brightness      = 0;
    Contrast        = 0;
    Gamma           = 1;
    Red             = 0;
    Green           = 0;
    Blue            = 0;
    ImageGeometry   = GlobalMainWindow->Diaporama->ImageGeometry;
}

QImage *cFilterCorrectObject::GetImage(QImage *LastLoadedImage,int Width,int Height,double PctDone,cFilterCorrectObject *PreviousFilter) {
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
        if (PreviousFilter->AspectRatio!=TheAspectRatio)    TheAspectRatio  =PreviousFilter->AspectRatio+(TheAspectRatio-PreviousFilter->Blue)*AspectRatio;
    }

    // Prepare values from sourceimage size
    double   RealImageW=double(LastLoadedImage->width());               // Get real image widht
    double   RealImageH=double(LastLoadedImage->height());              // Get real image height
    double   Hyp=sqrt(RealImageW*RealImageW+RealImageH*RealImageH);     // Calc hypothenuse of the image to define full canvas

    // Rotate image if needed and create a SourceImage
    if (TheRotateFactor!=0) {
        QTransform matrix;
        matrix.rotate(TheRotateFactor,Qt::ZAxis);
        SourceImage=new QImage(LastLoadedImage->transformed(matrix));
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

    PB.drawImage(QRectF(DstX,DstY,DstW,DstH),*SourceImage,QRectF((RealImageW/2)-(Hyp/2)+SrcX,(RealImageH/2)-(Hyp/2)+SrcY,SrcW,SrcH));

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
    Element.setAttribute("ImageGeometry",   ImageGeometry);
    Element.setAttribute("AspectRatio",     AspectRatio);

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cFilterCorrectObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString /*PathForRelativPath*/) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        X               =Element.attribute("X").toDouble();                      // X position (in %) relative to up/left corner
        Y               =Element.attribute("Y").toDouble();                      // Y position (in %) relative to up/left corner
        ZoomFactor      =Element.attribute("ZoomFactor").toDouble();             // Zoom factor (in %)
        ImageRotation   =Element.attribute("ImageRotation").toInt();            // Image rotation (in °)
        Brightness      =Element.attribute("Brightness").toInt();
        Contrast        =Element.attribute("Contrast").toInt();
        Gamma           =Element.attribute("Gamma").toDouble();
        Red             =Element.attribute("Red").toInt();
        Green           =Element.attribute("Green").toInt();
        Blue            =Element.attribute("Blue").toInt();
        ImageGeometry   =Element.attribute("ImageGeometry").toInt();
        AspectRatio     =Element.attribute("AspectRatio").toDouble();

        return true;
    }
    return false;
}

//*********************************************************************************************************************************************
// Object for Brush definition
//*********************************************************************************************************************************************

cBrushDefinition::cBrushDefinition() {
    TypeComposition     =COMPOSITIONTYPE_BACKGROUND;
    BrushType           =BRUSHTYPE_SOLID;               // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors, 5=brush library, 6=image disk
    PatternType         =Qt::Dense4Pattern;             // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    ColorD              =DEFAULT_SHAPE_BRUSHCOLORD;     // First Color
    ColorF              ="#000000";                     // Last Color
    ColorIntermed       ="#777777";                     // Intermediate Color
    Intermediate        =0.1;                           // Intermediate position of 2nd color (in %)
    GradientOrientation =6;                             // 1=Up-Left, 2=Up, 3=Up-right, ...
    BrushImage          ="";                            // Image name if image from library
    BrushFileName       ="";                            // Image name if image from disk
    Image               =NULL;
    Video               =NULL;
    SoundVolume         =1;                             // Volume of soundtrack
}

//====================================================================================================================
cBrushDefinition::~cBrushDefinition() {
    if (Image) {
        if (TypeComposition!=COMPOSITIONTYPE_SHOT) delete Image;
        Image=NULL;
    }
    if (Video) {
        if (TypeComposition!=COMPOSITIONTYPE_SHOT) delete Video;
        Video=NULL;
    }
}

//====================================================================================================================
QBrush *cBrushDefinition::GetBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush) {
    switch (BrushType) {
        case BRUSHTYPE_NOBRUSH :        return new QBrush(Qt::NoBrush);
        case BRUSHTYPE_SOLID :          return new QBrush(QColor(ColorD),Qt::SolidPattern);
        case BRUSHTYPE_PATTERN :        return new QBrush(QColor(ColorD),(Qt::BrushStyle)(PatternType+3));
        case BRUSHTYPE_GRADIENT2 :      return GetGradientBrush(Rect,BrushType,GradientOrientation,ColorD,ColorF,ColorIntermed,Intermediate);
        case BRUSHTYPE_GRADIENT3 :      return GetGradientBrush(Rect,BrushType,GradientOrientation,ColorD,ColorF,ColorIntermed,Intermediate);
        case BRUSHTYPE_IMAGELIBRARY :   return GetLibraryBrush(Rect);
        case BRUSHTYPE_IMAGEDISK :      return GetImageDiskBrush(Rect,PreviewMode,Position,StartPosToAdd,SoundTrackMontage,PctDone,PreviousBrush);
    }
    return new QBrush(Qt::NoBrush);
}

//====================================================================================================================
QBrush *cBrushDefinition::GetImageDiskBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush) {
    if (BrushFileName=="") return new QBrush(Qt::NoBrush);

    // W and H = 0 when producing sound track in render process
    bool    SoundOnly=((Rect.width()==0)&&(Rect.height()==0));

    QImage *RenderImage=(Image?Image->ImageAt(PreviewMode,false,&Image->BrushFileTransform):
                        Video?Video->ImageAt(PreviewMode,Position,StartPosToAdd,false,SoundTrackMontage,SoundVolume,SoundOnly,&Video->BrushFileTransform):
                        NULL);
    if ((!SoundOnly)&&(RenderImage)) {
        // Create brush image with ken burns effect !
        QImage *Img=BrushFileCorrect.GetImage(RenderImage,Rect.width(),Rect.height(),PctDone,PreviousBrush?&PreviousBrush->BrushFileCorrect:NULL);
        QBrush *Ret=new QBrush(*Img);
        delete Img;
        delete RenderImage;
        return Ret;
    } else {
        if (RenderImage) delete RenderImage;
        return new QBrush(Qt::NoBrush);
    }
}

//====================================================================================================================

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
        double Ratio=double(LastLoadedBackgroundImage->height())/double(LastLoadedBackgroundImage->width());
        double H    =Rect.height()+1;
        double W    =H/Ratio;
        QImage NewImg1;
        if (W<(Rect.width()+1)) {
            NewImg1=QImage(LastLoadedBackgroundImage->scaledToWidth(Rect.width()+1));
        } else {
            NewImg1=QImage(LastLoadedBackgroundImage->scaledToHeight(Rect.height()+1));
        }
        W=NewImg1.width();
        H=GetHeightForWidth(W,Rect);
        if (H<NewImg1.height()) {
            H=NewImg1.height();
            W=GetWidthForHeight(H,Rect);
        }
        if ((W!=NewImg1.width())||(H!=NewImg1.height())) return new QBrush(QImage(NewImg1.copy(0,0,W,H))); else return new QBrush(NewImg1);
    } else return new QBrush(Qt::NoBrush);
}

//====================================================================================================================
// Return height for width depending on Rect geometry
int cBrushDefinition::GetHeightForWidth(int WantedWith,QRectF Rect) {
    double   Ratio=Rect.width()/Rect.height();
    return int(double(double(WantedWith)/Ratio));
}

//====================================================================================================================
// Return width for height depending on Rect geometry
int cBrushDefinition::GetWidthForHeight(int WantedHeight,QRectF Rect) {
    double   Ratio=Rect.height()/Rect.width();
    return int(double(double(WantedHeight)/Ratio));
}

//====================================================================================================================
// create a COMPOSITIONTYPE_SHOT brush as a copy of a given brush

void cBrushDefinition::CopyFromBrushDefinition(cBrushDefinition *BrushToCopy) {
    TypeComposition     =COMPOSITIONTYPE_SHOT;
    BrushType           =BrushToCopy->BrushType;
    PatternType         =BrushToCopy->PatternType;
    GradientOrientation =BrushToCopy->GradientOrientation;
    ColorD              =BrushToCopy->ColorD;
    ColorF              =BrushToCopy->ColorF;
    ColorIntermed       =BrushToCopy->ColorIntermed;
    Intermediate        =BrushToCopy->Intermediate;
    BrushImage          =BrushToCopy->BrushImage;
    BrushFileName       =BrushToCopy->BrushFileName;
    Image               =BrushToCopy->Image;
    Video               =BrushToCopy->Video;
    SoundVolume         =BrushToCopy->SoundVolume;
    BrushFileCorrect    =BrushToCopy->BrushFileCorrect;
}

//====================================================================================================================

void cBrushDefinition::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    if ((PathForRelativPath!="")&&(BrushFileName!="")) {
        if (ForceAbsolutPath) BrushFileName=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(BrushFileName));
            else BrushFileName=QDir::cleanPath(QDir(PathForRelativPath).relativeFilePath(BrushFileName));
    }
    // Attribut of the object
    Element.setAttribute("TypeComposition",TypeComposition);
    Element.setAttribute("BrushType",BrushType);                                                    // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
    Element.setAttribute("PatternType",PatternType);                                                // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    Element.setAttribute("ColorD",ColorD);                                                          // First Color
    Element.setAttribute("ColorF",ColorF);                                                          // Last Color
    Element.setAttribute("ColorIntermed",ColorIntermed);                                            // Intermediate Color
    Element.setAttribute("Intermediate",Intermediate);                                              // Intermediate position of 2nd color (in %)
    Element.setAttribute("GradientOrientation",GradientOrientation);                                // 0=Radial, 1=Up-Left, 2=Up, 3=Up-right, 4=Right, 5=bt-right, 6=bottom, 7=bt-Left, 8=Left
    Element.setAttribute("BrushImage",BrushImage);                                                  // Image name if image from library
    Element.setAttribute("BrushFileName",BrushFileName);                                            // Image name if image from disk
    if (Video!=NULL) {
        if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
            Element.setAttribute("StartPos",Video->StartPos.toString());                            // Start position (video only)
            Element.setAttribute("EndPos",Video->EndPos.toString());                                // End position (video only)
            Video->BrushFileTransform.SaveToXML(Element,"ImageTransformation",PathForRelativPath);  // Image transformation
        } else Element.setAttribute("SoundVolume",QString("%1").arg(SoundVolume,0,'f'));            // Volume of soundtrack (for video only)
    } else if (Image!=NULL) {
        if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
            Image->BrushFileTransform.SaveToXML(Element,"ImageTransformation",PathForRelativPath);  // Image transformation
        }
    }
    BrushFileCorrect.SaveToXML(Element,"ImageCorrection",PathForRelativPath);       // Image correction if image from disk
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cBrushDefinition::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        // Attribut of the object
        TypeComposition=    Element.attribute("TypeComposition").toInt();
        BrushType=          Element.attribute("BrushType").toInt();                         // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
        PatternType=        Element.attribute("PatternType").toInt();                       // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
        ColorD=             Element.attribute("ColorD");                                    // First Color
        ColorF=             Element.attribute("ColorF");                                    // Last Color
        ColorIntermed=      Element.attribute("ColorIntermed");                             // Intermediate Color
        Intermediate=       Element.attribute("Intermediate").toDouble();                   // Intermediate position of 2nd color (in %)
        GradientOrientation=Element.attribute("GradientOrientation").toInt();               // 0=Radial, 1=Up-Left, 2=Up, 3=Up-right, 4=Right, 5=bt-right, 6=bottom, 7=bt-Left, 8=Left
        BrushImage         =Element.attribute("BrushImage");                                // Image name if image from library
        BrushFileName      =Element.attribute("BrushFileName");                             // Image name if image from disk

        if ((PathForRelativPath!="")&&(BrushFileName!=""))
            BrushFileName=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(BrushFileName));

        BrushFileCorrect.LoadFromXML(Element,"ImageCorrection",PathForRelativPath);         // Image correction if image from disk

        if ((TypeComposition!=COMPOSITIONTYPE_SHOT)&&(BrushType==BRUSHTYPE_IMAGEDISK)) {
            bool IsValide=false;
            QString Extension=QFileInfo(BrushFileName).suffix().toLower();
            for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowImageExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowImageExtension[i]==Extension) {
                Image=new cimagefilewrapper();
                IsValide=Image->GetInformationFromFile(BrushFileName);
                if (!IsValide) {
                    delete Image;
                    Image=NULL;
                }
                break;
            }
            if (Image==NULL) for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowVideoExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowVideoExtension[i]==Extension) {
                Video=new cvideofilewrapper();
                IsValide=Video->GetInformationFromFile(BrushFileName,false);
                if (!IsValide) {
                    delete Video;
                    Video=NULL;
                }
                if (Video!=NULL) {
                }
                break;
            }
        }

        if (Video!=NULL) {
            if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                    // Global definition only !
                Video->StartPos =QTime().fromString(Element.attribute("StartPos"));                         // Start position (video only)
                Video->EndPos   =QTime().fromString(Element.attribute("EndPos"));                           // End position (video only)
                Video->BrushFileTransform.LoadFromXML(Element,"ImageTransformation",PathForRelativPath);    // Image transformation
            } else
                SoundVolume=Element.attribute("SoundVolume").toDouble();                                 // Volume of soundtrack (for video only)
        } else if (Image!=NULL) {
            if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                    // Global definition only !
                Image->BrushFileTransform.LoadFromXML(Element,"ImageTransformation",PathForRelativPath);    // Image transformation
            }
        }

        return true;
    }
    return false;
}

//*********************************************************************************************************************************************
// Base object for composition definition
//*********************************************************************************************************************************************

cBackgroundObject::cBackgroundObject(QString FileName,int TheGeometry) {
    IsValide    = false;
    FilePath    = FileName;
    Name        = QFileInfo(FileName).baseName();

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
    IsValide=!Icon.isNull();
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
    for (int i=0;i<Files.count();i++) if (Files[i].isFile() && ((QString(Files[i].suffix()).toLower()=="jpg")||(QString(Files[i].suffix()).toLower()=="png"))) {
        QString FileName=AdjustDirForOS(QFileInfo(Files[i]).absoluteFilePath());
        if (QFileInfo(QString(FileName)).isFile()) List.append(cBackgroundObject(Files[i].absoluteFilePath(),Geometry));
    }
}

//====================================================================================================================

int cBackgroundList::SearchImage(QString NameToFind) {
    int Ret=0;
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
    QString FileName="transitions-img/tr-"+Familly+"-"+SubType+".png";
    Icon=QImage(AdjustDirForOS(FileName));
    if (Icon.isNull()) {
        Icon=QImage(AdjustDirForOS("transitions-img/tr-icon-error.png"));
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
        else return new QImage("transitions-img/tr-icon-error.png");
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
