#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include "esp_log.h"
// static char *TAG = "BUFF";
// uint16_t color[LCD_High][LCD_Widht];
bool ref_map = 0;
bool bitmap1[160][160];
bool bitmap2[160][160];

#include "vector_font.h"
extern FT_Face s_face;

uint16_t *color = NULL;
extern const uint8_t code_bitmap[];
extern const lv_font_fmt_txt_glyph_dsc_t bit_glyph_dsc[];


void screen_buf_init()
{
    color = (uint16_t *)heap_caps_calloc(1, LCD_Widht * LCD_High * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
}

void screen_buf_uninit()
{
    if (color != NULL)
    {
        heap_caps_free(color);
        color = NULL;
    }
}
// 软件滚动屏幕
void sw_screen_roll(uint16_t speed)
{
    uint16_t data[speed][LCD_Widht];
    for (int high = 0; high < speed; high++)
    {
        for (size_t width = 0; width < LCD_Widht; width++)
            // data[high][width] = color[high][width];
            data[high][width] = color[high * LCD_Widht + width];
    }
    for (size_t high = 0; high < LCD_High - speed; high++)
    {
        for (size_t width = 0; width < LCD_Widht; width++)
        {
            color[high * LCD_Widht + width] = color[(high + speed) * LCD_Widht + width];
        }
    }
    for (int high = 0; high < speed; high++)
    {
        for (size_t width = 0; width < LCD_Widht; width++)
            color[(LCD_High - speed + high) * LCD_Widht + width] = data[high][width];
    }
}
// 刷新屏幕
void display_refresh()
{
    lcd_spi_block_write(0, 0, LCD_Widht - 1, LCD_High - 1);
    lcd_spi_write_colors((uint16_t *)color, LCD_Widht * LCD_High);

    ESP_LOGI("SCREEN", "刷屏完成");
}
// 全屏填充颜色
void lcd_write_color_full(uint16_t color_)
{
    for (size_t i = 0; i < LCD_High * LCD_Widht; i++)
    {
        color[i] = color_;
    }
    // for (size_t high = 0; high < LCD_High; high++)
    //     for (size_t width = 0; width < LCD_Widht; width++)
    //         // color_data[high * LCD_Widht + width] = color_;
    //         color[high][width] = color_;
    display_refresh();
    ESP_LOGI("SCREEN", "清屏完成");
}

void code_read(uint16_t list_num)
{
    uint16_t ofs_y = (160 - bit_glyph_dsc[list_num].box_h) / 2;
    uint16_t ofs_x = (160 - bit_glyph_dsc[list_num].box_w) / 2;
    if (DISPLAY_DIRECT_VAL == DISPLAY_DIRECT_VAL_0)
    {
        for (size_t high = 0; high < ofs_y; high++) // 顶部填充
            for (size_t width = 0; width < 160; width++)
                if (ref_map)
                    bitmap2[high][width] = 0;
                else
                    bitmap1[high][width] = 0;

        for (size_t high = ofs_y; high < bit_glyph_dsc[list_num].box_h + ofs_y; high++)
        {
            for (size_t width = 0; width < ofs_x; width++)
                if (ref_map)
                    bitmap2[high][width] = 0;
                else
                    bitmap1[high][width] = 0;
            for (size_t width = ofs_x; width < bit_glyph_dsc[list_num].box_w + ofs_x; width++)
            {
                uint8_t temp = code_bitmap[bit_glyph_dsc[list_num].bitmap_index + (high * bit_glyph_dsc[list_num].box_w + width - ofs_x) / 8];
                if ((temp >> (7 - (high * bit_glyph_dsc[list_num].box_w + width - ofs_x) % 8)) & 1)
                {
                    if (ref_map)
                        bitmap2[high][width] = 1;
                    else
                        bitmap1[high][width] = 1;
                }
                else
                {
                    if (ref_map)
                        bitmap2[high][width] = 0;
                    else
                        bitmap1[high][width] = 0;
                }
            }
            for (size_t width = ofs_x + bit_glyph_dsc[list_num].box_w; width < 160; width++)
                if (ref_map)
                    bitmap2[high][width] = 0;
                else
                    bitmap1[high][width] = 0;
        }
        for (size_t high = bit_glyph_dsc[list_num].box_h + ofs_y; high < 160; high++) // 底部填充
            for (size_t width = 0; width < 160; width++)
                if (ref_map)
                    bitmap2[high][width] = 0;
                else
                    bitmap1[high][width] = 0;
    }
    else if (DISPLAY_DIRECT_VAL == DISPLAY_DIRECT_VAL_90)
    {
    }
    else if (DISPLAY_DIRECT_VAL == DISPLAY_DIRECT_VAL_180)
    {
        for (size_t high = 0; high < ofs_y; high++) // 顶部填充
            for (size_t width = 0; width < 160; width++)
                if (ref_map)
                    bitmap2[159 - high][159 - width] = 0;
                else
                    bitmap1[159 - high][159 - width] = 0;
        for (size_t high = ofs_y; high < bit_glyph_dsc[list_num].box_h + ofs_y; high++)
        {
            for (size_t width = 0; width < ofs_x; width++)
                if (ref_map)
                    bitmap2[159 - high][159 - width] = 0;
                else
                    bitmap1[159 - high][159 - width] = 0;
            for (size_t width = ofs_x; width < bit_glyph_dsc[list_num].box_w + ofs_x; width++)
            {
                uint8_t temp = code_bitmap[bit_glyph_dsc[list_num].bitmap_index + ((high - ofs_y) * bit_glyph_dsc[list_num].box_w + width - ofs_x) / 8];
                if ((temp >> (7 - ((high - ofs_y) * bit_glyph_dsc[list_num].box_w + width - ofs_x) % 8)) & 1)
                {
                    if (ref_map)
                        bitmap2[159 - high][159 - width] = 1;
                    else
                        bitmap1[159 - high][159 - width] = 1;
                }
                else
                {
                    if (ref_map)
                        bitmap2[159 - high][159 - width] = 0;
                    else
                        bitmap1[159 - high][159 - width] = 0;
                }
            }
            for (size_t width = ofs_x + bit_glyph_dsc[list_num].box_w; width < 160; width++)
                if (ref_map)
                    bitmap2[159 - high][159 - width] = 0;
                else
                    bitmap1[159 - high][159 - width] = 0;
        }
        for (size_t high = bit_glyph_dsc[list_num].box_h + ofs_y; high < 160; high++) // 底部填充
            for (size_t width = 0; width < 160; width++)
                if (ref_map)
                    bitmap2[159 - high][159 - width] = 0;
                else
                    bitmap1[159 - high][159 - width] = 0;
    }
    else if ((DISPLAY_DIRECT_VAL == DISPLAY_DIRECT_VAL_270))
    {
    }
}
