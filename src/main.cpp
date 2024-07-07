#include <Arduino.h>

#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#include "fonts.h"
#include "driver/touch_sensor.h"
#include "nvs_flash.h"
#include "nvs.h"

void IRAM_ATTR handleReset(void);
void IRAM_ATTR handleSensor(void);
void IRAM_ATTR handleDistanceSensor(void);

volatile int32_t counter = 0;

nvs_handle_t hSensorValue;

void initUi();
int updateUi();
void initDistanceSensor();

void setup()
{
  // Serial.begin(3000000);
  Serial.begin(115200);

  Serial.println("\n\nStarting");

  Wire.begin(23, 22);

  // Bus scan
  for (int address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0)
    {
      Serial.printf("I2C device found at address 0x%X (%d)\n", address, address);
    }
  }

  nvs_flash_init();
  nvs_open("storage", NVS_READWRITE, &hSensorValue);
  nvs_get_i32(hSensorValue, "counter", (int32_t *)&counter);

  // touch_pad_init();
  // touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V5);

  // touch_pad_config(TOUCH_PAD_NUM0, 0);
  // touch_pad_filter_start(10);

  pinMode(17, INPUT_PULLUP);
  attachInterrupt(17, handleReset, FALLING);

  initUi();

  initDistanceSensor();

  Serial.println("Started");
}

void loop()
{
  // uint16_t sensor;
  // touch_pad_read_raw_data(TOUCH_PAD_NUM0, &sensor);
  // Serial.printf("sensor = %d\n", (int)sensor);

  static int savedCounter = -1;
  if (counter != savedCounter)
  {
    nvs_set_i32(hSensorValue, "counter", (int32_t)counter);
    nvs_commit(hSensorValue);
    savedCounter = counter;
  }

  delay(updateUi());
}

void IRAM_ATTR handleReset(void)
{
  counter = 0;
}

