// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Scriptor.h"

#include "MainFrm.h"
#include "Additional Source\Crystal Edit\editcmd.h"

#include "SettingsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_SCRIPT_COMPILER_SETTINGS, OnScriptCompilerSettings)
	ON_COMMAND(ID_COMPILER_SELECT_TA2, OnCompilerSelectTa)
	ON_UPDATE_COMMAND_UI(ID_COMPILER_SELECT_TA2, OnUpdateCompilerSelectTa)
	ON_COMMAND(ID_COMPILER_SELECT_TAK, OnCompilerSelectTak)
	ON_UPDATE_COMMAND_UI(ID_COMPILER_SELECT_TAK, OnUpdateCompilerSelectTak)
	ON_COMMAND(ID_VIEW_TOLLBARS_COMPILER, OnViewTollbarsCompiler)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOLLBARS_COMPILER, OnUpdateViewTollbarsCompiler)
	ON_COMMAND(ID_VIEW_TOLLBARS_DEBUG, OnViewTollbarsDebug)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOLLBARS_DEBUG, OnUpdateViewTollbarsDebug)
	ON_COMMAND(ID_VIEW_TOLLBARS_EDIT, OnViewTollbarsEdit)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOLLBARS_EDIT, OnUpdateViewTollbarsEdit)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INDICATOR_POSITION, OnUpdateIndicatorPosition)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateIndicatorOvr)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_CAPS, OnUpdateIndicatorCaps)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_NUM, OnUpdateIndicatorNum)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCRL, OnUpdateIndicatorScrl)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,               // status line indicator 
	ID_EDIT_INDICATOR_POSITION,	 
	//ID_EDIT_INDICATOR_COL,
	//ID_EDIT_INDICATOR_CRLF,
	ID_INDICATOR_OVR,	
	//ID_EDIT_INDICATOR_READ,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static UINT prog_indicators[] =
{
	ID_PROGRESS_BAR,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(
            this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | 
            CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
            CRect(0, 0, 0, 0), IDR_MAINFRAME ) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_EditBar.CreateEx(
            this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | 
            CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
            CRect(0, 0, 0, 0), IDR_EDIT_BAR ) ||
		!m_EditBar.LoadToolBar(IDR_EDIT_BAR))
	{
		TRACE0("Failed to create compiler toolbar\n");
		return -1;      // fail to create
	}
	
	if (!m_CompilerTools.CreateEx(
            this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | 
            CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
            CRect(0, 0, 0, 0), IDR_COMPILER_TOOLS ) ||
		!m_CompilerTools.LoadToolBar(IDR_COMPILER_TOOLS))
	{
		TRACE0("Failed to create compiler toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

    if (!m_DebugOutputBar.Create(_T("Debug"), this, IDR_DEBUG_OUTPUT_BAR))
    {
        TRACE0("Failed to create debug bar\n");
        return -1;      // fail to create
	}

    m_DebugOutputBar.SetBarStyle(m_DebugOutputBar.GetBarStyle() |
        CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_EditBar.EnableDocking(CBRS_ALIGN_ANY);
    m_CompilerTools.EnableDocking(CBRS_ALIGN_ANY);
    m_DebugOutputBar.EnableDocking(CBRS_ALIGN_ANY);

	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndToolBar);
    DockControlBarLeftOf(&m_EditBar,&m_wndToolBar);
    DockControlBarLeftOf(&m_CompilerTools,&m_EditBar);
    DockControlBar(&m_DebugOutputBar, AFX_IDW_DOCKBAR_BOTTOM);

    CString sProfile( (LPCSTR)IDS_CONTROL_BAR_STATES_KEY );
    if (VerifyBarState(sProfile))
    {
        LoadBarState(sProfile);
        m_DebugOutputBar.LoadState(sProfile);
    }

	return 0;
}

void CMainFrame::DockControlBarLeftOf(CToolBar* Bar,CToolBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;

	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout();
	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw=LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;

	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line.  By calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar,n,&rect);
}

// This function is Copyright (c) 2000, Cristi Posea.
// See www.datamekanix.com for more control bars tips&tricks.
BOOL CMainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        int nDockedCount = pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMainFrame script controls

void CMainFrame::InitializeScriptControls()
{
    RECT MyRect;
	m_wndStatusBar.SetIndicators(prog_indicators,
		  sizeof(prog_indicators)/sizeof(UINT));
	m_wndStatusBar.GetItemRect(m_wndStatusBar.CommandToIndex(ID_PROGRESS_BAR), &MyRect);
    MyRect.top += 2;
    MyRect.bottom -= 2;
	m_ScriptProgress.Create(0, MyRect, &m_wndStatusBar, 1);
    m_ScriptProgress.ModifyStyle( WS_BORDER, WS_VISIBLE );
	m_bScriptProgressCtrlCreated = TRUE;

    m_DebugOutputBar.GetDebugWindow()->ClearOutputWindow();
}

void CMainFrame::DestroyScriptControls()
{
    m_ScriptProgress.DestroyWindow();
	m_bScriptProgressCtrlCreated = FALSE;
	m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.RedrawWindow();
}


/////////////////////////////////////////////////////////////////////////////
// Update the indicators

void CMainFrame::OnUpdateIndicatorPosition(CCmdUI* pCmdUI) 
{
    pCmdUI->SetText("");
}
/*
void CMainFrame::OnUpdateIndicatorOvr(CCmdUI* pCmdUI) 
{
    if( GetActiveView()==NULL )
        pCmdUI->SetText("");
    else pCmdUI->ContinueRouting();
}

void CMainFrame::OnUpdateIndicatorCaps(CCmdUI* pCmdUI) 
{
    if( GetActiveView()==NULL )
        pCmdUI->SetText("");
    else pCmdUI->ContinueRouting();
}

void CMainFrame::OnUpdateIndicatorNum(CCmdUI* pCmdUI) 
{
    if( GetActiveView()==NULL )
        pCmdUI->SetText("");
    else pCmdUI->ContinueRouting();
}

void CMainFrame::OnUpdateIndicatorScrl(CCmdUI* pCmdUI) 
{
    if( GetActiveView()==NULL )
        pCmdUI->SetText("");
    else pCmdUI->ContinueRouting();
}
*/

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnScriptCompilerSettings() 
{
    CSettingsDialog     SettingsDialog;
    SettingsDialog.DoModal();
}

void CMainFrame::OnCompilerSelectTa() 
{
	CompilerSettings.TA = TRUE;
}

void CMainFrame::OnUpdateCompilerSelectTa(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
    pCmdUI->SetCheck( CompilerSettings.TA );
}

void CMainFrame::OnCompilerSelectTak() 
{
	CompilerSettings.TA = FALSE;
}

void CMainFrame::OnUpdateCompilerSelectTak(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
    pCmdUI->SetCheck( !CompilerSettings.TA );
}

void CMainFrame::OnViewToolbar() 
{
	BOOL bShow = m_wndToolBar.IsVisible();
	ShowControlBar(&m_wndToolBar, !bShow, FALSE);
}

void CMainFrame::OnUpdateViewToolbar(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_wndToolBar.IsVisible());
}

void CMainFrame::OnViewTollbarsCompiler() 
{
	BOOL bShow = m_CompilerTools.IsVisible();
	ShowControlBar(&m_CompilerTools, !bShow, FALSE);
}

void CMainFrame::OnUpdateViewTollbarsCompiler(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_CompilerTools.IsVisible());
}

void CMainFrame::OnViewTollbarsDebug() 
{
	BOOL bShow = m_DebugOutputBar.IsVisible();
	ShowControlBar(&m_DebugOutputBar, !bShow, FALSE);
}

void CMainFrame::OnUpdateViewTollbarsDebug(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_DebugOutputBar.IsVisible());
}

void CMainFrame::OnViewTollbarsEdit() 
{
	BOOL bShow = m_EditBar.IsVisible();
	ShowControlBar(&m_EditBar, !bShow, FALSE);
}

void CMainFrame::OnUpdateViewTollbarsEdit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(m_EditBar.IsVisible());
}

void CMainFrame::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    // Get the view sub menu
    CMenu* pViewMenu = GetMenu()->GetSubMenu(2);

    // Use the toolbars submenu as the popup
	(pViewMenu->GetSubMenu(0))->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,point.x,point.y,this);
}

BOOL CMainFrame::DestroyWindow() 
{
    CString sProfile( (LPCSTR)IDS_CONTROL_BAR_STATES_KEY );
	m_DebugOutputBar.SaveState(sProfile);
    SaveBarState(sProfile);

	return CMDIFrameWnd::DestroyWindow();
}
