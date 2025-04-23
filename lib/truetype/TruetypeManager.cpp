/*
  Read truetype(.ttf) from SD and generate bitmap.

  TrueType™ Reference Manual
  https://developer.apple.com/fonts/TrueType-Reference-Manual/
*/

#include "LittleFS.h"
#include "TruetypeManager.h"
#include <TFT_eSPI.h>
TruetypeManager::TruetypeManager()
{
  for (int i = 0; i < BUF_COUNT; i++)
  {
    _framebuffers[i].framebuffer = (uint8_t *)calloc(sizeof(uint8_t), FRAMEBUFFER_SIZE);
    _framebuffers[i].hadData = false;
    if (!_framebuffers[i].framebuffer)
    {
      Serial.println("alloc memory failed !!!");
      freeAllFramebuffer();
      // return false;
    }
  }
}

TruetypeManager::~TruetypeManager()
{
  // 释放truetype对象
  // delete _truetype;
  freeAllFramebuffer();
}

framebuffer_t *TruetypeManager::getFramebuffer(uint8_t index)
{
  return &_framebuffers[index];
}

bool TruetypeManager::resetFramebuffer(uint8_t index)
{
  _framebuffers[index].hadData = false;
  memset(_framebuffers[index].framebuffer, 0, FRAMEBUFFER_SIZE);
  return true;
}
// void TruetypeManager::setFramebuffer(uint8_t index, uint8_t *framebuffer)
// {
//   _framebuffers[index].framebuffer = framebuffer;
// }

bool TruetypeManager::initTruetype(const String &path, truetypeClass *truetype)
{
  LittleFS.begin();
  File fontFile = LittleFS.open(path, "r");
  Serial.println("fontFile.name():" + String(fontFile.name()));
  Serial.println("fontFile.size():" + String(fontFile.size()));
  if (!_truetype.setTtfFile(fontFile))
  {
    Serial.println("read ttf failed");
    // freeAllFramebuffer();
    return false;
  }

  _truetype.setCharacterSize(160);
  _truetype.setTextBoundary(0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
  _truetype.setCharacterSpacing(0);
  _truetype.setTextColor(0x01, 0x01);
  if (truetype != nullptr)
  {
    Serial.println("truetype is nullptr");
    truetype = &_truetype;
  }
  return true;
}

bool TruetypeManager::checkFileExists(const char *filename)
{
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

// void setDrawString(String *drawString){
//   //static String draw_string[] = {"以", "无", "所", "得", "故","。"};
//   static uint16_t draw_string_len = sizeof(drawString) / sizeof(draw_string[0]);
//   static uint16_t draw_string_index = 0;
// }
uint8_t *TruetypeManager::readTextToFramebuffer()
{
  //static String draw_string[] = {"以", "无", "所", "得", "故", "，", "菩", "提", "萨", "埵", "，", "依", "般", "若", "波", "罗", "蜜", "多", "故", "，", "心", "无", "罣", "碍", "；", "无", "罣", "碍", "故", "，", "无", "有", "恐", "怖", "，", "远", "离", "颠", "倒", "梦", "想", "，", "究", "竟", "涅", "槃", "。", "三", "世", "诸", "佛", "，", "依", "般", "若", "波", "罗", "蜜", "多", "故", "，", "得", "阿", "耨", "多", "罗", "三", "藐", "三", "菩", "提", "。"};
  String str = _draw_strings[_draw_string_index];
  unsigned long startTimeRead = millis();
  uint8_t index = _draw_string_index % 2;

  framebuffer_t *framebuffer_t = &_framebuffers[index];
  bool hadData = framebuffer_t->hadData;
  if(hadData){
    return nullptr;
  }
  uint8_t *framebuffer = framebuffer_t->framebuffer;
  memset(framebuffer, 0, FRAMEBUFFER_SIZE);
  _truetype.setFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT, BITS_PER_PIXEL, 0, framebuffer);
  _truetype.textDraw(0, 0, str);
  _draw_string_index++;
  if (_draw_string_index >= _draw_strings_length)
  {
    _draw_string_index = 0;
  }
  framebuffer_t->hadData = true;
  return framebuffer;
}

uint16_t TruetypeManager::getPixelColor(uint8_t *framebuffer, uint16_t x, uint16_t y)
{
  // return _framebuffer[bufIndex][y * DISPLAY_WIDTH + x];

  uint16_t byte_index = x * WIDTH_BYTES + (y / 8);
  uint8_t bit_position = 7 - (y % 8); // 从高位到低位
  uint8_t bit_mask = _BV(bit_position);
  if (framebuffer[byte_index] & bit_mask)
  {
    return TFT_WHITE;
  }
  else
  {
    return TFT_BLACK;
  }
}

// 获取单个汉字的Unicode十六进制值
uint16_t TruetypeManager::getChineseUnicode(const String &character)
{
  if (character.length() == 0)
    return 0;

  uint16_t unicode = 0;
  unsigned char c1 = character[0];

  if ((c1 & 0x80) == 0)
  {
    // ASCII字符
    unicode = c1;
  }
  else if ((c1 & 0xE0) == 0xC0 && character.length() >= 2)
  {
    // 双字节UTF-8
    unsigned char c2 = character[1];
    unicode = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
  }
  else if ((c1 & 0xF0) == 0xE0 && character.length() >= 3)
  {
    // 三字节UTF-8 (常见中文)
    unsigned char c2 = character[1];
    unsigned char c3 = character[2];
    unicode = ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
  }

  return unicode;
}

void TruetypeManager::freeFramebuffer(uint8_t index)
{
  if (_framebuffers[index].framebuffer)
  {
    free(_framebuffers[index].framebuffer);
    _framebuffers[index].framebuffer = nullptr;
    _framebuffers[index].hadData = false;
  }
}
void TruetypeManager::freeAllFramebuffer()
{
  for (int i = 0; i < BUF_COUNT; i++)
  {
    freeFramebuffer(i);
  }
}
void TruetypeManager::setDrawString(const String drawStrings[])
{
  // Calculate the length by finding the first null element
  uint16_t length = 0;
  while (drawStrings[length].length() > 0 && length < MAX_DRAW_STRINGS) {
    length++;
  }
  
  // Copy the new strings
  for (int i = 0; i < length; i++) {
    _draw_strings[i] = drawStrings[i];
  }
  
  // Update the length and reset the index
  _draw_strings_length = length;
  _draw_string_index = 0;
  
  Serial.print("Set new draw strings with length: ");
  Serial.println(_draw_strings_length);
}
void TruetypeManager::setDrawString(const char *drawString) {
  if (drawString == nullptr || strlen(drawString) == 0) {
    Serial.println("Error: Empty string provided");
    return;
  }
  
  uint16_t length = 0;
  uint16_t i = 0;
  uint16_t byteIndex = 0;
  
  // 清空之前的字符串数组
  for (int j = 0; j < _draw_strings_length; j++) {
    _draw_strings[j] = "";
  }
  
  // 分割字符串，正确处理UTF-8编码字符
  while (drawString[byteIndex] != '\0' && length < MAX_DRAW_STRINGS) {
    char currentByte = drawString[byteIndex];
    String character = "";
    
    if ((currentByte & 0x80) == 0) {
      // ASCII字符 (1字节)
      character = String(drawString[byteIndex]);
      byteIndex += 1;
    } else if ((currentByte & 0xE0) == 0xC0) {
      // 2字节UTF-8字符
      if (drawString[byteIndex + 1] != '\0') {
        character = String(drawString + byteIndex).substring(0, 2);
        byteIndex += 2;
      } else {
        break; // 字符串末尾，不完整字符
      }
    } else if ((currentByte & 0xF0) == 0xE0) {
      // 3字节UTF-8字符 (中文等)
      if (drawString[byteIndex + 1] != '\0' && drawString[byteIndex + 2] != '\0') {
        character = String(drawString + byteIndex).substring(0, 3);
        byteIndex += 3;
      } else {
        break; // 字符串末尾，不完整字符
      }
    } else if ((currentByte & 0xF8) == 0xF0) {
      // 4字节UTF-8字符 (表情符号等)
      if (drawString[byteIndex + 1] != '\0' && drawString[byteIndex + 2] != '\0' && drawString[byteIndex + 3] != '\0') {
        character = String(drawString + byteIndex).substring(0, 4);
        byteIndex += 4;
      } else {
        break; // 字符串末尾，不完整字符
      }
    } else {
      // 无效UTF-8序列，跳过
      byteIndex++;
      continue;
    }
    
    if (character.length() > 0) {
      _draw_strings[length] = character;
      length++;
    }
  }
  
  // 更新长度和重置索引
  _draw_strings_length = length;
  _draw_string_index = 0;
  
  Serial.print("Set new draw strings with length: ");
  Serial.println(_draw_strings_length);
}