// ScriptorView.h : interface of the CScriptorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTORVIEW_H__A9067140_759B_4141_A246_43E002E94F9B__INCLUDED_)
#define AFX_SCRIPTORVIEW_H__A9067140_759B_4141_A246_43E002E94F9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Additional Source\Crystal Edit\CCrystalEditView.h"

class CScriptorDoc;

class CScriptorView : public CCrystalEditView
{
protected: // create from serialization only
	CScriptorView();
	DECLARE_DYNCREATE(CScriptorView)

	// Attributes
public:
	CScriptorDoc* GetDocument();

	virtual CCrystalTextBuffer* LocateTextBuffer();

protected:
	virtual DWORD ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK* pBuf, int& nActualItems);

	// Operations
public:
	void GotoErrorLine(int iLineIndex);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptorView)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CScriptorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Generated message map functions
protected:
	//{{AFX_MSG(CScriptorView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG // debug version in ScriptorView.cpp
inline CScriptorDoc* CScriptorView::GetDocument()
{
	return (CScriptorDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTORVIEW_H__A9067140_759B_4141_A246_43E002E94F9B__INCLUDED_)
