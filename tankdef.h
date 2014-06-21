#ifndef __TANKDEF_H__
#define __TANKDEF_H__

#include "ctanks.h"

#define MAX_PROGRAM_SIZE 10000
#define MAX_NAME_SIZE 100
#define MAX_AUTHOR_SIZE 200
#define MAX_COLOR_SIZE 7

#define max_size(p) MAX_ ## p ## _SIZE

struct tankdef {
  struct sensor  sensors[TANK_MAX_SENSORS]; /* Sensor array */
  char program[MAX_PROGRAM_SIZE];
  char name[MAX_NAME_SIZE];
  char author[MAX_AUTHOR_SIZE];
  char color[MAX_COLOR_SIZE];
};

void
print_tank(struct tankdef);

void
print_sensor(struct sensor);

#endif /* __TANKDEF_H__ */
