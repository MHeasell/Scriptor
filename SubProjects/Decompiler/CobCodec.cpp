#include "stdafx.h"
#include "DeCom_Exports.h"
#include "CobCodec.h"
#include "Defs.h"

void CCobCodec::InitStack()
{
	OpString = new OPSTRING[16];
	OpPos = 0;
}

void CCobCodec::KillStack()
{
	delete [] OpString;
}

int CCobCodec::Push( long Val, BYTE Type )
{
	return STACK.Push( &StackVal(Val,0,Type) );
}

int CCobCodec::NewOpString()
{
	for(int Index=0;(OpPos & (1<<Index));Index++)
		if(Index+1==16) return -1;
	OpPos |= (1<<Index);
	OpString[Index].Values.Reset();
	return Index;
}

void CCobCodec::KillOpString(int Index)
{
	OpPos ^= (1<<Index);
}

int CCobCodec::PushOp( long Operator )
{
	int Index = NewOpString();
	if( Index < 0 ) return Index;

	StackVal Val,Val2,Val3;
	OpString[Index].Operator = Operator;

	if( ((Operator >= OP_ADD ) && (Operator <= OP_RANDOM)) ||
		((Operator >= OP_LESS) && (Operator <= OP_OR    )) )
	{
		STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
		STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
	}
	else switch( Operator )
	{
		case OP_GET_UNITVAL:
			STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
			break;
		case OP_GET:
			STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
			STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
			STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
			STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
			STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
			break;
		case OP_NOT:
			STACK.Pop( &Val ); OpString[Index].Values.Push( &Val );
			break;
		case CMD_PLAY_SOUND:
			STACK.Pop( &Val2);
			STACK.Pop( &Val ); 
			OpString[Index].Values.Push( &Val );
			OpString[Index].Values.Push( &Val2 );
			break;
		case CMD_UK_MAP:
			STACK.Pop( &Val2);
			STACK.Pop( &Val3);
			for(int x=0;x<Val2.Val;x++)
			{
				STACK.Pop( &Val );
				OpString[Index].Values.Push( &Val );
			}
			OpString[Index].Values.Push( &Val2 );
			OpString[Index].Values.Push( &Val3 );
			break;
	}
	Val.SetVal(Index,1,0); STACK.Push( &Val );
	return Index+1;
}

void CCobCodec::Pop( WORD Flags, int Back )
{
	StackVal Val;

	if( Back )
	{
		CStack TmpStack;
		StackVal tmp;
		for(int x=0;x<Back;x++)
		{
			STACK.Pop( &tmp );
			TmpStack.Push( &tmp );
		}
		STACK.Pop( &Val );
		for(x=0;x<Back;x++)
		{
			TmpStack.Pop( &tmp );
			STACK.Push( &tmp );
		}
	}
	else STACK.Pop( &Val );
	Pop( Val, Flags, 1 );
}

void CCobCodec::Pop( StackVal Val, WORD Flags, int First )
{
	long Index;
	long Operator;
	int NeedPara = 0;
	StackVal newVal;
#define POPNEWVAL	if( newVal.IsOpString() && \
						(GetOpPriority(OpString[newVal.Val].Operator) < GetOpPriority(OpString[Index].Operator)) )\
					{ \
						NeedPara = TRUE; \
						if(!(Flags & FLG_NOWRITE)) fout<<"( "; \
					} \
					Pop( newVal, Flags ); \
					if(NeedPara) \
					{ \
						if(!(Flags & FLG_NOWRITE)) fout<<" )"; \
						NeedPara = FALSE; \
					}

	if( Val.IsOpString() )
	{
		Index = Val.Val;
		Operator = OpString[Index].Operator;
		if( ((Operator >= OP_ADD ) && (Operator <= OP_BIT_OR)) ||
			((Operator >= OP_LESS) && (Operator <= OP_OR    )) )
		{
			OpString[Index].Values.Pop( &newVal );
			POPNEWVAL
			//Pop( newVal, Flags );

			if(!(Flags & FLG_NOWRITE)) fout<<" "<< GetOperator( Operator ) <<" ";

			OpString[Index].Values.Pop( &newVal );
			POPNEWVAL
			//Pop( newVal, Flags );

			KillOpString( Index );
		}
		else switch( Operator )
		{
			case OP_NOT:
				if(!(Flags & FLG_NOWRITE)) fout<<GetOperator( Operator );
				OpString[Index].Values.Pop( &newVal );
				POPNEWVAL
				KillOpString( Index );
				break;
			case OP_RANDOM:
				if(!(Flags & FLG_NOWRITE)) fout<<"Rand( ";
				OpString[Index].Values.Pop( &newVal );
				Pop( newVal, Flags );
				if(!(Flags & FLG_NOWRITE)) fout<<", ";
				OpString[Index].Values.Pop( &newVal );
				Pop( newVal, Flags );
				if(!(Flags & FLG_NOWRITE)) fout<<" )";
				KillOpString( Index );
				break;
			case OP_GET_UNITVAL:
				if(!(Flags & FLG_NOWRITE)) fout<<"get ";
				OpString[Index].Values.Pop( &newVal );
				if( newVal.IsOpString() )
					Pop( newVal, Flags );
				else if(!(Flags & FLG_NOWRITE)) 
					 PrintUnitValue( newVal.Val );
				KillOpString( Index );
				break;
			case OP_GET:
				if(!(Flags & FLG_NOWRITE)) fout<<"get ";
				OpString[Index].Values.Pop( &newVal );
				if(!(Flags & FLG_NOWRITE)) 
				{
					PrintUnitValue( newVal.Val );
					fout<<"(";
				}
				if( (newVal.Val==7) || (newVal.Val==8) )
				{
					OpString[Index].Values.Pop( &newVal );
					if( newVal.Flags() == TYP_CONST )
						{if(!(Flags & FLG_NOWRITE))
							fout<<PrintPieceName(newVal.Val);}
					else Pop( newVal, Flags );
				}
				else
				{
					OpString[Index].Values.Pop( &newVal );
					Pop( newVal, Flags );
				}
				OpString[Index].Values.Pop( &newVal );
				if( (newVal.Val != 0) || (newVal.Flags() != TYP_CONST) )
				{
					if(!(Flags & FLG_NOWRITE)) fout<<",";
					Pop( newVal, Flags );
					
					OpString[Index].Values.Pop( &newVal );
					if( (newVal.Val != 0) || (newVal.Flags() != TYP_CONST) )
					{
						if(!(Flags & FLG_NOWRITE)) fout<<",";
						Pop( newVal, Flags );

						OpString[Index].Values.Pop( &newVal );
						if( (newVal.Val != 0) || (newVal.Flags() != TYP_CONST) )
						{
							if(!(Flags & FLG_NOWRITE)) fout<<",";
							Pop( newVal, Flags );
						}
					}
				}
				if(!(Flags & FLG_NOWRITE)) fout<<")";
				KillOpString( Index );
				break;
			case CMD_PLAY_SOUND:
				if(!(Flags & FLG_NOWRITE)) fout<<"play-sound( \"";
				OpString[Index].Values.Pop( &newVal );
				if(!(Flags & FLG_NOWRITE))
				{
					fout<<PrintSoundName(newVal.Val)<<"\", ";
				}
				OpString[Index].Values.Pop( &newVal );
				Pop( newVal, FLG_SOUND );
				if(!(Flags & FLG_NOWRITE)) fout<<" )";
				KillOpString( Index );
				break;
			case CMD_UK_MAP:
				if(!(Flags & FLG_NOWRITE)) fout<<"Mission-Command( \"";
				OpString[Index].Values.Pop( &newVal );
				if(!(Flags & FLG_NOWRITE))
				{
					fout<<PrintSoundName(newVal.Val)<<"\"";
				}
				OpString[Index].Values.Pop( &newVal );
				long Count;
				Count = newVal.Val;
				for( int x=0;x<Count;x++)
				{
					OpString[Index].Values.Pop( &newVal );
					if(!(Flags & FLG_NOWRITE)) fout<<", ";
					Pop( newVal, Flags );
				}
				if(!(Flags & FLG_NOWRITE)) fout<<" )";
				KillOpString( Index );
				break;
		}
	}
	else
	{
		if(Flags & FLG_NOWRITE) return;
		if( Val.Flags() == TYP_CONST )
			PrintConst( Val.Val, Flags, First );
		else if( Val.Flags() == TYP_FVAR )
			fout<<FuncVar[Val.Val].Name;
		else if( Val.Flags() == TYP_SVAR )
			fout<<StaticVar[Val.Val].Name;
	}
#undef POPNEWVAL
}

int CCobCodec::NextIsZero()
{
	StackVal Val;
	STACK.Peek( &Val );
	if( (Val.Flags() == TYP_CONST) &&
		(Val.Val == 0) )
		return 1;
	else return 0;
}

int CCobCodec::IsIncDec(long What, BYTE Flag)
{
	int Ret = 0;
	StackVal Val,A,B;
	STACK.Peek( &Val );
	if( Val.IsOpString() &&
		((OpString[Val.Val].Operator == OP_ADD) || (OpString[Val.Val].Operator == OP_SUBTRACT)) )
	{
		OpString[Val.Val].Values.Pop( &A );
		OpString[Val.Val].Values.Pop( &B );
		if( (A.Flags() & Flag) &&
			(A.Val == What) &&
			(B.Flags() & TYP_CONST) &&
			(B.Val == 1) )
			Ret = ((OpString[Val.Val].Operator == OP_ADD) ? 1:2);
		OpString[Val.Val].Values.Push( &B );
		OpString[Val.Val].Values.Push( &A );
	}
	return Ret;
}

void CCobCodec::PrintConst( long Val, WORD Flags, int First )
{
	if( (Flags & FLG_BOOL) && First )
	{
		if( Val ) fout<<"TRUE";
		else fout<<"FALSE";
	}
	else if( (Flags & FLG_PIECE) && First )
	{
		if( (Val >= 0) && ( Val < Header.NumberOfPieces) )
			fout<<PrintPieceName(Val);
		else fout<<std::dec<<Val;
	}
	else if( Flags & FLG_LINEAR )
	{
		fout<<"["<<std::dec<<std::setprecision(FloatPrec)<<(float(Val)/(LConst))<<"]";
	}
	else if( Flags & FLG_ANGULAR )
	{
		fout<<"<"<<std::dec<<std::setprecision(FloatPrec)<<(float(Val)/(AConst))<<">";
	}
	else if( Flags & FLG_EXPLODE )
	{
		PrintExpConsts( Val );
	}
	else if( Flags & FLG_SOUND )
	{
		PrintSndConsts( Val );
	}
	else if( Flags & FLG_UNITVALUE )
	{
		PrintUnitValue( Val );
	}
	else if( Flags & FLG_SIGNAL )
	{
		for(int x=0,y=1;x<Signals.GetSize();x++)
			if(Signals[x].Val==Val)
			{
				y=0;
				fout<<Signals[x].Name;
				break;
			}
		if(y) fout<<Val;
	}
	else
	{
		if(TryKeepSmall && (Val >= LConst))
			fout<<"["<<std::dec<<std::setprecision(FloatPrec)<<(float(Val)/(LConst))<<"]";
		else fout<<std::dec<<Val;
	}
}

DWORD CCobCodec::GetPieceNameOffset(long Index)
{
	DWORD Offset=Header.NumberOfScripts+Header.NumberOfScripts+Index;
	return OffsetArray[Offset] - OffsetToFirstName;
}

LPSTR CCobCodec::PrintPieceName(long Index)
{
	if( ((Index)>=Header.NumberOfPieces)||((Index)<0))
		return "Error";
	else
		return (LPSTR)NameArray+GetPieceNameOffset(Index);
}

DWORD CCobCodec::GetScriptNameOffset(long Index)
{
	DWORD Offset=Header.NumberOfScripts+Index;
	return OffsetArray[Offset] - OffsetToFirstName;
}

LPSTR CCobCodec::PrintScriptName(long Index)
{
	if( ((Index)>=Header.NumberOfScripts)||((Index)<0))
		return "Error";
	else
		return (LPSTR)NameArray+GetScriptNameOffset(Index);
}

DWORD CCobCodec::GetSoundNameOffset(long Index)
{
	DWORD Offset=Header.NumberOfScripts+Header.NumberOfScripts+Header.NumberOfPieces+Index;
	return OffsetArray[Offset] - OffsetToFirstName;
}

LPSTR CCobCodec::PrintSoundName(long Index)
{
	if( ((Index)>=Header.NumberOfSounds)||((Index)<0))
		return "Error";
	else
		return (LPSTR)NameArray+GetSoundNameOffset(Index);
}
/*
void CCobCodec::PrintNumType(long Val,WORD type)
{
	float fl;
	if(TryKeepSmall && (Val >= LConst)) type=TYP_LINEAR;
	switch(type)
	{
	case TYP_BOOL:if(Val==0) fout<<"FALSE";
				  else fout<<"TRUE";
				  return;
	case TYP_FLOAT:fout<<std::dec<<std::setprecision(FloatPrec)<<Val;
				   return;
	case TYP_ANGULAR:fl=float(Val)/(AConst);
					 //if( (fl>=Max) || (fl<=Min) )
						fout<<"<"<<std::dec<<std::setprecision(FloatPrec)<<fl<<">";
					 //else fout<<"0";
					 return;
	case TYP_LINEAR:fl=float(Val)/(LConst);
					//if( (fl>=Max) || (fl<=Min) )
						fout<<"["<<std::dec<<std::setprecision(FloatPrec)<<fl<<"]";
					//else fout<<"0";
					return;
	case TYP_EXPLODECONSTS:PrintExpConsts(Val);
						   return;
	case TYP_SNDCONST:PrintSndConsts(Val);
					  return;
	case TYP_INTEGER:
	default:fout<<std::dec<<Val;
	}
}
*/
void CCobCodec::PrintUnitValue(long Val,int DoGet)
{
	for(int x=0;x<UnitValues.GetSize();x++)
		if(UnitValues[x].Val==Val)
		{
			//fout<<"( ";
			//*if(DoGet)*/ fout<<"get ";
			fout<<UnitValues[x].Name;
			//fout<<" )";
			return;
		}
	//*if(DoGet)*/ fout<<"get ";
	fout<<"UNKNOWN_UNIT_VALUE("<<std::dec<<Val<<")";
}

void CCobCodec::PrintSndConsts(long Val)
{
	switch(Val)
	{
	case 4:fout<<"PLAYSOUND_PRIORITY_BATTLE";
		   break;
	default:fout<<std::dec<<Val;
	}
}

void CCobCodec::PrintExpConsts(long Val)
{
	if(TAK)
	{
		switch(Val)
		{
		case      4:fout<<"FALL";
					break;
		case     32:fout<<"BITMAPONLY";
					break;
		case    256:fout<<"EXPTYPE_SMALLEXPLOSION";
					break;
		case    512:fout<<"EXPTYPE_MEDIUMEXPLOSION";
					break;
		case   1024:fout<<"EXPTYPE_LARGEEXPLOSION";
					break;
		case   2048:fout<<"EXPTYPE_XLARGEEXPLOSION";
					break;
		case   4096:fout<<"EXPTYPE_BLOODEXPLOSION";
					break;
		case   8192:fout<<"EXPTYPE_SMALLSMOKE";
					break;
		case  16384:fout<<"EXPTYPE_MEDIUMSMOKE";
					break;
		case  65536:fout<<"EXPTYPE_XLARGESMOKE";
					break;
		default:fout<<std::dec<<Val;
		}
	}
	else
	{
		switch(Val)
		{
		case      1:fout<<"SHATTER";
					break;
		case      2:fout<<"EXPLODE_ON_HIT";
					break;
		case      4:fout<<"FALL";
					break;
		case      8:fout<<"SMOKE";
					break;
		case     16:fout<<"FIRE";
					break;
		case     32:fout<<"BITMAPONLY";
					break;
		case    256:fout<<"BITMAP1";
					break;
		case    512:fout<<"BITMAP2";
					break;
		case   1024:fout<<"BITMAP3";
					break;
		case   2048:fout<<"BITMAP4";
					break;
		case   4096:fout<<"BITMAP5";
					break;
		case   8192:fout<<"BITMAPNUKE";
					break;
		case  16128:fout<<"BITMAPMASK";
					break;
		default:fout<<std::dec<<Val;
		}
	}
}
/*
void CCobCodec::PrintOp(StackVal stackval,WORD extra,int para)
{
	/*OPSTRING Op;
	if(stackval.Flag==FLG_OPSTRING)
	{
		OpPos--;
		Op=OpString[stackval.Val];
		if(Op.Val_1.Flag==10)
		{
			if(Op.Val_1.Val==0x10041000)
			{
				fout<<"Rand(";
				PrintOp(Op.Val_2,extra,para);
				fout<<",";
				PrintOp(Op.Val_3,extra,para);
				fout<<")";
			}
			else if(Op.Val_1.Val==0x10042000)
			{
				fout<<"UNIT_VALUE( ";
				PrintOp(Op.Val_2,extra,para);
				fout<<" )";
			}
			else
			{
				fout<<"( "<<GetOperator(Op.Val_1.Val);
				PrintOp(Op.Val_2,extra,para);
				fout<<" )";
			}
		}
		else if(Op.Val_1.Flag==FLG_GET)
			{
				Op.Val_1.Flag=FLG_UNITVALUE;
				PrintVal(Op.Val_1);
				fout<<"(";
				if((Op.Val_1.Val==7)||(Op.Val_1.Val==8))
					fout<<PrintPieceName(Op.Val_2.Val);
				else PrintOp(Op.Val_2,extra,para);
				if(Op.Val_3.Flag!=FLG_CONSTANT)
				{
					fout<<",";
					PrintOp(Op.Val_3,extra,para);
				}
				fout<<")";
			}
		else
		{
			if(para) fout<<"( ";
			PrintOp(Op.Val_1,extra,para);
			fout<<" "<<GetOperator(Op.Val_2.Val)<<" ";
			PrintOp(Op.Val_3,extra,para);
			if(para) fout<<" )";
		}
	}
	else if(stackval.Flag==FLG_MISCMD)
	{
		MisPos--;
		fout<<"Mission-Command( \""
			<<PrintSoundName(MisCmd[MisPos].Param1)<<"\" ";
		for(int x=(MisCmd[MisPos].Param2)-1;x>=0;x--)
		{
			fout<<", ";//<<x<<" ";
			PrintOp(MisCmd[MisPos].StackVals[x]);
		}
		if(MisCmd[MisPos].StackVals) delete [] MisCmd[MisPos].StackVals;
		fout<<" )";
	}
	else
	{
		//if(para) fout<<"( ";
		PrintVal(stackval,extra);
		//if(para) fout<<" )";
	}*/
//}
/*
void CCobCodec::PrintVal(StackVal stackval,WORD extra)
{
	int x,y;
	switch(stackval.Flag)
	{
	case FLG_CONSTANT:PrintNumType(stackval.Val,extra);
					  return;
	case FLG_STATICVAR:fout<<StaticVar[stackval.Val].Name;
					   return;
	case FLG_FUNCTIONVAR:fout<<FuncVar[stackval.Val].Name;
						 return;
	case FLG_SIGNAL:y=1;
					for(x=0;x<Signals.GetSize();x++)
						if(Signals[x].Val==stackval.Val)
						{
							y=0;
							fout<<Signals[x].Name;
						}
					if(y) fout<<stackval.Val;
					return;
	case FLG_UNITVALUE:PrintUnitValue(stackval.Val,extra);
					   return;
	}
}*/

char CCobCodec::GetAxis(DWORD Index)
{
	switch(Index)
	{
	case 0:return'x';
	case 1:return'y';
	case 2:return'z';
	default:return'0';
	}
}

LPCTSTR CCobCodec::GetOperator(long Val)
{
	if(Val==0x1005A000) return "!";
	for(int x=0;x<Operators.GetSize();x++)
		if(Operators[x].Val==Val)
		{
			return Operators[x].Op;
		}
	return "UK";
}

int CCobCodec::GetOpPriority(long Val)
{
	switch( Val )
	{
	case OP_RANDOM:
	case OP_GET_UNITVAL:
	case OP_GET:
	case CMD_PLAY_SOUND:
	case CMD_UK_MAP:
		return 1000;
	}
	for(int x=0;x<Operators.GetSize();x++)
		if(Operators[x].Val==Val)
		{
			return Operators[x].Priority;
		}
	return 0;
}

void CCobCodec::TabSpace(int tab,long count)
{
	int x;
	if (Offsets) fout<<"/*"<<std::hex<<std::setw(6)<<count<<":*/";
	for(x=0;x<tab;x++) fout<<"	";
	if(WasElse)
	{
		WasElse=false;
		fout<<"else ";
	}
}

void CCobCodec::Move_Turn_Cmd(long* DataPtr,int* pos,int tab,int Flag,long count)
{
	WORD Const;
	// 1 = turn now
	// 2 = move now
	// 3 = turn
	// 4 = move

	if( (Flag==3) || (Flag==1))
		 Const=FLG_ANGULAR;
	else Const=FLG_LINEAR;

	TabSpace(tab,count);
	if( (Flag==4) || (Flag==2) ) fout<<"move ";
	else fout<<"turn ";
	fout<<PrintPieceName(*(DataPtr+1))
		<<" to "<<GetAxis(*(DataPtr+2))<<"-axis ";
	Pop( Const );
	if( (Flag==2) || (Flag==1) ) fout<<" now;\n";
	else
	{
		fout<<" speed ";
		Pop( Const );
		fout<<";\n";
	}
}

long CCobCodec::ProcessBlock(long* DataPtr,int tab,long offset,long Length)
{
	long Val;
	long Val2;
	int x=0;
	int y;
	int pos=0;
	Var var;
	bool comp=false;
	StackVal* Temp_Stack;
	StackVal StackVal_1,StackVal_2;
	long count=offset;
	last=count;
	TabSpace(tab-1,count);
	fout<<"{\n";
	while(count<Length)
	{
		x=0;
		switch(*DataPtr)
		{
		case 0x10001000:x++;
		case 0x10002000:x++;
		case 0x1000B000:x++;
		case 0x1000C000:x++;
						Move_Turn_Cmd(DataPtr,&pos,tab,x,count);
						DataPtr+=3;
						count+=3;
						last=count;
						break;
		case 0x10003000:std::cout<<"	spin_piece2;\n";
						TabSpace(tab,count);
						fout<<"spin "<<PrintPieceName(*(DataPtr+1))
							<<" around "<<GetAxis(*(DataPtr+2))<<"-axis"
							<<" speed ";
						Pop( FLG_ANGULAR );
						if( !NextIsZero() )
						{
							fout<<" accelerate ";
							Pop( FLG_ANGULAR );
						}
						else Pop( FLG_NOWRITE );
						fout<<";\n";
						count+=3;
						DataPtr+=3;
						last=count;
	 					break;
		case 0x10004000:std::cout<<"	stop_spin2;\n";
						TabSpace(tab,count);
						fout<<"stop-spin "<<PrintPieceName(*(DataPtr+1))
							<<" around "<<GetAxis(*(DataPtr+2))<<"-axis";
						if( !NextIsZero() )
						{
							fout<<" decelerate ";
							Pop( FLG_ANGULAR );
						}
						else Pop( FLG_NOWRITE );
						fout<<";\n";
						count+=3;
						DataPtr+=3;
						last=count;
	 					break;
		case 0x10005000:std::cout<<"	show1;\n";
						TabSpace(tab,count);
						fout<<"show "<<PrintPieceName(*(DataPtr+1))
							<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x10006000:std::cout<<"	hide1;\n";
						TabSpace(tab,count);
						fout<<"hide "<<PrintPieceName(*(DataPtr+1))
							<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x10007000:std::cout<<"	cache1;\n";
						TabSpace(tab,count);
						fout<<"cache "<<PrintPieceName(*(DataPtr+1))
							<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x10008000:std::cout<<"	dont_cache1;\n";
						TabSpace(tab,count);
						fout<<"dont-cache "<<PrintPieceName(*(DataPtr+1))
							<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x1000A000:std::cout<<"	dont_shadow\n";
						TabSpace(tab,count);
						fout<<"dont-shadow "<<PrintPieceName(*(DataPtr+1))
							<<";\n";
						count+=2;
						DataPtr+=2;
						break;
		case 0x1000E000:std::cout<<"	dont_shade1;\n";
						TabSpace(tab,count);
						fout<<"dont-shade "<<PrintPieceName(*(DataPtr+1))
							<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x1000F000:std::cout<<"	emit_sfx_from_piece1;\n";
						TabSpace(tab,count);
						fout<<"emit-sfx ";
						Pop( 0 );
						fout<<" from "<<PrintPieceName(*(DataPtr+1))<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x10011000:std::cout<<"	wait_for_turn2;\n";
						TabSpace(tab,count);
						fout<<"wait-for-turn "<<PrintPieceName(*(DataPtr+1))
							<<" around "<<GetAxis(*(DataPtr+2))<<"-axis;\n";
						DataPtr+=3;
						count+=3;
						last=count;
						break;
		case 0x10012000:std::cout<<"	wait_for_move2;\n";
						TabSpace(tab,count);
						fout<<"wait-for-move "<<PrintPieceName(*(DataPtr+1))
							<<" along "<<GetAxis(*(DataPtr+2))<<"-axis;\n";
						DataPtr+=3;
						count+=3;
						last=count;
						break;
		case 0x10013000:std::cout<<"	sleep0;\n";
						TabSpace(tab,count);
						fout<<"sleep ";
						Pop( 0 );
						fout<<";\n";
						count++;
						DataPtr++;
						last=count;
	 					break;
		case 0x10021001:std::cout<<"	push_constant1;\n";
						pos = Push( *(DataPtr+1), TYP_CONST );
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// PushConst->"<<std::dec<<float(*(DataPtr+1))
								<<" StackPos "<<pos<<std::endl;
						}
						DataPtr+=2;
						count+=2;
	 					continue;
		case 0x10021002:std::cout<<"	push_variable1;\n";
						pos = Push( *(DataPtr+1), TYP_FVAR );
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// PushFVar->"<<std::dec<<*(DataPtr+1)
								<<" StackPos "<<pos<<std::endl;
						}
						DataPtr+=2;
						count+=2;
	 					continue;
		case 0x10021004:std::cout<<"	push_static_var1;\n";
						pos = Push( *(DataPtr+1), TYP_SVAR );
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// PushStatic->"<<std::dec<<*(DataPtr+1)
								<<" StackPos "<<pos<<std::endl;
						}
						DataPtr+=2;
						count+=2;
	 					continue;
		case 0x10022000:std::cout<<"	stack_allocate0;\n";
						if(*(DataPtr+1)==0x10065000)
						{
							for(x=0;x<NumLabels;x++) 
								if(Labels[x]==count)
								{
									TabSpace(tab,count);
									fout<<"Label"<<x+1<<":\n";
								}
							if(ShowPush)
							{
								TabSpace(tab,count);
								fout<<"// StackAllocate\n";
							}
							DataPtr++;
							count++;
							last=count;
							break;
						}
						x=NumFuncVar;
						while(*DataPtr==0x10022000)
						{
							if(ShowPush)
							{
								TabSpace(tab,count);
								fout<<"// StackAllocate\n";
							}
							x++;
							ltoa(x,var.Name.GetBuffer(1),10);
							var.Name.ReleaseBuffer();
							var.Name="Func_Var_"+var.Name;
							var.Type=1;
							FuncVar.Add(var);
							DataPtr++;
							count++;
						}
						TabSpace(tab,count);
						fout<<"var ";
						for(y=NumFuncVar;y<x;y++)
						{
							fout<<FuncVar[y].Name;
							if((y+1)!=x) fout<<", ";
						}
						fout<<";\n";
						NumFuncVar=x;
						last=count;
						break;
		case 0x10023002:std::cout<<"	pop_variable1;\n";
						TabSpace(tab,count);
						if( Val = IsIncDec(*(DataPtr+1),TYP_FVAR) )
						{
							if( Val == 1 )
								 fout<<"++";
							else fout<<"--";
							fout<<FuncVar[*(DataPtr+1)].Name;
							Pop( FLG_NOWRITE );
						}
						else
						{
							fout<<FuncVar[*(DataPtr+1)].Name
								<<" = ";
							WORD Type;
							switch(FuncVar[*(DataPtr+1)].Type)
							{
							case TYP_BOOL: Type = FLG_BOOL; break;
							case 5:		   Type = FLG_PIECE; break;
							default:	   Type = 0; break;
							}
							Pop( Type );
						}
						fout<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x10023004:std::cout<<"	pop_static_var1;\n";
						TabSpace(tab,count);
						if( Val = IsIncDec(*(DataPtr+1),TYP_SVAR) )
						{
							if( Val == 1 )
								 fout<<"++";
							else fout<<"--";
							fout<<StaticVar[*(DataPtr+1)].Name;
							Pop( FLG_NOWRITE );
						}
						else
						{
							fout<<StaticVar[*(DataPtr+1)].Name
								<<" = ";
							WORD Type;
							switch(StaticVar[*(DataPtr+1)].Type)
							{
							case TYP_BOOL: Type = FLG_BOOL; break;
							case 5:		   Type = FLG_PIECE; break;
							default:	   Type = 0; break;
							}
							Pop( Type );
						}
						fout<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x10024000:std::cout<<"	stop-sound?0;\n";
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// NoReturnValue\n";
						}
						TabSpace(tab,count);
						Pop( 0 );
						fout<<";\n";
						/*STACK.Pop(&StackVal_1);
						if(StackVal_1.Flag==FLG_MISCMD)
						{
							TabSpace(tab,count);
							PrintOp(StackVal_1);
							fout<<";\n";
						}
						else STACK.Push(&StackVal_1);*/
						count++;
						DataPtr++;
						last=count;
	 					break;
		case 0x10041000:std::cout<<"	rand0;\n";
						/*STACK.Pop(&StackVal_1);
						STACK.Pop(&StackVal_2);
						OpString[OpPos].Val_1.Val=*DataPtr;
						OpString[OpPos].Val_1.Flag=10;
						OpString[OpPos].Val_2=StackVal_2;
						OpString[OpPos].Val_3=StackVal_1;
						StackVal_1.Flag=FLG_OPSTRING;
						StackVal_1.Val=OpPos;
						OpPos++;
						STACK.Push(&StackVal_1);*/
						pos = PushOp( OP_RANDOM );
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// Rand()"
								<<" OpPos "<<pos<<"\n";
						}
						count++;
						DataPtr++;
	 					break;
		case 0x10042000:std::cout<<"	get_unit_value0;\n";
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						pos = PushOp( OP_GET_UNITVAL );
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// get UnitValue"
								<<" OpPos "<<pos<<"\n";
						}
						/*STACK.Pop(&StackVal_1);
						if(StackVal_1.Flag==FLG_OPSTRING)
						{
							OpString[OpPos].Val_1.Val=*DataPtr;
							OpString[OpPos].Val_1.Flag=10;
							OpString[OpPos].Val_2=StackVal_1;
							StackVal_1.Flag=FLG_OPSTRING;
							StackVal_1.Val=OpPos;
							OpPos++;
							STACK.Push(&StackVal_1);
						}
						else
						{
							StackVal_1.Flag=FLG_UNITVALUE;
							STACK.Push(&StackVal_1);
						}*/
						count++;
						DataPtr++;
	 					break;
		case 0x10043000:std::cout<<"	get0;\n";
						/*STACK.Pop(&StackVal_1);
						STACK.Pop(&StackVal_1);
						STACK.Pop(&StackVal_1);
						STACK.Pop(&StackVal_2);
						OpString[OpPos].Val_2=StackVal_2;
						OpString[OpPos].Val_3=StackVal_1;
						STACK.Pop(&StackVal_1);
						OpString[OpPos].Val_1.Val=StackVal_1.Val;
						OpString[OpPos].Val_1.Flag=FLG_GET;
						StackVal_1.Flag=FLG_OPSTRING;
						StackVal_1.Val=OpPos;
						OpPos++;
						STACK.Push(&StackVal_1);*/
						pos = PushOp( OP_GET );
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// Get()"
								<<" OpPos "<<pos<<"\n";
						}
						count++;
						DataPtr++;
						break;
		case 0x10031000:
		case 0x10032000:
		case 0x10033000:
		case 0x10034000:
		case 0x10035000:
		case 0x10036000:
		case 0x10039000:
		case 0x1003A000:
		case 0x1003B000:
		case 0x10051000:
		case 0x10052000:
		case 0x10053000:
		case 0x10054000:
		case 0x10055000:
		case 0x10056000:
		case 0x10057000:
		case 0x10058000:
						/*STACK.Pop(&StackVal_1);
						STACK.Pop(&StackVal_2);
						OpString[OpPos].Val_1=StackVal_2;
						OpString[OpPos].Val_2.Val=*DataPtr;
						OpString[OpPos].Val_2.Flag=10;
						OpString[OpPos].Val_3=StackVal_1;
						StackVal_1.Flag=FLG_OPSTRING;
						StackVal_1.Val=OpPos;
						OpPos++;
						STACK.Push(&StackVal_1);*/
						pos = PushOp( *DataPtr );
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// OP -> "
								<<GetOperator(*DataPtr)
								<<" OpPos "<<pos<<"\n";
						}
						count++;
						DataPtr++;
	 					break;
		case 0x1005A000:
						/*STACK.Pop(&StackVal_1);
						OpString[OpPos].Val_1.Val=*DataPtr;
						OpString[OpPos].Val_1.Flag=10;
						OpString[OpPos].Val_2=StackVal_1;
						StackVal_1.Flag=FLG_OPSTRING;
						StackVal_1.Val=OpPos;
						OpPos++;
						STACK.Push(&StackVal_1);*/
						pos = PushOp( OP_NOT );
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// OP -> "
								<<GetOperator(*DataPtr)
								<<" OpPos "<<pos<<"\n";
						}
						count++;
						DataPtr++;
	 					break;
		case 0x10061000:std::cout<<"	start_script2;\n";
						TabSpace(tab,count);
						fout<<"start-script "<<PrintScriptName(*(DataPtr+1))
							<<"(";
						Temp_Stack=new StackVal[*(DataPtr+2)];
						for(x=0;x<(*(DataPtr+2));x++)
						{
							STACK.Pop(&StackVal_1);
							Temp_Stack[x]=StackVal_1;
						}
						for(x=(*(DataPtr+2))-1;x>=0;x--)
						{
							//PrintOp(Temp_Stack[x]);
							Pop( Temp_Stack[x], 0, 1);
							if(x!=0) fout<<",";
						}
						if(Temp_Stack) delete [] Temp_Stack;
						fout<<");\n";
						count+=3;
						DataPtr+=3;
						last=count;
	 					break;
		case 0x10062000:std::cout<<"	call_script2;\n";
						TabSpace(tab,count);
						fout<<"call-script "<<PrintScriptName(*(DataPtr+1))
							<<"(";
						Temp_Stack=new StackVal[*(DataPtr+2)];
						for(x=0;x<(*(DataPtr+2));x++)
						{
							STACK.Pop(&StackVal_1);
							Temp_Stack[x]=StackVal_1;
						}
						for(x=(*(DataPtr+2))-1;x>=0;x--)
						{
							//PrintOp(Temp_Stack[x]);
							Pop( Temp_Stack[x], 0, 1);
							if(x!=0) fout<<",";
						}
						if(Temp_Stack) delete [] Temp_Stack;
						fout<<");\n";
						if(ShowPush)
						{
							TabSpace(tab+1,count);
							fout<<"// Params -> "
								<<std::dec<<*(DataPtr+1)<<" , "
								<<*(DataPtr+2)<<std::endl;
						}
						count+=3;
						DataPtr+=3;
						last=count;
	 					break;
		case CMD_FAKE_JUMP:std::cout<<"	jump1;\n";
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// JUMP ->"<<std::hex
								<<*(DataPtr+1)
								<<"\n";
						}
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case CMD_JUMP:std::cout<<"	jump1;\n";
						for(x=0;x<NumLabels;x++) 
							if(Labels[x]==count)
							{
								TabSpace(tab,count);
								fout<<"Label"<<x+1<<":\n";
							}
						if( Loops.GetSize()&&(Loops[Loops.GetUpperBound()].Start==(*(DataPtr+1)) ) )
						{
							TabSpace(tab,count);
							fout<<"continue;\n";
						}
						else if( Loops.GetSize()&&(Loops[Loops.GetUpperBound()].Stop==(*(DataPtr+1)) ) )
						{
							TabSpace(tab,count);
							fout<<"break;\n";
						}
						else if(count>(*(DataPtr+1)))
						{
							TabSpace(tab,count);
							fout<<"jump "<<std::hex
								<<*(DataPtr+1)
								<<";\n";
						}
						/*else if( *(DataPtr+(*(DataPtr+1)-count))==0x10063000 )
						{
							TabSpace(tab,count);
							fout<<"continue;\n";
						}
						else if(*(DataPtr+1)==Length)
						{
							TabSpace(tab,count);
							fout<<"break;\n";
						}*/
						else
						{
							for(x=0;x<NumLabels;x++)
								if(Labels[x]==*(DataPtr+1)) break;
							Labels[x]=*(DataPtr+1);
							TabSpace(tab,count);
							fout<<"goto "
								<<"Label"<<x+1
								<<";";
							if(ShowPush) fout<<" -> "<<*(DataPtr+1);
							fout<<std::endl;
							if(x==NumLabels) NumLabels++;
						}
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x10065000:std::cout<<"	return0;\n";
						if(STACK.Size())
						{
							//STACK.Pop(&StackVal_1);
							if( ShowReturn || 
								( (count+1)<Length ) ||
								( tab > 1 ) ||
								( !NextIsZero() ) ) 
							{
								TabSpace(tab,count);
								fout<<"return (";
								//PrintOp(StackVal_1);
								Pop( 0 );
								fout<<");\n";
							}
							else Pop( FLG_NOWRITE );
						}
						else
						{
							if( ShowReturn|| ((count+1)<Length) || (tab > 1) )
							{
								TabSpace(tab,count);
								fout<<"return;\n";
							}
						}
						count++;
						DataPtr++;
						last=count;
	 					break;
		case 0x10066000:std::cout<<"	jump_if_false1;\n";
						TabSpace(tab,count);
						x=0;
						bool WasLoop;
						WasLoop=false;
#define GET_END_OPCODE(arg)	(*(DataPtr + ( ( *(DataPtr+1) - count ) arg ) ))
#define LAST_CMD_IN_BLOCK	GET_END_OPCODE(-2)
#define JUMP_OFFSET			GET_END_OPCODE(-1)
						if( LAST_CMD_IN_BLOCK == CMD_JUMP )
						{
							Val = JUMP_OFFSET;
							if( Val == last )
							{
								LOOP Loop;
								WasLoop=true;
								LAST_CMD_IN_BLOCK = CMD_FAKE_JUMP;
								Loop.Start=last;
								Loop.Stop=*(DataPtr+1);
								Loops.Add(Loop);
								fout<<"while";
							}
							else if( Val > *(DataPtr+1) )
							{
								fout<<"if";
								LAST_CMD_IN_BLOCK = CMD_FAKE_JUMP;
								Val2=Val;
								x=1;
							}
							else fout<<"if";
						}
						else fout<<"if";

						/*STACK.Pop(&StackVal_1);
						if(StackVal_1.Flag!=FLG_OPSTRING)
						{
							fout<<"( ";
							PrintVal(StackVal_1,TYP_BOOL);
							fout<<" )";
						}
						else PrintOp(StackVal_1);*/
						fout<<"( ";
						Pop( FLG_BOOL );
						fout<<" )";

						if(ShowPush) fout<<" // else JUMP -> "<<std::hex<<*(DataPtr+1);
						fout<<"\n";

						Val=*(DataPtr+1);
						DataPtr+=2;
						count+=2;
						Val=ProcessBlock(DataPtr,tab+1,count,Val);
						DataPtr+=(Val-count);
						count=Val;
						last=count;
						if(x)
						{
							Val = GetNextCmd(DataPtr,count,Val2);
							if( (!Val) || (Val != CMD_JUMP_IF))
							{
								TabSpace(tab,count);
								fout<<"else\n";
								Val=ProcessBlock(DataPtr,tab+1,count,Val2);
								DataPtr+=(Val-count);
								count=Val;
								last=count;
							}
							else WasElse=true;
						}
						if(WasLoop) Loops.RemoveAt(Loops.GetUpperBound());
#undef GET_END_OPCODE
#undef LAST_CMD_IN_BLOCK
#undef JUMP_OFFSET
						break;
		case 0x10067000:std::cout<<"	signal0;\n";
						TabSpace(tab,count);
						fout<<"signal ";
						//STACK.Pop(&StackVal_1);
						//StackVal_1.Flag=FLG_SIGNAL;
						//PrintVal(StackVal_1);
						Pop( FLG_SIGNAL );
						fout<<";\n";
						count++;
						DataPtr++;
						last=count;
	 					break;
		case 0x10068000:std::cout<<"	set_signal0;\n";
						TabSpace(tab,count);
						fout<<"set-signal-mask ";
						//STACK.Pop(&StackVal_1);
						//StackVal_1.Flag=FLG_SIGNAL;
						//PrintVal(StackVal_1);
						Pop( FLG_SIGNAL );
						fout<<";\n";
						count++;
						DataPtr++;
						last=count;
	 					break;
		case 0x10071000:std::cout<<"	explode1;\n";
						TabSpace(tab,count);
						fout<<"explode "
							<<PrintPieceName(*(DataPtr+1))
							<<" type ";
						//STACK.Pop(&StackVal_1);
						//PrintOp(StackVal_1,TYP_EXPLODECONSTS,0);
						Pop( FLG_EXPLODE );
						fout<<";\n";
						count+=2;
						DataPtr+=2;
						last=count;
	 					break;
		case 0x10072000:std::cout<<"	play_sound1;\n";
						pos = Push( *(DataPtr+1), TYP_CONST );
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// PushSound->"<<std::dec<<*(DataPtr+1)
								<<" StackPos "<<pos<<std::endl;
						}
						pos = PushOp( CMD_PLAY_SOUND );
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// play sound"
								<<" OpPos "<<pos<<"\n";
						}
						//STACK.Pop(&StackVal_1);
						//PrintOp(StackVal_1,TYP_SNDCONST);
						//Pop( FLG_SOUND );
						//fout<<" );\n";
						count+=2;
						DataPtr+=2;
						//last=count;
	 					break;
		case CMD_UK_MAP:std::cout<<"	Mission Command1;\n";
						pos = Push( *(DataPtr+1), TYP_CONST );
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// PushCommand->"<<std::dec<<*(DataPtr+1)
								<<" StackPos "<<pos<<std::endl;
						}
						pos = Push( *(DataPtr+2), TYP_CONST );
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// PushCommandStack->"<<std::dec<<*(DataPtr+1)
								<<" StackPos "<<pos<<std::endl;
						}
						pos = PushOp( CMD_UK_MAP );
						if(ShowPush)
						{
							TabSpace(tab,count);
							fout<<"// Mission Command\n"
								<<" OpPos "<<pos<<"\n";
						}
						/*StackVal_2.Val=MisPos;
						StackVal_2.Flag=FLG_MISCMD;
						MisCmd[MisPos].Param1=*(DataPtr+1);
						MisCmd[MisPos].Param2=*(DataPtr+2);
						Temp_Stack=new StackVal[*(DataPtr+2)];
						for(x=0;x<(*(DataPtr+2));x++)
						{
							STACK.Pop(&StackVal_1);
							Temp_Stack[x]=StackVal_1;
						}
						MisCmd[MisPos].StackVals=Temp_Stack;
						STACK.Push(&StackVal_2);
						MisPos++;*/
						count+=(3);
						DataPtr+=(3);
						//last=count;
	 					break;
		case 0x10082000:std::cout<<"	set_value0;\n";
						TabSpace(tab,count);
						fout<<"set ";
						Pop( FLG_UNITVALUE, 1 );
						//STACK.Pop(&StackVal_1);
						//STACK.Pop(&StackVal_2);
						//StackVal_2.Flag=4;
						//PrintVal(StackVal_2,0);
						fout<<" to ";
						//PrintOp(StackVal_1);
						Pop(0);
						fout<<";\n";
						count++;
						DataPtr++;
						last=count;
	 					break;
		case 0x10083000:std::cout<<"	attach-unit0;\n";
						TabSpace(tab,count);
						fout<<"attach-unit ";
						Pop( FLG_NOWRITE );
						//STACK.Pop(&StackVal_1);
						//STACK.Pop(&StackVal_1);
						//STACK.Pop(&StackVal_2);
						//PrintOp(StackVal_2);
						Pop( 0, 1);
						fout<<" to ";
						//if(StackVal_1.Flag==FLG_OPSTRING)
						//	PrintOp(StackVal_1);
						//else fout<<PrintPieceName(StackVal_1.Val);
						Pop( FLG_PIECE );
						fout<<";\n";
						count++;
						DataPtr++;
						last=count;
	 					break;
		case 0x10084000:std::cout<<"	drop-unit0;\n";
						TabSpace(tab,count);
						fout<<"drop-unit ";
						//STACK.Pop(&StackVal_1);
						//PrintOp(StackVal_1);
						Pop(0);
						fout<<";\n";
						count++;
						DataPtr++;
						last=count;
	 					break;
		case COBBLER_CRAP:TabSpace(tab,count);
						  fout<<"// "<<(char*)DataPtr<<std::endl;
						  DataPtr+=45;
						  count+=45;
						  break;
		default:/*if( (*DataPtr>(0x11111111))||(*DataPtr<0) )
				{
					DataPtr+=Length-count;
					count=Length;
					break;
				}*/
				std::cout<<"	UnKnown "<<std::hex<<*DataPtr<<";\n";
				TabSpace(tab,count);
				fout<<"UnKnown "<<std::hex<<*DataPtr<<";\n";
				DataPtr++;
				count++;
				last=count;
		}
	}
	TabSpace(tab-1,count);
	fout<<"}\n";
	return count;
}

long CCobCodec::GetNextCmd(long* DataPtr,long offset,long Length)
{
	for(long count=offset;count<Length;)
	{
		if( (*DataPtr >= CMD_PUSH_CON)&&(*DataPtr <= CMD_PUSH_SVAR) )
		{
			DataPtr+=2;
			count+=2;
		}
		else if( (*DataPtr >= OP_ADD)&&(*DataPtr <= OP_NOT) )
		{
			DataPtr++;
			count++;
		}
		else return *DataPtr;
	}
	return 0;
}

long CCobCodec::ProcessScript(LPSTR Name,char* ScriptCode,long offset,long Length)
{
	if(StdLibs)
	{
		if(IsYardH && (stricmp(Name,"OpenYard")==0) )
		{
			if(GotCYard) fout<<"#include \"Yard.h\"\n";
			GotOYard = true;
			return Length;
		}
		if(IsYardH && (stricmp(Name,"CloseYard")==0) )
		{
			if(GotOYard) fout<<"#include \"Yard.h\"\n";
			GotCYard = true;
			return Length;
		}
		if( TAK )
		{
			if( HaveMaxDFlames && 
				HaveMaxSmokePuffs && 
				(stricmp(Name,"SmokeControl")==0) )
			{
				fout<<"#include \"SmokeUnit.h\"\n";
				return Length;
			}
		}
		else // if( TA )
		{
			if( /*HaveMaxDFlames && 
				HaveMaxSmokePuffs && */
				(stricmp(Name,"SmokeUnit")==0) )
			{
				fout<<"#include \"SmokeUnit.h\"\n";
				return Length;
			}
		}
		if(HaveMaxDFlames && HaveMaxDFLevel && (stricmp(Name,"DamageFlameControl")==0) )
		{
			fout<<"#include \"FlameUnit.h\"\n";
			return Length;
		}
		if(GotStates && (stricmp(Name,"InitState")==0))
		{
			return Length;
		}
		if(GotStates && (stricmp(Name,"RequestState")==0))
		{
			fout<<"#define ACTIVATECMD			call-script "
				<<PrintScriptName(NumGo)<<"();\n"
				<<"#define DEACTIVATECMD		call-script "
				<<PrintScriptName(NumStop)<<"();\n"
				<<"#include \"StateChg.h\"\n";
			return Length;
		}
	} // End if( StdLibs )

	int InFile=0;
	CString LastFile;
	if(MiscLibs && CheckAnimFunc(Name))
	{
		CString Path,Dir;
		Path=MainFile.Left(MainFile.ReverseFind('\\')+1);
		Dir=MainFile.Right(MainFile.GetLength()-(MainFile.ReverseFind('\\')+1));
		Dir=Dir.Left(Dir.ReverseFind('.'));
		if(!DirCreated)
		{
			CreateDirectory(Path+Dir,0);
			DirCreated = true;
		}
		CString ScriptName=Name;
		ScriptName += ".bos";
		fout.close();
		fout.open(Path+Dir+"\\"+ScriptName);
		LastFile=Dir+"\\"+ScriptName;
		InFile=1;
	}

	long* DataPtr=reinterpret_cast < long* > ( ScriptCode );

	int x,count=0;
	fout<<std::endl;
	while(*(DataPtr+count) == 0x10022000)
	{
		count++;
	}
	count=CheckCommonFunc(Name,count);
	fout<<Name<<"(";
	for(x=0;x<count;x++)
	{
		fout<<FuncVar[x].Name;
		if((x+1)!=count) fout<<", ";
	}
	NumFuncVar=count;
	fout<<")\n";
	Labels=new long[16];
	NumLabels=0;
	WasElse=false;
	ProcessBlock(DataPtr+count,1,offset+count,Length);
	delete [] Labels;
	FuncVar.RemoveAll();
	if(InFile)
	{
		fout.close();
		fout.open(MainFile,std::ios::ate);
		fout<<"#include \""<<LastFile<<"\"\n";
	}
	return Length;
}

int CCobCodec::CheckAnimFunc(LPCTSTR Func)
{
	for(int x=AnimFunc.GetUpperBound();x>=0;x--)
		if(AnimFunc[x].CompareNoCase(Func)==0) return TRUE;
	return FALSE;
}

LRESULT CCobCodec::OpenCob(LPCSTR FileName)
{
	unsigned long BytesRead;


	File = CreateFile(FileName, GENERIC_READ, (FILE_SHARE_READ | FILE_SHARE_WRITE),
		NULL, OPEN_EXISTING, (FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
		NULL);
	if (File == INVALID_HANDLE_VALUE) 
	{
		std::cout<<"Could not open file.\n";
		return 0;
	}

	if (!ReadFile(File, &Header, sizeof(Header), &BytesRead, NULL))
	{
		CloseHandle(File);
		std::cout<<"Error Reading Header/n";
		return 0;
	}

	if( Header.VersionSignature == 6 ) TAK=true;
	else TAK=false;

	if(TAK) OffsetToFirstName=Header.OffsetToSoundNameArray + (Header.NumberOfSounds * 4);
	else OffsetToFirstName=Header.OffsetToNameArray;

	return 1;
}

LRESULT CCobCodec::MakeBos(LPCSTR FileName,CProgressCtrl* Prog)
{
	int x;								// for loops and other nonsense
	long Pos;
	long Size;							
	long Offset;
	unsigned long BytesRead;
	char* ScriptBuf;


	MainFile = FileName;
	DirCreated = FALSE;
	fout.open(FileName);				// Open BOS file for output
	if(!fout)
	{
		std::cout<<"Unable to open BOS";
		return 0;
	}

//  Store all offsets in OffsetArray

	SetFilePointer(File,Header.OffsetToScriptCodeIndexArray,0,FILE_BEGIN);
		// Move file pointer to first offset
	Size = Header.NumberOfScripts;		// Number of offsets to scripts
	Size+= Header.NumberOfScripts;		// Number of offsets to script names
	Size+= Header.NumberOfPieces;		// Number of offsets to piece names
	if(TAK)Size+=Header.NumberOfSounds; // Number of offsets to sound names
	OffsetArray=new DWORD[Size];		// Allocate memory for array
	Size*=4;					// Size is in number of offsets
								// We need it in number of bytes
	if (!ReadFile(File,OffsetArray,Size,&BytesRead, NULL))
	{
		CloseHandle(File);
		fout<<"Error Reading Offsets\n";
		return 0;
	}

//  Store all names in NameArray

	Size=GetFileSize(File,0) - OffsetToFirstName;
	NameArray=new unsigned char[Size];
	if (!ReadFile(File, NameArray,Size,&BytesRead, NULL))
	{
		CloseHandle(File);
		fout<<"Error Reading Names\n";
		return 0;
	}

	if(HeadInfo) PrintHeader();

	fout.setf(std::ios::fixed,std::ios::floatfield);			// fstream crap
	fout.setf(std::ios::showpoint);						// more fstream crap

	if(TAK) fout<<"#define TAK			// This is a TAK script\n\n";
	else fout<<"#define TA			// This is a TA script\n\n";

	fout<<"#include \"sfxtype.h\"\n#include \"exptype.h\"\n\n";

	if(Header.NumberOfPieces) fout<<"piece  ";
	for(x=0;x<Header.NumberOfPieces;x++)
	{
		fout<<NameArray+GetPieceNameOffset(x);
		if(x<(Header.NumberOfPieces-1))
			if(((x+1)%8)==0) fout<<",\n       ";
			else fout<<", ";
		else fout<<";\n\n";
	}

	Var var;
	CString temp="Static_Var_";
	for(x=0;x<Header.NumberOfStaticVars;x++)
	{
		ltoa(x+1,var.Name.GetBuffer(1),10);
		var.Name.ReleaseBuffer();
		var.Name=temp+var.Name;
		var.Type=1;
		StaticVar.Add(var);
	}

	if(Guess||StdLibs)
	{
		GotOYard=false;
		GotCYard=false;
		if(TAK)
		{
			MakeSomeGuesses_TAK(Header.OffsetToScriptCode,
								Header.LengthOfScripts,
								Header.NumberOfScripts);
		}
		else
		{
			MakeSomeGuesses_TA( Header.OffsetToScriptCode,
								Header.LengthOfScripts,
								Header.NumberOfScripts);
		}
	}

	if(StaticVar.GetSize()) fout<<"static-var  "<<std::dec;
	for(x=0;x<StaticVar.GetSize();x++)
	{
		fout<<StaticVar[x].Name;
		if(x<(StaticVar.GetSize()-1))
			if(((x+1)%5)==0) fout<<",\n            ";
			else fout<<", ";
		else fout<<";\n\n";
	}

if(PrintSoundList)
{
	if( (TAK)&&(Header.NumberOfSounds>0) )
	{
		fout<<"sound  "<<std::dec;
		for(x=0;x<Header.NumberOfSounds;x++)
		{
			fout<<NameArray+GetSoundNameOffset(x);
			if(x<(Header.NumberOfSounds-1))
				if(((x+1)%5)==0) fout<<",\n            ";
				else fout<<", ";
			else fout<<";\n\n";
		}
	}
}

	if(Smoke_Flame.GetSize()>0)
	{
		fout<<"// Smoke and Flame definitions\n";
		for(x=0;x<Smoke_Flame.GetSize();x++)
		{
			fout<<"#define ";
			if(Smoke_Flame[x].Name.GetLength()<25)
				fout<<Smoke_Flame[x].Name<<"	";
			else fout<<Smoke_Flame[x].Name;
			fout<<"			"<<Smoke_Flame[x].Val;
			if(Smoke_Flame[x].Name=="MAX_DAMAGE_FLAMES")
				fout<<" // You must have this number of damage pieces";
			fout<<std::endl;
		}
		fout<<std::endl;
	}

	if(Signals.GetSize()>0)
	{
		fout<<"// Signal definitions\n";
		for(x=0;x<Signals.GetSize();x++)
		{
			fout<<"#define ";
			if(Signals[x].Name.GetLength()<8)
				fout<<Signals[x].Name<<"	";
			else fout<<Signals[x].Name;
			fout<<"			"<<Signals[x].Val<<std::endl;
		}
		fout<<std::endl;
	}

	if( (NumSmokePieces!=0) && (StdLibs) )
	{
		for(x=0;x<NumSmokePieces;x++)
		{
			fout << "#define "
				 << "SMOKEPIECE" << (x+1) << " "
				 << PrintPieceName(SmokePieces[x]) 
				 << std::endl;
		}
		fout<<std::endl;
	}

	if( GotStates && !TAK )
	{
		fout<<"#include \"StateChg.h\"\n";
	}

	if(Prog) Prog->SetRange(0,Header.NumberOfScripts);
	if(Prog) Prog->SetStep(1);

	Pos=0;
	for(x=0;x<Header.NumberOfScripts;x++)
	{
		Offset=Header.OffsetToScriptCode + (OffsetArray[x] * 4);
		if(x<(Header.NumberOfScripts-1)) 
			Size=(Header.OffsetToScriptCode + (OffsetArray[x+1] * 4))-Offset;
		else Size=(Header.OffsetToScriptCode+(Header.LengthOfScripts*4))-Offset;
			//Size=Header.OffsetToScriptCodeIndexArray-Offset;
		ScriptBuf=new char[Size];
		SetFilePointer(File,Offset,0,FILE_BEGIN);
		if (!ReadFile(File,ScriptBuf,Size,&BytesRead, NULL))
		{
			CloseHandle(File);
			std::cout<<"Error Reading Script "<<Offset<<" "<<OffsetArray[0]<<" "<<OffsetArray[1];
			return 0;
		}
		Pos=ProcessScript((LPSTR)NameArray+GetScriptNameOffset(x),ScriptBuf,Pos,(Size/4)+Pos);
		
		delete [] ScriptBuf;
		if(Prog) Prog->StepIt();
	}

	StaticVar.RemoveAll();
	Signals.RemoveAll();

	/*fout<<std::endl;
	for(x=0;x<Common_Funcs.GetSize();x++)
	{
		fout<<Common_Funcs[x].Name<<"	"
			<<Common_Funcs[x].Game<<std::endl;
	}*/

	fout.close();

	if(Prog) Prog->SetPos(0);
	return 1;
}

void CCobCodec::AddSignal(UNITVAL Signal)
{
	int x,place=(-1);

	for(x=0;x<Signals.GetSize();x++)
		if(Signal.Val==Signals[x].Val) return;

	for(x=0;x<Signals.GetSize();x++)
	{
		if(Signal.Val<Signals[x].Val)
		{
			place=x;
			break;
		}
	}
	Signals.Add(Signal);
	if(place==(-1)) return;
	for(x=Signals.GetSize()-1;x>place;x--)
		Signals[x]=Signals[x-1];
	Signals[place]=Signal;
}

int CCobCodec::MakeSomeGuesses_TA(long Offset,long Length,long NumScripts)
{
	unsigned long BR;
	long Size=Length*4;
	long End;
	long* ScriptBuf;
	long Spot;
	int x,Ret=(-1);
	UNITVAL Signal;
	CArray <CString,CString&> Scripts;

	ScriptBuf=new long[Length];
	SetFilePointer(File,Offset,0,FILE_BEGIN);
	if (!ReadFile(File,ScriptBuf,Size,&BR, NULL))
	{
		CloseHandle(File);
		std::cout<<"Error Reading Script "<<Offset<<" "<<OffsetArray[0]<<" "<<OffsetArray[1];
		return 0;
	}

		// Look for various headers
/*-------------------------------------------------*/
	bool IsOYard=false;
	bool IsCYard=false;
	bool IsInitState=false;
	bool IsChangeState=false;
	bool IsGo=false;
	bool IsStop=false;

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"OpenYard",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0) IsOYard=true;	
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"CloseYard",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0) IsCYard=true;
	Ret=(-1);

	if(IsCYard && IsOYard) IsYardH=true;

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"InitState",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0) IsInitState=true;
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"RequestState",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0) IsChangeState=true;
	Ret=(-1);

	// Find SmokeUnit
/*-------------------------------------------------*/
	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"SmokeUnit",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_STACK_ALC)&&
				(ScriptBuf[Spot+1]==CMD_STACK_ALC)&&
				(ScriptBuf[Spot+2]==CMD_STACK_ALC) )
			{
				if( ScriptBuf[Spot+3]!=CMD_STACK_ALC )
				{
					NumSmokePieces = 1;
				}
				else NumSmokePieces = -1;
				break;
			}
		}// End for
		if(NumSmokePieces==1)
		{
			for(Spot=OffsetArray[Ret];Spot<End;Spot++)
			{
				if( (ScriptBuf[Spot]==CMD_EMIT_SFX) )
				{
					SmokePieces.Add( ScriptBuf[Spot+1] );
					break;
				}
			} // End for
		}
		else if(NumSmokePieces==(-1))
		{
			NumSmokePieces = 0;
			for(Spot=OffsetArray[Ret];Spot<End;Spot++)
			{
				if( (ScriptBuf[Spot]==CMD_EMIT_SFX) )
				{
					NumSmokePieces++;
					SmokePieces.Add( ScriptBuf[Spot+1] );
				}
			} // End for
		}
	}// End if
	Ret=(-1);

	if(!Guess)
	{
		goto Exit;
	}

	// Find Static-Vars
/*-------------------------------------------------*/

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"FirePrimary",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="gun_1";
				StaticVar[ScriptBuf[Spot+1]].Type=5;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"FireSecondary",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="gun_2";
				StaticVar[ScriptBuf[Spot+1]].Type=5;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"FireTertiary",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="gun_3";
				StaticVar[ScriptBuf[Spot+1]].Type=5;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"AimPrimary",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="bAiming";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"StartMoving",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="bMoving";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"QueryNanoPiece",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-1]==OP_NOT)&&
				(ScriptBuf[Spot-3]==CMD_PUSH_SVAR)&&
				(ScriptBuf[Spot-2]==ScriptBuf[Spot+1]) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="spray";
				StaticVar[ScriptBuf[Spot+1]].Type=5;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	if( !StdLibs )
	{
	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"InitState",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="statechg_DesiredState";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]==0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="statechg_StateChanging";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);
	}

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"SetMaxReloadTime",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="restore_delay";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"Demo",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="unitviewer";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	// Find Signals
/*-------------------------------------------------*/

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"StartMoving",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_MOVE";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"AimPrimary",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_AIM";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"AimSecondary",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_AIM_2";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"AimTertiary",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_AIM_3";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"Activate",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_ACTIVATE";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

Exit:
// Find State Variables
/*-------------------------------------------------*/

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"RequestState",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_CALL_SCRPT)&&
				(ScriptBuf[Spot+2]==0) )
			{
				NumGo = ScriptBuf[Spot+1];
				break;
			} // End if
		}// End for
		for(Spot++;Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_CALL_SCRPT)&&
				(ScriptBuf[Spot+2]==0) )
			{
				NumStop = ScriptBuf[Spot+1];
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_PUSH_SVAR)&&
				(ScriptBuf[Spot+2]==CMD_JUMP_IF)&&
				(ScriptBuf[Spot+4]==CMD_PUSH_FVAR)&&
				(ScriptBuf[Spot+5]==0)&&
				(ScriptBuf[Spot+6]==CMD_POP_SVAR) )
			{
				if(StdLibs && IsInitState && IsChangeState && (NumGo>=0) && (NumStop>=0))
				{
					StaticVar.RemoveAt(ScriptBuf[Spot+1]);
					StaticVar.RemoveAt(ScriptBuf[Spot+7]);
					GotStates = true;
				}
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	if(ScriptBuf) delete [] ScriptBuf;
	return 1;
}

int CCobCodec::MakeSomeGuesses_TAK(long Offset,long Length,long NumScripts)
{
	unsigned long BR;
	long Size=Length*4;
	long End;
	long* ScriptBuf;
	long Spot;
	int x,Ret=(-1);
	UNITVAL Signal;
	CArray <CString,CString&> Scripts;

	ScriptBuf=new long[Length];
	SetFilePointer(File,Offset,0,FILE_BEGIN);
	if (!ReadFile(File,ScriptBuf,Size,&BR, NULL))
	{
		CloseHandle(File);
		std::cout<<"Error Reading Script "<<Offset<<" "<<OffsetArray[0]<<" "<<OffsetArray[1];
		return 0;
	}

	// Look for various headers
/*-------------------------------------------------*/
	bool IsOYard=false;
	bool IsCYard=false;
	bool IsInitState=false;
	bool IsChangeState=false;
	bool IsGo=false;
	bool IsStop=false;

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"OpenYard",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0) IsOYard=true;	
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"CloseYard",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0) IsCYard=true;
	Ret=(-1);

	if(IsCYard && IsOYard) IsYardH=true;

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"InitState",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0) IsInitState=true;
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"RequestState",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0) IsChangeState=true;
	Ret=(-1);


	// Find SmokeUnit and FlameUnit Stuff
/*-------------------------------------------------*/
	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"SmokeControl",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_FVAR)&&
				(ScriptBuf[Spot+1]==3)&&
				(ScriptBuf[Spot-1]==OP_RANDOM)&&
				(ScriptBuf[Spot-4]==0) )
			{
				if(!HaveMaxDFlames)
				{
					HaveMaxDFlames=true;
					Signal.Name="MAX_DAMAGE_FLAMES";
					Signal.Val=ScriptBuf[Spot-2];
					Smoke_Flame.Add(Signal);
				}
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_FVAR)&&
				(ScriptBuf[Spot+1]==2)&&
				(ScriptBuf[Spot-1]==OP_DIVIDE)&&
				(ScriptBuf[Spot-4]==1000) )
			{
				if(!HaveMaxSmokePuffs)
				{
					HaveMaxSmokePuffs=true;
					Signal.Name="MAX_SMOKE_PUFFS_PER_SECOND";
					Signal.Val=ScriptBuf[Spot-2];
					Smoke_Flame.Add(Signal);
				}
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"DamageFlameControl",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_FVAR)&&
				(ScriptBuf[Spot+1]==3)&&
				(ScriptBuf[Spot-1]==OP_MULTIPLY) )
			{
				if(!HaveMaxDFlames)
				{
					HaveMaxDFlames=true;
					Signal.Name="MAX_DAMAGE_FLAMES";
					Signal.Val=ScriptBuf[Spot-4];
					Smoke_Flame.Add(Signal);
				}
				if(!HaveMaxDFLevel)
				{
					HaveMaxDFLevel=true;
					Signal.Name="MAX_DAMAGE_FLAME_LEVEL";
					Signal.Val=ScriptBuf[Spot-2];
					Smoke_Flame.Add(Signal);
				}
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	if(!Guess)
	{
		goto Exit;
	}

	// Find Static-Vars
/*-------------------------------------------------*/

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"StatusControl",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-1]==OP_GET_UNITVAL)&&
				(ScriptBuf[Spot-3]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-2]==32) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="vetlevel";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"MoveWatcher",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="moving";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"MoveRate",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="moving";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0)&&
				(ScriptBuf[Spot-5]==CMD_STRT_SCRPT)&&
				(strnicmp(PrintScriptName(ScriptBuf[Spot-4]),"WakeControl",MAX_PATH)==0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="wakes";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"FireWeapon",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="firing";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-1]==OP_RANDOM) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="ran_attack";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"AimWeapon",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="aiming";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"TargetCleared",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="kill_attack";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"Dying",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="busy_dying";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"SetMaxReloadTime",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="restore_delay";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"RestoreAfterDelay",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SLEEP)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_SVAR) )
			{
				StaticVar[ScriptBuf[Spot-1]].Name="restore_delay";
				StaticVar[ScriptBuf[Spot-1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"MotionControl",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-1]==OP_GET_UNITVAL)&&
				(ScriptBuf[Spot-3]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-2]==29) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="velocity";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0)&&
				(ScriptBuf[Spot-5]==CMD_CALL_SCRPT)&&
				(strnicmp(PrintScriptName(ScriptBuf[Spot-4]),"restore",MAX_PATH)==0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="restored";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if( (strnicmp(PrintScriptName(x),"MeleeControl",MAX_PATH)==0)||
			(strnicmp(PrintScriptName(x),"DemonControl",MAX_PATH)==0)||
			(strnicmp(PrintScriptName(x),"RestoreWatcher",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0)&&
				(ScriptBuf[Spot-5]==CMD_CALL_SCRPT)&&
				(strnicmp(PrintScriptName(ScriptBuf[Spot-4]),"restore_legs",MAX_PATH)==0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="legs_restored";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0)&&
				(ScriptBuf[Spot-5]==CMD_CALL_SCRPT)&&
				((strnicmp(PrintScriptName(ScriptBuf[Spot-4]),"restore_x",MAX_PATH)==0)||
				 (strnicmp(PrintScriptName(ScriptBuf[Spot-4]),"restore",MAX_PATH)==0)) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="restored";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"setSFXoccupy",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-4]==CMD_JUMP_IF) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="airborne";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"beginflight",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="fly_var";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"flightcontrol",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0)&&
				(ScriptBuf[Spot-4]==CMD_JUMP_IF)&&
				(ScriptBuf[Spot-6]==CMD_PUSH_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="flying";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"land",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="landing";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"row",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="oarsdown";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"stop",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]==0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="ready";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"TurnDirection",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_FVAR)&&
				(ScriptBuf[Spot-1]==0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="direction";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"WindChange",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="winddir";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"Create",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-1]==OP_GET_UNITVAL)&&
				(ScriptBuf[Spot-3]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-2]==27) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="buildangle";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_INTEGER;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"StartBuilding",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="building";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0)&&
				(ScriptBuf[Spot+2]==CMD_CALL_SCRPT)&&
				(strnicmp(PrintScriptName(ScriptBuf[Spot+3]),"startbuild",MAX_PATH)==0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="pause_loop";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if( !GotStates && (strnicmp(PrintScriptName(x),"InitState",MAX_PATH)==0) )
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="statechg_DesiredState";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_POP_SVAR)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]==0) )
			{
				StaticVar[ScriptBuf[Spot+1]].Name="statechg_StateChanging";
				StaticVar[ScriptBuf[Spot+1]].Type=TYP_BOOL;
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	// Find Signals
/*-------------------------------------------------*/

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"AimWeapon",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_AIM";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"RestoreAfterDelay",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_RESTORE";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
	if( (strnicmp(PrintScriptName(x),"MoveWatcher",MAX_PATH)==0)||
		(strnicmp(PrintScriptName(x),"MoveRate",MAX_PATH)==0) )
		Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIG_MSK)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_MOVE";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"TurnDirection",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_TURN";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"WakeControl",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIG_MSK)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_WAKE";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"HitByWeapon",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_ROLL";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"WindChange",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIGNAL)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_WIND";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"land",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIG_MSK)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_LANDING";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIG_MSK)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0)&&
				((ScriptBuf[Spot+3]==CMD_JUMP_IF)||(ScriptBuf[Spot+6]==CMD_JUMP_IF)) )
			{
				Signal.Name="SIG_FALLING";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"BeginLanding",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_SIG_MSK)&&
				(ScriptBuf[Spot-2]==CMD_PUSH_CON)&&
				(ScriptBuf[Spot-1]!=0) )
			{
				Signal.Name="SIG_ABORT";
				Signal.Val=ScriptBuf[Spot-1];
				AddSignal(Signal);
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

Exit:
	// Find State Variables
/*-------------------------------------------------*/

	for(x=0;x<NumScripts;x++)
		if(strnicmp(PrintScriptName(x),"RequestState",MAX_PATH)==0) 
			Ret=x;
	if(Ret>=0)
	{
		if(Ret+1==NumScripts) End=Length;
		else End=OffsetArray[Ret+1];
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_CALL_SCRPT)&&
				(ScriptBuf[Spot+2]==0) )
			{
				NumGo = ScriptBuf[Spot+1];
				break;
			} // End if
		}// End for
		for(Spot++;Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_CALL_SCRPT)&&
				(ScriptBuf[Spot+2]==0) )
			{
				NumStop = ScriptBuf[Spot+1];
				break;
			} // End if
		}// End for
		for(Spot=OffsetArray[Ret];Spot<End;Spot++)
		{
			if( (ScriptBuf[Spot]==CMD_PUSH_SVAR)&&
				(ScriptBuf[Spot+2]==CMD_JUMP_IF)&&
				(ScriptBuf[Spot+4]==CMD_PUSH_FVAR)&&
				(ScriptBuf[Spot+5]==0)&&
				(ScriptBuf[Spot+6]==CMD_POP_SVAR) )
			{
				if(StdLibs && IsInitState && IsChangeState && (NumGo>=0) && (NumStop>=0))
				{
					StaticVar.RemoveAt(ScriptBuf[Spot+1]);
					StaticVar.RemoveAt(ScriptBuf[Spot+7]);
					GotStates = true;
				}
				break;
			} // End if
		}// End for
	}// End if
	Ret=(-1);

	if(ScriptBuf) delete [] ScriptBuf;
	return 1;
}

LRESULT CCobCodec::SetAttr(DECOM_SETTINGS& Set)
{
	Offsets=Set.ShowOffsets;
	HeadInfo=Set.ShowHeader;
	ShowPush=Set.ShowPush;
	Guess=Set.DoGuess;
	FloatPrec=Set.Precision;
	PrintSoundList=Set.ShowSndList;
	StdLibs=Set.ShowStdLib;
	MiscLibs=Set.ShowMiscLib;
	AConst=Set.AngConst;
	LConst=Set.LinConst;
	IncDec=Set.IncDec;
	TryKeepSmall=Set.TryKeepSmall;
	ShowReturn=Set.ShowReturn;
	return 1;
}

int CCobCodec::CheckCommonFunc(LPSTR Name,int numvar)
{
	int Game;
	int x,y;
	Var var;

	if(TAK) Game=2;
	else Game=1;
	for(x=0;x<Common_Funcs.GetSize();x++)
		if( (Common_Funcs[x].Name.CompareNoCase(Name)==0)
		  &&( (Common_Funcs[x].Game==Game)||(Common_Funcs[x].Game==0) )
		  &&(numvar>=Common_Funcs[x].NumArgs) )
		{
			for(y=0;y<Common_Funcs[x].NumArgs;y++)
			{
				var.Name=Common_Funcs[x].Args[y].Name;
				var.Type=Common_Funcs[x].Args[y].Type;
				FuncVar.Add(var);
			}
			return Common_Funcs[x].NumArgs;
		}
	CString temp="Func_Var_";
	for(x=0;x<numvar;x++)
	{
		ltoa(x+1,var.Name.GetBuffer(1),10);
		var.Name.ReleaseBuffer();
		var.Name=temp+var.Name;
		var.Type=1;
		FuncVar.Add(var);
	}
	return numvar;
}

bool IsHex(char c)
{
	if( ((c>='0')&&(c<='9'))
	  ||((c>='A')&&(c<='F'))
	  ||((c>='a')&&(c<='f')) ) return true;
	return false;
}

int CCobCodec::GetINIInfo()
{
	HANDLE INI;
	unsigned long BR;
	long Size;
	unsigned char* start;
	unsigned char* buf;
	CString str1,str2;
	OPERATOR Operator;
	UNITVAL UnitVal;
	COM_FUNC ComFunc;
	bool out=false;
	int x;

	UnitValues.SetSize(0);
	Operators.SetSize(0);
	INI=CreateFile("Decompiler.cfg",
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS),
					NULL);
	if(INI == INVALID_HANDLE_VALUE) return 0;
	Size=GetFileSize(INI,0);
	buf=new unsigned char[Size];
	if( !ReadFile(INI,buf,Size,&BR,NULL) ) goto EXIT;
	start=buf;

	for(str1="";(str1.CompareNoCase("OPERATORS")!=0);buf++)
	{
		for(;*buf!='[';buf++) if( (buf-start)>=Size ) goto EXIT;
		buf++;
		for(str1="";*buf!=']';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
	}
	for(out=false;;)
	{
		for(;*buf!='{';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			if(*buf=='[') 
			{
				out=true; 
				break;
			}
		}
		if(out) break;
		buf++;
		for(;*buf!='"';buf++) if( (buf-start)>=Size ) goto EXIT;
		buf++;
		for(str1="";*buf!='"';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
		Operator.Op=str1;
		buf++;
		for(;*buf!='=';buf++) if( (buf-start)>=Size ) goto EXIT;
		for(;!IsHex(*buf);buf++) if( (buf-start)>=Size ) goto EXIT;
		for(str2="";IsHex(*buf);buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str2+=*buf;
		}
		Operator.Val=strtol(str2,0,16);
		buf++;
		for(;!( (*buf>='0')&&(*buf<='9') );buf++) if( (buf-start)>=Size ) goto EXIT;
		for(str2="";( (*buf>='0')&&(*buf<='9') );buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str2+=*buf;
		}
		Operator.Priority=strtol(str2,0,10);
		for(;*buf!='}';buf++) if( (buf-start)>=Size ) goto EXIT;
		Operators.Add(Operator);
	}

	for(str1="";(str1.CompareNoCase("UNITVLAUES")!=0);buf++)
	{
		for(;*buf!='[';buf++) if( (buf-start)>=Size ) goto EXIT;
		buf++;
		for(str1="";*buf!=']';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
	}
	for(out=false;;)
	{
		for(;*buf!='"';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			if(*buf=='[')
			{
				out=true; 
				break;
			}
		}
		if(out) break;
		buf++;
		for(str1="";*buf!='"';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
		buf++;
		for(;!( (*buf>='0')&&(*buf<='9') );buf++) if( (buf-start)>=Size ) goto EXIT;
		for(str2="";( (*buf>='0')&&(*buf<='9') );buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str2+=*buf;
		}
		UnitVal.Name=str1;
		UnitVal.Val=strtol(str2,0,10);
		UnitValues.Add(UnitVal);
	}

	for(str1="";(str1.CompareNoCase("ANIM_SCRIPTS")!=0);buf++)
	{
		for(;*buf!='[';buf++) if( (buf-start)>=Size ) goto EXIT;
		buf++;
		for(str1="";*buf!=']';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
	}
	for(out=false;;)
	{
		for(;*buf!='"';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			if(*buf=='[')
			{
				out=true; 
				break;
			}
		}
		if(out) break;
		buf++;
		for(str1="";*buf!='"';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
		buf++;
		AnimFunc.Add(str1);
	}

	for(str1="";(str1.CompareNoCase("COMMON_FUNC")!=0);buf++)
	{
		for(;*buf!='[';buf++) if( (buf-start)>=Size ) goto EXIT;
		buf++;
		for(str1="";*buf!=']';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
	}
	for(out=false;;)
	{
		for(;*buf!='{';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			if(*buf=='[') 
			{
				out=true; 
				break;
			}
		}
		if(out) break;
		buf++;
		for(;*buf!='"';buf++) if( (buf-start)>=Size ) goto EXIT;
		buf++;
		for(str1="";*buf!='"';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
		ComFunc.Name=str1;
		buf++;
		for(;*buf!='=';buf++) if( (buf-start)>=Size ) goto EXIT;
		buf++;
		for(str1="";*buf!=';';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
		if(str1=="TA") ComFunc.Game=1;
		else if(str1=="TAK") ComFunc.Game=2;
		else ComFunc.Game=0;
		buf++;
		for(;*buf!='=';buf++) if( (buf-start)>=Size ) goto EXIT;
		buf++;
		for(str1="";*buf!=';';buf++)
		{
			if( (buf-start)>=Size ) goto EXIT;
			str1+=*buf;
		}
		buf++;
		ComFunc.NumArgs=strtol(str1,0,10);
		if(ComFunc.NumArgs) ComFunc.Args=new ARG[ComFunc.NumArgs];
		else ComFunc.Args=0;
		for(x=0;x<ComFunc.NumArgs;x++)
		{
			for(;*buf!='"';buf++) if( (buf-start)>=Size ) goto EXIT;
			buf++;
			for(str1="";*buf!='"';buf++)
			{
				if( (buf-start)>=Size ) goto EXIT;
				str1+=*buf;
			}
			ComFunc.Args[x].Name=str1;
			for(;!IsHex(*buf);buf++) if( (buf-start)>=Size ) goto EXIT;
			for(str2="";IsHex(*buf);buf++)
			{
				if( (buf-start)>=Size ) goto EXIT;
				str2+=*buf;
			}
			ComFunc.Args[x].Type=strtol(str2,0,10);
		}
		for(;*buf!='}';buf++) if( (buf-start)>=Size ) goto EXIT;
		Common_Funcs.Add(ComFunc);
	}
	
EXIT:
	if(start) delete [] start;
	CloseHandle(INI);
	return 1;
}

void CCobCodec::PrintHeader()
{
	fout<<"//            * Header Info *\n";
	fout<<"// VersionSignature                     "
		<<std::dec<<Header.VersionSignature<<"\n";
	fout<<"// NumberOfScripts                     "
		<<Header.NumberOfScripts<<"\n";
	fout<<"// NumberOfPieces                      "
		<<Header.NumberOfPieces<<"\n";
	fout<<"// LengthOfScripts                     "
		<<std::hex<<Header.LengthOfScripts<<"\n";
	fout<<"// NumberOfStaticVars                  "
		<<std::dec<<Header.NumberOfStaticVars<<"\n";
	fout<<"// UKZero                              "
		<<Header.UKZero<<"\n";
	fout<<"// OffsetToScriptCodeIndexArray        "
		<<std::hex<<Header.OffsetToScriptCodeIndexArray<<"\n";
	fout<<"// OffsetToScriptNameOffsetArray       "
		<<Header.OffsetToScriptNameOffsetArray<<"\n";
	fout<<"// OffsetToPieceNameOffsetArray        "
		<<Header.OffsetToPieceNameOffsetArray<<"\n";
	fout<<"// OffsetToScriptCode                  "
		<<Header.OffsetToScriptCode<<"\n";
	fout<<"// OffsetToNameArray                   "
		<<Header.OffsetToNameArray<<"\n";
	if(TAK)
	{
	fout<<"// OffsetToSoundNameArray              "
		<<Header.OffsetToSoundNameArray<<"\n";
	fout<<"// NumberOfSounds                      "
		<<std::dec<<Header.NumberOfSounds<<"\n";
	}
	fout<<"\n"
		<<"// OffsetToFirstName                   "
		<<std::hex<<OffsetToFirstName
		<<"\n\n";
}
