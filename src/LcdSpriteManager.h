// LCDManager.h
#ifndef __LCD_SPRITE_MANAGER_H__
#define __LCD_SPRITE_MANAGER_H__

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
// 定义文本边界结构体
struct TextBounds {
    int16_t x;       // 左边界
    int16_t y;       // 上边界
    uint16_t width;  // 文本宽度
    uint16_t height; // 文本高度
};

class LcdSpriteManager {
private:
    TFT_eSPI *tft;
    TFT_eSprite *sprite;

public:
    // 构造函数和析构函数
    LcdSpriteManager();
    ~LcdSpriteManager();
    
    // 初始化显示屏
    void init();
    
    // Sprite相关函数
    bool createSprite(int16_t width, int16_t height, uint8_t colorDepth = 16);
    void pushSprite(int32_t x, int32_t y, uint16_t transparentColor = 0xFFFF);
    void pushSprite(int32_t x, int32_t y, int32_t w, int32_t h, int32_t sx, int32_t sy);
    
    // 旋转设置
    void setRotation(uint8_t rotation);
    void setSpriteRotation(uint8_t rotation);
    
    // 清屏函数
    void fillScreen(uint32_t color);
    void fillSprite(uint32_t color);
    
    // 绘图函数
    void drawPixel(int32_t x, int32_t y, uint32_t color);
    void drawPixelAndUpdate(int32_t x, int32_t y, uint32_t color);
    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
    void drawHLine(int32_t x, int32_t y, int32_t w, uint32_t color);
    void drawVLine(int32_t x, int32_t y, int32_t h, uint32_t color);
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color);
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
    void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color1, uint16_t color2);
    // 使用原始图像数据绘制到 Sprite
    void drawImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data);

    // 如果需要，可以添加额外的方法来处理不同格式的图像数据
    //void drawRGBImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint8_t *data);
    // 文本相关函数
    void setTextColor(uint32_t color);
    void setTextColor(uint32_t fgcolor, uint32_t bgcolor);
    void setTextSize(uint8_t size);
    void setTextFont(uint8_t font);
    void setTextDatum(uint8_t datum);
    void setTextWrap(bool wrapX, bool wrapY = false);
    void drawString(const String &string, int32_t x, int32_t y);
    void drawString(const String &string, int32_t x, int32_t y, uint8_t font);
    void drawNumber(int32_t number, int32_t x, int32_t y);
    void drawFloat(float floatNumber, uint8_t decimal, int32_t x, int32_t y);
    // 在 LcdSpriteManager.h 的公共成员部分添加
    // 绘制单色位图
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor);
    // 绘制带有透明度的单色位图（背景不绘制）
    void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
    void loadFont(const uint8_t *font_data);
    // 中文汉字显示相关函数
    void drawChinese(int32_t x, int32_t y, const uint8_t *font_data, uint32_t fgcolor, uint32_t bgcolor);
    void drawChineseString(int32_t x, int32_t y, const char *text, const uint8_t **font_map, uint32_t fgcolor, uint32_t bgcolor);
    // 尺寸获取函数
    int32_t width();
    int32_t height();
    int32_t tftWidth();
    int32_t tftHeight();
    
    // 图像相关函数
    // bool drawJpeg(const char *filename, int32_t x, int32_t y);
    // bool drawJpeg(const char *filename, int32_t x, int32_t y, int32_t maxWidth, int32_t maxHeight);
    
    // 像素操作函数
    void setPixel(int32_t x, int32_t y, uint32_t color);
    uint16_t getPixel(int32_t x, int32_t y);
    void copySprite(int32_t destX, int32_t destY, int32_t sourceX, int32_t sourceY, int32_t width, int32_t height);
    void scroll(int16_t dx, int16_t dy);
    void setColorDepth(int8_t bits);
    
    // 更新屏幕
    void update();
    void update(int16_t x1,int16_t y1,int16_t x2,int16_t y2);
    // 获取原始对象
    TFT_eSPI* getTft();
    TFT_eSprite* getSprite();
    void set_scroll_window(int16_t top_fixed,int16_t scroll_content,int16_t bottom_fixed);
    void scroll_start(int16_t address);
    void draw_centered_box(int16_t w,int16_t h,uint16_t color);
    void draw_rectangle(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t front_color,uint16_t back_color);
    // 获取文字边界框
    TextBounds getTextBounds(const String &text, int32_t x, int32_t y, uint16_t textColor);
    void draw_pixel(int16_t x,int16_t y,uint16_t color);
};

#endif // __LCD_SPRITE_MANAGER_H__