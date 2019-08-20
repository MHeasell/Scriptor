// ScriptorView.cpp : implementation of the CScriptorView class
//

#include "stdafx.h"
#include "Scriptor.h"

#include "ScriptorDoc.h"
#include "ScriptorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptorView

IMPLEMENT_DYNCREATE(CScriptorView, CCrystalEditView)

BEGIN_MESSAGE_MAP(CScriptorView, CCrystalEditView)
//{{AFX_MSG_MAP(CScriptorView)
ON_WM_CONTEXTMENU()
//}}AFX_MSG_MAP
// Standard printing commands
ON_COMMAND(ID_FILE_PRINT, CCrystalEditView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_DIRECT, CCrystalEditView::OnFilePrint)
ON_COMMAND(ID_FILE_PRINT_PREVIEW, CCrystalEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptorView construction/destruction

CScriptorView::CScriptorView()
{
	// TODO: add construction code here
}

CScriptorView::~CScriptorView()
{
}

BOOL CScriptorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CCrystalEditView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CScriptorView diagnostics

#ifdef _DEBUG
void CScriptorView::AssertValid() const
{
	CCrystalEditView::AssertValid();
}

void CScriptorView::Dump(CDumpContext& dc) const
{
	CCrystalEditView::Dump(dc);
}

CScriptorDoc* CScriptorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CScriptorDoc)));
	return (CScriptorDoc*)m_pDocument;
}
#endif //_DEBUG


void CScriptorView::GotoErrorLine(int iLineIndex)
{
	CPoint ptError(0, iLineIndex);

	SetCursorPos(ptError);
	EnsureVisible(ptError);
	/*
    if (m_pTextBuffer != NULL)
	{
		DWORD dwFlags = GetLineFlags( iLineIndex );
		DWORD dwMask  = LF_COMPILATION_ERROR;
		m_pTextBuffer->SetLineFlag(iLineIndex, dwMask, (dwFlags & dwMask) == 0, FALSE);
	}*/
}


/////////////////////////////////////////////////////////////////////////////
// CScriptorView message handlers

CCrystalTextBuffer* CScriptorView::LocateTextBuffer()
{
	return &GetDocument()->m_xTextBuffer;
}

void CScriptorView::OnInitialUpdate()
{
	CCrystalEditView::OnInitialUpdate();

	//SetFont(GetDocument()->m_lf);
	SetFont(theApp.m_CurrentFont);
}

void CScriptorView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Load the custom context menu
	CMenu pScriptMenu;
	pScriptMenu.LoadMenu(IDR_SCRIPT_CONTEXT_MENU);

	// Get the edit sub menu
	CMenu* pEditMenu = pScriptMenu.GetSubMenu(0);

	// Use the it as the popup
	pEditMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
}
