// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__ADD0AEE1_B395_4590_8FBD_693B97754934__INCLUDED_)
#define AFX_MAINFRM_H__ADD0AEE1_B395_4590_8FBD_693B97754934__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DebugOutputBar.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CProgressCtrl       m_ScriptProgress;
	BOOL                m_bScriptProgressCtrlCreated;
    CDebugOutputBar     m_DebugOutputBar;

// Operations
public:
    void InitializeScriptControls();
    void DestroyScriptControls();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar      m_wndStatusBar;
	CToolBar        m_wndToolBar;
    CToolBar        m_EditBar;
    CToolBar        m_CompilerTools;

    void DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf);
    BOOL VerifyBarState(LPCTSTR lpszProfileName);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnScriptCompilerSettings();
	afx_msg void OnCompilerSelectTa();
	afx_msg void OnUpdateCompilerSelectTa(CCmdUI* pCmdUI);
	afx_msg void OnCompilerSelectTak();
	afx_msg void OnUpdateCompilerSelectTak(CCmdUI* pCmdUI);
	afx_msg void OnViewTollbarsCompiler();
	afx_msg void OnUpdateViewTollbarsCompiler(CCmdUI* pCmdUI);
	afx_msg void OnViewTollbarsDebug();
	afx_msg void OnUpdateViewTollbarsDebug(CCmdUI* pCmdUI);
	afx_msg void OnViewTollbarsEdit();
	afx_msg void OnUpdateViewTollbarsEdit(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnUpdateIndicatorPosition(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateIndicatorOvr(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateIndicatorCaps(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateIndicatorNum(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateIndicatorScrl(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__ADD0AEE1_B395_4590_8FBD_693B97754934__INCLUDED_)
