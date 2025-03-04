#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


// a packet will also include the mac address
typedef struct {

  byte module_id[6];
  byte user_auth[8];

  float heat;
  float moisture;

} Packet_t;

byte mac[6];


byte self_id[6];
byte user_id[8];

const char* domain = "http://181.214.153.123:52067";
const char* pass = "onmyhonor";
const char* router = "optix";

WiFiClient client;

// in the future for this function, I want it to be able to ask for a id, and then self assign itself with a unique identifier


// int handshake(String domain){

//   // initiate handshake with server, ask for assignmnet of user id and new module id;

//   if(WiFi.status() == WL_CONNECTED){

//     HTTPClient http;

//     String mac_str = "";
  
//     for (int i = 0; i < 6; i++){
//       if(mac[i] < 0x10){ mac_str += "0"; }
//       mac_str += String(mac[i], HEX);
//       if(i < 5){
//         mac_str += ":";
//       }
//     }


    
//   }

//   return 0;
// }

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

    String request_data = ("module_id=" + bytes_to_string(packet.module_id, 6) + 
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



void setup(){

  Serial.begin(115200);

  // set pin modes here when they are decided... !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // testing, operations normal...

  // wait while we make handshake with router

  WiFi.begin(router, pass);

  // dht.begin();


  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
  }

  // This only runs when we are connected to the wifi on the router...

  WiFi.macAddress(mac);


  // This is basically saying, if I have an assigned ID, dont get another one from the server, but if this is the first time this module is running, then grab a unique id.

  // if(module_id == Null){
  //   int handshake_out = handshake();

  //   if(handshake_out == 0){


    
  //   } 
  // }

  
  
}


void loop(){

  digitalWrite(LED_BUILTIN, LOW);

  Packet_t working_packet = {
    .module_id = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06},
    .user_auth = {0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0xA7, 0xB8},

    .heat = 25.5,
    .moisture = 72.3
  };

  int result = transmit_packet(working_packet);
  if(result == 0){
    // successfull packet transmition


  } else{

    // on failure, blink a few times to make the idea clear
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

  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);
}
