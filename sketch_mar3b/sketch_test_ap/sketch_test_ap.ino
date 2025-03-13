#include <ESP8266WiFi.h>

const char *ssid ="ELDONHERE";
const char *password = "poop";

byte mac[6];

void setup(){
  
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.begin();
  WiFi.macAddress(mac);

  Serial.println(WiFi.softAPIP());


  pinMode(LED_BUILTIN, OUTPUT);

}

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

void loop(){


  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);


  Serial.println(bytes_to_string(mac, 6));
  Serial.println(WiFi.softAPIP());
  Serial.println("Looped");
  


}