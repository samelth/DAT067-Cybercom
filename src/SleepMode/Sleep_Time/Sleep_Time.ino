
#include <MKRNB.h>
#include "arduino_secrets.h"
const char PINNUMBER[]     = SECRET_PINNUMBER;
unsigned int localPort = 2390;      // Local port to listen for UDP packets
IPAddress Server(85, 230, 107, 67);
const int PACKET_SIZE = 128;

GPRS gprs;
NB nbAccess;
NBScanner scanner;
NBUDP Udp;
String response;

byte packetBuffer[PACKET_SIZE];

//Packet identifiers
char packetNr0='0';
char packetNr1='0';
char packetNr2='0';

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting PacketEcho Client...");
  connect();
  

 
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);

}

void loop() {
  
  sendUDPpacket(Server);
  delay(2000);
  Serial.println("\nEntering SleepMode");
  digitalWrite(LED_BUILTIN, LOW);
  //LowPower.deepSleep(3000000)
 
  
}

unsigned long sendUDPpacket(IPAddress& address){
  memset(packetBuffer, 0, PACKET_SIZE);
  int counter = 0;

  packetBuffer[counter++]  = 'P'; 
  packetBuffer[counter++]  = 'a';
  packetBuffer[counter++]  = 'c';
  packetBuffer[counter++]  = 'k';
  packetBuffer[counter++]  = 'e';
  packetBuffer[counter++]  = 't';
  packetBuffer[counter++]  = 'N';
  packetBuffer[counter++]  = 'r';
  packetBuffer[counter++]  = ':';
  packetBuffer[counter++]  = ' ';
  packetBuffer[counter++]  = packetNr2;
  packetBuffer[counter++]  = packetNr1;
  packetBuffer[counter++]  = packetNr0;

  Udp.beginPacket(address, localPort);
  Udp.write(packetBuffer, PACKET_SIZE);
  Udp.endPacket();

   //Put the packet numbers to a printable string
  char *pNr; 
  pNr =(char*) malloc(4*sizeof(char));
  pNr[0]=packetNr2;
  pNr[1]=packetNr1;
  pNr[2]=packetNr0;
  pNr[3]='\0';
  incPacketNr();
  
  Serial.print("Sending packet: ");
  Serial.println(pNr);
  free(pNr);
}

void incPacketNr(){
  packetNr0++;
  if(packetNr0==':'){
    packetNr0='0';
    packetNr1++;
    if(packetNr1==':'){
      packetNr1='0';
      packetNr2++;
      if(packetNr2==':'){
        packetNr2='0';
      }
    }
  }
}

void connect(){
  Serial.println("Connecting...");
    boolean connected = false;
  // After starting the modem with NB.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected) {
    if ((nbAccess.begin(PINNUMBER) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  Serial.println("Success!");
  
    Serial.println("Request PSM");
  
  MODEM.sendf("AT+CPSMS=1,,,\"00000110\",\"00011110\"");
  MODEM.waitForResponse(2000);
  MODEM.sendf("AT+UCPSMS?");
  MODEM.waitForResponse(2000, &response);
  Serial.println(response);
  MODEM.sendf("AT+UPSMR=1");
  delay(5000);  
  }
