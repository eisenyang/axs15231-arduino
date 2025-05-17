/*
  Read truetype(.ttf) from SD and generate bitmap.

  TrueType™ Reference Manual
  https://developer.apple.com/fonts/TrueType-Reference-Manual/
*/

#pragma once
#include "truetype_Arduino.h"
#define BUF_COUNT  4
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

typedef struct {
    wchar_t *wcs_str;
    int str_len;    // 字符串长度
    int group_num;  // 分组数量
    int* group_len;  // 每个分组的长度
} string_cache_t;
class TruetypeManager{
  public:
    TruetypeManager();
    ~TruetypeManager();
    bool init_classifier(string_cache_t* sc, const char* str, int m);
    void free_classifier(string_cache_t *sc);
    char* get_group_char(const string_cache_t* sc, int group, int index);


    bool initTruetype(const String& path, truetypeClass *truetype);
    void readTextToAllFramebuffer();
    
    uint8_t *readTextToFramebuffer();
    uint16_t getPixelColor(uint8_t *framebuffer, uint16_t x, uint16_t y);
    uint16_t getChineseUnicode(const String& character);
    bool checkFileExists(const char *filename);
    framebuffer_t *getFramebuffer(uint8_t index);
    bool resetFramebuffer(uint8_t index);
    void setDrawString(const String drawStrings[]);
    void setDrawString(const char *drawString);
    //void setFramebuffer(uint8_t index, uint8_t *framebuffer);
    void freeFramebuffer(uint8_t index);
    void freeAllFramebuffer();
  private:
    truetypeClass _truetype = truetypeClass();
    //uint8_t *_framebuffer[BUF_COUNT];
    // uint8_t _readFromTtfIndex = -1;
    // uint8_t _writeToDisplayIndex = -1;
    // uint8_t _framebufferNum = 0;
    // uint8_t _availableIndex = -1;
    String _draw_strings[MAX_DRAW_STRINGS];
    uint16_t _draw_strings_length = 0;
    uint16_t _draw_string_index = 0;
    uint8_t _framebuffer_index = 0;
    framebuffer_t _framebuffers[BUF_COUNT];
};
