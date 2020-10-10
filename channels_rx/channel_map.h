#include "VarSpeedServo.h"

#define CHANNEL_COUNT 8

#define CHANNEL_1   A0
#define CHANNEL_2   A1
#define CHANNEL_3   A2
#define CHANNEL_4   A3
#define CHANNEL_5   4
#define CHANNEL_6   7
#define CHANNEL_7   8
#define CHANNEL_8   9

#define BASE_SPEED  200   // max speed = 255

VarSpeedServo channel_1;
VarSpeedServo channel_2;
VarSpeedServo channel_3;
VarSpeedServo channel_4;
VarSpeedServo channel_5;
VarSpeedServo channel_6;
VarSpeedServo channel_7;
VarSpeedServo channel_8;

bool      wait_                   = false;
uint8_t   write_iter              = 0;
uint8_t   CHANNELS[CHANNEL_COUNT] = { 0,0,0,0,0,0,0,0 };

void setup_outputs(){
  channel_1.attach(CHANNEL_1);
  channel_2.attach(CHANNEL_2);
  channel_3.attach(CHANNEL_3);
  channel_4.attach(CHANNEL_4);
  channel_5.attach(CHANNEL_5);
  channel_6.attach(CHANNEL_6);
  channel_7.attach(CHANNEL_7);
  channel_8.attach(CHANNEL_8);
}

void write_channel_data(uint8_t angle, uint8_t channel_number)
{
  if( angle > 165 )angle = 165;
  if( angle < 10 ) angle = 10;
  switch(channel_number){
    case 0: channel_1.write(angle, (uint8_t)BASE_SPEED,wait_);break;
    case 1: channel_2.write(angle, (uint8_t)BASE_SPEED,wait_);break;
    case 2: channel_3.write(angle, (uint8_t)BASE_SPEED,wait_);break;
    case 3: channel_4.write(angle, (uint8_t)BASE_SPEED,wait_);break;
    case 4: channel_5.write(angle, (uint8_t)BASE_SPEED,wait_);break;
    case 5: channel_6.write(angle, (uint8_t)BASE_SPEED,wait_);break;
    case 6: channel_7.write(angle, (uint8_t)BASE_SPEED,wait_);break;
    case 7: channel_8.write(angle, (uint8_t)BASE_SPEED,wait_);break;
  }
}

void write_all_channel_data(){
  for( write_iter=0 ; write_iter<CHANNEL_COUNT ; write_iter++)
  {
        Serial.print(CHANNELS[write_iter]);
        Serial.print(" | ");
        write_channel_data(CHANNELS[write_iter],write_iter);
  }
  Serial.println("");
}
