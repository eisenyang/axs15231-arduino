#ifndef __LCD_ARDUINO_SPI_H__
#define __LCD_ARDUINO_SPI_H__
#include <Arduino.h>
#include "lcd_spi_common.h"
void lcd_arduino_spi_init();
void lcd_arduino_spi_uninit();
void lcd_arduino_spi_write_cmd(uint8_t command);
void lcd_arduino_spi_write_data(uint8_t data);

void lcd_arduino_spi_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_arduino_spi_write_color(uint16_t color);
void lcd_arduino_spi_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed);
void lcd_arduino_spi_scroll_start(uint16_t line_num);

// Test functions
void lcd_arduino_spi_test_draw_frame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t front_color, uint16_t back_color);
void lcd_arduino_spi_test_draw_top_marker(uint16_t x, uint16_t front_color, uint16_t back_color);
void lcd_arduino_spi_clear_screen(uint16_t color);
void lcd_arduino_spi_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_arduino_spi_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_arduino_spi_draw_row(uint16_t y,uint16_t color);
void lcd_arduino_spi_block_write_row(uint16_t y);
void lcd_arduino_spi_start_write_color();
void lcd_arduino_spi_end_write_color();
void lcd_arduino_spi_continue_write_color(uint16_t color);
// void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
// void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);
// void lcd_draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
// void lcd_draw_text(uint16_t x, uint16_t y, const char *text, uint16_t color);

#endif