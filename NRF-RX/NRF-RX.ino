#include "RX_Defs.h"
#include "CHWriter.h"
#include "fhss.h"
#include "nrf24l01_iface.h"
#include "binding.h"

uint32_t        start_50hz = 0;
static uint32_t last_50hz  = 0;

void setup(){
  Serial.begin(DEFAULT_BAUDRATE);
  setup_push_button();
  Serial.println("Initializing RF24-RX 8ch");
  
  Serial.println("Initializing NRF24L01+ Radio");
  setup_radio();
  Serial.println("Initializing NRF24L01+ Radio DONE");
  
  bind_routine();
  
  Serial.println("Initializing Timer");
  setup_timer();
  Serial.println("Initializing Timer DONE");

  setup_i2c_master();
  
}


void loop(){
  start_50hz = micros(); 
  
  perform_hop();
  get_data_nrf();
  
  /* 20ms ~ 50Hz */
  if( start_50hz - last_50hz >= 20000UL )
  {
    write_channels();
    last_50hz = start_50hz;
  }
}
