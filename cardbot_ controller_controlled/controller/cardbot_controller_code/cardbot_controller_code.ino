#include <esp_now.h>
#include <WiFi.h>

#define UP_BUTTON_PIN 2
#define DOWN_BUTTON_PIN 3
#define LEFT_BUTTON_PIN 6
#define RIGHT_BUTTON_PIN 4

esp_now_peer_info_t peer_info;
uint8_t broadcast_address[] = {YOUR ROBOT MAC ADDRESS HERE};

void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

//Data Stucture
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

typedef struct Button {
  uint8_t pin;
  bool pressed;

  void set_pin(uint8_t gpio_pin) {
    pin = gpio_pin;
    pinMode(pin, INPUT_PULLDOWN);
  }

  bool get_value() {
    pressed = digitalRead(pin);
    return pressed;
  }

} Button;

Button up_button;
Button down_button;
Button left_button;
Button right_button;

void get_controller_state() {
  controller.up_state = up_button.get_value();
  controller.down_state = down_button.get_value();
  controller.left_state = left_button.get_value();
  controller.right_state = right_button.get_value();
};


void setup() {

  Serial.begin(115200);

  right_button.set_pin(RIGHT_BUTTON_PIN);
  left_button.set_pin(LEFT_BUTTON_PIN);
  up_button.set_pin(UP_BUTTON_PIN);
  down_button.set_pin(DOWN_BUTTON_PIN);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(on_data_sent);  // This does nothign but exists.

  memcpy(peer_info.peer_addr, broadcast_address, 6);
  peer_info.channel = 0;
  peer_info.encrypt = false;

  if (esp_now_add_peer(&peer_info) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  static controller_message last_state;

  get_controller_state();

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
