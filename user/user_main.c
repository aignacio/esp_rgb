/**************************************************************
#
#	Project: LightDNA - Device code of ESP8266
#	Author: Anderson Ignacio da Silva
#   Date: 20/10/15
#	Target: ESP-8266
#	Version: 1.2.1 - Version to test RSSI range with routers
#	Inst.: to open the gate just pub the msg:"W0001-1" in topic:"homestark/gate/%08X/status" or topic:"homestark/gate"
**************************************************************/
#define DEBUG_OLED

#include "ets_sys.h"
#include "driver/uart.h"
#include "osapi.h"
#include "mqtt.h"
#include "wifi.h"
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"
// #include "driver/gpio16.h"
#ifdef	DEBUG_OLED
	#include "driver/i2c.h"
	#include "driver/i2c_oled.h"
	#define LED(x)	GPIO_OUTPUT_SET(GPIO_ID_PIN(12), x)
#endif

#define FW_VERSION_LIGHTDNA "1.2.1-20/10/2015"
#define LED_DEBUG
#define UART_PEN

#define TOPIC_DEFAULT	"homestark/%08X"
#define TOPIC_DIMMER	"homestark/%08X/dimmer"
#define TOPIC_MSP		"homestark/%08X/msp"
#define TOPIC_STATUS	"homestark/gate/%08X/status"
#define TOPIC_CURRENT	"lights/%08X/current"
#define TOPIC_PIR		"homestark/%08X/pir"
#define TOPIC_LDR		"homestark/%08X/ldr"
#define TOPIC_RSSI		"homestark/%08X/rssi"
#define TOPIC_TEMP		"homestark/%08X/temperature"
#define MESSAGE_OFFLINE	"Device %08X it's offline!"

#define PERIPHS_IO_MUX_PULLDWN          BIT6
#define PERIPHS_IO_MUX_SLEEP_PULLDWN    BIT2
#define PIN_PULLDWN_DIS(PIN_NAME)             CLEAR_PERI_REG_MASK(PIN_NAME, PERIPHS_IO_MUX_PULLDWN)
#define PIN_PULLDWN_EN(PIN_NAME)              SET_PERI_REG_MASK(PIN_NAME, PERIPHS_IO_MUX_PULLDWN)


#define RELE(x)		GPIO_OUTPUT_SET(GPIO_ID_PIN(5), x)

MQTT_Client mqttClient;

bool	OLED, CleanOLED = false,status_b = false,blinkS = false;

static ETSTimer StatusTimer;

typedef enum states
{
  FIRST_IDT,
  DATA_B,
  COMPLETE
}states_buffer;

states_buffer serial_s = FIRST_IDT;

uint8_t topic_dimmer[64],
		topic_current[64],
		topic_status[64],
		topic_pir[64],
		topic_temp[64],
		topic_rssi[64],
		topic_ldr[64],
		topic_msp[64],
		topic_default[64],
		MessageDead[64],
		DeviceID[8],
		InitMessage[100],
		StatusMessage[100],
		StatusRSSI[10];

uint8_t i = 0,
		buffer_serial[50],
		dummy = 0;

uint8_t mac[6],
		temp[64];

bool blink,flag=false;

void PrepareTopics(){
	os_sprintf(topic_default, TOPIC_DEFAULT, system_get_chip_id());
	os_sprintf(topic_dimmer, TOPIC_DIMMER, system_get_chip_id());
	os_sprintf(topic_current, TOPIC_CURRENT, system_get_chip_id());
	os_sprintf(topic_status, TOPIC_STATUS, system_get_chip_id());
	os_sprintf(topic_temp, TOPIC_TEMP, system_get_chip_id());
	os_sprintf(topic_pir, TOPIC_PIR, system_get_chip_id());
	os_sprintf(topic_ldr, TOPIC_LDR, system_get_chip_id());
	os_sprintf(topic_rssi, TOPIC_RSSI, system_get_chip_id());
	os_sprintf(topic_msp, TOPIC_MSP, system_get_chip_id());
	os_sprintf(MessageDead, MESSAGE_OFFLINE, system_get_chip_id());
	os_sprintf(DeviceID, "%08X", system_get_chip_id());
}

void wifiConnectCb(uint8_t status)
{
	if(status == STATION_GOT_IP){
		MQTT_Connect(&mqttClient);
	} else {
		MQTT_Disconnect(&mqttClient);
	}
}

void mqttConnectedCb(uint32_t *args)
{
	MQTT_Client* client = (MQTT_Client*)args;

	INFO("MQTT: Connected\r\n");
	// os_sprintf(InitMessage, "Gate connected!", system_get_chip_id(),mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	MQTT_Publish(&mqttClient, topic_status, "Gate connected", sizeof("Gate connected"), 0, 0);
	MQTT_Subscribe(client, topic_status, 0);
	MQTT_Subscribe(client, "homestark/gate", 0);
}

void mqttDisconnectedCb(uint32_t *args)
{
	MQTT_Client* client = (MQTT_Client*)args;
	INFO("MQTT: Disconnected\r\n");
}

void mqttPublishedCb(uint32_t *args)
{
	MQTT_Client* client = (MQTT_Client*)args;
	INFO("MQTT: Published\r\n");
}

void infoRSSI(){
	sint8 rssiSignal = wifi_station_get_rssi();
	os_sprintf(StatusRSSI, "%d", rssiSignal);
   	MQTT_Publish(&mqttClient, topic_rssi, StatusRSSI, sizeof(StatusRSSI), 0, 0);
   	#ifdef DEBUG_OLED
   		OLED_Print(0,4,"RSSI:", 1);
   		OLED_Print(0,6,StatusRSSI, 1);
   	#endif
}

void ShowID()
{
	os_sprintf(StatusMessage, "OK - %s - DEV: %08X MAC: %02X-%02X-%02X-%02X-%02X-%02X", FW_VERSION_LIGHTDNA, system_get_chip_id(),mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
   	MQTT_Publish(&mqttClient, topic_status, StatusMessage, sizeof(StatusMessage), 0, 0);
}

void openGate(){
	RELE(1);
	os_delay_us(300000);
	RELE(0);
}

void notifyGate(){
	uint32_t test = 0;
	test = GPIO_INPUT_GET(GPIO_ID_PIN(13));
	if(test)
		MQTT_Publish(&mqttClient, topic_status, "Gate is open", sizeof("Gate is open"), 0, 1);
	else
		MQTT_Publish(&mqttClient, topic_status, "Gate is closed", sizeof("Gate is closed"), 0, 1);
}


void mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
	char *topicBuf = (char*)os_zalloc(topic_len+1),
			*dataBuf = (char*)os_zalloc(data_len+1);
	MQTT_Client* client = (MQTT_Client*)args;
	os_memcpy(topicBuf, topic, topic_len);
	topicBuf[topic_len] = 0;
	os_memcpy(dataBuf, data, data_len);
	dataBuf[data_len] = 0;

	if(dataBuf[0]=='R'&&dataBuf[1]=='0'&&dataBuf[2]=='2'&&dataBuf[3]=='5'&&dataBuf[4]=='5')
		ShowID();
	else if(dataBuf[0]=='R'&&dataBuf[1]=='0'&&dataBuf[2]=='2'&&dataBuf[3]=='5'&&dataBuf[4]=='4')
		infoRSSI();
	else if(dataBuf[0]=='R'&&dataBuf[1]=='0'&&dataBuf[2]=='0'&&dataBuf[3]=='0'&&dataBuf[4]=='1')
		notifyGate();
	else if(dataBuf[0]=='W'&&dataBuf[1]=='0'&&dataBuf[2]=='0'&&dataBuf[3]=='0'&&dataBuf[4]=='1'&&dataBuf[5]=='-'&&dataBuf[6]=='1')
		openGate();
	os_free(topicBuf);
	os_free(dataBuf);
}

void BlinkStatusCB(){
	if(blink){
		GPIO_OUTPUT_SET(GPIO_ID_PIN(13), 0);
		blink = false;
	}
	else{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(13), 1);
		blink = true;
	}
  	os_timer_disarm(&StatusTimer);
	os_timer_setfn(&StatusTimer, BlinkStatusCB, NULL);
	os_timer_arm(&StatusTimer, 500, 0);
}

void SendMQTT()
{
	i = 0;
	switch(buffer_serial[0]) {
		case 'F':
		buffer_serial[0] = ' ';
			MQTT_Publish(&mqttClient, topic_msp, buffer_serial, sizeof(buffer_serial), 0, 0);
		break;
		case 'D':
		buffer_serial[0] = ' ';
			MQTT_Publish(&mqttClient, topic_dimmer, buffer_serial, sizeof(buffer_serial), 0, 0);
		break;
		case 'C':
			buffer_serial[0] = ' ';
			MQTT_Publish(&mqttClient, topic_current, buffer_serial, sizeof(buffer_serial), 0, 0);
		break;
		case 'P':
			buffer_serial[0] = ' ';
			MQTT_Publish(&mqttClient, topic_pir, buffer_serial, sizeof(buffer_serial), 0, 0);
		break;
		case 'T':
			buffer_serial[0] = ' ';
			MQTT_Publish(&mqttClient, topic_temp, buffer_serial, sizeof(buffer_serial), 0, 0);
		break;
		case 'L':
			buffer_serial[0] = ' ';
			MQTT_Publish(&mqttClient, topic_ldr, buffer_serial, sizeof(buffer_serial), 0, 0);
		break;
	}
  serial_s = FIRST_IDT;
}

void CbRXSerial(uint8 CharBuffer)
{
	//os_printf(CharBuffer);
	switch(serial_s)
	{
	case FIRST_IDT:
	  	if(CharBuffer == '#')
	  	{
	    	serial_s = DATA_B;
	    	for(i=0;i<40;i++)
	    		buffer_serial[i] = '\0';
	    	i = 0;
		}
	break;
	case DATA_B:
	    if(CharBuffer != '#' && i < 40)
	    {
	      buffer_serial[i] = CharBuffer;
	      i++;
	    }
	    else
	    {
	      	serial_s = COMPLETE;
	    }
	break;
	}

	if(serial_s == COMPLETE)    SendMQTT();
}

void interruptHandler(){
	uint32 gpio_status;
	bool status_reset_button;
	gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);

	if(gpio_status & BIT(13))
	{
		//clear interrupt status
		gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_DISABLE);
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(13));
		gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_POSEDGE);
		//ResetToAP();
		if(status_b == true)
		{
			notifyGate();
		}
		else
		{
			notifyGate();
		}
		//if(status_reset_button)
			//INFO("\nz\rPUSH BUTTON PRESSED");
		//else
		//	INFO("\n\rPUSH BUTTON NOT PRESSED");


	}
}

void user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	os_delay_us(1000000);

	PrepareTopics();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,  FUNC_GPIO5); //"Connected to the botoeira"

	//Setting interrupt in pin GPIO13 - Button Reset AP
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	PIN_PULLDWN_EN(PERIPHS_IO_MUX_MTCK_U);

	ETS_GPIO_INTR_ATTACH(interruptHandler, NULL);
	ETS_GPIO_INTR_DISABLE();
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(13));
	gpio_register_set(GPIO_ID_PIN(13), GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
	              | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
	              | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(13));
	gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ENABLE();

	CFG_Load();

	MQTT_InitConnection(&mqttClient, sysCfg.mqtt_host, sysCfg.mqtt_port, sysCfg.security);
	//MQTT_InitConnection(&mqttClient, "192.168.11.122", 1880, 0);

	MQTT_InitClient(&mqttClient, sysCfg.device_id, sysCfg.mqtt_user, sysCfg.mqtt_pass, sysCfg.mqtt_keepalive, 1);
	//MQTT_InitClient(&mqttClient, "client_id", "user", "pass", 120, 1);

	MQTT_InitLWT(&mqttClient, topic_status , MessageDead, 0, 0);   //Last Will Teastment - Indicate that device has been dead, /lights/device_addres/status(Topic) Device device_address it's offline!(message)
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);
	MQTT_OnData(&mqttClient, mqttDataCb);

	WIFI_Connect(sysCfg.sta_ssid, sysCfg.sta_pwd, wifiConnectCb);

	os_printf("\r\nGate Device:\r\n");
	os_printf("\r\nSystem started");
	os_printf("\r\nDEVICE ID:");
	os_printf(DeviceID);
	os_printf("\r\nMAC ADDRESS:");
    wifi_get_macaddr(SOFTAP_IF, mac);
    os_sprintf(temp, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	os_printf(temp);
	os_printf("\r\n");
}
