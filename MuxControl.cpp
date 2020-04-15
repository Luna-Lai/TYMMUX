#include "MuxControl.h"

#define DebugMSG

void MuxControl::MuxInit(void)
{
  printMsg("12 Channel Switch Control!");

 // set the slaveSelectPin as an output:
  gpioControl(slaveSelectPin, OUTPUT);
  // initialize SPI:
  initSPI();
}

void MuxControl::MUX_ParserVaildCmd(char *str)
{             
        String inputData = str;
        if(inputData=="help")  {            
            printMsg("\nMUX Command List");
            printMsg(" enter dc   : enter daisy chain");
            printMsg(" reset      : hardware reset");  
            printMsg(" allsw a,b,c: a:0x0F(S1~4) b:0x0F(S5~8) c:0x0F(S9~12)");          
            printMsg(" onsw a,b   : a:1~12 b:1 close / 0 open"); 
            printMsg(" readstatus : read S1~S12 status");                             
        }
            
        else if(inputData=="enter dc")  {
#ifdef DebugMSG             
            printMsg("\nEnter Daisy Chain \n");
#endif //DebugMSG              
            MUX_DaisyChainCmd();
        }
        else if(inputData=="reset")  {
#ifdef DebugMSG
            printMsg("\nhardware reset");
#endif //DebugMSG
           MUX_HardwareReset();
//              MUX_SerialWrite(0x0B,0xA3,0x05);
        }
        else if(inputData=="readstatus")  {
           MUX_PrintRelayStatus();
        }
        else if(inputData.substring(0,5) == "allsw")    
        {
            char *s;              
            strtok(str," ");
            s=str;
              
            s=strtok(NULL,",");
            RelayStatus[0]= (strtol(s,&s,0) & 0x0f);           // only accept low nibble 
            s=strtok(NULL,",");
            RelayStatus[1]= (strtol(s,&s,0)& 0x0f);           // only accept low nibble 
            s=strtok(NULL,",");
            RelayStatus[2]= (strtol(s,&s,0)&0x0f);            // only accept low nibble 
  
#ifdef DebugMSG            
            MUX_PrintRelayStatus();
#endif //DebugMSG              
            MUX_SerialWrite(RelayStatus[0],RelayStatus[1],RelayStatus[2]);
            Serial.write("\r\n");              
        }
        else if(inputData.substring(0,5) == "onesw")  
        {
            char *s;
            int RelaySetting[3];
              
            strtok(str," ");
            s=str;
              
            s=strtok(NULL,",");
            RelaySetting[0]= strtol(s,&s,0);             
            s=strtok(NULL,",");
            RelaySetting[1]= strtol(s,&s,0);
              
            if( RelaySetting[0]>12)
                printMsg("\n invalid parameter");
            else if( RelaySetting[0]==0)
                printMsg("\n invalid parameter");
            else if( RelaySetting[1]>=2)
                printMsg("\n invalid parameter");  
            else
              {   
                switch(RelaySetting[0]){
                case 1: 
                  if(RelaySetting[1])
                    RelayStatus[0] |= 0x01;
                  else
                   RelayStatus[0] &= ~0x01;
                  break;
                case 2: 
                  if(RelaySetting[1])
                    RelayStatus[0] |= 0x02;
                  else
                   RelayStatus[0] &= ~0x02;
                  break;
                case 3: 
                  if(RelaySetting[1])
                    RelayStatus[0] |= 0x04;
                  else
                   RelayStatus[0] &= ~0x04;
                  break;  
                case 4: 
                  if(RelaySetting[1])
                    RelayStatus[0] |= 0x08;
                  else
                   RelayStatus[0] &= ~0x08;
                  break; 

                case 5: 
                  if(RelaySetting[1])
                    RelayStatus[1] |= 0x01;
                  else
                   RelayStatus[1] &= ~0x01;
                  break;
                case 6: 
                  if(RelaySetting[1])
                    RelayStatus[1] |= 0x02;
                  else
                   RelayStatus[1] &= ~0x02;
                  break;
                case 7: 
                  if(RelaySetting[1])
                    RelayStatus[1] |= 0x04;
                  else
                   RelayStatus[1] &= ~0x04;
                  break;  
                case 8: 
                  if(RelaySetting[1])
                    RelayStatus[1] |= 0x08;
                  else
                   RelayStatus[1] &= ~0x08;
                  break;  

                  // IC 3
                case 9: 
                  if(RelaySetting[1])
                    RelayStatus[2] |= 0x01;
                  else
                   RelayStatus[2] &= ~0x01;
                  break;
                case 10: 
                  if(RelaySetting[1])
                    RelayStatus[2] |= 0x02;
                  else
                   RelayStatus[2] &= ~0x02;
                  break;
                case 11: 
                  if(RelaySetting[1])
                    RelayStatus[2] |= 0x04;
                  else
                   RelayStatus[2] &= ~0x04;
                  break;  
                case 12: 
                  if(RelaySetting[1])
                    RelayStatus[2] |= 0x08;
                  else
                   RelayStatus[2] &= ~0x08;
                  break; 
                default:
                  break;                  
                }                
#ifdef DebugMSG            
           MUX_PrintRelayStatus();
#endif //DebugMSG     

             }
            writeMsg("\r\n");              
              }
            else{
            //Serial.write(str);
            writeMsg("invalid command\r\n");
            }   
        str=strtok(NULL,",");
}

void MuxControl::MUX_HardwareReset(void) {
    // take the RSTZ pin low to reset the chip:
  gpioControl(slaveRSTZPin, LOW);
  delaymS(10);
  gpioControl(slaveRSTZPin, HIGH);
  delaymS(120);       // Time for system resumming
}

void MuxControl::MUX_DaisyChainCmd(void) {
  // take the SS pin low to select the chip:
  gpioControl(slaveSelectPin, LOW);
  delaymS(100);
  //  send in the address and value via SPI:
  spiTransmit(0x25);
  spiTransmit(0x00);
  delaymS(100);
  // take the SS pin high to de-select the chip:
  gpioControl(slaveSelectPin, HIGH);
}

void MuxControl::MUX_SerialWrite(int gpioAB1_4, int gpioAB5_8, int gpioAB9_12) {
  // take the SS pin low to select the chip:
  gpioControl(slaveSelectPin, LOW);
  delaymS(100);
  //  send in the address and value via SPI:
  spiTransmit(gpioAB9_12);
  spiTransmit(gpioAB5_8);
  spiTransmit(gpioAB1_4);
  delaymS(100);
  // take the SS pin high to de-select the chip:
  gpioControl(slaveSelectPin, HIGH);
}

void MuxControl::MUX_PrintRelayStatus( void){

              printMsg("SWITCH 1~4 ");
              printMsg( RelayStatus[0],HEX);
              printMsg("SWITCH 5~8 ");
              printMsg( RelayStatus[1],HEX);
              printMsg("SWITCH 9~12 ");
              printMsg( RelayStatus[2],HEX);
}


// Arduino functions
void MuxControl::gpioControl(uint8_t pin, uint8_t level)
{
    digitalWrite(pin, level);
}

void gpioSetting(uint8_t pin, uint8_t level)
{
    pinMode(pin, level);
};

void MuxControl::printMsg(const char* msg_str)
{
    Serial.println(*msg_str);
};
void MuxControl::printMsg(int value, int format )
{
    Serial.println(value,format);
}
void MuxControl::writeMsg(const char* msg_str)
{
    Serial.write(*msg_str);
};
void MuxControl::spiTransmit(int data ) 
{
    SPI.transfer(data);
}

void MuxControl::initSPI(void)
{
  SPI.begin();
}

void MuxControl::delaymS(int miniseconds)
{
  delay(miniseconds);
}
