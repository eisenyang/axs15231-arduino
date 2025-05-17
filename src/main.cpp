#include <LittleFS.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd_spi_api.h"
#include <TFT_eSPI.h>
#include "TruetypeManager.h"
#define MY_TTF "/AlibabaPuHuiTi-3-35-Thin.ttf"
#define SCREEN_ROW 1 // 开屏行数
#define SCROLL_ROW 1 // 滚屏行数
#define ENABLE_SERIAL 0 // 是否开启串口打印
void srcoll_screen(void);
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
volatile int64_t te_start_time = esp_timer_get_time();


// 2. 定义统一的函数指针类型
//typedef void (*FuncPtr)(FuncParam);


typedef struct{
  uint16_t y_scroll_offset = 0;
  uint8_t availableIndex = 0;
  bool scrollCompleted = true;
  bool readCompleted = false;
  bool scrollInit = false;
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

void setAvailableIndex(scroll_info_t &scroll_info){
  if(scroll_info.availableIndex >= BUF_COUNT - 1){
    scroll_info.availableIndex = 0;
  }
  else{
    scroll_info.availableIndex++;
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

void scroll_init(uint16_t line_num,spi_transaction_t &t){
    scroll_info.readCompleted = false;
    if(address <= 0){
      address = LCD_HEIGHT;
    }
    lcd_spi_write_cmd(0x37);    
    // 一次传输完成
    lcd_spi_set_cs_low();
    uint8_t data[2] = {(uint8_t)(line_num >> 8), (uint8_t)(line_num & 0xff)};
    memset(&t, 0, sizeof(t));
    t.length = 16;  // 一次传输所有数据
    t.tx_buffer = data;
    t.user = (void*)1; 
}

void scroll_completed(spi_transaction_t &t){
  lcd_spi_scroll_end(t);
  lcd_spi_set_cs_high();
  address = address - SCROLL_ROW;
  ulong currentTime = micros();
  if (scroll_info.y_scroll_offset >= HEIGHT_PIXELS)
  {
    scroll_info.y_scroll_offset = 0;
    truetypeManager.resetFramebuffer(scroll_info.availableIndex);
    setAvailableIndex(scroll_info);
  }
  scroll_info.y_scroll_offset++;
  scroll_info.scrollCompleted = true;
}

void writeBufToScreen(uint8_t *framebuffer, uint16_t y)
{
  static uint16_t top_offset = 0;
    if (y%SCREEN_ROW == 0)
    {
      if(ENABLE_SERIAL){
        Serial.printf("<======================%d行滚屏运行开始======================>",SCREEN_ROW);
      }
      
      spriteTextManager.setRowAddress(top_offset, address,SCREEN_ROW);
      if(ENABLE_SERIAL){
        Serial.printf("%d行写入开始，当前写入的行:%s",SCREEN_ROW,String(y));
      }
    }

     // 一次准备一整行的数据
    
    static uint16_t color_buffer[LCD_WIDTH];  // 用于批量传输的缓冲区

    for (int x = 0; x < LCD_WIDTH; x++)
    {
        if (y < HEIGHT_PIXELS && x < WIDTH_PIXELS)
        {
            color_buffer[x] = truetypeManager.getPixelColor(framebuffer, x, y);
        }
        else
        {
            color_buffer[x] = TFT_BLACK;
        }
    }
    
    // 批量传输一整行
    spriteTextManager.writeColors(color_buffer, LCD_WIDTH);

    if (y%SCREEN_ROW == SCREEN_ROW-1)
    {
        //spriteTextManager.disableWriteColor();
        if(ENABLE_SERIAL){
            Serial.printf("%d行写入结束，当前写结束的行:%s",SCREEN_ROW,String(y));
            Serial.printf("<======================%d行滚屏运行结束======================>",SCREEN_ROW);
        }
    }
    if (y >= HEIGHT_PIXELS)
    {
      // 一行结束，重新初始化
      y = 0;
      spriteTextManager.clearSprite();
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
    ulong currentTime = micros();
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
  }
}
void TaskReadBufFromTruetype(void *pvParameters)
{

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(10); // 2毫秒
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
void srcoll_screen(){
  if(scroll_info.readCompleted){
    scroll_info.readCompleted = false;
    if(address <= 0){
      address = LCD_HEIGHT;
    }
    ulong startTime = micros();
    spriteTextManager.scrollStart(address);
    address = address - SCROLL_ROW;
    ulong currentTime = micros();
    if (scroll_info.y_scroll_offset >= HEIGHT_PIXELS)
    {
      scroll_info.y_scroll_offset = 0;
      truetypeManager.resetFramebuffer(scroll_info.availableIndex);
      setAvailableIndex(scroll_info);
    }
    scroll_info.y_scroll_offset++;
    scroll_info.scrollCompleted = true;
  }
}
void te_irs_task(void *pvParameters){

  if (xSemaphoreTake(xMutex, 0) == pdTRUE)
    {
      int64_t current_time = esp_timer_get_time();
      int64_t te_time = current_time - te_start_time;
      if(te_time > 2000){
        srcoll_screen();
        te_start_time = current_time;
      }
      xSemaphoreGive(xMutex);
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
  //attachInterrupt(digitalPinToInterrupt(PIN_NUM_TE), handleRisingEdge, RISING);
  lcd_spi_set_te_callback(te_irs_task);
  while (!truetypeManager.initTruetype(MY_TTF, nullptr))
  {
    Serial.println("initTruetype failed");
    delay(100);
  }
  //String draw_strings[] = {"可"};
  //static String draw_string[] = {"I","L","Y","H"};
  //static String draw_string[] = {"我","爱","你","!"};
  const char* draw_string = 
  "觀自在菩薩，行";
  const int m = 30;
  string_cache_t sc;
  if(!truetypeManager.init_classifier(&sc, draw_string, m)){
    Serial.println("init_classifier failed");
    return;
  }
  Serial.println(sc.group_num);

  truetypeManager.setDrawString(draw_string);
  xTaskCreatePinnedToCore(TaskReadBufFromTruetype, "TaskReadBufFromTruetype", 8048, NULL, 1, NULL, 1);
  //xTaskCreatePinnedToCore(TaskScrollScreen, "TaskScrollScreen", 8048, NULL, 1, NULL, 1);
  if(true){
    xTimer = xTimerCreate("MyTimer", pdMS_TO_TICKS(3), pdTRUE, NULL, TaskReadBuf2Screen);
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