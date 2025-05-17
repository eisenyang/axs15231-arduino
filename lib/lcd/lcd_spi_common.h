#ifndef __LCD_SPI_COMMON_H__
#define __LCD_SPI_COMMON_H__
#include <Arduino.h>
// #define PIN_NUM_MISO (10)
// #define PIN_NUM_MOSI 17
// #define PIN_NUM_CLK 18
// #define PIN_NUM_CS 14

// #define PIN_NUM_DC 15
// #define PIN_NUM_RST 13
// #define PIN_NUM_BCKL (-1)
// #define PIN_NUM_TE 10
// #define LCD_WIDTH 172
// #define LCD_HEIGHT 640

#define PIN_NUM_MISO GPIO_NUM_16
#define PIN_NUM_MOSI GPIO_NUM_17
#define PIN_NUM_CLK  GPIO_NUM_18
#define PIN_NUM_CS   GPIO_NUM_14
#define PIN_NUM_DC   GPIO_NUM_15
#define PIN_NUM_RST  GPIO_NUM_13
#define PIN_NUM_BCKL GPIO_NUM_NC
#define PIN_NUM_TE   GPIO_NUM_10
#define LCD_WIDTH  172
#define LCD_HEIGHT 640
#define SPI_FREQUENCY (32 * 1000 * 1000)
#define LCD_HOST    SPI2_HOST
#define DMA_CHAN    SPI_DMA_CH_AUTO
// Colors
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

#endif
