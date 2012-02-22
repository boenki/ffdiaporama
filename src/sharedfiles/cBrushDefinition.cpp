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

#include "cBrushDefinition.h"

// Include some common various class
#include "../fmt_filters/fmt_filters.h"

//*********************************************************************************************************************************************
// Utility function to create a gradient brush
//*********************************************************************************************************************************************

QBrush *GetGradientBrush(QRectF Rect,int BrushType,int GradientOrientation,QString ColorD,QString ColorF,QString ColorIntermed,double Intermediate) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:GetGradientBrush");

    QGradient Gradient;
    switch (GradientOrientation) {
        case GRADIENTORIENTATION_UPLEFT:        Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()),QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()));                                                                                             break;          // Up-Left
        case GRADIENTORIENTATION_UP:            Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()),QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()));                                                                            break;          // Up
        case GRADIENTORIENTATION_UPRIGHT:       Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()),QPointF(Rect.x(),Rect.y()+Rect.height()));                                                                                             break;          // Up-right
        case GRADIENTORIENTATION_LEFT:          Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()+Rect.height()/2),QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()/2));                                                                           break;          // Left
        case GRADIENTORIENTATION_RADIAL:        Gradient=QRadialGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()/2),Rect.width()>Rect.height()?Rect.width():Rect.height(),QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()/2));    break;                                                                                                                                      // Radial
        case GRADIENTORIENTATION_RIGHT:         Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()/2),QPointF(Rect.x(),Rect.y()+Rect.height()/2));                                                                           break;          // Right
        case GRADIENTORIENTATION_BOTTOMLEFT:    Gradient=QLinearGradient(QPointF(Rect.x(),Rect.y()+Rect.height()),QPointF(Rect.x()+Rect.width(),Rect.y()));                                                                                             break;          // bt-Left
        case GRADIENTORIENTATION_BOTTOM:        Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width()/2,Rect.y()+Rect.height()),QPointF(Rect.x()+Rect.width()/2,Rect.y()));                                                                            break;          // bottom
        case GRADIENTORIENTATION_BOTTOMRIGHT:   Gradient=QLinearGradient(QPointF(Rect.x()+Rect.width(),Rect.y()+Rect.height()),QPointF(Rect.x(),Rect.y()));                                                                                             break;          // bt-right
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:DrawShape");

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
        case 4 : Painter.drawEllipse(QRectF(left,top,width-1,height-1));                                break;  // Ellipse
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundObject::cBackgroundObject");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundList::cBackgroundList");

    Geometry=-1;
}

//====================================================================================================================

void cBackgroundList::ScanDisk(QString Path,int TheGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundList::ScanDisk");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBackgroundList::SearchImage");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::cBrushDefinition");

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

    // Image correction part
    ImageRotation       =0;                             // Image rotation
    X                   =0;                             // X position (in %) relative to up/left corner
    Y                   =0;                             // Y position (in %) relative to up/left corner
    ZoomFactor          =1;                             // Zoom factor (in %)
    Brightness          =0;
    Contrast            =0;
    Gamma               =1;
    Red                 =0;
    Green               =0;
    Blue                =0;
    LockGeometry        =false;
    AspectRatio         =1;
    FullFilling         =false;
}

//====================================================================================================================
cBrushDefinition::~cBrushDefinition() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::~cBrushDefinition");

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

QBrush *cBrushDefinition::GetBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetBrush");

    switch (BrushType) {
        case BRUSHTYPE_NOBRUSH :        return new QBrush(Qt::NoBrush);
        case BRUSHTYPE_SOLID :          return new QBrush(QColor(ColorD),Qt::SolidPattern);
        case BRUSHTYPE_PATTERN :        return new QBrush(QColor(ColorD),(Qt::BrushStyle)(PatternType+3));
        case BRUSHTYPE_GRADIENT2 :      return GetGradientBrush(Rect,BrushType,GradientOrientation,ColorD,ColorF,ColorIntermed,Intermediate);
        case BRUSHTYPE_GRADIENT3 :      return GetGradientBrush(Rect,BrushType,GradientOrientation,ColorD,ColorF,ColorIntermed,Intermediate);
        case BRUSHTYPE_IMAGELIBRARY :   return GetLibraryBrush(Rect);
        case BRUSHTYPE_IMAGEDISK :      return GetImageDiskBrush(Rect,PreviewMode,Position,StartPosToAdd,SoundTrackMontage,PctDone,PreviousBrush,UseBrushCache);
    }
    return new QBrush(Qt::NoBrush);
}

//====================================================================================================================

void cBrushDefinition::GetDefaultFraming(FramingType TheFramingType,bool LockGeometry,double &X,double &Y,double &ZoomFactor,double &AspectRatio) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetDefaultFraming");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetFramingStyle");

    return  QString("###X:%1").arg(X,0,'e',4)+
            QString("###Y:%1").arg(Y,0,'e',4)+
            QString("###ZoomFactor:%1").arg(ZoomFactor,0,'e')+
            QString("###ImageRotation:%1").arg(ImageRotation,0,'e')+
            QString("###AspectRatio:%1").arg(AspectRatio,0,'e');
}

//====================================================================================================================

void cBrushDefinition::InitDefaultFramingStyle(bool LockGeometry,double AspectRatio) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::InitDefaultFramingStyle");

    double X,Y,ZoomFactor;

    GetDefaultFraming(ADJUST_WITH,LockGeometry,X,Y,ZoomFactor,AspectRatio);     DefaultFramingW=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
    GetDefaultFraming(ADJUST_HEIGHT,LockGeometry,X,Y,ZoomFactor,AspectRatio);   DefaultFramingH=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
    GetDefaultFraming(ADJUST_FULL,LockGeometry,X,Y,ZoomFactor,AspectRatio);     DefaultFramingF=GetFramingStyle(X,Y,ZoomFactor,AspectRatio,0);
}

//====================================================================================================================

void cBrushDefinition::ApplyStyle(bool LockGeometry,QString Style) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::ApplyStyle");

    QStringList List;

    // String to StringList
    while (Style.contains("###")) {
        List.append(Style.left(Style.indexOf("###")));
        Style=Style.mid(Style.indexOf("###")+QString("###").length());
    }
    if (!Style.isEmpty()) List.append(Style);

    // Apply
    LockGeometry=LockGeometry;
    for (int i=0;i<List.count();i++) {
        if      (List[i].startsWith("X:"))              X             =List[i].mid(QString("X:").length()).toDouble();
        else if (List[i].startsWith("Y:"))              Y             =List[i].mid(QString("Y:").length()).toDouble();
        else if (List[i].startsWith("ZoomFactor:"))     ZoomFactor    =List[i].mid(QString("ZoomFactor:").length()).toDouble();
        else if (List[i].startsWith("ImageRotation:"))  ImageRotation =List[i].mid(QString("ImageRotation:").length()).toDouble();
        else if (List[i].startsWith("AspectRatio:"))    AspectRatio   =List[i].mid(QString("AspectRatio:").length()).toDouble();

    }
}

//====================================================================================================================

QBrush *cBrushDefinition::GetImageDiskBrush(QRectF Rect,bool PreviewMode,int Position,int StartPosToAdd,cSoundBlockList *SoundTrackMontage,double PctDone,cBrushDefinition *PreviousBrush,bool UseBrushCache) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetImageDiskBrush");

    // If not an image or a video or filename is empty then return
    if ((Image?Image->FileName:Video?Video->FileName:"")=="") return new QBrush(Qt::NoBrush);

    // W and H = 0 when producing sound track in render process
    bool    SoundOnly=((Rect.width()==0)&&(Rect.height()==0));

    if (!SoundOnly) {
        QImage *RenderImage=NULL;
        if (Video) {
            // Only slide dialog set UseBrushCache to true => use LuloImageCache to Cache rendered image
            if (UseBrushCache) {
                QImage                *LN_Image=NULL;
                cLuLoImageCacheObject *ImageObject=ApplicationConfig->ImagesCache.FindObject(Video,Position+StartPosToAdd,(!PreviewMode || ApplicationConfig->Smoothing),true);
                if (PreviewMode) LN_Image=ImageObject->CachePreviewImage;
                    else         LN_Image=ImageObject->CacheRenderImage;
                if (LN_Image) RenderImage=new QImage(LN_Image->copy());
                    else RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,SoundVolume,SoundOnly,&Video->BrushFileTransform,false);
                if (!LN_Image) {
                    if (PreviewMode)    ImageObject->CachePreviewImage=new QImage(RenderImage->copy());
                        else            ImageObject->CacheRenderImage=new QImage(RenderImage->copy());
                }
            } else RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,SoundVolume,SoundOnly,&Video->BrushFileTransform,false);
        } else if (Image) RenderImage=Image->ImageAt(PreviewMode,&Image->BrushFileTransform);

        QBrush *Ret=NULL;

        if (RenderImage) {
            if (FullFilling) {
                // Create brush image with distortion
                Ret=new QBrush(RenderImage->scaled(Rect.width(),Rect.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
                delete RenderImage;
                RenderImage=NULL;
            } else {
                // Create brush image with ken burns effect !
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
                if (PreviousBrush) {
                    if (PreviousBrush->X!=TheXFactor)                  TheXFactor      =PreviousBrush->X+(TheXFactor-PreviousBrush->X)*PctDone;
                    if (PreviousBrush->Y!=TheYFactor)                  TheYFactor      =PreviousBrush->Y+(TheYFactor-PreviousBrush->Y)*PctDone;
                    if (PreviousBrush->ZoomFactor!=TheZoomFactor)      TheZoomFactor   =PreviousBrush->ZoomFactor+(TheZoomFactor-PreviousBrush->ZoomFactor)*PctDone;
                    if (PreviousBrush->ImageRotation!=TheRotateFactor) TheRotateFactor =PreviousBrush->ImageRotation+(TheRotateFactor-PreviousBrush->ImageRotation)*PctDone;
                    if (PreviousBrush->Brightness!=TheBrightness)      TheBrightness   =PreviousBrush->Brightness+(TheBrightness-PreviousBrush->Brightness)*PctDone;
                    if (PreviousBrush->Contrast!=TheContrast)          TheContrast     =PreviousBrush->Contrast+(TheContrast-PreviousBrush->Contrast)*PctDone;
                    if (PreviousBrush->Gamma!=TheGamma)                TheGamma        =PreviousBrush->Gamma+(TheGamma-PreviousBrush->Gamma)*PctDone;
                    if (PreviousBrush->Red!=TheRed)                    TheRed          =PreviousBrush->Red+(TheRed-PreviousBrush->Red)*PctDone;
                    if (PreviousBrush->Green!=TheGreen)                TheGreen        =PreviousBrush->Green+(TheGreen-PreviousBrush->Green)*PctDone;
                    if (PreviousBrush->Blue!=TheBlue)                  TheBlue         =PreviousBrush->Blue+(TheBlue-PreviousBrush->Blue)*PctDone;
                    if (PreviousBrush->AspectRatio!=TheAspectRatio)    TheAspectRatio  =PreviousBrush->AspectRatio+(TheAspectRatio-PreviousBrush->AspectRatio)*PctDone;
                }

                // Prepare values from sourceimage size
                double   RealImageW=double(RenderImage->width());               // Get real image widht
                double   RealImageH=double(RenderImage->height());              // Get real image height
                double   Hyp=sqrt(RealImageW*RealImageW+RealImageH*RealImageH);     // Calc hypothenuse of the image to define full canvas
                int      iHyp=int(Hyp);                     if (Hyp-iHyp>0.5d) iHyp++;              if (int(iHyp/2)*2<iHyp) iHyp--;     Hyp=iHyp;
                int      HypPixel=int(Hyp*TheZoomFactor);   if (Hyp-HypPixel>0.5d) HypPixel++;

                // Expand canvas
                QImage   NewRenderImage(Hyp,Hyp,QImage::Format_ARGB32_Premultiplied);
                QPainter Painter;
                Painter.begin(&NewRenderImage);
                Painter.setCompositionMode(QPainter::CompositionMode_Source);
                Painter.fillRect(QRect(0,0,Hyp+1,Hyp+1),Qt::transparent);
                Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                Painter.drawImage(QPoint((Hyp-RealImageW)/2,(Hyp-RealImageH)/2),*RenderImage);
                Painter.end();
                delete RenderImage;

                // Rotate image (if needed)
                if (TheRotateFactor!=0) {
                    QTransform matrix;
                    matrix.rotate(TheRotateFactor,Qt::ZAxis);
                    int W=NewRenderImage.width();
                    int H=NewRenderImage.height();
                    NewRenderImage=NewRenderImage.transformed(matrix,ApplicationConfig->Smoothing?Qt::SmoothTransformation:Qt::FastTransformation);
                    int ax=NewRenderImage.width()-W;
                    int ay=NewRenderImage.height()-H;
                    NewRenderImage=NewRenderImage.copy(ax/2,ay/2,NewRenderImage.width()-ax+1,NewRenderImage.height()-ay+1);
                }

                // Get part we need and scaled it to destination size
                NewRenderImage=NewRenderImage.copy(Hyp*TheXFactor,Hyp*TheYFactor,HypPixel,HypPixel*TheAspectRatio)
                                    .scaled(Rect.width()+1,double(Rect.width()+1)*TheAspectRatio,Qt::IgnoreAspectRatio,
                                    ApplicationConfig->Smoothing?Qt::SmoothTransformation:Qt::FastTransformation);

                // Apply correction filters to DestImage
                fmt_filters::image img(NewRenderImage.bits(),NewRenderImage.width(),NewRenderImage.height());
                if (TheBrightness!=0)                           fmt_filters::brightness(img,TheBrightness);
                if (TheContrast!=0)                             fmt_filters::contrast(img,TheContrast);
                if (TheGamma!=1)                                fmt_filters::gamma(img,TheGamma);
                if ((TheRed!=0)||(TheGreen!=0)||(TheBlue!=0))   fmt_filters::colorize(img,TheRed,TheGreen,TheBlue);

                if (!NewRenderImage.isNull()) Ret=new QBrush(NewRenderImage);
            }
        }
        return Ret;
    } else {
        // Force loading of sound of video
        if (Video) {
            QImage *RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,SoundVolume,SoundOnly,&Video->BrushFileTransform,false);
            if (RenderImage) delete RenderImage;
        }
        return new QBrush(Qt::NoBrush);
    }
}

//====================================================================================================================
// Note:This function is use only by DlgImageCorrection !
void cBrushDefinition::ApplyFilter(QImage *Image) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::ApplyFilter");

    if (Image==NULL) return;
    fmt_filters::image img(Image->bits(),Image->width(),Image->height());
    if (Brightness!=0)                      fmt_filters::brightness(img,Brightness);
    if (Contrast!=0)                        fmt_filters::contrast(img,Contrast);
    if (Gamma!=1)                           fmt_filters::gamma(img,Gamma);
    if ((Red!=0)||(Green!=0)||(Blue!=0))    fmt_filters::colorize(img,Red,Green,Blue);
}

//====================================================================================================================

QBrush *cBrushDefinition::GetLibraryBrush(QRectF Rect) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetLibraryBrush");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetHeightForWidth");

    double   Ratio=Rect.width()/Rect.height();
    return int(double(double(WantedWith)/Ratio));
}

//====================================================================================================================
// Return width for height depending on Rect geometry
int cBrushDefinition::GetWidthForHeight(int WantedHeight,QRectF Rect) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetWidthForHeight");

    double   Ratio=Rect.height()/Rect.width();
    return int(double(double(WantedHeight)/Ratio));
}

//====================================================================================================================

QString cBrushDefinition::GetFramingStyle() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::GetFramingStyle");

    return  QString("###X:%1").arg(X,0,'e')+
            QString("###Y:%1").arg(Y,0,'e')+
            QString("###ZoomFactor:%1").arg(ZoomFactor,0,'e')+
            QString("###LockGeometry:%1").arg(LockGeometry?1:0)+
            QString("###AspectRatio:%1").arg(AspectRatio,0,'e');
}

//====================================================================================================================
// create a COMPOSITIONTYPE_SHOT brush as a copy of a given brush

void cBrushDefinition::CopyFromBrushDefinition(cBrushDefinition *BrushToCopy) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::CopyFromBrushDefinition");

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
    DefaultFramingW     =BrushToCopy->DefaultFramingW;
    DefaultFramingH     =BrushToCopy->DefaultFramingH;
    DefaultFramingF     =BrushToCopy->DefaultFramingF;

    // Image correction part
    ImageRotation       =BrushToCopy->ImageRotation;
    X                   =BrushToCopy->X;
    Y                   =BrushToCopy->Y;
    ZoomFactor          =BrushToCopy->ZoomFactor;
    Brightness          =BrushToCopy->Brightness;
    Contrast            =BrushToCopy->Contrast;
    Gamma               =BrushToCopy->Gamma;
    Red                 =BrushToCopy->Red;
    Green               =BrushToCopy->Green;
    Blue                =BrushToCopy->Blue;
    LockGeometry        =BrushToCopy->LockGeometry;
    FullFilling         =BrushToCopy->FullFilling;
    AspectRatio         =BrushToCopy->AspectRatio;
}

//====================================================================================================================

void cBrushDefinition::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::SaveToXML");

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
                    Element.setAttribute("StartPos",Video->StartPos.toString("HH:mm:ss.zzz"));              // Start position (video only)
                    Element.setAttribute("EndPos",Video->EndPos.toString("HH:mm:ss.zzz"));                  // End position (video only)
                    Video->BrushFileTransform.SaveToXML(Element,"ImageTransformation");                     // Image transformation
                } else Element.setAttribute("SoundVolume",QString("%1").arg(SoundVolume,0,'f'));            // Volume of soundtrack (for video only)
            } else if (Image!=NULL) {
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
                    Element.setAttribute("BrushFileName",BrushFileName);                                    // File name if image from disk
                    Image->BrushFileTransform.SaveToXML(Element,"ImageTransformation");                     // Image transformation
                    Element.setAttribute("ImageOrientation",Image->ImageOrientation);
                }
            }
            break;
    }

    // Image correction part
    QDomElement CorrectElement=DomDocument.createElement("ImageCorrection");
    CorrectElement.setAttribute("X",               X);                 // X position (in %) relative to up/left corner
    CorrectElement.setAttribute("Y",               Y);                 // Y position (in %) relative to up/left corner
    CorrectElement.setAttribute("ZoomFactor",      ZoomFactor);        // Zoom factor (in %)
    CorrectElement.setAttribute("ImageRotation",   ImageRotation);     // Image rotation (in °)
    CorrectElement.setAttribute("Brightness",      Brightness);
    CorrectElement.setAttribute("Contrast",        Contrast);
    CorrectElement.setAttribute("Gamma",           Gamma);
    CorrectElement.setAttribute("Red",             Red);
    CorrectElement.setAttribute("Green",           Green);
    CorrectElement.setAttribute("Blue",            Blue);
    CorrectElement.setAttribute("LockGeometry",    LockGeometry?1:0);
    CorrectElement.setAttribute("AspectRatio",     AspectRatio);
    CorrectElement.setAttribute("FullFilling",     FullFilling?1:0);
    Element.appendChild(CorrectElement);

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cBrushDefinition::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::LoadFromXML");

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
                        Image->ImageOrientation=Element.attribute("ImageOrientation").toInt();
                        IsValide=Image->GetInformationFromFile(BrushFileName,AliasList,ModifyFlag);
                        if (!IsValide) {
                            delete Image;
                            Image=NULL;
                        }
                        break;
                    }
                    if (Image==NULL) for (int i=0;i<ApplicationConfig->AllowVideoExtension.count();i++) if (ApplicationConfig->AllowVideoExtension[i]==Extension) {
                        Video=new cVideoFile(OBJECTTYPE_VIDEOFILE,ApplicationConfig);
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

        // Image correction part
        if ((Element.elementsByTagName("ImageCorrection").length()>0)&&(Element.elementsByTagName("ImageCorrection").item(0).isElement()==true)) {
            QDomElement CorrectElement=Element.elementsByTagName("ImageCorrection").item(0).toElement();

            if (CorrectElement.hasAttribute("X"))              X               =CorrectElement.attribute("X").toDouble();                      // X position (in %) relative to up/left corner
            if (CorrectElement.hasAttribute("Y"))              Y               =CorrectElement.attribute("Y").toDouble();                      // Y position (in %) relative to up/left corner
            if (CorrectElement.hasAttribute("ZoomFactor"))     ZoomFactor      =CorrectElement.attribute("ZoomFactor").toDouble();             // Zoom factor (in %)
            if (CorrectElement.hasAttribute("ImageRotation"))  ImageRotation   =CorrectElement.attribute("ImageRotation").toDouble();          // Image rotation (in °)
            if (CorrectElement.hasAttribute("Brightness"))     Brightness      =CorrectElement.attribute("Brightness").toInt();
            if (CorrectElement.hasAttribute("Contrast"))       Contrast        =CorrectElement.attribute("Contrast").toInt();
            if (CorrectElement.hasAttribute("Gamma"))          Gamma           =CorrectElement.attribute("Gamma").toDouble();
            if (CorrectElement.hasAttribute("Red"))            Red             =CorrectElement.attribute("Red").toInt();
            if (CorrectElement.hasAttribute("Green"))          Green           =CorrectElement.attribute("Green").toInt();
            if (CorrectElement.hasAttribute("Blue"))           Blue            =CorrectElement.attribute("Blue").toInt();
            if (CorrectElement.hasAttribute("AspectRatio"))    AspectRatio     =CorrectElement.attribute("AspectRatio").toDouble();
            if (CorrectElement.hasAttribute("FullFilling"))    FullFilling     =CorrectElement.attribute("FullFilling").toInt()==1;

            // If old ImageGeometry value in project file then compute LockGeometry
            if (CorrectElement.hasAttribute("ImageGeometry"))           LockGeometry=(CorrectElement.attribute("ImageGeometry").toInt()!=2);
                else if (CorrectElement.hasAttribute("LockGeometry"))   LockGeometry=(CorrectElement.attribute("LockGeometry").toInt()==1); // Else load saved value

        }

        return (BrushType==BRUSHTYPE_IMAGEDISK)?(Image!=NULL)||(Video!=NULL):true;
    }
    return false;
}
