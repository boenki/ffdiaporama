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

#include "cBrushDefinition.h"

//#define DEBUGMODE

//*********************************************************************************************************************************************
// Utility function to create a gradient brush
//*********************************************************************************************************************************************

QBrush *GetGradientBrush(QRectF Rect,int BrushType,int GradientOrientation,QString ColorD,QString ColorF,QString ColorIntermed,double Intermediate) {
    #ifdef DEBUGMODE
    qDebug() << "IN:GetGradientBrush";
    #endif
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

//====================================================================================================================
// Utility function to draw a shape
//====================================================================================================================

void DrawShape(QPainter &Painter,int BackgroundForm,double left,double top,double width,double height,double CenterX,double CenterY) {
    double RayX=0,RayY=0;

    switch (BackgroundForm) {
        //0 = no shape
        case 1 : Painter.drawRect(QRectF(left,top,width-1,height-1)); break;  // Rectangle
        case 2 : // Round rect
            RayX=width/10;     if (RayX>16) RayX=16; else if (RayX<2)  RayX=2;
            RayY=height/10;    if (RayY>16) RayY=16; else if (RayY<2)  RayY=2;
            Painter.drawRoundedRect(QRectF(left,top,width-1,height-1),RayX,RayY,Qt::AbsoluteSize);
            break;
        case 3 : // Buble
            RayX=2*width/10;   if (RayX<4)  RayX=4;
            RayY=2*height/10;  if (RayY<4)  RayY=4;
            Painter.drawRoundedRect(QRectF(left,top,width-1,height-1),RayX,RayY,Qt::AbsoluteSize);
            break;
        case 4 : Painter.drawEllipse(QRectF(left,top,width-1,height-1));                        break;  // Ellipse
        case 5 : DrawPolygonR(Painter,width,height,CenterX,CenterY,3,90);                               break;  // Triangle UP
        case 6 : DrawPolygonR(Painter,width,height,CenterX,CenterY,3,0);                                break;  // Triangle Right
        case 7 : DrawPolygonR(Painter,width,height,CenterX,CenterY,3,-90);                              break;  // Triangle Down
        case 8 : DrawPolygonR(Painter,width,height,CenterX,CenterY,3,-180);                             break;  // Triangle left
        case 9 : DrawPolygonR(Painter,width,height,CenterX,CenterY,4,0);                                break;  // Losange
        case 10: DrawPolygonR(Painter,width,height,CenterX,CenterY,5,90-(double(360)/5));               break;  // pentagone
        case 11: DrawPolygonR(Painter,width,height,CenterX,CenterY,6,-(double(360)/6));                 break;  // hexagone
        case 12: DrawPolygonR(Painter,width,height,CenterX,CenterY,8,-(double(360)/8));                 break;  // Octogone
    }
}

//====================================================================================================================
// Utility function to draw a regular polygon shape
//====================================================================================================================

void DrawPolygonR(QPainter &Painter,double width,double height,double CenterX,double CenterY,int MaxPoint,double StartAngle) {
    QPointF Table[10];
    double  vcos,vsin,Angle;
    int     i;

    Angle=StartAngle;
    for (i=0;i<MaxPoint;i++) {
        vcos=cos(Angle*3.14159265/180)*(width/2);
        vsin=sin(Angle*3.14159265/180)*(height/2);
        Table[i]=QPointF(CenterX+vcos,CenterY-vsin);
        Angle=Angle+(double(360)/MaxPoint);
        if (Angle>=360) Angle=-Angle+360;
    }
    Painter.drawPolygon(Table,MaxPoint);
}

//*********************************************************************************************************************************************
// Base object for composition definition
//*********************************************************************************************************************************************

cBackgroundObject::cBackgroundObject(QString FileName,int TheGeometry) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBackgroundObject::cBackgroundObject";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBackgroundList::cBackgroundList";
    #endif
    Geometry=-1;
}

//====================================================================================================================

void cBackgroundList::ScanDisk(QString Path,int TheGeometry) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBackgroundList::ScanDisk";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBackgroundList::SearchImage";
    #endif
    int Ret=-1;
    int j=0;
    while ((j<List.count())&&(Ret==-1)) if (List[j].Name==NameToFind) Ret=j; else j++;
    if (Ret==-1) Ret=0; // If not found : switch to first background
    return Ret;
}

//*********************************************************************************************************************************************
// Object for Brush definition
//*********************************************************************************************************************************************

cBrushDefinition::cBrushDefinition(cBaseApplicationConfig *TheApplicationConfig,cBackgroundList *TheBackgroundList) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::cBrushDefinition";
    #endif
    TypeComposition     =COMPOSITIONTYPE_BACKGROUND;
    BrushType           =BRUSHTYPE_SOLID;               // 0=No brush !, 1=Solid one color, 2=Pattern, 3=Gradient 2 colors, 4=Gradient 3 colors, 5=brush library, 6=image disk
    PatternType         =Qt::Dense4Pattern;             // Type of pattern when BrushType is Pattern (Qt::BrushStyle standard)
    ColorD              ="#C0C0C0";                     // First Color
    ColorIntermed       ="#777777";                     // Intermediate Color
    ColorF              ="#000000";                     // Last Color
    Intermediate        =0.1;                           // Intermediate position of 2nd color (in %)
    GradientOrientation =6;                             // 1=Up-Left, 2=Up, 3=Up-right, ...
    BrushImage          ="";                            // Image name if image from library
    Image               =NULL;
    Video               =NULL;
    SoundVolume         =1;                             // Volume of soundtrack
    ApplicationConfig   =TheApplicationConfig;
    BackgroundList      =TheBackgroundList;
}

//====================================================================================================================
cBrushDefinition::~cBrushDefinition() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::~cBrushDefinition";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::GetBrush";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::GetDefaultFraming";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::GetFramingStyle";
    #endif
    return  QString("###X:%1").arg(X,0,'e',4)+
            QString("###Y:%1").arg(Y,0,'e',4)+
            QString("###ZoomFactor:%1").arg(ZoomFactor,0,'e')+
            QString("###ImageRotation:%1").arg(ImageRotation,0,'e')+
            QString("###AspectRatio:%1").arg(AspectRatio,0,'e');
}

//====================================================================================================================

void cBrushDefinition::InitDefaultFramingStyle(bool LockGeometry,double AspectRatio) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::InitDefaultFramingStyle";
    #endif
    double X,Y,ZoomFactor;

    GetDefaultFraming(ADJUST_WITH,LockGeometry,X,Y,ZoomFactor,AspectRatio);     DefaultFramingW=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
    GetDefaultFraming(ADJUST_HEIGHT,LockGeometry,X,Y,ZoomFactor,AspectRatio);   DefaultFramingH=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
    GetDefaultFraming(ADJUST_FULL,LockGeometry,X,Y,ZoomFactor,AspectRatio);     DefaultFramingF=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
}

//====================================================================================================================

void cBrushDefinition::ApplyStyle(bool LockGeometry,QString Style) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::ApplyStyle";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::GetImageDiskBrush";
    #endif
    if ((Image?Image->FileName:Video?Video->FileName:"")=="") return new QBrush(Qt::NoBrush);

    // W and H = 0 when producing sound track in render process
    bool    SoundOnly=((Rect.width()==0)&&(Rect.height()==0));

    if (!SoundOnly) {
        QImage *RenderImage=(Image?Image->ImageAt(PreviewMode,false,&Image->BrushFileTransform):
                            Video?Video->ImageAt(PreviewMode,Position,StartPosToAdd,false,SoundTrackMontage,SoundVolume,SoundOnly,&Video->BrushFileTransform,false):
                            NULL);

        QBrush *Ret=NULL;
        QImage *Img=NULL;
        if (RenderImage) {
            if (BrushFileCorrect.FullFilling) {
                // Create brush image with distortion
                Img=new QImage(RenderImage->scaled(Rect.width(),Rect.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
            } else {
                // Create brush image with ken burns effect !
                Img=BrushFileCorrect.GetImage(RenderImage,Rect.width(),Rect.height(),PctDone,PreviousBrush?&PreviousBrush->BrushFileCorrect:NULL,ApplicationConfig->Smoothing);
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
            QImage *RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,false,SoundTrackMontage,SoundVolume,SoundOnly,&Video->BrushFileTransform,false);
            delete RenderImage;
        }
        return new QBrush(Qt::NoBrush);
    }
}

//====================================================================================================================

QBrush *cBrushDefinition::GetLibraryBrush(QRectF Rect) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::GetLibraryBrush";
    #endif
    if (!BackgroundList) return NULL;
    int BackgroundImageNumber=BackgroundList->SearchImage(BrushImage);
    if ((BackgroundImageNumber>=0)&&(BackgroundImageNumber<BackgroundList->List.count())) {
        double Ratio=double(BackgroundList->List[BackgroundImageNumber].BackgroundImage.height())/double(BackgroundList->List[BackgroundImageNumber].BackgroundImage.width());
        double H    =Rect.height()+1;
        double W    =H/Ratio;
        QImage NewImg1;
        if (W<(Rect.width()+1)) {
            NewImg1=QImage(BackgroundList->List[BackgroundImageNumber].BackgroundImage.scaledToWidth(Rect.width()+1,Qt::SmoothTransformation));
        } else {
            NewImg1=QImage(BackgroundList->List[BackgroundImageNumber].BackgroundImage.scaledToHeight(Rect.height()+1,Qt::SmoothTransformation));
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::GetHeightForWidth";
    #endif
    double   Ratio=Rect.width()/Rect.height();
    return int(double(double(WantedWith)/Ratio));
}

//====================================================================================================================
// Return width for height depending on Rect geometry
int cBrushDefinition::GetWidthForHeight(int WantedHeight,QRectF Rect) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::GetWidthForHeight";
    #endif
    double   Ratio=Rect.height()/Rect.width();
    return int(double(double(WantedHeight)/Ratio));
}

//====================================================================================================================

QString cBrushDefinition::GetFramingStyle() {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::GetFramingStyle";
    #endif
    return  QString("###X:%1").arg(BrushFileCorrect.X,0,'e')+
            QString("###Y:%1").arg(BrushFileCorrect.Y,0,'e')+
            QString("###ZoomFactor:%1").arg(BrushFileCorrect.ZoomFactor,0,'e')+
            QString("###LockGeometry:%1").arg(BrushFileCorrect.LockGeometry?1:0)+
            QString("###AspectRatio:%1").arg(BrushFileCorrect.AspectRatio,0,'e');
}

//====================================================================================================================
// create a COMPOSITIONTYPE_SHOT brush as a copy of a given brush

void cBrushDefinition::CopyFromBrushDefinition(cBrushDefinition *BrushToCopy) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::CopyFromBrushDefinition";
    #endif
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
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::SaveToXML";
    #endif
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    QString         BrushFileName=(Image?Image->FileName:Video?Video->FileName:"");

    if ((PathForRelativPath!="")&&((Image?Image->FileName:Video?Video->FileName:"")!="")) {
        if (ForceAbsolutPath)
            BrushFileName=QDir::cleanPath(QDir(QFileInfo(PathForRelativPath).absolutePath()).absoluteFilePath(BrushFileName));
        else
            BrushFileName=QDir::cleanPath(QDir(QFileInfo(PathForRelativPath).absolutePath()).relativeFilePath(BrushFileName));
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
                    Video->BrushFileTransform.SaveToXML(Element,"ImageTransformation");                     // Image transformation
                } else Element.setAttribute("SoundVolume",QString("%1").arg(SoundVolume,0,'f'));            // Volume of soundtrack (for video only)
            } else if (Image!=NULL) {
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
                    Element.setAttribute("BrushFileName",BrushFileName);                                    // File name if image from disk
                    Image->BrushFileTransform.SaveToXML(Element,"ImageTransformation");                     // Image transformation
                    cLuLoImageCacheObject *ImageObject=Image->ApplicationConfig->ImagesCache.FindObject(QFileInfo(Image->FileName).absoluteFilePath(),&Image->BrushFileTransform,Image->ApplicationConfig->Smoothing,true); // Get image object
                    if (ImageObject) Element.setAttribute("ImageOrientation",ImageObject->ImageOrientation);
                }
            }
            break;
    }

    BrushFileCorrect.SaveToXML(Element,"ImageCorrection");       // Image correction if image from disk
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cBrushDefinition::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag) {
    #ifdef DEBUGMODE
    qDebug() << "IN:cBrushDefinition::LoadFromXML";
    #endif
    if (ModifyFlag) *ModifyFlag=false;
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
                    for (int i=0;i<ApplicationConfig->AllowImageExtension.count();i++) if (ApplicationConfig->AllowImageExtension[i]==Extension) {
                        Image=new cImageFile(ApplicationConfig);
                        cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(BrushFileName,&Image->BrushFileTransform,ApplicationConfig->Smoothing,true); // Get image object
                        if (ImageObject) {  // File already loaded
                            if (Element.hasAttribute("ImageOrientation")) ImageObject->ImageOrientation=Element.attribute("ImageOrientation").toInt();
                            Image->GetInformationFromFile(BrushFileName,AliasList,NULL);
                            //Image->FileName=QFileInfo(BrushFileName).absoluteFilePath();
                        } else { // File not previously loaded
                            IsValide=Image->GetInformationFromFile(BrushFileName,AliasList,ModifyFlag);
                            if (!IsValide) {
                                delete Image;
                                Image=NULL;
                            }
                        }
                        break;
                    }
                    if (Image==NULL) for (int i=0;i<ApplicationConfig->AllowVideoExtension.count();i++) if (ApplicationConfig->AllowVideoExtension[i]==Extension) {
                        Video=new cVideoFile(false,ApplicationConfig);
                        IsValide=Video->GetInformationFromFile(BrushFileName,AliasList,ModifyFlag);
                        if (!IsValide) {
                            delete Video;
                            Video=NULL;
                        }
                        if (Video!=NULL) Video->OpenCodecAndFile();
                        break;
                    }
                }
                if (Video!=NULL) {
                    if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                    // Global definition only !
                        Video->StartPos =QTime().fromString(Element.attribute("StartPos"));                         // Start position (video only)
                        Video->EndPos   =QTime().fromString(Element.attribute("EndPos"));                           // End position (video only)
                        Video->BrushFileTransform.LoadFromXML(Element,"ImageTransformation");                       // Image transformation
                    } else SoundVolume=Element.attribute("SoundVolume").toDouble();                                 // Volume of soundtrack (for video only)
                } else if (Image!=NULL) {
                    if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                    // Global definition only !
                        Image->BrushFileTransform.LoadFromXML(Element,"ImageTransformation");                       // Image transformation
                    }
                }
                break;
        }

        BrushFileCorrect.LoadFromXML(Element,"ImageCorrection");         // Image correction if image from disk
        return (BrushType==BRUSHTYPE_IMAGEDISK)?(Image!=NULL)||(Video!=NULL):true;
    }
    return false;
}
