/*
** menuhelp.c
**
** Copyright 1995 Francesco Bellifemine, Donato Dalbis, Alessandro Giancane
*/

#include <windows.h>
#include "..\inc\menuhelp.h"


/*
** Constants
*/

#define BUFFER_LEN				256


/*
** Compilation unit's local variables
*/

static TCHAR cBuffer[BUFFER_LEN];


/*
** ShowMenuItemHelp
*/

VOID ShowMenuItemHelp( hInstance, hWnd, wMenuId )
HANDLE hInstance;
HWND   hWnd;
WORD   wMenuId;
{
	int    nOldBkMode, nReturn;
	HDC    hDC;
	RECT   Rect;
	HPEN   hOldPen;
	HFONT  hOldFont;
	DWORD  dwOldBkColor;
	HBRUSH hOldBrush;

	hDC = GetDC( hWnd );
	nOldBkMode = SetBkMode( hDC, TRANSPARENT );
	dwOldBkColor = SetBkColor( hDC, RGB( 192, 192, 192 ) );
	hOldPen = SelectObject( hDC, GetProp( hWnd, TEXT("GrayPen") ) );
	hOldBrush = SelectObject(hDC, GetProp(hWnd, TEXT("GrayBrush")));
	Rectangle( hDC, 289, 229, 455, 270 );
	Rect.left =  290;
	Rect.top = 230;
	Rect.right = 454;
	Rect.bottom = 269;
	hOldFont = SelectObject( hDC, GetProp( hWnd, TEXT("HelpFont") ) );
	nReturn = LoadString( hInstance, wMenuId, cBuffer, BUFFER_LEN );
	if( !nReturn )
		cBuffer[0] = (char)0;
	DrawText( hDC, cBuffer, -1, &Rect, DT_CENTER | DT_NOPREFIX | DT_WORDBREAK | DT_NOCLIP );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldBrush );
	SelectObject( hDC, hOldFont );
	SetBkMode( hDC, nOldBkMode );
	SetBkColor( hDC, dwOldBkColor );
	ReleaseDC( hWnd, hDC );
	return;
}
