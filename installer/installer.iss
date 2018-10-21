[Setup]
AppName=Osu-Relife
AppVersion=0.4
DefaultDirName={pf}\Osu-Relife
DefaultGroupName=OsuRelife
UninstallDisplayIcon={app}\osu-relife.exe
Compression=lzma2
SolidCompression=yes
DisableFinishedPage=yes
OutputBaseFilename=osu-relife_setup
DisableProgramGroupPage=yes

[Files]
Source: "..\Release\osu-relife.exe"; DestDir: "{app}"
Source: "..\Release\pill.dll"; DestDir: "{app}"

[Run]
Filename: {app}\osu-relife.exe; Parameters: "/install"; Description: Install Osu-Relife; Flags: shellexec skipifsilent;

[UninstallRun]
Filename: "{app}\osu-relife.exe"; Parameters: "/uninstall"

[ICONS]
Name: "{userdesktop}\Osu-Relife Switch"; Filename: "{app}\osu-relife.exe"
