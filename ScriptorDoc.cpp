// ScriptorDoc.cpp : implementation of the CScriptorDoc class
//

#include "stdafx.h"
#include "Scriptor.h"

#include "ScriptorDoc.h"
#include "MainFrm.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptorDoc

IMPLEMENT_DYNCREATE(CScriptorDoc, CDocument)

BEGIN_MESSAGE_MAP(CScriptorDoc, CDocument)
//{{AFX_MSG_MAP(CScriptorDoc)
ON_COMMAND(ID_SCRIPT_COMPILE, OnScriptCompile)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptorDoc construction/destruction

#pragma warning(disable : 4355) // Otherwise it would complain about using \
								// 'this' in the initializer list
CScriptorDoc::CScriptorDoc() : m_xTextBuffer(this)
{
	// TODO: add one-time construction code here

	//	Initialize LOGFONT structure
	memset(&m_lf, 0, sizeof(m_lf));
	m_lf.lfWeight = FW_NORMAL;
	m_lf.lfCharSet = ANSI_CHARSET;
	m_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lf.lfQuality = DEFAULT_QUALITY;
	m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(m_lf.lfFaceName, "Courier");
}

CScriptorDoc::~CScriptorDoc()
{
}

BOOL CScriptorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_xTextBuffer.InitNew();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptorDoc serialization

void CScriptorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CScriptorDoc diagnostics

#ifdef _DEBUG
void CScriptorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CScriptorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScriptorDoc commands

void CScriptorDoc::DeleteContents()
{
	CDocument::DeleteContents();

	m_xTextBuffer.FreeAll();
}

BOOL CScriptorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// If we are trying to open a cob file, intercept it, and send it on to the handler
	auto FileType = strrchr(lpszPathName, '.');
	if ((FileType != NULL) && (stricmp(FileType + 1, "cob") == 0))
	{
		strcpy(theApp.m_AllPurposeString, lpszPathName);
		PostMessage(NULL, SCRIPTOR_OPENCOB, 0, 0);
		return FALSE;
	}

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return m_xTextBuffer.LoadFromFile(lpszPathName);
}

BOOL CScriptorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	m_xTextBuffer.SaveToFile(lpszPathName);

	return TRUE;
}

LRESULT WINAPI CompileDebugOutput(LPCTSTR File, LPCTSTR ErrStr, WORD Line, char ErrType)
{
	// Get a pointer to the debug window
	CDebugOutputWindow* pDebugWindow = ((CMainFrame*)theApp.m_pMainWnd)->m_DebugOutputBar.GetDebugWindow();

	// Check for a simple message string
	if (Line == 0)
	{
		pDebugWindow->AddTextLine(ErrStr);
	}
	else
	{
		// Format and output the error string
		CString NewErrorString;
		NewErrorString.Format("%s (%d): %s    [ %s ]",
			ErrType ? "Error" : "Warning",
			Line,
			ErrStr,
			File);
		pDebugWindow->AddErrorLine(NewErrorString, File, Line);
	}

	return 1;
}

LRESULT WINAPI CompileProgress(DWORD BytesRead, DWORD TotalBytes)
{
	if (TotalBytes > 1000)
	{
		BytesRead /= 1000;
		TotalBytes /= 1000;
	}
	CMainFrame* pFrame = ((CMainFrame*)theApp.m_pMainWnd);
	if (pFrame->m_bScriptProgressCtrlCreated)
	{
		pFrame->m_ScriptProgress.SetRange(0, (short)TotalBytes);
		pFrame->m_ScriptProgress.SetPos(BytesRead);
	}
	return 1;
}

void CScriptorDoc::OnScriptCompile()
{
	int Ret;
	int pos;
	CString FilePath;
	CString CobPath;
	CString CobFile;
	CString Dir;

	theApp.DoWaitCursor(1);

	OnFileSave(); // Save current file

	FilePath = GetPathName();
	if (FilePath != "")
	{
		pos = FilePath.ReverseFind('.');
		if (pos == (-1))
		{
			CobPath = FilePath + ".cob";
		}
		else if ((FilePath[pos + 1] == 'h') || (FilePath[pos + 1] == 'H'))
		{
			AfxMessageBox("Cannot compile \'.h\' libraries");
			return;
		}
		else
		{
			CobPath = FilePath.Left(pos) + ".cob";
		}
		pos = CobPath.ReverseFind('\\');
		if (LocalSettings.CobFile_SameAsBos)
		{
			CobFile = CobPath.Right(CobPath.GetLength() - (pos + 1));
		}
		else
		{
			CobFile = LocalSettings.CobFile;
		}
		if (LocalSettings.CobDir_SameAsBos)
		{
			CobPath = CobPath.Left(pos + 1);
		}
		else
		{
			CobPath = LocalSettings.CobDir;
		}
		CobPath += CobFile;

		((CMainFrame*)AfxGetMainWnd())->InitializeScriptControls();

		Dir = FilePath.Left(FilePath.ReverseFind('\\') + 1);
		if (CompilerSettings.CurrentDirectory)
			delete[] CompilerSettings.CurrentDirectory;
		CompilerSettings.CurrentDirectory = new char[Dir.GetLength() + 1];
		strcpy(CompilerSettings.CurrentDirectory, Dir);
		if (CompilerSettings.CurrentFile)
			delete[] CompilerSettings.CurrentFile;
		CompilerSettings.CurrentFile = new char[FilePath.GetLength() + 1];
		strcpy(CompilerSettings.CurrentFile, FilePath);

		// Call the compile function from the CompilerXX.dll
		Ret = BOSCOM_Compile(BosCom, FilePath, CobPath, CompilerSettings, CompileDebugOutput, CompileProgress);

		// Get a pointer to the debug window
		CDebugOutputWindow* pDebugWindow = ((CMainFrame*)theApp.m_pMainWnd)->m_DebugOutputBar.GetDebugWindow();

		if (Ret == 0)
		{
			PlaySound("SystemExclamation", 0, SND_ALIAS | SND_NOWAIT);
			pDebugWindow->AddTextLine();
			pDebugWindow->AddTextLine("One or more errors occured while compiling the script.");
			//pDebugWindow->AddTextLine( "Find " );
		}
		else if (Ret == 2)
		{
			PlaySound("SystemQuestion", 0, SND_ALIAS | SND_NOWAIT);
			pDebugWindow->AddTextLine();
			pDebugWindow->AddTextLine("One or more warnings occured while compiling the script.");
		}
		else
		{
			PlaySound("SystemAsterisk", 0, SND_ALIAS | SND_NOWAIT);
		}

		((CMainFrame*)AfxGetMainWnd())->DestroyScriptControls();
	}
	theApp.DoWaitCursor(-1);
}
