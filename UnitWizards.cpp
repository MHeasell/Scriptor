// UnitWizards.cpp : implementation file
//

#include "stdafx.h"
#include "Scriptor.h"
#include "UnitWizards.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitWizards dialog


CUnitWizards::CUnitWizards(CWnd* pParent /*=NULL*/)
	: CDialog(CUnitWizards::IDD, pParent)
{
    m_WizardCompleted = FALSE;
    strcpy( m_FileToOpen, "\0\0\0" );

	//{{AFX_DATA_INIT(CUnitWizards)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CUnitWizards::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnitWizards)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnitWizards, CDialog)
	//{{AFX_MSG_MAP(CUnitWizards)
	ON_LBN_SELCHANGE(IDC_WIZ_LIST, OnSelchangeWizList)
	ON_EN_UPDATE(IDC_NEW_FILENAME, OnUpdateNewFilename)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitWizards message handlers

#define FUNC_NAME( INT_ORDINAL_VALUE )      ((LPCTSTR)( INT_ORDINAL_VALUE ))
#define GET_DLL_FUNC( DLL_HANDLE, INT_ORDINAL_VALUE, FUNC_TYPE )     \
    ( (FUNC_TYPE)GetProcAddress( (DLL_HANDLE), ((LPCTSTR)(INT_ORDINAL_VALUE)) ) )

#define GET_TITLE_FUNC			1
#define GET_DESC_FUNC			2
#define DO_WIZ_FUNC				3
typedef LPCTSTR (CALLBACK* LPFNDLLFUNC1)(void);
typedef LPCTSTR (CALLBACK* LPFNDLLFUNC2)(void);
typedef LPTSTR (CALLBACK* LPFNDLLFUNC3)(LPCTSTR);


#define WIZARD_VALIDATION           ( 0x91100119 )
typedef struct WIZARD_INFO_t
{
    char        Title[64];
    char        Description[512];
    DWORD       Validation;

}* LPWIZARD_INFO_t;

#define GET_WIZARD_INFO         1
typedef void (CALLBACK* LPGETWIZARDINFO)(LPWIZARD_INFO_t);
//void WINAPI GetWizardInfo( LPWIZARD_INFO_t pWizInfo );


typedef struct DO_WIZARD_INFO_t
{
// Input items ( set these before calling DoWizard() )
    char        DesiredPath[MAX_PATH];
    char        DesiredName[64];

// Output items
    BOOL        bWizardCompleted;
    char        FileToOpen[MAX_PATH];

}* LPDO_WIZARD_INFO_t;

#define DO_WIZARD               2
typedef void (CALLBACK* LPDOWIZARD)(LPDO_WIZARD_INFO_t);
//void WINAPI DoWizard();

typedef struct WIZARD_LIST_ITEM
{
    HINSTANCE           hWizardDLL;
    WIZARD_INFO_t       WizardInfo;
    LPDOWIZARD          DoWizard;

}* LPWIZARD_LIST_ITEM;

BOOL CUnitWizards::OnInitDialog() 
{
    // Call the default
    CDialog::OnInitDialog();

    // Get a pointer to the [OK] button
    CButton* pOKbutton = (CButton*)GetDlgItem(IDOK);

    // Disable it (there is no name entered yet)
    pOKbutton->EnableWindow(0);

    CListBox*           pWizardList;
    HINSTANCE           hWizardDLL;
    CFileFind           FileFinder;
    BOOL                bWorking;
    LPGETWIZARDINFO     GetWizardInfo;
    WIZARD_INFO_t       WizardInfo;
    LPWIZARD_LIST_ITEM  pWizardListItem;

    // Get a pointer to the wizard list box
    pWizardList = (CListBox*)GetDlgItem(IDC_WIZ_LIST);

    // Look for any wizards
    bWorking = FileFinder.FindFile("Unit Wizards\\*.unw");

    while( bWorking )
    {
        bWorking = FileFinder.FindNextFile();

        // Load the dll
        hWizardDLL = AfxLoadLibrary( FileFinder.GetFilePath() );

        // Get the info function
        GetWizardInfo = GET_DLL_FUNC( hWizardDLL, GET_WIZARD_INFO, LPGETWIZARDINFO );

        // Get the wizards info
        GetWizardInfo( &WizardInfo );

        // Validate it
        if( WizardInfo.Validation!=WIZARD_VALIDATION )
        {
            AfxFreeLibrary( hWizardDLL );
            continue;
        }

        // Create a new list item
        pWizardListItem = new WIZARD_LIST_ITEM;
        pWizardListItem->hWizardDLL = hWizardDLL;
        pWizardListItem->WizardInfo = WizardInfo;
        pWizardListItem->DoWizard = GET_DLL_FUNC( hWizardDLL, DO_WIZARD, LPDOWIZARD );

        // Add the item to the list box and set its data
        pWizardList->SetItemDataPtr( pWizardList->AddString( WizardInfo.Title ), pWizardListItem );

    } // end while( bWorking )

    // If there is an item in the list...
    if( pWizardList->GetCount() )
	{
        // Set the current selection to the first item
        pWizardList->SetCurSel(0);

        // Get a pointer to the description box
        CEdit* pDescription = (CEdit*)GetDlgItem(IDC_DESC);

        // Get the first item's data
        pWizardListItem = (LPWIZARD_LIST_ITEM)pWizardList->GetItemDataPtr(0);

        // Set the description
		pDescription->SetWindowText( pWizardListItem->WizardInfo.Description );

    } // end if( pWizardList->GetCount() )
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUnitWizards::OnSelchangeWizList() 
{
    CListBox*           pWizardList;
    CEdit*              pDescription;
    int                 CurrentSelection;
    LPWIZARD_LIST_ITEM  pWizardListItem;

    // Get a pointer to the wizard list box
    pWizardList = (CListBox*)GetDlgItem(IDC_WIZ_LIST);

    // Get the current selection
    CurrentSelection = pWizardList->GetCurSel();

    // Get its data
    pWizardListItem = (LPWIZARD_LIST_ITEM)pWizardList->GetItemDataPtr( CurrentSelection );

    // Get a pointer to the description box
    pDescription = (CEdit*)GetDlgItem(IDC_DESC);

    // Set the description
	pDescription->SetWindowText( pWizardListItem->WizardInfo.Description );
}

void CUnitWizards::OnOK() 
{
    CListBox*           pWizardList;
    CEdit*              pNameBox;
    CString             ChoosenName;
    int                 CurrentSelection;
    LPWIZARD_LIST_ITEM  pWizardListItem;
    DO_WIZARD_INFO_t    DoWizardInfo;

    // Get a pointer to the wizard list box
    pWizardList = (CListBox*)GetDlgItem(IDC_WIZ_LIST);

    // Get the current selection
    CurrentSelection = pWizardList->GetCurSel();

    // If we couldn't get the current selection, just exit
    if( CurrentSelection==LB_ERR )
    {
        CDialog::OnOK();
        return;
    }

    // Get a pointer to the name box
    pNameBox = (CEdit*)GetDlgItem(IDC_NEW_FILENAME);

    // Get the name from the box
    pNameBox->GetWindowText( ChoosenName );

    // Get the current selection's data
    pWizardListItem = (LPWIZARD_LIST_ITEM)pWizardList->GetItemDataPtr( CurrentSelection );

    // Set the info to be passed to DoWizard()
    strcpy( DoWizardInfo.DesiredPath, theApp.m_StartDir );
    ChoosenName.TrimLeft();
    ChoosenName.TrimRight();
    strcpy( DoWizardInfo.DesiredName, ChoosenName );

    // Call Mr Wizard
	pWizardListItem->DoWizard( &DoWizardInfo );
    
    // Copy the returned info
    m_WizardCompleted = DoWizardInfo.bWizardCompleted;
    strcpy( m_FileToOpen, DoWizardInfo.FileToOpen );
	
	CDialog::OnOK();
}

void CUnitWizards::OnUpdateNewFilename() 
{
	CButton* OK = (CButton*)GetDlgItem(IDOK);
	CListBox* List = (CListBox*)GetDlgItem(IDC_WIZ_LIST);
	CEdit* FileName = (CEdit*)GetDlgItem(IDC_NEW_FILENAME);
	CString File_Name;

	FileName->GetWindowText(File_Name);
	File_Name.TrimLeft();
	File_Name.TrimRight();

	/*for(int a=1,int x=File_Name.GetLength()-1;x>=0;x--)
		if(!iswspace(File_Name[x])*/

	if(File_Name=="")
	{
		OK->EnableWindow(0);
		return;
	}
	
	if(!List->GetCount())
	{
		OK->EnableWindow(0);
		return;
	}

	if(List->GetCurSel()==LB_ERR)
	{
		OK->EnableWindow(0);
		return;
	}

	OK->EnableWindow(1);
	
}

void CUnitWizards::OnDestroy() 
{
	CDialog::OnDestroy();
    
    CListBox*           pWizardList;
    int                 Selection;
    LPWIZARD_LIST_ITEM  pWizardListItem;
    HINSTANCE           hWizardDLL;
    
    // Get a pointer to the wizard list box
    pWizardList = (CListBox*)GetDlgItem(IDC_WIZ_LIST);

    // Get the last selection
    Selection = pWizardList->GetCount() - 1;
	
    // Iterate through each item
    for( ; Selection>=0; Selection--)
    {
         // Get this item's data
        pWizardListItem = (LPWIZARD_LIST_ITEM)pWizardList->GetItemDataPtr( Selection );

        // Save the DLL's handle
        hWizardDLL = pWizardListItem->hWizardDLL;

        // Free the items memory
        delete pWizardListItem;

        // Free the DLL
        AfxFreeLibrary( hWizardDLL );
    }

}
