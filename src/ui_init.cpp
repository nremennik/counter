#include <Arduino.h>

#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#include "fonts.h"

extern volatile int counter;

SSD1306Wire *display;
OLEDDisplayUi *ui;

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

void initUi()
{
    display = new SSD1306Wire(0x3c, -1, -1, GEOMETRY_128_64, I2C_ONE, 400000);
    ui = new OLEDDisplayUi(display);

    ui->setTargetFPS(30);
    ui->setActiveSymbol(activeSymbol);
    ui->setInactiveSymbol(inactiveSymbol);
    ui->setIndicatorPosition(TOP);
    ui->setIndicatorDirection(LEFT_RIGHT);
    ui->setFrameAnimation(SLIDE_LEFT);
    // ui->disableAllIndicators();
    ui->disableAutoTransition();
    ui->setFrames(frames, framesCount);
    ui->init();
    display->flipScreenVertically();
    ui->switchToFrame(0);
    ui->update();
}

int updateUi()
{
    return (ui->update());
}
