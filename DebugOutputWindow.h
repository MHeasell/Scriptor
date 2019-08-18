#if !defined(AFX_DEBUGOUTPUTWINDOW_H__8DD8FD7D_CBB8_454C_AF7A_49C0A31E08A7__INCLUDED_)
#define AFX_DEBUGOUTPUTWINDOW_H__8DD8FD7D_CBB8_454C_AF7A_49C0A31E08A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DebugOutputWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebugOutputWindow window

class CDebugOutputWindow : public CEdit
{
	// Construction
public:
	CDebugOutputWindow();

	// Attributes
public:
	struct ErrorListElement
	{
		char FilePath[MAX_PATH];
		WORD ErrorLineNumber;
	};

	CMap<int, int, ErrorListElement, ErrorListElement&> m_ErrorList;

	// Operations
public:
	BOOL AddTextLine(LPCTSTR strText = NULL);
	BOOL AddErrorLine(LPCTSTR strText, LPCTSTR strFile, WORD wLine);
	void ClearOutputWindow();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugOutputWindow)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CDebugOutputWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDebugOutputWindow)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGOUTPUTWINDOW_H__8DD8FD7D_CBB8_454C_AF7A_49C0A31E08A7__INCLUDED_)
