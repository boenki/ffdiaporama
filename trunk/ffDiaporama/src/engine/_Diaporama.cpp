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

// Specific inclusions
#include "_Diaporama.h"
#include "../CustomCtrl/_QCustomDialog.h"
#include "../ffDiaporama/mainwindow.h"

#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextBlockFormat>

int64_t TotalLoadSources=0,TotalAssembly=0,TotalLoadSound=0;

// Composition parameters
#define SCALINGTEXTFACTOR                   700     // 700 instead of 400 (ffD 1.0/1.1/1.2) to keep similar display from plaintext to richtext

cCompositionObjectContext::cCompositionObjectContext(int ObjectNumber,bool PreviewMode,bool IsCurrentObject,cDiaporamaObjectInfo *Info,double width,double height,
                                                     cDiaporamaShot *CurShot,cDiaporamaShot *PreviousShot,cSoundBlockList *SoundTrackMontage,bool AddStartPos,
                                                     qlonglong ShotDuration) {
    this->NeedPreparedBrush =false;
    this->PrevCompoObject   =NULL;
    this->width             =width;
    this->height            =height;
    this->PreviewMode       =PreviewMode;
    this->UseBrushCache     =false;
    this->Transfo           =false;
    this->EnableAnimation   =true;
    this->NewX              =0;
    this->NewY              =0;
    this->NewW              =0;
    this->NewH              =0;
    this->ShotDuration      =ShotDuration;
    this->IsCurrentObject   =IsCurrentObject;
    this->CurShot           =CurShot;
    this->PreviousShot      =PreviousShot;
    this->Info              =Info;
    this->ObjectNumber      =ObjectNumber;
    this->SoundTrackMontage =SoundTrackMontage;
    this->AddStartPos       =AddStartPos;
}

//====================================================================================================================
// Compute
//  BlockPctDone, ImagePctDone
//  VideoPosition, StartPosToAdd, PrevCompoObject
//====================================================================================================================

void cCompositionObjectContext::Compute() {
    cDiaporamaObject    *CurObject      =IsCurrentObject?Info->CurrentObject:Info->TransitObject;
    int                 CurTimePosition =(IsCurrentObject?Info->CurrentObject_InObjectTime:Info->TransitObject_InObjectTime);
    int                 BlockSpeedWave  =CurShot->ShotComposition.List[ObjectNumber]->BlockSpeedWave;
    int                 ImageSpeedWave  =CurShot->ShotComposition.List[ObjectNumber]->BackgroundBrush->ImageSpeedWave;

    if (BlockSpeedWave==SPEEDWAVE_PROJECTDEFAULT) BlockSpeedWave=CurShot->Parent->Parent->BlockAnimSpeedWave;
    if (ImageSpeedWave==SPEEDWAVE_PROJECTDEFAULT) ImageSpeedWave=CurShot->Parent->Parent->ImageAnimSpeedWave;

    Object      =CurShot->ShotComposition.List[ObjectNumber];
    BlockPctDone=ComputePCT(BlockSpeedWave,IsCurrentObject?Info->CurrentObject_PCTDone:Info->TransitObject_PCTDone);;
    ImagePctDone=ComputePCT(ImageSpeedWave,IsCurrentObject?Info->CurrentObject_PCTDone:Info->TransitObject_PCTDone);;

    // Get PrevCompoObject to enable animation from previous shot
    if (PreviousShot) {
        int k=0;
        while (k<PreviousShot->ShotComposition.List.count()) {
            if (PreviousShot->ShotComposition.List[k]->IndexKey==CurShot->ShotComposition.List[ObjectNumber]->IndexKey) {
                PrevCompoObject=PreviousShot->ShotComposition.List[k];
                k=PreviousShot->ShotComposition.List.count();
            } else k++;
        }
    }
    // Calc StartPosToAdd for video depending on AddStartPos
    StartPosToAdd=((AddStartPos&&CurShot->ShotComposition.List[ObjectNumber]->BackgroundBrush->Video)?QTime(0,0,0,0).msecsTo(CurShot->ShotComposition.List[ObjectNumber]->BackgroundBrush->Video->StartPos):0);
    VideoPosition=0;

    if (CurShot->ShotComposition.List[ObjectNumber]->BackgroundBrush->Video) {
        // Calc VideoPosition depending on video set to pause (visible=off) in previous shot
        int ThePosition=0;
        int TheShot=0;
        while ((TheShot<CurObject->List.count())&&(ThePosition+CurObject->List[TheShot]->StaticDuration<CurTimePosition)) {
            for (int w=0;w<CurObject->List[TheShot]->ShotComposition.List.count();w++) if (CurObject->List[TheShot]->ShotComposition.List[w]->IndexKey==CurShot->ShotComposition.List[ObjectNumber]->IndexKey) {
                if (CurObject->List[TheShot]->ShotComposition.List[w]->IsVisible) VideoPosition+=CurObject->List[TheShot]->StaticDuration;
                break;
            }
            ThePosition+=CurObject->List[TheShot]->StaticDuration;
            TheShot++;
        }
        VideoPosition+=(CurTimePosition-ThePosition);

    } else VideoPosition=CurTimePosition;


    // PreparedBrush->W and PreparedBrush->H = 0 when producing sound track in render process
    if ((!Object->IsVisible)||(width==0)||(height==0)||(Transfo && ((NewW==0)||(NewH==0)))||(!Transfo && ((Object->w==0)||(Object->h==0)))) return;

    // Define values depending on BlockPctDone and PrevCompoObject
    TheX             =Transfo?NewX:Object->x;
    TheY             =Transfo?NewY:Object->y;
    TheW             =Transfo?NewW:Object->w;
    TheH             =Transfo?NewH:Object->h;
    TheRotateZAxis   =Object->RotateZAxis+(EnableAnimation && (Object->BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*Object->TurnZAxis:0);
    TheRotateXAxis   =Object->RotateXAxis+(EnableAnimation && (Object->BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*Object->TurnXAxis:0);
    TheRotateYAxis   =Object->RotateYAxis+(EnableAnimation && (Object->BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*Object->TurnYAxis:0);
    TheTxtZoomLevel  =Object->TxtZoomLevel;
    TheTxtScrollX    =Object->TxtScrollX;
    TheTxtScrollY    =Object->TxtScrollY;

    if (PrevCompoObject) {
        if (PrevCompoObject->x!=TheX)                       TheX            =PrevCompoObject->x+(TheX-PrevCompoObject->x)*BlockPctDone;
        if (PrevCompoObject->y!=TheY)                       TheY            =PrevCompoObject->y+(TheY-PrevCompoObject->y)*BlockPctDone;
        if (PrevCompoObject->w!=TheW)                       TheW            =PrevCompoObject->w+(TheW-PrevCompoObject->w)*BlockPctDone;
        if (PrevCompoObject->h!=TheH)                       TheH            =PrevCompoObject->h+(TheH-PrevCompoObject->h)*BlockPctDone;
        if (PrevCompoObject->RotateZAxis!=TheRotateZAxis)   TheRotateZAxis  =PrevCompoObject->RotateZAxis+(TheRotateZAxis-PrevCompoObject->RotateZAxis)*BlockPctDone;
        if (PrevCompoObject->RotateXAxis!=TheRotateXAxis)   TheRotateXAxis  =PrevCompoObject->RotateXAxis+(TheRotateXAxis-PrevCompoObject->RotateXAxis)*BlockPctDone;
        if (PrevCompoObject->RotateYAxis!=TheRotateYAxis)   TheRotateYAxis  =PrevCompoObject->RotateYAxis+(TheRotateYAxis-PrevCompoObject->RotateYAxis)*BlockPctDone;
        if (PrevCompoObject->TxtZoomLevel!=TheTxtZoomLevel) TheTxtZoomLevel =PrevCompoObject->TxtZoomLevel+(TheTxtZoomLevel-PrevCompoObject->TxtZoomLevel)*BlockPctDone;
        if (PrevCompoObject->TxtScrollX!=TheTxtScrollX)     TheTxtScrollX   =PrevCompoObject->TxtScrollX+(TheTxtScrollX-PrevCompoObject->TxtScrollX)*BlockPctDone;
        if (PrevCompoObject->TxtScrollY!=TheTxtScrollY)     TheTxtScrollY   =PrevCompoObject->TxtScrollY+(TheTxtScrollY-PrevCompoObject->TxtScrollY)*BlockPctDone;
    } else {
        if (EnableAnimation && (Object->BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)) {
            TheRotateZAxis=Object->RotateZAxis+360*Object->TurnZAxis*BlockPctDone;
            TheRotateXAxis=Object->RotateXAxis+360*Object->TurnXAxis*BlockPctDone;
            TheRotateYAxis=Object->RotateYAxis+360*Object->TurnYAxis*BlockPctDone;
        }
    }

    //**********************************************************************************

    X=TheX*width;
    Y=TheY*height;
    W=TheW*width;
    H=TheH*height;

    //**********************************************************************************

    if ((W>0)&&(H>0)) {

        //***********************************************************************************
        // Compute shape
        //***********************************************************************************
        X=round(X);
        Y=round(Y);
        W=round(W/2)*2;
        H=round(H/2)*2;

        //**********************************************************************************
        // Opacity and dissolve annimation
        //**********************************************************************************
        DestOpacity =(Object->Opacity==1?0.75:Object->Opacity==2?0.50:Object->Opacity==3?0.25:1);
        if (EnableAnimation) {
            if (Object->BlockAnimType==BLOCKANIMTYPE_DISSOLVE) {

                double BlinkNumber=0;
                switch (Object->Dissolve) {
                    case BLOCKANIMVALUE_APPEAR        : DestOpacity=DestOpacity*BlockPctDone;       break;
                    case BLOCKANIMVALUE_DISAPPEAR     : DestOpacity=DestOpacity*(1-BlockPctDone);   break;
                    case BLOCKANIMVALUE_BLINK_SLOW    : BlinkNumber=0.25;                           break;
                    case BLOCKANIMVALUE_BLINK_MEDIUM  : BlinkNumber=0.5;                            break;
                    case BLOCKANIMVALUE_BLINK_FAST    : BlinkNumber=1;                              break;
                    case BLOCKANIMVALUE_BLINK_VERYFAST: BlinkNumber=2;                              break;
                }
                if (BlinkNumber!=0) {
                    BlinkNumber=BlinkNumber*ShotDuration;
                    if (int(BlinkNumber/1000)!=(BlinkNumber/1000)) BlinkNumber=int(BlinkNumber/1000)+1; else BlinkNumber=int(BlinkNumber/1000); // Adjust to upper 1000
                    double FullPct=BlockPctDone*BlinkNumber*100;
                    FullPct=int(FullPct)-int(FullPct/100)*100;
                    FullPct=(FullPct/100)*2;
                    if (FullPct<1)  DestOpacity=DestOpacity*(1-FullPct);
                        else        DestOpacity=DestOpacity*(FullPct-1);
                }
            }
        }

        //***********************************************************************************
        // Compute shape
        //***********************************************************************************
        PolygonList=ComputePolygon(Object->BackgroundForm,X,Y,W,H);
        ShapeRect  =PolygonToRectF(PolygonList);

        //***********************************************************************************
        // Prepare Transform Matrix
        //***********************************************************************************
        if ((Object->TextClipArtName=="")&&
            (!((Object->BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(Object->BackgroundBrush->Image)&&(Object->BackgroundBrush->Image->IsVectorImg)))&&
            (((Object->BackgroundBrush->BrushType!=BRUSHTYPE_NOBRUSH)||(Object->PenSize!=0)))&&
            (Object->BackgroundBrush->BrushType!=BRUSHTYPE_NOBRUSH)
           ) NeedPreparedBrush=true;
    }
}

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
    Text                    = "";                                               // Text of the object
    TextClipArtName         = "";                                               // Clipart name (if clipart mode)
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

    BlockSpeedWave          = SPEEDWAVE_PROJECTDEFAULT;

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
    QRectF Rect =GetPrivateTextMargin();
    this->TMx   =Rect.left();
    this->TMy   =Rect.top();
    this->TMw   =Rect.width();
    this->TMh   =Rect.height();
}

//====================================================================================================================

QRectF cCompositionObject::GetPrivateTextMargin() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject::GetPrivateTextMargin");

    QRectF RectF;
    if (TMType==TEXTMARGINS_FULLSHAPE) {
            QRectF X100=RectF=PolygonToRectF(ComputePolygon(BackgroundForm,0,0,100,100));
            RectF=QRectF(X100.left()/100,X100.top()/100,X100.width()/100,X100.height()/100);
    } else if (TMType==TEXTMARGINS_SHAPEDEFAULT) {
        if (TextClipArtName!="") {
            cTextFrameObject *TFO=&TextFrameList.List[TextFrameList.SearchImage(TextClipArtName)];
            RectF=QRectF(TFO->TMx,TFO->TMy,TFO->TMw,TFO->TMh);
        } else {
            RectF=QRectF(ShapeFormDefinition[BackgroundForm].TMx,ShapeFormDefinition[BackgroundForm].TMy,
                         ShapeFormDefinition[BackgroundForm].TMw,ShapeFormDefinition[BackgroundForm].TMh);
        }
    } else RectF=QRectF(TMx,TMy,TMw,TMh);
    return RectF;
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

    Element.setAttribute("BlockSpeedWave",BlockSpeedWave);          // Block speed wave

    // Block animation
    Element.setAttribute("BlockAnimType",BlockAnimType);            // Block animation type
    Element.setAttribute("TurnZAxis",TurnZAxis);                    // Number of turn from Z axis
    Element.setAttribute("TurnXAxis",TurnXAxis);                    // Number of turn from X axis
    Element.setAttribute("TurnYAxis",TurnYAxis);                    // Number of turn from Y axis
    Element.setAttribute("Dissolve",Dissolve);                      // Dissolve value

    // Text part
    Element.setAttribute("TextClipArtName",TextClipArtName);        // ClipArt name (if text clipart mode)
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

        if (Element.hasAttribute("BlockSpeedWave"))             BlockSpeedWave  =Element.attribute("BlockSpeedWave").toInt();           // Block speed wave

        if (Element.hasAttribute("BlockAnimType"))              BlockAnimType   =Element.attribute("BlockAnimType").toInt();            // Block animation type
        if (Element.hasAttribute("TurnZAxis"))                  TurnZAxis       =Element.attribute("TurnZAxis").toInt();                // Number of turn from Z axis
        if (Element.hasAttribute("TurnXAxis"))                  TurnXAxis       =Element.attribute("TurnXAxis").toInt();                // Number of turn from X axis
        if (Element.hasAttribute("TurnYAxis"))                  TurnYAxis       =Element.attribute("TurnYAxis").toInt();                // Number of turn from Y axis
        if (Element.hasAttribute("Dissolve"))                   Dissolve        =Element.attribute("Dissolve").toInt();                 // Dissolve value

        // Text part
        if (Element.hasAttribute("TextClipArtName"))            TextClipArtName =Element.attribute("TextClipArtName");                      // ClipArt name (if text clipart mode)
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
            if (ModifyFlag) ((MainWindow *)ApplicationConfig->TopLevelWindow)->SetModifyFlag(true);
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

void cCompositionObject::ApplyAutoCompoSize(int AutoCompoStyle,int ffDProjectGeometry,bool AllowMove) {
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
    w=DestWidth/ScreenWidth;
    h=DestHeight/ScreenHeight;
    if (AllowMove) {
        x=((ScreenWidth-DestWidth)/2)/ScreenWidth;
        y=((ScreenHeight-DestHeight)/2)/ScreenHeight;
    }
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
        if (((MainWindow *)ApplicationConfig->TopLevelWindow)->Diaporama->ImageGeometry==GEOMETRY_4_3)        { DisplayW=1440; DisplayH=1080; }
        else if (((MainWindow *)ApplicationConfig->TopLevelWindow)->Diaporama->ImageGeometry==GEOMETRY_16_9)  { DisplayW=1920; DisplayH=1080; }
        else if (((MainWindow *)ApplicationConfig->TopLevelWindow)->Diaporama->ImageGeometry==GEOMETRY_40_17) { DisplayW=1920; DisplayH=816;  }
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
    BlockSpeedWave       =CompositionObjectToCopy->BlockSpeedWave;
    BlockAnimType        =CompositionObjectToCopy->BlockAnimType;
    TurnZAxis            =CompositionObjectToCopy->TurnZAxis;
    TurnXAxis            =CompositionObjectToCopy->TurnXAxis;
    TurnYAxis            =CompositionObjectToCopy->TurnYAxis;
    Dissolve             =CompositionObjectToCopy->Dissolve;
    Opacity              =CompositionObjectToCopy->Opacity;
    Text                 =CompositionObjectToCopy->Text;
    TextClipArtName      =CompositionObjectToCopy->TextClipArtName;
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
void cCompositionObject::DrawCompositionObject(QPainter *DestPainter,double  ADJUST_RATIO,double width,double height,bool PreviewMode,qlonglong Position,
                                               cSoundBlockList *SoundTrackMontage,double BlockPctDone,double ImagePctDone,cCompositionObject *PrevCompoObject,bool UseBrushCache,
                                               qlonglong ShotDuration,bool EnableAnimation,
                                               bool Transfo,double NewX,double NewY,double NewW,double NewH,bool DisplayTextMargin,cCompositionObjectContext *PreparedBrush) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionObject:DrawCompositionObject");

    // W and H = 0 when producing sound track in render process
    if (!IsVisible) return;

    bool SoundOnly=(DestPainter==NULL)||(width==0)||(height==0)||(Transfo && ((NewW==0)||(NewH==0)))||(!Transfo && ((w==0)||(h==0)));

    if (SoundOnly) {
        // if SoundOnly then load Brush of type BRUSHTYPE_IMAGEDISK to SoundTrackMontage
        if ((BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(SoundTrackMontage!=NULL)) {
            QBrush *BR=BackgroundBrush->GetBrush(QRectF(0,0,0,0),PreviewMode,Position,SoundTrackMontage,ImagePctDone,NULL);
            if (BR) delete BR;
        }
    } else {
        QPainter::RenderHints hints=(!PreviewMode || (ApplicationConfig==NULL) || ApplicationConfig->Smoothing)?
                    QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen:
                    QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen;

        double              TheX,TheY,TheW,TheH;
        double              TheRotateZAxis,TheRotateXAxis,TheRotateYAxis;
        double              TheTxtZoomLevel,TheTxtScrollX,TheTxtScrollY;
        double              X,Y,W,H,DestOpacity;
        QList<QPolygonF>    PolygonList;
        QRectF              ShapeRect;

        if (PreparedBrush) {

            TheX            =PreparedBrush->TheX;
            TheY            =PreparedBrush->TheY;
            TheW            =PreparedBrush->TheW;
            TheH            =PreparedBrush->TheH;
            TheRotateZAxis  =PreparedBrush->TheRotateZAxis;
            TheRotateXAxis  =PreparedBrush->TheRotateXAxis;
            TheRotateYAxis  =PreparedBrush->TheRotateYAxis;
            TheTxtZoomLevel =PreparedBrush->TheTxtZoomLevel;
            TheTxtScrollX   =PreparedBrush->TheTxtScrollX;
            TheTxtScrollY   =PreparedBrush->TheTxtScrollY;
            X               =PreparedBrush->X;
            Y               =PreparedBrush->Y;
            W               =PreparedBrush->W;
            H               =PreparedBrush->H;
            DestOpacity     =PreparedBrush->DestOpacity;
            PolygonList     =PreparedBrush->PolygonList;
            ShapeRect       =PreparedBrush->ShapeRect;

        } else {

            // Define values depending on BlockPctDone and PrevCompoObject
            TheX             =Transfo?NewX:x;
            TheY             =Transfo?NewY:y;
            TheW             =Transfo?NewW:w;
            TheH             =Transfo?NewH:h;
            TheRotateZAxis   =RotateZAxis+(EnableAnimation && (BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*TurnZAxis:0);
            TheRotateXAxis   =RotateXAxis+(EnableAnimation && (BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*TurnXAxis:0);
            TheRotateYAxis   =RotateYAxis+(EnableAnimation && (BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)?360*TurnYAxis:0);
            TheTxtZoomLevel  =TxtZoomLevel;
            TheTxtScrollX    =TxtScrollX;
            TheTxtScrollY    =TxtScrollY;

            if (PrevCompoObject) {
                if (PrevCompoObject->x!=TheX)                       TheX            =PrevCompoObject->x+(TheX-PrevCompoObject->x)*BlockPctDone;
                if (PrevCompoObject->y!=TheY)                       TheY            =PrevCompoObject->y+(TheY-PrevCompoObject->y)*BlockPctDone;
                if (PrevCompoObject->w!=TheW)                       TheW            =PrevCompoObject->w+(TheW-PrevCompoObject->w)*BlockPctDone;
                if (PrevCompoObject->h!=TheH)                       TheH            =PrevCompoObject->h+(TheH-PrevCompoObject->h)*BlockPctDone;
                if (PrevCompoObject->RotateZAxis!=TheRotateZAxis)   TheRotateZAxis  =PrevCompoObject->RotateZAxis+(TheRotateZAxis-PrevCompoObject->RotateZAxis)*BlockPctDone;
                if (PrevCompoObject->RotateXAxis!=TheRotateXAxis)   TheRotateXAxis  =PrevCompoObject->RotateXAxis+(TheRotateXAxis-PrevCompoObject->RotateXAxis)*BlockPctDone;
                if (PrevCompoObject->RotateYAxis!=TheRotateYAxis)   TheRotateYAxis  =PrevCompoObject->RotateYAxis+(TheRotateYAxis-PrevCompoObject->RotateYAxis)*BlockPctDone;
                if (PrevCompoObject->TxtZoomLevel!=TheTxtZoomLevel) TheTxtZoomLevel =PrevCompoObject->TxtZoomLevel+(TheTxtZoomLevel-PrevCompoObject->TxtZoomLevel)*BlockPctDone;
                if (PrevCompoObject->TxtScrollX!=TheTxtScrollX)     TheTxtScrollX   =PrevCompoObject->TxtScrollX+(TheTxtScrollX-PrevCompoObject->TxtScrollX)*BlockPctDone;
                if (PrevCompoObject->TxtScrollY!=TheTxtScrollY)     TheTxtScrollY   =PrevCompoObject->TxtScrollY+(TheTxtScrollY-PrevCompoObject->TxtScrollY)*BlockPctDone;
            } else {
                if (EnableAnimation && (BlockAnimType==BLOCKANIMTYPE_MULTIPLETURN)) {
                    TheRotateZAxis=RotateZAxis+360*TurnZAxis*BlockPctDone;
                    TheRotateXAxis=RotateXAxis+360*TurnXAxis*BlockPctDone;
                    TheRotateYAxis=RotateYAxis+360*TurnYAxis*BlockPctDone;
                }
            }

            //**********************************************************************************

            X           =TheX*width;
            Y           =TheY*height;
            W           =TheW*width;
            H           =TheH*height;
            DestOpacity =(Opacity==1?0.75:Opacity==2?0.50:Opacity==3?0.25:1);

            if ((W>0)&&(H>0)) {
                X=round(X);
                Y=round(Y);
                W=round(W/2)*2;
                H=round(H/2)*2;
                //**********************************************************************************
                // Opacity and dissolve annimation
                //**********************************************************************************
                if (EnableAnimation) {
                    if (BlockAnimType==BLOCKANIMTYPE_DISSOLVE) {

                        double BlinkNumber=0;
                        switch (Dissolve) {
                            case BLOCKANIMVALUE_APPEAR        : DestOpacity=DestOpacity*BlockPctDone;       break;
                            case BLOCKANIMVALUE_DISAPPEAR     : DestOpacity=DestOpacity*(1-BlockPctDone);   break;
                            case BLOCKANIMVALUE_BLINK_SLOW    : BlinkNumber=0.25;                           break;
                            case BLOCKANIMVALUE_BLINK_MEDIUM  : BlinkNumber=0.5;                            break;
                            case BLOCKANIMVALUE_BLINK_FAST    : BlinkNumber=1;                              break;
                            case BLOCKANIMVALUE_BLINK_VERYFAST: BlinkNumber=2;                              break;
                        }
                        if (BlinkNumber!=0) {
                            BlinkNumber=BlinkNumber*ShotDuration;
                            if (int(BlinkNumber/1000)!=(BlinkNumber/1000)) BlinkNumber=int(BlinkNumber/1000)+1; else BlinkNumber=int(BlinkNumber/1000); // Adjust to upper 1000
                            double FullPct=BlockPctDone*BlinkNumber*100;
                            FullPct=int(FullPct)-int(FullPct/100)*100;
                            FullPct=(FullPct/100)*2;
                            if (FullPct<1)  DestOpacity=DestOpacity*(1-FullPct);
                                else        DestOpacity=DestOpacity*(FullPct-1);
                        }
                    }
                }
                //***********************************************************************************
                // Compute shape
                //***********************************************************************************
                PolygonList=ComputePolygon(BackgroundForm,X,Y,W,H);
                ShapeRect  =PolygonToRectF(PolygonList);
            }
        }

        if ((W>0)&&(H>0)) {

            DestPainter->save();
            DestPainter->setOpacity(DestOpacity);
            DestPainter->setRenderHints(hints,true);
            DestPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);

            QPainter    *Painter=DestPainter;
            QImage      ShadowImg;

            //***********************************************************************************
            // If shadow, draw all on a buffered image instead of drawing directly to destination painter
            //***********************************************************************************
            if (FormShadow) {
                ShadowImg=QImage(width,height,QImage::Format_ARGB32_Premultiplied);
                Painter=new QPainter();
                Painter->begin(&ShadowImg);
                Painter->setRenderHints(hints,true);
                Painter->setCompositionMode(QPainter::CompositionMode_Source);
                Painter->fillRect(QRect(0,0,width,height),Qt::transparent);
                Painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            }

            //***********************************************************************************
            // Prepare Transform Matrix
            //***********************************************************************************

            QPointF     CenterF(round(ShapeRect.center().x()/2)*2,round(ShapeRect.center().y()/2)*2);
            QTransform  Matrix;
            Matrix.translate(CenterF.x(),CenterF.y());                          // Translate to be sure we are on the center of the shape
            if (TheRotateZAxis!=0) Matrix.rotate(TheRotateZAxis,Qt::ZAxis);     // Standard axis
            if (TheRotateXAxis!=0) Matrix.rotate(TheRotateXAxis,Qt::XAxis);     // Rotate from X axis
            if (TheRotateYAxis!=0) Matrix.rotate(TheRotateYAxis,Qt::YAxis);     // Rotate from Y axis
            Painter->setWorldTransform(Matrix,false);

            if (TextClipArtName!="") {
                QSvgRenderer SVGImg(TextClipArtName);
                if (!SVGImg.isValid()) {
                    ToLog(LOGMSG_CRITICAL,QString("IN:cCompositionObject:DrawCompositionObject: Error loading ClipArt Image %1").arg(TextClipArtName));
                } else {
                    SVGImg.render(Painter,QRectF(-W/2,-H/2,W,H));
                }
            } else if ((BackgroundBrush->BrushType==BRUSHTYPE_IMAGEDISK)&&(BackgroundBrush->Image)&&(BackgroundBrush->Image->IsVectorImg)) {
                // Vector Image
                if (!BackgroundBrush->Image->VectorImage) BackgroundBrush->Image->VectorImage=new QSvgRenderer(BackgroundBrush->Image->FileName);
                if ((BackgroundBrush->Image->VectorImage)&&(BackgroundBrush->Image->VectorImage->isValid())) BackgroundBrush->Image->VectorImage->render(Painter,QRectF(-W/2,-H/2,W,H));
            } else if ((BackgroundBrush->BrushType!=BRUSHTYPE_NOBRUSH)||(PenSize!=0)) {
                for (int i=0;i<PolygonList.count();i++) PolygonList[i].translate(-CenterF.x(),-CenterF.y());
                QRectF NewShapeRect=PolygonToRectF(PolygonList);

                //***********************************************************************************
                // Prepare pen
                //***********************************************************************************

                QPen Pen;
                Pen.setCapStyle(Qt::RoundCap);
                Pen.setJoinStyle(Qt::RoundJoin);
                Pen.setCosmetic(false);
                Pen.setStyle(Qt::SolidLine);
                if (PenSize==0) Painter->setPen(Qt::NoPen); else {
                    Pen.setColor(PenColor);
                    Pen.setWidthF(double(PenSize)*ADJUST_RATIO);
                    Pen.setStyle((Qt::PenStyle)PenStyle);
                    Painter->setPen(Pen);
                }

                //***********************************************************************************
                // Prepare brush
                //***********************************************************************************
                if (BackgroundBrush->BrushType==BRUSHTYPE_NOBRUSH) Painter->setBrush(Qt::NoBrush); else {

                    // Create brush with filter and Ken Burns effect !
                    QBrush *BR=BackgroundBrush->GetBrush(QRectF(0,0,W,H),PreviewMode,Position,SoundTrackMontage,ImagePctDone,PrevCompoObject?PrevCompoObject->BackgroundBrush:NULL,UseBrushCache);
                    if (BR) {
                        QTransform  MatrixBR;
                        // Avoid phantom lines for image brush
                        if ((!BR->textureImage().isNull())&&((TheRotateZAxis!=0)||(TheRotateXAxis!=0)||(TheRotateYAxis!=0))) {
                            QImage   TempImage(W+4,H+4,QImage::Format_ARGB32_Premultiplied);
                            QPainter TempPainter;
                            TempPainter.begin(&TempImage);
                            TempPainter.setRenderHints(hints,true);
                            TempPainter.setCompositionMode(QPainter::CompositionMode_Source);
                            TempPainter.fillRect(QRect(0,0,TempImage.width(),TempImage.height()),Qt::transparent);
                            TempPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                            TempPainter.drawImage(2,2,BR->textureImage());
                            TempPainter.end();
                            delete BR;
                            BR=new QBrush(TempImage);
                            MatrixBR.translate(NewShapeRect.left()+(X-ShapeRect.left())-2,NewShapeRect.top()+(Y-ShapeRect.top())-2);
                        } else {
                            MatrixBR.translate(NewShapeRect.left()+(X-ShapeRect.left()),NewShapeRect.top()+(Y-ShapeRect.top()));
                        }
                        BR->setTransform(MatrixBR);         // Apply transform matrix to the brush
                        Painter->setBrush(*BR);
                        delete BR;
                    } else {
                        ToLog(LOGMSG_CRITICAL,"Error in cCompositionObject::DrawCompositionObject Brush is NULL !");
                        Painter->setBrush(Qt::NoBrush);
                    }
                }

                //***********************************************************************************
                // Draw shape (with pen and brush and transform matrix)
                //***********************************************************************************
                if (BackgroundBrush->BrushType==BRUSHTYPE_NOBRUSH) Painter->setCompositionMode(QPainter::CompositionMode_Source);
                for (int i=0;i<PolygonList.count();i++) Painter->drawPolygon(PolygonList.at(i));
                if (BackgroundBrush->BrushType==BRUSHTYPE_NOBRUSH) Painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            }

            //**********************************************************************************
            // Text part
            //**********************************************************************************
            if ((TheTxtZoomLevel>0)&&(Text!="")) {

                double          FullMargin=((TMType==TEXTMARGINS_FULLSHAPE)||(TMType==TEXTMARGINS_CUSTOM))?0:double(PenSize)*ADJUST_RATIO/double(2);
                QRectF          TextMargin;
                double          PointSize =((double(width)/double(SCALINGTEXTFACTOR)));
                QTextDocument   TextDocument;

                if ((TMType==TEXTMARGINS_FULLSHAPE)||(TMType==TEXTMARGINS_CUSTOM)) TextMargin=QRectF(TMx*TheW*width,TMy*TheH*height,TMw*TheW*width,TMh*TheH*height);
                else if (TextClipArtName!="") {
                        cTextFrameObject *TFO=&TextFrameList.List[TextFrameList.SearchImage(TextClipArtName)];
                        TextMargin=QRectF(TFO->TMx*TheW*width+FullMargin,TFO->TMy*TheH*height+FullMargin,
                                          TFO->TMw*TheW*width-FullMargin*2,TFO->TMh*TheH*height-FullMargin*2);
                } else {
                    TextMargin=QRectF(ShapeFormDefinition[BackgroundForm].TMx*TheW*width+FullMargin,ShapeFormDefinition[BackgroundForm].TMy*TheH*height+FullMargin,
                                      ShapeFormDefinition[BackgroundForm].TMw*TheW*width-FullMargin*2,ShapeFormDefinition[BackgroundForm].TMh*TheH*height-FullMargin*2);
                }
                TextMargin.translate(-ShapeRect.width()/2,-ShapeRect.height()/2);

                if (DisplayTextMargin) {
                    QPen PP(Qt::blue);
                    PP.setStyle(Qt::DotLine);
                    PP.setWidth(1);
                    Painter->setPen(PP);
                    Painter->setBrush(Qt::NoBrush);
                    Painter->drawRect(TextMargin);
                }

                Painter->setClipRect(TextMargin);
                Painter->setClipping(true);         // Not sure is needed !

                TextDocument.setHtml(Text);
                TextDocument.setTextWidth(TextMargin.width()/PointSize);

                QRectF  FmtBdRect(0,0,
                                  double(TextDocument.documentLayout()->documentSize().width())*(TheTxtZoomLevel/100)*PointSize,
                                  double(TextDocument.documentLayout()->documentSize().height())*(TheTxtZoomLevel/100)*PointSize);

                int     MaxH  =TextMargin.height()>FmtBdRect.height()?TextMargin.height():FmtBdRect.height();
                double  DecalX=(TheTxtScrollX/100)*TextMargin.width()+TextMargin.center().x()-TextMargin.width()/2+(TextMargin.width()-FmtBdRect.width())/2;
                double  DecalY=(-TheTxtScrollY/100)*MaxH+TextMargin.center().y()-TextMargin.height()/2;

                if (VAlign==0)      ;                                                               //Qt::AlignTop (Nothing to do)
                else if (VAlign==1) DecalY=DecalY+(TextMargin.height()-FmtBdRect.height())/2;       //Qt::AlignVCenter
                else                DecalY=DecalY+(TextMargin.height()-FmtBdRect.height());         //Qt::AlignBottom)

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
                    Painter->save();
                    switch (StyleText) {
                        case 2: Painter->translate(DecalX-1,DecalY-1);   break;  //2=shadow up-left
                        case 3: Painter->translate(DecalX+1,DecalY-1);   break;  //3=shadow up-right
                        case 4: Painter->translate(DecalX-1,DecalY+1);   break;  //4=shadow bt-left
                        case 5: Painter->translate(DecalX+1,DecalY+1);   break;  //5=shadow bt-right
                    }
                    Painter->scale((TheTxtZoomLevel/100)*PointSize,(TheTxtZoomLevel/100)*PointSize);
                    TextDocument.documentLayout()->draw(Painter,Context);
                    Painter->restore();
                    TextDocument.setHtml(Text);     // Restore Text Document
                }

                Painter->save();
                Painter->translate(DecalX,DecalY);
                Painter->scale((TheTxtZoomLevel/100)*PointSize,(TheTxtZoomLevel/100)*PointSize);
                TextDocument.documentLayout()->draw(Painter,Context);
                Painter->restore();
            }

            //**********************************************************************************
            // Block shadow part
            //**********************************************************************************

            // if shadow, draw buffered image to destination painter
            if (FormShadow) {
                Painter->end();
                delete Painter;

                // 1st step : construct ImgShadow as a mask from ShadowImg
                QImage      ImgShadow   =ShadowImg.copy();
                u_int8_t    *Data       =ImgShadow.bits();
                QColor      SColor      =QColor(FormShadowColor);
                u_int8_t    R           =SColor.red();
                u_int8_t    G           =SColor.green();
                u_int8_t    B           =SColor.blue();

                for (int i=0;i<(width-1)*(height-1);i++) {
                  if (*(Data+3)!=0) {
                    *Data++=B;
                    *Data++=G;
                    *Data++=R;
                    if (*Data) *Data++=0xff;    // force alpha chanel to 1 or 0
                  } else Data+=4;
                }

                // 2nd step : Draw images
                double Distance=double(FormShadowDistance)*ADJUST_RATIO;
                switch (FormShadow) {
                    case 1  : DestPainter->setOpacity(0.75*DestOpacity); DestPainter->drawImage(-Distance,-Distance,ImgShadow);     break;      // shadow up-left
                    case 2  : DestPainter->setOpacity(0.75*DestOpacity); DestPainter->drawImage(Distance,-Distance,ImgShadow);      break;      // shadow up-right
                    case 3  : DestPainter->setOpacity(0.75*DestOpacity); DestPainter->drawImage(-Distance,Distance,ImgShadow);      break;      // shadow bt-left
                    default : DestPainter->setOpacity(0.75*DestOpacity); DestPainter->drawImage(Distance,Distance,ImgShadow);       break;      // shadow bt-right
                }
                DestPainter->setOpacity(DestOpacity);
                DestPainter->drawImage(0,0,ShadowImg);

            }

            DestPainter->restore();
        }
    }
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

bool cCompositionList::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,cCompositionList *ObjectComposition,QStringList *AliasList,cBaseApplicationConfig *ApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cCompositionList:LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        bool    IsOk=true;

        // Load composition list
        List.clear();
        TypeComposition=Element.attribute("TypeComposition").toInt();
        int CompositionNumber=Element.attribute("CompositionNumber").toInt();
        for (int i=0;i<CompositionNumber;i++) {
            cCompositionObject *CompositionObject=new cCompositionObject(TypeComposition,0,((MainWindow *)ApplicationConfig->TopLevelWindow)->ApplicationConfig);    // IndexKey will be load from XML
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
    StaticDuration                  = ((MainWindow *)Parent->Parent->ApplicationConfig->TopLevelWindow)->ApplicationConfig->FixedDuration;    // Duration (in msec) of the static part animation
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
        // Composition list for this object
        ShotComposition.LoadFromXML(Element,"ShotComposition",PathForRelativPath,ObjectComposition,AliasList,Parent->Parent->ApplicationConfig);
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
    TransitionSpeedWave                     = SPEEDWAVE_PROJECTDEFAULT;
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
void cDiaporamaObject::DrawThumbnail(int ThumbWidth,int ThumbHeight,QPainter *Painter,int AddX,int AddY,int ShotNumber) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporamaObject:DrawThumbnail");

    QImage *Thumb=(ShotNumber==0)?Thumbnail:NULL;

    if ((!Thumb)||(Thumb->isNull())||(Thumb->width()!=ThumbWidth)||(Thumb->height()!=ThumbHeight)) {
        if ((ShotNumber==0)&&(Thumbnail)) {
            delete Thumbnail;
            Thumbnail=NULL;
        }
        Thumb=new QImage(ThumbWidth,ThumbHeight,QImage::Format_ARGB32_Premultiplied);
        QPainter  P;
        P.begin(Thumb);
        P.fillRect(0,0,ThumbWidth,ThumbHeight,Transparent);
        if (List.count()>0) for (int j=0;j<List[ShotNumber]->ShotComposition.List.count();j++) {
            int StartPosToAdd=(List[ShotNumber]->ShotComposition.List[j]->BackgroundBrush->Video!=NULL)?QTime(0,0,0,0).msecsTo(List[ShotNumber]->ShotComposition.List[j]->BackgroundBrush->Video->StartPos):0;
            if (ShotNumber!=0) {
                // Calc Start position of the video (depending on visible state)
                int IndexKeyToFind=List[ShotNumber]->ShotComposition.List[j]->IndexKey;
                for (int k=0;k<ShotNumber;k++)
                  for (int l=0;l<List[k]->ShotComposition.List.count();l++) if (List[k]->ShotComposition.List[l]->IndexKey==IndexKeyToFind) {
                    if (List[k]->ShotComposition.List[l]->IsVisible) StartPosToAdd+=List[k]->StaticDuration;
                    l=List[k]->ShotComposition.List.count();    // Stop loop
                }
            }
            List[ShotNumber]->ShotComposition.List[j]->DrawCompositionObject(&P,double(ThumbHeight)/1080,ThumbWidth,ThumbHeight,true,StartPosToAdd,NULL,0,0,NULL,false,List[ShotNumber]->StaticDuration,false);
        }
        P.end();
        if (ShotNumber==0) Thumbnail=Thumb;
    }
    Painter->drawImage(AddX,AddY,*Thumb);
    if ((Thumbnail!=Thumb)&&(Thumb)) delete Thumb;
}

//===============================================================

int cDiaporamaObject::GetSpeedWave() {
    if (TransitionSpeedWave==SPEEDWAVE_PROJECTDEFAULT) return Parent->TransitionSpeedWave; else return TransitionSpeedWave;
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
    Element.appendChild(SubElement);

    // Transition properties
    SubElement=DomDocument.createElement("Transition");
    SubElement.setAttribute("TransitionFamilly",TransitionFamilly);                         // Transition familly
    SubElement.setAttribute("TransitionSubType",TransitionSubType);                         // Transition type in the familly
    SubElement.setAttribute("TransitionDuration",TransitionDuration);                       // Transition duration (in msec)
    SubElement.setAttribute("TransitionSpeedWave",TransitionSpeedWave);                     // Transition speed wave
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
            if (IsOk && ModifyFlag) ((MainWindow *)Parent->ApplicationConfig->TopLevelWindow)->SetModifyFlag(true);
            if (ModifyFlag) ((MainWindow *)Parent->ApplicationConfig->TopLevelWindow)->SetModifyFlag(true);
        }
        // Transition properties
        if ((Element.elementsByTagName("Transition").length()>0)&&(Element.elementsByTagName("Transition").item(0).isElement()==true)) {
            QDomElement SubElement=Element.elementsByTagName("Transition").item(0).toElement();
            TransitionFamilly =SubElement.attribute("TransitionFamilly").toInt();                                                           // Transition familly
            TransitionSubType =SubElement.attribute("TransitionSubType").toInt();                                                           // Transition type in the familly
            TransitionDuration=SubElement.attribute("TransitionDuration").toInt();                                                          // Transition duration (in msec)
            if (SubElement.hasAttribute("TransitionSpeedWave")) TransitionSpeedWave=SubElement.attribute("TransitionSpeedWave").toInt();    // Transition speed wave
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
            cMusicObject *MusicObject=new cMusicObject(((MainWindow *)Parent->ApplicationConfig->TopLevelWindow)->ApplicationConfig);
            bool ModifyFlag=false;
            if (!MusicObject->LoadFromXML(Element,"Music-"+QString("%1").arg(i),PathForRelativPath,AliasList,&ModifyFlag)) IsOk=false;
            MusicList.append(*MusicObject);
            if (ModifyFlag) ((MainWindow *)Parent->ApplicationConfig->TopLevelWindow)->SetModifyFlag(true);
        }

        // Global blocks composition table
        IsOk=ObjectComposition.LoadFromXML(Element,"ObjectComposition",PathForRelativPath,NULL,AliasList,Parent->ApplicationConfig);         // ObjectComposition

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

cDiaporama::cDiaporama(cBaseApplicationConfig *TheApplicationConfig) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:cDiaporama");

    ApplicationConfig           = TheApplicationConfig;
    ProjectInfo                 = new cffDProjectFile(ApplicationConfig);
    CurrentCol                  = -1;                                                               // Current selected item
    CurrentPosition             = -1;                                                               // Current position (msec)
    CurrentChapter              = -1;
    CurrentChapterName          = QString("");
    IsModify                    = false;                                                            // true if project was modify
    ProjectFileName             = "";                                                               // Path and name of current file project
    ProjectInfo->Composer       = ApplicationConfig->ApplicationName+QString(" ")+ApplicationConfig->ApplicationVersion;
    ProjectInfo->Author         = ApplicationConfig->DefaultAuthor;
    ProjectInfo->DefaultLanguage= ApplicationConfig->DefaultLanguage;
    TransitionSpeedWave         = ApplicationConfig->DefaultTransitionSpeedWave;                    // Speed wave for transition
    BlockAnimSpeedWave          = ApplicationConfig->DefaultBlockAnimSpeedWave;                     // Speed wave for block animation
    ImageAnimSpeedWave          = ApplicationConfig->DefaultImageAnimSpeedWave;                     // Speed wave for image framing and correction animation

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
        QBrush *BR=List[Index]->BackgroundBrush->GetBrush(QRectF(0,0,Width,Height),true,0,NULL,1,NULL);
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
    ProjectInfo->Composer   =ApplicationConfig->ApplicationName+QString(" ")+ApplicationConfig->ApplicationVersion;

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
        for (int j=0;j<MusicTrack->NbrPacketForFPS;j++) MusicTrack->AppendNullSoundPacket(Position);
        return;
    }

    qlonglong     StartPosition=0;
    cMusicObject  *CurMusic=GetMusicObject(Column,StartPosition); // Get current music file from column and position
    if (CurMusic==NULL) {
        for (int j=0;j<MusicTrack->NbrPacketForFPS;j++) MusicTrack->AppendNullSoundPacket(Position);
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
                double  PctDone  =ComputePCT(SPEEDWAVE_SINQUARTER,double(Position)/double(List[Column]->TransitionDuration));
                double  AncReduce=List[Column-1]->MusicPause?0:List[Column-1]->MusicReduceVolume?List[Column-1]->MusicReduceFactor:1;
                double  NewReduce=List[Column]->MusicPause?0:List[Column]->MusicReduceVolume?List[Column]->MusicReduceFactor:1;
                double  ReduceFactor=AncReduce+(NewReduce-AncReduce)*PctDone;
                Factor*=ReduceFactor;
            } else Factor=Factor*List[Column]->MusicReduceFactor;
        }

        // Get more music bloc at correct position (volume is always 100% @ this point !)
        CurMusic->ImageAt(PreviewMode,Position+StartPosition,MusicTrack,false,1,true,false);

        // Apply correct volume to block in queue
        if (Factor!=1.0) for (int i=0;i<MusicTrack->NbrPacketForFPS;i++) MusicTrack->ApplyVolume(i,Factor);
    }
    // Ensure we have enought data
    //while (MusicTrack->List.count()<MusicTrack->NbrPacketForFPS) MusicTrack->AppendNullSoundPacket();
}

//============================================================================================
// Function use directly or with thread to prepare an image number Column at given position
//  Extend=amout of padding (top and bottom) for cinema mode with DVD
//  IsCurrentObject : If true : prepare CurrentObject - If false : prepare Transition Object
//============================================================================================

void ComputeCompositionObjectContext(cCompositionObjectContext &PreparedBrush) {
    PreparedBrush.Compute();
}

void cDiaporama::PrepareImage(cDiaporamaObjectInfo *Info,int W,int H,bool IsCurrentObject,bool PreviewMode,bool AddStartPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:PrepareImage");

    bool SoundOnly=((W==0)&&(H==0));  // W and H = 0 when producing sound track in render process

    // return immediatly if we have image
    if (((!SoundOnly)&&(IsCurrentObject)&&(Info->CurrentObject_PreparedImage!=NULL)) ||
        ((!SoundOnly)&&(!IsCurrentObject)&&(Info->TransitObject_PreparedImage!=NULL))) return;

    qlonglong           Duration            =IsCurrentObject?Info->CurrentObject_ShotDuration:Info->TransitObject_ShotDuration;
    cDiaporamaShot      *CurShot            =IsCurrentObject?Info->CurrentObject_CurrentShot:Info->TransitObject_CurrentShot;
    cDiaporamaObject    *CurObject          =IsCurrentObject?Info->CurrentObject:Info->TransitObject;
    int                 CurTimePosition     =(IsCurrentObject?Info->CurrentObject_InObjectTime:Info->TransitObject_InObjectTime);
    cSoundBlockList     *SoundTrackMontage  =(IsCurrentObject?Info->CurrentObject_SoundTrackMontage:Info->TransitObject_SoundTrackMontage);
    int                 ObjectNumber        =IsCurrentObject?Info->CurrentObject_Number:Info->TransitObject_Number;
    int                 ShotNumber          =IsCurrentObject?Info->CurrentObject_ShotSequenceNumber:Info->TransitObject_ShotSequenceNumber;
    cDiaporamaShot      *PreviousShot       =(ShotNumber>0?List[ObjectNumber]->List[ShotNumber-1]:NULL);
    QImage              *Image              =NULL;

    if (SoundOnly) {
        // if sound only then parse all shot objects to create SoundTrackMontage
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

                CurShot->ShotComposition.List[j]->DrawCompositionObject(NULL,double(H)/double(1080),0,0,true,VideoPosition+StartPosToAdd,SoundTrackMontage,1,1,NULL,false,CurShot->StaticDuration,true);
            }
        }

    } else {

        Image=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
        if ((!Image)||(Image->isNull())) return;

        // Prepare a transparent image
        QPainter P;
        P.begin(Image);
        P.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
        P.setCompositionMode(QPainter::CompositionMode_Source);
        P.fillRect(0,0,W,H,Qt::transparent);
        P.setCompositionMode(QPainter::CompositionMode_SourceOver);

        if (List.count()>0) {
            QList<cCompositionObjectContext> PreparedBrushList;

            // Construct collection
            for (int j=0;j<CurShot->ShotComposition.List.count();j++)
                PreparedBrushList.append(cCompositionObjectContext(j,PreviewMode,IsCurrentObject,Info,W,H,CurShot,PreviousShot,SoundTrackMontage,AddStartPos,Duration));

            // Compute each item of the collection
            QFuture<void> DoCompute = QtConcurrent::map(PreparedBrushList,ComputeCompositionObjectContext);
            if (DoCompute.isRunning()) DoCompute.waitForFinished();

            // Draw collection
            for (int j=0;j<CurShot->ShotComposition.List.count();j++)
                CurShot->ShotComposition.List[j]->DrawCompositionObject(&P,double(H)/double(1080),W,H,PreparedBrushList[j].PreviewMode,PreparedBrushList[j].VideoPosition+PreparedBrushList[j].StartPosToAdd,
                                                                        PreparedBrushList[j].SoundTrackMontage,
                                                                        PreparedBrushList[j].BlockPctDone,PreparedBrushList[j].ImagePctDone,
                                                                        PreparedBrushList[j].PrevCompoObject,false,Duration,
                                                                        true,false,0,0,0,0,false,&PreparedBrushList[j]);
            PreparedBrushList.clear();
        }
        P.end();

        if (IsCurrentObject) Info->CurrentObject_PreparedImage=Image; else Info->TransitObject_PreparedImage=Image;
    }
}

//=============================================================================================================================
// Function use directly or with thread to make assembly of background and images and make mix (sound & music) when transition
//=============================================================================================================================
void cDiaporama::DoAssembly(double PCT,cDiaporamaObjectInfo *Info,int W,int H,QImage::Format QTFMT) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:DoAssembly");

    if ((Info->RenderedImage!=NULL)||((W==0)&&(H==0))) return;    // return immediatly if we already have image or if sound only

    QImage   *Image=new QImage(W,H,QTFMT);
    QPainter P;

    P.begin(Image);
    P.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);

    // Draw background
    if ((Info->IsTransition)&&((Info->CurrentObject_Number==0)||(Info->CurrentObject_BackgroundIndex!=Info->TransitObject_BackgroundIndex))) {
        double Opacity;
        if ((Info->TransitObject)&&(Info->TransitObject_PreparedBackground)) {
            Opacity=1-ComputePCT(Info->CurrentObject->GetSpeedWave(),Info->TransitionPCTDone);
            P.setOpacity(Opacity);
            P.drawImage(0,0,*Info->TransitObject_PreparedBackground);
        }
        if (Info->CurrentObject_PreparedBackground) {
            Opacity=ComputePCT(Info->CurrentObject->GetSpeedWave(),Info->TransitionPCTDone);
            if (Info->TransitObject) P.setOpacity(Opacity);
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
        DoTransition(Info->TransitionFamilly,Info->TransitionSubType,PCT,Info->TransitObject_PreparedImage,Info->CurrentObject_PreparedImage,&P,W,H);
    } else if (Info->CurrentObject_PreparedImage!=NULL) P.drawImage(0,0,*Info->CurrentObject_PreparedImage);
    P.end();
    Info->RenderedImage=Image;
}

//============================================================================================
// Function use directly or with thread to load source image
// Assume SourceImage is null
// Produce sound only if W and H=0
//============================================================================================

void cDiaporama::LoadSources(cDiaporamaObjectInfo *Info,int W,int H,bool PreviewMode,bool AddStartPos) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cDiaporama:LoadSources");
    if (!Info->CurrentObject) return;

    QFutureWatcher<void> ThreadPrepareCurrentMusicBloc;
    QFutureWatcher<void> ThreadPrepareTransitMusicBloc;

    // W and H = 0 when producing sound track in render process
    bool SoundOnly=((W==0)&&(H==0));

    //==============> Music track part

    if ((Info->CurrentObject)&&(Info->CurrentObject_MusicTrack))
        ThreadPrepareCurrentMusicBloc.setFuture(QtConcurrent::run(this,&cDiaporama::PrepareMusicBloc,PreviewMode,Info->CurrentObject_Number,Info->CurrentObject_InObjectTime,Info->CurrentObject_MusicTrack));

    if ((Info->TransitObject)&&(Info->TransitObject_MusicTrack))
        ThreadPrepareTransitMusicBloc.setFuture(QtConcurrent::run(this,&cDiaporama::PrepareMusicBloc,PreviewMode,Info->TransitObject_Number,Info->TransitObject_InObjectTime,Info->TransitObject_MusicTrack));

    //==============> Image part

    // Transition Object if a previous was not keep !
    if (Info->IsTransition) {
        if (Info->TransitObject) {
            PrepareImage(Info,W,H,false,PreviewMode,AddStartPos);
            #ifdef Q_OS_WIN
            QApplication::processEvents();  //==============> Special case for windows because of windows threading method
            #endif
        } else if (!Info->TransitObject_PreparedImage) {
            Info->TransitObject_PreparedImage=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
            QPainter P;
            P.begin(Info->TransitObject_PreparedImage);
            P.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform|QPainter::HighQualityAntialiasing|QPainter::NonCosmeticDefaultPen);
            //P.setCompositionMode(QPainter::CompositionMode_Source);
            P.fillRect(0,0,W,H,Qt::black);//Qt::transparent);
            //P.setCompositionMode(QPainter::CompositionMode_SourceOver);
            P.end();
        }
    }

    // Load Source image
    PrepareImage(Info,W,H,true,PreviewMode,AddStartPos);
    #ifdef Q_OS_WIN
    QApplication::processEvents();  //==============> Special case for windows because of windows threading method
    #endif

    //==============> Background part

    if (!SoundOnly) {
        // Search background context for CurrentObject if a previous was not keep !
        if (Info->CurrentObject_BackgroundBrush==NULL) {
            if ((Info->CurrentObject_BackgroundIndex>=List.count())||(List[Info->CurrentObject_BackgroundIndex]->BackgroundType==false))
                     Info->CurrentObject_BackgroundBrush=new QBrush(Qt::black);   // If no background definition @ first object
                else Info->CurrentObject_BackgroundBrush=List[Info->CurrentObject_BackgroundIndex]->BackgroundBrush->GetBrush(QRectF(0,0,W,H),PreviewMode,0,NULL,1,NULL);
            Info->CurrentObject_PreparedBackground=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
            QPainter P;
            P.begin(Info->CurrentObject_PreparedBackground);
            if (Info->CurrentObject_BackgroundBrush) P.fillRect(QRect(0,0,W,H),*Info->CurrentObject_BackgroundBrush);
                else                                 P.fillRect(QRect(0,0,W,H),QBrush(Qt::black));
            P.end();
        }
        // same job for Transition Object if a previous was not keep !
        if ((Info->TransitObject)&&(Info->TransitObject_BackgroundBrush==NULL)) {
            if ((Info->TransitObject_BackgroundIndex>=List.count())||(List[Info->TransitObject_BackgroundIndex]->BackgroundType==false))
                     Info->TransitObject_BackgroundBrush=new QBrush(Qt::black);   // If no background definition @ first object
                else Info->TransitObject_BackgroundBrush=List[Info->TransitObject_BackgroundIndex]->BackgroundBrush->GetBrush(QRectF(0,0,W,H),PreviewMode,0,NULL,1,NULL);
            Info->TransitObject_PreparedBackground=new QImage(W,H,QImage::Format_ARGB32_Premultiplied);
            QPainter P;
            P.begin(Info->TransitObject_PreparedBackground);
            if (Info->TransitObject_BackgroundBrush) P.fillRect(QRect(0,0,W,H),*Info->TransitObject_BackgroundBrush);
                else                                 P.fillRect(QRect(0,0,W,H),QBrush(Qt::black));
            P.end();
        }
    }

    //==============> Mixing of music and soundtrack
    QTime b;
    b.start();

    if (ThreadPrepareCurrentMusicBloc.isRunning()) ThreadPrepareCurrentMusicBloc.waitForFinished();
    if (ThreadPrepareTransitMusicBloc.isRunning()) ThreadPrepareTransitMusicBloc.waitForFinished();

    // Special case to clear music buffer if not transition and music of CurrentObject is in pause mode
    if ((!Info->IsTransition)&&(Info->CurrentObject)&&(Info->CurrentObject_MusicTrack)&&(Info->CurrentObject->MusicPause)&&(Info->CurrentObject_MusicTrack->List.count()>0)) {
        Info->CurrentObject_MusicTrack->ClearList();
        for (int i=0;i<Info->CurrentObject_MusicTrack->NbrPacketForFPS;i++) Info->CurrentObject_MusicTrack->AppendNullSoundPacket(Info->CurrentObject_StartTime+Info->CurrentObject_InObjectTime);
    }

    // Soundtrack mix with fade in/fade out
    if ((Info->IsTransition)&&((Info->CurrentObject_SoundTrackMontage)||(Info->TransitObject_SoundTrackMontage))) {

        if (Info->CurrentObject_SoundTrackMontage==NULL) {
            // if we don't have a CurrentObject_SoundTrackMontage, we need to create it because it's the destination !
            Info->CurrentObject_SoundTrackMontage=new cSDLSoundBlockList();
            Info->CurrentObject_SoundTrackMontage->SetFPS(Info->FrameDuration,2,Info->TransitObject_SoundTrackMontage->SamplingRate,AV_SAMPLE_FMT_S16);
        }
        // Ensure this track have enough data
        while (Info->CurrentObject_SoundTrackMontage->List.count()<Info->CurrentObject_SoundTrackMontage->NbrPacketForFPS)
            Info->CurrentObject_SoundTrackMontage->AppendNullSoundPacket(Info->CurrentObject_StartTime+Info->CurrentObject_InObjectTime);

        // Mix current and transit SoundTrackMontage (if needed)
        // @ the end: only current SoundTrackMontage exist !
        for (int i=0;i<Info->CurrentObject_SoundTrackMontage->NbrPacketForFPS;i++) {
            // Mix the 2 sources buffer using the first buffer as destination and remove one paquet from the TransitObject_SoundTrackMontage
            int16_t *Paquet=Info->TransitObject_SoundTrackMontage?Info->TransitObject_SoundTrackMontage->DetachFirstPacket():NULL;
            int32_t mix;
            int16_t *Buf1=Info->CurrentObject_SoundTrackMontage->List[i];
            int     Max=Info->CurrentObject_SoundTrackMontage->SoundPacketSize/(Info->CurrentObject_SoundTrackMontage->SampleBytes*Info->CurrentObject_SoundTrackMontage->Channels);

            double  FadeAdjust   =sin(1.5708*double(Info->CurrentObject_InObjectTime+(double(i)/double(Info->CurrentObject_SoundTrackMontage->NbrPacketForFPS))*double(Info->FrameDuration))/double(Info->TransitionDuration));
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
    // Mix the 2 sources buffer using the first buffer as destination and remove one paquet from the PreviousMusicTrack
    if ((Info->IsTransition)&&(Info->TransitObject_MusicTrack)&&(Info->TransitObject_MusicTrack->List.count()>0))
        for (int i=0;i<Info->CurrentObject_MusicTrack->NbrPacketForFPS;i++) {
            int16_t *Paquet=Info->TransitObject_MusicTrack->DetachFirstPacket();
            // Ensure paquet exist, elsewhere create one and init it to 0 (silence)
            if (!Paquet) {
                Paquet=(int16_t *)av_malloc(Info->TransitObject_MusicTrack->SoundPacketSize+8);
                memset((u_int8_t *)Paquet,0,Info->TransitObject_MusicTrack->SoundPacketSize+8);
            }
            int32_t mix;
            int16_t *Buf1=Info->CurrentObject_MusicTrack->List[i];
            int     Max=Info->CurrentObject_MusicTrack->SoundPacketSize/(Info->CurrentObject_MusicTrack->SampleBytes*Info->CurrentObject_MusicTrack->Channels);
            double  FadeAdjust   =sin(1.5708*double(Info->CurrentObject_InObjectTime+(double(i)/double(Info->CurrentObject_MusicTrack->NbrPacketForFPS))*double(Info->FrameDuration))/double(Info->TransitionDuration));
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

//============================================================================================
void cDiaporama::CloseUnusedLibAv(int CurrentCell) {
    // Parse all unused slide to close unused libav buffer, codec, ...
    for (int i=0;i<List.count();i++) {
        /*for (int j=0;j<List[i]->ObjectComposition.List.count();j++) {
            if (List[i]->ObjectComposition.List[j]->BackgroundBrush->Video!=NULL)
                List[i]->ObjectComposition.List[j]->BackgroundBrush->Video->CacheImage.clear();
        }*/
        if ((i<CurrentCell-1)||(i>CurrentCell+1)) {
            for (int j=0;j<List[i]->ObjectComposition.List.count();j++)
                if ((List[i]->ObjectComposition.List[j]->BackgroundBrush->Video!=NULL)&&(List[i]->ObjectComposition.List[j]->BackgroundBrush->Video->LibavFile!=NULL))
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
    CurrentObject_ShotDuration          =PreviousFrame->CurrentObject_ShotDuration;        // Time the static shot end (if CurrentObject_CurrentShotType=SHOTTYPE_STATIC)
    CurrentObject_PCTDone               =PreviousFrame->CurrentObject_PCTDone;
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
    TransitObject_Number                =PreviousFrame->TransitObject_Number;               // Object number
    TransitObject_StartTime             =PreviousFrame->TransitObject_StartTime;            // Position (in msec) of the first frame relative to the diaporama
    TransitObject_InObjectTime          =PreviousFrame->TransitObject_InObjectTime;         // Position (in msec) in the object
    TransitObject                       =PreviousFrame->TransitObject;                      // Link to the current object
    TransitObject_ShotSequenceNumber    =PreviousFrame->TransitObject_ShotSequenceNumber;   // Number of the shot sequence in the current object
    TransitObject_CurrentShot           =PreviousFrame->TransitObject_CurrentShot;          // Link to the current shot in the current object
    TransitObject_CurrentShotType       =PreviousFrame->TransitObject_CurrentShotType;      // Type of the current shot : Static/Mobil/Video
    TransitObject_ShotDuration          =PreviousFrame->TransitObject_ShotDuration;         // Time the static shot end (if TransitObject_CurrentShotType=SHOTTYPE_STATIC)
    TransitObject_PCTDone               =PreviousFrame->TransitObject_PCTDone;
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
    CurrentObject_ShotDuration          =0;                 // Time the static shot end (if CurrentObject_CurrentShotType=SHOTTYPE_STATIC)
    CurrentObject_PCTDone               =0;                 // PCT achevement for static shot
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
    TransitObject_Number                =0;                 // Object number
    TransitObject_StartTime             =0;                 // Position (in msec) of the first frame relative to the diaporama
    TransitObject_InObjectTime          =0;                 // Position (in msec) in the object
    TransitObject                       =NULL;              // Link to the current object
    TransitObject_ShotSequenceNumber    =0;                 // Number of the shot sequence in the current object
    TransitObject_CurrentShot           =NULL;              // Link to the current shot in the current object
    TransitObject_CurrentShotType       =0;                 // Type of the current shot : Static/Mobil/Video
    TransitObject_ShotDuration          =0;                 // Time the static shot end (if TransitObject_CurrentShotType=SHOTTYPE_STATIC)
    TransitObject_PCTDone               =0;                 // PCT achevement for static shot
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
            CurrentObject_PCTDone=(double(CurrentObject_InObjectTime)-double(CurPos))/(double(CurrentObject_ShotDuration));

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
            TransitionPCTDone =double(CurrentObject_InObjectTime)/double(TransitionDuration);

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
                TransitObject_PCTDone=(double(TransitObject_InObjectTime)-double(CurPos))/(double(TransitObject_ShotDuration));

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
