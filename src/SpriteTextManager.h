#ifndef __SPRITE_TEXT_MANAGER_H__
#define __SPRITE_TEXT_MANAGER_H__
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "lcd.h"

#define SPRITE_WIDTH 40
#define SPRITE_HEIGHT 40

class SpriteTextManager{
    private:
        TFT_eSPI *tft;
        TFT_eSprite *sprite;
    public:
        SpriteTextManager();
        ~SpriteTextManager();
        void init(uint16_t width, uint16_t height);
        void init();
        bool createSprite(uint16_t width, uint16_t height,uint16_t colorDepth);
        void fillScreen(uint16_t color);
        void loadFont(const uint8_t *font_data);
        void drawString(const String &string);
        void setScrollWindow(int16_t top_fixed,int16_t scroll_content,int16_t bottom_fixed);
        void scrollStart(int16_t address);
        void draw2LCD(int16_t x,int16_t y,uint16_t color);
        uint16_t getPixel(int16_t x,int16_t y);
};
#endif