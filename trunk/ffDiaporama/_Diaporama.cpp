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
#include "_Diaporama.h"

#include "mainwindow.h"

//============================================
// Global static
//============================================

double  ADJUST_RATIO=1;   // Adjustement ratio for pixel size (all size are given for full hd and adjust for real wanted size)

//====================================================================================================================
// Utility function to draw a shape
//====================================================================================================================

void DrawShape(QPainter &Painter,int BackgroundForm,double left,double top,double width,double height,double CenterX,double CenterY) {
    double RayX=0,RayY=0;

    switch (BackgroundForm) {
        //0 = no shape
        case 1 : Painter.drawRect(QRectF(left,top,width-1,height-1));                                   break;  // Rectangle
        case 2 :                                                                                                // Round rect
            RayX=width/10;     if (RayX>16) RayX=16; else if (RayX<8)  RayX=8;
            RayY=height/10;    if (RayY>16) RayY=16; else if (RayY<8)  RayY=8;
            Painter.drawRoundedRect(QRectF(left,top,width-1,height-1),RayX,RayY,Qt::AbsoluteSize);
            break;
        case 3 :                                                                                                // Buble
            RayX=2*width/10;
            RayY=2*height/10;
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

cCompositionObject::cCompositionObject(int TheTypeComposition,int TheIndexKey) {
    // Attribut of the text object
    ZValue                  = 500;
    TypeComposition         = TheTypeComposition;
    IndexKey                = TheIndexKey;

    x                       = 0.25;         // Position (x,y) and size (width,height)
    y                       = 0.25;
    w                       = 0.5;
    h                       = 0.5;

    RotateZAxis             = 0;            // Rotation from Z axis
    RotateXAxis             = 0;            // Rotation from X axis
    RotateYAxis             = 0;            // Rotation from Y axis

    // Text part
    Text                    = "";           // Text of the object
    FontName                = DEFAULT_FONT_FAMILLY;                             // font name
    FontSize                = DEFAULT_FONT_SIZE;                                // font size
    FontColor               = DEFAULT_FONT_COLOR;                               // font color
    FontShadowColor         = DEFAULT_FONT_SHADOWCOLOR;                         // font shadow color
    IsBold                  = DEFAULT_FONT_ISBOLD;                              // true if bold mode
    IsItalic                = DEFAULT_FONT_ISITALIC;                            // true if Italic mode
    IsUnderline             = DEFAULT_FONT_ISUNDERLINE;                         // true if Underline mode
    HAlign                  = DEFAULT_FONT_HALIGN;                              // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
    VAlign                  = DEFAULT_FONT_VALIGN;                              // Vertical alignement : 0=up, 1=center, 2=bottom
    StyleText               = DEFAULT_FONT_TEXTEFFECT;                          // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right

    // Shap part
    BackgroundForm          = 1;                                                // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
    Opacity                 = DEFAULT_SHAPE_OPACITY;                            // Style of the background of the form
    PenSize                 = DEFAULT_SHAPE_BORDERSIZE;                         // Width of the border of the form
    PenStyle                = Qt::SolidLine;                                    // Style of the pen border of the form
    PenColor                = DEFAULT_SHAPE_BORDERCOLOR;                        // Color of the border of the form
    FormShadow              = 0;                                                // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    FormShadowDistance      = 5;                                                // Distance from form to shadow

    // BackgroundBrush is initilise by object constructor except TypeComposition and key
    BackgroundBrush.TypeComposition = TypeComposition;
}

//====================================================================================================================

void cCompositionObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    Element.setAttribute("TypeComposition",TypeComposition);
    Element.setAttribute("IndexKey",IndexKey);

    // Attribut of the object
    Element.setAttribute("ZValue",ZValue);                      // Z value ordering (low is background)
    Element.setAttribute("x",x);                                // Position x
    Element.setAttribute("y",y);                                // Position x
    Element.setAttribute("w",w);                                // size width
    Element.setAttribute("h",h);                                // size height
    Element.setAttribute("RotateZAxis",RotateZAxis);            // Rotation from Z axis
    Element.setAttribute("RotateXAxis",RotateXAxis);            // Rotation from X axis
    Element.setAttribute("RotateYAxis",RotateYAxis);            // Rotation from Y axis
    Element.setAttribute("BackgroundTransparent",Opacity);      // Opacity of the form

    // Text part
    Element.setAttribute("Text",Text);                          // Text of the object
    Element.setAttribute("FontName",FontName);                  // font name
    Element.setAttribute("FontSize",FontSize);                  // font size
    Element.setAttribute("FontColor",FontColor);                // font color
    Element.setAttribute("FontShadowColor",FontShadowColor);    // font shadow color
    Element.setAttribute("IsBold",IsBold?"1":"0");              // true if bold mode
    Element.setAttribute("IsItalic",IsItalic?"1":"0");          // true if Italic mode
    Element.setAttribute("IsUnderline",IsUnderline?"1":"0");    // true if Underline mode
    Element.setAttribute("HAlign",HAlign);                      // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
    Element.setAttribute("VAlign",VAlign);                      // Vertical alignement : 0=up, 1=center, 2=bottom
    Element.setAttribute("StyleText",StyleText);                // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right

    // Shap part
    Element.setAttribute("BackgroundForm",BackgroundForm);                      // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
    Element.setAttribute("PenSize",PenSize);                                    // Width of the border of the form
    Element.setAttribute("PenStyle",PenStyle);                                  // Style of the pen border of the form
    Element.setAttribute("PenColor",PenColor);                                  // Color of the border of the form
    Element.setAttribute("FormShadow",FormShadow);                              // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    Element.setAttribute("FormShadowDistance",FormShadowDistance);              // Distance from form to shadow
    BackgroundBrush.SaveToXML(Element,"BackgroundBrush",PathForRelativPath);    // Brush of the background of the form

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cCompositionObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        TypeComposition     =Element.attribute("TypeComposition").toInt();
        IndexKey            =Element.attribute("IndexKey").toInt();

        // Attribut of the object
        ZValue              =Element.attribute("ZValue").toInt();                   // Z value ordering (low is background)
        x                   =Element.attribute("x").toDouble();                     // Position x
        y                   =Element.attribute("y").toDouble();                     // Position x
        w                   =Element.attribute("w").toDouble();                     // size width
        h                   =Element.attribute("h").toDouble();                     // size height
        Opacity             =Element.attribute("BackgroundTransparent").toInt();    // Style Opacity of the background of the form
        RotateZAxis         =Element.attribute("RotateZAxis").toDouble();           // Rotation from Z axis
        RotateXAxis         =Element.attribute("RotateXAxis").toDouble();           // Rotation from X axis
        RotateYAxis         =Element.attribute("RotateYAxis").toDouble();           // Rotation from Y axis

        // Text part
        Text                =Element.attribute("Text");                             // Text of the object
        FontName            =Element.attribute("FontName");                         // font name
        FontSize            =Element.attribute("FontSize").toInt();                 // font size
        FontColor           =Element.attribute("FontColor");                        // font color
        FontShadowColor     =Element.attribute("FontShadowColor");                  // font shadow color
        IsBold              =Element.attribute("IsBold")=="1";                      // true if bold mode
        IsItalic            =Element.attribute("IsItalic")=="1";                    // true if Italic mode
        IsUnderline         =Element.attribute("IsUnderline")=="1";                 // true if Underline mode
        HAlign              =Element.attribute("HAlign").toInt();                   // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
        VAlign              =Element.attribute("VAlign").toInt();                   // Vertical alignement : 0=up, 1=center, 2=bottom
        StyleText           =Element.attribute("StyleText").toInt();                // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right

        // Shap part
        BackgroundForm      =Element.attribute("BackgroundForm").toInt();           // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
        PenSize             =Element.attribute("PenSize").toInt();                  // Width of the border of the form
        PenStyle            =Element.attribute("PenStyle").toInt();                 // Style of the pen border of the form
        PenColor            =Element.attribute("PenColor");                         // Color of the border of the form
        FormShadow          =Element.attribute("FormShadow").toInt();               // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
        FormShadowDistance  =Element.attribute("FormShadowDistance").toInt();       // Distance from form to shadow

        // Compatibility with old release : remove case BackgroundForm==0
        if (BackgroundForm==0) {
            BackgroundForm           =1;        // Set to rectangle
            PenSize                  =0;        // border=0
            BackgroundBrush.BrushType=0;        // brushtype=no brush
        } else BackgroundBrush.LoadFromXML(Element,"BackgroundBrush",PathForRelativPath);  // Brush of the background of the form

        return true;
    }
    return false;
}

//====================================================================================================================

void cCompositionObject::CopyFromCompositionObject(cCompositionObject *CompositionObjectToCopy) {
    ZValue               =CompositionObjectToCopy->ZValue;
    x                    =CompositionObjectToCopy->x;
    y                    =CompositionObjectToCopy->y;
    w                    =CompositionObjectToCopy->w;
    h                    =CompositionObjectToCopy->h;
    RotateZAxis          =CompositionObjectToCopy->RotateZAxis;
    RotateXAxis          =CompositionObjectToCopy->RotateXAxis;
    RotateYAxis          =CompositionObjectToCopy->RotateYAxis;
    Opacity              =CompositionObjectToCopy->Opacity;
    Text                 =CompositionObjectToCopy->Text;
    FontName             =CompositionObjectToCopy->FontName;
    FontSize             =CompositionObjectToCopy->FontSize;
    FontColor            =CompositionObjectToCopy->FontColor;
    FontShadowColor      =CompositionObjectToCopy->FontShadowColor;
    IsBold               =CompositionObjectToCopy->IsBold;
    IsItalic             =CompositionObjectToCopy->IsItalic;
    IsUnderline          =CompositionObjectToCopy->IsUnderline;
    HAlign               =CompositionObjectToCopy->HAlign;
    VAlign               =CompositionObjectToCopy->VAlign;
    StyleText            =CompositionObjectToCopy->StyleText;
    BackgroundForm       =CompositionObjectToCopy->BackgroundForm;
    PenSize              =CompositionObjectToCopy->PenSize;
    PenStyle             =CompositionObjectToCopy->PenStyle;
    PenColor             =CompositionObjectToCopy->PenColor;
    FormShadow           =CompositionObjectToCopy->FormShadow;
    FormShadowDistance   =CompositionObjectToCopy->FormShadowDistance;

    BackgroundBrush.CopyFromBrushDefinition(&CompositionObjectToCopy->BackgroundBrush);
}

//====================================================================================================================

void cCompositionObject::DrawCompositionObject(QPainter &DestPainter,int AddX,int AddY,int width,int height,bool PreviewMode,int Position,cSoundBlockList *SoundTrackMontage,double PctDone,cCompositionObject *PrevCompoObject) {
    // W and H = 0 when producing sound track in render process
    bool    SoundOnly=((width==0)&&(height==0));

    if (SoundOnly) {
        // if SoundOnly then load Brush of type BRUSHTYPE_IMAGEDISK to SoundTrackMontage
        if (BackgroundBrush.BrushType==BRUSHTYPE_IMAGEDISK) {
            QBrush *BR=BackgroundBrush.GetBrush(QRectF(0,0,0,0),PreviewMode,Position,SoundTrackMontage,1,NULL);
            delete BR;
        }
    } else {
        // Define values depending on PctDone and PrevCompoObject
        double TheX=x;
        double TheY=y;
        double TheW=w;
        double TheH=h;
        double TheRotateZAxis=RotateZAxis;
        double TheRotateXAxis=RotateXAxis;
        double TheRotateYAxis=RotateYAxis;
        if (PrevCompoObject) {
            if (PrevCompoObject->x!=TheX)                       TheX          =PrevCompoObject->x+(TheX-PrevCompoObject->x)*PctDone;
            if (PrevCompoObject->y!=TheY)                       TheY          =PrevCompoObject->y+(TheY-PrevCompoObject->y)*PctDone;
            if (PrevCompoObject->w!=TheW)                       TheW          =PrevCompoObject->w+(TheW-PrevCompoObject->w)*PctDone;
            if (PrevCompoObject->h!=TheH)                       TheH          =PrevCompoObject->h+(TheH-PrevCompoObject->h)*PctDone;
            if (PrevCompoObject->RotateZAxis!=TheRotateZAxis)   TheRotateZAxis=PrevCompoObject->RotateZAxis+(TheRotateZAxis-PrevCompoObject->RotateZAxis)*PctDone;
            if (PrevCompoObject->RotateXAxis!=TheRotateXAxis)   TheRotateXAxis=PrevCompoObject->RotateXAxis+(TheRotateXAxis-PrevCompoObject->RotateXAxis)*PctDone;
            if (PrevCompoObject->RotateYAxis!=TheRotateYAxis)   TheRotateYAxis=PrevCompoObject->RotateYAxis+(TheRotateYAxis-PrevCompoObject->RotateYAxis)*PctDone;
        }

        QPen    Pen;
        double  FullMargin=0;
        double  W=TheW*double(width);
        double  H=TheH*double(height);
        double  Wb=sqrt(W*W+H*H);
        double  Hb=Wb*H/W;
        if (Hb<W) {
            Hb=Wb;
            Wb=Hb*W/H;
        }

        Wb=Wb+double(PenSize)*ADJUST_RATIO*2;
        Hb=Hb+double(PenSize)*ADJUST_RATIO*2;
        AddX-=(Wb-W)/2;
        AddY-=(Hb-H)/2;
        QImage   Img(Wb+2,Hb+2,QImage::Format_ARGB32_Premultiplied);
        QPainter Painter;
        Painter.begin(&Img);
        Painter.setCompositionMode(QPainter::CompositionMode_Source);
        Painter.fillRect(QRect(0,0,Wb+2,Hb+2),Qt::transparent);
        Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        Painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
        Pen.setCapStyle(Qt::RoundCap);
        Pen.setJoinStyle(Qt::RoundJoin);
        Pen.setCosmetic(false);

        Pen.setStyle(Qt::SolidLine);

        // All coordonates from center
        QTransform  Matrix;
        Matrix.translate(W/2+(Wb-W)/2,H/2+(Hb-H)/2);
        if (TheRotateZAxis!=0) Matrix.rotate(TheRotateZAxis,Qt::ZAxis);   // Standard axis
        if (TheRotateXAxis!=0) Matrix.rotate(TheRotateXAxis,Qt::XAxis);   // Rotate from X axis
        if (TheRotateYAxis!=0) Matrix.rotate(TheRotateYAxis,Qt::YAxis);   // Rotate from Y axis
        Painter.setWorldTransform(Matrix,false);

        // Draw ExternalBorder border
        if (PenSize==0) Painter.setPen(Qt::NoPen); else {
            Pen.setColor(PenColor);
            FullMargin=double(PenSize)*ADJUST_RATIO/2;
            Pen.setWidthF(double(PenSize)*ADJUST_RATIO);
            Pen.setStyle((Qt::PenStyle)PenStyle);
            Painter.setPen(Pen);
        }
        // Draw internal shape
        if (BackgroundBrush.BrushType==BRUSHTYPE_NOBRUSH) Painter.setBrush(Qt::transparent); else {
            // Create brush with Ken Burns effect !
            QBrush      *BR=BackgroundBrush.GetBrush(QRectF(-1,-1,W-FullMargin*2,H-FullMargin*2),PreviewMode,Position,SoundTrackMontage,PctDone,PrevCompoObject?&PrevCompoObject->BackgroundBrush:NULL);
            QTransform  MatrixBR;
            MatrixBR.translate(FullMargin-W/2,FullMargin-H/2);
            BR->setTransform(MatrixBR);  // Apply transforme matrix to the brush
            Painter.setBrush(*BR);
            delete BR;
        }
        if (BackgroundBrush.BrushType==BRUSHTYPE_NOBRUSH) Painter.setCompositionMode(QPainter::CompositionMode_Source);
        DrawShape(Painter,BackgroundForm,FullMargin-W/2,FullMargin-H/2,W-FullMargin*2,H-FullMargin*2,0,0);
        if (BackgroundBrush.BrushType==BRUSHTYPE_NOBRUSH) Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        Painter.setPen(Qt::NoPen);
        FullMargin=FullMargin*2;

        // Create font and TextOption
        QTextOption OptionText;
        QFont       font=QFont(FontName,10,IsBold?QFont::Bold:QFont::Normal,IsItalic?QFont::StyleItalic:QFont::StyleNormal);        // FontSize is always 10 and size if given with setPointSizeF !
        font.setUnderline(IsUnderline);                                                                                             // Set underline
        font.setPointSizeF((double(width)/double(SCALINGTEXTFACTOR))*double(FontSize));                                             // Scale font
        OptionText=QTextOption(((HAlign==0)?Qt::AlignLeft:(HAlign==1)?Qt::AlignHCenter:(HAlign==2)?Qt::AlignRight:Qt::AlignJustify) // Setup horizontal alignement
                    |(VAlign==0?Qt::AlignTop:VAlign==1?Qt::AlignVCenter:Qt::AlignBottom));                                          // Setup vertical alignement
        OptionText.setWrapMode(QTextOption::WordWrap);                                                                              // Setup word wrap text option

        double MarginX=FullMargin;
        double MarginY=FullMargin;

        if (BackgroundForm==3) {                        // 3=Buble
            MarginX=MarginX+W/250;
            MarginY=MarginY+H/250;
        } else if (BackgroundForm==4) {                 // 2=Ellipse
            MarginX=MarginX+(0.29*(W/2));               // 0.29=1-cos(radians(45°))
            MarginY=MarginY+(0.29*(H/2));               // 0.29=1-sin(radians(45°))
        }

        // Paint Shadow of the text
        Painter.setFont(font);
        Pen.setColor(FontShadowColor);
        Pen.setWidth(1);
        Pen.setStyle(Qt::SolidLine);
        Painter.setPen(Pen);
        Painter.setBrush(Qt::NoBrush);

        switch (StyleText) {
            case 0 :                // 0=normal
                break;
            case 1 :                // 1=outerline
                Painter.drawText(QRectF(MarginX-1-W/2,MarginY-1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                Painter.drawText(QRectF(MarginX-1-W/2,MarginY+1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                Painter.drawText(QRectF(MarginX+1-W/2,MarginY+1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                Painter.drawText(QRectF(MarginX+1-W/2,MarginY-1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                Painter.drawText(QRectF(MarginX  -W/2,MarginY-1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                Painter.drawText(QRectF(MarginX  -W/2,MarginY+1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                Painter.drawText(QRectF(MarginX-1-W/2,MarginY  -H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                Painter.drawText(QRectF(MarginX+1-W/2,MarginY  -H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                break;
            case 2:                 //2=shadow up-left
                Painter.drawText(QRectF(MarginX-1-W/2,MarginY-1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                break;
            case 3:                 //3=shadow up-right
                Painter.drawText(QRectF(MarginX+1-W/2,MarginY-1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                break;
            case 4:                 //4=shadow bt-left
                Painter.drawText(QRectF(MarginX-1-W/2,MarginY+1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                break;
            case 5:                 //5=shadow bt-right
                Painter.drawText(QRectF(MarginX+1-W/2,MarginY+1-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
                break;
        }

        // Paint text
        Painter.setPen(QColor(FontColor));
        Painter.drawText(QRectF(MarginX-W/2,MarginY-H/2,W-2*MarginX,H-2*MarginY),Text,OptionText);
        Painter.end();

        DestPainter.save();
        if (FormShadow) {
            double Distance=double(FormShadowDistance)*ADJUST_RATIO;
            QImage ImgShadow=Img.copy();
            Uint8  *Data=ImgShadow.bits();
            for (int i=0;i<ImgShadow.height()*ImgShadow.width();i++) {
                *Data++=0;  // R
                *Data++=0;  // G
                *Data++=0;  // B
                Data++;     // Keep Alpha chanel
            }
            DestPainter.setOpacity(Opacity==0?0.75:Opacity==1?0.50:Opacity==2?0.25:0.10);
            switch (FormShadow) {
                case 1  : DestPainter.drawImage(AddX+TheX*double(width)-Distance,AddY+TheY*double(height)-Distance,ImgShadow); break;
                case 2  : DestPainter.drawImage(AddX+TheX*double(width)+Distance,AddY+TheY*double(height)-Distance,ImgShadow); break;
                case 3  : DestPainter.drawImage(AddX+TheX*double(width)-Distance,AddY+TheY*double(height)+Distance,ImgShadow); break;
                default : DestPainter.drawImage(AddX+TheX*double(width)+Distance,AddY+TheY*double(height)+Distance,ImgShadow); break;
            }
        }
        if ((Opacity>0)&&(Opacity<4)) DestPainter.setOpacity(Opacity==1?0.75:Opacity==2?0.50:0.25); else DestPainter.setOpacity(1);
        DestPainter.drawImage(AddX+TheX*double(width),AddY+TheY*double(height),Img);
        DestPainter.restore();
    }
}

//*********************************************************************************************************************************************

cCompositionList::cCompositionList() {
    TypeComposition=COMPOSITIONTYPE_BACKGROUND;
}

//====================================================================================================================

cCompositionList::~cCompositionList() {

}

//====================================================================================================================

void cCompositionList::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    // Save composition list
    Element.setAttribute("TypeComposition",TypeComposition);
    Element.setAttribute("CompositionNumber",List.count());
    for (int i=0;i<List.count();i++) List[i].SaveToXML(Element,"Composition-"+QString("%1").arg(i),PathForRelativPath);
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cCompositionList::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        bool IsOk=true;
        // Load composition list
        List.clear();
        TypeComposition=Element.attribute("TypeComposition").toInt();
        int CompositionNumber=Element.attribute("CompositionNumber").toInt();
        for (int i=0;i<CompositionNumber;i++) {
            cCompositionObject *CompositionObject=new cCompositionObject(TypeComposition,0);    // IndexKey will be load from XML
            if (!CompositionObject->LoadFromXML(Element,"Composition-"+QString("%1").arg(i),PathForRelativPath)) IsOk=false;
            List.append(*CompositionObject);
        }
        return IsOk;
    } else return false;
}

//*********************************************************************************************************************************************
//
// Base class containing scene definition
//
//*********************************************************************************************************************************************

cDiaporamaShot::cDiaporamaShot(cDiaporamaObject *DiaporamaObject) {
    Parent                          = DiaporamaObject;
    StaticDuration                  = Parent->Parent->FixedDuration;    // Duration (in msec) of the static part animation
    MobilDuration                   = Parent->Parent->MobilDuration;    // Duration (in msec) of the mobil part animation
    ShotComposition.TypeComposition = COMPOSITIONTYPE_SHOT;
}

//====================================================================================================================

cDiaporamaShot::~cDiaporamaShot() {
}


//====================================================================================================================

int cDiaporamaShot::GetStaticDuration() {
    return StaticDuration;
}

int cDiaporamaShot::GetMobilDuration() {
    if (Parent->TypeObject==DIAPORAMAOBJECTTYPE_EMPTY) return 0;
    return MobilDuration;
}

//===============================================================

void cDiaporamaShot::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    Element.setAttribute("StaticDuration",StaticDuration);                              // Duration (in msec) of the static part animation
    Element.setAttribute("MobilDuration",MobilDuration);                                // Duration (in msec) of the static part animation
    FilterCorrection.SaveToXML(Element,"FilterCorrection",PathForRelativPath);          // Image correction
    ShotComposition.SaveToXML(Element,"ShotComposition",PathForRelativPath);            // Composition list for this object
    domDocument.appendChild(Element);
}

//===============================================================

bool cDiaporamaShot::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        //==========> Récupération des données !
        if (Element.attribute("DefaultStaticDuration")=="1") StaticDuration=-1;
            else StaticDuration=Element.attribute("StaticDuration").toInt();           // Duration (in msec) of the static part animation

        if (Element.attribute("DefaultMobilDuration")=="1") MobilDuration=Parent->Parent->MobilDuration;
            else MobilDuration=Element.attribute("MobilDuration").toInt();            // Duration (in msec) of the static part animation

        FilterCorrection.LoadFromXML(Element,"FilterCorrection",PathForRelativPath);    // Image correction

        // Recupération des anciennes sauvegardes (De la v0 à la 20110513 )
        if (Element.hasAttribute("X")) {
            FilterCorrection.X                       =Element.attribute("X").toDouble();                      // X position (in %) relative to up/left corner
            FilterCorrection.Y                       =Element.attribute("Y").toDouble();                      // Y position (in %) relative to up/left corner
            FilterCorrection.ZoomFactor              =Element.attribute("ZoomFactor").toDouble();             // Zoom factor (in %)
            FilterCorrection.ImageRotation           =Element.attribute("ImageRotation").toInt();            // Image rotation (in °)
        }

        ShotComposition.LoadFromXML(Element,"ShotComposition",PathForRelativPath);      // Composition list for this object
//
        if (Parent->Video) {
            Parent->Video->StartPos   =QTime().fromString(Element.attribute("StartPos"));   // Start position (video only)
            Parent->Video->EndPos     =QTime().fromString(Element.attribute("EndPos"));     // End position (video only)
        }

        return true;
    }
    return false;
}

//*********************************************************************************************************************************************
//
// Base object for all media type
//
//*********************************************************************************************************************************************

cDiaporamaObject::cDiaporamaObject(cDiaporama *Diaporama) {
    Parent                                  = Diaporama;
    TypeObject                              = DIAPORAMAOBJECTTYPE_EMPTY;
    SlideName                               = QCoreApplication::translate("MainWindow","Title","Default slide name when no file");
    NextIndexKey                            = 1;

    // Set default/initial value
    BackgroundType                          = false;                        // Background type : false=same as precedent - true=new background definition
    BackgroundBrush.BrushType               = BRUSHTYPE_SOLID;
    BackgroundBrush.ColorD                  = "#000000";                    // Background color
    Image                                   = NULL;                         // Embeded Object for title and image type
    Video                                   = NULL;                         // Embeded Object for video type
    MusicType                               = false;                        // Music type : false=same as precedent - true=new playlist definition
    MusicPause                              = false;                        // true if music is pause during this object
    MusicReduceVolume                       = false;                        // true if volume if reduce by MusicReduceFactor
    MusicReduceFactor                       = 0.2;                          // factor for volume reduction if MusicReduceVolume is true
    TransitionFamilly                       = TRANSITIONFAMILLY_BASE;       // Transition familly
    TransitionSubType                       = 0;                            // Transition type in the familly
    TransitionDuration                      = 1000;                         // Transition duration (in msec)
    BackgroundComposition.TypeComposition   = COMPOSITIONTYPE_BACKGROUND;
    ObjectComposition.TypeComposition       = COMPOSITIONTYPE_OBJECT;
    Thumbnail                               = NULL;

    // Add an empty scene
    List.append(cDiaporamaShot(this));
}

//====================================================================================================================

cDiaporamaObject::~cDiaporamaObject() {
    List.clear();
    if (Image!=NULL) {
        delete Image;
        Image=NULL;
    }
    if (Video!=NULL) {
        delete Video;
        Video=NULL;
    }
    if (Thumbnail) {
        delete Thumbnail;
        Thumbnail=NULL;
    }
}

//====================================================================================================================

QString cDiaporamaObject::GetDisplayName() {
    if (Image!=NULL) return QFileInfo(Image->FileName).baseName();
    else if (Video!=NULL) return QFileInfo(Video->FileName).baseName();
    return "";
}

//====================================================================================================================

bool cDiaporamaObject::LoadMedia(QString &filename,int MediaType) {
    // Clean all
    if (Image!=NULL) {
        delete Image;
        Image=NULL;
    }
    if (Video!=NULL) {
        delete Video;
        Video=NULL;
    }

    TypeObject=DIAPORAMAOBJECTTYPE_EMPTY;//MediaType;

    bool IsValide=false;
    switch (MediaType) {
        case DIAPORAMAOBJECTTYPE_EMPTY :
            IsValide=true;
            break;
        case DIAPORAMAOBJECTTYPE_IMAGE :
            Image=new cimagefilewrapper();
            IsValide=Image->GetInformationFromFile(filename);
            break;
        case DIAPORAMAOBJECTTYPE_VIDEO :
            Video=new cvideofilewrapper();
            IsValide=Video->GetInformationFromFile(filename,false);
            Video->StartPos              =QTime(0,0,0,0);
            Video->EndPos                =Video->Duration;
            List[0].StaticDuration       =Video->StartPos.msecsTo(Video->EndPos);
            List[0].MobilDuration        =0;
            break;
    }
    return IsValide;
}

//====================================================================================================================
// Apply Default framing to first shot (used when add new object)
//====================================================================================================================

void cDiaporamaObject::ApplyDefaultFraming(int DefaultFraming) {
    // Ensure image is loaded
    QImage *ReturnImage=NULL;
    double   RealImageW=Parent->InternalWidth;
    double   RealImageH=Parent->InternalHeight;

    if (Video!=NULL)        ReturnImage=Video->ImageAt(true,0,true,NULL,1,false,NULL);  // Video
    else if (Image!=NULL)   ReturnImage=Image->ImageAt(true,true,NULL);                // Image

    if (ReturnImage!=NULL) {
        RealImageW=ReturnImage->width();
        RealImageH=ReturnImage->height();
    }
    delete ReturnImage;

    double   Hyp=sqrt(RealImageW*RealImageW+RealImageH*RealImageH);     // Calc hypothenuse
    double   VirtImageW=Hyp;                                            // Calc canvas size
    double   VirtImageH=Parent->GetHeightForWidth(VirtImageW);          // Calc canvas size
    // Ensure complete image
    if (VirtImageH<Hyp) {
        VirtImageH=Hyp;
        VirtImageW=Parent->GetWidthForHeight(VirtImageH);
    }

    double MagnetX1=(VirtImageW-RealImageW)/2;
    double MagnetX2=MagnetX1+RealImageW;
    double MagnetY1=(VirtImageH-RealImageH)/2;
    double MagnetY2=MagnetY1+RealImageH;

    switch (DefaultFraming) {
        case 0 :    // Adjust to Width
            List[0].FilterCorrection.X=MagnetX1/VirtImageW;
            List[0].FilterCorrection.ZoomFactor=(MagnetX2-MagnetX1)/VirtImageW;
            List[0].FilterCorrection.Y=((VirtImageH-Parent->GetHeightForWidth(MagnetX2-MagnetX1))/2)/VirtImageH;
            break;
        case 1 :    // Adjust to Height
            List[0].FilterCorrection.Y=MagnetY1/VirtImageH;
            List[0].FilterCorrection.ZoomFactor=(MagnetY2-MagnetY1)/VirtImageH;
            List[0].FilterCorrection.X=((VirtImageW-Parent->GetWidthForHeight(MagnetY2-MagnetY1))/2)/VirtImageW;
            break;
        case 2 :    // Adjust to Full
            List[0].FilterCorrection.X=MagnetX1/VirtImageW;
            List[0].FilterCorrection.ZoomFactor=(MagnetX2-MagnetX1)/VirtImageW;
            List[0].FilterCorrection.Y=((VirtImageH-Parent->GetHeightForWidth(MagnetX2-MagnetX1))/2)/VirtImageH;
            if (List[0].FilterCorrection.Y>MagnetY1/VirtImageH) {
                List[0].FilterCorrection.Y=MagnetY1/VirtImageH;
                List[0].FilterCorrection.ZoomFactor=(MagnetY2-MagnetY1)/VirtImageH;
                List[0].FilterCorrection.X=((VirtImageW-Parent->GetWidthForHeight(MagnetY2-MagnetY1))/2)/VirtImageW;
            }
            break;
    }
}

//====================================================================================================================
// Load image from source object at given position(video) and Apply filter if ApplyFilter==true
// return new image (in all case)
// Call by :
//      DlgImageProperties for s_CompositionNeedRefreshBackgroundImage()
//      DlgVideoProperties for s_CompositionNeedRefreshBackgroundImage()
//  and by CanvasImageAt
//====================================================================================================================

QImage  *cDiaporamaObject::GetImageAt(int Position,cSoundBlockList *SoundTrackMontage) {
    QImage  *ReturnImage=NULL;

    // Try to load image from source
    if (Video!=NULL) ReturnImage=Video->ImageAt(true,Position,false,SoundTrackMontage,1,false,NULL);   // Video
    else if (Image!=NULL) ReturnImage=Image->ImageAt(true,false,NULL);                                           // Image
    else {                                                                                                                                              // Title
        // Create an empty transparent image
        ReturnImage=new QImage(Parent->InternalWidth,Parent->InternalHeight,QImage::Format_ARGB32_Premultiplied);
        QPainter P;
        P.begin(ReturnImage);
        P.setCompositionMode(QPainter::CompositionMode_Source);
        P.fillRect(QRect(0,0,Parent->InternalWidth,Parent->InternalHeight),Qt::transparent);
        P.setCompositionMode(QPainter::CompositionMode_SourceOver);
        P.end();
    }
    if (SoundTrackMontage!=NULL) for (int j=0;j<SoundTrackMontage->NbrPacketForFPS;j++) SoundTrackMontage->AppendNullSoundPacket();

    return ReturnImage;
}

//====================================================================================================================
// Use GetImageAt to load image and then put image on a Canvas of size Width x Height
// if Painter is not null then paint image directly in the painter and return NULL
// else prepare and return a new image
// Note :
//      AddX x AddY is origin position of the source image on the canvas (usefull for thumbails generation only)
//      if ImagePosition is not null then it's fill with real position of image in the canvas
//      if ForcedImageRotation is not null then use it's value instead of Current shot value
//      if VideoCachedMode=true then use cached image instead of read image from disk (usefull for thumbnails)
//====================================================================================================================

QImage *cDiaporamaObject::CanvasImageAt(int Width,int Height,int Position,QPainter *Painter,int AddX,int AddY,QRectF *ImagePosition,int *ForcedImageRotation,
                                        bool ApplyShotText,bool ApplyShotFilter,bool ApplyFraming,cSoundBlockList *SoundTrackMontage) {

    // Call PrepareImage on a painter to put image on a canvas
    QImage *SourceImage = GetImageAt(Position,SoundTrackMontage);
    QImage *ReturnImage = NULL;

    if ((SourceImage==NULL)||(SourceImage->isNull())) return NULL;

    if (Painter!=NULL) {
        PrepareImage(Painter,Width,Height,Position,SourceImage,AddX,AddY,ImagePosition,ForcedImageRotation,ApplyShotText,ApplyShotFilter,ApplyFraming);
    } else {
        // Call only by QCustomScene (Painter is NULL)
        // Prepare a background with transparent brush (emulating transparency)
        ReturnImage = new QImage(Width,Height,QImage::Format_ARGB32_Premultiplied);
        QPainter    P;
        P.begin(ReturnImage);
        P.fillRect(0,0,Width,Height,Parent->Transparent);
        PrepareImage(&P,Width,Height,Position,SourceImage,AddX,AddY,ImagePosition,ForcedImageRotation,ApplyShotText,ApplyShotFilter,ApplyFraming);
        P.end();
    }

    // No future need of source image
    delete SourceImage;

    // return new image
    return ReturnImage;
}


// PrepareImage subfunction (Call only by CanvasImageAt)
void cDiaporamaObject::PrepareImage(QPainter *P,int Width,int Height,int Position,QImage *LastLoadedImage,int AddX,int AddY,QRectF *ImagePosition,int *ForcedImageRotation,bool ApplyShotText,bool ApplyShotFilter,bool ApplyFraming) {

    ADJUST_RATIO=double(Height)/double(1080);    // fixe Adjustment ratio for this slide

    // Calc current sequence depending on Position
    int Sequence=0;
    int CurPos  =0;
    while ((Sequence<List.count()-1)&&((CurPos+(Sequence>0?List[Sequence].GetMobilDuration():0)+List[Sequence].GetStaticDuration())<=Position)) {
        CurPos=CurPos+(Sequence>0?List[Sequence].GetMobilDuration():0)+List[Sequence].GetStaticDuration();
        Sequence++;
    }

//==========>

    QImage                  *SourceImage=NULL;
    cFilterCorrectObject    *FilterCorrection=&List[Sequence].FilterCorrection;
    double                  XFactor     =0;
    double                  YFactor     =0;
    double                  ZoomFactor  =1;
    double                  RotateFactor=0;
    double                  PctDone     =1;     // 100%

    // Calc image modification factor depending on position
    if (ForcedImageRotation!=NULL) RotateFactor=double(*ForcedImageRotation); else RotateFactor=FilterCorrection->ImageRotation;

    if ((ImagePosition==NULL)&&(ApplyFraming)) {
        XFactor         =FilterCorrection->X;
        YFactor         =FilterCorrection->Y;
        ZoomFactor      =FilterCorrection->ZoomFactor;
    }

    if ((ApplyFraming)&&(Sequence>0)&&((Position-CurPos)<(Sequence>0?List[Sequence].GetMobilDuration():0))) {
        PctDone=(double(Position)-double(CurPos))/(double(List[Sequence].GetMobilDuration()));
        CalcTransformations(Sequence,PctDone,XFactor,YFactor,ZoomFactor,RotateFactor,*FilterCorrection);
    }

    // Calc real image size
    double   RealImageW=double(LastLoadedImage->width());
    double   RealImageH=double(LastLoadedImage->height());
    double   Hyp=sqrt(RealImageW*RealImageW+RealImageH*RealImageH);     // Calc hypothenuse
    double   VirtImageW=Hyp;                                            // Calc canvas size
    double   VirtImageH=Parent->GetHeightForWidth(VirtImageW);          // Calc canvas size
    // Ensure complete image
    if (VirtImageH<Hyp) {
        VirtImageH=Hyp;
        VirtImageW=Parent->GetWidthForHeight(VirtImageH);
    }

    // Global image Composition
    QImage      GlobalImageComposition(RealImageW,RealImageH,QImage::Format_ARGB32_Premultiplied);
    QPainter    PB;
    PB.begin(&GlobalImageComposition);
    PB.setCompositionMode(QPainter::CompositionMode_Source);
    PB.fillRect(QRect(0,0,GlobalImageComposition.width(),GlobalImageComposition.height()),Qt::transparent);
    PB.setCompositionMode(QPainter::CompositionMode_SourceOver);
    PB.end();

    // Rotate image if needed
    if (RotateFactor!=0) {
        QTransform matrix;
        matrix.rotate(RotateFactor,Qt::ZAxis);
        SourceImage=new QImage(LastLoadedImage->transformed(matrix));
        GlobalImageComposition=GlobalImageComposition.transformed(matrix);

        // update real image size
        RealImageW=double(SourceImage->width());
        RealImageH=double(SourceImage->height());
    } else SourceImage=LastLoadedImage;

    // Calc source and destination, size and position
    double   DestX=0;                                                    // X position in the virtual image of the destination image
    double   DestY=0;                                                    // Y position in the virtual image of the destination image
    double   DestW=VirtImageW;                                           // with of the destination image in the virtual image
    double   DestH=VirtImageH;                                           // height of the destination image in the virtual image

    double   SrcX=(XFactor*VirtImageW-(VirtImageW-RealImageW)/2);        // X position in the source image of the destination image
    double   SrcY=(YFactor*VirtImageH-(VirtImageH-RealImageH)/2);        // Y position in the source image of the destination image
    double   SrcW=(ZoomFactor*VirtImageW);                               // with of the destination image in the source image
    double   SrcH=(ZoomFactor*VirtImageH);                               // height of the destination image in the source image

    // Adjust positions by croping to the source image
    if (SrcX<0)                 {   SrcW=SrcW+SrcX;                                 DestW=DestW+SrcX/ZoomFactor;    DestX=DestX-SrcX/ZoomFactor;    SrcX=0;     }
    if (SrcY<0)                 {   SrcH=SrcH+SrcY;                                 DestH=DestH+SrcY/ZoomFactor;    DestY=DestY-SrcY/ZoomFactor;    SrcY=0;     }
    if (SrcX+SrcW>RealImageW)   {   DestW=DestW-(SrcX+SrcW-RealImageW)/ZoomFactor;  SrcW=(RealImageW-SrcX);                                                     }
    if (SrcY+SrcH>RealImageH)   {   DestH=DestH-(SrcY+SrcH-RealImageH)/ZoomFactor;  SrcH=(RealImageH-SrcY);                                                     }

    // translate DestX,DestY,DestW,DestH to destination coordinates
    DestX=((double(Width)/VirtImageW)*DestX);
    DestW=((double(Width)/VirtImageW)*DestW);
    DestY=((double(Height)/VirtImageH)*DestY);
    DestH=((double(Height)/VirtImageH)*DestH);

    //**********************************
    // Draw image layer
    //**********************************
    // Shrink to used part of the image
    QImage Image1=SourceImage->copy(SrcX,SrcY,SrcW,SrcH);

    // Scaled part as needed
    QImage Image2=Image1.scaled(int(DestW),int(DestH),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

    // Apply global filter to part image
    if (TypeObject!=DIAPORAMAOBJECTTYPE_EMPTY) FilterTransform.ApplyFilter(&Image2);

    // Apply shot filter to part image
    if ((TypeObject!=DIAPORAMAOBJECTTYPE_EMPTY)&&(ApplyShotFilter)) FilterCorrection->ApplyFilter(&Image2);

//==========>

    P->drawImage(QRectF(DestX+AddX,DestY+AddY,DestW,DestH),Image2);

    //**********************************
    // Composition layers
    //**********************************
    // Add global image composition layer
    P->drawImage(QRectF(DestX+AddX,DestY+AddY,DestW,DestH),GlobalImageComposition,QRectF(SrcX,SrcY,SrcW,SrcH));

    // Add static shot composition
    if (ApplyShotText) for (int j=0;j<List[Sequence].ShotComposition.List.count();j++) {
        cCompositionObject *PrevCompoObject=NULL;
        if (Sequence>0) {
            int k=0;
            while (k<List[Sequence-1].ShotComposition.List.count()) {
                if (List[Sequence-1].ShotComposition.List[k].IndexKey==List[Sequence].ShotComposition.List[j].IndexKey) {
                    PrevCompoObject=&List[Sequence-1].ShotComposition.List[k];
                    k=List[Sequence-1].ShotComposition.List.count();
                } else k++;
            }
        }

        List[Sequence].ShotComposition.List[j].DrawCompositionObject(*P,AddX,AddY,Width,Height,true,0,NULL,PctDone,PrevCompoObject);
    }

    if (ImagePosition!=NULL) *ImagePosition=QRectF(DestX,DestY,DestW,DestH);

    if (SourceImage!=LastLoadedImage) delete SourceImage;
}

//===============================================================
// Draw Thumb
void cDiaporamaObject::DrawThumbnail(int ThumbWidth,int ThumbHeight,QPainter *Painter,int AddX,int AddY) {
    if (!Thumbnail) Thumbnail=CanvasImageAt(ThumbWidth,ThumbHeight,0,NULL,0,0,NULL,NULL,true,true,true,NULL);
    Painter->drawImage(AddX,AddY,*Thumbnail);
}

//===============================================================

void cDiaporamaObject::CalcTransformations(int Sequence,double PctDone,double &XFactor,double &YFactor,double &ZoomFactor,double &RotateFactor,cFilterCorrectObject &FilterCorrection) {
    cFilterCorrectObject AncFilterCorrection =List[Sequence-1].FilterCorrection;;

    // Adjust XFactor, YFactor and ZoomFactor depending on position in Mobil part animation
    double AncXFactor     =List[Sequence-1].FilterCorrection.X;
    double AncYFactor     =List[Sequence-1].FilterCorrection.Y;
    double AncZoomFactor  =List[Sequence-1].FilterCorrection.ZoomFactor;
    double AncRotateFactor=List[Sequence-1].FilterCorrection.ImageRotation;

    if (AncXFactor!=XFactor)                                            XFactor                     =AncXFactor+(XFactor-AncXFactor)*PctDone;
    if (AncYFactor!=YFactor)                                            YFactor                     =AncYFactor+(YFactor-AncYFactor)*PctDone;
    if (AncZoomFactor!=ZoomFactor)                                      ZoomFactor                  =AncZoomFactor+(ZoomFactor-AncZoomFactor)*PctDone;
    if (AncRotateFactor!=RotateFactor)                                  RotateFactor                =AncRotateFactor+(RotateFactor-AncRotateFactor)*PctDone;
    if (AncFilterCorrection.Brightness!=FilterCorrection.Brightness)    FilterCorrection.Brightness =AncFilterCorrection.Brightness+(FilterCorrection.Brightness-AncFilterCorrection.Brightness)*PctDone;
    if (AncFilterCorrection.Contrast!=FilterCorrection.Contrast)        FilterCorrection.Contrast   =AncFilterCorrection.Contrast+(FilterCorrection.Contrast-AncFilterCorrection.Contrast)*PctDone;
    if (AncFilterCorrection.Gamma!=FilterCorrection.Gamma)              FilterCorrection.Gamma      =AncFilterCorrection.Gamma+(FilterCorrection.Gamma-AncFilterCorrection.Gamma)*PctDone;
    if (AncFilterCorrection.Red!=FilterCorrection.Red)                  FilterCorrection.Red        =AncFilterCorrection.Red+(FilterCorrection.Red-AncFilterCorrection.Red)*PctDone;
    if (AncFilterCorrection.Green!=FilterCorrection.Green)              FilterCorrection.Green      =AncFilterCorrection.Green+(FilterCorrection.Green-AncFilterCorrection.Green)*PctDone;
    if (AncFilterCorrection.Blue!=FilterCorrection.Blue)                FilterCorrection.Blue       =AncFilterCorrection.Blue+(FilterCorrection.Blue-AncFilterCorrection.Blue)*PctDone;
}

//===============================================================

int cDiaporamaObject::GetCumulTransitDuration() {
    // Adjust duration to ensure transition will be full !
    int ObjectIndex=Parent->GetObjectIndex(this);
    int TransitDuration=TransitionDuration;
    if (ObjectIndex<(Parent->List.count()-1)) TransitDuration+=Parent->List[ObjectIndex+1].TransitionDuration;
    return TransitDuration;
}

//===============================================================

int cDiaporamaObject::GetDuration() {
    int Duration=0;
    for (int i=0;i<List.count();i++) Duration=Duration+(i>0?List[i].GetMobilDuration():0)+List[i].GetStaticDuration();

    // Adjust duration to ensure transition will be full !
    int TransitDuration=GetCumulTransitDuration();
    if (Duration<TransitDuration) Duration=TransitDuration;

    return Duration;
}

//===============================================================

void cDiaporamaObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    QString         FileName;

    if ((Image!=NULL)&&(Image->FileName!=""))       FileName=Image->FileName;
    else if ((Video!=NULL)&&(Video->FileName!=""))  FileName=Video->FileName;

    Element.setAttribute("TypeObject",TypeObject);
    Element.setAttribute("SlideName", SlideName);

    if (PathForRelativPath!="") FileName=QDir(PathForRelativPath).relativeFilePath(FileName);
    Element.setAttribute("ObjectFileName",  FileName);
    if (Video!=NULL) {
        Element.setAttribute("StartPos",Video->StartPos.toString());                        // Start position (video only)
        Element.setAttribute("EndPos",Video->EndPos.toString());                            // End position (video only)
        Element.setAttribute("SoundVolume",QString("%1").arg(Video->SoundVolume,0,'f'));    // Volume of soundtrack (for video only)
    }

    QDomElement SubElement=DomDocument.createElement("Background");
    SubElement.setAttribute("BackgroundType",BackgroundType?"1":"0");                       // Background type : false=same as precedent - true=new background definition
    BackgroundBrush.SaveToXML(SubElement,"BackgroundBrush",PathForRelativPath);             // Background brush
    BackgroundComposition.SaveToXML(SubElement,"BackgroundComposition",PathForRelativPath); // Background composition
    Element.appendChild(SubElement);

    SubElement=DomDocument.createElement("Transition");
    SubElement.setAttribute("TransitionFamilly",TransitionFamilly);                         // Transition familly
    SubElement.setAttribute("TransitionSubType",TransitionSubType);                         // Transition type in the familly
    SubElement.setAttribute("TransitionDuration",TransitionDuration);                       // Transition duration (in msec)
    Element.appendChild(SubElement);

    FilterTransform.SaveToXML(Element,"GlobalImageFilters",PathForRelativPath);             // Global Image filters

    Element.setAttribute("NextIndexKey",    NextIndexKey);
    ObjectComposition.SaveToXML(SubElement,"ObjectComposition",PathForRelativPath);         // ObjectComposition

    // Save shot list
    Element.setAttribute("ShotNumber",List.count());
    for (int i=0;i<List.count();i++) List[i].SaveToXML(Element,"Shot-"+QString("%1").arg(i),PathForRelativPath);

    // Save music type and list
    Element.setAttribute("MusicType",         MusicType?"1":"0");                           // Music type : false=same as precedent - true=new playlist definition
    Element.setAttribute("MusicPause",        MusicPause?"1":"0");                          // true if music is pause during this object
    Element.setAttribute("MusicReduceVolume", MusicReduceVolume?"1":"0");                   // true if volume if reduce by MusicReduceFactor
    Element.setAttribute("MusicReduceFactor",QString("%1").arg(MusicReduceFactor,0,'f'));   // factor for volume reduction if MusicReduceVolume is true
    Element.setAttribute("MusicNumber",       MusicList.count());                           // Number of file in the playlist
    for (int i=0;i<MusicList.count();i++) MusicList[i].SaveToXML(Element,"Music-"+QString("%1").arg(i),PathForRelativPath);

    domDocument.appendChild(Element);
}

//===============================================================

bool cDiaporamaObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        SlideName=Element.attribute("SlideName");
        QString FileName=Element.attribute("ObjectFileName","");
        if (PathForRelativPath!="") FileName=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(FileName));

        int TheTypeObject=Element.attribute("TypeObject").toInt();
        if (LoadMedia(FileName,TheTypeObject)) {
            bool IsOk=true;

            // Load shot list
            List.clear();
            int ShotNumber=Element.attribute("ShotNumber").toInt();
            for (int i=0;i<ShotNumber;i++) {
                cDiaporamaShot *imagesequence=new cDiaporamaShot(this);
                if (!imagesequence->LoadFromXML(Element,"Shot-"+QString("%1").arg(i),PathForRelativPath)) IsOk=false;
                List.append(*imagesequence);
            }
            //==========> Récupération des données : Adjust duration
            for (int i=0;i<ShotNumber;i++) if (List[i].StaticDuration==-1) List[i].StaticDuration=ShotNumber>1?Parent->FixedDuration:Parent->NoShotDuration;
            //==========>

            if (Video!=NULL) {
                Video->SoundVolume=Element.attribute("SoundVolume").toDouble();         // Volume of soundtrack (for video only)
                Video->StartPos   =QTime().fromString(Element.attribute("StartPos"));   // Start position (video only)
                Video->EndPos     =QTime().fromString(Element.attribute("EndPos"));     // End position (video only)
            }

            if ((Element.elementsByTagName("Background").length()>0)&&(Element.elementsByTagName("Background").item(0).isElement()==true)) {
                QDomElement SubElement=Element.elementsByTagName("Background").item(0).toElement();
                BackgroundType  =SubElement.attribute("BackgroundType")=="1"; // Background type : false=same as precedent - true=new background definition

                //==========> Récupération des données ! if Object don't use full canvas (height is hypothenuse of the image rectangle and width is calc from aspect ratio)
                // Note : C'était idiot mais l'attribut full canvas était placé sur le background !
                if ((SubElement.hasAttribute("FullCanvas"))&&(SubElement.attribute("FullCanvas")!="1")) {
                    // Recalculate all shot size and position for normal to full canvas
                    QImage   *ReturnImage=NULL;

                    if (Video!=NULL)        ReturnImage=Video->ImageAt(true,0,true,NULL,1,false,NULL);  // Video
                    else if (Image!=NULL)   ReturnImage=Image->ImageAt(true,true,NULL);                // Image

                    if (ReturnImage!=NULL) {
                        // Calc size
                        double   RealImageW     =ReturnImage->width();
                        double   RealImageH     =ReturnImage->height();
                        double   Hyp            =sqrt(RealImageW*RealImageW+RealImageH*RealImageH);         // Calc hypothenuse
                        double   FullCanvasW    =Hyp;                                                       // Calc full canvas size
                        double   FullCanvasH    =Parent->GetHeightForWidth(FullCanvasW);
                        double   NormalCanvasW  =RealImageW;                                                // Calc normal canvas size
                        double   NormalCanvasH  =Parent->GetHeightForWidth(NormalCanvasW);
                        if (FullCanvasH<Hyp) {          // Ensure complete image
                            FullCanvasH=Hyp;
                            FullCanvasW=Parent->GetWidthForHeight(FullCanvasH);
                        }
                        if (NormalCanvasH<RealImageH) { // Ensure complete image
                            NormalCanvasH=RealImageH;
                            NormalCanvasW=Parent->GetWidthForHeight(NormalCanvasH);
                        }
                        // Do transformation
                        for (int i=0;i<List.count();i++) {
                            List[i].FilterCorrection.ZoomFactor=List[i].FilterCorrection.ZoomFactor*NormalCanvasW/FullCanvasW;
                            List[i].FilterCorrection.X=List[i].FilterCorrection.X*NormalCanvasW/FullCanvasW+((FullCanvasW-NormalCanvasW)/2)/FullCanvasW;
                            if (List[i].FilterCorrection.X+List[i].FilterCorrection.ZoomFactor>1) List[i].FilterCorrection.X=1-List[i].FilterCorrection.ZoomFactor;
                            List[i].FilterCorrection.Y=List[i].FilterCorrection.Y*NormalCanvasH/FullCanvasH+((FullCanvasH-NormalCanvasH)/2)/FullCanvasH;
                            if (List[i].FilterCorrection.Y+List[i].FilterCorrection.ZoomFactor>1) List[i].FilterCorrection.Y=1-List[i].FilterCorrection.ZoomFactor;
                        }
                        delete ReturnImage;
                    }
                }
                //==========> Récupération des données !
                if (!BackgroundBrush.LoadFromXML(SubElement,"BackgroundBrush",PathForRelativPath)) IsOk=false;                          // Background brush
                if ((!IsOk)||(!BackgroundComposition.LoadFromXML(SubElement,"BackgroundComposition",PathForRelativPath))) IsOk=false;   // Background composition
            }

            if ((Element.elementsByTagName("Transition").length()>0)&&(Element.elementsByTagName("Transition").item(0).isElement()==true)) {
                QDomElement SubElement=Element.elementsByTagName("Transition").item(0).toElement();
                TransitionFamilly =SubElement.attribute("TransitionFamilly").toInt();                                                   // Transition familly
                TransitionSubType =SubElement.attribute("TransitionSubType").toInt();                                                   // Transition type in the familly
                TransitionDuration=SubElement.attribute("TransitionDuration").toInt();                                                  // Transition duration (in msec)
                Element.appendChild(SubElement);
            }

            FilterTransform.LoadFromXML(Element,"GlobalImageFilters",PathForRelativPath);                                           // Global Image filters

            //==========> Récupération des données !
            if (Element.elementsByTagName("GlobalImageComposition").length()>0) {
                if ((Element.elementsByTagName("GlobalImageComposition").length()>0)&&(Element.elementsByTagName("GlobalImageComposition").item(0).isElement()==true)) {
                    QDomElement SubElement=Element.elementsByTagName("GlobalImageComposition").item(0).toElement();
                    int CompositionNumber=SubElement.attribute("CompositionNumber").toInt();
                    for (int i=0;i<CompositionNumber;i++) {

                        cCompositionObject *CompositionObject    =new cCompositionObject(COMPOSITIONTYPE_OBJECT,NextIndexKey);
                        CompositionObject->LoadFromXML(SubElement,"Composition-"+QString("%1").arg(i),PathForRelativPath);
                        ObjectComposition.List.append(*CompositionObject);

                        cCompositionObject *CompositionShotObject=new cCompositionObject(COMPOSITIONTYPE_SHOT,NextIndexKey);
                        CompositionShotObject->CopyFromCompositionObject(CompositionObject);
                        List[0].ShotComposition.List.append(*CompositionShotObject);
                        NextIndexKey++;
                    }
                }
            }
            //==========>

            NextIndexKey=Element.attribute("NextIndexKey").toInt();
            ObjectComposition.LoadFromXML(Element,"ObjectComposition",PathForRelativPath);         // ObjectComposition

            // Construct link to video and image object from DiaporamaObject->ObjectComposition to all DiaporamaShotObject.ShotComposition objects !
            for (int i=0;i<List.count();i++) for (int j=0;j<List[i].ShotComposition.List.count();j++) {
                int k=0;
                while (k<ObjectComposition.List.count()) {
                    if (ObjectComposition.List[k].IndexKey==List[i].ShotComposition.List[j].IndexKey) {
                        List[i].ShotComposition.List[j].BackgroundBrush.Video=ObjectComposition.List[k].BackgroundBrush.Video;
                        List[i].ShotComposition.List[j].BackgroundBrush.Image=ObjectComposition.List[k].BackgroundBrush.Image;
                        k=ObjectComposition.List.count();
                    } else k++;
                }
            }


            // Load Music list
            MusicList.clear();
            MusicType         =Element.attribute("MusicType")=="1";                     // Music type : false=same as precedent - true=new playlist definition
            MusicPause        =Element.attribute("MusicPause")=="1";                    // true if music is pause during this object
            MusicReduceVolume =Element.attribute("MusicReduceVolume")=="1";             // true if volume if reduce by MusicReduceFactor
            MusicReduceFactor =Element.attribute("MusicReduceFactor").toDouble();       // factor for volume reduction if MusicReduceVolume is true
            int MusicNumber   =Element.attribute("MusicNumber").toInt();                // Number of file in the playlist
            for (int i=0;i<MusicNumber;i++) {
                cMusicObject *MusicObject=new cMusicObject();
                if (!MusicObject->LoadFromXML(Element,"Music-"+QString("%1").arg(i),PathForRelativPath)) IsOk=false;
                MusicList.append(*MusicObject);
            }

            //==========> Récupération des données !
            if (SlideName=="") SlideName=Image?QFileInfo(Image->FileName).fileName():Video?QFileInfo(Video->FileName).fileName():QCoreApplication::translate("MainWindow","Title","Default slide name when no file");;
            //==========> Récupération des données !

            return IsOk;
        } else return false;
    } else return false;
}

//*********************************************************************************************************************************************
//
// Global class containing media objects
//
//*********************************************************************************************************************************************

cDiaporama::cDiaporama(cApplicationConfig *TheApplicationConfig) {
    Timeline            = NULL;
    ApplicationConfig   = TheApplicationConfig;
    CurrentCol          = -1;                                                               // Current selected item
    CurrentPosition     = -1;                                                               // Current position (msec)
    IsModify            = false;                                                            // true if project was modify
    ProjectFileName     = "";                                                               // Path and name of current file project

    // Init dest file values (TODO : Make and use values from application configurations values !)
    OutputFileFormat    = ApplicationConfig->DefaultFormat;
    VideoCodec          = ApplicationConfig->DefaultVideoCodec;
    VideoFrameRate      = 25;
    VideoBitRate        = ApplicationConfig->DefaultVideoBitRate;
    VideoCodec          = ApplicationConfig->DefaultAudioCodec;
    AudioFrequency      = 48000;
    AudioBitRate        = ApplicationConfig->DefaultAudioBitRate;
    LastImageSize       = ApplicationConfig->DefaultImageSize;
    LastStandard        = ApplicationConfig->DefaultStandard;

    LastLoadedBackgroundImageName="";
    LastLoadedBackgroundImage    =NULL;
    Transparent.setTextureImage(QImage("icons/transparent.png"));                           // Load transparent brush

    // Set default value
    DefineSizeAndGeometry(ApplicationConfig->ImageGeometry);                                // Default to 16:9
    NoShotDuration      = ApplicationConfig->NoShotDuration;                                // Default duration for fixed image when is alone (no shot)
    FixedDuration       = ApplicationConfig->FixedDuration;                                 // Default duration for fixed image (msec)
    MobilDuration       = ApplicationConfig->MobilDuration;                                 // Default duration for mobil image (msec)
    SpeedWave           = ApplicationConfig->SpeedWave;                                     // Default speed wave methode
}

//====================================================================================================================

cDiaporama::~cDiaporama() {
    Clean();
}

//====================================================================================================================

void cDiaporama::Clean() {
    if (LastLoadedBackgroundImage!=NULL) {
        delete LastLoadedBackgroundImage;
        LastLoadedBackgroundImage=NULL;
    }
}

//====================================================================================================================

void cDiaporama::DefineSizeAndGeometry(int Geometry) {
    Clean();
    ImageGeometry   =Geometry;
    InternalHeight  =720;
    ApplicationConfig->PreviewMaxHeight=InternalHeight;
    InternalWidth   =GetWidthForHeight(InternalHeight);
    BackgroundList.ScanDisk("background",ImageGeometry);
    LumaList_Bar.SetGeometry(ImageGeometry);
    LumaList_Center.SetGeometry(ImageGeometry);
    LumaList_Checker.SetGeometry(ImageGeometry);
    LumaList_Clock.SetGeometry(ImageGeometry);
    LumaList_Box.SetGeometry(ImageGeometry);
    LumaList_Snake.SetGeometry(ImageGeometry);
}

//=======================================================
// Return height for width depending on project geometry
//=======================================================

int cDiaporama::GetHeightForWidth(int WantedWith) {
    switch (ImageGeometry) {
        case GEOMETRY_4_3 :     return int(double(3)*(double(WantedWith)/double(4)));       break;
        case GEOMETRY_16_9 :    return int(double(9)*(double(WantedWith)/double(16)));      break;
        case GEOMETRY_40_17 :   return int(double(17)*(double(WantedWith)/double(40)));     break;
    }
    return 0;
}

//=======================================================
// Return width for height depending on project geometry
//=======================================================

int cDiaporama::GetWidthForHeight(int WantedHeight) {
    switch (ImageGeometry) {
        case GEOMETRY_4_3 :     return int(double(4)*(double(WantedHeight)/double(3)));       break;
        case GEOMETRY_16_9 :    return int(double(16)*(double(WantedHeight)/double(9)));      break;
        case GEOMETRY_40_17 :   return int(double(40)*(double(WantedHeight)/double(17)));     break;
    }
    return 0;
}

//====================================================================================================================

int cDiaporama::GetTransitionDuration(int index) {
    int Duration=0;
    if ((index>=0)&&(List.count()>0)&&((index<List.count())&&((List[index].TransitionFamilly!=0)||(List[index].TransitionSubType!=0)))) Duration=List[index].TransitionDuration;
    return Duration;
}

//====================================================================================================================

int cDiaporama::GetDuration() {
    int Duration=0;
    for (int i=0;i<List.count();i++) Duration=Duration+List[i].GetDuration()-GetTransitionDuration(i+1);
    return Duration;
}

//====================================================================================================================

int cDiaporama::GetObjectStartPosition(int index) {
    int Duration=0;
    for (int i=0;i<index;i++) Duration=Duration+List[i].GetDuration()-GetTransitionDuration(i+1);
    return Duration;
}

//====================================================================================================================

int cDiaporama::GetObjectIndex(cDiaporamaObject *ObjectToFind) {
    int i=0;
    while ((i<List.count())&&(ObjectToFind!=&List[i])) i++;
    if (i<List.count()) return i; else return -1;
}

//====================================================================================================================

void cDiaporama::PrepareBackground(int Index,int Width,int Height,QPainter *Painter,int AddX,int AddY,bool ApplyComposition) {
    // Background type : false=same as precedent - true=new background definition
    while ((Index>0)&&(!List[Index].BackgroundType)) Index--;
    // Make painter translation to ensure QBrush image will start at AddX AddY position
    Painter->save();
    Painter->translate(AddX,AddY);
    if (!List[Index].BackgroundType) Painter->fillRect(QRect(0,0,Width,Height),QBrush(Qt::black)); else {
        QBrush *BR=List[Index].BackgroundBrush.GetBrush(QRectF(0,0,Width,Height),true,0,NULL,1,NULL);
        Painter->fillRect(QRect(0,0,Width,Height),*BR);
        delete BR;
    }
    //
    if (ApplyComposition) for (int j=0;j<List[Index].BackgroundComposition.List.count();j++) List[Index].BackgroundComposition.List[j].DrawCompositionObject(*Painter,0,0,Width,Height,true,0,NULL,1,NULL);
    Painter->restore();
}

//====================================================================================================================

cMusicObject *cDiaporama::GetMusicObject(int ObjectIndex,int &StartPosition,int *CountObject) {
    if (ObjectIndex>=List.count()) return NULL;

    cMusicObject *Ret =NULL;
    int          Index=ObjectIndex;

    StartPosition=0;

    // Music type : false=same as precedent - true=new playlist definition
    while ((Index>0)&&(!List[Index].MusicType)) {
        if (!List[Index-1].MusicPause) StartPosition+=(List[Index-1].GetDuration()-GetTransitionDuration(Index));
        Index--;
    }

    // Now we have the object owner of the playlist (or 0). Then we can calculate wich music in the playlist is usable for this object
    int i=0;
    while ((i<List[Index].MusicList.count())&&(StartPosition>QTime(0,0,0,0).msecsTo(List[Index].MusicList[i].Duration))) {
        if ((Index>0)&&(!List[Index-1].MusicPause)) StartPosition-=QTime(0,0,0,0).msecsTo(List[Index].MusicList[i].Duration);
            else return NULL;
        i++;
    }

    if ((i<List[Index].MusicList.count())&&(StartPosition<=QTime(0,0,0,0).msecsTo(List[Index].MusicList[i].Duration))) Ret=&List[Index].MusicList[i];
    if (CountObject) {
        *CountObject=0;
        while (Index>0) {
            Index--;
            if (List[Index].MusicType) *CountObject+=1;
        }
    }

    return Ret;
}

//====================================================================================================================

bool cDiaporama::SaveFile(QWidget *ParentWindow) {
    QFile           file(ProjectFileName);
    QDomDocument    domDocument(APPLICATION_NAME);
    QDomElement     Element;
    QDomElement     root;

    // Create xml document and root
    root=domDocument.createElement(APPLICATION_ROOTNAME);
    domDocument.appendChild(root);

    // Save rendering informations on project
    Element=domDocument.createElement("Render");
    Element.setAttribute("OutputFileName",  OutputFileName);
    Element.setAttribute("VideoCodec",      VideoCodec);
    Element.setAttribute("VideoFrameRate",  VideoFrameRate);
    Element.setAttribute("VideoBitRate",    VideoBitRate);
    Element.setAttribute("AudioCodec",      AudioCodec);
    Element.setAttribute("AudioFrequency",  AudioFrequency);
    Element.setAttribute("AudioBitRate",    AudioBitRate);
    Element.setAttribute("LastImageSize",   LastImageSize);
    Element.setAttribute("LastStandard",    LastStandard);
    root.appendChild(Element);

    // Save basic information on project
    Element=domDocument.createElement("Project");
    Element.setAttribute("ImageGeometry",   ImageGeometry);
    Element.setAttribute("NoShotDuration",  NoShotDuration);
    Element.setAttribute("FixedDuration",   FixedDuration);
    Element.setAttribute("MobilDuration",   MobilDuration);
    Element.setAttribute("SpeedWave",       SpeedWave);

    // Save object list
    Element.setAttribute("ObjectNumber",List.count());
    for (int i=0;i<List.count();i++) List[i].SaveToXML(root,"Object-"+(QString("%1").arg(i,10)).trimmed(),QFileInfo(ProjectFileName).absolutePath());

    // Write file to disk
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error creating data file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","Error creating data file","Error message").toLocal8Bit().constData());
        return false;
    }
    root.appendChild(Element);

    // Save file now
    QTextStream out(&file);
    domDocument.save(out,4);
    file.close();
    return true;
}

//====================================================================================================================

bool cDiaporama::LoadFile(QWidget *ParentWindow,QString ProjectFileName) {
    QFile           file(ProjectFileName);
    QDomDocument    domDocument;
    QDomElement     root;
    QString         errorStr;
    int             errorLine,errorColumn;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error reading project file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","Error reading project file","Error message").toLocal8Bit().constData());
        return false;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn)) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error reading content of project file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","Error reading content of project file","Error message").toLocal8Bit().constData());
        return false;
    }

    root = domDocument.documentElement();
    if (root.tagName()!=APPLICATION_ROOTNAME) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","The file is not a valid project file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","The file is not a valid project file","Error message").toLocal8Bit().constData());
        return false;
    }

    this->ProjectFileName =ProjectFileName;

    // Load rendering informations on project
    if ((root.elementsByTagName("Render").length()>0)&&(root.elementsByTagName("Render").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("Render").item(0).toElement();
        OutputFileName  =Element.attribute("OutputFileName");
        VideoCodec      =Element.attribute("VideoCodec");
        VideoFrameRate  =Element.attribute("VideoFrameRate").toDouble();
        VideoBitRate    =Element.attribute("VideoBitRate").toInt();
        AudioCodec      =Element.attribute("AudioCodec");
        AudioFrequency  =Element.attribute("AudioFrequency").toInt();
        AudioBitRate    =Element.attribute("AudioBitRate").toInt();
        LastImageSize   =Element.attribute("LastImageSize").toInt();
        LastStandard    =Element.attribute("LastStandard").toInt();
    }

    // Load basic information on project
    bool IsOk=true;
    if ((root.elementsByTagName("Project").length()>0)&&(root.elementsByTagName("Project").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("Project").item(0).toElement();
        ImageGeometry   =Element.attribute("ImageGeometry").toInt();
        NoShotDuration  =Element.attribute("NoShotDuration").toInt();
        FixedDuration   =Element.attribute("FixedDuration").toInt();
        MobilDuration   =Element.attribute("MobilDuration").toInt();
        SpeedWave       =Element.attribute("SpeedWave").toInt();

        DefineSizeAndGeometry(ImageGeometry);

        // Load object list
        List.clear();
        int ObjectNumber=Element.attribute("ObjectNumber").toInt();
        for (int i=0;i<ObjectNumber;i++) if ((root.elementsByTagName("Object-"+QString("%1").arg(i)).length()>0)&&
                                             (root.elementsByTagName("Object-"+QString("%1").arg(i)).item(0).isElement()==true)) {
            List.append(cDiaporamaObject(this));
            if (List[List.count()-1].LoadFromXML(root,"Object-"+QString("%1").arg(i).trimmed(),QFileInfo(ProjectFileName).absolutePath())) {
                if (ParentWindow!=NULL) ((MainWindow *)ParentWindow)->AddObjectToTimeLine(i);
            } else {
                List.removeLast();
                IsOk=false;
            }
        }
    } else IsOk=false;
    return IsOk;
}

//====================================================================================================================

bool cDiaporama::AppendFile(QWidget *ParentWindow,QString ProjectFileName) {
    QFile           file(ProjectFileName);
    QDomDocument    domDocument;
    QDomElement     root;
    QString         errorStr;
    int             errorLine,errorColumn;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error reading project file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","Error reading project file","Error message").toLocal8Bit().constData());
        return false;
    }

    if (!domDocument.setContent(&file, true, &errorStr, &errorLine,&errorColumn)) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Error reading content of project file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","Error reading content of project file","Error message").toLocal8Bit().constData());
        return false;
    }

    root = domDocument.documentElement();
    if (root.tagName()!=APPLICATION_ROOTNAME) {
        if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","The file is not a valid project file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QCoreApplication::translate("MainWindow","The file is not a valid project file","Error message").toLocal8Bit().constData());
        return false;
    }

    if ((root.elementsByTagName("Project").length()>0)&&(root.elementsByTagName("Project").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("Project").item(0).toElement();
        int TheImageGeometry   =Element.attribute("ImageGeometry").toInt();
        if (TheImageGeometry!=ImageGeometry) {
            if (ParentWindow!=NULL) QMessageBox::critical(NULL,QCoreApplication::translate("MainWindow","Error","Error message"),QCoreApplication::translate("MainWindow","Impossible to import this file :\nImage geometry in this file is not the same than the current project","Error message"),QMessageBox::Close);
                else                printf("%s\n",QCoreApplication::translate("MainWindow","Impossible to import this file :\nImage geometry in this file is not the same than the current project","Error message").toLocal8Bit().constData());
            return false;
        }
    }

    // Load basic information on project
    bool IsOk=true;
    if ((root.elementsByTagName("Project").length()>0)&&(root.elementsByTagName("Project").item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName("Project").item(0).toElement();

        // Load object list
        int ObjectNumber=Element.attribute("ObjectNumber").toInt();
        for (int i=0;i<ObjectNumber;i++) if ((root.elementsByTagName("Object-"+QString("%1").arg(i)).length()>0)&&
                                             (root.elementsByTagName("Object-"+QString("%1").arg(i)).item(0).isElement()==true)) {
            List.append(cDiaporamaObject(this));
            if (List[List.count()-1].LoadFromXML(root,"Object-"+QString("%1").arg(i).trimmed(),QFileInfo(ProjectFileName).absolutePath())) {
                if (ParentWindow!=NULL) ((MainWindow *)ParentWindow)->AddObjectToTimeLine(i);
            } else {
                List.removeLast();
                IsOk=false;
            }
        }
    } else IsOk=false;
    return IsOk;
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
// Note : Position is relative to the start of the Column object !
//============================================================================================
void cDiaporama::PrepareMusicBloc(int Column,int Position,cSoundBlockList *MusicTrack) {
    int     StartPosition;

    if ((Column<List.count())&&(!List[Column].MusicPause)) {
        cMusicObject *CurMusic=GetMusicObject(Column,StartPosition);                                         // Get current music file from column and position
        if (CurMusic!=NULL) {
            CurMusic->Music->ImageAt(false,Position+StartPosition,false,MusicTrack,1,false,NULL);               // Get music bloc at correct position
            double Factor=CurMusic->Volume;
            if (List[Column].MusicReduceVolume) Factor=Factor*List[Column].MusicReduceFactor;
            if (Factor!=1.0) for (int i=0;i<MusicTrack->NbrPacketForFPS;i++) MusicTrack->ApplyVolume(i,Factor);
        } else for (int j=0;j<MusicTrack->NbrPacketForFPS;j++) MusicTrack->AppendNullSoundPacket();
    } else for (int j=0;j<MusicTrack->NbrPacketForFPS;j++) MusicTrack->AppendNullSoundPacket();
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
//  Extend=amout of padding (top and bottom) for cinema mode with DVD
//  IsCurrentObject : If true : prepare CurrentObject - If false : prepare Transition Object
//============================================================================================
void cDiaporama::PrepareImage(cDiaporamaObjectInfo *Info,int W,int H,bool IsCurrentObject,bool PreviewMode) {
    ADJUST_RATIO=double(H)/double(1080);    // fixe Adjustment ratio for this slide

    // W and H = 0 when producing sound track in render process
    bool SoundOnly=((W==0)&&(H==0));

    if ((IsCurrentObject)&&(Info->CurrentObject_PreparedImage!=NULL)) return;     // return immediatly if we have image
    if (!IsCurrentObject) {
        if (Info->TransitObject_PreparedImage!=NULL) return;    // return immediatly if we have image
        if ((!SoundOnly)&&(Info->TransitObject_SourceImage==NULL)) {
            // Special case for transition image of first slide
            Info->TransitObject_PreparedImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
            QPainter P;
            P.begin(Info->TransitObject_PreparedImage);
            P.setCompositionMode(QPainter::CompositionMode_Source);
            P.fillRect(QRect(0,0,W,H),Qt::transparent);
            P.setCompositionMode(QPainter::CompositionMode_SourceOver);
            P.end();
            return;
        }
    }

    cDiaporamaObject        *CurObject  =IsCurrentObject?Info->CurrentObject:Info->TransitObject;
    cDiaporamaShot          *CurShot    =IsCurrentObject?Info->CurrentObject_CurrentShot:Info->TransitObject_CurrentShot;
    QImage                  *SourceImage=IsCurrentObject?Info->CurrentObject_SourceImage:Info->TransitObject_SourceImage;
    int                     ShotType    =IsCurrentObject?Info->CurrentObject_CurrentShotType:Info->TransitObject_CurrentShotType;
    int                     ShotSeqNum  =IsCurrentObject?Info->CurrentObject_ShotSequenceNumber:Info->TransitObject_ShotSequenceNumber;
    double                  MobilPCTDone=IsCurrentObject?Info->CurrentObject_MobilPCTDone:Info->TransitObject_MobilPCTDone;
    double                  PCTDone     =IsCurrentObject?Info->CurrentObject_PCTDone:Info->TransitObject_PCTDone;

    double                  DestX,DestY,DestW,DestH;                        // Coordonates of destination image in the virtual image
    double                  SrcX,SrcY,SrcW,SrcH;                            // Coordonates of the destination image in the source image
    double                  RealImageW,RealImageH,VirtImageW,VirtImageH;    // Destination image size
    double                  XFactor,YFactor,ZoomFactor,RotateFactor;        // Transformation value
    cFilterCorrectObject    FilterCorrection;                               // Filter correction value
    QImage                  *ImageToUse=NULL;
    QImage                  *Image=NULL;
    QImage                  Image1,Image2;                                  // Temporary object use to create frame (Image2 is the rendered image for montage)
    QPainter                P;

    qDebug()<<"PCTDone"<<PCTDone;

    if (!SoundOnly) {
        Image=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
        P.begin(Image);
        P.setCompositionMode(QPainter::CompositionMode_Source);
        P.fillRect(0,0,W,H,Qt::transparent);
        P.setCompositionMode(QPainter::CompositionMode_SourceOver);
    }

    if ((List.count()>0)&&(SourceImage!=NULL)) {

        // Prepare image
        if (!SoundOnly) {
            // Calc real image size
            RealImageW=double(SourceImage->width());
            RealImageH=double(SourceImage->height());
            double   Hyp=sqrt(RealImageW*RealImageW+RealImageH*RealImageH); // Calc hypothenuse
            VirtImageW=Hyp;                                                 // Calc canvas size
            VirtImageH=CurObject->Parent->GetHeightForWidth(VirtImageW);    // Calc canvas size
            // Ensure complete image
            if (VirtImageH<Hyp) {
                VirtImageH=Hyp;
                VirtImageW=CurObject->Parent->GetWidthForHeight(VirtImageH);
            }

            // Global image Composition
            QImage      GlobalImageComposition(RealImageW,RealImageH,QImage::Format_ARGB32_Premultiplied);
            QPainter    PB;
            PB.begin(&GlobalImageComposition);
            PB.setCompositionMode(QPainter::CompositionMode_Source);
            PB.fillRect(QRect(0,0,GlobalImageComposition.width(),GlobalImageComposition.height()),Qt::transparent);
            PB.setCompositionMode(QPainter::CompositionMode_SourceOver);
            PB.end();

            XFactor         =CurShot->FilterCorrection.X;
            YFactor         =CurShot->FilterCorrection.Y;
            ZoomFactor      =CurShot->FilterCorrection.ZoomFactor;
            RotateFactor    =CurShot->FilterCorrection.ImageRotation;
            FilterCorrection=CurShot->FilterCorrection;

            if (ShotType==SHOTTYPE_MOBIL)
                CurObject->CalcTransformations(ShotSeqNum,MobilPCTDone,XFactor,YFactor,ZoomFactor,RotateFactor,FilterCorrection);

            // Rotate image if needed
            if (RotateFactor!=0) {
                QTransform matrix;
                matrix.rotate(RotateFactor,Qt::ZAxis);
                ImageToUse=new QImage(SourceImage->transformed(matrix));
                GlobalImageComposition=GlobalImageComposition.transformed(matrix);

                // update real image size
                RealImageW=double(ImageToUse->width());
                RealImageH=double(ImageToUse->height());
            } else ImageToUse=SourceImage;

            // Calc source and destination size
            DestX=0;                                                    // X Position in the virtual image of the destination image
            DestY=0;                                                    // Y Position in the virtual image of the destination image
            DestW=VirtImageW;                                           // with of the destination image in the virtual image
            DestH=VirtImageH;                                           // height of the destination image in the virtual image

            SrcX=(XFactor*VirtImageW-(VirtImageW-RealImageW)/2);        // X Position in the source image of the destination image
            SrcY=(YFactor*VirtImageH-(VirtImageH-RealImageH)/2);        // Y Position in the source image of the destination image
            SrcW=(ZoomFactor*VirtImageW);                               // with of the destination image in the source image
            SrcH=(ZoomFactor*VirtImageH);                               // height of the destination image in the source image

            // Adjust positions by croping to the source image
            if (SrcX<0)                 {   SrcW=SrcW+SrcX;                                 DestW=DestW+SrcX/ZoomFactor;    DestX=DestX-SrcX/ZoomFactor;    SrcX=0;     }
            if (SrcY<0)                 {   SrcH=SrcH+SrcY;                                 DestH=DestH+SrcY/ZoomFactor;    DestY=DestY-SrcY/ZoomFactor;    SrcY=0;     }
            if (SrcX+SrcW>RealImageW)   {   DestW=DestW-(SrcX+SrcW-RealImageW)/ZoomFactor;  SrcW=(RealImageW-SrcX);                                                     }
            if (SrcY+SrcH>RealImageH)   {   DestH=DestH-(SrcY+SrcH-RealImageH)/ZoomFactor;  SrcH=(RealImageH-SrcY);                                                     }

            // translate DestX,DestY,DestW,DestH to destination coordinates
            DestX=((double(W)/VirtImageW)*DestX);
            DestW=((double(W)/VirtImageW)*DestW);
            DestY=((double(H)/VirtImageH)*DestY);
            DestH=((double(H)/VirtImageH)*DestH);

            //**********************************
            // Draw image layer
            //**********************************
            // Shrink to used part of the image
            Image1=ImageToUse->copy(SrcX,SrcY,SrcW,SrcH);
            if (ImageToUse!=SourceImage) {
                delete ImageToUse;
                ImageToUse=NULL;
            }

            // Scaled part as needed
            Image2=Image1.scaled(DestW,DestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

            // Apply global filter to part image
            if (CurObject->TypeObject!=DIAPORAMAOBJECTTYPE_EMPTY) CurObject->FilterTransform.ApplyFilter(&Image2);

            // Apply shot filter to part image
            if (CurObject->TypeObject!=DIAPORAMAOBJECTTYPE_EMPTY) FilterCorrection.ApplyFilter(&Image2);

            P.drawImage(QRectF(DestX,DestY,DestW,DestH),Image2);

            //**********************************
            // Composition layers
            //**********************************
            P.drawImage(QRectF(DestX,DestY,DestW,DestH),GlobalImageComposition,QRectF(SrcX,SrcY,SrcW,SrcH));
        }

        cSoundBlockList *SoundTrackMontage=(IsCurrentObject?Info->CurrentObject_SoundTrackMontage:Info->TransitObject_SoundTrackMontage);
        int             ObjectNumber =IsCurrentObject?Info->CurrentObject_Number:Info->TransitObject_Number;
        int             ShotNumber   =IsCurrentObject?Info->CurrentObject_ShotSequenceNumber:Info->TransitObject_ShotSequenceNumber;
        cDiaporamaShot  *PreviousShot=(ShotNumber>0?&List[ObjectNumber].List[ShotNumber-1]:NULL);
        for (int j=0;j<CurShot->ShotComposition.List.count();j++) {
            cCompositionObject *PrevCompoObject=NULL;
            if (PreviousShot) {
                int k=0;
                while (k<PreviousShot->ShotComposition.List.count()) {
                    if (PreviousShot->ShotComposition.List[k].IndexKey==CurShot->ShotComposition.List[j].IndexKey) {
                        PrevCompoObject=&PreviousShot->ShotComposition.List[k];
                        k=PreviousShot->ShotComposition.List.count();
                    } else k++;
                }
            }
            CurShot->ShotComposition.List[j].DrawCompositionObject(P,0,0,W,H,PreviewMode,IsCurrentObject?Info->CurrentObject_InObjectTime:Info->TransitObject_InObjectTime,SoundTrackMontage,PCTDone,PrevCompoObject);
            // Special case when no sound and video object
            if ((!SoundTrackMontage)&&(CurShot->ShotComposition.List[j].BackgroundBrush.Video)) CurShot->ShotComposition.List[j].BackgroundBrush.Video->NextPacketPosition+=Info->FrameDuration;
        }
        // Special case when no sound and video object
        if ((!SoundTrackMontage)&&(CurObject->Video)) CurObject->Video->NextPacketPosition+=Info->FrameDuration;

        if (!SoundOnly) P.end();
    }
    if (IsCurrentObject) Info->CurrentObject_PreparedImage=Image; else Info->TransitObject_PreparedImage=Image;
}

//=============================================================================================================================
// Function use directly or with thread to make assembly of background and images and make mix (sound & music) when transition
//=============================================================================================================================
void cDiaporama::DoAssembly(cDiaporamaObjectInfo *Info,int W,int H) {
    if (Info->RenderedImage!=NULL) return;    // return immediatly if we have image
    bool SoundOnly=((W==0)&&(H==0));

    // Prepare image
    if (!SoundOnly) {
        QImage  *Image=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
        QPainter P;
        P.begin(Image);
        P.fillRect(0,0,W,H,Qt::black);  // Always start with a black frame
        // Draw background
        if ((Info->IsTransition)&&((Info->CurrentObject_Number==0)||(Info->CurrentObject_BackgroundIndex!=Info->TransitObject_BackgroundIndex))) {
            double Opacity;
            if ((Info->TransitObject)&&(Info->TransitObject_PreparedBackground)) {
                Opacity=1-(Info->TransitionPCTDone);
                P.setOpacity(Opacity);
                P.drawImage(0,0,*Info->TransitObject_PreparedBackground);
            }
            if (Info->CurrentObject_PreparedBackground) {
                Opacity=(Info->TransitionPCTDone);
                P.setOpacity(Opacity);
                P.drawImage(0,0,*Info->CurrentObject_PreparedBackground);
            }
            P.setOpacity(1);
        } else {
            if (Info->CurrentObject_PreparedBackground) P.drawImage(0,0,*Info->CurrentObject_PreparedBackground);
                else P.fillRect(QRect(0,0,W,H),Qt::black);
        }
        // Add prepared images
        if ((Info->IsTransition)&&(Info->CurrentObject_PreparedImage!=NULL)) {
            if (Info->TransitObject_PreparedImage==NULL) {
                Info->TransitObject_PreparedImage=new QImage(Info->CurrentObject_PreparedImage->width(),Info->CurrentObject_PreparedImage->height(),QImage::Format_ARGB32_Premultiplied);
                Info->TransitObject_PreparedImage->fill(0);
                Info->TransitObject_FreePreparedImage=true;
            }
            switch (Info->TransitionFamilly) {
            case TRANSITIONFAMILLY_BASE        : DoBasic(Info,&P,W,H);                  break;
            case TRANSITIONFAMILLY_ZOOMINOUT   : DoZoom(Info,&P,W,H);                   break;
            case TRANSITIONFAMILLY_PUSH        : DoPush(Info,&P,W,H);                   break;
            case TRANSITIONFAMILLY_SLIDE       : DoSlide(Info,&P,W,H);                  break;
            case TRANSITIONFAMILLY_LUMA_BAR    : DoLuma(&LumaList_Bar,Info,&P,W,H);     break;
            case TRANSITIONFAMILLY_LUMA_BOX    : DoLuma(&LumaList_Box,Info,&P,W,H);     break;
            case TRANSITIONFAMILLY_LUMA_CENTER : DoLuma(&LumaList_Center,Info,&P,W,H);  break;
            case TRANSITIONFAMILLY_LUMA_CHECKER: DoLuma(&LumaList_Checker,Info,&P,W,H); break;
            case TRANSITIONFAMILLY_LUMA_CLOCK  : DoLuma(&LumaList_Clock,Info,&P,W,H);   break;
            case TRANSITIONFAMILLY_LUMA_SNAKE  : DoLuma(&LumaList_Snake,Info,&P,W,H);   break;
            }
        } else if (Info->CurrentObject_PreparedImage!=NULL) P.drawImage(0,0,*Info->CurrentObject_PreparedImage);
        P.end();
        Info->RenderedImage=Image;
    }
}


//============================================================================================

void cDiaporama::DoBasic(cDiaporamaObjectInfo *Info,QPainter *P,int,int) {
    switch (Info->TransitionSubType) {
    case 0:
        P->drawImage(0,0,*Info->CurrentObject_PreparedImage);
        break;
    case 1:
        P->setOpacity(1-Info->TransitionPCTDone);
        P->drawImage(0,0,*Info->TransitObject_PreparedImage);
        P->setOpacity(Info->TransitionPCTDone);
        P->drawImage(0,0,*Info->CurrentObject_PreparedImage);
        P->setOpacity(1);
        break;
    }
}

//============================================================================================

void cDiaporama::DoLuma(cLumaList *LumaList,cDiaporamaObjectInfo *Info,QPainter *P,int W,int H) {
    QImage  Img=Info->CurrentObject_PreparedImage->copy();
    if (Info->TransitionSubType<LumaList->List.count()) {
        // Get a copy of luma image scaled to correct size
        QImage  Luma=((W==LUMADLG_WIDTH)&&(H==LUMADLG_HEIGHT))?LumaList->List[Info->TransitionSubType].DlgLumaImage:
                        LumaList->List[Info->TransitionSubType].OriginalLuma.scaled(Info->CurrentObject_PreparedImage->size(),Qt::IgnoreAspectRatio/*,Qt::SmoothTransformation*/).convertToFormat(QImage::Format_ARGB32_Premultiplied);

        // Apply PCTDone to luma mask
        uint8_t limit    =uint8_t(Info->TransitionPCTDone*double(0xff))+1;
        uint32_t *LumaData=(uint32_t *)Luma.bits();
        uint32_t *ImgData =(uint32_t *)Img.bits();
        uint32_t *ImgData2=(uint32_t *)Info->TransitObject_PreparedImage->bits();

        for (int i=0;i<W*H;i++) {
            if (((*LumaData++) & 0xff)>limit) *ImgData=*ImgData2;
            ImgData++;
            ImgData2++;
        }
    }
    // Draw transformed image
    P->drawImage(0,0,Img);
}

//============================================================================================

void cDiaporama::DoZoom(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H) {
    bool    Reverse=(Info->TransitionSubType & 0x1)==1;
    QPoint  box;
    int     wt= int(double(W)*(Reverse?(1-Info->TransitionPCTDone):Info->TransitionPCTDone));
    int     ht= int(double(H)*(Reverse?(1-Info->TransitionPCTDone):Info->TransitionPCTDone));

    switch (Info->TransitionSubType) {
    case 0 :
    case 1 : box=QPoint(0,(H-ht)/2);            break;  // Border Left Center
    case 2 :
    case 3 : box=QPoint(W-wt,(H-ht)/2);         break;  // Border Right Center
    case 4 :
    case 5 : box=QPoint((W-wt)/2,0);            break;  // Border Top Center
    case 6 :
    case 7 : box=QPoint((W-wt)/2,H-ht);         break;  // Border Bottom Center
    case 8 :
    case 9 : box=QPoint(0,0);                   break;  // Upper Left Corner
    case 10:
    case 11: box=QPoint(W-wt,0);                break;  // Upper Right Corner
    case 12:
    case 13: box=QPoint(0,H-ht);                break;  // Bottom Left Corner
    case 14:
    case 15: box=QPoint(W-wt,H-ht);             break;  // Bottom Right Corner
    case 16:
    case 17: box=QPoint((W-wt)/2,(H-ht)/2);     break;  // Center
    }

    // Draw transformed image
    if (!Reverse) {
        // Old image will desapear progressively during the second half time of the transition
        if (Info->TransitionPCTDone<0.5) P->drawImage(0,0,*Info->TransitObject_PreparedImage); else {
            P->setOpacity(1-(Info->TransitionPCTDone-0.5)*2);
            P->drawImage(0,0,*Info->TransitObject_PreparedImage);
            P->setOpacity(1);
        }
    } else {
        // New image will apear immediatly during the old image is moving out
        //P->setOpacity(Info->TransitionPCTDone);
        P->drawImage(0,0,*Info->CurrentObject_PreparedImage);
        //P->setOpacity(1);
    }
    P->drawImage(box,(Reverse?Info->TransitObject_PreparedImage:Info->CurrentObject_PreparedImage)->scaled(QSize(wt,ht)));
}

//============================================================================================

void cDiaporama::DoSlide(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H) {
    QRect   box1,box2;
    bool    Reverse=Info->TransitionSubType>=8;
    double  PCT=(Reverse?(1-Info->TransitionPCTDone):Info->TransitionPCTDone);
    int     PCTW=int(PCT*double(W));
    int     PCTH=int(PCT*double(H));

    switch (Info->TransitionSubType) {
    case 0 :
    case 8 :    box1=QRect(W-PCTW,0,PCTW,H);            box2=QRect(0,0,PCTW,H);                 break;      // Since left to right
    case 1 :
    case 9 :    box1=QRect(0,0,PCTW,H);                 box2=QRect(W-PCTW,0,PCTW,H);            break;      // Since right to left
    case 2 :
    case 10:    box1=QRect(0,H-PCTH,W,PCTH);            box2=QRect(0,0,W,PCTH);                 break;      // Since up to down
    case 3 :
    case 11:    box1=QRect(0,0,W,PCTH);                 box2=QRect(0,H-PCTH,W,PCTH);            break;      // Since down to up
    case 4 :
    case 12:    box1=QRect(W-PCTW,H-PCTH,PCTW,PCTH);    box2=QRect(0,0,PCTW,PCTH);              break;      // Since the upper left corner
    case 5 :
    case 13:    box1=QRect(0,H-PCTH,PCTW,PCTH);         box2=QRect(W-PCTW,0,PCTW,PCTH);         break;      // Since the upper right corner
    case 6 :
    case 14:    box1=QRect(W-PCTW,0,PCTW,PCTH);         box2=QRect(0,H-PCTH,PCTW,PCTH);         break;      // Since the lower left corner
    case 7 :
    case 15:    box1=QRect(0,0,PCTW,PCTH);              box2=QRect(W-PCTW,H-PCTH,PCTW,PCTH);    break;      // Since the lower right corner
    }
    // Draw transformed image
    if (!Reverse) {
        // Old image will desapear progressively during the second half time of the transition
        if (Info->TransitionPCTDone<0.5) P->drawImage(0,0,*Info->TransitObject_PreparedImage); else {
            P->setOpacity(1-(Info->TransitionPCTDone-0.5)*2);
            P->drawImage(0,0,*Info->TransitObject_PreparedImage);
            P->setOpacity(1);
        }
    } else {
        // New image will apear immediatly during the old image is moving out
        //P->setOpacity(Info->TransitionPCTDone);
        P->drawImage(0,0,*Info->CurrentObject_PreparedImage);
        //P->setOpacity(1);
    }
    P->drawImage(box2,Reverse?*Info->TransitObject_PreparedImage:*Info->CurrentObject_PreparedImage,box1);
}

//============================================================================================

void cDiaporama::DoPush(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H) {
    QRect   box1,box2;
    QRect   box3,box4;
    QPoint  box;
    int     wt,ht;
    int     PCTW=int(Info->TransitionPCTDone*double(W));
    int     PCTH=int(Info->TransitionPCTDone*double(H));
    int     PCTWB=int((1-Info->TransitionPCTDone)*double(W));
    int     PCTHB=int((1-Info->TransitionPCTDone)*double(H));

    switch (Info->TransitionSubType) {
    case 0 :    // Since left to right
        box1=QRect(W-PCTW,0,PCTW,H);    box2=QRect(0,0,PCTW,H);         box3=QRect(0,0,PCTWB,H);         box4=QRect(W-PCTWB,0,PCTWB,H);
        P->drawImage(box4,*Info->TransitObject_PreparedImage,box3);
        P->drawImage(box2,*Info->CurrentObject_PreparedImage,box1);
        break;
    case 1 :    // Since right to left
        box1=QRect(0,0,PCTW,H);         box2=QRect(W-PCTW,0,PCTW,H);    box3=QRect(W-PCTWB,0,PCTWB,H);   box4=QRect(0,0,PCTWB,H);
        P->drawImage(box4,*Info->TransitObject_PreparedImage,box3);
        P->drawImage(box2,*Info->CurrentObject_PreparedImage,box1);
        break;
    case 2 :    // Since up to down
        box1=QRect(0,H-PCTH,W,PCTH);    box2=QRect(0,0,W,PCTH);         box3=QRect(0,0,W,PCTHB);         box4=QRect(0,H-PCTHB,W,PCTHB);
        P->drawImage(box4,*Info->TransitObject_PreparedImage,box3);
        P->drawImage(box2,*Info->CurrentObject_PreparedImage,box1);
        break;
    case 3 :    // Since down to up
        box1=QRect(0,0,W,PCTH);         box2=QRect(0,H-PCTH,W,PCTH);    box3=QRect(0,H-PCTHB,W,PCTHB);   box4=QRect(0,0,W,PCTHB);
        P->drawImage(box4,*Info->TransitObject_PreparedImage,box3);
        P->drawImage(box2,*Info->CurrentObject_PreparedImage,box1);
        break;
    case 4 :    // Enterring : zoom in from border Left Center - Previous image : zoom out to border Right Center
        wt=int(double(W)*(1-Info->TransitionPCTDone));
        ht=int(double(H)*(1-Info->TransitionPCTDone));
        box=QPoint(W-wt,(H-ht)/2);
        P->drawImage(box,Info->TransitObject_PreparedImage->scaled(QSize(wt,ht)));
        wt=int(double(W)*Info->TransitionPCTDone);
        ht=int(double(H)*Info->TransitionPCTDone);
        box=QPoint(0,(H-ht)/2);
        P->drawImage(box,Info->CurrentObject_PreparedImage->scaled(QSize(wt,ht)));
        break;
    case 5 :    // Enterring : zoom in from border Right Center - Previous image : zoom out to border Left Center
        wt=int(double(W)*(1-Info->TransitionPCTDone));
        ht=int(double(H)*(1-Info->TransitionPCTDone));
        box=QPoint(0,(H-ht)/2);
        P->drawImage(box,Info->TransitObject_PreparedImage->scaled(QSize(wt,ht)));
        wt=int(double(W)*Info->TransitionPCTDone);
        ht=int(double(H)*Info->TransitionPCTDone);
        box=QPoint(W-wt,(H-ht)/2);
        P->drawImage(box,Info->CurrentObject_PreparedImage->scaled(QSize(wt,ht)));
        break;
    case 6 :    // Enterring : zoom in from border Top Center - Previous image : zoom out to border bottom Center
        wt=int(double(W)*(1-Info->TransitionPCTDone));
        ht=int(double(H)*(1-Info->TransitionPCTDone));
        box=QPoint((W-wt)/2,H-ht);
        P->drawImage(box,Info->TransitObject_PreparedImage->scaled(QSize(wt,ht)));
        wt=int(double(W)*Info->TransitionPCTDone);
        ht=int(double(H)*Info->TransitionPCTDone);
        box=QPoint((W-wt)/2,0);
        P->drawImage(box,Info->CurrentObject_PreparedImage->scaled(QSize(wt,ht)));
        break;
    case 7 :    // Enterring : zoom in from border bottom Center - Previous image : zoom out to border Top Center
        wt=int(double(W)*(1-Info->TransitionPCTDone));
        ht=int(double(H)*(1-Info->TransitionPCTDone));
        box=QPoint((W-wt)/2,0);
        P->drawImage(box,Info->TransitObject_PreparedImage->scaled(QSize(wt,ht)));
        wt=int(double(W)*Info->TransitionPCTDone);
        ht=int(double(H)*Info->TransitionPCTDone);
        box=QPoint((W-wt)/2,H-ht);
        P->drawImage(box,Info->CurrentObject_PreparedImage->scaled(QSize(wt,ht)));
        break;
    }
}

//============================================================================================
// Function use directly or with thread to load source image
// Assume SourceImage is null
// Produce sound only if W and H=0
//============================================================================================

void cDiaporama::LoadSources(cDiaporamaObjectInfo *Info,int W,int H,bool PreviewMode) {
    // W and H = 0 when producing sound track in render process
    bool SoundOnly=((W==0)&&(H==0));

    QFuture<void> ThreadLoadSourceImage;
    QFuture<void> ThreadLoadTransitImage;
    QFuture<void> ThreadPrepareMusic;         // Thread preparation of music
    QFuture<void> ThreadPrepareMusicTransit;  // Thread preparation of music of transit object

    if (Info->CurrentObject==NULL) {
        // Special for preview if no object
        // create an empty transparent image
        if (!SoundOnly) {
            Info->CurrentObject_SourceImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
            QPainter PT;
            PT.begin(Info->CurrentObject_SourceImage);
            PT.setCompositionMode(QPainter::CompositionMode_Source);
            PT.fillRect(QRect(0,0,W,H),Qt::transparent);
            PT.setCompositionMode(QPainter::CompositionMode_SourceOver);
            PT.end();
        } else Info->CurrentObject_SourceImage=new QImage(5,5,QImage::Format_ARGB32_Premultiplied); // Create a very small image to have a ptr !
    } else {
        //==============> Image part
        ThreadLoadSourceImage=QtConcurrent::run(this,&cDiaporama::ThreadLoadSourceVideoImage,Info,PreviewMode,W,H);
        // same job for Transition Object if a previous was not keep !
        if (Info->TransitObject) ThreadLoadTransitImage=QtConcurrent::run(this,&cDiaporama::ThreadLoadTransitVideoImage,Info,PreviewMode,W,H);
        //==============> Background part
        if (!SoundOnly) {
            // Search background context for CurrentObject if a previous was not keep !
            if (Info->CurrentObject_BackgroundBrush==NULL) {
                if ((Info->CurrentObject_BackgroundIndex>=List.count())||(List[Info->CurrentObject_BackgroundIndex].BackgroundType==false))
                    Info->CurrentObject_BackgroundBrush=new QBrush(Qt::black);   // If no background definition @ first object
                    else Info->CurrentObject_BackgroundBrush=List[Info->CurrentObject_BackgroundIndex].BackgroundBrush.GetBrush(QRectF(0,0,W,H),PreviewMode,0,NULL,1,NULL);
                // Create PreparedBackground
                Info->CurrentObject_PreparedBackground=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
                QPainter P;
                P.begin(Info->CurrentObject_PreparedBackground);
                if (Info->CurrentObject_BackgroundBrush) P.fillRect(QRect(0,0,W,H),*Info->CurrentObject_BackgroundBrush); else P.fillRect(0,0,W,H,Qt::black);
                // Apply composition to background
                for (int j=0;j<List[Info->CurrentObject_BackgroundIndex].BackgroundComposition.List.count();j++)
                    List[Info->CurrentObject_BackgroundIndex].BackgroundComposition.List[j].DrawCompositionObject(P,0,0,W,H,PreviewMode,0,NULL,1,NULL);
                P.end();
            }
            // same job for Transition Object if a previous was not keep !
            if ((Info->TransitObject)&&(Info->TransitObject_BackgroundBrush==NULL)) {
                if ((Info->TransitObject_BackgroundIndex>=List.count())||(List[Info->TransitObject_BackgroundIndex].BackgroundType==false))
                    Info->TransitObject_BackgroundBrush=new QBrush(Qt::black);   // If no background definition @ first object
                    else Info->TransitObject_BackgroundBrush=List[Info->TransitObject_BackgroundIndex].BackgroundBrush.GetBrush(QRectF(0,0,W,H),PreviewMode,0,NULL,1,NULL);
                // Create PreparedBackground
                Info->TransitObject_PreparedBackground=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
                QPainter P;
                P.begin(Info->TransitObject_PreparedBackground);
                if (Info->TransitObject_BackgroundBrush) P.fillRect(QRect(0,0,W,H),*Info->TransitObject_BackgroundBrush); else P.fillRect(0,0,W,H,Qt::black);
                // Apply composition to background
                for (int j=0;j<List[Info->TransitObject_BackgroundIndex].BackgroundComposition.List.count();j++)
                    List[Info->TransitObject_BackgroundIndex].BackgroundComposition.List[j].DrawCompositionObject(P,0,0,W,H,PreviewMode,0,NULL,1,NULL);
                P.end();
            }
        }
        // Start threads for next music bloc
        if ((Info->CurrentObject)&&(Info->CurrentObject_MusicTrack))
            ThreadPrepareMusic=QtConcurrent::run(this,&cDiaporama::PrepareMusicBloc,Info->CurrentObject_Number,Info->CurrentObject_InObjectTime,Info->CurrentObject_MusicTrack);
        if ((Info->TransitObject)&&(Info->TransitObject_MusicTrack))
            ThreadPrepareMusicTransit=QtConcurrent::run(this,&cDiaporama::PrepareMusicBloc,Info->TransitObject_Number,Info->TransitObject_InObjectTime,Info->TransitObject_MusicTrack);
    }

    // Wait for threads to be finished
    ThreadLoadSourceImage.waitForFinished();
    ThreadLoadTransitImage.waitForFinished();

    // Soundtrack mix with fade in/fade out
    if ((Info->IsTransition)&&((Info->CurrentObject_SoundTrackMontage)||(Info->TransitObject_SoundTrackMontage))) {

        if (Info->CurrentObject_SoundTrackMontage==NULL) {
            // if we don't have a CurrentObject_SoundTrackMontage, we need to creat it because it's the destination !
            Info->CurrentObject_SoundTrackMontage=new cSoundBlockList();
            Info->CurrentObject_SoundTrackMontage->SetFPS(Info->TransitObject_SoundTrackMontage->FPS);
        }
        // Ensure this track have enough data
        while (Info->CurrentObject_SoundTrackMontage->List.count()<Info->CurrentObject_SoundTrackMontage->NbrPacketForFPS) {
            Info->CurrentObject_SoundTrackMontage->AppendNullSoundPacket();
        }

        int MaxPacket=0;
        MaxPacket=Info->CurrentObject_SoundTrackMontage->List.count();
        if ((Info->TransitObject_SoundTrackMontage)&&(MaxPacket>Info->TransitObject_SoundTrackMontage->List.count())) MaxPacket=Info->TransitObject_SoundTrackMontage->List.count();
        if (MaxPacket>Info->CurrentObject_SoundTrackMontage->NbrPacketForFPS) MaxPacket=Info->CurrentObject_SoundTrackMontage->NbrPacketForFPS;

        for (int i=0;i<MaxPacket;i++) {
            // Mix the 2 sources buffer using the first buffer as destination and remove one paquet from the TransitObject_SoundTrackMontage
            int16_t *Paquet=Info->TransitObject_SoundTrackMontage?Info->TransitObject_SoundTrackMontage->DetachFirstPacket():NULL;
            int32_t mix;
            int16_t *Buf1=Info->CurrentObject_SoundTrackMontage->List[i];
            int     Max=Info->CurrentObject_SoundTrackMontage->SoundPacketSize/(Info->CurrentObject_SoundTrackMontage->SampleBytes*Info->CurrentObject_SoundTrackMontage->Channels);
            double  FadeDelta=(double(i)/double(MaxPacket))*(Info->TransitionPCTEnd-Info->TransitionPCTDone);
            double  FadeAdjust   =(Info->TransitionPCTDone+FadeDelta);;
            double  FadeAdjust2  =(1-Info->TransitionPCTDone-FadeDelta);

            int16_t *Buf2=(Paquet!=NULL)?Paquet:NULL;
            for (int j=0;j<Max;j++) {
                // Left channel : Adjust if necessary (16 bits)
                if (Buf2) mix=int32_t(double(*(Buf1))*FadeAdjust+double(*(Buf2++))*FadeAdjust2); else mix=int32_t(double(*(Buf1))*FadeAdjust);
                if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                *(Buf1++)=int16_t(mix);
                // Right channel : Adjust if necessary (16 bits)
                if (Buf2) mix=int32_t(double(*(Buf1))*FadeAdjust+double(*(Buf2++))*FadeAdjust2); else mix=int32_t(double(*(Buf1))*FadeAdjust);
                if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                *(Buf1++)=int16_t(mix);
            }
            if (Paquet) av_free(Paquet);
        }
    }

    // Ensure thread for music is finished
    ThreadPrepareMusic.waitForFinished();
    ThreadPrepareMusicTransit.waitForFinished();

    // Mix music with fade out of previous music track (if needed)
    if ((Info->IsTransition)&&(Info->TransitObject_MusicTrack)&&(Info->TransitObject_MusicTrack->List.count()>0)) {
        int MaxPacket=0;
        MaxPacket=Info->CurrentObject_MusicTrack->List.count();
        if ((MaxPacket>Info->TransitObject_MusicTrack->List.count())) MaxPacket=Info->TransitObject_MusicTrack->List.count();
        if (MaxPacket>Info->CurrentObject_MusicTrack->NbrPacketForFPS) MaxPacket=Info->CurrentObject_MusicTrack->NbrPacketForFPS;

        for (int i=0;i<MaxPacket;i++) {
            // Mix the 2 sources buffer using the first buffer as destination and remove one paquet from the PreviousMusicTrack
            int16_t *Paquet=Info->TransitObject_MusicTrack->DetachFirstPacket();
            int32_t mix;
            int16_t *Buf1=Info->CurrentObject_MusicTrack->List[i];
            int     Max=Info->CurrentObject_MusicTrack->SoundPacketSize/(Info->CurrentObject_MusicTrack->SampleBytes*Info->CurrentObject_MusicTrack->Channels);
            double  FadeDelta=(double(i)/double(MaxPacket))*(Info->TransitionPCTEnd-Info->TransitionPCTDone);
            double  FadeAdjust2  =(1-Info->TransitionPCTDone-FadeDelta);

            int16_t *Buf2=(Paquet!=NULL)?Paquet:NULL;
            for (int j=0;j<Max;j++) {
                // Left channel : Adjust if necessary (16 bits)
                mix=int32_t(*(Buf1)+double(*(Buf2++))*FadeAdjust2);
                if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                *(Buf1++)=int16_t(mix);
                // Right channel : Adjust if necessary (16 bits)
                mix=int32_t(*(Buf1)+double(*(Buf2++))*FadeAdjust2);
                if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                *(Buf1++)=int16_t(mix);
            }
            if (Paquet) av_free(Paquet);
        }
    }
}

void cDiaporama::ThreadLoadSourceVideoImage(cDiaporamaObjectInfo *Info,bool PreviewMode,int W,int H) {
    if (Info->CurrentObject->Video) {
        Info->CurrentObject_SourceImage=Info->CurrentObject->Video->ImageAt(PreviewMode,Info->CurrentObject_InObjectTime,false,Info->CurrentObject_SoundTrackMontage,Info->CurrentObject->Video->SoundVolume,false,NULL);
    } else if (Info->CurrentObject->Image) {
        if (Info->CurrentObject_SourceImage==NULL) Info->CurrentObject_SourceImage=Info->CurrentObject->Image->ImageAt(PreviewMode,false,NULL);
    } else {
        // W and H = 0 when producing sound track in render process
        if ((W!=0)&&(H!=0)) {
            // Title mode : create an empty transparent image
            Info->CurrentObject_SourceImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
            QPainter PT;
            PT.begin(Info->CurrentObject_SourceImage);
            PT.setCompositionMode(QPainter::CompositionMode_Source);
            PT.fillRect(QRect(0,0,W,H),Qt::transparent);
            PT.setCompositionMode(QPainter::CompositionMode_SourceOver);
            PT.end();
        } else {
            // Create a very small image to have a ptr
            Info->CurrentObject_SourceImage=new QImage(5,5,QImage::Format_ARGB32_Premultiplied);
        }
    }
    // Prepare images for Current Object
    PrepareImage(Info,W,H,true,PreviewMode);

}

void cDiaporama::ThreadLoadTransitVideoImage(cDiaporamaObjectInfo *Info,bool PreviewMode,int W,int H) {
    if (Info->TransitObject->Video) {
        Info->TransitObject_SourceImage=Info->TransitObject->Video->ImageAt( // Video
            PreviewMode,Info->TransitObject_InObjectTime,false,Info->TransitObject_SoundTrackMontage,Info->TransitObject->Video->SoundVolume,false,NULL);
    } else if ((Info->TransitObject->Image)&&(Info->TransitObject_SourceImage==NULL)) {
        Info->TransitObject_SourceImage=Info->TransitObject->Image->ImageAt(PreviewMode,false,NULL);
    } else {
        // Title mode : create an empty transparent image
        Info->TransitObject_SourceImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
        QPainter PT;
        PT.begin(Info->TransitObject_SourceImage);
        PT.setCompositionMode(QPainter::CompositionMode_Source);
        PT.fillRect(QRect(0,0,W,H),Qt::transparent);
        PT.setCompositionMode(QPainter::CompositionMode_SourceOver);
        PT.end();
    }
    // Prepare images for Transit Object
    PrepareImage(Info,W,H,false,PreviewMode);
}

//*********************************************************************************************************************************************
// Class object for rendering
//*********************************************************************************************************************************************

// make a copy of PreviousFrame
cDiaporamaObjectInfo::cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame) {
    FrameDuration                       =PreviousFrame->FrameDuration;
    RenderedImage                       =NULL;                                              // Final image rendered
    FreeRenderedImage                   =true;                                              // True if allow to delete RenderedImage during destructor
    TransitionFamilly                   =PreviousFrame->TransitionFamilly;                  // Transition familly
    TransitionSubType                   =PreviousFrame->TransitionSubType;                  // Transition type in the familly
    TransitionDuration                  =PreviousFrame->TransitionDuration;                 // Transition duration (in msec)

    // Current object
    CurrentObject_Number                =PreviousFrame->CurrentObject_Number;               // Object number
    CurrentObject_StartTime             =PreviousFrame->CurrentObject_StartTime;            // Position (in msec) of the first frame relative to the diaporama
    CurrentObject_InObjectTime          =PreviousFrame->CurrentObject_InObjectTime;         // Position (in msec) in the object
    CurrentObject                       =PreviousFrame->CurrentObject;                      // Link to the current object
    CurrentObject_ShotSequenceNumber    =PreviousFrame->CurrentObject_ShotSequenceNumber;   // Number of the shot sequence in the current object
    CurrentObject_CurrentShot           =PreviousFrame->CurrentObject_CurrentShot;          // Link to the current shot in the current object
    CurrentObject_CurrentShotType       =PreviousFrame->CurrentObject_CurrentShotType;      // Type of the current shot : Static/Mobil/Video
    CurrentObject_EndStaticShot         =PreviousFrame->CurrentObject_EndStaticShot;        // Time the static shot end (if CurrentObject_CurrentShotType=SHOTTYPE_STATIC)
    CurrentObject_MobilPCTDone          =PreviousFrame->CurrentObject_MobilPCTDone;         // PCT achevement for mobil shot (if CurrentObject_CurrentShotType=SHOTTYPE_MOBIL)
    CurrentObject_PCTDone               =PreviousFrame->CurrentObject_PCTDone;
    CurrentObject_SourceImage           =PreviousFrame->CurrentObject_SourceImage;          // Source image
    CurrentObject_FreeSourceImage       =false;                                             // True if allow to delete CurrentObject_SourceImage during destructor
    CurrentObject_BackgroundIndex       =PreviousFrame->CurrentObject_BackgroundIndex;      // Object number containing current background definition
    CurrentObject_BackgroundBrush       =PreviousFrame->CurrentObject_BackgroundBrush;      // Current background brush
    CurrentObject_FreeBackgroundBrush   =false;                                             // True if allow to delete CurrentObject_BackgroundBrush during destructor
    CurrentObject_PreparedBackground    =PreviousFrame->CurrentObject_PreparedBackground;   // Current image produce for background
    CurrentObject_FreePreparedBackground=false;                                             // True if allow to delete CurrentObject_FreePreparedBackground during destructor
    CurrentObject_SoundTrackMontage     =PreviousFrame->CurrentObject_SoundTrackMontage;    // Sound for playing sound from montage track
    CurrentObject_FreeSoundTrackMontage =false;                                             // True if allow to delete CurrentObject_SoundTrackMontage during destructor
    CurrentObject_PreparedImage         =PreviousFrame->CurrentObject_PreparedImage;        // Current image prepared
    CurrentObject_FreePreparedImage     =false;                                             // True if allow to delete CurrentObject_PreparedImage during destructor
    CurrentObject_MusicTrack            =PreviousFrame->CurrentObject_MusicTrack;           // Sound for playing music from music track
    CurrentObject_FreeMusicTrack        =false;                                             // True if allow to delete CurrentObject_MusicTrack during destructor
    CurrentObject_MusicObject           =PreviousFrame->CurrentObject_MusicObject;          // Ref to the current playing music

    // Transitionnal object
    IsTransition                        =PreviousFrame->IsTransition;                       // True if transition in progress
    TransitionPCTDone                   =PreviousFrame->TransitionPCTDone;                  // PCT achevement for transition
    TransitionPCTEnd                    =PreviousFrame->TransitionPCTEnd;                   // PCT achevement @ end of this frame for fade in/out
    TransitObject_Number                =PreviousFrame->TransitObject_Number;               // Object number
    TransitObject_StartTime             =PreviousFrame->TransitObject_StartTime;            // Position (in msec) of the first frame relative to the diaporama
    TransitObject_InObjectTime          =PreviousFrame->TransitObject_InObjectTime;         // Position (in msec) in the object
    TransitObject                       =PreviousFrame->TransitObject;                      // Link to the current object
    TransitObject_ShotSequenceNumber    =PreviousFrame->TransitObject_ShotSequenceNumber;   // Number of the shot sequence in the current object
    TransitObject_CurrentShot           =PreviousFrame->TransitObject_CurrentShot;          // Link to the current shot in the current object
    TransitObject_CurrentShotType       =PreviousFrame->TransitObject_CurrentShotType;      // Type of the current shot : Static/Mobil/Video
    TransitObject_EndStaticShot         =PreviousFrame->TransitObject_EndStaticShot;        // Time the static shot end (if TransitObject_CurrentShotType=SHOTTYPE_STATIC)
    TransitObject_MobilPCTDone          =PreviousFrame->TransitObject_MobilPCTDone;         // PCT achevement for mobil shot (if TransitObject_CurrentShotType=SHOTTYPE_MOBIL)
    TransitObject_PCTDone               =PreviousFrame->TransitObject_PCTDone;
    TransitObject_SourceImage           =PreviousFrame->TransitObject_SourceImage;          // Source image
    TransitObject_FreeSourceImage       =false;                                             // True if allow to delete TransitObject_SourceImage during destructor
    TransitObject_BackgroundIndex       =PreviousFrame->TransitObject_BackgroundIndex;      // Object number containing current background definition
    TransitObject_BackgroundBrush       =PreviousFrame->TransitObject_BackgroundBrush;      // Current background brush
    TransitObject_FreeBackgroundBrush   =false;                                             // True if allow to delete TransitObject_BackgroundBrush during destructor
    TransitObject_PreparedBackground    =PreviousFrame->TransitObject_PreparedBackground;   // Current image produce for background
    TransitObject_FreePreparedBackground=false;                                             // True if allow to delete TransitObject_PreparedBackground during destructor
    TransitObject_SoundTrackMontage     =PreviousFrame->TransitObject_SoundTrackMontage;    // Sound for playing sound from montage track
    TransitObject_FreeSoundTrackMontage =false;                                             // True if allow to delete TransitObject_SoundTrackMontage during destructor
    TransitObject_PreparedImage         =PreviousFrame->TransitObject_PreparedImage;        // Current image prepared
    TransitObject_FreePreparedImage     =false;                                             // True if allow to delete TransitObject_PreparedImage during destructor
    TransitObject_MusicTrack            =PreviousFrame->TransitObject_MusicTrack;           // Sound for playing music from music track
    TransitObject_FreeMusicTrack        =false;                                             // True if allow to delete TransitObject_MusicTrack during destructor
    TransitObject_MusicObject           =PreviousFrame->TransitObject_MusicObject;          // Ref to the current playing music
}


cDiaporamaObjectInfo::cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame,int TimePosition,cDiaporama *Diaporama,double TheFrameDuration) {
    //==============> Pre-initialise all values
    FrameDuration                       =TheFrameDuration;
    RenderedImage                       =NULL;              // Final image rendered
    FreeRenderedImage                   =true;              // True if allow to delete RenderedImage during destructor

    // Current object
    CurrentObject_Number                =0;                 // Object number
    CurrentObject_StartTime             =0;                 // Position (in msec) of the first frame relative to the diaporama
    CurrentObject_InObjectTime          =0;                 // Position (in msec) in the object
    CurrentObject                       =NULL;              // Link to the current object
    CurrentObject_ShotSequenceNumber    =0;                 // Number of the shot sequence in the current object
    CurrentObject_CurrentShot           =NULL;              // Link to the current shot in the current object
    CurrentObject_CurrentShotType       =0;                 // Type of the current shot : Static/Mobil/Video
    CurrentObject_EndStaticShot         =0;                 // Time the static shot end (if CurrentObject_CurrentShotType=SHOTTYPE_STATIC)
    CurrentObject_MobilPCTDone          =0;                 // PCT achevement for mobil shot (if CurrentObject_CurrentShotType=SHOTTYPE_MOBIL)
    CurrentObject_PCTDone               =0;                 // PCT achevement for static shot
    CurrentObject_SourceImage           =NULL;              // Source image
    CurrentObject_FreeSourceImage       =true;              // True if allow to delete CurrentObject_SourceImage during destructor
    CurrentObject_BackgroundIndex       =0;                 // Object number containing current background definition
    CurrentObject_BackgroundBrush       =NULL;              // Current background brush
    CurrentObject_FreeBackgroundBrush   =true;              // True if allow to delete CurrentObject_BackgroundBrush during destructor
    CurrentObject_PreparedBackground    =NULL;              // Current image produce for background
    CurrentObject_FreePreparedBackground=true;              // True if allow to delete CurrentObject_FreePreparedBackground during destructor
    CurrentObject_SoundTrackMontage     =NULL;              // Sound for playing sound from montage track
    CurrentObject_FreeSoundTrackMontage =true;              // True if allow to delete CurrentObject_SoundTrackMontage during destructor
    CurrentObject_PreparedImage         =NULL;              // Current image prepared
    CurrentObject_FreePreparedImage     =true;              // True if allow to delete CurrentObject_PreparedImage during destructor
    CurrentObject_MusicTrack            =NULL;              // Sound for playing music from music track
    CurrentObject_FreeMusicTrack        =true;              // True if allow to delete CurrentObject_MusicTrack during destructor
    CurrentObject_MusicObject           =NULL;              // Ref to the current playing music

    // Transitionnal object
    IsTransition                        =false;             // True if transition in progress
    TransitionPCTDone                   =0;                 // PCT achevement for transition
    TransitionPCTEnd                    =0;                 // PCT achevement @ end of this frame for fade in/out
    TransitObject_Number                =0;                 // Object number
    TransitObject_StartTime             =0;                 // Position (in msec) of the first frame relative to the diaporama
    TransitObject_InObjectTime          =0;                 // Position (in msec) in the object
    TransitObject                       =NULL;              // Link to the current object
    TransitObject_ShotSequenceNumber    =0;                 // Number of the shot sequence in the current object
    TransitObject_CurrentShot           =NULL;              // Link to the current shot in the current object
    TransitObject_CurrentShotType       =0;                 // Type of the current shot : Static/Mobil/Video
    TransitObject_EndStaticShot         =0;                 // Time the static shot end (if TransitObject_CurrentShotType=SHOTTYPE_STATIC)
    TransitObject_MobilPCTDone          =0;                 // PCT achevement for mobil shot (if TransitObject_CurrentShotType=SHOTTYPE_MOBIL)
    TransitObject_PCTDone               =0;                 // PCT achevement for static shot
    TransitObject_SourceImage           =NULL;              // Source image
    TransitObject_FreeSourceImage       =true;              // True if allow to delete TransitObject_SourceImage during destructor
    TransitObject_BackgroundIndex       =0;                 // Object number containing current background definition
    TransitObject_BackgroundBrush       =NULL;              // Current background brush
    TransitObject_FreeBackgroundBrush   =true;              // True if allow to delete TransitObject_BackgroundBrush during destructor
    TransitObject_PreparedBackground    =NULL;              // Current image produce for background
    TransitObject_FreePreparedBackground=true;              // True if allow to delete TransitObject_PreparedBackground during destructor
    TransitObject_SoundTrackMontage     =NULL;              // Sound for playing sound from montage track
    TransitObject_FreeSoundTrackMontage =true;              // True if allow to delete TransitObject_SoundTrackMontage during destructor
    TransitObject_PreparedImage         =NULL;              // Current image prepared
    TransitObject_FreePreparedImage     =true;              // True if allow to delete TransitObject_PreparedImage during destructor
    TransitObject_MusicTrack            =NULL;              // Sound for playing music from music track
    TransitObject_FreeMusicTrack        =true;              // True if allow to delete TransitObject_MusicTrack during destructor
    TransitObject_MusicObject           =NULL;              // Ref to the current playing music

    if (Diaporama) {
        //==============> Retrieve object informations depending on position (in msec)
        // Search wich object for given time position
        while ((CurrentObject_Number<Diaporama->List.count())&&
               ((CurrentObject_StartTime+(Diaporama->List[CurrentObject_Number].GetDuration()-Diaporama->GetTransitionDuration(CurrentObject_Number+1))<=TimePosition))) {

            CurrentObject_StartTime=CurrentObject_StartTime+Diaporama->List[CurrentObject_Number].GetDuration()-Diaporama->GetTransitionDuration(CurrentObject_Number+1);
            CurrentObject_Number++;
        }
        // Adjust CurrentObject_Number
        if (CurrentObject_Number>=Diaporama->List.count()) {
            if (Diaporama->List.count()>0) {   // Force to last object
                CurrentObject_Number    =Diaporama->List.count()-1;
                CurrentObject           =&Diaporama->List[CurrentObject_Number];
                CurrentObject_StartTime =Diaporama->GetObjectStartPosition(CurrentObject_Number);
            } else {                // Force to first or none object
                CurrentObject_Number   =0;
                CurrentObject_StartTime=0;
                CurrentObject=NULL;
            }
        } else CurrentObject=&Diaporama->List[CurrentObject_Number];
        CurrentObject_InObjectTime=TimePosition-CurrentObject_StartTime;

        // Now calculate wich sequence in the current object is
        if (CurrentObject) {
            int CurPos  =0;
            while ((CurrentObject_ShotSequenceNumber<CurrentObject->List.count()-1)&&
                   ((CurPos+(CurrentObject_ShotSequenceNumber>0?CurrentObject->List[CurrentObject_ShotSequenceNumber].GetMobilDuration():0)+
                     CurrentObject->List[CurrentObject_ShotSequenceNumber].GetStaticDuration())<=CurrentObject_InObjectTime)) {
                CurPos=CurPos+(CurrentObject_ShotSequenceNumber>0?CurrentObject->List[CurrentObject_ShotSequenceNumber].GetMobilDuration():0)+
                       CurrentObject->List[CurrentObject_ShotSequenceNumber].GetStaticDuration();
                CurrentObject_ShotSequenceNumber++;
            }
            CurrentObject_CurrentShot=&CurrentObject->List[CurrentObject_ShotSequenceNumber];

            // Now calculate type of sequence and PCTDone for mobile sequence and CurrentObject_EndStaticShot for static sequence
            if ((CurrentObject_ShotSequenceNumber>0)&&((CurrentObject_InObjectTime-CurPos)<(CurrentObject_ShotSequenceNumber>0?CurrentObject_CurrentShot->GetMobilDuration():0))) {
                CurrentObject_CurrentShotType =SHOTTYPE_MOBIL;
                switch (Diaporama->SpeedWave) {
                case SPEEDWAVE_LINEAR :
                    CurrentObject_MobilPCTDone=(double(CurrentObject_InObjectTime)-double(CurPos))/(double(CurrentObject_CurrentShot->GetMobilDuration()));
                    break;
                case SPEEDWAVE_SINQUARTER :
                    CurrentObject_MobilPCTDone=(double(CurrentObject_InObjectTime)-double(CurPos))/(double(CurrentObject_CurrentShot->GetMobilDuration()));
                    CurrentObject_MobilPCTDone=sin(1.5708*CurrentObject_MobilPCTDone);
                    break;
                }

            } else {

                CurrentObject_CurrentShotType =SHOTTYPE_STATIC;
                switch (Diaporama->SpeedWave) {
                case SPEEDWAVE_LINEAR :
                    CurrentObject_PCTDone=(double(CurrentObject_InObjectTime)-double(CurPos))/(double(CurrentObject_CurrentShot->GetStaticDuration()));
                    break;
                case SPEEDWAVE_SINQUARTER :
                    CurrentObject_PCTDone=(double(CurrentObject_InObjectTime)-double(CurPos))/(double(CurrentObject_CurrentShot->GetStaticDuration()));
                    CurrentObject_PCTDone=sin(1.5708*CurrentObject_PCTDone);
                    break;
                }

                // Force all to SHOTTYPE_VIDEO
                CurrentObject_CurrentShotType=SHOTTYPE_VIDEO;
            }
        } else {
            CurrentObject_ShotSequenceNumber=0;
            CurrentObject_CurrentShot       =NULL;
            CurrentObject_CurrentShotType   =SHOTTYPE_STATIC;
        }

        // Calculate wich BackgroundIndex to be use (Background type : false=same as precedent - true=new background definition)
        CurrentObject_BackgroundIndex=CurrentObject_Number;
        while ((CurrentObject_BackgroundIndex>0)&&(!Diaporama->List[CurrentObject_BackgroundIndex].BackgroundType)) CurrentObject_BackgroundIndex--;

        // Define if for this position we have a transition with a previous object
        if ((CurrentObject!=NULL)&&((CurrentObject->TransitionFamilly!=0)||(CurrentObject->TransitionSubType!=0))&&(CurrentObject_InObjectTime<CurrentObject->TransitionDuration)) {
            TransitionFamilly =CurrentObject->TransitionFamilly;                      // Transition familly
            TransitionSubType =CurrentObject->TransitionSubType;                      // Transition type in the familly
            TransitionDuration=CurrentObject->TransitionDuration;                     // Transition duration (in msec)
            IsTransition      =true;
            switch (Diaporama->SpeedWave) {
            case SPEEDWAVE_LINEAR :
                TransitionPCTDone=double(CurrentObject_InObjectTime)/double(TransitionDuration);
                break;
            case SPEEDWAVE_SINQUARTER :
                TransitionPCTDone=double(CurrentObject_InObjectTime)/double(TransitionDuration);
                TransitionPCTDone=sin(1.5708*TransitionPCTDone);
                TransitionPCTEnd =(double(CurrentObject_InObjectTime)+FrameDuration)/double(TransitionDuration);
                TransitionPCTEnd =sin(1.5708*TransitionPCTDone);
                break;
            }
            // If CurrentObject is not the first object
            if (CurrentObject_Number>0) {
                TransitObject_Number        =CurrentObject_Number-1;
                TransitObject               =&Diaporama->List[TransitObject_Number];
                TransitObject_StartTime     =Diaporama->GetObjectStartPosition(TransitObject_Number);
                TransitObject_InObjectTime  =TimePosition-TransitObject_StartTime;
                // Now calculate wich sequence in the Transition object is
                int CurPos  =0;
                while ((TransitObject_ShotSequenceNumber<TransitObject->List.count()-1)&&
                       ((CurPos+(TransitObject_ShotSequenceNumber>0?TransitObject->List[TransitObject_ShotSequenceNumber].GetMobilDuration():0)+
                         TransitObject->List[TransitObject_ShotSequenceNumber].GetStaticDuration())<=TransitObject_InObjectTime)) {
                    CurPos=CurPos+(TransitObject_ShotSequenceNumber>0?TransitObject->List[TransitObject_ShotSequenceNumber].GetMobilDuration():0)+
                           TransitObject->List[TransitObject_ShotSequenceNumber].GetStaticDuration();
                    TransitObject_ShotSequenceNumber++;
                }
                TransitObject_CurrentShot=&TransitObject->List[TransitObject_ShotSequenceNumber];
                // Now calculate type of sequence and PCTDone for mobile sequence and CurrentObject_EndStaticShot for static sequence
                if ((TransitObject_ShotSequenceNumber>0)&&((TransitObject_InObjectTime-CurPos)<(TransitObject_ShotSequenceNumber>0?TransitObject_CurrentShot->GetMobilDuration():0))) {
                    TransitObject_CurrentShotType =SHOTTYPE_MOBIL;
                    TransitObject_MobilPCTDone    =(double(TransitObject_InObjectTime)-double(CurPos))/(double(TransitObject_CurrentShot->GetMobilDuration()));
                } else {
                    /*if (TransitObject->Video==NULL) {
                        TransitObject_CurrentShotType=SHOTTYPE_STATIC;
                        TransitObject_EndStaticShot  =TransitObject_InObjectTime+TransitObject_CurrentShot->GetStaticDuration();
                    } else TransitObject_CurrentShotType=SHOTTYPE_VIDEO;*/
                    // Force all to SHOTTYPE_VIDEO
                    TransitObject_CurrentShotType=SHOTTYPE_VIDEO;
                }
                // Calculate wich BackgroundIndex to be use for transition object (Background type : false=same as precedent - true=new background definition)
                TransitObject_BackgroundIndex=TransitObject_Number;
                while ((TransitObject_BackgroundIndex>0)&&(!Diaporama->List[TransitObject_BackgroundIndex].BackgroundType)) TransitObject_BackgroundIndex--;
            }
        }

        // Search music objects
        int StartPosition;
        if (CurrentObject!=NULL) CurrentObject_MusicObject=Diaporama->GetMusicObject(CurrentObject_Number,StartPosition);
        if (TransitObject!=NULL) TransitObject_MusicObject=Diaporama->GetMusicObject(TransitObject_Number,StartPosition);

        //==============> Try to re-use values from PreviousFrame
        if (PreviousFrame) {
            //************ Background
            if (PreviousFrame->CurrentObject_BackgroundIndex==CurrentObject_BackgroundIndex) {
                CurrentObject_BackgroundBrush=PreviousFrame->CurrentObject_BackgroundBrush;             // Use the same background
                PreviousFrame->CurrentObject_FreeBackgroundBrush=false;                                 // Set tag to not delete previous background
                CurrentObject_PreparedBackground=PreviousFrame->CurrentObject_PreparedBackground;
                PreviousFrame->CurrentObject_FreePreparedBackground=false;
            }
            // Background of transition Object
            if (TransitObject) {
                if (PreviousFrame->CurrentObject_BackgroundIndex==TransitObject_BackgroundIndex) {
                    TransitObject_BackgroundBrush=PreviousFrame->CurrentObject_BackgroundBrush;             // Use the same background
                    PreviousFrame->CurrentObject_FreeBackgroundBrush=false;                                 // Set tag to not delete previous background
                    TransitObject_PreparedBackground=PreviousFrame->CurrentObject_PreparedBackground;
                    PreviousFrame->CurrentObject_FreePreparedBackground=false;
                } else if (PreviousFrame->TransitObject_BackgroundIndex==TransitObject_BackgroundIndex) {
                    TransitObject_BackgroundBrush=PreviousFrame->TransitObject_BackgroundBrush;         // Use the same background
                    PreviousFrame->TransitObject_FreeBackgroundBrush=false;                             // Set tag to not delete previous background
                    TransitObject_PreparedBackground=PreviousFrame->TransitObject_PreparedBackground;
                    PreviousFrame->TransitObject_FreePreparedBackground=false;
                }
                // Special case to disable free of background brush if transit object and current object use the same
                if (TransitObject_BackgroundBrush==CurrentObject_BackgroundBrush) {
                    TransitObject_FreeBackgroundBrush=false;
                    TransitObject_FreePreparedBackground=false;
                }
            }

            //************ SourceImage
            if ((PreviousFrame->CurrentObject_CurrentShotType!=SHOTTYPE_VIDEO)&&(PreviousFrame->CurrentObject_Number==CurrentObject_Number)) {
                CurrentObject_SourceImage=PreviousFrame->CurrentObject_SourceImage;                     // Use the same image source
                PreviousFrame->CurrentObject_FreeSourceImage=false;                                     // Set tag to not delete previous source image
            }
            // SourceImage of transition Object
            if (TransitObject) {
                if ((PreviousFrame->CurrentObject_CurrentShotType!=SHOTTYPE_VIDEO)&&(PreviousFrame->CurrentObject_Number==TransitObject_Number)) {
                    TransitObject_SourceImage=PreviousFrame->CurrentObject_SourceImage;                 // Use the same image source
                    PreviousFrame->CurrentObject_FreeSourceImage=false;                                 // Set tag to not delete previous source image
                } else if ((PreviousFrame->TransitObject_CurrentShotType!=SHOTTYPE_VIDEO)&&(PreviousFrame->TransitObject_Number==TransitObject_Number)) {
                        TransitObject_SourceImage=PreviousFrame->TransitObject_SourceImage;             // Use the same image source
                        PreviousFrame->TransitObject_FreeSourceImage=false;                             // Set tag to not delete previous source image
                }
            }

            //************ SoundTrackMontage
            if ((PreviousFrame->CurrentObject_Number==CurrentObject_Number)) {
                CurrentObject_SoundTrackMontage=PreviousFrame->CurrentObject_SoundTrackMontage;         // Use the same SoundTrackMontage
                PreviousFrame->CurrentObject_FreeSoundTrackMontage=false;                               // Set tag to not delete previous SoundTrackMontage
            }
            // SoundTrackMontage of transition Object
            if (TransitObject) {
                if ((PreviousFrame->CurrentObject_Number==TransitObject_Number)) {
                    TransitObject_SoundTrackMontage=PreviousFrame->CurrentObject_SoundTrackMontage;     // Use the same SoundTrackMontage
                    PreviousFrame->CurrentObject_FreeSoundTrackMontage=false;                           // Set tag to not delete previous SoundTrackMontage
                } else if ((PreviousFrame->TransitObject_Number==TransitObject_Number)) {
                    TransitObject_SoundTrackMontage=PreviousFrame->TransitObject_SoundTrackMontage;     // Use the same SoundTrackMontage
                    PreviousFrame->TransitObject_FreeSoundTrackMontage=false;                           // Set tag to not delete previous SoundTrackMontage
                }
            }

            //************ Music
            if ((PreviousFrame->CurrentObject_MusicObject==CurrentObject_MusicObject)) {
                CurrentObject_MusicTrack=PreviousFrame->CurrentObject_MusicTrack;                       // Use the same Music track
                PreviousFrame->CurrentObject_FreeMusicTrack=false;                                      // Set tag to not delete previous SoundTrackMontage
            }
            // Music of transition Object
            if (TransitObject) {
                if ((PreviousFrame->CurrentObject_MusicObject==TransitObject_MusicObject)) {
                    TransitObject_MusicTrack=PreviousFrame->CurrentObject_MusicTrack;                   // Use the same SoundTrackMontage
                    PreviousFrame->CurrentObject_FreeMusicTrack=false;                                  // Set tag to not delete previous SoundTrackMontage
                } else if ((PreviousFrame->TransitObject_MusicObject==TransitObject_MusicObject)) {
                    TransitObject_MusicTrack=PreviousFrame->TransitObject_MusicTrack;                   // Use the same SoundTrackMontage
                    PreviousFrame->TransitObject_FreeMusicTrack=false;                                  // Set tag to not delete previous SoundTrackMontage
                }
                // Special case to disable TransitObject_MusicTrack if transit object and current object use the same
                if (CurrentObject_MusicObject==TransitObject_MusicObject) {
                    TransitObject_FreeMusicTrack=false;
                    TransitObject_MusicTrack=NULL;
                }
            }

            //************ PreparedImage
            if ((PreviousFrame->CurrentObject_CurrentShot==CurrentObject_CurrentShot)&&                 // Same shot
                (PreviousFrame->CurrentObject_CurrentShotType==SHOTTYPE_STATIC)&&(CurrentObject_CurrentShotType==SHOTTYPE_STATIC)) {
                CurrentObject_PreparedImage=PreviousFrame->CurrentObject_PreparedImage;                 // Use the same PreparedImage
                PreviousFrame->CurrentObject_FreePreparedImage=false;                                   // Set tag to not delete previous PreparedImage
            }
            // PreparedImage of transition Object
            if (TransitObject) {
                if ((PreviousFrame->CurrentObject_CurrentShot==TransitObject_CurrentShot)&&             // Same shot
                    (PreviousFrame->CurrentObject_CurrentShotType==SHOTTYPE_STATIC)&&(TransitObject_CurrentShotType==SHOTTYPE_STATIC)) {
                    TransitObject_PreparedImage=PreviousFrame->CurrentObject_PreparedImage;             // Use the same PreparedImage
                    PreviousFrame->CurrentObject_FreePreparedImage=false;                               // Set tag to not delete previous PreparedImage
                } else if ((PreviousFrame->TransitObject_CurrentShot==TransitObject_CurrentShot)&&      // Same shot
                    (PreviousFrame->TransitObject_CurrentShotType==SHOTTYPE_STATIC)&&(TransitObject_CurrentShotType==SHOTTYPE_STATIC)) {
                    TransitObject_PreparedImage=PreviousFrame->TransitObject_PreparedImage;             // Use the same PreparedImage
                    PreviousFrame->TransitObject_FreePreparedImage=false;                               // Set tag to not delete previous PreparedImage
                }
            }
        }
    }
}

//============================================================================================
// Destructor
//============================================================================================
cDiaporamaObjectInfo::~cDiaporamaObjectInfo() {
    // CurrentObject
    if ((CurrentObject_FreeBackgroundBrush)&&(CurrentObject_BackgroundBrush)) {
        delete CurrentObject_BackgroundBrush;
        CurrentObject_BackgroundBrush=NULL;
    }
    if ((CurrentObject_FreePreparedBackground)&&(CurrentObject_PreparedBackground)) {
        delete CurrentObject_PreparedBackground;
        CurrentObject_PreparedBackground=NULL;
    }
    if ((CurrentObject_FreeSourceImage)&&(CurrentObject_SourceImage)) {
        delete CurrentObject_SourceImage;
        CurrentObject_SourceImage=NULL;
    }
    if ((CurrentObject_FreeSoundTrackMontage)&&(CurrentObject_SoundTrackMontage)) {
        delete CurrentObject_SoundTrackMontage;
        CurrentObject_SoundTrackMontage=NULL;
    }
    if ((CurrentObject_FreePreparedImage)&&(CurrentObject_PreparedImage)) {
        delete CurrentObject_PreparedImage;
        CurrentObject_PreparedImage=NULL;
    }
    if ((CurrentObject_FreeMusicTrack)&&(CurrentObject_MusicTrack)) {
        delete CurrentObject_MusicTrack;
        CurrentObject_MusicTrack=NULL;
    }

    // Transition Object
    if ((TransitObject_FreeBackgroundBrush)&&(TransitObject_BackgroundBrush)) {
        delete TransitObject_BackgroundBrush;
        TransitObject_BackgroundBrush=NULL;
    }
    if ((TransitObject_FreePreparedBackground)&&(TransitObject_PreparedBackground)) {
        delete TransitObject_PreparedBackground;
        TransitObject_PreparedBackground=NULL;
    }
    if ((TransitObject_FreeSourceImage)&&(TransitObject_SourceImage)) {
        delete TransitObject_SourceImage;
        TransitObject_SourceImage=NULL;
    }
    if ((TransitObject_FreeSoundTrackMontage)&&(TransitObject_SoundTrackMontage)) {
        delete TransitObject_SoundTrackMontage;
        TransitObject_SoundTrackMontage=NULL;
    }
    if ((TransitObject_FreePreparedImage)&&(TransitObject_PreparedImage)) {
        delete TransitObject_PreparedImage;
        TransitObject_PreparedImage=NULL;
    }
    if ((TransitObject_FreeMusicTrack)&&(TransitObject_MusicTrack)) {
        delete TransitObject_MusicTrack;
        TransitObject_MusicTrack=NULL;
    }

    // Common
    if ((FreeRenderedImage)&&(RenderedImage)) {
        delete RenderedImage;
        RenderedImage=NULL;
    }
}
