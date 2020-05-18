
//wifi
#define WIFI_SSID "VM-2G"
#define PASSWORD "1234567890"
//mqtt server
#define MQTT_SERVER "kelvinweb.co.uk"
#define MQTT_IP "192.168.2.90"
#define MQTT_PORT 1883      //61613 //1883
//#define TOPIC = "kelvinweb/"

#define MQTT_USERNAME "admin"
#define MQTT_PASSWORD  "1944"
#define TEMP_ENTRIES 4


#define BT_ID "ESP32bt-lights"
  #define MQTT_ID   "ESP32garden"
  #define TOPIC_CONTROL "kelvinweb/garden/lights/control"
  #define TOPIC_STATUS  "kelvinweb/garden/lights/status"
  #define TOPIC_STATUS_CNT  "kelvinweb/garden/lights/status/count"
  #define TOPIC_STATUS_CNT2  "kelvinweb/garden/lights/status/count2"
  #define TOPIC_STATUS_VER  "kelvinweb/garden/lights/status/ver"
  #define TOPIC_TEMPERATURES "kelvinweb/garden/lights/temperature"
/*
#define MQTT_ID   "ESP32boiler"
#define BT_ID "ESP32bt-boiler"

#define TOPIC_B_CONTROL    "kelvinweb/boiler/control"
#define TOPIC_TEMPERATURES "kelvinweb/boiler/temperature"
#define TOPIC_STATUS       "kelvinweb/boiler/status"
#define TOPIC_STATUS_CNT   "kelvinweb/boiler/status/count"
#define TOPIC_STATUS_CNT2  "kelvinweb/boiler/status/count2"
#define TOPIC_STATUS_VER   "kelvinweb/boiler/status/ver"

*/

#define LOOP_TIME  6000
