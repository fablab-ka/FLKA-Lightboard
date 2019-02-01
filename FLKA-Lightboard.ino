

/****************************************************************************************************
*                                                                                                   *
*****************************************************************************************************/

#include "AllIncludes.h"

// globale Variablen ==============================================================================//
volatile boolean pending_reboot = false;
volatile boolean pending_RGB = false;
volatile uint8_t cycleRGB = 0;
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



char* byte2text(uint16_t value){
  static char convbuffer[6];
  memset(convbuffer, 0, sizeof(convbuffer));
  if (value < 10) {
    convbuffer[0]=0x30;
    convbuffer[1]=value+0x30;
  } else 
  itoa(value,convbuffer, 10);
  return convbuffer;
}

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, 14, NEO_GRB + NEO_KHZ800);  //D5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(9, 12, NEO_GRB + NEO_KHZ800);

uint32_t octalColorCodes[] = {
         0,                                                      // white
         ((uint32_t)0x2D << 16) | ((uint32_t)0x00 << 8) | 0x00,  // red
         ((uint32_t)0x69 << 16) | ((uint32_t)0x69 << 8) | 0x00,  // yellow
         ((uint32_t)0x11 << 16) | ((uint32_t)0x6E << 8) | 0x00,  // green
         ((uint32_t)0x00 << 16) | ((uint32_t)0x42 << 8) | 0x17,  // cyan
         ((uint32_t)0x00 << 16) | ((uint32_t)0x00 << 8) | 0x64,  // blue
         ((uint32_t)0x50 << 16) | ((uint32_t)0x00 << 8) | 0x50,  // purple
         ((uint32_t)0x3F << 16) | ((uint32_t)0x3F << 8) | 0x3F   // white
};

void encodeIP2color(uint8_t frameID, uint32_t frameData) {
  for(int i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  yield();
  strip.setPixelColor(0, octalColorCodes[6]); //direction-marker
  for(int i=1; i<6; i++){
    uint8_t j = frameData % 8;
    frameData = frameData >> 3;
    strip.setPixelColor(i, octalColorCodes[j]);
  }
  yield();
  uint8_t startPos  = (frameData % 8)+frameID;
  strip.setPixelColor(6, octalColorCodes[startPos]);
  strip.show();
}

void showMyIP() {
  uint32_t started = millis();
  uint32_t lastrun = 0;
  boolean singleFrame = (myHigherIP == 49320) ? true : false;
  //boolean singleFrame = (myHigherIP == 49321) ? true : false;
  uint8_t frameNo = 0;
  while (started+(80*1000) > millis() && (!pending_reboot)) {
    os_printf("while... lastrun: %u   millis: %u\n", lastrun, millis());
    if (( lastrun + 10*1000) < millis() || (0 == lastrun)) {
      if (singleFrame) {
        encodeIP2color(0, myLowerIP);
      } else {
        if (0 == frameNo ) {
          encodeIP2color(2, myLowerIP);
          frameNo=1;
        } else {
          encodeIP2color(4, myHigherIP);
          frameNo=0;
        }
      }
      os_printf("Frame %u sent\n", frameNo);
      lastrun= millis();
    }  
    yield();
    delay(200);
  }
  for(int i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
  os_printf("strip cleared\n");
}


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
  showMyIP();
}



void loop() {
  do_pending_Actions();
}
