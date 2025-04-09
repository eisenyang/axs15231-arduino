#include <Arduino.h>

#define PIN_NUM_MISO (10)
#define PIN_NUM_MOSI 17
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 14

#define PIN_NUM_DC 15
#define PIN_NUM_RST 13
#define PIN_NUM_BCKL (-1)

#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define BRRED 0XFC07
#define PINK 0XF81F
#define RED 0xF800
#define BROWN 0XBC40
#define GRAY 0X8430
#define GBLUE 0X07FF
#define GREEN 0x07E0
#define BLUE 0x001F
#define BLACK 0x0000

#define SPI_FREQUENCY 32 * 1000 * 1000

#define LCD_WIDTH 172
#define LCD_HEIGHT 640

void lcd_init();
void lcd_write_cmd(uint8_t command);
void lcd_write_data(uint8_t data);

void lcd_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_write_color(uint16_t color);
void lcd_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed);
void lcd_scroll_start(uint16_t line_num);

// Test functions
void lcd_test_draw_frame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t front_color, uint16_t back_color);
void lcd_test_draw_top_marker(uint16_t x, uint16_t front_color, uint16_t back_color);