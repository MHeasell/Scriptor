// Decompiler.h : main header file for the DECOMPILER DLL
//

#if !defined(AFX_DECOMPILER_H__AF869705_CD99_11D3_BA3A_0080C8C11E51__INCLUDED_)
#define AFX_DECOMPILER_H__AF869705_CD99_11D3_BA3A_0080C8C11E51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h" // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDecompilerApp
// See Decompiler.cpp for the implementation of this class
//

class CDecompilerApp : public CWinApp
{
public:
	CDecompilerApp();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDecompilerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDecompilerApp)
	// NOTE - the ClassWizard will add and remove member functions here.
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECOMPILER_H__AF869705_CD99_11D3_BA3A_0080C8C11E51__INCLUDED_)
