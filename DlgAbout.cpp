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

#include "DlgAbout.h"
#include "ui_DlgAbout.h"

DlgAbout::DlgAbout(QWidget *parent):QDialog(parent),ui(new Ui::DlgAbout) {
    ui->setupUi(this);
    ui->ApplicationNameLabel->setText("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"\
                                      "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head>"\
                                      "<body><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                                      "<span style=\" font-size:14pt; font-weight:600;\">"+
                                      QString(APPLICATION_NAME)+" "+QString(APPLICATION_VERSION)+
                                      "</span></p></body></html>");
    ui->SystemInfoED->setText(SystemProperties);
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->OKBT,SIGNAL(clicked()),this,SLOT(accept()));
}

DlgAbout::~DlgAbout() {
    delete ui;
}
