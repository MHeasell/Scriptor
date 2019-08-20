// DebugOutputWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Scriptor.h"
#include "DebugOutputWindow.h"

#include "ChildFrm.h"
#include "ScriptorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugOutputWindow

CDebugOutputWindow::CDebugOutputWindow()
{
	//m_ErrorList.SetSize(0);
}

CDebugOutputWindow::~CDebugOutputWindow()
{
}


BEGIN_MESSAGE_MAP(CDebugOutputWindow, CEdit)
//{{AFX_MSG_MAP(CDebugOutputWindow)
ON_WM_LBUTTONDBLCLK()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Operations

BOOL CDebugOutputWindow::AddTextLine(LPCTSTR strText)
{
	char NewTextLine[512];
	if (strText == NULL)
		sprintf(NewTextLine, "\r\n");
	else
		sprintf(NewTextLine, "%s\r\n", strText);

	int Line = GetLineCount();
	int Index = LineIndex(Line);
	SetSel(Index, Index);
	ReplaceSel(NewTextLine);
	SetSel(-1, -1);

	return TRUE;
}

BOOL CDebugOutputWindow::AddErrorLine(LPCTSTR strText, LPCTSTR strFile, WORD wLine)
{
	char NewTextLine[512];
	sprintf(NewTextLine, "%s\r\n", strText);

	int Line = GetLineCount();
	int Index = LineIndex(Line--);
	SetSel(Index, Index);
	ReplaceSel(NewTextLine);
	SetSel(-1, -1);

	ErrorListElement Element;
	strcpy(Element.FilePath, strFile);
	Element.ErrorLineNumber = wLine;

	//m_ErrorList.Add( Element );
	m_ErrorList.SetAt(Line, Element);

	return TRUE;
}

void CDebugOutputWindow::ClearOutputWindow()
{
	SetWindowText("");
	m_ErrorList.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// CDebugOutputWindow message handlers

void CDebugOutputWindow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int Index;
	int Line;
	int Length;
	ErrorListElement Element;

	// Retrieve the line at which the button was clicked
	Index = CharFromPos(point);
	Line = LineFromChar(Index);

	// Look for this line in the error list
	if (m_ErrorList.Lookup(Line, Element))
	{
		// Select the line
		Index = LineIndex(Line);
		Length = LineLength(Index);
		SetSel(Index, Index + Length);

		// Get the line number for the error
		Line = Element.ErrorLineNumber;

		// Open the file
		if (theApp.OpenDocumentFile(Element.FilePath))
		{
			// If it could be opened, try to get the active view
			CScriptorView* pView = ((CScriptorView*)(((CMDIChildWnd*)(((CFrameWnd*)AfxGetMainWnd())->GetActiveFrame()))->GetActiveView()));
			if (pView)
			{
				pView->GotoErrorLine(Line - 1);
			}
		}
	}

	//CEdit::OnLButtonDblClk(nFlags, point);
}
