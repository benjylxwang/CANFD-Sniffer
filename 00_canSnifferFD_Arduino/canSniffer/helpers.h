#ifndef HELPERS_H
#define HELPERS_H

#include <ACAN2517FD.h>
#include <Arduino.h>

// Settings
// Choose between STANDALONE, FAKE_SERIAL, and CHECK_CONNECTION
#define TEST_MODE FAKE_SERIAL
#define STANDALONE 1
#define FAKE_SERIAL 2
#define CHECK_CONNECTION 3

const char SEPARATOR = ',';
const char TERMINATOR = '\n';
const char RXBUF_LEN = 100;

extern ACAN2517FD can;

//------------------------------------------------------------------------------
// Printing a packet to serial
void printHex(int num);
void printPacket(CANFDMessage packet);

//------------------------------------------------------------------------------
// CAN packet simulator
void CANsimulate(void);

//------------------------------------------------------------------------------
// CAN RX, TX
void CANRead();
void sendPacketToCan(CANFDMessage packet);

//------------------------------------------------------------------------------
// Serial parser
char getNum(char c);
char *strToHex(char *str, byte *hexArray, byte *len);
void SerialRead(void);

#endif