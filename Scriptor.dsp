# Microsoft Developer Studio Project File - Name="Scriptor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Scriptor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Scriptor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Scriptor.mak" CFG="Scriptor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Scriptor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Scriptor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Scriptor - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib imagehlp.lib Compiler10.lib Decompiler10.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Scriptor - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib imagehlp.lib Compiler10.lib Decompiler10.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Scriptor - Win32 Release"
# Name "Scriptor - Win32 Debug"
# Begin Group "Core Files"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Scriptor.cpp
# End Source File
# Begin Source File

SOURCE=.\Scriptor.rc
# End Source File
# Begin Source File

SOURCE=.\ScriptorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptorView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Scriptor.h
# End Source File
# Begin Source File

SOURCE=.\ScriptorDoc.h
# End Source File
# Begin Source File

SOURCE=.\ScriptorView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\compiler.bmp
# End Source File
# Begin Source File

SOURCE=.\res\edit_bar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Scriptor.ico
# End Source File
# Begin Source File

SOURCE=.\res\Scriptor.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ScriptorDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Scriptor.reg
# End Source File
# End Group
# Begin Group "Support Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DebugOutputBar.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugOutputBar.h
# End Source File
# Begin Source File

SOURCE=.\DebugOutputWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugOutputWindow.h
# End Source File
# Begin Source File

SOURCE=.\Descript.cpp
# End Source File
# Begin Source File

SOURCE=.\Descript.h
# End Source File
# Begin Source File

SOURCE=.\ScriptCompiler.h
# End Source File
# Begin Source File

SOURCE=.\ScriptDeompiler.h
# End Source File
# Begin Source File

SOURCE=.\ScriptorBos.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDialog.h
# End Source File
# Begin Source File

SOURCE=.\UnitWizards.cpp
# End Source File
# Begin Source File

SOURCE=.\UnitWizards.h
# End Source File
# Begin Source File

SOURCE=.\util_TxtParse.cpp
# End Source File
# Begin Source File

SOURCE=.\util_TxtParse.h
# End Source File
# End Group
# Begin Group "Additional Files"

# PROP Default_Filter ""
# Begin Group "Crystal Edit Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalEditView.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalEditView.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalEditView.inl"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalTextBuffer.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalTextBuffer.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalTextBuffer.inl"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalTextView.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\ccrystaltextview.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalTextView.inl"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CCrystalTextView2.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\cedefs.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CEditReplaceDlg.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CEditReplaceDlg.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CFindTextDlg.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\CFindTextDlg.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\editcmd.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\editreg.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\mg_cur.cur"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Crystal Edit\mg_icons.bmp"
# End Source File
# End Group
# Begin Group "Sizing Control Bar Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Additional Source\Sizing Control Bar\scbarg.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Sizing Control Bar\scbarg.h"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Sizing Control Bar\sizecbar.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Sizing Control Bar\sizecbar.h"
# End Source File
# End Group
# Begin Group "Directory Dialog Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Additional Source\DirDialog\DirDialog.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\DirDialog\DirDialog.h"
# End Source File
# End Group
# Begin Group "Hyperlink Control Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\Additional Source\Hyperlink Control\HyperLink.cpp"
# End Source File
# Begin Source File

SOURCE=".\Additional Source\Hyperlink Control\hyperlink.h"
# End Source File
# End Group
# End Group
# End Target
# End Project
