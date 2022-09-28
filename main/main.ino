/*
  Credits:
    - https://github.com/projetsdiy/esp-01-WiFi-Scanner-ESP-01-OLED-I2C-Display
    - IoT community
    - founders of the libraries
    - me lol
*/

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>  // parse json
#include <ESP8266HTTPClient.h>
#include <UnixTime.h>  // unix -> normal
#include <Wire.h>

#include "ESP8266WiFi.h"

WiFiClientSecure client;

HTTPClient http;

String http_addr = "https://economia.awesomeapi.com.br/json/last/USD-RON";
String http_addr2 = "https://economia.awesomeapi.com.br/json/last/USD-EUR";


UnixTime stamp(3);

char buffer[20];
char bufrssi[4];
char* ssid = "numeretea";
char* password ="parola";
bool Connecte = false;
bool affiche = 0;

#define OLED_address 0x3c

extern "C" {
#include "user_interface.h"
}

void setup() {
  Serial.begin(9600);
  delay(2000);
  client.setInsecure();
  Serial.print("Heap: ");
  Serial.println(system_get_free_heap_size());
  Serial.print("Boot Vers: ");
  Serial.println(system_get_boot_version());
  Serial.print("CPU: ");
  Serial.println(system_get_cpu_freq());
  Serial.println();
  Wire.pins(0, 2);
  Wire.begin();
  StartUp_OLED();
  clear_display();
  sendStrXY(" Foarte tare ", 0, 1);
  sendStrXY(" yesyes text ", 2, 1);
  sendStrXY(" wow omg  ", 4, 1);
  delay(2000);
  Serial.println("Fin du setup");
}

StaticJsonDocument<384> doc;  // pentru parsare JSON

void Connexion()
{
  affiche = false;
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }
  }
}

void loop() {
  Connexion();
    if (!affiche) {
//      affiche = true;
if (!client.connected())
;// aia e
      clear_display();

      // On récupère et on prépare le buffer contenant l'adresse IP attibué à
      // l'ESP-01
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) +
                     '.' + String(ip[3]);
      ipStr.toCharArray(buffer, 20);

      // On récupère et on prépare le buffer de char contenant la force du
      // signal WiFi
      long rssi = WiFi.RSSI();
      String strRssi;
      strRssi = String(rssi);
      strRssi.toCharArray(bufrssi, 4);
//      Serial.println(rssi);

      // On actualise l'affichage sur l'écran OLED
      // --- AICI ---

      // -- conexiune http --
      // initiate the HTTP connection
      Serial.print("[HTTP] starting...\n");
      // configure traged server and url
      Serial.println(http_addr);

      // --- api cam meh, trebuie schema

      http.begin(client, http_addr2);  // HTTPS
      int httpCode = http.GET();
      http.end();
      
      // ---

      
      http.begin(client, http_addr);  // HTTPS

      // start connection and send HTTP header
      httpCode = http.GET();
      Serial.println("htttpcode ");
      Serial.print(httpCode);
     
      if (httpCode > 0) {
        // If the request is valid
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
          StaticJsonDocument<384> doc;

DeserializationError error = deserializeJson(doc, payload);

if (error) {
  Serial.print(F("deserializeJson() failed: "));
  Serial.println(error.f_str());
  return;
}

JsonObject USDRON = doc["USDRON"];

double USDRON_bid = USDRON["bid"]; // "5.0992"
int USDRON_timestamp = USDRON["timestamp"]; // "1664382171"

          Serial.println("bid ");
          Serial.print(USDRON_bid);
          stamp.getDateTime(USDRON_timestamp);
          String combinat = String(stamp.day) + "." + (stamp.month > 9 ? String(stamp.month) : "0" + String(stamp.month)) + "  " +
                            String(stamp.hour) + ":" + (stamp.minute > 9 ? String(stamp.minute) : "0" + String(stamp.minute)) +
                            ":" + (stamp.second > 9 ? String(stamp.second) : "0" + String(stamp.second));
                            
          Serial.println("combinat: ");
          Serial.print(combinat);
          
          char bbb[16];
          char ccc[12];
          
          combinat.toCharArray(bbb,16);
          dtostrf(USDRON_bid, 6, 4, ccc);

          for(short i=6; i>0; --i)
            ccc[i]=ccc[i-1];
          for(short i=7; i>1; --i)
            ccc[i]=ccc[i-1];
          ccc[0]='$';
          ccc[1]='=';
          ccc[8]=' ';
          ccc[9]='R'; ccc[10]='O'; ccc[11]='N';
          
          sendStrXY(bbb, 0, 1);
          sendStrXY(ccc, 3, 1);
          sendStrXY("by Sabin", 6, 8);
          Serial.println("ok");
          http.end();
          Serial.println("http end");
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n",
                      http.errorToString(httpCode).c_str());
      }
  } else {
    clear_display();
    sendStrXY(" Foarte da ", 0, 1);
    sendStrXY(" se conecteaza ", 2, 1);
    sendStrXY(" sper ", 4, 1);
  }
  Serial.println("delay acum, loop");
  Serial.println(" ");
  delay(30000);
}
