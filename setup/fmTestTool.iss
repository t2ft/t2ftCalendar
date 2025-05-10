; ------------------------------------------------------------------------------
; Installation of frimotionTool
; Copyright (C) 2022-2024 by Pressfinish GmbH, Germering, Germany
; ------------------------------------------------------------------------------
; 2021-08-25  TTHA  initial version
;                   use windeployqt.exe to collect all dependencies. 
;                   to make this work, these two custom build steps have to be
;                   added to the release build configuration in qtCreator:
;                1) Command:            cmd.exe
;                   Arguments:          /C "copy %{CurrentRun:Executable:NativeFilePath} %{sourceDir}\..\setup\deploy\%{CurrentRun:Executable:FileName}"
;                   Working Directory:  %{buildDir}
;                2) Command:            windeployqt.exe
;                   Arguments:          %{CurrentRun:Executable:FileName}
;                   Working Directory:  %{sourceDir}\..\setup\deploy
; ------------------------------------------------------------------------------

; define this to enable debug ouputs and skip signing (for speed up) 
;#define _DEBUG
;#define _DEBUG_64
; define this do disable file signing
#define _NOSIGN
; define this to disable Visual-C runtime
#define NO_RUNTIME

; definitions specific for the installation of this application
#define deployDir "deploy\"
#define myAppBaseName "fmTestTool"
#define myAppTitle "Frimotion Production Test Tool"
#define myAppExeName myAppBaseName + ".exe"
#define myAppVersion GetVersionNumbersString(AddBackslash(deployDir) + myAppExeName)
#define myWizardSmallImageFile  "res\Pressfinish_Logo.bmp"
#define myWizardImageFile       "res\" + myAppBaseName + "_base.png"
#define docDir "..\..\..\..\doc\Prüfanweisung"
#define manualFile "PA_FMC-V1_de-r03.pdf"

; define this to the number applications that have to be closed before uninstall
; or do not define anything if there are none
; #define CLOSE_APPS 1

; define this to add extra "readyMemo" output or custom wizard page evaluations
#define MY_READYMEMO

#define myTranslationsPath '{app}\lang\'

; common functions and settings for all installation of the suite
;----------------------------------------------------------------------
; name of the program group
#define mySuiteName "fmTestTool"
; Company name
#define mySuitePublisher "PressFinish Electronics GmbH"
; Company URL
#define mySuiteURL "http://www.pressfinish.de"
; default installation path
#define mySuitePath '{pf}\' + mySuitePublisher + '\' + mySuiteName
; name of the configuration group
#define myConfigBaseName myAppBaseName

; base registry key
#define RegBaseKey 'Software\' + mySuitePublisher
; registry key to setting for the central suite application
#define RegSuiteKey RegBaseKey + '\' + mySuiteName
; registry key to setting for the installed application
#define RegAppKey RegBaseKey + '\' + myAppBaseName

; registry key to installer persistance settings
#define InstallerSettings RegSuiteKey + '\installer'

; MinGW Libraries
#define minGW_Libs32 "C:\Qt\Tools\mingw810_32\lib"

; ------------------------------------------------------------------------------
; files that will be copied on customers PC and that need to be signed
#define fApp deployDir + myAppExeName

#define nFilesToSign 1

#dim filesToSign[nFilesToSign]
#define filesToSign[0] fApp

#ifndef _DEBUG
#ifndef _NOSIGN
#define i
#for { i = 0 ; i < nFilesToSign ; i++ } Exec("signFile.bat",  filesToSign[i], sourcepath)
#endif
#endif


[Setup]
#ifndef _DEBUG_64
#ifndef _DEBUG
#ifndef _NOSIGN
SignTool=standard
#endif
#endif
#endif
AppPublisher={#mySuitePublisher}
AppPublisherURL={#mySuiteURL}
AppSupportURL={#mySuiteURL}
AppUpdatesURL={#mySuiteURL}
DefaultGroupName={code:suiteGroupName}
AllowNoIcons=true
LicenseFile=LICENSE
OutputDir=.\
Compression=lzma/Max
SolidCompression=true
MinVersion=0,6
UsePreviousLanguage=no
UsePreviousTasks=yes
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppID={code:getMyAppID}
AppName={#myAppTitle}
; set Windows "product version" property
AppVersion={#myAppVersion}
; set Windows "file version" property
VersionInfoVersion=1.0.0
OutputBaseFilename={#myAppBaseName}-{#myAppVersion}
SetupIconFile={#myAppBaseName}_setup.ico
UninstallDisplayIcon={app}\{#myAppExeName}
RestartIfNeededByRun=no
;regarding documentation this should speed up startup for signed installs
; but there is a drawback: setup will be divided up into 2 files :-(
;DiskSpanning=true
#define tImageName myAppBaseName
;#define tAppVersion "{#myAppVersion}"

; Use ImageMagic to insert Version number and application name into basic WizardImageFile
#ifdef tCount
#define genWizardImageFileName "WizardImage_" + tCount + ".bmp"
#else
#define genWizardImageFileName "WizardImage.bmp"
#endif

#define cmdline myWizardImageFile + \
  ' -background White -family Eurostile -gravity south -fill hsl(0,0,82,255) -pointsize 18 ' + \
  '-draw "text 0,0 ''Version ' + myAppVersion + "'"" " + \
  '-draw "text 0,18 ''' + tImageName + "'"" -flatten +matte BMP3:" + genWizardImageFileName

#expr Exec("cmd.exe", '/C convert ' + cmdline, sourcepath, 1)

WizardImageFile={#genWizardImageFileName}
WizardSmallImageFile={#myWizardSmallImageFile}

#ifndef _WizardImageBackColor
WizardImageBackColor=clWhite
#else
WizardImageBackColor={#myWizardImageBackColor}
#endif

#ifndef _WizardIMageStretch
WizardImageStretch=false
#else
WizardImageStretch={#myWizardImageStretch}
#endif

; don't allow user to select a different directory, if some components of the
; suite are installed already
DefaultDirName="{code:suiteInstallPath}"
AlwaysShowDirOnReadyPage=yes

[Files]
; application components
Source: {#deployDir}\*.*; DestDir: "{app}"; Flags: ignoreversion createallsubdirs recursesubdirs
#ifndef NO_RUNTIME
Source: {#deployDir}\vcredist*.*; DestDir: "{tmp}"; Flags: overwritereadonly ignoreversion; Permissions: everyone-full
;#else
;Source: "{#minGW_Libs32}\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
;Source: "{#minGW_Libs32}\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
;Source: "{#minGW_Libs32}\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
#endif
Source: "qt.conf"; DestDir: "{app}"; Flags: sharedfile overwritereadonly restartreplace uninsnosharedfileprompt

; other files
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion sharedfile uninsnosharedfileprompt
Source: "..\changelog.txt"; DestDir: "{app}"; Flags: ignoreversion sharedfile uninsnosharedfileprompt
Source: {#docDir}\{#manualFile}; DestDir: "{app}"; Flags: ignoreversion sharedfile uninsnosharedfileprompt

[Dirs]

[Tasks]
Name: dt; Description: {cm:TSdt}; GroupDescription: {cm:TSicongroup}; Flags: unchecked

[Icons]
Name: "{group}\{#myAppBaseName}"; Filename: "{app}\{#myAppExeName}"; Parameters: "lang=en"; Comment: {cm:CMgui}; Check: not WizardNoIcons
Name: "{group}\{#manualFile}"; Filename: "{app}\{#manualFile}"; Parameters: "lang=en"; Comment: {cm:CMgui}; Check: not WizardNoIcons
Name: "{commondesktop}\{#myAppBaseName}"; Filename: "{app}\{#myAppExeName}"; Parameters: "lang={language}"; Comment: {cm:CMgui}; Tasks: dt

[Registry]
; Preset useful defaults for all users

; installer persistance settings
Root: HKLM; Subkey: {#InstallerSettings}; Valuetype: dword; Valuename: "DesktopIcon"; ValueData: 0; Tasks: not dt
Root: HKLM; Subkey: {#InstallerSettings}; Valuetype: dword; Valuename: "DesktopIcon"; ValueData: 1; Tasks: dt

[Run]
#ifndef NO_RUNTIME
; installation of VCruntime
Filename: {tmp}\vcredist_x86.exe; Parameters: "/q /l {tmp}\vcredist_x86_log.txt"; WorkingDir: {tmp}; StatusMsg: {cm:Rvc}; Flags: waituntilterminated; Check: not isVCRedistInstalled
#endif
; Start GUI
Filename: "{app}\{#myAppExeName}"; Parameters: "lang={language}"; StatusMsg: {cm:Rgui}; WorkingDir: "{app}\"; Flags: postinstall nowait runasoriginaluser;

[Code]

#define UNINSTKEY         'Software\Microsoft\Windows\CurrentVersion\Uninstall\'
#define UUID_VCREDIST_X86 '{F0C3E5D1-1ADE-321E-8167-68EF0DE699B5}'

const //------------------------------------------------------------------------
// central definition of application GUUIDs
// allows detection of other installations
  UUID_App  = '{f0ed8b0c-c42a-4d03-8926-ecdc292eccd4}';
  nUUID_App         = 1;

type  //------------------------------------------------------------------------
  TAppIDs = Array[0..nUUID_App-1] of String;

var //--------------------------------------------------------------------------
  AppIDs    : TAppIDs;
  cachedSuiteInstalled : Boolean;
  cachedRuntimeInstalled : Boolean;
  cachedVCRedistInstalled : Boolean;
  cachedRuntimeRequired : Boolean;
  uninstallProgressBarStep : Longint;

//------------------------------------------------------------------------------
function getMyAppID(param:string) : String; forward;

//------------------------------------------------------------------------------
// convert Boolean to String
function BoolToStr(x: Boolean) : String;
begin
  if x then
    Result := 'true'
  else
    Result := 'false';
end;

//------------------------------------------------------------------------------
{ return the UUID of an application }
function getAppID(const inx : String) : String;
begin
  Result := AppIDs[StrToInt(inx)];
end;

//------------------------------------------------------------------------------
// return the UUID of the uninstall entry
function getInnoUninstallUUID(const uuid:String) : String;
begin
  Result := uuid + '_is1';
end;

//------------------------------------------------------------------------------
// return the registry key to the uninstall information
function getUninstallKey(const uuid:String) : String;
begin
  Result := '{#UNINSTKEY}' + uuid;
end;

//------------------------------------------------------------------------------
// check if a certain component of the dreaMTouch Suite is installed
// the component is identified by its UUID
function isInstalled(const uuid : String) : Boolean;
begin
  Result := RegKeyExists( HKLM, getUninstallKey(uuid) );
end; 

//------------------------------------------------------------------------------
// check if at least one component of the dreaMTouch suite is installed 
function isSuiteInstalled(param:string) : Boolean;
begin
  Result := cachedSuiteInstalled;
#ifdef _DEBUG
  MsgBox('isSuiteInstalled() =' + BoolToStr(Result), mbInformation, MB_OK);
#endif
end;

function getSuiteInstalled() : Boolean;
var
  i : Integer;
  found : Boolean;
begin
  found := false;
  for i:=0 to nUUID_App-1 do
      found := found or isInstalled( getInnoUninstallUUID(AppIDs[i]) );
  Result := found;
end;

//------------------------------------------------------------------------------
// calculate the installation path of the dreaMTouch suite
// all components have to be installed into the same directory
function suiteInstallPath(param : String) : String;
var
  i : Integer;
  path : String;
  found : Boolean;
begin
  found := false;
  for i:=0 to nUUID_App-1 do
    found := found or RegQueryStringValue(HKLM, getUninstallKey(getInnoUninstallUUID(AppIDs[i])), 'InstallLocation', path);
  if found then begin
    Result := path;
  end else begin
    Result := ExpandConstant('{#mySuitePath}');
  end;
#ifdef _DEBUG
  MsgBox('suiteInstallPath() ="' + Result + '"', mbInformation, MB_OK);
#endif
end;

//------------------------------------------------------------------------------
// calculate the icon group of the dreaMTouch suite
// all components have to be installed into the same icon group
function suiteGroupName(param : String) : String;
var
  i : Integer;
  group : String;
  found : Boolean;
begin
  found := false;
  for i:=0 to nUUID_App-1 do
    found := found or RegQueryStringValue(HKLM, getUninstallKey(getInnoUninstallUUID(AppIDs[i])), 'Inno Setup: Icon Group', group);
  if found then begin
    Result := group;
  end else begin
    Result := ExpandConstant('{#mySuiteName}');
  end;
#ifdef _DEBUG
  MsgBox('suiteGroupName() ="' + Result + '"', mbInformation, MB_OK);
#endif
end;

//------------------------------------------------------------------------------
// check if the Qt and MSVC runtime libraries are installed
// this is the case, if at least one of the components is installed 
function isRuntimeInstalled(param : String) : Boolean;
begin
  Result := cachedRuntimeInstalled;
#ifdef _DEBUG
  MsgBox('isRuntimeInstalled() =' + BoolToStr(Result), mbInformation, MB_OK);
#endif
end;
function getRuntimeInstalled() : Boolean;
begin
  Result := cachedSuiteInstalled;
end;

//------------------------------------------------------------------------------
// check if there are other components installed which 
// still require the runtime libraries
function isRuntimeRequired() : Boolean;
begin
  Result := cachedRuntimeRequired;
#ifdef _DEBUG
  MsgBox('isRuntimeRequired =' + BoolToStr(Result), mbInformation, MB_OK);
#endif
end;
function getRuntimeRequired(myUUID : String) : Boolean;
var
  req: Boolean;
begin
  req := false;
  if UUID_App <> myUUID then begin
    req := req or isInstalled(getInnoUninstallUUID(UUID_App));
  end;
  Result := req;
end;

//------------------------------------------------------------------------------
// check if the Visual C redistributable package is installed
function isVCRedistInstalled() : Boolean;
begin
  Result := cachedVCRedistInstalled;
#ifdef _DEBUG
  MsgBox('isVCRedistInstalled() =' + BoolToStr(Result), mbInformation, MB_OK);
#endif
end;

function getVCRedistInstalled() : Boolean;
begin
  Result := isInstalled('{#UUID_VCREDIST_X86}');
end;

function isWebserverEnabled() : Boolean;
var
  x: Cardinal;
begin
  Result := false;
  if RegQueryDWordValue(HKEY_LOCAL_MACHINE, '{#RegSuiteKey}', 'WebServer', x) then begin
    if x<>0 then
      Result := true;
  end; 
end;

function previousSelection(key:string; def:boolean) : Boolean;
var
  x: Cardinal;
begin
  Result := def;
  if RegQueryDWordValue(HKEY_LOCAL_MACHINE, '{#InstallerSettings}', key, x) then begin
    if x<>0 then
      Result := true
    else
      Result := false;
  end; 
end;

function previousSelectionString(key:string; def:String) : String;
var
  x: String;
begin
  Result := def;
  x := def;
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, '{#InstallerSettings}', key, x) then begin
    Result := x;
  end; 
end;

//------------------------------------------------------------------------------
#define n_App 0

function initVars :string;
begin
  AppIDs[{#n_App}] := UUID_App;
  Result := getMyAppID('');
end;

procedure initCache(id:string);
begin
  cachedSuiteInstalled := getSuiteInstalled;
  cachedRuntimeInstalled := getRuntimeInstalled;
  cachedVCRedistInstalled := getVCRedistInstalled;
  cachedRuntimeRequired := getRuntimeRequired(id);
end;

//------------------------------------------------------------------------------
procedure setTaskChecked(s:string; checked:boolean);
var
  i : integer;
begin
    i := WizardForm.TasksList.Items.IndexOf(ExpandConstant(s));
    if (i<>-1) then
      WizardForm.TasksList.Checked[i] := checked;
end;

//------------------------------------------------------------------------------
// separate filename and parameters
function SplitParameters(const FileName: string; var Parameters: string): string;
var
   i : integer;
   InQuote : boolean;
begin
   Result := '';
   Parameters := '';
   if FileName = '' then exit;
   InQuote := false;
   i := 1;
   while i <= length(FileName) do
   begin
      if (FileName[i] = '"') then InQuote := not InQuote;
      if (FileName[i] = ' ') and (not Inquote) then break;
      i := i + 1;
   end;
   Result := RemoveQuotes(Copy(FileName, 1, i - 1));
   Parameters := Copy(FileName, i + 1, length(FileName));
end;
 
//------------------------------------------------------------------------------
// read HKLM root keys for 64 and 32 bit installation codes
function HKLMRegQueryStringValue(const SubKeyName, ValueName: String; var ResultStr: String) : Boolean;
begin
  if isWin64 then begin
    Result := RegQueryStringValue(HKLM64, SubKeyName, ValueName, ResultStr);
#ifdef _DEBUG_64
    MsgBox('HKLM RegQueryStringValue(HKLM64, ' + SubKeyName + ', ' + ValueName + ', ' + ResultStr + ') = ' + BoolToStr(Result), mbInformation, MB_OK);
#endif
  end;
  if not Result then begin
    Result := RegQueryStringValue(HKLM32, SubKeyName, ValueName, ResultStr);
#ifdef _DEBUG_64
    MsgBox('HKLM RegQueryStringValue(HKLM32, ' + SubKeyName + ', ' + ValueName + ', ' + ResultStr + ') = ' + BoolToStr(Result), mbInformation, MB_OK);
#endif
  end;
end;

//------------------------------------------------------------------------------
// inform user about another installed version of this application and uninstall
// if required by user;
// returns True if uninstall was successfull
function UninstallOther(uuid : string) : Boolean;
var
  ver, fname, params, otherAppName, msg : String;
  res : Integer;
  r : Boolean;
begin
  Result := false;
  if HKLMRegQueryStringValue('{#UNINSTKEY}' + uuid, 'DisplayName', otherAppName) then begin
    msg := FmtMessage( ExpandConstant('{cm:iAlreadyInstalled}'), [ otherAppName ] )
    if Pos('version', Lowercase(otherAppName)) = 0 then
    begin
      if HKLMRegQueryStringValue('{#UNINSTKEY}' + uuid, 'DisplayVersion', ver) then
        msg := FmtMessage( ExpandConstant('{cm:iAlreadyInstalledVer}'), [ otherAppName, ver ] )
    end;
    if MsgBox( msg, mbConfirmation, MB_YESNO) = IDYES then begin
      // user wants to uninstall
      r := HKLMRegQueryStringValue('{#UNINSTKEY}' + uuid, 'QuietUninstallString', fname);
      if not r then
        r := HKLMRegQueryStringValue('{#UNINSTKEY}' + uuid, 'UninstallString', fname);
      if r then begin
        fname := SplitParameters(fname, params);
        if Lowercase(fname) = 'msiexec.exe' then begin
          params := '/quiet /X' + uuid;
        end;
        res := 1;
        //if CopyFileToTemp(fname) then begin
          Exec(fname, params, ExpandConstant('{tmp}'), SW_SHOW, ewWaitUntilTerminated, res);
        //end;
        if res = 0 then begin
          Result := true;
        end else begin
          MsgBox( FmtMessage( ExpandConstant('{cm:iUninstallFailed}'), [ otherAppName, ver] ), mbCriticalError, MB_OK);
        end;
      end;
    end;
  end;
end;

//------------------------------------------------------------------------------
//function MsiEnumRelatedProducts(lpUpgradeCode : String; dwReserved, iProductIndex : Cardinal; var lpProductBuf : TProductBuf): Cardinal;
function MsiEnumRelatedProducts(lpUpgradeCode : String; dwReserved, iProductIndex : Cardinal; lpProductBuf : String): Cardinal;
external 'MsiEnumRelatedProductsW@msi.dll setuponly stdcall';

//------------------------------------------------------------------------------
// look of old versions, installed by Microsoft Installer using the
// upgrade code and try to uninstall.
// returns TRUE, of all old versions have been uninstalled successfully
function removeMsiApplications(const UpgradeCodes : Array of String): Boolean;
var
  ret, i, j : Cardinal;
  ProductCode : String;
begin
  Result := true;
  for j := 0 to High(UpgradeCodes) do begin
    i := 0;
    repeat
      SetLength(ProductCode, 255);
      ret := MsiEnumRelatedProducts(UpgradeCodes[j], 0, i, ProductCode);
      SetLength(ProductCode, Pos(#0, ProductCode)-1);                                           
      if ret = 0 then begin                          
        if not uninstallOther(ProductCode) then
          ret := 1;
      end;
      i := i+1;
    until (ret <> 0); 
  end;
end;

#ifdef myHttpPort
//------------------------------------------------------------------------------
// get HTTP Port from Registry, of return default port Value
function getHttpPort(subKey : String) : String;
var
  port : Cardinal;
begin
  Result := '{#myHttpPort}';
  if RegQueryDWordValue(HKLM, subKey, 'HttpPort', port) then
    Result := IntToStr(port);
end;
#endif


//------------------------------------------------------------------------------
#ifdef MY_INITSETUP
function myInitializeSetup() : Boolean; forward;
#endif

//------------------------------------------------------------------------------
function InitializeSetup() : Boolean;
var
  id : String;
begin
  Result := true;
  id := initVars;
  if isInstalled(getInnoUninstallUUID(id)) then begin
    Result := uninstallOther(getInnoUninstallUUID(id));
  end;
  initCache(id);
#ifdef MY_INITSETUP
  Result := Result and myInitializeSetup;
#endif
#ifdef _DEBUG
  MsgBox('End: InitializeSetup; Result = ' + BoolToStr(Result)+ #13 + \
         '  cachedSuiteInstalled = '       + BoolToStr(cachedSuiteInstalled) + #13 + \
         '  cachedRuntimeInstalled = '     + BoolToStr(cachedRuntimeInstalled) + #13 + \
         '  cachedVCredistInstalled = '   + BoolToStr(cachedVCRedistInstalled) + #13 + \
         '  cachedRuntimeRequired = '     + BoolToStr(cachedRuntimeRequired) + #13 + \
        '   myAppID = ' + id \
    , mbInformation, MB_OK);
#endif
end;

//------------------------------------------------------------------------------
#ifdef MY_UNINSTINIT
function myInitializeUninstall() : Boolean; forward;
#endif

//------------------------------------------------------------------------------
function InitializeUninstall() : Boolean;
var
  id : String;
begin
  Result := true;
  id := initVars;
  initCache(id);
#ifdef MY_UNINSTINIT
  Result := Result and myInitializeUninstall;
#endif
#ifdef _DEBUG
  MsgBox('End: InitializeUninstall; Result = ' + BoolToStr(Result)+ #13 + \
         '  cachedSuiteInstalled = '       + BoolToStr(cachedSuiteInstalled) + #13 + \
         '  cachedRuntimeInstalled = '     + BoolToStr(cachedRuntimeInstalled) + #13 + \
         '  cachedVCredistInstalled = '   + BoolToStr(cachedVCRedistInstalled) + #13 + \
         '  cachedRuntimeRequired = '     + BoolToStr(cachedRuntimeRequired) + #13 + \
        '   myAppID = ' + id \
    , mbInformation, MB_OK);
#endif
end;

//------------------------------------------------------------------------------
procedure InitializeUninstallProgressForm();
var
  n : Longint;
begin
  n := 1;
#ifdef CLOSE_APPS
  n := {#CLOSE_APPS};
#endif
#ifdef CITHID_MTF
  n := n + 2;
#endif
  if (UninstallProgressForm.ProgressBar.Max > n) then begin
    uninstallProgressbarStep := UninstallProgressForm.ProgressBar.Max / n;
    if uninstallProgressbarStep < 1 then begin
      uninstallProgressbarStep := 1;
      UninstallProgressForm.ProgressBar.Max := n;
    end;
  end else begin
      uninstallProgressbarStep := 1;
      UninstallProgressForm.ProgressBar.Max := n;
  end;    
#ifdef _DEBUG
  MsgBox('n = ' + IntToStr(n) + \
      + #13 + 'Adjusted UninstallProgressForm.ProgressBar.Max = ' + IntToStr(UninstallProgressForm.ProgressBar.Max) \
      + #13 + 'uninstallProgressBarStep = ' + IntToStr(uninstallProgressBarStep) \
      , mbInformation, MB_OK);
#endif
end;

//------------------------------------------------------------------------------
#ifdef CLOSE_APPS
procedure closeApplicationsForUninstall(); forward;
#endif
#ifdef MY_UNINSTALLSTEP
procedure myUninstallStep(); forward;
#endif

//------------------------------------------------------------------------------
procedure CurUninstallStepChanged(CurUninstallStep:TUninstallStep);
var
  fname, wdir : String;
  res : Integer;
begin
  case CurUninstallStep of
    usUninstall:
      begin
#ifdef CLOSE_APPS
        // close running applications
        UninstallProgressForm.StatusLabel.Caption := ExpandConstant('{cm:Ucls}');
        closeApplicationsForUninstall;
#endif
#ifdef MY_UNINSTALLSTEP
        myUninstallStep;
#endif
      end;
  end;
end;

//------------------------------------------------------------------------------
#ifdef MY_READYMEMO
function myUpdateReadyMemo(Space, Newline : String; var first : Boolean) : String; forward;
#endif

function UpdateReadyMemo(Space, Newline, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo, MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo:String) : String;
var 
  memo : String;
  first : Boolean;
begin
  memo := '';
  first := true;
  if MemoUserInfoInfo <> '' then begin
    memo := memo + MemoUserInfoInfo + Newline;
    first := false;
  end;
  if MemoDirInfo <> '' then begin
    if not first then begin
      memo := memo + Newline;
    end;
    memo := memo + MemoDirInfo + Newline;
    first := false;
    if isSuiteInstalled('') then begin
      memo:= memo + Space + ' (' + ExpandConstant('{cm:iFixedPath}') + ')' + Newline;
    end;
  end;
  if MemoTypeInfo <> '' then begin
    if not first then begin
      memo := memo + Newline;
    end;
    memo := memo + MemoTypeInfo + Newline;
    first := false;
  end;
  if MemoComponentsInfo <> '' then begin
    if not first then begin
      memo := memo + Newline;
    end;
    memo := memo + MemoComponentsInfo + Newline;
    first := false;
  end;
  if MemoGroupInfo <> '' then begin
    if not first then begin
      memo := memo + Newline;
    end;
    memo := memo + MemoGroupInfo + Newline;
    if isSuiteInstalled('') then begin
      memo:= memo + Space + ' (' + ExpandConstant('{cm:iFixedGroup}') + ')' + Newline;
    end;
    first := false;
  end;
  if MemoTasksInfo <> '' then begin
    if not first then begin
      memo := memo + Newline;
    end;
    memo := memo + MemoTasksInfo + Newline;
    first := false;
  end;
#ifdef MY_READYMEMO
  memo := memo + myUpdateReadyMemo(Space, Newline, first);
#endif
  Result := memo;
end;

//------------------------------------------------------------------------------
function ShouldSkipPage(PageID:Integer) : Boolean;
begin
  Result := false;
  if ((PageID=wpSelectDir) or (PageID=wpSelectProgramGroup)) and (isSuiteInstalled('')) then
    Result := true;
end;

//------------------------------------------------------------------------------
function DelayRestart : Boolean;
begin
  Result := true;
  sleep(500);
end;

//------------------------------------------------------------------------------
#ifdef MY_STEPINSTALL
procedure myStepInstall; forward;
#endif
#ifdef MY_STEPPOSTINSTALL
procedure myStepPostInstall; forward;
#endif
#ifdef MY_STEPDONE
procedure myStepDone; forward;
#endif

//------------------------------------------------------------------------------
procedure CurStepChanged(CurStep: TSetupStep);
begin
  case CurStep of
    ssInstall:
      begin
#ifdef MY_STEPINSTALL
        myStepInstall;
#endif
      end;
    ssPostInstall:
      begin
#ifdef MY_STEPPOSTINSTALL
        myStepPostInstall;
#endif
      end;
    ssDone:
      begin
#ifdef MY_STEPDONE
        myStepDone;
#endif
      end;
  end;
end;

[Code]

var
  xtcDirPage : TInputDirWizardPage;

function getMyAppID(param:string) : String;
begin
  Result := UUID_App;
end;

procedure CurPageChanged(const iCurPage: integer);
begin
  if (iCurPage = wpSelectTasks) then
  begin
    // check GUI desktop link
    setTaskChecked('{cm:TSdt}', previousSelection('DesktopIcon', false));
  end;
end;

procedure InitializeWizard;
var
  x : String;
begin
  // get path settings from previous installation
  // - no paths -
  
  // if there are already some settings on this machine, they do have priority
end;

function myUpdateReadyMemo(Space, Newline : String; var first : Boolean) : String;
var
  memo : String;
begin
  memo := Newline + ExpandConstant('{cm:appLanguageCaption}') + Newline;
  memo := memo +Space +  ExpandConstant('{cm:appLanguage}') + Newline;
  Result := memo;
end;

procedure closeApplicationsForUninstall();
var
  wdir, fname : String;
  res : Integer;
begin
  wdir := ExpandConstant('{app}\');
  fname := wdir + '{#myAppExeName}';
  Exec(fname, '-close', wdir, SW_HIDE, ewWaitUntilTerminated, res);
end;

#ifdef _DEBUG
#expr SaveToFile(myAppBaseName + "PP.iss")
#endif

[UninstallDelete]

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"

[CustomMessages]
en.CMgui=Start {#myAppBaseName}
de.CMgui=Das {#myAppBaseName} starten

en.Rgui=Start {#myAppBaseName}
de.Rgui=Das {#myAppBaseName} starten

en.appLanguageCaption=User Interface Language of Application
en.appLanguage=Language=English
de.appLanguageCaption=Sprache des Benutzerinfaces der Applikation
de.appLanguage=Sprache=Deutsch

en.TSicongroup=Set Short-Cuts
de.TSicongroup=Programverknüpfungen

en.TSas=add {#myAppBaseName} to startup folder
de.TSas={#myAppBaseName} zum Autostart-Ordner hinzufügen

en.TSdt=add {#myAppBaseName} icon to desktop
de.TSdt={#myAppBaseName} zum Desktop hinzufügen

en.TSothergroup=Other Settings
de.TSothergroup=Weitere Einstellungen

en.iFixedPath=fixed path, where other components of the suite are installed already
de.iFixedPath=fester Pfad, in den bereits andere Komponenten der Suite installiert sind

en.iFixedGroup=fixed program group, other components of the suite are installed already
de.iFixedGroup=feste Programmgruppe, andere Komponenten der Suite sind bereits installiert

en.iNoInstallation=is already installed, nothing to do
de.iNoInstallation=Ist bereits installiert, keine weitere Aktion nötig

en.iInstall=will be installed.
de.iInstall=Wird installiert.

en.iAlreadyInstalledVer=The application '%1' is installed already!%n%nDo you want to uninstall %1 version %2 before installing {#myAppBaseName} version {#myAppVersion}?
en.iAlreadyInstalled=The application '%1' is installed already!%n%nDo you want to uninstall %1 before installing {#myAppBaseName} version {#myAppVersion}?
de.iAlreadyInstalledVer=Die Applikation '%1' ist bereits installiert!%n%nWollen Sie zuerst %1 Version %2 deinstallieren, bevor Sie {#myAppBaseName} Version {#myAppVersion} installieren?
de.iAlreadyInstalled=Die Applikation '%1' ist bereits installiert!%n%nWollen Sie zuerst %1 deinstallieren, bevor Sie {#myAppBaseName} Version {#myAppVersion} installieren?

en.iUninstallFailed=Uninstallation of %1 version %2 failed%n... aborting installation
de.iUninstallFailed=Deinstallation von %1 Version %2 fehlgeschlagen%n... die Installation wird abgebrochen

en.Rvc=Installing Visual-C++ 2015 runtime libraries 
de.Rvc=Installiere Visual-C++ 2015 Laufzeitumgebung

en.Ucls=Closing applications
de.Ucls=Anwendungen werden beendet
