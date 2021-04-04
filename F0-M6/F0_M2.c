//-----------------------------------------------------------------------------
// F0_M2
//-----------------------------------------------------------------------------
// AUTH: 
// DATE: 
//
// Target: C8051F02x
// Tool chain: KEIL Microvision 4
//
//-----------------------------------------------------------------------------
#include <C8051F020.h>
#include "c8051F020_SFR16.h"
#include "FO_M2__Structures_COMMANDES_INFORMATIONS_Serializer.h"
#include <F0_M2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RX_LENGTH    16                // length of UART RX buffer

// Initialisation Structures :
struct COMMANDES_SERIALIZER Envoi;
struct INFORMATIONS_SERIALIZER Reception;

char bit_reception_UART1;
int iter_M2 = 0;
char xdata commandes_M2[35];
char* ptr_commande;
char xdata nom_commandes_M2[35];
char argument1[10] = "";
char argument2[10] = "";
char argument3[10] = "";
char argument4[10] = "";

//-----------------------------------------------------------------------------
// Fonctions UART et d'envoie
//-----------------------------------------------------------------------------

void Send_char_M2(char c)
{
	
	//Desactive reception!
	SCON1 &= 0xEF; //REN1 = 0;
	SBUF1 = c;
	
	//Attente fin de transmission
	while((SCON1 & 0x02)==0x00){} //TI1 == 0
		
	//Remise � 0 du flag d'envoi une fois qu'on est sur que le caractere a �t� transmis
	SCON1 &= 0xFD; //TI1 = 0;
	SCON1 |= 0x10; //REN1 = 1;
}

void Send_string_M2(char* mot)
{
	
	//Tant qu'on n'a pas fini de lire toute la chaine
    while (*mot != '\0'){
        
        //Check si on est en train de lire le dernier caractere
        if(*(mot+1) == '\0'){
            Send_char_M2(*mot);
            Send_char_M2(0x0D); //<CR>
            Send_char_M2('\n');
        }
        
        else {
            Send_char_M2(*mot);
        }
        
        //Incremente la case memoire de lecture
        mot++;
    }
    Send_char_M2('\n');
}

char extraction(char* ptr_buffer)
{
    if(*ptr_commande == '\r' || *ptr_commande == '\0'){
        return 0;
    }

    //Tant qu'on n'est pas a la fin de la chaine
    while(*ptr_commande != '\r' && *ptr_commande != '\0' && *ptr_commande != ':'){
        //On ecrit le caractere de le buffer
        *ptr_buffer = *ptr_commande;
        *(ptr_buffer+1) = '\0';
        ptr_commande++;
        ptr_buffer++;
    }

    //Fin de chaine sans suite
    if(*ptr_commande == '\r' || *ptr_commande == '\0'){
        return 0;
    }
    
    //Il y a une suite (espace)
    return 1;
}

char extraction_arguments()
{
    char continuer;
    ptr_commande = &commandes_M2[0];
    strcpy(argument1, "");
    strcpy(argument2, "");
    strcpy(argument3, "");
    strcpy(argument4, "");

    continuer = extraction(argument1);
    if (continuer != 1){
        return continuer;
    }

    continuer = extraction(argument2);
    if (continuer != 1){
        return continuer;
    }

    continuer = extraction(argument3);
    if (continuer != 1){
        return continuer;
    }

    continuer = extraction(argument4);
    if (continuer != 1){
        return continuer;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Fonctions Convertion String vers Structure
//-----------------------------------------------------------------------------

struct INFORMATIONS_SERIALIZER recuperation_structure_serializer(struct INFORMATIONS_SERIALIZER STRUCT_SERIALIZER)
{
	if ((SCON1 & 0x01)==0x01) //(RI1 == 1)
    {
        SCON1 &= 0xFE; //RI1 = 0;
        SCON1 &= 0xEF; //REN1 = 0;
            
        // Récupération du char dans le buffer
        bit_reception_UART1 = SBUF1;
			
		if(bit_reception_UART1 != '>' && bit_reception_UART1 != '\n')
        {
            commandes_M2[iter_M2] = bit_reception_UART1;
            commandes_M2[iter_M2+1] = '\0';
            iter_M2++;
	    }

        if (bit_reception_UART1 == '\r')
        {
            STRUCT_SERIALIZER = F0_M2_fct_decodage(STRUCT_SERIALIZER);
        }

        SCON1 |= 0x10; //REN1 = 1;
	}
	return STRUCT_SERIALIZER;	
}

struct INFORMATIONS_SERIALIZER F0_M2_fct_decodage(struct INFORMATIONS_SERIALIZER STRUCT_INFOS)
{
	extraction_arguments();

	//Rep_getenc
	if (strcmp(nom_commandes_M2, "Rep_getenc_1_2") == 0){
		STRUCT_INFOS.Etat_Response = Rep_getenc;
		STRUCT_INFOS.Read_Val_enc_1 = atoi(argument1);
		STRUCT_INFOS.Read_Val_enc_2 = atoi(argument2);
		
	}

	//Rep_vpid
	else if (strcmp(nom_commandes_M2, "Rep_vpid") == 0){
		STRUCT_INFOS.Etat_Response = Rep_vpid;
		STRUCT_INFOS.Read_P = atoi(argument1);
		STRUCT_INFOS.Read_I = atoi(argument2);
		STRUCT_INFOS.Read_D = atoi(argument3);
		STRUCT_INFOS.Read_L_A = atoi(argument4);
		
	}

	//Rep_dpid
	else if (strcmp(nom_commandes_M2, "Rep_dpid") == 0){
		STRUCT_INFOS.Etat_Response = Rep_dpid;
		STRUCT_INFOS.Read_P = atoi(argument1);
		STRUCT_INFOS.Read_I = atoi(argument2);
		STRUCT_INFOS.Read_D = atoi(argument3);
		STRUCT_INFOS.Read_L_A = atoi(argument4);

	}

	//Rep_pids
	else if (strcmp(nom_commandes_M2, "Rep_pids") == 0){
		STRUCT_INFOS.Etat_Response = Rep_pids;
		STRUCT_INFOS.Read_Pids = atoi(argument1);
	}

	//Rep_vel
	else if (strcmp(nom_commandes_M2, "Rep_vel") == 0){
		STRUCT_INFOS.Etat_Response = Rep_vel;
		STRUCT_INFOS.Read_Vitesse_mot1 = atoi(argument1);
		STRUCT_INFOS.Read_Vitesse_mot2 = atoi(argument2);
	}

	return STRUCT_INFOS;
}

//-----------------------------------------------------------------------------
// Fonctions Convertion Structure vers String
//-----------------------------------------------------------------------------

void F0_M2_fct_encodage(struct COMMANDES_SERIALIZER structure)
{
	char Message[50]="";
	char Tampon[10];
	strcpy(Message, "");

    //Reset
	if (structure.Etat_Commande == Reset)
    {
		strcat(Message,"reset");
	}

    //Stop
	if (structure.Etat_Commande == Stop){
		strcat(Message,"stop");
	}

	//mogo_1
	if (structure.Etat_Commande == mogo_1)
    {
		strcat(Message,"mogo 1:");
		sprintf(Tampon,"%d", (int)structure.Vitesse_Mot1); //char 
		strcat(Message,Tampon);
	}

	//mogo_2
	if (structure.Etat_Commande == mogo_2)
    {
		strcat(Message,"mogo 2:");
		sprintf(Tampon,"%d",structure.Vitesse_Mot2);
		strcat(Message,Tampon);
	}

	//mogo_1_2
	if (structure.Etat_Commande == mogo_1_2)
    {
		strcat(Message,"mogo 1:");
		sprintf(Tampon,"%d",structure.Vitesse_Mot1);
		strcat(Message,Tampon);
		strcat(Message," 2:");
		sprintf(Tampon,"%d",structure.Vitesse_Mot2);
		strcat(Message,Tampon);		
	}

	//Vpid_read
	if (structure.Etat_Commande == Vpid_read)
    {
		strcat(Message,"vpid");	
                		
		//Prepare de quoi reconnaitre la reponse
		strcpy(nom_commandes_M2, "Rep_vpid");
	}

	//Vpid_set
	if (structure.Etat_Commande == Vpid_set)
    {
		strcat(Message,"vpid ");
		sprintf(Tampon,"%d:",structure.Set_P);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d:",structure.Set_I);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d:",structure.Set_D);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d",structure.Set_L_A);
		strcat(Message,Tampon);
	}

	//digo_1
	if (structure.Etat_Commande == digo_1)
    {
		strcat(Message,"digo 1:");
		sprintf(Tampon,"%d:",structure.Ticks_mot1);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d",structure.Vitesse_Mot1);
		strcat(Message,Tampon);
	}

	//digo_2
	if (structure.Etat_Commande == digo_2)
    {
		strcpy(Message,"digo 2:");
		sprintf(Tampon,"%d:",structure.Ticks_mot2);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d",structure.Vitesse_Mot2);
		strcat(Message,Tampon);
	}

	//digo_1_2
	if (structure.Etat_Commande == digo_1_2)
    {
		strcpy(Message,"digo 1:");
		sprintf(Tampon,"%d:",structure.Ticks_mot1);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d ",structure.Vitesse_Mot1);
		strcat(Message,Tampon);
		
		strcat(Message,"2:");
		sprintf(Tampon,"%d:",structure.Ticks_mot2);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d",structure.Vitesse_Mot2);
		strcat(Message,Tampon);
	}

	//Dpid_read
	if (structure.Etat_Commande == Dpid_read)
    {
		strcat(Message,"Dpid_read");
                		
		//Prepare de quoi reconnaitre la reponse
		strcpy(nom_commandes_M2, "Rep_dpid");
	}

	//Dpid_set
	if (structure.Etat_Commande == Dpid_set)
    {
		strcat(Message,"dpid ");
		sprintf(Tampon,"%d:",structure.Set_P);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d:",structure.Set_I);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d:",structure.Set_D);
		strcat(Message,Tampon);
		sprintf(Tampon,"%d",structure.Set_L_A);
		strcat(Message,Tampon);	
	}
	
	//Rpid_Stinger
	if (structure.Etat_Commande == Rpid_Stinger){
		strcat(Message,"rpid s");
        		
		//Prepare de quoi reconnaitre la reponse
		strcpy(nom_commandes_M2, "Rep_rpid");
    }

    //Pids
	if (structure.Etat_Commande == Pids){
		strcat(Message,"pids");
		
		//Prepare de quoi reconnaitre la reponse
		strcpy(nom_commandes_M2, "Rep_pids");
	}
	
	//Getenc_1
	if (structure.Etat_Commande == Getenc_1)
    {
		strcat(Message,"getenc 1");
        			
		//Prepare de quoi reconnaitre la reponse
		strcpy(nom_commandes_M2, "getenc_1");
	}
	
	//Getenc_2
	if (structure.Etat_Commande == Getenc_2){
		strcat(Message,"getenc 2");
        			
		//Prepare de quoi reconnaitre la reponse
		strcpy(nom_commandes_M2, "getenc_2");
	}
	
	//Getenc_1_2
	if (structure.Etat_Commande == Getenc_1_2){
		strcat(Message,"getenc 1 2");
			
		//Prepare de quoi reconnaitre la reponse
		strcpy(nom_commandes_M2, "getenc_1_2");
	}
	
	//Clrenc_1
	if (structure.Etat_Commande == Clrenc_1){
		strcat(Message,"clrenc 1");
	}
	
	//Clrenc_2
	if (structure.Etat_Commande == Clrenc_2){
		strcat(Message,"clrenc 2");
	}
	
	//Clrenc_1_2
	if (structure.Etat_Commande == Clrenc_1_2){
		strcat(Message,"clrenc 1 2");
	}
	
	//Vel
	if (structure.Etat_Commande == Vel){
		strcat(Message,"vel");
        			
		//Prepare de quoi reconnaitre la reponse
		strcpy(nom_commandes_M2, "vel");
	}
	
	//Restore
	if (structure.Etat_Commande == Restore){
		strcat(Message,"restore");
		
	}
	if(strcmp(Message, "") != 0){
		Send_string_M2(Message);
	}
}