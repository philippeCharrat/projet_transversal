#ifndef _F0_M2_
#define _F0_M2_
#include "FO_M2__Structures_COMMANDES_INFORMATIONS_Serializer.h"

void Send_char_M2(char);
void Send_string_M2(char*);
char extraction(char*);
char extraction_arguments(void);
struct INFORMATIONS_SERIALIZER recuperation_structure_serializer(struct INFORMATIONS_SERIALIZER);
struct INFORMATIONS_SERIALIZER F0_M2_fct_decodage(struct INFORMATIONS_SERIALIZER);
void F0_M2_fct_encodage(struct COMMANDES_SERIALIZER);

#endif