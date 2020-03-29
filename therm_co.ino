#include <WiFi.h>
#include <HTTPClient.h>
#include <DHTesp.h>

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Fujitsu.h>

#include "define.hpp"
#include "utilities.hpp"
#include "Clim.hpp"
#include "Geo.hpp"


// #define DEBUG

const uint8_t pinRelaie = 5;
const uint8_t pinDht = 4;
const uint8_t pinIr = 17;
unsigned long long time_before = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);
DHTesp dhtSensor;
IRFujitsuAC ac(pinIr);
Clim clim(&ac);
Geo geo(pinRelaie);
WiFiClient wifi_client_mqtt;
PubSubClient mqtt_client(MQTT_SERVER, 1883, callback, wifi_client_mqtt);


void setup() {
	pinMode(pinRelaie, OUTPUT);

	Serial.begin(115200);
	for(uint8_t t = 4; t > 0; t--) {
		Serial.printf("[SETUP] WAIT %d...\n", t);
		Serial.flush();
		delay(500);
	}

	init_wifi();

	timeClient.begin();
	delay(100);
	timeClient.update();
	dhtSensor.setup(pinDht, DHTesp::DHT11);

	/*
	* initialisatitonn du client mqtt avec les subscriptions
	*/
	if(mqtt_client.connect("thermostatClient", MQTT_USER, MQTT_PASSWORD)){
		mqtt_client.publish("Celeste", "hello World thermo 0.1.2");
		mqtt_client.subscribe(TOPIC_SET);
		mqtt_client.subscribe(TOPIC_UPDATE);
		mqtt_client.subscribe(TOPIC_STATUS);
	}


}

void loop() {

	mqtt_client.loop();

	if(millis() - time_before > TIMETODO){
		time_before = millis();
		float temp_actuel = dhtSensor.getTemperature();
#ifdef DEBUG
temp_actuel = 20.0;
#endif
		int day = timeClient.getDay();
		day -= 1;
		if(day == -1){
			day = 6;
		}
		reconnection();
		mqtt_client.publish(TOPIC_GET, String(temp_actuel).c_str(), true);
		// mqtt_client.publish(TOPIC_STATUS, get_status().c_str(), true);

		int heure = (int)timeClient.getHours();
		clim.check(temp_actuel, heure, day);
		geo.check(temp_actuel, heure, day);

	}   

}
