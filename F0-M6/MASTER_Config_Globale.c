//-----------------------------------------------------------------------------
// MASTER_Config_Globale.c
//
//Gestion des trames de commandes et d'informations
//R�ception sur l'UART des commandes et stockage dans un tableau binaire
//Lecture dans un tableau des informations et envoie par l'UART
//-----------------------------------------------------------------------------

#include <C8051F020.h>
#include "c8051F020_SFR16.h"

//-----------------------------------------------------------------------------
// Initialisation clock du Microcontr�leur (sysclock � 22MHz)
//-----------------------------------------------------------------------------
void Oscillator_init(void){
	int i = 0;
	OSCXCN    = 0x67;  // Config de l'horloge externe - Quartz > 6,7 MHz
	for (i = 0; i < 3000; i++);  // attente stabilisation Fosc quartz  
	while ((OSCXCN & 0x80) == 0); // validation stabilit� du quartz
	OSCICN    = 0x0C;  // Commutation sur oscillateur externe
}
//-----------------------------------------------------------------------------
// Initialisation Ports I/O du Microcontr�leur
//-----------------------------------------------------------------------------
void Port_IO_init(void){
	
	XBR0 = (1<<2);
	XBR2 = 0x44;
	
	P0MDOUT |= (1<<0);
	P0MDOUT &= ~(1<<1);
	P0MDOUT |= (1<<2);
	P0MDOUT &= ~(1<<3);
	P1MDOUT |= 0x01; //P1.0 en Push-Pull (sortie) : ENVOI_AV
	P1MDOUT |= 0x02; //P1.1 en Push-Pull (sortie) : ENVOI_AR
	P1MDOUT &= ~0x04; //P1.2 en Open-drain (entree) : ECHO_AV
	P1MDOUT &= ~0x08; //P1.3 en Open-drain (entree) : ECHO_AR
	P1MDOUT |= 0x10; //P1.4 en Push-Pull (sortie) : Commande_H
}
//-----------------------------------------------------------------------------
// Initialisation Sources de reset du Microcontr�leur
//-----------------------------------------------------------------------------

void Reset_Sources_init(void){
	//Watchdog
	WDTCN = 0xDE;
	WDTCN = 0XAD;
	
	//Autres
}	

void Config_interrupt(){
	 //P.122 pour EIE2
	 EIE2 |= 0x04; //Activation des interruptions du timer 4 et de Int7 (BP)
	 
	//P.119 pour IE
	 IE |= (1<<5); //Activation de l'interruption du timer 2
		 
	 //P.119 pour EA
	 EA = 1; //Activation des interruptions 
}

void Config_Timer() {
	// But : Configuration du TIMER 1
	TH1 = 0xB8; //Baud-rate de 19200
	TF1 = 0;
	TR1 = 1; //start timer
	
	TMOD = 0x22;
	
	CKCON |= (1<<4);
	
	CKCON &= ~(1<<3);
	ET0 = 1;
	TH0 = 0x48;
	TF0 = 0;
	TR0 = 0;
	
	//But Timer 2 : 
	 T2CON = 0x00;	
	 RCAP2 = 0x5358; //Reload au temps max d'un signal de echo
	 
	 //P.238 pour T2CON
	 TR2 = 0;//Desactivation de timer2
	
	
	//But :Config Timer 4
	T4CON = 0x00;//Mode Auto-Reload : CP/RL4 = 0
	CKCON &= ~(0x40);//Utilisation de SYSCLK/12 pour les timers 4
	//P.248 pour RCAP4H et RCAP4L
	RCAP4 = 34070;
	//P.248 pour RCAP4H et RCAP4L
	TH4 = 0x85; //On fait commencer le timer 4 au bon nombre 20ms=36864
	TL4 = 0x16; //Pour que notre premiere periode soit bonne
	//P.247 pour T4CON
	T4CON |= (1<<2);//Activation de timer4
	 
}

void Config_UART0(void){
	// But : Configuration de l'UART 0
	SCON0 = 0x50;
	SCON1 = 0x50;
	PCON |= 0x90;
	PCON &= ~0x08;
	
	ET1 = 0;
}
//-----------------------------------------------------------------------------
// Initialisation globale du Microcontr�leur
//-----------------------------------------------------------------------------
void Init_Device(void)
{
	Oscillator_init();
	Port_IO_init();
	Reset_Sources_init();
	Config_interrupt();
	Config_Timer();
	Config_UART0();
}
