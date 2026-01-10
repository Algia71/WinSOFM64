/*
** iniutil.h
**
** Copyright 1995 Francesco Bellifemine, Donato Dalbis, Alessandro Giancane
*/

#ifndef _INIUTIL_DEF
#define _INIUTIL_DEF

BOOL   INIGetSaveOnExit( VOID );
VOID   INISetSaveOnExit( BOOL );
BOOL   INIGetShowIdeal( VOID );
VOID   INISetShowIdeal( BOOL );
BOOL   INIGetDrawMap( VOID );
VOID   INISetDrawMap( BOOL );
BOOL   INIGetAvrgWeightsInit( VOID );
VOID   INISetAvrgWeightsInit( BOOL );
WORD   INIGetDrawCycle( VOID );
VOID   INISetDrawCycle( WORD );
DWORD  INIGetIterations( VOID );
VOID   INISetIterations( DWORD );
double INIGetAlpha( VOID );
VOID   INISetAlpha( double );
double INIGetKappa( VOID );
VOID   INISetKappa( double );
int    INIGetA( VOID );
VOID   INISetA( int );
double INIGetAcca( VOID );
VOID   INISetAcca( double );

#endif
