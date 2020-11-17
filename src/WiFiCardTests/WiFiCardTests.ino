#include <SPI.h>
#include <WiFiNINA.h>
#include "secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS; //WiFi radio status

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress server(188, 149, 54, 45); // Victor's echo server
const int PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP Udp;



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

  Serial.println("\nStarting connection to server...");
  if(Udp.begin(localPort)){
    Serial.println("\nConnected to server!");
  }

}

void loop() {
  sendNTPpacket(server); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(3000);
  if ( Udp.parsePacket()!=0 ) {
      
    // We've received a packet
    Serial.println("packet received");
    int chars = Udp.read(packetBuffer, PACKET_SIZE); // read the packet into the buffer
    /*
     * Convert the bytes to printable characters and print them.
     * Terminate either on number of characters actually received 
     * or a null character which means end of string.
     * Both conditions are needed here to avoid out of bounds
     * access of data.
     */
    for(int i=0;i<chars && packetBuffer[i] > 0;i++)
    {
      char b1[2];
      sprintf(b1, "%c", packetBuffer[i]);
      b1[1]=0;
      Serial.print(b1);
    }
    Serial.println();
  }
  delay(7000); // wait ten seconds
}

unsigned long sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, PACKET_SIZE); // set all bytes in the buffer to 0
  int counter = 0;
  packetBuffer[counter++] = 'H'; 
  packetBuffer[counter++] = 'e';
  packetBuffer[counter++] = 'l';
  packetBuffer[counter++] = 'l';
  packetBuffer[counter++]  = 'o';
  packetBuffer[counter++]  = ' ';
  packetBuffer[counter++]  = 'w';
  packetBuffer[counter++]  = 'o';
  packetBuffer[counter++]  = 'r';
  packetBuffer[counter++]  = 'l';
  packetBuffer[counter++]  = 'd';
  packetBuffer[counter++]  = 'W';
  packetBuffer[counter++]  = 'i';
  packetBuffer[counter++]  = 'F';
  packetBuffer[counter++]  = 'i';

  Udp.beginPacket(address, localPort);
  Udp.write(packetBuffer, PACKET_SIZE);
  Udp.endPacket();
  Serial.println("packet sent");
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
