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

void Timers_Init(void);
void Int_Init(void);
void ISR_T2(void);
unsigned int MES_Dist_AV (void);
unsigned int MES_Dist_AR (void);

// Variables g�n�rales

sbit ENVOI_AV = P1^0;
sbit ENVOI_AR = P1^1;
sbit ECHO_AV = P1^2;
sbit ECHO_AR = P1^3;

bit ret = 0;
		 
unsigned int a;
unsigned int b;

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void) {
      
	 Init_Device();
// D�but Insertion Code Initialisations ***************************************
	 Int_Init();
	 Timers_Init();
// Fin Code Initialisations ***************************************************

// D�but Insertion Code Phase D�marrage ***************************************	
	a = MES_Dist_AV();
	//b = MES_Dist_AR();
// Fin Code phase D�marrage	***************************************************
	
	while(1){
	}		
}

//-----------------------------------------------------------------------------
// Fonctions de configuration des divers p�riph�riques
//-----------------------------------------------------------------------------

	 void Int_Init(void) {
		 
	 //P.119 pour IE
	 IE |= (1<<5); //Activation de l'interruption du timer 2
		 
	 //P.119 pour EA
	 EA = 1; //Activation des interruptions 
		 
	 }

	 void Timers_Init(void) {
	
	 //P.238 pour T2CON
	 T2CON = 0x00;	
					//Mode Auto-Reload : CP/RL2 = 0
					//CLK interne utilise : C/T2 = 0
					//Timer2 desactive : TR2 = 0
					//Non Utilisation de T2EX : EXEN2 = 0
					//UART0 non lie a Timer2 : TCLK0 et RCLK0 a 0
					//Clear de EXF2 (flag d'interruption de capture) : EXF2 = 0
					//Clear de TF2 (flag d'interruption d'overflow) : TF2 = 0
	 
	 RCAP2 = 0x5358; //Reload au temps max d'un signal de echo

	 //P.226 pour CKCON
	 CKCON &= ~(0x32);//Utilisation de SYSCLK/12 pour le timer 2	 
	 
	 //P.238 pour T2CON
	 TR2 = 0;//Desactivation de timer2
	 }
	 




//-----------------------------------------------------------------------------
// Fonctions d'interruptions
//-----------------------------------------------------------------------------
	 
void ISR_T2(void) interrupt 5 {
	
	EXF2 = 0;
	TF2 = 0;
	if(ENVOI_AV || ENVOI_AR){
		ENVOI_AV = 0;
		ENVOI_AR = 0;
		TR2 = 0;
	}
	else{
		ret = 1;
	}

}
//-----------------------------------------------------------------------------
// Fonctions principales
//-----------------------------------------------------------------------------

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