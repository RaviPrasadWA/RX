#define DEFAULT_BAUDRATE    115200
#define BIND_BUTTON         A0
#define VERIFIED            23
#define BIND_PRESSED        165

void(* reset_func) (void) = 0;

struct tx_config {
  uint8_t   total_hops;
  uint32_t  tx_id;
  uint8_t   mode;
} tx_config;

struct rx_config {
  uint8_t  ack;
  uint32_t tx_id;
  uint8_t  mode;// mode = 1:BIND ; mode= 2:HOPS; mode=3 :DONE
} rx_config;

struct verify_config {
  uint8_t     is_verified;
} verify_config;

typedef struct{  // Struct of data to recieved from PTX
  uint8_t ch1;
  uint8_t ch2;
  uint8_t ch3;
  uint8_t ch4;
  uint8_t ch5;
  uint8_t ch6;
  uint8_t ch7;
  uint8_t ch8;
}A_t;

typedef struct{ // Struct of data to send to PTX
  int var1_value;
  int var2_value;
  int var3_value;
  int var4_value;
  int var5_value;
  int var6_value;
}B_t;

A_t data_RX;
B_t data_TX;


void setup_push_button(){
  pinMode(BIND_BUTTON, INPUT);
}

int get_button_state(){
  return digitalRead(BIND_BUTTON);
}
