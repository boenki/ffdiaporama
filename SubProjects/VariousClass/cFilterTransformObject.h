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

#ifndef CFILTERTRANSFORMOBJECT_H
#define CFILTERTRANSFORMOBJECT_H

// Basic inclusions (common to all files)
#include "_GlobalDefines.h"

// Include some additional standard class
#include <QImage>
#include <QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

// OnOffFilter mask definition
#define FilterEqualize                      0x01
#define FilterDespeckle                     0x02
#define FilterGray                          0x04

// Base object for filters image = transform filters
class   cFilterTransformObject {
public:
    double   BlurSigma;
    double   BlurRadius;
    int      OnOffFilter;                // On-Off filter = combination of Despeckle, Equalize, Gray and Negative;

    cFilterTransformObject();

    void        ApplyFilter(QImage *Image);
    void        SaveToXML(QDomElement &domDocument,QString ElementName);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName);
    QString     FilterToString();
};

#endif  // CFILTERTRANSFORMOBJECT_H
