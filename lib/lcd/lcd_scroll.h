#ifndef __LCD_SCROLL_H__
#define __LCD_SCROLL_H__
#include <LittleFS.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd_spi_api.h"
#include <TFT_eSPI.h>
#include "TruetypeManager.h"
#include "lcd_spi_api.h"
typedef struct{
  uint16_t y_scroll_offset = 0;
  uint8_t availableIndex = 0;
  bool scrollCompleted = true;
  bool readCompleted = false;
  bool scrollInit = false;
}scroll_info_t;
bool init_scroll();
void start_scroll();
void set_scroll_string(const char *str);
void srcoll_screen();
void set_available_index(scroll_info_t &scroll_info);
void te_irs_task(void *pvParameters);
void task_read_buf_from_truetype(void *pvParameters);
void task_read_buf_to_screen(TimerHandle_t pxTimer);
void write_buf_to_screen(uint8_t *framebuffer, uint16_t y);
#endif