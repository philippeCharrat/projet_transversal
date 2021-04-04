#ifndef __BYTE_DEF__
#define __BYTE_DEF__

typedef unsigned char BYTE;
typedef char SIGNED_BYTE;

#endif



#ifndef __FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande__
#define __FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande__


///*****************************************************************************************************************************************
// Structure de donn�es pour les commandes transmises par la centrale ce commande
// Les commandes d�cod�es par FO-M1 seront stock�es dans une structure de ce type pour �tre utilis�es par FO-M5 
//*****************************************************************************************************************************************

enum Epreuve {Epreuve_non, epreuve1, epreuve2, epreuve3, epreuve4, epreuve5, epreuve6, epreuve7, epreuve8, Fin_Epreuve, Stop_Urgence};
enum Mouvement {Mouvement_non, Avancer, Reculer, Stopper, Rot_90D, Rot_90G, Rot_180D, Rot_180G, Rot_AngD, Rot_AngG, Depl_Coord}; 
enum ACQ_Son {ACQ_non, ACQ_oui};
enum DCT_Obst {DCT_non, oui_180, oui_360};
enum Lumiere {Lumiere_non, Allumer, Eteindre};
enum Servo {Servo_non, Servo_H, Servo_V};
enum Energie{Energie_non,Mesure_I,Mesure_E};
enum Position{Position_non, Init_Position, Demande_Position};
enum Photo{Photo_non,Photo_1,Photo_Multiple,Photo_continue,Photo_stop}; 

typedef struct COMMANDES       // Cette structure contient toutes les commandes envoy�es par la centrale de commande
                               // Les variables de type enum correspondent aux commandes
                               // Les variables de type BYTE, UNSIGNED_BYTE ou int correspondent aux param�tres
                
{           
   enum  Epreuve Etat_Epreuve;     //Contient le num�ro d'�preuve en cours - Commandes "D" "E" "Q"
   
   enum  Mouvement Etat_Mouvement; //Contient le type de mouvement demand� - Commandes "A" "B" "S" "RD" "RG" "RC" "RA" "G"
                                   // Param�tres des commandes pr�c�dentes:     
   BYTE  Vitesse;                  // Param�tre vitesse - Peut aussi �tre modifi� par la commande "TV"
   BYTE  Coord_X;	                 // Param�tre coordonn�e X
   BYTE  Coord_Y;                  // Param�tre coordonn�e Y
   int   Angle;                    // Param�tre angle
   
   enum  ACQ_Son Etat_ACQ_Son;     // Contient la commande d'acquisition de signaux sonores "ASS"
                                   // Param�tre de la commande pr�c�dente:     
   BYTE  ACQ_Duree;                // Param�tre dur�e de la s�quence d'acquisition
   
   enum  DCT_Obst Etat_DCT_Obst;   // Contient la commande de d�tection d'obstacle "MOB D" "MOB"
                                   // Param�tre de la commandes pr�c�dente:     
   BYTE  DCT_Obst_Resolution;      // Param�tre r�solution angulaire de la d�tection d'obstacle
   
   enum  Lumiere Etat_Lumiere;     // Contient la commande d'allumage du pointeur lumineux "L" "LS"
                                   // Param�tres de la commande pr�c�dente:     
   BYTE  Lumiere_Intensite;        // Param�tre intensit� lumineuse 							  
   BYTE  Lumiere_Duree;            // Param�tre dur�e d'allumage
   BYTE  Lumire_Extinction;        // Param�tre dur�e d'extinction
   BYTE  Lumiere_Nbre;             // Param�tre nombre de cycles d'allumage/extinction
   
   enum  Servo Etat_Servo;        // Contient la commande de positionnement d'un servo "CS C" "CS H" "CS V" 
                                  // Param�tre de la commande pr�c�dente:     
   SIGNED_BYTE  Servo_Angle;      // Param�tre angle de rotation
   enum  Energie Etat_Energie;    // Contient la commande d'envoi du courant instantan� ou de l'energie consomm�e - Commandes "MI" "ME"
   enum  Position Etat_Position;  // Contient les commandes de gestion de la position, initialisation ou demande d'envoi
		                              // de la position - Commandes: "IPO" "POS"		
   BYTE  Pos_Coord_X;	            // Param�tre coordonn�e X pour initialisation de la position
   BYTE  Pos_Coord_Y;             // Param�tre coordonn�e Y pour initialisation de la position
   int   Pos_Angle;             // Param�tre angle pour initialisation de la position	 
	
   enum  Photo Etat_Photo;        // Contient les commandes de la gestion de la prise de photo - Commandes "PPH" "SPH"
	BYTE  Photo_Duree;	            // Param�tre dur�e entre 2 photos
   BYTE  Photo_Nbre;              // Param�tre nombre de photos	 
		 
};



//*****************************************************************************************************************************************
// Structure de donn�es pour les informations � emettre vers la centrale ce commande
// Les donn�es "Informations" produites par FO-M5  seront stock�es dans une structure de ce type pour �tre utilis�es par FO-M1 qui devra
// les encoder en messages ASCII 
//*****************************************************************************************************************************************

enum Invite {Invite_non, Invite_oui};
enum BUT_Mouvement {BUT_Atteint_non, BUT_Atteint_oui};
enum BUT_Servo {BUT_Servo_non, BUT_Servo_H, BUT_Servo_V, BUT_Servo_C};
enum RESULT_Courant{RESULT_Courant_non,RESULT_Courant_oui};
enum RESULT_Energie{RESULT_Energie_non,RESULT_Energie_oui};
enum RESULT_Position{RESULT_Position_non,RESULT_Position_oui};
enum RESULT_DCT_Obst {DCT_Obst_non, DCT_Obst_180_oui, DCT_Obst_360_oui};
enum Aux {Aux_non, Aux_oui};

typedef struct INFORMATIONS       // Cette structure contient toutes les informations susceptibles d'�tre transmises � la centrale de commande
	                                // Les variables de type enum correspondent aux types des informations
                                  // Les variables de type BYTE, UNSIGNED_BYTE ou int correspondent aux param�tres
                
{           
   enum  Invite Etat_Invite;      // Contient l'identificateur information d'invite "I"
                                  // Param�tre de l'information pr�c�dente:     
   char  *MSG_Invit;	            //Pointeur sur une chaine de caract�re d'invite     
   
   enum  BUT_Mouvement Etat_BUT_Mouvement; // Contient l'identificateur information Arriv�e au point sp�cifi� "B"
	                                         // Pas de param�tre associ� pour l'information pr�c�dente:     
   enum  BUT_Servo Etat_BUT_Servo;         // Contient l'identificateur information Servomoteur positionn� "AS C" "AS H" "AS H"
	                                         // Pas de param�tre associ� pour l'information pr�c�dente:     
	 
   enum  RESULT_DCT_Obst Etat_DCT_Obst;    // Contient l'identificateur information D�tection des obstacles "KOB"
                                           // Param�tre de l'information pr�c�dente:    
	 int   *Tab_Val_Obst;                    //Pointeur sur un tableau de valeurs de distance
	 BYTE	 Nbre_Val_obst;                    //Nombre de points dans le tableau
	 
	 enum RESULT_Courant Etat_RESULT_Courant; // Contient un idendicateur d'information "courant mesur�"
	 unsigned int Mesure_Courant;            // Valeur du courant mesur� en mA   
	 
	 enum RESULT_Energie Etat_RESULT_Energie; // Contient un idendicateur d'information "Energie Consomm�e"
	 unsigned int Mesure_Energie;            // Valeur de l'�nergie consomm�e depuis le d�but de l'�preuve
	 
	 enum RESULT_Position Etat_RESULT_Position; // Contient un idendicateur d'information "Position"
   BYTE  Pos_Coord_X;	                     // Information Param�tre coordonn�e X
   BYTE  Pos_Coord_Y;                      // Information Param�tre coordonn�e Y
   int   Pos_Angle;                        // Information Param�tre angle
	 
   enum   Aux Etat_Aux;                    // Contient l'identificateur information auxiliaires "IA"
                                           // Param�tre de l'information pr�c�dente:     
   char  *MSG_Aux;	                       //Pointeur sur une chaine de caract�re d'informations auxiliaires  
};

#endif
// *******************************************************************************************
