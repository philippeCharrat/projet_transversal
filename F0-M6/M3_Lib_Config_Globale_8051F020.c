/******************************************************************************
; 	FILE NAME  	: M3_Lib_Config_Globale_8051F020.C
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: Configurations GLOBALES pour le 8051F020
;
; 	REVISION 1.0
;   Ce fichier contient les codes de configuration globale du 8051F020
;     - Configuration de l'horloge système SYSCLK
;     - Configuration des broches du 80851F020
;     - Configuration Reset et watchdog
*/
//******************************************************************************

#include "C8051F020.h"



// Peripheral specific initialization functions,
// Called from the Init_Device() function

//-----------------------------------------------------------------------------
// Dévalidation du watchdog
//-----------------------------------------------------------------------------
void Reset_Sources_Init()
{
	 WDTCN = 0xDE;
	 WDTCN = 0XAD;
// Début Insertion Code configuration Reset et watchdog ***********************
	
// Fin Insertion Code configuration Reset et watchdog *************************
}

//-----------------------------------------------------------------------------
// Configuration des Ports d'entrée-sorties
//-----------------------------------------------------------------------------

void Port_IO_Init()
{
    // P0.0  -  Unassigned, Open-Drain, Digital
    // P0.1  -  Unassigned, Open-Drain, Digital
    // P0.2  -  Unassigned, Open-Drain, Digital
    // P0.3  -  Unassigned,  Open-Drain, Digital
    // P0.4  -  Unassigned,  Open-Drain, Digital
    // P0.5  -  Unassigned,  Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    // P1.0  -  Unassigned,  Open-Drain, Digital
    // P1.1  -  Unassigned,  Open-Drain, Digital
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  LED       ,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    // P2.0  -  Unassigned,  Open-Drain, Digital
    // P2.1  -  Unassigned,  Open-Drain, Digital
    // P2.2  -  Unassigned,  Open-Drain, Digital
    // P2.3  -  Unassigned,  Open-Drain, Digital
    // P2.4  -  Unassigned,  Open-Drain, Digital
    // P2.5  -  Unassigned,  Open-Drain, Digital
    // P2.6  -  Unassigned,  Open-Drain, Digital
    // P2.7  -  Unassigned,  Open-Drain, Digital

    // P3.0  -  Unassigned,  Open-Drain, Digital
    // P3.1  -  Unassigned,  Open-Drain, Digital
    // P3.2  -  Unassigned,  Open-Drain, Digital
    // P3.3  -  Unassigned,  Open-Drain, Digital
    // P3.4  -  Unassigned,  Open-Drain, Digital
    // P3.5  -  Unassigned,  Open-Drain, Digital
    // P3.6  -  Unassigned,  Open-Drain, Digital Input INT6
    // P3.7  -  BP        ,  Open-Drain, Digital Input INT7
		
		// P4.0 to P7.7   Unassigned,  Open-Drain, Digital
		
// Début Insertion Code configuration Crossbar et GPIO ************************
	P1MDOUT |= (1<<0);
	P1MDOUT |= (1<<1);
	XBR2 = 0x40;
     
// Fin Insertion Code configuration Crossbar et GPIO **************************

}

//-----------------------------------------------------------------------------
// Configuration de l'oscillateur - SYSCLK = 22 118 400Hz 
//-----------------------------------------------------------------------------
void Oscillator_Init()
{
// Début Insertion Code configuration SYSCLK **********************************
    int i = 0;
    OSCXCN    = 0x67;
    for (i = 0; i < 3000; i++);  // Wait 1ms for initialization
    while ((OSCXCN & 0x80) == 0);
    OSCICN    = 0x0C;
// Fin Insertion Code configuration SYSCLK ************************************
}

//-----------------------------------------------------------------------------
// Initialisation globale du Microcontrôleur - 
//-----------------------------------------------------------------------------
void Init_Device(void)
{
    Reset_Sources_Init();
    Port_IO_Init();
    Oscillator_Init();
}
