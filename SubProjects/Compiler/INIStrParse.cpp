// INIStrParse.cpp: implementation of the CINIStrParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Defs.h"
#include "INIStrParse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CINIStrParse::CINIStrParse(CArray<CMD_TYPE, CMD_TYPE&>* Coms,
	CArray<OPERATOR, OPERATOR&>* Ops,
	CArray<CONS, CONS&>* Cons,
	CStringArray* Errs)
	: CStrParse()
{
	ComLst = Coms;
	OpLst = Ops;
	ConLst = Cons;
	ErrLst = Errs;
	InCommands = false;
	InConsts = false;
	InOps = false;
	InErrors = false;
	Locale = 0;
	WantVar = true;
	WantVal = false;

#ifdef _DEBUG
	fout.open("debug_INI.txt");
#endif
}

CINIStrParse::~CINIStrParse()
{
#ifdef _DEBUG
	fout.close();
#endif
}

void CINIStrParse::HandleCategory(CString Cat)
{
	if (Cat.CompareNoCase("OPERATORS") == 0)
	{
#ifdef _DEBUG
		fout << "-> In Operators\n";
#endif
		InCommands = InConsts = InErrors = false;
		InOps = true;
	}
	else if (Cat.CompareNoCase("STD_DEFS") == 0)
	{

#ifdef _DEBUG
		fout << "-> In Standard Defs\n";
#endif
		InCommands = InErrors = false;
		InConsts = true;
		InOps = false;
	}
	else if (Cat.CompareNoCase("COMMANDS") == 0)
	{
#ifdef _DEBUG
		fout << "-> In Commands\n";
#endif
		InErrors = false;
		InCommands = true;
		InConsts = InOps = false;
	}
	else if (Cat.CompareNoCase("ERRORS") == 0)
	{
#ifdef _DEBUG
		fout << "-> In Errors\n";
#endif
		InErrors = true;
		InCommands = InConsts = InOps = false;
	}
	else
	{
#ifdef _DEBUG
		fout << "-> In something else\n";
#endif
		InCommands = InConsts = InOps = InErrors = false;
	}
}

void GetList(CString Val, CStringArray* StrArray)
{
	int x;
	CString temp;

	StrArray->RemoveAll();
	StrArray->SetSize(0);
	if (Val.Find("|") == (-1))
	{
		StrArray->Add(Val);
		return;
	}
	temp = "";
	for (x = 0; x < Val.GetLength(); x++)
		if (Val[x] == '|')
		{
			if (temp != "")
				StrArray->Add(temp);
			temp = "";
		}
		else
			temp += Val[x];
	if (temp != "")
		StrArray->Add(temp);
}

void CINIStrParse::GetExp(CString Str)
{
	if (Str.CompareNoCase("FMT_STR") == 0)
	{
		Cur->Expecting = FMT_STR;
	}
	else if (Str.CompareNoCase("EXP_VARIES") == 0)
	{
		Cur->Expecting = EXP_VARIES;
	}
	else if (Str.CompareNoCase("EXP_ELSE") == 0)
	{
		Cur->Expecting = EXP_ELSE;
	}
	else if (Str.CompareNoCase("NEXT_VARI") == 0)
	{
		Cur->Expecting = NEXT_VARI;
	}
	else if (Str.CompareNoCase("END_VARI") == 0)
	{
		Cur->Expecting = END_VARI;
	}
	else if (Str.CompareNoCase("CMD_VAL") == 0)
	{
		Cur->Expecting = CMD_VAL;
	}
	else if (Str.CompareNoCase("SINGLE") == 0)
	{
		Cur->Expecting = SINGLE;
	}
	else if (Str.CompareNoCase("VALSTR") == 0)
	{
		Cur->Expecting = VALSTR;
	}
	else if (Str.CompareNoCase("LIST") == 0)
	{
		Cur->Expecting = LIST;
	}
	else if (Str.CompareNoCase("NOTHING") == 0)
	{
		Cur->Expecting = NOTHING;
	}
	else if (Str.CompareNoCase("BRAK_OPEN") == 0)
	{
		Cur->Expecting = BRAK_OPEN;
	}
	else if (Str.CompareNoCase("NEW_STR") == 0)
	{
		Cur->Expecting = NEW_STR;
	}
}

void CINIStrParse::GetType(CStringArray& StrArray)
{
	int x;

	for (x = 0; x < StrArray.GetSize(); x++)
	{
		if (StrArray[x].CompareNoCase("TYP_NUM") == 0)
		{
			Cur->Type |= TYP_NUM;
		}
		else if (StrArray[x].CompareNoCase("TYP_OPERATOR") == 0)
		{
			Cur->Type |= TYP_OPERATOR;
		}
		else if (StrArray[x].CompareNoCase("TYP_MISC") == 0)
		{
			Cur->Type |= TYP_MISC;
		}
		else if (StrArray[x].CompareNoCase("TYP_STRING") == 0)
		{
			Cur->Type |= TYP_STRING;
		}
		else if (StrArray[x].CompareNoCase("TYP_PREPROC") == 0)
		{
			Cur->Type |= TYP_PREPROC;
		}
		else if (StrArray[x].CompareNoCase("TYP_SCRIPT") == 0)
		{
			Cur->Type |= TYP_SCRIPT;
		}
		else if (StrArray[x].CompareNoCase("TYP_CONST") == 0)
		{
			Cur->Type |= TYP_CONST;
		}
		else if (StrArray[x].CompareNoCase("TYP_PIECE") == 0)
		{
			Cur->Type |= TYP_PIECE;
		}
		else if (StrArray[x].CompareNoCase("TYP_FVAR") == 0)
		{
			Cur->Type |= TYP_FVAR;
		}
		else if (StrArray[x].CompareNoCase("TYP_SVAR") == 0)
		{
			Cur->Type |= TYP_SVAR;
		}
		else if (StrArray[x].CompareNoCase("TYP_SOUND") == 0)
		{
			Cur->Type |= TYP_SOUND;
		}
		else if (StrArray[x].CompareNoCase("TYP_CMD") == 0)
		{
			Cur->Type |= TYP_CMD;
		}
		else if (StrArray[x].CompareNoCase("TYP_AXIS") == 0)
		{
			Cur->Type |= TYP_AXIS;
		}
		else if (StrArray[x].CompareNoCase("TYP_VAR") == 0)
		{
			Cur->Type |= TYP_VAR;
		}
		else if (StrArray[x].CompareNoCase("TYP_IDENT") == 0)
		{
			Cur->Type |= TYP_IDENT;
		}
		else if (StrArray[x].CompareNoCase("TYP_VALUE") == 0)
		{
			Cur->Type |= TYP_VALUE;
		}
		else if (StrArray[x].CompareNoCase("TYP_UNIQUE") == 0)
		{
			Cur->Type |= TYP_UNIQUE;
		}
		else if (StrArray[x].CompareNoCase("TYP_LABEL") == 0)
		{
			Cur->Type |= TYP_LABEL;
		}
		else if (StrArray[x].CompareNoCase("HTYP_PLACEHOLD") == 0)
		{
			Cur->Type |= HTYP_PLACEHOLD;
		}
		else if (StrArray[x].CompareNoCase("HTYP_CUROFFSET") == 0)
		{
			Cur->Type |= HTYP_CUROFFSET;
		}
		else if (StrArray[x].CompareNoCase("HTYP_ELSEHOLD") == 0)
		{
			Cur->Type |= HTYP_ELSEHOLD;
		}
		else if (StrArray[x].CompareNoCase("HTYP_LOOP") == 0)
		{
			Cur->Type |= HTYP_LOOP;
		}
		else if (StrArray[x].CompareNoCase("HTYP_LASTLOOP") == 0)
		{
			Cur->Type |= HTYP_LASTLOOP;
		}
	}
}

void CINIStrParse::GetWTD(CStringArray& StrArray)
{
	int x;

	for (x = 0; x < StrArray.GetSize(); x++)
	{
		if (StrArray[x].CompareNoCase("NOTHING") == 0)
		{
			Cur->WhatToDo = NOTHING;
		}
		else if (StrArray[x].CompareNoCase("END") == 0)
		{
			Cur->WhatToDo = END;
		}
		else if (StrArray[x].CompareNoCase("INC_LOCALE") == 0)
		{
			Cur->WhatToDo = INC_LOCALE;
		}
		else if (StrArray[x].CompareNoCase("DO_PREOP") == 0)
		{
			Cur->WhatToDo = DO_PREOP;
		}
		else if (StrArray[x].CompareNoCase("KILL_PREOP") == 0)
		{
			Cur->WhatToDo = KILL_PREOP;
		}
		else if (StrArray[x].CompareNoCase("GET_STRS") == 0)
		{
			Cur->WhatToDo = GET_STRS;
		}
		else if (StrArray[x].CompareNoCase("FORCE_INSERT") == 0)
		{
			Cur->WhatToDo = FORCE_INSERT;
		}
		else if (StrArray[x].CompareNoCase("BACK") == 0)
		{
			Cur->WhatToDo = BACK;
		}
		else if (StrArray[x].CompareNoCase("SAVELAST") == 0)
		{
			Cur->WhatToDo = SAVELAST;
		}
		else if (StrArray[x].CompareNoCase("GOTOSAVE") == 0)
		{
			Cur->WhatToDo = GOTOSAVE;
		}
		else if (StrArray[x].CompareNoCase("ADD_HOLD") == 0)
		{
			Cur->WhatToDo = ADD_HOLD;
		}
		else if (StrArray[x].CompareNoCase("COUNT") == 0)
		{
			Cur->WhatToDo = COUNT;
		}
		else if (StrArray[x].CompareNoCase("INITCOUNT") == 0)
		{
			Cur->WhatToDo = INITCOUNT;
		}
		else if (StrArray[x].CompareNoCase("PLACECOUNT") == 0)
		{
			Cur->WhatToDo = PLACECOUNT;
		}
		else if (StrArray[x].CompareNoCase("DELAY_CALL") == 0)
		{
			Cur->WhatToDo = DELAY_CALL;
		}
		else if (StrArray[x].CompareNoCase("END_NOCOUNT") == 0)
		{
			Cur->WhatToDo = END_NOCOUNT;
		}
		else if (StrArray[x].CompareNoCase("FILL_HOLDS") == 0)
		{
			Cur->WhatToDo = FILL_HOLDS;
		}
		else if (StrArray[x].CompareNoCase("ADDFVAR") == 0)
		{
			Cur->WhatToDo = ADDFVAR;
		}
		else if (StrArray[x].CompareNoCase("ADDSVAR") == 0)
		{
			Cur->WhatToDo = ADDSVAR;
		}
		else if (StrArray[x].CompareNoCase("ADDSOUND") == 0)
		{
			Cur->WhatToDo = ADDSOUND;
		}
		else if (StrArray[x].CompareNoCase("ADDPIECE") == 0)
		{
			Cur->WhatToDo = ADDPIECE;
		}
		else if (StrArray[x].CompareNoCase("NODUMPVALBUF") == 0)
		{
			Cur->WhatToDo = NODUMPVALBUF;
		}
		else if (StrArray[x].CompareNoCase("ENDRETCMD") == 0)
		{
			Cur->WhatToDo = ENDRETCMD;
		}
		else if (StrArray[x].CompareNoCase("ADDTOVALBUF") == 0)
		{
			Cur->WhatToDo = ADDTOVALBUF;
		}
		else if (StrArray[x].CompareNoCase("PUT_CMD") == 0)
		{
			Cur->WhatToDo = ADDTOCMDBUF;
			Cur->CmdBufFlg |= PUT_CMD;
		}
		else if (StrArray[x].CompareNoCase("PRE_CMD") == 0)
		{
			Cur->WhatToDo = ADDTOCMDBUF;
			Cur->CmdBufFlg |= PRE_CMD;
		}
		else if (StrArray[x].CompareNoCase("POST_CMD") == 0)
		{
			Cur->WhatToDo = ADDTOCMDBUF;
			Cur->CmdBufFlg |= POST_CMD;
		}
		else if (StrArray[x].CompareNoCase("SPOT_BEGIN") == 0)
		{
			Cur->WhatToDo = ADDTOCMDBUF;
			Cur->CmdBufFlg |= SPOT_BEGIN;
		}
		else if (StrArray[x].CompareNoCase("SPOT_END") == 0)
		{
			Cur->WhatToDo = ADDTOCMDBUF;
			Cur->CmdBufFlg |= SPOT_END;
		}
		else if (StrArray[x].CompareNoCase("TWOVALS") == 0)
		{
			Cur->WhatToDo = ADDTOCMDBUF;
			Cur->CmdBufFlg |= TWOVALS;
		}
	}
}

void CINIStrParse::HandleVarVal(CString Var, CString Val)
{
	CStringArray StrArray;

#ifdef _DEBUG
	fout << Var << " = " << Val << std::endl;
#endif

	if (InCommands)
	{
		if (Locale == 1)
		{
			if (Var.CompareNoCase("Keyword") == 0)
				Cmd.Command = Val;
			else if (Var.CompareNoCase("Flag") == 0)
			{
				if (Val.CompareNoCase("STANDARD") == 0)
					Cmd.Flag |= REGULAR;
				else if (Val.CompareNoCase("NOSCRIPT") == 0)
					Cmd.Flag |= NOSCRIPT;
				else if (Val.CompareNoCase("NOCMD") == 0)
					Cmd.Flag |= NOCMD;
				else if (Val.CompareNoCase("RETURNS_VALUE") == 0)
					Cmd.Flag |= RETURNS_VALUE;
				else if (Val.CompareNoCase("NOTA") == 0)
					Cmd.Flag |= NOTA;
			}
		}
		else if (Locale == 2)
		{
			if (Var.CompareNoCase("Expecting") == 0)
			{
				GetExp(Val);
			}
			else if (Var.CompareNoCase("OfType") == 0)
			{
				GetList(Val, &StrArray);
				GetType(StrArray);
			}
			else if (Var.CompareNoCase("Error") == 0)
			{
				if (Val == "")
					Cur->Error = 0;
				else
					Cur->Error = (WORD)atol(Val);
			}
			else if (Var.CompareNoCase("String") == 0)
			{
				if (Val == "")
					Cur->Fmt = ";";
				else
					Cur->Fmt = Val;
			}
			else if (Var.CompareNoCase("Value") == 0)
			{
				Cur->Val = strtol(Val, 0, 16);
			}
			else if (Var.CompareNoCase("WhatToDo") == 0)
			{
				GetList(Val, &StrArray);
				GetWTD(StrArray);
			}
		}
	}
	else if (InConsts && (Locale == 1))
	{
		if (Var.CompareNoCase("Name") == 0)
			Const.Name = Val;
		else if (Var.CompareNoCase("Value") == 0)
			Const.Val = Val;
	}
	else if (InOps && (Locale == 1))
	{
		if (Var.CompareNoCase("Operator") == 0)
			Op.Op = Val;
		else if (Var.CompareNoCase("Value") == 0)
			Op.Val = strtol(Val, 0, 16);
		else if (Var.CompareNoCase("Priority") == 0)
			Op.Priority = strtol(Val, 0, 10);
		else if (Var.CompareNoCase("LeftOnly") == 0)
		{
			if (Val == "0")
				Op.LeftOnly = 0;
			else
				Op.LeftOnly = 1;
		}
	}
	else if (InErrors && (Locale == 1))
	{
		if ((Var != "") && (Val != ""))
		{
			if (ErrLst->GetUpperBound() < atol(Val))
			{
				ErrLst->SetSize(atol(Val) + 1);
			}
			ErrLst->SetAt(atol(Val), Var);
		}
	}
}

void CINIStrParse::HandleLocaleChange(int up)
{
#ifdef _DEBUG
	fout << "Locale - " << Locale << " " << ((up) ? "UP" : "DOWN") << std::endl;
#endif
	if (up)
	{
		if (InCommands)
		{
			if (Locale == 0)
			{
				Cmd.Flag = 0;
				Cmd.Format = new CMD_FMT;
				Cur = Cmd.Format;
				LastWasCmd = true;
			}
			else if (Locale == 1)
			{
				if (!LastWasCmd)
				{
					Cur->Next = new CMD_FMT;
					Cur = Cur->Next;
				}
				else
					LastWasCmd = false;
				Cur->Expecting = 0;
				Cur->Type = 0;
				Cur->Val = 0;
				Cur->Fmt = "";
				Cur->Error = 0;
				Cur->WhatToDo = 0;
				Cur->CmdBufFlg = 0;
				Cur->Next = 0;
			}
		}
		else if (InConsts && (Locale == 0))
		{
			Const.Args = 0;
			Const.Flag = CONST_STDDEF;
		}
		else if (InOps && (Locale == 0))
		{
			Op.LeftOnly = 0;
		}
	}
	else
	{
		if (InCommands)
		{
			if (Locale == 1)
			{
				if (ComLst)
					ComLst->Add(Cmd);
			}
			else if (Locale == 2)
			{
			}
		}
		else if (InConsts && (Locale == 1))
		{
			if (ConLst)
				ConLst->Add(Const);
		}
		else if (InOps && (Locale == 1))
		{
			if (OpLst)
				OpLst->Add(Op);
		}
	}
}

WORD CINIStrParse::ParseStr(CString& Str, DWORD* Arg1, DWORD* Arg2)
{
	WORD Ret = 99;
	DWORD Flag = *Arg1;
	CString Val = Str;

	switch (Flag)
	{
		case 1:
			HandleCategory(Val);
			WantVar = true;
			break;
		case 2:
			HandleLocaleChange(1);
			Locale++;
			break;
		case 3:
			HandleLocaleChange(0);
			Locale--;
			break;
		case 5:
			if (WantVar)
			{
				Var = Val;
				WantVal = true;
				WantVar = false;
			}
			else
			{
				HandleVarVal(Var, Val);
				WantVal = false;
				WantVar = true;
			}
			break;
		case 10:
			if (Val == "//")
				Ret = 1;
			else
				Ret = 2;
	}
	if (WantVal)
		*Arg2 = 1;
	else
		*Arg2 = 0;
	return Ret;
}