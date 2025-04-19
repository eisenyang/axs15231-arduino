#include <FS.h>
#include <SPIFFS.h>

// 安全配置STB
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STBTT_MALLOC(size) heap_caps_malloc(size, MALLOC_CAP_8BIT)
#define STBTT_FREE(ptr) heap_caps_free(ptr)
#include "stb_truetype.h"
// 获取单个汉字的Unicode十六进制值
uint16_t getChineseUnicode(const String& character) {
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
void setup() {
  Serial.begin(115200);
  while (!Serial)
  {
      delay(100);
  }

  // 初始化带错误检查的SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS初始化失败，请检查：");
    Serial.println("1. 开发板是否支持SPIFFS");
    Serial.println("2. 分区表配置是否正确");
    while(1);
  }

  // 检查字体文件
  //File fontFile = SPIFFS.open("/simhei_subset.ttf", "r");
  File fontFile = SPIFFS.open("/simhei_subset.ttf", "r");
  if (!fontFile) {
    Serial.println("字体文件未找到，请执行：");
    Serial.println("1. 将simhei_subset.ttf放入data目录");
    Serial.println("2. 运行'pio run -t uploadfs'");
    while(1);
  }

  // 安全加载字体
  size_t fontSize = fontFile.size();
  uint8_t* fontData = (uint8_t*)heap_caps_malloc(fontSize, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
  
  if (!fontData) {
    Serial.printf("内存不足! 需要%.2fKB内存\n", fontSize/1024.0);
    fontFile.close();
    while(1);
  }


  ulong start = millis();
  // 读取文件内容
  size_t bytesRead = fontFile.read(fontData, fontSize);
  fontFile.close();

  if (bytesRead != fontSize) {
    Serial.println("文件读取不完整，可能损坏");
    heap_caps_free(fontData);
    while(1);
  }

  // 初始化字体
  stbtt_fontinfo font;
  if (!stbtt_InitFont(&font, fontData, 0)) {
    Serial.println("STB初始化失败，可能原因：");
    Serial.println("1. 字体文件损坏");
    Serial.println("2. 不支持的字体特性");
    heap_caps_free(fontData);
    while(1);
  }

  // 验证字体参数
  int ascent, descent;
  stbtt_GetFontVMetrics(&font, &ascent, &descent, nullptr);
  Serial.printf("字体加载成功！Ascent:%d Descent:%d\n", ascent, descent);

  // 示例：渲染"你好"
  uint32_t unicode = getChineseUnicode("好");
  
  
  int w, h, xoff, yoff;
  uint8_t* bitmap = stbtt_GetCodepointBitmap(&font, 
    stbtt_ScaleForPixelHeight(&font, 60),
    stbtt_ScaleForPixelHeight(&font, 60),
    unicode, &w, &h, &xoff, &yoff);



  ulong end = millis();
  Serial.printf("渲染时间: %dms\n", end - start);
  // 打印结果
  //Serial.printf("\n'好'字(%dx%d):\n", w, h);
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      //Serial.print(bitmap[y*w + x] > 80 ? "#" : " ");
      Serial.print(bitmap[y*w + x] > 0 ? "#" : " ");
    }
    Serial.println();
  }

  // 清理资源
  stbtt_FreeBitmap(bitmap, nullptr);
  heap_caps_free(fontData);
}

void loop() {}