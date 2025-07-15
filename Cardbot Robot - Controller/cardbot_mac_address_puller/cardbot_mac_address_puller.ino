//  Cardbot Mac Address Puller
//  by Makr
//  Version: 0.1.0
//  Dates 7/14/2025
//  Cardbot Version: All ESP32 based
//  Controller Version: All ESP32 based

#include <WiFi.h>
#include <esp_wifi.h>

uint8_t mac_address[6];

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.STA.begin();

  esp_wifi_get_mac(WIFI_IF_STA, mac_address);

  delay(1000);

  // Plain text mac address
  Serial.print("Mac Address:   ");
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                mac_address[0], mac_address[1], mac_address[2],
                mac_address[3], mac_address[4], mac_address[5]);
  // Array Format for use with ESP_NOW
  Serial.print("Array Format:  ");
  Serial.printf("{0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x}\n",
                mac_address[0], mac_address[1], mac_address[2],
                mac_address[3], mac_address[4], mac_address[5]);
}

void loop() {
  // put your main code here, to run repeatedly:
}
