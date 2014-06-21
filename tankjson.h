#ifndef __TANKJSON_H__
#define __TANKJSON_H__

#include "tankdef.h"

char* writeTankToJSON(struct tankdef);
struct tankdef readTankFromJSON(char*);
int jsonArraySize(char*);
void readTanksFromJSON(struct tankdef tanksToCreate[], const int, char*);

#endif
