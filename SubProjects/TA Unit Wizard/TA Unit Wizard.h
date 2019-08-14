// TA Unit Wizard.h : main header file for the TA UNIT WIZARD DLL
//

#if !defined(AFX_TAUNITWIZARD_H__A2B97271_58B7_4B29_91E1_74FF57DE9145__INCLUDED_)
#define AFX_TAUNITWIZARD_H__A2B97271_58B7_4B29_91E1_74FF57DE9145__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Exports.h"

/////////////////////////////////////////////////////////////////////////////
// CTAUnitWizardApp
// See TA Unit Wizard.cpp for the implementation of this class
//

class CTAUnitWizardApp : public CWinApp
{
public:
	CTAUnitWizardApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTAUnitWizardApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTAUnitWizardApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////


extern LPDO_WIZARD_INFO_t       g_pDoWizardInfo;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAUNITWIZARD_H__A2B97271_58B7_4B29_91E1_74FF57DE9145__INCLUDED_)
