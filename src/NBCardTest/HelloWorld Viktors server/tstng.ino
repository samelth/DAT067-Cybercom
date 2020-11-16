#include <MKRNB.h>

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(188, 149, 54, 45); // Victor's echo server

const int PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

// A UDP instance to let us send and receive packets over UDP
NBUDP Udp;
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting Arduino GPRS NTP client.");
  // connection state
  boolean connected = false;
  nbAccess.shutdown();
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

  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
}

void loop()
{
  sendNTPpacket(timeServer); // send an NTP packet to a time server
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
  packetBuffer[counter++]  = '!';

  Udp.beginPacket(address, localPort);
  Udp.write(packetBuffer, PACKET_SIZE);
  Udp.endPacket();
  Serial.println("packet sent");
}
