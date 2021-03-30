//-----------------------------------------------------------------------------
// FO-M4_Main.c
//-----------------------------------------------------------------------------
// AUTH: 
// DATE: 
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include <C8051F020.h>
#include <c8051F020_SFR16.h>
#include <FO-M4_Lib_Config_Globale_8051F020.h>
#include <FO-M4_Lib_Divers.h>

// Prototypes de Fonctions


unsigned int MES_Dist_AV (void) {
	ret = 0;
	T2 = 65536 - 19;
	ENVOI_AV = 1;
	TR2 = 1;
	while (!ECHO_AV){}
	TR2 = 1;
	while (ECHO_AV && !ret){}
	TR2 = 0;
	if (ret){ // Si on a overload
		return 0;
	}
	else{ // On a pas Overload
		return(((float)T2*9.362/1000)-200);
	}
}

unsigned int MES_Dist_AR (void) {
	ret = 0;
	T2 = 65536 - 19;
	ENVOI_AR = 1;
	TR2 = 1;
	while (!ECHO_AR && !ret){}
	TR2 = 0;
	if (ret){
		return 0;
	}
	else{ // On a pas Overload
		return(((float)T2*9.362/1000)-200);
	}
}