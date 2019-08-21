// BosSrcParse.cpp: implementation of the CBosSrcParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Defs.h"
#include "Exports.h"
#include "BosCmdParse.h"
#include "BosSrcParse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBosSrcParse::CBosSrcParse() : CTxtParse()
{
	ProgressCallback = 0;
	LastPos = 0;
	LastLine = 0;
#ifdef _DEBUG
	fout.open("debug_src.txt");
#endif
}

CBosSrcParse::CBosSrcParse(PROGRESSCALLBACK pProgressCallback) : CTxtParse()
{
	ProgressCallback = pProgressCallback;
	LastPos = 0;
	LastLine = 0;
#ifdef _DEBUG
	fout.open("debug_src.txt");
#endif
}

CBosSrcParse::~CBosSrcParse()
{
#ifdef _DEBUG
	fout.close();
#endif
}

void CBosSrcParse::Progress(long CurPos, long EndPos)
{
	if (ProgressCallback)
		ProgressCallback(CurPos, EndPos);
}

int CBosSrcParse::PassFilter(char c)
{
	if (c == 0x0D)
	{
		Line_Count++;
		return 0;
	}
	if ((c < 33) || (c > 126))
		return 0;
	switch (c)
	{
		case '%':
		case '\'':
		case '@':
		case '^':
		case '`':
		case '~':
			return 0;
		default:
			return 1;
	}
	return 1;
}

inline int IsOperator(char c)
{
	switch (c)
	{
		case '$':
		case '!':
		case '*':
		case '+':
		case '-':
		case '/':
		case '<':
		case '=':
		case '>':
		case '?':
		case '|':
		case '&':
			return true;
		default:
			return false;
	}
	return false;
}

inline int IsNum(char c1, char c2, int CanBeNeg)
{
	if (isdigit(c1) || (c1 == '.'))
		return TRUE;
	if (((c1 == '<') || (c1 == '[')) && ((isdigit(c2)) || (c2 == '.') || (c2 == '-')))
		return TRUE;
	if ((c1 == '-') && (isdigit(c2) || (c2 == '.')) && CanBeNeg)
		return TRUE;
	return FALSE;
}

CString CBosSrcParse::GetStr(DATA*& TxtBuf, DWORD* Arg1, DWORD* Arg2)
{
	//int x;
	WORD* IdentTypeFlag = pLOWORD(Arg1);
	WORD* ExFlag1 = pHIWORD(Arg1);
	BYTE* PassedEOL = pLOBYTE(pLOWORD(Arg2));
	BYTE* LookingFor = pHIBYTE(pLOWORD(Arg2));
	WORD* CharsFromLast = pHIWORD(Arg2);
	bool IsFloat = false;
	bool IsOct = false;
	bool IsHex = false;
	bool IsAng = false;
	bool IsLin = false;
	bool IgnoreLine = false;
	DATA* tempHold;
	CString Ident = "";

	if ((*TxtBuf == '/') && (((*(TxtBuf + 1)) == '/') || ((*(TxtBuf + 1)) == '*')))
	{
#ifdef _DEBUG
		fout << "Found Comment\n";
#endif
		Ident += *TxtBuf;
		Ident += *(TxtBuf + 1);
		TxtBuf += 2;
		*IdentTypeFlag = TYP_MISC;
		*ExFlag1 = 1;
	} // end if(isComment)
	else if (isalpha(*TxtBuf) || (*TxtBuf == '_'))
	{
#ifdef _DEBUG
		fout << "Found Ident\n";
#endif
		for (; (isalnum(*TxtBuf) || (*TxtBuf == '_') || (*TxtBuf == '-')); TxtBuf++)
		{
			if (TxtBuf - BufferData.TxtStart >= BufferData.BufLength)
				break;
			Ident += *TxtBuf;
		}
		*IdentTypeFlag = TYP_IDENT;
	} // end if(isalpha)
	  /*	else if( isdigit(*TxtBuf) || (*TxtBuf=='.') ||
		   ( ( (*TxtBuf=='-') || (*TxtBuf=='[') || (*TxtBuf=='<') ) &&
			 ( (isdigit(*(TxtBuf+1)) || (*(TxtBuf+1)=='.') || (*(TxtBuf+1)=='-') ) &&
			 (!(*LookingFor)) ) ) ) */
	else if (IsNum(*TxtBuf, *(TxtBuf + 1), !(*LookingFor)))
	{
#ifdef _DEBUG
		fout << "Found Num\n";
#endif

		if (*TxtBuf == '[')
		{
			IsLin = true;
			TxtBuf++;
			tempHold = TxtBuf;
		}
		else if (*TxtBuf == '<')
		{
			IsAng = true;
			TxtBuf++;
			tempHold = TxtBuf;
		}
		if (*TxtBuf == '-')
		{
			Ident = "-";
			TxtBuf++;
		}

		if (*TxtBuf == '0')
		{
			if ((*(TxtBuf + 1) == 'x') || (*(TxtBuf + 1) == 'X'))
			{
				IsHex = true;
			}
			else
				IsOct = true;
		}
		for (; (isxdigit(*TxtBuf) || (*TxtBuf == '.') || (*TxtBuf == 'x') || (*TxtBuf == 'X')); TxtBuf++)
		{
			if (*TxtBuf == '.')
			{
				if (IsHex)
					break;
				IsOct = false;
				IsFloat = true;
			}
			Ident += *TxtBuf;
		}

		*IdentTypeFlag = TYP_NUM;
		if (IsFloat)
			*ExFlag1 = NTYP_FLOAT;
		else if (IsHex)
			*ExFlag1 = NTYP_HEX;
		else if (IsOct)
			*ExFlag1 = NTYP_OCT;
		else
			*ExFlag1 = NTYP_DEC;

		if (IsAng)
		{
			if (*TxtBuf != '>')
			{
				Ident = "<";
				*IdentTypeFlag |= TYP_OPERATOR;
				TxtBuf = tempHold;
			}
			else
			{
				TxtBuf++;
				*ExFlag1 |= NTYP_ANGULAR;
			}
		}
		else if (IsLin)
		{
			if (*TxtBuf != ']')
			{
				Ident = "[";
				*IdentTypeFlag |= TYP_MISC;
				TxtBuf = tempHold;
			}
			else
			{
				TxtBuf++;
				*ExFlag1 |= NTYP_LINEAR;
			}
		}
	} // end if(isdigit)
	else if (IsOperator(*TxtBuf))
	{
#ifdef _DEBUG
		fout << "Found Op\n";
#endif
		for (; IsOperator(*TxtBuf); TxtBuf++)
			Ident += *TxtBuf;
		*IdentTypeFlag = TYP_OPERATOR;
	}
	else if (*TxtBuf == '\"')
	{
#ifdef _DEBUG
		fout << "Found String\n";
#endif
		*ExFlag1 = 0;
		for (TxtBuf++; ((*TxtBuf) != ('\"')); TxtBuf++)
		{
			if (*TxtBuf == 0x0A)
			{
				*ExFlag1 = 1;
				break;
			}
			Ident += *TxtBuf;
		}
		TxtBuf++;
		*IdentTypeFlag = TYP_STRING;
	} // end if(isString)
	else if (*TxtBuf == '#')
	{
#ifdef _DEBUG
		fout << "Found PreProc\n";
#endif
		for (TxtBuf++; isalpha(*TxtBuf); TxtBuf++)
		{
			if (TxtBuf - BufferData.TxtStart >= BufferData.BufLength)
				break;
			Ident += *TxtBuf;
		}
		*IdentTypeFlag = TYP_PREPROC;
	} // end if(isPreProc)
	else if (*TxtBuf == '\\')
	{
#ifdef _DEBUG
		fout << "Found escape seq \n";
#endif
		TxtBuf++;
		if (*TxtBuf == 0x0D)
		{

#ifdef _DEBUG
			fout << "Escaping EOL \n";
#endif
			*IdentTypeFlag = TYP_IGNOREME;
			LastLine = Line_Count + 1;
			IgnoreLine = true;
		}
		else if (*TxtBuf == '"')
		{
			Ident += *TxtBuf;
			TxtBuf++;
			*IdentTypeFlag = TYP_MISC;
			*ExFlag1 = 0;
		}
	}
	else
	// if its a misc character like '{','('...
	{
#ifdef _DEBUG
		fout << "Found Misc\n";
#endif
		Ident += *TxtBuf;
		TxtBuf++;
		*IdentTypeFlag = TYP_MISC;
		*ExFlag1 = 0;
	}

	// Get the amount of characters that have passed since last GetStr
	if (LastPos)
		*CharsFromLast = (TxtBuf - LastPos);
	// Save Current Position so we can see how far
	// we've gone when GetStr is called again
	LastPos = TxtBuf;

	// Check to see if Line_Count is greater than last time
	if (LastLine != Line_Count)
		*PassedEOL = 1;
	else
		*PassedEOL = 0;
	// Save our current line to check next time
	if (!IgnoreLine)
		LastLine = Line_Count;

	// return our str
	return Ident;
}

int CBosSrcParse::HandleRet(DATA*& TxtBuf, CString RetStr, WORD Ret)
{
	int x, locale;
	DATA* BufPtr;
	CString temp;
	switch (Ret)
	{
		case RET_INSERT:
			if (RetStr == "")
				break;
			TxtBuf -= (RetStr.GetLength());
			BufPtr = TxtBuf;
			for (x = 0; x < RetStr.GetLength(); x++)
			{
				*BufPtr = RetStr[x];
				BufPtr++;
			}
			break;
		case RET_LINE_CMNT:
			for (; *TxtBuf != 0x0D; TxtBuf++)
				if ((TxtBuf - BufferData.TxtStart) >= BufferData.BufLength)
					break;
			Line_Count++;
			TxtBuf++;
			break;
		case RET_BLCK_CMNT:
			for (; !((*TxtBuf == '*') && (*(TxtBuf + 1) == '/')); TxtBuf++)
				if ((TxtBuf - BufferData.TxtStart) >= BufferData.BufLength)
					break;
				else if (*TxtBuf == 0x0D)
					Line_Count++;
			TxtBuf += 2;
			break;
		case RET_NEXT_LINE:
			for (; *TxtBuf != 0x0D; TxtBuf++)
				if ((TxtBuf - BufferData.TxtStart) >= BufferData.BufLength)
					break;
				else if ((*TxtBuf == '\\') && (*TxtBuf == 0x0D))
				{
					TxtBuf += 2;
					Line_Count++;
				}
			Line_Count++;
			TxtBuf++;
			break;
		case RET_SEEK_NEXT:
#ifdef _DEBUG
			fout << "Seeking Next\n";
#endif
			if ((RetStr == "if") || (RetStr == "ifdef") || (RetStr == "ifndef"))
				locale = 1;
			else if (RetStr == "else")
			{
				return 1;
			}
			else if (RetStr == "elif")
			{
				TxtBuf -= 3;
				*TxtBuf = '#';
				return 1;
			}
			else if (RetStr == "endif")
			{
				return 1;
			}
			else
				locale = 0;
			for (; true; TxtBuf++)
			{
				if ((TxtBuf - BufferData.TxtStart) >= BufferData.BufLength)
					return 0;
				else if (*TxtBuf == 0x0D)
					Line_Count++;
				else if (*TxtBuf == '#')
				{
					BufPtr = TxtBuf;
					for (TxtBuf++, temp = ""; isalpha(*TxtBuf); TxtBuf++)
						temp += *TxtBuf;
#ifdef _DEBUG
					fout << "Found " << temp << ", Locale -" << locale << std::endl;
#endif
					if (temp == "endif")
					{
						if (locale == 0)
						{
							return 1;
						}
						else
							locale--;
					}
					else if (temp == "else")
					{
						if (locale == 0)
						{
							return 1;
						}
					}
					else if (temp == "elif")
					{
						if (locale == 0)
						{
							TxtBuf -= 3;
							*TxtBuf = '#';
							return 1;
						}
					}
					else if ((temp == "if") || (temp == "ifdef") || (temp == "ifndef"))
					{
						locale++;
					}
				} // end if(#)
			}	 // end for
			break;
		case RET_SEEK_ENDIF:
#ifdef _DEBUG
			fout << "Seeking endif\n";
#endif
			for (locale = 0; true; TxtBuf++)
				if ((TxtBuf - BufferData.TxtStart) >= BufferData.BufLength)
					return 0;
				else if (*TxtBuf == 0x0D)
					Line_Count++;
				else if (*TxtBuf == '#')
				{
					BufPtr = TxtBuf;
					for (TxtBuf++, temp = ""; isalpha(*TxtBuf); TxtBuf++)
						temp += *TxtBuf;
#ifdef _DEBUG
					fout << "Found " << temp << ", Locale -" << locale << std::endl;
#endif
					if (temp == "endif")
					{
						if (locale == 0)
						{
							return 1;
						}
						else
							locale--;
					}
					else if ((temp == "if") || (temp == "ifdef") || (temp == "ifndef"))
					{
						locale++;
					}
				}
			break;
		case RET_EXIT:
			return 0;
	}
	return 1;
}

LRESULT CBosSrcParse::ParseBos(LPCTSTR Path,
	CBosCmdParse& StrParse,
	SETTINGS Set,
	OUTPUTWINCALLBACK Out,
	PROGRESSCALLBACK Prog)
{
	LRESULT ReturnValue;
	HANDLE Bos;
	long Length;
	DATA* Buf;
	unsigned long BR;

	ProgressCallback = Prog;
	Bos = CreateFile(Path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
		NULL);
	if (Bos == INVALID_HANDLE_VALUE)
		return 0;

	Length = GetFileSize(Bos, 0);
	Buf = new DATA[Length];
	ReadFile(Bos, Buf, Length, &BR, 0);
	if (!StrParse.Initialize(Set, Out, Prog))
	{
		ReturnValue = 0;
		goto EXIT;
	}
	if (ProgressCallback)
	{
		ProgressCallback(0, Length);
	}
	if (Out)
	{
		Out(Set.CurrentFile, "Compiling Bos...", 0, 0);
		//Out(strcat("    ",Path),0,0);
	}

	ReturnValue = CTxtParse::ParseTxt(Buf, Length, StrParse);

EXIT:
	if (Buf)
		delete[] Buf;
	CloseHandle(Bos);
	return ReturnValue;
}