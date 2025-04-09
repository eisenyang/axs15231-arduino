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
