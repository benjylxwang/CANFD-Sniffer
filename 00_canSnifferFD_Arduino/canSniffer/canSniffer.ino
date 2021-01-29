// Developer:
//        Benjamin Wang & WonSeok Cho, 2021, All rights reserved.
//          Based on CAN Sniffer for CAN 2.0 OBD2 by Adam Varga
//------------------------------------------------------------------------------
#include <ACAN2517FD.h>
#include <Arduino.h>
#include <SPI.h>

#include "helpers.h"
//------------------------------------------------------------------------------
// FOR CAN-FD SHIELD: https://www.longan-labs.cc/1030012.html
// static const byte MCP2517_CS = 9;  // CS input of MCP2517
// static const byte MCP2517_INT = 2; // INT output of MCP2517

// FOR CANBed-FD: https://www.longan-labs.cc/1030009.html
static const byte MCP2517_CS = 17; // CS input of MCP2517
static const byte MCP2517_INT = 7; // INT output of MCP2517

#define CAN_SPEED (33E3) //LOW=33E3, MID=95E3, HIGH=500E3 (for Vectra)
//------------------------------------------------------------------------------
// Can controller class
ACAN2517FD can(MCP2517_CS, SPI, MCP2517_INT);

//------------------------------------------------------------------------------
// Setup
void setup()
{
  //--- Start serial
  Serial.begin(115200);
  //--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial)
  {
    delay(50);
  }
  //----------------------------------- Begin SPI
  SPI.begin();
#if TEST_MODE == CHECK_CONNECTION
  //--- Configure ACAN2517FD
  Serial.print("sizeof (ACAN2517FDSettings): ");
  Serial.print(sizeof(ACAN2517FDSettings));
  Serial.println(" bytes");
  Serial.println("Configure ACAN2517FD");
#endif

  ACAN2517FDSettings settings(ACAN2517FDSettings::OSC_20MHz, 500UL * 1000UL, DataBitRateFactor::x1);

  settings.mRequestedMode = ACAN2517FDSettings::NormalFD; // Select loopback mode
  //--- Default values are too high for an Arduino Uno that contains 2048 bytes of RAM: reduce them
  settings.mDriverTransmitFIFOSize = 1;
  settings.mDriverReceiveFIFOSize = 1;

#if TEST_MODE == CHECK_CONNECTION
  //--- RAM Usage
  Serial.print("MCP2517FD RAM Usage: ");
  Serial.print(settings.ramUsage());
  Serial.println(" bytes");
#endif

  //--- Begin
  const uint32_t errorCode = can.begin(settings, [] { can.isr(); });

#if TEST_MODE == CHECK_CONNECTION
  if (errorCode == 0)
  {
    Serial.println("CAN INIT OK!");
    Serial.print("Bit Rate prescaler: ");
    Serial.println(settings.mBitRatePrescaler);
    Serial.print("Arbitration Phase segment 1: ");
    Serial.println(settings.mArbitrationPhaseSegment1);
    Serial.print("Arbitration Phase segment 2: ");
    Serial.println(settings.mArbitrationPhaseSegment2);
    Serial.print("Arbitration SJW:");
    Serial.println(settings.mArbitrationSJW);
    Serial.print("Actual Arbitration Bit Rate: ");
    Serial.print(settings.actualArbitrationBitRate());
    Serial.println(" bit/s");
    Serial.print("Exact Arbitration Bit Rate ? ");
    Serial.println(settings.exactArbitrationBitRate() ? "yes" : "no");
    Serial.print("Arbitration Sample point: ");
    Serial.print(settings.arbitrationSamplePointFromBitStart());
    Serial.println("%");
  }
  else
  {
    Serial.print("Configuration error 0x");
    Serial.println(errorCode, HEX);
  }
#endif
}
//------------------------------------------------------------------------------
// Main
void loop()
{
  SerialRead();
  CANRead();
#if TEST_MODE == FAKE_SERIAL
  CANsimulate();
  delay(100);
#endif
}