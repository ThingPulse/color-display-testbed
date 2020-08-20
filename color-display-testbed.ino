#include <Arduino.h>
#include <SPI.h>
#include <ESP8266WiFi.h>

#include <XPT2046_Touchscreen.h>
#include "TouchControllerWS.h"

#include <MiniGrafx.h>
#include <ILI9341_SPI.h>

#include "ArialRounded.h"
#include "icons.h"

// Pins for the ILI9341
#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8
#define TOUCH_CS D3
#define TOUCH_IRQ  D4

#define MINI_BLACK 0
#define MINI_WHITE 1
#define MINI_YELLOW 2
#define MINI_BLUE 3

// defines the colors usable in the paletted 16 color frame buffer
uint16_t palette[] = {ILI9341_BLACK,  // 0
                      ILI9341_WHITE,  // 1
                      ILI9341_YELLOW, // 2
                      0x7E3C          // 3
                      };

int SCREEN_WIDTH = 240;
int SCREEN_HEIGHT = 320;
// Limited to 4 colors due to memory constraints
int BITS_PER_PIXEL = 2; // 2^2 =  4 colors

ADC_MODE(ADC_VCC);

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);

XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);
TouchControllerWS touchController(&ts);

void drawLabelValue(uint8_t line, String label, String value);
void drawAbout();

bool isAboutScreenShowing;

void setup() {
  Serial.begin(115200);

  // The LED pin needs to set HIGH
  // Use this pin to save energy
  // Turn on the background LED
  Serial.println(TFT_LED);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);    // HIGH to Turn on;

  Serial.println("Initializing touch screen");
  ts.begin();

  gfx.init();
  drawAbout();
  gfx.commit();
  isAboutScreenShowing = true;
}

void loop() {
  bool changeScreen = false;
  
  gfx.fillBuffer(MINI_BLACK);
  if (touchController.isTouched(0)) {
    Serial.println("Touched");
    changeScreen = true;
  }

  if (changeScreen) {
    Serial.println("Screen changed");
    if (isAboutScreenShowing) {
      drawTestSuccess();
      isAboutScreenShowing = false;
    } else {
      drawAbout();
      isAboutScreenShowing = true;
    }
    gfx.commit();
  }
}

void drawAbout() {
  drawLogo();

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  drawLabelValue(7, "Heap Mem:", String(ESP.getFreeHeap() / 1024)+"kb");
  drawLabelValue(8, "Flash Mem:", String(ESP.getFlashChipRealSize() / 1024 / 1024) + "MB");
  drawLabelValue(9, "WiFi Strength:", String(WiFi.RSSI()) + "dB");
  drawLabelValue(10, "Chip ID:", String(ESP.getChipId()));
  drawLabelValue(11, "VCC: ", String(ESP.getVcc() / 1024.0) +"V");
  drawLabelValue(12, "CPU Freq.: ", String(ESP.getCpuFreqMHz()) + "MHz");
  char time_str[15];
  const uint32_t millis_in_day = 1000 * 60 * 60 * 24;
  const uint32_t millis_in_hour = 1000 * 60 * 60;
  const uint32_t millis_in_minute = 1000 * 60;
  uint8_t days = millis() / (millis_in_day);
  uint8_t hours = (millis() - (days * millis_in_day)) / millis_in_hour;
  uint8_t minutes = (millis() - (days * millis_in_day) - (hours * millis_in_hour)) / millis_in_minute;
  sprintf(time_str, "%2dd%2dh%2dm", days, hours, minutes);
  drawLabelValue(13, "Uptime: ", time_str);
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setColor(MINI_YELLOW);
  gfx.drawString(15, 250, "Last Reset: ");
  gfx.setColor(MINI_WHITE);
  gfx.drawStringMaxWidth(15, 265, 240 - 2 * 15, ESP.getResetInfo());
}


void drawTestSuccess() {
  drawLogo();

  drawLabelValue(7, "Touch test: ", "successful");
  drawLabelValue(9, "Touch test: ", "successful");
  drawLabelValue(11, "Touch test: ", "successful");
}

void drawLogo() {
  gfx.fillBuffer(MINI_BLACK);
  gfx.drawPalettedBitmapFromPgm(20, 5, ThingPulseLogo);

  gfx.setFont(ArialRoundedMTBold_14);
  gfx.setTextAlignment(TEXT_ALIGN_CENTER);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(120, 90, "https://thingpulse.com");  
}

void drawLabelValue(uint8_t line, String label, String value) {
  const uint8_t labelX = 15;
  const uint8_t valueX = 150;
  gfx.setTextAlignment(TEXT_ALIGN_LEFT);
  gfx.setColor(MINI_YELLOW);
  gfx.drawString(labelX, 30 + line * 15, label);
  gfx.setColor(MINI_WHITE);
  gfx.drawString(valueX, 30 + line * 15, value);
}
