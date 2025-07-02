#include "vector_font.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <stdlib.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "ft2build.h"
#include "config.h"

static const char *TAG = "vector_font";

static FT_Library s_library;
FT_Face s_face;
extern bool ref_map;
extern bool bitmap1[160][160];
extern bool bitmap2[160][160];
extern TaskHandle_t vector_code_task_handle;
void vector_font_init(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 1,
    };

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    FT_Error error = FT_Init_FreeType(&s_library);
    if (error)
    {
        ESP_LOGE(TAG, "初始化FreeType库时出错: %d", error);
        abort();
    }
    error = FT_New_Face(s_library,
                        "/spiffs/simhei.ttf",
                        0,
                        &s_face);
    if (error)
    {
        ESP_LOGE(TAG, "加载字体时出错: %d", error);
        abort();
    }
}

void vector_font_render_code(FT_ULong char_code, bool ref_map_buff)
{
    if (!s_face)
    {
        ESP_LOGE(TAG, "字体未加载");
        return;
    }
    FT_Set_Char_Size(s_face, 0, 200 * 50, 0, 0);

    FT_UInt glyph_index = FT_Get_Char_Index(s_face, char_code);
    if (glyph_index == 0)
    {
        ESP_LOGE(TAG, "字符码点 %lx 没有对应字形", char_code);
        return;
    }
    vTaskDelay(1);
    FT_Error error = FT_Load_Glyph(s_face, glyph_index, FT_LOAD_DEFAULT);
    if (error)
    {
        ESP_LOGE(TAG, "加载字形失败: %d", error);
        return;
    }
    vTaskDelay(1);
    error = FT_Render_Glyph(s_face->glyph, FT_RENDER_MODE_MONO);
    if (error)
    {
        ESP_LOGE(TAG, "渲染字形失败: %d", error);
        return;
    }
    ESP_LOGI(TAG, "渲染字形成功");
    vTaskDelay(1);
    /*将字形位图复制到整个位图中*/
    FT_GlyphSlot slot = s_face->glyph;
    if (!slot || !slot->bitmap.buffer)
    {
        ESP_LOGE(TAG, "字形或位图数据为空");
        return;
    }
    if (s_face->glyph->bitmap.pitch * 8 > 160 || s_face->glyph->bitmap.rows > 160 || s_face->glyph->bitmap.width > 160)
    {
        ESP_LOGE(TAG, "字形位图大小超过限制");
        return;
    }
    vTaskDelay(1);
    for (int iy = 0; iy < slot->bitmap.rows; iy++)
    {
        for (int ix = 0; ix < slot->bitmap.pitch * 8; ix++)
        {
            if (DISPLAY_DIRECT_VAL == DISPLAY_DIRECT_VAL_180)
            {
                uint8_t temp = slot->bitmap.buffer[(iy * slot->bitmap.pitch * 8 + ix) / 8];
                if ((temp >> (7 - (iy * slot->bitmap.pitch * 8 + ix) % 8)) & 1)
                {
                    if (ref_map_buff)
                        bitmap1[159 - iy][159 - ix] = 1;
                    else
                        bitmap2[159 - iy][159 - ix] = 1;

                    // putchar('#');
                }
                else
                {
                    if (ref_map_buff)
                        bitmap1[159 - iy][159 - ix] = 0;
                    else
                        bitmap2[159 - iy][159 - ix] = 0;
                    // putchar(' ');
                }
            }
        }
        for (int ix = slot->bitmap.pitch * 8; ix < 160; ix++)
            if (ref_map_buff)
                bitmap1[159 - iy][159 - ix] = 0;
            else
                bitmap2[159 - iy][159 - ix] = 0;
        // putchar('\n');
    }
    for (int iy = slot->bitmap.rows; iy < 160; iy++)
    {
        for (int ix = 0; ix < 160; ix++)
            if (ref_map_buff)
                bitmap1[159 - iy][159 - ix] = 0;
            else
                bitmap2[159 - iy][159 - ix] = 0;
    }

    // ESP_LOGI(TAG, "字形位图显示 宽度: %d", s_face->glyph->bitmap.width);
    // ESP_LOGI(TAG, "字形位图显示 高度: %d", s_face->glyph->bitmap.rows);
    // ESP_LOGI(TAG, "字形位图显示 行字节数: %d", s_face->glyph->bitmap.pitch);
    // ESP_LOGI(TAG, "字形位图显示 像素数据指针: %p", s_face->glyph->bitmap.buffer);
    // ESP_LOGI(TAG, "字形位图显示 灰度级数: %d", s_face->glyph->bitmap.num_grays);
    // ESP_LOGI(TAG, "字形位图显示 像素模式: %d", s_face->glyph->bitmap.pixel_mode);
}
