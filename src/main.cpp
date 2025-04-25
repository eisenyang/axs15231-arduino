#include <LittleFS.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd.h"
#include <TFT_eSPI.h>
#include "TruetypeManager.h"
#define MY_TTF "/AlibabaPuHuiTi-3-35-Thin.ttf"
#define SCREEN_ROW 1 // 开屏行数
#define SCROLL_ROW 1 // 滚屏行数
#define ENABLE_SERIAL 0 // 是否开启串口打印
SpriteTextManager spriteTextManager;
TruetypeManager truetypeManager;
// 定义定时器句柄
TimerHandle_t xTimer;

void initSprite()
{
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

void handleTruetype(uint8_t *framebuffer, uint16_t y)
{

  static uint16_t address = LCD_HEIGHT;
  if (address <= 0)
  {
    address = LCD_HEIGHT;
  }

  static uint16_t top_offset = 0;
  

    if (y%SCREEN_ROW == 0)
    {
      if(ENABLE_SERIAL){
        Serial.printf("<======================%d行滚屏运行开始======================>",SCREEN_ROW);
      }
      uint16_t y_offset = address-1;
      
      spriteTextManager.setRowAddress(top_offset, y_offset,SCREEN_ROW);
      if(ENABLE_SERIAL){
        Serial.printf("%d行写入开始，当前写入的行:%s",SCREEN_ROW,String(y));
      }
      spriteTextManager.enableWriteColor();
    }

    // 一个循环就是一行
    for (int x = 0; x < LCD_WIDTH; x++)
    {
      if (y < HEIGHT_PIXELS)
      {
        
        // 每一列，写入颜色
        if(x<WIDTH_PIXELS){
          uint16_t color = truetypeManager.getPixelColor(framebuffer, x, y);
          spriteTextManager.writeColor(color);
        }else{
          spriteTextManager.writeColor(TFT_BLACK);
        }
        
        if(x==0){
          if(ENABLE_SERIAL){
            Serial.println("开始写入行:"+String(y)+",写入列:"+String(x));
          }
        }
        if(x==LCD_WIDTH-1){
          if(ENABLE_SERIAL){
            Serial.println("结束写入行:"+String(y)+",写入列:"+String(x));
          }
        }
        
      }
      else if (y >= HEIGHT_PIXELS)
      {
        // 一行结束，重新初始化
        y = 0;
        spriteTextManager.clearSprite();
      }
    }

    
    if (y%SCREEN_ROW == SCREEN_ROW-1)
    {

      spriteTextManager.disableWriteColor();
      if(ENABLE_SERIAL){
        Serial.printf("%d行写入结束，当前写结束的行:%s",SCREEN_ROW,String(y));
        Serial.println("开始滚动到:"+String(address));
        Serial.printf("<======================%d行滚屏运行结束======================>",SCREEN_ROW);
      }
      spriteTextManager.scrollStart(address);
      address = address - SCROLL_ROW;
    }
   
}

void TaskTruetypeHandle(TimerHandle_t pxTimer)
{
  static uint16_t y = 0;
  static uint8_t availableIndex = 0;
  framebuffer_t *framebuffer_t = truetypeManager.getFramebuffer(availableIndex);
  uint8_t *framebuffer = framebuffer_t->framebuffer;
  if (!framebuffer_t->hasData)
  {
    return;
  }
  handleTruetype(framebuffer, y);
  if (y >= HEIGHT_PIXELS)
  {
    y = 0;
    truetypeManager.resetFramebuffer(availableIndex);
    if (availableIndex == 0)
    {
      availableIndex = 1;
    }
    else
    {
      availableIndex = 0;
    }
  }
  y++;
}

void TaskTruetypeRead(void *pvParameters)
{

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(2); // 1秒
  // 获取当前时间（系统启动后的 tick 数）
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    truetypeManager.readTextToFramebuffer();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(100);
  }
  initSprite();

  while (!truetypeManager.initTruetype(MY_TTF, nullptr))
  {
    Serial.println("initTruetype failed");
    delay(100);
  }
  //String draw_strings[] = {"可"};
  //static String draw_string[] = {"I","L","Y","H"};
  //static String draw_string[] = {"我","爱","你","!"};
  static String draw_string[] = {
    "書", "藝", "以", "無", "所", "得", "故", "，", "菩", "提", "薩", "埵", "，", "依", "般", "若", "波", "羅", "蜜", "多", "故", "，", "心", "無", "罣", "礙", "；", "無", "罣", "礙", "故", "，", "無", "有", "恐", "怖", "，", "遠", "離", "顛", "倒", "夢", "想", "，", "究", "竟", "涅", "槃", "。", "三", "世", "諸", "佛", "，", "依", "般", "若", "波", "羅", "蜜", "多", "故", "，", "得", "阿", "耨", "多", "羅", "三", "藐", "三", "菩", "提", "。"
  };
  
  truetypeManager.setDrawString(draw_string);
  xTaskCreatePinnedToCore(TaskTruetypeRead, "TaskTruetypeRead", 8048, NULL, 1, NULL, 1);
  xTimer = xTimerCreate("MyTimer", pdMS_TO_TICKS(5), pdTRUE, NULL, TaskTruetypeHandle);
  if (xTimer == NULL)
  {
    Serial.println("定时器创建失败！");
  }
  else
  {
    Serial.println("定时器创建成功！");
  }

  if (xTimer != NULL)
  {
    if (xTimerStart(xTimer, 0) != pdPASS)
    {
      Serial.println("定时器启动失败！");
    }
  }
}

void loop()
{
  // readTruetype(0);
  // handleTruetype();
  ;
}