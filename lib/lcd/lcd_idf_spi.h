#ifndef __LCD_IDF_SPI_H__
#define __LCD_IDF_SPI_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "lcd_idf_spi_init_cmds.h"
#include "lcd_spi_common.h"

// Callback function type definition
typedef void (*te_callback_t)(void* arg);

// SPI settings



// Function declarations
esp_err_t lcd_idf_spi_init(void);
void lcd_idf_spi_cs_and_dc_start(void);
void lcd_idf_spi_cs_and_dc_end(void);
void lcd_idf_spi_deinit(void);
void lcd_idf_spi_write_cmd(uint8_t cmd);
void lcd_idf_spi_write_data(uint8_t data);
void lcd_idf_spi_write_data_16(uint16_t data_16);
void lcd_idf_spi_read_data(uint8_t command, uint8_t *data, uint16_t length);
uint8_t lcd_idf_spi_read_register(uint8_t reg);
void lcd_idf_spi_read_registers(uint8_t reg, uint8_t *data, uint16_t length);
void lcd_idf_spi_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_idf_spi_write_color(uint16_t color);
void lcd_idf_spi_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed);
void lcd_idf_spi_scroll_start(uint16_t line_num);

// TE signal handling
void lcd_idf_spi_set_te_callback(te_callback_t callback);
bool lcd_idf_spi_wait_for_te(uint32_t timeout_ms);

// Drawing functions
void lcd_idf_spi_clear_screen(uint16_t color);
void lcd_idf_spi_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_idf_spi_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_idf_spi_draw_row(uint16_t y, uint16_t color);
void lcd_idf_spi_block_write_row(uint16_t y);
void lcd_idf_spi_start_write_color(void);
void lcd_idf_spi_end_write_color(void);
void lcd_idf_spi_continue_write_color(uint16_t color);

#endif // __IDF_SPI_H__
