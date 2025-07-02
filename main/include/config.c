#include "config.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/timer.h"
#include <esp_timer.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include "lvgl.h"
#include <unistd.h>
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/semphr.h"
extern SemaphoreHandle_t font_render_semaphore;
static char *TAG = "CONFIG";

lv_obj_t *scr;

void main_task(void *arg);
// extern uint16_t color[LCD_High][LCD_Widht];
extern uint16_t *color;
extern TaskHandle_t vector_code_task_handle;
#include "vector_font.h"
extern FT_Face s_face;
extern bool ref_map;
extern bool bitmap1[160][160];
extern bool bitmap2[160][160];
void init_display()
{
    axs15231_init();
    ESP_LOGI(TAG, "清屏");
    screen_buf_init();
    lcd_write_color_full(rgb888_to_rgb565(0xff, 0xff, 0xff)); // 清屏
    // code_read(1);
    uint16_t cc = 0;

    for (size_t high = 0; high < 160; high++)
    {
        for (size_t width = 6; width < 166; width++)
        {
            if (ref_map)
            {
                if (bitmap2[high][width - 6])
                    color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_CODE_COLOR);
                else
                    color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_BG_COLOR);
            }
            else
            {

                if (bitmap1[high][width - 6])
                    color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_CODE_COLOR);
                else
                    color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_BG_COLOR);
            }
        }
    }

    display_refresh();
    lcd_spi_set_scroll_window(0, LCD_High - 1, 0);
    lcd_spi_scroll_start(LCD_High - 1);
    uint16_t line = 160; // 剩余未读取行数
    while (1)
    {
#if USE_HW_ROLL // 硬件滚屏
        for (size_t high = 0; high < WRITE_LINE_NUM; high++)
        {
            for (size_t width = 0; width < 6; width++)
                color[high * LCD_Widht + width] = hsv_to_rgb565(cc * 255 / 640, 255, 255);
            for (size_t width = 6; width < 166; width++)
            {
                if (ref_map)
                {
                    if (bitmap2[high + 160 - line][width - 6])
                        color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_CODE_COLOR);
                    else
                        color[high * LCD_Widht + width] = rgb_to_rgb565(0xffff00);
                }
                else
                {
                    if (bitmap1[high + 160 - line][width - 6])
                        color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_CODE_COLOR);
                    else
                        color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_BG_COLOR);
                }
            }
            for (size_t width = 166; width < LCD_Widht; width++)
                color[high * LCD_Widht + width] = hsv_to_rgb565(cc * 255 / 640, 255, 255);
            line--;
        }
        if (LCD_High - cc < WRITE_LINE_NUM)
        {
            ESP_LOGI(TAG, "剩余行数不足，重新读取");
        }
        else
        {
            lcd_spi_block_write(0, cc, LCD_Widht - 1, cc + WRITE_LINE_NUM - 1);
            lcd_spi_write_colors(color, LCD_Widht * WRITE_LINE_NUM);
            // display_refresh();
        }

        lcd_spi_scroll_start(cc);
#if ROLL_UP // 向上滚屏
        cc -= WRITE_LINE_NUM;
        if (cc > 640)
            cc = 639;
#else
        cc += WRITE_LINE_NUM;
        if (cc >= 640)
            cc = 0;
#endif
#else // 软件滚屏
        for (size_t high = 0; high < WRITE_LINE_NUM; high++)
        {
            for (size_t width = 0; width < 6; width++)
            {
                // color[high][width] = hsv_to_rgb565(cc * 255 / 640, 255, 255);
                color[high * LCD_Widht + width] = hsv_to_rgb565(cc * 255 / 640, 255, 255);
            }
            for (size_t width = 6; width < 166; width++)
            {
                if (ref_map)
                {
                    if (bitmap2[high + 160 - line][width - 6])
                        color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_CODE_COLOR);
                    else
                        color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_BG_COLOR);
                }
                else
                {
                    if (bitmap1[high + 160 - line][width - 6])
                        color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_CODE_COLOR);
                    else
                        color[high * LCD_Widht + width] = rgb_to_rgb565(SCREEN_BG_COLOR);
                }
            }
            for (size_t width = 166; width < LCD_Widht; width++)
            {
                color[high * LCD_Widht + width] = hsv_to_rgb565(cc * 255 / 640, 255, 255);
            }
            line--;
        }
        cc += WRITE_LINE_NUM;
        if (cc >= 640)
            cc = 0;
        sw_screen_roll(WRITE_LINE_NUM);
        display_refresh();
#endif
        if (line <= 1)
        {
            line = 160;
            xSemaphoreGive(font_render_semaphore);
        }
        vTaskDelay(2);
    }

#if USE_LVGL
    lv_init();
    ESP_LOGI(TAG, "初始化 LVGL 端口");
    lvgl_port_cfg_t port_cfg = {
        .task_priority = 4,       // LVGL任务优先级
        .task_stack = 1024 * 4,   // LVGL任务栈大小
        .task_affinity = 1,       // LVGL任务固定在核心（-1没有亲和力）
        .task_max_sleep_ms = 500, // LVGL任务的最大睡眠时间
        .timer_period_ms = 5,     // LVGL计时器滴答周期（毫秒）
    };
    lvgl_port_init(&port_cfg);
    ESP_LOGI(TAG, "初始化 LVGL");
    lv_display_t *display = lv_display_create(LCD_Widht, LCD_High);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);                                                          /*Set the color format of the display*/
    lv_display_set_buffers(display, color, NULL, LCD_Widht * LCD_High * sizeof(uint16_t), LV_DISPLAY_RENDER_MODE_PARTIAL); /*Initialize the display buffer.*/
    lv_display_set_flush_cb(display, my_disp_flush);
    scr = lv_disp_get_scr_act(display);                                                      /*Get the current screen*/
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT); /*Set the background color of the screen*/
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello, LVGL!");                                                    /*Create a label and set its text*/
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT); /*Set the text color of the label*/
    lv_obj_center(label);                                                                        /*Center the label on the screen*/
    ESP_LOGI(TAG, "显示初始化完成");
    xTaskCreatePinnedToCore(main_task, "main_run", 1024 * 5, NULL, 3, NULL, 0);
#endif
}
#if USE_LVGL
uint16_t zxc = 0;
void main_task(void *arg)
{
    while (1)
    {
        lv_obj_align(lv_obj_get_child(scr, 0), LV_ALIGN_CENTER, 0, zxc);
        zxc += 10;
        if (zxc >= 300)
        {
            zxc = 0;
        }
        vTaskDelay(30);
    }
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p)
{
    lcd_spi_block_write(area->x1, area->y1, area->x2, area->y2);
    lcd_spi_write_colors((const uint16_t *)color_p, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1));
    lv_display_flush_ready(disp); /*表示刷新完成*/
    ESP_LOGI(TAG, "flush_cb X1: %lu, Y1: %lu, X2: %lu, Y2: %lu", area->x1, area->y1, area->x2, area->y2);
}
#endif
