#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "index.h"

// ── WiFi Credentials ──────────────────────────────────────────────
const char* ssid     = "Tata_House";
const char* password = "trueassteel";

// ── Server & WebSocket ────────────────────────────────────────────
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ── Bot State Variables ───────────────────────────────────────────
String botCommand = "stop";   // "forward" | "backward" | "left" | "right" | "stop"
int    botPWM     = 128;      // 0–255
String botMode    = "Manual"; // "Manual" | "Line Following" | "Obstacle Detection"

int out1 = 22, out2 = 21, enA = 23, out3 = 19, out4 = 18, enB = 15, lIR = 13, rIR = 12;

int freq = 10000;
int res = 8;

int enAspeed, enBspeed;

int linearPWM = 160;
int turnPWM = 140;
int lastTurn = 0;

void moveForward(int speedM) {
  enBspeed = speedM;
  enAspeed = speedM;

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  digitalWrite(out1, HIGH); digitalWrite(out2, LOW);
  digitalWrite(out3, HIGH); digitalWrite(out4, LOW);
}
void turnLeft(int speedM, String mode) {
  enBspeed = speedM;
  enAspeed = speedM;

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  if(mode == "LF"){
    digitalWrite(out1, LOW); digitalWrite(out2, HIGH);
    digitalWrite(out3, HIGH); digitalWrite(out4, LOW);
  }else if(mode == "Manual"){
    digitalWrite(out1, LOW); digitalWrite(out2, LOW);
    digitalWrite(out3, HIGH); digitalWrite(out4, LOW);  
  }
}
void turnRight(int speedM, String mode) {
  enBspeed = speedM;
  enAspeed = speedM;
  
  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  if(mode == "LF"){
    digitalWrite(out1, HIGH); digitalWrite(out2, LOW);
    digitalWrite(out3, LOW); digitalWrite(out4, HIGH);
  }else if(mode == "Manual"){
    digitalWrite(out1, HIGH); digitalWrite(out2, LOW);
    digitalWrite(out3, LOW); digitalWrite(out4, LOW);  
  }
}

void moveBackward(int speedM) {
  enBspeed = speedM;
  enAspeed = speedM;

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);

  digitalWrite(out1, LOW); digitalWrite(out2, HIGH);
  digitalWrite(out3, LOW); digitalWrite(out4, HIGH);
}

void stopMotors() {
  ledcWrite(enA, 0);
  ledcWrite(enB, 0);
  
  digitalWrite(out1, LOW); digitalWrite(out2, LOW);
  digitalWrite(out3, LOW); digitalWrite(out4, LOW);
}


// ── WebSocket Event Handler ───────────────────────────────────────
void onWsEvent(AsyncWebSocket*       server,
               AsyncWebSocketClient* client,
               AwsEventType          type,
               void*                 arg,
               uint8_t*              data,
               size_t                len)
{
  switch (type) {

    case WS_EVT_CONNECT:
      Serial.printf("[WS] Client #%u connected from %s\n",
                    client->id(),
                    client->remoteIP().toString().c_str());
      break;

    case WS_EVT_DISCONNECT:
      Serial.printf("[WS] Client #%u disconnected\n", client->id());
      break;

    case WS_EVT_DATA: {
      AwsFrameInfo* info = (AwsFrameInfo*)arg;

      // Only handle complete, text frames
      if (info->final && info->index == 0 &&
          info->len == len && info->opcode == WS_TEXT)
      {
        // Null-terminate the buffer
        char buf[len + 1];
        memcpy(buf, data, len);
        buf[len] = '\0';

        Serial.printf("[WS] Received: %s\n", buf);

        // ── Parse JSON ──
        StaticJsonDocument<256> doc;
        DeserializationError err = deserializeJson(doc, buf);

        if (!err) {
          // Update mode
          if (doc.containsKey("mode")) {
            botMode = doc["mode"].as<String>();
            Serial.printf("  Mode    → %s\n", botMode.c_str());
          }

          // Update PWM
          if (doc.containsKey("pwm")) {
            botPWM = doc["pwm"].as<int>();
            botPWM = constrain(botPWM, 0, 255);
            Serial.printf("  PWM     → %d\n", botPWM);
          }

          // Update command (only meaningful in Manual mode)
          if (doc.containsKey("cmd")) {
            botCommand = doc["cmd"].as<String>();
            Serial.printf("  Command → %s\n", botCommand.c_str());
          }

          // ── Acknowledge back to client ──
          StaticJsonDocument<128> ack;
          ack["status"] = "ok";
          ack["mode"]   = botMode;
          ack["pwm"]    = botPWM;
          ack["cmd"]    = botCommand;

          String ackStr;
          serializeJson(ack, ackStr);
          client->text(ackStr);

        } else {
          Serial.printf("  JSON parse error: %s\n", err.c_str());
        }
      }
      break;
    }

    case WS_EVT_ERROR:
      Serial.printf("[WS] Error from client #%u\n", client->id());
      break;

    default:
      break;
  }
}

// ── Setup ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(lIR, INPUT);
  pinMode(rIR, INPUT);
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);

  ledcAttach(enA, freq, res);
  ledcAttach(enB, freq, res);

  ledcWrite(enA, enAspeed);
  ledcWrite(enB, enBspeed);
  Serial.println("\n\n=== Bot WebSocket Controller ===");

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Register WebSocket handler
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Serve the main HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  // 404 handler
  server.onNotFound([](AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("HTTP server started.");
  Serial.println("Open browser at: http://" + WiFi.localIP().toString());
}

void applyManualControl(){
	Serial.println("Manual");
}

void runObstacleDetection(){
	Serial.println("OD");
}

// ── Loop ──────────────────────────────────────────────────────────
void loop() {
  // Clean up disconnected WebSocket clients periodically
  ws.cleanupClients();

  if (botMode == "Manual") {
    applyManualControl();
  }else if (botMode == "Line Following") {
      int sLread = digitalRead(lIR);
	  int sRread = digitalRead(rIR);
	
	  if(sLread == LOW && sRread == LOW){
	    moveForward(linearPWM);
	  }
	  else if(sLread == HIGH && sRread == LOW){
	    lastTurn = 1;
	    turnLeft(turnPWM, "LF");
	  }
	  else if(sLread == LOW && sRread == HIGH){
	    lastTurn = 2;
	    turnRight(turnPWM, "LF");
	  }
	  else if(sLread == HIGH && sRread == HIGH){
	    if(lastTurn == 1){
	      turnLeft(turnPWM, "LF");
	      delay(10);
	    }
	    else if(lastTurn == 2){
	      turnRight(turnPWM, "LF");
	      delay(10);
	    }
	    else{
	     moveBackward(linearPWM);  
	    }
	  }
  }else if (botMode == "Obstacle Detection") {
    runObstacleDetection();
  }

  delay(10);
}
