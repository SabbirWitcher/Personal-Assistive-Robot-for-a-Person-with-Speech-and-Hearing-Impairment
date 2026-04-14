#include <WiFi.h>
#include <WebServer.h>

// =====================
// PIN MAPPING (ESP32-S3)
// =====================
static const int FR_PWM = 16; static const int FR_IN1 = 7;  static const int FR_IN2 = 15;
static const int FL_PWM = 4;  static const int FL_IN1 = 6;  static const int FL_IN2 = 5;
static const int BL_PWM = 39; static const int BL_IN1 = 41; static const int BL_IN2 = 40;
static const int BR_PWM = 1;  static const int BR_IN1 = 42; static const int BR_IN2 = 2;

const int freq = 5000;
const int resolution = 8;
const int motorSpeed = 200; // 0-255

WebServer server(80);

void setMotor(int pin, int in1, int in2, int v1, int v2, int spd) {
  ledcWrite(pin, spd);
  digitalWrite(in1, v1);
  digitalWrite(in2, v2);
}

void stopMotors() {
  ledcWrite(FR_PWM, 0); ledcWrite(FL_PWM, 0); ledcWrite(BL_PWM, 0); ledcWrite(BR_PWM, 0);
  digitalWrite(FR_IN1, LOW); digitalWrite(FR_IN2, LOW);
  digitalWrite(FL_IN1, LOW); digitalWrite(FL_IN2, LOW);
  digitalWrite(BL_IN1, LOW); digitalWrite(BL_IN2, LOW);
  digitalWrite(BR_IN1, LOW); digitalWrite(BR_IN2, LOW);
}

void handleMove() {
  if (server.hasArg("dir")) {
    char cmd = server.arg("dir")[0];
    Serial.print("Command received: "); Serial.println(cmd);

    if (cmd == 'F') { // Forward
      setMotor(FR_PWM, FR_IN1, FR_IN2, HIGH, LOW, motorSpeed);
      setMotor(FL_PWM, FL_IN1, FL_IN2, HIGH, LOW, motorSpeed);
      setMotor(BL_PWM, BL_IN1, BL_IN2, HIGH, LOW, motorSpeed);
      setMotor(BR_PWM, BR_IN1, BR_IN2, HIGH, LOW, motorSpeed);
    } 
    else if (cmd == 'B') { // Backward
      setMotor(FR_PWM, FR_IN1, FR_IN2, LOW, HIGH, motorSpeed);
      setMotor(FL_PWM, FL_IN1, FL_IN2, LOW, HIGH, motorSpeed);
      setMotor(BL_PWM, BL_IN1, BL_IN2, LOW, HIGH, motorSpeed);
      setMotor(BR_PWM, BR_IN1, BR_IN2, LOW, HIGH, motorSpeed);
    }
    else if (cmd == 'R') { // Strafe Right
      setMotor(FR_PWM, FR_IN1, FR_IN2, LOW, HIGH, motorSpeed);
      setMotor(FL_PWM, FL_IN1, FL_IN2, HIGH, LOW, motorSpeed);
      setMotor(BL_PWM, BL_IN1, BL_IN2, LOW, HIGH, motorSpeed);
      setMotor(BR_PWM, BR_IN1, BR_IN2, HIGH, LOW, motorSpeed);
    }
    else if (cmd == 'L') { // Strafe Left
      setMotor(FR_PWM, FR_IN1, FR_IN2, HIGH, LOW, motorSpeed);
      setMotor(FL_PWM, FL_IN1, FL_IN2, LOW, HIGH, motorSpeed);
      setMotor(BL_PWM, BL_IN1, BL_IN2, HIGH, LOW, motorSpeed);
      setMotor(BR_PWM, BR_IN1, BR_IN2, LOW, HIGH, motorSpeed);
    }
    else if (cmd == 'T') { // Search (Rotate)
      setMotor(FR_PWM, FR_IN1, FR_IN2, LOW, HIGH, motorSpeed);
      setMotor(FL_PWM, FL_IN1, FL_IN2, HIGH, LOW, motorSpeed);
      setMotor(BL_PWM, BL_IN1, BL_IN2, HIGH, LOW, motorSpeed);
      setMotor(BR_PWM, BR_IN1, BR_IN2, LOW, HIGH, motorSpeed);
    }
    else { stopMotors(); }
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Update with your home Wi-Fi
  WiFi.begin("SSID", "PASS");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  ledcAttach(FR_PWM, freq, resolution);
  ledcAttach(FL_PWM, freq, resolution);
  ledcAttach(BL_PWM, freq, resolution);
  ledcAttach(BR_PWM, freq, resolution);

  pinMode(FR_IN1, OUTPUT); pinMode(FR_IN2, OUTPUT);
  pinMode(FL_IN1, OUTPUT); pinMode(FL_IN2, OUTPUT);
  pinMode(BL_IN1, OUTPUT); pinMode(BL_IN2, OUTPUT);
  pinMode(BR_IN1, OUTPUT); pinMode(BR_IN2, OUTPUT);

  server.on("/move", handleMove);
  server.begin();
}

void loop() {
  server.handleClient();
}