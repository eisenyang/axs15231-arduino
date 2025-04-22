/*
  Read truetype(.ttf) from SD and generate bitmap.

  TrueType™ Reference Manual
  https://developer.apple.com/fonts/TrueType-Reference-Manual/
*/

#pragma once
#include "truetype_Arduino.h"
#define BUF_COUNT  2
#define QUEUE_LENGTH 2
#define WIDTH_BYTES 20
#define HEIGHT_PIXELS 160
#define WIDTH_PIXELS 160
#define DISPLAY_HEIGHT HEIGHT_PIXELS
#define FRAMEBUFFER_SIZE (WIDTH_BYTES * HEIGHT_PIXELS)
#define BITS_PER_PIXEL 1 // either 1, 4, or 8
#define DISPLAY_WIDTH (WIDTH_BYTES * (8 / BITS_PER_PIXEL))

class TruetypeManager{
  public:
    TruetypeManager();
    ~TruetypeManager();
    bool initTruetype(const String& path, truetypeClass *truetype);
    uint8_t *readTextToFramebuffer(uint8_t index);

    uint16_t getPixelColor(uint8_t *framebuffer, uint16_t x, uint16_t y);
    uint16_t getChineseUnicode(const String& character);
    //truetypeClass *getTruetype();
    bool checkFileExists(const char *filename);
    uint8_t *getFramebuffer(uint8_t index);
    void setFramebuffer(uint8_t index, uint8_t *framebuffer);
    void freeFramebuffer(uint8_t index);
    void freeAllFramebuffer();
  private:
    truetypeClass _truetype = truetypeClass();
    uint8_t *_framebuffer[BUF_COUNT];
};
