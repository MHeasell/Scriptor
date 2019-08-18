#ifndef _EXPORTS_H_
#define _EXPORTS_H_


#define WIZARD_VALIDATION (0x91100119)
typedef struct WIZARD_INFO_t
{
	char Title[64];
	char Description[512];
	DWORD Validation;

} * LPWIZARD_INFO_t;

typedef struct DO_WIZARD_INFO_t
{
	// Input items ( set these before calling DoWizard() )
	char DesiredPath[MAX_PATH];
	char DesiredName[64];

	// Output items
	BOOL bWizardCompleted;
	char FileToOpen[MAX_PATH];

} * LPDO_WIZARD_INFO_t;

#define GET_WIZARD_INFO 1
#define DO_WIZARD 2

void WINAPI GetWizardInfo(LPWIZARD_INFO_t pWizInfo);
void WINAPI DoWizard(LPDO_WIZARD_INFO_t pDoWizInfo);


#endif // !defined (_EXPORTS_H_)