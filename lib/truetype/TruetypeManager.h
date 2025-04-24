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
#define MAX_DRAW_STRINGS 250
typedef struct Framebuffer{
  uint8_t *framebuffer;
  bool hasData;
} framebuffer_t;
class TruetypeManager{
  public:
    TruetypeManager();
    ~TruetypeManager();
    bool initTruetype(const String& path, truetypeClass *truetype);
    uint8_t *readTextToFramebuffer();
    uint16_t getPixelColor(uint8_t *framebuffer, uint16_t x, uint16_t y);
    uint16_t getChineseUnicode(const String& character);
    bool checkFileExists(const char *filename);
    framebuffer_t *getFramebuffer(uint8_t index);
    bool resetFramebuffer(uint8_t index);
    void setDrawString(const String drawStrings[]);
    void setDrawString(const char *drawString);
    void freeFramebuffer(uint8_t index);
    void freeAllFramebuffer();
  private:
    truetypeClass _truetype = truetypeClass();
    String _draw_strings[MAX_DRAW_STRINGS];
    uint16_t _draw_strings_length = 0;
    uint16_t _draw_string_index = 0;
    framebuffer_t _framebuffers[BUF_COUNT];
};
