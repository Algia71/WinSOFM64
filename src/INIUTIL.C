/*
** iniutil.c
**
** Copyright 1995 Francesco Bellifemine, Donato Dalbis, Alessandro Giancane
*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "..\inc\iniutil.h"


/*
** Costanti
*/

#define APPL_PROGRAM			0
#define APPL_NET				1
#define KEY_SAVEONEXIT			0
#define KEY_SHOWIDEAL			1
#define KEY_DRAWMAP				2
#define KEY_AVRGWEIGHTSINIT		3
#define KEY_DRAWCYCLE			4
#define KEY_ITERATIONS			5
#define KEY_ALPHA               6
#define KEY_KAPPA				7
#define KEY_A					8
#define KEY_ACCA				9

#define DEF_DRAWCYCLE			TEXT("500")
#define DEF_ITERATIONS			TEXT("200000")
#define DEF_ALPHA				TEXT("0.8")
#define DEF_KAPPA				TEXT("5.0")
#define DEF_A					TEXT("10")
#define DEF_ACCA				TEXT("2.0")


/*
** Variabili locali all'unità di compilazione
*/

static TCHAR cIniFileName[] = TEXT("winsofm.ini");
static TCHAR cYes[] = TEXT("yes");
static TCHAR cNo[] = TEXT("no");
static TCHAR *cApplications[] = { TEXT("program"),
	TEXT("net") };
static TCHAR *cKeyNames[] = { TEXT("SaveOnExit"),
	TEXT("ShowIdeal"),
	TEXT("DrawMap"),
	TEXT("AverageWeightsInit"),
	TEXT("DrawCycle"),
	TEXT("Iterations"),
	TEXT("AlphaPar"),
	TEXT("KappaPar"),
	TEXT("APar"),
	TEXT("AccaPar") };


/*
** INIGetSaveOnExit
*/

BOOL INIGetSaveOnExit()
{
	TCHAR cTemp[4];

	GetPrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_SAVEONEXIT], cYes, cTemp, sizeof( cTemp ), cIniFileName );
	if( !lstrcmp( cTemp, cYes ) )
		return( TRUE );
	else
		return( FALSE );
}


/*
** INISetSaveOnExit
*/

VOID INISetSaveOnExit( BOOL bSave )
{
	if( bSave )
		WritePrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_SAVEONEXIT], cYes, cIniFileName );
	else
		WritePrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_SAVEONEXIT], cNo, cIniFileName );
	return;
}


/*
** INIGetShowIdeal
*/

BOOL INIGetShowIdeal()
{
	TCHAR cTemp[4];

	GetPrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_SHOWIDEAL], cYes, cTemp, sizeof( cTemp ), cIniFileName );
	if( !lstrcmp( cTemp, cYes ) )
		return( TRUE );
	else
		return( FALSE );
}


/*
** INISetShowIdeal
*/

VOID INISetShowIdeal( BOOL bShow )
{
	if( bShow )
		WritePrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_SHOWIDEAL], cYes, cIniFileName );
	else
		WritePrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_SHOWIDEAL], cNo, cIniFileName );
	return;
}


/*
** INIGetDrawMap
*/

BOOL INIGetDrawMap()
{
	TCHAR cTemp[4];

	GetPrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_DRAWMAP], cYes, cTemp, sizeof( cTemp ), cIniFileName );
	if( !lstrcmp( cTemp, cYes ) )
		return( TRUE );
	else
		return( FALSE );
}


/*
** INISetDrawMap
*/

VOID INISetDrawMap( BOOL bDraw )
{
	if( bDraw )
		WritePrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_DRAWMAP], cYes, cIniFileName );
	else
		WritePrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_DRAWMAP], cNo, cIniFileName );
	return;
}


/*
** INIGetAvrgWeightsInit
*/

BOOL INIGetAvrgWeightsInit()
{
	TCHAR cTemp[4];

	GetPrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_AVRGWEIGHTSINIT], cNo, cTemp, sizeof( cTemp ), cIniFileName );
	if( !lstrcmp( cTemp, cYes ) )
		return( TRUE );
	else
		return( FALSE );
}


/*
** INISetAvrgWeightsInit
*/

VOID INISetAvrgWeightsInit( BOOL bAvrg )
{
	if( bAvrg )
		WritePrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_AVRGWEIGHTSINIT], cYes, cIniFileName );
	else
		WritePrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_AVRGWEIGHTSINIT], cNo, cIniFileName );
	return;
}


/*
** INIGetDrawCycle
*/

WORD INIGetDrawCycle()
{
	TCHAR cTemp[20];

	GetPrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_DRAWCYCLE], DEF_DRAWCYCLE, cTemp, sizeof( cTemp ), cIniFileName );
	return((WORD)_wtoi(cTemp));
}


/*
** INISetDrawCycle
*/

VOID INISetDrawCycle( WORD wCycle )
{
	TCHAR cTemp[10];

	wsprintf( cTemp, TEXT("%u"), wCycle );
	WritePrivateProfileString( cApplications[APPL_PROGRAM], cKeyNames[KEY_DRAWCYCLE], cTemp, cIniFileName );
	return;
}


/*
** INIGetIterations
*/

DWORD INIGetIterations()
{
	TCHAR cTemp[20];

	GetPrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_ITERATIONS], DEF_ITERATIONS, cTemp, sizeof( cTemp ), cIniFileName );
	return((DWORD)_wtol(cTemp));
}


/*
** INISetIterations
*/

VOID INISetIterations( DWORD dwIterations )
{
	TCHAR cTemp[20];

	wsprintf( cTemp, TEXT("%lu"), dwIterations );
	WritePrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_ITERATIONS], cTemp, cIniFileName );
	return;
}


/*
** INIGetAlpha
*/

double INIGetAlpha()
{
	TCHAR cTemp[40];

	GetPrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_ALPHA], DEF_ALPHA, cTemp, sizeof( cTemp ), cIniFileName );
	return(_wtof(cTemp));
}


/*
** INISetAlpha
*/

VOID INISetAlpha( double dblAlpha )
{
	TCHAR cTemp[10];

	swprintf(cTemp, 10, TEXT("%1.4f"), dblAlpha);
	WritePrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_ALPHA], cTemp, cIniFileName );
    return;
}


/*
** INIGetKappa
*/

double INIGetKappa()
{
	TCHAR cTemp[40];

	GetPrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_KAPPA], DEF_KAPPA, cTemp, sizeof( cTemp ), cIniFileName );
	return(_wtof(cTemp));
}


/*
** INISetKappa
*/

VOID INISetKappa( double dblKappa )
{
	TCHAR cTemp[20];

	swprintf(cTemp, 20, TEXT("%4.4f"), dblKappa);
	WritePrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_KAPPA], cTemp, cIniFileName );
	return;
}


/*
** INIGetA
*/

int INIGetA()
{
	TCHAR cTemp[10];

	GetPrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_A], DEF_A, cTemp, sizeof( cTemp ), cIniFileName );
	return(_wtoi(cTemp));
}


/*
** INISetA
*/

VOID INISetA( int nA )
{
	TCHAR cTemp[10];

	wsprintf( cTemp, TEXT("%d"), nA );
	WritePrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_A], cTemp, cIniFileName );
	return;
}


/*
** INIGetAcca
*/

double INIGetAcca()
{
	TCHAR cTemp[40];

	GetPrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_ACCA], DEF_ACCA, cTemp, sizeof( cTemp ), cIniFileName );
	return(_wtof(cTemp));
}


/*
** INISetAcca
*/

VOID INISetAcca( double dblAcca )
{
	TCHAR cTemp[40];

	swprintf(cTemp, 40, TEXT("%4.4f"), dblAcca);
	WritePrivateProfileString( cApplications[APPL_NET], cKeyNames[KEY_ACCA], cTemp, cIniFileName );
	return;
}
