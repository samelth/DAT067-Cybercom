#include <MKRNB.h>

#include "arduino_secrets.h"
#include "ArduinoLowPower.h"
#include "RTCZero.h"
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
const int BLINK_DELAY = 1000;
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
    
  }
  
  nbUdp.begin(rxPort);
  socket = nbUdp.getSocket();
  pinMode(LED_BUILTIN, OUTPUT);
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
  LowPower.sleep(2000);
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
  
  if(rxSize)
  {
    rxPkt++;
    clearBuffer(rxBuffer, PACKET_SIZE);
    rxSize = nbUdp.read(rxBuffer, PACKET_SIZE);
    Serial.print("Received: ");
    printBuffer(rxBuffer);
    Serial.println();
    blink(BLINK_DELAY);
  }
}

void clearBuffer(byte *buffer, int len)
{
  memset(buffer, 0, len);
}

int insertInt(byte *buffer, int len, int index, int n)
{
  int i = index;
  if(i >= len - 1)
  {
    Serial.println("Buffer full.");
    return i;
  }

  if(n / 10 == 0)
  {
    buffer[i] = n + '0';
    return i;
  }

  int a = n;
  int b = 1;
  int c;
  int d;

  while(a / 10)
  {
    a /= 10;
    b *= 10;
  }

  buffer[i++] = a + '0';
  b *= a;
  c = (n / 10);
  d = n - b;

  while(d / 10)
  {
    c /= 10;
    d /= 10;
  }

  while(c / 10)
  {
    c /= 10;
    buffer[i++] = '0';
  }

  insertInt(buffer, len, i, n - b);
}

void printBuffer(byte *buffer)
{
  Serial.println((char *) buffer);
}

void printFailureRate()
{
  Serial.print("Failure rate: ");
  clearBuffer(failBuffer, PACKET_SIZE);
  int index = insertInt(failBuffer, PACKET_SIZE, 0, txPkt - rxPkt);
  failBuffer[index + 1] = '/';
  insertInt(failBuffer, PACKET_SIZE, index + 2, txPkt);
  printBuffer(failBuffer);
  Serial.println();
}

void blink(int delay_time){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(delay_time);
  digitalWrite(LED_BUILTIN, LOW);
}
