// Scriptor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Scriptor.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ScriptorDoc.h"
#include "ScriptorView.h"
#include "Descript.h"
#include "UnitWizards.h"

#include "util_TxtParse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptorApp

BEGIN_MESSAGE_MAP(CScriptorApp, CWinApp)
	//{{AFX_MSG_MAP(CScriptorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_EDIT_SETFONT, OnEditSetfont)
	ON_COMMAND(ID_FILE_DESCRIPT, OnFileDescript)
	ON_COMMAND(ID_FILE_UNITWIZARDS, OnFileUnitwizards)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
    // Custom message handlers
    ON_MESSAGE( SCRIPTOR_OPENCOB, OnOpenCob )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptorApp construction

CScriptorApp::CScriptorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CScriptorApp object

CScriptorApp theApp;

LPVOID                  BosCom = NULL;
LPVOID                  BosDec = NULL;
BOS_KEYWORD_t*          g_BosKeywords = NULL;
SETTINGS                CompilerSettings;
DECOM_SETTINGS          DecompilerSettings;
LOCAL_SETTINGS          LocalSettings;

/////////////////////////////////////////////////////////////////////////////
// CScriptorApp initialization

BOOL CScriptorApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

    // initialized OLE 2.0 libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDS_OLE_INIT_FAILED);
		return FALSE;
	}

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("KhalvKalash"));

	LoadStdProfileSettings(5);  // Load standard INI file options (including MRU)

    m_pDocManager = new CScriptManager;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_SCRIPTTYPE,
		RUNTIME_CLASS(CScriptorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CScriptorView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if(cmdInfo.m_nShellCommand!=CCommandLineInfo::FileNew)
	    if (!ProcessShellCommand(cmdInfo))
		    return FALSE;

    // Get the apps ececutable directory (from the command line arguments)
    strncpy( m_AppDir, __argv[0], strrchr(__argv[0],'\\')-(__argv[0]) );

    // Get the apps starting directory (from the current directory)
    GetCurrentDirectory( MAX_PATH, m_StartDir );
/*
    char    strTemp[1024];
    sprintf( strTemp, "m_AppDir - \"%s\"\nm_StartDir - \"%s\"",
        m_AppDir,
        m_StartDir );
    AfxMessageBox( strTemp );
*/
    // Get the apps settings
    GetProfileSettings("Settings");

    // Get the font information
    GetProfileFont( "Font", &m_OriginalFont );
    // Set it as the current font
	m_CurrentFont = m_OriginalFont;

#ifndef _DEBUG
    SetCurrentDirectory( m_AppDir );
#endif

    // Get the list of bos keywords
    LoadBosKeywords();

    // Initialize the compiler
    BosCom = BOSCOM_Initialize();

    // Initialize the decompiler
	BosDec = BOSDEC_Initialize();

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

void CScriptorApp::GetProfileSettings(LPCTSTR szSec)
{
	CString Str;
	LocalSettings.CobFile_SameAsBos = GetProfileInt(szSec, "CobFile_SAB", 1);
	LocalSettings.CobDir_SameAsBos = GetProfileInt(szSec, "CobDir_SAB", 1);
	LocalSettings.CobFile = GetProfileString(szSec, "CobFile", "CobFile");
	LocalSettings.CobDir = GetProfileString(szSec, "CobDir", "C:\\");
	//LocalSettings.LastCob = GetProfileString(szSec, "LastCob", "C:\\*.cob");
	//LocalSettings.LastBos = GetProfileString(szSec, "LastBos", "C:\\*.bos");
	LocalSettings.De_Bos_SA_Cob = GetProfileInt(szSec, "DeBos_SAC", 0);

	LocalSettings.LastCobArr.SetSize(0);
	Str = GetProfileString(szSec, "LastCob");
	if(!Str.IsEmpty())
	{
		LocalSettings.LastCobArr.Add(Str);
		WriteProfileString(szSec, "LastCob",NULL);
	}
	for(int x=1;true;x++)
	{
		Str.Format("DeCobFile%d",x);
		Str = GetProfileString(szSec, Str, "");
		if(Str.IsEmpty()) break;
		LocalSettings.LastCobArr.Add(Str);
	}
	LocalSettings.LastBosArr.SetSize(0);
	Str = GetProfileString(szSec, "LastBos");
	if(!Str.IsEmpty())
	{
		LocalSettings.LastBosArr.Add(Str);
		WriteProfileString(szSec, "LastBos",NULL);
	}
	for(x=1;true;x++)
	{
		Str.Format("DeBosFile%d",x);
		Str = GetProfileString(szSec, Str, "");
		if(Str.IsEmpty()) break;
		LocalSettings.LastBosArr.Add(Str);
	}

	CString Angular = GetProfileString(szSec, "Angular", "182.0");
	CompilerSettings.AngularConst = (float)atof(Angular);
	CString Linear = GetProfileString(szSec, "Linear", "163840.0");
	CompilerSettings.LinearConst = (float)atof(Linear);
	CompilerSettings.TA = GetProfileInt(szSec, "TA", 1);

    CString DefaultIncludeDirectory = m_StartDir;
    DefaultIncludeDirectory += "\\include\\";
	Str = GetProfileString(szSec, "Include", DefaultIncludeDirectory);
	CompilerSettings.IncludeDirectory = new char[Str.GetLength()+1];
	strcpy(CompilerSettings.IncludeDirectory,Str);

	CompilerSettings.CurrentDirectory = NULL;
	CompilerSettings.CurrentFile = NULL;

	DecompilerSettings.ShowOffsets = GetProfileInt(szSec, "ShowOffsets", 0);
	DecompilerSettings.ShowHeader = GetProfileInt(szSec, "ShowHeader", 0);
	DecompilerSettings.ShowPush = GetProfileInt(szSec, "ShowPush", 0);
	DecompilerSettings.DoGuess = GetProfileInt(szSec, "DoGuess", 1);
	DecompilerSettings.Precision = GetProfileInt(szSec, "Precision", 6);
	DecompilerSettings.ShowSndList = GetProfileInt(szSec, "ShowSndList", 0);
	DecompilerSettings.ShowStdLib = GetProfileInt(szSec, "ShowStdLib", 0);
	DecompilerSettings.ShowMiscLib = GetProfileInt(szSec, "ShowMiscLib", 0);
	DecompilerSettings.AngConst = CompilerSettings.AngularConst;
	DecompilerSettings.LinConst = CompilerSettings.LinearConst;
	DecompilerSettings.IncDec = GetProfileInt(szSec, "IncDec", 1);
	DecompilerSettings.TryKeepSmall = GetProfileInt(szSec, "TryKeepSmall", 1);
	DecompilerSettings.ShowReturn = GetProfileInt(szSec, "ShowReturn", 0);
}

void CScriptorApp::WriteProfileSettings(LPCTSTR szSec)
{
	WriteProfileInt(szSec, "CobFile_SAB", LocalSettings.CobFile_SameAsBos);
	WriteProfileInt(szSec, "CobDir_SAB", LocalSettings.CobDir_SameAsBos);
	WriteProfileString(szSec, "CobFile", LocalSettings.CobFile);
	WriteProfileString(szSec, "CobDir", LocalSettings.CobDir);
	//WriteProfileString(szSec, "LastCob", LocalSettings.LastCob);
	//WriteProfileString(szSec, "LastBos", LocalSettings.LastBos);
	WriteProfileInt(szSec, "DeBos_SAC", LocalSettings.De_Bos_SA_Cob);

	CString Str;
	for(int x=0;x<LocalSettings.LastCobArr.GetSize();x++)
	{
		Str.Format("DeCobFile%d",x+1);
		WriteProfileString(szSec, Str, LocalSettings.LastCobArr[x]);
	}
	for(x=0;x<LocalSettings.LastBosArr.GetSize();x++)
	{
		Str.Format("DeBosFile%d",x+1);
		WriteProfileString(szSec, Str, LocalSettings.LastBosArr[x]);
	}

	CString Angular;
	Angular.Format("%f",CompilerSettings.AngularConst);
	WriteProfileString(szSec, "Angular", Angular);
	CString Linear;
	Linear.Format("%f",CompilerSettings.LinearConst);
	WriteProfileString(szSec, "Linear", Linear);
	WriteProfileInt(szSec, "TA", CompilerSettings.TA);
	WriteProfileString(szSec, "Include", CompilerSettings.IncludeDirectory);

	WriteProfileInt(szSec, "ShowOffsets", DecompilerSettings.ShowOffsets);
	WriteProfileInt(szSec, "ShowHeader", DecompilerSettings.ShowHeader);
	WriteProfileInt(szSec, "ShowPush", DecompilerSettings.ShowPush);
	WriteProfileInt(szSec, "DoGuess", DecompilerSettings.DoGuess);
	WriteProfileInt(szSec, "Precision", DecompilerSettings.Precision);
	WriteProfileInt(szSec, "ShowSndList", DecompilerSettings.ShowSndList);
	WriteProfileInt(szSec, "ShowStdLib", DecompilerSettings.ShowStdLib);
	WriteProfileInt(szSec, "ShowMiscLib", DecompilerSettings.ShowMiscLib);
	WriteProfileInt(szSec, "IncDec", DecompilerSettings.IncDec);
	WriteProfileInt(szSec, "TryKeepSmall", DecompilerSettings.TryKeepSmall);
	WriteProfileInt(szSec, "ShowReturn", DecompilerSettings.ShowReturn);
}

void CScriptorApp::GetProfileFont(LPCTSTR szSec, LOGFONT* plf)
{
    ZeroMemory( plf, sizeof(LOGFONT) );
	plf->lfHeight = GetProfileInt(szSec, "Height", 12);
	plf->lfWeight = GetProfileInt(szSec, "Weight", FW_NORMAL);
	plf->lfItalic = (BYTE)GetProfileInt(szSec, "Italic", 0);
	plf->lfUnderline = (BYTE)GetProfileInt(szSec, "Underline", 0);
	plf->lfPitchAndFamily = (BYTE)GetProfileInt(szSec, "PitchAndFamily", DEFAULT_PITCH | FF_DONTCARE);
	plf->lfCharSet = (BYTE)GetProfileInt(szSec, "CharSet", ANSI_CHARSET);
	CString strFont = GetProfileString(szSec, "FaceName", "Courier");
	lstrcpyn((TCHAR*)plf->lfFaceName, strFont, sizeof plf->lfFaceName);
	plf->lfFaceName[sizeof plf->lfFaceName-1] = 0;
    plf->lfQuality = DEFAULT_QUALITY;
	plf->lfOutPrecision = OUT_DEFAULT_PRECIS;
	plf->lfClipPrecision = CLIP_DEFAULT_PRECIS;
}

void CScriptorApp::WriteProfileFont(LPCTSTR szSec, const LOGFONT* plf, LOGFONT* plfOld)
{
	if (plf->lfHeight != plfOld->lfHeight)
		WriteProfileInt(szSec, "Height", plf->lfHeight);
    if (plf->lfHeight != plfOld->lfHeight)
		WriteProfileInt(szSec, "Height", plf->lfHeight);
	if (plf->lfWeight != plfOld->lfWeight)
		WriteProfileInt(szSec, "Weight", plf->lfWeight);
	if (plf->lfItalic != plfOld->lfItalic)
		WriteProfileInt(szSec, "Italic", plf->lfItalic);
	if (plf->lfUnderline != plfOld->lfUnderline)
		WriteProfileInt(szSec, "Underline", plf->lfUnderline);
	if (plf->lfPitchAndFamily != plfOld->lfPitchAndFamily)
		WriteProfileInt(szSec, "PitchAndFamily", plf->lfPitchAndFamily);
	if (plf->lfCharSet != plfOld->lfCharSet)
		WriteProfileInt(szSec, "CharSet", plf->lfCharSet);
	if (_tcscmp(plf->lfFaceName, plfOld->lfFaceName) != 0)
		WriteProfileString(szSec, "FaceName", (LPCTSTR)plf->lfFaceName);
	*plfOld = *plf;
}

void CScriptorApp::LoadBosKeywords()
{
    char        CfgFilePath[MAX_PATH];

#ifndef _DEBUG
    sprintf( CfgFilePath, "%s\\Compiler.cfg", m_AppDir );
#else
    sprintf( CfgFilePath, "%s\\Compiler.cfg", m_StartDir );
#endif

    HANDLE hCfgFile = CreateFile(
        CfgFilePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
		(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
		NULL );
    if( hCfgFile==INVALID_HANDLE_VALUE ) return;
    DWORD BR,Size = GetFileSize( hCfgFile, NULL );
    BYTE* pFileBuffer = new BYTE[Size];
    ReadFile( hCfgFile, pFileBuffer, Size, &BR, NULL );
    CloseHandle(hCfgFile);

    // Delete the old keywords (if any)
    DestroyBosKeywords();

    // Begin looking for keywords
    Parse_Info      Info;
    util_TxtParse   theCfgFile;
    BOOL bValid = theCfgFile.Begin( pFileBuffer, Size, &Info );

    // Find the [COMMAND] block
    while( bValid )
    {
        if( (Info.Type==PARSE_Header)&&(stricmp("COMMANDS",Info.Value)==0) )
            break;

        bValid = theCfgFile.Continue( &Info );
    }
    if( bValid==FALSE ) { delete [] pFileBuffer; return; }

    int Level = 0;
    bValid = theCfgFile.Continue( &Info );
    while( bValid )
    {
        switch( Info.Type )
        {
        case PARSE_LevelUp:
            Level++; break;
        case PARSE_LevelDown:
            Level--; break;
        case PARSE_VarVal:
            // We are only interested if the level is 1 and the variable is 'Keyword'
            if( (Level==1)&&(stricmp("Keyword",Info.Variable)==0) )
            {
                // If the first character is not '@', add it to the list
                if( Info.Value[0]!='@' )
                {
                    BOS_KEYWORD_t* pKW;
                    if( g_BosKeywords==NULL )
                    {
                        g_BosKeywords = new BOS_KEYWORD_t(Info.Value);
                    }
                    else
                    {
                        pKW = g_BosKeywords;
                        while( pKW->Next ) pKW=pKW->Next;
                        pKW->Next = new BOS_KEYWORD_t(Info.Value);
                    }
                }
            }
            break;
        }

        bValid = theCfgFile.Continue( &Info );
    }
    
}


/////////////////////////////////////////////////////////////////////////////
// CScriptManager

CScriptManager::CScriptManager():CDocManager()
{}

static void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	CDocTemplate* pTemplate, CString* pstrDefaultExt)
{
	ASSERT_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CString strFilterExt, strFilterName;
	if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) &&
	 !strFilterExt.IsEmpty() &&
	 pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) &&
	 !strFilterName.IsEmpty())
	{
		// a file based document template - add to filter list
#ifndef _MAC
		ASSERT(strFilterExt[0] == '.');
#endif
		if (pstrDefaultExt != NULL)
		{
			// set the default extension
#ifndef _MAC
			*pstrDefaultExt = ((LPCTSTR)strFilterExt) + 1;  // skip the '.'
#else
			*pstrDefaultExt = strFilterExt;
#endif
			ofn.lpstrDefExt = (LPTSTR)(LPCTSTR)(*pstrDefaultExt);
			ofn.nFilterIndex = ofn.nMaxCustFilter + 1;  // 1 based number
		}

		// add to filter
		filter += strFilterName;
		ASSERT(!filter.IsEmpty());  // must have a file type name
		filter += (TCHAR)'\0';  // next string please
#ifndef _MAC
		filter += (TCHAR)'*';
#endif
		filter += strFilterExt;
		filter += (TCHAR)'\0';  // next string please
		ofn.nMaxCustFilter++;
	}
}

BOOL CScriptManager::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
{
	CFileDialog dlgFile(bOpenFileDialog);

	CString title;
	VERIFY(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	CString strDefault;
	if (pTemplate != NULL)
	{
		ASSERT_VALID(pTemplate);
		AppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate, &strDefault);
	}
	else
	{
		// do for all doc template
		POSITION pos = m_templateList.GetHeadPosition();
		BOOL bFirst = TRUE;
		while (pos != NULL)
		{
			CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetNext(pos);
			AppendFilterSuffix(strFilter, dlgFile.m_ofn, pTemplate,
				bFirst ? &strDefault : NULL);
			bFirst = FALSE;
		}
	}

	if(bOpenFileDialog)
	{
		CString BothFilter="Scriptor Files (*.bos;*.h)";
		BothFilter += (TCHAR)'\0';   // next string please
		BothFilter += _T("*.bos;*.h");
		BothFilter += (TCHAR)'\0';   // next string please
		strFilter = BothFilter + strFilter;
	    strFilter += "Library Files (*.h)";
	    strFilter += (TCHAR)'\0';   // next string please
	    strFilter += _T("*.h");
	    strFilter += (TCHAR)'\0';   // next string please
	    strFilter += "Compiled Scripts (*.cob)";
	    strFilter += (TCHAR)'\0';   // next string please
	    strFilter += _T("*.cob");
	    strFilter += (TCHAR)'\0';   // next string please
	}
    else
    {
	    strFilter += "Library Files (*.h)";
	    strFilter += (TCHAR)'\0';   // next string please
	    strFilter += _T("*.h");
	    strFilter += (TCHAR)'\0';   // next string please
    }
	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
#ifndef _MAC
	strFilter += _T("*.*");
#else
	strFilter += _T("****");
#endif
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
#ifndef _MAC
	dlgFile.m_ofn.lpstrTitle = title;
#else
	dlgFile.m_ofn.lpstrPrompt = title;
#endif
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	fileName.ReleaseBuffer();
	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

#include "Additional Source\Hyperlink Control\hyperlink.h"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CHyperLink	m_Author2;
	CHyperLink	m_SizingControlBarLink;
	CHyperLink	m_HyperlinkControlLink;
	CHyperLink	m_DirDialogLink;
	CHyperLink	m_CrystalEditLink;
	CHyperLink	m_HomeLink;
	CHyperLink	m_AuthorLink;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_AUTHOR2, m_Author2);
	DDX_Control(pDX, IDC_SIZING_BAR_LINK, m_SizingControlBarLink);
	DDX_Control(pDX, IDC_HYPERLINK_LINK, m_HyperlinkControlLink);
	DDX_Control(pDX, IDC_DIRDIALOG_LINK, m_DirDialogLink);
	DDX_Control(pDX, IDC_CRYSTAL_EDIT_LINK, m_CrystalEditLink);
	DDX_Control(pDX, IDC_HOME_LINK, m_HomeLink);
	DDX_Control(pDX, IDC_AUTHOR, m_AuthorLink);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CScriptorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_AuthorLink.SetURL("mailto:KhalvKalash@hotmail.com");
    m_AuthorLink.ModifyLinkStyle( 0, CHyperLink::StyleUseHover );
	
	m_Author2.SetURL("mailto:KhalvKalash@hotmail.com");
    m_Author2.ModifyLinkStyle( 0, CHyperLink::StyleUseHover );

    m_HomeLink.SetURL("http://webpages.acs.ttu.edu/lojones/TA/Apps.html");
    m_HomeLink.ModifyLinkStyle( 0, CHyperLink::StyleUseHover );

    m_SizingControlBarLink.SetURL("http://www.datamekanix.com/");
    m_SizingControlBarLink.ModifyLinkStyle( 0, CHyperLink::StyleUseHover );

    m_HyperlinkControlLink.SetURL("http://www.codeguru.com/controls/hyperlinkex.shtml");
    m_HyperlinkControlLink.ModifyLinkStyle( 0, CHyperLink::StyleUseHover );

    m_DirDialogLink.SetURL("http://www.codeguru.com/dialog/DirDialogPhillips.shtml");
    m_DirDialogLink.ModifyLinkStyle( 0, CHyperLink::StyleUseHover );

    m_CrystalEditLink.SetURL("http://www.codetools.com/editctrl/crysedit.asp");
    m_CrystalEditLink.ModifyLinkStyle( 0, CHyperLink::StyleUseHover );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CScriptorApp message handlers

void CScriptorApp::OnEditSetfont() 
{
    // Construct the font selection dialog
	CFontDialog dlg( &m_CurrentFont,
        CF_SCREENFONTS |
        CF_INITTOLOGFONTSTRUCT |
        CF_FIXEDPITCHONLY );

    // Run the dialog and check if the user pressed [OK]
    if( dlg.DoModal()==IDOK )
	{
        // Switch all the views of all open documents to the new font.
        POSITION            Pos;
        POSITION            DocPos;
        POSITION            ViewPos;
	    CDocTemplate*       pDTemp;
	    CDocument*          pDoc;
	    CScriptorView*      pView;
		
		Pos = GetFirstDocTemplatePosition();
		while(Pos)
		{
			pDTemp = GetNextDocTemplate(Pos);
			DocPos = pDTemp->GetFirstDocPosition();
			while(DocPos)
			{
				pDoc = pDTemp->GetNextDoc(DocPos);
				ViewPos = pDoc->GetFirstViewPosition();
				while(ViewPos)
				{
					pView = (CScriptorView*)pDoc->GetNextView(ViewPos);
                    pView->SetFont( m_CurrentFont );

				}// end while( ViewPos )

			}// end while( DocPos )

		}// end while( Pos )

    } // end if( IDOK )
	
}

int CScriptorApp::ExitInstance() 
{    
	BOSCOM_Destroy( BosCom );

	BOSDEC_Destroy( BosDec );

    // Save the font settings
	WriteProfileFont("Font", &m_CurrentFont, &m_OriginalFont);

    // Save the app settings
	WriteProfileSettings("Settings");

    DestroyBosKeywords();

    // Set the current directory to what it was when we came in
    SetCurrentDirectory( m_StartDir );
	
	return CWinApp::ExitInstance();
}

void CScriptorApp::OnFileDescript() 
{
	CDescript       DeScriptDlg;

	if( DeScriptDlg.DoModal()==IDOK )
	{
		theApp.OpenDocumentFile(LocalSettings.LastBosArr[0]);
	}
}

void CScriptorApp::OnFileUnitwizards() 
{
	CUnitWizards    UnitWizDlg;

	if( (UnitWizDlg.DoModal()==IDOK)&&(UnitWizDlg.m_WizardCompleted) )
	{
        OpenDocumentFile( UnitWizDlg.m_FileToOpen );
	}
}

void CScriptorApp::OnOpenCob( WPARAM wParam, LPARAM lParam )
{
	CDescript       DeScriptDlg;

    strcpy( DeScriptDlg.m_FileToOpen, m_AllPurposeString );
	if( DeScriptDlg.DoModal()==IDOK )
	{
		theApp.OpenDocumentFile(LocalSettings.LastBosArr[0]);
	}
}
