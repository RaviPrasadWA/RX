#include <Wire.h>

#define     CHANNEL_COUNT   8
#define     SERVO_MIN       1100
#define     SERVO_MAX       1800
#define     ANGLE_MIN       0
#define     ANGLE_MAX       180

const byte  SLAVE_ADDRESS           = 42;
uint8_t     CHANNELS[CHANNEL_COUNT] = {0,0,0,0,0,0,0,0};
uint8_t     INDEX                   = 0;

template <typename T> unsigned int I2C_writeAnything (const T& value)
{
Wire.write((byte *) &value, sizeof (value));
return sizeof (value);
}  // end of I2C_writeAnything

void initialize_channel_data(){
  for(INDEX = 0; INDEX < CHANNEL_COUNT; INDEX++)
    CHANNELS[INDEX] = 0;
}

void setup_i2c_master(){
  Wire.begin();
  //initialize_channel_data();
}

void write_channels(){
  Wire.beginTransmission (SLAVE_ADDRESS);
  for( INDEX=0 ; INDEX<CHANNEL_COUNT; INDEX++ )
    I2C_writeAnything(CHANNELS[INDEX]);
  Wire.endTransmission ();
}
