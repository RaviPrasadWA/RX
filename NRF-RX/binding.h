#include "EEPROMex.h"


void write_eeprom_long(uint8_t addr, uint32_t value){
  EEPROM.writeLong(addr, value);
}

void write_eeprom_byte(uint8_t addr, uint8_t value){
  EEPROM.writeLong(addr, value);
}

uint32_t read_eeprom_long(uint8_t addr){
  return EEPROM.readLong(addr);
}

uint8_t read_eeprom_byte(uint8_t addr){
  return EEPROM.readByte(addr);
}

void  write_verification_header_eeprom(){
  write_eeprom_byte(2,VERIFIED);
}

void write_bind_button_pressed_to_eeprom(){
  write_eeprom_byte(3, BIND_PRESSED);
}

void clear_bind_button_status_from_eeprom(){
  write_eeprom_byte(3, 0);
}

void write_tx_headers_to_eeprom(){ 
  // 0 -> total number of hops specified by the transmitter
  // 1 -> <IMPORTANT> unique transmitter ID for binding i.e., RX will be bound to this transmitter
  // 2 -> <IMPORTANT> Verified the TX and the HOPS
  // 3 -> <IMPORTANT> Wether the BIND button is continously pressed
  write_eeprom_byte(0,tx_config.total_hops);
  write_eeprom_long(1,tx_config.tx_id);
}

void clear_binding_data_eeprom(){
  write_eeprom_byte(0,0);
  write_eeprom_long(1,0);
}

void request_hops(uint32_t  tx_id){
  rx_config.ack   = 1;
  rx_config.tx_id = tx_id;
  rx_config.mode  = 2;
}


void request_hops_done(uint8_t total_hops){
  uint32_t  tx_id      = read_eeprom_long(1);
  rx_config.ack   = 1;
  rx_config.tx_id = tx_id;
  rx_config.mode  = 3;
}

void write_hops_to_eeprom(){
  uint8_t   total_hops = read_eeprom_byte(0);
  for( uint8_t i=0 ; i<=total_hops ; i++ )
    write_eeprom_byte(i+10, fhss_schema[i]);
}

void fetch_hops_from_eeprom(){
  uint8_t   total_hops = read_eeprom_byte(0);
  fhss_schema = (byte*) malloc(total_hops * sizeof(byte));
  for( uint8_t i=0 ; i<=total_hops ; i++ )
    fhss_schema[i] = read_eeprom_byte(i+10);
}

void ignore_read(uint8_t total_hops){
  byte sequence[total_hops];
  radio.read(&sequence,sizeof(sequence));
}

uint8_t get_fhss_hops(){
  uint8_t   total_hops = read_eeprom_byte(0);
  if(radio.available()){
    ignore_read(total_hops);
    byte sequence[total_hops];
    radio.read(&sequence,sizeof(sequence));
    fhss_schema = (byte*) malloc(total_hops * sizeof(byte));
    for (uint8_t i=0 ; i< total_hops; i++)
      fhss_schema[i] = sequence[i];
 
    request_hops_done(total_hops);
    radio.flush_tx(); // First, empty TX FIFO
    radio.writeAckPayload(1,&rx_config,sizeof(rx_config));  // Than, write data in TX FIFO
    memset(sequence, 0, sizeof(sequence));
    return 1;
  }
  return 0;
}

void get_hops(){
  Serial.print("HOPS = [*");
  for ( uint8_t i=0 ; i<250 ; i++ ){
    uint8_t flag = get_fhss_hops();
    if( flag == 1 ){
      Serial.println("*] Complete");
      break;
    }else{
      Serial.print("*");
    }  
    delay(500);
    if( i == 249 )
      Serial.println("!] ERROR");
  }
}

void print_all_hops(){
  uint8_t   total_hops = read_eeprom_byte(0);
  Serial.print("HOP SEQUENCE = [");
  for (uint8_t i=0 ; i< total_hops; i++)
  {
    Serial.print(fhss_schema[i]);
    Serial.print(",");
  }
  Serial.println("]");
}

void print_tx_config_details(){
  Serial.print("BIND data TX-ID = ");
  Serial.print(tx_config.tx_id);
  Serial.print(" Total HOPS = ");
  Serial.print( tx_config.total_hops );
  Serial.print(" MODE = ");
  Serial.print( tx_config.mode );
  Serial.println(" ");
}

uint8_t get_bind_data(){
  if(radio.available()) {
    radio.read(&tx_config,sizeof(tx_config));
    if( tx_config.mode == 1 )
    {
      request_hops(tx_config.tx_id);
      radio.flush_tx(); // First, empty TX FIFO
      radio.writeAckPayload(1,&rx_config,sizeof(rx_config));  // Than, write data in TX FIFO
      write_tx_headers_to_eeprom();
      return 1;
    }
  }
  return 0;
}


void perform_bind(){
  Serial.print("Binding = [*");
  for ( uint8_t i=0 ;i <=250; i++ )
   {
      uint8_t flag = get_bind_data();
      if( flag == 1 ){
        Serial.println("*] Complete");
        break;
      }
      else
        Serial.print("*");
      delay(500);
      if( i == 249 )
        Serial.println("!] ERROR");
   }
   print_tx_config_details();
}

uint8_t verify_hops(){
  uint8_t   total_hops = read_eeprom_byte(0);
  verify_config.is_verified = VERIFIED;
  if(radio.available()){
    byte sequence[total_hops];
    radio.read(&sequence,sizeof(sequence));
    
    for (uint8_t i=0 ; i< total_hops; i++)
    {
      
      if( fhss_schema[i] != sequence[i] )
        verify_config.is_verified = 0;
    }

    if( verify_config.is_verified == VERIFIED )
    {
      radio.flush_tx(); // First, empty TX FIFO
      radio.writeAckPayload(1,&verify_config,sizeof(verify_config));  // Than, write data in TX FIFO
      write_verification_header_eeprom();
      return 1;
    }
    memset(sequence, 0, sizeof(sequence));
  }
  return 0;
}

void perform_verification(){
  Serial.print("Verifying HOPS = [*");
  for ( uint8_t i=0 ;i <=250; i++ )
   {
      uint8_t flag = verify_hops();
      if( flag == 1 ){
        Serial.println("*] Complete");
        break;
      }
      else
        Serial.print("*");
      delay(500);
      if( i == 249 )
        Serial.println("!] ERROR");
   }
}


void delay_restart_rx(){
  Serial.print("Restarting the RX = [*");
  for(uint8_t x=0 ; x<20 ; x++){
    Serial.print("*");
    delay(275);
  }
  Serial.print("*] RE-STARTING RX");
  Serial.println(" ");
  delay(1500);
  reset_func();
}  


bool is_bind_button_released_from_previous_bind(){
  // This function helps determine wether the BIND 
  // button during previous bind was pressed and is still pressed
  // FALSE -> bind button still not released
  uint32_t prev_pressed = read_eeprom_byte(3);

  if( prev_pressed == BIND_PRESSED )
    return false;
  return true;
}

void start_rx(bool clear_prev){
  Serial.println("Fetching the HOPS from EEPROM");
  if( clear_prev )
    clear_bind_button_status_from_eeprom();
  fetch_hops_from_eeprom();
  print_all_hops();
  Serial.println("Fetching the HOPS from EEPROM COMPLETED");
}

void bind_routine(){
  if( get_button_state() == HIGH ){
    if( is_bind_button_released_from_previous_bind() )
    {
      Serial.println("Binding routine START");
      clear_binding_data_eeprom();
      perform_bind();
      Serial.println("[bind](sub-routine) => Getting the hops");
      get_hops();
      Serial.println("[bind](sub-routine) => Getting the hops DONE");
      
      Serial.println("[bind](sub-routine) => Verifying the hopping spectrum");
      perform_verification();
      Serial.println("[bind](sub-routine) => Verifying the hopping spectrum DONE");
      print_all_hops();
      write_hops_to_eeprom();
      write_bind_button_pressed_to_eeprom();
      Serial.println("Binding routine DONE");
      delay_restart_rx();
    }else{
      start_rx(false);
    }
  }else{
    start_rx(true);
  }
}
