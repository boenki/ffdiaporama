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

#include "wgt_QBackgroundWidget.h"
#include "mainwindow.h"

//*********************************************************************************************************************************************
// Base object for composition definition
//*********************************************************************************************************************************************

cCompositionObject::cCompositionObject() {
    // Attribut of the text object
    ZValue          = 500;
    Text            = "";           // Text of the object
    x               = 0.25;         // Position (x,y) and size (width,height)
    y               = 0.25;
    w               = 0.5;
    h               = 0.5;
    FontName        = "";           // font name
    FontSize        = 9;            // font size
    FontColor       = "#ffffff";    // font color
    FontShadowColor = "#000000";    // font shadow color
    IsBold          = false;        // true if bold mode
    IsItalic        = false;        // true if Italic mode
    IsUnderline     = false;        // true if Underline mode
    HAlign          = 0;            // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
    VAlign          = 0;            // Vertical alignement : 0=up, 1=center, 2=bottom
    StyleText       = 0;            // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right

    // Attribut of the form object
    BackgroundForm          = 0;            // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
    BackgroundTransparent   = 0;            // Style of the background of the form
    PenSize                 = 0;            // Width of the pen of the form
    PenColor                = "#ffffff";    // Color of the pen of the form
    // BackgroundBrush is initilise par object constructor
}

//====================================================================================================================

void cCompositionObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    // Attribut of the object
    Element.setAttribute("ZValue",ZValue);                      // Z value ordering (low is background)
    Element.setAttribute("Text",Text);                          // Text of the object
    Element.setAttribute("x",x);                                // Position x
    Element.setAttribute("y",y);                                // Position x
    Element.setAttribute("w",w);                                // size width
    Element.setAttribute("h",h);                                // size height
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
    Element.setAttribute("BackgroundForm",BackgroundForm);      // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
    Element.setAttribute("BackgroundTransparent",BackgroundTransparent);    // Style of the background of the form
    Element.setAttribute("PenSize",PenSize);                    // Width of the pen of the form
    Element.setAttribute("PenColor",PenColor);                  // Color of the pen of the form
    BackgroundBrush.SaveToXML(Element,"BackgroundBrush",PathForRelativPath);        // Brush of the background of the form

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cCompositionObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        // Attribut of the object
        ZValue          =Element.attribute("ZValue").toInt();           // Z value ordering (low is background)
        Text            =Element.attribute("Text");                     // Text of the object
        x               =Element.attribute("x").toDouble();              // Position x
        y               =Element.attribute("y").toDouble();              // Position x
        w               =Element.attribute("w").toDouble();              // size width
        h               =Element.attribute("h").toDouble();              // size height
        FontName        =Element.attribute("FontName");                 // font name
        FontSize        =Element.attribute("FontSize").toInt();         // font size
        FontColor       =Element.attribute("FontColor");                // font color
        FontShadowColor =Element.attribute("FontShadowColor");          // font shadow color
        IsBold          =Element.attribute("IsBold")=="1";              // true if bold mode
        IsItalic        =Element.attribute("IsItalic")=="1";            // true if Italic mode
        IsUnderline     =Element.attribute("IsUnderline")=="1";         // true if Underline mode
        HAlign          =Element.attribute("HAlign").toInt();           // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
        VAlign          =Element.attribute("VAlign").toInt();           // Vertical alignement : 0=up, 1=center, 2=bottom
        StyleText       =Element.attribute("StyleText").toInt();        // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right
        BackgroundForm  =Element.attribute("BackgroundForm").toInt();   // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
        BackgroundTransparent =Element.attribute("BackgroundTransparent").toInt();  // Style of the background of the form
        PenSize         =Element.attribute("PenSize").toInt();          // Width of the pen of the form
        PenColor        =Element.attribute("PenColor");                 // Color of the pen of the form
        BackgroundBrush.LoadFromXML(Element,"BackgroundBrush",PathForRelativPath);        // Brush of the background of the form
        return true;
    }
    return false;
}

//====================================================================================================================

void cCompositionObject::DrawCompositionObject(QPainter &Painter,int AddX,int AddY,int width,int height) {
    width--;
    height--;
    Painter.save();
    Painter.translate(AddX+x*double(width),AddY+y*double(height));

    // Create font and TextOption
    QTextOption OptionText;
    QFont       font=QFont(FontName,10/*int(FontSize)*/,IsBold?QFont::Bold:QFont::Normal,IsItalic?QFont::StyleItalic:QFont::StyleNormal);
    QPen        Pen;
    font.setUnderline(IsUnderline);                                                                                             // Set underline
    font.setPointSizeF((double(width)/double(SCALINGTEXTFACTOR))*double(FontSize));                  // Scale font
    //font.setPixelSize(int((double(width)/double(SCALINGTEXTFACTOR))*double(FontSize)));                  // Scale font
    OptionText=QTextOption(((HAlign==0)?Qt::AlignLeft:(HAlign==1)?Qt::AlignHCenter:(HAlign==2)?Qt::AlignRight:Qt::AlignJustify) // Setup horizontal alignement
                |(VAlign==0?Qt::AlignTop:VAlign==1?Qt::AlignVCenter:Qt::AlignBottom));                                          // Setup vertical alignement
    OptionText.setWrapMode(QTextOption::WordWrap);                                                                              // Setup word wrap text option

    // Calc margin (depending on form)
    double MarginX=double(PenSize)/2+1+1;
    double MarginY=double(PenSize)/2+1;

    if (BackgroundForm==3) {                        // 3=Buble
        MarginX=MarginX+double(width)/250;
        MarginY=MarginY+double(height)/250;
    } else if (BackgroundForm==4) {                 // 2=Ellipse
          double RX=double(w)*double(width)/2;
          double RY=double(h)*double(height)/2;
          double cos45=0.29;   // 1-cos(radians(45))
          double sin45=0.29;   // 1-sin(radians(45))
          MarginX=MarginX+(cos45*RX);
          MarginY=MarginY+(sin45*RY);
    }

    double   CenterX=w*double(width)/2;
    double   CenterY=h*double(height)/2;

    // Paint background if needed
    if (BackgroundForm!=0) {                        // other than 0=None
        // Prepare Pen & Brush
        if (PenSize==0) Pen=Qt::NoPen; else {
            Pen=QPen(QColor(PenColor));
            Pen.setWidth(PenSize);
            Pen.setJoinStyle(Qt::RoundJoin);
        }
        Painter.setPen(Pen);

        // Draw the form
        double   RayX,RayY;
        QPointF Table[10];
        double   vcos,vsin,Angle;
        int     i;

        // Set brush
        if (BackgroundTransparent<4) {
            QBrush *BR=BackgroundBrush.GetBrush(QRectF(0,0,w*width,h*height));
            Painter.setBrush(*BR);
            delete BR;
            Painter.setOpacity(BackgroundTransparent==0?1:BackgroundTransparent==1?0.75:BackgroundTransparent==2?0.50:0.25);
        } else Painter.setBrush(Qt::NoBrush);

        switch (BackgroundForm) {
            //0 = no shape
            case 1:
                Painter.drawRect(QRectF(0,0,w*width,h*height));
                break;
            case 2:
                RayX=double(width)/10;    if (RayX>16) RayX=16; else if (RayX<8)  RayX=8;
                RayY=double(height)/10;   if (RayY>16) RayY=16; else if (RayY<8)  RayY=8;
                Painter.drawRoundedRect(QRectF(0,0,w*width,h*height),RayX,RayY,Qt::AbsoluteSize);
                break;
            case 3:
                RayX=2*double(width)/10;    //if (RayX>50) RayX=16; else if (RayX<8)  RayX=8;
                RayY=2*double(height)/10;   //if (RayY>16) RayY=16; else if (RayY<8)  RayY=8;
                Painter.drawRoundedRect(QRectF(0,0,w*width,h*height),RayX,RayY,Qt::AbsoluteSize);
                break;
            case 4:
                Painter.drawEllipse(QRectF(0,0,w*width,h*height));
                break;
            case 5:     // Triangle UP
                Table[0]=QPointF((w/2)*width,0);
                Table[1]=QPointF(0,h*height);
                Table[2]=QPointF(w*width,h*height);
                Painter.drawPolygon(Table,3);
                break;
            case 6:     // Triangle Right
                Table[0]=QPointF(0,0);
                Table[1]=QPointF(0,h*height);
                Table[2]=QPointF(w*width,(h/2)*height);
                Painter.drawPolygon(Table,3);
                break;
            case 7:     // Triangle Down
                Table[0]=QPointF(0,0);
                Table[1]=QPointF(w*width,0);
                Table[2]=QPointF((w/2)*width,h*height);
                Painter.drawPolygon(Table,3);
                break;
            case 8:     // Triangle left
                Table[0]=QPointF(w*width,0);
                Table[1]=QPointF(w*width,h*height);
                Table[2]=QPointF(0,(h/2)*height);
                Painter.drawPolygon(Table,3);
                break;
            case 9:     // Losange
                Table[0]=QPointF(w*width/2,0);
                Table[1]=QPointF(w*width,h*height/2);
                Table[2]=QPointF(w*width/2,h*height);
                Table[3]=QPointF(0,h*height/2);
                Painter.drawPolygon(Table,4);
                break;
            case 10:     // Pentagone
                Angle=90-(double(360)/5);
                for (i=0;i<5;i++) {
                    vcos=cos(Angle*3.14159265/180)*(w*width/2);
                    vsin=sin(Angle*3.14159265/180)*(h*height/2);
                    Table[i]=QPointF(CenterX+vcos,CenterY-vsin);
                    Angle=Angle+(double(360)/5);
                    if (Angle>360) Angle=-Angle+360;
                }
                Painter.drawPolygon(Table,5);
                break;
            case 11:    // Hexagone
                Angle=-(double(360)/6);
                for (i=0;i<6;i++) {
                    vcos=cos(Angle*3.14159265/180)*(w*width/2);
                    vsin=sin(Angle*3.14159265/180)*(h*height/2);
                    Table[i]=QPointF(CenterX+vcos,CenterY-vsin);
                    Angle=Angle+(double(360)/6);
                    if (Angle>360) Angle=-Angle+360;
                }
                Painter.drawPolygon(Table,6);
                break;
            case 12:    // Octogone
                Angle=-(double(360)/8);
                for (i=0;i<8;i++) {
                    vcos=cos(Angle*3.14159265/180)*(w*width/2);
                    vsin=sin(Angle*3.14159265/180)*(h*height/2);
                    Table[i]=QPointF(CenterX+vcos,CenterY-vsin);
                    Angle=Angle+(double(360)/8);
                    if (Angle>360) Angle=-Angle+360;
                }
                Painter.drawPolygon(Table,8);
                break;
        }

        Painter.setOpacity(1);
        Painter.setBrush(Qt::NoBrush);
    }

    // Paint Shadow of the text
    Painter.setFont(font);
    Pen=QPen(QColor(FontShadowColor));
    Pen.setCapStyle(Qt::RoundCap);
    Pen.setJoinStyle(Qt::RoundJoin);
    Painter.setPen(Pen);

    switch (StyleText) {
        case 0 :                // 0=normal
            break;
        case 1 :                // 1=outerline
            Painter.drawText(QRectF(MarginX-1,MarginY-1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            Painter.drawText(QRectF(MarginX-1,MarginY+1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            Painter.drawText(QRectF(MarginX+1,MarginY+1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            Painter.drawText(QRectF(MarginX+1,MarginY-1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            Painter.drawText(QRectF(MarginX  ,MarginY-1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            Painter.drawText(QRectF(MarginX  ,MarginY+1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            Painter.drawText(QRectF(MarginX-1,MarginY  ,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            Painter.drawText(QRectF(MarginX+1,MarginY  ,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            break;
        case 2:                 //2=shadow up-left
            Painter.drawText(QRectF(MarginX-1,MarginY-1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            break;
        case 3:                 //3=shadow up-right
            Painter.drawText(QRectF(MarginX+1,MarginY-1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            break;
        case 4:                 //4=shadow bt-left
            Painter.drawText(QRectF(MarginX-1,MarginY+1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            break;
        case 5:                 //5=shadow bt-right
            Painter.drawText(QRectF(MarginX+1,MarginY+1,w*width-2*MarginX,h*height-2*MarginY),Text,OptionText);
            break;
    }

    // Paint text
    Painter.setPen(QColor(FontColor));
    Painter.drawText(QRectF(MarginX,MarginY,w*double(width)-2*MarginX,h*double(height)-2*MarginY),Text,OptionText);
    Painter.restore();
}

//*********************************************************************************************************************************************

cCompositionList::cCompositionList() {

}

//====================================================================================================================

cCompositionList::~cCompositionList() {

}

//====================================================================================================================

void cCompositionList::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    // Save composition list
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
        int CompositionNumber=Element.attribute("CompositionNumber").toInt();
        for (int i=0;i<CompositionNumber;i++) {
            cCompositionObject *CompositionObject=new cCompositionObject();
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
    Parent                  = DiaporamaObject;
    DefaultStaticDuration   = true;             // true if object use Diaporama duration instead of it's own duration
    DefaultMobilDuration    = true;             // true if object use Diaporama duration instead of it's own duration
    StaticDuration          = 7000;             // Duration (in msec) of the static part animation
    MobilDuration           = 3000;             // Duration (in msec) of the static part animation
    ImageRotation           = 0;                // Image rotation
    X                       = 0;                // X position (in %) relative to up/left corner
    Y                       = 0;                // Y position (in %) relative to up/left corner
    ZoomFactor              = 1;                // Zoom factor (in %)
    StartPos                = QTime(0,0,0,0);   // Start position
    EndPos                  = QTime(0,0,0,0);   // End position
}

//====================================================================================================================

cDiaporamaShot::~cDiaporamaShot() {
}


//====================================================================================================================

int cDiaporamaShot::GetStaticDuration() {
    if (DefaultStaticDuration) return Parent->List.count()>1?Parent->Parent->FixedDuration:Parent->Parent->NoShotDuration; else return StaticDuration;
}

int cDiaporamaShot::GetMobilDuration() {
    if (DefaultMobilDuration) return Parent->Parent->MobilDuration; else return MobilDuration;
}

//===============================================================

void cDiaporamaShot::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    Element.setAttribute("DefaultStaticDuration",DefaultStaticDuration?"1":"0");        // true if object use Diaporama duration instead of it's own duration
    Element.setAttribute("StaticDuration",StaticDuration);                              // Duration (in msec) of the static part animation
    Element.setAttribute("DefaultMobilDuration",DefaultMobilDuration?"1":"0");          // true if object use Diaporama duration instead of it's own duration
    Element.setAttribute("MobilDuration",MobilDuration);                                // Duration (in msec) of the static part animation
    Element.setAttribute("X",X);                                                        // X position (in %) relative to up/left corner
    Element.setAttribute("Y",Y);                                                        // Y position (in %) relative to up/left corner
    Element.setAttribute("ZoomFactor",ZoomFactor);                                      // Zoom factor (in %)
    Element.setAttribute("ImageRotation",ImageRotation);                                // Image rotation (in °)
    Element.setAttribute("StartPos",StartPos.toString());                               // Start position (video only)
    Element.setAttribute("EndPos",EndPos.toString());                                   // End position (video only)
    FilterCorrection.SaveToXML(Element,"FilterCorrection",PathForRelativPath);          // Image correction
    ShotComposition.SaveToXML(Element,"ShotComposition",PathForRelativPath);            // Composition list for this object
    domDocument.appendChild(Element);
}

//===============================================================

bool cDiaporamaShot::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        DefaultStaticDuration   =Element.attribute("DefaultStaticDuration")=="1";       // true if object use Diaporama duration instead of it's own duration
        StaticDuration          =Element.attribute("StaticDuration").toInt();           // Duration (in msec) of the static part animation
        DefaultMobilDuration    =Element.attribute("DefaultMobilDuration")=="1";        // true if object use Diaporama duration instead of it's own duration
        MobilDuration           =Element.attribute("MobilDuration").toInt();            // Duration (in msec) of the static part animation
        X                       =Element.attribute("X").toDouble();                      // X position (in %) relative to up/left corner
        Y                       =Element.attribute("Y").toDouble();                      // Y position (in %) relative to up/left corner
        ZoomFactor              =Element.attribute("ZoomFactor").toDouble();             // Zoom factor (in %)
        ImageRotation           =Element.attribute("ImageRotation").toInt();            // Image rotation (in °)
        StartPos                =QTime().fromString(Element.attribute("StartPos"));     // Start position (video only)
        EndPos                  =QTime().fromString(Element.attribute("EndPos"));       // End position (video only)
        FilterCorrection.LoadFromXML(Element,"FilterCorrection",PathForRelativPath);    // Image correction
        ShotComposition.LoadFromXML(Element,"ShotComposition",PathForRelativPath);      // Composition list for this object
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
    Parent                  = Diaporama;
    TypeObject              = DIAPORAMAOBJECTTYPE_EMPTY;

    // Set default/initial value
    BackgroundType              = false;                    // Background type : false=same as precedent - true=new background definition
    BackgroundBrush.BrushType   =BRUSHTYPE_SOLID;
    BackgroundBrush.ColorD      ="#000000";                 // Background color
    FullCanvas                  = false;                    // true if Object use full canvas (height is hypothenuse of the image rectangle and width is calc from aspect ratio)
    Image                       = NULL;                     // Embeded Object for title and image type
    Video                       = NULL;                     // Embeded Object for video type
    MusicType                   = false;                    // Music type : false=same as precedent - true=new playlist definition
    MusicPause                  = false;                    // true if music is pause during this object
    MusicReduceVolume           = false;                    // true if volume if reduce by MusicReduceFactor
    MusicReduceFactor           = 0.2;                      // factor for volume reduction if MusicReduceVolume is true
    TransitionFamilly           = TRANSITIONFAMILLY_BASE;   // Transition familly
    TransitionSubType           = 0;                        // Transition type in the familly
    TransitionDuration          = 1000;                     // Transition duration (in msec)
    SoundVolume                 = 1;                        // Volume of soundtrack (for video only)

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
}

//====================================================================================================================

QString cDiaporamaObject::GetDisplayName() {
    if (Image!=NULL) return QFileInfo(Image->FileName).baseName();
    else if (Video!=NULL) return QFileInfo(Video->FileName).baseName();
    return "";
}

//====================================================================================================================

bool cDiaporamaObject::LoadMedia(QString filename,int MediaType) {
    // Clean all
    if (Image!=NULL) {
        delete Image;
        Image=NULL;
    }
    if (Video!=NULL) {
        delete Video;
        Video=NULL;
    }

    TypeObject=MediaType;

    bool IsValide=false;
    switch (MediaType) {
        case DIAPORAMAOBJECTTYPE_EMPTY :
            IsValide=true;
            break;
        case DIAPORAMAOBJECTTYPE_IMAGE :
            Image=new cimagefilewrapper();
            IsValide=Image->GetInformationFromFile(Parent->ApplicationConfig,filename);
            break;
        case DIAPORAMAOBJECTTYPE_VIDEO :
            Video=new cvideofilewrapper();
            IsValide=Video->GetInformationFromFile(filename,false);
            List[0].StartPos             =QTime(0,0,0,0);
            List[0].EndPos               =Video->Duration;
            List[0].DefaultStaticDuration=false;
            List[0].DefaultMobilDuration =false;
            List[0].StaticDuration       =List[0].StartPos.msecsTo(List[0].EndPos);
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

    if (Video!=NULL)        ReturnImage=Video->ImageAt(true,Parent->ApplicationConfig->PreviewMaxHeight,0,true,true,NULL,1,false);  // Video
    else if (Image!=NULL)   ReturnImage=Image->ImageAt(true,Parent->ApplicationConfig->PreviewMaxHeight,true);                // Image

    if (ReturnImage!=NULL) {
        RealImageW=ReturnImage->width();
        RealImageH=ReturnImage->height();
    }
    delete ReturnImage;

    double   VirtImageW;
    double   VirtImageH;

    if (FullCanvas) CalcFullCanvas(RealImageW,RealImageH,VirtImageW,VirtImageH);
     else CalcNormalCanvas(RealImageW,RealImageH,VirtImageW,VirtImageH);

    double MagnetX1=(VirtImageW-RealImageW)/2;
    double MagnetX2=MagnetX1+RealImageW;
    double MagnetY1=(VirtImageH-RealImageH)/2;
    double MagnetY2=MagnetY1+RealImageH;

    switch (DefaultFraming) {
        case 0 :    // Adjust to Width
            List[0].X=MagnetX1/VirtImageW;
            List[0].ZoomFactor=(MagnetX2-MagnetX1)/VirtImageW;
            List[0].Y=((VirtImageH-Parent->GetHeightForWidth(MagnetX2-MagnetX1))/2)/VirtImageH;
            break;
        case 1 :    // Adjust to Height
            List[0].Y=MagnetY1/VirtImageH;
            List[0].ZoomFactor=(MagnetY2-MagnetY1)/VirtImageH;
            List[0].X=((VirtImageW-Parent->GetWidthForHeight(MagnetY2-MagnetY1))/2)/VirtImageW;
            break;
        case 2 :    // Adjust to Full
            List[0].X=MagnetX1/VirtImageW;
            List[0].ZoomFactor=(MagnetX2-MagnetX1)/VirtImageW;
            List[0].Y=((VirtImageH-Parent->GetHeightForWidth(MagnetX2-MagnetX1))/2)/VirtImageH;
            if (List[0].Y>MagnetY1/VirtImageH) {
                List[0].Y=MagnetY1/VirtImageH;
                List[0].ZoomFactor=(MagnetY2-MagnetY1)/VirtImageH;
                List[0].X=((VirtImageW-Parent->GetWidthForHeight(MagnetY2-MagnetY1))/2)/VirtImageW;
            }
            break;
    }
}

//====================================================================================================================
// Recalculate all shot size and position for full to normal canvas
//====================================================================================================================

void cDiaporamaObject::SwitchShotsToNormalCanvas() {
    QImage *ReturnImage=NULL;
    double   RealImageW;
    double   RealImageH;
    double   FullCanvasW;
    double   FullCanvasH;
    double   NormalCanvasW;
    double   NormalCanvasH;

    if (Video!=NULL)        ReturnImage=Video->ImageAt(true,Parent->ApplicationConfig->PreviewMaxHeight,0,true,true,NULL,1,false);  // Video
    else if (Image!=NULL)   ReturnImage=Image->ImageAt(true,Parent->ApplicationConfig->PreviewMaxHeight,true);                // Image

    // Calc size
    RealImageW=Parent->InternalWidth;
    RealImageH=Parent->InternalHeight;
    if (ReturnImage!=NULL) {
        RealImageW=ReturnImage->width();
        RealImageH=ReturnImage->height();
        CalcFullCanvas(RealImageW,RealImageH,FullCanvasW,FullCanvasH);
        CalcNormalCanvas(RealImageW,RealImageH,NormalCanvasW,NormalCanvasH);
    }

    // Do transformation
    for (int i=0;i<List.count();i++) {
        List[i].X=List[i].X*FullCanvasW/NormalCanvasW-((FullCanvasW-NormalCanvasW)/2)/NormalCanvasW;  if (List[i].X<0) List[i].X=0;
        List[i].Y=List[i].Y*FullCanvasH/NormalCanvasH-((FullCanvasH-NormalCanvasH)/2)/NormalCanvasH;  if (List[i].Y<0) List[i].Y=0;;
        List[i].ZoomFactor=List[i].ZoomFactor*FullCanvasW/NormalCanvasW;
    }
    delete ReturnImage;
}

//====================================================================================================================
// Recalculate all shot size and position for normal to full canvas
//====================================================================================================================

void cDiaporamaObject::SwitchShotsToFullCanvas() {
    QImage *ReturnImage=NULL;
    double   RealImageW;
    double   RealImageH;
    double   FullCanvasW;
    double   FullCanvasH;
    double   NormalCanvasW;
    double   NormalCanvasH;

    if (Video!=NULL)        ReturnImage=Video->ImageAt(true,Parent->ApplicationConfig->PreviewMaxHeight,0,true,true,NULL,1,false);  // Video
    else if (Image!=NULL)   ReturnImage=Image->ImageAt(true,Parent->ApplicationConfig->PreviewMaxHeight,true);                // Image

    // Calc size
    RealImageW=Parent->InternalWidth;
    RealImageH=Parent->InternalHeight;
    if (ReturnImage!=NULL) {
        RealImageW=ReturnImage->width();
        RealImageH=ReturnImage->height();
        CalcFullCanvas(RealImageW,RealImageH,FullCanvasW,FullCanvasH);
        CalcNormalCanvas(RealImageW,RealImageH,NormalCanvasW,NormalCanvasH);
    }

    // Do transformation
    for (int i=0;i<List.count();i++) {
        List[i].ZoomFactor=List[i].ZoomFactor*NormalCanvasW/FullCanvasW;
        List[i].X=List[i].X*NormalCanvasW/FullCanvasW+((FullCanvasW-NormalCanvasW)/2)/FullCanvasW;  if (List[i].X+List[i].ZoomFactor>1) List[i].X=1-List[i].ZoomFactor;
        List[i].Y=List[i].Y*NormalCanvasH/FullCanvasH+((FullCanvasH-NormalCanvasH)/2)/FullCanvasH;  if (List[i].Y+List[i].ZoomFactor>1) List[i].Y=1-List[i].ZoomFactor;
    }
    delete ReturnImage;
}

//====================================================================================================================
// Load image from source object at given position(video) and Apply filter if ApplyFilter==true
// return new image (in all case)
// Call by :
//      DlgImageProperties for s_CompositionNeedRefreshBackgroundImage()
//      DlgVideoProperties for s_CompositionNeedRefreshBackgroundImage()
//  and by CanvasImageAt
//====================================================================================================================

QImage  *cDiaporamaObject::GetImageAt(int Position,bool VideoCachedMode,cSoundBlockList *SoundTrackMontage) {
    QImage  *ReturnImage=NULL;

    // Try to load image from source
    if (Video!=NULL) ReturnImage=Video->ImageAt(true,Parent->ApplicationConfig->PreviewMaxHeight,Position,VideoCachedMode,false,SoundTrackMontage,1,false);   // Video
    else if (Image!=NULL) ReturnImage=Image->ImageAt(true,Parent->ApplicationConfig->PreviewMaxHeight,false);                                           // Image
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
                                        bool VideoCachedMode,bool ApplyShotText,bool ApplyShotFilter,cSoundBlockList *SoundTrackMontage) {

    // Call PrepareImage on a painter to put image on a canvas
    QImage *SourceImage = GetImageAt(Position+QTime(0,0,0,0).msecsTo(List[0].StartPos),VideoCachedMode,SoundTrackMontage);
    QImage *ReturnImage = NULL;

    if ((SourceImage==NULL)||(SourceImage->isNull())) return NULL;

    if (Painter!=NULL) {
        PrepareImage(Painter,Width,Height,Position,SourceImage,AddX,AddY,ImagePosition,ForcedImageRotation,ApplyShotText,ApplyShotFilter);
    } else {
        // Call only by QCustomScene (Painter is NULL)
        // Prepare a background with transparent brush (emulating transparency)
        ReturnImage = new QImage(Width,Height,QImage::Format_ARGB32_Premultiplied);
        QPainter    P;
        P.begin(ReturnImage);
        P.fillRect(0,0,Width,Height,Parent->Transparent);
        PrepareImage(&P,Width,Height,Position,SourceImage,AddX,AddY,ImagePosition,ForcedImageRotation,ApplyShotText,ApplyShotFilter);
        P.end();
    }

    // No future need of source image
    delete SourceImage;

    // return new image
    return ReturnImage;
}


// PrepareImage subfunction (Call only by CanvasImageAt)
void cDiaporamaObject::PrepareImage(QPainter *P,int Width,int Height,int Position,QImage *LastLoadedImage,int AddX,int AddY,QRectF *ImagePosition,int *ForcedImageRotation,
                                    bool ApplyShotText,bool ApplyShotFilter) {
    double                   XFactor     =0;
    double                   YFactor     =0;
    double                   ZoomFactor  =1;
    double                   RotateFactor=0;
    cFilterCorrectObject    FilterCorrection;

    // Calc current sequence depending on Position
    int Sequence=0;
    int CurPos  =0;
    while ((Sequence<List.count()-1)&&((CurPos+(Sequence>0?List[Sequence].GetMobilDuration():0)+List[Sequence].GetStaticDuration())<=Position)) {
        CurPos=CurPos+(Sequence>0?List[Sequence].GetMobilDuration():0)+List[Sequence].GetStaticDuration();
        Sequence++;
    }

    // Calc image modification factor depending on position
    if (ForcedImageRotation!=NULL) RotateFactor=double(*ForcedImageRotation); else RotateFactor=List[Sequence].ImageRotation;

    if (ImagePosition==NULL) {
        XFactor         =List[Sequence].X;
        YFactor         =List[Sequence].Y;
        ZoomFactor      =List[Sequence].ZoomFactor;
        FilterCorrection=List[Sequence].FilterCorrection;
    }

    if ((Sequence>0)&&((Position-CurPos)<(Sequence>0?List[Sequence].GetMobilDuration():0))) {
        double   PctDone=(double(Position)-double(CurPos))/(double(List[Sequence].GetMobilDuration()));
        CalcTransformations(Sequence,PctDone,XFactor,YFactor,ZoomFactor,RotateFactor,FilterCorrection);
    }

    // Calc real image size
    double   RealImageW=double(LastLoadedImage->width());
    double   RealImageH=double(LastLoadedImage->height());
    double   VirtImageW;
    double   VirtImageH;

    // Calc canvas
    if (FullCanvas) CalcFullCanvas(RealImageW,RealImageH,VirtImageW,VirtImageH);
     else CalcNormalCanvas(RealImageW,RealImageH,VirtImageW,VirtImageH);

    // Global image Composition
    QImage      GlobalImageComposition(RealImageW,RealImageH,QImage::Format_ARGB32_Premultiplied);
    QPainter    PB;
    PB.begin(&GlobalImageComposition);
    PB.setCompositionMode(QPainter::CompositionMode_Source);
    PB.fillRect(QRect(0,0,GlobalImageComposition.width(),GlobalImageComposition.height()),Qt::transparent);
    PB.setCompositionMode(QPainter::CompositionMode_SourceOver);
    for (int j=0;j<ObjectComposition.List.count();j++) ObjectComposition.List[j].DrawCompositionObject(PB,0,0,RealImageW,RealImageH);
    PB.end();

    QImage  *SourceImage=NULL;

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
    if ((TypeObject!=DIAPORAMAOBJECTTYPE_EMPTY)&&(ApplyShotFilter)) FilterCorrection.ApplyFilter(&Image2);

    P->drawImage(QRectF(DestX+AddX,DestY+AddY,DestW,DestH),Image2);

    //**********************************
    // Composition layers
    //**********************************
    // Add global image composition layer
    P->drawImage(QRectF(DestX+AddX,DestY+AddY,DestW,DestH),GlobalImageComposition,QRectF(SrcX,SrcY,SrcW,SrcH));

    // Add static shot composition
    if (ApplyShotText) for (int j=0;j<List[Sequence].ShotComposition.List.count();j++) List[Sequence].ShotComposition.List[j].DrawCompositionObject(*P,AddX,AddY,Width,Height);

    if (ImagePosition!=NULL) *ImagePosition=QRectF(DestX,DestY,DestW,DestH);

    if (SourceImage!=LastLoadedImage) delete SourceImage;
}

//===============================================================

void cDiaporamaObject::CalcTransformations(int Sequence,double PctDone,double &XFactor,double &YFactor,double &ZoomFactor,double &RotateFactor,cFilterCorrectObject &FilterCorrection) {
    cFilterCorrectObject AncFilterCorrection =List[Sequence-1].FilterCorrection;;

    // Adjust XFactor, YFactor and ZoomFactor depending on position in Mobil part animation
    double AncXFactor     =List[Sequence-1].X;
    double AncYFactor     =List[Sequence-1].Y;
    double AncZoomFactor  =List[Sequence-1].ZoomFactor;
    double AncRotateFactor=List[Sequence-1].ImageRotation;

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

void cDiaporamaObject::CalcFullCanvas(double RealImageW,double RealImageH,double &VirtImageW,double &VirtImageH) {
    // Calc hypothenuse
    double   Hyp=sqrt(RealImageW*RealImageW+RealImageH*RealImageH);
    // Calc canvas size
    VirtImageW=Hyp;
    VirtImageH=Parent->GetHeightForWidth(VirtImageW);
    // Ensure complete image
    if (VirtImageH<Hyp) {
        VirtImageH=Hyp;
        VirtImageW=Parent->GetWidthForHeight(VirtImageH);
    }
}

//===============================================================

void cDiaporamaObject::CalcNormalCanvas(double RealImageW,double RealImageH,double &VirtImageW,double &VirtImageH) {
    // Calc canvas size
    VirtImageW=RealImageW;
    VirtImageH=Parent->GetHeightForWidth(VirtImageW);
    // Ensure complete image
    if (VirtImageH<RealImageH) {
        VirtImageH=RealImageH;
        VirtImageW=Parent->GetWidthForHeight(VirtImageH);
    }
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

    if (PathForRelativPath!="") FileName=QDir(PathForRelativPath).relativeFilePath(FileName);

    Element.setAttribute("ObjectFileName",  FileName);
    Element.setAttribute("TypeObject",      TypeObject);
    Element.setAttribute("SoundVolume",     QString("%1").arg(SoundVolume,0,'f'));          // Volume of soundtrack (for video only)

    QDomElement SubElement=DomDocument.createElement("Background");
    SubElement.setAttribute("BackgroundType",BackgroundType?"1":"0");                       // Background type : false=same as precedent - true=new background definition
    SubElement.setAttribute("FullCanvas",FullCanvas?"1":"0");                               // true if Object use full canvas (height is hypothenuse of the image rectangle and width is calc from aspect ratio)
    BackgroundBrush.SaveToXML(SubElement,"BackgroundBrush",PathForRelativPath);             // Background brush
    BackgroundComposition.SaveToXML(SubElement,"BackgroundComposition",PathForRelativPath); // Background composition
    Element.appendChild(SubElement);

    SubElement=DomDocument.createElement("Transition");
    SubElement.setAttribute("TransitionFamilly",TransitionFamilly);                         // Transition familly
    SubElement.setAttribute("TransitionSubType",TransitionSubType);                         // Transition type in the familly
    SubElement.setAttribute("TransitionDuration",TransitionDuration);                       // Transition duration (in msec)
    Element.appendChild(SubElement);

    FilterTransform.SaveToXML(Element,"GlobalImageFilters",PathForRelativPath);             // Global Image filters
    ObjectComposition.SaveToXML(Element,"GlobalImageComposition",PathForRelativPath);       // Global Image composition

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

        QString FileName=Element.attribute("ObjectFileName","");
        if (PathForRelativPath!="") FileName=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(FileName));

        int TheTypeObject=Element.attribute("TypeObject").toInt();
        if (LoadMedia(FileName,TheTypeObject)) {
            bool IsOk=true;

            SoundVolume=Element.attribute("SoundVolume").toDouble();    // Volume of soundtrack (for video only)

            if ((Element.elementsByTagName("Background").length()>0)&&(Element.elementsByTagName("Background").item(0).isElement()==true)) {
                QDomElement SubElement=Element.elementsByTagName("Background").item(0).toElement();
                BackgroundType  =SubElement.attribute("BackgroundType")=="1";                                                           // Background type : false=same as precedent - true=new background definition
                FullCanvas      =SubElement.attribute("FullCanvas")=="1";                                                               // true if Object use full canvas (height is hypothenuse of the image rectangle and width is calc from aspect ratio)
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
            ObjectComposition.LoadFromXML(Element,"GlobalImageComposition",PathForRelativPath);                                     // Global Image composition

            // Load shot list
            List.clear();
            int ShotNumber=Element.attribute("ShotNumber").toInt();
            for (int i=0;i<ShotNumber;i++) {
                cDiaporamaShot *imagesequence=new cDiaporamaShot(this);
                if (!imagesequence->LoadFromXML(Element,"Shot-"+QString("%1").arg(i),PathForRelativPath)) IsOk=false;
                List.append(*imagesequence);
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
        QBrush *BR=List[Index].BackgroundBrush.GetBrush(QRectF(0,0,Width,Height));
        Painter->fillRect(QRect(0,0,Width,Height),*BR);
        delete BR;
    }
    //
    if (ApplyComposition) for (int j=0;j<List[Index].BackgroundComposition.List.count();j++) List[Index].BackgroundComposition.List[j].DrawCompositionObject(*Painter,0,0,Width,Height);
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

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
// Note : Position is relative to the start of the Column object !
//============================================================================================
void cDiaporama::PrepareMusicBloc(int Column,int Position,cSoundBlockList *MusicTrack) {
    int     StartPosition;

    if ((Column<List.count())&&(!List[Column].MusicPause)) {
        cMusicObject *CurMusic=GetMusicObject(Column,StartPosition);                                         // Get current music file from column and position
        if (CurMusic!=NULL) {
            int OldBlock=MusicTrack->List.count();
            CurMusic->Music->ImageAt(false,0,Position+StartPosition,false,false,MusicTrack,1,false);               // Get music bloc at correct position
            double Factor=CurMusic->Volume;
            if (List[Column].MusicReduceVolume) Factor=Factor*List[Column].MusicReduceFactor;
            if (Factor!=1.0) for (int i=OldBlock;i<MusicTrack->List.count();i++) MusicTrack->ApplyVolume(i,Factor);
        } else for (int j=0;j<MusicTrack->NbrPacketForFPS;j++) MusicTrack->AppendNullSoundPacket();
    } else for (int j=0;j<MusicTrack->NbrPacketForFPS;j++) MusicTrack->AppendNullSoundPacket();
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
//  Extend=amout of padding (top and bottom) for cinema mode with DVD
//  IsCurrentObject : If true : prepare CurrentObject - If false : prepare Transition Object
//============================================================================================
void cDiaporama::PrepareImage(cDiaporamaObjectInfo *Info,int W,int H,int Extend,bool IsCurrentObject) {
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

    double                  DestX,DestY,DestW,DestH;                        // Coordonates of destination image in the virtual image
    double                  SrcX,SrcY,SrcW,SrcH;                            // Coordonates of the destination image in the source image
    double                  RealImageW,RealImageH,VirtImageW,VirtImageH;    // Destination image size
    double                  XFactor,YFactor,ZoomFactor,RotateFactor;        // Transformation value
    cFilterCorrectObject    FilterCorrection;                               // Filter correction value
    QImage                  *ImageToUse=NULL;
    QImage                  *Image=NULL;
    QImage                  Image1,Image2;                                  // Temporary object use to create frame (Image2 is the rendered image for montage)

    // Prepare image
    if (!SoundOnly) {
        Image=new QImage(W,H+Extend+Extend,QImage::Format_ARGB32_Premultiplied);
        QPainter P;
        P.begin(Image);
        P.setCompositionMode(QPainter::CompositionMode_Source);
        P.fillRect(0,0,W,H+Extend+Extend,Qt::transparent);
        P.setCompositionMode(QPainter::CompositionMode_SourceOver);
        if ((List.count()>0)&&(SourceImage!=NULL)) {

            // Calc real image size
            RealImageW=double(SourceImage->width());
            RealImageH=double(SourceImage->height());
            VirtImageW=0;
            VirtImageH=0;

            // Calc canvas
            if (CurObject->FullCanvas) CurObject->CalcFullCanvas(RealImageW,RealImageH,VirtImageW,VirtImageH);
             else CurObject->CalcNormalCanvas(RealImageW,RealImageH,VirtImageW,VirtImageH);

            // Global image Composition
            QImage      GlobalImageComposition(RealImageW,RealImageH,QImage::Format_ARGB32_Premultiplied);
            QPainter    PB;
            PB.begin(&GlobalImageComposition);
            PB.setCompositionMode(QPainter::CompositionMode_Source);
            PB.fillRect(QRect(0,0,GlobalImageComposition.width(),GlobalImageComposition.height()),Qt::transparent);
            PB.setCompositionMode(QPainter::CompositionMode_SourceOver);
            for (int j=0;j<CurObject->ObjectComposition.List.count();j++) CurObject->ObjectComposition.List[j].DrawCompositionObject(PB,0,0,RealImageW,RealImageH);
            PB.end();

            XFactor         =CurShot->X;
            YFactor         =CurShot->Y;
            ZoomFactor      =CurShot->ZoomFactor;
            RotateFactor    =CurShot->ImageRotation;
            FilterCorrection=CurShot->FilterCorrection;

            if (ShotType==SHOTTYPE_MOBIL) CurObject->CalcTransformations(ShotSeqNum,MobilPCTDone,XFactor,YFactor,ZoomFactor,RotateFactor,FilterCorrection);

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

            P.drawImage(QRectF(DestX,DestY+Extend,DestW,DestH),Image2);

            //**********************************
            // Composition layers
            //**********************************
            // Add global image composition layer
            P.drawImage(QRectF(DestX,DestY+Extend,DestW,DestH),GlobalImageComposition,QRectF(SrcX,SrcY,SrcW,SrcH));
            // Add static shot composition
            for (int j=0;j<CurShot->ShotComposition.List.count();j++) CurShot->ShotComposition.List[j].DrawCompositionObject(P,0,Extend,W,H);
        }

        P.end();
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
        if (List.count()>0) {
            // Draw background
            if ((Info->IsTransition)&&((Info->CurrentObject_Number==0)||(Info->CurrentObject_BackgroundIndex!=Info->TransitObject_BackgroundIndex))) {
                if ((Info->TransitObject)&&(Info->TransitObject_PreparedBackground)) {
                    //P.setOpacity(1-Info->TransitionPCTDone);
                    P.drawImage(0,0,*Info->TransitObject_PreparedBackground);
                } else P.fillRect(QRect(0,0,W,H),Qt::black);
                if (Info->CurrentObject_PreparedBackground) {
                    P.setOpacity(Info->TransitionPCTDone);
                    P.drawImage(0,0,*Info->CurrentObject_PreparedBackground);
                }
                P.setOpacity(1);
            } else {
                if (Info->CurrentObject_PreparedBackground) P.drawImage(0,0,*Info->CurrentObject_PreparedBackground);
                    else P.fillRect(QRect(0,0,W,H),Qt::black);
            }
            // Add prepared images
            if ((Info->IsTransition)&&(Info->TransitObject_PreparedImage!=NULL)&&(Info->CurrentObject_PreparedImage!=NULL)) {
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
        }
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
    // Get a copy of luma image scaled to correct size
    QImage  Luma=((W==LUMADLG_WIDTH)&&(H==LUMADLG_HEIGHT))?LumaList->List[Info->TransitionSubType].DlgLumaImage:
                    LumaList->List[Info->TransitionSubType].OriginalLuma.scaled(Info->CurrentObject_PreparedImage->size(),Qt::IgnoreAspectRatio/*,Qt::SmoothTransformation*/).convertToFormat(QImage::Format_ARGB32_Premultiplied);
    QImage  Img=Info->CurrentObject_PreparedImage->copy();

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
        }
    } else {
        //==============> Image part
        // Title or image mode : Get source image only if SourceImageColumn!=Column
        if (Info->CurrentObject_SourceImage==NULL) {
            if ((Info->CurrentObject->Video==NULL)) {
                // Get source image (without any preparation)
                if (Info->CurrentObject->Image!=NULL) {                                 // Image
                    if (!SoundOnly) ThreadLoadSourceImage=QtConcurrent::run(this,&cDiaporama::ThreadLoadSourcePhotoImage,Info,PreviewMode);
                } else {                                                                // Title
                    if (!SoundOnly) {
                        // create an empty transparent image
                        Info->CurrentObject_SourceImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
                        QPainter PT;
                        PT.begin(Info->CurrentObject_SourceImage);
                        PT.setCompositionMode(QPainter::CompositionMode_Source);
                        PT.fillRect(QRect(0,0,W,H),Qt::transparent);
                        PT.setCompositionMode(QPainter::CompositionMode_SourceOver);
                        PT.end();
                    }
                }
            // Video mode : Get source image (without any preparation)
            } else ThreadLoadSourceImage=QtConcurrent::run(this,&cDiaporama::ThreadLoadSourceVideoImage,Info,PreviewMode,SoundOnly);
        }
        // same job for Transition Object if a previous was not keep !
        if ((Info->TransitObject)&&(Info->TransitObject_SourceImage==NULL)) {
            if ((Info->TransitObject->Video==NULL)) {
                // Get source image (without any preparation)
                if (Info->TransitObject->Image!=NULL) {                                // Image
                    ThreadLoadTransitImage=QtConcurrent::run(this,&cDiaporama::ThreadLoadTransitPhotoImage,Info,PreviewMode);
                } else {                                                               // Title
                    if (!SoundOnly) {
                        // create an empty transparent image
                        Info->TransitObject_SourceImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
                        QPainter PT;
                        PT.begin(Info->TransitObject_SourceImage);
                        PT.setCompositionMode(QPainter::CompositionMode_Source);
                        PT.fillRect(QRect(0,0,W,H),Qt::transparent);
                        PT.setCompositionMode(QPainter::CompositionMode_SourceOver);
                        PT.end();
                    }
                }
            // Video mode : Get source image (without any preparation)
            } else ThreadLoadTransitImage=QtConcurrent::run(this,&cDiaporama::ThreadLoadTransitVideoImage,Info,PreviewMode,SoundOnly);
        }
        //==============> Background part
        // Search background context for CurrentObject if a previous was not keep !
        if (Info->CurrentObject_BackgroundBrush==NULL) {
            if ((Info->CurrentObject_BackgroundIndex>=List.count())||(List[Info->CurrentObject_BackgroundIndex].BackgroundType==false))
                Info->CurrentObject_BackgroundBrush=new QBrush(Qt::black);   // If no background definition @ first object
                else Info->CurrentObject_BackgroundBrush=List[Info->CurrentObject_BackgroundIndex].BackgroundBrush.GetBrush(QRectF(0,0,W,H));
            // Create PreparedBackground
            if (!SoundOnly) {
                Info->CurrentObject_PreparedBackground=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
                QPainter P;
                P.begin(Info->CurrentObject_PreparedBackground);
                if (Info->CurrentObject_BackgroundBrush) P.fillRect(QRect(0,0,W,H),*Info->CurrentObject_BackgroundBrush); else P.fillRect(0,0,W,H,Qt::black);
                // Apply composition to background
                for (int j=0;j<List[Info->CurrentObject_BackgroundIndex].BackgroundComposition.List.count();j++) List[Info->CurrentObject_BackgroundIndex].BackgroundComposition.List[j].DrawCompositionObject(P,0,0,W,H);
                P.end();
            }
        }
        // same job for Transition Object if a previous was not keep !
        if ((Info->TransitObject)&&(Info->TransitObject_BackgroundBrush==NULL)) {
            if ((Info->TransitObject_BackgroundIndex>=List.count())||(List[Info->TransitObject_BackgroundIndex].BackgroundType==false))
                Info->TransitObject_BackgroundBrush=new QBrush(Qt::black);   // If no background definition @ first object
                else Info->TransitObject_BackgroundBrush=List[Info->TransitObject_BackgroundIndex].BackgroundBrush.GetBrush(QRectF(0,0,W,H));
            // Create PreparedBackground
            if (!SoundOnly) {
                Info->TransitObject_PreparedBackground=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
                QPainter P;
                P.begin(Info->TransitObject_PreparedBackground);
                if (Info->TransitObject_BackgroundBrush) P.fillRect(QRect(0,0,W,H),*Info->TransitObject_BackgroundBrush); else P.fillRect(0,0,W,H,Qt::black);
                // Apply composition to background
                for (int j=0;j<List[Info->TransitObject_BackgroundIndex].BackgroundComposition.List.count();j++) List[Info->TransitObject_BackgroundIndex].BackgroundComposition.List[j].DrawCompositionObject(P,0,0,W,H);
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

void cDiaporama::ThreadLoadSourceVideoImage(cDiaporamaObjectInfo *Info,bool PreviewMode,bool SoundOnly) {
    Info->CurrentObject_SourceImage=Info->CurrentObject->Video->ImageAt(PreviewMode,ApplicationConfig->PreviewMaxHeight,
        Info->CurrentObject_InObjectTime+QTime(0,0,0,0).msecsTo(Info->CurrentObject->List[0].StartPos),
        false,false,Info->CurrentObject_SoundTrackMontage,Info->CurrentObject->SoundVolume,SoundOnly);
}

void cDiaporama::ThreadLoadSourcePhotoImage(cDiaporamaObjectInfo *Info,bool PreviewMode) {
    Info->CurrentObject_SourceImage=Info->CurrentObject->Image->ImageAt(PreviewMode,ApplicationConfig->PreviewMaxHeight,false);
}

void cDiaporama::ThreadLoadTransitVideoImage(cDiaporamaObjectInfo *Info,bool PreviewMode,bool SoundOnly) {
    Info->TransitObject_SourceImage=Info->TransitObject->Video->ImageAt( // Video
        PreviewMode,ApplicationConfig->PreviewMaxHeight,Info->TransitObject_InObjectTime+QTime(0,0,0,0).msecsTo(Info->TransitObject->List[0].StartPos),
        false,false,Info->TransitObject_SoundTrackMontage,Info->TransitObject->SoundVolume,SoundOnly);
}

void cDiaporama::ThreadLoadTransitPhotoImage(cDiaporamaObjectInfo *Info,bool PreviewMode) {
    Info->TransitObject_SourceImage=Info->TransitObject->Image->ImageAt(PreviewMode,ApplicationConfig->PreviewMaxHeight,false);
}

//*********************************************************************************************************************************************
// Class object for rendering
//*********************************************************************************************************************************************

// make a copy of PreviousFrame
cDiaporamaObjectInfo::cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame) {
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

    RenderedImage                       =NULL;                                              // Final image rendered
    FreeRenderedImage                   =true;                                              // True if allow to delete RenderedImage during destructor
    TransitionFamilly                   =PreviousFrame->TransitionFamilly;                  // Transition familly
    TransitionSubType                   =PreviousFrame->TransitionSubType;                  // Transition type in the familly
    TransitionDuration                  =PreviousFrame->TransitionDuration;                 // Transition duration (in msec)
}


cDiaporamaObjectInfo::cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame,int TimePosition,cDiaporama *Diaporama,double FrameDuration) {
    //==============> Pre-initialise all values

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

    RenderedImage                       =NULL;              // Final image rendered
    FreeRenderedImage                   =true;              // True if allow to delete RenderedImage during destructor

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
                if (CurrentObject->Video==NULL) {
                    CurrentObject_CurrentShotType=SHOTTYPE_STATIC;
                    CurrentObject_EndStaticShot  =CurrentObject_InObjectTime+CurrentObject_CurrentShot->GetStaticDuration();
                } else CurrentObject_CurrentShotType=SHOTTYPE_VIDEO;
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
            // If CurrentObject if not the first object
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
                    if (TransitObject->Video==NULL) {
                        TransitObject_CurrentShotType=SHOTTYPE_STATIC;
                        TransitObject_EndStaticShot  =TransitObject_InObjectTime+TransitObject_CurrentShot->GetStaticDuration();
                    } else TransitObject_CurrentShotType=SHOTTYPE_VIDEO;
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
