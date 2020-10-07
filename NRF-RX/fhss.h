volatile boolean  fired = false;
int               interrupt_time = 10; // In millisenconds (Do not overdo it with too short interrupt time)
byte              Int_cnt = 0; // Interrupt counter
byte              Int_TX_cnt = 5; // Setting up the number of interrupts count that trig the data tramission 
//
// Channels hopping schema (Each number MUST BE unique in the sequence)
// It's possible to use non unique number in sequence, but in this case we must send the ptr_fhss_schema value to PRX
// thought the data_TX struct to align ptr_fhss_schema of PRX
// However, I prefer the sequence of unique numbers
byte              *fhss_schema;
byte              ptr_fhss_schema = 0; // Pointer to the fhss_schema array
unsigned long     last_rx_time = 0;
bool              fhss_on = false;  // Syncronization flag with PTX (at setup time must be false)

ISR (TIMER1_COMPA_vect)
{
  fired = true;
}

void setup_timer(){
  // Setup interrupt every interrupt_time value (MUST BE THE SAME OF PTX)
  // CTC mode with clk/8 prescaler
  TCCR1A = 0;
  TCCR1B = 1<<WGM12 | 1<<CS11;
  TCNT1 = 0;         // reset counter
  OCR1A =  (interrupt_time*2000)-1;       // compare A register value
  TIFR1 |= _BV (OCF1A);    // clear interrupt flag
  TIMSK1 = _BV (OCIE1A);   // interrupt on Compare A Match
  last_rx_time = millis();
}
