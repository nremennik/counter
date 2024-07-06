#include <Arduino.h>

#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#include "fonts.h"
#include "driver/touch_sensor.h"
#include "nvs_flash.h"
#include "nvs.h"

SSD1306Wire display(0x3c, 23, 22);
OLEDDisplayUi ui(&display);

void IRAM_ATTR handleReset(void);
void IRAM_ATTR handleSensor(void);

volatile int32_t counter = 0;

void frame1(OLEDDisplay *d, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  // d->drawFastImage(x, y, 128, 16, tabs[2]);
  d->setTextAlignment(TEXT_ALIGN_CENTER);
  d->setFont(Open_Sans_Regular_22);
  // d->setColor(WHITE);
  // d->fillRect(0, 0, 128, 64);
  // d->setColor(BLACK);

  char buffer[128];
  itoa(counter, buffer, 10);
  d->drawString(64 + x, 14 + y, buffer);
}
void frame2(OLEDDisplay *d, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
  // d->drawFastImage(x, y, 128, 16, tabs[2]);
  d->setTextAlignment(TEXT_ALIGN_CENTER);
  d->setFont(Open_Sans_Condensed_Light_16);
  // d->setColor(WHITE);
  // d->fillRect(0, 0, 128, 64);
  // d->setColor(BLACK);
  d->drawString(64 + x, 14 + y, "Intelligent\nSystems");
}

FrameCallback frames[] =
    {
        frame1,
        frame2};
int framesCount = 1;
nvs_handle_t hSensorValue;

void setup()
{
  // Serial.begin(3000000);
  Serial.begin(115200);

  Serial.println("\n\nStarting");

  nvs_flash_init();
  nvs_open("storage", NVS_READWRITE, &hSensorValue);
  nvs_get_i32(hSensorValue, "counter", (int32_t *)&counter);

  ui.setTargetFPS(30);
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(TOP);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  // ui.disableAllIndicators();

  // ui.disableAutoTransition();
  ui.setFrames(frames, framesCount);

  ui.init();

  display.flipScreenVertically();
  // display.setContrast(0, 0, 0);
  ui.switchToFrame(0);
  ui.update();

  // for (int i = 0; i <= 100; i += 10)
  // {
  //   display.setBrightness(i);
  //   delay(ui.update());
  //   delay(50);
  // }
  // display.setContrast(100);
  // delay(ui.update());

  // touch_pad_init();
  // touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V5);

  // touch_pad_config(TOUCH_PAD_NUM0, 0);
  // touch_pad_filter_start(10);

  pinMode(17, INPUT_PULLUP);
  attachInterrupt(17, handleReset, FALLING);
  pinMode(4, INPUT);
  attachInterrupt(4, handleSensor, FALLING);

  Serial.println("Started");
}

void loop()
{
  uint16_t sensor;
  // touch_pad_read_raw_data(TOUCH_PAD_NUM0, &sensor);
  // Serial.printf("sensor = %d\n", (int)sensor);
  int dv = ui.update();
  if (dv <= 0)
    dv = 1;
  nvs_set_i32(hSensorValue, "counter", (int32_t)counter);
  nvs_commit(hSensorValue);
  delay(dv);
}

void IRAM_ATTR handleReset(void)
{
  counter = 0;
}
void IRAM_ATTR handleSensor(void)
{
  counter++;
}
