// INITxtParse.cpp: implementation of the CINITxtParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Defs.h"
#include "INIStrParse.h"
#include "INITxtParse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINITxtParse::CINITxtParse() : CTxtParse()
{
	INI_Path = "";
}

CINITxtParse::CINITxtParse(CString Path) : CTxtParse()
{
	INI_Path = Path;
}

CINITxtParse::~CINITxtParse()
{
}

int CINITxtParse::PassFilter(char c)
{
	if ((c < 33) || (c > 126))
		return 0;
	return 1;
}

CString CINITxtParse::GetStr(DATA*& TxtBuf, DWORD* Arg1, DWORD* Arg2)
{
	CString Val = "";

	if ((*TxtBuf == '/') && (((*(TxtBuf + 1)) == '/') || ((*(TxtBuf + 1)) == '*')))
	{
		Val += *TxtBuf;
		Val += *(TxtBuf + 1);
		TxtBuf += 2;
		*Arg1 = 10;
	} // end if(isComment)
	else if ((*TxtBuf == '[') && (*Arg2 != 1))
	{
		TxtBuf++;
		for (; *TxtBuf != ']'; TxtBuf++)
			Val += (*TxtBuf);
		TxtBuf++;
		*Arg1 = 1;
	} // end if(new category)
	else if (*TxtBuf == '{')
	{
		TxtBuf++;
		Val = "{";
		*Arg1 = 2;
	} // end if(open)
	else if (*TxtBuf == '}')
	{
		TxtBuf++;
		Val = "}";
		*Arg1 = 3;
	} // end if(close)
	/*else if(*TxtBuf==';')
	{
		TxtBuf++;
		*Arg1=0;
	} // end if(end)
	/*else if(*TxtBuf=='=')
	{
		TxtBuf++;
		for(;*TxtBuf!=';';TxtBuf++) ;
		TxtBuf++;
		*Arg1=0;
	} // end if(end)*/
	else
	{
		if (*Arg2 == 0)
		{
			for (; *TxtBuf != '='; TxtBuf++)
				Val += (*TxtBuf);
			Val.TrimRight();
		}
		else
		{
			for (; *TxtBuf != ';'; TxtBuf++)
				Val += (*TxtBuf);
			Val.TrimRight();
		}
		TxtBuf++;
		*Arg1 = 5;
	}
	return Val;
}

int CINITxtParse::HandleRet(DATA*& TxtBuf, CString RetStr, WORD Ret)
{
	switch (Ret)
	{
		case 1:
			for (; *TxtBuf != 0x0A; TxtBuf++)
				;
			Line_Count++;
			TxtBuf++;
			if (*TxtBuf == 0x0D)
				TxtBuf++;
			break;
		case 2:
			for (; (*TxtBuf != '*') && (*(TxtBuf + 1) != '\\'); TxtBuf++)
				if (*TxtBuf == 0x0A)
					Line_Count++;
			TxtBuf += 2;
			break;
		case 0:
			return 0;
	}
	return 1;
}

LRESULT CINITxtParse::ParseTxt(CString Path, CStrParse& StrParse)
{
	INI_Path = Path;
	return ParseTxt(StrParse);
}

LRESULT CINITxtParse::ParseTxt(CStrParse& StrParse)
{
	HANDLE INI;
	long Length;
	DATA* Buf;
	unsigned long BR;

	INI = CreateFile(INI_Path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
		NULL);
	if (INI == INVALID_HANDLE_VALUE)
		return 0;
	Length = GetFileSize(INI, 0);
	Buf = new DATA[Length];
	ReadFile(INI, Buf, Length, &BR, 0);
	return CTxtParse::ParseTxt(Buf, Length, StrParse);
}