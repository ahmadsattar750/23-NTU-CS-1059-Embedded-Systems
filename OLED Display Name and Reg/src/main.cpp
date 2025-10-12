#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
 
// ---- OLED setup ---- 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_ADDR 0x3C 
 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); 
 
void setup() { 
  Wire.begin(21, 22); // ESP32 default I2C pins (SDA=21, SCL=22) 
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { 
    // If it fails, check wiring and address (0x3C/0x3D) 
    for (;;); 
  } 
 
  display.clearDisplay(); 
   
} 
 
void loop() {
  display.clearDisplay();  // Clear the screen

  // Draw a rectangle border
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);

  // Print roll number
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(22, 10);
  display.println("23-NTU-CS-1059");

  // Print name
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 26);
  display.println("AHMAD SATTAR");

  display.display();  // Push everything to the OLED
  delay(2000);        // Optional delay
}
