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

/* Shape form to add :
    - PARALLÉLOGRAMME
    - TRAPÈZE
*/

#include "cBrushDefinition.h"

// Include some common various class
#include "../fmt_filters/fmt_filters.h"

//============================================
// Global static
//============================================

QBrush  Transparent;        // Transparent brush

#define PI              3.14159265
#define COSSIN45        0.707106781
#define OPTION_UP       0x01
#define OPTION_DOWN     0x02
#define OPTION_RIGHT    0x04
#define OPTION_LEFT     0x08
#define OPTION_ALL      0x0F

QList<cShapeFormDefinition> ShapeFormDefinition;

cShapeFormDefinition::cShapeFormDefinition(bool Enable,QString Name) {
    this->Enable=Enable;
    this->Name=Name;
}

// Utility function to init shape collection and translate shape names
void ShapeFormDefinitionInit() {
    ShapeFormDefinition.append(cShapeFormDefinition(false,QApplication::translate("Shape forms","No shape")));                       // SHAPEFORM_NOSHAPE
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Rectangle")));                      // SHAPEFORM_RECTANGLE
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Rounded rectangle")));              // SHAPEFORM_ROUNDRECT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Bubble")));                         // SHAPEFORM_BUBBLE
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Ellipse")));                        // SHAPEFORM_ELLIPSE
    ShapeFormDefinition.append(cShapeFormDefinition(false,QApplication::translate("Shape forms","Old Triangle up version")));        // SHAPEFORM_TRIANGLEUP
    ShapeFormDefinition.append(cShapeFormDefinition(false,QApplication::translate("Shape forms","Old Triangle right version")));     // SHAPEFORM_TRIANGLERIGHT
    ShapeFormDefinition.append(cShapeFormDefinition(false,QApplication::translate("Shape forms","Old Triangle down version")));      // SHAPEFORM_TRIANGLEDOWN
    ShapeFormDefinition.append(cShapeFormDefinition(false,QApplication::translate("Shape forms","Old Triangle left version")));      // SHAPEFORM_TRIANGLELEFT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Rhombus")));                        // SHAPEFORM_RHOMBUS (Losange)
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Pentagon")));                       // SHAPEFORM_PENTAGON
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Hexagon")));                        // SHAPEFORM_HEXAGON
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Octogon")));                        // SHAPEFORM_OCTOGON
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Simple arrow up")));                // SHAPEFORM_SIMPLEARROWUP
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Simple arrow right")));             // SHAPEFORM_SIMPLEARROWRIGHT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Simple arrow down")));              // SHAPEFORM_SIMPLEARROWDOWN
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Simple arrow left")));              // SHAPEFORM_SIMPLEARROWLEFT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Heart")));                          // SHAPEFORM_HEART
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Up-Left")));                 // SHAPEFORM_PUZZLEUL
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Up-Center")));               // SHAPEFORM_PUZZLEUC
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Up-Right")));                // SHAPEFORM_PUZZLEUR
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double vertical arrows")));         // SHAPEFORM_DOUBLEARROWVERT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double horizontal arrows")));       // SHAPEFORM_DOUBLEARROWHORIZ
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double diagonal arrows 1")));       // SHAPEFORM_DOUBLEARROWDIAG1
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double diagonal arrows 2")));       // SHAPEFORM_DOUBLEARROWDIAG2
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Spade")));                          // SHAPEFORM_SPADE
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Middle-Left")));             // SHAPEFORM_PUZZLEML
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Middle-Center")));           // SHAPEFORM_PUZZLEMC
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Middle-Right")));            // SHAPEFORM_PUZZLEMR
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Right triangle Up-Left")));         // SHAPEFORM_RIGHTTRIANGLEUL
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Right triangle Up-Right")));        // SHAPEFORM_RIGHTTRIANGLEUR
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Right triangle Down-Left")));       // SHAPEFORM_RIGHTTRIANGLEDL
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Right triangle Down-Right")));      // SHAPEFORM_RIGHTTRIANGLEDR
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Club")));                           // SHAPEFORM_CLUB
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Down-Left")));               // SHAPEFORM_PUZZLEDL
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Down-Center")));             // SHAPEFORM_PUZZLEDC
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Puzzle Down-Right")));              // SHAPEFORM_PUZZLEDR
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Single triangle up")));             // SHAPEFORM_STRIANGLEUP
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Single triangle right")));          // SHAPEFORM_STRIANGLERIGHT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Single triangle down")));           // SHAPEFORM_STRIANGLEDOWN
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Single triangle left")));           // SHAPEFORM_STRIANGLELEFT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Pushed up")));                      // SHAPEFORM_PUSHEDUP
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Sharp down")));                     // SHAPEFORM_SHARPDOWN
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Four-pointed stars")));             // SHAPEFORM_STAR4
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Five-pointed stars")));             // SHAPEFORM_STAR5
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double triangle up")));             // SHAPEFORM_DTRIANGLEUP
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double triangle right")));          // SHAPEFORM_DTRIANGLERIGHT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double triangle down")));           // SHAPEFORM_DTRIANGLEDOWN
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double triangle left")));           // SHAPEFORM_DTRIANGLELEFT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double chevron up")));              // SHAPEFORM_DCHEVRONUP
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Single chevron down")));            // SHAPEFORM_SCHEVRONDOWN
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Six-pointed stars")));              // SHAPEFORM_STAR6
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Height-pointed stars")));           // SHAPEFORM_STAR8
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Pushed left")));                    // SHAPEFORM_PUSHEDLEFT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double chevron left")));            // SHAPEFORM_DCHEVRONLEFT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Single chevron left")));            // SHAPEFORM_SCHEVRONLEFT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Sharp left")));                     // SHAPEFORM_SHARPLEFT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Single chevron up")));              // SHAPEFORM_SCHEVRONUP
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double chevron down")));            // SHAPEFORM_DCHEVRONDOWN
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Six tooth gear")));                 // SHAPEFORM_GEAR6
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Eight tooth gear")));               // SHAPEFORM_GEAR8
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Sharp right")));                    // SHAPEFORM_SHARPRIGHT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Single chevron right")));           // SHAPEFORM_SCHEVRONRIGHT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Double chevron right")));           // SHAPEFORM_DCHEVRONRIGHT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Pushed right")));                   // SHAPEFORM_PUSHEDRIGHT
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Sharp up")));                       // SHAPEFORM_SHARPUP
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Pushed down")));                    // SHAPEFORM_PUSHEDDOWN
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Ten tooth gear")));                 // SHAPEFORM_GEAR10
    ShapeFormDefinition.append(cShapeFormDefinition(true, QApplication::translate("Shape forms","Twelve tooth gear")));              // SHAPEFORM_GEAR12
}

//*********************************************************************************************************************************************

cAutoFramingDef AUTOFRAMINGDEF[NBR_AUTOFRAMING];

cAutoFramingDef::cAutoFramingDef(QString ToolTip,bool IsInternal,int GeometryType) {
    this->ToolTip       =ToolTip;
    this->GeometryType  =GeometryType;
    this->IsInternal    =IsInternal;
}

void AutoFramingDefInit() {
    for (int i=0;i<NBR_AUTOFRAMING;i++) switch(i) {
        case AUTOFRAMING_CUSTOMUNLOCK   :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Custom geometry - unlocked"),false,                 AUTOFRAMING_GEOMETRY_CUSTOM);   break;
        case AUTOFRAMING_CUSTOMLOCK     :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Custom geometry - locked"),false,                   AUTOFRAMING_GEOMETRY_CUSTOM);   break;
        case AUTOFRAMING_CUSTOMIMGLOCK  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Custom size - Image geometry"),false,               AUTOFRAMING_GEOMETRY_IMAGE);    break;
        case AUTOFRAMING_CUSTOMPRJLOCK  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Custom size - Project geometry"),false,             AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_FULLMAX        :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Full image"),false,                                 AUTOFRAMING_GEOMETRY_IMAGE);    break;
        case AUTOFRAMING_FULLMIN        :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Full inner image"),true,                            AUTOFRAMING_GEOMETRY_IMAGE);    break;
        case AUTOFRAMING_HEIGHTLEFTMAX  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project height - to the left"),false,               AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTLEFTMIN  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner height - to the left"),true,          AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTMIDLEMAX :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project height - in the center"),false,             AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTMIDLEMIN :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner height - in the center"),true,        AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTRIGHTMAX :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project height - to the right"),false,              AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_HEIGHTRIGHTMIN :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner height - to the right"),true,         AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHTOPMAX    :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project width - at the top"),false,                 AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHTOPMIN    :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner width - at the top"),true,            AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHMIDLEMAX  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project width - in the middle"),false,              AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHMIDLEMIN  :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner width - in the middle"),true,         AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHBOTTOMMAX :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project width - at the bottom"),false,              AUTOFRAMING_GEOMETRY_PROJECT);  break;
        case AUTOFRAMING_WIDTHBOTTOMMIN :   AUTOFRAMINGDEF[i]=cAutoFramingDef(QApplication::translate("Framing styles","Project inner width - at the bottom"),true,         AUTOFRAMING_GEOMETRY_PROJECT);  break;
    }
}

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
// Utilities functions to compute shape
//====================================================================================================================

QList<QPolygonF> ComputePolygonRect(QRectF Rect) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    Path.moveTo(Rect.left(),Rect.top());
    Path.lineTo(Rect.right(),Rect.top());
    Path.lineTo(Rect.right(),Rect.bottom());
    Path.lineTo(Rect.left(),Rect.bottom());
    Path.lineTo(Rect.left(),Rect.top());
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputePolygonRoundRect(QRectF Rect,qreal RayX,qreal RayY) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    //Path.addRoundedRect(Rect.left(),Rect.top(),Rect.width(),Rect.height(),RayX,RayY);
    Path.moveTo(Rect.left()+RayX,Rect.top());
    Path.lineTo(Rect.right()-RayX,Rect.top());
    Path.cubicTo(QPointF(Rect.right()-RayX,Rect.top()),QPointF(Rect.right(),Rect.top()),QPointF(Rect.right(),Rect.top()+RayX));
    Path.lineTo(Rect.right(),Rect.bottom()-RayY);
    Path.cubicTo(QPointF(Rect.right(),Rect.bottom()),QPointF(Rect.right()-RayX,Rect.bottom()),QPointF(Rect.right()-RayX,Rect.bottom()));
    Path.lineTo(Rect.left()+RayX,Rect.bottom());
    Path.cubicTo(QPointF(Rect.left(),Rect.bottom()),QPointF(Rect.left(),Rect.bottom()-RayY),QPointF(Rect.left(),Rect.bottom()-RayY));
    Path.lineTo(Rect.left(),Rect.top()+RayY);
    Path.cubicTo(QPointF(Rect.left(),Rect.top()),QPointF(Rect.left()+RayX,Rect.top()),QPointF(Rect.left()+RayX,Rect.top()));
    //Path.arcTo(QRectF(Rect.left(),Rect.top(),RayX*2,RayY*2),180,-90);
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputePolygonEllipse(QRectF Rect) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    Path.moveTo(Rect.right(),Rect.center().y());
    Path.arcTo(Rect,0,  90);
    Path.arcTo(Rect,90, 90);
    Path.arcTo(Rect,180,90);
    Path.arcTo(Rect,270,90);
    //Path.addEllipse(Rect);
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeHeart(QRectF Rect) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    qreal               WRect=(Rect.width()/2)*(2/(1+COSSIN45)),HRect=2*Rect.height()/3;
    qreal               WRectH=WRect/2,HRectH=HRect/2;
    QRectF              SubRect1=QRectF(Rect.left(),                    Rect.top(), WRect,HRect);
    QRectF              SubRect2=QRectF(Rect.right()-SubRect1.width(),  Rect.top(), WRect,HRect);

    Path.moveTo(SubRect1.left()+WRectH+COSSIN45*WRectH,SubRect1.top()+HRectH-COSSIN45*HRectH);
    Path.arcTo(SubRect1,45,180);
    Path.lineTo(QPointF(Rect.center().x(),Rect.bottom()));
    Path.lineTo(SubRect2.left()+WRectH+COSSIN45*WRectH,SubRect2.top()+HRectH+COSSIN45*HRectH);
    Path.arcTo(SubRect2,-45,180);
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeSpade(QRectF Rect) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    qreal               WRect=(Rect.width()/2)*(2/(1+COSSIN45)),HRect=(2*Rect.height()/3)-(Rect.height()*0.1);
    qreal               WRectH=WRect/2,HRectH=HRect/2;
    QRectF              SubRect1=QRectF(Rect.left(),                    Rect.bottom()-HRect-(Rect.height()*0.1), WRect,HRect);
    QRectF              SubRect2=QRectF(Rect.right()-SubRect1.width(),  SubRect1.top(),                          WRect,HRect);
    QRectF              SubRect3=QRectF(Rect.left(),                    Rect.bottom()-(Rect.height()*0.4),       Rect.width()/2,Rect.height()*0.4);
    QRectF              SubRect4=QRectF(SubRect3.right(),               SubRect3.top(),                          SubRect3.width(),SubRect3.height());

    Path.moveTo(SubRect1.left()+WRectH+COSSIN45*WRectH,SubRect1.top()+HRectH+COSSIN45*HRectH);
    Path.arcTo(SubRect1,-45,-180);
    Path.lineTo(QPointF(Rect.center().x(),Rect.top()));
    Path.lineTo(SubRect2.left()+WRectH+COSSIN45*WRectH,SubRect2.top()+HRectH-COSSIN45*HRectH);
    Path.arcTo(SubRect2,45,-180);
    Path.arcTo(SubRect3,0,-90);
    Path.lineTo(SubRect4.center().x(),SubRect4.bottom());
    Path.arcTo(SubRect4,-90,-90);
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeClub(QRectF Rect) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    qreal               WRect=Rect.width()/2;
    qreal               HRect=Rect.height()/2;
    qreal               HRectH=HRect/2;
    QRectF              SubRect0=QRectF(Rect.center().x()-WRect/2,      Rect.top(),                                             WRect,HRect);
    QRectF              SubRect1=QRectF(Rect.left(),                    SubRect0.bottom()-(SubRect0.height()/2)*(1-COSSIN45),   WRect,HRect);
    QRectF              SubRect2=QRectF(Rect.right()-SubRect1.width(),  SubRect1.top(),                                         WRect,HRect);
    QRectF              SubRect3=QRectF(Rect.left(),                    SubRect1.top(),                                         Rect.width()/2,Rect.bottom()-SubRect1.top());
    QRectF              SubRect4=QRectF(SubRect3.right(),               SubRect3.top(),                                         SubRect3.width(),SubRect3.height());

    Path.moveTo(SubRect1.right(),SubRect1.top()+HRectH);
    Path.arcTo(SubRect1,0,-270);
    Path.arcTo(SubRect0,225,-270);
    Path.arcTo(SubRect2,90,-270);
    Path.arcTo(SubRect3,0,-90);
    Path.lineTo(SubRect4.center().x(),SubRect4.bottom());
    Path.arcTo(SubRect4,-90,-90);
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputePolygonR(double width,double height,double CenterX,double CenterY,int MaxPoint,double StartAngle) {
    QList<QPolygonF>    List;
    QPolygonF           Polygon;
    qreal               vcos,vsin,Angle;
    int                 i;

    Angle=StartAngle;
    for (i=0;i<MaxPoint;i++) {
        vcos=cos(Angle*3.14159265/180)*(width/2);
        vsin=sin(Angle*3.14159265/180)*(height/2);
        Polygon<<QPointF(CenterX+vcos,CenterY-vsin);
        Angle=Angle+(double(360)/MaxPoint);
        if (Angle>=360) Angle=-Angle+360;
    }
    List.append(Polygon);
    return List;
}

QList<QPolygonF> ComputeSingleChevron(QRectF Rect,int Angle,int Options) {
    QList<QPolygonF>    List;
    QPainterPath        Path;

    switch (Angle) {
        case 0 :
            if ((Options&OPTION_UP)==OPTION_UP) {
                Path.moveTo(QPointF(0,Rect.height()*0.2)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width()/2,0)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width(),Rect.height()*0.2)+QPointF(Rect.left(),Rect.top()));
            } else {
                Path.moveTo(QPointF(0,0)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width(),0)+QPointF(Rect.left(),Rect.top()));
            }
            Path.lineTo(QPointF(Rect.width(),Rect.height())+QPointF(Rect.left(),Rect.top()));
            if ((Options&OPTION_DOWN)==OPTION_DOWN) Path.lineTo(QPointF(Rect.width()/2,Rect.height()*0.8)+QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,Rect.height())+QPointF(Rect.left(),Rect.top()));
            if ((Options&OPTION_UP)==OPTION_UP) Path.lineTo(QPointF(0,Rect.height()*0.2)+QPointF(Rect.left(),Rect.top()));
                else Path.lineTo(QPointF(0,Rect.height())+QPointF(Rect.left(),Rect.top()));
            break;
        case 180 :
            if ((Options&OPTION_DOWN)==OPTION_DOWN) {
                Path.moveTo(QPointF(0,Rect.height()*0.8)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width()/2,Rect.height())+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width(),Rect.height()*0.8)+QPointF(Rect.left(),Rect.top()));
            } else {
                Path.moveTo(QPointF(0,Rect.height())+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width(),Rect.height())+QPointF(Rect.left(),Rect.top()));
            }
            Path.lineTo(QPointF(Rect.width(),0)+QPointF(Rect.left(),Rect.top()));
            if ((Options&OPTION_UP)==OPTION_UP)     Path.lineTo(QPointF(Rect.width()/2,Rect.height()*0.2)+QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,0)+QPointF(Rect.left(),Rect.top()));
            if ((Options&OPTION_DOWN)==OPTION_DOWN) Path.lineTo(QPointF(0,Rect.height()*0.8)+QPointF(Rect.left(),Rect.top()));
                else Path.lineTo(QPointF(0,Rect.height())+QPointF(Rect.left(),Rect.top()));
            break;
        case 90 :
            if ((Options&OPTION_LEFT)==OPTION_LEFT) {
                Path.moveTo(QPointF(Rect.width()*0.2,0)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(0,Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width()*0.2,Rect.height())+QPointF(Rect.left(),Rect.top()));
            } else {
                Path.moveTo(QPointF(0,0)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(0,Rect.height())+QPointF(Rect.left(),Rect.top()));
            }
            Path.lineTo(QPointF(Rect.width(),Rect.height())+QPointF(Rect.left(),Rect.top()));
            if ((Options&OPTION_RIGHT)==OPTION_RIGHT) Path.lineTo(QPointF(Rect.width()*0.8,Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),0)+QPointF(Rect.left(),Rect.top()));
            if ((Options&OPTION_LEFT)==OPTION_LEFT) Path.lineTo(QPointF(Rect.width()*0.2,0)+QPointF(Rect.left(),Rect.top()));
                else Path.lineTo(QPointF(0,0)+QPointF(Rect.left(),Rect.top()));
            break;
        case -90 :
            if ((Options&OPTION_RIGHT)==OPTION_RIGHT) {
                Path.moveTo(QPointF(Rect.width()*0.8,0)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width(),Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width()*0.8,Rect.height())+QPointF(Rect.left(),Rect.top()));
            } else {
                Path.moveTo(QPointF(Rect.width(),0)+QPointF(Rect.left(),Rect.top()));
                Path.lineTo(QPointF(Rect.width(),Rect.height())+QPointF(Rect.left(),Rect.top()));
            }
            Path.lineTo(QPointF(0,Rect.height())+QPointF(Rect.left(),Rect.top()));
            if ((Options&OPTION_LEFT)==OPTION_LEFT) Path.lineTo(QPointF(Rect.width()*0.2,Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,0)+QPointF(Rect.left(),Rect.top()));
            if ((Options&OPTION_RIGHT)==OPTION_RIGHT) Path.lineTo(QPointF(Rect.width()*0.8,0)+QPointF(Rect.left(),Rect.top()));
                else    Path.lineTo(QPointF(Rect.width(),0)+QPointF(Rect.left(),Rect.top()));
            break;
    }
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeDoubleChevron(QRectF Rect,int Angle) {
    QList<QPolygonF>    List;
    QPainterPath        Path1,Path2;
    switch (Angle) {
        case 0 :
            Path1.moveTo(QPointF(Rect.width()/2,0)                   +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width(),  Rect.height()*0.2)   +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width(),  Rect.height()*0.55)  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()/2,Rect.height()*0.35)  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,             Rect.height()*0.55)  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,             Rect.height()*0.2)   +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()/2,0)                   +QPointF(Rect.left(),Rect.top()));

            Path2.moveTo(QPointF(Rect.width()/2,Rect.height()*0.45)  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width(),  Rect.height()*0.65)  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width(),  Rect.height())       +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()/2,Rect.height()*0.8)   +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(0,             Rect.height())       +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(0,             Rect.height()*0.65)  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()/2,Rect.height()*0.45)  +QPointF(Rect.left(),Rect.top()));
            break;
        case 180 :
            Path2.moveTo(QPointF(Rect.width()/2,Rect.height())       +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width(),  Rect.height()*0.8)   +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width(),  Rect.height()*0.45)  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()/2,Rect.height()*0.65)  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(0,             Rect.height()*0.45)  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(0,             Rect.height()*0.8)   +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()/2,Rect.height())       +QPointF(Rect.left(),Rect.top()));

            Path1.moveTo(QPointF(Rect.width()/2,Rect.height()*0.55)  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width(),  Rect.height()*0.35)  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width(),  0)                   +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()/2,Rect.height()*0.2)   +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,             0)                   +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,             Rect.height()*0.35)  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()/2,Rect.height()*0.55)  +QPointF(Rect.left(),Rect.top()));
            break;
        case 90 :
            Path1.moveTo(QPointF(0,                 Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.2,  Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.55, Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.35, Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.55, 0)              +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.2,  0)              +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,                 Rect.height()/2)+QPointF(Rect.left(),Rect.top()));

            Path2.moveTo(QPointF(Rect.width()*0.45, Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.65, Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width(),      Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.8,  Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width(),      0)              +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.65, 0)              +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.45, Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            break;
        case -90 :
            Path1.moveTo(QPointF(Rect.width()*0.55, Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.35, Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,                 Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.2,  Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,                 0)              +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.35, 0)              +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width()*0.55, Rect.height()/2)+QPointF(Rect.left(),Rect.top()));

            Path2.moveTo(QPointF(Rect.width(),      Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.8,  Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.45, Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.65, Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.45, 0)              +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width()*0.8,  0)              +QPointF(Rect.left(),Rect.top()));
            Path2.lineTo(QPointF(Rect.width(),      Rect.height()/2)+QPointF(Rect.left(),Rect.top()));
            break;
    }
    List.append(Path1.toFillPolygon(QTransform()));
    List.append(Path2.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeSingleTriangle(QRectF Rect,int Angle) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    qreal               RWA=Rect.width()/2;
    qreal               RHA=Rect.height()/2;

    switch (Angle) {
        case 0 :
            Path.moveTo(QPointF(RWA, 0)         +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),   Rect.height())      +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,              Rect.height())      +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWA,            0)                  +QPointF(Rect.left(),Rect.top()));
            break;
        case 180 :
            Path.moveTo(QPointF(RWA,            Rect.height())      +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),   0)                  +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,              0)                  +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWA,            Rect.height())      +QPointF(Rect.left(),Rect.top()));
            break;
        case 90 :
            Path.moveTo(QPointF(0,              RHA)                +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),   Rect.height())      +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),   0)                  +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,              RHA)                +QPointF(Rect.left(),Rect.top()));
            break;
        case -90 :
            Path.moveTo(QPointF(Rect.width(),   RHA)                +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,              Rect.height())      +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,              0)                  +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),   RHA)                +QPointF(Rect.left(),Rect.top()));
            break;
    }
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeDoubleTriangle(QRectF Rect,int Angle) {
    QList<QPolygonF>    List;
    QPainterPath        Path1,Path2;
    qreal               RWA=Rect.width()/2;
    qreal               RHA=Rect.height()/2;

    switch (Angle) {
        case 0 :
            Path1.moveTo(QPointF(RWA, 0)                            +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width(),   RHA)               +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,              RHA)               +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(RWA,            0)                 +QPointF(Rect.left(),Rect.top()));
            Path2.moveTo(QPointF(RWA, 0)                            +QPointF(Rect.left(),Rect.top()+RHA));
            Path2.lineTo(QPointF(Rect.width(),   RHA)               +QPointF(Rect.left(),Rect.top()+RHA));
            Path2.lineTo(QPointF(0,              RHA)               +QPointF(Rect.left(),Rect.top()+RHA));
            Path2.lineTo(QPointF(RWA,            0)                 +QPointF(Rect.left(),Rect.top()+RHA));
            break;
        case 180 :
            Path1.moveTo(QPointF(RWA,            RHA)               +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(Rect.width(),   0)                 +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,              0)                 +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(RWA,            RHA)               +QPointF(Rect.left(),Rect.top()));
            Path2.moveTo(QPointF(RWA,            RHA)               +QPointF(Rect.left(),Rect.top()+RHA));
            Path2.lineTo(QPointF(Rect.width(),   0)                 +QPointF(Rect.left(),Rect.top()+RHA));
            Path2.lineTo(QPointF(0,              0)                 +QPointF(Rect.left(),Rect.top()+RHA));
            Path2.lineTo(QPointF(RWA,            RHA)               +QPointF(Rect.left(),Rect.top()+RHA));
            break;
        case 90 :
            Path1.moveTo(QPointF(0,              RHA)               +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(RWA,            Rect.height())     +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(RWA,            0)                 +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,              RHA)               +QPointF(Rect.left(),Rect.top()));
            Path2.moveTo(QPointF(0,              RHA)               +QPointF(Rect.left()+RWA,Rect.top()));
            Path2.lineTo(QPointF(RWA,            Rect.height())     +QPointF(Rect.left()+RWA,Rect.top()));
            Path2.lineTo(QPointF(RWA,            0)                 +QPointF(Rect.left()+RWA,Rect.top()));
            Path2.lineTo(QPointF(0,              RHA)               +QPointF(Rect.left()+RWA,Rect.top()));
            break;
        case -90 :
            Path1.moveTo(QPointF(RWA,            RHA)               +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,              Rect.height())     +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(0,              0)                 +QPointF(Rect.left(),Rect.top()));
            Path1.lineTo(QPointF(RWA,            RHA)               +QPointF(Rect.left(),Rect.top()));
            Path2.moveTo(QPointF(RWA,            RHA)               +QPointF(Rect.left()+RWA,Rect.top()));
            Path2.lineTo(QPointF(0,              Rect.height())     +QPointF(Rect.left()+RWA,Rect.top()));
            Path2.lineTo(QPointF(0,              0)                 +QPointF(Rect.left()+RWA,Rect.top()));
            Path2.lineTo(QPointF(RWA,            RHA)               +QPointF(Rect.left()+RWA,Rect.top()));
            break;
    }
    List.append(Path1.toFillPolygon(QTransform()));
    List.append(Path2.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeSimpleArrow(QRectF Rect,int Angle) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    qreal               RWA=Rect.width()/2 ,RWB=Rect.width()/3;
    qreal               RHA=Rect.height()/2,RHB=Rect.height()/3;

    switch (Angle) {
        case 0 :
            Path.moveTo(QPointF(RWA,                0)              +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),       RHB)            +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(2*RWB,              RHB)            +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(2*RWB,              Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWB,                Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWB,                RHB)            +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,                  RHB)            +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWA,                0)              +QPointF(Rect.left(),Rect.top()));
            break;
        case 180 :
            Path.moveTo(QPointF(RWA,                RHB)            +QPointF(Rect.left(),Rect.bottom()-RHB));
            Path.lineTo(QPointF(Rect.width(),       0)              +QPointF(Rect.left(),Rect.bottom()-RHB));
            Path.lineTo(QPointF(Rect.width()-RWB,   0)              +QPointF(Rect.left(),Rect.bottom()-RHB));
            Path.lineTo(QPointF(Rect.width()-RWB,   0)              +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWB,                0)              +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWB,                0)              +QPointF(Rect.left(),Rect.bottom()-RHB));
            Path.lineTo(QPointF(0,                  0)              +QPointF(Rect.left(),Rect.bottom()-RHB));
            Path.lineTo(QPointF(RWA,                RHB)            +QPointF(Rect.left(),Rect.bottom()-RHB));
            break;
        case 90 :
            Path.moveTo(QPointF(0,                  RHA)            +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWB,                Rect.height())  +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWB,                2*RHB)          +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),       2*RHB)          +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.width(),       RHB)            +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWB,                RHB)            +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(RWB,                0)              +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,                  RHA)            +QPointF(Rect.left(),Rect.top()));
            break;
        case -90 :
            Path.moveTo(QPointF(RWB,                RHA)            +QPointF(Rect.right()-RWB,Rect.top()));
            Path.lineTo(QPointF(0,                  Rect.height())  +QPointF(Rect.right()-RWB,Rect.top()));
            Path.lineTo(QPointF(0,                  2*RHB)          +QPointF(Rect.right()-RWB,Rect.top()));
            Path.lineTo(QPointF(0,                  2*RHB)          +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,                  RHB)            +QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(0,                  RHB)            +QPointF(Rect.right()-RWB,Rect.top()));
            Path.lineTo(QPointF(0,                  0)              +QPointF(Rect.right()-RWB,Rect.top()));
            Path.lineTo(QPointF(RWB,                RHA)            +QPointF(Rect.right()-RWB,Rect.top()));
            break;
    }
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeDoubleArrow(QRectF Rect,int Angle) {
    QList<QPolygonF>    List;
    QPointF             Table[10];
    QPainterPath        Path;
    qreal               Ray=(sqrt(qreal(Rect.width())*qreal(Rect.width())+qreal(Rect.height())*qreal(Rect.height())))/2;
    qreal               RB=2*Ray/3;

    switch (Angle) {
        case 0:
            Table[0] =QPointF(Ray,      0);
            Table[1] =QPointF(2*Ray,    RB);
            Table[2] =QPointF(2*RB,     RB);
            Table[3] =QPointF(2*RB,     2*RB);
            Table[4] =QPointF(2*Ray,    2*RB);
            Table[5] =QPointF(Ray,      2*Ray);
            Table[6] =QPointF(0,        2*RB);
            Table[7] =QPointF(RB,       2*RB);
            Table[8] =QPointF(RB,       RB);
            Table[9] =QPointF(0,        RB);
            Table[10]=QPointF(Ray,      0);
            break;
        case 90:
            Table[0] =QPointF(0,        Ray);
            Table[1] =QPointF(RB,       2*Ray);
            Table[2] =QPointF(RB,       2*RB);
            Table[3] =QPointF(2*RB,     2*RB);
            Table[4] =QPointF(2*RB,     2*Ray);
            Table[5] =QPointF(2*Ray,    Ray);
            Table[6] =QPointF(2*RB,     0);
            Table[7] =QPointF(2*RB,     RB);
            Table[8] =QPointF(RB,       RB);
            Table[9] =QPointF(RB,       0);
            Table[10]=QPointF(0,        Ray);
            break;
        case 45:
            Table[0] =QPointF(2*Ray,    0);
            Table[1] =QPointF(2*Ray,    Ray);
            Table[2] =QPointF(Ray+RB,   RB);
            Table[3] =QPointF(RB,       Ray+RB);
            Table[4] =QPointF(Ray,      2*Ray);
            Table[5] =QPointF(0,        2*Ray);
            Table[6] =QPointF(0,        Ray);
            Table[7] =QPointF(RB/2,     Ray+RB/2);
            Table[8] =QPointF(Ray+RB/2, RB/2);
            Table[9] =QPointF(Ray,      0);
            Table[10]=QPointF(2*Ray,    0);
            break;
        case -45:
            Table[0] =QPointF(0,        0);
            Table[1] =QPointF(Ray,      0);
            Table[2] =QPointF(RB,       RB/2);
            Table[3] =QPointF(Ray+RB,   Ray+RB/2);
            Table[4] =QPointF(2*Ray,    Ray);
            Table[5] =QPointF(2*Ray,    2*Ray);
            Table[6] =QPointF(Ray,      2*Ray);
            Table[7] =QPointF(Ray+RB/2, Ray+RB);
            Table[8] =QPointF(RB/2,     RB);
            Table[9] =QPointF(0,        Ray);
            Table[10]=QPointF(0,        0);
            break;
    }
    //Scale
    qreal x1=Table[0].x();
    qreal y1=Table[0].y();
    qreal x2=Table[0].x();
    qreal y2=Table[0].y();
    for (int i=1;i<11;i++) {
        if (Table[i].x()<x1) x1=Table[i].x();
        if (Table[i].y()<y1) y1=Table[i].y();
        if (Table[i].x()>x2) x2=Table[i].x();
        if (Table[i].y()>y2) y2=Table[i].y();
    }
    qreal ScaleX=Rect.width()/(x2-x1);
    qreal ScaleY=Rect.height()/(y2-y1);
    Path.moveTo(QPointF(Table[0].x()*ScaleX,Table[0].y()*ScaleY)+QPointF(Rect.left(),Rect.top()));
    for (int i=1;i<11;i++) Path.lineTo(QPointF(Table[i].x()*ScaleX,Table[i].y()*ScaleY)+QPointF(Rect.left(),Rect.top()));

    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeStar(QRectF Rect,int Pointed) {
    QList<QPolygonF>    List;
    QPointF             Table[100];
    QPainterPath        Path;
    qreal               Segment=(180/Pointed),CurAngle=-90;
    qreal               Ray1=(sqrt(qreal(Rect.width())*qreal(Rect.width())+qreal(Rect.height())*qreal(Rect.height())))/2;
    qreal               Ray2=Ray1/2;

    //Compute star
    for (int i=0;i<=Pointed;i++) {
        Table[i*2]=QPointF(cos(CurAngle*PI/180)*Ray1+Ray1,sin(CurAngle*PI/180)*Ray1+Ray1);
        CurAngle=CurAngle+Segment;
        Table[i*2+1]=QPointF(cos(CurAngle*PI/180)*Ray2+Ray1,sin(CurAngle*PI/180)*Ray2+Ray1);
        CurAngle=CurAngle+Segment;
    }

    //Scale
    qreal x1=Table[0].x();
    qreal y1=Table[0].y();
    qreal x2=Table[0].x();
    qreal y2=Table[0].y();
    for (int i=1;i<=Pointed*2;i++) {
        if (Table[i].x()<x1) x1=Table[i].x();
        if (Table[i].y()<y1) y1=Table[i].y();
        if (Table[i].x()>x2) x2=Table[i].x();
        if (Table[i].y()>y2) y2=Table[i].y();
    }
    qreal ScaleX=Rect.width()/(x2-x1);
    qreal ScaleY=Rect.height()/(y2-y1);
    Path.moveTo(QPointF(Table[0].x()*ScaleX,Table[0].y()*ScaleY)+QPointF(Rect.left(),Rect.top()));
    for (int i=1;i<Pointed*2;i++) Path.lineTo(QPointF(Table[i].x()*ScaleX,Table[i].y()*ScaleY)+QPointF(Rect.left(),Rect.top()));

    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeGear(QRectF Rect,int tooth) {
    QList<QPolygonF>    List;
    QPointF             Table[100];
    QPainterPath        Path;
    qreal               Segment=(180/tooth),CurAngle=90;
    qreal               PartSegment=Segment/10;
    qreal               Ray1=(sqrt(qreal(Rect.width())*qreal(Rect.width())+qreal(Rect.height())*qreal(Rect.height())))/2;
    qreal               Ray2=Ray1*0.8;

    //Compute gear
    for (int i=0;i<=tooth;i++) {
        Table[i*4]=QPointF(cos(CurAngle*PI/180)*Ray1+Ray1,sin(CurAngle*PI/180)*Ray1+Ray1);
        CurAngle=CurAngle+PartSegment;
        Table[i*4+1]=QPointF(cos(CurAngle*PI/180)*Ray1+Ray1,sin(CurAngle*PI/180)*Ray1+Ray1);
        CurAngle=CurAngle+Segment-PartSegment;
        Table[i*4+2]=QPointF(cos(CurAngle*PI/180)*Ray2+Ray1,sin(CurAngle*PI/180)*Ray2+Ray1);
        CurAngle=CurAngle+PartSegment;
        Table[i*4+3]=QPointF(cos(CurAngle*PI/180)*Ray2+Ray1,sin(CurAngle*PI/180)*Ray2+Ray1);
        CurAngle=CurAngle+Segment-PartSegment;
    }

    //Scale
    qreal x1=Table[0].x();
    qreal y1=Table[0].y();
    qreal x2=Table[0].x();
    qreal y2=Table[0].y();
    for (int i=1;i<=tooth*4;i++) {
        if (Table[i].x()<x1) x1=Table[i].x();
        if (Table[i].y()<y1) y1=Table[i].y();
        if (Table[i].x()>x2) x2=Table[i].x();
        if (Table[i].y()>y2) y2=Table[i].y();
    }
    qreal ScaleX=Rect.width()/(x2-x1);
    qreal ScaleY=Rect.height()/(y2-y1);
    Path.moveTo(QPointF(Table[0].x()*ScaleX,Table[0].y()*ScaleY)+QPointF(Rect.left(),Rect.top()));
    for (int i=1;i<tooth*4;i++) Path.lineTo(QPointF(Table[i].x()*ScaleX,Table[i].y()*ScaleY)+QPointF(Rect.left(),Rect.top()));

    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputeRightTriangle(QRectF Rect,int Angle) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    switch (Angle) {
        case 0:
            Path.moveTo(QPointF(Rect.left(),Rect.bottom()));
            Path.lineTo(QPointF(Rect.right(),Rect.top()));
            Path.lineTo(QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.left(),Rect.bottom()));
            break;
        case 1:
            Path.moveTo(QPointF(Rect.right(),Rect.bottom()));
            Path.lineTo(QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.right(),Rect.top()));
            Path.lineTo(QPointF(Rect.right(),Rect.bottom()));
            break;
        case 2 :
            Path.moveTo(QPointF(Rect.left(),Rect.top()));
            Path.lineTo(QPointF(Rect.right(),Rect.bottom()));
            Path.lineTo(QPointF(Rect.left(),Rect.bottom()));
            Path.lineTo(QPointF(Rect.left(),Rect.top()));
            break;
        case 3 :
            Path.moveTo(QPointF(Rect.right(),Rect.top()));
            Path.lineTo(QPointF(Rect.left(),Rect.bottom()));
            Path.lineTo(QPointF(Rect.right(),Rect.bottom()));
            Path.lineTo(QPointF(Rect.right(),Rect.top()));
            break;
    }
    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

QList<QPolygonF> ComputePuzzle(QRectF Rect,int Forme) {
    QList<QPolygonF>    List;
    QPainterPath        Path;
    qreal               QuarterW=Rect.width()/4, COSSINW=COSSIN45*(QuarterW/2),COSSINiW=(1-COSSIN45)*(QuarterW/2);
    qreal               QuarterH=Rect.height()/4,COSSINH=COSSIN45*(QuarterH/2),COSSINiH=(1-COSSIN45)*(QuarterH/2);
    qreal               RullerX =Rect.left()+(Rect.width()-(QuarterW/2)-COSSINW)/2;
    qreal               RullerY =Rect.top()+(Rect.height()-(QuarterH/2)-COSSINH)/2;
    /*QRectF              NewRect=QRectF(Rect.left(),Rect.top(),
                                       (Forme&OPTION_RIGHT)==OPTION_RIGHT?Rect.width()-QuarterW+COSSINiW:Rect.width(),
                                       (Forme&OPTION_DOWN) ==OPTION_DOWN?Rect.height()-(QuarterH/2)-COSSINH:Rect.height());*/
    QRectF              NewRect=QRectF(Rect.left(),Rect.top(),Rect.width()-QuarterW+COSSINiW,Rect.height()-(QuarterH/2)-COSSINH);
    QRectF              RectUp   =QRectF(RullerX-QuarterW/2,        NewRect.top()-COSSINiH,     QuarterW,QuarterH);
    QRectF              RectDown =QRectF(RullerX-QuarterW/2,        NewRect.bottom()-COSSINiH,  QuarterW,QuarterH);
    QRectF              RectLeft =QRectF(NewRect.left()-COSSINiW,   RullerY-QuarterH/2,         QuarterW,QuarterH);
    QRectF              RectRight=QRectF(NewRect.right()-COSSINiW,  RullerY-QuarterH/2,         QuarterW,QuarterH);

    // Up
    Path.moveTo(QPointF(NewRect.left(),NewRect.top()));
    if ((Forme&OPTION_UP)==OPTION_UP) {
        Path.lineTo(QPointF(RectUp.left()-COSSINiW,NewRect.top()));
        Path.arcTo(RectUp,90+45,90+90+90);
    }
    Path.lineTo(QPointF(NewRect.right(),NewRect.top()));

    // Right
    if ((Forme&OPTION_RIGHT)==OPTION_RIGHT) {
        Path.lineTo(QPointF(RectRight.left()+COSSINiW,RullerY-COSSINH));
        Path.arcTo(RectRight,90+45,-90-90-90);
    }
    Path.lineTo(QPointF(NewRect.right(),NewRect.bottom()));

    // Down
    if ((Forme&OPTION_DOWN)==OPTION_DOWN) {
        Path.lineTo(QPointF(RectDown.right()-COSSINiW,NewRect.bottom()));
        Path.arcTo(RectDown,45,-90-90-90);
    }
    Path.lineTo(QPointF(NewRect.left(),NewRect.bottom()));

    // Left
    if ((Forme&OPTION_LEFT)==OPTION_LEFT) {
        Path.lineTo(QPointF(NewRect.left(),RectLeft.bottom()-COSSINiH));
        Path.arcTo(RectLeft,90+90+45,90+90+90);
    }
    Path.lineTo(QPointF(NewRect.left(),NewRect.top()));

    List.append(Path.toFillPolygon(QTransform()));
    return List;
}

// Utilities functions to compute a polygon for a given form

QList<QPolygonF> ComputePolygon(int BackgroundForm,qreal left,qreal top,qreal width,qreal height,qreal CenterX,qreal CenterY) {
    qreal RayX=0,RayY=0;
    switch (BackgroundForm) {
        case SHAPEFORM_ROUNDRECT : // Round rect
            RayX=width/10;     if (RayX>16) RayX=16; else if (RayX<2)  RayX=2;
            RayY=height/10;    if (RayY>16) RayY=16; else if (RayY<2)  RayY=2;
            return ComputePolygonRoundRect(QRectF(left,top,width,height),RayX,RayY);
            break;
        case SHAPEFORM_BUBBLE : // Bubble
            RayX=2*width/10;   if (RayX<4)  RayX=4;
            RayY=2*height/10;  if (RayY<4)  RayY=4;
            return ComputePolygonRoundRect(QRectF(left,top,width,height),RayX,RayY);
            break;
        case SHAPEFORM_ELLIPSE          : return ComputePolygonEllipse(QRectF(left,top,width,height));                                          break;  // Ellipse
        case SHAPEFORM_TRIANGLEUP       : return ComputePolygonR(width,height,CenterX,CenterY,3,90);                                            break;  // Triangle Up
        case SHAPEFORM_TRIANGLERIGHT    : return ComputePolygonR(width,height,CenterX,CenterY,3,0);                                             break;  // Triangle Right
        case SHAPEFORM_TRIANGLEDOWN     : return ComputePolygonR(width,height,CenterX,CenterY,3,-90);                                           break;  // Triangle Down
        case SHAPEFORM_TRIANGLELEFT     : return ComputePolygonR(width,height,CenterX,CenterY,3,-180);                                          break;  // Triangle left
        case SHAPEFORM_RIGHTTRIANGLEUL  : return ComputeRightTriangle(QRectF(left,top,width,height),0);                                         break;  // Right triangle Up-Left
        case SHAPEFORM_RIGHTTRIANGLEUR  : return ComputeRightTriangle(QRectF(left,top,width,height),1);                                         break;  // Right triangle Up-Right
        case SHAPEFORM_RIGHTTRIANGLEDL  : return ComputeRightTriangle(QRectF(left,top,width,height),2);                                         break;  // Right triangle Down-Left
        case SHAPEFORM_RIGHTTRIANGLEDR  : return ComputeRightTriangle(QRectF(left,top,width,height),3);                                         break;  // Right triangle Down-Right
        case SHAPEFORM_RHOMBUS          : return ComputePolygonR(width,height,CenterX,CenterY,4,0);                                             break;  // Losange
        case SHAPEFORM_PENTAGON         : return ComputePolygonR(width,height,CenterX,CenterY,5,90-(double(360)/5));                            break;  // Pentagone
        case SHAPEFORM_HEXAGON          : return ComputePolygonR(width,height,CenterX,CenterY,6,-(double(360)/6));                              break;  // Hexagone
        case SHAPEFORM_OCTOGON          : return ComputePolygonR(width,height,CenterX,CenterY,8,-(double(360)/8));                              break;  // Octogone
        case SHAPEFORM_SIMPLEARROWUP    : return ComputeSimpleArrow(QRectF(left,top,width,height),0);                                           break;  // Simple arrow top
        case SHAPEFORM_SIMPLEARROWRIGHT : return ComputeSimpleArrow(QRectF(left,top,width,height),-90);                                         break;  // Simple arrow right
        case SHAPEFORM_SIMPLEARROWDOWN  : return ComputeSimpleArrow(QRectF(left,top,width,height),180);                                         break;  // Simple arrow down
        case SHAPEFORM_SIMPLEARROWLEFT  : return ComputeSimpleArrow(QRectF(left,top,width,height),90);                                          break;  // Simple arrow left
        case SHAPEFORM_DOUBLEARROWVERT  : return ComputeDoubleArrow(QRectF(left,top,width,height),0);                                           break;  // Double vert arrows
        case SHAPEFORM_DOUBLEARROWHORIZ : return ComputeDoubleArrow(QRectF(left,top,width,height),90);                                          break;  // Double horiz arrows
        case SHAPEFORM_DOUBLEARROWDIAG1 : return ComputeDoubleArrow(QRectF(left,top,width,height),-45);                                         break;  // Double diag arrows 1
        case SHAPEFORM_DOUBLEARROWDIAG2 : return ComputeDoubleArrow(QRectF(left,top,width,height),45);                                          break;  // Double diag arrows 2
        case SHAPEFORM_STRIANGLEUP      : return ComputeSingleTriangle(QRectF(left,top,width,height),0);                                        break;  // Single triangle top
        case SHAPEFORM_STRIANGLERIGHT   : return ComputeSingleTriangle(QRectF(left,top,width,height),-90);                                      break;  // Single triangle right
        case SHAPEFORM_STRIANGLEDOWN    : return ComputeSingleTriangle(QRectF(left,top,width,height),180);                                      break;  // Single triangle down
        case SHAPEFORM_STRIANGLELEFT    : return ComputeSingleTriangle(QRectF(left,top,width,height),90);                                       break;  // Single triangle left
        case SHAPEFORM_DTRIANGLEUP      : return ComputeDoubleTriangle(QRectF(left,top,width,height),0);                                        break;  // Double chevron top
        case SHAPEFORM_DTRIANGLERIGHT   : return ComputeDoubleTriangle(QRectF(left,top,width,height),-90);                                      break;  // Double chevron right
        case SHAPEFORM_DTRIANGLEDOWN    : return ComputeDoubleTriangle(QRectF(left,top,width,height),180);                                      break;  // Double chevron down
        case SHAPEFORM_DTRIANGLELEFT    : return ComputeDoubleTriangle(QRectF(left,top,width,height),90);                                       break;  // Double chevron left
        case SHAPEFORM_SCHEVRONUP       : return ComputeSingleChevron(QRectF(left,top,width,height),0,OPTION_UP|OPTION_DOWN);                   break;  // Single chevron up
        case SHAPEFORM_SCHEVRONRIGHT    : return ComputeSingleChevron(QRectF(left,top,width,height),-90,OPTION_LEFT|OPTION_RIGHT);              break;  // Single chevron right
        case SHAPEFORM_SCHEVRONDOWN     : return ComputeSingleChevron(QRectF(left,top,width,height),180,OPTION_UP|OPTION_DOWN);                 break;  // Single chevron down
        case SHAPEFORM_SCHEVRONLEFT     : return ComputeSingleChevron(QRectF(left,top,width,height),90,OPTION_LEFT|OPTION_RIGHT);               break;  // Single chevron left
        case SHAPEFORM_PUSHEDUP         : return ComputeSingleChevron(QRectF(left,top,width,height),0,OPTION_DOWN);                             break;  // Pushed up
        case SHAPEFORM_PUSHEDRIGHT      : return ComputeSingleChevron(QRectF(left,top,width,height),-90,OPTION_LEFT);                           break;  // Pushed right
        case SHAPEFORM_PUSHEDDOWN       : return ComputeSingleChevron(QRectF(left,top,width,height),180,OPTION_UP);                             break;  // Pushed down
        case SHAPEFORM_PUSHEDLEFT       : return ComputeSingleChevron(QRectF(left,top,width,height),90,OPTION_RIGHT);                           break;  // Pushed left
        case SHAPEFORM_SHARPUP          : return ComputeSingleChevron(QRectF(left,top,width,height),0,OPTION_UP);                               break;  // Sharp up
        case SHAPEFORM_SHARPRIGHT       : return ComputeSingleChevron(QRectF(left,top,width,height),-90,OPTION_RIGHT);                          break;  // Sharp right
        case SHAPEFORM_SHARPDOWN        : return ComputeSingleChevron(QRectF(left,top,width,height),180,OPTION_DOWN);                           break;  // Sharp down
        case SHAPEFORM_SHARPLEFT        : return ComputeSingleChevron(QRectF(left,top,width,height),90,OPTION_LEFT);                            break;  // Sharp left
        case SHAPEFORM_DCHEVRONUP       : return ComputeDoubleChevron(QRectF(left,top,width,height),0);                                         break;  // Double chevron up
        case SHAPEFORM_DCHEVRONRIGHT    : return ComputeDoubleChevron(QRectF(left,top,width,height),-90);                                       break;  // Double chevron right
        case SHAPEFORM_DCHEVRONDOWN     : return ComputeDoubleChevron(QRectF(left,top,width,height),180);                                       break;  // Double chevron down
        case SHAPEFORM_DCHEVRONLEFT     : return ComputeDoubleChevron(QRectF(left,top,width,height),90);                                        break;  // Double chevron left
        case SHAPEFORM_STAR4            : return ComputeStar(QRectF(left,top,width,height),4);                                                  break;  // Star 4
        case SHAPEFORM_STAR5            : return ComputeStar(QRectF(left,top,width,height),5);                                                  break;  // Star 5
        case SHAPEFORM_STAR6            : return ComputeStar(QRectF(left,top,width,height),6);                                                  break;  // Star 6
        case SHAPEFORM_STAR8            : return ComputeStar(QRectF(left,top,width,height),8);                                                  break;  // Star 8
        case SHAPEFORM_GEAR6            : return ComputeGear(QRectF(left,top,width,height),6);                                                  break;  // Gear 6
        case SHAPEFORM_GEAR8            : return ComputeGear(QRectF(left,top,width,height),8);                                                  break;  // Gear 8
        case SHAPEFORM_GEAR10           : return ComputeGear(QRectF(left,top,width,height),10);                                                 break;  // Gear 10
        case SHAPEFORM_GEAR12           : return ComputeGear(QRectF(left,top,width,height),12);                                                 break;  // Gear 12
        case SHAPEFORM_HEART            : return ComputeHeart(QRectF(left,top,width,height));                                                   break;  // Heart
        case SHAPEFORM_SPADE            : return ComputeSpade(QRectF(left,top,width,height));                                                   break;  // Spade
        case SHAPEFORM_CLUB             : return ComputeClub(QRectF(left,top,width,height));                                                    break;  // Club
        case SHAPEFORM_PUZZLEUL         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_RIGHT|OPTION_DOWN);                         break;  // Puzzle Up-Left
        case SHAPEFORM_PUZZLEUC         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_LEFT|OPTION_RIGHT|OPTION_DOWN);             break;  // Puzzle Up-Center
        case SHAPEFORM_PUZZLEUR         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_LEFT|OPTION_DOWN);                          break;  // Puzzle Up-Right
        case SHAPEFORM_PUZZLEML         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_UP|OPTION_RIGHT|OPTION_DOWN);               break;  // Puzzle Middle-Left
        case SHAPEFORM_PUZZLEMC         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_UP|OPTION_LEFT|OPTION_RIGHT|OPTION_DOWN);   break;  // Puzzle Middle-Center
        case SHAPEFORM_PUZZLEMR         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_UP|OPTION_LEFT|OPTION_DOWN);                break;  // Puzzle Middle-Right
        case SHAPEFORM_PUZZLEDL         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_UP|OPTION_RIGHT);                           break;  // Puzzle Down-Left
        case SHAPEFORM_PUZZLEDC         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_UP|OPTION_LEFT|OPTION_RIGHT);               break;  // Puzzle Down-Center
        case SHAPEFORM_PUZZLEDR         : return ComputePuzzle(QRectF(left,top,width,height),OPTION_UP|OPTION_LEFT);                            break;  // Puzzle Down-Right
        default                         : return ComputePolygonRect(QRectF(left,top,width,height));                                             break;  // No shape or rectangle or unknown shape
    }
}

QRectF PolygonToRectF(QList<QPolygonF> Polygon) {
    qreal x1=0,x2=0,y1=0,y2=0;
    if ((Polygon.count()==0)||(Polygon.at(0).toList().count()==0)) return QRectF();
    for (int j=0;j<Polygon.count();j++) {
        QList<QPointF> List=Polygon.at(j).toList();
        if (j==0) {
            x1=List[0].x();
            x2=List[0].x();
            y1=List[0].y();
            y2=List[0].y();
        }
        for (int i=0;i<List.count();i++) {
            if (List[i].x()<x1) x1=List[i].x();
            if (List[i].x()>x2)  x2=List[i].x();
            if (List[i].y()<y1)  y1=List[i].y();
            if (List[i].y()>y2)  y2=List[i].y();
        }
    }
    return QRectF(x1,y1,x2-x1,y2-y1);
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
    Deinterlace         =false;                         // Add a YADIF filter to deinterlace video (on/off)
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
    BlurSigma           =0;
    BlurRadius          =5;
    OnOffFilter         =0;
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
                    else RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,Deinterlace,SoundVolume,SoundOnly,false);
                if (!LN_Image) {
                    if (PreviewMode)    ImageObject->CachePreviewImage=new QImage(RenderImage->copy());
                        else            ImageObject->CacheRenderImage=new QImage(RenderImage->copy());
                }
            } else RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,Deinterlace,SoundVolume,SoundOnly,false);
        } else if (Image) RenderImage=Image->ImageAt(PreviewMode);

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
                double  TheBlurSigma    =BlurSigma;
                double  TheBlurRadius   =BlurRadius;

                int     TheOnOffFilter  =OnOffFilter;
                bool    ProgressifOnOffFilter=false;

                // Adjust values depending on PctDone and previous Filter (if exist)
                if (PreviousBrush) {
                    if (PreviousBrush->X!=TheXFactor)                   TheXFactor      =PreviousBrush->X+(TheXFactor-PreviousBrush->X)*PctDone;
                    if (PreviousBrush->Y!=TheYFactor)                   TheYFactor      =PreviousBrush->Y+(TheYFactor-PreviousBrush->Y)*PctDone;
                    if (PreviousBrush->ZoomFactor!=TheZoomFactor)       TheZoomFactor   =PreviousBrush->ZoomFactor+(TheZoomFactor-PreviousBrush->ZoomFactor)*PctDone;
                    if (PreviousBrush->ImageRotation!=TheRotateFactor)  TheRotateFactor =PreviousBrush->ImageRotation+(TheRotateFactor-PreviousBrush->ImageRotation)*PctDone;
                    if (PreviousBrush->Brightness!=TheBrightness)       TheBrightness   =PreviousBrush->Brightness+(TheBrightness-PreviousBrush->Brightness)*PctDone;
                    if (PreviousBrush->Contrast!=TheContrast)           TheContrast     =PreviousBrush->Contrast+(TheContrast-PreviousBrush->Contrast)*PctDone;
                    if (PreviousBrush->Gamma!=TheGamma)                 TheGamma        =PreviousBrush->Gamma+(TheGamma-PreviousBrush->Gamma)*PctDone;
                    if (PreviousBrush->Red!=TheRed)                     TheRed          =PreviousBrush->Red+(TheRed-PreviousBrush->Red)*PctDone;
                    if (PreviousBrush->Green!=TheGreen)                 TheGreen        =PreviousBrush->Green+(TheGreen-PreviousBrush->Green)*PctDone;
                    if (PreviousBrush->Blue!=TheBlue)                   TheBlue         =PreviousBrush->Blue+(TheBlue-PreviousBrush->Blue)*PctDone;
                    if (PreviousBrush->AspectRatio!=TheAspectRatio)     TheAspectRatio  =PreviousBrush->AspectRatio+(TheAspectRatio-PreviousBrush->AspectRatio)*PctDone;
                    if (PreviousBrush->BlurSigma!=TheBlurSigma)         TheBlurSigma    =PreviousBrush->BlurSigma+(TheBlurSigma-PreviousBrush->BlurSigma)*PctDone;
                    if (PreviousBrush->BlurRadius!=TheBlurRadius)       TheBlurRadius   =PreviousBrush->BlurRadius+(TheBlurRadius-PreviousBrush->BlurRadius)*PctDone;;
                    if (PreviousBrush->OnOffFilter!=TheOnOffFilter)     ProgressifOnOffFilter=true;
                }

                // Prepare values from sourceimage size
                double   RealImageW=double(RenderImage->width());                       if ((int(RealImageW) & 0x01)==1) RealImageW=RealImageW+1;    // Get real image widht
                double   RealImageH=double(RenderImage->height());                      if ((int(RealImageH) & 0x01)==1) RealImageH=RealImageH+1;    // Get real image height
                double   Hyp       =sqrt(RealImageW*RealImageW+RealImageH*RealImageH);  if ((int(Hyp)        & 0x01)==1) Hyp       =Hyp+1;           // Calc hypothenuse of the image to define full canvas
                double   HypPixel  =Hyp*TheZoomFactor;

                // Expand canvas
                QImage   NewRenderImage(Hyp,Hyp,QImage::Format_ARGB32_Premultiplied);
                QPainter Painter;
                Painter.begin(&NewRenderImage);
                Painter.setCompositionMode(QPainter::CompositionMode_Source);
                Painter.fillRect(QRect(0,0,Hyp,Hyp),Qt::transparent);
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
                    NewRenderImage=NewRenderImage.copy(ax/2,ay/2,NewRenderImage.width()-ax,NewRenderImage.height()-ay);
                }

                /*****************************************************************************
                  Le problème de ligne parasite vient du calcul suivant
                  pour le "résoudre", j'ai mis +2 et +2 mais ce n'est pas très mathématique !
                ******************************************************************************/

                // Get part we need and scaled it to destination size
                NewRenderImage=NewRenderImage.copy(Hyp*TheXFactor,Hyp*TheYFactor,HypPixel,HypPixel*TheAspectRatio)
                                    .scaled(Rect.width()+2,double(Rect.width())*TheAspectRatio+2,Qt::IgnoreAspectRatio,
                                    ApplicationConfig->Smoothing?Qt::SmoothTransformation:Qt::FastTransformation);

                // Apply correction filters to DestImage
                fmt_filters::image img(NewRenderImage.bits(),NewRenderImage.width(),NewRenderImage.height());
                if (TheBrightness!=0)                                           fmt_filters::brightness(img,TheBrightness);
                if (TheContrast!=0)                                             fmt_filters::contrast(img,TheContrast);
                if (TheGamma!=1)                                                fmt_filters::gamma(img,TheGamma);
                if ((TheRed!=0)||(TheGreen!=0)||(TheBlue!=0))                   fmt_filters::colorize(img,TheRed,TheGreen,TheBlue);
                if (TheBlurSigma<0)                                             fmt_filters::blur(img,TheBlurRadius,-TheBlurSigma);
                if (TheBlurSigma>0)                                             fmt_filters::sharpen(img,TheBlurRadius,TheBlurSigma);

                if ((OnOffFilter!=0)||((PreviousBrush!=NULL)&&(PreviousBrush->OnOffFilter!=0))) {
                    QImage PreviousImage=NewRenderImage.copy();
                    if (OnOffFilter!=0) {
                        // Apply previous filter to image
                        if ((OnOffFilter & FilterDespeckle)!=0)                 fmt_filters::despeckle(img);
                        if ((OnOffFilter & FilterEqualize)!=0)                  fmt_filters::equalize(img);
                        if ((OnOffFilter & FilterGray)!=0)                      fmt_filters::gray(img);
                        if ((OnOffFilter & FilterNegative)!=0)                  fmt_filters::negative(img);
                        if ((OnOffFilter & FilterEmboss)!=0)                    fmt_filters::emboss(img,10,1);
                    }
                    if (ProgressifOnOffFilter) {
                        // Apply previous filter to copied image
                        fmt_filters::image PreviousImg(PreviousImage.bits(),PreviousImage.width(),PreviousImage.height());
                        if ((PreviousBrush->OnOffFilter & FilterDespeckle)!=0)  fmt_filters::despeckle(PreviousImg);
                        if ((PreviousBrush->OnOffFilter & FilterEqualize)!=0)   fmt_filters::equalize(PreviousImg);
                        if ((PreviousBrush->OnOffFilter & FilterGray)!=0)       fmt_filters::gray(PreviousImg);
                        if ((PreviousBrush->OnOffFilter & FilterNegative)!=0)   fmt_filters::negative(PreviousImg);
                        if ((PreviousBrush->OnOffFilter & FilterEmboss)!=0)     fmt_filters::emboss(PreviousImg,10,1);

                        // Mix images
                        QPainter P;
                        P.begin(&PreviousImage);
                        P.setOpacity(PctDone);
                        P.drawImage(0,0,NewRenderImage);
                        P.setOpacity(1);
                        P.end();
                        if (!PreviousImage.isNull()) Ret=new QBrush(PreviousImage);
                    } else if (!NewRenderImage.isNull()) Ret=new QBrush(NewRenderImage);
                } else if (!NewRenderImage.isNull()) Ret=new QBrush(NewRenderImage);
            }
        }
        return Ret;
    } else {
        // Force loading of sound of video
        if (Video) {
            QImage *RenderImage=Video->ImageAt(PreviewMode,Position,StartPosToAdd,SoundTrackMontage,Deinterlace,SoundVolume,SoundOnly,false);
            if (RenderImage) delete RenderImage;
        }
        return new QBrush(Qt::NoBrush);
    }
}

//====================================================================================================================
// Note:This function is use only by cBrushDefinition !
void cBrushDefinition::ApplyFilter(QImage *Image) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::ApplyFilter");

    if (Image==NULL) return;
    fmt_filters::image img(Image->bits(),Image->width(),Image->height());
    if (Brightness!=0)                          fmt_filters::brightness(img,Brightness);
    if (Contrast!=0)                            fmt_filters::contrast(img,Contrast);
    if (Gamma!=1)                               fmt_filters::gamma(img,Gamma);
    if ((Red!=0)||(Green!=0)||(Blue!=0))        fmt_filters::colorize(img,Red,Green,Blue);
    if ((OnOffFilter & FilterDespeckle)!=0)     fmt_filters::despeckle(img);
    if ((OnOffFilter & FilterEqualize)!=0)      fmt_filters::equalize(img);
    if ((OnOffFilter & FilterGray)!=0)          fmt_filters::gray(img);
    if ((OnOffFilter & FilterNegative)!=0)      fmt_filters::negative(img);
    if ((OnOffFilter & FilterEmboss)!=0)        fmt_filters::emboss(img,10,1);
    if (BlurSigma<0)                            fmt_filters::blur(img,BlurRadius,-BlurSigma);
    if (BlurSigma>0)                            fmt_filters::sharpen(img,BlurRadius,BlurSigma);
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
    Deinterlace         =BrushToCopy->Deinterlace;

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
    BlurSigma           =BrushToCopy->BlurSigma;
    BlurRadius          =BrushToCopy->BlurRadius;
    OnOffFilter         =BrushToCopy->OnOffFilter;
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
                } else {
                    Element.setAttribute("SoundVolume",QString("%1").arg(SoundVolume,0,'f'));               // Volume of soundtrack (for video only)
                    Element.setAttribute("Deinterlace",Deinterlace?"1":0);                                  // Add a YADIF filter to deinterlace video (on/off) (for video only)
                }
            } else if (Image!=NULL) {
                if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                // Global definition only !
                    Element.setAttribute("BrushFileName",BrushFileName);                                    // File name if image from disk
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
    CorrectElement.setAttribute("BlurSigma",       BlurSigma);
    CorrectElement.setAttribute("BlurRadius",      BlurRadius);
    CorrectElement.setAttribute("OnOffFilter",     OnOffFilter);
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
                    } else {
                        SoundVolume=Element.attribute("SoundVolume").toDouble();                                    // Volume of soundtrack (for video only)
                        Deinterlace=Element.attribute("Deinterlace")=="1";                                          // Add a YADIF filter to deinterlace video (on/off) (for video only)
                    }
                } else if (Image!=NULL) {
                    if (TypeComposition!=COMPOSITIONTYPE_SHOT) {                                                    // Global definition only !
                        // Old Image transformation (for compatibility with version prio to 1.5)
                        if ((Element.elementsByTagName("ImageTransformation").length()>0)&&(Element.elementsByTagName("ImageTransformation").item(0).isElement()==true)) {
                            QDomElement SubElement=Element.elementsByTagName("ImageTransformation").item(0).toElement();
                            if (SubElement.hasAttribute("BlurSigma"))      BlurSigma=  SubElement.attribute("BlurSigma").toDouble();
                            if (SubElement.hasAttribute("BlurRadius"))     BlurRadius= SubElement.attribute("BlurRadius").toDouble();
                            if (SubElement.hasAttribute("OnOffFilter"))    OnOffFilter=SubElement.attribute("OnOffFilter").toInt();
                        }
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
            if (CorrectElement.hasAttribute("BlurSigma"))      BlurSigma       =CorrectElement.attribute("BlurSigma").toDouble();
            if (CorrectElement.hasAttribute("BlurRadius"))     BlurRadius      =CorrectElement.attribute("BlurRadius").toDouble();
            if (CorrectElement.hasAttribute("OnOffFilter"))    OnOffFilter     =CorrectElement.attribute("OnOffFilter").toInt();

            // If old ImageGeometry value in project file then compute LockGeometry
            if (CorrectElement.hasAttribute("ImageGeometry"))           LockGeometry=(CorrectElement.attribute("ImageGeometry").toInt()!=2);
                else if (CorrectElement.hasAttribute("LockGeometry"))   LockGeometry=(CorrectElement.attribute("LockGeometry").toInt()==1); // Else load saved value

        }

        return (BrushType==BRUSHTYPE_IMAGEDISK)?(Image!=NULL)||(Video!=NULL):true;
    }
    return false;
}

//====================================================================================================================

int cBrushDefinition::GetImageType() {
    int ImageType=IMAGETYPE_UNKNOWN;
    if (Image!=NULL) {
        if ((Image->ImageWidth>1080)&&(Image->ImageHeight>1080)) ImageType=IMAGETYPE_PHOTOLANDSCAPE; else ImageType=IMAGETYPE_CLIPARTLANDSCAPE;
        if ((qreal(Image->ImageWidth)/qreal(Image->ImageHeight))<1) ImageType++;
    } else if (Video!=NULL) {
        ImageType=IMAGETYPE_VIDEOLANDSCAPE;
        if ((qreal(Video->ImageWidth)/qreal(Video->ImageHeight))<1) ImageType++;
    }
    return ImageType;
}

//====================================================================================================================

void cBrushDefinition::ApplyMaskToImageToWorkspace(QImage *SrcImage,QRectF CurSelRect,int BackgroundForm,int AutoFramingStyle) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::ApplyMaskToImageToWorkspace");
    // Create shape mask
    int RowHeight=SrcImage->width();
    QImage Image(RowHeight,RowHeight,QImage::Format_ARGB32_Premultiplied);
    QPainter PainterImg;
    PainterImg.begin(&Image);
    PainterImg.setPen(Qt::NoPen);
    PainterImg.fillRect(QRect(0,0,RowHeight,RowHeight),QBrush(0x555555));
    PainterImg.setBrush(Qt::transparent);
    PainterImg.setCompositionMode(QPainter::CompositionMode_Source);
    QList<QPolygonF> List=ComputePolygon(BackgroundForm,CurSelRect.left(),CurSelRect.top(),CurSelRect.width(),CurSelRect.height(),CurSelRect.width()/2+CurSelRect.left(),CurSelRect.height()/2+CurSelRect.top());
    for (int i=0;i<List.count();i++) PainterImg.drawPolygon(List.at(i));
    PainterImg.setCompositionMode(QPainter::CompositionMode_SourceOver);
    PainterImg.end();

    // Apply mask to workspace image
    PainterImg.begin(SrcImage);
    PainterImg.setOpacity(0.75);
    PainterImg.drawImage(0,0,Image);
    PainterImg.setOpacity(1);
    PainterImg.end();

    // Add Icon (if wanted)
    if ((AutoFramingStyle>=0)&&(AutoFramingStyle<NBR_AUTOFRAMING)) {
        QImage IconGeoImage;
        switch (AUTOFRAMINGDEF[AutoFramingStyle].GeometryType) {
            case AUTOFRAMING_GEOMETRY_CUSTOM :  IconGeoImage=AutoFramingStyle==AUTOFRAMING_CUSTOMUNLOCK?QImage(AUTOFRAMING_ICON_GEOMETRY_UNLOCKED):
                                                                                                        QImage(AUTOFRAMING_ICON_GEOMETRY_LOCKED);       break;
            case AUTOFRAMING_GEOMETRY_PROJECT : IconGeoImage=QImage(AUTOFRAMING_ICON_GEOMETRY_PROJECT);                                                 break;
            case AUTOFRAMING_GEOMETRY_IMAGE :   IconGeoImage=QImage(AUTOFRAMING_ICON_GEOMETRY_IMAGE);                                                   break;
        }
        QPainter P;
        P.begin(SrcImage);
        P.drawImage(SrcImage->width()-IconGeoImage.width()-2,SrcImage->height()-IconGeoImage.height()-2,IconGeoImage);
        P.end();
    }
}

void cBrushDefinition::ApplyMaskToImageToWorkspace(QImage *SrcImage,int AutoFramingStyle,int BackgroundForm,int WantedSize,qreal maxw,qreal maxh,qreal minw,qreal minh,qreal X,qreal Y,qreal ZoomFactor,qreal AspectRatio,qreal ProjectGeometry) {
    QRectF CurSelRect;
    switch (AutoFramingStyle) {
        case AUTOFRAMING_CUSTOMUNLOCK   :   CurSelRect=QRectF(WantedSize*X,WantedSize*Y,WantedSize*ZoomFactor-1,WantedSize*ZoomFactor*AspectRatio-1);                   break;
        case AUTOFRAMING_CUSTOMLOCK     :   CurSelRect=QRectF(WantedSize*X,WantedSize*Y,WantedSize*ZoomFactor-1,WantedSize*ZoomFactor*AspectRatio-1);                   break;
        case AUTOFRAMING_CUSTOMIMGLOCK  :   CurSelRect=QRectF(WantedSize*X,WantedSize*Y,WantedSize*ZoomFactor-1,WantedSize*ZoomFactor*(maxh/maxw)-1);                   break;
        case AUTOFRAMING_CUSTOMPRJLOCK  :   CurSelRect=QRectF(WantedSize*X,WantedSize*Y,WantedSize*ZoomFactor-1,WantedSize*ZoomFactor*ProjectGeometry-1);               break;
        case AUTOFRAMING_FULLMAX        :   CurSelRect=QRectF((WantedSize-maxw)/2,(WantedSize-maxh)/2,maxw-1,maxh-1);                                                   break;
        case AUTOFRAMING_FULLMIN        :   CurSelRect=QRectF((WantedSize-minw)/2,(WantedSize-minh)/2,minw-1,minh-1);                                                   break;
        case AUTOFRAMING_HEIGHTLEFTMAX  :   CurSelRect=QRectF((WantedSize-maxw)/2,(WantedSize-maxh)/2,maxh/ProjectGeometry-1,maxh-1);                                   break;
        case AUTOFRAMING_HEIGHTLEFTMIN  :   CurSelRect=QRectF((WantedSize-minw)/2,(WantedSize-minh)/2,minh/ProjectGeometry-1,minh-1);                                   break;
        case AUTOFRAMING_HEIGHTMIDLEMAX :   CurSelRect=QRectF((WantedSize-(maxh/ProjectGeometry))/2,(WantedSize-maxh)/2,maxh/ProjectGeometry-1,maxh-1);                 break;
        case AUTOFRAMING_HEIGHTMIDLEMIN :   CurSelRect=QRectF((WantedSize-(minh/ProjectGeometry))/2,(WantedSize-minh)/2,minh/ProjectGeometry-1,minh-1);                 break;
        case AUTOFRAMING_HEIGHTRIGHTMAX :   CurSelRect=QRectF(WantedSize-(maxh/ProjectGeometry)-(WantedSize-maxw)/2,(WantedSize-maxh)/2,maxh/ProjectGeometry-1,maxh-1); break;
        case AUTOFRAMING_HEIGHTRIGHTMIN :   CurSelRect=QRectF(WantedSize-(minh/ProjectGeometry)-(WantedSize-minw)/2,(WantedSize-minh)/2,minh/ProjectGeometry-1,minh-1); break;
        case AUTOFRAMING_WIDTHTOPMAX    :   CurSelRect=QRectF((WantedSize-maxw)/2,(WantedSize-maxh)/2,maxw-1,maxw*ProjectGeometry-1);                                   break;
        case AUTOFRAMING_WIDTHTOPMIN    :   CurSelRect=QRectF((WantedSize-minw)/2,(WantedSize-minh)/2,minw-1,minw*ProjectGeometry-1);                                   break;
        case AUTOFRAMING_WIDTHMIDLEMAX  :   CurSelRect=QRectF((WantedSize-maxw)/2,(WantedSize-(maxw*ProjectGeometry))/2,maxw-1,maxw*ProjectGeometry-1);                 break;
        case AUTOFRAMING_WIDTHMIDLEMIN  :   CurSelRect=QRectF((WantedSize-minw)/2,(WantedSize-(minw*ProjectGeometry))/2,minw-1,minw*ProjectGeometry-1);                 break;
        case AUTOFRAMING_WIDTHBOTTOMMAX :   CurSelRect=QRectF((WantedSize-maxw)/2,WantedSize-(maxw*ProjectGeometry)-(WantedSize-maxh)/2,maxw-1,maxw*ProjectGeometry-1); break;
        case AUTOFRAMING_WIDTHBOTTOMMIN :   CurSelRect=QRectF((WantedSize-minw)/2,WantedSize-(minw*ProjectGeometry)-(WantedSize-minh)/2,minw-1,minw*ProjectGeometry-1); break;
        default : return;
    }
    ApplyMaskToImageToWorkspace(SrcImage,CurSelRect,BackgroundForm,AutoFramingStyle);
}

int cBrushDefinition::GetCurrentFramingStyle(qreal ProjectGeometry) {
    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return -1;

    int aX=X*dmax;
    int aY=Y*dmax;
    int aW=dmax*ZoomFactor;
    int aH=dmax*ZoomFactor*AspectRatio;

    if (!LockGeometry)                                                                                                                                                      return AUTOFRAMING_CUSTOMUNLOCK;
    if      ((aX==int((dmax-maxw)/2))                            &&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxw))                &&(aH==int(maxh)))                   return AUTOFRAMING_FULLMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int((dmax-minh)/2))                               &&(aW==int(minw))                &&(aH==int(minh)))                   return AUTOFRAMING_FULLMIN;
    else if ((aX==int((dmax-maxw)/2))                            &&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxh/ProjectGeometry))&&(aH==int(maxh)))                   return AUTOFRAMING_HEIGHTLEFTMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int((dmax-minh)/2))                               &&(aW==int(minh/ProjectGeometry))&&(aH==int(minh)))                   return AUTOFRAMING_HEIGHTLEFTMIN;
    else if ((aX==int((dmax-(maxh/ProjectGeometry))/2))          &&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxh/ProjectGeometry))&&(aH==int(maxh)))                   return AUTOFRAMING_HEIGHTMIDLEMAX;
    else if ((aX==int((dmax-(minh/ProjectGeometry))/2))          &&(aY==int((dmax-minh)/2))                               &&(aW==int(minh/ProjectGeometry))&&(aH==int(minh)))                   return AUTOFRAMING_HEIGHTMIDLEMIN;
    else if ((aX==int(dmax-(maxh/ProjectGeometry)-(dmax-maxw)/2))&&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxh/ProjectGeometry))&&(aH==int(maxh)))                   return AUTOFRAMING_HEIGHTRIGHTMAX;
    else if ((aX==int(dmax-(minh/ProjectGeometry)-(dmax-minw)/2))&&(aY==int((dmax-minh)/2))                               &&(aW==int(minh/ProjectGeometry))&&(aH==int(minh)))                   return AUTOFRAMING_HEIGHTRIGHTMIN;
    else if ((aX==int((dmax-maxw)/2))                            &&(aY==int((dmax-maxh)/2))                               &&(aW==int(maxw))                &&(aH==int(maxw*ProjectGeometry)))   return AUTOFRAMING_WIDTHTOPMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int((dmax-minh)/2))                               &&(aW==int(minw))                &&(aH==int(minw*ProjectGeometry)))   return AUTOFRAMING_WIDTHTOPMIN;
    else if ((aX==int((dmax-maxw)/2))                            &&(aY==int((dmax-(maxw*ProjectGeometry))/2))             &&(aW==int(maxw))                &&(aH==int(maxw*ProjectGeometry)))   return AUTOFRAMING_WIDTHMIDLEMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int((dmax-(minw*ProjectGeometry))/2))             &&(aW==int(minw))                &&(aH==int(minw*ProjectGeometry)))   return AUTOFRAMING_WIDTHMIDLEMIN;
    else if ((aX==int((dmax-maxw)/2))                            &&(aY==int(dmax-(maxw*ProjectGeometry)-(dmax-maxh)/2))   &&(aW==int(maxw))                &&(aH==int(maxw*ProjectGeometry)))   return AUTOFRAMING_WIDTHBOTTOMMAX;
    else if ((aX==int((dmax-minw)/2))                            &&(aY==int(dmax-(minw*ProjectGeometry)-(dmax-minh)/2))   &&(aW==int(minw))                &&(aH==int(minw*ProjectGeometry)))   return AUTOFRAMING_WIDTHBOTTOMMIN;
    else if (AspectRatio==(maxh/maxw))                                                                                                                                                          return AUTOFRAMING_CUSTOMIMGLOCK;
    else if (AspectRatio==ProjectGeometry)                                                                                                                                                      return AUTOFRAMING_CUSTOMPRJLOCK;
    else                                                                                                                                                                                        return AUTOFRAMING_CUSTOMLOCK;
}

//====================================================================================================================

QImage *cBrushDefinition::ImageToWorkspace(QImage *SrcImage,int WantedSize,qreal &maxw,qreal &maxh,qreal &minw,qreal &minh,qreal &x1,qreal &x2,qreal &x3,qreal &x4,qreal &y1,qreal &y2,qreal &y3,qreal &y4) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::ImageToWorkspace");
    QImage  *RetImage   =NULL;
    QImage  *SourceImage=NULL;
    qreal   Hyp         =sqrt(qreal(SrcImage->width())*qreal(SrcImage->width())+qreal(SrcImage->height())*qreal(SrcImage->height()));   // Calc hypothenuse of the image to define full canvas
    qreal   DstX,DstY,DstW,DstH;

    // calc rectangle before rotation
    qreal  rx=qreal(SrcImage->width())*(WantedSize/Hyp)/2;
    qreal  ry=qreal(SrcImage->height())*(WantedSize/Hyp)/2;

    //RotatePoint.X = ((Pt.X - Centre.X) * Cos(AngCrad) - (Pt.Y - Centre.Y) * Sin(AngCrad) + Centre.X)
    //RotatePoint.Y = ((Pt.X - Centre.X) * Sin(AngCrad) + (Pt.Y - Centre.Y) * Cos(AngCrad) + Centre.Y)

    qreal  xtab[4],ytab[4];
    xtab[0]=-rx*cos((ImageRotation)*PI/180)+ry*sin(ImageRotation*PI/180)+WantedSize/2;
    xtab[1]=+rx*cos((ImageRotation)*PI/180)+ry*sin(ImageRotation*PI/180)+WantedSize/2;
    xtab[2]=-rx*cos((ImageRotation)*PI/180)-ry*sin(ImageRotation*PI/180)+WantedSize/2;
    xtab[3]=+rx*cos((ImageRotation)*PI/180)-ry*sin(ImageRotation*PI/180)+WantedSize/2;
    ytab[0]=-rx*sin((ImageRotation)*PI/180)+ry*cos(ImageRotation*PI/180)+WantedSize/2;
    ytab[1]=+rx*sin((ImageRotation)*PI/180)+ry*cos(ImageRotation*PI/180)+WantedSize/2;
    ytab[2]=-rx*sin((ImageRotation)*PI/180)-ry*cos(ImageRotation*PI/180)+WantedSize/2;
    ytab[3]=+rx*sin((ImageRotation)*PI/180)-ry*cos(ImageRotation*PI/180)+WantedSize/2;

    // Sort xtab and ytab
    for (int i=0;i<4;i++) for (int j=0;j<3;j++) {
        if (xtab[j]>xtab[j+1]) {    qreal a=xtab[j+1];  xtab[j+1]=xtab[j];  xtab[j]=a;  }
        if (ytab[j]>ytab[j+1]) {    qreal a=ytab[j+1];  ytab[j+1]=ytab[j];  ytab[j]=a;  }
    }
    maxw=xtab[3]-xtab[0];   minw=xtab[2]-xtab[1];
    maxh=ytab[3]-ytab[0];   minh=ytab[2]-ytab[1];

    // Rotate image if needed and create a SourceImage
    if (ImageRotation!=0) {
        QTransform matrix;
        matrix.rotate(ImageRotation,Qt::ZAxis);
        SourceImage=new QImage(SrcImage->transformed(matrix,Qt::SmoothTransformation));

    // If no rotation then SourceImage=SrcImage
    } else SourceImage=SrcImage;

    // Calc coordinates of the part in the source image
    qreal  RealImageW=qreal(SourceImage->width());                  // Get real image widht
    qreal  RealImageH=qreal(SourceImage->height());                 // Get real image height

    DstX=((Hyp-RealImageW)/2)*(WantedSize/Hyp);
    DstY=((Hyp-RealImageH)/2)*(WantedSize/Hyp);
    DstW=RealImageW*(WantedSize/Hyp);
    DstH=RealImageH*(WantedSize/Hyp);

    QImage  ToUseImage=SourceImage->scaled(DstW,DstH);
    if (SourceImage!=SrcImage) delete SourceImage;

    if (ToUseImage.format()!=QImage::Format_ARGB32_Premultiplied) ToUseImage=ToUseImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    // On/Off filters and blur/sharpen
//REMOVE_FILTER    if (Image) Image->BrushFileTransform.ApplyFilter(&ToUseImage);
//REMOVE_FILTER        else if (Video) Video->BrushFileTransform.ApplyFilter(&ToUseImage);

    // Brightness, contrast, gamma and colors adjustments
    ApplyFilter(&ToUseImage);

    RetImage=new QImage(WantedSize,WantedSize,QImage::Format_ARGB32_Premultiplied);
    QPainter P;
    P.begin(RetImage);
    P.fillRect(QRectF(0,0,WantedSize,WantedSize),Transparent);
    P.drawImage(QRectF(DstX,DstY,DstW,DstH),ToUseImage,QRectF(0,0,DstW,DstH));
    P.end();

    x1=xtab[0]; y1=ytab[0];
    x2=xtab[1]; y2=ytab[1];
    x3=xtab[2]; y3=ytab[2];
    x4=xtab[3]; y4=ytab[3];

    return RetImage;
}

//====================================================================================================================

bool cBrushDefinition::CalcWorkspace(qreal &dmax,qreal &maxw,qreal &maxh,qreal &minw,qreal &minh,qreal &x1,qreal &x2,qreal &x3,qreal &x4,qreal &y1,qreal &y2,qreal &y3,qreal &y4) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::CalcWorkspace");

    int     ImgWidth =Image?Image->ImageWidth:Video?Video->ImageWidth:0;
    int     ImgHeight=Image?Image->ImageHeight:Video?Video->ImageHeight:0;

    if ((Video!=NULL)&&(ImgWidth==1920)&&(ImgHeight=1088)&&(Video->ApplicationConfig->Crop1088To1080)) ImgHeight=1080;

    if ((ImgWidth==0)||(ImgHeight==0)) return false;

    dmax=sqrt(qreal(ImgWidth)*qreal(ImgWidth)+qreal(ImgHeight)*qreal(ImgHeight));   // Calc hypothenuse of the image to define full canvas

    // calc rectangle before rotation
    qreal  rx=qreal(ImgWidth)/2;
    qreal  ry=qreal(ImgHeight)/2;

    qreal  xtab[4],ytab[4];
    xtab[0]=-rx*cos((ImageRotation)*PI/180)+ry*sin(ImageRotation*PI/180)+dmax/2;
    xtab[1]=+rx*cos((ImageRotation)*PI/180)+ry*sin(ImageRotation*PI/180)+dmax/2;
    xtab[2]=-rx*cos((ImageRotation)*PI/180)-ry*sin(ImageRotation*PI/180)+dmax/2;
    xtab[3]=+rx*cos((ImageRotation)*PI/180)-ry*sin(ImageRotation*PI/180)+dmax/2;
    ytab[0]=-rx*sin((ImageRotation)*PI/180)+ry*cos(ImageRotation*PI/180)+dmax/2;
    ytab[1]=+rx*sin((ImageRotation)*PI/180)+ry*cos(ImageRotation*PI/180)+dmax/2;
    ytab[2]=-rx*sin((ImageRotation)*PI/180)-ry*cos(ImageRotation*PI/180)+dmax/2;
    ytab[3]=+rx*sin((ImageRotation)*PI/180)-ry*cos(ImageRotation*PI/180)+dmax/2;

    // Sort xtab and ytab
    for (int i=0;i<4;i++) for (int j=0;j<3;j++) {
        if (xtab[j]>xtab[j+1]) {    qreal a=xtab[j+1];  xtab[j+1]=xtab[j];  xtab[j]=a;  }
        if (ytab[j]>ytab[j+1]) {    qreal a=ytab[j+1];  ytab[j+1]=ytab[j];  ytab[j]=a;  }
    }
    maxw=xtab[3]-xtab[0];   minw=xtab[2]-xtab[1];
    maxh=ytab[3]-ytab[0];   minh=ytab[2]-ytab[1];
    x1=xtab[0];             y1=ytab[0];
    x2=xtab[1];             y2=ytab[1];
    x3=xtab[2];             y3=ytab[2];
    x4=xtab[3];             y4=ytab[3];

    return true;
}

//====================================================================================================================

void cBrushDefinition::ApplyAutoFraming(int AutoAdjust,qreal ProjectGeometry) {
    switch (AutoAdjust) {
        case AUTOFRAMING_FULLMAX        :   s_AdjustWH();                           break;
        case AUTOFRAMING_FULLMIN        :   s_AdjustMinWH();                        break;
        case AUTOFRAMING_HEIGHTLEFTMAX  :   s_AdjustHLeft(ProjectGeometry);         break;
        case AUTOFRAMING_HEIGHTLEFTMIN  :   s_AdjustMinHLeft(ProjectGeometry);      break;
        case AUTOFRAMING_HEIGHTMIDLEMAX :   s_AdjustHMidle(ProjectGeometry);        break;
        case AUTOFRAMING_HEIGHTMIDLEMIN :   s_AdjustMinHMidle(ProjectGeometry);     break;
        case AUTOFRAMING_HEIGHTRIGHTMAX :   s_AdjustHRight(ProjectGeometry);        break;
        case AUTOFRAMING_HEIGHTRIGHTMIN :   s_AdjustMinHRight(ProjectGeometry);     break;
        case AUTOFRAMING_WIDTHTOPMAX    :   s_AdjustWTop(ProjectGeometry);          break;
        case AUTOFRAMING_WIDTHTOPMIN    :   s_AdjustMinWTop(ProjectGeometry);       break;
        case AUTOFRAMING_WIDTHMIDLEMAX  :   s_AdjustWMidle(ProjectGeometry);        break;
        case AUTOFRAMING_WIDTHMIDLEMIN  :   s_AdjustMinWMidle(ProjectGeometry);     break;
        case AUTOFRAMING_WIDTHBOTTOMMAX :   s_AdjustWBottom(ProjectGeometry);       break;
        case AUTOFRAMING_WIDTHBOTTOMMIN :   s_AdjustMinWBottom(ProjectGeometry);    break;
    }
}

void cBrushDefinition::s_AdjustWTop(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustWTop");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=maxw;
    //qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-maxh)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinWTop(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinWTop");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=minw;
    //qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-minh)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustWMidle(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustWMidle");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=maxw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinWMidle(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinWMidle");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=minw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustWBottom(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustWBottom");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=maxw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=(dmax-H-(dmax-maxh)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinWBottom(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinWBottom");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal W=minw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=(dmax-H-(dmax-minh)/2)/dmax;
    ZoomFactor=W/dmax;
}

//====================================================================================================================

void cBrushDefinition::s_AdjustHLeft(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustHLeft");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=maxh;
    qreal W=H/AspectRatio;
    X=((dmax-maxw)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinHLeft(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinHLeft");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=minh;
    qreal W=H/AspectRatio;
    X=((dmax-minw)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustHMidle(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustHMidle");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=maxh;
    qreal W=H/AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinHMidle(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinHMidle");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=minh;
    qreal W=H/AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustHRight(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustHRight");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=maxh;
    qreal W=H/AspectRatio;
    X=(dmax-W-(dmax-maxw)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinHRight(qreal ProjectGeometry) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinHRight");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =ProjectGeometry;
    qreal H=minh;
    qreal W=H/AspectRatio;
    X=(dmax-W-(dmax-minw)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

//====================================================================================================================

void cBrushDefinition::s_AdjustWH() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustWH");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =maxh/maxw;
    qreal W=maxw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}

void cBrushDefinition::s_AdjustMinWH() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cBrushDefinition::s_AdjustMinWH");

    qreal dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4;
    if (!CalcWorkspace(dmax,maxw,maxh,minw,minh,x1,x2,x3,x4,y1,y2,y3,y4)) return;

    LockGeometry=true;
    AspectRatio =minh/minw;
    qreal W=minw;
    qreal H=W*AspectRatio;
    X=((dmax-W)/2)/dmax;
    Y=((dmax-H)/2)/dmax;
    ZoomFactor=W/dmax;
}
