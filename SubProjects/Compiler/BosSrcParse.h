// BosSrcParse.h: interface for the CBosSrcParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOSSRCPARSE_H__B04E66E5_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_)
#define AFX_BOSSRCPARSE_H__B04E66E5_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "TxtParse.h"

#define DATA unsigned char

class CBosSrcParse : public CTxtParse
{
private:
	DATA* LastPos;
	long LastLine;
	//CProgressCtrl* Progress_Control;
	PROGRESSCALLBACK ProgressCallback;

#ifdef _DEBUG
	std::ofstream fout;
#endif

	virtual void Progress(long CurPos, long EndPos);
	virtual int PassFilter(char c);
	virtual CString GetStr(DATA*& TxtBuf, DWORD* Arg1, DWORD* Arg2);
	virtual int HandleRet(DATA*& TxtBuf, CString RetStr, WORD Ret);

public:
	CBosSrcParse();
	CBosSrcParse(PROGRESSCALLBACK pProgressCallback);
	virtual ~CBosSrcParse();
	LRESULT ParseBos(LPCTSTR Path,
		CBosCmdParse& StrParse,
		SETTINGS Set,
		OUTPUTWINCALLBACK Out = 0,
		PROGRESSCALLBACK Prog = 0);
};

#endif // !defined(AFX_BOSSRCPARSE_H__B04E66E5_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_)
