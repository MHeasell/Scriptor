// TxtParse.h: interface for the CTxtParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TXTPARSE_H__0B84D9C0_9D40_11D3_BA39_0080C8C11E51__INCLUDED_)
#define AFX_TXTPARSE_H__0B84D9C0_9D40_11D3_BA39_0080C8C11E51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

typedef struct _BUFDAT
{
	long BufLength;
	unsigned char* TxtStart;
} BUFDAT;

class CTxtParse  
{
private:
	virtual void Progress(long CurPos,long EndPos);
	virtual int PassFilter(char c);
	virtual CString GetStr(unsigned char* &TxtBuf,DWORD* Arg1,DWORD* Arg2);
	virtual int HandleRet(unsigned char* &TxtBuf,CString RetStr,WORD Ret);
	
public:
	BUFDAT BufferData;
	int Line_Count;

	CTxtParse();
	virtual ~CTxtParse();

	LRESULT ParseTxt(unsigned char* TxtBuf,
					 long BufLength,
					 CStrParse& StrParse);
};

#endif // !defined(AFX_TXTPARSE_H__0B84D9C0_9D40_11D3_BA39_0080C8C11E51__INCLUDED_)
