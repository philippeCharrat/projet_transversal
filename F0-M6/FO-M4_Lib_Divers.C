/******************************************************************************
; 	FILE NAME  	: FO-M4_Lib_Divers.C
; 	TARGET MCUs	: C8051F020, 
; 	DESCRIPTION	: Fonctions diverses mises � disposition
;
; 	REVISION 1.0
;   Ce fichier contient diverses fonctions susceptibles de vous faire gagner 
;   un peu de temps
*/
//******************************************************************************

#include "C8051F020.h"
#include <c8051F020_SFR16.h>
#include <FO-M4_Lib_Divers.h>

//*****************************************************************************	 
//*****************************************************************************	 
// Software_Delay_1ms -- Temporisation superieur a 1ms
//  Pour SYSCLK = 2 MHz
//  A v�rifi�e
void Delay_Oscillator(void)
   { 
	 unsigned int i;
	 for(i=0;i<200;i++){}
	 }
//*****************************************************************************	 
//*****************************************************************************	 
// Software_Delay_10ms -- Temporisation 10ms
//  Pour SYSCLK = 2 MHz
//  V�rifi�e en  simulation
void Software_Delay_10ms(void)
   { 
	 unsigned int i;
	 for(i=0;i<998;i++){}
	 }
//*****************************************************************************	 
//*****************************************************************************	 
// Software_Delay -- Temporisation param�trable
//      Pour SYSCLK = 2 MHz
//      V�rifi�e en  simulation
//      L'argument pass� en exprim� en centi�mes de seconde (dans l'hypoth�se 
//      o� SYSCLK = 2MHz)
	 
 void Software_Delay(unsigned int hundredth_second)
   { 
	 unsigned int i;
	 
	 for(i=0;i<hundredth_second;i++)
      { 
	    Software_Delay_10ms();
			}
	 }


