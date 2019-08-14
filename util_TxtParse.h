// util_TxtParse.h //                             Author: KhalvKalash
////////////////////                                  Date: 2/26/2001
//
/////////////////////////////////////////////////////////////////////
#ifndef _UTIL_TXT_PARSE_H_
#define _UTIL_TXT_PARSE_H_
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// Information gathered from the last parse update

typedef struct Parse_Info
{
	DWORD			Type;
	char			Variable[128];
	char			Value[128];
} *LPParse_Info;

// Possible values of the Type member
enum Parse_Info_Types
{
	PARSE_Header,           // An header was encountered ie [HEADER NAME]
                            // The header name is in the Value Member
	PARSE_VarVal,
	PARSE_LevelUp,
	PARSE_LevelDown,
};


/////////////////////////////////////////////////////////////////////
//
class util_TxtParse  
{

private:
	BYTE*				m_Txt;
	DWORD				m_TxtLength;
	DWORD				m_Offset;
	BOOL				m_LastWasVar;
	int					m_Locale;

	BOOL MoveToNextLine()
	{
		while( m_Txt[m_Offset]!=0x0A )
		{
			m_Offset++;
			if( m_Offset==m_TxtLength ) return FALSE;
		}
		return TRUE;
	}
	BOOL MoveToNextValid()
	{
		while( TRUE )
		{
			if( m_Offset==m_TxtLength ) return FALSE;
            else if( iswspace(m_Txt[m_Offset]) ) m_Offset++;
			else if( (m_Txt[m_Offset]=='/')&&(m_Txt[m_Offset+1]=='/') )
			{
				if( !MoveToNextLine() ) return FALSE;
			}
            else break;
		}
		return TRUE;
	}

public:
	BOOL Begin( BYTE* Txt, DWORD TxtLength, Parse_Info* Info );
	BOOL Continue( Parse_Info* Info );
	int  Locale(){return m_Locale;};
	void KillTrailers(Parse_Info* Info)
	{
		int len = strlen(Info->Variable)-1;
		for(;(len>=0)&&(iswspace(Info->Variable[len])); len--) Info->Variable[len]='\0';
        len = strlen(Info->Value)-1;
		for(;(len>=0)&&(iswspace(Info->Value[len])); len--) Info->Value[len]='\0';
	}

/////////////////////////////////////////////////
// Default Constructor/Deconstructor
public:
	util_TxtParse();
	virtual ~util_TxtParse();
/////////////////////////////////////////////////

}; // End class - util_TxtParse
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
#endif // !defined(_UTIL_TXT_PARSE_H_)
