#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <Shellapi.h>
#include "blitzcpu.h"

//*****************************************************************************************************************

int main(int argc, char *argv[]) {
    WCHAR   wFile[512],wParam[512],wFolder[512];
    char    Param[2048],Folder[2048],ConfFile[2048];
	int		ForceSSE=0;

    printf("Starting ffDiaporamaStart\n\n");

    // Software to start
    swprintf(wFile,L"%S","ffDiaporama.exe");

    // compute current folder
    strcpy(Folder,argv[0]);
    if (strrchr(Folder,'\\')) *strrchr(Folder,'\\')=0;
    if (strlen(Folder)==0) {
        _getcwd(Folder,2048);
        if (strrchr(Folder,'\\')) *strrchr(Folder,'\\')=0;
    }
    printf("Current folder: %s\n\n",Folder);

	// try to read config file
	sprintf(ConfFile,"%s\\ffDSSE.ini",Folder);
    std::ifstream ConfigSSEFile;
	ConfigSSEFile.open(ConfFile);
	if (ConfigSSEFile.is_open()) {
        printf("Loading config file: %s\n\n",ConfFile);
		while (!ConfigSSEFile.eof()) {
			char ToRead[2048];
			ConfigSSEFile>>ToRead;
			if (strcmp(ToRead,"SSE=1")==0) 			ForceSSE=1;
				else if (strcmp(ToRead,"SSE=2")==0) ForceSSE=2;
				else if (strcmp(ToRead,"SSE=3")==0) ForceSSE=3;
				else if (strcmp(ToRead,"SSE=4")==0) ForceSSE=4;
		}
	}
	ConfigSSEFile.close();	

	// if no config file, try to detect processor extensions
	if (ForceSSE==0) {
		if (BlitzCPUInfo::haveExtension(BlitzCPUInfo::SSE2) || BlitzCPUInfo::haveExtension(BlitzCPUInfo::AMD3DNOW)) ForceSSE=2;
	}
	if (ForceSSE==0) ForceSSE=1;
	
    // add folders to system path
    char *CurPath=getenv("PATH");
    if (CurPath==NULL) {
        printf("Failed to get path environment variable !\n");
        exit(1);
    }
    char *Path=(char*)malloc((strlen(CurPath)+4096)*sizeof(char));
    if (Path==NULL) {
        printf("Failed to allocate memory!\n");
        exit(1);
    }
	switch (ForceSSE) {
        case 1 : sprintf(Path,"PATH=%s;%s\\32bitsse1",CurPath,Folder);						break;
        case 2 : sprintf(Path,"PATH=%s;%s\\32bit;%s\\32bit\\sse2",CurPath,Folder,Folder);	break;
        case 3 : sprintf(Path,"PATH=%s;%s\\32bit;%s\\32bit\\sse3",CurPath,Folder,Folder);	break;
        case 4 : sprintf(Path,"PATH=%s;%s\\32bit;%s\\32bit\\sse4",CurPath,Folder,Folder);	break;
	}
    _putenv(Path);
    printf( "New PATH variable: %s\n\n",Path);
    free(Path);

	// compute new folder depending on processor flags
	if (ForceSSE==1) {
        printf("\nStarting 32bit sse1 version:%s\\32bitsse1\\ffDiaporama.exe\n",Folder);
		swprintf(wFolder,L"%S\\32bitsse1",Folder);
	} else {
        printf("\nStarting 32bit sse%d version:%s\\32bit\\ffDiaporama.exe\n",ForceSSE,Folder);
		swprintf(wFolder,L"%S\\32bit",Folder);
	}

    // Get parameters
    Param[0]=0;
    for (int i=1;i<argc;i++) {
        strcat(Param,"\"");
        strcat(Param,argv[i]);
        strcat(Param,"\" ");
    }
    swprintf(wParam,L"%S",Param);

    ShellExecute(NULL,NULL,wFile,wParam,wFolder,SW_SHOWNORMAL);
    return 0;
}
