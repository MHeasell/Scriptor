#include "stdafx.h"
#include "DeCom_Exports.h"
#include "CobCodec.h"

typedef struct _BOSDEC
{
	CCobCodec* Cob;
} BOSDEC;

LPVOID BOSDEC_Initialize()
{
	BOSDEC* BosDec = new BOSDEC;
	BosDec->Cob = new CCobCodec;
	return BosDec;
}

LRESULT BOSDEC_Decompile(LPVOID pBosDec,
	LPCTSTR CobPath,
	LPCTSTR BosPath,
	DECOM_SETTINGS& Set)
{
	BOSDEC* BosDec = (BOSDEC*)pBosDec;
	BosDec->Cob->OpenCob(CobPath);
	BosDec->Cob->SetAttr(Set);
	BosDec->Cob->MakeBos(BosPath, 0);
	return 1;
}

LRESULT BOSDEC_Destroy(LPVOID pBosDec)
{
	if (!pBosDec)
		return 1;
	BOSDEC* BosDec = (BOSDEC*)pBosDec;
	if (BosDec->Cob)
		delete BosDec->Cob;
	delete BosDec;
	return 1;
}