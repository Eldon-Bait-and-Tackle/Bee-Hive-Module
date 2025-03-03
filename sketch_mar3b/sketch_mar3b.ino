#include <cstring>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

struct {

  int8_t module_id[6];
  int8_t user_auth[8];

  float heat;
  float moisture;

} Packet_t;


int8_t self_id[6];
int8_t user_id[8];

String domain;
String wifipass;
String router;

WiFiClient client;

byte mac[6];




// this is for assigning yourself a user id and password
int handshake(string domain){

  // initiate handshake with server, ask for assignmnet of user id and new module id;



  
}


int send_packet(Packet_t packet){

  // send a network packet to the server to be interpreted
  // return true if sucessful...

  if(WiFi.status() == WL_CONNECTED){

    HTTPClient http;

    http.begin(client, domain);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String request = ("temp=" + String())

  }
  else{
    // this fires if not connected to the WiFi using the client
    return 1;
  }



  return 0;
}


void setup(){

  Serial.begin(115200);
  Wifi.begin(router, wifipass);


  // wait while we make handshake with router
  while(WiFi.stats() != WL_CONNECTED){

    delay(1000);

  }

  // This only runs when we are connected to the wifi on the router...

  // Set pin mode for the sensors.

  WiFi.macAddress(mac);





  return 0;
  



}


void loop(){

}
