#include <esp_now.h>
#include <WiFi.h>

void onData(const esp_now_recv_info *info, const uint8_t *data, int len) {
  uint8_t cmd = data[0];
  // 'info->src_addr' is now where the MAC address lives
}

void setup(){
  Wifi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onData);
}

void loop(){
  
}
