#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define BATTERY_PIN D6
#define DELAY 1000

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
  .hmac = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0xA7, 0xB8},
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
    return 1;
  }
}

int connect_wifi(){
  if(WiFi.status() == WL_CONNECTED){
    return 0;
  }

  WiFi.disconnect();
  delay(300);

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



/// MODULE LOOP AND SETUP \/


void setup(){
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(BATTERY_PIN, INPUT_PULLUP);
  
  Serial.println("Starting up");
  connect_wifi();
}

void loop(){
  delay(DELAY);
  Serial.println("Loop Started");

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("No connection, attempting to reconnect");
    connect_wifi();
  }

  working_packet.battery_state = digitalRead(BATTERY_PIN);
  Serial.print("D6 (Battery Pin) State: ");
  Serial.println(working_packet.battery_state);
  
  int result = transmit_packet(working_packet);
  if(result == 0){
    Serial.println("Transmition successsful");
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
  } 
  else if (result == 1){
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Transmition failed: No WiFi connection");
  }
  else if (result == 2){
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Transmition failed: HTTP POST error");
  }
}
