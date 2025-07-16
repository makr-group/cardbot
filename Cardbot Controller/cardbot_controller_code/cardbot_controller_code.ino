// ╔══════════════════════════════════════════════════════════════════╗
// ║                                                                  ║
// ║     ██████╗ █████╗ ██████╗ ██████╗ ██████╗  ██████╗ ████████╗    ║
// ║    ██╔════╝██╔══██╗██╔══██╗██╔══██╗██╔══██╗██╔═══██╗╚══██╔══╝    ║
// ║    ██║     ███████║██████╔╝██║  ██║██████╔╝██║   ██║   ██║       ║
// ║    ██║     ██╔══██║██╔══██╗██║  ██║██╔══██╗██║   ██║   ██║       ║
// ║    ╚██████╗██║  ██║██║  ██║██████╔╝██████╔╝╚██████╔╝   ██║       ║
// ║     ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝ ╚═════╝  ╚═════╝    ╚═╝       ║
// ║                                                                  ║
// ╚══════════════════════════════════════════════════════════════════╝
//
//  █▄▄ █▄█   █▀▄▀█ ▄▀█ █▄▀ █▀█
//  █▄█ ░█░   █░▀░█ █▀█ █░█ █▀▄
//
//  Cardbot Controller Code
//  Version: 0.1.1
//  Date: 7/10/2025
//  Main controller code for cardbot
//  Carbot Version: 1.0
//  Controller Version: 1.0

#include <esp_now.h>
#include <WiFi.h>

#define UP_BUTTON_PIN 2
#define DOWN_BUTTON_PIN 3
#define LEFT_BUTTON_PIN 6
#define RIGHT_BUTTON_PIN 4

esp_now_peer_info_t peer_info;
uint8_t broadcast_address[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  // YOUR MAC ADDRESS HERE

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

//  █▄▄ █░█ ▀█▀ ▀█▀ █▀█ █▄░█   █▀ █▀▀ ▀█▀ █░█ █▀█
//  █▄█ █▄█ ░█░ ░█░ █▄█ █░▀█   ▄█ ██▄ ░█░ █▄█ █▀▀

typedef struct button {
  uint8_t pin;
  bool pressed;

  void setPin(uint8_t gpio_pin) {
    pin = gpio_pin;
    pinMode(pin, INPUT_PULLDOWN);
  }

  bool getValue() {
    pressed = digitalRead(pin);
    return pressed;
  }

} Button;

button up_button;
button down_button;
button left_button;
button right_button;

//  █▀▀ █▀█ █▄░█ ▀█▀ █▀█ █▀█ █░░ █░░ █▀▀ █▀█   █▀ █▀▀ ▀█▀ █░█ █▀█
//  █▄▄ █▄█ █░▀█ ░█░ █▀▄ █▄█ █▄▄ █▄▄ ██▄ █▀▄   ▄█ ██▄ ░█░ █▄█ █▀▀

typedef struct controller_message {
  int16_t joy_1_x_axis;
  int16_t joy_1_y_axis;
  bool up_state;
  bool down_state;
  bool left_state;
  bool right_state;
  uint16_t connection_check = 0;
};

controller_message controller;

void getControllerState() {
  controller.up_state = up_button.getValue();
  controller.down_state = down_button.getValue();
  controller.left_state = left_button.getValue();
  controller.right_state = right_button.getValue();
};

//  █▀ █▀▀ ▀█▀ █░█ █▀█
//  ▄█ ██▄ ░█░ █▄█ █▀▀

void setup() {

  Serial.begin(115200);

  right_button.setPin(RIGHT_BUTTON_PIN);
  left_button.setPin(LEFT_BUTTON_PIN);
  up_button.setPin(UP_BUTTON_PIN);
  down_button.setPin(DOWN_BUTTON_PIN);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onDataSent);  // This does nothign but exists.

  memcpy(peer_info.peer_addr, broadcast_address, 6);
  peer_info.channel = 0;
  peer_info.encrypt = false;

  if (esp_now_add_peer(&peer_info) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

//  █▀▄▀█ ▄▀█ █ █▄░█   █░░ █▀█ █▀█ █▀█
//  █░▀░█ █▀█ █ █░▀█   █▄▄ █▄█ █▄█ █▀▀

void loop() {
  static controller_message last_state;

  getControllerState();

  if (controller.down_state != last_state.down_state || controller.up_state != last_state.up_state || controller.left_state != last_state.left_state || controller.right_state != last_state.right_state) {

    esp_err_t result = esp_now_send(broadcast_address, (uint8_t *)&controller, sizeof(controller_message));

    if (result == ESP_OK) {
      Serial.println("Sending confirmed");
    } else {
      Serial.println("Sending error");
    }

    controller.connection_check = controller.connection_check + 1;

    last_state = controller;
  }

  static unsigned long current_millis;
  static unsigned long previous_millis;

  current_millis = millis();

  if (current_millis - previous_millis >= 100) {
    previous_millis = current_millis;

    controller.connection_check = controller.connection_check + 1;
    esp_now_send(broadcast_address, (uint8_t *)&controller, sizeof(controller_message));
  }
}
