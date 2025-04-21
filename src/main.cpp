#include <FS.h>
#include <SPIFFS.h>
#include <esp_spiram.h>
#include "SpriteTextManager.h"
// 安全配置STB
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#define STBTT_MALLOC(size) heap_caps_malloc(size, MALLOC_CAP_8BIT)
#define STBTT_FREE(ptr) heap_caps_free(ptr)
#include "stb_truetype.h"
#define MY_FONT_PATH  "/hkjgxjh.ttf"
SpriteTextManager spriteTextManager;
stbtt_fontinfo stbttFont;
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
bool initSPIFFS(){
  // 初始化带错误检查的SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS初始化失败，请检查：");
    Serial.println("1. 开发板是否支持SPIFFS");
    Serial.println("2. 分区表配置是否正确");
    return false;
  }

  if (psramFound()) {
    size_t psramSize = ESP.getPsramSize();
    Serial.print("PSRAM 可用，大小: ");
    Serial.print(psramSize / 1024 / 1024);
    Serial.println(" MB");
  } else {
    Serial.println("PSRAM 不可用或未启用");
    return false;
  }
  return true;
}

void initSprite(){
  spriteTextManager.init();
  spriteTextManager.fillScreen(TFT_BLACK);
  spriteTextManager.setScrollWindow(0, LCD_HEIGHT, 0);
  Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
  Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
  Serial.printf("Total heap: %d bytes\n", ESP.getHeapSize());
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
  Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
  Serial.printf("Flash Speed: %d Hz\n", ESP.getFlashChipSpeed());
}

bool initTruetype(stbtt_fontinfo *stbttFont,uint8_t* ttfData){
  // 初始化字体
  if (!stbtt_InitFont(stbttFont, ttfData, 0)) {
    Serial.println("STB初始化失败，可能原因：");
    Serial.println("1. 字体文件损坏");
    Serial.println("2. 不支持的字体特性");
    heap_caps_free(ttfData);
    return false;
    //..while(1);
  }
  // 验证字体参数
  int ascent, descent;
  stbtt_GetFontVMetrics(stbttFont, &ascent, &descent, nullptr);
  Serial.printf("字体加载成功！Ascent:%d Descent:%d\n", ascent, descent);
  return true;
}
uint8_t* loadTtfFont(){
  File ttfFile = SPIFFS.open(MY_FONT_PATH, "r");
  if (!ttfFile) {
    Serial.println("字体文件未找到，请执行：");
    Serial.println("1. hkjgxjh.ttf放入data目录");
    Serial.println("2. 运行'pio run -t uploadfs'");
    //while(1);
    return nullptr;
  }

  // 安全加载字体
  size_t ttfSize = ttfFile.size();
  if(ttfSize == 0){
    Serial.printf("字体文件大小为:0");
    ttfFile.close();
    return nullptr;
  }
  Serial.println("ttfSize: "+String(ttfSize));
  uint8_t* ttfData = (uint8_t*)ps_malloc(ttfSize);
  
  if (ttfData) {
    Serial.println("PSRAM 内存分配成功");
    // 使用内存（示例：填充0）
    memset(ttfData, 0, ttfSize);
          // 释放内存
    //ps_free(psramBuffer);
    // 释放内存
    //Serial.println("PSRAM 内存已释放");
  } else {
    Serial.printf("内存不足! 需要%.2fKB内存\n", ttfSize/1024.0);
    ttfFile.close();
    return nullptr;
  }


  
  // 读取文件内容
  size_t bytesRead = ttfFile.read(ttfData, ttfSize);
  ttfFile.close();

  if (bytesRead != ttfSize) {
    Serial.println("文件读取不完整，可能损坏");
    heap_caps_free(ttfData);
    return nullptr;
  }
  return ttfData;
}

uint8_t*  readTruetype(stbtt_fontinfo *stbttFont,uint16_t fontSize,int* w,int* h)
{
    //static String draw_string[] = {"以","无","所","得","故","，","菩","提","萨","埵","，","依","般","若","波","罗","蜜","多","故","，","心","无","罣","碍","；","无","罣","碍","故","，","无","有","恐","怖","，","远","离","颠","倒","梦","想","，","究","竟","涅","槃","。","三","世","诸","佛","，","依","般","若","波","罗","蜜","多","故","，","得","阿","耨","多","罗","三","藐","三","菩","提","。"};
    static String draw_string[] = {"你","好","世","界","欢","迎","来","到","美","丽","的","中","国"};
    static uint16_t draw_string_len = sizeof(draw_string) / sizeof(draw_string[0]);
    static uint16_t draw_string_index = 0;
    String str = draw_string[draw_string_index];
    uint16_t unicode = getChineseUnicode(str);
    draw_string_index++;
    if(draw_string_index >= draw_string_len){
        draw_string_index = 0;
    }    
    ulong start = millis();
    float scale = stbtt_ScaleForPixelHeight(stbttFont, fontSize);
    int xoff, yoff;
    uint8_t* bitmap = stbtt_GetCodepointBitmap(stbttFont, 
      scale,
      scale,
      unicode, w, h, &xoff, &yoff);
    ulong end = millis();
    Serial.printf("渲染时间: %dms\n", end - start);
    //memset(bitmap,0,*w**h);
    return bitmap;
}

void handleTruetype(uint8_t* bitmap,int w,int h)
{

    unsigned long startTime = micros();
    static uint16_t address = LCD_HEIGHT;
    if (address <= 0)
    {
        address = LCD_HEIGHT;
    }
    //memset(framebuffer, 0, FRAMEBUFFER_SIZE);
    //truetype.pushText();
    
    unsigned long startTimeTotal = micros();
    static uint16_t top_offset = 2;
    for(int y = 0; y < SPRITE_WIDTH; y++){
      //一个循环就是一行
      for (int x = 0; x < SPRITE_HEIGHT ; x++)
      {
          if (y < SPRITE_HEIGHT)
          {
              uint16_t y_offset = address-1;
              uint16_t color = TFT_BLACK;
              if (x<w-2 && y<=h-2)
              {
                color = bitmap[x*w + y];
              }
              
              //Serial.println("color:"+String(color));
              if (x == 0)
              {
                //每一行开始
                spriteTextManager.setRowAddress(top_offset, y_offset);
                spriteTextManager.enableWriteColor();
              }
              //每一列，写入颜色
              spriteTextManager.writeColor(color);
              if (x == SPRITE_WIDTH - 1)
              {
                  //每一行结束
                  spriteTextManager.disableWriteColor();
              }
          }else if (y >= SPRITE_HEIGHT)
          {
              //一行结束，重新初始化
              y = 0;
              spriteTextManager.clearSprite();
          }
      }
      delay(12);
      spriteTextManager.scrollStart(address);
      address--;
      
    }
}
void setup() {
  Serial.begin(115200);
  while (!Serial)
  {
      delay(100);
  }
  // 初始化带错误检查的SPIFFS
  if(!initSPIFFS()){
    while(1);
    delay(1000);
  }
  initSprite();

  uint8_t* ttfData = loadTtfFont();

  // 初始化字体
  
  if(!initTruetype(&stbttFont,ttfData)){
    while(1);
    delay(1000);
  }

  
  // ulong start = millis();
  // int w, h;
  // uint8_t* bitmap = readTruetype(&font,160,&w,&h);
  // Serial.printf("w:%d h:%d\n",w,h);
  // // 打印结果
  // Serial.printf("\n'好'字(%dx%d):\n", w, h);
  // for (int y=0; y<h; y++) {
  //   for (int x=0; x<w; x++) {
  //     //Serial.print(bitmap[y*w + x] > 80 ? "#" : " ");
  //     Serial.print(bitmap[y*w + x] > 0 ? "#" : " ");
  //   }
  //   Serial.println();
  // }

  // // 清理资源
  // stbtt_FreeBitmap(bitmap, nullptr);
  // heap_caps_free(ttfData);
}

void loop() {
  ulong start = millis();
  int w, h;
  uint8_t* bitmap = readTruetype(&stbttFont,160,&w,&h);
  Serial.printf("w:%d h:%d\n",w,h);
  // 打印结果
  // Serial.printf("\n'好'字(%dx%d):\n", w, h);
  // for (int y=0; y<h; y++) {
  //   for (int x=0; x<w; x++) {
  //     //Serial.print(bitmap[y*w + x] > 80 ? "#" : " ");
  //     Serial.print(bitmap[y*w + x] > 0 ? "#" : " ");
  //   }
  //   Serial.println();
  // }

  handleTruetype(bitmap,w,h);
  // 清理资源
  stbtt_FreeBitmap(bitmap, nullptr);
}