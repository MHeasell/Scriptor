// CobScriptCode.cpp: implementation of the CCobScriptCode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Defs.h"
#include "CobScriptCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CCobValBuf Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

CCobValBuf::CCobValBuf(std::ofstream& ffout, int Child)
{
	MaxLength = 8;
	Buffer = new long[MaxLength];
	Length = 0;
	AmChild = Child;
	HaveChild = false;
	ChildBuf = 0;
	Operators.SetSize(0);
	Holding = false;
	fout = &ffout;
	//fout.open("C:\\windows\\desktop\\debug\\debug_ValBuf.txt",ios::app);
	*fout << "Created\n";
}

CCobValBuf::~CCobValBuf()
{
	if (ChildBuf)
		delete ChildBuf;
	Operators.RemoveAll();
	if (Buffer)
		delete[] Buffer;
	*fout << "Deleted\n";
}

#else

CCobValBuf::CCobValBuf(int Child)
{
	MaxLength = 8;
	Buffer = new long[MaxLength];
	Length = 0;
	AmChild = Child;
	HaveChild = false;
	ChildBuf = 0;
	Operators.SetSize(0);
	Holding = false;
}

CCobValBuf::~CCobValBuf()
{
	if (ChildBuf)
		delete ChildBuf;
	Operators.RemoveAll();
	if (Buffer)
		delete[] Buffer;
}

#endif

//////////////////////////////////////////////////////////////////////
// CCobValBuf Implementation
//////////////////////////////////////////////////////////////////////

int CCobValBuf::IncMax(int Length2)
{
	int x;
	long* temp;
	int PrevMax;

	for (PrevMax = MaxLength; (Length + Length2) >= MaxLength; MaxLength = MaxLength << 1)
		;
	if (MaxLength != PrevMax)
	{
		temp = Buffer;
		Buffer = new long[MaxLength];
		for (x = Length - 1; x >= 0; x--)
			Buffer[x] = temp[x];
		delete[] temp;
	}
	return 1;
}

void CCobValBuf::ReadyDump()
{
	IncMax(Operators.GetSize());
	for (int pos = Operators.GetUpperBound(); pos >= 0; pos--)
	{
		Buffer[Length] = Operators[pos].Val;
		Length++;
	}
	Operators.RemoveAll();
}

int CCobValBuf::AddItem(CCobValBuf* ValBuf)
{
	int x;
	if (!ValBuf)
		return 0;
	if (HaveChild)
		return ChildBuf->AddItem(ValBuf);
	IncMax(ValBuf->GetLength());
	for (x = 0; x < ValBuf->GetLength(); x++)
		Buffer[Length + x] = ValBuf->Buffer[x];
	Length += ValBuf->GetLength();
	return 1;
}

int CCobValBuf::AddItem(CCobCmdBuf* CmdBuf)
{
	int x;
	if (!CmdBuf)
		return 0;
	if (HaveChild)
		return ChildBuf->AddItem(CmdBuf);
	IncMax(CmdBuf->GetLength());
	for (x = 0; x < CmdBuf->GetLength(); x++)
		Buffer[Length + x] = CmdBuf->Buffer[x];
	Length += CmdBuf->GetLength();
	return 1;
}

int CCobValBuf::AddItem(WORD Type, long Val1, long Val2)
{
	int x;
	OPERATOR Op;
#ifdef _DEBUG
	*fout << "Adding Item" << Type << " - " << Val1 << " - " << Val2 << std::endl;
#endif
	if (HaveChild)
	{
#ifdef _DEBUG
		*fout << "Have Child\n";
#endif
		x = ChildBuf->AddItem(Type, Val1, Val2);
		if (x == 2)
		{
			IncMax(ChildBuf->GetLength());
			for (x = 0; x < ChildBuf->GetLength(); x++)
				Buffer[Length + x] = ChildBuf->Buffer[x];
			Length += ChildBuf->GetLength();
			HaveChild = false;
			delete ChildBuf;
			ChildBuf = 0;
		}
		return 1;
	}
	if (Type & TYP_MISC)
	{
#ifdef _DEBUG
		*fout << "Got Type Misc " << Val1 << std::endl;
#endif
		if (Val1 == 1) // '('
		{
#ifdef _DEBUG
			*fout << "Creating Child\n";
			ChildBuf = new CCobValBuf(*fout, true);
#else
			ChildBuf = new CCobValBuf(true);
#endif
			HaveChild = true;
			return 1;
		}
		else if (Val1 == 2) // ')'
		{
			if (AmChild)
			{
				ReadyDump();
				return 2;
			}
			else
				return 0;
		}
	}
	else if (Type & TYP_OPERATOR)
	{
#ifdef _DEBUG
		*fout << "Got Op " << std::hex << Val1 << std::dec << std::endl;
#endif
		Op.Val = Val1;
		Op.Priority = Val2;
		if (Type & WAIT)
		{
			if (!Holding)
			{
				Operators.Add(Op);
				Holding = true;
			}
		}
		else
		{
			if (Operators.GetSize() >= 1)
			{
				x = Operators.GetUpperBound();
				if (Operators[x].Priority >= Op.Priority)
				{
					IncMax(1);
					Buffer[Length] = Operators[x].Val;
					Length++;
					Operators[x] = Op;
				}
				else
					Operators.Add(Op);
			}
			else
				Operators.Add(Op);
		}
	}
	else
	{
#ifdef _DEBUG
		*fout << "Got Val " << Val2 << std::endl;
#endif
		if (Holding)
		{
			IncMax(3);
			Buffer[Length] = Val1;
			Buffer[Length + 1] = Val2;
			Buffer[Length + 2] = Operators[Operators.GetUpperBound()].Val;
			Operators.RemoveAt(Operators.GetUpperBound());
			Operators.FreeExtra();
			Length += 3;
			Holding = false;
		}
		else
		{
			IncMax(2);
			Buffer[Length] = Val1;
			Buffer[Length + 1] = Val2;
			Length += 2;
		}
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////
// CCobCmdBuf Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCobCmdBuf::CCobCmdBuf(int NoCmd)
{
	NoCmdBuf = NoCmd;
	MaxLength = 16;
	Buffer = new long[16];
	HaveCmd = false;
	CmdPos = 0;
	Length = (NoCmdBuf ? 0 : 1);
#ifdef _DEBUG
	fout.open("C:\\windows\\desktop\\debug\\debug_CmdBuf.txt", std::ios::app);
	fout << "Created\n\n";
#endif
}

CCobCmdBuf::~CCobCmdBuf()
{
#ifdef _DEBUG
	fout << "Destroyed\n\n";
	fout.close();
#endif
	if (Buffer)
		delete[] Buffer;
}

//////////////////////////////////////////////////////////////////////
// CCobCmdBuf Implementation
//////////////////////////////////////////////////////////////////////

void CCobCmdBuf::Shift(int Start, int Spaces, int Length)
{
	for (int pos = ((Length - 1) + Spaces + Start); (Start <= (pos - Spaces)); pos--)
		Buffer[pos] = Buffer[pos - Spaces];
}

int CCobCmdBuf::AddItem(char Flag, CCobValBuf* ValBuf)
{
	int x;
	long* temp = Buffer;

	if (NoCmdBuf)
		return 0;
	for (x = 0; ((Length + ValBuf->GetLength()) > MaxLength); MaxLength = MaxLength << 1, x = 1)
		;
	if (x) //if( ((Length+ValBuf->GetLength()) > MaxLength) )
	{
		//MaxLength=MaxLength<<1;
		Buffer = new long[MaxLength];
		memcpy(Buffer, temp, Length << 2);
		//for(x=(Length-1);x>=0;x--) Buffer[x]=temp[x];
		delete[] temp;
	}

	if (Flag & SPOT_BEGIN)
	{
		Shift(0, ValBuf->GetLength(), Length);
		for (x = 0; x < ValBuf->GetLength(); x++)
			Buffer[x] = ValBuf->Buffer[x];
		Length += ValBuf->GetLength();
		CmdPos += ValBuf->GetLength();
	}
	else
	{
		Shift(CmdPos, ValBuf->GetLength(), Length - CmdPos);
		for (x = 0; x < ValBuf->GetLength(); x++)
			Buffer[CmdPos + x] = ValBuf->Buffer[x];
		Length += ValBuf->GetLength();
		CmdPos += ValBuf->GetLength();
	}

#ifdef _DEBUG
	fout << "Buffer - Added ValBuf(" << ValBuf->GetLength() << ")";
	if (NoCmdBuf)
		fout << " - NoCmd\n";
	else
		fout << std::endl;
	for (x = 0; x < Length; x++)
	{
		if ((!NoCmdBuf) && (x == CmdPos))
			fout << "CMD- ";
		else if (x >= 10)
			fout << x << " - ";
		else
			fout << " " << x << " - ";
		fout << std::hex << Buffer[x] << std::dec << std::endl;
	}
	fout << std::endl;
#endif

	return 1;
}

int CCobCmdBuf::AddItem(char Flag, CCobCmdBuf* CmdBuf)
{
	int x;
	long* temp = Buffer;

	if (NoCmdBuf)
		return 0;
	if (((Length + CmdBuf->GetLength()) > MaxLength))
	{
		MaxLength = MaxLength << 1;
		Buffer = new long[MaxLength];
		for (x = (Length - 1); x >= 0; x--)
			Buffer[x] = temp[x];
		delete[] temp;
	}

	if (Flag & SPOT_BEGIN)
	{
		Shift(0, CmdBuf->GetLength(), Length);
		for (x = 0; x < CmdBuf->GetLength(); x++)
			Buffer[x] = CmdBuf->Buffer[x];
		Length += CmdBuf->GetLength();
		CmdPos += CmdBuf->GetLength();
	}
	else
	{
		Shift(CmdPos, CmdBuf->GetLength(), Length - CmdPos);
		for (x = 0; x < CmdBuf->GetLength(); x++)
			Buffer[CmdPos + x] = CmdBuf->Buffer[x];
		Length += CmdBuf->GetLength();
		CmdPos += CmdBuf->GetLength();
	}

#ifdef _DEBUG
	fout << "Buffer - Added CmdBuf(" << CmdBuf->GetLength() << ")";
	if (NoCmdBuf)
		fout << " - NoCmd\n";
	else
		fout << std::endl;
	for (x = 0; x < Length; x++)
	{
		if ((!NoCmdBuf) && (x == CmdPos))
			fout << "CMD- ";
		else if (x >= 10)
			fout << x << " - ";
		else
			fout << " " << x << " - ";
		fout << std::hex << Buffer[x] << std::dec << std::endl;
	}
	fout << std::endl;
#endif

	return 1;
}

int CCobCmdBuf::AddItem(char Flag, long item1, long item2)
{
	int x;
	long* temp = Buffer;

	if ((Length == MaxLength) || ((Flag & TWOVALS) && (Length == (MaxLength - 1))))
	{
		MaxLength = MaxLength << 1;
		Buffer = new long[MaxLength];
		for (x = (Length - 1); x >= 0; x--)
			Buffer[x] = temp[x];
		delete[] temp;
	}

	if (Flag & TWOVALS)
		x = 2;
	else
		x = 1;

	if (Flag & PUT_CMD)
	{
		Buffer[CmdPos] = item1;
		HaveCmd = true;
	}
	else if (Flag & PRE_CMD)
	{
		if (Flag & SPOT_BEGIN)
		{
			Shift(0, x, Length);
			Buffer[0] = item1;
			if (x == 2)
				Buffer[1] = item2;
			Length += x;
			CmdPos += x;
		}
		else
		{
			Shift(CmdPos, x, Length - CmdPos);
			Buffer[CmdPos] = item1;
			if (x == 2)
				Buffer[CmdPos + 1] = item2;
			Length += x;
			CmdPos += x;
		}
	} // end if(Pre_Cmd)
	else if (Flag & POST_CMD)
	{
		if (Flag & SPOT_BEGIN)
		{
			Shift(CmdPos + 1, x, Length - (CmdPos + 1));
			Buffer[CmdPos + 1] = item1;
			if (x == 2)
				Buffer[CmdPos + 2] = item2;
			Length += x;
		}
		else
		{
			Buffer[Length] = item1;
			if (x == 2)
				Buffer[Length + 1] = item2;
			Length += x;
		}
	}

#ifdef _DEBUG
	fout << "Buffer\n";
	for (x = 0; x < Length; x++)
	{
		if ((!NoCmdBuf) && (x == CmdPos))
			fout << "CMD- ";
		else if (x >= 10)
			fout << x << " - ";
		else
			fout << " " << x << " - ";
		fout << std::hex << Buffer[x] << std::dec << std::endl;
	}
	fout << std::endl;
#endif

	return 1;
}

//////////////////////////////////////////////////////////////////////
// CCobScriptCode Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCobScriptCode::CCobScriptCode()
{
	MaxLength = 32768;
	ScriptBuf = new long[MaxLength];
	Length = 0;
	CurOffset = 0;
}

CCobScriptCode::~CCobScriptCode()
{
	if (ScriptBuf)
		delete[] ScriptBuf;
}

//////////////////////////////////////////////////////////////////////
// CCobScriptCode Implementation
//////////////////////////////////////////////////////////////////////

int CCobScriptCode::AddCmd(CCobCmdBuf* Cmd)
{
	if (!Cmd || !Cmd->CanDump())
		return 0;
	if (Cmd->GetLength() + CurOffset > MaxLength)
	{
		long* temp = ScriptBuf;
		MaxLength = MaxLength << 1;
		ScriptBuf = new long[MaxLength];
		for (int x = (Length - 1); x >= 0; x--)
			ScriptBuf[x] = temp[x];
		delete[] temp;
	}

	for (int x = (Cmd->GetLength() - 1); x >= 0; x--)
		ScriptBuf[CurOffset + x] = Cmd->Buffer[x];
	Length += Cmd->GetLength();
	CurOffset += Cmd->GetLength();

	return 1;
}