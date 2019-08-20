#if !defined(AFX_UNITWIZARDS_H__2D90D127_DDAA_40C6_B44A_9C7D178A3817__INCLUDED_)
#define AFX_UNITWIZARDS_H__2D90D127_DDAA_40C6_B44A_9C7D178A3817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitWizards.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUnitWizards dialog

class CUnitWizards : public CDialog
{
	// Construction
public:
	CUnitWizards(CWnd* pParent = NULL); // standard constructor

	// Attributes
public:
	BOOL m_WizardCompleted;
	char m_FileToOpen[MAX_PATH];

	// Dialog Data
	//{{AFX_DATA(CUnitWizards)
	enum
	{
		IDD = IDD_UNIT_WIZ
	};
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnitWizards)
protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
													 //}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUnitWizards)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWizList();
	virtual void OnOK();
	afx_msg void OnUpdateNewFilename();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNITWIZARDS_H__2D90D127_DDAA_40C6_B44A_9C7D178A3817__INCLUDED_)
