#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

// 安全配置STB (确保只包含一次)
#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#endif
#ifndef STBTT_STATIC
#define STBTT_STATIC
#endif
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

// 高级TTF渲染器类
class AdvancedTTFRenderer {
private:
  // 字体缓冲区和信息
  unsigned char* ttfBuffer;
  size_t bufferSize;
  stbtt_fontinfo fontInfo;
  bool fontLoaded;
  
public:
  AdvancedTTFRenderer() : ttfBuffer(nullptr), bufferSize(0), fontLoaded(false) {}
  
  ~AdvancedTTFRenderer() {
    close();
  }
  
  bool open(const char* filename) {
    // 清理先前的资源
    close();
    
    // 打开字体文件
    File fontFile = SPIFFS.open(filename, "r");
    if (!fontFile) {
      Serial.println("无法打开字体文件");
      return false;
    }
    
    // 获取文件大小
    size_t fileSize = fontFile.size();
    Serial.printf("字体文件大小: %.2f MB\n", fileSize / (1024.0 * 1024.0));
    
    // 为避免内存限制，我们尝试用更智能的方法加载字体
    // 首先读取字体的前16KB，这通常包含字体目录和关键表
    size_t initialSize = 16 * 1024;
    unsigned char* initialBuffer = (unsigned char*)heap_caps_malloc(initialSize, MALLOC_CAP_8BIT);
    if (!initialBuffer) {
      Serial.println("内存分配失败");
      fontFile.close();
      return false;
    }
    
    fontFile.seek(0);
    size_t bytesRead = fontFile.read(initialBuffer, initialSize);
    if (bytesRead != initialSize) {
      Serial.println("读取字体文件初始部分失败");
      heap_caps_free(initialBuffer);
      fontFile.close();
      return false;
    }
    
    // 检查并获取表的数量
    uint16_t numTables = (initialBuffer[4] << 8) | initialBuffer[5];
    Serial.printf("字体表数量: %d\n", numTables);
    
    // 计算所需的缓冲区大小:
    // 12字节表目录头 + numTables * 16字节表条目
    size_t directorySize = 12 + numTables * 16;
    
    // 分配足够大的最终缓冲区
    // 我们尝试加载更大的部分，包括关键表和常用字符的字形数据
    bufferSize = min(fileSize, (size_t)(4 * 1024 * 1024)); // 最大4MB
    ttfBuffer = (unsigned char*)heap_caps_malloc(bufferSize, MALLOC_CAP_8BIT);
    if (!ttfBuffer) {
      Serial.println("无法分配字体缓冲区");
      heap_caps_free(initialBuffer);
      fontFile.close();
      return false;
    }
    
    // 复制已读取的初始数据
    memcpy(ttfBuffer, initialBuffer, initialSize);
    heap_caps_free(initialBuffer);
    
    // 读取剩余部分
    fontFile.seek(initialSize);
    size_t remainingBytes = fontFile.read(ttfBuffer + initialSize, bufferSize - initialSize);
    fontFile.close();
    
    if (remainingBytes != bufferSize - initialSize) {
      Serial.println("读取字体文件剩余部分失败");
      heap_caps_free(ttfBuffer);
      ttfBuffer = nullptr;
      return false;
    }
    
    Serial.printf("已读取 %.2f MB 字体数据\n", bufferSize / (1024.0 * 1024.0));
    
    // 初始化STB字体信息
    if (stbtt_InitFont(&fontInfo, ttfBuffer, 0) == 0) {
      Serial.println("初始化字体失败");
      heap_caps_free(ttfBuffer);
      ttfBuffer = nullptr;
      return false;
    }
    
    // 提取字体元数据
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
    Serial.printf("字体度量: 上升=%d, 下降=%d, 行间距=%d\n", ascent, descent, lineGap);
    
    fontLoaded = true;
    Serial.println("字体加载成功");
    
    return true;
  }
  
  void close() {
    if (ttfBuffer) {
      heap_caps_free(ttfBuffer);
      ttfBuffer = nullptr;
    }
    
    fontLoaded = false;
    bufferSize = 0;
  }
  
  // 带有锐化处理的位图渲染
  uint8_t* renderCharacterSharpened(uint32_t codepoint, float pixelHeight, int* width, int* height, int* xoff, int* yoff) {
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
    
    // 使用稍微更高的过采样率，稍后我们会缩小以实现更清晰的边缘
    float oversampleScale = 2.0f;
    float scale = stbtt_ScaleForPixelHeight(&fontInfo, pixelHeight * oversampleScale);
    
    // 获取字形边界框
    int ix0, iy0, ix1, iy1;
    stbtt_GetGlyphBitmapBox(&fontInfo, glyphIndex, scale, scale, &ix0, &iy0, &ix1, &iy1);
    
    // 计算过采样位图尺寸
    int overW = ix1 - ix0;
    int overH = iy1 - iy0;
    
    // 确保位图尺寸合理
    if (overW <= 0) overW = pixelHeight * oversampleScale;
    if (overH <= 0) overH = pixelHeight * oversampleScale;
    
    // 计算最终位图尺寸
    int w = overW / oversampleScale;
    int h = overH / oversampleScale;
    
    Serial.printf("字形尺寸: %d x %d\n", w, h);
    
    // 创建过采样位图
    uint8_t* overBitmap = (uint8_t*)heap_caps_malloc(overW * overH, MALLOC_CAP_8BIT);
    if (!overBitmap) {
      Serial.println("无法分配过采样位图内存");
      return nullptr;
    }
    
    // 初始化过采样位图
    memset(overBitmap, 0, overW * overH);
    
    // 渲染字形到过采样位图
    stbtt_MakeGlyphBitmap(&fontInfo, overBitmap, overW, overH, overW, scale, scale, glyphIndex);
    
    // 创建最终位图
    uint8_t* bitmap = (uint8_t*)heap_caps_malloc(w * h, MALLOC_CAP_8BIT);
    if (!bitmap) {
      Serial.println("无法分配最终位图内存");
      heap_caps_free(overBitmap);
      return nullptr;
    }
    
    // 对过采样位图进行下采样并应用锐化
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        int sum = 0;
        int count = 0;
        
        // 对2x2区域进行平均
        for (int sy = 0; sy < oversampleScale; sy++) {
          for (int sx = 0; sx < oversampleScale; sx++) {
            int ox = x * oversampleScale + sx;
            int oy = y * oversampleScale + sy;
            if (ox < overW && oy < overH) {
              sum += overBitmap[oy * overW + ox];
              count++;
            }
          }
        }
        
        // 计算平均值
        int avg = (count > 0) ? (sum / count) : 0;
        
        // 应用锐化 - 简单的阈值处理
        if (avg > 200) avg = 255;
        else if (avg < 50) avg = 0;
        
        bitmap[y * w + x] = avg;
      }
    }
    
    // 释放过采样位图
    heap_caps_free(overBitmap);
    
    // 设置输出参数
    *width = w;
    *height = h;
    *xoff = ix0 / oversampleScale;
    *yoff = iy0 / oversampleScale;
    
    return bitmap;
  }
  
  // 标准位图渲染
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
    
    // 直接使用stb库提供的功能渲染字形为位图
    return stbtt_GetGlyphBitmap(&fontInfo, scale, scale, glyphIndex, width, height, xoff, yoff);
  }
  
  // 获取汉字信息
  void getCharInfo(uint32_t codepoint) {
    if (!fontLoaded) {
      Serial.println("字体未加载");
      return;
    }
    
    int glyphIndex = stbtt_FindGlyphIndex(&fontInfo, codepoint);
    if (glyphIndex == 0) {
      Serial.printf("未找到字符 U+%04X 的字形\n", codepoint);
      return;
    }
    
    // 获取字形轮廓
    stbtt_vertex* vertices = NULL;
    int numVertices = stbtt_GetGlyphShape(&fontInfo, glyphIndex, &vertices);
    
    Serial.printf("字符 U+%04X (索引 %d) 有 %d 个轮廓点\n", codepoint, glyphIndex, numVertices);
    
    // 检查是否是复合字形
    // 注意：这不是直接检查的标准方法，但可以根据轮廓点数量和类型推断
    bool isComplex = false;
    int moveCount = 0;
    
    for (int i = 0; i < numVertices; i++) {
      if (vertices[i].type == STBTT_vmove) {
        moveCount++;
      }
    }
    
    isComplex = (moveCount > 5 || numVertices > 50);
    Serial.printf("字符复杂度: %s (移动命令: %d, 总点数: %d)\n", 
                 isComplex ? "复杂" : "简单", moveCount, numVertices);
    
    // 释放顶点 - 修复宏问题
    if (vertices) {
      heap_caps_free(vertices);
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

  // 创建高级TTF渲染器
  AdvancedTTFRenderer renderer;
  if (!renderer.open("/simhei.ttf")) {
    Serial.println("打开字体文件失败!");
    while(1);
  }
  
  // 要渲染的汉字
  String characters[] = {"阖", "人", "福"};
  int numChars = sizeof(characters) / sizeof(characters[0]);
  
  // 渲染并显示每个汉字
  for (int i = 0; i < numChars; i++) {
    String ch = characters[i];
    uint32_t unicode = getChineseUnicode(ch);
    
    Serial.printf("\n========= 渲染汉字: %s (U+%04X) =========\n", ch.c_str(), unicode);
    
    // 获取字符信息
    renderer.getCharInfo(unicode);
    
    // 测量渲染时间
    uint32_t startTime = millis();
    
    // 使用锐化渲染提高复杂字符的清晰度
    int w, h, xoff, yoff;
    uint8_t* bitmap = renderer.renderCharacterSharpened(unicode, 60, &w, &h, &xoff, &yoff);
    
    uint32_t endTime = millis();
    
    if (bitmap) {
      Serial.printf("字符 '%s' (U+%04X) 渲染成功, 尺寸: %d x %d, 耗时: %dms\n", 
                   ch.c_str(), unicode, w, h, endTime - startTime);
      
      // 打印位图
      for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
          Serial.print(bitmap[y * w + x] > 127 ? "#" : " ");
        }
        Serial.println();
      }
      
      // 释放位图
      heap_caps_free(bitmap);
    } else {
      Serial.printf("字符 '%s' (U+%04X) 渲染失败\n", ch.c_str(), unicode);
    }
  }
  
  // 关闭渲染器
  renderer.close();
}

void loop() {
  // 空循环
}