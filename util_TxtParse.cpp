// util_TxtParse.cpp //                           Author: KhalvKalash
//////////////////////                                Date: 2/26/2001
//
/////////////////////////////////////////////////////////////////////


//#include "sys_Defs.h"
#include "StdAfx.h"
#include <assert.h>
#include "util_TxtParse.h"


//////////////////////////////////////////////////////////////////////
// Default Construction/Destruction
//
util_TxtParse::util_TxtParse()
{}
util_TxtParse::~util_TxtParse()
{}
//
//////////////////////////////////////////////////////////////////////

BOOL util_TxtParse::Begin( BYTE* Txt, DWORD TxtLength, Parse_Info* Info )
{
	assert( (Txt!=NULL)&&(TxtLength!=0) );
	m_Txt = Txt;
	m_TxtLength = TxtLength;
	m_Offset = 0;
	m_LastWasVar = FALSE;
	m_Locale = 0;

	return Continue(Info);
}

#define READ_UNTIL( str, condition )		for( x=0; (condition); x++)\
{ str[x]=m_Txt[m_Offset]; m_Offset++; if(m_Offset==m_TxtLength) return FALSE;\
if( (m_Txt[m_Offset]=='/')&&(m_Txt[m_Offset+1]=='/') )\
{ if( !MoveToNextLine() ) return FALSE; }}\
str[x]='\0'; m_Offset++;

BOOL util_TxtParse::Continue( Parse_Info* Info )
{
	int x=0;
	while( m_Offset<m_TxtLength )
	{
		if( !MoveToNextValid() ) return FALSE;
		switch( m_Txt[m_Offset] )
		{
			case '[':
			{
				m_Offset++;
				READ_UNTIL( Info->Value, m_Txt[m_Offset]!=']' );
				Info->Type = PARSE_Header;
				m_LastWasVar = FALSE;
                KillTrailers(Info);
				return TRUE;
			}

			case '{':
			{
				m_Locale++;
				m_Offset++;
				m_LastWasVar = FALSE;
				Info->Type = PARSE_LevelUp;
				return TRUE;
			}

			case '}':
			{
				m_Locale--;
				m_Offset++;
				m_LastWasVar = FALSE;
				Info->Type = PARSE_LevelDown;
				return TRUE;
			}

			default:
			{
				//if( !m_LastWasVar )
				{
					READ_UNTIL( Info->Variable, m_Txt[m_Offset]!='=' );
				//	Info->Class = PARSE_Variable;
				//	m_LastWasVar = TRUE;
				//	return TRUE;
				//}
				//else
				//{
					if( !MoveToNextValid() ) return FALSE;
					READ_UNTIL( Info->Value, m_Txt[m_Offset]!=';' );
				//	Info->Class = PARSE_Value;
				//	m_LastWasVar = FALSE;
					Info->Type = PARSE_VarVal;
                    KillTrailers(Info);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

#undef READ_UNTIL