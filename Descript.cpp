// Descript.cpp : implementation file
//

#include "stdafx.h"
#include "Scriptor.h"
#include "Descript.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDescript dialog


CDescript::CDescript(CWnd* pParent /*=NULL*/)
	: CDialog(CDescript::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDescript)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
    m_FileToOpen[0] = '\0';
}


void CDescript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDescript)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDescript, CDialog)
	//{{AFX_MSG_MAP(CDescript)
	ON_BN_CLICKED(IDC_BROWSE_COB, OnBrowseCob)
	ON_BN_CLICKED(IDC_BROWSE_BOS, OnBrowseBos)
	ON_BN_CLICKED(IDC_BOS_CHECK, OnBosCheck)
	ON_CBN_SELENDOK(IDC_COB_PATH, OnSelendokCobPath)
	ON_CBN_CLOSEUP(IDC_COB_PATH, OnCloseupCobPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDescript message handlers

BOOL CDescript::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox* Cob_Path = (CComboBox*)GetDlgItem(IDC_COB_PATH);
	//Cob_Path->SetWindowText(LocalSettings.LastCob);
    if( m_FileToOpen[0]=='\0' )
    {
	    if( LocalSettings.LastCobArr.GetSize() )
		    Cob_Path->SetWindowText(LocalSettings.LastCobArr[0]);
	    for(int x=0;x<LocalSettings.LastCobArr.GetSize();x++)
		    Cob_Path->AddString(LocalSettings.LastCobArr[x]);
    }
    else
    {
        Cob_Path->SetWindowText( m_FileToOpen );
        Cob_Path->AddString( m_FileToOpen );
	    for(int x=0;x<LocalSettings.LastCobArr.GetSize();x++)
		    Cob_Path->AddString(LocalSettings.LastCobArr[x]);
    }

	CComboBox* Bos_Path = (CComboBox*)GetDlgItem(IDC_BOS_PATH);
	//Bos_Path->SetWindowText(LocalSettings.LastBos);
    if( m_FileToOpen[0]=='\0' )
    {
	    if( LocalSettings.LastBosArr.GetSize() )
		    Bos_Path->SetWindowText(LocalSettings.LastBosArr[0]);
	    for(int x=0;x<LocalSettings.LastBosArr.GetSize();x++)
		    Bos_Path->AddString(LocalSettings.LastBosArr[x]);
    }
    else
    {
        LPTSTR Ext = strrchr( m_FileToOpen, '.' );
        char NewCobPath[MAX_PATH];
        if( Ext )
        {
            strncpy( NewCobPath, m_FileToOpen, Ext-m_FileToOpen );
            NewCobPath[Ext-m_FileToOpen] = '\0';
            strcat( NewCobPath, ".bos" );
        }
        else sprintf( NewCobPath, "%s.bos", m_FileToOpen );
        Bos_Path->SetWindowText( NewCobPath );
        Bos_Path->AddString( NewCobPath );
	    for(int x=0;x<LocalSettings.LastBosArr.GetSize();x++)
		    Bos_Path->AddString(LocalSettings.LastBosArr[x]);
    }

	CString Str1,Str2;
	CButton* Check = (CButton*)GetDlgItem(IDC_BOS_CHECK);
	Check->SetCheck(LocalSettings.De_Bos_SA_Cob);
	if(Check->GetCheck())
	{
		Cob_Path->GetWindowText(Str1);
		Bos_Path->GetWindowText(Str2);
		Str1 = Str1.Mid(Str1.ReverseFind('\\')+1 , Str1.ReverseFind('.')-(Str1.ReverseFind('\\')+1));
		Str2 = Str2.Left(Str2.ReverseFind('\\')+1);
		Bos_Path->SetWindowText(Str2 + Str1 + ".bos");
	}

	CButton* Guess = (CButton*)GetDlgItem(IDC_GUESS);
	Guess->SetCheck(DecompilerSettings.DoGuess);
	CButton* Header = (CButton*)GetDlgItem(IDC_HEADER);
	Header->SetCheck(DecompilerSettings.ShowHeader);
	CButton* Offset = (CButton*)GetDlgItem(IDC_OFFSET);
	Offset->SetCheck(DecompilerSettings.ShowOffsets);
	CButton* Push = (CButton*)GetDlgItem(IDC_PUSH);
	Push->SetCheck(DecompilerSettings.ShowPush);
	
	CButton* Std = (CButton*)GetDlgItem(IDC_STDLIBS);
	Std->SetCheck(DecompilerSettings.ShowStdLib);
	CButton* Misc = (CButton*)GetDlgItem(IDC_MISCLIBS);
	Misc->SetCheck(DecompilerSettings.ShowMiscLib);
	CButton* ID = (CButton*)GetDlgItem(IDC_INCDEC);
	ID->SetCheck(DecompilerSettings.IncDec);
	CButton* TKS = (CButton*)GetDlgItem(IDC_TRYKEEPSMALL);
	TKS->SetCheck(DecompilerSettings.TryKeepSmall);
	CButton* Ret = (CButton*)GetDlgItem(IDC_RETURN);
	Ret->SetCheck(DecompilerSettings.ShowReturn);

	CString Str;
	Str.Format("%d",DecompilerSettings.Precision);
	CEdit* Prec = (CEdit*)GetDlgItem(IDC_PRECISION);
	Prec->SetWindowText(Str);
	CSpinButtonCtrl* Spin = (CSpinButtonCtrl*)GetDlgItem(IDC_SPIN);
	Spin->SetRange(1,6);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDescript::OnBrowseCob() 
{
	CString Str1,Str2;
	CButton* Check = (CButton*)GetDlgItem(IDC_BOS_CHECK);
	CComboBox* Cob_Path = (CComboBox*)GetDlgItem(IDC_COB_PATH);
	CComboBox* Bos_Path = (CComboBox*)GetDlgItem(IDC_BOS_PATH);
	LPCSTR File;
	CString CobPath;
	Cob_Path->GetWindowText(CobPath);
	File=CobPath;
	LPCSTR CobFilter="TA COB Files (*.cob)|*.cob|All Files (*.*)|*.*||";
	CFileDialog FDlg(TRUE,"cob",File,OFN_HIDEREADONLY,CobFilter);
	if(FDlg.DoModal()==IDOK)
	{
		CobPath=FDlg.GetPathName();
		Cob_Path->SetWindowText(CobPath);
		if(Check->GetCheck())
		{
			Bos_Path->GetWindowText(Str2);
			Str1 = CobPath.Mid(CobPath.ReverseFind('\\')+1 , CobPath.ReverseFind('.')-(CobPath.ReverseFind('\\')+1));
			Str2 = Str2.Left(Str2.ReverseFind('\\')+1);
			Bos_Path->SetWindowText(Str2 + Str1 + ".bos");
		}
	}
}

void CDescript::OnBrowseBos() 
{
	CComboBox* Bos_Path = (CComboBox*)GetDlgItem(IDC_BOS_PATH);
	LPCSTR File;
	CString BosPath;
	Bos_Path->GetWindowText(BosPath);
	File=BosPath;
	LPCSTR BosFilter="TA BOS Files (*.bos)|*.bos|All Files (*.*)|*.*||";
	CFileDialog FDlg(FALSE,"bos",File,OFN_HIDEREADONLY,BosFilter);
	if(FDlg.DoModal()==IDOK)
	{
		BosPath=FDlg.GetPathName();
		Bos_Path->SetWindowText(BosPath);
	}
}

void CDescript::OnBosCheck() 
{
	CString Str1,Str2;
	CButton* Check = (CButton*)GetDlgItem(IDC_BOS_CHECK);
	CComboBox* Cob_Path = (CComboBox*)GetDlgItem(IDC_COB_PATH);
	CComboBox* Bos_Path = (CComboBox*)GetDlgItem(IDC_BOS_PATH);
	if(Check->GetCheck())
	{
		Cob_Path->GetWindowText(Str1);
		Bos_Path->GetWindowText(Str2);
		Str1 = Str1.Mid(Str1.ReverseFind('\\')+1 , Str1.ReverseFind('.')-(Str1.ReverseFind('\\')+1));
		Str2 = Str2.Left(Str2.ReverseFind('\\')+1);
		Bos_Path->SetWindowText(Str2 + Str1 + ".bos");
	}
}

void CDescript::OnSelendokCobPath() 
{
	CButton* Check = (CButton*)GetDlgItem(IDC_BOS_CHECK);
	if(Check->GetCheck()) WasOK = true;
}

void CDescript::OnCloseupCobPath() 
{
	if(WasOK) WasOK = false;
	else return;
	CString Str1,Str2;
	CButton* Check = (CButton*)GetDlgItem(IDC_BOS_CHECK);
	CComboBox* Cob_Path = (CComboBox*)GetDlgItem(IDC_COB_PATH);
	CComboBox* Bos_Path = (CComboBox*)GetDlgItem(IDC_BOS_PATH);
	if(Check->GetCheck())
	{
		//Cob_Path->GetWindowText(Str1);
		Cob_Path->GetLBText(Cob_Path->GetCurSel(),Str1);
		Bos_Path->GetWindowText(Str2);
		Str1 = Str1.Mid(Str1.ReverseFind('\\')+1 , Str1.ReverseFind('.')-(Str1.ReverseFind('\\')+1));
		Str2 = Str2.Left(Str2.ReverseFind('\\')+1);
		Bos_Path->SetWindowText(Str2 + Str1 + ".bos");
	}
}

void CDescript::OnOK() 
{
	CComboBox* Cob_Path = (CComboBox*)GetDlgItem(IDC_COB_PATH);
	CComboBox* Bos_Path = (CComboBox*)GetDlgItem(IDC_BOS_PATH);
	CButton* Guess = (CButton*)GetDlgItem(IDC_GUESS);
	CButton* Header = (CButton*)GetDlgItem(IDC_HEADER);
	CButton* Offset = (CButton*)GetDlgItem(IDC_OFFSET);
	CButton* Push = (CButton*)GetDlgItem(IDC_PUSH);
	CButton* Std = (CButton*)GetDlgItem(IDC_STDLIBS);
	CButton* Misc = (CButton*)GetDlgItem(IDC_MISCLIBS);
	CButton* Return = (CButton*)GetDlgItem(IDC_RETURN);
	CEdit* Prec = (CEdit*)GetDlgItem(IDC_PRECISION);
	CString CobPath;
	CString BosPath;
	CString Str;

	Cob_Path->GetWindowText(CobPath);
	Bos_Path->GetWindowText(BosPath);
	DecompilerSettings.DoGuess = Guess->GetCheck();
	DecompilerSettings.ShowHeader = Header->GetCheck();
	DecompilerSettings.ShowOffsets = Offset->GetCheck();
	DecompilerSettings.ShowPush = Push->GetCheck();
	DecompilerSettings.ShowReturn = Return->GetCheck();
	
	DecompilerSettings.ShowStdLib = Std->GetCheck();
	DecompilerSettings.ShowMiscLib = Misc->GetCheck();

	Prec->GetWindowText(Str);
	DecompilerSettings.Precision = atol(Str);

	BOSDEC_Decompile(BosDec,CobPath,BosPath,DecompilerSettings);

	for(int x=0;x<LocalSettings.LastCobArr.GetSize();x++)
		if(LocalSettings.LastCobArr[x].CompareNoCase(CobPath)==0)
			LocalSettings.LastCobArr.RemoveAt(x);
	LocalSettings.LastCobArr.FreeExtra();
	LocalSettings.LastCobArr.InsertAt(0,CobPath);
	if(LocalSettings.LastCobArr.GetSize() > 5)
	{
		LocalSettings.LastCobArr.RemoveAt(5);
		LocalSettings.LastCobArr.FreeExtra();
	}
	//LocalSettings.LastCob = CobPath;
	for(x=0;x<LocalSettings.LastBosArr.GetSize();x++)
		if(LocalSettings.LastBosArr[x].CompareNoCase(BosPath)==0)
			LocalSettings.LastBosArr.RemoveAt(x);
	LocalSettings.LastBosArr.FreeExtra();
	LocalSettings.LastBosArr.InsertAt(0,BosPath);
	if(LocalSettings.LastBosArr.GetSize() > 5)
	{
		LocalSettings.LastBosArr.RemoveAt(5);
		LocalSettings.LastBosArr.FreeExtra();
	}
	//LocalSettings.LastBos = BosPath;
	
	CDialog::OnOK();
}
