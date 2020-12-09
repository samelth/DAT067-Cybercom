#include <MKRNB.h>
#include "arduino_secrets.h"
#include "ArduinoLowPower.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;

const unsigned int localPort = 2390;      // Local port to listen for UDP packets

const IPAddress VictorServer(188, 149, 54, 45); // Victor's echo server
const IPAddress SamuelServer(83, 252, 118, 131); // Samuel's echo server
const IPAddress KarlServer(85, 230, 107, 67); // Karl's echo server
const IPAddress WilliamServer(188,148,194,26); // William's echo server
IPAddress server = WilliamServer; 

const int PACKET_SIZE = 128;
int packetLost = 0;
int packetSucess = 0; 
int timer=10;

byte packetSendBuffer[PACKET_SIZE]; //buffer to hold incoming and outgoing packets
byte packetReceiveBuffer[PACKET_SIZE];

// Initialize the library instance
GPRS gprs;
NB nbAccess;
NBScanner scanner;

//Packet identifiers
char packetNr0='0';
char packetNr1='0';
char packetNr2='0';

// A UDP instance to let us send and receive packets over UDP
NBUDP Udp;
void setup(){
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Starting MeasurePackageLoss Client...");
  connect();
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
  sendUDPpacket(server); //Send an UDP packet to an echo server.
}



void loop(){
  if(Udp.parsePacket()!=0) {  //If there are incoming packets on buffer, print them FCFS.
    Udp.read(packetReceiveBuffer, PACKET_SIZE); // Read the packet into the buffer
    Serial.print("received packet: ");
    Serial.println( (char *) packetReceiveBuffer);
    if (Udp.parsePacket() == 0) {
      incPacketNr();
      packetSucess++;
      printStatus();
      resetTimer(10);
      sendUDPpacket(server); // send next udp packet. 
    }
  }
  if(timer>0){
    timer--;
  }else {
    sendUDPpacket(server); //Send an UDP packet to an echo server.
    resetTimer(10);
    packetLost++; 
    Serial.println("timed out packet");
    printStatus();
  }
  delay(1000); // wait atleast one second
}

void resetTimer(int init){
  timer=init;
}

int isSamePacketNr(){ // Unfinished
  return packetNr2 == packetReceiveBuffer[0] &&
         packetNr1 == packetReceiveBuffer[1] && 
         packetNr0 == packetReceiveBuffer[2];
} 
void printStatus(){
  char buffer[255];
  sprintf(buffer,"Packet lost: %d Packet success: %d , Packet-fidelity: %d %%", 
    packetLost, packetSucess, 100*packetSucess/(packetLost + packetSucess)); 
  Serial.println(buffer); //packetLost,packetSucess);
}
unsigned long sendUDPpacket(IPAddress& address)
{
  memset(packetSendBuffer, 0, PACKET_SIZE); // Set all bytes in the buffer to 0
  int counter = 0;
  packetSendBuffer[counter++]  = packetNr2;
  packetSendBuffer[counter++]  = packetNr1;
  packetSendBuffer[counter++]  = packetNr0;

  
  Udp.beginPacket(address, localPort);
  Udp.write(packetSendBuffer, PACKET_SIZE);
  Udp.endPacket();

  //Put the packet numbers to a printable string
  char *pNr; 
  pNr =(char*) malloc(4*sizeof(char));
  pNr[0]=packetNr2;
  pNr[1]=packetNr1;
  pNr[2]=packetNr0;
  pNr[3]='\0';
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
      Serial.println("Not connected, trying again..");
      delay(1000);
    }
  }
  Serial.println("Success!");
}
