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

#include "MASTER_Config_Globale.h"

#include "FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "FO_M2__Structures_COMMANDES_INFORMATIONS_Serializer.h"

#include <F0_M1.h>
#include <F0_M2.h>
#include <F0_M3.h>
#include <F0_M4.h>
// ---

// Prototypes de Fonctions
void Timer0_ISR(void);

// Variables générales

// Partie : Interruption
	
//Variables globales
char demande_pids = 0;
bit demandeDPL = 0;
int cpt = 0;
// 
unsigned int distance;
unsigned char tempsH = 0;
bit finRotH = 0;
long int cptH = 0;

enum dplcCoord {ROT1,DPL1,ROT2,DPL2,ROT3};
enum dplcCoord dplc_en_cours = ROT1;

// Strucutre 
struct COMMANDES commandeCentraleCommande = {
		Epreuve_non,
		Mouvement_non, 20, 0, 0, 0,
		ACQ_non, 0,
		DCT_non, 0,
		Lumiere_non, 0, 0, 0, 0,
		Servo_non, 0,
		Energie_non,
		Position_non, 0, 0, 0,
		Photo_non, 0, 0
	};

struct INFORMATIONS informationsCentraleCommande = {
    Invite_non, "Message d'invite",
    BUT_Atteint_non,
    BUT_Servo_non,
    DCT_Obst_non, 0, 0,
    RESULT_Courant_non, 0,
    RESULT_Energie_non, 0,
    RESULT_Position_non, 0, 0, 0,
    Aux_non, "???"
	};

struct COMMANDES_SERIALIZER commandeSerializer = {
	Commande_non,
	0,       
	0,
	0,              
	0,         
	0,           
	0,   
	0,        
	0,  
	};

struct INFORMATIONS_SERIALIZER informationsSerializer = {
	Reponse_non, 	   
	0,           
	0,                
	0,                 
	0,                    
	0,                    
	0,                 
	0,                   
	0,          
	0,       
	};
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void) {
	// Appel des configurations globales
	Init_Device();
	Send_string_UART0("SYSTEME OK !\n\0");

	while (1)
	{
		commandeSerializer.Etat_Commande = Commande_non;
		
		commandeCentraleCommande = recuperation_structure_commande(commandeCentraleCommande);
		informationsSerializer = recuperation_structure_serializer(informationsSerializer);
		// Modif philippe 
		informationsCentraleCommande.Etat_Invite = Invite_non;
		
		// ------------------------- Gestion de l'epreuvre ---------------------------------------------
		
		switch (commandeCentraleCommande.Etat_Epreuve)
        {
					// cas : d'arret d'urgence ?
        case epreuve1:
					informationsCentraleCommande.Etat_Invite = Invite_oui;
					informationsCentraleCommande.MSG_Invit = "Epreuve 1 start !\0";
					commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
          break;
				
        case epreuve2:
					informationsCentraleCommande.Etat_Invite = Invite_oui;
					informationsCentraleCommande.MSG_Invit = "Epreuve 2 start !\0";
					commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
          break;
				
        case epreuve3:
					informationsCentraleCommande.Etat_Invite = Invite_oui;
					informationsCentraleCommande.MSG_Invit = "Epreuve 3 start !\0";
					commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
          break;
        
				case epreuve4:
					informationsCentraleCommande.Etat_Invite = Invite_oui;				
					informationsCentraleCommande.MSG_Invit = "Epreuve 4 start !\0";
					commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
          break;
				
        case epreuve5:
					informationsCentraleCommande.Etat_Invite = Invite_oui;
					informationsCentraleCommande.MSG_Invit = "Epreuve 5 start !\0";					
					commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
          break;
				
        case epreuve6:
					informationsCentraleCommande.Etat_Invite = Invite_oui;
					informationsCentraleCommande.MSG_Invit = "Epreuve 6 start !\0";
					commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
          break;
				
        case epreuve7:
					informationsCentraleCommande.Etat_Invite = Invite_oui;
					informationsCentraleCommande.MSG_Invit = "Epreuve 7 start !\0";
					commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
          break;

        case epreuve8:
					informationsCentraleCommande.Etat_Invite = Invite_oui;
					informationsCentraleCommande.MSG_Invit = "Epreuve 8 start !\0";					
					commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
          break;
				
        case Fin_Epreuve:
					informationsCentraleCommande.Etat_Invite = Invite_oui;
					informationsCentraleCommande.MSG_Invit = "Epreuve end !";					
					commandeCentraleCommande.Etat_Epreuve = Epreuve_non;
          break;
				
        default:
            break;
        }
		
		// ------------------------- Gestion du type de mouvement ---------------------------------------------

		if (commandeCentraleCommande.Etat_Mouvement == Stopper)
		{
				commandeSerializer.Etat_Commande = Stop;
				commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
		}
		else if(commandeCentraleCommande.Etat_Mouvement != Mouvement_non)
		{
			//Demande si les moteurs sont deja utilises
			if(demande_pids == 0)
			{
				commandeSerializer.Etat_Commande = Pids;
				demande_pids = 1;
			}
			// Si on recu la reponse du if precedent
			else if(informationsSerializer.Etat_Response == Rep_pids)
			{
				demande_pids = 0;
				informationsSerializer.Etat_Response = Reponse_non;
				if(informationsSerializer.Read_Pids == 0)
				{
					switch(commandeCentraleCommande.Etat_Mouvement)
					{
					case Avancer:
						commandeSerializer.Etat_Commande = mogo_1_2;
						commandeSerializer.Vitesse_Mot1 = (commandeCentraleCommande.Vitesse*28)/100;
						commandeSerializer.Vitesse_Mot2 = (commandeCentraleCommande.Vitesse*28)/100;
					
						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Reculer:
						commandeSerializer.Etat_Commande = mogo_1_2;
						commandeSerializer.Vitesse_Mot1 = -(commandeCentraleCommande.Vitesse*28)/100;
						commandeSerializer.Vitesse_Mot2 = -(commandeCentraleCommande.Vitesse*28)/100;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_90D:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = -540;
						commandeSerializer.Ticks_mot2 = 540;
						commandeSerializer.Vitesse_Mot1 = 5;
						commandeSerializer.Vitesse_Mot2 = 5;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_90G:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = 540;
						commandeSerializer.Ticks_mot2 = -540;
						commandeSerializer.Vitesse_Mot1 = 5;
						commandeSerializer.Vitesse_Mot2 = 5;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_180D:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = -1080;
						commandeSerializer.Ticks_mot2 = 1080;
						commandeSerializer.Vitesse_Mot1 = 5;
						commandeSerializer.Vitesse_Mot2 = 5;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_180G:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = 1080;
						commandeSerializer.Ticks_mot2 = -1080;
						commandeSerializer.Vitesse_Mot1 = 5;
						commandeSerializer.Vitesse_Mot2 = 5;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_AngD:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = -(abs((float)commandeCentraleCommande.Angle)/180.0) * 1080;
						commandeSerializer.Ticks_mot2 = (abs((float)commandeCentraleCommande.Angle)/180.0) * 1080;
						commandeSerializer.Vitesse_Mot1 = 5;
						commandeSerializer.Vitesse_Mot2 = 5;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_AngG:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = (abs((float)commandeCentraleCommande.Angle)/180.0) * 1080;
						commandeSerializer.Ticks_mot2 = -(abs((float)commandeCentraleCommande.Angle)/180.0) * 1080;
						commandeSerializer.Vitesse_Mot1 = 5;
						commandeSerializer.Vitesse_Mot2 = 5;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Depl_Coord:
						if(demandeDPL == 0)
						{
							demandeDPL = 1;
							switch(dplc_en_cours)
							{
								case ROT1:
									//Send_string_UART0("ROT1");
									commandeSerializer.Etat_Commande = digo_1_2;
									commandeSerializer.Ticks_mot1 = -540;
									commandeSerializer.Ticks_mot2 = 540;
									commandeSerializer.Vitesse_Mot1 = 5;
									commandeSerializer.Vitesse_Mot2 = 5;
									break;
								case DPL1:
									Send_string_UART0("DPL1");
									commandeSerializer.Etat_Commande = digo_1_2;
									commandeSerializer.Ticks_mot1 = 3150;
									commandeSerializer.Ticks_mot2 = 3150;
									commandeSerializer.Vitesse_Mot1 = 10;
									commandeSerializer.Vitesse_Mot2 = 10;
									break;
								case ROT2:
									Send_string_UART0("ROT2");
									commandeSerializer.Etat_Commande = digo_1_2;
									commandeSerializer.Ticks_mot1 = -540;
									commandeSerializer.Ticks_mot2 = 540;
									commandeSerializer.Vitesse_Mot1 = 5;
									commandeSerializer.Vitesse_Mot2 = 5;
									break;
								case DPL2:
									Send_string_UART0("DPL2");
									commandeSerializer.Etat_Commande = digo_1_2;
									commandeSerializer.Ticks_mot1 = 3150;
									commandeSerializer.Ticks_mot2 = 3150;
									commandeSerializer.Vitesse_Mot1 = 10;
									commandeSerializer.Vitesse_Mot2 = 10;
									break;
								case ROT3:
									Send_string_UART0("ROT3");
									commandeSerializer.Etat_Commande = digo_1_2;
									commandeSerializer.Ticks_mot1 = -540;
									commandeSerializer.Ticks_mot2 = 540;
									commandeSerializer.Vitesse_Mot1 = 5;
									commandeSerializer.Vitesse_Mot2 = 5;
									break;
							}
						}
						break;
					default:
						break;
					}
				}
				else
				{
					Send_string_UART0("TAMERE");
					if(commandeCentraleCommande.Etat_Mouvement == Depl_Coord)
					{
						Send_string_UART0("TEST");
						if(demandeDPL == 1)
						{
							demandeDPL = 0;
							Send_string_UART0("CHANGEMENT");
							switch(dplc_en_cours)
							{
								case ROT1:
									dplc_en_cours = DPL1;
									break;
								case DPL1:
									dplc_en_cours = ROT2;
									break;
								case ROT2:
									dplc_en_cours = DPL2;
									break;
								case DPL2:
									dplc_en_cours = ROT3;
									break;
								case ROT3:
									dplc_en_cours = ROT1;
									commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
									break;
								}
							}
					}
				}
			}
			else
			{
				commandeSerializer.Etat_Commande = Commande_non;
			}
		}
		switch (commandeCentraleCommande.Etat_ACQ_Son)
        {
        case ACQ_oui:
			//AS1		
            break;
        default:
            break;
        }

        switch (commandeCentraleCommande.Etat_DCT_Obst)
        {
        case oui_180:
			//M3 & M4
            break;
        case oui_360:
			//M3 & M4
            break;
        default:
            break;
        }

        switch (commandeCentraleCommande.Etat_Lumiere)
        {
        case Allumer:
			//MS1 && S6 && S1
            break;
        case Eteindre:
			//MS1 && S6 && S1
            break;
        default:
            break;
        }

        switch (commandeCentraleCommande.Etat_Servo)
        {
        case Servo_H:
				tempsH = CDE_Servo_H(commandeCentraleCommande.Servo_Angle);
				commandeCentraleCommande.Etat_Servo = Servo_non;
				TR0 = 1;
            break;
        case Servo_V:
			//S3
            break;
        default:
            break;
        }

        switch (commandeCentraleCommande.Etat_Energie)
        {
        case Mesure_I:
			//M5
            break;
        case Mesure_E:
			//M5
            break;
        default:
            break;
        }

		switch (commandeCentraleCommande.Etat_Position)
        {
        case Init_Position:

            break;
        case Demande_Position:
			//envoi la position actuelle
			commandeCentraleCommande.Etat_Position = Position_non;
            break;
        default:
            break;
        }

        switch (commandeCentraleCommande.Etat_Photo)
        {
        case Photo_1:

            break;
        case Photo_Multiple:

            break;
        case Photo_continue:

            break;
        case Photo_stop:

            break;
        default:
            break;
        }
		if(finRotH == 1 && informationsCentraleCommande.Etat_Invite != Invite_oui)
		{
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "AS H";
			finRotH = 0;
		}
		F0_M2_fct_encodage(commandeSerializer);
		Convertion_A_to_S(informationsCentraleCommande);
	}
}

void Timer0_ISR (void) interrupt 1 {
	cptH++;
	if(cptH >= 100*tempsH)
	{
		finRotH = 1;
		cptH = 0;
		TR0 = 0;
	}
}