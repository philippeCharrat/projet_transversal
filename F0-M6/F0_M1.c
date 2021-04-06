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
#include "F0_M1.h"

// ---

// Prototypes de Fonctions	
// Variables globales utiles
char message_is_set;
int commande_connue;
char message_is_ready;
int j,k,m,fin_commande,analyse_commande;
signed int i;
char bit_reception_UART0;
int iter_M1;

// Variables char[] 
char xdata buffer[5];
char xdata strings[8];
char xdata commandes_M1[35];
char xdata commande[35];
char xdata params[15];
char xdata message[50];
char xdata message_com[4];
char asuppr[15] = "Test";
char* ptr_asuppr = &asuppr[0];

// Variables pointeurs 
char* ptrcommande; 
char* ptrcommandes_M1;
char* ptrmessage;
char* ptrbuffer;
char* ptrstrings;
char* ptr_Invite;

struct argument_complexe args;
struct  COMMANDES commandeenvoieStA;
struct  INFORMATIONS informationenvoieAtS;

//-----------------------------------------------------------------------------
// Fonctions UART et d'envoie
//-----------------------------------------------------------------------------


void Send_string_UART0(char* mot){
	// But : Fonction pour envoyer une string de manière automatique 
	// Input : 
	//		- mot : string avec les chars à envoyer (via pointeur)
	// Output : 
	//		none
	// Tant que le char n'est pas la fin de la commande ('\r') 
	while (*mot != '\0'){
		if(*(mot+1) == '\r'){	
			Transmettre_UART0(*mot, 1); //Fin de chaine 
		} else { 
			Transmettre_UART0(*mot, 0); //milieu du mot
		}
		mot++;
	}
}

void Send_char_UART0(char c){
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

void Transmettre_UART0(char caractere, bit LF){
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
		
	if(LF){Transmettre_UART0(0x0D, 0);
	Transmettre_UART0(0x0A, 0);} //Retour à la ligne
	EA = 1;
}

char* split_element_M1(char* ptr_commande) {
	// But : Récupérer un paramètre (plusieurs char) et les placer dans un tableau globale. La fonction va aussi passer un flag à 1 si la commande est finie ('\r'). 
	// Input : 
	//		- *ptr_commande : pointeur vers le buffer de la commande complète. 
	// Output : 
	//		- *ptr_commande : pointeur (incrémenté des m caractères parcourus) vers le buffer de la commande complète. 
  	m = 0;
	// Tant que le char n'est pas un espace 
	while( *ptr_commande != ' ') {
		// Cas : le char est la fin de la commande  
		if (*ptr_commande == '\r') {
			// flag de fin modifié
			fin_commande = 1;
			break;
			// Cas : char est "quelconque"
		} else {
			// Ajout dans le tableau params 
			params[m] = *ptr_commande;
			params[m+1] = '\0';
			m++;
			ptr_commande++;
		}
	}
	// Saut de l'espace 
	ptr_commande++;
	return ptr_commande; 
}

struct argument_complexe param_complexe(char* params) {
	// But : Convertir string en paramètres complexe, forme : param:valeur (exemple : A:12) 
	// Input : 
	//		- *param : pointeur vers la string paramètres 
	// Output : 
	//		- args : structure composée de deux champs :
	//					* param : nom du paramètre (1 char) 
	//					* valeur : entier 
	// Remarque : Si param plus de 1 char ? Idée : boucle jusqu'à ':'
	args.param = params[0];
	args.valeur = int_neg_or_positiv(2, params);
	return args;
}

int int_neg_or_positiv(int min, char* params) {
	// But : Gestion du signe pour la convertion str vers int : ['-','1','2'] => -12
	// Input : 
	//		- min : int pour indiquer le début du nombre
	//		- *param : pointeur vers la string contenat les chars 
	// Output : 
	//		- i : int signé 
	// Cas : int négatif 
	if (params[min] == '-') {
		// Récupération de sa valeur absolue 
		i = convertion_str_int_M1(min+1, params);
		// Passage en négatif 
		i = 0-i;
		// Cas : int positif
	} else { i = convertion_str_int_M1(min, params); }
	return i; 
} 

int convertion_str_int_M1(int k, char* params) {
	// But : Convertir plusieurs chars vers un int (absolue) : ['1','2'] => 12
	// Input : 
	//		- k : int pour indiquer le début du nombre
	//		- *param : pointeur vers la string contenat les chars 
	// Output : 
	//		- atoi(buffer) : int non signé
	// Remarque : atoi() permet une convertion de char[] en int
	// Boucle jusqu'à 10 (valeur arbitraire) 
	for(j=k;j<10; j++){
		// Si le char est un digit
		if( params[j] == '1' || params[j] == '2'|| params[j] == '3'|| params[j] == '4'|| params[j] == '5'|| params[j] == '6'|| params[j] == '7'|| params[j] == '8'|| params[j] == '9'|| params[j] == '0') {
			buffer[j-k]=params[j];
		}
		else { 
			buffer[j-k]='\0';
			break;
		}
	}
	return atoi(buffer);		
}

char* convertion_int_array_M1(int i) {
	// But : Convertir plusieurs chars vers un int (absolue) : ['1','2'] => 12
	// Input : 
	//	- i : int 
	// Output : 
	//	- ptr : vers le buffer qui contient les chars
	k = 0;
if (i/1000 != 0) { 
		buffer[k] = '0'+i/1000;
		k++;
		i = i-(i/1000)*1000;
	
	} else {
	    buffer[k] = '0';
		k++;
	}
	if (i/100 != 0) { 
		buffer[k] = '0'+i/100;
		k++;
		i = i-(i/100)*100;
	} else {
	    buffer[k] = '0';
		k++;
	} 
	if (i/10 != 0) { 
		buffer[k] = '0'+i/10;
		i = i-(i/10)*10;
		k++;
	} else {
	    buffer[k] = '0';
		k++;
	}
	if (i != 0) { 
		buffer[k] = '0'+i;
		k++;
	} else {
	    buffer[k] = '0';
		k++;
	}
	buffer[k] = '\0';
	return &buffer[0];
}

char* ajout_char_array(char* ptrbuffer, char* ptrmessage) {
	// But : Convertir plusieurs chars vers un int (absolue) : ['1','2'] => 12
	// Input : 
	//		- *ptrmessage : pointeur vers le message
	//		- *ptrbuffer : pointeur vers le buffer de int
	// Output : 
	//		none
	while (1==1){
		// Si le char est un digit
		if( *ptrbuffer == '1' || *ptrbuffer == '2'|| *ptrbuffer == '3'|| *ptrbuffer == '4'|| *ptrbuffer == '5'|| *ptrbuffer == '6'|| *ptrbuffer == '7'|| *ptrbuffer == '8'|| *ptrbuffer == '9'|| *ptrbuffer == '0') {
			ptrmessage = ajout_char(ptrmessage, *ptrbuffer);
			ptrbuffer++;
		}
		else { 
			return ptrmessage;
		}
	}
}

char* ajout_char_string(char* ptrstrings, char* ptrmessage) {
	// But : Convertir plusieurs chars vers un int (absolue) : ['1','2'] => 12
	// Input : 
	//		- *ptrmessage : pointeur vers le message
	//		- *ptrbuffer : pointeur vers le buffer de int
	// Output : 
	//		none
	while (1==1){
		// Si le char est un digit
		if( *ptrstrings != '\0') {
			ptrmessage = ajout_char(ptrmessage, *ptrstrings);
			ptrstrings++;
		}
		else { 
			return ptrmessage;
		}
	}
}

char* ajout_char(char* ptrmessage, char c) {
	// But : Ajout d'un char sur message
	// Input : 
	//		- ptrmessage : pointeur vers le message
	//		- char : char à ajouter 
	// Output : 
	//		- ptrmessage : pointeur incrémenté
	*ptrmessage = c;
	++ptrmessage;
	//*ptrmessage	= '\0';
	return ptrmessage;
}

struct COMMANDES recuperation_structure_commande(struct COMMANDES pCommandeenvoieStA){
	// But : Récupération de la structure commande dans le mains 
	// Input : 
	//		- pCommandeenvoieStA  : Une structure de la commande avec les anciennes instructions
	// Output : 
	//		- pCommandeenvoieStA  : Une structure de la commande avec les nouvelles instructions
	// Partie Réception 
	if (RI0 == 1){
		commandeenvoieStA = pCommandeenvoieStA;
		RI0 = 0;
		REN0 = 0;
		// Récupération du char dans le buffer
		bit_reception_UART0 = SBUF0;
		// Ajout du char dans la string commande 
		commandes_M1[iter_M1] = bit_reception_UART0;
		commandes_M1[iter_M1+1] = '\0';
		Send_char_UART0(bit_reception_UART0);
		// Incrément du tab commande
		iter_M1++;
		// Si fin de commande 
		if (bit_reception_UART0 == '\r'){
			// Affichage en console
			Send_string_UART0("Commande recue : ");
			Send_string_UART0(commandes_M1);
			ptrcommandes_M1 = &commandes_M1[0];
			
			// Convertion de commande vers struct
			pCommandeenvoieStA = Convertion_S_to_A(ptrcommandes_M1);
			iter_M1 = 0;
		}
	}
	return pCommandeenvoieStA;
}

//-----------------------------------------------------------------------------
// Fonctions Convertion String vers Structure
//-----------------------------------------------------------------------------

struct COMMANDES Convertion_S_to_A(char * ptrcommande) {
	// But : Récupérer la partie commande et appeler les fonctions correspondante
	// Input : 
	//		none
	// Output : 
	//		none
	// Remarque : strcmp() permet de chercher une suite de char dans un autre.
	// Remarque : A vérifier que je n'ai pas oublier de fonction comme le son ou MOU ... 
	
	// Récupération de la partie commande dans le tableau params
	ptrcommande = split_element_M1(ptrcommande);
	commande_connue = 1;
	fin_commande = 0;
	analyse_commande = 0;
	
	// Test des différents cas de figures 
	if (params[0] == 'D' || params[0] == 'E' || params[0] == 'Q') {
			Convertion_Etat(params[0],ptrcommande);
	} else if (strcmp(params, "ASS") == 0) { 
			Convertion_Sonore(ptrcommande);
	} else if (strcmp(params, "MOB") == 0 || strcmp(params, "MOS") == 0 || strcmp(params, "MOU") == 0) {
			Convertion_Detection(ptrcommande);
	} else if (strcmp(params, "CS") == 0) {
			Convertion_Servomoteur(ptrcommande);
	} else if (strcmp(params, "MI") == 0 || strcmp(params, "ME") == 0 ) {
			if (strcmp(params, "MI") == 0) {	commandeenvoieStA.Etat_Energie = Mesure_I; }
			else { commandeenvoieStA.Etat_Energie = Mesure_E;}
	} else if (strcmp(params, "IPO") == 0 || strcmp(params, "POS") == 0) {
			Convertion_Coord(params,ptrcommande);
	}	else if (strcmp(params, "L") == 0 || strcmp(params, "LS") == 0 ) {
			Convertion_Lumineux(params, ptrcommande);
	}else if (strcmp(params, "PPH") == 0 || strcmp(params, "SPH") == 0 ) {
			Convertion_Photo(params, ptrcommande);
	} else if (params[0] == 'A' || params[0] == 'B' || params[0] == 'S' || strcmp(params, "RD") == 0 || strcmp(params, "RG") == 0 || strcmp(params, "RC") == 0 || strcmp(params, "RA") == 0|| strcmp(params, "TV") == 0) {
		if (commandeenvoieStA.Etat_Mouvement != Depl_Coord){
			Convertion_Mouvement(params,ptrcommande);}
	} else if (params[0] == 'G'){
			Convertion_Coord_Init(ptrcommande);
	} else {
			commande_connue = 0;
	}
	
	// Partie Réponse : 
	memset(message_com,0,10);
	ptrmessage = &message_com[0];
	ptrmessage = ajout_char(ptrmessage, 0x0D);
	ptrmessage = ajout_char(ptrmessage, 0x0A);
	if (commande_connue == 1) {	ptrmessage = ajout_char(ptrmessage, 0x3E);	}
	else {	ptrmessage = ajout_char(ptrmessage, 0x23); }
	ptrmessage = ajout_char(ptrmessage, '\r');
	Send_string_UART0(&message_com[0]);
		
	return commandeenvoieStA;
} 
   
void Convertion_Etat(char etat, char* ptrcommande) {
	// But : Fonction pour les modifications de l'état d'épreuve
	// Input : 
	//		- etat : char qui définie l'état 
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Cas D : Commencez étape 
		
	if (etat == 'D') {
		// Valeur par défaut 
		commandeenvoieStA.Etat_Epreuve = epreuve1;
		// Récupération et convertion de l'étape : 
		ptrcommande = split_element_M1(ptrcommande);
		
		i = int_neg_or_positiv(0,params);
		// Modification de la structure en fonction 
		if ( i == 1 ) {	commandeenvoieStA.Etat_Epreuve = epreuve1;	}
		else if ( i == 2 ) {commandeenvoieStA.Etat_Epreuve = epreuve2;}
		else if ( i == 3 ) {commandeenvoieStA.Etat_Epreuve = epreuve3;}
		else if ( i == 4 ) {commandeenvoieStA.Etat_Epreuve = epreuve4;}
		else if ( i == 5 ) {commandeenvoieStA.Etat_Epreuve = epreuve5;}
		else if ( i == 6 ) {commandeenvoieStA.Etat_Epreuve = epreuve6;}
		else if ( i == 7 ) {commandeenvoieStA.Etat_Epreuve = epreuve7;}
		else if ( i == 8 ) {commandeenvoieStA.Etat_Epreuve = epreuve8;}
		// Cas E : Fin de l'épreuve 
	} else if (etat == 'E') {commandeenvoieStA.Etat_Epreuve = Fin_Epreuve; }
	// Cas Q : Arrêt de l'épreuve
	else { commandeenvoieStA.Etat_Epreuve = Stop_Urgence; }
}

void Convertion_Mouvement(char *mouvement, char* ptrcommande) {
	// But : Fonction de gestion des mouvements
	// Input : 
	//		- *mouvement : pointeur pour connaitre le type de mouvement 
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Cas : Modification vitesse 
	if (strcmp(mouvement, "TV") == 0) {
					// Récupération du premier paramètre
				ptrcommande = split_element_M1(ptrcommande);
				j = int_neg_or_positiv(0, params);
				if (j > 5 && j< 100) { 
					commandeenvoieStA.Vitesse = j;	
				}
	// Cas : Avancer ou reculer
	} else if (mouvement[0] == 'A'|| mouvement[0] == 'B') {
				// Modification de la structure avec l'action 
				if (mouvement[0] == 'A') { commandeenvoieStA.Etat_Mouvement = Avancer; }
				else { commandeenvoieStA.Etat_Mouvement = Reculer; }
				if (fin_commande == 0) {
					ptrcommande = split_element_M1(ptrcommande);
					// Convertion du param
					j = int_neg_or_positiv(0, params);
					// Modification de la vitesse
					if (j > 5 && j<100) {commandeenvoieStA.Vitesse = j;	}
				}
				if (commandeenvoieStA.Vitesse == 0) {commandeenvoieStA.Vitesse = 20;}
	// Cas : Stop le mouvement
	} else if (mouvement[0]== 'S') {
			commandeenvoieStA.Etat_Mouvement = Stopper;
	// Cas : Tourner à droite de 90° 
	} else if (strcmp(mouvement, "RD") == 0) {
			commandeenvoieStA.Etat_Mouvement = Rot_90D;
	// Cas : Tourner à gauche de 90°
	} else if (strcmp(mouvement, "RG") == 0) {
			commandeenvoieStA.Etat_Mouvement = Rot_90G;
	// Cas : Rotation de 180°
	} else if (strcmp(mouvement, "RC") == 0) {
			commandeenvoieStA.Etat_Mouvement = Rot_180D;
			if (fin_commande == 0) {
				ptrcommande = split_element_M1(ptrcommande);
				// Cas : Droite 
				if (params[0] == 'D') { commandeenvoieStA.Etat_Mouvement = Rot_180D; }
				// Cas : Gauche
				else if (params[0] == 'G'){ commandeenvoieStA.Etat_Mouvement = Rot_180G; }
			}
			// Cas : Rotation d'un angle
	} else if (strcmp(mouvement, "RA") == 0) {
				// Valeurs par défaut 
				commandeenvoieStA.Etat_Mouvement =Rot_AngD;
				commandeenvoieStA.Angle = 90; 
			if (fin_commande == 0) {
				ptrcommande = split_element_M1(ptrcommande);
				// Convertion du param en struct
				if (params[1] == ':') {
					args = param_complexe(params);
					// Cas : Droite ou Gauche
					if (args.param == 'D') {commandeenvoieStA.Etat_Mouvement =Rot_AngD; }
					else if (args.param == 'G'){commandeenvoieStA.Etat_Mouvement =Rot_AngG;}
					// Ajout de l'angle
					commandeenvoieStA.Angle = args.valeur;
			}
		}
	} 
}			

void Convertion_Coord_Init(char* ptrcommande) {
	// But : Modification de l'état sonore
	// Input : 
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Modification de la structure
	commandeenvoieStA.Etat_Mouvement =Depl_Coord;
	while (fin_commande == 0) {
		// Récupérations et convertions des paramètres 
		ptrcommande = split_element_M1(ptrcommande);
		// Si l'argument est complexe
		if (params[1] == ':') { 
			args = param_complexe(params);
			// Différents cas possibles 
			if ( args.param == 'X') {
				if ( args.valeur > -99 && args.valeur < 99) {
					commandeenvoieStA.Coord_X = args.valeur;
				}
			} else if ( args.param == 'Y') {
				if ( args.valeur > -99 && args.valeur < 99) {
					commandeenvoieStA.Coord_Y = args.valeur;
				}
			} else if ( args.param == 'A') {
				if ( args.valeur > -180 && args.valeur < 180) {
					commandeenvoieStA.Angle = args.valeur;
				}
			}
		}
	} 
}

void  Convertion_Sonore(char* ptrcommande){	
	// But : Modification de l'état sonore
	// Input : 
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Valeurs par défauts
	ptrcommande = split_element_M1(ptrcommande);
			if (*ptrcommande == '0') { commandeenvoieStA.Etat_ACQ_Son = ACQ_non; }
			else {
				commandeenvoieStA.Etat_ACQ_Son = ACQ_oui;
					i = int_neg_or_positiv(0, params);
					if (i > 0 && i < 100) {	commandeenvoieStA.ACQ_Duree = i; }
					else { commandeenvoieStA.ACQ_Duree = 99; }
			}
		}
void Convertion_Detection(char* ptrcommande) {
	// But : Fonction de gestion de la détection d'obstacle
	// Input : 
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Valeurs par défauts
		if (params[2] == 'U') {
		commandeenvoieStA.Etat_DCT_Obst =oui_180_unique;
		ptrcommande = split_element_M1(ptrcommande);
		if (params[0] == '2') {
			commandeenvoieStA.Etat_DCT_Obst =oui_360_unique;
		}
	}
	else if (params[2] == 'B') {
		commandeenvoieStA.DCT_Obst_Resolution = 30;
		commandeenvoieStA.Etat_DCT_Obst = oui_360;
		// Boucle des paramètres
		while (fin_commande == 0) {
			// Récupération du paramètre
			ptrcommande = split_element_M1(ptrcommande);
			// Cas : Angle détection
			if( params[0] == '1' || params[0] == '0'|| params[0] == '2') {
				i = int_neg_or_positiv(0, params);
				if (i == 0) {	commandeenvoieStA.Etat_DCT_Obst = DCT_non;}
				else if (i == 1) {commandeenvoieStA.Etat_DCT_Obst = oui_180;}
				else {commandeenvoieStA.Etat_DCT_Obst = oui_360;}
			}
			// Cas :Résolution angulaire
			if (params[0] == 'A' ) {
				args = param_complexe(params);
				if (args.valeur > 5 && args.valeur < 45) {commandeenvoieStA.DCT_Obst_Resolution = args.valeur;}
				else { commandeenvoieStA.DCT_Obst_Resolution = 30; }
			} 
		}
	}	else if (params[2] == 'S') {
		commandeenvoieStA.DCT_Obst_Resolution = 30;
		commandeenvoieStA.Etat_DCT_Obst = oui_360_proche;
		// Boucle des paramètres
		while (fin_commande == 0) {
			// Récupération du paramètre
			ptrcommande = split_element_M1(ptrcommande);
			// Cas : Angle détection
			if( params[0] == '1' || params[0] == '0'|| params[0] == '2') {
				i = int_neg_or_positiv(0, params);
				if (i == 0) {	commandeenvoieStA.Etat_DCT_Obst = DCT_non;}
				else if (i == 1) {commandeenvoieStA.Etat_DCT_Obst = oui_180_proche;}
				else {commandeenvoieStA.Etat_DCT_Obst = oui_360_proche;}
			}
			// Cas :Résolution angulaire
			if (params[0] == 'A' ) {
				args = param_complexe(params);
				if (args.valeur > 5 && args.valeur < 45) {commandeenvoieStA.DCT_Obst_Resolution = args.valeur;}
				else { commandeenvoieStA.DCT_Obst_Resolution = 30; }
			} 
		}
	}
}
void Convertion_Servomoteur(char* ptrcommande) {	
	// But : Fonction de gestion des servomoteurs
	// Input : 
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Valeurs par défauts
	commandeenvoieStA.Servo_Angle = 0;
	commandeenvoieStA.Etat_Servo = Servo_H;
	// Tant que l'on a des paramètres : 
	while (fin_commande == 0) {
		// Récupération des paramètres
		ptrcommande = split_element_M1(ptrcommande);
		
		// Cas : Servomoteur Horizontale
		if (params[0] == 'H') { commandeenvoieStA.Etat_Servo = Servo_H; }
		// Cas : Servomoteur Verticale
		if (params[0] == 'V') { 
			commandeenvoieStA.Etat_Servo = Servo_V; }
		// Ajout de l'angle
		if (params[0] == 'A') {
			args = param_complexe(params);	
			commandeenvoieStA.Servo_Angle = args.valeur;
		}
	}
}

void Convertion_Coord(char* params,char* ptrcommande) {	
	// But : Fonction de gestion des coordonnées
	// Input : 
	//		- *params : pointeur vers le type de commande
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Cas : Initialisation de coord
	if (strcmp(params, "IPO") == 0) {
		// Valeurs par défaults
		commandeenvoieStA.Etat_Position = Init_Position;
		commandeenvoieStA.Pos_Angle = 0;
		// Boucle pour récupérer les paramètres
		while (fin_commande == 0) {
			// Récupération des paramètres
			ptrcommande = split_element_M1(ptrcommande);
			args = param_complexe(params);
			// Différents cas : 
			if ( args.param == 'X') { commandeenvoieStA.Pos_Coord_X = args.valeur; } 
			if ( args.param == 'Y') { commandeenvoieStA.Pos_Coord_Y = args.valeur; }
			if ( args.param == 'A') { commandeenvoieStA.Pos_Angle = args.valeur; }
		}
		// Sinon : Récupération de coord
	}	else {commandeenvoieStA.Etat_Position = Demande_Position;}			
}

void Convertion_Lumineux(char* params, char* ptrcommande) {
	// But : Fonction de gestion du pointeur lumineux
	// Input : 
	//		- *mouvement : pointeur pour connaitre le type de commandes_M1 
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Cas : Fin  
	if (strcmp(params, "LS") == 0) {
		commandeenvoieStA.Etat_Lumiere = Eteindre;
		// Cas : Initialisation 
	} else {
		// Valeurs par défaults
		commandeenvoieStA.Etat_Lumiere = Allumer;
		commandeenvoieStA.Lumiere_Intensite = 100;			
		commandeenvoieStA.Lumiere_Duree = 99;
		commandeenvoieStA.Lumire_Extinction = 0;
		commandeenvoieStA.Lumiere_Intensite = 1;
		// Boucle pour récupérer les paramètres 
		while (fin_commande == 0) {
			// Récupérations et convertions des paramètres 
			ptrcommande = split_element_M1(ptrcommande);
			// Si l'argument est complexe
			if (params[1] == ':') {
				args = param_complexe(params);
				// Différents cas possibles 
				if ( args.param == 'I') {
					if ( args.valeur > 0 && args.valeur < 101) {
						commandeenvoieStA.Lumiere_Intensite = args.valeur;
					}
				} else if ( args.param == 'D') {
					if ( args.valeur > 0 && args.valeur < 101) {
						commandeenvoieStA.Lumiere_Duree = args.valeur;
					}
				} else if ( args.param == 'E') {
					if ( args.valeur > 0 && args.valeur < 101) {
						commandeenvoieStA.Lumire_Extinction = args.valeur;
					}
				} else if ( args.param == 'N') {
					if ( args.valeur > 0 && args.valeur < 101) {
						commandeenvoieStA.Lumiere_Nbre = args.valeur;
					}
				}
				
			} // Sinon, ignoré
		}
	}
}

void Convertion_Photo(char* params, char* ptrcommande) {
	// But : Fonction de gestion des photos
	// Input : 
	//		- *params : pointeur pour connaitre le type de commandes_M1 
	//		- *ptrcommande : pointeur vers les paramètres possibles 
	// Output : 
	//		none
	// Cas : Initialisation  
	if (strcmp(params, "PPH") == 0) {
		// Valeurs par défaut 
		commandeenvoieStA.Etat_Photo = Photo_1;
		commandeenvoieStA.Photo_Duree = 1;
		commandeenvoieStA.Photo_Nbre = 1;
		
		while (fin_commande == 0) {
			// Récupérations et convertions des paramètres 
			ptrcommande = split_element_M1(ptrcommande);
			// Différents cas possibles 
			if ( params[0] == 'O') {
				commandeenvoieStA.Etat_Photo = Photo_1;
			}
			if ( params[0] == 'S') {
				commandeenvoieStA.Etat_Photo = Photo_Multiple; 
			}
			if ( params[0] == 'E') {
				args = param_complexe(params);
				commandeenvoieStA.Photo_Duree = args.valeur;
			}
			if ( params[0] == 'N') {
				args = param_complexe(params);
				commandeenvoieStA.Photo_Nbre = args.valeur;
			}
		}
		// Cas : Fin de photo
	} else {
			commandeenvoieStA.Etat_Photo = Photo_stop;
	}
}

//-----------------------------------------------------------------------------
// Fonctions Convertion Structure vers String
//-----------------------------------------------------------------------------

void Convertion_A_to_S(struct INFORMATIONS informationenvoieAS) {
	// But : Convertion de la structure en string
	// Input : 
	//		none
	// Output : 
	//		none
	// Initialisation des variables
	message_is_set = 0;
	memset(message,0,30);
	ptrmessage = &message[0];
	// Différents cas possible 
	// Si le message en lien avec l'état 
	if (informationenvoieAS.Etat_Invite == Invite_oui && message_is_set==0) {
		message_is_set= Convertion_Invite(ptrmessage,informationenvoieAS);
		
	}
	// Si le message en lien avec l'arrivé
	if (informationenvoieAS.Etat_BUT_Mouvement == BUT_Atteint_oui && message_is_set==0) {
		message_is_set= Convertion_Arrive(ptrmessage); 
	}
	if (informationenvoieAS.Etat_BUT_Servo == BUT_Atteint_oui && message_is_set==0) {
		message_is_set= Convertion_Arrive_Servo(ptrmessage,informationenvoieAS); 
	}
	// Si le message en lien avec le courant
	if (informationenvoieAS.Etat_RESULT_Courant == RESULT_Courant_oui && message_is_set==0) {
		message_is_set=Convertion_Mesure('I',ptrmessage);
	}
	// Si le message en lien avec l'énergie
	if (informationenvoieAS.Etat_RESULT_Energie == RESULT_Energie_oui && message_is_set==0) {
		message_is_set=Convertion_Mesure('E',ptrmessage);
	}
	// Si le message en lien avec l'angle
	if (informationenvoieAS.Etat_RESULT_Position == RESULT_Position_oui && message_is_set==0) {
		
		message_is_set=Convertion_Position(ptrmessage, informationenvoieAS);
	}
	// Si le message est initialisé
	if (message_is_set=='1') {
		ptrmessage = ajout_char(ptrmessage, '\n');
		ptrmessage = ajout_char(ptrmessage, '\r');
		Send_string_UART0(&message[0]);
	}
}	

char Convertion_Invite(char *ptrmessage,struct INFORMATIONS informationenvoieAS) {
	// But : Fonction Invité de Commande
	// Input : 
	//		- *ptrmessage : pointeur vers le char[] pour le message
	// Output : 
	//		- char : pour vérifier le bodéroulement 
	// Ajout de la partie Information 
	ptrmessage = ajout_char(ptrmessage, 'I');
	ptrmessage = ajout_char(ptrmessage, ' '); 
	strcat(message,informationenvoieAS.MSG_Invit);
	
	
	return '1';
}

char Convertion_Arrive(char *ptrmessage) {
	// But : Fonction Arrivé 
	// Input : 
	//		- *ptrmessage : pointeur vers le char[] poour le message
	// Output : 
	//		- char : pour vérifier le bon déroulement 
	// Ajout de la partie Information 
	ptrmessage = ajout_char(ptrmessage, 'B');
	return '1';
}

char Convertion_Arrive_Servo(char *ptrmessage, struct INFORMATIONS informationenvoieAS) {
	// But : Fonction Arrivé 
	// Input : 
	//		- *ptrmessage : pointeur vers le char[] poour le message
	// Output : 
	//		- char : pour vérifier le bon déroulement 
	// Ajout de la partie Information 
	strcpy(strings,"ASS ");
	ptrstrings = &strings[0];
	ptrmessage = ajout_char_string(ptrstrings, ptrmessage);
	if (informationenvoieAS.Etat_BUT_Servo == BUT_Servo_V) {
		ptrmessage = ajout_char(ptrmessage, 'V');
	}
	if (informationenvoieAS.Etat_BUT_Servo == BUT_Servo_H) {
		ptrmessage = ajout_char(ptrmessage, 'H');
	}
	return '1';
}

char Convertion_Mesure(char type, char *ptrmessage) {
	// But : Fonction Mesure
	// Input : 
	//		- *ptrmessage : pointeur vers le char[] poour le message
	// Output : 
	//		- char : pour vérifier le bo n déroulement 
	// Ajout de la partie Information 
	ptrmessage = ajout_char(ptrmessage, 'K');
	// Cas : Courant 
	if (type == 'I') {
		// Ajout de ' ' et valeur 
		ptrmessage = ajout_char(ptrmessage, type);
		ptrmessage = ajout_char(ptrmessage, ' ');
		ptrbuffer = convertion_int_array_M1(informationenvoieAtS.Mesure_Courant);
		ptrmessage = ajout_char_array(ptrbuffer,ptrmessage);
		return 1;
	}
	// Cas : Energie
	if (type == 'E') {
		// Ajout de ' ' et valeur 
		ptrmessage = ajout_char(ptrmessage, type);
		ptrmessage = ajout_char(ptrmessage, ' ');
		ptrbuffer = convertion_int_array_M1(informationenvoieAtS.Mesure_Energie);
		ptrmessage = ajout_char_array(ptrbuffer,ptrmessage);
		return 1;
	}
	ptrmessage--;
	return 0;
}

char Convertion_Info_Obstacle(char *ptrmessage) {
	// Todo :A comprendre mdrrrr
	ptrmessage++;
	return 1;
}

char Convertion_Position(char *ptrmessage, struct INFORMATIONS informationenvoieAS) {
	strcpy(strings," VPO X:\0");
	ptrstrings = &strings[0];
	ptrmessage = ajout_char_string(ptrstrings, ptrmessage);
	ptrbuffer = convertion_int_array_M1(informationenvoieAS.Pos_Coord_X);
	ptrmessage = ajout_char_array(ptrbuffer,ptrmessage);
	
	strcpy(strings," Y:\0");
	ptrstrings = &strings[0];	
	ptrmessage = ajout_char_string(ptrstrings, ptrmessage);
	ptrbuffer = convertion_int_array_M1(informationenvoieAS.Pos_Coord_Y);
	ptrmessage = ajout_char_array(ptrbuffer,ptrmessage);
	
	strcpy(strings," A:\0");
	ptrstrings = &strings[0];	
	ptrmessage = ajout_char_string(ptrstrings, ptrmessage);
	ptrbuffer = convertion_int_array_M1(informationenvoieAS.Pos_Angle);
	ptrmessage = ajout_char_array(ptrbuffer,ptrmessage);
	return 1;
}

void prepare_message_random(void) {
	message_is_ready = 1;
	informationenvoieAtS.Etat_Invite = Invite_oui ;
	informationenvoieAtS.MSG_Invit = ptr_asuppr;
}
