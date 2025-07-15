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
//  Cardbot Robot Code - Controller
//  Version: 0.1.1
//  Date: 7/8/2025
//  Cardbot code for autonomous mode and controllers
//  Carbot Version: 1.2
//  Controller Version: 1.0

#define LM_PIN_FORWARD 5
#define LM_PIN_BACKWARD 4
#define RM_PIN_FORWARD 3
#define RM_PIN_BACKWARD 2

#define BUTTON_PIN 20

#include <WiFi.h>
#include <esp_now.h>

// Global Varibales
bool automonus_active = false;
uint16_t last_connection_check = 0;

//  █▀▄▀█ █▀█ ▀█▀ █▀█ █▀█   █▀ █▀▀ ▀█▀ █░█ █▀█
//  █░▀░█ █▄█ ░█░ █▄█ █▀▄   ▄█ ██▄ ░█░ █▄█ █▀▀

typedef struct motor {
  // Controls the motor's basic movements
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

//  █▀█ █▀█ █▄▄ █▀█ ▀█▀   █▀ █▀▀ ▀█▀ █░█ █▀█
//  █▀▄ █▄█ █▄█ █▄█ ░█░   ▄█ ██▄ ░█░ █▄█ █▀▀

void stop(int time = 0) {
  left_motor.stop();
  right_motor.stop();
  delay(time);
}
void forwards(int time = 0) {
  left_motor.forward();
  right_motor.forward();
  delay(time);
  if (time > 0) {
    delay(100);
  }
}
void backwards(int time = 0) {
  left_motor.backward();
  right_motor.backward();
  delay(time);
  if (time > 0) {
    delay(100);
  }
}
void pivot_left(int time = 0) {
  left_motor.backward();
  right_motor.forward();
  delay(time);
  if (time > 0) {
    delay(100);
  }
}
void pivot_right(int time = 0) {
  left_motor.forward();
  right_motor.backward();
  delay(time);
  if (time > 0) {
    delay(100);
  }
}
void turnLeft_forward(int time = 0) {
  left_motor.stop();
  right_motor.forward();
  delay(time);
  if (time > 0) {
    delay(100);
  }
}
void turnLeft_backward(int time = 0) {
  left_motor.stop();
  right_motor.backward();
  delay(time);
  if (time > 0) {
    delay(100);
  }
}
void turnRight_forward(int time = 0) {
  left_motor.forward();
  right_motor.stop();
  delay(time);
  if (time > 0) {
    delay(50);
  }
}
void turnRight_backward(int time = 0) {
  left_motor.backward();
  right_motor.stop();
  delay(time);
  if (time > 0) {
    delay(50);
  }
}

//  █▀▀ █▀█ █▄░█ ▀█▀ █▀█ █▀█ █░░ █░░ █▀▀ █▀█   █▀ █▀▀ ▀█▀ █░█ █▀█
//  █▄▄ █▄█ █░▀█ ░█░ █▀▄ █▄█ █▄▄ █▄▄ ██▄ █▀▄   ▄█ ██▄ ░█░ █▄█ █▀▀

typedef struct controller_message {
  // message structure between
  int16_t joy_1_x_axis;
  int16_t joy_1_y_axis;
  bool up_state;
  bool down_state;
  bool left_state;
  bool right_state;
  uint16_t connection_check = 0;
};

controller_message controller;

// callback function on controller message
void controller_mode(const esp_now_recv_info_t *info, const uint8_t *incoming_data, int len) {

  memcpy(&controller, incoming_data, sizeof(controller));
  if (!automonus_active) {  
    // controller will be locked out in while in automonus mode 
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
}

//  ▄▀█ █░█ ▀█▀ █▀█ █▀▄▀█ █▀█ █▄░█ █░█ █▀   █▀▄▀█ █▀█ █▀▄ █▀▀
//  █▀█ █▄█ ░█░ █▄█ █░▀░█ █▄█ █░▀█ █▄█ ▄█   █░▀░█ █▄█ █▄▀ ██▄

// automonus code to run on button press
void automonus_mode() {
  // atonmonus code here
}

//  █▀ █▀▀ ▀█▀ █░█ █▀█
//  ▄█ ██▄ ░█░ █▄█ █▀▀

void setup() {
  left_motor.setup(LM_PIN_FORWARD, LM_PIN_BACKWARD);
  right_motor.setup(RM_PIN_FORWARD, RM_PIN_BACKWARD);

  pinMode(BUTTON_PIN, INPUT_PULLDOWN);

  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    return;
  }

  // Register callback function
  esp_now_register_recv_cb(controller_mode);
}

//  █▀▄▀█ ▄▀█ █ █▄░█   █░░ █▀█ █▀█ █▀█
//  █░▀░█ █▀█ █ █░▀█   █▄▄ █▄█ █▄█ █▀▀

// main control loop checks for button press and controller disconnect
void loop() {
  static unsigned long currentMillis;
  static unsigned long previousMillis = 0;

  //activates automonus_mode on button press
  if (digitalRead(BUTTON_PIN)) {
    automonus_active = true;  // locks out controller
    delay(1000);              // 1 sec delay befor start
    automonus_mode();
    automonus_active = false;
  }

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

    last_connection_check = controller.connection_check;
  }
}
