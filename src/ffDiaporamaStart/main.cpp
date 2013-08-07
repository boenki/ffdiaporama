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

#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <Shellapi.h>

//*****************************************************************************************************************

int main(int argc, char *argv[]) {
    WCHAR   wFile[512],wParam[512],wFolder[512];
    char    Param[2048],Folder[2048];

    // Software to start
    swprintf(wFile,512,L"%S","ffDiaporama.exe");

    // compute current folder
    strcpy_s(Folder,2048,argv[0]);
    if (strrchr(Folder,'\\')) *strrchr(Folder,'\\')=0;
    if (strlen(Folder)==0) {
        _getcwd(Folder,2048);
        if (strrchr(Folder,'\\')) *strrchr(Folder,'\\')=0;
    }
    // compute new folder depending on processor flags
    swprintf(wFolder,512,L"%S",Folder);

    // Get parameters
    Param[0]=0;
    for (int i=1;i<argc;i++) {
        strcat_s(Param,2048,"\"");
        strcat_s(Param,2048,argv[i]);
        strcat_s(Param,2048,"\" ");
    }
    swprintf(wParam,512,L"%S",Param);

    ShellExecute(NULL,NULL,wFile,wParam,wFolder,SW_SHOWNORMAL);
    return 0;
}
