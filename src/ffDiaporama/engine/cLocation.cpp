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

cLocation::cLocation(cBaseApplicationConfig *ApplicationConfig):Icon(ApplicationConfig) {
    GPS_cx          =0;
    GPS_cy          =0;
    ZoomLevel       =13;
    FavKey          =-1;
    ThumbnailResKey =-1;
}

//========================================================================================================================

void cLocation::SaveToXML(QDomElement *ParentElement,QString,QString PathForRelativPath,bool ForceAbsolutPath,cReplaceObjectList *ReplaceList,QList<qlonglong> *ResKeyList) {
    ParentElement->setAttribute("Name",Name);
    ParentElement->setAttribute("Address",Address);
    ParentElement->setAttribute("GPS_cx",GPS_cx);
    ParentElement->setAttribute("GPS_cy",GPS_cy);
    ParentElement->setAttribute("ZoomLevel",ZoomLevel);
    Icon.SaveToXML(ParentElement,"Icon",PathForRelativPath,ForceAbsolutPath,ReplaceList,ResKeyList);
    if (ResKeyList) ResKeyList->append(ThumbnailResKey);
    ParentElement->setAttribute("ThumbResKey",ThumbnailResKey);
}

//========================================================================================================================

bool cLocation::LoadFromXML(QDomElement *ParentElement,QString,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag,QList<cSlideThumbsTable::TRResKeyItem> *ResKeyList,bool DuplicateRes) {
    bool IsOk;
    if (ParentElement->hasAttribute("Name"))        Name            =ParentElement->attribute("Name");
    if (ParentElement->hasAttribute("Address"))     Address         =ParentElement->attribute("Address");
    if (ParentElement->hasAttribute("GPS_cx"))      GPS_cx          =GetDoubleValue(*ParentElement,"GPS_cx");
    if (ParentElement->hasAttribute("GPS_cy"))      GPS_cy          =GetDoubleValue(*ParentElement,"GPS_cy");
    if (ParentElement->hasAttribute("ZoomLevel"))   ZoomLevel       =ParentElement->attribute("ZoomLevel").toInt();
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
    return IsOk;
}

//========================================================================================================================

void cLocation::AddToFavorite() {
    QImage  Image=Icon.GetImageDiskBrush(QRect(0,0,64,64),true,0,NULL,1,NULL);
    QDomDocument domDocument;
    QDomElement  root=domDocument.createElement("Icon");
    domDocument.appendChild(root);
    Icon.SaveToXML(&root,"Icon","",true,NULL,NULL);
    FavKey=Icon.ApplicationConfig->LocationTable->AppendLocation(Name,Address,GPS_cy,GPS_cx,ZoomLevel,domDocument.toString(),Image);
}

//========================================================================================================================

void cLocation::UpdateFavorite() {
    QImage  Image=Icon.GetImageDiskBrush(QRect(0,0,64,64),true,0,NULL,1,NULL);
    QDomDocument domDocument;
    QDomElement  root=domDocument.createElement("Icon");
    domDocument.appendChild(root);
    Icon.SaveToXML(&root,"Icon","",true,NULL,NULL);
    Icon.ApplicationConfig->LocationTable->UpdateLocation(FavKey,Name,Address,GPS_cy,GPS_cx,ZoomLevel,domDocument.toString(),Image);
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
    Query.prepare(QString("SELECT Name,Address,Latitude,Longitude,Zoomlevel,Icon FROM Location WHERE Key=:Key"));
    Query.bindValue(":Key",Key,QSql::In);
    if (!Query.exec()) {
        DisplayLastSQLError(&Query);
        return false;
    }
    while (Query.next()) {
        FavKey       =Key;
        Name         =Query.value(0).toString();
        Address      =Query.value(1).toString();
        GPS_cy       =Query.value(2).toDouble();
        GPS_cx       =Query.value(3).toDouble();
        ZoomLevel    =Query.value(4).toInt();
        QString sIcon=Query.value(5).toString();
        QDomDocument domDocument;
        QString      errorStr;
        int          errorLine,errorColumn;
        if (domDocument.setContent(sIcon,true,&errorStr,&errorLine,&errorColumn)) {
            if ((domDocument.elementsByTagName("Icon").length()>0)&&(domDocument.elementsByTagName("Icon").item(0).isElement()==true)) {
                QDomElement Element=domDocument.elementsByTagName("Icon").item(0).toElement();
                Icon.LoadFromXML(&Element,"Icon","",NULL,NULL,NULL,true);
                QImage ThumbImage=Icon.GetImageDiskBrush(QRect(0,0,64,64),false,0,NULL,1,NULL);
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

QImage cLocation::GetThumb() {
    QImage Thumb;
    if (!Icon.ApplicationConfig->SlideThumbsTable->GetThumbs(&ThumbnailResKey,&Thumb)) {
        Thumb=Icon.GetImageDiskBrush(QRect(0,0,64,64),false,0,NULL,1,NULL);
        Icon.ApplicationConfig->SlideThumbsTable->SetThumbs(&ThumbnailResKey,Thumb);
    }
    return Thumb;
}
