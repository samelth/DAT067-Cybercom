#include <SPI.h>
#include <WiFiNINA.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS; //WiFi radio status

void setup() {
  Serial.begin(9600);
  while(!Serial){
    
  }

  if(WiFi.status() == WL_NO_MODULE){
    Serial.println("No communication with WiFi module");
    while(true);
  }

  String fv = WiFi.firmwareVersion();
  if(fv < WIFI_FIRMWARE_LATEST_VERSION){
    Serial.println("WiFi firmware update available");
  }

  while(status != WL_CONNECTED){
    Serial.print("Connecting to: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);

    delay(10000);
  }

  Serial.print("Connected to network");
  printCurrentNet();

}

void loop() {
  // put your main code here, to run repeatedly:

}

void printWifiData(){
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void printCurrentNet(){
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  printWifiData();
}
