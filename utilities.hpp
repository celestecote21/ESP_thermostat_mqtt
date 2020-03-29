#ifndef UTILITIES_HEADER
#define UTILITIES_HEADER

// #define SERIAL_DEBUG

#include "Arduino.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <HTTPUpdate.h>

void init_wifi();
void callback(char* topic, byte* data, unsigned int length);
void reconnection();
void parse_ordre(String str);
void doUpdate(String link);

typedef struct ordre{
	int start_hour;
	int finish_hour;
	float temp_set;
	int day;
}ordre_t;

typedef struct etat{
	ordre_t last_ordre;
	boolean isActif : 1;
	boolean isArret : 1;

}etat_t;

#endif
