// TA Unit Wizard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "TA Unit Wizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


void WINAPI GetWizardInfo(LPWIZARD_INFO_t pWizInfo)
{
	strcpy(pWizInfo->Title, "Total Annihilation Script Creation Wizard");
	strcpy(pWizInfo->Description,
		"Automates the redundant process of making a new unit script from scratch");
	pWizInfo->Validation = WIZARD_VALIDATION;
}

void WINAPI DoWizard(LPDO_WIZARD_INFO_t pDoWizInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Initialize the completion flag to FALSE just in case
	pDoWizInfo->bWizardCompleted = FALSE;

	// Sanity check
	if (pDoWizInfo == NULL)
		return;

	// Set the global pointer
	g_pDoWizardInfo = pDoWizInfo;
}


/////////////////////////////////////////////////////////////////////////////
// CTAUnitWizardApp

BEGIN_MESSAGE_MAP(CTAUnitWizardApp, CWinApp)
//{{AFX_MSG_MAP(CTAUnitWizardApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTAUnitWizardApp construction

CTAUnitWizardApp::CTAUnitWizardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTAUnitWizardApp object

CTAUnitWizardApp theApp;

LPDO_WIZARD_INFO_t g_pDoWizardInfo;
