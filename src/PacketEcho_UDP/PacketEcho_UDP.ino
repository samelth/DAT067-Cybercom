#include <MKRNB.h>

#include "arduino_secrets.h"
#include "PacketEcho_UDP.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

IPAddress victorServer(188, 149, 54, 45); // Victor's echo server
IPAddress samuelServer(83, 252, 118, 131); // Samuel's echo server
IPAddress karlServer(85, 230, 107, 67); // Karl's echo server
IPAddress williamServer(188,148,194,26); // William's echo server
IPAddress nullIP(0,0,0,0);

const int PACKET_SIZE = 128;
const uint16_t rxPort = 2390;
const uint16_t txPort = 2390;
const IPAddress &txIP = williamServer;

byte rxBuffer[PACKET_SIZE];
byte txBuffer[PACKET_SIZE];

byte failBuffer[PACKET_SIZE];

int rxPkt = 0;
int txPkt = 0;
int socket;

String response;

GPRS gprs;
NB nb;
NBScanner nbScanner;
NBUDP nbUdp;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Starting PacketEcho client..");
  
  connect();
  
  //Close all sockets
  for(int i=0;i<7;i++){
    MODEM.sendf("AT+USOCL=%d",i);
     if(MODEM.waitForResponse(2000, &response)!=1){
    Serial.println("Socket already closed");
    }else{
       Serial.println("Socket closed");
      }
    delay(200);
  }
  
  nbUdp.begin(rxPort);
  socket = nbUdp.getSocket();
}

void loop()
{
  checkSerialInput();
  tx();
  delay(500);
  rx();
  delay(500);
  printFailureRate();
  delay(500);
  resetUdpSocket();
  Serial.println();
}

void resetUdpSocket(){
  MODEM.sendf("AT+USOCL=%d",socket);
  if(MODEM.waitForResponse(2000, &response)!=1){
  // (AT+USOCTL=<socket>,1) Returns the last IP stack error code produced while operating on <socket>.
  // Very useful for debugging. Error codes are found in SARA R4 AT command pdf (Appendix 5). 
  Serial.println("Error number: ");
  MODEM.sendf("AT+USOCTL=%d,1",socket); 
  }else{
     //Serial.println("Socket closed");
  }
  //Open socket
  nbUdp.begin(rxPort);
  socket = nbUdp.getSocket();
}

void connect()
{
  Serial.println("Connecting..");
  bool connected = false;
  // After starting the modem with NB.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected) {
    if ((nb.begin(PINNUMBER) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Connection failed. Trying again..");
      delay(1000);
    }
  }
  Serial.println("Connected.");
}

void checkSerialInput()
{
  //When running the program, send 'p' via serial to access the pause menu.
  if(Serial.read() == 'p')
  {
    Serial.println("Pause menu - enter a number, or 'p' to unpause.\n 1. Your IP address \n 2. Current signal strength \n 3. Current carrier \n 4. Reconnect");
    delay(500);
    while(Serial.read() != 'p')
    {
      int s = Serial.read();
      while(s !=- 1)
      {
        switch(s)
        {
        case '1': 
          Serial.println(gprs.getIPAddress());
          break;
        case '2':
          Serial.println(nbScanner.getSignalStrength());
          break;
        case '3':
          Serial.println(nbScanner.getCurrentCarrier());
          break;
        case '4':
          connect();
          break;
        default:
          break;
        }
        s = Serial.read();
      }
    }
  }
}

void tx()
{
  clearBuffer(txBuffer, PACKET_SIZE);
  int index = 0;
  txBuffer[index++]  = 'p';
  txBuffer[index++]  = 'k';
  txBuffer[index++]  = 't';
  txBuffer[index++]  = 'N';
  txBuffer[index++]  = 'o';
  txBuffer[index++]  = ' ';
  index = insertInt(txBuffer, PACKET_SIZE, index, txPkt + 1);
  nbUdp.beginPacket(txIP, txPort);
  nbUdp.write(txBuffer, PACKET_SIZE);
  nbUdp.endPacket();
  txPkt++;
  Serial.print("Sent: ");
  printBuffer(txBuffer);
  Serial.println();
}

void rx()
{
  int rxSize = 0;
  int rxAttempts = 0;
  while(!(rxSize = nbUdp.parsePacket()) && (rxAttempts++ < 10))
  {
    delay(500);
  }
  
  while(rxSize)
  {
    rxPkt++;
    clearBuffer(rxBuffer, PACKET_SIZE);
    nbUdp.read(rxBuffer, PACKET_SIZE);
    Serial.print("Received: ");
    printBuffer(rxBuffer);
    delay(500);
    rxSize = nbUdp.parsePacket();
  }
}

void clearBuffer(byte *buffer, int len)
{
  memset(buffer, 0, len);
}

int insertInt(byte *buffer, int len, int index, int n)
{
  if(index >= len - 1)
    return index;

  int noOfDigits = 1;
  int a = n;
  while(a /= 10)
    noOfDigits++;
  
  int resultingLen = index + noOfDigits + (n < 0 ? 1 : 0);

  if(resultingLen >= len)
    return index;

  if(n < 0)
  {
    buffer[index] = '-';
    return insertInt(buffer, len, index + 1, ~n + 1);
  }

  int i = index + noOfDigits - 1;
  while(i >= index)
  {
    buffer[i--] = n % 10 + '0';
    n /= 10;
  }
  return index + noOfDigits;
}

void printBuffer(byte *buffer)
{
  Serial.println((char *)buffer);
}

void printFailureRate()
{
  Serial.print("Failure rate: ");
  clearBuffer(failBuffer, PACKET_SIZE);
  int index = insertInt(failBuffer, PACKET_SIZE, 0, txPkt - rxPkt);
  if(index >= PACKET_SIZE)
    return;
  failBuffer[index++] = '/';
  insertInt(failBuffer, PACKET_SIZE, index, txPkt);
  printBuffer(failBuffer);
}
