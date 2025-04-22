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
    bool checkFileExists(const char *filename);
    uint8_t initTtfFile(const char *ttf_file);
    void initTextDraw();
    void init();
    void setDrawString(String draw_string[]);
    uint8_t *readTextToBuffer(uint8_t bufIndex);
    void freeFramebuffer(uint8_t index);
    void freeAllFramebuffer();
    uint16_t getPixelColor(uint8_t bufIndex, uint16_t x, uint16_t y);
  private:
    truetypeClass _truetype = truetypeClass();
    String *_draw_string;
    uint16_t _draw_string_len;
    uint16_t _draw_string_index;
    uint8_t *_framebuffer[BUF_COUNT];
};
