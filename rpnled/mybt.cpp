#include "mybt.h"   //Software Serial Port

OneWireSerial bt;
char* bt_buffer;
int bt_maxlen;

btCallback bt_callback;

int bt_index = 0;
bool bt_newphrase = 1;
uint8_t bt_phraselen = 0;
int bt_phrasecnt = 0;
int bt_rxc = 0;

void btReset(void) {
  bt_index = 0;
  bt_newphrase = 1;
  bt_phraselen = 0;
  bt_phrasecnt = 0;
  bt_rxc = 0;
}

void btUpdate(void) {
  while (bt.available() > 0) {
    if (bt_newphrase) {
      bt_phraselen = bt.read();
      bt_newphrase = 0;
      bt_phrasecnt = 0;
      DEBUG("New phrase: ", bt_phraselen);
      if (bt_phraselen == 0) {
        bt_callback(bt_rxc);
        btReset();
      }
      continue;
    }

    if (bt_rxc < bt_maxlen-1) {
      if (bt_phrasecnt++ == bt_phraselen-1) {
        bt_buffer[bt_rxc++] = 0;
        DEBUG("  end phrase: ", (int)bt_buffer[bt_rxc-1]);
        bt_newphrase = 1;
      } else {
        bt_buffer[bt_rxc++] = bt.read();
        DEBUG("  stored: ", (int)bt_buffer[bt_rxc-1]);
      
        if (bt_buffer[bt_rxc-1] == 0) {
          DEBUG("--ERROR--: ", bt_rxc);
          bt_callback(bt_rxc);
          btReset();
          continue;
        }
      }
    }
  }
}

void btSend(char c) {
  bt.write(c);
}

void btSetup(char* b, int maxlen, btCallback callback) {
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  bt = OneWireSerial(TxD,RxD);
  bt.begin(9600); //Set BluetoothBee BaudRate 
  
  bt_buffer = b;
  bt_maxlen = maxlen;
  bt_callback = callback;
  btReset();
}
