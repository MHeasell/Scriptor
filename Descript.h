#if !defined(AFX_DESCRIPT_H__4D2B3554_D1B3_460D_BFAF_25718163437D__INCLUDED_)
#define AFX_DESCRIPT_H__4D2B3554_D1B3_460D_BFAF_25718163437D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Descript.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDescript dialog

class CDescript : public CDialog
{
// Construction
public:
	CDescript(CWnd* pParent = NULL);   // standard constructor

    char m_FileToOpen[MAX_PATH];
    bool WasOK;

// Dialog Data
	//{{AFX_DATA(CDescript)
	enum { IDD = IDD_DESCRIPT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDescript)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDescript)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseCob();
	afx_msg void OnBrowseBos();
	afx_msg void OnBosCheck();
	afx_msg void OnSelendokCobPath();
	afx_msg void OnCloseupCobPath();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESCRIPT_H__4D2B3554_D1B3_460D_BFAF_25718163437D__INCLUDED_)
