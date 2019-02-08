

/****************************************************************************************************
*                                                                                                   *
*****************************************************************************************************/

#include "AllIncludes.h"

// globale Variablen ==============================================================================//
volatile boolean pending_reboot = false;
volatile boolean pending_LED_Action = true;
volatile uint8_t LED_Action_Type = 3;
volatile uint8_t RGBtoSet[3];
volatile uint32_t lastRun=millis();
volatile uint16_t RGBdelay=50;

// Konfiguration für Flash
struct T_ConfigStruct GlobalConfig;

void ConfigSave( struct T_ConfigStruct* conf) {
   EEPROM.begin(512);
   uint8_t* confAsArray = (uint8_t *) conf;
   for (uint16_t i=0; i<sizeof(T_ConfigStruct); i++) {
     EEPROM.write(i,confAsArray[i]);
   }
   EEPROM.commit();
   EEPROM.end();
}

void ConfigLoad( struct T_ConfigStruct* conf) {
   EEPROM.begin(512);
   uint8_t* confAsArray = (uint8_t*) conf;
   for (uint16_t i=0; i<sizeof(T_ConfigStruct); i++) {
     confAsArray[i]=EEPROM.read(i);
   }
   EEPROM.commit();
   EEPROM.end();
}

void LoadAndCheckConfiguration( void) {
  struct T_ConfigStruct newConf;
  ConfigLoad(&newConf);
  if (strncmp(newConf.magic, GlobalConfig.magic, sizeof(newConf.magic)) == 0){
    Serial.println("Magic string from configuration is ok, overload configuration from eeprom");
    ConfigLoad(&GlobalConfig);
  } else {
    Serial.println("Magic string mismatch, keeping with default configuration");
  }
}

char myIPaddress[18]="000.000.000.000  ";
IPAddress myIP = 0;
uint16_t myLowerIP, myHigherIP = 0;

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, 14, NEO_GRB + NEO_KHZ800);  //D5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, 12, NEO_GRB + NEO_KHZ800);



void setup(){
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  LoadAndCheckConfiguration();
  WiFi.hostname(GlobalConfig.myHostname);
  ETS_UART_INTR_DISABLE();
  wifi_station_disconnect();
  ETS_UART_INTR_ENABLE();
  SPIFFS.begin();
  WiFi.setOutputPower(20.5); // möglicher Range: 0.0f ... 20.5f
  WiFi.mode(WIFI_STA);
  Serial.println("Start in Client Mode!");
  WiFi.begin();
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Start in Access Point Mode!");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(GlobalConfig.mySSID, GlobalConfig.myPWD, GlobalConfig.myChannel);
    strlcpy(myIPaddress, WiFi.softAPIP().toString().c_str(), sizeof(myIPaddress));
    myIP=WiFi.softAPIP();
    Serial.println(GlobalConfig.mySSID);
  } else {
    WiFi.setAutoReconnect(true);
    strlcpy(myIPaddress, WiFi.localIP().toString().c_str(), sizeof(myIPaddress));
    myIP=WiFi.localIP();
  }
  myHigherIP=myIP[1]+256*myIP[0];
  myLowerIP=myIP[3]+256*myIP[2];
  os_printf("IP: %s  Higher: %u   Lower: %u\n", myIP.toString().c_str(), myHigherIP, myLowerIP);
   
  int16_t numWLANs=  WiFi.scanComplete();
  if (numWLANs == -2) { // es gab bislang noch keinen WLAN-Scan
    WiFi.scanNetworks(true);
  }
  setupWebserver();
  if (MDNS.begin(CONF_MDNS_NAME)) {
    Serial.println("MDNS started!");
  } else {
    Serial.println("MDNS not initialized!");
  }
  strip.begin();
  strip.setBrightness(100);
  strip.show(); 
}



void loop() {
  do_pending_Actions();
}
