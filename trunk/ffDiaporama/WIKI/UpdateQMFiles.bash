#!/bin/bash
# ======================================================================
#    This file is part of ffDiaporama
#    ffDiaporama is a tools to make diaporama as video
#    Copyright (C) 2011-2013 Dominique Levray <domledom@laposte.net>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#   ====================================================================== */

PREPLANGUAGE() {
	echo "Prepare for language: "$1
	./WikiMakeBin make=$1
	cd $1
    rm wiki_img
    ln -s ../wiki_img wiki_img
    rm img
    ln -s ../../src/ffDiaporama/img img
    cp ../wiki.css wiki.css
    qhelpgenerator -c "wiki_"$1".qhp" -o "wiki_"$1".qch"
	qcollectiongenerator "wiki_"$1".qhcp" -o "wiki_"$1".qhc"
    mv "wiki_"$1".qhc" ../../locale
    mv "wiki_"$1".qch" ../../locale
	cd ..
}

cd fake
/usr/lib/x86_64-linux-gnu/qt5/bin/lrelease fake.pro
cd ..
PREPLANGUAGE "en"
PREPLANGUAGE "fr"
PREPLANGUAGE "es"
PREPLANGUAGE "pt"
PREPLANGUAGE "ru"
PREPLANGUAGE "it"
PREPLANGUAGE "el"
PREPLANGUAGE "de"
cp ../locale/wiki_* ~/.ffDiaporama
