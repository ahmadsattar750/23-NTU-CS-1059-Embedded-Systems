#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_W 128
#define OLED_H 64
Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, -1);

//PINS
#define LED_PIN 17
#define BUTTON_PIN 27
#define BUZZER_PIN 16

bool isLedOn = false;
bool buttonPressed = false;
bool longPressTriggered = false;
unsigned long pressStartTime = 0;
const unsigned long longPressTime = 2000;

void showMessage(const char* text) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 25);
  display.println(text);
  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  showMessage("initialize");
}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);

  // Button pressed initially
  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true;
    pressStartTime = millis();
    longPressTriggered = false;
  }

  // Check for long press
  if (buttonState == LOW && buttonPressed && !longPressTriggered) {
    if (millis() - pressStartTime >= longPressTime) {
      showMessage("  --Buzzer ON--");
      tone(BUZZER_PIN, 1500);
      delay(500);
      noTone(BUZZER_PIN);
      longPressTriggered = true;
    }
  }

  // Button released
  if (buttonState == HIGH && buttonPressed) {
    if (!longPressTriggered) {
      isLedOn = !isLedOn;
      digitalWrite(LED_PIN, isLedOn);
      if (isLedOn) showMessage("  --LED ON--");
      else showMessage("  --LED OFF--");
    }
    buttonPressed = false;
    delay(250); // Debounce
  }
}
