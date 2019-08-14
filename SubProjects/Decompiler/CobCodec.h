#include "stdafx.h"
#include <iomanip>
#include <fstream>
#include "Stack.h"

struct HEADER_COB
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
	long OffsetToSoundNameArray;
	long NumberOfSounds;
};

struct OPSTRING
{
	long Operator;
	CStack Values;
};

struct MISCMD
{
	StackVal* StackVals;
	long Param1;
	long Param2;
};

struct Var
{
	CString Name;
	WORD Type;
};

struct OPERATOR
{
	CString Op;
	long Val;
	int Priority;
};

struct UNITVAL
{
	CString Name;
	long Val;
};

struct ARG
{
	CString Name;
	int Type;
};

struct COM_FUNC
{
	CString Name;
	char Game;
	int NumArgs;
	ARG* Args;
};

typedef struct _LOOP
{
	long Start;
	long Stop;
} LOOP;

class CCobCodec
{
private:
	HANDLE File;
	CString MainFile;
	std::ofstream fout;
	HEADER_COB Header;
	DWORD* OffsetArray;
	unsigned char* NameArray;
	CStack STACK;
	OPSTRING* OpString;
	WORD OpPos;
	MISCMD MisCmd[4];
	int MisPos;
	long* Labels;
	char NumLabels;
	//Var* StaticVar;
	//Var* FuncVar;
	CArray <Var,Var&> StaticVar;
	CArray <Var,Var&> FuncVar;
	int NumFuncVar;
	CArray <OPERATOR,OPERATOR&> Operators;
	CArray <UNITVAL,UNITVAL&> UnitValues;
	CArray <COM_FUNC,COM_FUNC&> Common_Funcs;
	CArray <UNITVAL,UNITVAL&> Signals;
	CArray <UNITVAL,UNITVAL&> Smoke_Flame;
	CArray <LOOP,LOOP&> Loops;
	CStringArray AnimFunc;
	bool HaveMaxDFlames;
	bool HaveMaxSmokePuffs;
	bool HaveMaxDFLevel;
	int NumSmokePieces;
	CArray <int,int> SmokePieces;
	bool IsYardH;
	bool GotOYard;
	bool GotCYard;
	bool GotStates;
	int NumGo;
	int NumStop;
	int DirCreated;
	int FloatPrec;
	float Max;
	float Min;
	float AConst;
	float LConst;
	bool TAK;
	bool WasElse;
	long last;
	DWORD OffsetToFirstName;
	// Options
	int PrintSoundList;
	int HeadInfo;
	int Offsets;
	int ShowPush;
	int Guess;
	int StdLibs;
	int MiscLibs;
	int IncDec;
	int TryKeepSmall;
	int ShowReturn;

	int GetINIInfo();
	void InitStack();
	void KillStack();
	inline int NewOpString();
	inline void KillOpString(int Index);
	int Push( long Val, BYTE Type );
	int PushOp( long Operator );
	void Pop( WORD Flags, int Back=0 );
	void Pop( StackVal Val, WORD Flags, int First=0 );
	int NextIsZero();
	int IsIncDec(long What,BYTE Flag);
	DWORD GetPieceNameOffset(long Index);
	LPSTR PrintPieceName(long Index);
	DWORD GetScriptNameOffset(long Index);
	LPSTR PrintScriptName(long Index);
	DWORD GetSoundNameOffset(long Index);
	LPSTR PrintSoundName(long Index);
	void AddSignal(UNITVAL Signal);
	void PrintConst( long Val, WORD Flags, int First );
	void PrintUnitValue(long Val,int DoGet=1);
	void PrintExpConsts(long Val);
	void PrintSndConsts(long Val);
	//void PrintNumType(long Val,WORD type);
	//void PrintOp(StackVal stackval,WORD extra=1,int para=1);
	//void PrintVal(StackVal stackval,WORD extra=1);
	char  GetAxis(DWORD Index);
	LPCTSTR GetOperator(long Val);
	int GetOpPriority(long Val);
	void TabSpace(int tab,long count);
	int CheckAnimFunc(LPCTSTR Func);
	int CheckCommonFunc(LPSTR Name,int numvar);
	long GetNextCmd(long* DataPtr,long offset,long Length);
	int MakeSomeGuesses_TA(long Offset,long Length,long NumScripts);
	int MakeSomeGuesses_TAK(long Offset,long Length,long NumScripts);
	void Move_Turn_Cmd(long* DataPtr,int* pos,int tab,int Flag,long count);
	long ProcessBlock(long* DataPtr,int tab,long offset,long Length);
	long ProcessScript(LPSTR Name,char* ScriptCode,long offset,long Length);
	void PrintHeader();

public:
	CCobCodec()
	{
		int z=10;
		Offsets=false;
		HeadInfo=false;
		ShowPush=false;
		Guess=false;
		HaveMaxDFlames=false;
		HaveMaxSmokePuffs=false;
		HaveMaxDFLevel=false;
		NumSmokePieces = 0;
		SmokePieces.SetSize(0);
		IsYardH=false;
		GotStates=false;
		NumGo = -1;
		NumStop = -1;
		InitStack();
		MisPos=0;
		FloatPrec=2;
		for(int x=0;x<(FloatPrec-1);x++) z*=10;
		Max=float(1)/float(z);
		Min=Max*(-1);
		StaticVar.SetSize(0);
		FuncVar.SetSize(0);
		Signals.SetSize(0);
		Smoke_Flame.SetSize(0);
		Loops.SetSize(0);
		OffsetArray=0;
		NameArray=0;
		GetINIInfo();
		AConst=182.0F;
		LConst=163840;
		WasElse=false;
		PrintSoundList=true;
		IncDec=true;
		TryKeepSmall=true;
		ShowReturn=true;
	};

	~CCobCodec()
	{
		KillStack();
		if(OffsetArray) delete [] OffsetArray;
		if(NameArray) delete [] NameArray;
	};
	LRESULT OpenCob(LPCSTR FileName);
	LRESULT SetAttr(DECOM_SETTINGS& Set);
	LRESULT MakeBos(LPCSTR FileName,CProgressCtrl* Prog=NULL);
};