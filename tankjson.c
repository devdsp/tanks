#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "tankdef.h"
#include "tankjson.h"

json_t*
tankToJSON(struct tankdef theTank);

char*
writeTankToJSON(struct tankdef theTank) {
  json_t* root = tankToJSON(theTank);
  char* ret_string = json_dumps(root,0);
  json_decref(root);

  return ret_string;
}

json_t*
tankToJSON(struct tankdef theTank) {
  json_t* root = json_object();
  json_t* sensors = json_array();
  int i;
  for(i=0; i<TANK_MAX_SENSORS; i++) {
    json_t* sensor = json_object();
    json_object_set( sensor, "angle", json_real(theTank.sensors[i].angle) );
    json_object_set( sensor, "width", json_real(theTank.sensors[i].width) );
    json_object_set( sensor, "range", json_integer(theTank.sensors[i].range) );
    json_object_set( sensor, "turret", json_integer(theTank.sensors[i].turret) );
    json_array_append(sensors, sensor);
    json_decref(sensor);
  }
  json_object_set( root, "sensors", sensors );
  json_decref(sensors);
  json_object_set( root, "name", json_string(theTank.name) );
  json_object_set( root, "author", json_string(theTank.author) );
  json_object_set( root, "program", json_string(theTank.program) );
  json_object_set( root, "color", json_string(theTank.color) );
  return root;
}

void
loadJSONString(char* dest, const int MAX_LEN, const json_t* object, const char* name) {
  const json_t* tmp = json_object_get(object, name);
  const char* string = json_string_value(tmp);
  if( (string == NULL) || (strlen(string) > (size_t)MAX_LEN) ) {
    fprintf(stderr, "%s field too large in JSON object.\n", name);
    exit(1);
  } else {
    strcpy(dest, string);
  }
}

struct sensor
sensorFromJSON(json_t* sensor) {
  struct sensor theSensor;
  json_t* tmp = json_object_get(sensor, "angle");
  theSensor.angle = json_real_value(tmp);
  tmp = json_object_get(sensor, "width");
  theSensor.width = json_real_value(tmp);
  tmp = json_object_get(sensor, "range");
  theSensor.range = json_integer_value(tmp);
  tmp = json_object_get(sensor, "turret");
  theSensor.turret = json_integer_value(tmp);
  return theSensor;
}

struct tankdef
tankFromJSON(json_t* root) {
  size_t i;
  if( !json_is_object(root) ) {
    fprintf(stderr, "Error: root is not an object!\n");
    json_decref(root);
    exit(1);
  }
  
  struct tankdef theTank;
  loadJSONString(theTank.name, max_size(NAME), root, "name");
  loadJSONString(theTank.author, max_size(AUTHOR), root, "author");
  loadJSONString(theTank.program, max_size(PROGRAM), root, "program");
  loadJSONString(theTank.color, max_size(COLOR), root, "color");

  json_t* sensors = json_object_get(root, "sensors");
  if(!json_is_array(sensors)) {
    fprintf(stderr, "Error: sensors object is not an array!\n");
    exit(1);
  }
  for(i=0; i<json_array_size(sensors); i++) {
    json_t* sensor = json_array_get(sensors, i);
    theTank.sensors[i] = sensorFromJSON(sensor);
  }
  return theTank;
}

struct tankdef
readTankFromJSON(char* jsonString) {
  json_t* root;
  json_error_t error;
  root = json_loads(jsonString, 0, &error);
  if( root == NULL ) {
    fprintf(stderr, "Error: text is invalid JSON.\n");
    fprintf(stderr, "Error: on line %d: %s\n", error.line, error.text);
    exit(1);
  }
  struct tankdef theTank = tankFromJSON(root);
  json_decref(root);
  return theTank;
}

int
jsonArraySize(char* jsonString) {
  json_t* root;
  json_error_t error;
  root = json_loads(jsonString, 0, &error);
  if( root == NULL ) {
    fprintf(stderr, "Error: text is invalid JSON.\n");
    fprintf(stderr, "Error: on line %d: %s\n", error.line, error.text);
    exit(1);
  }
  if( !json_is_array(root) ) {
    fprintf(stderr, "Error: root is not an array!\n");
    json_decref(root);
    exit(1);
  }
  return json_array_size(root);
}

void
readTanksFromJSON(struct tankdef mytankdefs[],
                  const int MAX_TANKS,
                  char* jsonString) {
  json_t* root;
  json_error_t error;
  size_t i;
  root = json_loads(jsonString, 0, &error);
  if( root == NULL ) {
    fprintf(stderr, "Error: text is invalid JSON.\n");
    fprintf(stderr, "Error: on line %d: %s\n", error.line, error.text);
    exit(1);
  }
  if( !json_is_array(root) ) {
    fprintf(stderr, "Error: root is not an array!\n");
    json_decref(root);
    exit(1);
  }
  if(json_array_size(root) > (size_t)MAX_TANKS) {
    fprintf(stderr,"Error: Too many tanks! Tried to load %d, Max: %d\n",
               (int)json_array_size(root), MAX_TANKS);
  }
  for(i=0; i<json_array_size(root); i++) {
    json_t* tank = json_array_get(root, i);
    mytankdefs[i] = tankFromJSON(tank);
  }
}
