// INIStrParse.h: interface for the CINIStrParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INISTRPARSE_H__AFE6C035_A1EE_11D3_BA39_0080C8C11E51__INCLUDED_)
#define AFX_INISTRPARSE_H__AFE6C035_A1EE_11D3_BA39_0080C8C11E51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "StrParse.h"
#ifdef _DEBUG
 #include <fstream>
#endif
class CINIStrParse : public CStrParse  
{
private:
	int Locale;
	CString Var;
	bool WantVar;
	bool WantVal;
	CArray <CMD_TYPE,CMD_TYPE&>* ComLst;
	CMD_TYPE Cmd;
	CMD_FMT* Cur;
	CArray <OPERATOR,OPERATOR&>* OpLst;
	OPERATOR Op;
	CArray <CONS,CONS&>* ConLst;
	CONS Const;
	CStringArray* ErrLst;
	bool InCommands;
	bool InConsts;
	bool InOps;
	bool LastWasCmd;
	bool InErrors;

#ifdef _DEBUG
	std::ofstream fout;
#endif

	void GetExp(CString Str);
	void GetType(CStringArray& StrArray);
	void GetWTD(CStringArray& StrArray);
	void HandleCategory(CString Cat);
	void HandleVarVal(CString Var,CString Val);
	void HandleLocaleChange(int up);

public:
	CINIStrParse(CArray <CMD_TYPE,CMD_TYPE&>* Coms,
				 CArray <OPERATOR,OPERATOR&>* Ops,
				 CArray <CONS,CONS&>* Cons,
				 CStringArray* Errs);
	virtual ~CINIStrParse();

	virtual WORD ParseStr(CString& Str,DWORD* Arg1,DWORD* Arg2);

};

#endif // !defined(AFX_INISTRPARSE_H__AFE6C035_A1EE_11D3_BA39_0080C8C11E51__INCLUDED_)
