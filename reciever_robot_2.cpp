#include <esp_now.h>
#include <Wifi.h>

void onData(const uint_8 *data, const uint8_t *mac, int len){
  uint8_t cmd = data[0];
}
void setup(){
  Wifi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onData);
}

void loop{
  
}
