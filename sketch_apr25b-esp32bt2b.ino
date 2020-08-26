/*********
  esp32 gpio control

  ESP32 NodeMCU Developmentboard

*********/
#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "BluetoothSerial.h"
#include "OTA.h"

//unsigned long entry;// telnet use

//======================================================

const char* version = " version2 260820 esp32 bt2boiler ota tplink ";
//adc over mqtt with bluetooth printBT and ota update enabled. Output mV
//======================================================

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

char* ssid = WIFI_SSID;
char MQTTid[20];
const char* password = PASSWORD;
const char* mqtt_server = MQTT_IP ;
int error_RCerroorCount = 0;
int status[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char statusStr[11];
//adc

//int cal[10]    = { -8, -8, -8, -8};
int adcpin[10] = {34, 35, 32, 33};


int temperature[TEMP_ENTRIES];


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


int readGPIOstatus(void) ;

// LED Pin

const int pin[10] = { 2, 23, 22, 21, 19, 18, 5, 17, 16, 4}; //request to gpio convert
//pin                34,21,22,25,27,28,29,30,31,32
//const int pin[8] = {  16, 5, 4, 0, 2,14,12,13};    //request to gpio convert esp8266
//pin                 16,17,18,19,20,23,24,25
int gpioNumber = 10;
//========================================


//========================================
void setup() {
  int i;
  char buffer [20];
  Serial.begin(115200);
  Serial.println(version);
  i = random(1000);

  itoa (i, buffer, 10);
  strcpy(MQTTid, MQTT_ID);
  strcat(MQTTid, buffer);
  Serial.println(MQTTid);


  SerialBT.begin(BT_ID); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  if (SerialBT.available()) {
    Serial.println("bt available");
  }
  delay(10);
  SerialBT.println("Restart");

  setup_wifi();
  client.setServer(mqtt_server, MQTT_PORT);
  client.setCallback(callback);

  for (int i = 0; i < gpioNumber; i++) {
    pinMode( pin[i], OUTPUT);
  }
  delay(1000);
  client.publish(TOPIC_STATUS, version );
}//========================================


//========================================

void setup_wifi() {
  int setCount = 0;
  int error = 0;
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  ArduinoOTA.setHostname("GardenLights");
  setupOTA();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    SerialBT.print(".");
    setCount++;
    if (setCount > 20) {
      error = -1;
      break;
    }
  }
  if (error == 0)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("ESP32 IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    SerialBT.println("WiFi not connected");
    delay(5000);
  }
}
//========================================


//========================================
void callback(char* topic, byte* message, unsigned int length) {
  static int count2;
  Serial.print("   Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == TOPIC_CONTROL) {

    Serial.print("Changing output to ");
    Serial.println(messageTemp);
    //result = strcmp(str1, messageTemp);
    int n = messageTemp[7] - 49;
    int c = messageTemp[12];

    //Serial.println(messageTemp[12]);

    //Serial.print(n);
    //Serial.print(c);

    if (c == 78) {
      Serial.println("on");
      digitalWrite(pin[n], HIGH);
    }
    else if (c == 70) {
      Serial.println("off");
      digitalWrite(pin[n], LOW);
    }
    else if (c == 't') {
      Serial.println("status");
      SerialBT.print("status request  ");
      SerialBT.println(version);
      SerialBT.print("IP address: ");
      SerialBT.println(WiFi.localIP());
      SerialBT.print("SSID: ");
      SerialBT.println(ssid);//
      SerialBT.print("gpio state: ");
      SerialBT.println(statusStr);
      SerialBT.print("mqtt id :");
      SerialBT.println(MQTTid);
    }
    else {
      Serial.println("error");
      SerialBT.println("request error");
    }
    readGPIOstatus();
    char tempString[8];
    itoa(count2++, tempString, 10);
    client.publish(TOPIC_STATUS_CNT2 , tempString);
    client.publish(TOPIC_STATUS_VER, version );
    client.publish(TOPIC_STATUS, statusStr);

  }
}
//========================================


//========================================
void reconnect() {
  static int countR = 0;
  int retry = 0;
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTTid)) {     //"ESP32client2")) {
      Serial.println("connected");
      // Subscribe
      delay(1000);

      client.subscribe(TOPIC_CONTROL);
      client.publish(TOPIC_STATUS, version );
      SerialBT.println(version);
      SerialBT.print("reconnected, " );
      SerialBT.println(countR);
    } else {
      //Serial.print(" failed, rc=");
      //SerialBT.print(" failed, rc=");
      //Serial.print(client.state());
      //SerialBT.print(client.state());
      // Wait 5 seconds before retrying

      if (error_RCerroorCount++ > 100) {
        SerialBT.print("mqtt id :");
        SerialBT.println(MQTTid);
        //Serial.print(" failed, rc=");
        SerialBT.print(" failed, rc=");
        //Serial.print(client.state());
        SerialBT.print(client.state());
        SerialBT.print("RC reboot ");
        ESP.restart();
      }
      delay(5000);
      retry++;
      if (retry > 10) {
        break;
      }
    }
  }
}
//========================================


//========================================

int readTemp(void) {
  char data[100];
  char buffer [20];
  //Serial.println("========================");
  //Serial.println(ReadVoltage(33),3);
  SerialBT.println("analogRead ");

  SerialBT.println(analogRead(adcpin[0]));
  SerialBT.println(analogRead(adcpin[1]));
  SerialBT.println(analogRead(adcpin[2]));
  int temp = 0;
  strcpy(data, "{");
  for (int count = 0; count < TEMP_ENTRIES; count++) {
    //temp=((analogRead(adcpin[count])-400)/10)+cal[count]; // convert to temp
    temp =  analogRead(adcpin[count]);
    //Serial.println(temp);

    temperature[count] = temp;

    strcat(data, "\"temp");
    itoa (count, buffer, 10);
    //strcpy(data,buffer);
    strcat(data, buffer);
    strcat(data, "\":");
    itoa (temp, buffer, 10);
    strcat(data, buffer);
    if (count < TEMP_ENTRIES - 1)strcat(data, ",");

  }
  strcat(data, "}");

  //Serial.println(data);
  SerialBT.print(data);
  client.publish(TOPIC_TEMPERATURES, data);

  //dvm reads 714 adc reads 644 temp is 22degC 22 deg should be 400 + 220 = 620mv.
  //is dvm wrong?
  //Serial.println("========================");
}
//========================================

//========================================

int readGPIOstatus() {
  static int count = 0;
  count++;
  for (int i = 0; i < gpioNumber; i++) {
    status[i] = digitalRead(pin[i]);
    //Serial.print(status[i]);
    statusStr[i] = status[i] + 48;
  }
  statusStr[gpioNumber] = 0;
  Serial.println(statusStr);

  return (count);
}
//========================================


//========================================
void loop() {

  int wifirestart = 0;
  int status2;

  status2 = WiFi.status();
  if (status2 == WL_DISCONNECTED || status2 == WL_CONNECTION_LOST) {
    while ( status2 != WL_CONNECTED) {
      SerialBT.println("WIFI not connected");
      status2 = WiFi.begin(ssid, password);
      Serial.print("WIFI failed");
      SerialBT.print("WIFI failed");
      WiFi.printDiag(SerialBT);
      SerialBT.print("mqtt id :");
      SerialBT.println(MQTTid);
      wifirestart++;
      delay(6000);
      if (wifirestart > 10) {
        wifirestart = 0;
        ESP.restart();
      }
    }
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //OTA===========================
  //entry=micros();
  ArduinoOTA.handle();
  //TelnetStream.println(micros()-entry);
  //TelnetStream.println("Loop");

  delay(100);
  //==============================

  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;


    int count = readGPIOstatus();
    char tempString[8];
    itoa(count, tempString, 10);

    client.publish(TOPIC_STATUS_CNT, tempString);
    client.publish(TOPIC_STATUS, statusStr);
    Serial.println(count);
    SerialBT.println(count);
  }
}
//========================================


//========================================
