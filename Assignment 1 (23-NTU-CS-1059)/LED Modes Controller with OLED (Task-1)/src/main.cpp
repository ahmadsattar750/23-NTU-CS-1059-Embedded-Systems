#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Screen & Display Settings ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Hardware Pin Assignments ---
#define LED_YELLOW_PIN 16
#define LED_BLUE_PIN   18
#define LED_CYAN_PIN   17
#define MODE_BTN_PIN   33
#define RESET_BTN_PIN  27

// --- LEDC PWM Configuration ---
#define PWM_CHAN_YELLOW 0
#define PWM_CHAN_BLUE   1
#define PWM_CHAN_CYAN   2
#define PWM_FREQ        4000
#define PWM_RESOLUTION  8

// --- Global Variables ---
hw_timer_t *blinkTimer = nullptr;
int currentMode = 0;
int blinkStep = 0;
bool prevModeBtnState = HIGH;
bool prevResetBtnState = HIGH;
unsigned long lastPressTime = 0;
const int debounceDelay = 600;
volatile unsigned long timerTick = 0;

// --- Interrupt Service Routine (ISR) for Timer ---
void IRAM_ATTR onTimerInterrupt() {
  timerTick++;
}

// --- Function to Update the OLED Display ---
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 0);
  display.println("LED PANEL");
  display.drawLine(0, 20, 127, 20, SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(10, 35);

  if (currentMode == 0) display.print("Mode: OFF");
  else if (currentMode == 1) display.print("Mode: Blink");
  else if (currentMode == 2) display.print("Mode: ON");
  else if (currentMode == 3) display.print("Mode: PWM");

  display.display();
}

// --- Setup Function ---
void setup() {
  Serial.begin(115200);

  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_CYAN_PIN, OUTPUT);
  pinMode(MODE_BTN_PIN, INPUT_PULLUP);
  pinMode(RESET_BTN_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  ledcSetup(PWM_CHAN_YELLOW, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHAN_BLUE, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHAN_CYAN, PWM_FREQ, PWM_RESOLUTION);

  ledcAttachPin(LED_YELLOW_PIN, PWM_CHAN_YELLOW);
  ledcAttachPin(LED_BLUE_PIN, PWM_CHAN_BLUE);
  ledcAttachPin(LED_CYAN_PIN, PWM_CHAN_CYAN);

  blinkTimer = timerBegin(0, 80, true);
  timerAttachInterrupt(blinkTimer, &onTimerInterrupt, true);
  timerAlarmWrite(blinkTimer, 1000000, true);
  timerAlarmEnable(blinkTimer);

  ledcWrite(PWM_CHAN_YELLOW, 0);
  ledcWrite(PWM_CHAN_BLUE, 0);
  ledcWrite(PWM_CHAN_CYAN, 0);
  updateDisplay();
}

// --- Main Loop ---
void loop() {
  bool modeBtnState = digitalRead(MODE_BTN_PIN);
  bool resetBtnState = digitalRead(RESET_BTN_PIN);

  if (millis() - lastPressTime > debounceDelay) {
    if (modeBtnState == LOW && prevModeBtnState == HIGH) {
      currentMode = (currentMode + 1) % 4;
      blinkStep = 0;
      updateDisplay();
      lastPressTime = millis();
    }
    if (resetBtnState == LOW && prevResetBtnState == HIGH) {
      currentMode = 0;
      blinkStep = 0;
      updateDisplay();
      lastPressTime = millis();
    }
  }
  prevModeBtnState = modeBtnState;
  prevResetBtnState = resetBtnState;

  if (currentMode == 0) {
    ledcWrite(PWM_CHAN_YELLOW, 0);
    ledcWrite(PWM_CHAN_BLUE, 0);
    ledcWrite(PWM_CHAN_CYAN, 0);
  } 
  else if (currentMode == 1) {
    static unsigned long lastTickHandled = 0;
    if (timerTick != lastTickHandled) {
      lastTickHandled = timerTick;
      blinkStep = (blinkStep + 1) % 3;
      if (blinkStep == 0) {
        ledcWrite(PWM_CHAN_YELLOW, 255);
        ledcWrite(PWM_CHAN_BLUE, 0);
        ledcWrite(PWM_CHAN_CYAN, 0);
      } else if (blinkStep == 1) {
        ledcWrite(PWM_CHAN_YELLOW, 0);
        ledcWrite(PWM_CHAN_BLUE, 255);
        ledcWrite(PWM_CHAN_CYAN, 0);
      } else {
        ledcWrite(PWM_CHAN_YELLOW, 0);
        ledcWrite(PWM_CHAN_BLUE, 0);
        ledcWrite(PWM_CHAN_CYAN, 255);
      }
    }
  } 
  else if (currentMode == 2) {
    ledcWrite(PWM_CHAN_YELLOW, 255);
    ledcWrite(PWM_CHAN_BLUE, 255);
    ledcWrite(PWM_CHAN_CYAN, 255);
  } 
  else if (currentMode == 3) {
    for (int dutyCycle = 0; dutyCycle <= 255 && currentMode == 3; dutyCycle++) {
      ledcWrite(PWM_CHAN_YELLOW, dutyCycle);
      ledcWrite(PWM_CHAN_BLUE, dutyCycle);
      ledcWrite(PWM_CHAN_CYAN, dutyCycle);
      delay(5);
      if (digitalRead(MODE_BTN_PIN) == LOW || digitalRead(RESET_BTN_PIN) == LOW) return;
    }
    for (int dutyCycle = 255; dutyCycle >= 0 && currentMode == 3; dutyCycle--) {
      ledcWrite(PWM_CHAN_YELLOW, dutyCycle);
      ledcWrite(PWM_CHAN_BLUE, dutyCycle);
      ledcWrite(PWM_CHAN_CYAN, dutyCycle);
      delay(5);
      if (digitalRead(MODE_BTN_PIN) == LOW || digitalRead(RESET_BTN_PIN) == LOW) return;
    }
  }
}