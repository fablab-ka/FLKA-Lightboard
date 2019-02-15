

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

Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, 14, NEO_GRB + NEO_KHZ800);  //D5
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, 12, NEO_GRB + NEO_KHZ800);



void setup(){
  pinMode( 0,OUTPUT);
  digitalWrite(0, HIGH);
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
    Serial.print("Start in Access Point Mode: ");
    delay(200);
    WiFi.mode(WIFI_AP);
    Serial.println(WiFi.softAP(GlobalConfig.mySSID, GlobalConfig.myPWD, GlobalConfig.myChannel) ? "succesful!" : "failed!");
    strlcpy(myIPaddress, WiFi.softAPIP().toString().c_str(), sizeof(myIPaddress));
    myIP=WiFi.softAPIP();
    Serial.println(GlobalConfig.mySSID);
  } else {
    WiFi.setAutoReconnect(true);
    strlcpy(myIPaddress, WiFi.localIP().toString().c_str(), sizeof(myIPaddress));
    myIP=WiFi.localIP();
    
  }
  Serial.println( myIP.toString().c_str() );
  myHigherIP=myIP[1]+256*myIP[0];
  myLowerIP=myIP[3]+256*myIP[2];
 
  setupWebserver();
  if (MDNS.begin(CONF_MDNS_NAME)) {
    Serial.println("MDNS started!");
  } else {
    Serial.println("MDNS not initialized!");
  }
  MDNS.addService("http","tcp",80);
  strip.begin();
  strip.setBrightness(100);
  strip.show(); 
}



void loop() {
  do_pending_Actions();
}
