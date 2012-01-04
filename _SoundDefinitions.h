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

#ifndef SOUNDDEFINITIONS_H
#define SOUNDDEFINITIONS_H

// Basic inclusions (common to all files)
#include "SubProjects/VariousClass/_GlobalDefines.h"

// Include some additional standard class

// Basic inclusions (common to all files)
#include "SubProjects/VariousClass/cDeviceModelDef.h"
#include "SubProjects/VariousClass/cDeviceModelDef.h"
#include "SubProjects/VariousClass/cSoundBlockList.h"
#include "SubProjects/VariousClass/_SDL_Support.h"
#include "SubProjects/VariousClass/cBaseMediaFile.h"

//*********************************************************************************************************************************************
// Base object for music definition
//*********************************************************************************************************************************************

class cMusicObject {
public:
    bool                IsValide;
    QString             FilePath;
    QTime               StartPos;               // Start position
    QTime               EndPos;                 // End position
    QTime               Duration;               // Duration
    bool                FadeIn;
    bool                FadeOut;
    double              Volume;                 // Volume as % from 10% to 150%
    cVideoFile          *Music;                 // Embeded Object (music is the same as video without video track !)

    cMusicObject();
    ~cMusicObject();

    void        SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath);
    bool        LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag,cBaseApplicationConfig *ApplicationConfig);
    bool        LoadMedia(QString &filename,QStringList *AliasList,bool *ModifyFlag,cBaseApplicationConfig *ApplicationConfig);
};

#endif // SOUNDDEFINITIONS_H
