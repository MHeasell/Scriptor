// StrParse.cpp: implementation of the CStrParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StrParse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrParse::CStrParse()
{
	Line_Count = 0;
}

CStrParse::~CStrParse()
{
}

WORD CStrParse::ParseStr(CString& Str, DWORD* Arg1, DWORD* Arg2)
{
	// Add handling code in a derived class
	return 1;
}