#ifndef _DEFS_H
#define _DEFS_H

#define pLOWORD(val) (WORD*)val
#define pHIWORD(val) ((WORD*)val)+1
#define pLOBYTE(val) (BYTE*)val
#define pHIBYTE(val) ((BYTE*)val)+1

typedef struct _HOLD
{
	long Offset;
	int Locale;
	WORD Flag;
} HOLD;

typedef struct _DELAY
{
	CString Command;
	int Locale;
	WORD Flag;
} DELAY;

typedef struct _SCRIPT
{
	CString Name;
	long Offset;
} SCRIPT;

typedef struct _OPERATOR
{
	CString Op;
	long Val;
	int Priority;
	char LeftOnly;
} OPERATOR;

typedef struct _CONS
{
	CString Name;
	CString Val;
	char Args;
	char Flag;
} CONS;

typedef struct _CMD_FMT
{
	char Expecting;
	WORD Type;
	long Val;
	WORD Error;
	CString Fmt;
	char WhatToDo;
	char CmdBufFlg;
	_CMD_FMT* Next;
} CMD_FMT;

typedef struct _CMD_TYPE
{
	CString Command;
	char Flag;
	CMD_FMT* Format;
} CMD_TYPE;

// Str Types
#define TYP_NUM				2
#define TYP_OPERATOR		4
#define TYP_MISC			8
#define TYP_STRING			16
#define TYP_PREPROC			32
// Additional Types
#define TYP_SCRIPT			64
#define TYP_CONST			128
#define TYP_IGNOREME		256
#define TYP_PIECE			512
#define TYP_SVAR			1024
#define TYP_FVAR			2048
#define TYP_SOUND			4096
#define TYP_CMD				8192
#define TYP_AXIS			16384
#define TYP_VAR				(TYP_SVAR|TYP_FVAR)
#define TYP_IDENT			(TYP_PIECE|TYP_VAR|TYP_SOUND|TYP_CMD|TYP_AXIS|TYP_SCRIPT)
#define TYP_VALUE			(TYP_VAR|TYP_NUM|TYP_PIECE)
#define TYP_UNIQUE			1
#define TYP_LABEL			32768

// NUM Types
#define NTYP_DEC				1
#define NTYP_FLOAT				2
#define NTYP_HEX				4
#define NTYP_OCT				8
#define NTYP_ANGULAR			16
#define NTYP_LINEAR				32

// Command Flags
#define REGULAR					1
#define NOSCRIPT				2
#define NOCMD					4
#define RETURNS_VALUE			8
#define NOTA					16

// Command Format Expecting Flags
#define FMT_STR					1
#define LIST					2
#define EXP_VARIES				3
#define CMD_VAL					4
#define SINGLE					5
#define VALSTR					6
#define NEXT_VARI				7
#define END_VARI				8
#define EXP_ELSE				9
#define BRAK_OPEN				10
#define NEW_STR					11

// Command Format What To Do Flags
#define NOTHING					90
#define END						91
#define ADDFVAR					92
#define ADDSVAR					93
#define ADDSOUND				94
#define ADDTOCMDBUF				95
#define COUNT					96
#define BACK					97
#define INC_LOCALE				98
#define ADDPIECE				99
#define ADD_HOLD				100
#define DELAY_CALL				101
#define END_NOCOUNT				102
#define FILL_HOLDS				103
#define INITCOUNT				104
#define PLACECOUNT				105
#define SAVELAST				106
#define GOTOSAVE				107
#define NODUMPVALBUF			108
#define ADDTOVALBUF				109
#define ENDRETCMD				110
#define DO_PREOP				111
#define KILL_PREOP				112
#define GET_STRS				113
#define FORCE_INSERT			114

// CCobValBuf
#define WAIT			1

// PreOp Flags
#define	INC				1
#define DEC				2
#define UNKNOWN			3
#define NONE			4

// Hold Flags
#define	HTYP_PLACEHOLD	2
#define HTYP_CUROFFSET	4
#define HTYP_ELSEHOLD	8
#define HTYP_LOOP		16
#define HTYP_LASTLOOP	32

// CCobCmdBuf
#define PUT_CMD			1
#define PRE_CMD			2
#define POST_CMD		4
#define SPOT_BEGIN		8
#define SPOT_END		16
#define TWOVALS			32

// Ret Types for CBosSrcParse::HandleRet
#define RET_NOTSPECIAL		1
#define RET_EXIT			2
#define RET_INSERT			3
#define RET_LINE_CMNT		4
#define RET_BLCK_CMNT		5
#define RET_NEXT_PREPROC	6
#define RET_SEEK_ENDIF		7
#define RET_SEEK_NEXT		8
#define RET_NEXT_LINE		9

// Ident Types
#define IDNT_ALL			99
#define IDNT_COMMAND		1
#define IDNT_PIECE			2
#define IDNT_FUNCVAR		3
#define IDNT_STATICVAR		4
#define IDNT_SOUND			5
#define IDNT_CONST			6
#define IDNT_LABEL			7
#define IDNT_AXIS			8

// Constant Types
#define CONST_STDDEF		2
#define CONST_BOSDEF		3
#define CONST_STDMACRO		4
#define CONST_BOSMACRO		5

// PreProccessor IfTypes
#define PP_ITYP_IF			1
#define PP_ITYP_IFDEF		2
#define PP_ITYP_IFNDEF		3
#define PP_ITYP_ELIF		4
#define PP_ITYP_ELSE		5
#define PP_ITYP_ENDIF		6

// Cob Command values
#define CMD_POP_FVAR		0x10023002
#define CMD_POP_SVAR		0x10023004
#define CMD_PUSH_CON		0x10021001
#define CMD_PUSH_FVAR		0x10021002
#define CMD_PUSH_SVAR		0x10021004
#define CMD_STACK_ALLOCATE	0x10022000
#define CMD_RETURN			0x10065000

// Error Types
#define ERT_WARNING			0
#define ERT_ERROR			1

// Error Codes
#define ERR_FORCED			9999
#define ERR_COMPILE			3333
#define ERR_OUTFUNC			1
#define ERR_FMT_PIECE		2
#define ERR_FMT_STD			3
#define ERR_FMT_AX			4
#define ERR_FMT_SCR			5
#define ERR_FMT_SOUND		6
#define ERR_SEMICOLON		7
#define ERR_OPENP			8
#define ERR_CLOSEP			9
#define ERR_OPEN			10
#define ERR_CLOSE			11
#define ERR_OPENBL			12
#define ERR_CLOSEBL			13
#define ERR_DUP				14
#define ERR_EXP_LAB			15
#define ERR_MAX_LAB			16
#define ERR_INFUNC			17
#define ERR_UNOP			18
#define ERR_EXP_IDNT		19
#define ERR_OPENB			20
#define ERR_CLOSEB			21
#define ERR_EXP_UNITVAL		22
#define ERR_MORVAL			23
#define ERR_UNEXP_COM		24
#define ERR_UNEXP_IDNT		25
#define ERR_UNK_IDNT		26
#define ERR_EXP_STR			27
#define ERR_INV_PREP		28
#define ERR_INV_DEF			29
#define ERR_EXP_CON			30
#define ERR_EXP_OP			31
#define ERR_UNEXP_CHAR		32

#endif