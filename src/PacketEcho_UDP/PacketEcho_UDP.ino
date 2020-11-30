#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;

unsigned int localPort = 2390;      // Local port to listen for UDP packets

IPAddress VictorServer(188, 149, 54, 45); // Victor's echo server
IPAddress SamuelServer(83, 252, 118, 131); // Samuel's echo server
const int PACKET_SIZE = 128;

byte packetBuffer[PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// Initialize the library instance
GPRS gprs;
NB nbAccess;
NBScanner scanner;

//If current IP address is equal to this nullIP we need to reconnect before we send any data.
IPAddress nullIP= IPAddress(0,0,0,0);

//Packet identifiers
char packetNr0='0';
char packetNr1='0';
char packetNr2='0';

// A UDP instance to let us send and receive packets over UDP
NBUDP Udp;
void setup()
{
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

void loop()
{
  //When running the program, send 'p' via serial to access the pause menu.
  if(Serial.read()==112){
    Serial.println("Pause Menu, press a number! \n 1. Your IP address \n 2. Current signal strength \n 3. Current carrier \n 4. Reconnect");
     delay(500);
     while(Serial.read()!=112){
      int s = Serial.read();
      while(s!=-1){
        switch(s){
          case 49: 
           Serial.println(gprs.getIPAddress());
           break;
         case 50:
          Serial.println(scanner.getSignalStrength());
          break;
         case 51:
          Serial.println(scanner.getCurrentCarrier());
          break;
         case 52:
          connect();
          break;
         default:
          break;
        }
        s=Serial.read();
      }
    }
  }
  if(gprs.getIPAddress()== nullIP){
    Serial.print("Connection lost.\n");
    connect();
    }
  Serial.print("");
  
  sendUDPpacket(SamuelServer); //Send an UDP packet to an echo server.
  delay(2000); //Wait two seconds
  
 
  while ( Udp.parsePacket()!=0 ) {  //If there are incoming packets on buffer, print them FCFS.
    Serial.print("Received ");
    int chars = Udp.read(packetBuffer, PACKET_SIZE); // Read the packet into the buffer
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
  delay(1000); // Wait one second
}

unsigned long sendUDPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, PACKET_SIZE); // Set all bytes in the buffer to 0
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
      Serial.println("Not connected, trying again..");
      delay(1000);
    }
  }
  Serial.println("Success!");
  }
