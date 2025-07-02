#pragma once
#include <stdio.h>
// #include <driver/gpio.h>

#define DISPLAY_SPI_SCLK_HZ (40 * 1000 * 1000)
#define TEST_LCD_BIT_PER_PIXEL (16)
/* SPI & QSPI */
#define LCD_Widht (172)
#define LCD_High (640)
#define LCD_SPI_HOST (SPI2_HOST)
#define LCD_SPI_CS (GPIO_NUM_3)
#define LCD_SPI_CLK (GPIO_NUM_5)
#define LCD_SPI_MOSI (GPIO_NUM_4)
// #define LCD_SPI_MISO (GPIO_NUM_7)
#define LCD_SPI_MISO (GPIO_NUM_NC)
#define LCD_SPI_RST (GPIO_NUM_1)
#define LCD_SPI_DC (GPIO_NUM_2)
#define LCD_SPI_BL (GPIO_NUM_6)
#define LCD_PIN_TE (GPIO_NUM_8)

#define LED_PIN (GPIO_NUM_11)
#define LED1_PIN (GPIO_NUM_12)
#define RGB_MAX_NUMS 16
#define RGB_PIN (GPIO_NUM_13)

#define LEDC_HS_TIMER LEDC_TIMER_0
#define LEDC_HS_MODE LEDC_LOW_SPEED_MODE
#define LEDC_HS_CH0_CHANNEL LEDC_CHANNEL_0
#define LEDC_HS_CH1_CHANNEL LEDC_CHANNEL_1
#define LEDC_HS_CH2_CHANNEL LEDC_CHANNEL_2
#include "lvgl.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum
    {
        DISPLAY_DIRECT_VAL_0 = 0, // 垂直方向
        DISPLAY_DIRECT_VAL_90,    // 水平方向
        DISPLAY_DIRECT_VAL_180,   // 反向垂直方向
        DISPLAY_DIRECT_VAL_270,   // 反向水平方向
    } DISPLAY_DIRECT_VAL_t;
#define DISPLAY_DIRECT_VAL DISPLAY_DIRECT_VAL_180

    void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p);
    void init_led();
    void led_set(uint32_t val);
    void led1_set(uint32_t val);
    void SetBrightness(uint32_t val);
    void init_display();
    void lcd_spi_write_colors(const uint16_t *colors, size_t len);
    void lcd_spi_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void lcd_spi_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed);
    void lcd_spi_scroll_start(uint16_t line_num);

    void axs15231_init();
    void lcd_spi_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void lcd_spi_write_colors(const uint16_t *colors, size_t len);
    void lcd_spi_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed);
    void lcd_spi_scroll_start(uint16_t line_num);
    uint16_t hsv_to_rgb565(uint16_t h, uint8_t s, uint8_t v);
    uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b);
    uint16_t rgb_to_rgb565(uint32_t color_888);
    void lcd_spi_write_line_colors(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *colors, size_t len);

    static const char draw_string[] =
        "觀自在菩薩，行深般若波羅蜜多時，照見五蘊皆空，度一切苦厄。"
        "舍利子，色不異空，空不異色，色即是空，空即是色，受想行識亦復如是。"
        "舍利子，是諸法空相，不生不滅，不垢不淨，不增不減。"
        "是故空中無色，無受想行識，無眼耳鼻舌身意，無色聲香味觸法，無眼界乃至無意識界，無無明亦無無明盡，乃至無老死，亦無老死盡，無苦集滅道，無智亦無得，以無所得故。"
        "菩提薩埵，依般若波羅蜜多故，心無掛礙；無掛礙故，無有恐怖，遠離顛倒夢想，究竟涅槃。"
        "三世諸佛，依般若波羅蜜多故，得阿耨多羅三藐三菩提。"
        "故知般若波羅蜜多，是大神咒，是大明咒，是無上咒，是無等等咒，能除一切苦，真實不虛。"
        "故說般若波羅蜜多咒，即說咒曰：揭諦揭諦，波羅揭諦，波羅僧揭諦，菩提薩婆訶。!!!";
    // 觀 -> U+89C0
    // 自 -> U+81EA
    // 在 -> U+5728
    // 菩 -> U+831B
    // 薩 -> U+84EE
    void screen_buf_init();
    void display_refresh();                     // 刷新屏幕
    void sw_screen_roll(uint16_t speed);        // 滚动屏幕
    void lcd_write_color_full(uint16_t color_); // 全屏填充颜色
    void code_read(uint16_t list_num);

#define Y 1
#define N 0
#define USE_LVGL N       // 是否使用 LVGL 图形库
#define ROLL_UP N        // 向上滚屏
#define USE_HW_ROLL Y    // 硬件滚动屏幕
#define WRITE_LINE_NUM 4 // 滚屏速度
#define SCREEN_BG_COLOR 0x000000
#define SCREEN_CODE_COLOR 0xFFFFFF

#ifdef __cplusplus
}
#endif