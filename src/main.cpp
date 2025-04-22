#include <LittleFS.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd.h"
#include <TFT_eSPI.h>
#include "truetype_Arduino.h"
#define BUF_COUNT  2
#define QUEUE_LENGTH 2
#define WIDTH_BYTES 20
#define HEIGHT_PIXELS 160
#define WIDTH_PIXELS 160
#define DISPLAY_HEIGHT HEIGHT_PIXELS
#define FRAMEBUFFER_SIZE (WIDTH_BYTES * HEIGHT_PIXELS)
#define BITS_PER_PIXEL 1 // either 1, 4, or 8
#define DISPLAY_WIDTH (WIDTH_BYTES * (8 / BITS_PER_PIXEL))
#define MY_TTF "/simhei.ttf"

static unsigned long startTime = millis();
static unsigned long count = 0;
SpriteTextManager spriteTextManager;
uint8_t *framebuffer[BUF_COUNT];
static uint8_t handlerBufIndex = 1;

// 定义消息结构体
typedef struct {
  uint8_t buf_id;   // 1 或 2
  uint8_t *buf_data; // 图片数据指针
} frameBufMessage_t;

// 声明队列句柄
QueueHandle_t frameBufQueue;
truetypeClass truetype = truetypeClass();




bool checkFileExists(const char *filename)
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
uint16_t get_bitmap_color(uint8_t *framebuffer, uint16_t x, uint16_t y) {
    uint16_t byte_index = y * WIDTH_BYTES + (x / 8);
    uint8_t bit_position = 7 - (x % 8); // 从高位到低位
    uint8_t bit_mask = _BV(bit_position);
    if (framebuffer[byte_index] & bit_mask) {
        return TFT_WHITE;
    } else {
        return TFT_BLACK;
    }
}

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

// 获取中文字符串的Unicode编码数组
void printChineseUnicode(const String& text) {
  Serial.println("Unicode编码值：");
  for (int i = 0; i < text.length(); i++) {
    // 获取UTF-8编码的字符并转换为Unicode
    char c1 = text[i];
    if ((c1 & 0x80) == 0) {
      // ASCII字符
      Serial.print("U+");
      Serial.print(c1, HEX);
      Serial.print(" (");
      Serial.print(c1);
      Serial.println(")");
    } else if ((c1 & 0xE0) == 0xC0 && i+1 < text.length()) {
      // 双字节UTF-8
      char c2 = text[i+1];
      uint16_t unicode = ((c1 & 0x1F) << 6) | (c2 & 0x3F);
      Serial.print("U+");
      Serial.print(unicode, HEX);
      Serial.print(" (");
      Serial.print(text.substring(i, i+2));
      Serial.println(")");
      i++;
    } else if ((c1 & 0xF0) == 0xE0 && i+2 < text.length()) {
      // 三字节UTF-8 (常见中文)
      char c2 = text[i+1];
      char c3 = text[i+2];
      uint16_t unicode = ((c1 & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
      Serial.print("0x");
      Serial.print(unicode, HEX);
      Serial.print(" (");
      Serial.print(text.substring(i, i+3));
      Serial.println(")");
      i += 2;
    }
  }
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

void freeFramebuffer(uint8_t index){
  if(framebuffer[index]){
    free(framebuffer[index]);
    framebuffer[index] = nullptr;
  }
}
void freeAllFramebuffer(){
  for(int i = 0 ;i < BUF_COUNT; i++){
      freeFramebuffer(i);
    }
}
bool initTruetype()
{
    for(int i = 0 ;i < BUF_COUNT; i++){
      framebuffer[i] = (uint8_t *)calloc(sizeof(uint8_t), FRAMEBUFFER_SIZE);
      if (!framebuffer[i])
      {
        Serial.println("alloc memory failed !!!");
        freeAllFramebuffer();
        return false;
      }
    }
    
    //memset(framebuffer, 0, FRAMEBUFFER_SIZE);
    
    LittleFS.begin();
    File fontFile = LittleFS.open(MY_TTF, "r");
    Serial.println("fontFile.name():"+String(fontFile.name()));
    Serial.println("fontFile.size():"+String(fontFile.size()));
    
    if (!truetype.setTtfFile(fontFile))
    {
      Serial.println("read ttf failed");
      freeAllFramebuffer();
      return false;
    }

    truetype.setCharacterSize(160);
    truetype.setTextBoundary(0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    truetype.setCharacterSpacing(0);
    truetype.setTextColor(0x01, 0x01);
    return true;
    
}
uint8_t * readTruetype(uint8_t bufIndex)
{
    static String draw_string[] = {"以","无","所","得","故","，","菩","提","萨","埵","，","依","般","若","波","罗","蜜","多","故","，","心","无","罣","碍","；","无","罣","碍","故","，","无","有","恐","怖","，","远","离","颠","倒","梦","想","，","究","竟","涅","槃","。","三","世","诸","佛","，","依","般","若","波","罗","蜜","多","故","，","得","阿","耨","多","罗","三","藐","三","菩","提","。"};
    //static String draw_string[] = {"老","伍","牛","X","欢","迎","来","到","美","丽","的","中","国"};
    static uint16_t draw_string_len = sizeof(draw_string) / sizeof(draw_string[0]);
    static uint16_t draw_string_index = 0;
    String str = draw_string[draw_string_index];
    uint16_t unicode = getChineseUnicode(str);
    

    unsigned long startTimeRead = millis();
    memset(framebuffer[bufIndex], 0, FRAMEBUFFER_SIZE);
    truetype.setFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT, BITS_PER_PIXEL, 0, framebuffer[bufIndex]);
    truetype.textDraw(0,0,str);
    unsigned long endTimeRead = millis();
    //Serial.println("readText Time:"+String((endTimeRead-startTimeRead))+"ms");
    draw_string_index++;
    if(draw_string_index >= draw_string_len){
        draw_string_index = 0;
    }
    return framebuffer[bufIndex];
}

void handleTruetype(uint8_t index)
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
              uint16_t color = get_bitmap_color(framebuffer[index], y, x);
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
            handlerBufIndex = index;
            handleTruetype(index);
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
            uint8_t* buf = readTruetype(0);
            frameBufMessage_t  frameBufMessage;
            frameBufMessage.buf_id = 0;
            frameBufMessage.buf_data = buf;
            xQueueSend(frameBufQueue, &frameBufMessage, portMAX_DELAY);
            //delay(10000);
            Serial.println("TaskFrontTypeRead");
          }else{
            uint8_t* buf = readTruetype(1);
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
  
    frameBufQueue = xQueueCreate(QUEUE_LENGTH, sizeof(frameBufMessage_t *)); 
    checkFileExists(MY_TTF);
    initSprite();
    initTruetype();
    
    xTaskCreatePinnedToCore(TaskFrontTypeRead, "TaskFrontTypeRead", 8048, NULL, 1, NULL, 1); 
    xTaskCreatePinnedToCore(TaskFrontTypeHandle, "TaskFrontTypeHandle", 8048, NULL, 1, NULL, 0); // 在核心0上运行
    
}

void loop()
{
    //readTruetype(0);
    //handleTruetype();
    ;
}