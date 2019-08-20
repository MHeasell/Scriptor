#if !defined(AFX_DEBUGOUTPUTBAR_H__37D27280_1317_46A6_A2B6_203628DC4B1B__INCLUDED_)
#define AFX_DEBUGOUTPUTBAR_H__37D27280_1317_46A6_A2B6_203628DC4B1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DebugOutputBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebugOutputBar window

#ifndef baseCMyBar
#define baseCMyBar CSizingControlBarG
#endif

#include "DebugOutputWindow.h"

class CDebugOutputBar : public baseCMyBar
{
	// Construction
public:
	CDebugOutputBar();

	// Attributes
protected:
	CDebugOutputWindow m_wndChild;
	CFont m_font;

	// Operations
public:
	CDebugOutputWindow* GetDebugWindow() { return (&m_wndChild); };

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebugOutputBar)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CDebugOutputBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDebugOutputBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEBUGOUTPUTBAR_H__37D27280_1317_46A6_A2B6_203628DC4B1B__INCLUDED_)
