// Setup for nRF24L01
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 radio(6,7); // CE, CSN

const uint64_t pipes[2] = { 0xABCDABCD71LL, 
                            0x544d52687CLL };  // Address of PTX and PRX

void setup_radio(){
  printf_begin();
  radio.begin();
  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MIN (-18dBm), RF24_PA_LOW (-12dBm), RF24_PA_HIGH (-6dBM), RF24_PA_MAX (0dBm)
  radio.setRetries(4,9);
//  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(11);
  radio.openWritingPipe(pipes[0]); 
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  radio.printDetails();
}

void write_ackpayload()
{
    radio.flush_tx(); // First, empty TX FIFO
    radio.writeAckPayload(1,&data_TX,sizeof(data_TX));  // Than, write data in TX FIFO
}

void perform_hop(){
  if(fired) {  // When the interrupt occurred, we need to perform the following task
    fired=false;  // Reset fired flag
    Int_cnt++;  // Increment Interrupts counter
    if(Int_cnt > Int_TX_cnt) Int_cnt=0; // To avoid Interrupts counter become bigger than Int_TX_cnt

    //    If we are not synced with PTX, we stay in listening mode on current channel until the PTX transmit on this channel
    //    (remeber that PTX still clycling continuosly throught all channels of fhss schema, even we are or not synced with its)
    //    If we are synced with PTX we can do hopping channel following fhss schema
    if(Int_cnt==(Int_TX_cnt-1) && fhss_on==true) {  // Only if we are synced with PTX and if it's time to perform channel change (10ms before expected data from PTX)
      ptr_fhss_schema++;  // Increment pointer of fhss schema array to perform next channel change
      if(ptr_fhss_schema >= sizeof(fhss_schema)) ptr_fhss_schema=0; // To avoid fhss schema array indexing overflow
      radio.setChannel(fhss_schema[ptr_fhss_schema]); // Change channel
      write_ackpayload(); // Prepare the data to be sent back to the PTX (In this case, in advance of 10ms on expected trasmission from PTX)
    }
  }
}

void get_data_nrf(){
  //  The following code serves to declare "out of sync" of the receiver if we don't receive data for a time needed to cover the entire channels sequency schema (plus a little extra delay)
  //  and change channel in case this one is pertrubated for a long time
  //  In this mode I'm able to resyncronize PTX and PRX in any case (Reset of PTX, reset of PRX, channels perturbation, ecc.) and very quickly
  if((millis() - last_rx_time) > ((((sizeof(fhss_schema))+5)*10*Int_TX_cnt))) { 
    last_rx_time = millis();
    fhss_on=false;
    ptr_fhss_schema++;
    if(ptr_fhss_schema >= sizeof(fhss_schema)) ptr_fhss_schema=0;
    radio.setChannel(fhss_schema[ptr_fhss_schema]);
  }
  if(radio.available()) { 
    // Ok, we received valid data
    //    This is the other part of the algoritm trik
    //    PTX transmit when its interrupt occurs, 
    //    so we can reset the PRX interrupt counter to align the interrupt time with PTX
    //    and we do this every time we receive data. In this manner we stay forever synchronized, 
    //    even if there are different clock drift
    TCNT1 = 0; // Reset Interrupt counter 
    fhss_on=true; // Now we can follow the fhss schema (we are synced with PTX channel and with interrupt time of PTX)
    last_rx_time = millis();  // Update received time
    radio.read(&data_RX,sizeof(data_RX));
    //  data_RX consists of data from the PTX
    CHANNELS[0] = data_RX.ch1;
    CHANNELS[1] = data_RX.ch2;
    CHANNELS[2] = data_RX.ch3;
    CHANNELS[3] = data_RX.ch4;
    CHANNELS[4] = data_RX.ch5;
    CHANNELS[5] = data_RX.ch6;
    CHANNELS[6] = data_RX.ch7;
    CHANNELS[7] = data_RX.ch8;
    Int_cnt = 0;
  }
}
