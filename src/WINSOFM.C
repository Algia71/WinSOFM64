/*
** winsofm.c
**
** Copyright 1996 Francesco Bellifemine, Donato Dalbis, Alessandro Giancane
**
** Utilizzo della SOFM di Kohonen per la quantizzazione vettoriale della
** distribuzione omogenea di punti casuali su un piano.
**
** Programma realizzato con Microsoft QuickC for Windows Version 1.00
*/

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "..\inc\iniutil.h"
#include "..\inc\menuhelp.h"
#include "..\inc\winsofm.h"


/*
** Variabili globali
*/

// Variabili generali
static HWND    hWndMain;			// Handle della finestra principale
static HANDLE  hInst;				// Handle dell'istanza dell'applicazione

// Variabili di controllo
static BOOL	   bSalvaImpostazioni;	// Salva impostazioni in uscita
static BOOL    bDisegnaMappa;		// Disegna la mappa durante il calcolo
static BOOL    bInCammino;			// True = mappa correntemente in calcolo
static BOOL    bInPausa;			// True = mappa correntemente in calcolo ma in pausa
static BOOL    bPesiMedi;			// Inizializza matrice dei pesi con valori casuali medi
static BOOL    bMostraIdeale;		// Mostra la quantizzazione ideale
static WORD    wCicloDisegno;		// Ciclo di ridisegno della mappa
static WORD    wPrevPerc;			// Valore precedente della percentuale di completamento

// Variabili rete neuronale
static int     nNeCorrente;			// Valore di neighbors corrente
static DWORD   dwCicliTotali;		// Cicli totali di apprendimento
static DWORD   dwCicloCorrente;		// Ciclo di apprendimento corrente
static double  dblEtaCorrente;		// Valore di Eta corrente
static double  dblAlpha;			// Vedi formula
static double  dblKappa;			// Vedi formula
static int     nA;					// Vedi formula
static double  dblAcca;				// Vedi formula
static double  dblErrore;			// Errore medio rispetto alla quantizzazione ideale
static double  Weights[NUM_INPUT][NUM_X_NODES][NUM_Y_NODES];	// Matrice dei pesi
static double  TempWeights[NUM_INPUT][NUM_X_NODES][NUM_Y_NODES];// Matrice dei pesi temporanea


/*
** WinMain
*/

int WINAPI wWinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
HINSTANCE hInstance;
HINSTANCE hPrevInstance;
PWSTR  lpszCmdLine;
int    nCmdShow;
{
	MSG    msg;
	int    nRc, nWndx, nWndy, nX, nY, nWidth, nHeight;
	long   nWndunits;
	BOOL   bContinua;
	HANDLE hAcc;

	hInst = hInstance;
	if( !hPrevInstance )
   	{
		if( ( nRc = RegisterClasses() ) == -1 )
       		return( -1 );
   	}
	nWndunits = GetDialogBaseUnits();
	nWndx = LOWORD( nWndunits );
	nWndy = HIWORD( nWndunits );
	nX = ( ( 40 * nWndx ) / 4 );
	nY = ( ( 26 * nWndy ) / 8 );
	nWidth = ( ( 244 * nWndx ) / 4 );
	nHeight = ( ( 196 * nWndy ) / 8 );
	hWndMain = CreateWindow(
		TEXT("WinsofmWClass"),
		TEXT("WinSOFM"),
        WS_CAPTION      |
        WS_SYSMENU      |
        WS_MINIMIZEBOX  |
        WS_BORDER       |
        WS_CLIPCHILDREN |
        WS_OVERLAPPED,
        nX, nY,
        nWidth, nHeight,
        NULL,
        NULL,
        hInst,
        NULL );
	if( !hWndMain )
    	return( -1 );
	ShowWindow( hWndMain, nCmdShow );
	hAcc = LoadAccelerators( hInst, TEXT("WinsofmAcc") );
	bContinua = TRUE;
	while( bContinua )
	{
		if( bInCammino && ( dwCicloCorrente < dwCicliTotali ) )
        {
        	SendMessage( hWndMain, UM_CICLO, 0, 0L );
        	while( PeekMessage( &msg, hWndMain, 0, 0, PM_REMOVE ) )
        	{
		   		if( !TranslateAccelerator( hWndMain, hAcc, &msg ) )
   				{
	    			TranslateMessage( &msg );
    				DispatchMessage( &msg );
    			}
    		}
        }
        else
        {
			if( bContinua = GetMessage( &msg, NULL, 0, 0) )
   			{
	   			if( !TranslateAccelerator( hWndMain, hAcc, &msg ) )
   				{
	    			TranslateMessage( &msg );
    				DispatchMessage( &msg );
    			}
    		}
   		}
   	}
 	return( msg.wParam );
}


/*
** MainWndProc
*/

LRESULT CALLBACK MainWndProc(hWnd, Message, wParam, lParam)
HWND hWnd;
UINT Message;
WPARAM wParam;
LPARAM lParam;
{
	switch( Message )
   	{
		case WM_MENUSELECT:
			{
				ShowMenuItemHelp( hInst, hWnd, wParam );
			}
			break;

    	case WM_COMMAND:
    		{
         		switch( wParam )
           		{
            		case IDM_ESCI:
            			{
            				SendMessage( hWnd, WM_CLOSE, 0, 0L );
            			}
		                break;

            		case IDM_INFORMAZIONI:
		            	{
                  			DLGPROC lpfnInformazioniProc;

							lpfnInformazioniProc = MakeProcInstance((DLGPROC)InformazioniProc, hInst);
							DialogBox( hInst, TEXT("InformazioniDlg"), hWnd, lpfnInformazioniProc );
                  			FreeProcInstance( lpfnInformazioniProc );
                 		}
                 		break;

					case IDM_PARAMETRI:
						{
		            		int     nRc;
                  			DLGPROC lpfnParametriProc;

							lpfnParametriProc = MakeProcInstance((DLGPROC)ParametriProc, hInst);
                  			nRc = DialogBox( hInst, TEXT("ParametriDlg"), hWnd, lpfnParametriProc );
                  			FreeProcInstance( lpfnParametriProc );
                  			if( nRc )
                  			{
								HDC hDC;

								SendMessage( hWnd, UM_INITNETWORK, 0, 0L );
								hDC = GetDC( hWnd );
								DrawSituation( hWnd, hDC );
								ReleaseDC( hWnd, hDC );
                  			}
						}
						break;

					case IDM_RETENEURONALE:
						{
							DLGPROC lpfnReteNeuronaleProc;

							lpfnReteNeuronaleProc = MakeProcInstance((DLGPROC)ReteNeuronaleProc, hInst);
							DialogBox( hInst, TEXT("ReteNeuronaleDlg"), hWnd, lpfnReteNeuronaleProc );
                  			FreeProcInstance( lpfnReteNeuronaleProc );
						}
						break;

					case IDM_SALVAIMPOSTAZIONI:
						{
							HMENU hMenu;

							hMenu = GetMenu( hWnd );
							if( bSalvaImpostazioni )
							{
								bSalvaImpostazioni = FALSE;
								CheckMenuItem( hMenu, IDM_SALVAIMPOSTAZIONI, MF_BYCOMMAND | MF_UNCHECKED );
							}
							else
							{
								bSalvaImpostazioni = TRUE;
								CheckMenuItem( hMenu, IDM_SALVAIMPOSTAZIONI, MF_BYCOMMAND | MF_CHECKED );
							}
						}
						break;

					case IDM_VAI:
						{
							HMENU hMenu;

							bInCammino = TRUE;
							bInPausa = FALSE;
							hMenu = GetMenu( hWnd );
							EnableMenuItem( hMenu, IDM_VAI, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_PASSO, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_PAUSA, MF_ENABLED );
							EnableMenuItem( hMenu, IDM_CONTINUA, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_RIPRISTINA, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_PARAMETRI, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_RETENEURONALE, MF_GRAYED );
							SendMessage( hWnd, WM_SETTEXT, 0, (DWORD)TEXT("WinSOFM - 0%") );
						}
						break;

					case IDM_PASSO:
						{
							HMENU hMenu;

							hMenu = GetMenu( hWnd );
							EnableMenuItem( hMenu, IDM_RIPRISTINA, MF_ENABLED );
							EnableMenuItem( hMenu, IDM_PARAMETRI, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_RETENEURONALE, MF_GRAYED );
							SendMessage( hWnd, UM_CICLO, 0, 0L );
						}
						break;

					case IDM_PAUSA:
						{
							HMENU hMenu;

							bInCammino = FALSE;
							bInPausa = TRUE;
							hMenu = GetMenu( hWnd );
							EnableMenuItem( hMenu, IDM_VAI, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_PASSO, MF_ENABLED );
							EnableMenuItem( hMenu, IDM_PAUSA, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_CONTINUA, MF_ENABLED );
							EnableMenuItem( hMenu, IDM_RIPRISTINA, MF_ENABLED );
						}
						break;

					case IDM_CONTINUA:
						{
							HMENU hMenu;

							bInPausa = FALSE;
							bInCammino = TRUE;
							hMenu = GetMenu( hWnd );
							EnableMenuItem( hMenu, IDM_VAI, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_PASSO, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_PAUSA, MF_ENABLED );
							EnableMenuItem( hMenu, IDM_CONTINUA, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_RIPRISTINA, MF_GRAYED );
						}
						break;

					case IDM_RIPRISTINA:
						{
							HDC hDC;
    						HMENU hMenu;

							bInCammino = FALSE;
							bInPausa = FALSE;
							wPrevPerc = 0;
							hMenu = GetMenu( hWnd );
							EnableMenuItem( hMenu, IDM_VAI, MF_ENABLED );
							EnableMenuItem( hMenu, IDM_PASSO, MF_ENABLED );
							EnableMenuItem( hMenu, IDM_PAUSA, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_CONTINUA, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_RIPRISTINA, MF_GRAYED );
							EnableMenuItem( hMenu, IDM_PARAMETRI, MF_ENABLED );
							EnableMenuItem( hMenu, IDM_RETENEURONALE, MF_ENABLED );
							SendMessage( hWnd, UM_INITNETWORK, 0, 0L );
							hDC = GetDC( hWnd );
							DrawSituation( hWnd, hDC );
							ReleaseDC( hWnd, hDC );
							SendMessage( hWnd, WM_SETTEXT, 0, (DWORD)TEXT("WinSOFM") );
						}
						break;

					default:
                		return( DefWindowProc( hWnd, Message, wParam, lParam ) );
                }
           	}
         	break;

		case UM_CICLO:
			{
				int    n, m, nmin, mmin, l, r, t, b;
				HDC    hDC;
				TCHAR  cBuffer[20];
				double x, y, d, min;

				if( dwCicloCorrente == dwCicliTotali )
				{
					MessageBeep( 0 );
					break;
				}
				dwCicloCorrente++;
				x = GetRandom();
				y = GetRandom();
				min = 1000000.0;
				nmin = 0;
				mmin = 0;
				for( n = 0; n < NUM_X_NODES; n++ )
				{
					for( m = 0; m < NUM_Y_NODES; m++ )
					{
						d = pow( x - Weights[0][n][m], 2 ) + pow( y - Weights[1][n][m], 2 );
						if( d < min )
						{
							min = d;
							nmin = n;
							mmin = m;
						}
					}
				}
				Neighbors( nmin, mmin, &l, &r, &t, &b );
				dblEtaCorrente = Eta();
				for( n = l; n <= r; n++ )
				{
					for( m = t; m <= b; m++ )
					{
						Weights[0][n][m] = Weights[0][n][m] + dblEtaCorrente * ( x - Weights[0][n][m] );
						Weights[1][n][m] = Weights[1][n][m] + dblEtaCorrente * ( y - Weights[1][n][m] );
					}
				}
				hDC = GetDC( hWnd );
				DrawSituation( hWnd, hDC );
				ReleaseDC( hWnd, hDC );
				if( dwCicloCorrente == dwCicliTotali )
				{
					HMENU hMenu;

					MessageBeep( 0 );
					SendMessage( hWnd, WM_SETTEXT, 0, (DWORD)TEXT("WinSOFM") );
					hMenu = GetMenu( hWnd );
					EnableMenuItem( hMenu, IDM_VAI, MF_GRAYED );
					EnableMenuItem( hMenu, IDM_PASSO, MF_GRAYED );
					EnableMenuItem( hMenu, IDM_PAUSA, MF_GRAYED );
					EnableMenuItem( hMenu, IDM_CONTINUA, MF_GRAYED );
					EnableMenuItem( hMenu, IDM_RIPRISTINA, MF_ENABLED );
					EnableMenuItem( hMenu, IDM_PARAMETRI, MF_GRAYED );
                   	hDC = GetDC( hWnd );
                   	DrawMappa( hWnd, hDC );
                   	dblErrore = CalculateError();
					DisplayErrore( hWnd, hDC );
                   	ReleaseDC( hWnd, hDC );
				}
				else
				{
					WORD wPerc;

					wPerc = (WORD)( ( (double)dwCicloCorrente / (double)dwCicliTotali ) * (double)100 );
					if( wPerc != wPrevPerc )
					{
						wPrevPerc = wPerc;
						wsprintf( cBuffer, TEXT("WinSOFM - %u%%"), wPerc );
						SendMessage( hWnd, WM_SETTEXT, 0, (DWORD)cBuffer );
					}
				}
			}
			break;

    	case WM_CREATE:
    		{
    			HMENU hMenu;

				srand( (unsigned)time( NULL ) );
				SetProp(hWnd, TEXT("TextFont"), CreateFont(7, 6, 0, 0, 700, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, TEXT("Helv")));
				SetProp(hWnd, TEXT("HelpFont"), CreateFont(7, 6, 0, 0, 400, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH | FF_SWISS, TEXT("Helv")));
				SetProp(hWnd, TEXT("KohonenBmp"), LoadBitmap(hInst, TEXT("KohonenBmp")));
				SetProp(hWnd, TEXT("GrigliaBmp"), LoadBitmap(hInst, TEXT("GrigliaBmp")));
				SetProp(hWnd, TEXT("FoglioBmp"), LoadBitmap(hInst, TEXT("FoglioBmp")));
				SetProp(hWnd, TEXT("GrayBrush"), CreateSolidBrush(RGB(192, 192, 192)));
				SetProp(hWnd, TEXT("BlackBrush"), CreateSolidBrush(RGB(0, 0, 0)));
				SetProp(hWnd, TEXT("RedPen"), CreatePen(PS_SOLID, 1, RGB(255, 0, 0)));
				SetProp(hWnd, TEXT("BlackPen"), CreatePen(PS_SOLID, 1, RGB(0, 0, 0)));
				SetProp(hWnd, TEXT("GrayPen"), CreatePen(PS_SOLID, 1, RGB(192, 192, 192)));
				SetProp(hWnd, TEXT("DarkGrayPen"), CreatePen(PS_SOLID, 1, RGB(128, 128, 128)));
				SetProp(hWnd, TEXT("WhitePen"), CreatePen(PS_SOLID, 1, RGB(255, 255, 255)));
				SetProp(hWnd, TEXT("DotPen"), CreatePen(PS_SOLID, 1, RGB(128, 0, 0)));
				hMenu = GetMenu( hWnd );
				if( INIGetSaveOnExit() )
				{
					bSalvaImpostazioni = TRUE;
					CheckMenuItem( hMenu, IDM_SALVAIMPOSTAZIONI, MF_CHECKED );
				}
				else
				{
					bSalvaImpostazioni = FALSE;
					CheckMenuItem( hMenu, IDM_SALVAIMPOSTAZIONI, MF_UNCHECKED );
				}
				bMostraIdeale = INIGetShowIdeal();
				bDisegnaMappa = INIGetDrawMap();
				bPesiMedi     = INIGetAvrgWeightsInit();
				wCicloDisegno = INIGetDrawCycle();
				dwCicliTotali = INIGetIterations();
				dblAlpha      = INIGetAlpha();
				dblKappa      = INIGetKappa();
				nA            = INIGetA();
				dblAcca       = INIGetAcca();
				bInCammino    = FALSE;
				bInPausa      = FALSE;
				EnableMenuItem( hMenu, IDM_VAI, MF_ENABLED );
				EnableMenuItem( hMenu, IDM_PASSO, MF_ENABLED );
				EnableMenuItem( hMenu, IDM_PAUSA, MF_GRAYED );
				EnableMenuItem( hMenu, IDM_CONTINUA, MF_GRAYED );
				EnableMenuItem( hMenu, IDM_RIPRISTINA, MF_GRAYED );
				EnableMenuItem( hMenu, IDM_PARAMETRI, MF_ENABLED );
				EnableMenuItem( hMenu, IDM_RETENEURONALE, MF_ENABLED );
				SendMessage( hWnd, UM_INITNETWORK, 0, 0L );
    		}
			break;

		case WM_DESTROY:
			{
    			DeleteObject(RemoveProp(hWnd, TEXT("TextFont") ) );
				DeleteObject(RemoveProp(hWnd, TEXT("HelpFont")));
				DeleteObject(RemoveProp(hWnd, TEXT("CopyrightBmp")));
				DeleteObject(RemoveProp(hWnd, TEXT("GrigliaBmp")));
				DeleteObject(RemoveProp(hWnd, TEXT("FoglioBmp")));
				DeleteObject(RemoveProp(hWnd, TEXT("GrayBrush")));
				DeleteObject(RemoveProp(hWnd, TEXT("BlackBrush")));
				DeleteObject(RemoveProp(hWnd, TEXT("RedPen")));
				DeleteObject(RemoveProp(hWnd, TEXT("BlackPen")));
				DeleteObject(RemoveProp(hWnd, TEXT("GrayPen")));
				DeleteObject(RemoveProp(hWnd, TEXT("DarkGrayPen")));
				DeleteObject(RemoveProp(hWnd, TEXT("WhitePen")));
				DeleteObject(RemoveProp(hWnd, TEXT("DotPen")));
			}
			break;

		case WM_PAINT:
			{
				HDC         hDC;
				PAINTSTRUCT ps;

         		hDC = BeginPaint( hWnd, &ps );
				DrawContorni( hWnd, hDC );
				DrawPlane( hWnd, hDC );
				DrawBitmap( hWnd, hDC );
				DrawTesto( hWnd, hDC );
				DrawSituation( hWnd, hDC );
				EndPaint( hWnd, &ps );
			}
         	break;

    	case WM_CLOSE:
    		{
    			if( bInCammino )
    			{
    				MessageBeep( 0 );
    				break;
    			}
         		DestroyWindow( hWnd );
       			INISetSaveOnExit( bSalvaImpostazioni );
         		if( bSalvaImpostazioni )
         		{
					INISetShowIdeal( bMostraIdeale );
                    INISetDrawMap( bDisegnaMappa );
                    INISetAvrgWeightsInit( bPesiMedi );
                    INISetDrawCycle( wCicloDisegno );
                    INISetIterations( dwCicliTotali );
                    INISetAlpha( dblAlpha );
                    INISetKappa( dblKappa );
                    INISetA( nA );
                    INISetAcca( dblAcca );
         		}
				PostQuitMessage( 0 );
			}
        	break;

		case UM_INITNETWORK:
			{
				dwCicloCorrente = 0;
				dblEtaCorrente = 1.0;
				nNeCorrente = 10;
				InitWeights();
			}
			break;

    	default:
			return( DefWindowProc( hWnd, Message, wParam, lParam ) );
   	}
 	return( 0L );
}


/*
** InformazioniProc
**
** Informazioni dialog-box window procedure
*/

BOOL FAR PASCAL InformazioniProc( hWndDlg, Message, wParam, lParam )
HWND hWndDlg;
WORD Message;
WORD wParam;
LONG lParam;
{
	switch( Message )
   	{
   	   	case WM_INITDIALOG:
    		{
        		CenterDialog( hWndDlg, 0 );
        	}
        	break;

    	case WM_CLOSE:
    		{
				PostMessage( hWndDlg, WM_COMMAND, IDCANCEL, 0L );
			}
         	break;

    	case WM_COMMAND:
    		{
        		switch( wParam )
           		{
            		case IDOK:
            			{
                			EndDialog( hWndDlg, TRUE );
                		}
                 		break;

            		case IDCANCEL:
						{
							EndDialog( hWndDlg, FALSE );
						}
                 		break;
           		}
         	}
         	break;

    	default:
        	return( FALSE );
   	}
 	return( TRUE );
}


/*
** ParametriProc
**
** Parametri dialog-box window procedure
*/

BOOL FAR PASCAL ParametriProc( hWndDlg, Message, wParam, lParam )
HWND hWndDlg;
WORD Message;
WORD wParam;
LONG lParam;
{
	switch( Message )
   	{
    	case WM_INITDIALOG:
    		{
    			TCHAR cBuffer[10];

    			memset( cBuffer, 0x00, sizeof( cBuffer ) );
    			wsprintf( cBuffer, TEXT("%lu"), dwCicliTotali );
    			SetDlgItemText( hWndDlg, 101, cBuffer );
    			SendDlgItemMessage( hWndDlg, 101, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
    			if( bDisegnaMappa )
    			{
    				SendDlgItemMessage( hWndDlg, 105, BM_SETCHECK, 1, 0L );
					EnableWindow( GetDlgItem( hWndDlg, 106 ), TRUE );
					EnableWindow( GetDlgItem( hWndDlg, 107 ), TRUE );
					EnableWindow( GetDlgItem( hWndDlg, 108 ), TRUE );
					SetDlgItemInt( hWndDlg, 107, wCicloDisegno, FALSE );
    			}
    			else
    			{
    				SendDlgItemMessage( hWndDlg, 105, BM_SETCHECK, 0, 0L );
					EnableWindow( GetDlgItem( hWndDlg, 106 ), FALSE );
					EnableWindow( GetDlgItem( hWndDlg, 107 ), FALSE );
					EnableWindow( GetDlgItem( hWndDlg, 108 ), FALSE );
					SetDlgItemInt( hWndDlg, 107, wCicloDisegno, FALSE );
    			}
    			if( bPesiMedi )
    			{
    			    SendDlgItemMessage( hWndDlg, 109, BM_SETCHECK, 1, 0L );
    			}
    			if( bMostraIdeale )
    			{
    			    SendDlgItemMessage( hWndDlg, 110, BM_SETCHECK, 1, 0L );
    			}
        		CenterDialog( hWndDlg, 0 );
        	}
        	break;

    	case WM_CLOSE:
    		{
				PostMessage( hWndDlg, WM_COMMAND, IDCANCEL, 0L );
			}
         	break;

    	case WM_COMMAND:
    		{
        		switch( wParam )
           		{
           			case 105:	// Check button per il ridisegno
           				{
           					if( SendDlgItemMessage( hWndDlg, 105, BM_GETCHECK, 0, 0L ) )
           					{
           						EnableWindow( GetDlgItem( hWndDlg, 106 ), TRUE );
           						EnableWindow( GetDlgItem( hWndDlg, 107 ), TRUE );
           						EnableWindow( GetDlgItem( hWndDlg, 108 ), TRUE );
            					SetFocus( GetDlgItem( hWndDlg, 107 ) );
            					SendDlgItemMessage( hWndDlg, 107, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
           					}
           					else
           					{
           						EnableWindow( GetDlgItem( hWndDlg, 106 ), FALSE );
           						EnableWindow( GetDlgItem( hWndDlg, 107 ), FALSE );
           						EnableWindow( GetDlgItem( hWndDlg, 108 ), FALSE );
           					}
           				}
           				break;

            		case IDOK:
            			{
            				TCHAR cBuffer[10];
            				long lCicli;

            				memset( cBuffer, 0x00, sizeof( cBuffer ) );
            				GetDlgItemText( hWndDlg, 101, cBuffer, 9 );
							lCicli = _wtol(cBuffer);
            				if( lCicli <= 0 || lCicli > MAX_CICLI )
            				{
            					MessageBeep( 0 );
            					SetFocus( GetDlgItem( hWndDlg, 101 ) );
            					SendDlgItemMessage( hWndDlg, 101, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
            					break;
            				}
							dwCicliTotali = (DWORD)lCicli;
							if( SendDlgItemMessage( hWndDlg, 105, BM_GETCHECK, 0, 0L ) )
							{
								int  nCicli;
								BOOL bTranslated;

								bDisegnaMappa = TRUE;
								nCicli = GetDlgItemInt( hWndDlg, 107, &bTranslated, TRUE );
								if( !bTranslated || nCicli <= 0 || nCicli > 1000 )
								{
									MessageBeep( 0 );
	            					SetFocus( GetDlgItem( hWndDlg, 107 ) );
    	        					SendDlgItemMessage( hWndDlg, 107, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
									break;
								}
								wCicloDisegno = (WORD)nCicli;
							}
							else
								bDisegnaMappa = FALSE;
							if( SendDlgItemMessage( hWndDlg, 109, BM_GETCHECK, 0, 0L ) )
								bPesiMedi = TRUE;
							else
								bPesiMedi = FALSE;
							if( SendDlgItemMessage( hWndDlg, 110, BM_GETCHECK, 0, 0L ) )
								bMostraIdeale = TRUE;
							else
								bMostraIdeale = FALSE;
                			EndDialog( hWndDlg, TRUE );
                		}
                 		break;

            		case IDCANCEL:
						{
							EndDialog( hWndDlg, FALSE );
						}
                 		break;
           		}
         	}
         	break;

    	default:
        	return( FALSE );
   	}
 	return( TRUE );
}


/*
** ReteNeuronaleProc
**
** Rete neuronale dialog-box window procedure
*/

BOOL FAR PASCAL ReteNeuronaleProc( hWndDlg, Message, wParam, lParam )
HWND hWndDlg;
WORD Message;
WORD wParam;
LONG lParam;
{
	switch( Message )
   	{
   	   	case WM_INITDIALOG:
    		{
    			TCHAR cBuffer[80];

				swprintf(cBuffer, 80, TEXT("%1.4f"), dblAlpha);
    			SendDlgItemMessage( hWndDlg, 101, WM_SETTEXT, 0, (DWORD)cBuffer );
				swprintf(cBuffer, 80, TEXT("%1.4f"), dblKappa);
				SendDlgItemMessage(hWndDlg, 102, WM_SETTEXT, 0, (DWORD)cBuffer);
				swprintf(cBuffer, 80, TEXT("%d"), nA);
				SendDlgItemMessage(hWndDlg, 103, WM_SETTEXT, 0, (DWORD)cBuffer);
				swprintf(cBuffer, 80, TEXT("%1.4f"), dblAcca);
				SendDlgItemMessage(hWndDlg, 104, WM_SETTEXT, 0, (DWORD)cBuffer);
        		CenterDialog( hWndDlg, 0 );
        	}
        	break;

    	case WM_CLOSE:
    		{
				PostMessage( hWndDlg, WM_COMMAND, IDCANCEL, 0L );
			}
         	break;

        case WM_PAINT:
        	{
        		HDC         hDC, hMemoryDC;
        		HBITMAP		hOldBitmap;
        		PAINTSTRUCT ps;

        		hDC = BeginPaint( hWndDlg, &ps );
        		hMemoryDC = CreateCompatibleDC( hDC );
        		hOldBitmap = SelectObject( hMemoryDC, GetProp( GetParent( hWndDlg ), TEXT("FoglioBmp") ) );
        		BitBlt( hDC, 0, 0, 450, 313, hMemoryDC, 0, 0, SRCCOPY );
        		SelectObject( hMemoryDC, hOldBitmap );
        		DeleteDC( hMemoryDC );
        		EndPaint( hWndDlg, &ps );
        	}
        	break;

    	case WM_COMMAND:
    		{
        		switch( wParam )
           		{
            		case IDOK:
            			{
            				int    nTemp;
			    			TCHAR  cBuffer[80];
                            double dblTemp;

    						GetDlgItemText( hWndDlg, 101, cBuffer, 80 );
							dblTemp = _wtof(cBuffer);
							if( dblTemp > 1.0 || dblTemp < -1.0 )
							{
								MessageBeep( 0 );
            					SetFocus( GetDlgItem( hWndDlg, 101 ) );
   	        					SendDlgItemMessage( hWndDlg, 101, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
								break;
							}
							dblAlpha = dblTemp;
    						GetDlgItemText( hWndDlg, 102, cBuffer, 80 );
							dblKappa = _wtof(cBuffer);
							GetDlgItemText( hWndDlg, 103, cBuffer, 80 );
							nTemp = _wtoi(cBuffer);
							if( nTemp <= 0 )
							{
								MessageBeep( 0 );
            					SetFocus( GetDlgItem( hWndDlg, 103 ) );
   	        					SendDlgItemMessage( hWndDlg, 103, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
								break;
							}
							nA = nTemp;
							GetDlgItemText( hWndDlg, 104, cBuffer, 80 );
							dblTemp = _wtof(cBuffer);
							if( dblTemp <= 0.0 )
							{
								MessageBeep( 0 );
            					SetFocus( GetDlgItem( hWndDlg, 104 ) );
   	        					SendDlgItemMessage( hWndDlg, 104, EM_SETSEL, 0, MAKELONG( 0, 32767 ) );
								break;
							}
                			dblAcca = dblTemp;
                			EndDialog( hWndDlg, TRUE );
                		}
                 		break;

            		case IDCANCEL:
						{
							EndDialog( hWndDlg, FALSE );
						}
                 		break;
           		}
         	}
         	break;

    	default:
        	return( FALSE );
   	}
 	return( TRUE );
}


/*
** RegisterClasses
**
** Register the application window class (for first instance only)
*/

int RegisterClasses()
{
	WNDCLASSEX wndclass;

	memset( &wndclass, 0x00, sizeof(WNDCLASSEX) );
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wndclass.lpfnWndProc = MainWndProc;
	wndclass.cbClsExtra = 0;
 	wndclass.cbWndExtra = 0;
 	wndclass.hInstance = hInst;
 	wndclass.hIcon = LoadIcon( hInst, TEXT("WinsofmIcon") );
 	wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndclass.hbrBackground = GetStockObject( LTGRAY_BRUSH );
 	wndclass.lpszMenuName = TEXT("WinsofmMenu");
	wndclass.lpszClassName = TEXT("WinsofmWClass");
 	if( !RegisterClassEx( &wndclass ) )
   		return( -1 );
	return( 0 );
}


/*
** CenterDialog
**
** Center the dialog-box in the parent client area
*/

void CenterDialog( hWnd, top )
HWND hWnd;
int  top;
{
	int   iwidth, iheight;
	RECT  swp, rParent;
	POINT pt;

	GetWindowRect( hWnd, &swp );
 	GetClientRect( hWndMain, &rParent );
	iwidth = swp.right - swp.left;
 	iheight = swp.bottom - swp.top;
	pt.x = ( rParent.right - rParent.left ) / 2;
 	pt.y = ( rParent.bottom - rParent.top ) / 2;
 	ClientToScreen( hWndMain, &pt );
	pt.x = pt.x - ( iwidth / 2 );
 	pt.y = pt.y - ( iheight / 2 );
	if( top )
   		pt.y = pt.y + top;
	MoveWindow( hWnd, pt.x, pt.y, iwidth, iheight, FALSE );
	return;
}


/*
** GetRandom
**
** Ritorna un numero casuale nell'intervallo 0.0 - 1.0
*/

double GetRandom()
{
	return( (double)rand() / (double)RAND_MAX );
}


/*
** Eta
*/

double Eta()
{
	return( dblAlpha * exp( ( -(double)dwCicloCorrente * dblKappa ) / (double)dwCicliTotali ) );
}


/*
** Neighbors
*/

void Neighbors( x, y, l, r, t, b )
int   x;
int   y;
int   *l;
int   *r;
int   *t;
int   *b;
{
	if( dwCicloCorrente >= (DWORD)( (double)dwCicliTotali / dblAcca ) )
		nNeCorrente = 0;
	else
	    nNeCorrente = (int)( (double)nA * ( (double)1 - ( ( dblAcca * (double)dwCicloCorrente ) / (double)dwCicliTotali ) ) );
	*l = x - nNeCorrente;
	*r = x + nNeCorrente;
	*t = y - nNeCorrente;
	*b = y + nNeCorrente;
	if( *l < 0 )
		*l = 0;
	if( *t < 0 )
		*t = 0;
	if( *r >= NUM_X_NODES )
		*r = NUM_X_NODES - 1;
	if( *b >= NUM_Y_NODES )
		*b = NUM_Y_NODES - 1;
	return;
}


/*
** InitWeights
**
** Inizializza i pesi della matrice a valori casuali
*/

void InitWeights()
{
	int n, m;

	for( n = 0; n < NUM_X_NODES; n++ )
	{
		for( m = 0; m < NUM_Y_NODES; m++ )
		{
			if( bPesiMedi )
			{
				double t;

				t = GetRandom() / (double)10;
				if( GetRandom() > 0.5 )
					t = -t;
				Weights[0][n][m] = 0.5 + t;
				t = GetRandom() / (double)10;
				if( GetRandom() > 0.5 )
					t = -t;
				Weights[1][n][m] = 0.5 + t;
			}
			else
			{
				Weights[0][n][m] = GetRandom();
				Weights[1][n][m] = GetRandom();
			}
		}
	}
	return;
}


/*
** CalculateError
**
** Questa funzione calcola l'errore commesso dalla rete neuronale
*/

double CalculateError()
{
	int    i, j;
	double Error;

	Error = 0;
	for( i = 0; i < NUM_X_NODES; i++ )
	{
		for( j = 0; j < NUM_Y_NODES; j++ )
		{
			Error += fabs( ( 0.09 * ( i + 1 ) ) - Weights[0][i][j] );
			Error += fabs( ( 0.09 * ( j + 1 ) ) - Weights[1][i][j] );
		}
	}
	return( Error );
}


/*
** DrawSituation
*/

void DrawSituation( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	if( bDisegnaMappa )
	{
		if( !( dwCicloCorrente % (DWORD)wCicloDisegno ) )
		{
			DrawPlane( hWnd, hDC );
    		if( bMostraIdeale )
    			DrawQVIdeale( hWnd, hDC );
    	    DrawMappa( hWnd, hDC );
    	}
    }
	DisplayCicliTotali( hWnd, hDC );
	DisplayCicloCorrente( hWnd, hDC );
	DisplayEtaCorrente( hWnd, hDC );
	DisplayNeCorrente( hWnd, hDC );
	return;
}


/*
** DrawMappa
**
** Disegna la mappa
*/

void DrawMappa( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	int    n, m;
	HPEN   hOldPen;

	hOldPen = SelectObject( hDC, GetProp( hWnd, TEXT("RedPen")) );
	for( n = 0; n < NUM_X_NODES; n++ )
	{
		MoveToEx( hDC, PIANO_X + (int)( Weights[0][n][0] * (double)PIANO_W ), PIANO_Y + (int)( Weights[1][n][0] * (double)PIANO_H ), NULL );
		for( m = 1; m < NUM_Y_NODES; m++ )
			LineTo( hDC, PIANO_X + (int)( Weights[0][n][m] * (double)PIANO_W ), PIANO_Y + (int)( Weights[1][n][m] * (double)PIANO_H ) );
	}
	for( n = 0; n < NUM_Y_NODES; n++ )
	{
		MoveToEx(hDC, PIANO_X + (int)(Weights[0][0][n] * (double)PIANO_W), PIANO_Y + (int)(Weights[1][0][n] * (double)PIANO_H), NULL );
		for( m = 1; m < NUM_X_NODES; m++ )
			LineTo( hDC, PIANO_X + (int)( Weights[0][m][n] * (double)PIANO_W ), PIANO_Y + (int)( Weights[1][m][n] * (double)PIANO_H ) );
	}
	SelectObject( hDC, hOldPen );
	return;
}


/*
** DrawBitmap
*/

void DrawBitmap( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	HDC     hMemoryDC;
	HBITMAP hOldBitmap;

	hMemoryDC = CreateCompatibleDC( hDC );
	hOldBitmap = SelectObject( hMemoryDC, GetProp( hWnd, TEXT("KohonenBmp")) );
	BitBlt( hDC, BMP_X, BMP_Y, BMP_W, BMP_H, hMemoryDC, 0, 0, SRCCOPY );
	SelectObject(hMemoryDC, GetProp(hWnd, TEXT("GrigliaBmp")));
	BitBlt( hDC, GRIGLIA_X, GRIGLIA_Y, GRIGLIA_W, GRIGLIA_H, hMemoryDC, 0, 0, SRCCOPY );
	SelectObject( hMemoryDC, hOldBitmap );
	DeleteDC( hMemoryDC );
    return;
}


/*
** DrawTesto
*/

void DrawTesto( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	int   nOldBkMode;
	DWORD dwOldTextColor, dwOldBkColor;
	HFONT hOldFont;

	nOldBkMode = SetBkMode( hDC, OPAQUE );
	dwOldTextColor = SetTextColor( hDC, COL_TXT_TEXT );
	dwOldBkColor = SetBkColor( hDC, COL_TXT_SFONDO );
	hOldFont = SelectObject( hDC, GetProp( hWnd, TEXT("TextFont")) );
	TextOut(hDC, TXT_TOT_X, TXT_TOT_Y, TEXT("Cicli totali:"), 13);
	TextOut(hDC, TXT_CURR_X, TXT_CURR_Y, TEXT("Ciclo corrente:"), 15);
	TextOut(hDC, TXT_ETA_X, TXT_ETA_Y, TEXT("Eta:"), 4);
	TextOut(hDC, TXT_NE_X, TXT_NE_Y, TEXT("Intorno:"), 8);
	TextOut(hDC, TXT_ID_X, TXT_ID_Y, TEXT("Errore:"), 7);
	SelectObject( hDC, hOldFont );
	SetBkColor( hDC, dwOldBkColor );
	SetTextColor( hDC, dwOldTextColor );
	SetBkMode( hDC, nOldBkMode );
	return;
}


/*
** DrawPlane
*/

void DrawPlane( hWnd, hDC )
HWND hWnd;
HDC hDC;
{
	HPEN   hOldPen;
	HBRUSH hOldBrush;

	hOldBrush = SelectObject(hDC, GetProp(hWnd, TEXT("BlackBrush")) );
	hOldPen = SelectObject(hDC, GetProp(hWnd, TEXT("BlackPen")));
	Rectangle( hDC, PIANO_X, PIANO_Y, PIANO_X + PIANO_W, PIANO_Y + PIANO_H );
	SelectObject(hDC, GetProp(hWnd, TEXT("DarkGrayPen")));
	MoveToEx( hDC, PIANO_X - 1, PIANO_Y + PIANO_H, NULL);
	LineTo( hDC, PIANO_X - 1, PIANO_Y - 1 );
	LineTo( hDC, PIANO_X + PIANO_W + 1, PIANO_Y - 1 );
	SelectObject(hDC, GetProp(hWnd, TEXT("WhitePen")));
	MoveToEx( hDC, PIANO_X + PIANO_W, PIANO_Y, NULL);
	LineTo( hDC, PIANO_X + PIANO_W, PIANO_Y + PIANO_H );
	LineTo( hDC, PIANO_X - 1, PIANO_Y + PIANO_H );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldBrush );
	return;
}


/*
** DrawContorni
*/

void DrawContorni( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	RECT ClientRect;
	HPEN hOldPen;

	GetClientRect( hWnd, &ClientRect );
	hOldPen = SelectObject(hDC, GetProp(hWnd, TEXT("DarkGrayPen")));
	MoveToEx(hDC, ClientRect.left + 4, ClientRect.bottom - 5, NULL);
	LineTo( hDC, ClientRect.left + 4, ClientRect.top + 4 );
	LineTo( hDC, ClientRect.right - 5, ClientRect.top + 4 );
	SelectObject(hDC, GetProp(hWnd, TEXT("WhitePen")));
	MoveToEx(hDC, ClientRect.right - 5, ClientRect.top + 5, NULL);
	LineTo( hDC, ClientRect.right - 5, ClientRect.bottom - 5 );
	LineTo( hDC, ClientRect.left + 5, ClientRect.bottom - 5 );
	SelectObject(hDC, GetProp(hWnd, TEXT("DarkGrayPen")));
	MoveToEx(hDC, 285, 270, NULL);
    LineTo( hDC, 285, 226 );
    LineTo( hDC, 459, 226 );
	SelectObject(hDC, GetProp(hWnd, TEXT("WhitePen")));
	MoveToEx(hDC, 458, 227, NULL);
    LineTo( hDC, 458, 270 );
    LineTo( hDC, 285, 270 );
	SelectObject( hDC, hOldPen );
	return;
}


/*
** DrawQVIdeale
*/

void DrawQVIdeale( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	HPEN   hOldPen;
	double x, y, x1, x2, y1, y2;

	hOldPen = SelectObject( hDC, GetProp( hWnd, TEXT("DotPen")) );
	x1 = (double)PIANO_W / ( ( (double)NUM_X_NODES + (double)1 ) * (double)2 );
	x2 = (double)PIANO_W - (double)PIANO_W / ( ( (double)NUM_X_NODES + (double)1 ) * (double)2 );
	y1 = (double)PIANO_H / ( ( (double)NUM_Y_NODES + (double)1 ) * (double)2 );
	y2 = (double)PIANO_H - (double)PIANO_H / ( ( (double)NUM_Y_NODES + (double)1 ) * (double)2 );
	for( x = x1; x < (double)PIANO_W; x += ( (double)PIANO_W / ( (double)NUM_X_NODES + (double)1 ) ) )
	{
		MoveToEx( hDC, (int)( (double)PIANO_X + x ), (int)( (double)PIANO_Y + y1 ), NULL );
		LineTo( hDC, (int)( (double)PIANO_X + x ), (int)( (double)PIANO_Y + y2 ) );
	}
	for( y = y1; y < (double)PIANO_H; y += ( (double)PIANO_H / ( (double)NUM_Y_NODES + (double)1 ) ) )
	{
		MoveToEx( hDC, (int)( (double)PIANO_X + x1 ), (int)( (double)PIANO_Y + y ), NULL );
		LineTo( hDC, (int)( (double)PIANO_X + x2 ), (int)( (double)PIANO_Y + y ) );
	}
	SelectObject( hDC, hOldPen );
	return;
}


/*
** DisplayCicliTotali
*/

void DisplayCicliTotali( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	int    nOldBkMode;
	TCHAR  cBuffer[30];
	HPEN   hOldPen;
	HFONT  hOldFont;
	DWORD  dwOldTextColor, dwOldBkColor;
	HBRUSH hOldBrush;

	nOldBkMode = SetBkMode( hDC, OPAQUE );
	dwOldTextColor = SetTextColor( hDC, COL_TXT_TOT );
	dwOldBkColor = SetBkColor( hDC, COL_TXT_SFONDO );
	hOldFont = SelectObject( hDC, GetProp( hWnd, TEXT("TextFont")) );
	hOldPen = SelectObject(hDC, GetProp(hWnd, TEXT("GrayPen")));
	hOldBrush = SelectObject(hDC, GetProp(hWnd, TEXT("GrayBrush")));
	Rectangle( hDC, TXT_TOT_V, TXT_TOT_Y, TXT_ETA_X - 2, TXT_TOT_Y + 11 );
	wsprintf( cBuffer, TEXT("%lu"), dwCicliTotali );
	TextOut( hDC, TXT_TOT_V, TXT_TOT_Y, cBuffer, lstrlen( cBuffer ) );
	SelectObject( hDC, hOldBrush );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldFont );
	SetBkColor( hDC, dwOldBkColor );
	SetTextColor( hDC, dwOldTextColor );
	SetBkMode( hDC, nOldBkMode );
	return;
}


/*
** DisplayCicloCorrente
*/

void DisplayCicloCorrente( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	int    nOldBkMode;
	TCHAR  cBuffer[30];
	HPEN   hOldPen;
	HFONT  hOldFont;
	DWORD  dwOldTextColor, dwOldBkColor;
	HBRUSH hOldBrush;

	nOldBkMode = SetBkMode( hDC, OPAQUE );
	dwOldTextColor = SetTextColor( hDC, COL_TXT_CURR );
	dwOldBkColor = SetBkColor( hDC, COL_TXT_SFONDO );
	hOldFont = SelectObject(hDC, GetProp(hWnd, TEXT("TextFont")));
	hOldPen = SelectObject(hDC, GetProp(hWnd, TEXT("GrayPen")));
	hOldBrush = SelectObject(hDC, GetProp(hWnd, TEXT("GrayBrush")));
	Rectangle( hDC, TXT_CURR_V, TXT_CURR_Y, TXT_NE_X - 2, TXT_CURR_Y + 11 );
	wsprintf( cBuffer, TEXT("%lu"), dwCicloCorrente );
	TextOut( hDC, TXT_CURR_V, TXT_CURR_Y, cBuffer, lstrlen( cBuffer ) );
	SelectObject( hDC, hOldBrush );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldFont );
	SetBkColor( hDC, dwOldBkColor );
	SetTextColor( hDC, dwOldTextColor );
	SetBkMode( hDC, nOldBkMode );
	return;
}


/*
** DisplayEtaCorrente
*/

void DisplayEtaCorrente( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	int    nOldBkMode;
	TCHAR  cBuffer[30];
	HPEN   hOldPen;
	HFONT  hOldFont;
	DWORD  dwOldTextColor, dwOldBkColor;
	HBRUSH hOldBrush;

	nOldBkMode = SetBkMode( hDC, OPAQUE );
	dwOldTextColor = SetTextColor( hDC, COL_TXT_ETA );
	dwOldBkColor = SetBkColor( hDC, COL_TXT_SFONDO );
	hOldFont = SelectObject( hDC, GetProp( hWnd, TEXT("TextFont")) );
	hOldPen = SelectObject(hDC, GetProp(hWnd, TEXT("GrayPen")));
	hOldBrush = SelectObject(hDC, GetProp(hWnd, TEXT("GrayBrush")));
	Rectangle( hDC, TXT_ETA_V, TXT_ETA_Y, TXT_ETA_V + 30, TXT_ETA_Y + 11 );
	swprintf(cBuffer, 30, TEXT("%1.4f"), dblEtaCorrente);
	TextOut( hDC, TXT_ETA_V, TXT_ETA_Y, cBuffer, lstrlen( cBuffer ) );
	SelectObject( hDC, hOldBrush );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldFont );
	SetBkColor( hDC, dwOldBkColor );
	SetTextColor( hDC, dwOldTextColor );
	SetBkMode( hDC, nOldBkMode );
	return;
}


/*
** DisplayNeCorrente
*/

void DisplayNeCorrente( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	int    nOldBkMode;
	TCHAR  cBuffer[30];
	HPEN   hOldPen;
	HFONT  hOldFont;
	DWORD  dwOldTextColor, dwOldBkColor;
	HBRUSH hOldBrush;

	nOldBkMode = SetBkMode( hDC, OPAQUE );
	dwOldTextColor = SetTextColor( hDC, COL_TXT_NE );
	dwOldBkColor = SetBkColor( hDC, COL_TXT_SFONDO );
	hOldFont = SelectObject( hDC, GetProp( hWnd, TEXT("TextFont")) );
	hOldPen = SelectObject(hDC, GetProp(hWnd, TEXT("GrayPen")));
	hOldBrush = SelectObject(hDC, GetProp(hWnd, TEXT("GrayBrush")));
	Rectangle( hDC, TXT_NE_V, TXT_NE_Y, TXT_NE_V + 30, TXT_NE_Y + 11 );
	wsprintf( cBuffer, TEXT("%d"), nNeCorrente );
	TextOut( hDC, TXT_NE_V, TXT_NE_Y, cBuffer, lstrlen( cBuffer ) );
	SelectObject( hDC, hOldBrush );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldFont );
	SetBkColor( hDC, dwOldBkColor );
	SetTextColor( hDC, dwOldTextColor );
	SetBkMode( hDC, nOldBkMode );
	return;
}


/*
** DisplayErrore
*/

void DisplayErrore( hWnd, hDC )
HWND hWnd;
HDC  hDC;
{
	int    nOldBkMode;
	TCHAR  cBuffer[30];
	HPEN   hOldPen;
	HFONT  hOldFont;
	DWORD  dwOldTextColor, dwOldBkColor;
	HBRUSH hOldBrush;

	nOldBkMode = SetBkMode( hDC, OPAQUE );
	dwOldTextColor = SetTextColor( hDC, COL_TXT_NE );
	dwOldBkColor = SetBkColor( hDC, COL_TXT_SFONDO );
	hOldFont = SelectObject( hDC, GetProp( hWnd, TEXT("TextFont")) );
	hOldPen = SelectObject(hDC, GetProp(hWnd, TEXT("GrayPen")));
	hOldBrush = SelectObject(hDC, GetProp(hWnd, TEXT("GrayBrush")));
	Rectangle( hDC, TXT_ID_V, TXT_ID_Y, TXT_ID_V + 80, TXT_ID_Y + 11 );
	swprintf(cBuffer, 30, TEXT("%7.2f"), dblErrore);
	TextOut( hDC, TXT_ID_V, TXT_ID_Y, cBuffer, lstrlen( cBuffer ) );
	SelectObject( hDC, hOldBrush );
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldFont );
	SetBkColor( hDC, dwOldBkColor );
	SetTextColor( hDC, dwOldTextColor );
	SetBkMode( hDC, nOldBkMode );
	return;
}
