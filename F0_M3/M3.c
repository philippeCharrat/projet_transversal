//-----------------------------------------------------------------------------
// M3.c
//-----------------------------------------------------------------------------
// AUTH: Lecourt-Capdeville	
// DATE: 02/03/2021
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
// Declarations Registres et Bits de l'espace SFR
#include <C8051F020.h>
#include "c8051F020_SFR16.h"
#include "M3_Lib_Config_Globale_8051F020.h"

//Variables globales
long int AngleAVise;
char Angle_actuel;
sbit Commande_H = P1^1;
char temps;
char tempAngle;
unsigned char tempTemps;

// Prototypes de Fonctions
unsigned char CDE_Servo_H (char Angle); // H comme plan Horizontal 
void Timers_Init(void);
void Int_Init(void);
void Timer4_ISR (void);

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------	
void main (void) {
	
    AngleAVise = 2762;
		Angle_actuel = 0;
	  Init_Device();  // Appel des configurations globales
		Int_Init();
		Timers_Init();
		tempAngle = 45;
		tempTemps = CDE_Servo_H(tempAngle);
		while(1){}
}
void Timers_Init(void) {
	
	 //P.247 pour T4CON
	 T4CON = 0x00;//Mode Auto-Reload : CP/RL4 = 0
							//CLK interne utilise : CT4 = 0
							//Timer4 desactive : TR4 = 0
							//Desactivation de T4EX : EXEN4 = 0
							//UART1 non lie a Timer4 : TCLK1 et RCLK1 a 0
							//Clear de EXF4 (flag d'interruption de capture) : EXF4 = 0
							//Clear de TF4 (flag d'interruption d'overflow) : TF4 = 0
	 
	 //P.226 pour CKCON
	 CKCON &= ~(0x40);//Utilisation de SYSCLK/12 pour les timers 4
	 //On utilise SYSCLK/12 pour avoir une frequence assez haute meme si baisse la precision
	 //On doit juste avoir : T_SIG_OUT < 65535 * 542.5 ns = 35.6 ms (28 Hz)
	 
	
	 //P.248 pour RCAP4H et RCAP4L
	 RCAP4 = 34070;
	 
	 //P.248 pour RCAP4H et RCAP4L
	 TH4 = 0x85; //On fait commencer le timer 4 au bon nombre 20ms=36864
	 TL4 = 0x16; //Pour que notre premiere periode soit bonne
	 
	 //P.247 pour T4CON
	 T4CON |= (1<<2);//Activation de timer4
	 }

	 unsigned char CDE_Servo_H (char Angle){
		 AngleAVise = 10*Angle;
		 AngleAVise = AngleAVise + 1500;
		 AngleAVise = AngleAVise*1000;
		 AngleAVise = AngleAVise/543;
		 if (Angle-Angle_actuel > 0){
			 temps = (Angle-Angle_actuel)*23/60;
		 }
		 else{
			 temps = (Angle_actuel-Angle)*23/60;
		 }
		 Angle_actuel = Angle;
		 
		 return temps;
	 }
	 
	 
	 void Int_Init(void) {
		 
	 //P.122 pour EIE2
	 EIE2 |= 0x04; //Activation des interruptions du timer 4 et de Int7 (BP)
		 
	 //P.119 pour EA
	 EA = 1; //Activation des interruptions
	 }
	 
	 void Timer4_ISR (void) interrupt 16 {
		 	T4CON &= ~(1<<7); //TF4 = 1
		 
		 if(!Commande_H ){
			 RCAP4 = 0xFFFF - (36832 - AngleAVise);
	 }
		 else {
			 RCAP4 = 0xFFFF - (AngleAVise);
		 }
		 Commande_H = !Commande_H;
 }	 