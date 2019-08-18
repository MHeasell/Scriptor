// INITxtParse.h: interface for the CINITxtParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITXTPARSE_H__AFE6C034_A1EE_11D3_BA39_0080C8C11E51__INCLUDED_)
#define AFX_INITXTPARSE_H__AFE6C034_A1EE_11D3_BA39_0080C8C11E51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TxtParse.h"

#define DATA unsigned char

class CINITxtParse : public CTxtParse
{
private:
	CString INI_Path;

	virtual int PassFilter(char c);
	virtual CString GetStr(DATA*& TxtBuf, DWORD* Arg1, DWORD* Arg2);
	virtual int HandleRet(DATA*& TxtBuf, CString RetStr, WORD Ret);

public:
	CINITxtParse();
	CINITxtParse(CString Path);
	virtual ~CINITxtParse();

	LRESULT ParseTxt(CStrParse& StrParse);
	LRESULT ParseTxt(CString Path, CStrParse& StrParse);
};

#endif // !defined(AFX_INITXTPARSE_H__AFE6C034_A1EE_11D3_BA39_0080C8C11E51__INCLUDED_)
