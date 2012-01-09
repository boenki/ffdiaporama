#include <windows.h>
#include <stdio.h>
#include <tchar.h>

int main(int argc, char *argv[]) {
    WCHAR wFile[512],wParam[512],wFolder[512];
    char  Param[256],Folder[256];

    strcpy(Folder,argv[0]);
    if (strrchr(Folder,'\\')) *strrchr(Folder,'\\')=0;

    Param[0]=0;
    for (int i=1;i<argc;i++) {
        strcat(Param,"\"");
        strcat(Param,argv[i]);
        strcat(Param,"\" ");
    }

    swprintf(wFile,L"%S","ffDiaporama.exe");
    swprintf(wParam,L"%S",Param);
    swprintf(wFolder,L"%S",Folder);
    ShellExecute(NULL,NULL,wFile,wParam,wFolder,SW_SHOWNORMAL);
    return 0;
}
