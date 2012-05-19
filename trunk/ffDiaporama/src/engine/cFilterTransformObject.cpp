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

// Include some common various class
#include "../fmt_filters/fmt_filters.h"
#include <QStringList>

#include "cFilterTransformObject.h"

//*********************************************************************************************************************************************
// Base object for filters transformation image
//*********************************************************************************************************************************************

cFilterTransformObject::cFilterTransformObject() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFilterTransformObject::cFilterTransformObject");

    BlurSigma               = 0;
    BlurRadius              = 5;
    OnOffFilter             = 0;
}

//====================================================================================================================

cFilterTransformObject::cFilterTransformObject(QString FilterString) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFilterTransformObject::cFilterTransformObject(QString FilterString)");

    QStringList Values=FilterString.split("##");
    if (Values.count()==3) {
        BlurSigma  =Values[0].toDouble()/100;
        BlurRadius =Values[1].toDouble()/100;
        OnOffFilter=Values[2].toInt();
    } else {
        ToLog(LOGMSG_CRITICAL,"Error in cFilterTransformObject::cFilterTransformObject(QString FilterString) : FilterString is incorrect !");
        BlurSigma  =0;
        BlurRadius =5;
        OnOffFilter=0;
    }
}

//====================================================================================================================

QString cFilterTransformObject::FilterToString() {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFilterTransformObject::FilterToString");

    if (!HaveFilter()) return ""; else return QString("%1##%2##%3").arg(int(BlurSigma*100)).arg(int(BlurRadius*100)).arg(OnOffFilter);
}

//====================================================================================================================

void cFilterTransformObject::ApplyFilter(QImage *Image) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFilterTransformObject::ApplyFilter");

    if ((Image==NULL)||((OnOffFilter==0)&&(BlurSigma==0))) return;
    if (Image->format()!=QImage::Format_ARGB32_Premultiplied) {
        ToLog(LOGMSG_INFORMATION,"Error in cFilterTransformObject::ApplyFilter, Image is not and ARGB32_Premultiplied");
        return;
    }
    fmt_filters::image img(Image->bits(),Image->width(),Image->height());
    if ((OnOffFilter & FilterDespeckle)==FilterDespeckle)   {
        ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Applying Despeckle filter"));
        fmt_filters::despeckle(img);
    }
    if ((OnOffFilter & FilterEqualize)==FilterEqualize) {
        ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Applying Equalize filter"));
        fmt_filters::equalize(img);
    }
    if ((OnOffFilter & FilterGray)==FilterGray) {
        ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Applying Gray filter"));
        fmt_filters::gray(img);
    }
    if (BlurSigma<0) {
        ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Applying blur filter"));
        fmt_filters::blur(img,BlurRadius,-BlurSigma);
    }
    if (BlurSigma>0) {
        ToLog(LOGMSG_INFORMATION,QApplication::translate("MainWindow","Applying sharpen filter"));
        fmt_filters::sharpen(img,BlurRadius,BlurSigma);
    }
}

//====================================================================================================================

bool cFilterTransformObject::HaveFilter() {
    return (OnOffFilter!=0)||(BlurSigma!=0);
}

//====================================================================================================================

void cFilterTransformObject::SaveToXML(QDomElement &domDocument,QString ElementName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFilterTransformObject::SaveToXML");

    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);

    // Attribut of the object
    Element.setAttribute("BlurSigma",    BlurSigma);
    Element.setAttribute("BlurRadius",   BlurRadius);
    Element.setAttribute("OnOffFilter",  OnOffFilter);

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cFilterTransformObject::LoadFromXML(QDomElement domDocument,QString ElementName) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:cFilterTransformObject::LoadFromXML");

    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        if (Element.hasAttribute("BlurSigma"))      BlurSigma=  Element.attribute("BlurSigma").toDouble();
        if (Element.hasAttribute("BlurRadius"))     BlurRadius= Element.attribute("BlurRadius").toDouble();
        if (Element.hasAttribute("OnOffFilter"))    OnOffFilter=Element.attribute("OnOffFilter").toInt();

        return true;
    }
    return false;
}
