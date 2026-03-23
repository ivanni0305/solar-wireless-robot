#include <esp_now.h>
#include <WiFi.h>

#define AIN1 27
#define AIN2 26
#define PWMA 25
#define BIN1 33
#define BIN2 32
#define PWMB 14
#define STBY 13
#define TRIG 5
#define ECHO 18
#define SAFE_DIST 20
#define SPEED 150
#define TIMEOUT 300

typedef struct { int x; int y; } JoyData;
JoyData data;
unsigned long lastRecv = 0;

long getDist() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long d = pulseIn(ECHO, HIGH, 30000);
  return d ? d * 0.034 / 2 : 999;
}

void stop() { digitalWrite(AIN1,0);digitalWrite(AIN2,0);digitalWrite(BIN1,0);digitalWrite(BIN2,0); }
void fwd()  { digitalWrite(AIN1,0);digitalWrite(AIN2,1);analogWrite(PWMA,SPEED);digitalWrite(BIN1,0);digitalWrite(BIN2,1);analogWrite(PWMB,SPEED); }
void bwd()  { digitalWrite(AIN1,1);digitalWrite(AIN2,0);analogWrite(PWMA,SPEED);digitalWrite(BIN1,1);digitalWrite(BIN2,0);analogWrite(PWMB,SPEED); }
void left() { digitalWrite(AIN1,1);digitalWrite(AIN2,0);analogWrite(PWMA,SPEED);digitalWrite(BIN1,0);digitalWrite(BIN2,1);analogWrite(PWMB,SPEED); }
void right(){ digitalWrite(AIN1,0);digitalWrite(AIN2,1);analogWrite(PWMA,SPEED);digitalWrite(BIN1,1);digitalWrite(BIN2,0);analogWrite(PWMB,SPEED); }

void onRecv(const uint8_t *mac, const uint8_t *in, int len) {
  memcpy(&data, in, sizeof(data));
  lastRecv = millis();
  bool obs = getDist() < SAFE_DIST;
  if      (data.y < 1500) { obs ? stop() : fwd(); }
  else if (data.y > 2500) bwd();
  else if (data.x < 1500) left();
  else if (data.x > 2500) right();
  else stop();
}

void setup() {
  Serial.begin(115200);
  pinMode(AIN1,OUTPUT);pinMode(AIN2,OUTPUT);pinMode(PWMA,OUTPUT);
  pinMode(BIN1,OUTPUT);pinMode(BIN2,OUTPUT);pinMode(PWMB,OUTPUT);
  pinMode(STBY,OUTPUT);digitalWrite(STBY,1);
  pinMode(TRIG,OUTPUT);pinMode(ECHO,INPUT);
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onRecv);
  Serial.println("Ready");
}

void loop() { if(millis()-lastRecv>TIMEOUT) stop(); }