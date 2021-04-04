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

// Prototypes de Fonctions
sbit ENVOI_AV = P1^0;
sbit ENVOI_AR = P1^1;
sbit ECHO_AV = P1^2;
sbit ECHO_AR = P1^3;
bit ret;

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
	while (!ECHO_AR){}
	TR2 = 1;
	while (ECHO_AR && !ret){}
	TR2 = 0;
	if (ret){ // Si on a overload
		return 0;
	}
	else{ // On a pas Overload
		return(((float)T2*9.362/1000)-200);
	}
}

void ISR_T2(void) interrupt 5 {
	
	EXF2 = 0;
	TF2 = 0;
	if(ENVOI_AV || ENVOI_AR){
		ENVOI_AV = 0;
		ENVOI_AR = 0;
		TR2 = 0;
	}
	else{	ret = 1;}
}