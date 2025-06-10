; Inno Setup Script für Espanso Helper
; Erstellt von AI

[Setup]
AppName=Espanso Helper
AppVersion=1.0
DefaultDirName={pf}\EspansoHelper
DefaultGroupName=Espanso Helper
OutputDir=.
OutputBaseFilename=EspansoHelperSetup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "german"; MessagesFile: "compiler:Languages\German.isl"

[Files]
Source: "build\Release\espanso_helper.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\Release\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\Release\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\Release\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\Release\platforms\*.*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "71.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Espanso Helper"; Filename: "{app}\espanso_helper.exe"; IconFilename: "{app}\71.ico"
Name: "{commondesktop}\Espanso Helper"; Filename: "{app}\espanso_helper.exe"; IconFilename: "{app}\71.ico"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Desktop-Verknüpfung erstellen"; GroupDescription: "Zusätzliche Aufgaben:"

[Run]
Filename: "{app}\espanso_helper.exe"; Description: "Espanso Helper jetzt starten"; Flags: nowait postinstall skipifsilent 