#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define BATTERY_PIN D4
#define T_PIN D6
#define M_PIN D7
#define ONE_HOUR_US 3600000000ULL

typedef struct {
  byte hmac[8];
  float heat;
  float moisture;
  int battery_state;
} Packet_t;

const char* domain = "181.41.202.157:28170";
const char* pass = "password";
const char* router = "ssid";

WiFiClient client;
Packet_t working_packet = {
  .hmac = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0xA7, 0xB8}, // placeholder change this to update to hmac
  .heat = 25.5,
  .moisture = 72.3
};

String bytes_to_string(byte* array, int length){
  String result = "";
  for(int i = 0; i < length; i++){
    if(array[i] < 0x10){
      result += "0";
    }
    result += String(array[i], HEX);
    if(i < length - 1){
      result += ":";
    }
  }
  return result;
}

int handshake(WiFiClient& client) {
  HTTPClient http;
  String challenge = "";

  if (http.begin(client, String("http://") + domain)) {
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument req1(128);
    req1["module_id"] = MODULE_ID;
    req1["chip_id"] = CHIP_ID;
    String req1Body;
    serializeJson(req1, req1Body);

    int httpCode = http.POST(req1Body);

    if (httpCode == HTTP_CODE_OK) {
      DynamicJsonDocument resp1(256);
      deserializeJson(resp1, http.getString());
      if (resp1.containsKey("challenge")) {
        challenge = resp1["challenge"].as<String>();
      }
    }
    http.end();
  }

  if (challenge.length() == 0) {
    return 1;
  }

  byte hmacResult[32];
  Sha256.initHmac(secret_key, 32);
  Sha256.print(challenge);
  Sha256.resultHmac(hmacResult);

  String hmacHex = "";
  for (int i = 0; i < 32; i++) {
    if (hmacResult[i] < 0x10) hmacHex += "0";
    hmacHex += String(hmacResult[i], HEX);
  }

  if (http.begin(client, String("http://") + domain)) {
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument req2(256);
    req2["module_id"] = MODULE_ID;
    req2["chip_id"] = CHIP_ID;
    req2["response"] = hmacHex;
    String req2Body;
    serializeJson(req2, req2Body);

    int httpCode = http.POST(req2Body);

    if (httpCode == HTTP_CODE_OK) {
      DynamicJsonDocument resp2(256);
      deserializeJson(resp2, http.getString());
      if (resp2.containsKey("auth_token")) {
        return 0;
      }
    }
    http.end();
  }
  return 1;
}

int transmit_packet(Packet_t packet){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(client, domain);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String request_data = ("module_id=" + String(ESP.getChipId()) +
                           "&hmac="+ bytes_to_string(packet.hmac, 8) + 
                           "&heat=" + String(packet.heat) + 
                           "&moisture=" + String(packet.moisture) + 
                           "&battery=" + String(packet.battery_state));
    int http_code = http.POST(request_data);

    if(http_code == HTTP_CODE_OK){
      http.end();
      return 0;
    }
    else{
      http.end();
      return 2;
    }

    
  } else{
    http.end();
    return 1;
  }
}

int connect_wifi(){
  if(WiFi.status() == WL_CONNECTED){
    return 0;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(router, pass);

  int attempts = 0;
  while ((WiFi.status() != WL_CONNECTED) && (attempts < 10)) { 
    attempts++; 
    delay(500);
  } 
  
  if (WiFi.status() == WL_CONNECTED) { 
    return 0;
  } 
  return 1;
}

void setup(){

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  pinMode(BATTERY_PIN, INPUT_PULLUP);
  pinMode(T_PIN, INPUT_PULLUP);
  pinMode(M_PIN, INPUT_PULLUP);

  
  // Serial.begin(115200);
  WiFi.forceSleepBegin();
}

void loop(){
  WiFi.forceSleepWake();
  delay(10);

  Packet_t working_packet;

  if(connect_wifi() == 0){
    working_packet.battery_state = digitalRead(BATTERY_PIN);
    working_packet.heat = digitalRead(T_PIN);
    working_packet.moisture = digitalRead(M_PIN);
    
    // for testing, consdier removing for release
    int result = transmit_packet(working_packet);

    if(result == 0){
      Serial.println("Transmition successsful");
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
    } 
    else if (result == 2){
      Serial.println("Transmition failed: HTTP POST error");
    }
    
    WiFi.disconnect(true);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Transmition failed: No WiFi connection");
  }

  Serial.println("Entering Deep Sleep for 1 hour...");
  ESP.deepSleep(ONE_HOUR_US);

  // esp is unreachable past this point
}
