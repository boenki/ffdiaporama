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

// Specific inclusions
#include "_SoundDefinitions.h"


//TODO:revoir cMusicObject::LoadMedia pour les cas de piste son incompatible !


//*********************************************************************************************************************************************
// Base object for music definition
//*********************************************************************************************************************************************

cMusicObject::cMusicObject() {
    IsValide    =false;
    FilePath    ="";
    StartPos    =QTime(0,0,0,0);                // Start position
    EndPos      =QTime(0,0,0,0);                // End position
    Duration    =QTime(0,0,0,0);                // Duration
    FadeIn      =false;
    FadeOut     =false;
    Volume      =1.0;                           // Volume as % from 1% to 150%
    Music       =NULL;                          // Embeded Object (music is the same as video without video track !)
}

//====================================================================================================================

cMusicObject::~cMusicObject() {
    if (Music!=NULL) {
        delete Music;
        Music=NULL;
    }
}

//====================================================================================================================

void cMusicObject::SaveToXML(QDomElement &domDocument,QString ElementName,QString PathForRelativPath,bool ForceAbsolutPath) {
    QDomDocument    DomDocument;
    QDomElement     Element=DomDocument.createElement(ElementName);
    QString         FileName;

    if (PathForRelativPath!="") {
        if (ForceAbsolutPath) FileName=QDir(QFileInfo(PathForRelativPath).absolutePath()).absoluteFilePath(FilePath);
            else FileName=QDir(QFileInfo(PathForRelativPath).absolutePath()).relativeFilePath(FilePath);
    } else FileName=FilePath;

    Element.setAttribute("FilePath",FileName);
    Element.setAttribute("StartPos",StartPos.toString());
    Element.setAttribute("EndPos",  EndPos.toString());
    Element.setAttribute("FadeIn",  FadeIn?"1":"0");
    Element.setAttribute("FadeOut", FadeOut?"1":"0");
    Element.setAttribute("Volume",  QString("%1").arg(Volume,0,'f'));

    domDocument.appendChild(Element);
}

//====================================================================================================================

bool cMusicObject::LoadFromXML(QDomElement domDocument,QString ElementName,QString PathForRelativPath,QStringList *AliasList,bool *ModifyFlag,cBaseApplicationConfig *ApplicationConfig) {
    if ((domDocument.elementsByTagName(ElementName).length()>0)&&(domDocument.elementsByTagName(ElementName).item(0).isElement()==true)) {
        QDomElement Element=domDocument.elementsByTagName(ElementName).item(0).toElement();

        FilePath=Element.attribute("FilePath","");
        if (PathForRelativPath!="") FilePath=QDir::cleanPath(QDir(PathForRelativPath).absoluteFilePath(FilePath));
        if (LoadMedia(FilePath,AliasList,ModifyFlag,ApplicationConfig)) {
            StartPos=QTime().fromString(Element.attribute("StartPos"));
            EndPos  =QTime().fromString(Element.attribute("EndPos"));
            FadeIn  =Element.attribute("FadeIn")=="1";
            FadeOut =Element.attribute("FadeOut")=="1";
            Volume  =Element.attribute("Volume").toDouble();
            return true;
        } else return false;
    } else return false;
}

//====================================================================================================================

bool cMusicObject::LoadMedia(QString &filename,QStringList *AliasList,bool *ModifyFlag,cBaseApplicationConfig *ApplicationConfig) {
    // Clean all
    if (Music!=NULL) {
        delete Music;
        Music=NULL;
    }

    Music=new cVideoFile(cVideoFile::MUSICFILE,ApplicationConfig);
    IsValide=(Music->GetInformationFromFile(filename,AliasList,ModifyFlag))&&(Music->OpenCodecAndFile());
    /*if (IsValide) {
        // Check if file have at least one sound track compatible
        if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->sample_fmt!=AV_SAMPLE_FMT_S16)) {
            ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only audio track with signed 16 bits sample format","Error message");
            IsValide=false;
        }
        if ((CurrentBrush->Video->AudioStreamNumber!=-1)&&(CurrentBrush->Video->ffmpegAudioFile->streams[CurrentBrush->Video->AudioStreamNumber]->codec->channels>2)) {
            ErrorMessage=ErrorMessage+"\n"+QApplication::translate("MainWindow","This application support only mono or stereo audio track","Error message");
            IsValide=false;
        }
    }*/
    FilePath=QFileInfo(Music->FileName).absoluteFilePath();
    StartPos=QTime(0,0,0,0);                // Start position
    EndPos  =Music->Duration;
    Duration=Music->Duration;
    return IsValide;
}
