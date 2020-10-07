#include <Wire.h>
#include "CHWriter.h"
#include "channel_map.h"

const byte        MY_ADDRESS              = 42;
volatile boolean  haveData                = false;

void setup()
{
  Wire.begin (MY_ADDRESS);
//  Serial.begin (115200);
  setup_outputs();
  Wire.onReceive (receiveEvent);
}  // end of setup

void loop()
{
  if( haveData )
  {
      write_all_channel_data();
      haveData = false;
  }
}  // end of loop

// called by interrupt service routine when incoming data arrives
void receiveEvent (int howMany)
{
  for( uint8_t i=0 ; i<howMany ; i++ )
    I2C_readAnything( CHANNELS[i] );
  haveData = true;
    
} 
