#pragma once
#include <stdint.h>

void lcd_color_helper_rgb565_to_rgb888(uint16_t color, uint8_t *data)
{
    // 将16位颜色值转换为RGB格式
    // 16位颜色格式: RRRRR/GGGGGG/BBBBB
    data[0] = ((color >> 11) & 0x1F) * 255 / 31; // 提取5位R并缩放到0-255
    data[1] = ((color >> 5) & 0x3F) * 255 / 63;  // 提取6位G并缩放到0-255
    data[2] = (color & 0x1F) * 255 / 31;         // 提取5位B并缩放到0-255
}