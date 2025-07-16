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
//  Cardbot Robot Code - Phone
//  Version: 0.1.1
//  Date: 7/14/2025
//  Cardbot code for autonomous mode and controllers
//  Carbot Version: 1.0

#include <LittleFS.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>


#define LM_PIN_FORWARD 5
#define LM_PIN_BACKWARD 4
#define RM_PIN_FORWARD 3
#define RM_PIN_BACKWARD 2
#define HTTP_PORT 80

//-------------------------------------

const char CARDBOT_NAME[] = "NAME HERE";

//-------------------------------------

const char *CARDBOT_SSID = CARDBOT_NAME;
const char *CARDBOT_PASSWORD = "PASSWORD_HERE";
IPAddress subnet(255,255,255,0);

// input array
bool input_array[4];
// forward button, backward button, left Button, right Button

//  █▀▄▀█ █▀█ ▀█▀ █▀█ █▀█   █▀ █▀▀ ▀█▀ █░█ █▀█
//  █░▀░█ █▄█ ░█░ █▄█ █▀▄   ▄█ ██▄ ░█░ █▄█ █▀▀

struct motor
{
  uint8_t forward_pin;
  uint8_t backward_pin;
  bool moving;

  void forward()
  {
    digitalWrite(forward_pin, HIGH);
    digitalWrite(backward_pin, LOW);
    moving = true;
  }

  void backward()
  {
    digitalWrite(forward_pin, LOW);
    digitalWrite(backward_pin, HIGH);
    moving = true;
  }

  void stop()
  {
    digitalWrite(forward_pin, LOW);
    digitalWrite(backward_pin, LOW);
    moving = false;
  }
};

motor left_motor = {LM_PIN_FORWARD, LM_PIN_BACKWARD};
motor right_motor = {RM_PIN_FORWARD, RM_PIN_BACKWARD};

//  █▀█ █▀█ █▄▄ █▀█ ▀█▀   █▀ █▀▀ ▀█▀ █░█ █▀█
//  █▀▄ █▄█ █▄█ █▄█ ░█░   ▄█ ██▄ ░█░ █▄█ █▀▀

void forwards()
{
  left_motor.forward();
  right_motor.forward();
}
void backwards()
{
  left_motor.backward();
  right_motor.backward();
}
void pivotLeft()
{
  left_motor.backward();
  right_motor.forward();
}
void pivotRight()
{
  left_motor.forward();
  right_motor.backward();
}
void turnLeftForward()
{
  left_motor.stop();
  right_motor.forward();
}
void turnLeftBackward()
{
  left_motor.stop();
  right_motor.backward();
}
void turnRigthForward()
{
  left_motor.forward();
  right_motor.stop();
}
void turnRightBackward()
{
  left_motor.backward();
  right_motor.stop();
}
void stop()
{
  left_motor.stop();
  right_motor.stop();
}

//  █▀▀ █ █░░ █▀▀   █▀ █▄█ ▀█▀ █▀▀ █▀▄▀█   █▀ █▀▀ ▀█▀ █░█ █▀█
//  █▀░ █ █▄▄ ██▄   ▄█ ░█░ ░█░ ██▄ █░▀░█   ▄█ ██▄ ░█░ █▄█ █▀▀

void initLittleFS()
{
  if (!LittleFS.begin())
  {
    Serial.println("Cannot mount LittleFS volume...");
  }
}

//  █░█░█ █▀▀ █▄▄ █▀ █▀▀ █▀█ █░█ █▀▀ █▀█   █▀ █▀▀ ▀█▀ █░█ █▀█
//  ▀▄▀▄▀ ██▄ █▄█ ▄█ ██▄ █▀▄ ▀▄▀ ██▄ █▀▄   ▄█ ██▄ ░█░ █▄█ █▀▀

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

boolean moving()
{
  if (left_motor.moving || right_motor.moving)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void onRootRequest(AsyncWebServerRequest *request)
{
  request->send(LittleFS, "/index.html", "text/html", false);
}

void initWebServer()
{
  server.on("/", onRootRequest);
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.begin();
}

void initWiFi()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192,168,10,10), IPAddress(192,168,10,1), subnet);
  WiFi.softAP(CARDBOT_SSID, CARDBOT_PASSWORD);
  Serial.println(WiFi.softAPIP().toString().c_str());
}

void notifyClients(const char *type, const char *data)
{
  JsonDocument doc;
  doc["type"] = type;
  doc["data"] = data;
  char output[64];
  size_t len = serializeJson(doc, output);
  ws.textAll(output, len);
}

// converts local ip form type IPADDRESS to char
const char *ipChar()
{
  uint8_t ipArray[] = {WiFi.localIP()[0], '.', WiFi.localIP()[1], '.', WiFi.localIP()[2], '.', WiFi.localIP()[3]};
  const char *charIP = reinterpret_cast<const char *>(ipArray);
  return charIP;
}


//  █░█░█ █▀▀ █▄▄   █░█ ▄▀█ █▄░█ █▀▄ █░░ █▀▀ █▀█
//  ▀▄▀▄▀ ██▄ █▄█   █▀█ █▀█ █░▀█ █▄▀ █▄▄ ██▄ █▀▄

// handles action messages
// json format {"type": "action", "data":"(button action)"}
void actionHandler(const char *data)
{
  if (strcmp(data, "up") == 0)
  {
    input_array[0] = 1;
    Serial.println("Up");
  }
  else if (strcmp(data, "dp") == 0)
  {
    input_array[1] = 1;
    Serial.println("DP");
  }
  else if (strcmp(data, "lp") == 0)
  {
    input_array[2] = 1;
    Serial.println("LP");
  }
  else if (strcmp(data, "rp") == 0)
  {
    input_array[3] = 1;
    Serial.println("RP");
  }
  else if (strcmp(data, "ur") == 0)
  {
    input_array[0] = 0;
    Serial.println("UR");
  }
  else if (strcmp((char *)data, "dr") == 0)
  {
    input_array[1] = 0;
    Serial.println("DR");
  }
  else if (strcmp((char *)data, "lr") == 0)
  {
    input_array[2] = 0;
    Serial.println("LR");
  }
  else if (strcmp((char *)data, "rr") == 0)
  {
    input_array[3] = 0;
    Serial.println("RR");
  }
  else
  {
    input_array[3] = 0;
    input_array[2] = 0;
    input_array[1] = 0;
    input_array[0] = 0;
  }
}
// handles request messages
// json format {"type": "request", "data":"(request type)"}
void requestHandler(const char *data)
{
  if (strcmp(data, "ip"))
  {
    notifyClients("ip", ipChar());
    return;
  }
  else if (strcmp(data, "name"))
  {
    notifyClients("name", CARDBOT_NAME);
    return;
  }
  else
  {
    return;
  }
}
// controls the robot inturpets input array
void robotControl()
{
  static bool updateCheck[4];

  if (updateCheck[0] != input_array[0] or updateCheck[1] != input_array[1] or updateCheck[2] != input_array[2] or
      updateCheck[3] != input_array[3])
  {

    // Forwards
    if (input_array[0] == true && input_array[1] == false && input_array[2] == false && input_array[3] == false)
    {
      forwards();
      notifyClients("motor", "on");
      Serial.print("Forward");
    }
    // Backwards
    else if (input_array[0] == false && input_array[1] == true && input_array[2] == false && input_array[3] == false)
    {
      backwards();
      notifyClients("motor", "on");
      Serial.print("Backwards");
    }
    // Pivot left
    else if (input_array[0] == false && input_array[1] == false && input_array[2] == true && input_array[3] == false)
    {
      pivotLeft();
      notifyClients("motor", "on");
      Serial.print("Pivot Left");
    }
    // Pivot right
    else if (input_array[0] == false && input_array[1] == false && input_array[2] == false && input_array[3] == true)
    {
      pivotRight();
      notifyClients("motor", "on");
      Serial.print("Pivot Right");
    }
    // Turn left forward
    else if (input_array[0] == true && input_array[1] == false && input_array[2] == true && input_array[3] == false)
    {
      turnLeftForward();
      notifyClients("motor", "on");
      Serial.print("Turn Left Forward");
    }
    // Turn left backward
    else if (input_array[0] == false && input_array[1] == true && input_array[2] == true && input_array[3] == false)
    {
      turnLeftBackward();
      notifyClients("motor", "on");
      Serial.print("Turn Left Backwards");
    }
    // Turn right forwards
    else if (input_array[0] == true && input_array[1] == false && input_array[2] == false && input_array[3] == true)
    {
      turnRigthForward();
      notifyClients("motor", "on");
      Serial.print("Turn Right Forwards");
    }
    // Turn right backward
    else if (input_array[0] == false && input_array[1] == true && input_array[2] == false && input_array[3] == true)
    {
      turnRightBackward();
      notifyClients("motor", "on");
      Serial.print("Turn Right Backwards");
    }
    else
    {
      stop();
      notifyClients("motor", "off");
    }
    updateCheck[0] = input_array[0];
    updateCheck[1] = input_array[1];
    updateCheck[2] = input_array[2];
    updateCheck[3] = input_array[3];
  }
}
// message type handler
void messageTypeHandler(const char *type, JsonDocument doc)
{
  if (strcmp(type, "action") == 0)
  {
    actionHandler(doc["data"]);
    Serial.println("Action Triggered");
    return;
  }
  else if (strcmp(type, "request") == 0)
  {
    requestHandler(doc["data"]);
    Serial.println("Request Triggered");
    return;
  }
  else
  {
    return;
  }
}
// message handler
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  JsonDocument doc;
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  int typeInt;
  DeserializationError err = deserializeJson(doc, data);
  if (err)
  {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
    return;
  }
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {

    deserializeJson(doc, data);
    const char *type = doc["type"];
    Serial.println(type);
    messageTypeHandler(type, doc);
  }
}

// event handler
void onEvent(AsyncWebSocket *server,      
             AsyncWebSocketClient *client, 
             AwsEventType type,            
             void *arg,                    
             uint8_t *data,               
             size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    notifyClients("name", CARDBOT_NAME);
    notifyClients("motor", "off");
    stop();
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    stop();
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    stop();
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  pinMode(left_motor.forward_pin, OUTPUT);
  pinMode(left_motor.backward_pin, OUTPUT);
  pinMode(right_motor.forward_pin, OUTPUT);
  pinMode(right_motor.backward_pin, OUTPUT);

  Serial.begin(115200);
  delay(500);

  initLittleFS();
  initWiFi();
  initWebServer();
  initWebSocket();
}

void loop()
{
  ws.cleanupClients();
  robotControl();
}