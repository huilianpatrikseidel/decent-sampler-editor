[Setup]
; App Information
AppId={{4A1D9B8C-9045-42E1-A8B9-536A18C7E12D}
AppName=Decent Sampler Editor
AppVersion=1.0.0
AppPublisher=Your Company
AppPublisherURL=https://www.yoursite.com
AppSupportURL=https://www.yoursite.com
AppUpdatesURL=https://www.yoursite.com

; Output Settings
DefaultDirName={autopf}\DecentSamplerEditor
DefaultGroupName=Decent Sampler Editor
AllowNoIcons=yes
OutputDir=.\Installer
OutputBaseFilename=DecentSamplerEditor_Setup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern stellar excludelightcontrols

; Advanced Settings
DisableWelcomePage=no
LicenseFile=Installer\LICENSE.txt
WizardImageFile=Installer\WizardImage.bmp
WizardImageStretch=yes
WizardSmallImageFile=Installer\WizardSmallImage.bmp

; Icon settings
UninstallDisplayIcon={app}\DecentSamplerEditor.exe

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Base files (executable and DLLs at the root of Deploy)
Source: "Deploy\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Decent Sampler Editor"; Filename: "{app}\DecentSamplerEditor.exe"
Name: "{group}\{cm:UninstallProgram,Decent Sampler Editor}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Decent Sampler Editor"; Filename: "{app}\DecentSamplerEditor.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\DecentSamplerEditor.exe"; Description: "{cm:LaunchProgram,Decent Sampler Editor}"; Flags: nowait postinstall skipifsilent
