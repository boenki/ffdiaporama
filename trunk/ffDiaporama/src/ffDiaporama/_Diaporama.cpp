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

#include "../engine/_QCustomDialog.h"
// Specific inclusions
#include "_Diaporama.h"
#include "_ApplicationDefinitions.h"
#include "mainwindow.h"

#include <QFileDialog>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>

// Composition parameters
#define SCALINGTEXTFACTOR                   700     // 700 instead of 400 (ffD 1.0/1.1/1.2) to keep similar display from plaintext to richtext

//*********************************************************************************************************************************************
// Base object for composition definition
//*********************************************************************************************************************************************

cCompositionObject::cCompositionObject(int TheTypeComposition,int TheIndexKey,cBaseApplicationConfig *TheApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject::cCompositionObject");

    // Attribut of the text object
    ApplicationConfig       = TheApplicationConfig;
    TypeComposition         = TheTypeComposition;
    IndexKey                = TheIndexKey;
    IsVisible               = true;
    SameAsPrevShot          = false;
    BackgroundBrush         = new cBrushDefinition(ApplicationConfig,&BackgroundList);  // ERROR : BackgroundList is global !

    x                       = 0.25;         // Position (x,y) and size (width,height)
    y                       = 0.25;
    w                       = 0.5;
    h                       = 0.5;

    RotateZAxis             = 0;                    // Rotation from Z axis
    RotateXAxis             = 0;                    // Rotation from X axis
    RotateYAxis             = 0;                    // Rotation from Y axis

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
    TxtZoomLevel            = 100;                                              // Zoom Level for text
    TxtScrollX              = 0;                                                // Scrolling X for text
    TxtScrollY              = 0;                                                // Scrolling Y for text
    TMType                  = TEXTMARGINS_SHAPEDEFAULT;

    // Shap part
    BackgroundForm          = 1;                                                // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
    Opacity                 = DEFAULT_SHAPE_OPACITY;                            // Style of the background of the form
    PenSize                 = DEFAULT_SHAPE_BORDERSIZE;                         // Width of the border of the form
    PenStyle                = Qt::SolidLine;                                    // Style of the pen border of the form
    PenColor                = DEFAULT_SHAPE_BORDERCOLOR;                        // Color of the border of the form
    FormShadowColor         = DEFAULT_SHAPE_SHADOWCOLOR;                        // Color of the shadow of the form
    FormShadow              = 0;                                                // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    FormShadowDistance      = 5;                                                // Distance from form to shadow

    // Block animation part
    BlockAnimType           = BLOCKANIMTYPE_NONE;
    TurnZAxis               = 0;                                                // Number of turn from Z axis
    TurnXAxis               = 0;                                                // Number of turn from X axis
    TurnYAxis               = 0;                                                // Number of turn from Y axis
    Dissolve                = BLOCKANIMVALUE_APPEAR;

    // BackgroundBrush is initilise by object constructor except TypeComposition and key
    BackgroundBrush->TypeComposition = TypeComposition;

    ApplyTextMargin(TEXTMARGINS_SHAPEDEFAULT);                                  // Init TMx,TMy,TMw,TMh (Text margins)
}

//====================================================================================================================

cCompositionObject::~cCompositionObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject::~cCompositionObject");

    if (BackgroundBrush) {
        delete BackgroundBrush;
        BackgroundBrush=NULL;
    }
}

//====================================================================================================================

void cCompositionObject::ApplyTextMargin(int TMType) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject::ApplyTextMargin");
    if ((this->TMType==TEXTMARGINS_CUSTOM)&&(TMType==TEXTMARGINS_CUSTOM)) return;   // Don't overwrite custom settings
    this->TMType=TMType;
    QRectF Rect=GetPrivateTextMargin();
    this->TMx   =Rect.left();
    this->TMy   =Rect.top();
    this->TMw   =Rect.width();
    this->TMh   =Rect.height();
}

//====================================================================================================================

QRectF cCompositionObject::GetPrivateTextMargin() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject::GetPrivateTextMargin");

    if (TMType==TEXTMARGINS_FULLSHAPE)              return PolygonToRectF(ComputePolygon(BackgroundForm,0,0,1,1,0,0));
        else if (TMType==TEXTMARGINS_SHAPEDEFAULT)  return QRectF(ShapeFormDefinition[BackgroundForm].TMx,ShapeFormDefinition[BackgroundForm].TMy,
                                                                  ShapeFormDefinition[BackgroundForm].TMw,ShapeFormDefinition[BackgroundForm].TMh);
    return QRectF(TMx,TMy,TMw,TMh);
}

//====================================================================================================================

QRectF cCompositionObject::GetTextMargin(QRectF Workspace,double  ADJUST_RATIO) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject::GetTextMargin");
    // if type is ShapeDefault, then adjust with border size
    if ((TMType==TEXTMARGINS_FULLSHAPE)||(TMType==TEXTMARGINS_CUSTOM)) {
        return QRectF(TMx*w*Workspace.width(),TMy*h*Workspace.height(),
                      TMw*w*Workspace.width(),TMh*h*Workspace.height());
    } else {
        double FullMargin=double(PenSize)*ADJUST_RATIO/double(2);
        return QRectF(ShapeFormDefinition[BackgroundForm].TMx*w*Workspace.width()+FullMargin,ShapeFormDefinition[BackgroundForm].TMy*h*Workspace.height()+FullMargin,
                      ShapeFormDefinition[BackgroundForm].TMw*w*Workspace.width()-FullMargin*2,ShapeFormDefinition[BackgroundForm].TMh*h*Workspace.height()-FullMargin*2);
    }
}

//====================================================================================================================

void cCompositionObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath,bool CheckTypeComposition) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    Element.setAttribute("TypeComposition",TypeComposition);
    Element.setAttribute("IndexKey",IndexKey);
    Element.setAttribute("IsVisible",IsVisible?"1":"0");
    Element.setAttribute("SameAsPrevShot",SameAsPrevShot?"1":"0");

    // Attribut of the object
    Element.setAttribute("x",x);                                    // Position x
    Element.setAttribute("y",y);                                    // Position x
    Element.setAttribute("w",w);                                    // size width
    Element.setAttribute("h",h);                                    // size height
    Element.setAttribute("RotateZAxis",RotateZAxis);                // Rotation from Z axis
    Element.setAttribute("RotateXAxis",RotateXAxis);                // Rotation from X axis
    Element.setAttribute("RotateYAxis",RotateYAxis);                // Rotation from Y axis
    Element.setAttribute("BackgroundTransparent",Opacity);          // Opacity of the form

    // Block animation
    Element.setAttribute("BlockAnimType",BlockAnimType);            // Block animation type
    Element.setAttribute("TurnZAxis",TurnZAxis);                    // Number of turn from Z axis
    Element.setAttribute("TurnXAxis",TurnXAxis);                    // Number of turn from X axis
    Element.setAttribute("TurnYAxis",TurnYAxis);                    // Number of turn from Y axis
    Element.setAttribute("Dissolve",Dissolve);                      // Dissolve value

    // Text part
    if ((!CheckTypeComposition)||(TypeComposition!=COMPOSITIONTYPE_SHOT)) {
        Element.setAttribute("Text",Text); // Text of the object
        if (Text!="") {
            Element.setAttribute("FontName",FontName);                      // font name
            Element.setAttribute("FontSize",FontSize);                      // font size
            Element.setAttribute("FontColor",FontColor);                    // font color
            Element.setAttribute("FontShadowColor",FontShadowColor);        // font shadow color
            Element.setAttribute("IsBold",IsBold?"1":"0");                  // true if bold mode
            Element.setAttribute("IsItalic",IsItalic?"1":"0");              // true if Italic mode
            Element.setAttribute("IsUnderline",IsUnderline?"1":"0");        // true if Underline mode
            Element.setAttribute("HAlign",HAlign);                          // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
            Element.setAttribute("VAlign",VAlign);                          // Vertical alignement : 0=up, 1=center, 2=bottom
            Element.setAttribute("StyleText",StyleText);                    // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right
        }
    }

    // Shot part of text part
    Element.setAttribute("TxtZoomLevel",TxtZoomLevel);                      // Zoom Level for text
    Element.setAttribute("TxtScrollX",TxtScrollX);                          // Scrolling X for text
    Element.setAttribute("TxtScrollY",TxtScrollY);                          // Scrolling Y for text
    Element.setAttribute("TMType",TMType);                                  // Text margins type
    Element.setAttribute("TMx",TMx);                                        // Text margins
    Element.setAttribute("TMy",TMy);                                        // Text margins
    Element.setAttribute("TMw",TMw);                                        // Text margins
    Element.setAttribute("TMh",TMh);                                        // Text margins

    // Shap part
    Element.setAttribute("BackgroundForm",BackgroundForm);                  // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
    Element.setAttribute("PenSize",PenSize);                                // Width of the border of the form
    Element.setAttribute("PenStyle",PenStyle);                              // Style of the pen border of the form
    Element.setAttribute("PenColor",PenColor);                              // Color of the border of the form
    Element.setAttribute("FormShadow",FormShadow);                          // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
    Element.setAttribute("FormShadowDistance",FormShadowDistance);          // Distance from form to shadow
    Element.setAttribute("FormShadowColor",FormShadowColor);                // Shadow color
    BackgroundBrush->SaveToXML(Element,"BackgroundBrush",PathForRelativPath,ForceAbsolutPath);    // Brush of the background of the form

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cCompositionObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,cCompositionList *ObjectComposition,QStringList *AliasList,bool CheckTypeComposition) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        bool    IsOk=true;

        if (Element.hasAttribute("TypeComposition"))            TypeComposition =Element.attribute("TypeComposition").toInt();
        if (Element.hasAttribute("IndexKey"))                   IndexKey        =Element.attribute("IndexKey").toInt();
        if (Element.hasAttribute("IsVisible"))                  IsVisible       =Element.attribute("IsVisible")=="1";
        if (Element.hasAttribute("SameAsPrevShot"))             SameAsPrevShot  =Element.attribute("SameAsPrevShot")=="1";

        // Attribut of the object
        if (Element.hasAttribute("x"))                          x               =Element.attribute("x").toDouble();                     // Position x
        if (Element.hasAttribute("y"))                          y               =Element.attribute("y").toDouble();                     // Position x
        if (Element.hasAttribute("w"))                          w               =Element.attribute("w").toDouble();                     // size width
        if (Element.hasAttribute("h"))                          h               =Element.attribute("h").toDouble();                     // size height
        if (Element.hasAttribute("BackgroundTransparent"))      Opacity         =Element.attribute("BackgroundTransparent").toInt();    // Style Opacity of the background of the form
        if (Element.hasAttribute("RotateZAxis"))                RotateZAxis     =Element.attribute("RotateZAxis").toDouble();           // Rotation from Z axis
        if (Element.hasAttribute("RotateXAxis"))                RotateXAxis     =Element.attribute("RotateXAxis").toDouble();           // Rotation from X axis
        if (Element.hasAttribute("RotateYAxis"))                RotateYAxis     =Element.attribute("RotateYAxis").toDouble();           // Rotation from Y axis


        if (Element.hasAttribute("BlockAnimType"))              BlockAnimType   =Element.attribute("BlockAnimType").toInt();        // Block animation type
        if (Element.hasAttribute("TurnZAxis"))                  TurnZAxis       =Element.attribute("TurnZAxis").toInt();            // Number of turn from Z axis
        if (Element.hasAttribute("TurnXAxis"))                  TurnXAxis       =Element.attribute("TurnXAxis").toInt();            // Number of turn from X axis
        if (Element.hasAttribute("TurnYAxis"))                  TurnYAxis       =Element.attribute("TurnYAxis").toInt();            // Number of turn from Y axis
        if (Element.hasAttribute("Dissolve"))                   Dissolve        =Element.attribute("Dissolve").toInt();             // Dissolve value

        // Text part
        if ((!CheckTypeComposition)||(TypeComposition!=COMPOSITIONTYPE_SHOT)) {
            Text=Element.attribute("Text");  // Text of the object
            if (Text!="") {
                if (Element.hasAttribute("FontName"))           FontName        =Element.attribute("FontName");                             // font name
                if (Element.hasAttribute("FontSize"))           FontSize        =Element.attribute("FontSize").toInt();                     // font size
                if (Element.hasAttribute("FontColor"))          FontColor       =Element.attribute("FontColor");                            // font color
                if (Element.hasAttribute("FontShadowColor"))    FontShadowColor =Element.attribute("FontShadowColor");                      // font shadow color
                if (Element.hasAttribute("IsBold"))             IsBold          =Element.attribute("IsBold")=="1";                          // true if bold mode
                if (Element.hasAttribute("IsItalic"))           IsItalic        =Element.attribute("IsItalic")=="1";                        // true if Italic mode
                if (Element.hasAttribute("IsUnderline"))        IsUnderline     =Element.attribute("IsUnderline")=="1";                     // true if Underline mode
                if (Element.hasAttribute("HAlign"))             HAlign          =Element.attribute("HAlign").toInt();                       // Horizontal alignement : 0=left, 1=center, 2=right, 3=justif
                if (Element.hasAttribute("VAlign"))             VAlign          =Element.attribute("VAlign").toInt();                       // Vertical alignement : 0=up, 1=center, 2=bottom
                if (Element.hasAttribute("StyleText"))          StyleText       =Element.attribute("StyleText").toInt();                    // Style : 0=normal, 1=outerline, 2=shadow up-left, 3=shadow up-right, 4=shadow bt-left, 5=shadow bt-right

                // Conversion from plaintext (ffd <1.3)
                if (!Text.startsWith("<!DOCTYPE HTML")) {
                    QTextDocument       TextDoc(Text);
                    QFont               Font=QFont(FontName,FontSize*2,IsBold?QFont::Bold:QFont::Normal,IsItalic?QFont::StyleItalic:QFont::StyleNormal);    // FontSize is always 10 and size if given with setPointSizeF !
                    QTextOption         OptionText((HAlign==0)?Qt::AlignLeft:(HAlign==1)?Qt::AlignHCenter:(HAlign==2)?Qt::AlignRight:Qt::AlignJustify); // Setup horizontal alignement
                    QTextCursor         Cursor(&TextDoc);
                    QTextCharFormat     TCF;
                    QTextBlockFormat    TBF;

                    Cursor.select(QTextCursor::Document);
                    OptionText.setWrapMode(QTextOption::WordWrap);                                                                              // Setup word wrap text option
                    Font.setUnderline(IsUnderline);                                                                                             // Set underline

                    TextDoc.setDefaultFont(Font);
                    TextDoc.setDefaultTextOption(OptionText);

                    TCF.setFont(Font);
                    TCF.setFontWeight(IsBold?QFont::Bold:QFont::Normal);
                    TCF.setFontItalic(IsItalic);
                    TCF.setFontUnderline(IsUnderline);
                    TCF.setForeground(QBrush(QColor(FontColor)));
                    TBF.setAlignment((HAlign==0)?Qt::AlignLeft:(HAlign==1)?Qt::AlignHCenter:(HAlign==2)?Qt::AlignRight:Qt::AlignJustify);
                    Cursor.setCharFormat(TCF);
                    Cursor.setBlockFormat(TBF);
                    Text=TextDoc.toHtml();
                }
            }
        }

        // Shot part of text part
        TMType=TEXTMARGINS_FULLSHAPE;   // For compatibility with version prior to 1.5 => force magins type to fullshape
        if (Element.hasAttribute("TxtZoomLevel"))               TxtZoomLevel        =Element.attribute("TxtZoomLevel").toInt();             // Zoom Level for text
        if (Element.hasAttribute("TxtScrollX"))                 TxtScrollX          =Element.attribute("TxtScrollX").toInt();               // Scrolling X for text
        if (Element.hasAttribute("TxtScrollY"))                 TxtScrollY          =Element.attribute("TxtScrollY").toInt();               // Scrolling Y for text
        if (Element.hasAttribute("TMType"))                     TMType              =Element.attribute("TMType").toInt();                   // Text margins type
        if (Element.hasAttribute("TMx"))                        TMx                 =Element.attribute("TMx").toDouble();                   // Text margins
        if (Element.hasAttribute("TMy"))                        TMy                 =Element.attribute("TMy").toDouble();                   // Text margins
        if (Element.hasAttribute("TMw"))                        TMw                 =Element.attribute("TMw").toDouble();                   // Text margins
        if (Element.hasAttribute("TMh"))                        TMh                 =Element.attribute("TMh").toDouble();                   // Text margins

        // Shap part
        if (Element.hasAttribute("BackgroundForm"))             BackgroundForm      =Element.attribute("BackgroundForm").toInt();           // Type of the form : 0=None, 1=Rectangle, 2=Ellipse
        if (Element.hasAttribute("PenSize"))                    PenSize             =Element.attribute("PenSize").toInt();                  // Width of the border of the form
        if (Element.hasAttribute("PenStyle"))                   PenStyle            =Element.attribute("PenStyle").toInt();                 // Style of the pen border of the form
        if (Element.hasAttribute("PenColor"))                   PenColor            =Element.attribute("PenColor");                         // Color of the border of the form
        if (Element.hasAttribute("FormShadowColor"))            FormShadowColor     =Element.attribute("FormShadowColor");                  // Color of the shadow of the form
        if (Element.hasAttribute("FormShadow"))                 FormShadow          =Element.attribute("FormShadow").toInt();               // 0=none, 1=shadow up-left, 2=shadow up-right, 3=shadow bt-left, 4=shadow bt-right
        if (Element.hasAttribute("FormShadowDistance"))         FormShadowDistance  =Element.attribute("FormShadowDistance").toInt();       // Distance from form to shadow

        if ((TypeComposition==COMPOSITIONTYPE_SHOT)&&(ObjectComposition!=NULL)) {
            // Construct link to video and image object from DiaporamaObject->ObjectComposition
            for (int i=0;i<ObjectComposition->List.count();i++) if (ObjectComposition->List[i]->IndexKey==IndexKey) {
                BackgroundBrush->Video=ObjectComposition->List[i]->BackgroundBrush->Video;
                BackgroundBrush->Image=ObjectComposition->List[i]->BackgroundBrush->Image;
                Text=ObjectComposition->List[i]->Text;
                if (Text!="") {
                    FontName        =ObjectComposition->List[i]->FontName;
                    FontSize        =ObjectComposition->List[i]->FontSize;
                    FontColor       =ObjectComposition->List[i]->FontColor;
                    FontShadowColor =ObjectComposition->List[i]->FontShadowColor;
                    IsBold          =ObjectComposition->List[i]->IsBold;
                    IsItalic        =ObjectComposition->List[i]->IsItalic;
                    IsUnderline     =ObjectComposition->List[i]->IsUnderline;
                    HAlign          =ObjectComposition->List[i]->HAlign;
                    VAlign          =ObjectComposition->List[i]->VAlign;
                    StyleText       =ObjectComposition->List[i]->StyleText;
                }
                break;
            }
        }

        // Compatibility with old release : remove case BackgroundForm==0
        if (BackgroundForm==0) {
            BackgroundForm           =1;        // Set to rectangle
            PenSize                  =0;        // border=0
            BackgroundBrush->BrushType=0;        // brushtype=no brush
        } else {
            bool ModifyFlag;
            IsOk=BackgroundBrush->LoadFromXML(Element,"BackgroundBrush",PathForRelativPath,AliasList,&ModifyFlag);  // Brush of the background of the form
            if (ModifyFlag) GlobalMainWindow->SetModifyFlag(true);
        }

        // Ensure unvisible video have no sound !
        if ((!IsVisible)&&(BackgroundBrush->Video!=NULL)) BackgroundBrush->SoundVolume=0;

        return IsOk;
    }
    return false;
}

//====================================================================================================================

int cCompositionObject::GetAutoCompoSize(int ffDProjectGeometry) {
    int   ImageType         =BackgroundBrush->GetImageType();
    int   AutoCompoStyle    =AUTOCOMPOSIZE_CUSTOM;

    // Calc screen size
    qreal ScreenWidth       =qreal(1920);
    qreal ScreenHeight      =qreal(ffDProjectGeometry==GEOMETRY_4_3?1440:ffDProjectGeometry==GEOMETRY_16_9?1080:ffDProjectGeometry==GEOMETRY_40_17?816:1920);
    qreal ScreenGeometry    =ScreenHeight/ScreenWidth;

    // Calc real image size (if it's and image)
    qreal RealImageWidth    =qreal(BackgroundBrush->Image?BackgroundBrush->Image->ImageWidth:BackgroundBrush->Video?BackgroundBrush->Video->ImageWidth:ScreenWidth);
    qreal RealImageHeight   =qreal(BackgroundBrush->Image?BackgroundBrush->Image->ImageHeight:BackgroundBrush->Video?BackgroundBrush->Video->ImageHeight:ScreenHeight); if ((BackgroundBrush->Video!=NULL)&&(RealImageWidth==1920)&&(RealImageHeight=1088)&&(BackgroundBrush->Video->ApplicationConfig->Crop1088To1080)) RealImageHeight=1080;
    //qreal RealImageGeometry =RealImageHeight/RealImageWidth;

    // Calc brush size
    qreal CanvasSize        =sqrt(RealImageWidth*RealImageWidth+RealImageHeight*RealImageHeight);   // Calc hypothenuse of the image to define full canvas
    qreal ImageWidth        =CanvasSize*BackgroundBrush->ZoomFactor;
    qreal ImageHeight       =CanvasSize*BackgroundBrush->ZoomFactor*BackgroundBrush->AspectRatio;
    qreal ImageGeometry     =ImageHeight/ImageWidth;

    // Calc destination size
    qreal DestWidth         =ScreenWidth*w;
    qreal DestHeight        =ScreenHeight*h;
    //qreal DestGeometry      =DestHeight/DestWidth;

    if ((DestWidth==ImageWidth)&&(DestHeight==ImageHeight)) AutoCompoStyle=AUTOCOMPOSIZE_REALSIZE; else {

        // Make adjustement if it's not an image and geometry is locked
        if ((ImageType==IMAGETYPE_UNKNOWN)&&(BackgroundBrush->LockGeometry)) {
            if ((ImageHeight*(h/w))<ScreenHeight)                       ScreenHeight=ScreenHeight*(h/w);
                else                                                    ScreenWidth =ScreenWidth/(h/w);
        }

        // Make adjustement if it's an image and ImageGeometry!=DestGeometry
        if ((ImageType!=IMAGETYPE_UNKNOWN)&&(ImageGeometry!=ScreenGeometry)) {
            if ((ImageHeight*(ScreenWidth/ImageWidth))<ScreenHeight)    ScreenHeight=ImageHeight*(ScreenWidth/ImageWidth);
                else                                                    ScreenWidth =ImageWidth*(ScreenHeight/ImageHeight);
        }

        if ((int(DestWidth)==int(ScreenWidth))&&    (int(DestHeight)==int(ScreenHeight)))       AutoCompoStyle=AUTOCOMPOSIZE_FULLSCREEN;
        if ((int(DestWidth)==int(ScreenWidth*0.9))&&(int(DestHeight)==int(ScreenHeight*0.9)))   AutoCompoStyle=AUTOCOMPOSIZE_TVMARGINS;
        if ((int(DestWidth)==int(2*ScreenWidth/3))&&(int(DestHeight)==int(2*ScreenHeight/3)))   AutoCompoStyle=AUTOCOMPOSIZE_TWOTHIRDSSCREEN;
        if ((int(DestWidth)==int(ScreenWidth/2))&&  (int(DestHeight)==int(ScreenHeight/2)))     AutoCompoStyle=AUTOCOMPOSIZE_HALFSCREEN;
        if ((int(DestWidth)==int(ScreenWidth/3))&&  (int(DestHeight)==int(ScreenHeight/3)))     AutoCompoStyle=AUTOCOMPOSIZE_THIRDSCREEN;
        if ((int(DestWidth)==int(ScreenWidth/4))&&  (int(DestHeight)==int(ScreenHeight/4)))     AutoCompoStyle=AUTOCOMPOSIZE_QUARTERSCREEN;
    }
    return AutoCompoStyle;
}

//====================================================================================================================

void cCompositionObject::ApplyAutoCompoSize(int AutoCompoStyle,int ffDProjectGeometry) {
    int   ImageType         =BackgroundBrush->GetImageType();

    // Calc screen size
    qreal ScreenWidth       =qreal(1920);
    qreal ScreenHeight      =qreal(ffDProjectGeometry==GEOMETRY_4_3?1440:ffDProjectGeometry==GEOMETRY_16_9?1080:ffDProjectGeometry==GEOMETRY_40_17?816:1920);
    //qreal ScreenGeometry    =ScreenHeight/ScreenWidth;

    // Calc real image size (if it's and image)
    qreal RealImageWidth    =qreal(BackgroundBrush->Image?BackgroundBrush->Image->ImageWidth:BackgroundBrush->Video?BackgroundBrush->Video->ImageWidth:ScreenWidth);
    qreal RealImageHeight   =qreal(BackgroundBrush->Image?BackgroundBrush->Image->ImageHeight:BackgroundBrush->Video?BackgroundBrush->Video->ImageHeight:ScreenHeight); if ((BackgroundBrush->Video!=NULL)&&(RealImageWidth==1920)&&(RealImageHeight=1088)&&(BackgroundBrush->Video->ApplicationConfig->Crop1088To1080)) RealImageHeight=1080;
    //qreal RealImageGeometry =RealImageHeight/RealImageWidth;

    // Calc brush size
    qreal CanvasSize        =sqrt(RealImageWidth*RealImageWidth+RealImageHeight*RealImageHeight);   // Calc hypothenuse of the image to define full canvas
    qreal ImageWidth        =CanvasSize*BackgroundBrush->ZoomFactor;
    qreal ImageHeight       =CanvasSize*BackgroundBrush->ZoomFactor*BackgroundBrush->AspectRatio;
    qreal ImageGeometry     =ImageHeight/ImageWidth;

    // Calc destination size
    qreal DestWidth =ScreenWidth;
    qreal DestHeight=ScreenHeight;
    switch (AutoCompoStyle) {
        case AUTOCOMPOSIZE_CUSTOM           :  DestWidth=ScreenWidth*w;    DestHeight=ScreenHeight*h;      break;      // Keep current value
        case AUTOCOMPOSIZE_REALSIZE         :  DestWidth=ImageWidth;       DestHeight=ImageHeight;         break;
        case AUTOCOMPOSIZE_FULLSCREEN       :  DestWidth=ScreenWidth;      DestHeight=ScreenHeight;        break;
        case AUTOCOMPOSIZE_TVMARGINS        :  DestWidth=ScreenWidth*0.9;  DestHeight=ScreenHeight*0.9;    break;      // TV Margins is 5% each
        case AUTOCOMPOSIZE_TWOTHIRDSSCREEN  :  DestWidth=2*ScreenWidth/3;  DestHeight=2*ScreenHeight/3;    break;
        case AUTOCOMPOSIZE_HALFSCREEN       :  DestWidth=ScreenWidth/2;    DestHeight=ScreenHeight/2;      break;
        case AUTOCOMPOSIZE_THIRDSCREEN      :  DestWidth=ScreenWidth/3;    DestHeight=ScreenHeight/3;      break;
        case AUTOCOMPOSIZE_QUARTERSCREEN    :  DestWidth=ScreenWidth/4;    DestHeight=ScreenHeight/4;      break;
    }
    qreal DestGeometry=DestHeight/DestWidth;

    // Make adjustement if it's not an image and geometry is locked
    if ((ImageType==IMAGETYPE_UNKNOWN)&&(BackgroundBrush->LockGeometry)) {
        if ((ImageHeight*(h/w))<DestHeight)                     DestHeight=DestHeight*(h/w);
            else                                                DestWidth =DestWidth/(h/w);
    }
    // Make adjustement if it's an image and ImageGeometry!=DestGeometry
    if ((ImageType!=IMAGETYPE_UNKNOWN)&&(ImageGeometry!=DestGeometry)) {
        if ((ImageHeight*(DestWidth/ImageWidth))<DestHeight)    DestHeight=ImageHeight*(DestWidth/ImageWidth);
            else                                                DestWidth =ImageWidth*(DestHeight/ImageHeight);
    }

    // Apply destination size to Composition object
    w           =DestWidth/ScreenWidth;
    h           =DestHeight/ScreenHeight;
    x           =((ScreenWidth-DestWidth)/2)/ScreenWidth;
    y           =((ScreenHeight-DestHeight)/2)/ScreenHeight;
    RotateZAxis =0;
    RotateXAxis =0;
    RotateYAxis =0;
}

//====================================================================================================================

QString cCompositionObject::GetBlockShapeStyle() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:GetBlockShapeStyle");

    return  QString("###BackgroundForm:%1").arg(BackgroundForm)+
            QString("###PenSize:%1").arg(PenSize)+
            QString("###PenStyle:%1").arg(PenStyle)+
            QString("###FormShadow:%1").arg(FormShadow)+
            QString("###FormShadowDistance:%1").arg(FormShadowDistance)+
            QString("###Opacity:%1").arg(Opacity)+
            "###PenColor:"+PenColor+
            "###FormShadowColor:"+FormShadowColor;
}

void cCompositionObject::ApplyBlockShapeStyle(QString StyleDef) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:ApplyBlockShapeStyle");

    QStringList List;

    // String to StringList
    while (StyleDef.contains("###")) {
        List.append(StyleDef.left(StyleDef.indexOf("###")));
        StyleDef=StyleDef.mid(StyleDef.indexOf("###")+QString("###").length());
    }
    if (!StyleDef.isEmpty()) List.append(StyleDef);

    // Apply Style
    for (int i=0;i<List.count();i++) {
        if      (List[i].startsWith("BackgroundForm:"))     BackgroundForm     =List[i].mid(QString("BackgroundForm:").length()).toInt();
        else if (List[i].startsWith("PenSize:"))            PenSize            =List[i].mid(QString("PenSize:").length()).toInt();
        else if (List[i].startsWith("PenStyle:"))           PenStyle           =List[i].mid(QString("PenStyle:").length()).toInt();
        else if (List[i].startsWith("FormShadow:"))         FormShadow         =List[i].mid(QString("FormShadow:").length()).toInt();
        else if (List[i].startsWith("FormShadowDistance:")) FormShadowDistance =List[i].mid(QString("FormShadowDistance:").length()).toInt();
        else if (List[i].startsWith("Opacity:"))            Opacity            =List[i].mid(QString("Opacity:").length()).toInt();
        else if (List[i].startsWith("PenColor:"))           PenColor           =List[i].mid(QString("PenColor:").length());
        else if (List[i].startsWith("FormShadowColor:"))    FormShadowColor    =List[i].mid(QString("FormShadowColor:").length());
    }
}

//====================================================================================================================

QString cCompositionObject::GetTextStyle() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:GetTextStyle");

    return  QString("FontSize:%1").arg(FontSize)+
            QString("###HAlign:%1").arg(HAlign)+
            QString("###VAlign:%1").arg(VAlign)+
            QString("###StyleText:%1").arg(StyleText)+
            "###FontColor:"+FontColor+
            "###FontShadowColor:"+FontShadowColor+
            QString("###Bold:%1").arg(IsBold?1:0)+
            QString("###Italic:%1").arg(IsItalic?1:0)+
            QString("###Underline:%1").arg(IsUnderline?1:0)+
            "###FontName:"+FontName;
}

void cCompositionObject::ApplyTextStyle(QString StyleDef) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:ApplyTextStyle");

    QStringList List;

    // String to StringList
    while (StyleDef.contains("###")) {
        List.append(StyleDef.left(StyleDef.indexOf("###")));
        StyleDef=StyleDef.mid(StyleDef.indexOf("###")+QString("###").length());
    }
    if (!StyleDef.isEmpty()) List.append(StyleDef);

    // Apply Style
    for (int i=0;i<List.count();i++) {
        if      (List[i].startsWith("FontSize:"))           FontSize       =List[i].mid(QString("FontSize:").length()).toInt();
        else if (List[i].startsWith("HAlign:"))             HAlign         =List[i].mid(QString("HAlign:").length()).toInt();
        else if (List[i].startsWith("VAlign:"))             VAlign         =List[i].mid(QString("VAlign:").length()).toInt();
        else if (List[i].startsWith("StyleText:"))          StyleText      =List[i].mid(QString("StyleText:").length()).toInt();
        else if (List[i].startsWith("FontColor:"))          FontColor      =List[i].mid(QString("FontColor:").length());
        else if (List[i].startsWith("FontShadowColor:"))    FontShadowColor=List[i].mid(QString("FontShadowColor:").length());
        else if (List[i].startsWith("Bold:"))               IsBold         =List[i].mid(QString("Bold:").length()).toInt()==1;
        else if (List[i].startsWith("Italic:"))             IsItalic       =List[i].mid(QString("Italic:").length()).toInt()==1;
        else if (List[i].startsWith("Underline:"))          IsUnderline    =List[i].mid(QString("Underline:").length()).toInt()==1;
        else if (List[i].startsWith("FontName:"))           FontName       =List[i].mid(QString("FontName:").length());
    }

    // Apply to html text
    QTextDocument       TextDoc;
    QFont               Font=QFont(FontName,FontSize,IsBold?QFont::Bold:QFont::Normal,IsItalic?QFont::StyleItalic:QFont::StyleNormal);
    QTextOption         OptionText((HAlign==0)?Qt::AlignLeft:(HAlign==1)?Qt::AlignHCenter:(HAlign==2)?Qt::AlignRight:Qt::AlignJustify);
    QTextCursor         Cursor(&TextDoc);
    QTextCharFormat     TCF;
    QTextBlockFormat    TBF;
    TextDoc.setHtml(Text);
    Cursor.select(QTextCursor::Document);
    OptionText.setWrapMode(QTextOption::WordWrap);
    Font.setUnderline(IsUnderline);
    TextDoc.setDefaultFont(Font);
    TextDoc.setDefaultTextOption(OptionText);
    TCF.setFont(Font);
    TCF.setFontWeight(IsBold?QFont::Bold:QFont::Normal);
    TCF.setFontItalic(IsItalic);
    TCF.setFontUnderline(IsUnderline);
    TCF.setForeground(QBrush(QColor(FontColor)));
    TBF.setAlignment((HAlign==0)?Qt::AlignLeft:(HAlign==1)?Qt::AlignHCenter:(HAlign==2)?Qt::AlignRight:Qt::AlignJustify);
    Cursor.setCharFormat(TCF);
    Cursor.setBlockFormat(TBF);
    Text=TextDoc.toHtml();
}

//====================================================================================================================

QString cCompositionObject::GetBackgroundStyle() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:GetBackgroundStyle");

    return  QString("BrushType:%1").arg(BackgroundBrush->BrushType)+
            QString("###PatternType:%1").arg(BackgroundBrush->PatternType)+
            QString("###GradientOrientation:%1").arg(BackgroundBrush->GradientOrientation)+
            "###ColorD:"+BackgroundBrush->ColorD+
            "###ColorF:"+BackgroundBrush->ColorF+
            "###ColorIntermed:"+BackgroundBrush->ColorIntermed+
            QString("###Intermediate:%1").arg(BackgroundBrush->Intermediate,0,'e')+
            "###BrushImage:"+BackgroundBrush->BrushImage;
}

void cCompositionObject::ApplyBackgroundStyle(QString StyleDef) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:ApplyBackgroundStyle");

    QStringList List;

    // String to StringList
    while (StyleDef.contains("###")) {
        List.append(StyleDef.left(StyleDef.indexOf("###")));
        StyleDef=StyleDef.mid(StyleDef.indexOf("###")+QString("###").length());
    }
    if (!StyleDef.isEmpty()) List.append(StyleDef);

    // Apply Style
    for (int i=0;i<List.count();i++) {
        if      (List[i].startsWith("BrushType:"))              BackgroundBrush->BrushType           =List[i].mid(QString("BrushType:").length()).toInt();
        else if (List[i].startsWith("PatternType:"))            BackgroundBrush->PatternType         =List[i].mid(QString("PatternType:").length()).toInt();
        else if (List[i].startsWith("GradientOrientation:"))    BackgroundBrush->GradientOrientation =List[i].mid(QString("GradientOrientation:").length()).toInt();
        else if (List[i].startsWith("ColorD:"))                 BackgroundBrush->ColorD              =List[i].mid(QString("ColorD:").length());
        else if (List[i].startsWith("ColorF:"))                 BackgroundBrush->ColorF              =List[i].mid(QString("ColorF:").length());
        else if (List[i].startsWith("ColorIntermed:"))          BackgroundBrush->ColorIntermed       =List[i].mid(QString("ColorIntermed:").length());
        else if (List[i].startsWith("Intermediate:"))           BackgroundBrush->Intermediate        =List[i].mid(QString("Intermediate:").length()).toDouble();
        else if (List[i].startsWith("BrushImage:"))             BackgroundBrush->BrushImage          =List[i].mid(QString("BrushImage:").length());
    }
}

//====================================================================================================================

QString cCompositionObject::GetCoordinateStyle() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:GetCoordinateStyle");

    QString Style=QString("###X:%1").arg(x,0,'e')+
            QString("###Y:%1").arg(y,0,'e')+
            QString("###W:%1").arg(w,0,'e')+
            QString("###H:%1").arg(h,0,'e')+
            QString("###RotateZAxis:%1").arg(RotateZAxis,0,'e')+
            QString("###RotateXAxis:%1").arg(RotateXAxis,0,'e')+
            QString("###RotateYAxis:%1").arg(RotateYAxis,0,'e');

    return Style;
}

void cCompositionObject::ApplyCoordinateStyle(QString StyleDef) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:ApplyCoordinateStyle");

    QStringList List;
    bool        RecalcAspectRatio=true;

    // String to StringList
    while (StyleDef.contains("###")) {
        List.append(StyleDef.left(StyleDef.indexOf("###")));
        StyleDef=StyleDef.mid(StyleDef.indexOf("###")+QString("###").length());
    }
    if (!StyleDef.isEmpty()) List.append(StyleDef);

    // Apply Style
    for (int i=0;i<List.count();i++) {
        if      (List[i].startsWith("X:"))              x               =List[i].mid(QString("X:").length()).toDouble();
        else if (List[i].startsWith("Y:"))              y               =List[i].mid(QString("Y:").length()).toDouble();
        else if (List[i].startsWith("W:"))              w               =List[i].mid(QString("W:").length()).toDouble();
        else if (List[i].startsWith("H:"))              h               =List[i].mid(QString("H:").length()).toDouble();
        else if (List[i].startsWith("RotateZAxis:"))    RotateZAxis     =List[i].mid(QString("RotateZAxis:").length()).toDouble();
        else if (List[i].startsWith("RotateXAxis:"))    RotateXAxis     =List[i].mid(QString("RotateXAxis:").length()).toDouble();
        else if (List[i].startsWith("RotateYAxis:"))    RotateYAxis     =List[i].mid(QString("RotateYAxis:").length()).toDouble();
    }
    // if not set by style then compute Aspect Ratio
    if (RecalcAspectRatio) {
        double DisplayW=1920,DisplayH=1080;
        if (GlobalMainWindow->Diaporama->ImageGeometry==GEOMETRY_4_3)        { DisplayW=1440; DisplayH=1080; }
        else if (GlobalMainWindow->Diaporama->ImageGeometry==GEOMETRY_16_9)  { DisplayW=1920; DisplayH=1080; }
        else if (GlobalMainWindow->Diaporama->ImageGeometry==GEOMETRY_40_17) { DisplayW=1920; DisplayH=816;  }
        BackgroundBrush->AspectRatio =(h*DisplayH)/(w*DisplayW);
    }
}

//====================================================================================================================

void cCompositionObject::CopyFromCompositionObject(cCompositionObject *CompositionObjectToCopy) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:CopyFromCompositionObject");

    IsVisible            =CompositionObjectToCopy->IsVisible;
    x                    =CompositionObjectToCopy->x;
    y                    =CompositionObjectToCopy->y;
    w                    =CompositionObjectToCopy->w;
    h                    =CompositionObjectToCopy->h;
    RotateZAxis          =CompositionObjectToCopy->RotateZAxis;
    RotateXAxis          =CompositionObjectToCopy->RotateXAxis;
    RotateYAxis          =CompositionObjectToCopy->RotateYAxis;
    BlockAnimType        =CompositionObjectToCopy->BlockAnimType;
    TurnZAxis            =CompositionObjectToCopy->TurnZAxis;
    TurnXAxis            =CompositionObjectToCopy->TurnXAxis;
    TurnYAxis            =CompositionObjectToCopy->TurnYAxis;
    Dissolve             =CompositionObjectToCopy->Dissolve;
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
    FormShadowColor      =CompositionObjectToCopy->FormShadowColor;
    FormShadow           =CompositionObjectToCopy->FormShadow;
    FormShadowDistance   =CompositionObjectToCopy->FormShadowDistance;
    TxtZoomLevel         =CompositionObjectToCopy->TxtZoomLevel;
    TxtScrollX           =CompositionObjectToCopy->TxtScrollX;
    TxtScrollY           =CompositionObjectToCopy->TxtScrollY;
    TMType               =CompositionObjectToCopy->TMType;
    TMx                  =CompositionObjectToCopy->TMx;
    TMy                  =CompositionObjectToCopy->TMy;
    TMw                  =CompositionObjectToCopy->TMw;
    TMh                  =CompositionObjectToCopy->TMh;

    BackgroundBrush->CopyFromBrushDefinition(CompositionObjectToCopy->BackgroundBrush);
}

//====================================================================================================================

// ADJUST_RATIO=Adjustement ratio for pixel size (all size are given for full hd and adjust for real wanted size)
void cCompositionObject::DrawCompositionObject(QPainter *DestPainter,double  ADJUST_RATIO,int AddX,int AddY,int width,int height,bool PreviewMode,qlonglong Position,qlonglong StartPosToAdd,
                                               cSoundBlockList *SoundTrackMontage,double PctDone,cCompositionObject *PrevCompoObject,bool UseBrushCache,qlonglong ShotDuration,bool EnableAnimation,
                                               bool Transfo,double NewX,double NewY,double NewW,double NewH) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:DrawCompositionObject");

    // W and H = 0 when producing sound track in render process
    if (!IsVisible) return;

    bool SoundOnly=(DestPainter==NULL)||(width==0)||(height==0)||(Transfo && ((NewW==0)||(NewH==0)))||(!Transfo && ((w==0)||(h==0)));

    if (SoundOnly) {
        // if SoundOnly then load Brush of type BRUSHTYPE_IMAGEDISK to SoundTrackMontage
        if ((BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(SoundTrackMontage!=NULL)) {
            QBrush *BR=BackgroundBrush->GetBrush(QRectF(0,0,0,0),PreviewMode,Position,StartPosToAdd,SoundTrackMontage,PctDone,NULL);
            if (BR) delete BR;
        }
    } else {

        if (!PreviewMode || GlobalMainWindow->ApplicationConfig->Smoothing) DestPainter->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
            else DestPainter->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);

        // Define values depending on PctDone and PrevCompoObject
        double TheX             =Transfo?NewX:x;
        double TheY             =Transfo?NewY:y;
        double TheW             =Transfo?NewW:w;
        double TheH             =Transfo?NewH:h;
        double TheRotateZAxis   =RotateZAxis+(EnableAnimation && (BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*TurnZAxis:0);
        double TheRotateXAxis   =RotateXAxis+(EnableAnimation && (BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*TurnXAxis:0);
        double TheRotateYAxis   =RotateYAxis+(EnableAnimation && (BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*TurnYAxis:0);
        double TheTxtZoomLevel  =TxtZoomLevel;
        double TheTxtScrollX    =TxtScrollX;
        double TheTxtScrollY    =TxtScrollY;

        if (PrevCompoObject) {
            if (PrevCompoObject->x!=TheX)                       TheX            =PrevCompoObject->x+(TheX-PrevCompoObject->x)*PctDone;
            if (PrevCompoObject->y!=TheY)                       TheY            =PrevCompoObject->y+(TheY-PrevCompoObject->y)*PctDone;
            if (PrevCompoObject->w!=TheW)                       TheW            =PrevCompoObject->w+(TheW-PrevCompoObject->w)*PctDone;
            if (PrevCompoObject->h!=TheH)                       TheH            =PrevCompoObject->h+(TheH-PrevCompoObject->h)*PctDone;
            if (PrevCompoObject->RotateZAxis!=TheRotateZAxis)   TheRotateZAxis  =PrevCompoObject->RotateZAxis+(TheRotateZAxis-PrevCompoObject->RotateZAxis)*PctDone;
            if (PrevCompoObject->RotateXAxis!=TheRotateXAxis)   TheRotateXAxis  =PrevCompoObject->RotateXAxis+(TheRotateXAxis-PrevCompoObject->RotateXAxis)*PctDone;
            if (PrevCompoObject->RotateYAxis!=TheRotateYAxis)   TheRotateYAxis  =PrevCompoObject->RotateYAxis+(TheRotateYAxis-PrevCompoObject->RotateYAxis)*PctDone;
            if (PrevCompoObject->TxtZoomLevel!=TheTxtZoomLevel) TheTxtZoomLevel =PrevCompoObject->TxtZoomLevel+(TheTxtZoomLevel-PrevCompoObject->TxtZoomLevel)*PctDone;
            if (PrevCompoObject->TxtScrollX!=TheTxtScrollX)     TheTxtScrollX   =PrevCompoObject->TxtScrollX+(TheTxtScrollX-PrevCompoObject->TxtScrollX)*PctDone;
            if (PrevCompoObject->TxtScrollY!=TheTxtScrollY)     TheTxtScrollY   =PrevCompoObject->TxtScrollY+(TheTxtScrollY-PrevCompoObject->TxtScrollY)*PctDone;
        } else {
            if (EnableAnimation && (BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)) {
                TheRotateZAxis=RotateZAxis+360*TurnZAxis*PctDone;
                TheRotateXAxis=RotateXAxis+360*TurnXAxis*PctDone;
                TheRotateYAxis=RotateYAxis+360*TurnYAxis*PctDone;
            }
        }

        double  W  =int(TheW*double(width));    if ((int(W)   & 0x01)==1)   W=W+1;
        double  H  =int(TheH*double(height));   if ((int(H)   & 0x01)==1)   H=H+1;
        double  Hyp=int(sqrt(W*W+H*H));         if ((int(Hyp) & 0x01)==1)   Hyp=Hyp+1;
        double  Wb =int(Hyp);
        double  Hb =Wb;         // always square image

        AddX-=(Wb-W)/2;
        AddY-=(Hb-H)/2;
        double  DstX=AddX+TheX*double(width);
        double  DstY=AddY+TheY*double(height);
        double  DstW=Wb;
        double  DstH=Hb;

        //***********************************************************************************
        // Prepare brush
        //***********************************************************************************

        if ((W>0)&&(H>0)) {
            QPen     Pen;
            QImage   Img(Hyp,Hyp,QImage::Format_ARGB32_Premultiplied);
            QPainter Painter;
            Painter.begin(&Img);
            Painter.setCompositionMode(QPainter::CompositionMode_Source);
            Painter.fillRect(QRect(0,0,Hyp,Hyp),Qt::transparent);
            Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            if (!PreviewMode || GlobalMainWindow->ApplicationConfig->Smoothing)  Painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
                else Painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
            Pen.setCapStyle(Qt::RoundCap);
            Pen.setJoinStyle(Qt::RoundJoin);
            Pen.setCosmetic(false);

            Pen.setStyle(Qt::SolidLine);

            // Draw ExternalBorder border
            if (PenSize==0) Painter.setPen(Qt::NoPen); else {
                Pen.setColor(PenColor);
                Pen.setWidthF(double(PenSize)*ADJUST_RATIO);
                Pen.setStyle((Qt::PenStyle)PenStyle);
                Painter.setPen(Pen);
            }
            // All coordonates from center
            double      CenterX=Hyp/2;
            double      CenterY=Hyp/2;
            QTransform  Matrix;
            Matrix.translate(CenterX,CenterY);
            if (TheRotateZAxis!=0) Matrix.rotate(TheRotateZAxis,Qt::ZAxis);   // Standard axis
            if (TheRotateXAxis!=0) Matrix.rotate(TheRotateXAxis,Qt::XAxis);   // Rotate from X axis
            if (TheRotateYAxis!=0) Matrix.rotate(TheRotateYAxis,Qt::YAxis);   // Rotate from Y axis
            Painter.setWorldTransform(Matrix,false);

            //**********************************************************************************
            // Block brush & shape part
            //**********************************************************************************

            if (BackgroundBrush->BrushType==BRUSHTYPE_NOBRUSH) Painter.setBrush(Qt::transparent); else {

                // Create brush with Ken Burns effect !
                QBrush *BR              =NULL;

                BR=BackgroundBrush->GetBrush(QRectF(0,0,W,H),PreviewMode,Position,StartPosToAdd,SoundTrackMontage,PctDone,PrevCompoObject?PrevCompoObject->BackgroundBrush:NULL,UseBrushCache);
                if (BR) {
                    QTransform  MatrixBR;
                    MatrixBR.translate(-W/2,-H/2);
                    BR->setTransform(MatrixBR);  // Apply transforme matrix to the brush
                    Painter.setBrush(*BR);
                    delete BR;
                } else {
                    ToLog(LOGMSG_CRITICAL,"Error in cCompositionObject::DrawCompositionObject Brush is NULL !");
                }
            }
            if (BackgroundBrush->BrushType==BRUSHTYPE_NOBRUSH) Painter.setCompositionMode(QPainter::CompositionMode_Source);
            QList<QPolygonF> List=ComputePolygon(BackgroundForm,-W/2,-H/2,W,H,0,0);
            for (int i=0;i<List.count();i++) Painter.drawPolygon(List.at(i));
            if (BackgroundBrush->BrushType==BRUSHTYPE_NOBRUSH) Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

            //**********************************************************************************
            // Text part
            //**********************************************************************************

            if (TheTxtZoomLevel>0) {

                QRectF          ShapeRect =PolygonToRectF(List);
                double          FullMargin=((TMType==TEXTMARGINS_FULLSHAPE)||(TMType==TEXTMARGINS_CUSTOM))?0:double(PenSize)*ADJUST_RATIO/double(2);
                QRectF          TextMargin;
                double          PointSize =((double(width)/double(SCALINGTEXTFACTOR)));
                QTextDocument   TextDocument;

                // if type is ShapeDefault, then adjust with border size
                if ((TMType==TEXTMARGINS_FULLSHAPE)||(TMType==TEXTMARGINS_CUSTOM)) {
                    TextMargin=QRectF(TMx*TheW*width,TMy*TheH*height,TMw*TheW*width,TMh*TheH*height);
                } else {
                    TextMargin=QRectF(ShapeFormDefinition[BackgroundForm].TMx*TheW*width+FullMargin,ShapeFormDefinition[BackgroundForm].TMy*TheH*height+FullMargin,
                                  ShapeFormDefinition[BackgroundForm].TMw*TheW*width-FullMargin*2,ShapeFormDefinition[BackgroundForm].TMh*TheH*height-FullMargin*2);
                }

                //****************
                /*
                Painter.save();
                QPen PP(Qt::white);
                PP.setStyle(Qt::DotLine);
                PP.setWidth(1);
                Painter.setPen(PP);
                Painter.setBrush(Qt::NoBrush);
                Painter.drawRect(QRectF(ShapeRect.left()+TextMargin.left(),ShapeRect.top()+TextMargin.top(),TextMargin.width(),TextMargin.height()));
                Painter.restore();
                */
                //****************

                Painter.setClipRect(QRectF(ShapeRect.left()+TextMargin.left(),ShapeRect.top()+TextMargin.top(),TextMargin.width(),TextMargin.height()));
                Painter.setClipping(true);

                TextDocument.setHtml(Text);
                TextDocument.setTextWidth(TextMargin.width()/PointSize);

                QRectF  FmtBdRect(0,0,
                                  double(TextDocument.documentLayout()->documentSize().width())*(TheTxtZoomLevel/100)*PointSize,
                                  double(TextDocument.documentLayout()->documentSize().height())*(TheTxtZoomLevel/100)*PointSize);

                double  DecalX=ShapeRect.left()+TextMargin.left()+(TheTxtScrollX/100)*TextMargin.width();
                int     MaxH  =TextMargin.height()>FmtBdRect.height()?TextMargin.height():FmtBdRect.height();
                double  DecalY=ShapeRect.top()+TextMargin.top()+(-TheTxtScrollY/100)*MaxH;

                if (VAlign==0)      /*Nothing to do*/;                                            //Qt::AlignTop
                else if (VAlign==1) DecalY=DecalY+(TextMargin.height()-FmtBdRect.height())/2;     //Qt::AlignVCenter
                else                DecalY=DecalY+(TextMargin.height()-FmtBdRect.height());       //Qt::AlignBottom)

                QAbstractTextDocumentLayout::PaintContext Context;

                QTextCursor         Cursor(&TextDocument);
                QTextCharFormat     TCF;
                Cursor.select(QTextCursor::Document);
                if (StyleText==1) {
                    // Add outerline for painting
                    TCF.setTextOutline(QPen(QColor(FontShadowColor)));
                    Cursor.mergeCharFormat(TCF);
                } else if (StyleText!=0) {
                    // Paint shadow of the text
                    TCF.setForeground(QBrush(QColor(FontShadowColor)));
                    Cursor.mergeCharFormat(TCF);
                    Painter.save();
                    switch (StyleText) {
                        case 2: Painter.translate(DecalX-1,DecalY-1);   break;  //2=shadow up-left
                        case 3: Painter.translate(DecalX+1,DecalY-1);   break;  //3=shadow up-right
                        case 4: Painter.translate(DecalX-1,DecalY+1);   break;  //4=shadow bt-left
                        case 5: Painter.translate(DecalX+1,DecalY+1);   break;  //5=shadow bt-right
                    }
                    Painter.scale((TheTxtZoomLevel/100)*PointSize,(TheTxtZoomLevel/100)*PointSize);
                    TextDocument.documentLayout()->draw(&Painter,Context);
                    Painter.restore();
                    TextDocument.setHtml(Text);     // Restore Text Document
                }
                Painter.translate(DecalX,DecalY);
                Painter.scale((TheTxtZoomLevel/100)*PointSize,(TheTxtZoomLevel/100)*PointSize);
                TextDocument.documentLayout()->draw(&Painter,Context);
            }

            Painter.end();

            //**********************************************************************************
            // Block shadow part
            //**********************************************************************************

            if ((FormShadow)&&(!Img.isNull())) Img=AddShadow(Img,DstX,DstY,DstW,DstH,double(FormShadowDistance)*ADJUST_RATIO);

            //**********************************************************************************
            // Opacity and dissolve annimation
            //**********************************************************************************
            double  DestOpacity=(Opacity==1?0.75:Opacity==2?0.50:Opacity==3?0.25:1);

            if (EnableAnimation) {
                if (BlockAnimType==BLOCKANIMTYPE_DISSOLVE) {

                    double BlinkNumber=0;
                    switch (Dissolve) {
                        case BLOCKANIMVALUE_APPEAR        : DestOpacity=DestOpacity*PctDone;        break;
                        case BLOCKANIMVALUE_DISAPPEAR     : DestOpacity=DestOpacity*(1-PctDone);    break;
                        case BLOCKANIMVALUE_BLINK_SLOW    : BlinkNumber=0.25;                       break;
                        case BLOCKANIMVALUE_BLINK_MEDIUM  : BlinkNumber=0.5;                        break;
                        case BLOCKANIMVALUE_BLINK_FAST    : BlinkNumber=1;                          break;
                        case BLOCKANIMVALUE_BLINK_VERYFAST: BlinkNumber=2;                          break;
                    }
                    if (BlinkNumber!=0) {
                        BlinkNumber=BlinkNumber*ShotDuration;
                        if (int(BlinkNumber/1000)!=(BlinkNumber/1000)) BlinkNumber=int(BlinkNumber/1000)+1; else BlinkNumber=int(BlinkNumber/1000); // Adjust to upper 1000
                        double FullPct=PctDone*BlinkNumber*100;
                        FullPct=int(FullPct)-int(FullPct/100)*100;
                        FullPct=(FullPct/100)*2;
                        if (FullPct<1)  DestOpacity=DestOpacity*(1-FullPct);
                            else        DestOpacity=DestOpacity*(FullPct-1);
                    }
                }

            }
            DestPainter->setOpacity(DestOpacity);

            //**********************************************************************************

            double  SrcX=0;
            double  SrcY=0;
            if (DstX<0) {
                SrcX=-DstX;
                DstW+=DstX;
                DstX=0;
            }
            if (DstY<0) {
                SrcY=-DstY;
                DstH+=DstY;
                DstY=0;
            }
            if (DstW>width)     DstW=width;
            if (DstH>height)    DstH=height;
            if (SrcX+DstW>Wb)   DstW=Wb-SrcX;
            if (SrcY+DstH>Hb)   DstH=Hb-SrcY;

            if ((!Img.isNull())) DestPainter->drawImage(QRectF(DstX,DstY,DstW,DstH),Img,QRectF(SrcX,SrcY,DstW,DstH));
            DestPainter->setOpacity(1);
        }
    }
}

//====================================================================================================================

QImage cCompositionObject::AddShadow(QImage SourceImage,double &DstX,double &DstY,double &DstW,double &DstH,double Distance) {

    // 1st step : construct ImgShadow as a mask from SourceImage

    QImage  ImgShadow   =SourceImage.copy();
    Uint8   *Data       =ImgShadow.bits();
    QColor  SColor      =QColor(FormShadowColor);
    Uint8   R           =SColor.red();
    Uint8   G           =SColor.green();
    Uint8   B           =SColor.blue();

    for (int i=0;i<(DstW-1)*(DstH-1);i++) {
      if (*(Data+3)!=0) {
        *Data++=B;
        *Data++=G;
        *Data++=R;
        Data++;     // Keep Alpha chanel
      } else Data+=4;
    }

    // 2nd step : Construct a DestImage mergin shadow image and source image

    DstW=DstW+Distance;
    DstH=DstH+Distance;

    QImage      DestImage(DstW,DstH,QImage::Format_ARGB32_Premultiplied);
    QPainter    Painter;

    Painter.begin(&DestImage);
    Painter.setCompositionMode(QPainter::CompositionMode_Source);
    Painter.fillRect(QRect(0,0,DstW,DstH),Qt::transparent);
    Painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    switch (FormShadow) {
        case 1  : Painter.setOpacity(0.75); Painter.drawImage(0,0,ImgShadow);               Painter.setOpacity(1);  Painter.drawImage(Distance,Distance,SourceImage);  DstX=DstX-Distance;     DstY=DstY-Distance;  break;
        case 2  : Painter.setOpacity(0.75); Painter.drawImage(Distance,0,ImgShadow);        Painter.setOpacity(1);  Painter.drawImage(0,Distance,SourceImage);                                 DstY=DstY-Distance;  break;
        case 3  : Painter.setOpacity(0.75); Painter.drawImage(0,Distance,ImgShadow);        Painter.setOpacity(1);  Painter.drawImage(Distance,0,SourceImage);         DstX=DstX-Distance;                          break;
        default : Painter.setOpacity(0.75); Painter.drawImage(Distance,Distance,ImgShadow); Painter.setOpacity(1);  Painter.drawImage(0,0,SourceImage);                                                             break;
    }

    Painter.end();
    return DestImage;
}

//*********************************************************************************************************************************************

cCompositionList::cCompositionList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionList:cCompositionList");

    TypeComposition=COMPOSITIONTYPE_BACKGROUND;
}

//====================================================================================================================

cCompositionList::~cCompositionList() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionList:~cCompositionList");

    while (List.count()>0) delete List.takeLast();
}

//====================================================================================================================

void cCompositionList::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionList:SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    // Save composition list
    Element.setAttribute("TypeComposition",TypeComposition);
    Element.setAttribute("CompositionNumber",List.count());
    for (int i=0;i<List.count();i++) List[i]->SaveToXML(Element,"Composition-"+QString("%1").arg(i),PathForRelativPath,ForceAbsolutPath);
    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cCompositionList::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,cCompositionList *ObjectComposition,QStringList *AliasList) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionList:LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        bool    IsOk=true;

        // Load composition list
        List.clear();
        TypeComposition=Element.attribute("TypeComposition").toInt();
        int CompositionNumber=Element.attribute("CompositionNumber").toInt();
        for (int i=0;i<CompositionNumber;i++) {
            cCompositionObject *CompositionObject=new cCompositionObject(TypeComposition,0,GlobalMainWindow->ApplicationConfig);    // IndexKey will be load from XML
            if (!CompositionObject->LoadFromXML(Element,"Composition-"+QString("%1").arg(i),PathForRelativPath,ObjectComposition,AliasList,true)) {
                //IsOk=false;
                delete CompositionObject;
            } else List.append(CompositionObject);
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaShot:cDiaporamaShot");

    Parent                          = DiaporamaObject;
    StaticDuration                  = GlobalMainWindow->ApplicationConfig->FixedDuration;    // Duration (in msec) of the static part animation
    ShotComposition.TypeComposition = COMPOSITIONTYPE_SHOT;
}

//====================================================================================================================

cDiaporamaShot::~cDiaporamaShot() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaShot:~cDiaporamaShot");

}

//===============================================================

void cDiaporamaShot::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaShot:SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    Element.setAttribute("StaticDuration",StaticDuration);                                      // Duration (in msec) of the static part animation
    ShotComposition.SaveToXML(Element,"ShotComposition",PathForRelativPath,ForceAbsolutPath);   // Composition list for this object
    domDocument.appendChild(Element);
}

//===============================================================

bool cDiaporamaShot::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,cCompositionList *ObjectComposition,QStringList *AliasList) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaShot:LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        StaticDuration=Element.attribute("StaticDuration").toInt();           // Duration (in msec) of the static part animation
        ShotComposition.LoadFromXML(Element,"ShotComposition",PathForRelativPath,ObjectComposition,AliasList);      // Composition list for this object
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:cDiaporamaObject");

    BackgroundBrush                         = new cBrushDefinition(Diaporama->ApplicationConfig,&BackgroundList);
    Parent                                  = Diaporama;
    TypeObject                              = DIAPORAMAOBJECTTYPE_EMPTY;
    SlideName                               = QApplication::translate("MainWindow","Title","Default slide name when no file");
    NextIndexKey                            = 1;

    // Set default/initial value
    StartNewChapter                         = false;                        // if true then start a new chapter from this slide
    BackgroundType                          = false;                        // Background type : false=same as precedent - true=new background definition
    BackgroundBrush->BrushType               = BRUSHTYPE_SOLID;
    BackgroundBrush->ColorD                  = "#000000";                    // Background color
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
    List.append(new cDiaporamaShot(this));
}

//====================================================================================================================

cDiaporamaObject::~cDiaporamaObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:~cDiaporamaObject");

    if (Thumbnail) {
        delete Thumbnail;
        Thumbnail=NULL;
    }
    if (BackgroundBrush) {
        delete BackgroundBrush;
        BackgroundBrush=NULL;
    }
    while (List.count()>0) delete List.takeLast();
}

//====================================================================================================================

QString cDiaporamaObject::GetDisplayName() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:GetDisplayName");

    return SlideName;
}

//===============================================================
// Draw Thumb
void cDiaporamaObject::DrawThumbnail(int ThumbWidth,int ThumbHeight,QPainter *Painter,int AddX,int AddY) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:DrawThumbnail");

    if ((!Thumbnail)||((Thumbnail)&&((Thumbnail->width()!=ThumbWidth)||(Thumbnail->height()!=ThumbHeight)))) {
        if (Thumbnail) {
            delete Thumbnail;
            Thumbnail=NULL;
        }
        Thumbnail = new QImage(ThumbWidth,ThumbHeight,QImage::Format_ARGB32_Premultiplied);
        QPainter  P;
        P.begin(Thumbnail);
        P.fillRect(0,0,ThumbWidth,ThumbHeight,Transparent);

        // Add static shot composition
        if (List.count()>0) for (int j=0;j<List[0]->ShotComposition.List.count();j++) {
            List[0]->ShotComposition.List[j]->DrawCompositionObject(&P,double(ThumbHeight)/1080,0,0,ThumbWidth,ThumbHeight,true,0,0,NULL,0,NULL,false,List[0]->StaticDuration,false);
        }

        P.end();
    }
    Painter->drawImage(AddX,AddY,*Thumbnail);
}

//===============================================================

qlonglong cDiaporamaObject::GetTransitDuration() {
    if ((TransitionFamilly==0)&&(TransitionSubType==0)) return 0; else return TransitionDuration;
}

qlonglong cDiaporamaObject::GetCumulTransitDuration() {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:GetCumulTransitDuration");   // Remove : Too much

    // Adjust duration to ensure transition will be full !
    int       ObjectIndex    =Parent->GetObjectIndex(this);
    qlonglong TransitDuration=GetTransitDuration();
    if (ObjectIndex<(Parent->List.count()-1)) TransitDuration+=Parent->List[ObjectIndex+1]->GetTransitDuration();
    return TransitDuration;
}

//===============================================================

qlonglong cDiaporamaObject::GetDuration() {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:GetDuration");       // Remove : Too much

    qlonglong Duration=0;
    for (int i=0;i<List.count();i++) Duration=Duration+List[i]->StaticDuration;

    // Adjust duration to ensure transition will be full !
    qlonglong TransitDuration=GetCumulTransitDuration();
    if (Duration<TransitDuration) Duration=TransitDuration;

    // Calc minimum duration to ensure all video will be full !
    int MaxMovieDuration=0;
    for (int Block=0;Block<ObjectComposition.List.count();Block++) if ((ObjectComposition.List[Block]->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(ObjectComposition.List[Block]->BackgroundBrush->Video)) {
        int IndexKey              =ObjectComposition.List[Block]->IndexKey;
        qlonglong WantedDuration  =ObjectComposition.List[Block]->BackgroundBrush->Video->StartPos.msecsTo(ObjectComposition.List[Block]->BackgroundBrush->Video->EndPos);
        qlonglong CurrentDuration =0;
        for (int i=0;i<List.count();i++) {
            for (int j=0;j<List[i]->ShotComposition.List.count();j++) if (List[i]->ShotComposition.List[j]->IndexKey==IndexKey) {
                if (List[i]->ShotComposition.List[j]->IsVisible) {
                    WantedDuration-=List[i]->StaticDuration;
                    if (WantedDuration<0) WantedDuration=0;
                }
            }
            CurrentDuration+=List[i]->StaticDuration;
        }
        CurrentDuration+=WantedDuration;
        if (MaxMovieDuration<CurrentDuration) MaxMovieDuration=CurrentDuration;
    }
    if (Duration<MaxMovieDuration)  Duration=MaxMovieDuration;
    if (Duration<33)                Duration=33;    // In all case minumum duration set to 1/30 sec

    return Duration;
}

//===============================================================

void cDiaporamaObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    // Slide properties
    Element.setAttribute("SlideName", SlideName);
    Element.setAttribute("StartNewChapter", StartNewChapter?"1":"0");
    Element.setAttribute("NextIndexKey",    NextIndexKey);

    // Background properties
    QDomElement SubElement=DomDocument.createElement("Background");
    SubElement.setAttribute("BackgroundType",BackgroundType?"1":"0");                                        // Background type : false=same as precedent - true=new background definition
    BackgroundBrush->SaveToXML(SubElement,"BackgroundBrush",PathForRelativPath,ForceAbsolutPath);             // Background brush
    BackgroundComposition.SaveToXML(SubElement,"BackgroundComposition",PathForRelativPath,ForceAbsolutPath); // Background composition
    Element.appendChild(SubElement);

    // Transition properties
    SubElement=DomDocument.createElement("Transition");
    SubElement.setAttribute("TransitionFamilly",TransitionFamilly);                         // Transition familly
    SubElement.setAttribute("TransitionSubType",TransitionSubType);                         // Transition type in the familly
    SubElement.setAttribute("TransitionDuration",TransitionDuration);                       // Transition duration (in msec)
    Element.appendChild(SubElement);

    // Music properties
    Element.setAttribute("MusicType",         MusicType?"1":"0");                           // Music type : false=same as precedent - true=new playlist definition
    Element.setAttribute("MusicPause",        MusicPause?"1":"0");                          // true if music is pause during this object
    Element.setAttribute("MusicReduceVolume", MusicReduceVolume?"1":"0");                   // true if volume if reduce by MusicReduceFactor
    Element.setAttribute("MusicReduceFactor",QString("%1").arg(MusicReduceFactor,0,'f'));   // factor for volume reduction if MusicReduceVolume is true
    Element.setAttribute("MusicNumber",       MusicList.count());                           // Number of file in the playlist
    for (int i=0;i<MusicList.count();i++) MusicList[i].SaveToXML(Element,"Music-"+QString("%1").arg(i),PathForRelativPath,ForceAbsolutPath);

    // Global blocks composition table
    ObjectComposition.SaveToXML(SubElement,"ObjectComposition",PathForRelativPath,ForceAbsolutPath);         // ObjectComposition

    // Shots definitions
    Element.setAttribute("ShotNumber",List.count());
    for (int i=0;i<List.count();i++) List[i]->SaveToXML(Element,"Shot-"+QString("%1").arg(i),PathForRelativPath,ForceAbsolutPath);

    if (Thumbnail) {
        QByteArray ba;
        QBuffer buf(&ba);
        Thumbnail->save(&buf,"PNG");
        QByteArray Compressed=qCompress(ba,1);
        QByteArray Hexed     =Compressed.toHex();
        Element.setAttribute("Thumbnail",QString(Hexed));
        Element.setAttribute("ThumbWidth",Thumbnail->width());
        Element.setAttribute("ThumbHeight",Thumbnail->height());
    }
    domDocument.appendChild(Element);
}

//===============================================================

bool cDiaporamaObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        bool IsOk=true;

        // Load shot list
        List.clear();   // Remove default 1st shot create by constructor

        // Slide properties
        SlideName=Element.attribute("SlideName");
        NextIndexKey=Element.attribute("NextIndexKey").toInt();
        if (Element.hasAttribute("StartNewChapter")) StartNewChapter=Element.attribute("StartNewChapter")=="1";

        // Background properties
        if ((Element.elementsByTagName("Background").length()>0)&&(Element.elementsByTagName("Background").item(0).isElement()==true)) {
            if (BackgroundBrush->Image) {
                delete BackgroundBrush->Image;
                BackgroundBrush->Image=NULL;
            }
            if (BackgroundBrush->Video) {
                delete BackgroundBrush->Video;
                BackgroundBrush->Video=NULL;
            }
            QDomElement SubElement=Element.elementsByTagName("Background").item(0).toElement();
            BackgroundType  =SubElement.attribute("BackgroundType")=="1"; // Background type : false=same as precedent - true=new background definition
            bool    ModifyFlag;
            if (!BackgroundBrush->LoadFromXML(SubElement,"BackgroundBrush",PathForRelativPath,AliasList,&ModifyFlag)) IsOk=false;
            if (IsOk && ModifyFlag) GlobalMainWindow->SetModifyFlag(true);
            // Background brush
            if ((!IsOk)||(!BackgroundComposition.LoadFromXML(SubElement,"BackgroundComposition",PathForRelativPath,NULL,AliasList))) IsOk=false;   // Background composition
            if (ModifyFlag) GlobalMainWindow->SetModifyFlag(true);
        }
        // Transition properties
        if ((Element.elementsByTagName("Transition").length()>0)&&(Element.elementsByTagName("Transition").item(0).isElement()==true)) {
            QDomElement SubElement=Element.elementsByTagName("Transition").item(0).toElement();
            TransitionFamilly =SubElement.attribute("TransitionFamilly").toInt();                                                   // Transition familly
            TransitionSubType =SubElement.attribute("TransitionSubType").toInt();                                                   // Transition type in the familly
            TransitionDuration=SubElement.attribute("TransitionDuration").toInt();                                                  // Transition duration (in msec)
            Element.appendChild(SubElement);
        }
        // Music properties
        MusicList.clear();
        MusicType         =Element.attribute("MusicType")=="1";                     // Music type : false=same as precedent - true=new playlist definition
        MusicPause        =Element.attribute("MusicPause")=="1";                    // true if music is pause during this object
        MusicReduceVolume =Element.attribute("MusicReduceVolume")=="1";             // true if volume if reduce by MusicReduceFactor
        MusicReduceFactor =Element.attribute("MusicReduceFactor").toDouble();       // factor for volume reduction if MusicReduceVolume is true
        int MusicNumber   =Element.attribute("MusicNumber").toInt();                // Number of file in the playlist
        for (int i=0;i<MusicNumber;i++) {
            cMusicObject *MusicObject=new cMusicObject(GlobalMainWindow->ApplicationConfig);
            bool ModifyFlag=false;
            if (!MusicObject->LoadFromXML(Element,"Music-"+QString("%1").arg(i),PathForRelativPath,AliasList,&ModifyFlag)) IsOk=false;
            MusicList.append(*MusicObject);
            if (ModifyFlag) GlobalMainWindow->SetModifyFlag(true);
        }

        // Global blocks composition table
        IsOk=ObjectComposition.LoadFromXML(Element,"ObjectComposition",PathForRelativPath,NULL,AliasList);         // ObjectComposition

        // Shots definitions
        int ShotNumber=Element.attribute("ShotNumber").toInt();
        for (int i=0;i<ShotNumber;i++) {
            cDiaporamaShot *imagesequence=new cDiaporamaShot(this);
            if (!imagesequence->LoadFromXML(Element,"Shot-"+QString("%1").arg(i),PathForRelativPath,&ObjectComposition,AliasList)) IsOk=false;
            List.append(imagesequence);
        }

        if (Element.hasAttribute("Thumbnail")) {
            int         ThumbWidth   =Element.attribute("ThumbWidth").toInt();
            int         ThumbHeight  =Element.attribute("ThumbHeight").toInt();
            Thumbnail=new QImage(ThumbWidth,ThumbHeight,QImage::Format_ARGB32_Premultiplied);
            QByteArray  Compressed   =QByteArray::fromHex(Element.attribute("Thumbnail").toUtf8());
            QByteArray  Decompressed =qUncompress(Compressed);
            Thumbnail->loadFromData(Decompressed);
        }

        //**** Compatibility with version prior to 1.5
        for (int i=0;i<ObjectComposition.List.count();i++) {
            if ((ObjectComposition.List.at(i)->BackgroundBrush->OnOffFilter!=0)||(ObjectComposition.List.at(i)->BackgroundBrush->GaussBlurSharpenSigma!=0)) {
                for (int j=0;j<List.count();j++) for (int k=0;k<List.at(j)->ShotComposition.List.count();k++) if (List.at(j)->ShotComposition.List.at(k)->IndexKey==ObjectComposition.List.at(i)->IndexKey) {
                    List.at(j)->ShotComposition.List.at(k)->BackgroundBrush->OnOffFilter=ObjectComposition.List.at(i)->BackgroundBrush->OnOffFilter;
                    List.at(j)->ShotComposition.List.at(k)->BackgroundBrush->GaussBlurSharpenSigma  =ObjectComposition.List.at(i)->BackgroundBrush->GaussBlurSharpenSigma;
                    List.at(j)->ShotComposition.List.at(k)->BackgroundBrush->BlurSharpenRadius      =ObjectComposition.List.at(i)->BackgroundBrush->BlurSharpenRadius;
                    List.at(j)->ShotComposition.List.at(k)->BackgroundBrush->TypeBlurSharpen        =ObjectComposition.List.at(i)->BackgroundBrush->TypeBlurSharpen;
                }
                ObjectComposition.List.at(i)->BackgroundBrush->OnOffFilter=0;
                ObjectComposition.List.at(i)->BackgroundBrush->GaussBlurSharpenSigma  =0;
                ObjectComposition.List.at(i)->BackgroundBrush->BlurSharpenRadius =5;
            }
        }

        return IsOk;
    } else return false;
}

//*********************************************************************************************************************************************
//
// Global class containing media objects
//
//*********************************************************************************************************************************************

cDiaporama::cDiaporama(cApplicationConfig *TheApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:cDiaporama");

    ApplicationConfig           = TheApplicationConfig;
    ProjectInfo                 = new cffDProjectFile(ApplicationConfig);
    CurrentCol                  = -1;                                                               // Current selected item
    CurrentPosition             = -1;                                                               // Current position (msec)
    IsModify                    = false;                                                            // true if project was modify
    ProjectFileName             = "";                                                               // Path and name of current file project
    ProjectInfo->Composer       = ApplicationConfig->ApplicationName+QString(" ")+ApplicationConfig->ApplicationVersion;
    ProjectInfo->Author         = ApplicationConfig->DefaultAuthor;
    ProjectInfo->DefaultLanguage= ApplicationConfig->DefaultLanguage;
    // Set default value
    DefineSizeAndGeometry(ApplicationConfig->ImageGeometry);                                // Default to 16:9
}

//====================================================================================================================

cDiaporama::~cDiaporama() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:~cDiaporama");

    if (ProjectInfo) {
        delete ProjectInfo;
        ProjectInfo=NULL;
    }
    while (List.count()>0) delete List.takeLast();
}

//====================================================================================================================

void cDiaporama::UpdateChapterInformation() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:UpdateChapterInformation");

    // Remove all chapters information
    int i=0;
    while (i<ProjectInfo->InformationList.count()) {
        if (((QString)ProjectInfo->InformationList[i]).startsWith("Chapter_")) ProjectInfo->InformationList.removeAt(i);
            else i++;
    }
    ProjectInfo->NbrChapters=0;
    // Create new
    for (int i=0;i<List.count();i++) if ((i==0)||(List[i]->StartNewChapter)) {
        QString ChapterNum=QString("%1").arg(ProjectInfo->NbrChapters++); while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
        int     NextChapter=i+1;
        qlonglong Start   =GetObjectStartPosition(i)+(i>0?List[i]->GetTransitDuration():0)-GetObjectStartPosition(0);
        qlonglong Duration=List[i]->GetDuration()-(i>0?List[i]->GetTransitDuration():0);
        while ((NextChapter<List.count())&&(!List[NextChapter]->StartNewChapter)) {
            Duration=Duration+List[NextChapter]->GetDuration();
            NextChapter++;
            if (NextChapter<List.count()) Duration=Duration-List[NextChapter]->GetTransitDuration();
        }
        ProjectInfo->InformationList.append("Chapter_"+ChapterNum+":InSlide" +QString("##")+QString("%1").arg(i+1));
        ProjectInfo->InformationList.append("Chapter_"+ChapterNum+":Start"   +QString("##")+QTime(0,0,0,0).addMSecs(Start).toString("hh:mm:ss.zzz"));
        ProjectInfo->InformationList.append("Chapter_"+ChapterNum+":End"     +QString("##")+QTime(0,0,0,0).addMSecs(Start+Duration).toString("hh:mm:ss.zzz"));
        ProjectInfo->InformationList.append("Chapter_"+ChapterNum+":Duration"+QString("##")+QTime(0,0,0,0).addMSecs(Duration).toString("hh:mm:ss.zzz"));
        ProjectInfo->InformationList.append("Chapter_"+ChapterNum+":title"+QString("##")+List[i]->SlideName);
    }
}

//====================================================================================================================

void cDiaporama::DefineSizeAndGeometry(int Geometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DefineSizeAndGeometry");

    ImageGeometry   =Geometry;
    InternalHeight  =PREVIEWMAXHEIGHT;
    InternalWidth   =GetWidthForHeight(InternalHeight);
    BackgroundList.ScanDisk("background",ImageGeometry);
    LumaList_Bar.SetGeometry(ImageGeometry);
    LumaList_Center.SetGeometry(ImageGeometry);
    LumaList_Checker.SetGeometry(ImageGeometry);
    LumaList_Clock.SetGeometry(ImageGeometry);
    LumaList_Box.SetGeometry(ImageGeometry);
    LumaList_Snake.SetGeometry(ImageGeometry);
    switch (Geometry) {
        case GEOMETRY_16_9:  ProjectInfo->ObjectGeometry=IMAGE_GEOMETRY_16_9;   break;
        case GEOMETRY_40_17: ProjectInfo->ObjectGeometry=IMAGE_GEOMETRY_40_17;  break;
        case GEOMETRY_4_3:
        default:             ProjectInfo->ObjectGeometry=IMAGE_GEOMETRY_4_3;    break;
    }
}

//=======================================================
// Return height for width depending on project geometry
//=======================================================

int cDiaporama::GetHeightForWidth(int WantedWith) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:GetHeightForWidth");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:GetWidthForHeight");

    switch (ImageGeometry) {
        case GEOMETRY_4_3 :     return int(double(4)*(double(WantedHeight)/double(3)));       break;
        case GEOMETRY_16_9 :    return int(double(16)*(double(WantedHeight)/double(9)));      break;
        case GEOMETRY_40_17 :   return int(double(40)*(double(WantedHeight)/double(17)));     break;
    }
    return 0;
}

//====================================================================================================================

qlonglong cDiaporama::GetTransitionDuration(int index) {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:GetTransitionDuration"); // Remove : too much

    qlonglong Duration=0;
    if ((index>=0)&&(List.count()>0)&&((index<List.count())&&(!((List[index]->TransitionFamilly==0)&&(List[index]->TransitionSubType==0))))) Duration=List[index]->TransitionDuration;
    return Duration;
}

//====================================================================================================================

qlonglong cDiaporama::GetDuration() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:GetDuration");

    qlonglong Duration=0;
    for (int i=0;i<List.count();i++) Duration=Duration+((List[i]->GetDuration()-GetTransitionDuration(i+1)>=33)?List[i]->GetDuration()-GetTransitionDuration(i+1):33);
    return Duration;
}

//====================================================================================================================

qlonglong cDiaporama::GetPartialDuration(int from,int to) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:GetPartialDuration");

    if (from<0)             from=0;
    if (from>=List.count()) from=List.count()-1;
    if (to<0)               to=0;
    if (to>=List.count())   to=List.count()-1;
    qlonglong Duration=0;
    for (int i=from;i<=to;i++) Duration=Duration+((List[i]->GetDuration()-GetTransitionDuration(i+1)>=33)?List[i]->GetDuration()-GetTransitionDuration(i+1):33);
    return Duration;
}

//====================================================================================================================

qlonglong cDiaporama::GetObjectStartPosition(int index) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:GetObjectStartPosition");

    qlonglong Duration=0;
    if ((index>=List.count())&&(List.count()>0)) {
        index=List.count()-1;
        Duration=List[index]->GetDuration();
    }
    for (int i=0;i<index;i++) Duration=Duration+((List[i]->GetDuration()-GetTransitionDuration(i+1))>=33?List[i]->GetDuration()-GetTransitionDuration(i+1):33);
    return Duration;
}

//====================================================================================================================

int cDiaporama::GetObjectIndex(cDiaporamaObject *ObjectToFind) {
    //ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:GetObjectIndex");          // Remove : Too much

    int i=0;
    while ((i<List.count())&&(ObjectToFind!=List[i])) i++;
    if (i<List.count()) return i; else return -1;
}

//====================================================================================================================

void cDiaporama::PrepareBackground(int Index,int Width,int Height,QPainter *Painter,int AddX,int AddY) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:PrepareBackground");

    // Background type : false=same as precedent - true=new background definition
    while ((Index>0)&&(!List[Index]->BackgroundType)) Index--;
    // Make painter translation to ensure QBrush image will start at AddX AddY position
    Painter->save();
    Painter->translate(AddX,AddY);
    Painter->fillRect(QRect(0,0,Width,Height),QBrush(Qt::black));
    if (List[Index]->BackgroundType) {
        QBrush *BR=List[Index]->BackgroundBrush->GetBrush(QRectF(0,0,Width,Height),true,0,0,NULL,1,NULL);
        Painter->fillRect(QRect(0,0,Width,Height),*BR);
        delete BR;
    }
    Painter->restore();
}

//====================================================================================================================

cMusicObject *cDiaporama::GetMusicObject(int ObjectIndex,qlonglong &StartPosition,int *CountObject,int *IndexObject) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:GetMusicObject");

    if (ObjectIndex>=List.count()) return NULL;

    cMusicObject *Ret =NULL;
    int          Index=ObjectIndex;

    StartPosition=0;

    // Music type : false=same as precedent - true=new playlist definition
    while ((Index>0)&&(!List[Index]->MusicType)) {
        if (!List[Index-1]->MusicPause) StartPosition+=((List[Index-1]->GetDuration()-GetTransitionDuration(Index))>=33?List[Index-1]->GetDuration()-GetTransitionDuration(Index):33);
        Index--;
    }

    // Now we have the object owner of the playlist (or 0). Then we can calculate wich music in the playlist is usable for this object
    int i=0;
    while ((i<List[Index]->MusicList.count())&&(StartPosition>QTime(0,0,0,0).msecsTo(List[Index]->MusicList[i].Duration))) {
        StartPosition-=QTime(0,0,0,0).msecsTo(List[Index]->MusicList[i].Duration);
        i++;
    }

    if ((i<List[Index]->MusicList.count())&&(StartPosition<=QTime(0,0,0,0).msecsTo(List[Index]->MusicList[i].Duration))) Ret=&List[Index]->MusicList[i];

    // Keep owner of the playlist (if wanted)
    if (IndexObject) *IndexObject=Index;

    // Calc object number in the playlist (if wanted)
    if (CountObject) {
        *CountObject=0;
        while (Index>0) {
            Index--;
            if (List[Index]->MusicType) *CountObject+=1;
        }
    }

    return Ret;
}

//====================================================================================================================

bool cDiaporama::SaveFile(QWidget *ParentWindow) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:SaveFile");

    QFile           file(ProjectFileName);
    QDomDocument    domDocument(APPLICATION_NAME);
    QDomElement     Element;
    QDomElement     root;

    UpdateChapterInformation();

    if ((ProjectInfo->Title=="")&&(ApplicationConfig->DefaultTitleFilling!=0)) {
        if (ApplicationConfig->DefaultTitleFilling==1) {
            // Fill with project name when project save (if not yet defined)
            ProjectInfo->Title=QFileInfo(ProjectFileName).completeBaseName();
        } else if (ApplicationConfig->DefaultTitleFilling==2) {
            // Fill with project folder when project save (if not yet defined)
            ProjectInfo->Title=ProjectFileName;
            if (ProjectInfo->Title!="") {
                ProjectInfo->Title=QFileInfo(ProjectInfo->Title).absolutePath();
                if (ProjectInfo->Title.endsWith(QDir::separator())) {
                    ProjectInfo->Title=QFileInfo(ProjectFileName).baseName();
                } else if (ProjectInfo->Title.lastIndexOf(QDir::separator())>0) ProjectInfo->Title=ProjectInfo->Title.mid(ProjectInfo->Title.lastIndexOf(QDir::separator())+1);
            }
        }
        if (ProjectInfo->Title.length()>30) ProjectInfo->Title=ProjectInfo->Title.left(30);
    }
    if (ProjectInfo->Author=="") ProjectInfo->Author=ApplicationConfig->DefaultAuthor;
    ProjectInfo->ffDRevision=CurrentAppVersion;

    // Create xml document and root
    root=domDocument.createElement(APPLICATION_ROOTNAME);
    domDocument.appendChild(root);

    // Save project properties
    ProjectInfo->SaveToXML(root);

    // Save basic information on project
    Element=domDocument.createElement("Project");
    Element.setAttribute("ImageGeometry",   ImageGeometry);

    // Save object list
    Element.setAttribute("ObjectNumber",List.count());
    for (int i=0;i<List.count();i++) List[i]->SaveToXML(root,"Object-"+(QString("%1").arg(i,10)).trimmed(),ProjectFileName,false);

    // Write file to disk
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        if (ParentWindow!=NULL) CustomMessageBox(NULL,QMessageBox::Critical,QApplication::translate("MainWindow","Error","Error message"),QApplication::translate("MainWindow","Error creating data file","Error message"),QMessageBox::Close);
            else                printf("%s\n",QApplication::translate("MainWindow","Error creating data file","Error message").toLocal8Bit().constData());
        return false;
    }
    root.appendChild(Element);

    // Save file now
    QTextStream out(&file);
    domDocument.save(out,4);
    file.close();
    return true;
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
// Note : Position is relative to the start of the Column object !
//============================================================================================
void cDiaporama::PrepareMusicBloc(bool PreviewMode,int Column,qlonglong Position,cSoundBlockList *MusicTrack) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:PrepareMusicBloc");

    if (Column>=List.count()) {
        for (int j=0;j<MusicTrack->NbrPacketForFPS;j++) MusicTrack->AppendNullSoundPacket();
        return;
    }

    qlonglong     StartPosition=0;
    cMusicObject  *CurMusic=GetMusicObject(Column,StartPosition); // Get current music file from column and position
    if (CurMusic==NULL) {
        for (int j=0;j<MusicTrack->NbrPacketForFPS;j++) MusicTrack->AppendNullSoundPacket();
        return;
    }


    bool IsCurrentTransitionIN =(Position<List[Column]->TransitionDuration);
    bool FadeEffect            =(IsCurrentTransitionIN && ((Column>0)&&(
                                    (List[Column-1]->MusicReduceVolume!=List[Column]->MusicReduceVolume)||
                                    (List[Column-1]->MusicPause!=List[Column]->MusicPause)||
                                    ((List[Column-1]->MusicReduceVolume==List[Column]->MusicReduceVolume)&&(List[Column-1]->MusicReduceFactor!=List[Column]->MusicReduceFactor))
                                )));

    if ((!IsCurrentTransitionIN && !List[Column]->MusicPause)||
        (IsCurrentTransitionIN && !List[Column]->MusicPause)||
        (IsCurrentTransitionIN && List[Column]->MusicPause && !List[Column-1]->MusicPause)
       ) {
        double Factor=CurMusic->Volume; // Master volume
        if (List[Column]->MusicReduceVolume || FadeEffect) {
            if (FadeEffect) {
                double PctDone=1;
                // calculate PctDone
                switch (GlobalMainWindow->ApplicationConfig->SpeedWave) {
                case SPEEDWAVE_LINEAR :
                    PctDone=double(Position)/double(List[Column]->TransitionDuration);
                    break;
                case SPEEDWAVE_SINQUARTER :
                    PctDone=double(Position)/double(List[Column]->TransitionDuration);
                    PctDone=sin(1.5708*PctDone);
                    break;
                }

                double  AncReduce=List[Column-1]->MusicPause?0:List[Column-1]->MusicReduceVolume?List[Column-1]->MusicReduceFactor:1;
                double  NewReduce=List[Column]->MusicPause?0:List[Column]->MusicReduceVolume?List[Column]->MusicReduceFactor:1;
                double  ReduceFactor=AncReduce+(NewReduce-AncReduce)*PctDone;
                Factor*=ReduceFactor;
            } else Factor=Factor*List[Column]->MusicReduceFactor;
        }

        // Get more music bloc at correct position (volume is always 100% @ this point !)
        CurMusic->ImageAt(PreviewMode,Position+StartPosition,0,MusicTrack,false,1,true,false);

        // Apply correct volume to block in queue
        if (Factor!=1.0) for (int i=0;i<MusicTrack->NbrPacketForFPS;i++) MusicTrack->ApplyVolume(i,Factor);
    }
    // Ensure we have enought data
    while (MusicTrack->List.count()<MusicTrack->NbrPacketForFPS) MusicTrack->AppendNullSoundPacket();
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
//  Extend=amout of padding (top and bottom) for cinema mode with DVD
//  IsCurrentObject : If true : prepare CurrentObject - If false : prepare Transition Object
//============================================================================================
void cDiaporama::PrepareImage(cDiaporamaObjectInfo *Info,int W,int H,bool IsCurrentObject,bool PreviewMode,bool AddStartPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:PrepareImage");

    bool                SoundOnly           =((W==0)&&(H==0));                     // W and H = 0 when producing sound track in render process
    qlonglong           Duration            =IsCurrentObject?Info->CurrentObject_ShotDuration:Info->TransitObject_ShotDuration;
    cDiaporamaShot      *CurShot            =IsCurrentObject?Info->CurrentObject_CurrentShot:Info->TransitObject_CurrentShot;
    cDiaporamaObject    *CurObject          =IsCurrentObject?Info->CurrentObject:Info->TransitObject;
    int                 CurTimePosition     =(IsCurrentObject?Info->CurrentObject_InObjectTime:Info->TransitObject_InObjectTime);
    double              PCTDone             =IsCurrentObject?Info->CurrentObject_PCTDone:Info->TransitObject_PCTDone;
    cSoundBlockList     *SoundTrackMontage  =(IsCurrentObject?Info->CurrentObject_SoundTrackMontage:Info->TransitObject_SoundTrackMontage);
    int                 ObjectNumber        =IsCurrentObject?Info->CurrentObject_Number:Info->TransitObject_Number;
    int                 ShotNumber          =IsCurrentObject?Info->CurrentObject_ShotSequenceNumber:Info->TransitObject_ShotSequenceNumber;
    cDiaporamaShot      *PreviousShot       =(ShotNumber>0?List[ObjectNumber]->List[ShotNumber-1]:NULL);
    QImage              *Image              =NULL;

    // Parse all shot objects to create SoundTrackMontage
    if (SoundOnly) {
        for (int j=0;j<CurShot->ShotComposition.List.count();j++) {
            if ((CurShot->ShotComposition.List[j]->BackgroundBrush->Video)&&(CurShot->ShotComposition.List[j]->BackgroundBrush->SoundVolume!=0)) {

                // Calc StartPosToAdd depending on AddStartPos
                qlonglong StartPosToAdd=(AddStartPos?QTime(0,0,0,0).msecsTo(CurShot->ShotComposition.List[j]->BackgroundBrush->Video->StartPos):0);

                // Calc VideoPosition depending on video set to pause (visible=off) in previous shot
                qlonglong VideoPosition=0;
                qlonglong ThePosition=0;
                int TheShot=0;
                while ((TheShot<CurObject->List.count())&&(ThePosition+CurObject->List[TheShot]->StaticDuration<CurTimePosition)) {
                    for (int w=0;w<CurObject->List[TheShot]->ShotComposition.List.count();w++) if (CurObject->List[TheShot]->ShotComposition.List[w]->IndexKey==CurShot->ShotComposition.List[j]->IndexKey) {
                        if (CurObject->List[TheShot]->ShotComposition.List[w]->IsVisible) VideoPosition+=CurObject->List[TheShot]->StaticDuration;
                        break;
                    }
                    ThePosition+=CurObject->List[TheShot]->StaticDuration;
                    TheShot++;
                }
                VideoPosition+=(CurTimePosition-ThePosition);

                CurShot->ShotComposition.List[j]->DrawCompositionObject(NULL,double(H)/double(1080),0,0,0,0,true,VideoPosition,StartPosToAdd,SoundTrackMontage,1,NULL,false,CurShot->StaticDuration,true);
            }
        }
        return;
    }

    // !SoundOnly : return immediatly if we have image
    if ((IsCurrentObject)&&(Info->CurrentObject_PreparedImage!=NULL)) return;

    // Special case for transition image of first slide
    if (!IsCurrentObject) {
        if (Info->TransitObject_PreparedImage!=NULL) return;    // return immediatly if we have image
        if (Info->TransitObject_SourceImage==NULL) {
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

    Image=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
    if ((!Image)||(Image->isNull())) {
        if (IsCurrentObject) Info->CurrentObject_PreparedImage=NULL; else Info->TransitObject_PreparedImage=NULL;
        return;
    }

    // Prepare a transparent image
    QPainter P;
    P.begin(Image);
    P.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
    P.setCompositionMode(QPainter::CompositionMode_Source);
    P.fillRect(0,0,W,H,Qt::transparent);
    P.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // Draw shot
    if (List.count()>0) for (int j=0;j<CurShot->ShotComposition.List.count();j++) {
        // Get PrevCompoObject to enable animation from previous shot
        cCompositionObject *PrevCompoObject=NULL;
        if (PreviousShot) {
            int k=0;
            while (k<PreviousShot->ShotComposition.List.count()) {
                if (PreviousShot->ShotComposition.List[k]->IndexKey==CurShot->ShotComposition.List[j]->IndexKey) {
                    PrevCompoObject=PreviousShot->ShotComposition.List[k];
                    k=PreviousShot->ShotComposition.List.count();
                } else k++;
            }
        }
        // Calc StartPosToAdd for video depending on AddStartPos
        qlonglong StartPosToAdd=((AddStartPos&&CurShot->ShotComposition.List[j]->BackgroundBrush->Video)?QTime(0,0,0,0).msecsTo(CurShot->ShotComposition.List[j]->BackgroundBrush->Video->StartPos):0);
        qlonglong VideoPosition=0;

        if (CurShot->ShotComposition.List[j]->BackgroundBrush->Video) {
            // Calc VideoPosition depending on video set to pause (visible=off) in previous shot
            int ThePosition=0;
            int TheShot=0;
            while ((TheShot<CurObject->List.count())&&(ThePosition+CurObject->List[TheShot]->StaticDuration<CurTimePosition)) {
                for (int w=0;w<CurObject->List[TheShot]->ShotComposition.List.count();w++) if (CurObject->List[TheShot]->ShotComposition.List[w]->IndexKey==CurShot->ShotComposition.List[j]->IndexKey) {
                    if (CurObject->List[TheShot]->ShotComposition.List[w]->IsVisible) VideoPosition+=CurObject->List[TheShot]->StaticDuration;
                    break;
                }
                ThePosition+=CurObject->List[TheShot]->StaticDuration;
                TheShot++;
            }
            VideoPosition+=(CurTimePosition-ThePosition);

        } else VideoPosition=CurTimePosition;

        // Draw object
        CurShot->ShotComposition.List[j]->DrawCompositionObject(&P,double(H)/double(1080),0,0,W,H,PreviewMode,VideoPosition,StartPosToAdd,SoundTrackMontage,PCTDone,PrevCompoObject,false,Duration,true);

    }
    P.end();

    if (IsCurrentObject) Info->CurrentObject_PreparedImage=Image; else Info->TransitObject_PreparedImage=Image;
}

//=============================================================================================================================
// Function use directly or with thread to make assembly of background and images and make mix (sound & music) when transition
//=============================================================================================================================
void cDiaporama::DoAssembly(cDiaporamaObjectInfo *Info,int W,int H) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DoAssembly");

    if (Info->RenderedImage!=NULL) return;    // return immediatly if we have image
    bool SoundOnly=((W==0)&&(H==0));

    // Prepare image
    if (!SoundOnly) {
        QImage  *Image=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
        QPainter P;
        P.begin(Image);
        P.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
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
        // Add prepared images and transition
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
            case TRANSITIONFAMILLY_DEFORM      : DoDeform(Info,&P,W,H);                 break;
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DoBasic");

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
    case 2:
        //P->setOpacity(1-Info->TransitionPCTDone);
        P->drawImage(0,0,*Info->TransitObject_PreparedImage);
        P->setOpacity(Info->TransitionPCTDone);
        P->drawImage(0,0,*Info->CurrentObject_PreparedImage);
        P->setOpacity(1);
        break;
    case 3:
        if (Info->TransitionPCTDone<0.5) {
            P->setOpacity(1-(Info->TransitionPCTDone)*2);
            P->drawImage(0,0,*Info->TransitObject_PreparedImage);
        } else {
            P->setOpacity((Info->TransitionPCTDone-0.5)*2);
            P->drawImage(0,0,*Info->CurrentObject_PreparedImage);
        }
        P->setOpacity(1);
        break;
    }
}

//============================================================================================

void cDiaporama::DoLuma(cLumaList *LumaList,cDiaporamaObjectInfo *Info,QPainter *P,int W,int H) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DoLuma");

    QImage  Img=Info->CurrentObject_PreparedImage->copy();
    if (Info->TransitionSubType<LumaList->List.count()) {
        // Get a copy of luma image scaled to correct size
        QImage  Luma=((W==LUMADLG_WIDTH)&&(H==LUMADLG_HEIGHT))?LumaList->List[Info->TransitionSubType].DlgLumaImage:
                        LumaList->List[Info->TransitionSubType].OriginalLuma.scaled(Info->CurrentObject_PreparedImage->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32_Premultiplied);

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DoZoom");

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
    P->drawImage(box,(Reverse?Info->TransitObject_PreparedImage:Info->CurrentObject_PreparedImage)->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
}

//============================================================================================

void cDiaporama::DoSlide(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DoSlide");

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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DoPush");

    QRect       box1,box2;
    QRect       box3,box4;
    QPoint      box;
    int         wt,ht;
    int         PCTW=int(Info->TransitionPCTDone*double(W));
    int         PCTH=int(Info->TransitionPCTDone*double(H));
    int         PCTWB=int((1-Info->TransitionPCTDone)*double(W));
    int         PCTHB=int((1-Info->TransitionPCTDone)*double(H));
    double      Rotate,dw,dh;
    QImage      Img;

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
        P->drawImage(box,Info->TransitObject_PreparedImage->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        wt=int(double(W)*Info->TransitionPCTDone);
        ht=int(double(H)*Info->TransitionPCTDone);
        box=QPoint(0,(H-ht)/2);
        P->drawImage(box,Info->CurrentObject_PreparedImage->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        break;
    case 5 :    // Enterring : zoom in from border Right Center - Previous image : zoom out to border Left Center
        wt=int(double(W)*(1-Info->TransitionPCTDone));
        ht=int(double(H)*(1-Info->TransitionPCTDone));
        box=QPoint(0,(H-ht)/2);
        P->drawImage(box,Info->TransitObject_PreparedImage->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        wt=int(double(W)*Info->TransitionPCTDone);
        ht=int(double(H)*Info->TransitionPCTDone);
        box=QPoint(W-wt,(H-ht)/2);
        P->drawImage(box,Info->CurrentObject_PreparedImage->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        break;
    case 6 :    // Enterring : zoom in from border Top Center - Previous image : zoom out to border bottom Center
        wt=int(double(W)*(1-Info->TransitionPCTDone));
        ht=int(double(H)*(1-Info->TransitionPCTDone));
        box=QPoint((W-wt)/2,H-ht);
        P->drawImage(box,Info->TransitObject_PreparedImage->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        wt=int(double(W)*Info->TransitionPCTDone);
        ht=int(double(H)*Info->TransitionPCTDone);
        box=QPoint((W-wt)/2,0);
        P->drawImage(box,Info->CurrentObject_PreparedImage->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        break;
    case 7 :    // Enterring : zoom in from border bottom Center - Previous image : zoom out to border Top Center
        wt=int(double(W)*(1-Info->TransitionPCTDone));
        ht=int(double(H)*(1-Info->TransitionPCTDone));
        box=QPoint((W-wt)/2,0);
        P->drawImage(box,Info->TransitObject_PreparedImage->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        wt=int(double(W)*Info->TransitionPCTDone);
        ht=int(double(H)*Info->TransitionPCTDone);
        box=QPoint((W-wt)/2,H-ht);
        P->drawImage(box,Info->CurrentObject_PreparedImage->scaled(QSize(wt,ht),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        break;
    case 8 :    // Rotating from y axis
        if (Info->TransitionPCTDone<0.5) {
            Rotate=double(90)*(Info->TransitionPCTDone*2);
            Img=RotateImage(0,Rotate,0,Info->TransitObject_PreparedImage);
        } else {
            Rotate=double(-90)*((1-Info->TransitionPCTDone)*2);
            Img=RotateImage(0,Rotate,0,Info->CurrentObject_PreparedImage);
        }
        dw=(double(W)-double(Img.width()))/2;
        dh=(double(H)-double(Img.height()))/2;
        P->drawImage(QPointF(dw,dh),Img);
        break;
    case 9 :    // Rotating from y axis
        if (Info->TransitionPCTDone<0.5) {
            Rotate=double(-90)*(Info->TransitionPCTDone*2);
            Img=RotateImage(0,Rotate,0,Info->TransitObject_PreparedImage);
        } else {
            Rotate=double(90)*((1-Info->TransitionPCTDone)*2);
            Img=RotateImage(0,Rotate,0,Info->CurrentObject_PreparedImage);
        }
        dw=(double(W)-double(Img.width()))/2;
        dh=(double(H)-double(Img.height()))/2;
        P->drawImage(QPointF(dw,dh),Img);
        break;
    case 10 :    // Rotating from x axis
        if (Info->TransitionPCTDone<0.5) {
            Rotate=double(90)*(Info->TransitionPCTDone*2);
            Img=RotateImage(Rotate,0,0,Info->TransitObject_PreparedImage);
        } else {
            Rotate=double(-90)*((1-Info->TransitionPCTDone)*2);
            Img=RotateImage(Rotate,0,0,Info->CurrentObject_PreparedImage);
        }
        dw=(double(W)-double(Img.width()))/2;
        dh=(double(H)-double(Img.height()))/2;
        P->drawImage(QPointF(dw,dh),Img);
        break;
    case 11 :    // Rotating from x axis
        if (Info->TransitionPCTDone<0.5) {
            Rotate=double(-90)*(Info->TransitionPCTDone*2);
            Img=RotateImage(Rotate,0,0,Info->TransitObject_PreparedImage);
        } else {
            Rotate=double(90)*((1-Info->TransitionPCTDone)*2);
            Img=RotateImage(Rotate,0,0,Info->CurrentObject_PreparedImage);
        }
        dw=(double(W)-double(Img.width()))/2;
        dh=(double(H)-double(Img.height()))/2;
        P->drawImage(QPointF(dw,dh),Img);
        break;
    case 12 :    // 1/2 Rotating from y axis (flip)
        dw=W/2;
        P->drawImage(QRectF(0,0,dw,H),*Info->TransitObject_PreparedImage,QRectF(0,0,dw,H));
        P->drawImage(QRectF(dw,0,dw,H),*Info->CurrentObject_PreparedImage,QRectF(dw,0,dw,H));
        if (Info->TransitionPCTDone<0.5) {
            Rotate=double(90)*(Info->TransitionPCTDone*2);
            Img=RotateImage(0,Rotate,0,Info->TransitObject_PreparedImage);
            dw=(double(W)-double(Img.width()))/2;
            dh=(double(H)-double(Img.height()))/2;
            P->drawImage(QRectF(W/2,dh,Img.width()/2,Img.height()),Img,QRectF(Img.width()/2,0,Img.width()/2,Img.height()));
        } else {
            Rotate=double(-90)*((1-Info->TransitionPCTDone)*2);
            Img=RotateImage(0,Rotate,0,Info->CurrentObject_PreparedImage);
            dw=(double(W)-double(Img.width()))/2;
            dh=(double(H)-double(Img.height()))/2;
            P->drawImage(QRectF(dw,dh,Img.width()/2,Img.height()),Img,QRectF(0,0,Img.width()/2,Img.height()));
        }
        break;
    case 13 :    // 1/2 Rotating from y axis (flip)
        dw=W/2;
        P->drawImage(QRectF(0,0,dw,H),*Info->CurrentObject_PreparedImage,QRectF(0,0,dw,H));
        P->drawImage(QRectF(dw,0,dw,H),*Info->TransitObject_PreparedImage,QRectF(dw,0,dw,H));
        if (Info->TransitionPCTDone<0.5) {
            Rotate=double(-90)*(Info->TransitionPCTDone*2);
            Img=RotateImage(0,Rotate,0,Info->TransitObject_PreparedImage);
            dw=(double(W)-double(Img.width()))/2;
            dh=(double(H)-double(Img.height()))/2;
            P->drawImage(QRectF(dw,dh,Img.width()/2,Img.height()),Img,QRectF(0,0,Img.width()/2,Img.height()));
        } else {
            Rotate=double(90)*((1-Info->TransitionPCTDone)*2);
            Img=RotateImage(0,Rotate,0,Info->CurrentObject_PreparedImage);
            dw=(double(W)-double(Img.width()))/2;
            dh=(double(H)-double(Img.height()))/2;
            P->drawImage(QRectF(W/2,dh,Img.width()/2,Img.height()),Img,QRectF(Img.width()/2,0,Img.width()/2,Img.height()));
        }
        break;
    case 14 :    // 1/2 Rotating from x axis (flip)
        dh=H/2;
        P->drawImage(QRectF(0,0,W,dh),*Info->TransitObject_PreparedImage,QRectF(0,0,W,dh));
        P->drawImage(QRectF(0,dh,W,dh),*Info->CurrentObject_PreparedImage,QRectF(0,dh,W,dh));
        if (Info->TransitionPCTDone<0.5) {
            Rotate=double(90)*(Info->TransitionPCTDone*2);
            Img=RotateImage(Rotate,0,0,Info->TransitObject_PreparedImage);
            dw=(double(W)-double(Img.width()))/2;
            dh=(double(H)-double(Img.height()))/2;
            P->drawImage(QRectF(dw,H/2,Img.width(),Img.height()/2),Img,QRectF(0,Img.height()/2,Img.width(),Img.height()/2));
        } else {
            Rotate=double(-90)*((1-Info->TransitionPCTDone)*2);
            Img=RotateImage(Rotate,0,0,Info->CurrentObject_PreparedImage);
            dw=(double(W)-double(Img.width()))/2;
            dh=(double(H)-double(Img.height()))/2;
            P->drawImage(QRectF(dw,dh,Img.width(),Img.height()/2),Img,QRectF(0,0,Img.width(),Img.height()/2));
        }
        break;
    case 15 :    // 1/2 Rotating from x axis (flip)
        dh=H/2;
        P->drawImage(QRectF(0,0,W,dh),*Info->CurrentObject_PreparedImage,QRectF(0,0,W,dh));
        P->drawImage(QRectF(0,dh,W,dh),*Info->TransitObject_PreparedImage,QRectF(0,dh,W,dh));
        if (Info->TransitionPCTDone<0.5) {
            Rotate=double(-90)*(Info->TransitionPCTDone*2);
            Img=RotateImage(Rotate,0,0,Info->TransitObject_PreparedImage);
            dw=(double(W)-double(Img.width()))/2;
            dh=(double(H)-double(Img.height()))/2;
            P->drawImage(QRectF(dw,dh,Img.width(),Img.height()/2),Img,QRectF(0,0,Img.width(),Img.height()/2));
        } else {
            Rotate=double(90)*((1-Info->TransitionPCTDone)*2);
            Img=RotateImage(Rotate,0,0,Info->CurrentObject_PreparedImage);
            dw=(double(W)-double(Img.width()))/2;
            dh=(double(H)-double(Img.height()))/2;
            P->drawImage(QRectF(dw,H/2,Img.width(),Img.height()/2),Img,QRectF(0,Img.height()/2,Img.width(),Img.height()/2));
        }
        break;
    }
}

//============================================================================================

void cDiaporama::DoDeform(cDiaporamaObjectInfo *Info,QPainter *P,int W,int H) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DoDeform");

    int         PCTW=int(Info->TransitionPCTDone*double(W));
    int         PCTH=int(Info->TransitionPCTDone*double(H));
    int         PCTWB=int((1-Info->TransitionPCTDone)*double(W));
    int         PCTHB=int((1-Info->TransitionPCTDone)*double(H));

    switch (Info->TransitionSubType) {
    case 0 :    // Since left to right
        P->drawImage(QRect(PCTW,0,W-PCTW,H),*Info->TransitObject_PreparedImage,QRect(0,0,W,H));
        P->drawImage(QRect(0,0,PCTW,H),*Info->CurrentObject_PreparedImage,QRect(0,0,W,H));
        break;
    case 1 :    // Since right to left
        P->drawImage(QRect(0,0,PCTWB,H),*Info->TransitObject_PreparedImage,QRect(0,0,W,H));
        P->drawImage(QRect(PCTWB,0,W-PCTWB,H),*Info->CurrentObject_PreparedImage,QRect(0,0,W,H));
        break;
    case 2 :    // Since up to down
        P->drawImage(QRect(0,PCTH,W,H-PCTH),*Info->TransitObject_PreparedImage,QRect(0,0,W,H));
        P->drawImage(QRect(0,0,W,PCTH),*Info->CurrentObject_PreparedImage,QRect(0,0,W,H));
        break;
    case 3 :    // Since down to up
        P->drawImage(QRect(0,0,W,PCTHB),*Info->TransitObject_PreparedImage,QRect(0,0,W,H));
        P->drawImage(QRect(0,PCTHB,W,H-PCTHB),*Info->CurrentObject_PreparedImage,QRect(0,0,W,H));
        break;
    }
}

//============================================================================================

QImage cDiaporama::RotateImage(double TheRotateXAxis,double TheRotateYAxis,double TheRotateZAxis,QImage *OldImg) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:RotateImage");

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
// Function use directly or with thread to load source image
// Assume SourceImage is null
// Produce sound only if W and H=0
//============================================================================================

void cDiaporama::LoadSources(cDiaporamaObjectInfo *Info,double ADJUST_RATIO,int W,int H,bool PreviewMode,bool AddStartPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:LoadSources");

    // W and H = 0 when producing sound track in render process
    bool SoundOnly=((W==0)&&(H==0));

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

        // if not PreviewMode then no need of music or sound

        // Load music bloc
        if ((PreviewMode || SoundOnly)&&(Info->CurrentObject)&&(Info->CurrentObject_MusicTrack)) {
            PrepareMusicBloc(PreviewMode,Info->CurrentObject_Number,Info->CurrentObject_InObjectTime,Info->CurrentObject_MusicTrack);
        }
        if ((PreviewMode || SoundOnly)&&(Info->TransitObject)&&(Info->TransitObject_MusicTrack)) {
            PrepareMusicBloc(PreviewMode,Info->TransitObject_Number,Info->TransitObject_InObjectTime,Info->TransitObject_MusicTrack);
        }
        // Transition Object if a previous was not keep !
        if (Info->TransitObject) LoadTransitVideoImage(Info,PreviewMode,W,H,AddStartPos);

        // Load Source image
        LoadSourceVideoImage(Info,PreviewMode,W,H,AddStartPos);

        //==============> Background part
        if (!SoundOnly) {
            // Search background context for CurrentObject if a previous was not keep !
            if (Info->CurrentObject_BackgroundBrush==NULL) {
                if ((Info->CurrentObject_BackgroundIndex>=List.count())||(List[Info->CurrentObject_BackgroundIndex]->BackgroundType==false))
                    Info->CurrentObject_BackgroundBrush=new QBrush(Qt::black);   // If no background definition @ first object
                    else Info->CurrentObject_BackgroundBrush=List[Info->CurrentObject_BackgroundIndex]->BackgroundBrush->GetBrush(QRectF(0,0,W,H),PreviewMode,0,0,NULL,1,NULL);
                // Create PreparedBackground
                Info->CurrentObject_PreparedBackground=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
                QPainter P;
                P.begin(Info->CurrentObject_PreparedBackground);
                P.fillRect(QRect(0,0,W,H),QBrush(Qt::black));
                if (Info->CurrentObject_BackgroundBrush) P.fillRect(QRect(0,0,W,H),*Info->CurrentObject_BackgroundBrush);
                // Apply composition to background
                for (int j=0;j<List[Info->CurrentObject_BackgroundIndex]->BackgroundComposition.List.count();j++)
                    List[Info->CurrentObject_BackgroundIndex]->BackgroundComposition.List[j]->DrawCompositionObject(&P,ADJUST_RATIO,0,0,W,H,PreviewMode,0,0,NULL,1,NULL,false,0,false);
                P.end();
            }
            // same job for Transition Object if a previous was not keep !
            if ((Info->TransitObject)&&(Info->TransitObject_BackgroundBrush==NULL)) {
                if ((Info->TransitObject_BackgroundIndex>=List.count())||(List[Info->TransitObject_BackgroundIndex]->BackgroundType==false))
                    Info->TransitObject_BackgroundBrush=new QBrush(Qt::black);   // If no background definition @ first object
                    else Info->TransitObject_BackgroundBrush=List[Info->TransitObject_BackgroundIndex]->BackgroundBrush->GetBrush(QRectF(0,0,W,H),PreviewMode,0,0,NULL,1,NULL);
                // Create PreparedBackground
                Info->TransitObject_PreparedBackground=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
                QPainter P;
                P.begin(Info->TransitObject_PreparedBackground);
                P.fillRect(QRect(0,0,W,H),QBrush(Qt::black));
                if (Info->TransitObject_BackgroundBrush) P.fillRect(QRect(0,0,W,H),*Info->TransitObject_BackgroundBrush);
                // Apply composition to background
                for (int j=0;j<List[Info->TransitObject_BackgroundIndex]->BackgroundComposition.List.count();j++)
                    List[Info->TransitObject_BackgroundIndex]->BackgroundComposition.List[j]->DrawCompositionObject(&P,ADJUST_RATIO,0,0,W,H,PreviewMode,0,0,NULL,1,NULL,false,0,false);
                P.end();
            }
        }
    }

    // Special case to clear music buffer if not transition and music of CurrentObject is in pause mode
    if ((!Info->IsTransition)&&(Info->CurrentObject)&&(Info->CurrentObject_MusicTrack)&&(Info->CurrentObject->MusicPause)&&(Info->CurrentObject_MusicTrack->List.count()>0)) {
        Info->CurrentObject_MusicTrack->ClearList();
        for (int i=0;i<Info->CurrentObject_MusicTrack->NbrPacketForFPS;i++) Info->CurrentObject_MusicTrack->AppendNullSoundPacket();
    }

    // Soundtrack mix with fade in/fade out
    if ((Info->IsTransition)&&((Info->CurrentObject_SoundTrackMontage)||(Info->TransitObject_SoundTrackMontage))) {

        if (Info->CurrentObject_SoundTrackMontage==NULL) {
            // if we don't have a CurrentObject_SoundTrackMontage, we need to create it because it's the destination !
            Info->CurrentObject_SoundTrackMontage=new cSDLSoundBlockList();
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

        // Mix current and transit SoundTrackMontage (if needed)
        // @ the end: only current SoundTrackMontage exist !
        for (int i=0;i<MaxPacket;i++) {
            // Mix the 2 sources buffer using the first buffer as destination and remove one paquet from the TransitObject_SoundTrackMontage
            int16_t *Paquet=Info->TransitObject_SoundTrackMontage?Info->TransitObject_SoundTrackMontage->DetachFirstPacket():NULL;
            int32_t mix;
            int16_t *Buf1=Info->CurrentObject_SoundTrackMontage->List[i];
            int     Max=Info->CurrentObject_SoundTrackMontage->SoundPacketSize/(Info->CurrentObject_SoundTrackMontage->SampleBytes*Info->CurrentObject_SoundTrackMontage->Channels);

            double  FadeAdjust   =sin(1.5708*double(Info->CurrentObject_InObjectTime+(double(i)/double(MaxPacket))*double(Info->FrameDuration))/double(Info->TransitionDuration));
            double  FadeAdjust2  =1-FadeAdjust;

            int16_t *Buf2=(Paquet!=NULL)?Paquet:NULL;
            if ((Buf1!=NULL)&&(Buf2==NULL)) {
                // Apply Fade in to Buf1
                for (int j=0;j<Max;j++) {
                    // Left channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf1))*FadeAdjust);
                    if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                    *(Buf1++)=int16_t(mix);
                    // Right channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf1))*FadeAdjust);
                    if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                    *(Buf1++)=int16_t(mix);
                }
            } else if ((Buf1!=NULL)&&(Buf2!=NULL)) {
                // do mixing
                for (int j=0;j<Max;j++) {
                    // Left channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf1))*FadeAdjust+double(*(Buf2++))*FadeAdjust2);
                    if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                    *(Buf1++)=int16_t(mix);
                    // Right channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf1))*FadeAdjust+double(*(Buf2++))*FadeAdjust2);
                    if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                    *(Buf1++)=int16_t(mix);
                }
                av_free(Paquet);
            } else if ((Buf1==NULL)&&(Buf2!=NULL)) {
                // swap buf1 and buf2
                Info->CurrentObject_SoundTrackMontage->List[i]=Buf2;
                // Apply Fade out to Buf2
                for (int j=0;j<Max;j++) {
                    // Left channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf2))*FadeAdjust2);
                    if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                    *(Buf2++)=int16_t(mix);
                    // Right channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf2))*FadeAdjust2);
                    if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                    *(Buf2++)=int16_t(mix);
                }
            }
        }
    }

    // Mix current and transit music
    // @ the end: only current music exist !
    // add fade out of previous music track (if needed)
    if ((Info->IsTransition)&&(Info->TransitObject_MusicTrack)&&(Info->TransitObject_MusicTrack->List.count()>0)) {
        int MaxPacket=0;
        MaxPacket=Info->CurrentObject_MusicTrack->List.count();
        if ((MaxPacket>Info->TransitObject_MusicTrack->List.count())) MaxPacket=Info->TransitObject_MusicTrack->List.count();
        if (MaxPacket>Info->CurrentObject_MusicTrack->NbrPacketForFPS) MaxPacket=Info->CurrentObject_MusicTrack->NbrPacketForFPS;

        // Mix the 2 sources buffer using the first buffer as destination and remove one paquet from the PreviousMusicTrack
        for (int i=0;i<MaxPacket;i++) {
            SDL_LockAudio();
            int16_t *Paquet=Info->TransitObject_MusicTrack->DetachFirstPacket();
            SDL_UnlockAudio();
            int32_t mix;
            int16_t *Buf1=Info->CurrentObject_MusicTrack->List[i];
            int     Max=Info->CurrentObject_MusicTrack->SoundPacketSize/(Info->CurrentObject_MusicTrack->SampleBytes*Info->CurrentObject_MusicTrack->Channels);
            double  FadeAdjust   =sin(1.5708*double(Info->CurrentObject_InObjectTime+(double(i)/double(MaxPacket))*double(Info->FrameDuration))/double(Info->TransitionDuration));
            double  FadeAdjust2  =1-FadeAdjust;

            int16_t *Buf2=(Paquet!=NULL)?Paquet:NULL;
            if ((Buf1!=NULL)&&(Buf2==NULL)) {
                // Nothing to do !
            } else if ((Buf1!=NULL)&&(Buf2!=NULL)) {
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
            } else if ((Buf1==NULL)&&(Buf2!=NULL)) {
                // swap buf1 and buf2
                Info->CurrentObject_MusicTrack->List[i]=Buf2;
                // Apply Fade to Buf2
                for (int j=0;j<Max;j++) {
                    // Left channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf2))*FadeAdjust2);
                    if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                    *(Buf2++)=int16_t(mix);
                    // Right channel : Adjust if necessary (16 bits)
                    mix=int32_t(double(*(Buf2))*FadeAdjust2);
                    if (mix>32767)  mix=32767; else if (mix<-32768) mix=-32768;
                    *(Buf2++)=int16_t(mix);
                }
            }
        }
    }
}

//============================================================================================

void cDiaporama::LoadSourceVideoImage(cDiaporamaObjectInfo *Info,bool PreviewMode,int W,int H,bool AddStartPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:LoadSourceVideoImage");

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
    // Prepare images for Current Object
    PrepareImage(Info,W,H,true,PreviewMode,AddStartPos);

}

//============================================================================================

void cDiaporama::LoadTransitVideoImage(cDiaporamaObjectInfo *Info,bool PreviewMode,int W,int H,bool AddStartPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:LoadTransitVideoImage");

    // W and H = 0 when producing sound track in render process
    if ((W!=0)&&(H!=0)) {
        // create an empty transparent image
        Info->TransitObject_SourceImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
        QPainter PT;
        PT.begin(Info->TransitObject_SourceImage);
        PT.setCompositionMode(QPainter::CompositionMode_Source);
        PT.fillRect(QRect(0,0,W,H),Qt::transparent);
        PT.setCompositionMode(QPainter::CompositionMode_SourceOver);
        PT.end();
    } else {
        // Create a very small image to have a ptr
        Info->TransitObject_SourceImage=new QImage(5,5,QImage::Format_ARGB32_Premultiplied);
    }
    // Prepare images for Transit Object
    PrepareImage(Info,W,H,false,PreviewMode,AddStartPos);
}


//============================================================================================
void cDiaporama::CloseUnusedLibAv(int CurrentCell) {
    // Parse all unused slide to close unused libav buffer, codec, ...
    for (int i=0;i<List.count();i++) {
        for (int j=0;j<List[i]->ObjectComposition.List.count();j++) if (List[i]->ObjectComposition.List[j]->BackgroundBrush->Video!=NULL) {
            while (List[i]->ObjectComposition.List[j]->BackgroundBrush->Video->CacheImage.count()>MAXCACHEIMAGE) List[i]->ObjectComposition.List[j]->BackgroundBrush->Video->CacheImage.removeFirst();
        }
        if ((i<CurrentCell-1)||(i>CurrentCell+1)) {
            for (int j=0;j<List[i]->ObjectComposition.List.count();j++)
                if ((List[i]->ObjectComposition.List[j]->BackgroundBrush->Video!=NULL)&&(List[i]->ObjectComposition.List[j]->BackgroundBrush->Video->ffmpegVideoFile!=NULL))
                    List[i]->ObjectComposition.List[j]->BackgroundBrush->Video->CloseCodecAndFile();
        }
    }
}
//*********************************************************************************************************************************************
// Class object for rendering
//*********************************************************************************************************************************************

// make a copy of PreviousFrame
cDiaporamaObjectInfo::cDiaporamaObjectInfo(cDiaporamaObjectInfo *PreviousFrame) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObjectInfo:cDiaporamaObjectInfo from PreviousFrame");

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
    CurrentObject_ShotDuration         =PreviousFrame->CurrentObject_ShotDuration;        // Time the static shot end (if CurrentObject_CurrentShotType=SHOTTYPE_STATIC)
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
    TransitObject_ShotDuration         =PreviousFrame->TransitObject_ShotDuration;        // Time the static shot end (if TransitObject_CurrentShotType=SHOTTYPE_STATIC)
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
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObjectInfo:cDiaporamaObjectInfo");

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
    CurrentObject_ShotDuration         =0;                 // Time the static shot end (if CurrentObject_CurrentShotType=SHOTTYPE_STATIC)
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
    TransitObject_ShotDuration         =0;                 // Time the static shot end (if TransitObject_CurrentShotType=SHOTTYPE_STATIC)
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

    if (!Diaporama) {
        CurrentObject_InObjectTime=TimePosition;
        TransitObject_InObjectTime=TimePosition;
    } else {
        //==============> Retrieve object information depending on position (in msec)
        // Search wich object for given time position
        int AdjustedDuration=(CurrentObject_Number<Diaporama->List.count())?Diaporama->List[CurrentObject_Number]->GetDuration()-Diaporama->GetTransitionDuration(CurrentObject_Number+1):0;
        if (AdjustedDuration<33) AdjustedDuration=33; // Not less than 1/30 sec
        while ((CurrentObject_Number<Diaporama->List.count())&&
               ((CurrentObject_StartTime+AdjustedDuration<=TimePosition))) {

            CurrentObject_StartTime=CurrentObject_StartTime+AdjustedDuration;
            CurrentObject_Number++;
            AdjustedDuration=(CurrentObject_Number<Diaporama->List.count())?Diaporama->List[CurrentObject_Number]->GetDuration()-Diaporama->GetTransitionDuration(CurrentObject_Number+1):0;
            if (AdjustedDuration<33) AdjustedDuration=33;   // Not less than 1/30 sec
        }
        // Adjust CurrentObject_Number
        if (CurrentObject_Number>=Diaporama->List.count()) {
            if (Diaporama->List.count()>0) {   // Force to last object
                CurrentObject_Number    =Diaporama->List.count()-1;
                CurrentObject           =Diaporama->List[CurrentObject_Number];
                CurrentObject_StartTime =Diaporama->GetObjectStartPosition(CurrentObject_Number);
            } else {                // Force to first or none object
                CurrentObject_Number   =0;
                CurrentObject_StartTime=0;
                CurrentObject=NULL;
            }
        } else CurrentObject=Diaporama->List[CurrentObject_Number];
        CurrentObject_InObjectTime=TimePosition-CurrentObject_StartTime;

        // Now calculate wich sequence in the current object is
        if (CurrentObject) {
            int CurPos  =0;
            while ((CurrentObject_ShotSequenceNumber<CurrentObject->List.count()-1)&&((CurPos+CurrentObject->List[CurrentObject_ShotSequenceNumber]->StaticDuration)<=CurrentObject_InObjectTime)) {
                CurPos=CurPos+CurrentObject->List[CurrentObject_ShotSequenceNumber]->StaticDuration;
                CurrentObject_ShotSequenceNumber++;
            }
            CurrentObject_CurrentShot=CurrentObject->List[CurrentObject_ShotSequenceNumber];
            if (CurrentObject_ShotSequenceNumber<CurrentObject->List.count()-1) CurrentObject_ShotDuration=CurrentObject_CurrentShot->StaticDuration;
                else CurrentObject_ShotDuration=CurrentObject_CurrentShot->Parent->GetDuration()-CurPos;

            // calculate CurrentObject_PCTDone
            switch (GlobalMainWindow->ApplicationConfig->SpeedWave) {
            case SPEEDWAVE_LINEAR :
                CurrentObject_PCTDone=(double(CurrentObject_InObjectTime)-double(CurPos))/(double(CurrentObject_ShotDuration));
                break;
            case SPEEDWAVE_SINQUARTER :
                CurrentObject_PCTDone=(double(CurrentObject_InObjectTime)-double(CurPos))/(double(CurrentObject_ShotDuration));
                CurrentObject_PCTDone=sin(1.5708*CurrentObject_PCTDone);
                break;
            }

            // Force all to SHOTTYPE_VIDEO
            CurrentObject_CurrentShotType=SHOTTYPE_VIDEO;

        } else {
            CurrentObject_ShotSequenceNumber=0;
            CurrentObject_CurrentShot       =NULL;
            CurrentObject_CurrentShotType   =SHOTTYPE_STATIC;
        }

        // Calculate wich BackgroundIndex to be use (Background type : false=same as precedent - true=new background definition)
        CurrentObject_BackgroundIndex=CurrentObject_Number;
        while ((CurrentObject_BackgroundIndex>0)&&(!Diaporama->List[CurrentObject_BackgroundIndex]->BackgroundType)) CurrentObject_BackgroundIndex--;

        // Define if for this position we have a transition with a previous object
        if ((CurrentObject!=NULL)&&((CurrentObject->TransitionFamilly!=0)||(CurrentObject->TransitionSubType!=0))&&(CurrentObject_InObjectTime<CurrentObject->TransitionDuration)) {
            TransitionFamilly =CurrentObject->TransitionFamilly;                      // Transition familly
            TransitionSubType =CurrentObject->TransitionSubType;                      // Transition type in the familly
            TransitionDuration=CurrentObject->TransitionDuration;                     // Transition duration (in msec)
            IsTransition      =true;
            switch (GlobalMainWindow->ApplicationConfig->SpeedWave) {
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
                TransitObject               =Diaporama->List[TransitObject_Number];
                TransitObject_StartTime     =Diaporama->GetObjectStartPosition(TransitObject_Number);
                TransitObject_InObjectTime  =TimePosition-TransitObject_StartTime;
                // Now calculate wich sequence in the Transition object is
                int CurPos  =0;
                while ((TransitObject_ShotSequenceNumber<TransitObject->List.count()-1)&&((CurPos+TransitObject->List[TransitObject_ShotSequenceNumber]->StaticDuration)<=TransitObject_InObjectTime)) {
                    CurPos=CurPos+TransitObject->List[TransitObject_ShotSequenceNumber]->StaticDuration;
                    TransitObject_ShotSequenceNumber++;
                }
                TransitObject_CurrentShot=TransitObject->List[TransitObject_ShotSequenceNumber];
                if (TransitObject_ShotSequenceNumber<TransitObject->List.count()-1) TransitObject_ShotDuration=TransitObject_CurrentShot->StaticDuration;
                    else TransitObject_ShotDuration=TransitObject_CurrentShot->Parent->GetDuration()-CurPos;

                TransitObject_CurrentShotType=SHOTTYPE_VIDEO;

                // calculate TransitObject_PCTDone
                switch (GlobalMainWindow->ApplicationConfig->SpeedWave) {
                case SPEEDWAVE_LINEAR :
                    TransitObject_PCTDone=(double(TransitObject_InObjectTime)-double(CurPos))/(double(TransitObject_ShotDuration));
                    break;
                case SPEEDWAVE_SINQUARTER :
                    TransitObject_PCTDone=(double(TransitObject_InObjectTime)-double(CurPos))/(double(TransitObject_ShotDuration));
                    TransitObject_PCTDone=sin(1.5708*TransitObject_PCTDone);
                    break;
                }

                // Force all to SHOTTYPE_VIDEO
                // Calculate wich BackgroundIndex to be use for transition object (Background type : false=same as precedent - true=new background definition)
                TransitObject_BackgroundIndex=TransitObject_Number;
                while ((TransitObject_BackgroundIndex>0)&&(!Diaporama->List[TransitObject_BackgroundIndex]->BackgroundType)) TransitObject_BackgroundIndex--;
            }
        }

        // Search music objects
        qlonglong StartPosition;
        if (CurrentObject!=NULL) CurrentObject_MusicObject=Diaporama->GetMusicObject(CurrentObject_Number,StartPosition);
        if (TransitObject!=NULL) TransitObject_MusicObject=Diaporama->GetMusicObject(TransitObject_Number,StartPosition);

        //==============> Try to re-use values from PreviousFrame
        if ((CurrentObject)&&(PreviousFrame)) {
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
            // Definitively check PreviousFrame to know if we realy need to free Background
            if (PreviousFrame->CurrentObject_FreePreparedBackground && ((PreviousFrame->CurrentObject_PreparedBackground==CurrentObject_PreparedBackground)||(PreviousFrame->CurrentObject_PreparedBackground==TransitObject_PreparedBackground)))
                PreviousFrame->CurrentObject_FreePreparedBackground=false;
            if (PreviousFrame->TransitObject_FreePreparedBackground && ((PreviousFrame->TransitObject_PreparedBackground==CurrentObject_PreparedBackground)||(PreviousFrame->TransitObject_PreparedBackground==TransitObject_PreparedBackground)||(PreviousFrame->TransitObject_PreparedBackground==PreviousFrame->CurrentObject_PreparedBackground)))
                PreviousFrame->TransitObject_FreePreparedBackground=false;

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
            // Definitively check PreviousFrame to know if we realy need to free MusicObject
            if (PreviousFrame->CurrentObject_FreeMusicTrack && ((PreviousFrame->CurrentObject_MusicTrack==CurrentObject_MusicTrack)||(PreviousFrame->CurrentObject_MusicTrack==TransitObject_MusicTrack)))
                PreviousFrame->CurrentObject_FreeMusicTrack=false;
            if (PreviousFrame->TransitObject_FreeMusicTrack && ((PreviousFrame->TransitObject_MusicTrack==CurrentObject_MusicTrack)||(PreviousFrame->TransitObject_MusicTrack==TransitObject_MusicTrack)||(PreviousFrame->TransitObject_MusicTrack==PreviousFrame->CurrentObject_MusicTrack)))
                PreviousFrame->TransitObject_FreeMusicTrack=false;

            //************ PreparedImage & RenderedImage
            if ((PreviousFrame->CurrentObject_CurrentShot==CurrentObject_CurrentShot)&&                 // Same shot
                (IsShotStatic(CurrentObject,CurrentObject_ShotSequenceNumber))) {
                CurrentObject_PreparedImage=PreviousFrame->CurrentObject_PreparedImage;                 // Use the same PreparedImage
                PreviousFrame->CurrentObject_FreePreparedImage=false;                                   // Set tag to not delete previous PreparedImage
                if ((!IsTransition)&&(!PreviousFrame->IsTransition)) {
                    RenderedImage=PreviousFrame->RenderedImage;   // Use the same RenderedImage
                    PreviousFrame->FreeRenderedImage=false;       // Set tag to not delete previous RenderedImage
                }
            }
            // PreparedImage of transition Object
            if (TransitObject) {
                if ((PreviousFrame->CurrentObject_CurrentShot==TransitObject_CurrentShot)&&             // Same shot
                    (IsShotStatic(TransitObject,TransitObject_ShotSequenceNumber))) {
                    TransitObject_PreparedImage=PreviousFrame->CurrentObject_PreparedImage;             // Use the same PreparedImage
                    PreviousFrame->CurrentObject_FreePreparedImage=false;                               // Set tag to not delete previous PreparedImage
                } else if ((PreviousFrame->TransitObject_CurrentShot==TransitObject_CurrentShot)&&      // Same shot
                    (IsShotStatic(TransitObject,TransitObject_ShotSequenceNumber))) {
                    TransitObject_PreparedImage=PreviousFrame->TransitObject_PreparedImage;             // Use the same PreparedImage
                    PreviousFrame->TransitObject_FreePreparedImage=false;                               // Set tag to not delete previous PreparedImage
                }
            }
        }
    }
}

bool cDiaporamaObjectInfo::IsShotStatic(cDiaporamaObject *Object,int ShotNumber) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObjectInfo:IsShotStatic");

    bool IsStatic=true;
    if (ShotNumber==0) {
        for (int i=0;i<Object->List[0]->ShotComposition.List.count();i++)
            if ((Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Video!=NULL)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BlockAnimType!=0))
                IsStatic=false;
    } else for (int i=0;i<Object->List[ShotNumber]->ShotComposition.List.count();i++) if (Object->List[ShotNumber]->ShotComposition.List[i]->IsVisible) {
        if (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Video!=NULL) IsStatic=false; else {
            if ((Object->List[ShotNumber]->ShotComposition.List[i]->x                                       !=Object->List[ShotNumber-1]->ShotComposition.List[i]->x)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->y                                       !=Object->List[ShotNumber-1]->ShotComposition.List[i]->y)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->w                                       !=Object->List[ShotNumber-1]->ShotComposition.List[i]->w)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->h                                       !=Object->List[ShotNumber-1]->ShotComposition.List[i]->h)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->RotateXAxis                             !=Object->List[ShotNumber-1]->ShotComposition.List[i]->RotateXAxis)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->RotateYAxis                             !=Object->List[ShotNumber-1]->ShotComposition.List[i]->RotateYAxis)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->RotateZAxis                             !=Object->List[ShotNumber-1]->ShotComposition.List[i]->RotateZAxis)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BlockAnimType                           !=0)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->TxtZoomLevel                            !=Object->List[ShotNumber-1]->ShotComposition.List[i]->TxtZoomLevel)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->TxtScrollX                              !=Object->List[ShotNumber-1]->ShotComposition.List[i]->TxtScrollX)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->TxtScrollY                              !=Object->List[ShotNumber-1]->ShotComposition.List[i]->TxtScrollY)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->X                      !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->X)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Y                      !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Y)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->ZoomFactor             !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->ZoomFactor)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->AspectRatio            !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->AspectRatio)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->ImageRotation          !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->ImageRotation)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Blue                   !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Blue)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Red                    !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Red)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Green                  !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Green)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Brightness             !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Brightness)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Contrast               !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Contrast)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Gamma                  !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Gamma)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->BlurSharpenRadius      !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->BlurSharpenRadius)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->GaussBlurSharpenSigma  !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->GaussBlurSharpenSigma)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->QuickBlurSharpenSigma  !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->QuickBlurSharpenSigma)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Desat                  !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Desat)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Swirl                  !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Swirl)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->Implode                !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->Implode)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->WaveAmp                !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->WaveAmp)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->WaveFreq               !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->WaveFreq)||
                (Object->List[ShotNumber]->ShotComposition.List[i]->BackgroundBrush->OnOffFilter            !=Object->List[ShotNumber-1]->ShotComposition.List[i]->BackgroundBrush->OnOffFilter))
                IsStatic=false;
        }
    }
    return IsStatic;
}

//============================================================================================
// Destructor
//============================================================================================
cDiaporamaObjectInfo::~cDiaporamaObjectInfo() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObjectInfo:~cDiaporamaObjectInfo");

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
