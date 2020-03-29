#include "utilities.hpp"


void init_wifi(){
  WiFi.begin(SSIDWIFI, PASSWORD);

  int nb_setup = 0;

  while (WiFi.status() != WL_CONNECTED) {
    nb_setup ++;
    delay(1000);
    Serial.println("Connecting to WiFi..");
    if(nb_setup > 5){
      ESP.restart();  
    }
  }
}

String get_status(){
	etat_t etat_clim = clim.get_status();
	etat_t etat_geo = geo.get_status();
	StaticJsonDocument<500> json;
	if(etat_clim.isArret == true){
		json["clim_arret"] = 1;
	}else{
		json["clim_arret"] = 0;
		json["clim_start"] = etat_clim.last_ordre.start_hour;
		json["clim_stop"] = etat_clim.last_ordre.finish_hour;
		json["clim_set"] = etat_clim.last_ordre.temp_set;
		json["clim_day"] = etat_clim.last_ordre.day; 
		json["clim_actif"] = etat_clim.isActif;
	}

	if(etat_geo.isArret == true){
		json["geo_arret"] = 1;
	}else{
		json["geo_arret"] = 0;
		json["geo_start"] = etat_geo.last_ordre.start_hour;
		json["geo_stop"] = etat_geo.last_ordre.finish_hour;
		json["geo_set"] = etat_geo.last_ordre.temp_set;
		json["geo_day"] = etat_geo.last_ordre.day;
		json["geo_actif"] = etat_geo.isActif;
	}
	String result;
	serializeJson(json, result);
	return result;
}


void callback(char* topic, byte* data, unsigned int length){
	String ordre_str;
	int i = 0;
	for(i = 0; i < length; i++){
		ordre_str += char(data[i]);
	}
#ifdef SERIAL_DEBUG
Serial.printf("topic: %s, message: %s \n", topic, ordre_str.c_str());
#endif

	if(String(topic) == TOPIC_UPDATE){
		doUpdate(ordre_str);
	}else if(String(topic) == TOPIC_SET){
		parse_ordre(ordre_str);
	}else if(String(topic) == TOPIC_STATUS){
		if(i < 5){ // la chose recu est toute petite donc n'est pas un message envoyer par l'esp
			mqtt_client.publish(TOPIC_STATUS, get_status().c_str());
		}
		return;
	}else{
		return;
	}
}
	

void parse_ordre(String str){
	if(str.indexOf("arret") >= 0){
		geo.arret();
		clim.arret();
		return;
	}

	StaticJsonDocument<500> json;
	deserializeJson(json, str.c_str());
	ordre_t new_ordre;
	new_ordre.start_hour = json["start"].as<int>();
	new_ordre.finish_hour = json["finish"].as<int>();
	new_ordre.temp_set = json["set"].as<float>();
	new_ordre.day = json["day"].as<int>();
	int type = json["type"].as<int>();
	float temp_default_geo = json["default"] | 12;
#ifdef SERIAL_DEBUG
	Serial.printf("temperature a mettre: %f", new_ordre.temp_set);
#endif
	if(type == 1){
		geo.set_geo(new_ordre, temp_default_geo);
	}else if(type == 2){
		clim.set_clim(new_ordre);
	}
}


void doUpdate(String link){
  
	Serial.println("Start update");
	mqtt_client.publish(TOPIC_DEBUG, "mise a jour");
	WiFiClient wifi_client_update;  
	t_httpUpdate_return ret = httpUpdate.update(wifi_client_update, link.c_str());
	switch (ret) {
		case HTTP_UPDATE_FAILED:
			Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
			ESP.restart();
		break;

		case HTTP_UPDATE_NO_UPDATES:
			Serial.println("HTTP_UPDATE_NO_UPDATES");
			ESP.restart();
		break;

		case HTTP_UPDATE_OK:
			Serial.println("HTTP_UPDATE_OK");
		break;
	}
}	

void reconnection(){

	if(WiFi.status() == WL_CONNECTED && mqtt_client.connected())
		return;

  int nb_reconnection = 0;
  if(WiFi.status() != WL_CONNECTED){
    WiFi.reconnect();
    // loop pour un temps de reconnection
    for(int i = 0; i < 5 || WiFi.status() != WL_CONNECTED; i++){
      #ifdef SERIAL_DEBUG
      Serial.printf("reconnection wifi attempt %d", i);
      #endif
      delay(SECONDE);  
    }
  }
  
  if(WiFi.status() != WL_CONNECTED){
    ESP.restart();
  }else if(!mqtt_client.connected()){
    while(!mqtt_client.connected()){
      nb_reconnection ++;
#ifdef SERIAL_DEBUG
      Serial.printf("reconnection mqtt attempt %d", nb_reconnection);
#endif
      if(mqtt_client.connect("thermostatClientReconnect", MQTT_USER, MQTT_PASSWORD)){ // la reconnection marche
        mqtt_client.publish("Celeste", "hello world reconnection thermo", true); // publication d'un message de reconnection
        mqtt_client.subscribe(TOPIC_SET); // subscription au autre topic
        mqtt_client.subscribe(TOPIC_UPDATE); //...
	mqtt_client.subscribe(TOPIC_STATUS);
      }else{
#ifdef SERIAL_DEBUG
      Serial.printf("ERREUR:reconnection mqtt attempt %d", nb_reconnection); // erreur dans la reconnection
#endif
        delay(SECONDE); //attente d'une seconde pour laisser un peu de temps
      }
      if(nb_reconnection > 5){
        ESP.restart();  
      }
    }
  }

     
	if(WiFi.status() == WL_CONNECTED && mqtt_client.connected()){
		return;
	}else{
		ESP.restart();
	}
}
