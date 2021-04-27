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

//Variables globales
char Angle_actuel;
sbit Commande_H = P1^4;
long AngleAVise = 2600; // Equivalent a l'angle 0

unsigned char CDE_Servo_H (char Angle){
	char temps;
	AngleAVise = 10*Angle;
	AngleAVise = AngleAVise + 1400;
	AngleAVise = AngleAVise*1010;
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