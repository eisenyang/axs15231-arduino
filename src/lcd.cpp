#include <Arduino.h>
#include <SPI.h>
#include "lcd.h"
#include "lcd_init_cmds.h"
#include "lcd_color_helpers.h"

#define LCD_DC_H digitalWrite(PIN_NUM_DC, HIGH)
#define LCD_DC_L digitalWrite(PIN_NUM_DC, LOW)
#define LCD_CS_H digitalWrite(PIN_NUM_CS, HIGH)
#define LCD_CS_L digitalWrite(PIN_NUM_CS, LOW)
#define LCD_RST_H digitalWrite(PIN_NUM_RST, HIGH)
#define LCD_RST_L digitalWrite(PIN_NUM_RST, LOW)

static SPIClass *lcd_spi = NULL;

static void lcd_reset()
{
    LCD_RST_H;
    delay(20);
    LCD_RST_L;
    delay(200);
    LCD_RST_H;
    delay(200);
}

void lcd_init()
{
    pinMode(PIN_NUM_RST, OUTPUT);
    pinMode(PIN_NUM_CS, OUTPUT);
    pinMode(PIN_NUM_DC, OUTPUT);
    LCD_CS_H;
    LCD_DC_H;

    lcd_reset();

    lcd_spi = new SPIClass(1);
    lcd_spi->begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI);
    lcd_spi->setFrequency(SPI_FREQUENCY);

    lcd_init_cmds();
}
void lcd_uninit()
{
    if(lcd_spi != NULL)
    {
        lcd_spi->end();
        delete lcd_spi;
        lcd_spi = NULL;
    }
}
void lcd_write_cmd(uint8_t command)
{
    LCD_CS_L;
    LCD_DC_L;
    // lcd_spi->transfer(command);
    lcd_spi->beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
    lcd_spi->write(command);
    lcd_spi->endTransaction();
    LCD_CS_H;
}

void lcd_write_data(uint8_t data)
{
    LCD_CS_L;
    LCD_DC_H;
    lcd_spi->beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
    lcd_spi->write(data);
    lcd_spi->endTransaction();
    // lcd_spi->transfer(data);
    LCD_CS_H;
}

void lcd_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_write_cmd(0x2a);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1 & 0xff);
    lcd_write_data(x2 >> 8);
    lcd_write_data(x2 & 0xff);

    lcd_write_cmd(0x2b);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1 & 0xff);
    lcd_write_data(y2 >> 8);
    lcd_write_data(y2 & 0xff);

    lcd_write_cmd(0x2c);
}

void lcd_write_color(uint16_t color)
{
    LCD_CS_L;
    LCD_DC_H;
    lcd_spi->beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
    lcd_spi->write(color >> 8);
    lcd_spi->write(color & 0xff);
    lcd_spi->endTransaction();
    LCD_CS_H;
}

void lcd_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed)
{
    lcd_write_cmd(0x33);
    lcd_write_data(top_fixed >> 8);        // 0
    lcd_write_data(top_fixed & 0xff);      // 1   前2位是顶部不动的区域
    lcd_write_data(scroll_content >> 8);   // 2
    lcd_write_data(scroll_content & 0xff); // 3   中间2位是滚动的内容
    lcd_write_data(bottom_fixed >> 8);     // 4
    lcd_write_data(bottom_fixed & 0xff);   // 5   后两个是底部不动的区域
}

void lcd_scroll_start(uint16_t line_num)
{
    lcd_write_cmd(0x37);
    lcd_write_data(line_num >> 8);
    lcd_write_data(line_num & 0xff);
}

void lcd_clear_screen(uint16_t color)
{
    lcd_block_write(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    uint16_t w = LCD_WIDTH;
    uint16_t h = LCD_HEIGHT;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            lcd_write_color(color);
        }
    }
}

void lcd_test_draw_frame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t front_color, uint16_t back_color)
{
    uint16_t w = x2 - x1;
    uint16_t h = y2 - y1;

    lcd_block_write(x1, y1, x2 - 1, y2 - 1);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (i == 0 || i == (h - 1) || j == 0 || j == (w - 1))
            {
                lcd_write_color(front_color);
            }
            else
            {
                lcd_write_color(back_color);
            }
        }
    }
}

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    lcd_block_write(x, y, x, y);
    lcd_write_color(color);
}



void lcd_test_draw_top_marker(uint16_t x, uint16_t front_color, uint16_t back_color)
{
    uint16_t height = 4;
    lcd_block_write(0, 0, LCD_WIDTH - 1, height);

    // lcd_write_cmd(0x2c);
    for (uint16_t i = 0; i < LCD_WIDTH * height; i++)
    {
        if ((i % LCD_WIDTH > x - 5 && i % LCD_WIDTH < x + 5) || i == 0)
        {
            lcd_write_color(front_color);
        }
        else
        {
            lcd_write_color(back_color);
        }
    }
}

void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    lcd_block_write(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    uint16_t w = LCD_WIDTH;
    uint16_t h = LCD_HEIGHT;


    // if(x1 == x2){
    //     for(int x = 0; x < w; x++){
    //         for(int y = 0; y < h; y++){
    //             if(y >= y1 && y <= y2){
    //                 if(x == x1){
    //                     lcd_write_color(color);
    //                 }
    //                 else{
    //                     lcd_write_color(BLACK);
    //                 }
    //             }
                
    //         }
    //     }
    // }
    // else if(y1 == y2){
    //     for(int x = 0; x < w; x++){
    //         for(int y = 0; y < h; y++){
    //             if(y == y1 && x >= x1 && x <= x2){
    //                 lcd_write_color(color);
    //             }
    //             else{
    //                 lcd_write_color(BLACK);
    //             }
    //         }
    //     }
    // }
    // else{
    //     lcd_write_color(BLACK);
    // }
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {

            if(y1 == y2){
                if(y == y1 && x >= x1 && x <= x2)
                {
                    lcd_write_color(color);
                }
                else{
                    lcd_write_color(BLACK);
                }
            }
            else if(x1 == x2){
                if(x == x1 && y >= y1 && y <= y2)
                {
                    lcd_write_color(color);
                }
                else{
                    lcd_write_color(BLACK);
                }
            }
            
        }
    }


    // for (int x = 0; x < w; x++)
    // {
    //     for (int y = 0; y < h; y++)
    //     {
    //         if(y == y1 && x >= x1 && x <= x2)
    //         {
    //             lcd_write_color(color);
    //         }
    //         else{
    //             lcd_write_color(BLACK);
    //         }
    //     }
    // }
}
void lcd_draw_line2(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    // 确定是否为水平线或垂直线，可以使用更简单的方法绘制
    if (y0 == y1) { // 水平线
        uint16_t start_x = (x0 < x1) ? x0 : x1;
        uint16_t end_x = (x0 > x1) ? x0 : x1;
        
        lcd_block_write(start_x, y0, end_x, y0);
        for (uint16_t x = start_x; x <= end_x; x++) {
            lcd_write_color(color);
        }
        return;
    }
    
    if (x0 == x1) { // 垂直线
        uint16_t start_y = (y0 < y1) ? y0 : y1;
        uint16_t end_y = (y0 > y1) ? y0 : y1;
        
        lcd_block_write(x0, start_y, x0, end_y);
        for (uint16_t y = start_y; y <= end_y; y++) {
            lcd_write_color(color);
        }
        return;
    }
    
    // 对于斜线，使用逐像素绘制的方法
    // 先计算Bresenham算法参数
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    // 确定绘制区域
    lcd_block_write(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    
    // 按行列遍历屏幕
    for (int y = 0; y < LCD_HEIGHT; y++) {
        for (int x = 0; x < LCD_WIDTH; x++) {
            // 对于每个像素，判断是否在线上
            // 使用与Bresenham算法类似的判断
            int cx = x0, cy = y0;
            int terr = err;
            bool is_on_line = false;
            
            if (x == cx && y == cy) {
                is_on_line = true;
            } else {
                while (true) {
                    int e2 = 2 * terr;
                    if (e2 > -dy) {
                        terr -= dy;
                        cx += sx;
                    }
                    if (e2 < dx) {
                        terr += dx;
                        cy += sy;
                    }
                    
                    if (cx == x && cy == y) {
                        is_on_line = true;
                        break;
                    }
                    
                    if (cx == x1 && cy == y1) {
                        break;
                    }
                }
            }
            
            if (is_on_line) {
                lcd_write_color(color);
            } else {
                lcd_write_color(BLACK);
            }
        }
    }
}

