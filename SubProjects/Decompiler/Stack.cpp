#include "stdafx.h"
#include "Stack.h"

CStack::CStack() : pos(0), Max(8)
{
	Stack = new StackVal[Max];
}

CStack::~CStack()
{
	if (Stack)
		delete[] Stack;
}

int CStack::Push(StackVal* Stackval)
{
	if (!Stackval)
		return 0;
	Stack[pos] = *Stackval;
	pos++;
	return pos;
}

int CStack::Pop(StackVal* Stackval)
{
	if ((!Stackval) || (pos == 0))
		return 0;
	pos--;
	*Stackval = Stack[pos];
	return 1;
}

int CStack::Peek(StackVal* Stackval)
{
	if ((!Stackval) || (pos == 0))
		return 0;
	*Stackval = Stack[pos - 1];
	return 1;
}
