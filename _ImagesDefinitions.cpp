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

void cBrushDefinition::GetDefaultFraming(FramingType TheFramingType,bool LockGeometry,double &X,double &Y,double &ZoomFactor,double &AspectRatio) {
    if ((Image==NULL)&&(Video==NULL)) return;

    // Calc coordinates of the part in the source image
    double  RealImageW  =double(Image!=NULL?Image->ImageWidth:Video->ImageWidth);
    double  RealImageH  =double(Image!=NULL?Image->ImageHeight:Video->ImageHeight);
    double  Hyp         =sqrt(RealImageW*RealImageW+RealImageH*RealImageH);             // Calc hypothenuse of the image to define full canvas
    double  DstX        =((Hyp-RealImageW)/2);
    double  DstY        =((Hyp-RealImageH)/2);
    double  DstW        =RealImageW;
    double  DstH        =RealImageH;
    double  W;
    double  H;

    switch (TheFramingType) {
        case ADJUST_WITH : // Adjust to With
            W=DstX+DstW-DstX;
            H=W*AspectRatio;
            X=((Hyp-W)/2)/Hyp;
            Y=((Hyp-H)/2)/Hyp;
            ZoomFactor=W/Hyp;
            break;
        case ADJUST_HEIGHT : // Adjust to Height
            H=DstY+DstH-DstY;
            W=H/AspectRatio;
            X=((Hyp-W)/2)/Hyp;
            Y=((Hyp-H)/2)/Hyp;
            ZoomFactor=W/Hyp;
            break;
        case ADJUST_FULL : // Adjust to FullImage
            if (!LockGeometry) {
                W=DstX+DstW-DstX;
                H=DstY+DstH-DstY;
                AspectRatio=H/W;
                X=((Hyp-W)/2)/Hyp;
                Y=((Hyp-H)/2)/Hyp;
                ZoomFactor=W/Hyp;
            } else {
                W=DstX+DstW-DstX;
                H=W*AspectRatio;
                if (H<DstY+DstH-DstY) {
                    H=DstY+DstH-DstY;
                    W=H/AspectRatio;
                    X=((Hyp-W)/2)/Hyp;
                    Y=((Hyp-H)/2)/Hyp;
                    ZoomFactor=W/Hyp;
                } else {
                    W=DstX+DstW-DstX;
                    H=W*AspectRatio;
                    X=((Hyp-W)/2)/Hyp;
                    Y=((Hyp-H)/2)/Hyp;
                    ZoomFactor=W/Hyp;
                }
            }
            break;
    }
}

//====================================================================================================================

QString cBrushDefinition::GetFramingStyle(double X,double Y,double ZoomFactor,double AspectRatio,double ImageRotation) {
    return  QString("###X:%1").arg(X,0,'e',4)+
            QString("###Y:%1").arg(Y,0,'e',4)+
            QString("###ZoomFactor:%1").arg(ZoomFactor,0,'e')+
            QString("###ImageRotation:%1").arg(ImageRotation,0,'e')+
            QString("###AspectRatio:%1").arg(AspectRatio,0,'e');
}

//====================================================================================================================

void cBrushDefinition::InitDefaultFramingStyle(bool LockGeometry,double AspectRatio) {
    double X,Y,ZoomFactor;

    GetDefaultFraming(ADJUST_WITH,LockGeometry,X,Y,ZoomFactor,AspectRatio);     DefaultFramingW=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
    GetDefaultFraming(ADJUST_HEIGHT,LockGeometry,X,Y,ZoomFactor,AspectRatio);   DefaultFramingH=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
    GetDefaultFraming(ADJUST_FULL,LockGeometry,X,Y,ZoomFactor,AspectRatio);     DefaultFramingF=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
}

//====================================================================================================================

void cBrushDefinition::ApplyStyle(bool LockGeometry,QString Style) {
    QStringList List;

    // String to StringList
    while (Style.contains("###")) {
        List.append(Style.left(Style.indexOf("###")));
        Style=Style.mid(Style.indexOf("###")+QString("###").length());
    }
    if (!Style.isEmpty()) List.append(Style);

    // Apply
    BrushFileCorrect.LockGeometry=LockGeometry;
    for (int i=0;i<List.count();i++) {
        if      (List[i].startsWith("X:"))              BrushFileCorrect.X             =List[i].mid(QString("X:").length()).toDouble();
        else if (List[i].startsWith("Y:"))              BrushFileCorrect.Y             =List[i].mid(QString("Y:").length()).toDouble();
        else if (List[i].startsWith("ZoomFactor:"))     BrushFileCorrect.ZoomFactor    =List[i].mid(QString("ZoomFactor:").length()).toDouble();
        else if (List[i].startsWith("ImageRotation:"))  BrushFileCorrect.ImageRotation =List[i].mid(QString("ImageRotation:").length()).toDouble();
        else if (List[i].startsWith("AspectRatio:"))    BrushFileCorrect.AspectRatio   =List[i].mid(QString("AspectRatio:").length()).toDouble();

    }
}

//====================================================================================================================

QBrush *cBrushDefinition::GetImageDiskBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush) {
    if ((Image?Image->FileName:Video?Video->FileName:"")=="") return new QBrush(Qt::NoBrush);

    // W and H = 0 when producing sound track in render process
    bool    SoundOnly=((Rect.width()==0)&&(Rect.height()==0));

    if (!SoundOnly) {
        QImage *RenderImage=(Image?Image->ImageAt(PreviewMode,false,&Image->BrushFileTransform,(!PreviewMode || GlobalMainWindow->ApplicationConfig->Smoothing)):
                            Video?Video->ImageAt(PreviewMode,Position,StartPosToAdd,false,SoundTrackMontage,SoundVolume,SoundOnly,&Video->BrushFileTransform):
                            NULL);

        QBrush *Ret=NULL;
        QImage *Img=NULL;
        if (RenderImage) {
            if (BrushFileCorrect.FullFilling) {
                // Create brush image with distortion
                Img=new QImage(RenderImage->scaled(Rect.width(),Rect.height(),Qt::IgnoreAspectRatio,(!PreviewMode || GlobalMainWindow->ApplicationConfig->Smoothing)?Qt::SmoothTransformation:Qt::FastTransformation));
            } else {
                // Create brush image with ken burns effect !
                Img=BrushFileCorrect.GetImage(RenderImage,Rect.width(),Rect.height(),PctDone,PreviousBrush?&PreviousBrush->BrushFileCorrect:NULL,(!PreviewMode || GlobalMainWindow->ApplicationConfig->Smoothing));
            }
            if (Img) {
                Ret=new QBrush(*Img);
                delete Img;
            }
            delete RenderImage;
        }
        return Ret;
    } else {
        // Force loading of sound of video
        if (Video) {
            QImage *RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,false,SoundTrackMontage,SoundVolume,SoundOnly,&Video->BrushFileTransform);
            delete RenderImage;
        }
        return new QBrush(Qt::NoBrush);
    }
}

//====================================================================================================================

QBrush *cBrushDefinition::GetLibraryBrush(QRectF Rect) {
    int BackgroundImageNumber=BackgroundList.SearchImage(BrushImage);
    if ((BackgroundImageNumber>=0)&&(BackgroundImageNumber<BackgroundList.List.count())) {
        double Ratio=double(BackgroundList.List[BackgroundImageNumber].BackgroundImage.height())/double(BackgroundList.List[BackgroundImageNumber].BackgroundImage.width());
        double H    =Rect.height()+1;
        double W    =H/Ratio;
        QImage NewImg1;
        if (W<(Rect.width()+1)) {
            NewImg1=QImage(BackgroundList.List[BackgroundImageNumber].BackgroundImage.scaledToWidth(Rect.width()+1,Qt::SmoothTransformation));
        } else {
            NewImg1=QImage(BackgroundList.List[BackgroundImageNumber].BackgroundImage.scaledToHeight(Rect.height()+1,Qt::SmoothTransformation));
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

QString cBrushDefinition::GetFramingStyle() {
    return  QString("###X:%1").arg(BrushFileCorrect.X,0,'e')+
            QString("###Y:%1").arg(BrushFileCorrect.Y,0,'e')+
            QString("###ZoomFactor:%1").arg(BrushFileCorrect.ZoomFactor,0,'e')+
            QString("###LockGeometry:%1").arg(BrushFileCorrect.LockGeometry?1:0)+
            QString("###AspectRatio:%1").arg(BrushFileCorrect.AspectRatio,0,'e');
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
    Image               =BrushToCopy->Image;
    Video               =BrushToCopy->Video;
    SoundVolume         =BrushToCopy->SoundVolume;
    BrushFileCorrect    =BrushToCopy->BrushFileCorrect;
    DefaultFramingW     =BrushToCopy->DefaultFramingW;
    DefaultFramingH     =BrushToCopy->DefaultFramingH;
    DefaultFramingF     =BrushToCopy->DefaultFramingF;
}

//====================================================================================================================

void cBrushDefinition::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    QString         BrushFileName=(Image?Image->FileName:Video?Video->FileName:"");

    if ((PathForRelativPath!="")&&((Image?Image->FileName:Video?Video->FileName:"")!="")) {
        if (ForceAbsolutPath) BrushFileName=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(BrushFileName));
            else BrushFileName=QDir::cleanPath(QDir(PathForRelativPath).relativeFilePath(BrushFileName));
    }
    // Attribut of the object
    Element.setAttribute("TypeComposition",TypeComposition);
    Element.setAttribute("BrushType",BrushType);                                                            // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
    switch (BrushType) {
        case BRUSHTYPE_PATTERN      :
            Element.setAttribute("PatternType",PatternType);                                                // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
            Element.setAttribute("ColorD",ColorD);                                                          // First Color
            break;
        case BRUSHTYPE_GRADIENT3    :
            Element.setAttribute("ColorIntermed",ColorIntermed);                                            // Intermediate Color
            Element.setAttribute("Intermediate",Intermediate);                                              // Intermediate position of 2nd color (in %)
        case BRUSHTYPE_GRADIENT2    :
            Element.setAttribute("ColorF",ColorF);                                                          // Last Color
            Element.setAttribute("GradientOrientation",GradientOrientation);                                // 0=Radial, 1=Up-Left, 2=Up, 3=Up-right, 4=Right, 5=bt-right, 6=bottom, 7=bt-Left, 8=Left
        case BRUSHTYPE_SOLID        :
            Element.setAttribute("ColorD",ColorD);                                                          // First Color
            break;
        case BRUSHTYPE_IMAGELIBRARY :
            Element.setAttribute("BrushImage",BrushImage);                                                  // Image name if image from library
            break;
        case BRUSHTYPE_IMAGEDISK :
            if (Video!=NULL) {
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
                    Element.setAttribute("BrushFileName",BrushFileName);                                    // File name if image from disk
                    Element.setAttribute("StartPos",Video->StartPos.toString());                            // Start position (video only)
                    Element.setAttribute("EndPos",Video->EndPos.toString());                                // End position (video only)
                    Video->BrushFileTransform.SaveToXML(Element,"ImageTransformation",PathForRelativPath);  // Image transformation
                } else Element.setAttribute("SoundVolume",QString("%1").arg(SoundVolume,0,'f'));            // Volume of soundtrack (for video only)
            } else if (Image!=NULL) {
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
                    Element.setAttribute("BrushFileName",BrushFileName);                                    // File name if image from disk
                    Image->BrushFileTransform.SaveToXML(Element,"ImageTransformation",PathForRelativPath);  // Image transformation
                    cLuLoImageCacheObject *ImageObject=GlobalMainWindow->ImagesCache.FindObject(QFileInfo((Image?Image->FileName:Video?Video->FileName:"")).absoluteFilePath(),&Image->BrushFileTransform,GlobalMainWindow->ApplicationConfig->Smoothing,true); // Get image object
                    if (ImageObject) Element.setAttribute("ImageOrientation",ImageObject->ImageOrientation);
                }
            }
            break;
    }

    BrushFileCorrect.SaveToXML(Element,"ImageCorrection");       // Image correction if image from disk
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cBrushDefinition::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList &AliasList) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        // Attribut of the object
        TypeComposition =Element.attribute("TypeComposition").toInt();
        BrushType       =Element.attribute("BrushType").toInt();                         // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors
        switch (BrushType) {
            case BRUSHTYPE_PATTERN      :
                PatternType         =Element.attribute("PatternType").toInt();          // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
                ColorD              =Element.attribute("ColorD");                       // First Color
                break;
            case BRUSHTYPE_GRADIENT3    :
                ColorIntermed       =Element.attribute("ColorIntermed");                // Intermediate Color
                Intermediate        =Element.attribute("Intermediate").toDouble();      // Intermediate position of 2nd color (in %)
            case BRUSHTYPE_GRADIENT2    :
                ColorF              =Element.attribute("ColorF");                       // Last Color
                GradientOrientation =Element.attribute("GradientOrientation").toInt();  // 0=Radial, 1=Up-Left, 2=Up, 3=Up-right, 4=Right, 5=bt-right, 6=bottom, 7=bt-Left, 8=Left
            case BRUSHTYPE_SOLID        :
                ColorD=Element.attribute("ColorD");                                    // First Color
                break;
            case BRUSHTYPE_IMAGELIBRARY :
                BrushImage=Element.attribute("BrushImage");  // Image name if image from library
                break;
            case BRUSHTYPE_IMAGEDISK :
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {
                    QString BrushFileName=Element.attribute("BrushFileName");                       // File name if image from disk
                    if ((PathForRelativPath!="")&&(BrushFileName!=""))
                        BrushFileName=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(BrushFileName));

                    bool IsValide=false;
                    QString Extension=QFileInfo(BrushFileName).suffix().toLower();
                    for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowImageExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowImageExtension[i]==Extension) {
                        Image=new cimagefilewrapper();
                        cLuLoImageCacheObject *ImageObject=GlobalMainWindow->ImagesCache.FindObject(BrushFileName,&Image->BrushFileTransform,GlobalMainWindow->ApplicationConfig->Smoothing,true); // Get image object
                        if ((ImageObject)&&(Element.hasAttribute("ImageOrientation"))) ImageObject->ImageOrientation=Element.attribute("ImageOrientation").toInt();
                        IsValide=Image->GetInformationFromFile(BrushFileName,AliasList);
                        if (!IsValide) {
                            delete Image;
                            Image=NULL;
                        }
                        break;
                    }
                    if (Image==NULL) for (int i=0;i<GlobalMainWindow->ApplicationConfig->AllowVideoExtension.count();i++) if (GlobalMainWindow->ApplicationConfig->AllowVideoExtension[i]==Extension) {
                        Video=new cvideofilewrapper();
                        IsValide=Video->GetInformationFromFile(BrushFileName,false,AliasList);
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
                    } else SoundVolume=Element.attribute("SoundVolume").toDouble();                                 // Volume of soundtrack (for video only)
                } else if (Image!=NULL) {
                    if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                    // Global definition only !
                        Image->BrushFileTransform.LoadFromXML(Element,"ImageTransformation",PathForRelativPath);    // Image transformation
                    }
                }
                break;
        }

        BrushFileCorrect.LoadFromXML(Element,"ImageCorrection");         // Image correction if image from disk
        return (BrushType==BRUSHTYPE_IMAGEDISK)?(Image!=NULL)||(Video!=NULL):true;
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

    // Load file
    BackgroundImage.load(FilePath);

    // Make Icon
    QImage *BrushImage=new QImage(BackgroundImage.copy());
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
        QString FileName=QFileInfo(Files[i]).absoluteFilePath();
        if (QFileInfo(QString(FileName)).isFile()) List.append(cBackgroundObject(Files[i].absoluteFilePath(),Geometry));
    }
}

//====================================================================================================================

int cBackgroundList::SearchImage(QString NameToFind) {
    int Ret=-1;
    int j=0;
    while ((j<List.count())&&(Ret==-1)) if (List[j].Name==NameToFind) Ret=j; else j++;
    if (Ret==-1) Ret=0; // If not found : switch to first background
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
    QString FileName=QString("transitions-img")+QDir().separator()+QString("tr-")+Familly+QString("-")+SubType+QString(".png");
    Icon=QImage(FileName);
    if (Icon.isNull()) {
        Icon=QImage(QString("transitions-img")+QDir().separator()+QString("tr-icon-error.png"));
        qDebug()<<"Icon not found:"<<QDir(FileName).absolutePath();
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
    DlgLumaImage=QImage(OriginalLuma.scaled(LUMADLG_WIDTH,LUMADLG_HEIGHT,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)).convertToFormat(QImage::Format_ARGB32_Premultiplied);
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
        List[i].DlgLumaImage=QImage(List[i].OriginalLuma.scaled(LUMADLG_WIDTH,LUMADLG_HEIGHT,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)).convertToFormat(QImage::Format_ARGB32_Premultiplied);
}
