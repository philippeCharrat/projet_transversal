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
char Angle_actuel;
char temps;

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