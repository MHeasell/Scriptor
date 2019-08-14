// Scriptor.h : main header file for the SCRIPTOR application
//

#if !defined(AFX_SCRIPTOR_H__24851C3E_7609_4D7E_A128_369A8D010A2E__INCLUDED_)
#define AFX_SCRIPTOR_H__24851C3E_7609_4D7E_A128_369A8D010A2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "ScriptCompiler.h"
#include "ScriptDeompiler.h"


enum SYS_CUSTOM_SCRIPTOR_MESSAGES
{
    SCRIPTOR_OPENCOB =    ( WM_USER+0x200 ),
};

struct BOS_KEYWORD_t
{
    char            Keyword[32];
    BOS_KEYWORD_t*  Next;

    BOS_KEYWORD_t( LPTSTR strWord=NULL, BOS_KEYWORD_t* pNext=NULL )
    {
        if( strWord ) strcpy( Keyword, strWord );
        else Keyword[0]='\0';
        Next = pNext;
    }
    ~BOS_KEYWORD_t()
    {
        delete Next;
    }
};

typedef struct LOCAL_SETTINGS
{
	char CobFile_SameAsBos;
	char CobDir_SameAsBos;
	CString CobFile;
	CString CobDir;
	CStringArray LastCobArr;
	CStringArray LastBosArr;
	char De_Bos_SA_Cob;
}* LPLOCAL_SETTINGS;

extern LPVOID               BosCom;
extern LPVOID               BosDec;
extern BOS_KEYWORD_t*       g_BosKeywords;
extern SETTINGS             CompilerSettings;
extern DECOM_SETTINGS       DecompilerSettings;
extern LOCAL_SETTINGS       LocalSettings;


/////////////////////////////////////////////////////////////////////////////
// CScriptorApp:
// See Scriptor.cpp for the implementation of this class
//

class CScriptorApp : public CWinApp
{
protected:

    // The font obtained at loadtime from the registry
    LOGFONT         m_OriginalFont;

	void GetProfileFont(LPCTSTR szSec, LOGFONT* plf);
	void WriteProfileFont(LPCTSTR szSec, const LOGFONT* plf, LOGFONT* plfOld);
	void GetProfileSettings(LPCTSTR szSec);
	void WriteProfileSettings(LPCTSTR szSec);

    void LoadBosKeywords();
    void DestroyBosKeywords()
    {
        delete g_BosKeywords;
        g_BosKeywords = NULL;
    }

public:

    // The location of the executable
    char            m_AppDir[MAX_PATH];

    // The current directory at startup
    char            m_StartDir[MAX_PATH];

    char            m_AllPurposeString[MAX_PATH];

    // The font currently set for all the views 
    LOGFONT         m_CurrentFont;

    // Constructor
	CScriptorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CScriptorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnEditSetfont();
	afx_msg void OnFileDescript();
	afx_msg void OnFileUnitwizards();
	//}}AFX_MSG
    afx_msg void OnOpenCob( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

extern CScriptorApp theApp;

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CScriptorApp:
// See Scriptor v1.cpp for the implementation of this class
//
class CScriptManager : public CDocManager
{
public:
	CScriptManager();
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);
};
/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTOR_H__24851C3E_7609_4D7E_A128_369A8D010A2E__INCLUDED_)
