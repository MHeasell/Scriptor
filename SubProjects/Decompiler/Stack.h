struct StackVal
{
	BYTE Type;
	BYTE Flag;
	long Val;
	StackVal(long val=0,BYTE type=0,BYTE flag=0)
	{
		Val = val;
		//*((BYTE*)(&Flag)) = type;
		Type = type;
		//*(((BYTE*)(&Flag))+1) = flag;
		Flag = flag;
	};
	void SetVal(long val,BYTE type,BYTE flag)
	{
		Val = val;
		//*((BYTE*)(&Flag)) = type;
		Type = type;
		//*(((BYTE*)(&Flag))+1) = flag;
		Flag = flag;
	};
	BYTE IsOpString()
	{ return Type/*LOBYTE( Flag )*/; };
	BYTE Flags()
	{ return Flag/*HIBYTE( Flag )*/; };
};

class CStack
{
private:
	int pos;
	int Max;
	StackVal *Stack;
public:
	CStack();
	~CStack();
	int Size(){return pos;};
	void Reset(){pos=0;};
	int Push(StackVal* Stackval);
	int Pop(StackVal* Stackval);
	int Peek(StackVal* Stackval);
};
