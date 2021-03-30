//-----------------------------------------------------------------------------
// F0-M01.c
//
// Dernière Modification : 15/03/2021 (aucune erreur déclarées et 3 warnings) 
// Auteur : 
// 	- Maxime LERICHE
//  - Philippe CHARRAT
// TODO : 
//  - fonction d'erreur 
//  - Convertion Struct to String 
//  - 
//-----------------------------------------------------------------------------

// Import des bibliothèques ---
#include <C8051F020.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "c8051F020_SFR16.h"

#include "M3_Lib_Config_Globale_8051F020.h"
#include "MASTER_Config_Globale.h"

#include <FO-M4_Lib_Config_Globale_8051F020.h>
#include <FO-M4_Lib_Divers.h>

#include "FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include <F0_M1.h>
#include <F0_M3.h>
#include <F0_M4.h>
// ---

// Prototypes de Fonctions
sbit Commande_H = P1^4;
// Variables g�n�rales

sbit ENVOI_AV = P1^0;
sbit ENVOI_AR = P1^1;
sbit ECHO_AV = P1^2;
sbit ECHO_AR = P1^3;
bit ret;

// Partie : Configuration 
void Config_UART0(void);
void Config_interrupt(void);
void Config_Timer(void);

// Partie : Envoies et Réception des messages 
void Send_char(char c);
void Interpretation_commande(void);
void Send_string(char*);
void Send_int(int i);
void Transmettre(char caractere, bit LF);
void Affichage_UART(char*);

// Partie : Interruption
void Timer4_ISR (void);
void ISR_T2(void);
	
// Variables globales utiles
char bit_reception_UART;
int iter;
int message_a_env;

// Variables char[] 
char xdata commandes[35];

// Variables pointeurs 
char* ptrcommandes; 
char* ptrmessages;

//Variables globales
char Angle_actuel;
char temps;
long AngleAVise; 



// 
int tab_dist[5];
int* ptr_tab_dist;
// Strucutre 
struct  COMMANDES commandeenvoieStAs;
struct  INFORMATIONS informationenvoieAtSt;
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void) {
	// Appel des configurations globales
	Init_Device();  
	Config_Timer();
	Config_UART0();
	Config_interrupt();
	Send_string("SYSTEME OK !\n");
	

	while (1){
		// Partie Réception 
		if (RI0 == 1){
			RI0 = 0;
			REN0 = 0;
			// Récupération du char dans le buffer
			bit_reception_UART = SBUF0;
			// Ajout du char dans la string commande 
			commandes[iter] = bit_reception_UART;
			commandes[iter+1] = '\0';
			Send_char(bit_reception_UART);
			// Incrément du tab commande
			iter++;
			// Si fin de commande 
			if (bit_reception_UART == '\r'){			
				// Affichage en console
				Send_string("Commande recue : ");
				Send_string(commandes);
				ptrcommandes = &commandes[0];
				
				// Convertion de commande vers struct
				commandeenvoieStAs = Convertion_S_to_A(ptrcommandes);
				iter = 0;
			}
			if (bit_reception_UART == 'q'){
				message_a_env = 1;
				Send_char('\n');
				informationenvoieAtSt.Etat_BUT_Servo = BUT_Servo_V;		
			}
		}
		// Partie Emission
		if (message_a_env == 1) {
			ptrmessages = Convertion_A_to_S(informationenvoieAtSt);
			Send_string(ptrmessages);
			message_a_env = 0;
		}	
		
		if (commandeenvoieStAs.Etat_DCT_Obst == oui_180 || commandeenvoieStAs.Etat_DCT_Obst == oui_360) {
			*ptr_tab_dist = MES_Dist_AV();
			ptr_tab_dist++;
		}
		
		if (commandeenvoieStAs.Etat_Servo == Servo_H) {
				temps = CDE_Servo_H(commandeenvoieStAs.Servo_Angle);
		}
	}
}
//-----------------------------------------------------------------------------
// Fonctions de configuration des divers périphériques et interruptions
//-----------------------------------------------------------------------------
void Config_interrupt(){
	 //P.122 pour EIE2
	 EIE2 |= 0x04; //Activation des interruptions du timer 4 et de Int7 (BP)
	 
	//P.119 pour IE
	 IE |= (1<<5); //Activation de l'interruption du timer 2
		 
	 //P.119 pour EA
	 EA = 1; //Activation des interruptions 
}

void Config_UART0(void){
	// But : Configuration de l'UART 0
	SCON0 = 0x90;
}

void Config_Timer() {
	// But : Configuration du TIMER 2
	TH1 = 0xDC; //Baud-rate de 19200
	TCLK0 = 0;
	RCLK0 = 0;
	TR1 = 0; //start timer
	
	TMOD |= (1<<5);
	TMOD &= ~(0x11010000);
	
	TCON &= ~(0x11000000);
	
	CKCON |= (1<<4);
	
	//But Timer 2 : 
	 T2CON = 0x00;	
	 RCAP2 = 0x5358; //Reload au temps max d'un signal de echo
	 //P.226 pour CKCON
	 CKCON &= ~(0x32);//Utilisation de SYSCLK/12 pour le timer 2	 
	 
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

//-----------------------------------------------------------------------------
// Fonctions UART et d'envoie
//-----------------------------------------------------------------------------

void Send_string(char* mot){
	// But : Fonction pour envoyer une string de manière automatique 
	// Input : 
	//		- mot : string avec les chars à envoyer (via pointeur)
	// Output : 
	//		none
	// Tant que le char n'est pas la fin de la commande ('\r') 
	while (*mot != '\0'){
		if(*(mot+1) == '\r'){	
			Transmettre(*mot, 1); //Fin de chaine 
		} else { 
			Transmettre(*mot, 0); //milieu du mot
		}
		mot++;
	}
}
/*
void Send_int(int i){
	// But : Fonction pour envoyer une string de manière automatique 
	// Input : 
	//		- mot : string avec les chars à envoyer (via pointeur)
	// Output : 
	//		none
	ptrbuffer = convertion_int_array(i);
	// Tant que le char n'est pas la fin de la commande ('\r') 
	while (*ptrbuffer != '\0'){
		if(*(ptrbuffer+1) == '\r'){	
			Transmettre(*ptrbuffer, 1); //Fin de chaine 
		} else { 
			Transmettre(*ptrbuffer, 0); //milieu du mot
		}
		ptrbuffer++;
	}
}*/
void Send_char(char c){
	// But : Fonction pour envoyer un caractère dans l'UART
	// Input : 
	//		- c : caractère à envoyer
	// Output : 
	//		none
	//Desactive reception
	REN0 = 0;
	SBUF0 = c;
	
	//Attente fin de transmission
	while(!TI0){}
		
	//Remise à 0 du flag d'envoi une fois qu'on est sur que le caractere a été transmis
	TI0 = 0;
	REN0 = 1;
}

void Transmettre(char caractere, bit LF){
	// But : Fonction pour envoyer un caractère et la 
	// Input : 
	//		- caractere : char à envoyer
	// Output : 
	//		none
	EA = 0;
	//desactive la reception
	REN0 = 0;
	
	//Ecrit la valeur dans SBUF0 pour transmettre
	SBUF0 = caractere;
	
	//Attente de la bonne transmission
	while(!TI0){}
		
	//Remise à 0 du flag d'envoi une fois qu'on est sur que le caractere a été transmis
	TI0 = 0;
	REN0 = 1;
		
	if(LF){Transmettre(0x0D, 0);
	Transmettre(0x0A, 0);} //Retour à la ligne
	EA = 1;
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

