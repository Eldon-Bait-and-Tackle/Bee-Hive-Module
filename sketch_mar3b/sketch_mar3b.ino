#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


typedef struct {
  byte user_auth[8];

  float heat;
  float moisture;

} Packet_t;



byte mac[6];
byte user_id[8];



const String domain = "127.0.0.1:52067";
const String pass = "onmyhonor";
const String router = "optix";



WiFiClient client;



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

  // send a network packet to the server to be interpreted
  // return true if sucessful...

  if(WiFi.status() == WL_CONNECTED){

    HTTPClient http;

    http.begin(client, domain);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int ard = 10;

    String request_data = ("module_id=" + String(ESP.getChipId()) +
                           "&user_auth="+ bytes_to_string(packet.user_auth, 8) + 
                           "&heat=" + String(packet.heat) + 
                           "&moisture=" + String(packet.moisture) + 
                           "&mac=" + bytes_to_string(mac, 6));


    int http_code = http.POST(request_data);

    if(http_code == HTTP_CODE_OK){
      String payload = http.getString();
      http.end();
      return 0;
    }
    else{
      http.end();
      return 2;
    }
  } else{
    // this fires if not connected to the WiFi using the client
    return 1;
  }


  return 1;
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
  
  // Try to connect with timeout
  while ((WiFi.status() != WL_CONNECTED) && (attempts < 10)) {
    attempts++;
    delay(500);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.macAddress(mac);
    return 0;
  }

  return 1;

}


void setup(){

  Serial.begin(115200);

  // set pin modes here when they are decided... !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  pinMode(LED_BUILTIN, OUTPUT);

  // wait while we make handshake with router

  Serial.print("Connecting to WiFi network: ");
  Serial.println(router);
  Serial.println(ESP.getChipId());


  int connection = connect_wifi();

  if(connection == 0){
    Serial.println("Could not connect to wifi on start up...");
  }

}

Packet_t working_packet = {
    .user_auth = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0xA7, 0xB8},
    .heat = 25.5,
    .moisture = 72.3
  };


void loop(){

  delay(1000);

  Serial.println("Loop Started");

  if(WiFi.status() != WL_CONNECTED){

    Serial.println("No connection, attempting to reconnect");
    int connected = connect_wifi();

    if(connected == 1){
      Serial.println("unable to connect to wifi on loop...");
    }
    else{
      Serial.println("Connected to wifi on loop...");
    }
  }

  int result = transmit_packet(working_packet);
  if(result == 0){
    // successfull packet transmition

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
    delay(200);
  } 
  else{
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }


  // wait this duration between sending transmitions, only need them once per our in production...

  delay(2000);
}
