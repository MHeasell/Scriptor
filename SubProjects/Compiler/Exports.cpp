#include "stdafx.h"
#include "Exports.h"
#include "BosCmdParse.h"
#include "BosSrcParse.h"

typedef struct _BOSCOM
{
	CBosCmdParse* Cmd;
} BOSCOM;

LPVOID BOSCOM_Initialize()
{
	BOSCOM* BosCom = new BOSCOM;
	BosCom->Cmd = new CBosCmdParse;
	return BosCom;
}

LPCTSTR BOSCOM_GetKWList(LPVOID pBosCom, int Index, int& Next)
{
	if (!pBosCom)
	{
		Next = 0;
		return "";
	}
	BOSCOM* BosCom = (BOSCOM*)pBosCom;

	if (Index == 0)
		BosCom->Cmd->PrepareKWList();

	if (Index == (BosCom->Cmd->CmdLen() - 1))
		Next = 0;
	else
		Next = Index + 1;

	return BosCom->Cmd->GetCmd(Index);
}

LRESULT BOSCOM_Compile(LPVOID pBosCom, LPCTSTR BosPath, LPCTSTR CobPath, SETTINGS Settings, OUTPUTWINCALLBACK OutputWinCallback, PROGRESSCALLBACK ProgressCallback)
{
	BOSCOM* BosCom = (BOSCOM*)pBosCom;
	CBosSrcParse BosSrc;
	BosSrc.ParseBos(BosPath, *(BosCom->Cmd), Settings, OutputWinCallback, ProgressCallback);
	return BosCom->Cmd->WriteCob(CobPath);
}

LRESULT BOSCOM_Destroy(LPVOID pBosCom)
{
	if (!pBosCom)
		return 1;
	BOSCOM* BosCom = (BOSCOM*)pBosCom;
	if (BosCom->Cmd)
		delete BosCom->Cmd;
	delete BosCom;
	return 1;
}