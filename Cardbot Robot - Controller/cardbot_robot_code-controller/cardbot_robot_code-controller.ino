#include <WiFi.h>
#include <esp_now.h>


#define LM_PIN_FORWARD 5
#define LM_PIN_BACKWARD 4
#define RM_PIN_FORWARD 3
#define RM_PIN_BACKWARD 2

uint16_t last_connection_check = 0;


// controller message set up
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

// motor setup
struct motor {
  uint8_t f_pin;
  uint8_t b_pin;
  bool moving;

  void forward() {
    digitalWrite(f_pin, HIGH);
    digitalWrite(b_pin, LOW);
    moving = true;
  }

  void backward() {
    digitalWrite(f_pin, LOW);
    digitalWrite(b_pin, HIGH);
    moving = true;
  }

  void stop() {
    digitalWrite(f_pin, LOW);
    digitalWrite(b_pin, LOW);
    moving = false;
  }

  void setup(uint8_t forward_pin, uint8_t backward_pin) {
    f_pin = forward_pin;
    b_pin = backward_pin;
    pinMode(forward_pin, OUTPUT);
    pinMode(backward_pin, OUTPUT);
  }
};

motor left_motor;
motor right_motor;

// direction control
void forwards() {
  left_motor.forward();
  right_motor.forward();
}
void backwards() {
  left_motor.backward();
  right_motor.backward();
}
void pivot_left() {
  left_motor.backward();
  right_motor.forward();
}
void pivot_right() {
  left_motor.forward();
  right_motor.backward();
}
void turnLeft_forward() {
  left_motor.stop();
  right_motor.forward();
}
void turnLeft_backward() {
  left_motor.stop();
  right_motor.backward();
}
void turnRight_forward() {
  left_motor.forward();
  right_motor.stop();
}
void turnRight_backward() {
  left_motor.backward();
  right_motor.stop();
}
void stop() {
  left_motor.stop();
  right_motor.stop();
}

// callback function on controller message
void robot_control(const esp_now_recv_info_t *info, const uint8_t *incoming_data, int len) {

  memcpy(&controller, incoming_data, sizeof(controller));

  if (controller.up_state && controller.left_state) {
    turnLeft_forward();
  } else if (controller.up_state && controller.right_state) {
    turnRight_forward();
  } else if (controller.down_state && controller.left_state) {
    turnLeft_backward();
  } else if (controller.down_state && controller.right_state) {
    turnRight_backward();
  } else if (controller.left_state) {
    pivot_left();
  } else if (controller.right_state) {
    pivot_right();
  } else if (controller.up_state) {
    forwards();
  } else if (controller.down_state) {
    backwards();
  } else {
    stop();
  }
}

void setup() {

  Serial.begin(115200);

  left_motor.setup(LM_PIN_FORWARD, LM_PIN_BACKWARD);
  right_motor.setup(RM_PIN_FORWARD, RM_PIN_BACKWARD);

  forwards();
  delay(10);
  backwards();
  delay(10);
  stop();

  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    return;
  }

  // Register callback function
  esp_now_register_recv_cb(robot_control);
}

void loop() {
  static unsigned long currentMillis;
  static unsigned long previousMillis = 0;

  currentMillis = millis();
// checks for controller connections 
  if (currentMillis - previousMillis >= 200) {
    previousMillis = currentMillis;

    if (controller.connection_check == last_connection_check) {
      controller.down_state = false;
      controller.up_state = false;
      controller.left_state = false;
      controller.right_state = false;
      stop();
    }

    Serial.println(last_connection_check);
    Serial.println(controller.connection_check);


    last_connection_check = controller.connection_check;
  }
}
