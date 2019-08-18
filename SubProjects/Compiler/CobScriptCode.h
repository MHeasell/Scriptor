// CobScriptCode.h: interface for the CCobScriptCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COBSCRIPTCODE_H__061DC280_A457_11D3_BA39_0080C8C11E51__INCLUDED_)
#define AFX_COBSCRIPTCODE_H__061DC280_A457_11D3_BA39_0080C8C11E51__INCLUDED_

#ifdef _DEBUG
#include <fstream>
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCobCmdBuf;

class CCobValBuf
{
private:
	int AmChild;
	int HaveChild;
	int Length;
	int MaxLength;
	CArray<OPERATOR, OPERATOR&> Operators;
	CCobValBuf* ChildBuf;
	int Holding;

	int IncMax(int Length2);

public:
#ifdef _DEBUG
	CCobValBuf(std::ofstream& ffout, int Child = 0);
#else
	CCobValBuf(int Child = 0);
#endif
	virtual ~CCobValBuf();

	long* Buffer;

#ifdef _DEBUG
	std::ofstream* fout;
#endif

	int AddItem(CCobValBuf* ValBuf);
	int AddItem(CCobCmdBuf* CmdBuf);
	int AddItem(WORD Type, long Val1, long Val2);
	void ReadyDump();
	int GetLength() { return Length; }
	int CanDump() { return !HaveChild; }
};

class CCobCmdBuf
{
private:
	int Length;
	int MaxLength;
	int CmdPos;
	bool HaveCmd;
	int NoCmdBuf;

	void Shift(int Start, int Spaces, int Length);

public:
	CCobCmdBuf(int NoCmd = false);
	virtual ~CCobCmdBuf();

	long* Buffer;

#ifdef _DEBUG
	std::ofstream fout;
#endif

	int AddItem(char Flag, CCobValBuf* ValBuf);
	int AddItem(char Flag, CCobCmdBuf* CmdBuf);
	int AddItem(char Flag, long item1, long item2 = 0);
	int GetLength() { return Length; }
	int CanDump()
	{
		if ((NoCmdBuf) && (Length > 0))
			return 1;
		if ((HaveCmd) && (Length > 0))
			return 1;
		return 0;
	}
};

class CCobScriptCode
{
private:
	long Length;
	long BufLength;
	long MaxLength;

public:
	long* ScriptBuf;
	long CurOffset;

	CCobScriptCode();
	virtual ~CCobScriptCode();

	int AddCmd(CCobCmdBuf* Cmd);
};

#endif // !defined(AFX_COBSCRIPTCODE_H__061DC280_A457_11D3_BA39_0080C8C11E51__INCLUDED_)
