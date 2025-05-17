/*
  Read truetype(.ttf) from SD and generate bitmap.

  TrueType™ Reference Manual
  https://developer.apple.com/fonts/TrueType-Reference-Manual/
*/

#include "LittleFS.h"
#include "TruetypeManager.h"
#include "lcd_spi_common.h"
#include <TFT_eSPI.h>
TruetypeManager::TruetypeManager()
{
  for (int i = 0; i < BUF_COUNT; i++)
  {
    _framebuffers[i].framebuffer = (uint8_t *)calloc(sizeof(uint8_t), FRAMEBUFFER_SIZE);
    _framebuffers[i].hasData = false;
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
// 初始化分类器 (只需调用一次)
bool TruetypeManager::init_classifier(string_cache_t* sc, const char* str, int m) {
    // 参数检查
    if (!sc || !str || m <= 0) return false;

    // 计算UTF-8字符数量和分配内存
    const char* p = str;
    int char_count = 0;
    while (*p) {
        if ((*p & 0x80) == 0) {
            // ASCII
            p += 1;
        } else if ((*p & 0xE0) == 0xC0) {
            // 2字节UTF-8
            p += 2;
        } else if ((*p & 0xF0) == 0xE0) {
            // 3字节UTF-8
            p += 3;
        } else if ((*p & 0xF8) == 0xF0) {
            // 4字节UTF-8
            p += 4;
        } else {
            p += 1; // 跳过无效字节
        }
        char_count++;
    }

    // 如果字符串长度小于等于m，使用字符串长度作为分组数
    int actual_groups = (char_count <= m) ? char_count : m;

    // 分配内存
    sc->wcs_str = (wchar_t*)malloc((char_count + 1) * sizeof(wchar_t));
    if (!sc->wcs_str) return false;

    // UTF-8转换为wchar_t
    p = str;
    int wchar_index = 0;
    while (*p && wchar_index < char_count) {
        uint32_t unicode = 0;
        if ((*p & 0x80) == 0) {
            // ASCII
            unicode = *p;
            p += 1;
        } else if ((*p & 0xE0) == 0xC0 && *(p+1)) {
            // 2字节UTF-8
            unicode = ((*p & 0x1F) << 6) | (*(p+1) & 0x3F);
            p += 2;
        } else if ((*p & 0xF0) == 0xE0 && *(p+1) && *(p+2)) {
            // 3字节UTF-8 (中文)
            unicode = ((*p & 0x0F) << 12) | ((*(p+1) & 0x3F) << 6) | (*(p+2) & 0x3F);
            p += 3;
        } else if ((*p & 0xF8) == 0xF0 && *(p+1) && *(p+2) && *(p+3)) {
            // 4字节UTF-8
            unicode = ((*p & 0x07) << 18) | ((*(p+1) & 0x3F) << 12) | 
                     ((*(p+2) & 0x3F) << 6) | (*(p+3) & 0x3F);
            p += 4;
        } else {
            p += 1; // 跳过无效字节
            continue;
        }
        sc->wcs_str[wchar_index++] = unicode;
    }
    sc->wcs_str[wchar_index] = 0; // 添加结束符

    // 存储基本信息
    sc->str_len = char_count;
    sc->group_num = actual_groups;  // 使用实际的分组数
    
    // 分配组长度数组
    sc->group_len = (int*)malloc(actual_groups * sizeof(int));
    if (!sc->group_len) {
        free(sc->wcs_str);
        return false;
    }

    // 初始化每个组的长度为0
    for (int i = 0; i < actual_groups; i++) {
        sc->group_len[i] = 0;
    }

    // 计算每个组实际包含的字符数
    for (int i = 0; i < char_count; i++) {
        int group = i % actual_groups;  // 字符属于哪个组
        sc->group_len[group]++;  // 该组的长度加1
    }

    // 打印调试信息
    // Serial.print("Total characters: ");
    // Serial.println(char_count);
    // Serial.print("Actual groups: ");
    // Serial.println(actual_groups);
    // for (int i = 0; i < actual_groups; i++) {
    //     Serial.print("Group ");
    //     Serial.print(i);
    //     Serial.print(" length: ");
    //     Serial.println(sc->group_len[i]);
    // }

    return true;
}
// 核心查询函数 (高频调用)
char* TruetypeManager::get_group_char(const string_cache_t* sc, int group, int index) {
    // 参数检查
    if (!sc || group < 0 || index < 0) {
        return nullptr;
    }

    // 检查组号是否超出范围
    if (group >= sc->group_num) {
        return nullptr;
    }

    // 检查索引是否超出该组的实际长度
    if (index >= sc->group_len[group]) {
        return nullptr;
    }

    // 计算实际位置：index * m + group
    int pos = index * sc->group_num + group;
    
    // 检查是否超出总字符串长度
    if (pos >= sc->str_len) {
        return nullptr;
    }

    // 获取Unicode码点
    wchar_t ch = sc->wcs_str[pos];
    
    // 分配静态缓冲区
    static char utf8_buf[4];
    
    // 转换为UTF-8
    if (ch <= 0x7F) {
        utf8_buf[0] = ch;
        utf8_buf[1] = '\0';
    } else if (ch <= 0x7FF) {
        utf8_buf[0] = 0xC0 | (ch >> 6);
        utf8_buf[1] = 0x80 | (ch & 0x3F);
        utf8_buf[2] = '\0';
    } else {
        utf8_buf[0] = 0xE0 | (ch >> 12);
        utf8_buf[1] = 0x80 | ((ch >> 6) & 0x3F);
        utf8_buf[2] = 0x80 | (ch & 0x3F);
        utf8_buf[3] = '\0';
    }

    return utf8_buf;
}

// 释放分类器资源
void TruetypeManager::free_classifier(string_cache_t* sc) {
    if (sc) {
        free(sc->wcs_str);
        free(sc->group_len);
    }
}










framebuffer_t *TruetypeManager::getFramebuffer(uint8_t index)
{
  return &_framebuffers[index];
}

bool TruetypeManager::resetFramebuffer(uint8_t index)
{
  _framebuffers[index].hasData = false;
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

  _truetype.setCharacterSize(WIDTH_PIXELS);
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

// void TruetypeManager::readTextToAllFramebuffer()
// {
//   for(int i = 0; i < BUF_COUNT; i++){
//     //readTextToFramebuffer(i);
//     framebuffer_t *framebuffer_t = &_framebuffers[i];
//     bool hasData = framebuffer_t->hasData;
//     if(hasData){
//       continue;
//     }
//     uint8_t *framebuffer = framebuffer_t->framebuffer;
//     memset(framebuffer, 0, FRAMEBUFFER_SIZE);
//     _truetype.setFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT, BITS_PER_PIXEL, 0, framebuffer);
//     _truetype.textDraw(0, 0, str);
//     framebuffer_t->hasData = true;
//   }
// }

uint8_t *TruetypeManager::readTextToFramebuffer()
{
  //static String draw_string[] = {"以", "无", "所", "得", "故", "，", "菩", "提", "萨", "埵", "，", "依", "般", "若", "波", "罗", "蜜", "多", "故", "，", "心", "无", "罣", "碍", "；", "无", "罣", "碍", "故", "，", "无", "有", "恐", "怖", "，", "远", "离", "颠", "倒", "梦", "想", "，", "究", "竟", "涅", "槃", "。", "三", "世", "诸", "佛", "，", "依", "般", "若", "波", "罗", "蜜", "多", "故", "，", "得", "阿", "耨", "多", "罗", "三", "藐", "三", "菩", "提", "。"};
  
  unsigned long startTime = micros();
  String str = _draw_strings[_draw_string_index];   
  framebuffer_t *framebuffer_t = &_framebuffers[_framebuffer_index];
  bool hasData = framebuffer_t->hasData;
  if(hasData){
    //vTaskDelay();
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
  if( _framebuffer_index >= BUF_COUNT - 1){
    _framebuffer_index = 0;
  }
  else{
    _framebuffer_index++;
  }
  
  // if(_framebuffer_index == 0){
  //   _framebuffer_index = 1;
  // }
  // else{
  //   _framebuffer_index = 0;
  // }
  framebuffer_t->hasData = true;
  return framebuffer;
}
uint16_t TruetypeManager::getPixelColor(uint8_t *framebuffer, uint16_t x, uint16_t y)
{
    // 检查是否使用抗锯齿或原始二值模式
    #define USE_ANTIALIASING 1
    
    #if USE_ANTIALIASING
        // 检查边界以避免访问越界内存
        if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
            return TFT_BLACK;
        }
        
        // 原始像素值
        uint16_t byte_index = x * WIDTH_BYTES + (y / 8);
        uint8_t bit_position = 7 - (y % 8); // 从高位到低位
        uint8_t bit_mask = _BV(bit_position);
        bool center_pixel = framebuffer[byte_index] & bit_mask;
        
        // 预先检查周围像素，判断是否位于边缘
        bool edge_detected = false;
        bool has_white = center_pixel;
        bool has_black = !center_pixel;
        
        // 首先检查临近像素以检测边缘
        for (int8_t dy = -1; dy <= 1 && !edge_detected; dy++) {
            for (int8_t dx = -1; dx <= 1 && !edge_detected; dx++) {
                if (dx == 0 && dy == 0) continue; // 跳过中心
                
                int16_t nx = x + dx;
                int16_t ny = y + dy;
                
                if (nx >= 0 && nx < DISPLAY_WIDTH && ny >= 0 && ny < DISPLAY_HEIGHT) {
                    byte_index = nx * WIDTH_BYTES + (ny / 8);
                    bit_position = 7 - (ny % 8);
                    bit_mask = _BV(bit_position);
                    bool neighbor_pixel = framebuffer[byte_index] & bit_mask;
                    
                    // 如果找到不同颜色的像素，说明我们在边缘
                    has_white |= neighbor_pixel;
                    has_black |= !neighbor_pixel;
                    
                    // 同时有黑色和白色像素时，检测到边缘
                    edge_detected = has_white && has_black;
                }
            }
        }
        
        // 如果不在边缘，对中心区域使用二值渲染
        if (!edge_detected) {
            return center_pixel ? TFT_WHITE : TFT_BLACK;
        }
        
        // 对边缘应用增强的抗锯齿
        const uint8_t SAMPLE_RADIUS = 2; // 在边缘使用更大的采样半径
        int16_t weighted_sum = 0;
        int16_t total_weight = 0;
        
        // 中心像素对最终颜色贡献更多
        if (center_pixel) {
            weighted_sum += 16; // 中心像素权重更高
            total_weight += 16;
        } else {
            total_weight += 16; // 仍然将中心计入分母
        }
        
        // 在更大的半径内采样边缘
        for (int8_t dy = -SAMPLE_RADIUS; dy <= SAMPLE_RADIUS; dy++) {
            for (int8_t dx = -SAMPLE_RADIUS; dx <= SAMPLE_RADIUS; dx++) {
                // 跳过中心像素，因为已经处理过
                if (dx == 0 && dy == 0) continue;
                
                int16_t nx = x + dx;
                int16_t ny = y + dy;
                
                // 检查邻域像素是否在边界内
                if (nx >= 0 && nx < DISPLAY_WIDTH && ny >= 0 && ny < DISPLAY_HEIGHT) {
                    // 基于距离和位置的权重计算
                    // 更近的像素权重更大，对角线像素权重更小
                    int16_t dist_squared = dx*dx + dy*dy;
                    uint8_t weight;
                    
                    // 基于距离分配权重
                    if (dist_squared <= 1) {
                        weight = 8; // 直接相邻（上下左右）
                    } else if (dist_squared <= 2) {
                        weight = 6; // 对角线邻居
                    } else if (dist_squared <= 4) {
                        weight = 4; // 稍远一些
                    } else {
                        weight = 2; // 半径内最远的像素
                    }
                    
                    byte_index = nx * WIDTH_BYTES + (ny / 8);
                    bit_position = 7 - (ny % 8);
                    bit_mask = _BV(bit_position);
                    
                    if (framebuffer[byte_index] & bit_mask) {
                        weighted_sum += weight;
                    }
                    
                    total_weight += weight;
                }
            }
        }
        
        // 计算抗锯齿灰度级别（0-255）
        uint16_t gray_level = (weighted_sum * 255) / total_weight;
        
        // S曲线对比度增强，更明显的抗锯齿
        // 这使得边缘过渡更平滑
        if (gray_level < 128) {
            // 较暗像素：应用曲线使渐变更明显
            gray_level = ((gray_level * gray_level) >> 7);
        } else {
            // 较亮像素：应用反曲线
            uint16_t inv = 255 - gray_level;
            inv = ((inv * inv) >> 7);
            gray_level = 255 - inv;
        }
        
        // 基于原始像素是黑色还是白色进一步增强对比度
        if (center_pixel) {
            // 如果中心是白色，偏向白色
            gray_level = 192 + (gray_level >> 2);
        } else {
            // 如果中心是黑色，仅当周围有强白色邻居时才变亮
            if (gray_level < 64) {
                gray_level = gray_level >> 1; // 使其更暗
            }
        }
        
        // 确保范围在0-255
        if (gray_level > 255) gray_level = 255;
        
        // 转换为RGB565格式 - 返回灰度值
        uint16_t r = gray_level >> 3;  // 红色5位
        uint16_t g = gray_level >> 2;  // 绿色6位
        uint16_t b = gray_level >> 3;  // 蓝色5位
        
        return (r << 11) | (g << 5) | b;
    #else
        // 原始二值实现
        uint16_t byte_index = x * WIDTH_BYTES + (y / 8);
        uint8_t bit_position = 7 - (y % 8); // 从高位到低位
        uint8_t bit_mask = _BV(bit_position);
        if (framebuffer[byte_index] & bit_mask)
        {
            return TFT_RED;
        }
        else
        {
            return TFT_BLACK;
        }
    #endif
}
uint16_t getPixelColor1(uint8_t *framebuffer, uint16_t x, uint16_t y)
{
  // Check if we're using anti-aliasing or original binary mode
  #define USE_ANTIALIASING 1
  
  #if USE_ANTIALIASING
    // Check boundaries to avoid accessing out of bounds memory
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
      return TFT_BLACK;
    }
    
    // Original pixel value
    uint16_t byte_index = x * WIDTH_BYTES + (y / 8);
    uint8_t bit_position = 7 - (y % 8); // 从高位到低位
    uint8_t bit_mask = _BV(bit_position);
    bool center_pixel = framebuffer[byte_index] & bit_mask;
    
    // Pre-calculate surrounding pixels to determine if we're near an edge
    // This helps us decide whether to apply anti-aliasing
    bool edge_detected = false;
    bool has_white = center_pixel;
    bool has_black = !center_pixel;
    
    // Check immediate neighbors first to detect edges
    for (int8_t dy = -1; dy <= 1 && !edge_detected; dy++) {
      for (int8_t dx = -1; dx <= 1 && !edge_detected; dx++) {
        if (dx == 0 && dy == 0) continue; // Skip center
        
        int16_t nx = x + dx;
        int16_t ny = y + dy;
        
        if (nx >= 0 && nx < DISPLAY_WIDTH && ny >= 0 && ny < DISPLAY_HEIGHT) {
          byte_index = nx * WIDTH_BYTES + (ny / 8);
          bit_position = 7 - (ny % 8);
          bit_mask = _BV(bit_position);
          bool neighbor_pixel = framebuffer[byte_index] & bit_mask;
          
          // If we find different colored pixels, we're at an edge
          has_white |= neighbor_pixel;
          has_black |= !neighbor_pixel;
          
          // Edge detected when we have both white and black pixels
          edge_detected = has_white && has_black;
        }
      }
    }
    
    // If we're not at an edge, use binary rendering for center areas
    if (!edge_detected) {
      return center_pixel ? TFT_WHITE : TFT_BLACK;
    }
    
    // For edges, apply enhanced anti-aliasing
    const uint8_t SAMPLE_RADIUS = 2; // Larger sample radius at edges for better smoothing
    int16_t weighted_sum = 0;
    int16_t total_weight = 0;
    
    // Center pixel contributes more to the final color
    if (center_pixel) {
      weighted_sum += 16; // Higher weight for center
      total_weight += 16;
    } else {
      total_weight += 16; // Still count center in denominator
    }
    
    // Sample in the larger radius for edges
    for (int8_t dy = -SAMPLE_RADIUS; dy <= SAMPLE_RADIUS; dy++) {
      for (int8_t dx = -SAMPLE_RADIUS; dx <= SAMPLE_RADIUS; dx++) {
        // Skip the center pixel as we already processed it
        if (dx == 0 && dy == 0) continue;
        
        int16_t nx = x + dx;
        int16_t ny = y + dy;
        
        // Check if the neighboring pixel is within bounds
        if (nx >= 0 && nx < DISPLAY_WIDTH && ny >= 0 && ny < DISPLAY_HEIGHT) {
          // Weight calculation based on distance and position
          // Closer pixels have more weight, diagonal pixels less
          int16_t dist_squared = dx*dx + dy*dy;
          uint8_t weight;
          
          // Assign weights based on distance
          if (dist_squared <= 1) {
            weight = 8; // Directly adjacent (up, down, left, right)
          } else if (dist_squared <= 2) {
            weight = 6; // Diagonal neighbors
          } else if (dist_squared <= 4) {
            weight = 4; // Slightly further
          } else {
            weight = 2; // Furthest pixels within radius
          }
          
          byte_index = nx * WIDTH_BYTES + (ny / 8);
          bit_position = 7 - (ny % 8);
          bit_mask = _BV(bit_position);
          
          if (framebuffer[byte_index] & bit_mask) {
            weighted_sum += weight;
          }
          
          total_weight += weight;
        }
      }
    }
    
    // Calculate anti-aliased gray level (0-255)
    uint16_t gray_level = (weighted_sum * 255) / total_weight;
    
    // S-curve contrast enhancement for more pronounced anti-aliasing
    // This makes transitions more gradual at edges
    if (gray_level < 128) {
      // Darker pixels: apply curve to make gradient more visible
      gray_level = ((gray_level * gray_level) >> 7);
    } else {
      // Lighter pixels: apply inverse curve
      uint16_t inv = 255 - gray_level;
      inv = ((inv * inv) >> 7);
      gray_level = 255 - inv;
    }
    
    // Further enhance contrast based on whether the original pixel was white or black
    if (center_pixel) {
      // If center was white, bias toward white
      gray_level = 192 + (gray_level >> 2);
    } else {
      // If center was black, only brighten if we have strong white neighbors
      if (gray_level < 64) {
        gray_level = gray_level >> 1; // Make darker
      }
    }
    
    // Ensure we stay in range
    if (gray_level > 255) gray_level = 255;
    
    // Convert to RGB565 format
    uint16_t r = gray_level >> 3;  // 5 bits for red
    uint16_t g = gray_level >> 2;  // 6 bits for green
    uint16_t b = gray_level >> 3;  // 5 bits for blue
    
    return (r << 11) | (g << 5) | b;
  #else
    // Original binary implementation
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
  #endif
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
    _framebuffers[index].hasData = false;
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