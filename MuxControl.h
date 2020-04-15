// SPI Daisy chain control 3 switch for 12 ports

#ifndef MuxControl_h
#define MuxControl_h

// Arduino lib
#include <SoftwareSerial.h>
#include <string.h>
#include <SPI.h>

#define DebugMSG

class MuxControl
{
private:
  void writeMsg(const char* msg_str);
  void printMsg(const char* msg_str);
  void printMsg( int value, int = DEC );
  void spiTransmit(int data);
  void gpioControl(uint8_t pin, uint8_t level);
  void gpioSetting(uint8_t pin, uint8_t level);
  void initSPI(void);
  void delaymS(int miniseconds);

public:
  char inData[80];
  byte index = 0;

  // set pin 10 as the slave select for the digital pot:
  const int slaveSelectPin = 10;
  const int slaveRSTZPin = 9;
  int RelayStatus[3]= {0,0,0};
 
  void MuxInit(void);
  void MUX_HardwareReset(void);
  void MUX_DaisyChainCmd(void);
  void MUX_SerialWrite(int gpioAB1_4, int gpioAB5_8, int gpioAB9_12);
  void MUX_PrintRelayStatus( void);
  void MUX_ParserVaildCmd( char *str);

};

#endif
