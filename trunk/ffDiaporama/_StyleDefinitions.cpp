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

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#include <winbase.h>
#endif

#include "DlgManageStyle.h"

bool toAssending(const cStyleCollectionItem &Item1 ,const cStyleCollectionItem &Item2) {
    return Item1.StyleName<Item2.StyleName;
}

//====================================================================================================================
// Style collections definition
//====================================================================================================================

cStyleCollectionItem::cStyleCollectionItem(bool IsGlobalConf,int IndexKey,QString TheStyleName,QString TheStyleDef) {
    FromGlobalConf  =IsGlobalConf;                          // true if device model is defined in global config file
    FromUserConf    =!IsGlobalConf;                         // true if device model is defined in user config file
    IsFind          =false;                                 // true if device model format is supported by installed version of ffmpeg
    StyleIndex      =IndexKey;                              // Style number index key
    StyleName       =TheStyleName;                          // Style name
    StyleDef        =TheStyleDef;                           // Style definition
}

//************************************************

cStyleCollectionItem::~cStyleCollectionItem() {

}

//************************************************

void cStyleCollectionItem::SaveToXML(QDomElement &domDocument,QString ElementName) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    Element.setAttribute("StyleIndex",      StyleIndex);
    Element.setAttribute("StyleName",       StyleName);
    Element.setAttribute("StyleDefinition", StyleDef);
    domDocument.appendChild(Element);
}

//************************************************

bool cStyleCollectionItem::LoadFromXML(QDomElement domDocument,QString ElementName,bool IsUserConfigFile) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();
        if (IsUserConfigFile) FromUserConf=true;
        StyleName=Element.attribute("StyleName");
        StyleDef =Element.attribute("StyleDefinition");

        if (FromUserConf==false) {
            BckStyleName=StyleName;
            BckStyleDef =StyleDef;
        }
        return true;
    } else return false;
}

//====================================================================================================================

cStyleCollection::cStyleCollection() {
    GeometryFilter=false;
}

//************************************************

cStyleCollection::~cStyleCollection() {
}

//====================================================================================================================

void cStyleCollection::SetActiveFilter(int Geometry) {
    switch (Geometry) {
        case GEOMETRY_4_3:      ActiveFilter="4:3-";        break;
        case GEOMETRY_16_9:     ActiveFilter="16:9-";       break;
        case GEOMETRY_40_17:    ActiveFilter="2.35:1-";     break;
    }
    GeometryFilter=true;
}

//************************************************

void cStyleCollection::SaveToXML(QDomDocument &domDocument,QDomElement &root) {
    int         j=0;
    QDomElement Element=domDocument.createElement(CollectionName);

    for (int i=0;i<Collection.count();i++) if (Collection[i].FromUserConf) {
        Collection[i].SaveToXML(Element,QString(CollectionName+QString("Item_%1").arg(j)));
        j++;
    }
    if (j>0) root.appendChild(Element);
}

//************************************************

void cStyleCollection::LoadFromXML(QDomDocument &domDocument,QDomElement root,int TypeConfigFile) {
    if ((root.elementsByTagName(CollectionName).length()>0)&&(root.elementsByTagName(CollectionName).item(0).isElement()==true)) {
        QDomElement Element=root.elementsByTagName(CollectionName).item(0).toElement();
        int i=0;
        while ((Element.elementsByTagName(CollectionName+QString("Item_%1").arg(i)).length()>0)&&(root.elementsByTagName(CollectionName+QString("Item_%1").arg(i)).item(0).isElement()==true)) {
            if (TypeConfigFile==GLOBALCONFIGFILE) {
                // Reading from global config file : append device
                Collection.append(cStyleCollectionItem(TypeConfigFile==GLOBALCONFIGFILE,i,"",""));
                Collection[i].LoadFromXML(Element,QString(CollectionName+QString("Item_%1").arg(i)),false);
                if (Collection[i].StyleName=="Big black text with white outlines")                      Collection[i].StyleName=QApplication::translate("DlgManageStyle","Big black text with white outlines");
                else if (Collection[i].StyleName=="Big light yellow text with dark brown shadow")       Collection[i].StyleName=QApplication::translate("DlgManageStyle","Big light yellow text with dark brown shadow");
                else if (Collection[i].StyleName=="Medium black text with white outlines")              Collection[i].StyleName=QApplication::translate("DlgManageStyle","Medium black text with white outlines");
                else if (Collection[i].StyleName=="Medium light yellow text with dark brown shadow")    Collection[i].StyleName=QApplication::translate("DlgManageStyle","Medium light yellow text with dark brown shadow");
                else if (Collection[i].StyleName=="Small white text with black outlines")               Collection[i].StyleName=QApplication::translate("DlgManageStyle","Small white text with black outlines");
                else if (Collection[i].StyleName=="Centered Blue Gradient")                             Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Blue Gradient");
                else if (Collection[i].StyleName=="Centered Brown Gradient")                            Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Brown Gradient");
                else if (Collection[i].StyleName=="Centered Dark-Gray Gradient")                        Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Dark-Gray Gradient");
                else if (Collection[i].StyleName=="Centered Green Gradient")                            Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Green Gradient");
                else if (Collection[i].StyleName=="Centered Light-Gray Gradient")                       Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Light-Gray Gradient");
                else if (Collection[i].StyleName=="Centered Red Gradient")                              Collection[i].StyleName=QApplication::translate("DlgManageStyle","Centered Red Gradient");
                else if (Collection[i].StyleName=="Transparent block (no brush)")                       Collection[i].StyleName=QApplication::translate("DlgManageStyle","Transparent block (no brush)");
                else if (Collection[i].StyleName=="16:9-Centered 10x15 Landscape Image")                Collection[i].StyleName=QApplication::translate("DlgManageStyle","16:9-Centered 10x15 Landscape Image");
                else if (Collection[i].StyleName=="16:9-Centered 10x15 Portrait Image")                 Collection[i].StyleName=QApplication::translate("DlgManageStyle","16:9-Centered 10x15 Portrait Image");
                else if (Collection[i].StyleName=="16:9-Centered 4:3 Image")                            Collection[i].StyleName=QApplication::translate("DlgManageStyle","16:9-Centered 4:3 Image");
                else if (Collection[i].StyleName=="16:9-Centered Cinema Image")                         Collection[i].StyleName=QApplication::translate("DlgManageStyle","16:9-Centered Cinema Image");
                else if (Collection[i].StyleName=="16:9-Full Screen block")                             Collection[i].StyleName=QApplication::translate("DlgManageStyle","16:9-Full Screen block");
                else if (Collection[i].StyleName=="2.35:1-Centered 10x15 Landscape Image")              Collection[i].StyleName=QApplication::translate("DlgManageStyle","2.35:1-Centered 10x15 Landscape Image");
                else if (Collection[i].StyleName=="2.35:1-Centered 10x15 Portrait Image")               Collection[i].StyleName=QApplication::translate("DlgManageStyle","2.35:1-Centered 10x15 Portrait Image");
                else if (Collection[i].StyleName=="2.35:1-Centered 16:9 Image")                         Collection[i].StyleName=QApplication::translate("DlgManageStyle","2.35:1-Centered 16:9 Image");
                else if (Collection[i].StyleName=="2.35:1-Centered 4:3 Image")                          Collection[i].StyleName=QApplication::translate("DlgManageStyle","2.35:1-Centered 4:3 Image");
                else if (Collection[i].StyleName=="2.35:1-Full Screen block")                           Collection[i].StyleName=QApplication::translate("DlgManageStyle","2.35:1-Full Screen block");
                else if (Collection[i].StyleName=="4:3-Centered 10x15 Landscape Image")                 Collection[i].StyleName=QApplication::translate("DlgManageStyle","4:3-Centered 10x15 Landscape Image");
                else if (Collection[i].StyleName=="4:3-Centered 10x15 Portrait Image")                  Collection[i].StyleName=QApplication::translate("DlgManageStyle","4:3-Centered 10x15 Portrait Image");
                else if (Collection[i].StyleName=="4:3-Centered 16:9 Image")                            Collection[i].StyleName=QApplication::translate("DlgManageStyle","4:3-Centered 16:9 Image");
                else if (Collection[i].StyleName=="4:3-Centered Cinema Image")                          Collection[i].StyleName=QApplication::translate("DlgManageStyle","4:3-Centered Cinema Image");
                else if (Collection[i].StyleName=="4:3-Full Screen block")                              Collection[i].StyleName=QApplication::translate("DlgManageStyle","4:3-Full Screen block");
                else if (Collection[i].StyleName=="Rounded rectangle with small brown border")          Collection[i].StyleName=QApplication::translate("DlgManageStyle","Rounded rectangle with small brown border");
                else if (Collection[i].StyleName=="Rectangle with no effect")                           Collection[i].StyleName=QApplication::translate("DlgManageStyle","Rectangle with no effect");
                Collection[i].BckStyleName=Collection[i].StyleName;
            } else {
                // Reading from user config file : search if Style already exist, then load it else append a new one
                QString ElementName=QString(CollectionName+QString("Item_%1").arg(i));
                if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
                    QDomElement TheElement=domDocument.elementsByTagName(ElementName).item(0).toElement();
                    int IndexKey=TheElement.attribute("StyleIndex").toInt();
                    int j=0;
                    while ((j<Collection.count())&&(Collection[j].StyleIndex!=IndexKey)) j++;
                    if (j<Collection.count()) Collection[IndexKey].LoadFromXML(Element,QString(CollectionName+QString("Item_%1").arg(i)),true); else {
                        j=Collection.count();
                        Collection.append(cStyleCollectionItem(false,IndexKey,"",""));
                        Collection[j].LoadFromXML(Element,QString(CollectionName+QString("Item_%1").arg(i)),true);
                    }
                }
            }
            i++;
        }
    }
    qSort(Collection.begin(),Collection.end(),toAssending);
}

//************************************************

QString cStyleCollection::PopupCollectionMenu(QWidget *ParentWindow,QString ActualStyleDef) {
    QString Item="";
    QMenu   *ContextMenu =new QMenu(ParentWindow);
    QMenu   *UpdateMenu  =new QMenu(ParentWindow);
    QAction *ActionCreate=new QAction(QApplication::translate("DlgManageStyle","Create new style"),ParentWindow);
    QAction *ActionManage=new QAction(QApplication::translate("DlgManageStyle","Manage existing style"),ParentWindow);
    UpdateMenu->setTitle(QApplication::translate("DlgManageStyle","Update existing style"));

    for (int i=0;i<Collection.count();i++) if (!GeometryFilter || Collection[i].StyleName.startsWith(ActiveFilter)) {
        if (GeometryFilter) Item=Collection[i].StyleName.mid(ActiveFilter.length()); else Item=Collection[i].StyleName;
        QAction *NormalAction=new QAction(Collection[i].FromUserConf?QIcon(ICON_USERCONF):QIcon(ICON_GLOBALCONF),Item,ParentWindow);
        NormalAction->setIconVisibleInMenu(true);
        ContextMenu->addAction(NormalAction);
        QAction *UpdateAction=new QAction(Collection[i].FromUserConf?QIcon(ICON_USERCONF):QIcon(ICON_GLOBALCONF),Item,ParentWindow);
        UpdateAction->setIconVisibleInMenu(true);
        UpdateAction->setToolTip(QApplication::translate("DlgManageStyle","Update style"));
        UpdateMenu->addAction(UpdateAction);
    }

    ContextMenu->addSeparator();
    ContextMenu->addAction(ActionCreate);
    ContextMenu->addMenu(UpdateMenu);
    ContextMenu->addAction(ActionManage);
    QAction *Ret=ContextMenu->exec(QCursor::pos());

    if (Ret!=NULL) {
        Item="";
        if (Ret==ActionCreate)                                                              CreateNewStyle(ParentWindow,ActualStyleDef);
        else if (Ret==ActionManage)                                                         ManageExistingStyle(ParentWindow);
        else if (Ret->toolTip()==QApplication::translate("DlgManageStyle","Update style"))  UpdateExistingStyle((GeometryFilter?ActiveFilter:"")+Ret->text(),ActualStyleDef);
        else Item=(GeometryFilter?ActiveFilter:"")+Ret->text();
    }
    delete ContextMenu;
    delete UpdateMenu;
    delete ActionCreate;
    delete ActionManage;
    return Item;
}

//************************************************

void cStyleCollection::UpdateExistingStyle(QString StyleName,QString ActualStyleDef) {
    int i=0;
    while ((i<Collection.count())&&(Collection[i].StyleName!=StyleName)) i++;
    if ((i<Collection.count())&&(Collection[i].StyleName==StyleName)) {
        Collection[i].StyleDef=ActualStyleDef;
        Collection[i].FromUserConf=true;
    }
}

//************************************************

void cStyleCollection::CreateNewStyle(QWidget *ParentWindow,QString ActualStyleDef) {
    bool    Ok,Continue=true;
    QString Text="";

    while (Continue) {
        Continue=false;
        Text=QInputDialog::getText(ParentWindow,QApplication::translate("DlgManageStyle","Create new style"),QApplication::translate("DlgManageStyle","Style name:"),QLineEdit::Normal,Text,&Ok);
        if (Ok && !Text.isEmpty()) {
            int i=0;
            while ((i<Collection.count())&&(Collection[i].StyleName!=Text)) i++;
            if ((i<Collection.count())&&(Collection[i].StyleName==Text)) {
                if (QMessageBox::question(ParentWindow,QApplication::translate("DlgManageStyle","Create new style"),
                                          QApplication::translate("DlgManageStyle","A style with this name already exist.\nDo you want to overwrite-it ?"),
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes) Collection[i].StyleDef=ActualStyleDef;
                else Continue=true;
            } else {
                int StyleIndex=0;
                for (int i=0;i<Collection.count();i++) if (Collection[i].StyleIndex>StyleIndex) StyleIndex=Collection[i].StyleIndex;
                StyleIndex++;
                Collection.append(cStyleCollectionItem(false,StyleIndex,Text,ActualStyleDef));
            }
        }
    }
    qSort(Collection.begin(),Collection.end(),toAssending);
}

//************************************************

void cStyleCollection::ManageExistingStyle(QWidget *ParentWindow) {
    DlgManageStyle Dlg(this,ParentWindow);
    Dlg.exec();
}

//************************************************

void cStyleCollection::StringToStringList(QString Item,QStringList &List) {
    int i=0;
    while ((i<Collection.count())&&(Collection[i].StyleName!=Item)) i++;
    if (i<Collection.count()) {
        QString String=Collection[i].StyleDef;
        while (String.contains("###")) {
            List.append(String.left(String.indexOf("###")));
            String=String.mid(String.indexOf("###")+QString("###").length());
        }
        if (!String.isEmpty()) List.append(String);
    }
}

