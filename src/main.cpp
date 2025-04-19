#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

// 安全配置STB
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STBTT_MALLOC(size) heap_caps_malloc(size, MALLOC_CAP_8BIT)
#define STBTT_FREE(ptr) heap_caps_free(ptr)
#include "stb_truetype.h"

// 获取单个汉字的Unicode十六进制值
uint32_t getChineseUnicode(const String& character) {
  if (character.length() == 0) return 0;
  
  uint32_t unicode = 0;
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

// TTF渲染器类
class TTFRenderer {
private:
  // 字体缓冲区和信息
  unsigned char* ttfBuffer;
  size_t bufferSize;
  stbtt_fontinfo fontInfo;
  bool fontLoaded;
  File fontFile;
  
public:
  TTFRenderer() : ttfBuffer(nullptr), bufferSize(0), fontLoaded(false) {}
  
  ~TTFRenderer() {
    close();
  }
  
  bool open(const char* filename) {
    // 清理先前的资源
    close();
    
    // 打开字体文件
    fontFile = SPIFFS.open(filename, "r");
    if (!fontFile) {
      Serial.println("无法打开字体文件");
      return false;
    }
    
    // 获取文件大小
    size_t fileSize = fontFile.size();
    Serial.printf("字体文件大小: %.2f MB\n", fileSize / (1024.0 * 1024.0));
    
    // 分配缓冲区
    // 为避免内存不足问题，我们只加载前2MB的字体数据，这通常足够包含所有表信息和常用字符的字形数据
    bufferSize = min(fileSize, (size_t)(2 * 1024 * 1024));
    ttfBuffer = (unsigned char*)STBTT_MALLOC(bufferSize);
    if (!ttfBuffer) {
      Serial.println("内存分配失败");
      fontFile.close();
      return false;
    }
    
    // 读取字体数据
    fontFile.seek(0);
    size_t bytesRead = fontFile.read(ttfBuffer, bufferSize);
    fontFile.close();
    
    if (bytesRead != bufferSize) {
      Serial.println("读取字体文件失败");
      STBTT_FREE(ttfBuffer);
      ttfBuffer = nullptr;
      return false;
    }
    
    Serial.printf("已读取 %.2f MB 字体数据\n", bytesRead / (1024.0 * 1024.0));
    
    // 验证字体格式
    Serial.printf("字体标识: %02X %02X %02X %02X\n", 
                 ttfBuffer[0], ttfBuffer[1], ttfBuffer[2], ttfBuffer[3]);
    
    // 初始化STB字体信息
    if (stbtt_InitFont(&fontInfo, ttfBuffer, 0) == 0) {
      Serial.println("初始化字体失败");
      STBTT_FREE(ttfBuffer);
      ttfBuffer = nullptr;
      return false;
    }
    
    fontLoaded = true;
    Serial.println("字体加载成功");
    
    return true;
  }
  
  void close() {
    if (ttfBuffer) {
      STBTT_FREE(ttfBuffer);
      ttfBuffer = nullptr;
    }
    
    fontLoaded = false;
    bufferSize = 0;
  }
  
  // 渲染字符
  uint8_t* renderCharacter(uint32_t codepoint, float pixelHeight, int* width, int* height, int* xoff, int* yoff) {
    if (!fontLoaded) {
      Serial.println("字体未加载");
      return nullptr;
    }
    
    // 查找字形索引
    int glyphIndex = stbtt_FindGlyphIndex(&fontInfo, codepoint);
    if (glyphIndex == 0) {
      Serial.printf("未找到字符 U+%04X 的字形索引\n", codepoint);
      return nullptr;
    }
    
    Serial.printf("找到字符 U+%04X 的字形索引: %d\n", codepoint, glyphIndex);
    
    // 计算缩放比例
    float scale = stbtt_ScaleForPixelHeight(&fontInfo, pixelHeight);
    
    // 获取字形边界框
    int ix0, iy0, ix1, iy1;
    stbtt_GetGlyphBitmapBox(&fontInfo, glyphIndex, scale, scale, &ix0, &iy0, &ix1, &iy1);
    
    // 计算位图尺寸
    int w = ix1 - ix0;
    int h = iy1 - iy0;
    
    Serial.printf("字形边界框: %d,%d - %d,%d (宽=%d, 高=%d)\n", 
                 ix0, iy0, ix1, iy1, w, h);
    
    // 确保位图尺寸合理
    if (w <= 0) w = pixelHeight;
    if (h <= 0) h = pixelHeight;
    
    // 创建位图
    uint8_t* bitmap = (uint8_t*)STBTT_MALLOC(w * h);
    if (!bitmap) {
      Serial.println("无法分配位图内存");
      return nullptr;
    }
    
    // 初始化位图
    memset(bitmap, 0, w * h);
    
    // 渲染字形到位图
    stbtt_MakeGlyphBitmap(&fontInfo, bitmap, w, h, w, scale, scale, glyphIndex);
    
    // 设置输出参数
    *width = w;
    *height = h;
    *xoff = ix0;
    *yoff = iy0;
    
    return bitmap;
  }
  
  // 获取字体的一些基本信息
  void getFontMetrics() {
    if (!fontLoaded) {
      Serial.println("字体未加载");
      return;
    }
    
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
    
    Serial.printf("字体度量: 上升=%d, 下降=%d, 行间距=%d\n", ascent, descent, lineGap);
    
    // 打印字体表信息
    unsigned char* data = ttfBuffer;
    uint16_t numTables = (data[4] << 8) | data[5];
    
    Serial.printf("字体表数量: %d\n", numTables);
    
    for (int i = 0; i < numTables; i++) {
      char tag[5] = {0};
      memcpy(tag, &data[12 + i*16], 4);
      
      uint32_t offset = (data[12 + i*16 + 8] << 24) | (data[12 + i*16 + 9] << 16) |
                        (data[12 + i*16 + 10] << 8) | data[12 + i*16 + 11];
      
      uint32_t length = (data[12 + i*16 + 12] << 24) | (data[12 + i*16 + 13] << 16) |
                        (data[12 + i*16 + 14] << 8) | data[12 + i*16 + 15];
      
      Serial.printf("表 '%s': 偏移=0x%08X, 长度=0x%08X\n", tag, offset, length);
    }
  }
};

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(100);
  }

  // 初始化SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS初始化失败!");
    while(1);
  }

  // 创建TTF渲染器
  TTFRenderer renderer;
  if (!renderer.open("/simhei.ttf")) {
    Serial.println("打开字体文件失败!");
    while(1);
  }
  
  // 显示字体信息
  renderer.getFontMetrics();
  
  // 测量渲染时间
  uint32_t startTime = millis();
  
  // 渲染"好"字
  String ch ="阖";
  uint32_t unicode = getChineseUnicode(ch);
  
  // 渲染字符
  int w, h, xoff, yoff;
  uint8_t* bitmap = renderer.renderCharacter(unicode, 60, &w, &h, &xoff, &yoff);
  
  uint32_t endTime = millis();
  
  if (bitmap) {
    Serial.printf("字符 '%s' (U+%04X) 渲染成功，大小: %d x %d, 渲染时间: %dms\n", ch.c_str(), unicode, w, h, endTime - startTime);
    
    // 打印位图
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        Serial.print(bitmap[y * w + x] > 0 ? "#" : " ");
      }
      Serial.println();
    }
    
    // 释放位图
    STBTT_FREE(bitmap);
  } else {
    Serial.printf("字符 '%s' (U+%04X) 渲染失败\n", ch.c_str(), unicode);
  }
  
  // 关闭渲染器
  renderer.close();
}

void loop() {
  // 空循环
}