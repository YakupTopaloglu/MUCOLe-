#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
  char text[250];
} struct_message;

struct_message incomingMessage;

void onDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memset(&incomingMessage, 0, sizeof(incomingMessage));
  memcpy(&incomingMessage, incomingData, min((int)sizeof(incomingMessage), len));

  Serial.print("Gelen mesaj: ");
  Serial.println(incomingMessage.text);

  if (info != nullptr) {
    Serial.print("Gonderen MAC: ");
    for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", info->src_addr[i]);
      if (i < 5) Serial.print(":");
    }
    Serial.println();
  }

  Serial.println("------");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);

  Serial.print("Alici MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW baslatilamadi");
    return;
  }

  esp_now_register_recv_cb(onDataRecv);
  Serial.println("Alici hazir.");
}

void loop() {
}