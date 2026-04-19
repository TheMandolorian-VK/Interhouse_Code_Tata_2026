// Include libraries
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  WiFi.mode(WIFI_STA); // Must be in station mode
  esp_now_init();

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, broadcastAddress, 6);
  esp_now_add_peer(&peer);
    
}

void loop() {
  uint8_t code = 1; //placeholder value for now
  esp_now_send(broadcastAddress, &code, 1);
}
