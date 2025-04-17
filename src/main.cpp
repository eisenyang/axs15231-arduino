#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd.h"
#include <TFT_eSPI.h>
#include <FS.h>
#include <LittleFS.h>
#include "simkai160.h"
#include "TruetypeManager.h"
SpriteTextManager spriteTextManager;
#define WIDTH_BYTES 20
#define HEIGHT_PIXELS 160
#define WIDTH_PIXELS 160
#define DISPLAY_HEIGHT HEIGHT_PIXELS
#define FRAMEBUFFER_SIZE (WIDTH_BYTES * HEIGHT_PIXELS)
#define BITS_PER_PIXEL 1 // either 1, 4, or 8
#define DISPLAY_WIDTH (WIDTH_BYTES * (8 / BITS_PER_PIXEL))
uint8_t *framebuffer;
// #define WIDTH_BYTES 20
// #define HEIGHT_PIXELS 160
// #define DISPLAY_HEIGHT HEIGHT_PIXELS
// #define FRAMEBUFFER_SIZE (WIDTH_BYTES * HEIGHT_PIXELS)
// #define BITS_PER_PIXEL 4 // either 1, 4, or 8
// #define DISPLAY_WIDTH (WIDTH_BYTES * (8 / BITS_PER_PIXEL))
#define MY_TTF "/simhei.ttf"
#define QUEUE_LENGTH 1
// 定义消息结构体
typedef struct {
  uint16_t unicode;
  bool processed;
} TextMessage_t;

// 声明队列句柄
QueueHandle_t textQueue;
static bool handle_flag = false;
truetypeClass truetype = truetypeClass();
bool checkFileExists(const char *filename)
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS挂载失败");
    return false;
  }

  if (SPIFFS.exists(filename))
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

void initTruetype()
{
    spriteTextManager.init();
    spriteTextManager.fillScreen(TFT_BLACK);
    spriteTextManager.setScrollWindow(0, LCD_HEIGHT, 0);
    framebuffer = (uint8_t *)calloc(sizeof(uint8_t), FRAMEBUFFER_SIZE);
    //memset(framebuffer, 0, FRAMEBUFFER_SIZE);
    if (!framebuffer)
    {
      Serial.println("alloc memory failed !!!");
      while (1)
        ;
    }
    Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d bytes\n", ESP.getFreePsram());
    Serial.printf("Total heap: %d bytes\n", ESP.getHeapSize());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
    Serial.printf("Flash Size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("Flash Speed: %d Hz\n", ESP.getFlashChipSpeed());


    SPIFFS.begin(true);
    File fontFile = SPIFFS.open(MY_TTF, "r");
    Serial.println("fontFile.name():"+String(fontFile.name()));
    Serial.println("fontFile.size():"+String(fontFile.size()));
    truetype.setFramebuffer(DISPLAY_WIDTH, DISPLAY_HEIGHT, BITS_PER_PIXEL, 0, framebuffer);
    if (!truetype.setTtfFile(fontFile))
    {
      Serial.println("read ttf failed");
    }
    else
    {
      truetype.setCharacterSize(160);
      truetype.setTextBoundary(0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
      truetype.setCharacterSpacing(5);
      truetype.setTextColor(0x01, 0x01);
    }
}

void handleTruetype()
{
    static uint16_t address = LCD_HEIGHT;
    static bool is_draw = false;
    static uint16_t y = 0;
    //static String draw_string[] = {"万", "事", "如", "意", "、", "阖", "家", "幸", "福", "！"};
    static String draw_string[] = {"以","无","所","得","故","，","菩","提","萨","埵","，","依","般","若","波","罗","蜜","多","故","，","心","无","罣","碍","；","无","罣","碍","故","，","无","有","恐","怖","，","远","离","颠","倒","梦","想","，","究","竟","涅","槃","。","三","世","诸","佛","，","依","般","若","波","罗","蜜","多","故","，","得","阿","耨","多","罗","三","藐","三","菩","提","。"};
    static uint16_t draw_string_len = sizeof(draw_string) / sizeof(draw_string[0]);
    static uint16_t draw_string_index = 0;
    if (address <= 0)
    {
        address = LCD_HEIGHT;
    }
    
    unsigned long startTimeTotal = micros();
    static uint16_t top_offset = 2;
    for (int x = 0; x < WIDTH_PIXELS && draw_string_index < draw_string_len; x++)
    {
        //Serial.println("handleTruetype");
        //truetype.textDraw(0, 0, L"福");
        //display_bitmap_rotate_90(framebuffer, WIDTH_BYTES, HEIGHT_PIXELS);

        //Serial.println("x:"+String(x)+" y:"+String(y));
        if (y < HEIGHT_PIXELS)
        {
            if (is_draw == false)
            {
                memset(framebuffer, 0, FRAMEBUFFER_SIZE);
                uint16_t unicode = getChineseUnicode(draw_string[draw_string_index]);
                truetype.readText(unicode);
                truetype.pushText();
                is_draw = true;
            }
            uint16_t y_offset = address-1;
            uint16_t color = get_bitmap_color(framebuffer, y, x);
            //Serial.println("color:"+String(color));
            if (x == 0)
            {
                spriteTextManager.setRowAddress(top_offset, y_offset);
                spriteTextManager.enableWriteColor();
            }
            spriteTextManager.writeColor(color);
            //Serial.println("x:"+String(x)+" y:"+String(y)+" color:"+String(color));
            if (x == WIDTH_PIXELS - 1)
            {
                spriteTextManager.disableWriteColor();
            }
        }else if (y >= HEIGHT_PIXELS)
        {
            draw_string_index++;
            is_draw = false;
            y = 0;
            spriteTextManager.clearSprite();
        }

    }
    
    if (draw_string_index >= draw_string_len)
    {
        draw_string_index = 0;
        is_draw = false;
        y = 0;
        spriteTextManager.clearSprite();
        Serial.println("clearSprite2");
    }
    spriteTextManager.scrollStart(address);
    y++;
    address--;
}

void readFrontType()
{
    static String draw_string[] = {"以","无","所","得","故","，","菩","提","萨","埵","，","依","般","若","波","罗","蜜","多","故","，","心","无","罣","碍","；","无","罣","碍","故","，","无","有","恐","怖","，","远","离","颠","倒","梦","想","，","究","竟","涅","槃","。","三","世","诸","佛","，","依","般","若","波","罗","蜜","多","故","，","得","阿","耨","多","罗","三","藐","三","菩","提","。"};
    static uint16_t draw_string_len = sizeof(draw_string) / sizeof(draw_string[0]);
    static uint16_t draw_string_index = 0;
    String str = draw_string[draw_string_index];
    uint16_t unicode = getChineseUnicode(str);
    TextMessage_t message;
    message.unicode = unicode;
    message.processed = false;
    truetype.readText('万');
    // if(xQueueSend(textQueue, &message, portMAX_DELAY) == pdPASS){
    //     draw_string_index++;
    //     Serial.println("xQueueSend===>");
    // }
    if(draw_string_index >= draw_string_len){
        draw_string_index = 0;
    }    
}

void TaskFrontTypeHandle(void *pvParameters)
{
    TextMessage_t receivedMessage;
     Serial.println("TaskHandle");
    while(1){
        if (xQueueReceive(textQueue, &receivedMessage, portMAX_DELAY) == pdPASS) {
            handle_flag = true;
            Serial.println("xQueueReceive===>");
            handleTruetype();
            //handle_flag = false;
        }
        //delay(1000);
    }
}

void TaskFrontTypeRead(void *pvParameters)
{
    Serial.println("TaskFrontTypeRead");
    while(1){
        if(uxQueueMessagesWaiting(textQueue) == QUEUE_LENGTH){
            //Serial.println("Queue is full");
            delay(10);
            return;
        }
        readFrontType();
        handleTruetype();
        //delay(10000000);
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }
    
    // 测试Unicode函数
        // 创建队列
    textQueue = xQueueCreate(QUEUE_LENGTH, sizeof(TextMessage_t));
    if (textQueue == NULL) {
        Serial.println("Queue creation failed!");
        while(1); // 如果队列创建失败，停止执行
    }
    initTruetype();
    
    //xTaskCreatePinnedToCore(TaskFrontTypeRead, "TaskFrontTypeRead", 16384, NULL, 1, NULL, 1); 
    //xTaskCreatePinnedToCore(TaskFrontTypeHandle, "TaskFrontTypeHandle", 8192, NULL, 1, NULL, 1); // 在核心0上运行

    readFrontType();
    
}

void loop()
{

    //handleTruetype();
    //Serial.println("address:"+String(address));
    handleTruetype();
    delay(1);
}