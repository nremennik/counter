#include <Arduino.h>
#include <Adafruit_VL53L0X.h>

TaskHandle_t hMeasureTask;
void measureTask(void *param);
void IRAM_ATTR handleDistanceSensor(void);

#define DISTANCE_THRESHOLD_MM 60
#define DISTANCE_MEASUREMENT_AREA_MM 120
#define DISTANCE_SENSOR_INTR 5
#define DISTANCE_SENSOR_RESET 18

Adafruit_VL53L0X *distanceSensor;

extern volatile int32_t counter;

void initDistanceSensor()
{
    distanceSensor = new Adafruit_VL53L0X();

    pinMode(DISTANCE_SENSOR_INTR, INPUT);
    attachInterrupt(DISTANCE_SENSOR_INTR, handleDistanceSensor, FALLING);
    // Reset sensor
    pinMode(DISTANCE_SENSOR_RESET, OUTPUT);
    digitalWrite(DISTANCE_SENSOR_RESET, 0);
    delay(100);
    digitalWrite(DISTANCE_SENSOR_RESET, 1);

    xTaskCreate(measureTask, "DistanceMeasure", 1024 * 5, nullptr, tskIDLE_PRIORITY, &hMeasureTask);

    // distanceSensor->begin(0x29, true, &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
    distanceSensor->begin();
    // distanceSensor->configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
    distanceSensor->setGpioConfig(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
                                  VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH,
                                  // VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_OUT,
                                  VL53L0X_INTERRUPTPOLARITY_LOW);
    distanceSensor->setInterruptThresholds(50, DISTANCE_MEASUREMENT_AREA_MM);
    distanceSensor->setDeviceMode(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, false);
    distanceSensor->startMeasurement();
}

volatile boolean interruptTriggered = false;
void IRAM_ATTR handleDistanceSensor(void)
{
    interruptTriggered = true;
}

void measureTask(void *param)
{
    static int lastDistance = 10000;
    VL53L0X_RangingMeasurementData_t mData;
    // distanceSensor->rangingTest(&mData, false);

    Serial.printf("Measuring task started\n");
    while (true)
    {
        if (interruptTriggered)
        {
            // Serial.printf("Interrupt fired\n");
            interruptTriggered = false;
            distanceSensor->getRangingMeasurement(&mData, false);
            if (mData.RangeStatus == 0)
            {
                if (lastDistance > DISTANCE_THRESHOLD_MM && mData.RangeMilliMeter < DISTANCE_THRESHOLD_MM)
                {
                    counter++;
                    lastDistance = mData.RangeMilliMeter;
                }
                lastDistance = mData.RangeMilliMeter;
                // Serial.printf("Distance is %d mm, counter=%d\n", mData.RangeMilliMeter, counter);
            }
            distanceSensor->clearInterruptMask(false);
        }
    }
}
