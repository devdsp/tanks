#ifndef __TANKDIR_H__
#define __TANKDIR_H__

#include "tankdef.h"

struct tankdef
readTankFromDir(char* path);

void
writeTankToDir(char* path, struct tankdef);

#endif
