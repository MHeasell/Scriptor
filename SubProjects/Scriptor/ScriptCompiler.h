#ifndef _EXPORTS_H
#define _EXPORTS_H

#undef EXPORT_MODE
#ifdef _MY_DLL
#define EXPORT_MODE __declspec(dllexport)
#else
#define EXPORT_MODE __declspec(dllimport)
#endif

typedef struct _SETTINGS
{
	LPTSTR CurrentFile;
	LPTSTR CurrentDirectory;
	LPTSTR IncludeDirectory;
	float AngularConst;
	float LinearConst;
	int TA;
} SETTINGS;

typedef LRESULT(CALLBACK* PROGRESSCALLBACK)(DWORD BytesRead, DWORD TotalBytes);
typedef LRESULT(CALLBACK* OUTPUTWINCALLBACK)(LPCTSTR File, LPCTSTR ErrStr, WORD Line, char ErrType);

EXPORT_MODE LPVOID BOSCOM_Initialize();
EXPORT_MODE LPCTSTR BOSCOM_GetKWList(LPVOID pBosCom, int Index, int& Next);
EXPORT_MODE LRESULT BOSCOM_Compile(LPVOID pBosCom, LPCTSTR BosPath, LPCTSTR CobPath, SETTINGS Settings, OUTPUTWINCALLBACK OutputWinCallback, PROGRESSCALLBACK ProgressCallback);
EXPORT_MODE LRESULT BOSCOM_Destroy(LPVOID pBosCom);

#endif