// Compiler.h : main header file for the COMPILER DLL
//

#if !defined(AFX_COMPILER_H__ECAC0A45_BDE3_11D3_BA3A_0080C8C11E51__INCLUDED_)
#define AFX_COMPILER_H__ECAC0A45_BDE3_11D3_BA3A_0080C8C11E51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Exports.h"

/////////////////////////////////////////////////////////////////////////////
// CCompilerApp
// See Compiler.cpp for the implementation of this class
//

class CCompilerApp : public CWinApp
{
public:
	CCompilerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompilerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCompilerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPILER_H__ECAC0A45_BDE3_11D3_BA3A_0080C8C11E51__INCLUDED_)
