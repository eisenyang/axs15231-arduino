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
static uint8_t handlerBufIndex = 1;
TruetypeManager truetypeManager;
// 定义消息结构体
typedef struct {
  uint8_t buf_id;   // 1 或 2
  uint8_t *buf_data; // 图片数据指针
} frameBufMessage_t;

// 声明队列句柄
QueueHandle_t frameBufQueue;
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


void handleTruetype(uint8_t *framebuffer)
{

    static uint16_t address = LCD_HEIGHT;
    if (address <= 0)
    {
        address = LCD_HEIGHT;
    }    
    
    static uint16_t top_offset = 2;
    for(int y = 0; y < HEIGHT_PIXELS; y++){
      //一个循环就是一行
      for (int x = 0; x < WIDTH_PIXELS ; x++)
      {
          if (y < HEIGHT_PIXELS)
          {
              uint16_t y_offset = address-1;
              uint16_t color = truetypeManager.getPixelColor(framebuffer, x, y);

              //Serial.println("color:"+String(color));
              if (x == 0)
              {
                //每一行开始
                //Serial.println("y_offset----->:"+String(y_offset));
                //Serial.println("top_offset----->:"+String(top_offset));
                spriteTextManager.setRowAddress(top_offset, y_offset);
                spriteTextManager.enableWriteColor();
              }
              //每一列，写入颜色
              spriteTextManager.writeColor(color);
              if (x == WIDTH_PIXELS - 1)
              {
                  //每一行结束
                  spriteTextManager.disableWriteColor();
              }
          }else if (y >= HEIGHT_PIXELS)
          {
              //一行结束，重新初始化
              y = 0;
              spriteTextManager.clearSprite();
          }
      }
      vTaskDelay(5);
      spriteTextManager.scrollStart(address);
      address--;
  }
}



void TaskFrontTypeHandle(void *pvParameters)
{
    frameBufMessage_t frameBufMessage;
    while(1){
        if (xQueueReceive(frameBufQueue, &frameBufMessage, portMAX_DELAY) == pdPASS) {
            uint8_t index = frameBufMessage.buf_id;
            uint8_t *framebuffer = frameBufMessage.buf_data;
            handlerBufIndex = index;
            handleTruetype(framebuffer);
        }
       //vTaskDelay(pdMS_TO_TICKS(1000));
        //delay(1000);
    }
}

void TaskFrontTypeRead(void *pvParameters)
{

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(20); // 1秒
    // 获取当前时间（系统启动后的 tick 数）
    xLastWakeTime = xTaskGetTickCount();
    while(1){
      //unsigned long startTime = micros();
        if(uxQueueMessagesWaiting(frameBufQueue) < QUEUE_LENGTH-1){
          if(handlerBufIndex == 1){
            uint8_t* buf = truetypeManager.readTextToFramebuffer(0);
            frameBufMessage_t  frameBufMessage;
            frameBufMessage.buf_id = 0;
            frameBufMessage.buf_data = buf;
            xQueueSend(frameBufQueue, &frameBufMessage, portMAX_DELAY);
            //delay(10000);
          }else{
            uint8_t* buf = truetypeManager.readTextToFramebuffer(1);
            frameBufMessage_t  frameBufMessage;
            frameBufMessage.buf_id = 1;
            frameBufMessage.buf_data = buf;
            xQueueSend(frameBufQueue, &frameBufMessage, portMAX_DELAY);
          }
        }
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
  
    frameBufQueue = xQueueCreate(QUEUE_LENGTH, sizeof(frameBufMessage_t)); 
    //truetypeManager.checkFileExists(MY_TTF);
    initSprite();

    while(!truetypeManager.initTruetype(MY_TTF, nullptr)){
      Serial.println("initTruetype failed");
      delay(100);
    }
    xTaskCreatePinnedToCore(TaskFrontTypeRead, "TaskFrontTypeRead", 8048, NULL, 1, NULL, 1); 
    xTaskCreatePinnedToCore(TaskFrontTypeHandle, "TaskFrontTypeHandle", 8048, NULL, 1, NULL, 0); // 在核心0上运行
    
}

void loop()
{
    //readTruetype(0);
    //handleTruetype();
    ;
}