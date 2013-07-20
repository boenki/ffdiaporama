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
#include "_Variables.h"

QString UpInitials(QString Source) {
    for (int i=0;i<Source.length();i++)
        if ((i==0)||(Source.at(i-1)==' ')) Source[i]=Source.at(i).toUpper();
    return Source;
}

// ************************************************
// Text variables like <%Txxx%>
//  PTT     Project title
//  PLD     Project long date
//  PSD     Project short date
//  PAU     Project author
//  PAL     Project album
//  PCT     Project comment
//  FFD     ffDiaporama version
// ************************************************

QString ResolveTextVariable(cDiaporamaObject *Object,QString SourceText) {
    if (!Object) return SourceText;

    QString PartT1="";
    QString PartT2=SourceText;
    QString VarName="";
    int     Pos;
    while ((Pos=PartT2.indexOf("&lt;%"))>=0) {                  // &lt; is < in html format !
        PartT1=PartT1+PartT2.left(Pos);
        PartT2=PartT2.mid(Pos+QString("&lt;%").length());
        if ((Pos=PartT2.indexOf("%&gt;"))>=0) {                 // %&gt; is > in html format !
            VarName=PartT2.left(Pos).toUpper();
            PartT2=PartT2.mid(Pos+QString("%&gt;").length());

            // VarName transformation
            int     ChapterNumber=Object->ComputeChapterNumber();
            QString ChapterNum=QString("%1").arg(ChapterNumber);
            while (ChapterNum.length()<3) ChapterNum="0"+ChapterNum;
            ChapterNum="Chapter_"+ChapterNum+":";

            // Project properties
            if      (VarName=="PTT") VarName=Object->Parent->ProjectInfo->Title;
            else if (VarName=="PAU") VarName=Object->Parent->ProjectInfo->Author;
            else if (VarName=="PAL") VarName=Object->Parent->ProjectInfo->Album;
            else if (VarName=="PCT") VarName=Object->Parent->ProjectInfo->Comment;
            else if (VarName=="PDD") VarName=Object->Parent->ProjectInfo->GetInformationValue("Duration");
            else if (VarName=="PSC") VarName=QString("%1").arg(Object->Parent->List.count());

            // Project date values
            else if (VarName=="PLD") VarName=Object->Parent->ProjectInfo->LongDate;
            else if (VarName=="PSD") VarName=Object->Parent->ProjectInfo->EventDate.toString(Object->Parent->ApplicationConfig->ShortDateFormat);
            else if (VarName=="PYR") VarName=QString("%1").arg(Object->Parent->ProjectInfo->EventDate.year());
            else if (VarName=="PMD") VarName=QString("%1").arg(Object->Parent->ProjectInfo->EventDate.month());
            else if (VarName=="PDY") VarName=QString("%1").arg(Object->Parent->ProjectInfo->EventDate.day());
            else if (VarName=="PMM") VarName=UpInitials(Object->Parent->ProjectInfo->EventDate.longMonthName(Object->Parent->ProjectInfo->EventDate.month()));
            else if (VarName=="PDW") VarName=UpInitials(Object->Parent->ProjectInfo->EventDate.longDayName(Object->Parent->ProjectInfo->EventDate.dayOfWeek()));

            // Current slide
            else if (VarName=="CSN") VarName=Object->GetDisplayName();
            else if (VarName=="CSR") VarName=QString("%1").arg(Object->GetSlideNumber()+1);

            // Current chapter
            else if (VarName=="CCN") VarName=Object->Parent->ProjectInfo->GetInformationValue(ChapterNum+"title");
            else if (VarName=="CCR") VarName=QString("%1").arg(ChapterNumber+1);
            else if (VarName=="CCD") VarName=Object->Parent->ProjectInfo->GetInformationValue(ChapterNum+"Duration");
            else if (VarName=="CCT") VarName=QString("%1").arg(Object->Parent->ProjectInfo->NbrChapters);

            // Various values
            else if (VarName=="FFD") VarName=Object->Parent->ProjectInfo->Composer;

            // Today date values
            else if (VarName=="TLD") VarName=UpInitials(QDate::currentDate().toString(Qt::DefaultLocaleLongDate));
            else if (VarName=="TSD") VarName=QDate::currentDate().toString(Object->Parent->ApplicationConfig->ShortDateFormat);
            else if (VarName=="TYR") VarName=QString("%1").arg(QDate::currentDate().year());
            else if (VarName=="TMD") VarName=QString("%1").arg(QDate::currentDate().month());
            else if (VarName=="TDY") VarName=QString("%1").arg(QDate::currentDate().day());
            else if (VarName=="TMM") VarName=UpInitials(QDate::currentDate().longMonthName(QDate::currentDate().month()));
            else if (VarName=="TDW") VarName=UpInitials(QDate::currentDate().longDayName(QDate::currentDate().dayOfWeek()));

            PartT1=PartT1+VarName;
        }
    }
    PartT1=PartT1+PartT2;
    return PartT1;
}

bool IsTextHaveVariables(QString SourceText) {
    if (SourceText.isEmpty()) return false;

    int     VarNbr=0;
    QString PartT1="";
    QString PartT2=SourceText;
    QString VarName="";
    int     Pos;
    while ((Pos=PartT2.indexOf("&lt;%"))>=0) {                  // &lt; is < in html format !
        PartT1=PartT1+PartT2.left(Pos);
        PartT2=PartT2.mid(Pos+QString("&lt;%").length());
        if ((Pos=PartT2.indexOf("%&gt;"))>=0) {                 // %&gt; is > in html format !
            VarName=PartT2.left(Pos).toUpper();
            PartT2=PartT2.mid(Pos+QString("%&gt;").length());
            VarNbr++;
        }
    }
    return VarNbr!=0;
}

bool IsObjectHaveVariables(cDiaporamaObject *Object) {
    for (int i=0;i<Object->ObjectComposition.List.count();i++)
        if (IsTextHaveVariables(Object->ObjectComposition.List[i]->Text))
            return true;
    return false;
}

//************************************************

void AppendAction(QMenu *Menu,QWidget *ParentWindow,QString Title) {
    QAction *Action=new QAction(Title,ParentWindow);
    Action->setIconVisibleInMenu(true);
    Action->setFont(QFont("Sans Serif",9));
    Action->setToolTip(QApplication::translate("Variables","Select this variable"));
    Menu->addAction(Action);
}

QString PopupVariableMenu(QWidget *ParentWindow) {
    ToLog(LOGMSG_DEBUGTRACE,"IN:PopupVariableMenu");

    QMenu   *ContextMenu    =new QMenu(ParentWindow);   ContextMenu->setFont(QFont("Sans Serif",9));
    QMenu   *PropertiesMenu =new QMenu(ParentWindow);   PropertiesMenu->setFont(QFont("Sans Serif",9));     PropertiesMenu->setTitle(QApplication::translate("Variables","Project properties values"));
    QMenu   *ProjectDateMenu=new QMenu(ParentWindow);   ProjectDateMenu->setFont(QFont("Sans Serif",9));    ProjectDateMenu->setTitle(QApplication::translate("Variables","Project date values"));
    QMenu   *SlideMenu      =new QMenu(ParentWindow);   SlideMenu->setFont(QFont("Sans Serif",9));          SlideMenu->setTitle(QApplication::translate("Variables","Current slide values"));
    QMenu   *ChapterMenu    =new QMenu(ParentWindow);   ChapterMenu->setFont(QFont("Sans Serif",9));        ChapterMenu->setTitle(QApplication::translate("Variables","Chapters values"));
    QMenu   *ChapterDateMenu=new QMenu(ParentWindow);   ChapterDateMenu->setFont(QFont("Sans Serif",9));    ChapterDateMenu->setTitle(QApplication::translate("Variables","Current chapter date values"));
    QMenu   *VariousMenu    =new QMenu(ParentWindow);   VariousMenu->setFont(QFont("Sans Serif",9));        VariousMenu->setTitle(QApplication::translate("Variables","Various values"));
    QMenu   *TodayDateMenu  =new QMenu(ParentWindow);   TodayDateMenu->setFont(QFont("Sans Serif",9));      TodayDateMenu->setTitle(QApplication::translate("Variables","Today date values"));
    QString Var="";

    AppendAction(PropertiesMenu,ParentWindow,QApplication::translate("Variables","Project title")+"\t<%PTT%>");
    AppendAction(PropertiesMenu,ParentWindow,QApplication::translate("Variables","Project author")+"\t<%PAU%>");
    AppendAction(PropertiesMenu,ParentWindow,QApplication::translate("Variables","Project album")+"\t<%PAL%>");
    AppendAction(PropertiesMenu,ParentWindow,QApplication::translate("Variables","Project comment")+"\t<%PCT%>");
    AppendAction(PropertiesMenu,ParentWindow,QApplication::translate("Variables","Project duration")+"\t<%PDD%>");
    AppendAction(PropertiesMenu,ParentWindow,QApplication::translate("Variables","Project slide count")+"\t<%PSC%>");

    AppendAction(ProjectDateMenu,ParentWindow,QApplication::translate("Variables","Long date")+"\t<%PLD%>");
    AppendAction(ProjectDateMenu,ParentWindow,QApplication::translate("Variables","Short date")+"\t<%PSD%>");
    AppendAction(ProjectDateMenu,ParentWindow,QApplication::translate("Variables","Year")+"\t<%PYR%>");
    AppendAction(ProjectDateMenu,ParentWindow,QApplication::translate("Variables","Month")+"\t<%PMM%>");
    AppendAction(ProjectDateMenu,ParentWindow,QApplication::translate("Variables","Month as number")+"\t<%PMD%>");
    AppendAction(ProjectDateMenu,ParentWindow,QApplication::translate("Variables","Day")+"\t<%PDD%>");
    AppendAction(ProjectDateMenu,ParentWindow,QApplication::translate("Variables","Day of week")+"\t<%PDW%>");

    AppendAction(SlideMenu,ParentWindow,QApplication::translate("Variables","Current slide name")+"\t<%CSN%>");
    AppendAction(SlideMenu,ParentWindow,QApplication::translate("Variables","Current slide number")+"\t<%CSR%>");

    AppendAction(ChapterMenu,ParentWindow,QApplication::translate("Variables","Current chapter name")+"\t<%CCN%>");
    AppendAction(ChapterMenu,ParentWindow,QApplication::translate("Variables","Current chapter number")+"\t<%CCR%>");
    AppendAction(ChapterMenu,ParentWindow,QApplication::translate("Variables","Current chapter duration")+"\t<%CCD%>");
    AppendAction(ChapterMenu,ParentWindow,QApplication::translate("Variables","Chapter count")+"\t<%CCT%>");

    AppendAction(ChapterDateMenu,ParentWindow,QApplication::translate("Variables","Long date")+"\t<%CLD%>");
    AppendAction(ChapterDateMenu,ParentWindow,QApplication::translate("Variables","Short date")+"\t<%CSD%>");
    AppendAction(ChapterDateMenu,ParentWindow,QApplication::translate("Variables","Year")+"\t<%CYR%>");
    AppendAction(ChapterDateMenu,ParentWindow,QApplication::translate("Variables","Month")+"\t<%CMM%>");
    AppendAction(ChapterDateMenu,ParentWindow,QApplication::translate("Variables","Month as number")+"\t<%CMD%>");
    AppendAction(ChapterDateMenu,ParentWindow,QApplication::translate("Variables","Day")+"\t<%CDD%>");
    AppendAction(ChapterDateMenu,ParentWindow,QApplication::translate("Variables","Day of week")+"\t<%CDW%>");

    AppendAction(VariousMenu,ParentWindow,QApplication::translate("Variables","ffDiaporama version")+"\t<%FFD%>");

    AppendAction(TodayDateMenu,ParentWindow,QApplication::translate("Variables","Long date")+"\t<%TLD%>");
    AppendAction(TodayDateMenu,ParentWindow,QApplication::translate("Variables","Short date")+"\t<%TSD%>");
    AppendAction(TodayDateMenu,ParentWindow,QApplication::translate("Variables","Year")+"\t<%TYR%>");
    AppendAction(TodayDateMenu,ParentWindow,QApplication::translate("Variables","Month")+"\t<%TMM%>");
    AppendAction(TodayDateMenu,ParentWindow,QApplication::translate("Variables","Month as number")+"\t<%TMD%>");
    AppendAction(TodayDateMenu,ParentWindow,QApplication::translate("Variables","Day")+"\t<%TDD%>");
    AppendAction(TodayDateMenu,ParentWindow,QApplication::translate("Variables","Day of week")+"\t<%TDW%>");

    ContextMenu->addMenu(PropertiesMenu);
    PropertiesMenu->addMenu(ProjectDateMenu);
    ContextMenu->addMenu(SlideMenu);
    ContextMenu->addMenu(ChapterMenu);
    ChapterMenu->addMenu(ChapterDateMenu);
    ContextMenu->addMenu(VariousMenu);
    VariousMenu->addMenu(TodayDateMenu);

    QAction *Ret=ContextMenu->exec(QCursor::pos());
    if (Ret) {
        Var=Ret->text();
        Var=Var.mid(Var.indexOf("<%"));
    }
    delete ContextMenu;
    return Var;
}
