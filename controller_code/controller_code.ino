#include <esp_now.h>
#include <WiFi.h>

uint8_t robotMAC[] = {0xF4, 0x2D, 0xC9, 0x6A, 0xA8, 0x6C};

#define VRX 34
#define VRY 35

typedef struct {
  int x;
  int y;
} JoystickData;

JoystickData data;
esp_now_peer_info_t peerInfo;

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_send_cb(onSent);
  memcpy(peerInfo.peer_addr, robotMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  Serial.println("Controller ready");
}

void loop() {
  data.x = analogRead(VRX);
  data.y = analogRead(VRY);
  esp_now_send(robotMAC, (uint8_t *)&data, sizeof(data));
  delay(20);
}