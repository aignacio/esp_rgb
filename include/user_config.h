#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#define CFG_HOLDER	0x00FE2AA3	/* Change this value to load default configurations */
#define CFG_LOCATION	0x3C	/* Please don't change or if you know what you doing */
#define CLIENT_SSL_ENABLE

/*DEFAULT CONFIGURATIONS*/
#define TEST_HOME

#ifdef TEST_HOME
	#define MQTT_HOST			"10.10.0.50" //or "mqtt.yourdomain.com"
#else
	#define MQTT_HOST			"10.10.0.50" //or "mqtt.yourdomain.com"
#endif
#define MQTT_PORT			1883
#define MQTT_BUF_SIZE		1024
#define MQTT_KEEPALIVE		120	 /*second*/

#define MQTT_CLIENT_ID		"Gate_%08X"
#define MQTT_USER			"DVES_USER"
#define MQTT_PASS			"DVES_PASS"

#ifdef  TEST_HOME
	#define STA_SSID "Wi-fi Anderson"
	#define STA_PASS "wifi3102230"
#else
	#define STA_SSID "LightDNA"
	#define STA_PASS "lightdnaAWGEShda"
	//#define STA_PASS "lightdna"
#endif
#define STA_TYPE AUTH_WPA2_PSK

#define MQTT_RECONNECT_TIMEOUT 	5	/*second*/

#define DEFAULT_SECURITY	0
#define QUEUE_BUFFER_SIZE		 		2048

#define PROTOCOL_NAMEv31	/*MQTT version 3.1 compatible with Mosquitto v0.15*/
//PROTOCOL_NAMEv311			/*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/
#endif
