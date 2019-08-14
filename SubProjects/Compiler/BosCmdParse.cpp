// BosCmdParse.cpp: implementation of the CBosCmdParse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Defs.h"

#ifdef _DEBUG
 #include <fstream.h>
 #include <iomanip.h>
#endif

#include "INIStrParse.h"
#include "INITxtParse.h"
#include "Exports.h"
#include "BosCmdParse.h"
#include "BosSrcParse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBosCmdParse::CBosCmdParse()
{
	Scripts.SetSize(0);
	Pieces.SetSize(0);
	StaticVars.SetSize(0);
	FuncVars.SetSize(0);
	Sounds.SetSize(0);
	Constants.SetSize(0);
	OpList.SetSize(0);
	Commands.SetSize(0);
	Errors.SetSize(0);
	CurFmt=0;
	Locale=0;
	Initialized=false;
	ScriptCode=0;
	ValBuf=0;
	CmdBuf=0;
	Count=0;
	CINIStrParse INI_Parse(&Commands,&OpList,&Constants,&Errors);
	CINITxtParse INI("Compiler.cfg");
	if(!INI.ParseTxt(INI_Parse))
		Got_INI=false;
	else Got_INI=true;
	OutputWin=0;

#ifdef _DEBUG
	fout.open("C:\\windows\\desktop\\debug\\debug_CmdBuf.txt");
	fout.close();
	vout.open("C:\\windows\\desktop\\debug\\debug_ValBuf.txt");
	fout.open("C:\\windows\\desktop\\DEBUG\\debug_PostINI.txt");
	PP.open("C:\\windows\\desktop\\DEBUG\\debug_PreProc.txt");
	fout<<"Operators: "<<OpList.GetSize()<<"\n";
	for(int x=0;x<OpList.GetSize();x++)
	{
		fout<<setw(5)<<OpList[x].Op<<" - "<<setw(3)<<OpList[x].Priority<<" - "<<hex<<OpList[x].Val<<dec<<endl;
	}
	fout<<"\n\nConstants: "<<Constants.GetSize()<<"\n";
	for(x=0;x<Constants.GetSize();x++)
	{
		fout<<Constants[x].Name<<" - "<<Constants[x].Val<<endl;
	}
	fout<<"\n\nCommands: "<<Commands.GetSize()<<"\n";
	for(x=0;x<Commands.GetSize();x++)
	{
		fout<<Commands[x].Command<<" - "<<Commands[x].Flag<<endl;
		CurFmt=Commands[x].Format;
		for(;CurFmt;CurFmt=CurFmt->Next)
		{
			fout<<"	"
				<<setw(2)<<int(CurFmt->Expecting)<<" - "
				<<setw(5)<<CurFmt->Type<<" - "
				<<setw(9)<<CurFmt->Val<<" - "
				<<setw(5)<<CurFmt->Fmt<<" - "
				<<setw(2)<<int(CurFmt->WhatToDo)<<" - "
				<<setw(2)<<int(CurFmt->CmdBufFlg)<<endl;
		}
		CurFmt=0;
	}
	fout<<"\n\nErrors: "<<Errors.GetSize()-1<<"\n";
	for(x=1;x<Errors.GetSize();x++)
	{
		fout<<Errors[x]<<endl;
	}
	fout.close();
	fout.open("C:\\windows\\desktop\\DEBUG\\debug_base.txt");
	spin.open("C:\\windows\\desktop\\DEBUG\\debug_spin.txt");
#endif
}

CBosCmdParse::~CBosCmdParse()
{
#ifdef _DEBUG
	fout.close();
#endif
}

void CBosCmdParse::PrepareKWList()
{
	KWList.RemoveAll();
	KWList.SetSize(0);
	for(int x=Commands.GetSize();x>=0;x--)
		if(Commands[x].Command[0] != '@')
			KWList.Add(Commands[x].Command);
	KWList.Add("#define");
	KWList.Add("#undef");
	KWList.Add("#include");
	KWList.Add("#ifdef");
	KWList.Add("#ifndef");
	KWList.Add("#if");
	KWList.Add("#elif");
	KWList.Add("#else");
	KWList.Add("#endif");
}

int CBosCmdParse::Initialize(SETTINGS Set,
							 OUTPUTWINCALLBACK Out,
							 PROGRESSCALLBACK pProgressCallback)
{
	if(!Got_INI) return 0;

	// Initialize Misc
	WasWarning=0;
	WasErrors=0;
	WasFatal=0;
	PreOp=NONE;
	if(Out) OutputWin=Out;
	ProgressCallback=pProgressCallback;
	Settings=Set;
	CurrentFile=Settings.CurrentFile;
	Locale = 0;
	CurFmt = NULL;
	Attach_Rider = false;

	// Initialize ScriptCode
	ScriptCode=new CCobScriptCode;

	// Initialize Game Setting
	CONS Const;
	Const.Val="";
	Const.Flag=CONST_BOSDEF;
	if(Settings.TA) Const.Name="TA";
	else Const.Name="TAK";
	Constants.Add(Const);

	// Initialize Lists
	BackBurner.SetSize(0);
	Holds.SetSize(0);
	Delays.SetSize(0);
	Pieces.SetSize(0);
	StaticVars.SetSize(0);
	Sounds.SetSize(0);
	FuncVars.SetSize(0);
	Scripts.SetSize(0);
	Strs.SetSize(0);

	// Initialize PreProccessor
	PreProc.OKtoInsert=true;
	PreProc.InPreProcFlag=false;
	PreProc.InMacroFlag=false;
	PreProc.LastWasInclude=false;
	PreProc.LastWasIfType=false;
	PreProc.LastWasDefine=false;
	PreProc.LastWasUnDefine=false;
	PreProc.ValBuf=0;

	Initialized=true;
	return 1;
}

int CBosCmdParse::WriteCob(LPCTSTR Path)
{
	int x;
	unsigned long BR;
#ifdef _DEBUG
	ofstream fout2;
	fout2.open("C:\\windows\\desktop\\DEBUG\\debug_out.txt");
	fout2<<"Lines "<<Line_Count<<endl;
	fout2<<"\nSVars:\n";
	for(x=0;x<StaticVars.GetSize();x++)
		fout2<<StaticVars[x]<<endl;
	fout2<<"\nSounds:\n";
	for(x=0;x<Sounds.GetSize();x++)
		fout2<<Sounds[x]<<endl;
	fout2<<"\nPieces:\n";
	for(x=0;x<Pieces.GetSize();x++)
		fout2<<Pieces[x]<<endl;
	fout2<<"\nConstants:\n";
	for(x=0;x<Constants.GetSize();x++)
		fout2<<Constants[x].Name<<" - "<<Constants[x].Val<<endl;
	fout2<<"\nScriptCode - "<<ScriptCode->CurOffset<<endl;
	fout2.close();
	fout2.open("C:\\windows\\desktop\\DEBUG\\debug_cob.txt");
	if(WasErrors)
	{
		fout2<<"There were Errors\n";
		fout2.close();
	}
#endif
	if(WasErrors)
	{
		CleanUp();
		return 0;
	}
	if(OutputWin)
	{
		OutputWin(Settings.CurrentFile,"",0,0);
		OutputWin(Settings.CurrentFile,"Making Cob...",0,0);
	}
	HANDLE Cob=CreateFile(Path,
							GENERIC_WRITE,
							FILE_SHARE_READ,
							NULL,
							CREATE_ALWAYS,
							(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
							NULL);
	if (Cob == INVALID_HANDLE_VALUE)
	{
		DoError(ERT_ERROR,"Could not create cob");
		CleanUp();
		return 0;
	}
	long HeadSize=( Settings.TA ? (sizeof(HEADER)-8):(sizeof(HEADER)) );
	HEADER Header={( Settings.TA ? 0x04:0x06 ),
				   Scripts.GetSize(),
				   Pieces.GetSize(),
				   ScriptCode->CurOffset,
				   StaticVars.GetSize(),
				   0,
				   HeadSize+(ScriptCode->CurOffset*4),
				   HeadSize+(ScriptCode->CurOffset*4)+(Scripts.GetSize()*4),
				   HeadSize+(ScriptCode->CurOffset*4)+(Scripts.GetSize()*8),
				   HeadSize,
				   HeadSize+(ScriptCode->CurOffset*4)+(Scripts.GetSize()*8)+(Pieces.GetSize()*4)+(Sounds.GetSize()*4),
				   HeadSize+(ScriptCode->CurOffset*4)+(Scripts.GetSize()*8)+(Pieces.GetSize()*4),
				   Sounds.GetSize()};
	long Size=(Scripts.GetSize()*2)+
			  Pieces.GetSize()+
			  Sounds.GetSize();
	long* OffsetArray=new long[Size];;
	LPCTSTR Name;
	long NameOffset=Header.OffsetToNameArray;
	SetFilePointer(Cob,NameOffset,0,FILE_BEGIN);
	for(x=0;x<Scripts.GetSize();x++)
	{
		OffsetArray[x]=Scripts[x].Offset;
		OffsetArray[x+Scripts.GetSize()]=NameOffset;
		Name=Scripts[x].Name;
		WriteFile(Cob,Name,Scripts[x].Name.GetLength()+1,&BR,0);
		NameOffset+=Scripts[x].Name.GetLength()+1;
	}
	for(x=0;x<Pieces.GetSize();x++)
	{
		OffsetArray[x+(Scripts.GetSize()*2)]=NameOffset;
		Name=Pieces[x];
		WriteFile(Cob,Name,Pieces[x].GetLength()+1,&BR,0);
		NameOffset+=Pieces[x].GetLength()+1;
	}
	for(x=0;x<Sounds.GetSize();x++)
	{
		OffsetArray[x+(Scripts.GetSize()*2)+Pieces.GetSize()]=NameOffset;
		Name=Sounds[x];
		WriteFile(Cob,Name,Sounds[x].GetLength()+1,&BR,0);
		NameOffset+=Sounds[x].GetLength()+1;
	}
	SetFilePointer(Cob,Header.OffsetToScriptCodeIndexArray,0,FILE_BEGIN);
	WriteFile(Cob,OffsetArray,Size*4,&BR,0);
	SetFilePointer(Cob,0,0,FILE_BEGIN);
	WriteFile(Cob,&Header,HeadSize,&BR,0);
	SetFilePointer(Cob,Header.OffsetToScriptCode,0,FILE_BEGIN);
	WriteFile(Cob,ScriptCode->ScriptBuf,ScriptCode->CurOffset*4,&BR,0);
	if(OffsetArray) delete [] OffsetArray;
	CloseHandle(Cob);
	if(ScriptCode) delete ScriptCode;

#ifdef _DEBUG
	fout2.close();
#endif

	if(OutputWin)
	{
		CString temp=Path;
		temp="Wrote "+temp;
		OutputWin(Settings.CurrentFile,temp,0,0);
	}

	CleanUp();

	if(WasWarning) return 2;
	return 1;
}

void CBosCmdParse::CleanUp()
{
	/* Clean up */

	// Clean Lists
	for(int x=Constants.GetUpperBound();x>=0;x--)
		if( (Constants[x].Flag==CONST_BOSDEF)||(Constants[x].Flag==CONST_BOSMACRO) )
			Constants.RemoveAt(x);
	Constants.FreeExtra();
	BackBurner.RemoveAll();
	Holds.RemoveAll();
	Delays.RemoveAll();
	Pieces.RemoveAll();
	StaticVars.RemoveAll();
	Sounds.RemoveAll();
	FuncVars.RemoveAll();
	Scripts.RemoveAll();
}

void CBosCmdParse::DoError(char Type,CString Error)
{
	if(Type) WasErrors++;
	else WasWarning++;
	if(OutputWin) OutputWin(CurrentFile,Error,(WORD)Line_Count,Type);
}

int CBosCmdParse::CompareStr(CString Str1,CString Str2)
{
	if(Str1.CompareNoCase(Str2)==0) return 1;
	return 0;
}

int CBosCmdParse::CompareStr2(CString Str1,CString Str2)
{
	if(Str1.Compare(Str2)==0) return 1;
	return 0;
}

WORD CBosCmdParse::CompareTo(CString Ident,WORD ToWhat,int* Index)
{
	int x;

	if( (ToWhat & TYP_CONST)||(!ToWhat) )
	{
		for(x=0;x<Constants.GetSize();x++)
			if( (CompareStr2(Constants[x].Name,Ident)) )
			{
				if(Index) *Index=x;
				return TYP_CONST;
			}
	}

	if( (ToWhat & TYP_CMD)||(!ToWhat) )
	{
		for(x=0;x<Commands.GetSize();x++)
			if( (CompareStr(Commands[x].Command,Ident)) )
			{
				if(Index) *Index=x;
				return TYP_CMD;
			}
	}

	if( (ToWhat & TYP_SCRIPT)||(!ToWhat) )
	{
		for(x=0;x<Scripts.GetSize();x++)
			if( (CompareStr(Scripts[x].Name,Ident)) )
			{
				if(Index) *Index=x;
				return TYP_SCRIPT;
			}
	}

	/*if( (ToWhat & TYP_LABEL)||(!ToWhat) )
	{
		for(x=0;x<Labels.GetSize();x++)
			if( (CompareStr(Labels[x].Name,Ident)) )
			{
				if(Index) *Index=x;
				return TYP_LABEL;
			}
	}*/

	if( (ToWhat & TYP_FVAR)||(!ToWhat) )
	{
		for(x=0;x<FuncVars.GetSize();x++)
			if( (CompareStr(FuncVars[x],Ident)) )
			{
				if(Index) *Index=x;
				return TYP_FVAR;
			}
	}

	if( (ToWhat & TYP_SVAR)||(!ToWhat) )
	{
		for(x=0;x<StaticVars.GetSize();x++)
			if( (CompareStr(StaticVars[x],Ident)) )
			{
				if(Index) *Index=x;
				return TYP_SVAR;
			}
	}

	if( (ToWhat & TYP_PIECE)||(!ToWhat) )
	{
		for(x=0;x<Pieces.GetSize();x++)
			if( (CompareStr(Pieces[x],Ident)) )
			{
				if(Index) *Index=x;
				return TYP_PIECE;
			}
	}

	if( (ToWhat & TYP_SOUND)||(!ToWhat) )
	{
		for(x=0;x<Sounds.GetSize();x++)
			if( (CompareStr(Sounds[x],Ident)) )
			{
				if(Index) *Index=x;
				return TYP_SOUND;
			}
	}

	if( (ToWhat & TYP_OPERATOR)||(!ToWhat) )
	{
		for(x=0;x<OpList.GetSize();x++)
			if( (CompareStr(OpList[x].Op,Ident)) )
			{
				if(Index) *Index=x;
				return TYP_OPERATOR;
			}
	}

	if( (ToWhat & TYP_AXIS)||(!ToWhat) )
	{
		if( (CompareStr("x-axis",Ident)) )
		{
			if(Index) *Index=0;
			return TYP_AXIS;
		}
		else if( (CompareStr("y-axis",Ident)) )
		{
			if(Index) *Index=1;
			return TYP_AXIS;
		}
		else if( (CompareStr("z-axis",Ident)) )
		{
			if(Index) *Index=2;
			return TYP_AXIS;
		}
	}

	if( (ToWhat & TYP_NUM) ) return TYP_NUM;
	if( (ToWhat & TYP_STRING) ) return TYP_STRING;

	return TYP_UNIQUE;
}

long CBosCmdParse::GetValBufValue(CCobValBuf* ValBuff)
{
	int a,b,x,z,found;

	if( (!ValBuff)||(ValBuff->GetLength()<1))
		return 0;
	
	CArray <long,long> Vals;
	Vals.SetSize(0);
#ifdef _DEBUG
	PP<<"Getting Value form ValBuf"<<endl;
#endif
	for(x=0;x<ValBuff->GetLength();x++)
	{
		switch(ValBuff->Buffer[x])
		{
		case CMD_PUSH_CON:Vals.Add(ValBuff->Buffer[x+1]);
#ifdef _DEBUG
						  PP<<"Got Value "<<ValBuff->Buffer[x+1]<<endl;
#endif
						  x++;
						  break;
		case CMD_PUSH_SVAR:
		case CMD_PUSH_FVAR:Vals.Add(0);
						   x++;
						   break;
		default:found=0;
#ifdef _DEBUG
				PP<<"Got Suspected Op "<<hex<<ValBuff->Buffer[x]<<dec<<endl;
#endif
				for(z=0;z<OpList.GetSize();z++)
				{
					if(OpList[z].Val==ValBuff->Buffer[x])
					{
						found=1;
						break;
					}
				}
				if(found)
				{
#ifdef _DEBUG
					PP<<"Got Op "<<OpList[z].Op<<endl;
#endif
					if(OpList[z].Op == "+")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" + "<<Vals[b]
						  <<" = "<<Vals[a] + Vals[b]<<endl;
#endif
						Vals[a]=(Vals[a] + Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == "-")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" - "<<Vals[b]
						  <<" = "<<Vals[a] - Vals[b]<<endl;
#endif
						Vals[a]=(Vals[a] - Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == "*")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" * "<<Vals[b]
						  <<" = "<<Vals[a] * Vals[b]<<endl;
#endif
						Vals[a]=(Vals[a] * Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == "/")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" / "<<Vals[b]
						  <<" = "<<Vals[a] / Vals[b]<<endl;
#endif
						if(Vals[b]==0) Vals[a]=0;
						else Vals[a]= (Vals[a] / Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == "==")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" == "<<Vals[b]
						  <<" = "<<(Vals[a] == Vals[b])<<endl;
#endif
						Vals[a] = (Vals[a] == Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == ">=")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" >= "<<Vals[b]
						  <<" = "<<(Vals[a] >= Vals[b])<<endl;
#endif
						Vals[a]= (Vals[a] >= Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == "<=")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" <= "<<Vals[b]
						  <<" = "<<(Vals[a] <= Vals[b])<<endl;
#endif
						Vals[a]=(Vals[a] <= Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == ">")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" > "<<Vals[b]
						  <<" = "<<(Vals[a] > Vals[b])<<endl;
#endif
						Vals[a]=(Vals[a] > Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == "<")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" < "<<Vals[b]
						  <<" = "<<(Vals[a] < Vals[b])<<endl;
#endif
						Vals[a]=(Vals[a] < Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == "!=")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" != "<<Vals[b]
						  <<" = "<<(Vals[a] != Vals[b])<<endl;
#endif
						Vals[a]=(Vals[a] != Vals[b]);
						Vals.RemoveAt(b);
					}
					else if(OpList[z].Op == "|")
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" | "<<Vals[b]
						  <<" = "<<(Vals[a] | Vals[b])<<endl;
#endif
						Vals[a]=(Vals[a] | Vals[b]);
						Vals.RemoveAt(b);
					}
					else if( (OpList[z].Op == "||")||(OpList[z].Op == "OR") )
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" || "<<Vals[b]
						  <<" = "<<(Vals[a] || Vals[b])<<endl;
#endif
						Vals[a]=(Vals[a] || Vals[b]);
						Vals.RemoveAt(b);
					}
					else if( (OpList[z].Op == "&&")||(OpList[z].Op == "AND") )
					{
						ASSERT(Vals.GetSize()>1);
						a=Vals.GetUpperBound()-1;
						b=a+1;
#ifdef _DEBUG
						PP<<Vals[a]<<" && "<<Vals[b]
						  <<" = "<<(Vals[a] && Vals[b])<<endl;
#endif
						Vals[a]=(Vals[a] && Vals[b]);
						Vals.RemoveAt(b);
					}
					else if( (OpList[z].Op == "!")||(OpList[z].Op == "NOT") )
					{
						ASSERT(Vals.GetSize()>0);
						a=Vals.GetUpperBound();
#ifdef _DEBUG
						PP<<"!"<<Vals[a]<<" = "<<(!Vals[a])<<endl;
#endif
						Vals[a]=(!Vals[a]);
					}
				}// end if(found)
				else
				{
					if(Vals.GetSize()>1)
					{
						a=Vals.GetUpperBound()-1;
						b=a+1;
						Vals[a]=Vals[a] == Vals[b];
						Vals.RemoveAt(b);
					}
					else if(Vals.GetSize()>0)
					{
						a=Vals.GetUpperBound();
						Vals[a]=!Vals[a];
					}
				} // end else
		}// end switch
	} // end for

	if(Vals.GetSize()) return Vals[0];
	return 0;
}

WORD CBosCmdParse::HandlePreProcessor(CString& Str,DWORD* Arg1,DWORD* Arg2)
{
	int index,x;
	CString temp;
	WORD* IdentTypeFlag=pLOWORD(Arg1);
	WORD* ExFlag1=pHIWORD(Arg1);
	BYTE* PassedEOL=pLOBYTE(pLOWORD(Arg2));
	BYTE* LookingFor=pHIBYTE(pLOWORD(Arg2));
	WORD* CharsFromLast=pHIWORD(Arg2);

#ifdef _DEBUG
	PP<<"PreProccessing on "<<Str<<" - "<<*PassedEOL<<" - "<<*CharsFromLast<<endl;
#endif

	if(PreProc.InPreProcFlag)
	{
#ifdef _DEBUG
	PP<<"In PreProc Chain"<<endl;
#endif
		if(PreProc.LastWasDefine)
		{
			if(!PreProc.GotDEF)
			{
#ifdef _DEBUG
				PP<<"Getting Def\n";
#endif
				if(!(*IdentTypeFlag & TYP_IDENT))
				{
					DoError(ERT_ERROR,"PreProcessor-> Expected Identifier");
					return RET_EXIT;
				}
				if( (Str!="TA")&&(Str!="TAK")&&(CompareTo(Str,TYP_CONST,&index) & TYP_CONST) )
				{
					DoError(ERT_WARNING,"Redefinition of "+Str);
					Constants.RemoveAt(index);
				}
				Def.Name=Str;
				Def.Val="";
				Def.Args=0;
				Def.Flag=CONST_BOSDEF;
				PreProc.GotDEF=true;
				PreProc.OKtoInsert=true;
			}
			else
			{
				if(*PassedEOL)
				{
					if(PreProc.GettingArgs)
					{
						DoError(ERT_ERROR,"PreProcessor Error: Found newline while processing macro argument list");
						return RET_EXIT;
					}
#ifdef _DEBUG
					PP<<"Adding "<<Def.Name<<endl;
#endif
					if(Def.Name=="TA")
					{
						if(!Settings.TA)
						{
							for(x=Constants.GetUpperBound();x>=0;x--)
							{
								if(Constants[x].Name=="TAK")
									Constants.RemoveAt(x);
							}
							Settings.TA=true;
							Constants.Add(Def);
						}
					}
					else if(Def.Name=="TAK")
					{
						if(Settings.TA)
						{
							for(x=Constants.GetUpperBound();x>=0;x--)
							{
								if(Constants[x].Name=="TA")
									Constants.RemoveAt(x);
							}
							Settings.TA=false;
							Constants.Add(Def);
						}
					}
					else Constants.Add(Def);
					PreProc.Keywords.RemoveAll();
					PreProc.InPreProcFlag=0;
					PreProc.LastWasDefine=false;
					return ParseStr(Str,Arg1,Arg2);
				}
				else if(!PreProc.CheckedForArgs)
				{
					if((Str=="(")&&(*CharsFromLast<=1))
					{
#ifdef _DEBUG
						PP<<"Found Arg List"<<endl;
#endif
						PreProc.GettingArgs=true;
						PreProc.OKtoInsert=false;
						Def.Flag=CONST_BOSMACRO;
						PreProc.CheckedForArgs=true;
						PreProc.LastWasIdent=false;
						PreProc.Keywords.SetSize(0);
						PreProc.LastStr="";
					}
					else
					{
#ifdef _DEBUG
						PP<<"Adding "<<Str<<" of type "<<*IdentTypeFlag<<" to Val"<<endl;
#endif
						if(*IdentTypeFlag & TYP_STRING)
						{
#ifdef _DEBUG
							PP<<"Got String"<<endl;
#endif
							Str="\""+Str+"\"";
						}
						else if(*IdentTypeFlag & TYP_NUM)
						{
							if(*ExFlag1 & NTYP_ANGULAR)
							{
								Str="<"+Str+">";
							}
							else if(*ExFlag1 & NTYP_LINEAR)
							{
								Str="["+Str+"]";
							}
						}
						Def.Val+=Str+" ";
						PreProc.CheckedForArgs=true;
						PreProc.GettingArgs=false;
					}
				}
				else if(PreProc.GettingArgs)
				{
					if( (Str==")")&&(PreProc.LastWasIdent)&&(PreProc.Keywords.GetSize()) )
					{
						PreProc.GettingArgs=false;
						PreProc.OKtoInsert=true;
					}
					else
					{
						if(PreProc.LastWasIdent)
						{
							if(Str!=",")
							{
								DoError(ERT_ERROR,"PreProcessor Error: Invalid Macro argument format");
								return RET_EXIT;
							}
							else PreProc.LastWasIdent=false;
						}
						else
						{
							for(x=PreProc.Keywords.GetUpperBound();x>=0;x--)
							{
								if(CompareStr(Str,PreProc.Keywords[x]))
								{
									DoError(ERT_ERROR,"PreProcessor Error: Duplicate argument in Macro");
									return RET_EXIT;
								}
							}
							PreProc.Keywords.Add(Str);
							Def.Args++;
							PreProc.LastWasIdent=true;
						}
					}
				}// end Getting Args
				else
				{
#ifdef _DEBUG
					PP<<"Adding "<<Str<<" of type "<<*IdentTypeFlag<<" to Val"<<endl;
#endif
					if(*IdentTypeFlag & TYP_STRING)
					{
#ifdef _DEBUG
						PP<<"Got String"<<endl;
#endif
						Str="\""+Str+"\"";
					}
					else if(*IdentTypeFlag & TYP_NUM)
					{
						if(*ExFlag1 & NTYP_ANGULAR)
						{
							Str="<"+Str+">";
						}
						else if(*ExFlag1 & NTYP_LINEAR)
						{
							Str="["+Str+"]";
						}
					}

					if(Def.Args)
					{
						index=1;
						for(x=PreProc.Keywords.GetUpperBound();x>=0;x--)
						{
							if(CompareStr(Str,PreProc.Keywords[x]))
							{
								index=0;
								itoa(x+1,temp.GetBuffer(2),10);
								temp.ReleaseBuffer();
								Def.Val+=("@"+temp);
							}
						}
						if(index) Def.Val+=Str+" ";
					}
					else Def.Val+=Str+" ";
				}
			}
		}
		else if(PreProc.LastWasUnDefine)
		{
			if(!PreProc.GotDEF)
			{
#ifdef _DEBUG
				PP<<"undefing "<<Str<<endl;
#endif
				if((CompareStr(Str,"TA"))||(CompareStr(Str,"TAK")))
				{
					DoError(ERT_WARNING,"Cannot Undefine "+Str+". It is used by the compiler");
				}
				else if(CompareTo(Str,TYP_CONST,&index) & TYP_CONST)
				{
					Constants.RemoveAt(index);
				}
#ifdef _DEBUG
				else PP<<"Not Found"<<endl;
#endif
				PreProc.LastWasUnDefine=false;
				PreProc.InPreProcFlag=false;
				PreProc.OKtoInsert=true;
			}
		}
		else if(PreProc.LastWasInclude)
		{
#ifdef _DEBUG
			PP<<"Getting Include path"<<endl;
#endif
			if(!(*IdentTypeFlag & TYP_STRING))
			{
				DoError(ERT_ERROR,"Expected string");
				return RET_EXIT;
			}
			HANDLE Bos;
			long Length;
			DATA* Buf;
			unsigned long BR;
			CBosSrcParse BosInc(ProgressCallback);
			CString LastFile;

			LastFile=CurrentFile;
			CurrentFile=Settings.CurrentDirectory+Str;
#ifdef _DEBUG
			PP<<"Looking for "<<Settings.CurrentFile<<endl;
#endif
			Bos=CreateFile(CurrentFile,
			   GENERIC_READ,
			   FILE_SHARE_READ,
			   NULL,
			   OPEN_EXISTING,
			   (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
			   NULL);
			if (Bos == INVALID_HANDLE_VALUE)
			{
				CurrentFile=Settings.IncludeDirectory+Str;
#ifdef _DEBUG
				PP<<"Looking for "<<Settings.CurrentFile<<endl;
#endif
				Bos=CreateFile(CurrentFile,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					 OPEN_EXISTING,
					 (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
					 NULL);
				if (Bos == INVALID_HANDLE_VALUE)
				{
					CurrentFile=LastFile;
					DoError(ERT_ERROR,"Cannot Find "+Str);
					return RET_EXIT;
				}
#ifdef _DEBUG
				else PP<<"Found Include in IncDir"<<endl;
#endif
			}
#ifdef _DEBUG
			else PP<<"Found Include in CurDir"<<endl;
#endif
			if(OutputWin)
			{
				CString temp=Str;
				temp="Compiling "+temp;
				OutputWin(CurrentFile,temp,0,0);
			}
			Length=GetFileSize(Bos,0);
			Buf=new DATA[Length];
			ReadFile(Bos,Buf,Length,&BR,0);
			PreProc.InPreProcFlag=false;
			PreProc.LastWasInclude=false;
			BosInc.ParseTxt(Buf,Length,*this);
			CurrentFile=LastFile;
			CloseHandle(Bos);
			delete [] Buf;
		}
		else if(PreProc.LastWasIfType)
		{
			switch(PreProc.LastWasIfType)
			{
			case PP_ITYP_IFDEF:if(*PassedEOL)
							   {
									DoError(ERT_ERROR,"Expected if Condition");
									return RET_EXIT;
							   }
							   else
							   {
								   PreProc.InPreProcFlag=false;
								   PreProc.OKtoInsert=true;
									if(!(CompareTo(Str,TYP_CONST,&index) & TYP_CONST) )
									{
										return RET_SEEK_NEXT;
									}
									else
									{
										return RET_NOTSPECIAL;
									}
							   }
							   break;
			case PP_ITYP_IFNDEF:if(*PassedEOL)
							   {
									DoError(ERT_ERROR,"Expected if Condition");
									return RET_EXIT;
							   }
							   else
							   {
								   PreProc.InPreProcFlag=false;
								   PreProc.OKtoInsert=true;
									if((CompareTo(Str,TYP_CONST,&index) & TYP_CONST) )
									{
										return RET_SEEK_NEXT;
									}
									else
									{
										RET_NOTSPECIAL;
									}
							   }
							   break;
			case PP_ITYP_IF:if(*PassedEOL)
							{
								if(!PreProc.ValBuf) PreProc.Eval=0;
								else
								{
									PreProc.ValBuf->ReadyDump();
									PreProc.Eval=GetValBufValue(PreProc.ValBuf);
									delete PreProc.ValBuf;
									PreProc.ValBuf=0;
								}
								*LookingFor=0;
								PreProc.InPreProcFlag=false;
								PreProc.OKtoInsert=true;
								if(!PreProc.Eval)
								{
#ifdef _DEBUG
									PP<<"If evaluated false\n";
#endif
									if( !(*IdentTypeFlag & TYP_PREPROC) )
										Str="";
									return RET_SEEK_NEXT;
								}
								else
								{
#ifdef _DEBUG
									PP<<"If evaluated true\n";
#endif
									return ParseStr(Str,Arg1,Arg2);
								}
							}
							else
							{
								if(PreProc.NoMoreValues)
								{
									DoError(ERT_ERROR,"Unexpected Value "+Str);
									return RET_EXIT;
								}
								if(!PreProc.ValBuf)
								{
#ifdef _DEBUG
									PreProc.ValBuf=new CCobValBuf(vout);
#else
									PreProc.ValBuf=new CCobValBuf();
#endif
								}
								x=GetValBuf(Str,Arg1,Arg2,TYP_NUM,PreProc.ValBuf);
								if(x)
								{
									if(x==RET_EXIT) return RET_EXIT;
								}
								else
								{
									PreProc.NoMoreValues=true;
								}
							}// end if(EOL)
							break;
			}// end switch
		}
		else
		{
		}
	}
	else
	{
#ifdef _DEBUG
		PP<<"New command\n";
#endif
		if(CompareStr(Str,"define"))
		{
			PreProc.OKtoInsert=false;
			PreProc.InPreProcFlag=true;
			PreProc.LastWasDefine=true;
			PreProc.GotDEF=false;
			PreProc.CheckedForArgs=false;
			PreProc.GettingArgs=false;
		}
		else if(CompareStr(Str,"undef"))
		{
			PreProc.OKtoInsert=false;
			PreProc.InPreProcFlag=true;
			PreProc.LastWasUnDefine=true;
			PreProc.GotDEF=false;
		}
		else if(CompareStr(Str,"ifdef"))
		{
			PreProc.OKtoInsert=false;
			PreProc.InPreProcFlag=true;
			PreProc.GotDEF=false;
			PreProc.LastWasIfType=PP_ITYP_IFDEF;
		}
		else if(CompareStr(Str,"ifndef"))
		{
			PreProc.OKtoInsert=false;
			PreProc.InPreProcFlag=true;
			PreProc.GotDEF=false;
			PreProc.LastWasIfType=PP_ITYP_IFNDEF;
		}
		else if(CompareStr(Str,"if"))
		{
			PreProc.NoMoreValues=false;
			PreProc.OKtoInsert=true;
			PreProc.InPreProcFlag=true;
			PreProc.LastWasIfType=PP_ITYP_IF;
			*LookingFor=0;
		}
		else if(CompareStr(Str,"elif"))
		{
			return RET_SEEK_ENDIF;
		}
		else if(CompareStr(Str,"else"))
		{
			return RET_SEEK_ENDIF;
		}
		else if(CompareStr(Str,"endif"))
		{
			return RET_NOTSPECIAL;
		}
		else if(CompareStr(Str,"include"))
		{
			PreProc.OKtoInsert=true;
			PreProc.InPreProcFlag=true;
			PreProc.LastWasInclude=true;
		}
		else
		{
			DoError(ERT_ERROR,"Invalid PreProcessor command \'"+Str+"\'");
			return RET_EXIT;
		}
	}
	
	return RET_NOTSPECIAL;
}

WORD CBosCmdParse::HandleMacro(CString& Str,DWORD* Arg1,DWORD* Arg2)
{
	int index,x;
	CString temp;
	WORD* IdentTypeFlag=pLOWORD(Arg1);
	WORD* ExFlag1=pHIWORD(Arg1);
	BYTE* PassedEOL=pLOBYTE(pLOWORD(Arg2));
	BYTE* LookingFor=pHIBYTE(pLOWORD(Arg2));
	WORD* CharsFromLast=pHIWORD(Arg2);

	if(!PreProc.InMacroFlag)
	{
#ifdef _DEBUG
		PP<<"Starting Macro\n";
#endif
		MacDef=Constants[*ExFlag1];
		PreProc.InMacroFlag=true;
		PreProc.Keywords.SetSize(0);
		PreProc.GotOpen=false;
		PreProc.GotClose=false;
	}
	else
	{
		if(!PreProc.GotOpen)
		{
#ifdef _DEBUG
			PP<<"Getting '('\n";
#endif
			if( (Str=="(")&&(*CharsFromLast<=1) )
			{
				PreProc.GotOpen=true;
				PreProc.LastStr="";
			}
			else
			{
				DoError(ERT_ERROR,"Unknown Identifier "+MacDef.Name);
				return RET_EXIT;
			}
		}
		else if(!PreProc.GotClose)
		{
			if(Str==",")
			{
#ifdef _DEBUG
				PP<<"Got ','\n";
#endif
				PreProc.Keywords.Add(PreProc.LastStr);
				PreProc.LastStr="";
			}
			else if(Str==")")
			{
#ifdef _DEBUG
				PP<<"Got ')'\n";
#endif
				PreProc.Keywords.Add(PreProc.LastStr);
				if(PreProc.Keywords.GetSize()!=MacDef.Args)
				{
					DoError(ERT_ERROR,"Not enough parameters in macro "+MacDef.Name);
					return RET_EXIT;
				}
				for(x=0;x<PreProc.Keywords.GetSize();x++)
				{
#ifdef _DEBUG
					PP<<"Merging Params\n";
#endif
					itoa(x+1,temp.GetBuffer(2),10);
					temp.ReleaseBuffer();
					for(index=MacDef.Val.Find("@"+temp);index!=(-1);index=MacDef.Val.Find("@"+temp))
					{
						MacDef.Val=MacDef.Val.Left(index)+PreProc.Keywords[x]+MacDef.Val.Right(MacDef.Val.GetLength()-(index+2));
#ifdef _DEBUG
						PP<<"@"<<x+1<<" at index "<<index<<endl
						  <<MacDef.Val<<endl;
#endif
					}
				}
				Str=MacDef.Val;
				PreProc.InMacroFlag=false;
				return RET_INSERT;
			}
			else
			{
#ifdef _DEBUG
				PP<<"Adding "<<Str<<" of type "<<*IdentTypeFlag<<" to Val"<<endl;
#endif
				if(*IdentTypeFlag & TYP_STRING)
				{
#ifdef _DEBUG
					PP<<"Got String"<<endl;
#endif
					Str="\""+Str+"\"";
				}
				else if(*IdentTypeFlag & TYP_NUM)
				{
					if(*ExFlag1 & NTYP_ANGULAR)
					{
						Str="<"+Str+">";
					}
					else if(*ExFlag1 & NTYP_LINEAR)
					{
						Str="["+Str+"]";
					}
				}
#ifdef _DEBUG
				PP<<"Got "<<Str<<endl;
#endif
				PreProc.LastStr+=Str;
			}
		}
	}
	return RET_NOTSPECIAL;
}

WORD CBosCmdParse::StartCmdFmt(CString& Str,DWORD* Arg1,DWORD* Arg2)
{
	int index;
	WORD* IdentTypeFlag=pLOWORD(Arg1);
	SCRIPT Script;

	// Now we see what kind of string we have

	if(*IdentTypeFlag & TYP_IDENT)
	{
		// Find what kind of identifier it is
		*IdentTypeFlag=CompareTo(Str,TYP_IDENT,&index);

		if(*IdentTypeFlag & TYP_UNIQUE)
		{
			// This means that the identifier is undeclared
			// If our Locale is 0 then its probably a script
			// If not cause an error
			if(Locale==0)
			{
				if(CompareTo("@NewScript",TYP_CMD,&index) & TYP_CMD)
				{
					if(PreOp!=NONE)
					{
						DoError(ERT_ERROR,"Illegal PreOp");
						PreOp=NONE;
					}
#ifdef _DEBUG
					spin<<"Starting new script "<<Str<<endl;
#endif
					CurFmt=Commands[index].Format;
					CmdBuf=new CCobCmdBuf(true);
					Script.Name=Str;
					Script.Offset=ScriptCode->CurOffset;
					Scripts.Add(Script);
					return HandleCmdFmt(Str,Arg1,Arg2,false);
				}
				else
				{
					DoError(ERT_ERROR,"New Script command not found");
					return RET_EXIT;
				}
			}// end if(Locale==0)
			else
			{
				DoError(ERT_ERROR,"Unknown Identifier "+Str);
				return RET_NEXT_LINE;
			}
		}
		else if(*IdentTypeFlag & TYP_FVAR)
		{
			// Probably a pop, start appropriate command processing
			if(CompareTo("@PopFVar",TYP_CMD,&index) & TYP_CMD)
			{
				CurFmt=Commands[index].Format;
				CmdBuf=new CCobCmdBuf;
				return HandleCmdFmt(Str,Arg1,Arg2);
			}
			else
			{
				DoError(ERT_ERROR,"Pop Variable command not found");
				return RET_EXIT;
			}
		}
		else if(*IdentTypeFlag & TYP_SVAR)
		{
			// Probably a pop, start appropriate command processing
			if(CompareTo("@PopSVar",TYP_CMD,&index) & TYP_CMD)
			{
				CurFmt=Commands[index].Format;
				CmdBuf=new CCobCmdBuf;
				return HandleCmdFmt(Str,Arg1,Arg2);
			}
			else
			{
				DoError(ERT_ERROR,"Pop Variable command not found");
				return RET_EXIT;
			}
		}
		else if(*IdentTypeFlag & TYP_CMD)
		{
#ifdef _DEBUG
			spin<<"Starting new command "<<Str<<endl;
#endif
			// We found a command, so get the format pointer and process it
			// Unless the Locale isn't right
			if( (Commands[index].Flag & NOSCRIPT)&&(Locale>0) )
			{
				DoError(ERT_ERROR,Str+" command must be called outside of a script body");
				return RET_EXIT;
			}
			if( (Commands[index].Flag & REGULAR)&&(Locale==0) )
			{
				DoError(ERT_ERROR,Str+" command must be called inside of a script body");
				return RET_EXIT;
			}
			if( (Commands[index].Flag & NOTA)&&(Settings.TA) )
			{
				DoError(ERT_WARNING,Str+" command is not compatible with OTA");
			}
			if(Commands[index].Flag & RETURNS_VALUE)
			{
				Attach_Rider = true;
			}
			if(PreOp!=NONE)
			{
				DoError(ERT_ERROR,"Illegal PreOp");
				PreOp=NONE;
			}
			CurFmt=Commands[index].Format;
			CmdBuf=new CCobCmdBuf(Commands[index].Flag & NOCMD);
			return HandleCmdFmt(Str,Arg1,Arg2,false);
		}
		else
		{
			// We shouldn't get any of these so cause the proper error
			DoError(ERT_ERROR,"Unexpected Identifier "+Str);
		}
	}
	// Test for preIncrement and preDecrement
	else if(*IdentTypeFlag & TYP_OPERATOR)
	{
		if(Str=="++")
		{
			PreOp=INC;
		}
		else if(Str=="--")
		{
			PreOp=DEC;
		}
		else
		{
			PreOp=UNKNOWN;
		}
	}
	else if(*IdentTypeFlag & TYP_MISC)
	{
		if(Str == "{")
		{
			if(Locale==0)
			{
				DoError(ERT_ERROR,"Script Declaration Expected");
				return RET_EXIT;
			}
			Locale++;
		}
		else if(Str == "}")
		{
			if(Locale==0)
			{
				DoError(ERT_ERROR,"\'}\'Unexpected");
				return RET_EXIT;
			}
			if(Locale==1)
			{
				if(ScriptCode->ScriptBuf[ScriptCode->CurOffset-1] != CMD_RETURN)
				{
					CmdBuf=new CCobCmdBuf;
					CmdBuf->AddItem(PUT_CMD,CMD_RETURN);
					if(Settings.TA)
					{
						CmdBuf->AddItem(PRE_CMD|TWOVALS,CMD_PUSH_CON,0);
					}
					else
					{
						CmdBuf->AddItem(PRE_CMD,CMD_STACK_ALLOCATE);
					}
					ScriptCode->AddCmd(CmdBuf);
					if(CmdBuf) delete CmdBuf;
				}
				FuncVars.RemoveAll();
			}// end if Locale == 1
			Locale--;
			for(int x=Delays.GetUpperBound();x>=0;x--)
			{
				if(Delays[x].Locale==Locale)
				{
					if(CompareTo(Delays[x].Command,TYP_CMD,&index) & TYP_CMD)
					{
#ifdef _DEBUG
						spin<<"Calling "<<Delays[x].Command<<" from StartCmdFmt(})\n";
#endif
						CurFmt=Commands[index].Format;
						CmdBuf=new CCobCmdBuf(Commands[index].Flag & NOCMD);
					}
					Delays.RemoveAt(x);
					return HandleCmdFmt(Str,Arg1,Arg2,false);
				}
			}// end for
		}
		else if(Str == ";")
		{
			for(int x=Delays.GetUpperBound();x>=0;x--)
			{
				if(Delays[x].Locale==Locale)
				{
					if(CompareTo(Delays[x].Command,TYP_CMD,&index) & TYP_CMD)
					{
#ifdef _DEBUG
						spin<<"Calling "<<Delays[x].Command<<" from StartCmdFmt(;)\n";
#endif
						CurFmt=Commands[index].Format;
						CmdBuf=new CCobCmdBuf(Commands[index].Flag & NOCMD);
					}
					Delays.RemoveAt(x);
					return HandleCmdFmt(Str,Arg1,Arg2,false);
				}
			}// end for
		}
	}
	// Everything past this point should produce and error
	// or a warning because it should not be there
	else
	{
		DoError(ERT_ERROR,"Unexpected item "+Str);
	}

	return RET_NOTSPECIAL;
}

int PushVal(WORD type,long index,long &Val1,long &Val2)
{
	if(type & TYP_FVAR)
	{
		Val1=CMD_PUSH_FVAR;
		Val2=index;
		return 1;
	}
	else if(type & TYP_SVAR)
	{
		Val1=CMD_PUSH_SVAR;
		Val2=index;
		return 1;
	}
	else if(type & TYP_FVAR)
	{
		Val1=CMD_PUSH_FVAR;
		Val2=index;
		return 1;
	}
	else if(type & TYP_PIECE)
	{
		Val1=CMD_PUSH_CON;
		Val2=index;
		return 1;
	}
	return 0;
}

long CBosCmdParse::GetNumber(CString Str,WORD Flag)
{
	if(Flag & NTYP_FLOAT)
	{
		if(Flag & NTYP_ANGULAR)
		{
			return ((long)(atof(Str)*(Settings.AngularConst)));
		}
		else if(Flag & NTYP_LINEAR)
		{
			return ((long)(atof(Str)*(Settings.LinearConst)));
		}
		else 
		{
			return ((long)atof(Str));
		}
	}
	else
	{
		if(Flag & NTYP_ANGULAR)
		{
			return ((long)(strtol(Str,0,0)*(Settings.AngularConst)));
		}
		else if(Flag & NTYP_LINEAR)
		{
			return ((long)(strtol(Str,0,0)*(Settings.LinearConst)));
		}
		else
		{
			return strtol(Str,0,0);
		}
	}
}

WORD CBosCmdParse::GetValBuf(CString& Str,DWORD* Arg1,DWORD* Arg2,WORD Type,CCobValBuf* &ValBuff)
{
	int index;
	WORD* IdentTypeFlag=pLOWORD(Arg1);
	WORD* ExFlag1=pHIWORD(Arg1);
	BYTE* PassedEOL=pLOBYTE(pLOWORD(Arg2));
	BYTE* LookingFor=pHIBYTE(pLOWORD(Arg2));
	WORD* CharsFromLast=pHIWORD(Arg2);
	long Val=0;
	long Val2=0;
	WORD strType;

#define WANT_OPERATOR (*LookingFor)

	if(WANT_OPERATOR) // Are we looking for an operator
	{
		if(*IdentTypeFlag & TYP_OPERATOR)
		{
			// Check to see if the operator is defined
			if(CompareTo(Str,TYP_OPERATOR,&index) & TYP_OPERATOR)
			{
				// Check to make sure its not an operator like !
				if(!OpList[index].LeftOnly)
				{
					ValBuff->AddItem(TYP_OPERATOR,OpList[index].Val,OpList[index].Priority);
				}
				else
				{
					DoError(ERT_ERROR,"Inproper Operator placement");
					return RET_EXIT;
				}
			}// end CompareTo
			else // Operator not defined
			{
				DoError(ERT_ERROR,"Unknown Operator");
				return RET_EXIT;
			}
			WANT_OPERATOR=false;
		} // if(isOperator)
		// Str might still be an operator like NOT or AND
		// If its not the ValString is over
		else if(*IdentTypeFlag & TYP_IDENT)
		{
			// Check to see if Str is in the OpList
			if(CompareTo(Str,TYP_OPERATOR,&index) & TYP_OPERATOR)
			{
				if(!OpList[index].LeftOnly)
				{
					ValBuff->AddItem(TYP_OPERATOR,OpList[index].Val,OpList[index].Priority);
				}
				else
				{
					DoError(ERT_ERROR,"Inproper operator placement");
					return RET_EXIT;
				}
				WANT_OPERATOR=false;
			}// end CompareTo
			else
			{
				WANT_OPERATOR=false;
				return 0;
			}
		} // end if(isIdentifier)
		else if(*IdentTypeFlag & TYP_MISC)
		{
			if(Str==")")
			{
				if(!ValBuff->AddItem(TYP_MISC,2,0))
				{
					WANT_OPERATOR=false;
					return 0;
				}
			}
			else
			{
				WANT_OPERATOR=false;
				return 0;
			}
		}// end if(isMisc)
		else
		{
			WANT_OPERATOR=false;
			return 0;
		}
	}// end if(LookingFor)
	// We are looking for and Value of Type
	else // if !LookingFor
	{
		// Check to see if we have a left only Op like NOT
		if(*IdentTypeFlag & TYP_OPERATOR)
		{
			// Check to see if the operator is defined
			if(CompareTo(Str,TYP_OPERATOR,&index) & TYP_OPERATOR)
			{
				// Make Sure its a Left Only Op
				if(OpList[index].LeftOnly)
				{
					ValBuff->AddItem(TYP_OPERATOR|WAIT,OpList[index].Val,OpList[index].Priority);
				}
				else
				{
					DoError(ERT_ERROR,"Expected identifier");
				}
			}
			else // Operator not defined
			{
				DoError(ERT_ERROR,"Unknown operator");
			}
		}//end if(isOperator)
		// Get the Type of Identifier and push it
		else if(*IdentTypeFlag & TYP_IDENT)
		{
			// Str might still be an operator like NOT or AND
			if(CompareTo(Str,TYP_OPERATOR,&index) & TYP_OPERATOR)
			{
				if(OpList[index].LeftOnly)
				{
					ValBuff->AddItem(TYP_OPERATOR|WAIT,OpList[index].Val,OpList[index].Priority);
				}
				else
				{
					DoError(ERT_ERROR,"Identifier expected");
				}
			}//end CompareTo(Operator)
			else if(CompareTo(Str,TYP_CMD,&index) & TYP_CMD)
			{
				if(Commands[index].Flag!=RETURNS_VALUE)
				{
					DoError(ERT_ERROR,Str+" Command does not return a value");
					return RET_EXIT;
				}
#ifdef _DEBUG
				spin<<"Starting Value Command "<<Str<<endl;
#endif
				BURNER_ITEM Burn;
				Burn.Fmt=CurFmt;
				Burn.CmdBuf=CmdBuf;
				Burn.ValBuf=ValBuff;
				BackBurner.Add(Burn);
				CurFmt=Commands[index].Format;
				ValBuff=0;
				CmdBuf=new CCobCmdBuf(Commands[index].Flag & NOCMD);
			}// end CompareTo(Cmd)
			else 
			{
				strType=CompareTo(Str,*IdentTypeFlag,&index);
				if(strType & TYP_UNIQUE)
				{
					if(CurFmt->Error)
						DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,""));
					else
						DoError(ERT_ERROR,"Unknown identifier "+Str);
					KillCmd();
					return RET_NEXT_LINE;
				}
				else if(!(Type & strType))
				{
					if(CurFmt->Error)
						DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,""));
					else
						DoError(ERT_ERROR,"Inproper Value Type "+Str);
					KillCmd();
					return RET_NEXT_LINE;
				}
				else if(!PushVal(strType,index,Val,Val2))
				{
					if(CurFmt->Error)
						DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,""));
					else
						DoError(ERT_ERROR,"Inproper Value Type "+Str);
					KillCmd();
					return RET_NEXT_LINE;
				}
				else ValBuff->AddItem(0,Val,Val2);
				WANT_OPERATOR=true;
			}// end else
		}//end if(isIdentifier)
		// We got a number so push it
		else if(*IdentTypeFlag & TYP_NUM)
		{
			ValBuff->AddItem(0,CMD_PUSH_CON,GetNumber(Str,*ExFlag1));
			WANT_OPERATOR=true;
		}
		// Could be a '(' or ')' so we'll check
		else if(*IdentTypeFlag & TYP_MISC)
		{
			if(Str=="(") ValBuff->AddItem(TYP_MISC,1,0);
			else
			{
				DoError(ERT_ERROR,"Unexpected Char");
			}
		}
		else
		{
			DoError(ERT_ERROR,"Expected Value");
			return RET_EXIT;
		}
	}// end !LookinFor

#undef WANT_OPERATOR

	return 1;
}

CString CBosCmdParse::GetFmtErr(WORD ErrNum,CString Str,CString Fmt)
{
	ASSERT(ErrNum);
	CString Ret = Errors[ErrNum];
	for(int pos = Ret.Find("%%");pos>=0;pos = Ret.Find("%%") )
	{
		Ret = Ret.Left(pos) + "=" + Ret.Right( Ret.GetLength()-(pos+2) );
	}
	for(pos = Ret.Find("%S");pos>=0;pos = Ret.Find("%S") )
	{
		Ret = Ret.Left(pos) + Str + Ret.Right( Ret.GetLength()-(pos+2) );
	}
	for(pos = Ret.Find("%F");pos>=0;pos = Ret.Find("%F") )
	{
		Ret = Ret.Left(pos) + Fmt + Ret.Right( Ret.GetLength()-(pos+2) );
	}

	return Ret;
}

WORD CBosCmdParse::HandleCmdFmt(CString& Str,DWORD* Arg1,DWORD* Arg2,bool Fresh)
{
	int index;
	WORD* IdentTypeFlag=pLOWORD(Arg1);
	WORD* ExFlag1=pHIWORD(Arg1);
	BYTE* PassedEOL=pLOBYTE(pLOWORD(Arg2));
	BYTE* LookingFor=pHIBYTE(pLOWORD(Arg2));
	WORD* CharsFromLast=pHIWORD(Arg2);
	CMD_FMT* tempFmt=0;
	long Val=0;
	long Val2=0;
	WORD strType;
	bool OKtoAdvance=false;

	// Spin on foramt until we need another Str or its the end
	for(;;)
	{	
#ifdef _DEBUG
		spin<<"Spinning on "<<Str<<endl;
#endif
		// First we process the Expecting Flags

		// If we have a type flag, compare it with what we got
		if(CurFmt->Expecting==CMD_VAL)
		{
			if(CurFmt->CmdBufFlg & PRE_CMD)
			{
				CurFmt->CmdBufFlg|=TWOVALS;
				Val=CMD_PUSH_CON;
				Val2=CurFmt->Val;
			}
			else
			{
				Val=CurFmt->Val;
			}
			OKtoAdvance=true;
		}
		else if(CurFmt->Expecting==NOTHING)
		{
			OKtoAdvance=true;
		}
		else if(CurFmt->Expecting==NEW_STR)
		{
			CString temp;
			temp="";
			Strs.Add(temp);
#ifdef _DEBUG
			spin<<"Adding new Str at ListPos "<<Strs.GetUpperBound()<<endl;
#endif
			OKtoAdvance=true;
		}
		else if(CurFmt->Expecting==FMT_STR)
		{
			if(!Fresh) break;
#ifdef _DEBUG
			spin<<"Comparing "<<Str<<" with FMT_STR "<<CurFmt->Fmt<<endl;
#endif
			if(CompareStr(Str,CurFmt->Fmt))
			{
				Fresh=false;
			}
			else
			{
				if(CurFmt->Error)
					DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,CurFmt->Fmt));
				else
					DoError(ERT_ERROR,"Expected command format word \""+CurFmt->Fmt+"\"");
				KillCmd();
				return RET_NEXT_LINE;
			}
			OKtoAdvance=true;
		}
		else if(CurFmt->Expecting==BRAK_OPEN)
		{
			if(!Fresh) break;
#ifdef _DEBUG
			spin<<"Comparing "<<Str<<" with FMT_STR {"<<endl;
#endif
			if(CompareStr(Str,"{"))
			{
				Fresh=false;
			}
			else
			{
				DoError(ERT_ERROR,"\'{\' Expected");
			}
			OKtoAdvance=true;
		}
		else if(CurFmt->Expecting==EXP_VARIES)
		{
			if(!Fresh) break;
			tempFmt=CurFmt->Next;
			Val=1;
#ifdef _DEBUG
			spin<<"Comparing "<<Str<<" with FMT_STR "<<tempFmt->Fmt<<endl;
#endif
			while(!CompareStr(Str,tempFmt->Fmt))
			{
#ifdef _DEBUG
				spin<<"Moving to next VARI"<<endl;
#endif
                for(int x=0;tempFmt&&(tempFmt->Expecting!=NEXT_VARI);tempFmt=tempFmt->Next)
					if(tempFmt->Expecting==EXP_VARIES)
					{
#ifdef _DEBUG
						spin<<"Incrementing Vari Locale"<<endl;
#endif
						x++;
					}
					else if(tempFmt->Expecting==END_VARI)
					{
						if(x)
						{
#ifdef _DEBUG
							spin<<"Decrementing Vari Locale"<<endl;
#endif
							x--;
						}
						else
						{
#ifdef _DEBUG
							spin<<"Ending VARI loop"<<endl;
#endif
							Val=0;
							break;
						}
					}
					else if(tempFmt->Expecting==EXP_ELSE)
					{
						if(x)
						{
#ifdef _DEBUG
							spin<<"Vari Locale not 0"<<endl;
#endif
						}
						else
						{
#ifdef _DEBUG
							spin<<"Got to Else, moving on from here"<<endl;
#endif
							Val=2;
							break;
						}
					}
				if(tempFmt) tempFmt=tempFmt->Next;
				if(!tempFmt)
				{
					Val=0;
					break;
				}
				if(Val==2) break;
			}
			if(Val)
			{
				CurFmt=tempFmt;
				continue;
			}
			else
			{
				if(CurFmt->Error)
					DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,CurFmt->Fmt));
				else
					DoError(ERT_ERROR,"Expected command format word \""+CurFmt->Fmt+"\"");
				KillCmd();
				return RET_NEXT_LINE;
			}
		}
		else if(CurFmt->Expecting==NEXT_VARI)
		{
#ifdef _DEBUG
			spin<<"Found stray NEXT_VARI. Moving to next END_VARI\n";
#endif
			tempFmt=CurFmt->Next;
			for(;tempFmt&&(tempFmt->Expecting!=END_VARI);tempFmt=tempFmt->Next);
			CurFmt=tempFmt->Next;
			continue;
		}
		else if(CurFmt->Expecting==EXP_ELSE)
		{
#ifdef _DEBUG
			spin<<"Found stray EXP_ELSE. Moving to next END_VARI\n";
#endif
			tempFmt=CurFmt->Next;
			for(;tempFmt&&(tempFmt->Expecting!=END_VARI);tempFmt=tempFmt->Next);
			CurFmt=tempFmt->Next;
			continue;
		}
		else if(CurFmt->Expecting==END_VARI)
		{
#ifdef _DEBUG
			spin<<"Found stray END_VARI. Skipping it\n";
#endif
			CurFmt=CurFmt->Next;
			continue;
		}
		else if(CurFmt->Expecting==SINGLE)
		{
			if(!Fresh) break;
			// If we eant a unique ident make sure its unique
			if(CurFmt->Type == TYP_UNIQUE)
			{
				strType=CompareTo(Str,*IdentTypeFlag,&index);
				if(!(strType & TYP_UNIQUE))
				{
					if(CurFmt->Error)
						DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,CurFmt->Fmt));
					else
					{
						if(strType & TYP_NUM)
							DoError(ERT_ERROR,"Value unexpected");
						else
							DoError(ERT_ERROR,"Duplicate Identifier "+Str);
					}
					KillCmd();
					return RET_NEXT_LINE;
				}
			}
			else if(CurFmt->Type == TYP_STRING)
			{
				if(!(*IdentTypeFlag & TYP_STRING))
				{
					if(CurFmt->Error)
						DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,CurFmt->Fmt));
					else
					{
						DoError(ERT_ERROR,"String Value unexpected");
					}
					KillCmd();
					return RET_NEXT_LINE;
				}
			}
			else
			{
				// Check to see if what we got is what we want
				strType=CompareTo(Str,*IdentTypeFlag,&index);
				if(strType & TYP_UNIQUE)
				{
					DoError(ERT_ERROR,"Unknown identifier "+Str);
					KillCmd();
					return RET_NEXT_LINE;
				}
				if(CurFmt->Type & strType)
				{
					if(CurFmt->CmdBufFlg & TWOVALS)
					{
						if(strType==TYP_NUM)
						{
							Val=CMD_PUSH_CON;
							Val2=GetNumber(Str,*ExFlag1);
						}
						else if(!PushVal(strType,index,Val,Val2))
						{
							if(CurFmt->Error)
								DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,CurFmt->Fmt));
							else
								DoError(ERT_ERROR,Str+" is not the correct type");
							KillCmd();
							return RET_NEXT_LINE;
						}
					}
					else
					{
						Val=index;
					}
				}
				else
				{
					if(CurFmt->Error)
						DoError(ERT_ERROR,GetFmtErr(CurFmt->Error,Str,CurFmt->Fmt));
					else
					DoError(ERT_ERROR,Str+" is not the correct type");
					KillCmd();
					return RET_NEXT_LINE;
				}
			}
			OKtoAdvance=true;
			Fresh=false;
		}
		else if(CurFmt->Expecting==VALSTR)
		{
			if(!ValBuf)
			{
#ifdef _DEBUG
				ValBuf=new CCobValBuf(vout);
#else
				ValBuf=new CCobValBuf();
#endif
			}
			if(!Fresh) break;
			Val=GetValBuf(Str,Arg1,Arg2,CurFmt->Type,ValBuf);
			if(Val)
			{
				if(Val==1) break;
				else return RET_EXIT;
			}
			else OKtoAdvance=true;
		}

		// Now what do we do with what we got

		if(CurFmt->WhatToDo==ADDTOCMDBUF)
		{
			if(CurFmt->Expecting==VALSTR)
			{
				if(!ValBuf->CanDump())
				{
					DoError(ERT_ERROR,"\')\' expected");
					return RET_EXIT;
				}
				if(ValBuf->GetLength()==0)
				{
					DoError(ERT_WARNING,"Expected Value");
					ValBuf->AddItem(0,CMD_PUSH_CON,0);
				}
				ValBuf->ReadyDump();
#ifdef _DEBUG
				spin<<"Adding ValBuf\n";
#endif
				CmdBuf->AddItem(CurFmt->CmdBufFlg,ValBuf);
				if(ValBuf) delete ValBuf;
				ValBuf=0;
			}
			else
			{
#ifdef _DEBUG
				spin<<"Adding Single or Cmd\n";
#endif
				CmdBuf->AddItem(CurFmt->CmdBufFlg,Val,Val2);
			}
		}
		else if(CurFmt->WhatToDo==NODUMPVALBUF)
		{
			if(!ValBuf->CanDump())
			{
				DoError(ERT_ERROR,"\')\' expected");
				return RET_EXIT;
			}
			ValBuf->ReadyDump();
#ifdef _DEBUG
			spin<<"Not Adding ValBuf\n";
#endif
		}
		else if(CurFmt->WhatToDo==ADDTOVALBUF)
		{
#ifdef _DEBUG
			if(!ValBuf) ValBuf = new CCobValBuf(vout);
#else
			if(!ValBuf) ValBuf = new CCobValBuf();
#endif
			ValBuf->AddItem(CurFmt->Type,Val,Val2);
		}
		else if(CurFmt->WhatToDo==ADDFVAR)
		{
			if(TYP_IDENT & *IdentTypeFlag)
			{
				CmdBuf->AddItem(PRE_CMD|SPOT_END,CurFmt->Val);
				FuncVars.Add(Str);
			}
			Fresh=false;
		}
		else if(CurFmt->WhatToDo==ADDSVAR)
		{
			if(TYP_IDENT & *IdentTypeFlag)
				StaticVars.Add(Str);
			Fresh=false;
		}
		else if(CurFmt->WhatToDo==ADDSOUND)
		{
			if( (CurFmt->CmdBufFlg & POST_CMD)&&(*IdentTypeFlag & TYP_STRING) )
			{
				if(CompareTo(Str,TYP_SOUND,&index) & TYP_SOUND)
					CmdBuf->AddItem(POST_CMD,index);
				else
				{
					Sounds.Add(Str);
					CmdBuf->AddItem(POST_CMD,Sounds.GetUpperBound());
				}
			}
			else if(TYP_IDENT & *IdentTypeFlag)
				Sounds.Add(Str);
			Fresh=false;
		}
		else if(CurFmt->WhatToDo==ADDPIECE)
		{
			if(TYP_IDENT & *IdentTypeFlag)
				Pieces.Add(Str);
			Fresh=false;
		}
		else if(CurFmt->WhatToDo==GET_STRS)
		{
			if(!Fresh) break;
#ifdef _DEBUG
			spin<<"Getting Str "<<Str<<endl;
#endif
			if(Str != CurFmt->Fmt)
			{
				Strs[Strs.GetUpperBound()]+=(Str)+" ";
				break;
			}
			Fresh=false;
		}
		else if(CurFmt->WhatToDo==FORCE_INSERT)
		{
#ifdef _DEBUG
			spin<<"Forcing insert ";
#endif
			index = Strs.GetUpperBound();
			ASSERT(index!=(-1));
			if(CurFmt->Val && (Strs[index]!="") )
				Strs[index]+=CurFmt->Fmt;
			Str = Strs[index];
			Strs.RemoveAt(index);		
#ifdef _DEBUG
			spin<<Str<<endl;
			spin<<"Finished Cmd on insert\n";
#endif
			if (Str!="")
			{
				ScriptCode->AddCmd(CmdBuf);
				if(CmdBuf) delete CmdBuf;
				CmdBuf=0;
				CurFmt=0;
				LastFmt=0;
				return RET_INSERT;
			}
			//return RET_NOTSPECIAL;
		}
		else if(CurFmt->WhatToDo==BACK)
		{
#ifdef _DEBUG
			spin<<"Moving Back\n";
#endif
			if(LastFmt) CurFmt=LastFmt;
			continue;
		}
		else if(CurFmt->WhatToDo==SAVELAST)
		{
#ifdef _DEBUG
			spin<<"Saving Last\n";
#endif
			if(LastFmt) SaveFmt=LastFmt;
		}
		else if(CurFmt->WhatToDo==GOTOSAVE)
		{
#ifdef _DEBUG
			spin<<"Going Back to Save\n";
#endif
			if(SaveFmt) CurFmt=SaveFmt;
			continue;
		}
		else if(CurFmt->WhatToDo==DO_PREOP)
		{
#ifdef _DEBUG
			spin<<"Handling PreOp\n";
#endif
			if(CmdBuf->Buffer[CmdBuf->GetLength()-2]==CMD_POP_FVAR)
				Val=CMD_PUSH_FVAR;
			else if(CmdBuf->Buffer[CmdBuf->GetLength()-2]==CMD_POP_SVAR)
				Val=CMD_PUSH_SVAR;
			elseVVal=0;
		VVal2=CmdBuf->Buffer[CmdBuf->GetLength()g1];
			switch(PrOp")
			{
			caseINC:	CmdBuf->AddItem(PRE_CMD|TWOVALS,Val,Val2);
				 	CmdBuf->AddItem(PRE_CMD|TWOVALS,CMD_PUSH_CONg1);
				e if(f(CompareTo(+"r,TYP_OPERATOR,&index) & TYP_OPERATORD)
				  {
					 	DoError(ERT_ERROR, Addation operatormiessine");
					) delete CmdBuf;
					)	break;
				  }
				 	CmdBuf->AddItem(PRE_CMT,OpList[index].Va1);
				e break;
			caseDEC:	CmdBuf->AddItem(PRE_CMD|TWOVALS,Val,Val2);
				 	CmdBuf->AddItem(PRE_CMD|TWOVALS,CMD_PUSH_CONg1);
				e if(f(CompareTo(-"r,TYP_OPERATOR,&index) & TYP_OPERATORD)
				  {
					 	DoError(ERT_ERROR, Addation operatormiessine");
					) delete CmdBuf;
					)	break;
				  }
				 	CmdBuf->AddItem(PRE_CMT,OpList[index].Va1);
				e break;
		fdeault:) delete CmdBuf;
		 }
			PreOp=NONE;
		}
		else if(CurFmt->WhatToDo=KILLO_PREOP)
		{
#ifdef _DEBUG
			spin<<	Killing PreOp\n";
#endif
			PreOp=NONE;
		}
		else if(CurFmt->WhatToDo=INC_LLOCLVE)
		{
#ifdef _DEBUG
			spin<<"Incrementing Localp\n";
#endif
			Locale++;
		}
		else if(CurFmt->WhatToDo=INITCSOUST)
		{
#ifdef _DEBUG
			spin<<IndataliztingCBoupt "<CBoupr<<endl;
#endif
		CBoupr=0;
		}
		else if(CurFmt->WhatToDo=CSOUST)
		{
		CBoupe++;
#ifdef _DEBUG
			spin<<"Adding toCBoupt "<CBoupr<<endl;
#endif
		}
		else if(CurFmt->WhatToDo=PLACECSOUST)
		{
#ifdef _DEBUG
			spin<<"AddingCBoupt todBu "<CBoupr<<endl;
#endif
		CCmdBuf->AddItem(CurFmt->CmdBufFlgCBoup0,0);
		}
		else if(CurFmt->WhatToDo==AD_HOLND)
		{
			int ,yf;
		HOLN Holdf;
		Hold].Fla+=CurFmt-rType;
			if( (CurFmt->Type &H TYPPLACEHOLND)||(CurFmt->Type &H TYP_ELSHOLND) )
			{
#ifdef _DEBUG
				spin<<"AddingPplac/EelseHoldd\n";
#endif
			Hold].Offset=ScriptCode->CurOffse+[CmdBuf->GetLength();
				CmdBuf->AddItem(POST_CMD,0,0);
			}
			else if(CurFmt->Type &H TYPCUROFFSET )
			{
#ifdef _DEBUG
				spin<<"AddingrOffseeHoldd\n";
#endif
			Hold].Offset=ScriptCode->CurOffse);
			}
		 if(CurFmt->Type &H TYPELASLOEOP)
			{
				forx=Holdrs.GetUpperBound(,y>=0;x>=0;x--)
				{
				 ifHoldr[ex].Flag &H TYPEOEOP)
					{
					Hold].LocaleHoldr[ex].Local;{
					yl=1;
						break;
				 }
			 }
			 if(ly)
				{
					DoError(ERT_ERROR,"lleaegal "+Str);
				}
			}
			els)
			{
			Hold].Locale.Local;{
		};
		Holdes.AddHold0);
		}
		else if(CurFmt->WhatToDo=FILLOHOLNSD)
		{
			int ,yf;
		 if(CurFmt->Type &H TYPELASLOEOP)
			{
				forx=Holdrs.GetUpperBound(,y>=0;x>=0;x--)
				{
				 if fHoldr[ex].Flag &H TYPEOEOP&&fHoldr[ex].Flag &H TYPCUROFFSET  P)
					{
#ifdef _DEBUG
						spin<<FKillingLoopgrOffseeHoldd\n";
#endif
						CmdBuf->AddItem(POST_CMDHoldr[ex]rOffseN,0);
					yl=1;
						break;
				 }
			 }
			 if(ly)
				{
					DoError(ERT_ERROR,"lleaegal "+Str);
				}
			}
			els)
			{
			 for(int xHoldrs.GetUpperBound(0;x>=0;x--)
				{
				 if fHoldr[ex].Locale==Locale&&fHoldr[ex].Flag &(CurFmt->Typ  P)
					{	
				 if fHoldr[ex].Flag &H TYPPLACEHOLND)||Holdr[ex].Flag &H TYP_ELSHOLND) )
						{
#ifdef _DEBUG
							spin<<FKillingPplac/EelseHoldd\n";
#endif
							ScriptCode->ScriptBufHoldr[ex]rOffse]t=ScriptCode->CurOffse+[CmdBuf->GetLength();
						Holdes.RemoveAt(x);
					 }
			
		else ifHoldr[ex].Flag &H TYPCUROFFSET )
						{
#ifdef _DEBUG
							spin<<FKillingrOffseeHoldd\n";
#endif
							CmdBuf->AddItem(POST_CMDHoldr[ex]rOffseN,0);
						Holdes.RemoveAt(x);
					 }
			
	}// end(=Locale)
				}// end for
			}// end else
		e
		else if(CurFmt->WhatToDo==VELY_OCLAL)
		{
#ifdef _DEBUG
			spin<<	DeladingCallt to "<<CurFmt->Fmt<<endl;
#endif
		=VELY 	Dela);
			Dela].Comman+=CurFmt->Fmt;
			Dela].Locale.Local;{
			Dela].Fla+0;{
			Delaes.Add	Dela0);
		}
		else if(CurFmt->WhatToDo= ENDNOCSOUST)
		{
#ifdef _DEBUG
			spin<< CommandfFinishedion\' "<<CurFmt->Fmt<"\('\n";
#endif
			if	Attach_Ride&&(CmdBuff
			{	
			CmdBuf->AddItem(POST_CMD|SPOT_END0x1002400,0);
				Attach_Rideh=false;
			}
			ScriptCode->AddCmd(CmdBuf);
		 if(CmdBuf) delete CmdBuf;
			CmdBuf=0;
			CurFmt=0;
			LastFmt=0;
		 if!Fresh)/return StartCmdFmt(Str,Arg1,Argf);
			break;
		}
		else if(CurFmt->WhatToDo= ENT)
		{
#ifdef _DEBUG
			spin<< CommandfFinishedion\' "<<CurFmt->Fmt<"\('\n";
#endif
			ifVValBuf)
			{					if(!ValBuf->CanDump())
				{
					DoError(ERT_ERROR,"\')\' expected");
					return RET_EXIT;
				}
				if(ValBuf->GetLength()==0)
				{
					DoError(ERT_WARNING,"Expected Value");
					ValBuf->AddItem(0,CMD_PUSH_CON,0);
				}
				ValBuf->ReadyDump();
#ifdef _DEBUG
				spin<<"Adding ValBuf\n";
#endif
				CmdBuf->AddItem(PRE_CMD|SPOT_ENDVValBuf);
				if(ValBuf) delete ValBuf;
				ValBuf=0;
			}
			if	BackBurnerGGetSize(ff
			{	
			CurFmt	BackBurne[	BackBurnerGGetUpperBound().>Fmt;
				ValBuf	BackBurne[	BackBurnerGGetUpperBound(). ValBuf;
			 if(CmdBuf)
				{
					if(!ValBuP)
					{
#ifdef _DEBUG
						ValBuf=new CCobValBuf(vout);
#else
						ValBuf=new CCobValBuf)";
#endif
				 }
			
	ValBuf->AddItem(CmdBuf);
					delete CmdBuf;
					CmdBuf=0;
			 }
				CmdBuf	BackBurne[	BackBurnerGGetUpperBound(). CmdBuf;
				BackBurner.RemoveAt	BackBurnerGGetUpperBound(f);
				*LookingFor=1;
				continue;
			}
			if	Attach_Ride&&(CmdBuff
			{	
			CmdBuf->AddItem(POST_CMD|SPOT_END0x1002400,0);
				Attach_Rideh=false;
			}
			ScriptCode->AddCmd(CmdBuf);
		 if(CmdBuf) delete CmdBuf;
			CmdBuf=0;
			CurFmt=0;
			LastFmt=0;
		VVal=0;
			for(int x=Delays.GetUpperBound();x>=0;x--)
			{
				if(Delays[x].Locale==Locale)
				{
					if(CompareTo(Delays[x].Command,TYP_CMD,&index) & TYP_CMD)
					{
#ifdef _DEBUG
						spin<<"Calling "<(Commands[index]CCommand<<" from	spi cycalp\n";
#endif
						Val=1;
						CurFmt=Commands[index].Format;
						CmdBuf=new CCobCmdBuf(Commands[index].Flag & NOCMD);
				 }
			
	Delays.RemoveAt(x);
					break;
				}
			}
			if.Va1 	continue;
		 if!Fresh)/return StartCmdFmt(Str,Arg1,Argf);
			break;
		}
		else if(CurFmt->WhatToDo= EN RE_CMD)
	{;
		 if	BackBurnerGGetSize(<=1)
			{
				DoError(ERT_ERROR, Command mustrreturn a value");
				return RET_EXIT;
			}
			CurFmt	BackBurne[	BackBurnerGGetUpperBound().>Fmt;
			if(!ValBuf->CanDump())
			{
				DoError(ERT_ERROR,"\')\' expected");
				return RET_EXIT;
			}
			ValBuf->ReadyDump();				BackBurne[	BackBurnerGGetUpperBound(). ValBuf->AddItemVValBuf);
		 delete ValBuf;
			ValBuf	BackBurne[	BackBurnerGGetUpperBound(). ValBuf;
			BackBurner.RemoveAt	BackBurnerGGetUpperBound(f);
			*LookingFor=1;
			continue;
		}
		// movt to the nextptart of th .Forma  if weparerRead}
		if OKtoAdvancT)
		{
#ifdef _DEBUG
			spin<<oAdvankinp\n";
#endif
			LastFmt	CurFm;}
			CurFmt(CurFmt->Next;
		}
}}

	return RET_NOTSPECIAL;
}
voidD CBosCmdParse:	KillCmd()
{
	PreProc.OKtoInsert=true;
	PreProc.IPPreProcFlag=false;
	PreProc.InMacroFlag=false;
 if(CmdBuf) delete CmdBuf;
	CmdBuf=0;
	CurFmt=0;
	LastFmt=0;}}

WORD CBosCmdParse:dPars(Stt(CString& Str,DWORD* Arg1,DWORD* Arg2)
{
	int index;
 // ure flais
	WORD* IdentTypeFlag=pLOWORD(Arg1);
	WORD* ExFlag1=pHIWORD(Arg1);{
#ifdef _DEBUG
fvou"<<Str<<"-x "<<it&(*IdentTypeFla()<<endl;
#endif;
 //Mmake sure	Str should not beignoared
iif(*IdentTypeFlag & TYP_GNOREMCE)
	{
	/return RET_NOTSPECIAL;
}f;
 // If we found a comden, s end th  appropriate REd
iif f(*IdentTypeFlag & TYP_MISC)&&(*ExFlag1) )
	{
	iif(Str==//")e return RET_LINP_CNIT;
		else return RETBLCKP_CNIT;
}f;
 // If ther" isad PreProcessor flag,hHandlh it
iif (*IdentTypeFlag & TYP(PRPROC) )
	 return:Handl PreProcessot(Str,Arg1,Argf);t
iif 	PreProc.InMacroFla )e return:HandleMacro(Str,Arg1,Argf);t
	// Check to see if weencBouphernd addefine, sttrementtormMacrt
iif 	PreProc.OKtoInser &&)
	f(*IdentTypeFlag & TYP_IDENT &&)
	f(CompareTo(Str,TYP_COSTD,&index) & TYP_COST1) )
	{
		// eot Stt to theddefidatio{
	iif( (Constants[index].Flao=CSOST_BOSDEFD)||(Constants[index].Flao=CSOST_STDDEFD) )
		(Str(Constants[index]VVal;
	#else
		{
		(*ExFlagl=index;
		 return:HandleMacro(Str,Arg1,Argf);
		}
		//Tell dPars(rct tooInser  Stt<itto thestrRem{
	/return RET_INSERT;
}f;
 // If wepare nsad PreProcessor proces, 	continu.t
iif 	PreProc.IPPreProcFla) )
	 return:Handl PreProcessot(Str,Arg1,Argf);t
 // If wepare processind a command,	continut todto rt
iif	CurFm)e return:HandltCmdFmt(Str,Arg1,Argf);t
 //*Look to see what we have und start the ndatald processing
/return StartCmdFmt(Str,Arg1,Argf);t
 //wer should notgeotther, but jmustpi calse
	return RET_NOTSPECIAL;
