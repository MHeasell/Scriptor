// ScriptorDoc.h : interface of the CScriptorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCRIPTORDOC_H__90A9204B_3050_48F5_954D_00D8C006DE54__INCLUDED_)
#define AFX_SCRIPTORDOC_H__90A9204B_3050_48F5_954D_00D8C006DE54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Additional Source\Crystal Edit\CCrystalTextBuffer.h"

class CScriptorDoc : public CDocument
{
protected: // create from serialization only
	CScriptorDoc();
	DECLARE_DYNCREATE(CScriptorDoc)

// Attributes
public:
	class CScriptorTextBuffer : public CCrystalTextBuffer
	{
	private:
		CScriptorDoc *m_pOwnerDoc;
	public:
		CScriptorTextBuffer(CScriptorDoc *pDoc) { m_pOwnerDoc = pDoc; };
		virtual void SetModified(BOOL bModified = TRUE)
			{ m_pOwnerDoc->SetModifiedFlag(bModified); };
	};

	CScriptorTextBuffer m_xTextBuffer;
	LOGFONT m_lf;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScriptorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CScriptorDoc)
	afx_msg void OnScriptCompile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTORDOC_H__90A9204B_3050_48F5_954D_00D8C006DE54__INCLUDED_)
