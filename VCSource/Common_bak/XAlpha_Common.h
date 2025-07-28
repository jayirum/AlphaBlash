#pragma once



namespace _XAlpha
{

#define CODE_PWD		"01"
#define CODE_LOGONOFF	"02"
#define CODE_CNTR		"03"
#define CODE_MSG		"04"
#define CODE_CNTR_HIST	"05"
#define CODE_TICK_KOSPI	"11"
#define CODE_TICK_OV	"12"

#define DEF_STX	0x02
#define DEF_ETX	0x03

#define RETCODE_PWD_OK		"00"
#define RETCODE_PWD_WRONG	"01"
#define RETCODE_CNTR_NODATA	"02"

#define DEF_ENTER	0x0a

#define SIDE_BUY	'B'
#define SIDE_SELL	'S'


	struct TCL_PWD
	{
		char Code[2];
		char Pwd[20];
	};

	struct TCL_RQST_CNTR_HIST
	{
		char	Code[2];
		char	MasterID[20];
	};

	struct TRET_MSG
	{
		char	Code[2];
		char	RetCode[2];
		char	Enter[1];
	};

	

	struct TCNTR
	{
		//char	STX[1];
		//char	Len[4];
		char	Code[2];
		char	oldDataYN[1];
		char	masterId[20];
		char	cntrNo[5];
		char	stkCd[10];
		char	bsTp[1];
		char	cntrQty[3];
		char	cntrPrc[10];
		char	clrPl[10];
		char	cmsn[5];
		char	clrTp[1];
		char	bf_nclrQty[3];
		char	af_nclrQty[3];
		char	bf_avgPrc[10];
		char	af_avgPrc[10];
		char	bf_amt[10];
		char	af_amt[10];
		char	ordTp[2];
		char	tradeTm[12];
		char	lvg[2];
		char	Enter[1];
	};


	// ACNT_MST CONN_YN
	struct TLOGON
	{
		//char	STX[1];
		//char	Len[4];
		char	Code[2];
		char	oldDataYN[1];
		char	masterId[20];
		char	Tm[12];
		char	loginTp[1];	// I/O
		char	masterNm[20];
		//char	ETX[1];
		char	Enter[1];
	};


	struct TTICK
	{
		char Code	[2];
		char stk	[8];
		char close	[15];
		char side[1];
		char time[11];	// ?

		//char open	[15];
		//char high	[15];
		//char low	[15];
		//char gap	[15];
		//char vol	[10];
		//char amt	[11];
		//char ydiffSign[2];
		//char chgrate[6];
		//char execvol[15];
		char Enter[1];
	};

}