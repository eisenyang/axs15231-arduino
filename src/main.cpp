#include <Arduino.h>
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
void print_bitmap(uint8_t *framebuffer, uint16_t width_in_bytes, uint16_t height_in_pixels) {
    for (int i = 0; i < (width_in_bytes * height_in_pixels); i++) {
        
        if ((i % width_in_bytes) == 0){
            Serial.println();
        }
        for (uint8_t bits = 8; bits > 0; bits--) {
            if (_BV(bits - 1) & framebuffer[i]){
                Serial.print(" ");
            }else{
                Serial.print("*");
            }

            //Serial.print(String(framebuffer[i]));
        }
    }

}

void print_bitmap_row_column(uint8_t *framebuffer, uint16_t width_in_bytes, uint16_t height_in_pixels) {
    // 用行列坐标访问位图
    for (uint16_t y = 0; y < HEIGHT_PIXELS; y++) {
        Serial.println();
        for (uint16_t x = 0; x < WIDTH_BYTES * 8; x++) {
            // 计算字节索引和位掩码
            uint16_t byte_index = y * WIDTH_BYTES + (x / 8);
            uint8_t bit_position = 7 - (x % 8); // 从高位到低位
            uint8_t bit_mask = _BV(bit_position);
            
            // 检查该位是否为1
            if (framebuffer[byte_index] & bit_mask) {
                // 位为1的处理
                Serial.print(" ");
            } else {
                // 位为0的处理
                Serial.print("*");
            }
        }
    }

}

void display_bitmap(uint8_t *framebuffer, uint16_t width_in_bytes, uint16_t height_in_pixels) {
    // 用行列坐标访问位图
    for (uint16_t y = 0; y < HEIGHT_PIXELS; y++) {
        for (uint16_t x = 0; x < WIDTH_BYTES * 8; x++) {
            // 计算字节索引和位掩码
            uint16_t byte_index = y * WIDTH_BYTES + (x / 8);
            uint8_t bit_position = 7 - (x % 8); // 从高位到低位
            uint8_t bit_mask = _BV(bit_position);
            
            // 检查该位是否为1
            if (framebuffer[byte_index] & bit_mask) {
                // 位为1的处理
                //Serial.print(" ");
                spriteTextManager.drawPixel2LCD(x, y, TFT_WHITE);
            } else {
                // 位为0的处理
                //Serial.print("*");
                spriteTextManager.drawPixel2LCD(x, y, TFT_BLACK);
            }
        }
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
void display_bitmap_rotate_90(uint8_t *framebuffer, uint16_t width_in_bytes, uint16_t height_in_pixels) {


    // 逆时针旋转90度显示位图
    for (uint16_t x = WIDTH_PIXELS - 1; x < WIDTH_PIXELS; x--) { // 注意：使用无符号类型，需要有效处理边界
        for (uint16_t y = 0; y < HEIGHT_PIXELS; y++) {
            // 计算原始位图中的字节索引和位掩码
            uint16_t byte_index = y * WIDTH_BYTES + (x / 8);
            uint8_t bit_position = 7 - (x % 8); // 从高位到低位
            uint8_t bit_mask = _BV(bit_position);
            
            // 检查该位是否为1
            if (framebuffer[byte_index] & bit_mask) {
                // 位为1的处理
                // 旋转后坐标：原来的x变为y，原来的y变为(WIDTH_BYTES*8-1-x)
                spriteTextManager.drawPixel2LCD(y, WIDTH_PIXELS - 1 - x, TFT_WHITE);
            } else {
                // 位为0的处理
                spriteTextManager.drawPixel2LCD(y, WIDTH_PIXELS - 1 - x, TFT_BLACK);
            }
        }
    }
}
void print_bitmap_rotate(uint8_t *framebuffer, uint16_t width_in_bytes, uint16_t height_in_pixels) {
    for (uint16_t x = WIDTH_BYTES * 8 - 1; x < WIDTH_BYTES * 8; x--) {
        for (uint16_t y = 0; y < HEIGHT_PIXELS; y++) {
                    // 计算原始位图中的字节索引和位掩码
            uint16_t byte_index = y * WIDTH_BYTES + (x / 8);
            uint8_t bit_position = 7 - (x % 8); // 从高位到低位
            
            // 计算旋转后的新坐标
            uint16_t new_x = y;
            uint16_t new_y = WIDTH_BYTES * 8 - 1 - x;
            
            // 直接使用framebuffer中的值作为颜色值
            // 注意：这里假设framebuffer中的值能直接用作颜色
            // 如果是单色位图(1位每像素)，需要先提取位值
            uint16_t color = 0;

            if (BITS_PER_PIXEL == 1) {
            // 单色位图，提取位值并映射到黑白色
            uint8_t bit_mask = _BV(bit_position);
            bool pixel_value = (framebuffer[byte_index] & bit_mask) != 0;
            color = pixel_value ? TFT_WHITE : TFT_BLACK;
        } 
        else if (BITS_PER_PIXEL == 4) {
            // 4位每像素，提取半字节值
            uint8_t nibble_pos = (x % 2) * 4; // 0或4
            uint8_t nibble_mask = 0xF << nibble_pos;
            uint8_t gray_value = (framebuffer[byte_index] & nibble_mask) >> nibble_pos;
            // 将4位灰度值(0-15)映射到16位色
            color = (gray_value << 12) | (gray_value << 7) | (gray_value << 1);
        }
        else if (BITS_PER_PIXEL == 8) {
            // 8位每像素，直接使用字节值
            uint8_t gray_value = framebuffer[byte_index];
            // 将8位灰度值(0-255)映射到16位色
            color = ((gray_value >> 3) << 11) | ((gray_value >> 2) << 5) | (gray_value >> 3);
        }
        
        // 在旋转后的位置绘制像素
        spriteTextManager.drawPixel2LCD(new_x, new_y, color);

        }
    }
}
void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }

    //以无所得故，菩提萨埵，依般若波罗蜜多故，心无罣碍；无罣碍故，无有恐怖，远离颠倒梦想，究竟涅槃。三世诸佛，依般若波罗蜜多故，得阿耨多罗三藐三菩提。
    spriteTextManager.init();
    spriteTextManager.fillScreen(TFT_BLACK);
    spriteTextManager.setScrollWindow(0, LCD_HEIGHT, 0);
    framebuffer = (uint8_t *)calloc(sizeof(uint8_t), FRAMEBUFFER_SIZE);
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
    //truetype.end();
    
}

void loop()
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
        
        //truetype.textDraw(0, 0, L"福");
        //display_bitmap_rotate_90(framebuffer, WIDTH_BYTES, HEIGHT_PIXELS);

        //Serial.println("x:"+String(x)+" y:"+String(y));
        if (y < HEIGHT_PIXELS)
        {
            if (is_draw == false)
            {
                memset(framebuffer, 0, FRAMEBUFFER_SIZE);
                String str = draw_string[draw_string_index];
                Serial.println("str:"+str);
                unsigned long startTime = micros();
                truetype.textDraw(0, 0, str);
                
                unsigned long endTime = micros();
                Serial.println("time:"+String(endTime-startTime));
                //display_bitmap_rotate_90(framebuffer, WIDTH_BYTES, HEIGHT_PIXELS);
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
    //Serial.println("address:"+String(address));
    delay(1);
}