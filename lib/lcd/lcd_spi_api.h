#ifndef __LCD_SPI_API_H__
#define __LCD_SPI_API_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "lcd_spi_init_cmds.h"
#include "lcd_spi_common.h"

// Callback function type definition
typedef void (*te_callback_t)(void* arg);

// SPI settings
// Function declarations
void spi_test(void);
esp_err_t lcd_spi_init(void);
void lcd_spi_set_scroll_rows(uint16_t rows);
void lcd_spi_set_cs_high(void);
void lcd_spi_set_cs_low(void);
void lcd_spi_set_dc_high(void);
void lcd_spi_set_dc_low(void);
void lcd_spi_deinit(void);
void lcd_spi_write_cmd(uint8_t cmd);
void lcd_spi_write_data(uint8_t data);
void lcd_spi_write_data_16(uint16_t data_16);
void lcd_spi_read_data(uint8_t command, uint8_t *data, uint16_t length);
uint8_t lcd_spi_read_register(uint8_t reg);
void lcd_spi_read_registers(uint8_t reg, uint8_t *data, uint16_t length);
void lcd_spi_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_spi_write_color(uint16_t color);
void lcd_spi_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed);
void lcd_spi_scroll_start(uint16_t line_num);

// TE signal handling
void lcd_spi_set_te_callback(te_callback_t callback);
bool lcd_spi_wait_for_te(uint32_t timeout_ms);

// Drawing functions
void lcd_spi_clear_screen(uint16_t color);
void lcd_spi_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_spi_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_spi_draw_row(uint16_t y, uint16_t color);
void lcd_spi_block_write_row(uint16_t y);
void lcd_spi_write_color(uint16_t color);
void lcd_spi_scroll_end(spi_transaction_t &t);
/**
 * @brief 批量写入多个颜色数据到LCD
 * @param colors 16位颜色值数组
 * @param len 颜色值数量
 */
void lcd_spi_write_colors(const uint16_t* colors, size_t len);

#endif // __IDF_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
