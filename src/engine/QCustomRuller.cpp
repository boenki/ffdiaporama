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

#include "QCustomRuller.h"

#include <QPainter>
#include <QFont>

//======================================
// Specific defines for this dialog box
//======================================
#define TAQUET_SIZE         9       // TAQUET_SIZE is QSlider::handle:horizontal (width+margin)/2
#define CompletTickH1       3       // Y start position of the complete tick line
#define CompletTickH2       15      // Y end position of the complete tick line
#define IntermediateTickH1  7       // Y start position of the intermediate tick line
#define IntermediateTickH2  15      // Y end position of the intermediate tick line
#define ThumbWidth          14      // Width of the thumb
#define ThumbHeight         20      // Height of the thumb
#define ThumbYPos           16      // Y midle position of the thumb

//====================================================================================================================

QCustomRuller::QCustomRuller(QWidget *parent):QSlider(parent) {
    StartPos=0;
    EndPos  =0;
    //setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    //setFixedHeight(32);
}

//====================================================================================================================

void QCustomRuller::ActiveSlider(int TotalDuration) {
    this->TotalDuration=TotalDuration;
}

//====================================================================================================================

void QCustomRuller::resizeEvent(QResizeEvent *) {
//    if (Slider!=NULL) Slider->setMinimumWidth(this->width());
}

//====================================================================================================================

void QCustomRuller::mousePressEvent(QMouseEvent *ev) {
    if (ev->button()==Qt::LeftButton) {
        emit PositionChangeByUser();
        int x1 =int(double(this->width()-TAQUET_SIZE*2)*(double(value())/double(maximum())))+TAQUET_SIZE;
        if ((ev->x()>=x1-ThumbWidth/2)&&(ev->x()<=x1+ThumbWidth/2)) QSlider::mousePressEvent(ev);   // It's on the thumb
        else if ((ev->x()>=TAQUET_SIZE-ThumbWidth/2)&&(ev->x()<=this->width()-TAQUET_SIZE+ThumbWidth/2))          // Set new value
            setValue(int(double((ev->x()-TAQUET_SIZE)/double(this->width()-TAQUET_SIZE*2))*double(maximum())));
    }
}

//====================================================================================================================

void QCustomRuller::paintEvent(QPaintEvent *) {
    QPainter    Painter(this);
    QFont       font= QApplication::font();
    Painter.setFont(font);
    #ifdef Q_OS_WIN
    font.setPointSizeF(double(90)/double(Painter.fontMetrics().boundingRect("0").height()));                    // Scale font
    #else
    font.setPointSizeF((double(110)/double(Painter.fontMetrics().boundingRect("0").height()))*ScreenFontAdjust);// Scale font
    #endif
    Painter.setFont(font);

    // Prepare QPainter and calc positions

    double      Width        = double(this->width());
    double      Height       = double(this->height());
    int         WidthTick    = Painter.fontMetrics().width("00:00");

    //********************************************************
    // Draw slider background
    //********************************************************

    Painter.fillRect(QRect(0,0,Width,Height),QColor(Qt::black));
    Painter.setPen(QColor(0x20,0x20,0x20));
    Painter.drawLine(0,0,Width,0);
    Painter.setPen(Qt::white);
    Painter.translate(0,(Height-32)/2);
    Height=32;

    if ((TotalDuration!=0)&&(StartPos<EndPos)) {
        // Draw zone
        long    PosStart=long((Width-TAQUET_SIZE*2)*(double(StartPos)/double(TotalDuration)));
        long    PosEnd  =long((Width-TAQUET_SIZE*2)*(double(EndPos)/double(TotalDuration)));
        Painter.fillRect(QRect(TAQUET_SIZE+PosStart,IntermediateTickH1,PosEnd-PosStart,IntermediateTickH2),QColor(Qt::blue));
        // Draw previous zone
        if (PreviousStartPos!=-1) {
            Painter.setOpacity(0.5);
            //long    PosStart=long((Width-TAQUET_SIZE*2)*(double(PreviousStartPos)/double(TotalDuration)));
            long    aPosEnd  =long((Width-TAQUET_SIZE*2)*(double(PrevisousEndPos)/double(TotalDuration)));
            Painter.fillRect(QRect(TAQUET_SIZE+PosStart,IntermediateTickH1,aPosEnd-PosStart,IntermediateTickH2),QColor(Qt::green));
            Painter.setOpacity(1);
        }
        // Draw next zone
        if (NextStartPos!=-1) {
            Painter.setOpacity(0.5);
            long    aPosStart=long((Width-TAQUET_SIZE*2)*(double(NextStartPos)/double(TotalDuration)));
            //long    PosEnd  =long((Width-TAQUET_SIZE*2)*(double(NextEndPos)/double(TotalDuration)));
            Painter.fillRect(QRect(TAQUET_SIZE+aPosStart,IntermediateTickH1,PosEnd-aPosStart,IntermediateTickH2),QColor(Qt::red));
            Painter.setOpacity(1);
        }
    }

    // Now : Draw line and texte of the ruller

    // Define an echelle
    double  Duration=TotalDuration/1000;           // Calc duration in sec (double)
    double  SizeTick;
    QString Ech;

    // Test for WidthTick pixels between 2 ticks
    if ((Duration<10)&&(Width/Duration)>WidthTick) {
        Ech     ="S" ;                               // Use sec as echelle
        SizeTick=(Width-TAQUET_SIZE*2)/Duration;
    } else if ((Duration<600)&&((Width-TAQUET_SIZE*2)/(Duration/10))>WidthTick) {
        Ech="10S";                                   // Use 10 sec as echelle
        SizeTick=(Width-TAQUET_SIZE*2)/(Duration/10);
    } else if ((Duration<6000)&&(((Width-TAQUET_SIZE*2)/(Duration/60))>WidthTick)) {
        Ech="M";                                     // Use minute as echelle
        SizeTick=(Width-TAQUET_SIZE*2)/(Duration/60);
    } else if ((Duration<360000)&&(((Width-TAQUET_SIZE*2)/(Duration/600))>WidthTick)) {
        Ech="10M";                                   // Use 10 minute as echelle
        SizeTick=(Width-TAQUET_SIZE*2)/(Duration/600);
    } else {
        Ech="H";                                     // Use hour as echelle
        SizeTick=(Width-TAQUET_SIZE*2)/(Duration/3600);
    }

    short int   TimeSec  = 0;
    short int   TimeMin  = 0;
    short int   TimeHour = 0;
    short int   Cur      = 0;
    QString     Text;

    while ((TAQUET_SIZE*2+Cur*SizeTick+(WidthTick/2))<Width) {
        Painter.drawLine(TAQUET_SIZE+Cur*SizeTick,CompletTickH1,TAQUET_SIZE+Cur*SizeTick,CompletTickH2);

        // Draw text
        if ((Ech=="H")||(Ech=="10M")) Text=QString("%1:%2").arg(TimeHour,2,10,QChar('0')).arg(TimeMin,2,10,QChar('0'));
            else Text=QString("%1:%2").arg(TimeMin,2,10,QChar('0')).arg(TimeSec,2,10,QChar('0'));

        if ((!((TimeSec==0)&&(TimeMin==0)&&(TimeHour==0)))&&((TAQUET_SIZE+Cur*SizeTick+(WidthTick/2))<Width))
            Painter.drawText(TAQUET_SIZE+Cur*SizeTick-(WidthTick/2),32-1,Text);

        // Draw intermediate tick and then increment time depending on echelle
        if (Ech=="S") {
            TimeSec+=1;
            Text="";
        } else if (Ech=="10S") {
            // Draw an intermediate tick
            Painter.drawLine(TAQUET_SIZE+(Cur+0.5)*SizeTick,IntermediateTickH1,TAQUET_SIZE+(Cur+0.5)*SizeTick,IntermediateTickH2);
            Text=QString("%1:%2").arg(TimeMin,2,10,QChar('0')).arg(TimeSec+5,2,10,QChar('0'));
            TimeSec+=10;
        } else if (Ech=="M") {
            // Draw an intermediate tick
            Painter.drawLine(TAQUET_SIZE+(Cur+0.5)*SizeTick,IntermediateTickH1,TAQUET_SIZE+(Cur+0.5)*SizeTick,IntermediateTickH2);
            Text=QString("%1:%2").arg(TimeMin,2,10,QChar('0')).arg(TimeSec+30,2,10,QChar('0'));
            TimeMin+=1;
        } else if (Ech=="10M") {
            // Draw an intermediate tick
            Painter.drawLine(TAQUET_SIZE+(Cur+0.5)*SizeTick,IntermediateTickH1,TAQUET_SIZE+(Cur+0.5)*SizeTick,IntermediateTickH2);
            Text=QString("%1:%2").arg(TimeHour,2,10,QChar('0')).arg(TimeMin+5,2,10,QChar('0'));
            TimeMin+=10;
        } else {
            // Draw an intermediate tick
            Painter.drawLine(TAQUET_SIZE+(Cur+0.5)*SizeTick,IntermediateTickH1,TAQUET_SIZE+(Cur+0.5)*SizeTick,IntermediateTickH2);
            Text=QString("%1:%2").arg(TimeHour,2,10,QChar('0')).arg(TimeMin+30,2,10,QChar('0'));
            TimeHour+=1;
        }
        // Draw text if there is place !
        if ((WidthTick<(SizeTick/3))&&(TAQUET_SIZE+(Cur+0.5)*SizeTick+(WidthTick/2)<Width)) Painter.drawText(TAQUET_SIZE+(Cur+0.5)*SizeTick-(WidthTick/2),32-1,Text);

        // Adjust time
        if (TimeSec>59) {
            TimeSec-=60;
            TimeMin+=1;
        }
        if (TimeMin>59) {
            TimeMin-=60;
            TimeHour+=1;
        }

        // Go to next tick
        Cur=Cur+1;
    }

    //********************************************************
    // Draw slider
    //********************************************************

    Painter.setPen(QColor(0x20,0x20,0x20));
    Painter.setBrush(QBrush(QColor(0xA0,0xA0,0xA0)));
    Painter.drawRect(TAQUET_SIZE-1,15,Width-TAQUET_SIZE*2+2,5);

    //********************************************************
    // Draw thumb
    //********************************************************
    int     x1 =int(double(Width-TAQUET_SIZE*2)*(double(value())/double(maximum())))+TAQUET_SIZE;
    QPointF Table[10];
    double  vcos,vsin,Angle;
    int     i;

    Angle=90;
    for (i=0;i<3;i++) {
        vcos=cos(Angle*3.14159265/180)*(ThumbWidth/2);
        vsin=sin(Angle*3.14159265/180)*(ThumbHeight/2);
        Table[i]=QPointF(x1+vcos,ThumbYPos-vsin);
        Angle=Angle+(double(360)/3);
        if (Angle>=360) Angle=-Angle+360;
    }
    Painter.setBrush(QBrush(QColor(0xCC,0xCC,0xCC)));
    Painter.drawPolygon(Table,3);
    Painter.setPen(QColor(0x70,0x70,0x70));
    Painter.drawLine(x1,7,x1,16);
}
