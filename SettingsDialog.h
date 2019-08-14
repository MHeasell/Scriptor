#if !defined(AFX_SETTINGSDIALOG_H__15E3424F_4975_4767_A28B_A05BF0DF366A__INCLUDED_)
#define AFX_SETTINGSDIALOG_H__15E3424F_4975_4767_A28B_A05BF0DF366A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsDialog dialog

class CSettingsDialog : public CDialog
{
// Attributes
protected:
    CString     m_LastCobFile;
    CString     m_LastCobDir;

// Construction
public:
	CSettingsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDialog)
	enum { IDD = IDD_SETTINGS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnOutDirCheck();
	afx_msg void OnOutFileCheck();
	afx_msg void OnBrowseInc();
	afx_msg void OnBrowseFile();
	afx_msg void OnBrowseDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDIALOG_H__15E3424F_4975_4767_A28B_A05BF0DF366A__INCLUDED_)
