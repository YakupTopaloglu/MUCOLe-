#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
  char text[250];
} struct_message;

struct_message outgoingMessage;

uint8_t receiverMac[] = {0xD8, 0x13, 0x2A, 0x7F, 0x6A, 0xF8};

void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Gonderim durumu: ");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Basarili");
  } else {
    Serial.println("Basarisiz");
  }

  if (info != nullptr) {
    Serial.print("Hedef MAC: ");
    for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", info->des_addr[i]);
      if (i < 5) Serial.print(":");
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);

  Serial.print("Gonderici MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW baslatilamadi");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Peer eklenemedi");
    return;
  }

  Serial.println("Gonderici hazir.");
  Serial.println("Seri monitore mesaj yaz ve enter'a bas.");
}

void loop() {
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();

    if (msg.length() > 0) {
      memset(&outgoingMessage, 0, sizeof(outgoingMessage));
      msg.toCharArray(outgoingMessage.text, sizeof(outgoingMessage.text));

      esp_err_t result = esp_now_send(receiverMac, (uint8_t *)&outgoingMessage, sizeof(outgoingMessage));

      if (result == ESP_OK) {
        Serial.print("Gonderildi: ");
        Serial.println(outgoingMessage.text);
      } else {
        Serial.print("Gonderme hatasi, kod: ");
        Serial.println(result);
      }
    }
  }
}