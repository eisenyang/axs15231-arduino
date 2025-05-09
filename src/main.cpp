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

static ulong teTime = millis();
static uint16_t address = LCD_HEIGHT;
SemaphoreHandle_t xMutex;
// 添加在全局变量区域
volatile unsigned long lastInterruptTime = 0;
volatile unsigned long interruptInterval = 0;
volatile uint32_t interruptCount = 0;
#define MAX_COUNTS 100
volatile bool needPrintStats = false;

typedef struct{
  uint16_t y_scroll_offset = 0;
  uint8_t availableIndex = 0;
  bool scrollCompleted = true;
  bool readCompleted = false;
  bool teEnabled = false;
}scroll_info_t;
scroll_info_t scroll_info;
void printInterruptTime(){
  if (needPrintStats && scroll_info.scrollCompleted) {
    Serial.println("------------------------");
    Serial.print("中断间隔时间(微秒): ");
    Serial.println(interruptInterval);
    Serial.print("中断频率(Hz): ");
    Serial.println(1000000.0 / interruptInterval);
    Serial.println("------------------------");
    needPrintStats = false;
  }
}
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
void srcoll_screen(){
  if(scroll_info.teEnabled && scroll_info.readCompleted){
    scroll_info.teEnabled = false;
    scroll_info.readCompleted = false;
    if(address <= 0){
      address = LCD_HEIGHT;
    }
    spriteTextManager.scrollStart(address);
    address = address - SCROLL_ROW;

    if (scroll_info.y_scroll_offset >= HEIGHT_PIXELS)
    {
      scroll_info.y_scroll_offset = 0;
      truetypeManager.resetFramebuffer(scroll_info.availableIndex);
      if (scroll_info.availableIndex == 0)
      {
        scroll_info.availableIndex = 1;
      }
      else
      {
        scroll_info.availableIndex = 0;
      }
    }
    scroll_info.y_scroll_offset++;
    scroll_info.scrollCompleted = true;
  }else{
    scroll_info.teEnabled = false;
  }
}
void writeBufToScreen(uint8_t *framebuffer, uint16_t y)
{
  static uint16_t top_offset = 0;

    // if (address <= 0)
    // {
    //   address = LCD_HEIGHT;
    // }
    if (y%SCREEN_ROW == 0)
    {
      if(ENABLE_SERIAL){
        Serial.printf("<======================%d行滚屏运行开始======================>",SCREEN_ROW);
      }
      //uint16_t y_offset = address-1;
      
      spriteTextManager.setRowAddress(top_offset, address,SCREEN_ROW);
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
        //Serial.println("开始滚动到:"+String(address));
        Serial.printf("<======================%d行滚屏运行结束======================>",SCREEN_ROW);
      }
      //readCompleted = true;
      //spriteTextManager.scrollStart(address);
      //address = address - SCROLL_ROW;
  }
  
   
}

void TaskReadBuf2Screen(TimerHandle_t pxTimer)
{

  //printInterruptTime();
  if(xSemaphoreTake(xMutex, 0) == pdTRUE){
    if(!scroll_info.scrollCompleted){
      xSemaphoreGive(xMutex);
      return;
    }
    ulong startTime = micros();
    
    
    framebuffer_t *framebuffer_t = truetypeManager.getFramebuffer(scroll_info.availableIndex);
    uint8_t *framebuffer = framebuffer_t->framebuffer;
    if (!framebuffer_t->hasData)
    {
      xSemaphoreGive(xMutex);
      return;
    }
    writeBufToScreen(framebuffer, scroll_info.y_scroll_offset);
    // if (y_scroll_offset >= HEIGHT_PIXELS)
    // {
    //   y_scroll_offset = 0;
    //   truetypeManager.resetFramebuffer(availableIndex);
    //   if (availableIndex == 0)
    //   {
    //     availableIndex = 1;
    //   }
    //   else
    //   {
    //     availableIndex = 0;
    //   }
    // }
    // y_scroll_offset++;
    ulong currentTime = micros();
    //Serial.printf("run micros:%f\n",(currentTime-startTime)/1000.0);
    scroll_info.readCompleted = true;
    xSemaphoreGive(xMutex);
  }
}

void TaskScrollScreen(void *pvParameters)
{
  while (1)
  {
    //
    if (xSemaphoreTake(xMutex, 0) == pdTRUE)
    {
      //printInterruptTime();
      srcoll_screen();
      xSemaphoreGive(xMutex);
    }
    //vTaskDelay(pdMS_TO_TICKS(2));
  }
}
void TaskReadBufFromTruetype(void *pvParameters)
{

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(20); // 2毫秒
  // 获取当前时间（系统启动后的 tick 数）
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    truetypeManager.readTextToFramebuffer();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    //vTaskDelay(pdMS_TO_TICKS(2));
  }
}
void IRAM_ATTR handleRisingEdge() {
  if (xSemaphoreTake(xMutex, 0) == pdTRUE) {  
    //scrollEnabled = true;
    scroll_info.teEnabled = true;
    xSemaphoreGive(xMutex);
  }


  unsigned long currentTime = micros(); // 使用micros()获取微秒级精度
  
  if (lastInterruptTime > 0) {
    interruptInterval = currentTime - lastInterruptTime;
  }
  
  lastInterruptTime = currentTime;
  interruptCount++;
  
  if (interruptCount >= MAX_COUNTS) {
    needPrintStats = true;
    interruptCount = 0;
  }
}
void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(100);
  }

  xMutex = xSemaphoreCreateMutex();
  static ulong lastTime = millis();
  initSprite();
  attachInterrupt(digitalPinToInterrupt(PIN_NUM_TE), handleRisingEdge, RISING);
  while (!truetypeManager.initTruetype(MY_TTF, nullptr))
  {
    Serial.println("initTruetype failed");
    delay(100);
  }
  //String draw_strings[] = {"可"};
  //static String draw_string[] = {"I","L","Y","H"};
  //static String draw_string[] = {"我","爱","你","!"};
  static const char draw_string[] = 
  "觀自在菩薩，行深般若波羅蜜多時，照見五蘊皆空，度一切苦厄。"
  "舍利子，色不異空，空不異色，色即是空，空即是色，受想行識亦復如是。"
  "舍利子，是諸法空相，不生不滅，不垢不淨，不增不減。"
  "是故空中無色，無受想行識，無眼耳鼻舌身意，無色聲香味觸法，無眼界乃至無意識界，無無明亦無無明盡，乃至無老死，亦無老死盡，無苦集滅道，無智亦無得，以無所得故。"
  "菩提薩埵，依般若波羅蜜多故，心無掛礙；無掛礙故，無有恐怖，遠離顛倒夢想，究竟涅槃。"
  "三世諸佛，依般若波羅蜜多故，得阿耨多羅三藐三菩提。"
  "故知般若波羅蜜多，是大神咒，是大明咒，是無上咒，是無等等咒，能除一切苦，真實不虛。"
  "故說般若波羅蜜多咒，即說咒曰：揭諦揭諦，波羅揭諦，波羅僧揭諦，菩提薩婆訶。!!!";
  
  truetypeManager.setDrawString(draw_string);
  xTaskCreatePinnedToCore(TaskReadBufFromTruetype, "TaskReadBufFromTruetype", 8048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskScrollScreen, "TaskScrollScreen", 8048, NULL, 1, NULL, 1);
  if(true){
    xTimer = xTimerCreate("MyTimer", pdMS_TO_TICKS(5), pdTRUE, NULL, TaskReadBuf2Screen);
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
  
}

void loop()
{
  // readTruetype(0);
  // handleTruetype();
  ;

  // if (needPrintStats) {
  //   Serial.println("------------------------");
  //   Serial.print("中断间隔时间(微秒): ");
  //   Serial.println(interruptInterval);
  //   Serial.print("中断频率(Hz): ");
  //   Serial.println(1000000.0 / interruptInterval);
  //   Serial.println("------------------------");
  //   needPrintStats = false;
  // }
}