// DebugOutputBar.cpp : implementation file
//

#include "stdafx.h"
#include "Scriptor.h"
#include "DebugOutputBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugOutputBar

CDebugOutputBar::CDebugOutputBar()
{
    m_szHorz = CSize(200, 100);
    m_szVert = CSize(100, 200);
    m_szFloat = CSize(400, 100);
}

CDebugOutputBar::~CDebugOutputBar()
{
}


BEGIN_MESSAGE_MAP(CDebugOutputBar, baseCMyBar)
	//{{AFX_MSG_MAP(CDebugOutputBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDebugOutputBar message handlers

int CDebugOutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (baseCMyBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetSCBStyle( GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD );

	if( !m_wndChild.Create( WS_CHILD | WS_VISIBLE |
                            WS_HSCROLL | WS_VSCROLL |
			                ES_MULTILINE | ES_WANTRETURN |
                            ES_AUTOVSCROLL | ES_READONLY,
		                    CRect(0,0,0,0), this,
                            IDC_DEBUG_OUTPUT_WINDOW) )
		return -1;

	m_wndChild.ModifyStyleEx( 0, WS_EX_CLIENTEDGE );

	// older versions of Windows* (NT 3.51 for instance)
	// fail with DEFAULT_GUI_FONT
	if (!m_font.CreateStockObject(DEFAULT_GUI_FONT))
		if (!m_font.CreatePointFont(80, "MS Sans Serif"))
			return -1;

	m_wndChild.SetFont(&m_font);

    m_wndChild.AddTextLine( "-------------------- Welcome to Scriptor v1 --------------------" );
	
	return 0;
}
