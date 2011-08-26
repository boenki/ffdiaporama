; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "ffDiaporama"
#define MyAppVersion "1.0.beta.3"
#define MyAppPublisher "The ffDiaporama Team"
#define MyAppURL "http://ffdiaporama.tuxfamily.org"
#define MyAppExeName "ffDiaporama.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
LicenseFile=C:\ffDiaporama\licences.txt
OutputDir=Y:\ffDiaporama\SiteWEB-Paquets
OutputBaseFilename={#MyAppName}.{#MyAppVersion}_setup
AppID={{113BC587-C2A9-457F-9022-9DB31ABBDD90}
AppName={#MyAppName}
AppCopyright={#MyAppPublisher}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
Compression=lzma/Max
SolidCompression=true
ShowLanguageDialog=no
VersionInfoDescription={#MyAppName}
VersionInfoCopyright={#MyAppPublisher}
VersionInfoProductName={#MyAppName}
VersionInfoTextVersion={#MyAppName} {#MyAppVersion}
VersionInfoCompany={#MyAppPublisher}
ChangesAssociations=true
VersionInfoVersion=0.99.4.3
VersionInfoProductVersion=0.99.4.3

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: C:\ffDiaporama\ffDiaporama.exe; DestDir: {app}; Flags: ignoreversion; Languages: english french; 
Source: C:\ffDiaporama\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; 
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
; NOTE: Ensure that you always include and set the WorkingDir.
; Filename is the "Target" parameter in your properties
; WorkingDir is the "Start In" parameter in your properties
; This is where the current directory will be set in your program at start up. 
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}" 
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; WorkingDir: "{app}" 
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon; WorkingDir: "{app}" 

[Registry] 
; Associate a File Extension with ffDiaporama : Not workinbg : need a launcher
;Root: HKCR; Subkey: ".ffd"; ValueType: string; ValueName: ""; ValueData: "ffDiaporama"; Flags: uninsdeletekey
;Root: HKCR; Subkey: ".ffd"; ValueType: string; ValueName: "Content Type"; ValueData: "ffDiaporama"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "ffDiaporama"; ValueType: string; ValueName: ""; ValueData: "ffDiaporama"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "ffDiaporama\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\img\logo.ico"
;Root: HKCR; Subkey: "ffDiaporama\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\ffDiaporama.exe"" ""%1""" 

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent
 
