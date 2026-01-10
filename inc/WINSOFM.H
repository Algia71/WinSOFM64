/*
** winsofm.h
**
** Copyright 1995 Francesco Bellifemine, Donato Dalbis, Alessandro Giancane
*/


/*
** Costanti messaggio
*/

#define UM_INITNETWORK			WM_USER
#define UM_CICLO				(WM_USER+1)


/*
** Costanti menu
*/

#define IDM_ESCI				1010
#define IDM_INFORMAZIONI		1020
#define IDM_PARAMETRI			1110
#define IDM_RETENEURONALE		1120
#define IDM_SALVAIMPOSTAZIONI	1130
#define IDM_VAI					1210
#define IDM_PASSO				1220
#define IDM_PAUSA				1230
#define IDM_CONTINUA			1240
#define IDM_RIPRISTINA			1250


/*
** Costanti piano
*/

#define PIANO_X					20
#define PIANO_Y					20
#define PIANO_W					250
#define PIANO_H					250


/*
** Costanti testo
*/

#define TXT_TOT_X				20
#define TXT_TOT_V				106
#define TXT_TOT_Y				279
#define TXT_CURR_X				20
#define TXT_CURR_V				106
#define TXT_CURR_Y				294
#define TXT_ETA_X				167
#define TXT_ETA_V				230
#define TXT_ETA_Y				279
#define TXT_NE_X				167
#define TXT_NE_V				230
#define TXT_NE_Y				294
#define TXT_ID_X				20
#define TXT_ID_V				106
#define TXT_ID_Y				309


/*
** Costanti colore
*/

#define COL_TXT_SFONDO			RGB( 192, 192, 192 )
#define COL_TXT_TEXT			RGB( 0, 0, 0 )
#define COL_TXT_TOT				RGB( 0, 0, 0 )
#define COL_TXT_CURR			RGB( 0, 0, 0 )
#define COL_TXT_NE				RGB( 0, 0, 0 )
#define COL_TXT_ETA				RGB( 0, 0, 0 )


/*
** Costanti bitmaps
*/

#define BMP_X					280
#define BMP_Y					20
#define BMP_W					180
#define BMP_H					70
#define GRIGLIA_X				284
#define GRIGLIA_Y				280
#define GRIGLIA_W				175
#define GRIGLIA_H				40


/*
** Costanti generiche
*/

#define MAX_CICLI				5000000
#define NUM_INPUT				2
#define NUM_X_NODES				10
#define NUM_Y_NODES				10


/*
** Dichiarazione delle funzioni
*/

// Funzioni finestra
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL InformazioniProc( HWND, WORD, WORD, LONG );
BOOL FAR PASCAL ParametriProc( HWND, WORD, WORD, LONG );
BOOL FAR PASCAL ReteNeuronaleProc( HWND, WORD, WORD, LONG );

// Funzioni e procedure di servizio
int    RegisterClasses( void );
void   CenterDialog( HWND, int );
void   UnRegisterClasses( void );
double GetRandom( void );

// Funzioni e procedure relative alla rete neuronale
void   Neighbors( int, int, int *, int *, int *, int * );
void   InitWeights( void );
double Eta( void );
double CalculateError( void );

// Funzioni e procedure di disegno e visualizzazione informazioni
void   DrawSituation( HWND, HDC );
void   DisplayCicliTotali( HWND, HDC );
void   DisplayCicloCorrente( HWND, HDC );
void   DisplayEtaCorrente( HWND, HDC );
void   DisplayNeCorrente( HWND, HDC );
void   DisplayErrore( HWND, HDC );
void   DrawBitmap( HWND, HDC );
void   DrawTesto( HWND, HDC );
void   DrawPlane( HWND, HDC );
void   DrawContorni( HWND, HDC );
void   DrawMappa( HWND, HDC );
void   DrawQVIdeale( HWND, HDC );
