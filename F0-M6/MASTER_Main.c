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

// Variables g�n�rales

// Partie : Interruption
	
//Variables globales
char temps;
char demande_pids = 0;

// 
unsigned int distance;

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
		commandeCentraleCommande = recuperation_structure_commande(commandeCentraleCommande);
		informationsSerializer = recuperation_structure_serializer(informationsSerializer);

		// ------------------------- Gestion de l'epreuvre ---------------------------------------------
		
		switch (commandeCentraleCommande.Etat_Epreuve)
        {
        case epreuve1:
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "Epreuve 1 start !";
			
			commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
            break;
        case epreuve2:
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "Epreuve 2 start !";
			
			commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
            break;
        case epreuve3:
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "Epreuve 3 start !";
			
			commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
            break;
        case epreuve4:
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "Epreuve 4 start !";
			
			commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
            break;
        case epreuve5:
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "Epreuve 5 start !";
			
			commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
            break;
        case epreuve6:
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "Epreuve 6 start !";
			
			commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
            break;
        case epreuve7:
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "Epreuve 7 start !";
			
			commandeCentraleCommande.Etat_Epreuve = Fin_Epreuve;
            break;
        case epreuve8:
			informationsCentraleCommande.Etat_Invite = Invite_oui;
			informationsCentraleCommande.MSG_Invit = "Epreuve 8 start !";
			
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
						commandeSerializer.Ticks_mot1 = (90/180) * 100;
						commandeSerializer.Ticks_mot2 = (90/180) * 100;
						commandeSerializer.Vitesse_Mot1 = 10;
						commandeSerializer.Vitesse_Mot2 = -10;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_90G:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = (90/180) * 100;
						commandeSerializer.Ticks_mot2 = (90/180) * 100;
						commandeSerializer.Vitesse_Mot1 = -10;
						commandeSerializer.Vitesse_Mot2 = 10;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_180D:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = 100;
						commandeSerializer.Ticks_mot2 = 100;
						commandeSerializer.Vitesse_Mot1 = 10;
						commandeSerializer.Vitesse_Mot2 = -10;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_180G:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = 100;
						commandeSerializer.Ticks_mot2 = 100;
						commandeSerializer.Vitesse_Mot1 = -10;
						commandeSerializer.Vitesse_Mot2 = 10;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_AngD:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = (abs(commandeCentraleCommande.Angle)/180) * 100;
						commandeSerializer.Ticks_mot2 = (abs(commandeCentraleCommande.Angle)/180) * 100;
						commandeSerializer.Vitesse_Mot1 = 10;
						commandeSerializer.Vitesse_Mot2 = -10;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Rot_AngG:
						commandeSerializer.Etat_Commande = digo_1_2;
						commandeSerializer.Ticks_mot1 = (abs(commandeCentraleCommande.Angle)/180) * 100;
						commandeSerializer.Ticks_mot2 = (abs(commandeCentraleCommande.Angle)/180) * 100;
						commandeSerializer.Vitesse_Mot1 = -10;
						commandeSerializer.Vitesse_Mot2 = 10;

						commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
						break;
					case Depl_Coord:
						// A faire !
						if(commandeCentraleCommande.Pos_Coord_X = commandeCentraleCommande.Coord_X)
						{
							if(commandeCentraleCommande.Pos_Coord_Y = commandeCentraleCommande.Coord_Y)
							{
								if(commandeCentraleCommande.Pos_Angle = commandeCentraleCommande.Angle)
								{
									commandeCentraleCommande.Etat_Mouvement = Mouvement_non;
								}
							}
						}
						break;
					default:
						break;
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
            //M3
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
			informationsCentraleCommande.Etat_RESULT_Position = RESULT_Position_oui;
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
			Convertion_A_to_S(informationsCentraleCommande);
	}
}