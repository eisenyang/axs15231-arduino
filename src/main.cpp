#include <LittleFS.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd.h"
#include <TFT_eSPI.h>
#include "TruetypeManager.h"
#define MY_TTF "/simhei.ttf"

SpriteTextManager spriteTextManager;
TruetypeManager truetypeManager;
// 定义定时器句柄
TimerHandle_t xTimer;
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


void handleTruetype(uint8_t *framebuffer,uint16_t y)
{

    static uint16_t address = LCD_HEIGHT;
    if (address <= 0)
    {
        address = LCD_HEIGHT;
    }    
    
    static uint16_t top_offset = 2;
    uint16_t y_offset = address-1;
    //一个循环就是一行
    for (int x = 0; x < WIDTH_PIXELS ; x++)
    {
        if (y < HEIGHT_PIXELS)
        {
            uint16_t color = truetypeManager.getPixelColor(framebuffer, x, y);
            //Serial.println("color:"+String(color));
            if (x == 0)
            {
              //每一行开始
              spriteTextManager.setRowAddress(top_offset, y_offset);
              spriteTextManager.enableWriteColor();
            }
            //每一列，写入颜色
            spriteTextManager.writeColor(color);
            if (x == WIDTH_PIXELS - 1)
            {   //每一行结束
                spriteTextManager.disableWriteColor();
            }

        }else if (y >= HEIGHT_PIXELS)
        {
            //一行结束，重新初始化
            y = 0;
            spriteTextManager.clearSprite();
        }
    }
    spriteTextManager.scrollStart(address);
    address--;

}



void TaskTruetypeHandle(TimerHandle_t pxTimer)
{
    static uint16_t y = 0;
    static uint8_t availableIndex = 0;
    framebuffer_t *framebuffer_t = truetypeManager.getFramebuffer(availableIndex);
    uint8_t *framebuffer = framebuffer_t->framebuffer;
    if(!framebuffer_t->hadData){
      return;
    }
    handleTruetype(framebuffer, y);
    if(y>=HEIGHT_PIXELS){
      y = 0;
      truetypeManager.resetFramebuffer(availableIndex);
      if(availableIndex == 0){
        availableIndex = 1;
      }else{
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
    while(1){
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

    while(!truetypeManager.initTruetype(MY_TTF, nullptr)){
      Serial.println("initTruetype failed");
      delay(100);
    }
    xTaskCreatePinnedToCore(TaskTruetypeRead, "TaskTruetypeRead", 8048, NULL, 1, NULL, 1); 
    //xTaskCreatePinnedToCore(TaskTruetypeHandle, "TaskTruetypeHandle", 8048, NULL, 1, NULL, 0); // 在核心0上运行
    // 参数：名称，周期(以ticks为单位)，是否自动重载，参数，回调函数
    xTimer = xTimerCreate("MyTimer", pdMS_TO_TICKS(5), pdTRUE, NULL, TaskTruetypeHandle);
    if(xTimer == NULL) {
      Serial.println("定时器创建失败！");
    } else {
      Serial.println("定时器创建成功！");
    }
  
    if(xTimer != NULL) {
      if(xTimerStart(xTimer, 0) != pdPASS) {
        Serial.println("定时器启动失败！");
      }
    }
}

void loop()
{
    //readTruetype(0);
    //handleTruetype();
    ;
}