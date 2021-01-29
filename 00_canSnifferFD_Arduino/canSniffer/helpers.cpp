#include "helpers.h"
#include <Arduino.h>
#include <SPI.h>

//------------------------------------------------------------------------------
// Printing a packet to serial
void printHex(int num)
{
    if (num < 0x10)
    {
        Serial.print("0");
    }
    Serial.print(num, HEX);
}

void printPacket(CANFDMessage packet)
{
    // packet format (hex string): [ID],[EXT],[TYPE],[DATABYTES 0..8B]\n
    // example: 014A,00,00,1A002B003C004D\n
    printHex(packet.id);
    Serial.print(SEPARATOR);
    printHex(packet.ext);
    Serial.print(SEPARATOR);
    printHex(packet.type);
    Serial.print(SEPARATOR);
    // DLC is determinded by number of data bytes, format: [00]
    for (int i = 0; i < packet.len; i++)
    {
        printHex(packet.data[i]);
    }
    Serial.print(TERMINATOR);
}
//------------------------------------------------------------------------------
// CAN packet simulator
void CANsimulate(void)
{
    CANFDMessage txPacket;

    int sampleIdList[] = {0x110, 0x115, 0x23A, 0x257, 0x501, 0x601, 0x621};
    int idIndex = random(sizeof(sampleIdList) / sizeof(sampleIdList[0]));

    txPacket.id = sampleIdList[idIndex];
    txPacket.ext = txPacket.id > 2047 ? true : false;
    txPacket.type = random(4);
    txPacket.len = random(1, 64);

    byte b;
    for (int i = 0; i < txPacket.len; i++)
    {
        b = random(256);
        txPacket.data[i] = b;
    }

    printPacket(txPacket);
}
//------------------------------------------------------------------------------
// CAN RX, TX
void CANRead()
{
    if (can.available())
    {
        CANFDMessage rxPacket;
        can.receive(rxPacket);

        // Print packet to serial
        printPacket(rxPacket);
    }
}

void sendPacketToCan(CANFDMessage packet)
{
    for (int retries = 10; retries > 0; retries--)
    {
        bool ok = can.tryToSend(packet);
        if (ok)
        {
            // success
            break;
        }
        else if (retries <= 1)
        {
            return;
        }
    }
}
//------------------------------------------------------------------------------
// Serial parser
char getNum(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return 0;
}

char *strToHex(char *str, byte *hexArray, byte *len)
{
    byte *ptr = hexArray;
    char *idx;
    for (idx = str; *idx != SEPARATOR && *idx != TERMINATOR; ++idx, ++ptr)
    {
        *ptr = (getNum(*idx++) << 4) + getNum(*idx);
    }
    *len = ptr - hexArray;
    return idx;
}

char *parseType(char *str, CANFDMessage::Type *type)
{
    switch (*(str + 1))
    {
    case '0':
        *type = CANFDMessage::CAN_REMOTE;
        break;
    case '1':
        *type = CANFDMessage::CAN_DATA;
        break;
    case '2':
        *type = CANFDMessage::CANFD_NO_BIT_RATE_SWITCH;
        break;
    case '3':
        *type = CANFDMessage::CANFD_WITH_BIT_RATE_SWITCH;
        break;
    default:
        *type = CANFDMessage::CAN_DATA;
        break;
    }
    return str + 2;
}

void rxParse(char *buf, int len)
{
    CANFDMessage rxPacket;
    char *ptr = buf;
    // All elements have to have leading zero!
    // Input format: [ID],[EXT],[TYPE],[DATABYTES 0..64B]\n

    // ID
    byte idTempArray[4], tempLen;
    ptr = strToHex(ptr, idTempArray, &tempLen);
    rxPacket.id = 0;
    for (int i = 0; i < tempLen; i++)
    {
        rxPacket.id |= idTempArray[i] << ((tempLen - i - 1) * 8);
    }

    // EXT
    byte ext;
    ptr = strToHex(ptr + 1, &ext, &tempLen);
    rxPacket.ext = ext ? true : false;

    // TYPE
    ptr = parseType(ptr + 1, &rxPacket.type);

    // DATA
    ptr = strToHex(ptr + 1, rxPacket.data, &rxPacket.len);

#if TEST_MODE == FAKE_SERIAL
    // echo back
    printPacket(rxPacket);
#else
    sendPacketToCan(rxPacket);
#endif
}

void SerialRead(void)
{
    static int rxPtr = 0;
    static char rxBuf[RXBUF_LEN];

    while (Serial.available() > 0)
    {
        if (rxPtr >= RXBUF_LEN)
        {
            rxPtr = 0;
        }
        char c = Serial.read();
        rxBuf[rxPtr++] = c;
        if (c == TERMINATOR)
        {
            rxParse(rxBuf, rxPtr);
            rxPtr = 0;
        }
    }
}
