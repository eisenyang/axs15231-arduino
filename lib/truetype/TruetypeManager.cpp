/*
  Read truetype(.ttf) from SD and generate bitmap.

  TrueType™ Reference Manual
  https://developer.apple.com/fonts/TrueType-Reference-Manual/
*/

#include "LittleFS.h"
#include "TruetypeManager.h"
#include <TFT_eSPI.h>
TruetypeManager::TruetypeManager() {
  // 初始化truetype对象
  //_truetype = new truetypeClass();
  //this->init();
  Serial.println("truetypeManager init");
}

TruetypeManager::~TruetypeManager() {
  // 释放truetype对象
  //delete _truetype;
}


void TruetypeManager::init() {
  // 初始化truetype对象

  for(int i = 0 ;i < BUF_COUNT; i++){
      _framebuffer[i] = (uint8_t *)calloc(sizeof(uint8_t), FRAMEBUFFER_SIZE);
      if (!_framebuffer[i])
      {
        Serial.println("alloc memory failed !!!");
        // freeAllFramebuffer();
        // return false;
      }
    }
}

bool TruetypeManager::checkFileExists(const char *filename) {
  if (!LittleFS.begin())
  {
    Serial.println("SPIFFS挂载失败");
    return false;
  }

  if (LittleFS.exists(filename))
  {
    Serial.println("文件存在-----》");
    return true;
  }
  else
  {
    Serial.println("文件不存在");
    return false;
  }
}

uint8_t TruetypeManager::initTtfFile(const char *ttf_file) {
  if (!checkFileExists(ttf_file)) {
    return 0;
  }
  // File fontFile = LittleFS.open(ttf_file, "r");
  // if (!fontFile) {
  //   Serial.println("打开文件失败");
  //   return 0;
  // }
  //return _truetype->setTtfFile(fontFile);
}

void TruetypeManager::initTextDraw() {
  _truetype.setCharacterSize(160);
  _truetype.setTextBoundary(0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
  _truetype.setCharacterSpacing(0);
  _truetype.setTextColor(0x01, 0x01);
}

void TruetypeManager::setDrawString(String draw_string[]) {
  _draw_string = draw_string;
  _draw_string_len = sizeof(draw_string) / sizeof(draw_string[0]);
  _draw_string_index = 0;
}

uint8_t *  TruetypeManager::readTextToBuffer(uint8_t bufIndex) {
  String str = _draw_string[_draw_string_index];
  memset(_framebuffer[bufIndex], 0, FRAMEBUFFER_SIZE);
  _truetype.setFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT, BITS_PER_PIXEL, 0, _framebuffer[bufIndex]);
  _truetype.textDraw(0,0,str);
  _draw_string_index++;
  if(_draw_string_index >= _draw_string_len){
      _draw_string_index = 0;
  }
  return _framebuffer[bufIndex];
}

void TruetypeManager::freeFramebuffer(uint8_t index){
  if(_framebuffer[index]){
    free(_framebuffer[index]);
    _framebuffer[index] = nullptr;
  }
}
void TruetypeManager::freeAllFramebuffer(){
  for(int i = 0 ;i < BUF_COUNT; i++){
      freeFramebuffer(i);
    }
}

uint16_t TruetypeManager::getPixelColor(uint8_t bufIndex, uint16_t x, uint16_t y){
    uint16_t byte_index = y * WIDTH_BYTES + (x / 8);
    uint8_t bit_position = 7 - (x % 8); // 从高位到低位
    uint8_t bit_mask = _BV(bit_position);
    if (_framebuffer[bufIndex][byte_index] & bit_mask) {
        return TFT_WHITE;
    } else {
        return TFT_BLACK;
    }
}