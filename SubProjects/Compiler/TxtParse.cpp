// TxtParse.cpp: implementation of the CTxtParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StrParse.h"
#include "TxtParse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTxtParse::CTxtParse()
{
	Line_Count = 1;
}

CTxtParse::~CTxtParse()
{
}

void CTxtParse::Progress(long CurPos, long EndPos)
{
}

int CTxtParse::PassFilter(char c)
{
	if (isalnum(c))
		return 1;
	if (c = 0x0A)
		Line_Count++;
	return 0;
}

CString CTxtParse::GetStr(unsigned char*& TxtBuf, DWORD* Arg1, DWORD* Arg2)
{
	int Len = 0;
	WORD* pArg1 = ((WORD*)Arg1);
	CString RetStr;

	if (isalpha(*TxtBuf))
	{
		*pArg1 = 0;
		for (; isalpha(*TxtBuf); TxtBuf++)
			RetStr += *TxtBuf;
		return RetStr;
	}
	else
	{
		*pArg1 = 1;
		for (; isalnum(*TxtBuf); TxtBuf++)
			RetStr += *TxtBuf;
		return RetStr;
	}
	return "";
}

int CTxtParse::HandleRet(unsigned char*& TxtBuf, CString RetStr, WORD Ret)
{
	return 1;
}

LRESULT CTxtParse::ParseTxt(unsigned char* TxtBuf,
	long BufLength,
	CStrParse& StrParse)
{
	unsigned char* TxtPtr; // The current position in the TxtBuf
	CString Str;		   // String returned by GetStr to be parsed
	DWORD Arg1 = 0;		   // Args reserved for future derivations
	DWORD Arg2 = 0;
	WORD RetCon; // Return value of ParseStr

	BufferData.BufLength = BufLength;
	BufferData.TxtStart = TxtBuf;
	// Parse Txt until done or broken
	for (TxtPtr = TxtBuf; (TxtPtr - TxtBuf) < BufLength;)
	{
		// Check if we pass any char filters
		if (PassFilter(*TxtPtr))
		{
			// We passed a filter so get the string
			Str = GetStr(TxtPtr, &Arg1, &Arg2);
			StrParse.Line_Count = Line_Count;
			// Pass string and flags to be parsed
			RetCon = StrParse.ParseStr(Str, &Arg1, &Arg2);
			// Hanlde the return values of the str parse and
			// break out of loop if necessary
			if (!HandleRet(TxtPtr, Str, RetCon))
				break;
			// Continue parsing Txt
			continue;
		}
		// We Didn't catch any filters so continue Parse
		TxtPtr++;
		// Call member to for Progress
		Progress(TxtPtr - TxtBuf, BufLength);
	}

	return 1;
}
