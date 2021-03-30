#ifndef _F0_M1_
#define _F0_M1_
#include "FO_M1__Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"

char* split_element(char* ptr_commande);
int convertion_str_int(int k, char* ptr);
struct argument_complexe param_complexe(char* params);
int int_neg_or_positiv(int min, char* params);
char* ajout_char(char* ptrmessage, char c);
char* convertion_int_array(int); 
char* ajout_char_array(char* ptrbuffer, char* ptrmessage);
char* ajout_char_string(char* ptrstrings, char* ptrmessage);
void prepare_message_random(void);

// Partie : Convertion String to Structure (Array)
struct COMMANDES Convertion_S_to_A(char * ptrcommande);
void Convertion_Etat(char etat, char* ptrcommande);
void Convertion_Mouvement(char *mouvement, char* ptrcommande);
void  Convertion_Sonore(char* ptrcommande);
void Convertion_Detection(char* ptrcommande);
void Convertion_Servomoteur(char* ptrcommande);
void Convertion_Coord(char* params,char* ptrcommande);
void Convertion_Lumineux(char* params, char* ptrcommande);
void Convertion_Photo(char* params, char* ptrcommande);

// Partie : Convertion Structure to String 
char* Convertion_A_to_S(struct INFORMATIONS informationenvoieAtS);
char Convertion_Invite(char *ptrmessage);
char Convertion_Arrive(char *ptrmessage);
char Convertion_Arrive_Servo(char *ptrmessage, struct INFORMATIONS informationenvoieAS);
char Convertion_Mesure(char type, char *ptrmessage);
char Convertion_Info_Obstacle(char *ptrmessage);
char Convertion_Position(char *ptrmessage, struct INFORMATIONS informationenvoieAS);

// Définition des structures 
typedef struct argument_complexe argument_complexe;
struct argument_complexe {
	char param;
	int valeur;
};
#endif