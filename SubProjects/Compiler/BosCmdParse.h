// BosCmdParse.h: interface for the CBosCmdParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOSCMDPARSE_H__B04E66E6_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_)
#define AFX_BOSCMDPARSE_H__B04E66E6_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "StrParse.h"
//#include "Defs.h"
#include "CobScriptCode.h"
#include "Exports.h"

#define DATA unsigned char
/*
typedef struct _SETTINGS
{
	CString CurrentDirectory;
	CString IncludeDirectory;
	float AngularConst;
	float LinearConst;
	int TA;
} SETTINGS;
*/
typedef struct _HEADER
{
	long VersionSignature;
	long NumberOfScripts;
	long NumberOfPieces;
	long LengthOfScripts;
	long NumberOfStaticVars;
	long UKZero;
	long OffsetToScriptCodeIndexArray;
	long OffsetToScriptNameOffsetArray;
	long OffsetToPieceNameOffsetArray;
	long OffsetToScriptCode;
	long OffsetToNameArray;
	long OffsetToSoundNameOffsetArray;
	long NumberOfSounds;
} HEADER;

typedef struct _BURNER_ITEM
{
	CMD_FMT* Fmt;
	CCobCmdBuf* CmdBuf;
	CCobValBuf* ValBuf;
} BURNER_ITEM;

typedef struct _PRE_PROCESSOR
{
	char OKtoInsert;

	char LastWasDefine;
	char GotDEF;
	char CheckedForArgs;
	char GettingArgs;
	char LastWasIdent;

	char LastWasUnDefine;

	char LastWasIfType;
	CCobValBuf* ValBuf;
	char NoMoreValues;
	long Eval;
	//char DidIf;

	char LastWasInclude;

	CString LastStr;
	CStringArray Keywords;

	char StdCount;
	char InPreProcFlag;

	char InMacroFlag;
	char GotOpen;
	char GotClose;
	char NumArgs;
} PREPROC;

class CBosCmdParse : public CStrParse  
{
private:
	PREPROC PreProc;
	CONS Def;
	CONS MacDef;
	bool Got_INI;
	SETTINGS Settings;
	CArray <CMD_TYPE,CMD_TYPE&> Commands;
	CMD_FMT* CurFmt;
	CMD_FMT* LastFmt;
	CMD_FMT* SaveFmt;
	CArray <BURNER_ITEM,BURNER_ITEM&> BackBurner;
	CArray <SCRIPT,SCRIPT&> Scripts;
	CStringArray Pieces;
	CStringArray StaticVars;
	CStringArray FuncVars;
	CStringArray Sounds;
	CArray <CONS,CONS&> Constants;
	CArray <OPERATOR,OPERATOR&> OpList;
	int Locale;
	CArray <HOLD,HOLD&> Holds;
	CArray <DELAY,DELAY&> Delays;
	CStringArray Strs;
	CStringArray Errors;
	CString CurrentFile;
	int PreOp;
	int Attach_Rider;
	long Count;
	//bool InValStr;
	bool Initialized;
	int WasErrors;
	int WasWarning;
	int WasFatal;
	CCobValBuf* ValBuf;
	CCobCmdBuf* CmdBuf;
	CCobScriptCode* ScriptCode;

	PROGRESSCALLBACK ProgressCallback;
	OUTPUTWINCALLBACK OutputWin;
	CStringArray KWList;

#ifdef _DEBUG
	std::ofstream fout;
	std::ofstream spin;
	std::ofstream vout;
	std::ofstream PP;
#endif

	int CompareStr(CString Str1,CString Str2);
	int CompareStr2(CString Str1,CString Str2);
	WORD CompareTo(CString Ident,WORD ToWhat,int* Index);
	CString GetFmtErr(WORD ErrNum,CString Str,CString Fmt);
	void DoError(char Type,CString Error);
	long GetNumber(CString Str,WORD Flag);
	long GetValBufValue(CCobValBuf* ValBuff);
	void KillCmd();
	void CleanUp();

	// Handling Methods
	WORD HandlePreProcessor(CString& Str,DWORD* Arg1,DWORD* Arg2);
	WORD HandleMacro(CString& Str,DWORD* Arg1,DWORD* Arg2);
	WORD GetValBuf(CString& Str,DWORD* Arg1,DWORD* Arg2,WORD Type,CCobValBuf* &ValBuff);
	WORD StartCmdFmt(CString& Str,DWORD* Arg1,DWORD* Arg2);
	WORD HandleCmdFmt(CString& Str,DWORD* Arg1,DWORD* Arg2,bool Fresh=true);

public:
	CBosCmdParse();
	virtual ~CBosCmdParse();

	int CmdLen(){ return KWList.GetSize(); };
	CString GetCmd(int index){ return KWList[index]; };
	void PrepareKWList();
	int Initialize(SETTINGS Set,OUTPUTWINCALLBACK Out=0,PROGRESSCALLBACK pProgressCallback=0);
	int WriteCob(LPCTSTR Path);
	virtual WORD ParseStr(CString& Str,DWORD* Arg1,DWORD* Arg2);
};

#endif // !defined(AFX_BOSCMDPARSE_H__B04E66E6_9DA1_11D3_BA39_0080C8C11E51__INCLUDED_)
