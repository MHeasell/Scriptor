#ifndef _DECOM_EXPORTS_H
#define _DECOM_EXPORTS_H

#undef EXPORT_MODE
#ifdef _MY_DLL
#define EXPORT_MODE __declspec(dllexport)
#else
#define EXPORT_MODE __declspec(dllimport)
#endif

typedef struct _DECOM_SETTINGS
{
	int ShowOffsets;
	int ShowHeader;
	int ShowPush;
	int DoGuess;
	long Precision;
	int ShowSndList;
	int ShowStdLib;
	int ShowMiscLib;
	float AngConst;
	float LinConst;
	int IncDec;
	int TryKeepSmall;
	int ShowReturn;
} DECOM_SETTINGS;

EXPORT_MODE LPVOID BOSDEC_Initialize();
EXPORT_MODE LRESULT BOSDEC_Decompile(LPVOID pBosDec,
	LPCTSTR CobPath,
	LPCTSTR BosPath,
	DECOM_SETTINGS& Set);
EXPORT_MODE LRESULT BOSDEC_Destroy(LPVOID pBosDec);

#endif