#include <sys/stat.h>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "tankdef.h"
#include "tankdir.h"

#define MATCH 0

int isSensor(char* string) {
  char* s = "sensor";
  size_t len = strlen(s);
  return ( strncmp(s, string, len) == MATCH ) && (strlen(string) > len);
}

int isNotRealDir(struct dirent* file) {
  return strcmp(file->d_name,".") == MATCH ||
        strcmp(file->d_name,"..") == MATCH;
}

char* readFile(char* file, const int MAX_SIZE) {
  char* buffer = NULL;
  long length;
  FILE* f = fopen(file, "r");
  if(f == NULL) {
    perror(file);
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  length = ftell(f);
  if(length>MAX_SIZE) {
    fprintf(stderr, "File %s to large for variable.\n", file);
    exit(1);
  }
  fseek(f, 0, SEEK_SET);
  buffer = (char*)malloc(length+1); // +1 for null terminator.
  if(buffer == NULL) {
    perror(file);
    exit(1);
  }
  fread(buffer, sizeof(char), length, f);
  buffer[length] = (char)'\0';
  fclose(f);
  return buffer;
}

void
sensorReadError(char* file) {
  fprintf(stderr, "Malformed sensor file: %s\n", file);
  exit(1);
}

struct sensor
parseSensor(char* file) {
  struct sensor theSensor;
  char* content = readFile(file, 50);
  char* token = strtok(content, " ");
  if(token == NULL) {
    sensorReadError(file);
  }
  theSensor.angle = atof(token);
  token = strtok(NULL, " ");
  if(token == NULL) {
    sensorReadError(file);
  }
  theSensor.width = atof(token);
  token = strtok(NULL, " ");
  if(token == NULL) {
    sensorReadError(file);
  }
  theSensor.range = atoi(token);
  token = strtok(NULL, " ");
  if(token == NULL) {
    sensorReadError(file);
  }
  theSensor.turret = atoi(token);
  free(content);
  return theSensor;
}

void
loadFileContent(char* dest, char* file, const int MAX_SIZE) {
  char* content = readFile(file, MAX_SIZE);
  strcpy(dest, content);
  free(content);
}

void
initSensor(struct sensor* theSensor) {
  theSensor->turret = 0;
  theSensor->width = 0.0;
  theSensor->range = 0;
  theSensor->angle = 0.0;
  theSensor->triggered = 0;
}

void
initTank(struct tankdef* theTank) {
  int i;
  strcpy(theTank->name, "noname");
  strcpy(theTank->author, "noauthor");
  strcpy(theTank->color, "#808080");
  strcpy(theTank->program, "");
  for(i=0; i<TANK_MAX_SENSORS; i++) {
    initSensor(&theTank->sensors[i]);
  }
}

struct tankdef
readTankFromDir(char* path) {
  DIR* dp;
  if( (dp=opendir(path))==NULL ) {
    perror(path);
    exit(1);
  }

  struct tankdef theTank;
  initTank(&theTank);
  struct dirent* files;
  while( (files=readdir(dp)) != NULL ) {
    if( isNotRealDir(files) ) {
      continue;
    }
    char file[1000];
    sprintf(file, "%s/%s", path, files->d_name);
    if( isSensor(files->d_name) ) {
      int sensorId = files->d_name[strlen("sensor")]-'0';
      theTank.sensors[sensorId] = parseSensor(file);
    } else if( strcmp(files->d_name, "name") == MATCH ) {
      loadFileContent( theTank.name, file, max_size(NAME) );
    } else if( strcmp(files->d_name, "author") == MATCH ) {
      loadFileContent( theTank.author, file, max_size(AUTHOR) );
    } else if( strcmp(files->d_name, "program") == MATCH ) {
      loadFileContent( theTank.program, file, max_size(PROGRAM) );
    } else if( strcmp(files->d_name, "color") == MATCH ) {
      loadFileContent( theTank.color, file, max_size(COLOR) );
    }
  }
  closedir(dp);
  return theTank;
}

void
writeFile(char* path, char* fileName, char* text) {
  char file[100] = "";
  strcat(file, path);
  if(file[strlen(file)-1] != '/') {
    strcat(file, "/");
  }
  strcat(file, fileName);
  FILE* pfile = fopen(file, "w");
  if(pfile == NULL) {
    char errorString[100];
    sprintf(errorString, "Error opening file %s/%s\n", path, fileName);
    perror(errorString);
    exit(1);
  }
  fprintf(pfile, "%s", text);
  fclose(pfile);
}

void
writeSensorFile(char* path, int sensorIndex, struct sensor theSensor) {
  char file[100] = "";
  strcat(file, path);
  if(file[strlen(file)-1] != '/') {
    strcat(file, "/");
  }
  char fileName[] = "sensor0";
  fileName[6] = sensorIndex + '0';
  strcat(file, fileName);
  FILE* pfile = fopen(file, "w");
  fprintf(pfile, "%.0f %.0f %d %d", theSensor.angle, theSensor.width, theSensor.range, theSensor.turret);
  fclose(pfile);
}

void
writeTankToDir(char* dir, struct tankdef theTank) {
  int i = 0;
  writeFile(dir, "name", theTank.name);
  writeFile(dir, "color", theTank.color);
  writeFile(dir, "author", theTank.author);
  writeFile(dir, "program", theTank.program);
  for(i=0; i<TANK_MAX_SENSORS; i++) {
    writeSensorFile(dir, i, theTank.sensors[i]);
  }
}
