/*
  Read truetype(.ttf) from SD and generate bitmap.

  TrueType™ Reference Manual
  https://developer.apple.com/fonts/TrueType-Reference-Manual/
*/

#include "LittleFS.h"
#include "TruetypeManager.h"
#include <TFT_eSPI.h>
TruetypeManager::TruetypeManager() {
  for(int i = 0 ;i < BUF_COUNT; i++){
      _framebuffer[i] = (uint8_t *)calloc(sizeof(uint8_t), FRAMEBUFFER_SIZE);
      if (!_framebuffer[i])
      {
        Serial.println("alloc memory failed !!!");
        freeAllFramebuffer();
        //return false;
      }
    }
}

TruetypeManager::~TruetypeManager() {
  // 释放truetype对象
  //delete _truetype;
  freeAllFramebuffer();
}

uint8_t *TruetypeManager::getFramebuffer(uint8_t index) {
  return _framebuffer[index];
}

void TruetypeManager::setFramebuffer(uint8_t index, uint8_t *framebuffer) {
  _framebuffer[index] = framebuffer;
}

bool TruetypeManager::initTruetype(const String& path, truetypeClass *truetype) {
    LittleFS.begin();
    File fontFile = LittleFS.open(path, "r");
    Serial.println("fontFile.name():"+String(fontFile.name()));
    Serial.println("fontFile.size():"+String(fontFile.size()));
    if (!_truetype.setTtfFile(fontFile))
    {
      Serial.println("read ttf failed");
      //freeAllFramebuffer();
      return false;
    }

    _truetype.setCharacterSize(160);
    _truetype.setTextBoundary(0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    _truetype.setCharacterSpacing(0);
    _truetype.setTextColor(0x01, 0x01);
    if(truetype != nullptr){
      Serial.println("truetype is nullptr");
      truetype = &_truetype;
    }
    return true;
}


bool TruetypeManager::checkFileExists(const char *filename) {
  if (!LittleFS.begin())
  {
    Serial.println("SPIFFS挂载失败");
    return false;
  }

  if (LittleFS.exists(filename))
  {
    Serial.println("文件存在");
    return true;
  }
  else
  {
    Serial.println("文件不存在");
    return false;
  }
}


uint8_t *TruetypeManager::readTextToFramebuffer(uint8_t index){


  static String draw_string[] = {"以","无","所","得","故","，","菩","提","萨","埵","，","依","般","若","波","罗","蜜","多","故","，","心","无","罣","碍","；","无","罣","碍","故","，","无","有","恐","怖","，","远","离","颠","倒","梦","想","，","究","竟","涅","槃","。","三","世","诸","佛","，","依","般","若","波","罗","蜜","多","故","，","得","阿","耨","多","罗","三","藐","三","菩","提","。"};
    //static String draw_string[] = {"老","伍","牛","X","欢","迎","来","到","美","丽","的","中","国"};
  static uint16_t draw_string_len = sizeof(draw_string) / sizeof(draw_string[0]);
  static uint16_t draw_string_index = 0;
  String str = draw_string[draw_string_index];
  //uint16_t unicode = getChineseUnicode(str);
  

  unsigned long startTimeRead = millis();
  uint8_t *framebuffer = this->getFramebuffer(index);
  memset(framebuffer, 0, FRAMEBUFFER_SIZE);
  _truetype.setFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT, BITS_PER_PIXEL, 0, framebuffer);
  _truetype.textDraw(0,0,str);
  //unsigned long endTimeRead = millis();
  // //Serial.println("readText Time:"+String((endTimeRead-startTimeRead))+"ms");
  draw_string_index++;
  if(draw_string_index >= draw_string_len){
      draw_string_index = 0;
  }
  return _framebuffer[index];
}

uint16_t TruetypeManager::getPixelColor(uint8_t *framebuffer, uint16_t x, uint16_t y){
  //return _framebuffer[bufIndex][y * DISPLAY_WIDTH + x];


    uint16_t byte_index = x * WIDTH_BYTES + (y / 8);
    uint8_t bit_position = 7 - (y % 8); // 从高位到低位
    uint8_t bit_mask = _BV(bit_position);
    if (framebuffer[byte_index] & bit_mask) {
        return TFT_WHITE;
    } else {
        return TFT_BLACK;
    }
}

// 获取单个汉字的Unicode十六进制值
uint16_t TruetypeManager::getChineseUnicode(const String& character) {
  if (character.length() == 0) return 0;
  
  uint16_t unicode = 0;
  unsigned char c1 = character[0];
  
  if ((c1 & 0x80) == 0) {
    // ASCII字符
    unicode = c1;
  } else if ((c1 & 0xE0) == 0xC0 && character.length() >= 2) {
    // 双字节UTF-8
    unsigned char c2 = character[1];
    unicode = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
  } else if ((c1 & 0xF0) == 0xE0 && character.length() >= 3) {
    // 三字节UTF-8 (常见中文)
    unsigned char c2 = character[1];
    unsigned char c3 = character[2];
    unicode = ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
  }
  
  return unicode;
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