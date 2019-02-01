
#include "Webcontent.h"
// Bestandteile der Webseite ==============================================================================//

// Webserver
AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws



void setupWebserver()
{
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.addHandler(new SPIFFSEditor(GlobalConfig.myAdminUser,GlobalConfig.myAdminPWD));
  server.on("/wificonnectAP", HTTP_GET, handleWiFiConnect2AP);
  server.on("/wificonfigAP", HTTP_POST|HTTP_GET, handleConfigAP);
  server.on("/wifiRestartAP", HTTP_POST, handleWiFiRestartAP);
  server.on("/wifiRestartSTA", HTTP_GET, handleWiFiRestartSTA);
  server.on("/wpsconfig", HTTP_GET, handleWPSConfig);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  
  server.begin();
}


void handleConfigAP(AsyncWebServerRequest *request){
  if(!request->authenticate(GlobalConfig.myAdminUser, GlobalConfig.myAdminPWD))
    return request->requestAuthentication();
  Serial.println("Config Start");

  struct T_ConfigStruct tempConf;
  char myAdminPWD2[20];
  
  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    if(! p->isFile()) {  // ONLY for GET and POST
      if (! strcmp(p->name().c_str(), "SSID")) {
        strlcpy( tempConf.mySSID, p->value().c_str(), sizeof(tempConf.mySSID));
        Serial.print("SSID: "); Serial.println(tempConf.mySSID);
      } 
      if (! strcmp(p->name().c_str(), "WLANPWD")) {
        strlcpy( tempConf.myPWD, p->value().c_str(), sizeof(tempConf.myPWD));
        Serial.print("WLAN-PWD: "); Serial.println(tempConf.myPWD);
      } 
      if (! strcmp(p->name().c_str(), "Kanal")) {
         tempConf.myChannel = atoi(p->value().c_str());
        Serial.print("Channel: "); Serial.println(tempConf.myChannel);
      } 
      if (! strcmp(p->name().c_str(), "ADMPWD")) {
        strlcpy( tempConf.myAdminPWD, p->value().c_str(), sizeof(tempConf.myAdminPWD));
        Serial.print("ADM: "); Serial.println(tempConf.myAdminPWD);
      } 
      if (! strcmp(p->name().c_str(), "ADMPWD2")) {
        strlcpy( myAdminPWD2, p->value().c_str(), sizeof(myAdminPWD2));
        Serial.print("ADMPWD2: "); Serial.println(myAdminPWD2);
      } 
    }
  }
  String page = FPSTR(HTML_HEAD);
  page.replace("{title}", FPSTR(HTML_TITLE_CONFIG));
  page += FPSTR(HTML_BASIC_STYLES);
  page += FPSTR(HTML_BODYSTART);
  File menuFile = SPIFFS.open("/.menu.html", "r");
  while (menuFile.available()){
    page += menuFile.readStringUntil('\n');
  }
  menuFile.close();
  page = page+"<h1>"+FPSTR(HTML_TITLE_CONFIG)+"</h1>";
  String form=FPSTR(HTML_GLOBAL_CONF );
  form.replace("{SSID}",GlobalConfig.mySSID);
  form.replace("{WLANPWD}",GlobalConfig.myPWD);
  String ch; ch+=GlobalConfig.myChannel;
  form.replace("{CH}",ch);
  form.replace("{ADMPWD}",GlobalConfig.myAdminPWD);

  if (params >2)  {
    Serial.println("Configuration complete");
    if((strlen(tempConf.myAdminPWD) > 0) and 
       (strlen(myAdminPWD2) >0) and
       (0 == strncmp(tempConf.myAdminPWD, myAdminPWD2, sizeof(myAdminPWD2))))  {
      strlcpy(tempConf.magic, CONF_MAGIC, sizeof(tempConf.magic));
      tempConf.version=CONF_VERSION;
      ConfigSave(&tempConf);
      pending_reboot=true;
      request->send(200, "text/plain", "OK");
      return;
    } else {
      page+=FPSTR(HTML_PWD_WARN);
    }
  }
  
  page+=form;page+="<br>";
  page+=FPSTR(HTML_BODYEND);
  request->send(200, "text/html", page);
}


void handleWiFiConnect2AP(AsyncWebServerRequest *request){
  Serial.println("WiFiConfig Start");
  int16_t numWLANs=  WiFi.scanComplete();
  if (numWLANs == -2) { // es gab bislang noch keinen WLAN-Scan
    WiFi.scanNetworks(true);
    Serial.print("Scan start at: ");Serial.println(millis());
  }
  String page = FPSTR(HTML_HEAD);
  page.replace("{title}", FPSTR(HTML_TITLE_APCONNECT));
  page += FPSTR(HTML_SELECTSCRIPT);
  page += FPSTR(HTML_BASIC_STYLES);
  page += FPSTR(HTML_BODYSTART);
  File menuFile = SPIFFS.open("/.menu.html", "r");
  while (menuFile.available()){
    page += menuFile.readStringUntil('\n');
  }
  menuFile.close();
  
  page = page+"<h1>"+FPSTR(HTML_TITLE_APCONNECT)+"</h1>";
  if (numWLANs == -1) { //scan still in progress
    // warte, bis der Scan fertig ist
    while(numWLANs == -1){ yield(); numWLANs=  WiFi.scanComplete();}
  }
  if (numWLANs == 0) {
    page += FPSTR(HTML_NO_WLAN_FOUND);  
  } else {
    page += FPSTR(HTML_DIVTABLESTART);
    for (uint16_t i=0; i<numWLANs; i++) {
      page += FPSTR(HTML_ROWTABLESTART);
      Serial.print("List Network no: ");
      Serial.println(i);
      String wlan    = FPSTR(HTML_ACCESSPOINT_INFO);
      String quality; quality+=WiFi.RSSI(i);
      wlan.replace("{wlan}",WiFi.SSID(i));
      wlan.replace("{quality}",quality);
      switch (WiFi.encryptionType(i)) {
        case 2: wlan.replace("{security}","WPA2 (TKIP)");break;
        case 4: wlan.replace("{security}","WPA (CCMP)");break;
        case 5: wlan.replace("{security}","WEP :-(");break;
        case 7: wlan.replace("{security}","ungesichert");break;
        case 8: wlan.replace("{security}","AUTO");break;
        default: wlan.replace("{security}","unknown");break;
      }
      wlan.replace("{mac}",WiFi.BSSIDstr(i));
      String channel; channel+=WiFi.channel(i);
      wlan.replace("{ch}",channel);
      page+=wlan;
      page += FPSTR(HTML_DIVEND);  //row
    }
    page += FPSTR(HTML_DIVEND);
    
  }
  page+="<br><br>";
  
  page+=FPSTR(HTML_FORM_SSID_PWD );page+="<br>";
  page+=FPSTR(HTML_LINK_SCAN_CONFIG);page+="<br>";
  page+=FPSTR(HTML_LINK_AP_CONFIG);page+="<br>";
  //page+=FPSTR(HTML_LINK_WPS);page+="<br>";
  page+=FPSTR(HTML_BODYEND);
  request->send(200, "text/html", page);
  WiFi.scanNetworks(true);  //Starte nächsten WLAN-Scan (nach dem Requestende des Servers!!)
}


void handleWiFiRestartAP(AsyncWebServerRequest *request){
  handleWiFiRestart(request, true);
}
void handleWiFiRestartSTA(AsyncWebServerRequest *request){
  handleWiFiRestart(request, false);
}

void handleWiFiRestart(AsyncWebServerRequest *request, boolean asAP){
  char  clientSSID[32] = "";
  char  clientPWD[64]  = "";  
  Serial.println("WiFiSave start");
  int params = request->params();
  String page = "";
  if (asAP) {
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(! p->isFile()) {  // ONLY for GET and POST
        if (! strcmp(p->name().c_str(), "s")) {
          strlcpy(  clientSSID, p->value().c_str(), sizeof( clientSSID));
          Serial.println("SSID received!");
        }
        if (! strcmp(p->name().c_str(), "p")) {
          strlcpy(  clientPWD, p->value().c_str(), sizeof( clientPWD));
          Serial.println("PWD received!");
        }
      }
    }
  }
  File menuFile = SPIFFS.open("/decodeIP.html", "r");
  while (menuFile.available()){
    Serial.println("  reading line...");
    page += menuFile.readStringUntil('\n');
  }
  request->send(200, "text/html", page);
  Serial.println("Request done");
  delay(3000); // allow js and css to be loaded ...
  struct station_config newConf;  memset(&newConf, 0, sizeof(newConf));
  strlcpy((char *)newConf.ssid, clientSSID, strlen(clientSSID)+1);
  strlcpy((char *)newConf.password, clientPWD, strlen(clientPWD)+1);
  ETS_UART_INTR_DISABLE();
  wifi_station_set_config(&newConf);
  ETS_UART_INTR_ENABLE();
  Serial.println("New config is written");
  pending_reboot=true; //make sure, ESP gets rebooted
}

void handleWPSConfig(AsyncWebServerRequest *request){
  Serial.println("wps config done");
  String message="<html><body><h1>WPSCONFIG NOT YET IMPLEMENTED</h1></body></html>";
  request->send(200, "text/html", message);
  
}

void handleApConfig(AsyncWebServerRequest *request){
  Serial.println("AP config done");
  String message="<html><body><h1>AP CONFIG NOT YET IMPLEMENTED</h1></body></html>";
  request->send(200, "text/html", message);
}


void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  StaticJsonBuffer<300> jsonBuffer;
  Serial.println("Websockets fired");
  if(type == WS_EVT_CONNECT){
    //client connected
    Serial.println("EVT_connect");
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("EVT_disconnect");
  } else if(type == WS_EVT_ERROR){
    Serial.println("EVT_ERROR");
  } else if(type == WS_EVT_PONG){
    // pong message was received (in response to a ping request maybe)
    // os_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    //data packet
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      if(info->opcode == WS_TEXT) {
        data[len] = 0;
        //char buffer[100]; 
        //snprintf(buffer, sizeof(buffer),  "received message: %s", (char*)data);
        //client->text(buffer);
        JsonObject &root = jsonBuffer.parseObject((char*)data);
       if ( !root.success() ) {
          Serial.println("parse JSON-Object failed");Serial.flush();
        } else {
          const char* magic = root["magic"];
          const char* cmd = root["cmd"];
          if (0 == strncmp("FLKA", magic, 4)) {
            if(0 == strncmp("RGB", cmd, 3)) {
              //set RGB color
              JsonArray& value = root["value"];
              RGBtoSet[0]=value[0];RGBtoSet[1]=value[1];RGBtoSet[2]=value[2];
              pending_RGB = true;
              cycleRGB=0;
            }
            if(0 == strncmp("CYCLE", cmd, 5)) {
              pending_RGB = false;
              cycleRGB = 1;
              RGBdelay=root["delay"];
            }
            if(0 == strncmp("RAINBOW", cmd, 7)) {
              pending_RGB = false;
              cycleRGB = 2;
              RGBdelay=root["delay"];
            }
            
          }
        }
      } else {
        // we currently dont react on binary messages
      }  
    } else {
      // we don't expect fragmentation either
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void do_SetRGB() {
  for(uint8_t i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, RGBtoSet[0], RGBtoSet[1], RGBtoSet[2]); 
  }
  yield();
  strip.show();
  pending_RGB = false;
  cycleRGB = 0;
}
void do_CycleRGB() {
  uint16_t i;
  static uint16_t j=0;
  if ((lastRun+RGBdelay) < millis()) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
 //     ESP.wdtFeed();
    }
    yield();
    j=(j+1) % 256;
    strip.show();
    lastRun=millis();
  }  
}


void do_Rainbow() {
  uint16_t i;
  static uint16_t j=0;
  if ((lastRun+RGBdelay) < millis()) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    yield();
    j=(j+1) % 256;
    strip.show();
    lastRun=millis();
  }  
}

void do_pending_Actions(void){
  // do all things, which need to be done after an asynchronous request
  if (pending_reboot) {
    Serial.println("Restarting system, hold on");
    delay(1000);
    // Sicherstellen, dass die GPIO-Pins dem Bootloader das erneute Starten des Programmes erlauben!
    pinMode( 0,OUTPUT);
    pinMode( 2,OUTPUT);
    pinMode(15,OUTPUT);
    digitalWrite(15, LOW);
    digitalWrite( 0,HIGH);
    digitalWrite( 2,HIGH);
    delay(50);
    ESP.restart();
  }  
  if (pending_RGB) {
    do_SetRGB();
  }
  if (1 == cycleRGB) {
    do_CycleRGB();
  }
  if (2 == cycleRGB) {
    do_Rainbow();
  }
}
