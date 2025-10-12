
#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
 
// Create display object 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); 
 
void setup() { 
  // Initialize OLED 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    for (;;); // Stop if OLED not found 
  } 
 
  display.clearDisplay(); 
  drawSmiley(); 
  display.display(); 
} 
 
void loop() { 
  // Nothing needed here 
} 
 
void drawSmiley() { 
  int centerX = SCREEN_WIDTH / 2; 
  int centerY = SCREEN_HEIGHT / 2; 
  int faceRadius = 20; 
 
  // Face outline 
  display.drawCircle(centerX, centerY, faceRadius, SSD1306_WHITE); 
 
  // Eyes 
  display.fillCircle(centerX - 7, centerY - 5, 2, SSD1306_WHITE); 
  display.fillCircle(centerX + 7, centerY - 5, 2, SSD1306_WHITE); 
 
  // Smile (manually draw a curved line using pixels) 
  for (int x = -8; x <= 8; x++) { 
    int y = (x * x) / 10;  // Adjust curvature 
    display.drawPixel(centerX + x, centerY + 6 - y, SSD1306_WHITE); 
  } 
}