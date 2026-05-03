#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

int out1 = 22, out2 = 21, enA = 23, out3 = 19, out4 = 18, enB = 15, lIR = 13, rIR = 12;

// Replace with your network credentials
const char* ssid = "Tata_House";
const char* password = "trueassteel";

// Global Variables to store GUI states
String currentCommand = "stop";
int linearPWM = 160;
int turnPWM = 140;
int enAspeed, enBspeed;

int currentMode = 0; // 0=Manual, 1=Line Follower, 2=Obstacle
int freq = 10000;
int res = 8;

AsyncWebServer server(80);

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



// Paste your full HTML code inside this R"rawliteral(...)rawliteral" block
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tata House Bot Control</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;700;800&display=swap" rel="stylesheet">
    <style>
        :root {
            --primary-bg: #9e2a0d;
            --glass-bg: rgba(255, 255, 255, 0.1);
            --accent: #ffffff;
            --danger: #ff4b2b;
            --btn-idle: rgba(0, 0, 0, 0.3);
        }

        body { 
            font-family: 'Inter', sans-serif; 
            text-align: center; 
            background-color: var(--primary-bg); 
            color: white;
            margin: 0;
            padding: 20px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            min-height: 100vh;
            user-select: none;
        }

        h1 { 
            font-weight: 800;
            text-transform: uppercase;
            letter-spacing: 4px;
            margin-bottom: 30px;
            font-size: 1.8rem;
            text-shadow: 0 4px 10px rgba(0,0,0,0.2);
        }

        .controls-container {
            background: var(--glass-bg);
            backdrop-filter: blur(10px);
            -webkit-backdrop-filter: blur(10px);
            padding: 40px;
            border-radius: 40px;
            border: 1px solid rgba(255, 255, 255, 0.2);
            box-shadow: 0 20px 50px rgba(0,0,0,0.3);
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .grid-container { 
            display: grid; 
            grid-template-columns: repeat(3, 80px); 
            grid-template-rows: repeat(3, 80px);
            gap: 15px; 
            margin-bottom: 30px;
        }

        button { 
            width: 80px;
            height: 80px;
            font-size: 22px; 
            border-radius: 50%; 
            border: none; 
            cursor: pointer; 
            background: var(--btn-idle); 
            color: white; 
            transition: all 0.2s ease;
            display: flex;
            align-items: center;
            justify-content: center;
            outline: none;
        }

        button:hover { background: rgba(255,255,255,0.2); }
        
        button:active, .active-key { 
            background: white !important; 
            color: var(--primary-bg) !important;
            transform: scale(0.92);
            box-shadow: 0 0 20px rgba(255,255,255,0.4);
        }

        .stop { 
            background: var(--danger);
            font-size: 28px;
        }

        .slider-section {
            margin: 10px 0;
            width: 100%;
            max-width: 270px;
        }

        .slider-label {
            font-weight: 700;
            font-size: 11px;
            letter-spacing: 2px;
            margin-top: 15px;
            margin-bottom: 8px;
            display: block;
        }

        .slider { 
            -webkit-appearance: none;
            width: 100%; 
            height: 8px;
            border-radius: 10px;
            background: rgba(255,255,255,0.2);
            outline: none;
            margin-bottom: 5px;
        }

        .slider::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 22px;
            height: 22px;
            border-radius: 50%;
            background: white;
            cursor: pointer;
        }

        .mode-btn { 
            width: 100%;
            max-width: 270px;
            border-radius: 50px; 
            padding: 18px; 
            background: transparent; 
            font-weight: 800;
            font-size: 14px;
            border: 2px solid white;
            color: white;
            letter-spacing: 1.5px;
            margin-top: 25px;
            cursor: pointer;
        }
    </style>
</head>
<body>

    <h1>Tata House: Interhouse Bot GUI</h1>

    <div class="controls-container">
        <div class="grid-container">
            <div></div>
            <button id="btn-forward" onmousedown="sendCmd('forward')" onmouseup="sendCmd('stop')" ontouchstart="sendCmd('forward')" ontouchend="sendCmd('stop')">▲</button>
            <div></div>
            
            <button id="btn-left" onmousedown="sendCmd('left')" onmouseup="sendCmd('stop')" ontouchstart="sendCmd('left')" ontouchend="sendCmd('stop')">◀</button>
            <button id="btn-stop" class="stop" onclick="sendCmd('stop')">■</button>
            <button id="btn-right" onmousedown="sendCmd('right')" onmouseup="sendCmd('stop')" ontouchstart="sendCmd('right')" ontouchend="sendCmd('stop')">▶</button>
            
            <div></div>
            <button id="btn-backward" onmousedown="sendCmd('backward')" onmouseup="sendCmd('stop')" ontouchstart="sendCmd('backward')" ontouchend="sendCmd('stop')">▼</button>
            <div></div>
        </div>

        <div class="slider-section">
            <span class="slider-label">SPEED: <span id="speedVal">160</span></span><br><br>
            <input type="range" min="0" max="255" value="160" class="slider" oninput="updateSpeed(this.value)">
        </div>

        <button class="mode-btn" id="modeBtn" onclick="toggleMode()">MODE: MANUAL</button>
    </div>

    <script>
        let modes = ["MANUAL", "LINE FOLLOWER", "OBSTACLE DETECTION"];
        let currentModeIndex = 0;
        let activeKey = null;

        const keyMap = {
            "ArrowUp": { cmd: "forward", id: "btn-forward" },
            "ArrowDown": { cmd: "backward", id: "btn-backward" },
            "ArrowLeft": { cmd: "left", id: "btn-left" },
            "ArrowRight": { cmd: "right", id: "btn-right" },
            " ": { cmd: "stop", id: "btn-stop" }
        };

        function sendCmd(command) {
            fetch('/control?cmd=' + command).catch(() => {});
        }

        function updateSpeed(v) { 
            document.getElementById('speedVal').innerText = v; 
            fetch('/speed?val=' + v).catch(()=>{}); 
        }

        function toggleMode() {
            currentModeIndex = (currentModeIndex + 1) % modes.length;
            document.getElementById('modeBtn').innerText = "MODE: " + modes[currentModeIndex];
            fetch('/mode?val=' + currentModeIndex).catch(() => {});
        }

        window.addEventListener("keydown", (e) => {
            if (keyMap[e.key] && activeKey !== e.key) {
                e.preventDefault();
                activeKey = e.key;
                document.getElementById(keyMap[e.key].id).classList.add("active-key");
                sendCmd(keyMap[e.key].cmd);
            }
        });

        window.addEventListener("keyup", (e) => {
            if (keyMap[e.key] && activeKey === e.key) {
                activeKey = null;
                document.getElementById(keyMap[e.key].id).classList.remove("active-key");
                sendCmd("stop");
            }
        });
    </script>
</body>
</html>
)rawliteral";


void setup() {
  pinMode(lIR, INPUT);
  pinMode(rIR, INPUT);
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  pinMode(out4, OUTPUT);

  ledcAttach(enA, freq, res);
  ledcAttach(enB, freq, res);

  ledcWrite(enA, linearPWM);
  ledcWrite(enB, linearPWM);
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("cmd")) currentCommand = request->getParam("cmd")->value();
    request->send(200, "text/plain", "OK");
  });

  server.on("/speed", HTTP_GET, [](AsyncWebServerRequest *r){
    if(r->hasParam("val")) {
      linearPWM = r->getParam("val")->value().toInt();
      turnPWM = constrain(linearPWM - 30, 0, 255);
    }
    r->send(200);
  });
  
  server.on("/mode", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("val")) {
      currentMode = request->getParam("val")->value().toInt();
      stopMotors(); // Safety reset on mode change
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  // Use a switch case or if statements to separate your logic
  if (currentMode == 0) {
    if (currentCommand == "forward") {
      moveForward(linearPWM);
    } 
    else if (currentCommand == "backward") {
      moveBackward(linearPWM);
    }
    else if (currentCommand == "left"){
      turnLeft(turnPWM, "Manual");  
    }
    else if (currentCommand == "right"){
      turnRight(turnPWM, "Manual");  
    }
    else if(currentCommand == "stop"){
      stopMotors();
    }
  } 
  else if (currentMode == 1) {
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
       moveForward(linearPWM);  
      }
    }
  } 
  else if (currentMode == 2) {
    // please do replace this with ultrasonic code
    stopMotors();
  }

}
