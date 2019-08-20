#include "stdafx.h"
#include "Scriptor.h"
#include "ScriptorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
// TEMP -	Bos keywords
static LPTSTR s_apszCppKeywordList[] =
{
    _T("static-var"),
    _T(".test"),
	_T("move"),
	_T("turn"),
	_T("sleep"),
	_T("return"),
	NULL*
};*/

static BOOL IsBosKeyword(LPCTSTR pszChars, int nLength)
{
	BOS_KEYWORD_t* pKW = g_BosKeywords;
	for (; pKW != NULL; pKW = pKW->Next)
	{
		if (strnicmp(pKW->Keyword, pszChars, nLength) == 0
			&& pKW->Keyword[nLength] == 0)
			return TRUE;
	}
	return FALSE;
}

static BOOL IsCppNumber(LPCTSTR pszChars, int nLength)
{
	if (nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x')
	{
		for (int I = 2; I < nLength; I++)
		{
			if (isdigit(pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') || (pszChars[I] >= 'a' && pszChars[I] <= 'f'))
				continue;
			return FALSE;
		}
		return TRUE;
	}
	if (!isdigit(pszChars[0]))
		return FALSE;
	for (int I = 1; I < nLength; I++)
	{
		if (!isdigit(pszChars[I]) && pszChars[I] != '+' && pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' && pszChars[I] != 'E')
			return FALSE;
	}
	return TRUE;
}

#define DEFINE_BLOCK(pos, colorindex)                                        \
	ASSERT((pos) >= 0 && (pos) <= nLength);                                  \
	if (pBuf != NULL)                                                        \
	{                                                                        \
		if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)) \
		{                                                                    \
			pBuf[nActualItems].m_nCharPos = (pos);                           \
			pBuf[nActualItems].m_nColorIndex = (colorindex);                 \
			nActualItems++;                                                  \
		}                                                                    \
	}

#define COOKIE_COMMENT 0x0001
#define COOKIE_PREPROCESSOR 0x0002
#define COOKIE_EXT_COMMENT 0x0004
#define COOKIE_STRING 0x0008
#define COOKIE_CHAR 0x0010

DWORD CScriptorView::ParseLine(DWORD dwCookie, int nLineIndex, TEXTBLOCK* pBuf, int& nActualItems)
{
	int nLength = GetLineLength(nLineIndex);
	if (nLength <= 0)
		return dwCookie & COOKIE_EXT_COMMENT;

	LPCTSTR pszChars = GetLineChars(nLineIndex);
	BOOL bFirstChar = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
	BOOL bRedefineBlock = TRUE;
	BOOL bDecIndex = FALSE;
	int nIdentBegin = -1;
	int I = 0;
	for (;; I++)
	{
		if (bRedefineBlock)
		{
			int nPos = I;
			if (bDecIndex)
				nPos--;
			if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
			}
			else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
			{
				DEFINE_BLOCK(nPos, COLORINDEX_STRING);
			}
			else if (dwCookie & COOKIE_PREPROCESSOR)
			{
				DEFINE_BLOCK(nPos, COLORINDEX_PREPROCESSOR);
			}
			else
			{
				DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
			}
			bRedefineBlock = FALSE;
			bDecIndex = FALSE;
		}

		if (I == nLength)
			break;

		if (dwCookie & COOKIE_COMMENT)
		{
			DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	String constant "...."
		if (dwCookie & COOKIE_STRING)
		{
			if (pszChars[I] == '"' && (I == 0 || pszChars[I - 1] != '\\'))
			{
				dwCookie &= ~COOKIE_STRING;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	Char constant '..'
		if (dwCookie & COOKIE_CHAR)
		{
			if (pszChars[I] == '\'' && (I == 0 || pszChars[I - 1] != '\\'))
			{
				dwCookie &= ~COOKIE_CHAR;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		//	Extended comment /*....*/
		if (dwCookie & COOKIE_EXT_COMMENT)
		{
			if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '*')
			{
				dwCookie &= ~COOKIE_EXT_COMMENT;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		if (I > 0 && pszChars[I] == '/' && pszChars[I - 1] == '/')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		//	Preprocessor directive #....
		if (dwCookie & COOKIE_PREPROCESSOR)
		{
			if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
			{
				DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
				dwCookie |= COOKIE_EXT_COMMENT;
			}
			continue;
		}

		//	Normal text
		if (pszChars[I] == '"')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_STRING;
			continue;
		}
		if (pszChars[I] == '\'')
		{
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_CHAR;
			continue;
		}
		if (I > 0 && pszChars[I] == '*' && pszChars[I - 1] == '/')
		{
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_EXT_COMMENT;
			continue;
		}

		if (bFirstChar)
		{
			if (pszChars[I] == '#')
			{
				DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
				dwCookie |= COOKIE_PREPROCESSOR;
				continue;
			}
			if (!isspace(pszChars[I]))
				bFirstChar = FALSE;
		}

		if (pBuf == NULL)
			continue; //	We don't need to extract keywords,
				//	for faster parsing skip the rest of loop

		if ((isalnum(pszChars[I])) || (pszChars[I] == '_') || (pszChars[I] == '.') || (pszChars[I] == '-'))
		//((nIdentBegin>0)&&(pszChars[I]=='-')) )
		{
			if (nIdentBegin == -1)
				nIdentBegin = I;
		}
		else
		{
			if (nIdentBegin >= 0)
			{
				if (IsBosKeyword(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
				}
				else if (IsCppNumber(pszChars + nIdentBegin, I - nIdentBegin))
				{
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
				}
				bRedefineBlock = TRUE;
				bDecIndex = TRUE;
				nIdentBegin = -1;
			}
		}
	}

	if (nIdentBegin >= 0)
	{
		if (IsBosKeyword(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
		}
		else if (IsCppNumber(pszChars + nIdentBegin, I - nIdentBegin))
		{
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
		}
	}

	if (pszChars[nLength - 1] != '\\')
		dwCookie &= COOKIE_EXT_COMMENT;
	return dwCookie;
}