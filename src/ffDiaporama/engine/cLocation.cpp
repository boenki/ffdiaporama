/* ======================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2013 Dominique Levray <domledom@laposte.net>

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

#include "cLocation.h"

#define DEFAULT_LineWidth   1
#define DEFAULT_LineColor   "#000000"
#define DEFAULT_EndPoint    cLocation::SMALLPOINT
#define DEFAULT_Size        cBrushDefinition::sMarker::MEDIUM
#define DEFAULT_Position    cLocation::TOP
#define DEFAULT_Distance    cLocation::MARKERDISTNORMAL

cLocation::cLocation(cBaseApplicationConfig *ApplicationConfig):Icon(NULL,ApplicationConfig) {
    FavKey          =-1;
    ThumbnailResKey =-1;
    GPS_cx          =0;
    GPS_cy          =0;
    ZoomLevel       =13;
    LineWidth       =DEFAULT_LineWidth;
    LineColor       =DEFAULT_LineColor;
    EndPoint        =DEFAULT_EndPoint;
    Size            =DEFAULT_Size;
    Position        =DEFAULT_Position;
    Distance        =DEFAULT_Distance;
}

//========================================================================================================================

void cLocation::SaveToXML(QDomElement *ParentElement,QString,QString PathForRelativPath,bool ForceAbsolutPath,cReplaceObjectList *ReplaceList,QList<qlonglong> *ResKeyList) {
    ParentElement->setAttribute("Name",Name);
    ParentElement->setAttribute("Address",Address);
    ParentElement->setAttribute("FAddress",FriendlyAddress);
    ParentElement->setAttribute("GPS_cx",GPS_cx);
    ParentElement->setAttribute("GPS_cy",GPS_cy);
    ParentElement->setAttribute("ZoomLevel",ZoomLevel);
    ParentElement->setAttribute("MarkerW",MarkerSize.width());
    ParentElement->setAttribute("MarkerH",MarkerSize.height());
    if (LineWidth!=DEFAULT_LineWidth)   ParentElement->setAttribute("LineWidth",LineWidth);
    if (LineColor!=DEFAULT_LineColor)   ParentElement->setAttribute("LineColor",LineColor);
    if (Size!=DEFAULT_Size)             ParentElement->setAttribute("Size",int(Size));
    if (Position!=DEFAULT_Position)     ParentElement->setAttribute("Position",int(Position));
    if (Distance!=DEFAULT_Distance)     ParentElement->setAttribute("Distance",int(Distance));
    if (EndPoint!=DEFAULT_EndPoint)     ParentElement->setAttribute("EndPoint",int(EndPoint));
    Icon.SaveToXML(ParentElement,"Icon",PathForRelativPath,ForceAbsolutPath,ReplaceList,ResKeyList);
    if (ResKeyList) ResKeyList->append(ThumbnailResKey);
    ParentElement->setAttribute("ThumbResKey",ThumbnailResKey);
}

//========================================================================================================================

bool cLocation::LoadFromXML(QDomElement *ParentElement,QString,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag,QList<cSlideThumbsTable::TRResKeyItem> *ResKeyList,bool DuplicateRes) {
    bool IsOk;
    if (ParentElement->hasAttribute("Name"))        Name            =ParentElement->attribute("Name");
    if (ParentElement->hasAttribute("Address"))     Address         =ParentElement->attribute("Address");
    if (ParentElement->hasAttribute("FAddress"))    FriendlyAddress =ParentElement->attribute("FAddress");
    if (ParentElement->hasAttribute("GPS_cx"))      GPS_cx          =GetDoubleValue(*ParentElement,"GPS_cx");
    if (ParentElement->hasAttribute("GPS_cy"))      GPS_cy          =GetDoubleValue(*ParentElement,"GPS_cy");
    if (ParentElement->hasAttribute("ZoomLevel"))   ZoomLevel       =ParentElement->attribute("ZoomLevel").toInt();
    if (ParentElement->hasAttribute("MarkerW"))     MarkerSize.setWidth(ParentElement->attribute("MarkerW").toInt());
    if (ParentElement->hasAttribute("MarkerH"))     MarkerSize.setHeight(ParentElement->attribute("MarkerH").toInt());
    if (ParentElement->hasAttribute("LineWidth"))   LineWidth       =ParentElement->attribute("LineWidth").toInt();
    if (ParentElement->hasAttribute("LineColor"))   LineColor       =ParentElement->attribute("LineColor");
    if (ParentElement->hasAttribute("Size"))        Size            =(cBrushDefinition::sMarker::MARKERSIZE)ParentElement->attribute("Size").toInt();
    if (ParentElement->hasAttribute("Position"))    Position        =(MARKERPOSITION)ParentElement->attribute("Position").toInt();
    if (ParentElement->hasAttribute("Distance"))    Distance        =(MARKERDISTANCE)ParentElement->attribute("Distance").toInt();
    if (ParentElement->hasAttribute("EndPoint"))    EndPoint        =(ENDPOINT)ParentElement->attribute("EndPoint").toInt();
    if (ParentElement->hasAttribute("ThumbResKey")) ThumbnailResKey =ParentElement->attribute("ThumbResKey").toLongLong();
    IsOk=Icon.LoadFromXML(ParentElement,"Icon",PathForRelativPath,AliasList,ModifyFlag,ResKeyList,DuplicateRes);
    if ((IsOk)&&(ThumbnailResKey==-1)) {
        QImage ThumbImage=Icon.GetImageDiskBrush(QRect(0,0,64,64),true,0,NULL,1,NULL);
        Icon.ApplicationConfig->SlideThumbsTable->SetThumbs(&ThumbnailResKey,ThumbImage);
    } else {
        // Translate ThumbnailResKey is needed
        if (ResKeyList) {
            for (int ResNum=0;ResNum<ResKeyList->count();ResNum++)
                if (ThumbnailResKey==ResKeyList->at(ResNum).OrigKey)
                    ThumbnailResKey=ResKeyList->at(ResNum).NewKey;
        }
        // Duplicate ressource if needed
        if (DuplicateRes) {
            QImage ThumbImage;
            Icon.ApplicationConfig->SlideThumbsTable->GetThumbs(&ThumbnailResKey,&ThumbImage);
            ThumbnailResKey=-1;
            Icon.ApplicationConfig->SlideThumbsTable->SetThumbs(&ThumbnailResKey,ThumbImage);
        }
    }
    if (FavKey==-1) SearchInFavorite();
    if ((FriendlyAddress.isEmpty())&&(!Address.isEmpty())) FriendlyAddress=Address;
    return IsOk;
}

//========================================================================================================================

void cLocation::AddToFavorite() {
    QImage  Image=Icon.GetImageDiskBrush(QRect(0,0,64,64),true,0,NULL,1,NULL);
    QDomDocument domDocument;
    QDomElement  root=domDocument.createElement("Icon");
    domDocument.appendChild(root);
    Icon.SaveToXML(&root,"Icon","",true,NULL,NULL);
    FavKey=Icon.ApplicationConfig->LocationTable->AppendLocation(Name,Address,FriendlyAddress,GPS_cy,GPS_cx,ZoomLevel,domDocument.toString(),Image);
}

//========================================================================================================================

void cLocation::UpdateFavorite() {
    QImage  Image=Icon.GetImageDiskBrush(QRect(0,0,64,64),true,0,NULL,1,NULL);
    QDomDocument domDocument;
    QDomElement  root=domDocument.createElement("Icon");
    domDocument.appendChild(root);
    Icon.SaveToXML(&root,"Icon","",true,NULL,NULL);
    Icon.ApplicationConfig->LocationTable->UpdateLocation(FavKey,Name,Address,FriendlyAddress,GPS_cy,GPS_cx,ZoomLevel,domDocument.toString(),Image);
}

//========================================================================================================================

void cLocation::RemoveFavorite() {
    if (FavKey==-1) return;
    QSqlQuery Query(Icon.ApplicationConfig->Database->db);
    Query.prepare(QString("DELETE FROM Location WHERE Key=:Key"));
    Query.bindValue(":Key",FavKey,QSql::In);
    if (!Query.exec()) DisplayLastSQLError(&Query);
}

//=====================================================================================================

bool cLocation::LoadFromFavorite(qlonglong Key) {
    QSqlQuery Query(Icon.ApplicationConfig->Database->db);
    Query.prepare(QString("SELECT Name,Address,FAddress,Latitude,Longitude,Zoomlevel,Icon,Thumbnail FROM Location WHERE Key=:Key"));
    Query.bindValue(":Key",Key,QSql::In);
    if (!Query.exec()) {
        DisplayLastSQLError(&Query);
        return false;
    }
    while (Query.next()) {
        FavKey          =Key;
        Name            =Query.value(0).toString();
        Address         =Query.value(1).toString();
        FriendlyAddress =Query.value(2).toString();
        GPS_cy          =Query.value(3).toDouble();
        GPS_cx          =Query.value(4).toDouble();
        ZoomLevel       =Query.value(5).toInt();
        QString sIcon   =Query.value(6).toString();
        QByteArray Data =Query.value(7).toByteArray();
        QImage Thumb;
        if (!Data.isEmpty()) Thumb.loadFromData(Data);
        QDomDocument domDocument;
        QString      errorStr;
        int          errorLine,errorColumn;
        if (domDocument.setContent(sIcon,true,&errorStr,&errorLine,&errorColumn)) {
            if ((domDocument.elementsByTagName("Icon").length()>0)&&(domDocument.elementsByTagName("Icon").item(0).isElement()==true)) {
                QDomElement Element=domDocument.elementsByTagName("Icon").item(0).toElement();
                Icon.LoadFromXML(&Element,"Icon","",NULL,NULL,NULL,true);
                QImage ThumbImage;
                if (Thumb.isNull()) ThumbImage=Icon.GetImageDiskBrush(QRect(0,0,64,64),false,0,NULL,1,NULL);
                    else            ThumbImage=Thumb;
                Icon.ApplicationConfig->SlideThumbsTable->SetThumbs(&ThumbnailResKey,ThumbImage);
                return true;
            }
        }
    }
    return false;
}

//=====================================================================================================

bool cLocation::SearchInFavorite() {
    bool      IsOk=false;
    QSqlQuery Query(Icon.ApplicationConfig->Database->db);
    Query.prepare(QString("SELECT Key FROM Location WHERE Latitude=:Latitude AND Longitude=:Longitude"));
    Query.bindValue(":Latitude", GPS_cy,QSql::In);
    Query.bindValue(":Longitude",GPS_cx,QSql::In);
    if (!Query.exec()) {
        DisplayLastSQLError(&Query);
        return false;
    }
    while (Query.next()) {
        FavKey=Query.value(0).toLongLong();
        IsOk=true;
    }
    return IsOk;
}

//=====================================================================================================

QImage cLocation::GetThumb(int IconSize) {
    QImage Thumb;
    cLuLoImageCacheObject *ImageObject=Icon.ApplicationConfig->ImagesCache.FindObject(ThumbnailResKey,-1,QDateTime(),0,true,true);
    if (ImageObject) Thumb=ImageObject->ValidateCacheRenderImageNC(ThumbnailResKey);
    if (Thumb.isNull()) {
        Thumb=Icon.GetImageDiskBrush(QRect(0,0,64,64),false,0,NULL,1,NULL);
        Icon.ApplicationConfig->SlideThumbsTable->SetThumbs(&ThumbnailResKey,Thumb);
        if (ImageObject) ImageObject->CacheRenderImage=new QImage(Thumb.copy());
    }
    if ((!Thumb.isNull())&&(Thumb.width()!=IconSize)) Thumb=Thumb.scaledToWidth(IconSize);
    return Thumb;
}

//=====================================================================================================

void cLocation::ComputeMarkerSize(QSize MapImageSize) {
    QFont   FontNormal,FontBold;
    int     IconSize;

    // Setup FontFactor
    double FontSize=120;
    switch (Size) {
        case cBrushDefinition::sMarker::SMALL:  IconSize=16;    FontSize=60;    break;
        case cBrushDefinition::sMarker::MEDIUM: IconSize=32;    FontSize=120;   break;
        case cBrushDefinition::sMarker::LARGE:  IconSize=64;    FontSize=240;   break;
    }

    // Setup fonts
    FontBold  =QFont("Sans serif",9,QFont::Normal,QFont::StyleNormal);      // Font size is adjusted after
    FontNormal=QFont("Sans serif",9,QFont::Normal,QFont::StyleNormal);      // Font size is adjusted after
    FontBold.setBold(true);
    FontBold.setUnderline(false);
    FontNormal.setUnderline(false);
    #ifdef Q_OS_WIN
    FontBold.setPointSizeF(double(FontSize*1.1)/ScaleFontAdjust);                       // Scale font
    FontNormal.setPointSizeF(double(FontSize)/ScaleFontAdjust);                         // Scale font
    #else
    FontBold.setPointSizeF((double(FontSize*1.1)/ScaleFontAdjust)*ScreenFontAdjust);    // Scale font
    FontNormal.setPointSizeF((double(FontSize)/ScaleFontAdjust)*ScreenFontAdjust);      // Scale font
    #endif

    // Compute MarkerSize
    MarkerSize.setWidth(QFontMetrics(FontBold).width(Name)+8+IconSize);
    MarkerSize.setHeight(IconSize+6);

    int nW=QFontMetrics(FontNormal).width(Address)+8+IconSize;
    if (nW>MarkerSize.width())  MarkerSize.setWidth(nW);
    if (MarkerSize.width()>(MapImageSize.width()*0.6-8-IconSize))   MarkerSize.setWidth(MapImageSize.width()*0.6-8-IconSize);   // Not more than 60% of the map width
    if (MarkerSize.height()>(MapImageSize.height()*0.6-6-IconSize)) MarkerSize.setWidth(MapImageSize.width()*0.6-6-IconSize);   // Not more than 60% of the map width
}
