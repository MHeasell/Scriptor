// SettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Scriptor.h"
#include "SettingsDialog.h"
#include "Additional Source\DirDialog\DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog dialog


CSettingsDialog::CSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDialog, CDialog)
	//{{AFX_MSG_MAP(CSettingsDialog)
	ON_BN_CLICKED(IDC_OUT_DIR_CHECK, OnOutDirCheck)
	ON_BN_CLICKED(IDC_OUT_FILE_CHECK, OnOutFileCheck)
	ON_BN_CLICKED(IDC_BROWSE_INC, OnBrowseInc)
	ON_BN_CLICKED(IDC_BROWSE_FILE, OnBrowseFile)
	ON_BN_CLICKED(IDC_BROWSE_DIR, OnBrowseDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog message handlers

BOOL CSettingsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    CButton*        pButton;
    CString         String;
    CEdit*          pEdit;
	
	pButton = (CButton*)GetDlgItem(IDC_TA);
	pButton->SetCheck( CompilerSettings.TA );

	pButton = (CButton*)GetDlgItem(IDC_TAK);
	pButton->SetCheck( !CompilerSettings.TA );

	String.Format( "%f", CompilerSettings.AngularConst );
	pEdit = (CEdit*)GetDlgItem(IDC_ANGCON);
	pEdit->SetWindowText( String );

	String.Format( "%f", CompilerSettings.LinearConst );
	pEdit = (CEdit*)GetDlgItem(IDC_LINCON);
	pEdit->SetWindowText( String );

    pEdit = (CEdit*)GetDlgItem(IDC_INCLUDE);
	pEdit->SetWindowText( CompilerSettings.IncludeDirectory );

	pButton = (CButton*)GetDlgItem(IDC_OUT_FILE_CHECK);
	pButton->SetCheck( LocalSettings.CobFile_SameAsBos );

	pEdit = (CEdit*)GetDlgItem(IDC_COBFILE);
    m_LastCobFile = LocalSettings.CobFile;
	pButton = (CButton*)GetDlgItem(IDC_BROWSE_FILE);
	if( LocalSettings.CobFile_SameAsBos )
	{
		pEdit->EnableWindow(FALSE);
		pButton->EnableWindow(FALSE);
        pEdit->SetWindowText( "Use the same file name as the .bos script being compiled" );
	}
	else
	{
		pEdit->EnableWindow(TRUE);
		pButton->EnableWindow(TRUE);
        pEdit->SetWindowText( m_LastCobFile );
	}

	pButton = (CButton*)GetDlgItem(IDC_OUT_DIR_CHECK);
	pButton->SetCheck( LocalSettings.CobDir_SameAsBos );

	pEdit = (CEdit*)GetDlgItem(IDC_COBDIR);
    m_LastCobDir = LocalSettings.CobDir;
	pButton = (CButton*)GetDlgItem(IDC_BROWSE_DIR);
	if(LocalSettings.CobDir_SameAsBos)
	{
		pEdit->EnableWindow(FALSE);
		pButton->EnableWindow(FALSE);
        pEdit->SetWindowText( "Use the same directory as the .bos script being compiled" );
	}
	else
	{
		pEdit->EnableWindow(TRUE);
		pButton->EnableWindow(TRUE);
        pEdit->SetWindowText( m_LastCobDir );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDialog::OnOK() 
{
    CButton*        pButton;
    CString         String;
    CEdit*          pEdit;

	pButton = (CButton*)GetDlgItem(IDC_TA);
	CompilerSettings.TA = pButton->GetCheck();

	pEdit = (CEdit*)GetDlgItem(IDC_ANGCON);
	pEdit->GetWindowText( String );
	CompilerSettings.AngularConst = (float)atof( String );

	pEdit = (CEdit*)GetDlgItem(IDC_LINCON);
	pEdit->GetWindowText( String );
	CompilerSettings.LinearConst = (float)atof( String );

	DecompilerSettings.AngConst = CompilerSettings.AngularConst;
	DecompilerSettings.LinConst = CompilerSettings.LinearConst;

    pEdit = (CEdit*)GetDlgItem(IDC_INCLUDE);
	pEdit->GetWindowText( String );
	if( CompilerSettings.IncludeDirectory ) delete [] CompilerSettings.IncludeDirectory;
	CompilerSettings.IncludeDirectory = new char[ String.GetLength()+1 ];
	strcpy( CompilerSettings.IncludeDirectory, String );

	pButton = (CButton*)GetDlgItem(IDC_OUT_FILE_CHECK);
	LocalSettings.CobFile_SameAsBos = pButton->GetCheck();

	pEdit = (CEdit*)GetDlgItem(IDC_COBFILE);
	if( !LocalSettings.CobFile_SameAsBos )
	{
		pEdit->GetWindowText( LocalSettings.CobFile );
	}

	pButton = (CButton*)GetDlgItem(IDC_OUT_DIR_CHECK);
	LocalSettings.CobDir_SameAsBos = pButton->GetCheck();

	pEdit = (CEdit*)GetDlgItem(IDC_COBDIR);
	if( !LocalSettings.CobDir_SameAsBos )
	{
		pEdit->GetWindowText( LocalSettings.CobDir );
		if(LocalSettings.CobDir[LocalSettings.CobDir.GetLength() - 1] != '\\')
			LocalSettings.CobDir+='\\';
	}
	
	CDialog::OnOK();
}

void CSettingsDialog::OnOutDirCheck() 
{
	CButton* DirCheck = (CButton*)GetDlgItem(IDC_OUT_DIR_CHECK);
	CEdit* CobDir = (CEdit*)GetDlgItem(IDC_COBDIR);
	CButton* BrowseCobDir = (CButton*)GetDlgItem(IDC_BROWSE_DIR);
	if(DirCheck->GetCheck())
	{
        CobDir->GetWindowText( m_LastCobDir );
		CobDir->EnableWindow(FALSE);
		BrowseCobDir->EnableWindow(FALSE);
        CobDir->SetWindowText( "Use the same directory as the .bos script being compiled" );
	}
	else
	{
		CobDir->EnableWindow(TRUE);
		BrowseCobDir->EnableWindow(TRUE);
        CobDir->SetWindowText( m_LastCobDir );
	}
}

void CSettingsDialog::OnOutFileCheck() 
{
	CButton* FileCheck = (CButton*)GetDlgItem(IDC_OUT_FILE_CHECK);
	CEdit* CobFile = (CEdit*)GetDlgItem(IDC_COBFILE);
	CButton* BrowseCobFile = (CButton*)GetDlgItem(IDC_BROWSE_FILE);

	if( FileCheck->GetCheck() )
	{
        CobFile->GetWindowText( m_LastCobFile );
		CobFile->EnableWindow(FALSE);
		BrowseCobFile->EnableWindow(FALSE);
        CobFile->SetWindowText( "Use the same file name as the .bos script being compiled" );
	}
	else
	{
		CobFile->EnableWindow(TRUE);
		BrowseCobFile->EnableWindow(TRUE);
        CobFile->SetWindowText( m_LastCobFile );
	}
}

void CSettingsDialog::OnBrowseInc() 
{
	CEdit*              pIncludeEdit;
    CString             strDirectory;

    // Get a pointer to the edit control and get its contents
    pIncludeEdit = (CEdit*)GetDlgItem(IDC_INCLUDE);
	pIncludeEdit->GetWindowText( strDirectory );

    // Create the directory delection dialog
    CDirDialog DDlg(
        strDirectory, 
        "Script Include Files (*.h; *.bos)|*.h;*.bos|All Files (*.*)|*.*||",
        this );

    // Let the user select
    if( DDlg.DoModal()==IDOK )
    {
        // Get the selection
        strDirectory = DDlg.GetPath();

        // Set it as the new include directory
		pIncludeEdit->SetWindowText( strDirectory );

    }
}

void CSettingsDialog::OnBrowseFile() 
{
	CEdit* CobFile = (CEdit*)GetDlgItem(IDC_COBFILE);
	LPCSTR File;
	CString CobPath;
	CobFile->GetWindowText(CobPath);
	File=CobPath;
	LPCSTR CobFilter="TA COB Files (*.cob)|*.cob|All Files (*.*)|*.*||";
	CFileDialog FDlg(FALSE,"cob",File,OFN_HIDEREADONLY /*| OFN_OVERWRITEPROMPT*/,CobFilter);
	if(FDlg.DoModal()==IDOK)
	{
		CobPath=FDlg.GetPathName();
		CobPath=CobPath.Right(CobPath.GetLength()-(CobPath.ReverseFind('\\')+1));
		CobFile->SetWindowText(CobPath);
	}
}

void CSettingsDialog::OnBrowseDir() 
{
    CEdit*              pCobEdit;
    CString             strDirectory;

    // Get a pointer to the edit control and get its contents
    pCobEdit = (CEdit*)GetDlgItem(IDC_COBDIR);
	pCobEdit->GetWindowText( strDirectory );

    // Create the directory delection dialog
    CDirDialog DDlg(
        strDirectory, 
        "Compiled Script Files (*.cob)|*.cob|All Files (*.*)|*.*||",
        this );

    // Let the user select
    if( DDlg.DoModal()==IDOK )
    {
        // Get the selection
        strDirectory = DDlg.GetPath();

        // Set it as the new cob directory
		pCobEdit->SetWindowText( strDirectory );

    }
}
