// StrParse.h: interface for the CStrParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STRPARSE_H__B04E66E3_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_)
#define AFX_STRPARSE_H__B04E66E3_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef _DEBUG
 #include <fstream.h>
#endif

class CStrParse  
{
protected:

#ifdef _DEBUG
	ofstream fout;
#endif

public:
	long Line_Count;
	CStrParse();
	virtual ~CStrParse();

	virtual WORD ParseStr(CString& Str,DWORD* Arg1,DWORD* Arg2);
};

#endif // !defined(AFX_STRPARSE_H__B04E66E3_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_)
