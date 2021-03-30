/******************************************************************************
; 	FILE NAME  	: FO-M4_Lib_Config_Globale_8051F020.C
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: Configurations GLOBALES pour le 8051F020
;
; 	REVISION 1.0
;   Ce fichier contient les codes de configuration globale du 8051F020
;     - Configuration de l'horloge syst�me SYSCLK
;     - Configuration des broches du 80851F020
;     - Configuration Reset et watchdog
*/
//******************************************************************************

#include "C8051F020.h"
#include <c8051F020_SFR16.h>
#include <FO-M4_Lib_Divers.h>

// Peripheral specific initialization functions,
// Called from the Init_Device() function

//-----------------------------------------------------------------------------
// D�validation du watchdog
//-----------------------------------------------------------------------------
void Reset_Sources_Init()
{
// D�but Insertion Code configuration Reset et watchdog ***********************
	
	 //P.131 pour WDTCN
	 WDTCN = 0xDE; //Desactivation du WatchDog en deux etapes
	 WDTCN = 0XAD;
	
// Fin Insertion Code configuration Reset et watchdog *************************
}

//-----------------------------------------------------------------------------
// Configuration des Ports d'entr�e-sorties
//-----------------------------------------------------------------------------

void Port_IO_Init()
{
    // P0.0  -  Unassigned,  Open-Drain, Digital
    // P0.1  -  Unassigned,  Open-Drain, Digital
    // P0.2  -  Unassigned,  Open-Drain, Digital
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
    // P3.5  -  SIG_OUT,  Push-Pull, Digital
    // P3.6  -  Unassigned,  Open-Drain, Digital Input INT6
    // P3.7  -  BP        ,  Open-Drain, Digital Input INT7
		
		// P4.0 to P7.7   Unassigned,  Open-Drain, Digital
		
// D�but Insertion Code configuration Crossbar************************		
	
	 //P.172 pour XBR2
	 XBR2 |= 0x40;//Activation du CROSSBAR

// Fin Insertion Code configuration Crossbar**************************

// D�but Insertion Code configuration GPIO ************************
	 
	 P1MDOUT |= 0x01; //P1.0 en Push-Pull (sortie) : ENVOI_AV
	 P1MDOUT |= 0x02; //P1.1 en Push-Pull (sortie) : ENVOI_AR
	 P1MDOUT &= ~0x04; //P1.2 en Open-drain (entree) : ECHO_AV
	 P1MDOUT &= ~0x08; //P1.3 en Open-drain (entree) : ECHO_AR
     
// Fin Insertion Code configuration GPIO **************************
}

//-----------------------------------------------------------------------------
// Configuration de l'oscillateur - SYSCLK = ??Hz 
//-----------------------------------------------------------------------------
void Oscillator_Init()
{
// D�but Insertion Code configuration SYSCLK **********************************
	
	 //P.137 pour OSCXCN
	 OSCXCN = 0x67;//Mise a zero de "XTLVLD" car le crystal n'est pas encore pret
								 //Activation du mode oscillateur : 110
								 //Reglage de la frequence superieur a 6.7 Mhz (22.1184 MHz) : 111
	
	 Delay_Oscillator();// Attente superieur a 1ms (mais pas trop long) pour laisser le crystat s'initialiser
	
	 while ((OSCXCN & (1<<7)) != (1<<7)) { //Tant que "XTLVLD" n'est pas a 1 on boucle car le crystal n'est pas pret
	 }

	 //P.136 pour OSCICN
	 OSCICN |= 0x08;//Changement de SYSCLK pour le crystal, la CLK interne est desactive par le systeme
	
// Fin Insertion Code configuration SYSCLK ************************************
}

//-----------------------------------------------------------------------------
// Initialisation globale du Microcontr�leur - 
//-----------------------------------------------------------------------------
void Init_Device(void)
{
    Reset_Sources_Init();
    Port_IO_Init();
    Oscillator_Init();
}