#ifndef PACKETECHO_UDP_H
#define PACKETECHO_UDP_H

void connect();
void checkSerialInput();
void checkConnection();
void tx();
void rx();
void clearBuffer(byte *buffer, int len);
int insertInt(byte *buffer, int len, int index, int n);
void printBuffer(byte *buffer);
void printFailureRate();

#endif
