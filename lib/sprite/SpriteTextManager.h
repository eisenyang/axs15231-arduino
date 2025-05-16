#ifndef __SPRITE_TEXT_MANAGER_H__
#define __SPRITE_TEXT_MANAGER_H__
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "lcd_spi_common.h"

#define SPRITE_WIDTH 160
#define SPRITE_HEIGHT 160

class SpriteTextManager{
    private:
        TFT_eSPI *tft;
        TFT_eSprite *sprite;
        //TFT_eSprite *sprite_rotate;
    public:
        SpriteTextManager();
        ~SpriteTextManager();
        void init(uint16_t width, uint16_t height);
        void init();
        bool createSprite(uint16_t width, uint16_t height,uint16_t colorDepth);
        void fillScreen(uint16_t color);
        void loadFont(const uint8_t *font_data);
        void drawCenterString(const String &string);
        void setScrollWindow(int16_t top_fixed,int16_t scroll_content,int16_t bottom_fixed);
        void scrollStart(int16_t address);
        void drawPixel2LCD(int16_t x,int16_t y,uint16_t color);
        void setRowAddress(uint16_t x_offset,uint16_t y);
        void setRowAddress(uint16_t x_offset,uint16_t y,uint16_t row_length);
        void enableWriteColor();
        void disableWriteColor();
        void writeColor(uint16_t color);
        void writeColors(const uint16_t* colors,size_t length);
        void blockLcdWrite(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1);
        void clearSprite();
        void drawRow2LCD(uint16_t y,uint16_t color);
        uint16_t getPixel(int16_t x,int16_t y);
        void setPixel(int16_t x,int16_t y,uint16_t color);
};
#endif