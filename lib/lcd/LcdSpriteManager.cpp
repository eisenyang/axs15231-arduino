// LcdSpriteManager.cp
#include "LcdSpriteManager.h"
#include "lcd_arduino_spi.h"
// 构造函数
LcdSpriteManager::LcdSpriteManager() {
    tft = new TFT_eSPI();
    sprite = nullptr;
    //lcd_init();
}

// 析构函数
LcdSpriteManager::~LcdSpriteManager() {
    if (sprite != nullptr) {
        sprite->deleteSprite();
        delete sprite;
    }
    delete tft;
    lcd_arduino_spi_uninit();
}

// 初始化显示屏
void LcdSpriteManager::init() {
    //tft->init();
    //tft->setRotation(0);
    
    // 初始化时就创建一个全屏的sprite
    //createSprite(tft->width(), tft->height());
    lcd_arduino_spi_init();
    createSprite(LCD_WIDTH, LCD_HEIGHT);
    //sprite->setRotation(1);
}

// 创建精灵缓冲区
bool LcdSpriteManager::createSprite(int16_t width, int16_t height, uint8_t colorDepth) {
    if (sprite != nullptr) {
        sprite->deleteSprite();
        delete sprite;
    }
    
    sprite = new TFT_eSprite(tft);
    bool success = sprite->createSprite(width, height, colorDepth);
    
    if (success) {
        sprite->setSwapBytes(true);
    } else {
        // 如果创建失败，确保sprite为nullptr
        delete sprite;
        sprite = nullptr;
    }
    
    return success;
}

// 设置屏幕方向
void LcdSpriteManager::setRotation(uint8_t rotation) {
    tft->setRotation(rotation);
    if (sprite != nullptr) {
        sprite->setRotation(rotation);
    }
}

// 设置Sprite方向
void LcdSpriteManager::setSpriteRotation(uint8_t rotation) {
    if (sprite != nullptr) {
        sprite->setRotation(rotation);
    }
}

// 清屏（通过sprite实现）
void LcdSpriteManager::fillScreen(uint32_t color) {
    if (sprite != nullptr) {
        sprite->fillSprite(color);
        //pushSprite(0, 0); // 立即推送到屏幕
        lcd_arduino_spi_clear_screen(color);
    }
}

// 清除Sprite
void LcdSpriteManager::fillSprite(uint32_t color) {
    if (sprite != nullptr) {
        sprite->fillSprite(color);
    }
}

// 将Sprite推送到TFT屏幕
void LcdSpriteManager::pushSprite(int32_t x, int32_t y, uint16_t transparentColor) {
    if (sprite != nullptr) {
        if (transparentColor != 0xFFFF) {
            sprite->pushSprite(x, y, transparentColor);
        } else {
            sprite->pushSprite(x, y);
        }
    }
}

// 将Sprite部分区域推送到TFT屏幕
void LcdSpriteManager::pushSprite(int32_t x, int32_t y, int32_t w, int32_t h, int32_t sx, int32_t sy) {
    if (sprite != nullptr) {
        sprite->pushSprite(x, y, w, h, sx, sy);
    }
}
// 在Sprite上绘制像素点
void LcdSpriteManager::drawPixel(int32_t x, int32_t y, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawPixel(x, y, color);
    }
}

// 在Sprite上直接更新像素点到屏幕
void LcdSpriteManager::drawPixelAndUpdate(int32_t x, int32_t y, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawPixel(x, y, color);
        // 仅更新这一个像素点
        sprite->pushSprite(x, y, 1, 1, x, y);
    }
}

// 在Sprite上绘制线段
void LcdSpriteManager::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawLine(x0, y0, x1, y1, color);
    }
}

// 在Sprite上绘制水平线
void LcdSpriteManager::drawHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawFastHLine(x, y, w, color);
    }
}

// 在Sprite上绘制垂直线
void LcdSpriteManager::drawVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawFastVLine(x, y, h, color);
    }
}

// 在Sprite上绘制矩形
void LcdSpriteManager::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawRect(x, y, w, h, color);
    }
}

// 在Sprite上绘制填充矩形
void LcdSpriteManager::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (sprite != nullptr) {
        sprite->fillRect(x, y, w, h, color);
    }
}

// 在Sprite上绘制圆形
void LcdSpriteManager::drawCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawCircle(x, y, r, color);
    }
}

// 在Sprite上绘制填充圆形
void LcdSpriteManager::fillCircle(int32_t x, int32_t y, int32_t r, uint32_t color) {
    if (sprite != nullptr) {
        sprite->fillCircle(x, y, r, color);
    }
}

// 在Sprite上绘制圆角矩形
void LcdSpriteManager::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawRoundRect(x, y, w, h, r, color);
    }
}

// 在Sprite上绘制填充圆角矩形
void LcdSpriteManager::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color) {
    if (sprite != nullptr) {
        sprite->fillRoundRect(x, y, w, h, r, color);
    }
}

// 在Sprite上绘制三角形
void LcdSpriteManager::drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawTriangle(x0, y0, x1, y1, x2, y2, color);
    }
}

// 在Sprite上绘制填充三角形
void LcdSpriteManager::fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
    if (sprite != nullptr) {
        sprite->fillTriangle(x0, y0, x1, y1, x2, y2, color);
    }
}

// 设置文本颜色
void LcdSpriteManager::setTextColor(uint32_t color) {
    if (sprite != nullptr) {
        sprite->setTextColor(color);
    }
}

// 设置文本颜色（带背景色）
void LcdSpriteManager::setTextColor(uint32_t fgcolor, uint32_t bgcolor) {
    if (sprite != nullptr) {
        sprite->setTextColor(fgcolor, bgcolor);
    }
}

// 设置文本大小
void LcdSpriteManager::setTextSize(uint8_t size) {
    if (sprite != nullptr) {
        sprite->setTextSize(size);
    }
}

// 设置文本字体
void LcdSpriteManager::setTextFont(uint8_t font) {
    if (sprite != nullptr) {
        sprite->setTextFont(font);
    }
}

// 设置文本对齐方式
void LcdSpriteManager::setTextDatum(uint8_t datum) {
    if (sprite != nullptr) {
        sprite->setTextDatum(datum);
    }
}

// 设置文本换行模式
void LcdSpriteManager::setTextWrap(bool wrapX, bool wrapY) {
    if (sprite != nullptr) {
        sprite->setTextWrap(wrapX, wrapY);
    }
}

// 在Sprite上绘制文本
void LcdSpriteManager::drawString(const String &string, int32_t x, int32_t y) {
    if (sprite != nullptr) {
        sprite->drawString(string, x, y);
        //sprite->println(string);







        // uint16_t *frameBuffer = (uint16_t*)sprite->getPointer();
        // if(!frameBuffer) {
        //     Serial.println("无法获取帧缓冲区");
        //     return;
        // }

        // int width = sprite->width();
        // int height = sprite->height();
        
        // // 读取所有像素
        // for(int y0=0; y0<height; y0++) {
        //     for(int x0=0; x0<width; x0++) {
        //     uint16_t pixel = frameBuffer[y0 * width + x0];
        //     lcd_block_write(x0+x, y0+y, x0+x+1,y0+y+1);
        //     uint16_t color = sprite->readPixel(x0, y0);
        //      lcd_write_color(color);
        //     // 处理像素数据...
        //     }
        // }
    }
}

// 在Sprite上绘制带大小的文本
void LcdSpriteManager::drawString(const String &string, int32_t x, int32_t y, uint8_t font) {
    if (sprite != nullptr) {
        sprite->drawString(string, x, y, font);
    }
}

// 在Sprite上绘制数字
void LcdSpriteManager::drawNumber(int32_t number, int32_t x, int32_t y) {
    if (sprite != nullptr) {
        sprite->drawNumber(number, x, y);
    }
}

// 在Sprite上绘制浮点数
void LcdSpriteManager::drawFloat(float floatNumber, uint8_t decimal, int32_t x, int32_t y) {
    if (sprite != nullptr) {
        sprite->drawFloat(floatNumber, decimal, x, y);
    }
}

// 获取当前使用对象的宽度
int32_t LcdSpriteManager::width() {
    if (sprite != nullptr) {
        return sprite->width();
    }
    //return tft->width();
    return LCD_WIDTH;
}

// 获取当前使用对象的高度
int32_t LcdSpriteManager::height() {
    if (sprite != nullptr) {
        return sprite->height();
    }
    //return tft->height();
    return LCD_HEIGHT;
}

// 获取TFT宽度
int32_t LcdSpriteManager::tftWidth() {
    //return tft->width();
    return LCD_WIDTH;
}

// 获取TFT高度
int32_t LcdSpriteManager::tftHeight() {
    //return tft->height();
    return LCD_HEIGHT;
}

// 使用 pushImage 在 Sprite 上绘制图像
void LcdSpriteManager::drawImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data) {
    if (sprite != nullptr) {
        sprite->pushImage(x, y, w, h, data);
    }
}

// 设置Sprite的像素颜色
void LcdSpriteManager::setPixel(int32_t x, int32_t y, uint32_t color) {
    if (sprite != nullptr) {
        sprite->drawPixel(x, y, color);
    }
}

// 获取Sprite上某个位置的像素颜色
uint16_t LcdSpriteManager::getPixel(int32_t x, int32_t y) {
    if (sprite != nullptr) {
        return sprite->readPixel(x, y);
    }
    return 0;
}

// 将一个Sprite区域复制到另一个位置
void LcdSpriteManager::copySprite(int32_t destX, int32_t destY, int32_t sourceX, int32_t sourceY, int32_t width, int32_t height) {
    if (sprite != nullptr) {
        for (int32_t y = 0; y < height; y++) {
            for (int32_t x = 0; x < width; x++) {
                uint16_t color = sprite->readPixel(sourceX + x, sourceY + y);
                sprite->drawPixel(destX + x, destY + y, color);
            }
        }
    }
}

// 实现滚动效果
void LcdSpriteManager::scroll(int16_t dx, int16_t dy) {
    if (sprite != nullptr) {
        // 创建临时缓冲区
        TFT_eSprite tempSprite(tft);
        tempSprite.createSprite(sprite->width(), sprite->height());
        
        // 复制原Sprite内容
        for (int16_t y = 0; y < sprite->height(); y++) {
            for (int16_t x = 0; x < sprite->width(); x++) {
                tempSprite.drawPixel(x, y, sprite->readPixel(x, y));
            }
        }
        
        // 清除原Sprite
        sprite->fillSprite(TFT_BLACK); // 或使用其他背景色
        
        // 将内容滚动后复制回原Sprite
        for (int16_t y = 0; y < sprite->height(); y++) {
            for (int16_t x = 0; x < sprite->width(); x++) {
                int16_t srcX = x - dx;
                int16_t srcY = y - dy;
                
                if (srcX >= 0 && srcX < sprite->width() && srcY >= 0 && srcY < sprite->height()) {
                    sprite->drawPixel(x, y, tempSprite.readPixel(srcX, srcY));
                }
            }
        }
        
        tempSprite.deleteSprite();
    }
}

// 设置Sprite颜色深度
void LcdSpriteManager::setColorDepth(int8_t bits) {
    if (sprite != nullptr) {
        sprite->setColorDepth(bits);
    }
}

// 添加一个方法来更新屏幕显示
void LcdSpriteManager::update() {
    if (sprite != nullptr) {
        //pushSprite(0, 0);
        lcd_arduino_spi_block_write(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
        for (int16_t y = 0; y < LCD_HEIGHT; y++) {
            for (int16_t x = 0; x < LCD_WIDTH; x++) {
                uint16_t color = sprite->readPixel(x, y);
                lcd_arduino_spi_write_color(color);
            }
        }

    }
}
// 添加一个方法来更新屏幕显示
void LcdSpriteManager::update(int16_t x1,int16_t y1,int16_t x2,int16_t y2) {
    if (sprite != nullptr) {
        //pushSprite(0, 0);
        lcd_arduino_spi_block_write(x1, y1, x2, y2);
        for (int16_t y = y1; y <= y2; y++) {
            for (int16_t x = x1; x <= x2; x++) {
                uint16_t color = sprite->readPixel(x, y);
                lcd_arduino_spi_write_color(color);
            }
        }
    }
}
// 获取原始TFT_eSPI对象的指针
TFT_eSPI* LcdSpriteManager::getTft() {
    return tft;
}

// 获取TFT_eSprite对象的指针
TFT_eSprite* LcdSpriteManager::getSprite() {
    return sprite;
}

// 绘制单个汉字
// font_data: 汉字点阵数据, 16x16位图 (32字节)
void LcdSpriteManager::drawChinese(int32_t x, int32_t y, const uint8_t *font_data, uint32_t fgcolor, uint32_t bgcolor) {
    if (sprite == nullptr || font_data == nullptr) return;
    
    // 默认16x16大小的汉字点阵
    const int font_width = 16;
    const int font_height = 16;
    
    // 逐行逐列绘制汉字
    for (int row = 0; row < font_height; row++) {
        // 汉字16x16点阵, 每行2个字节, 每个字节8个点
        uint8_t high_byte = font_data[row * 2];
        uint8_t low_byte = font_data[row * 2 + 1];
        
        for (int col = 0; col < 8; col++) {
            // 绘制高字节部分(前8位)
            if (high_byte & (0x80 >> col)) {
                sprite->drawPixel(x + col, y + row, fgcolor);
            } else if (bgcolor != fgcolor) {
                sprite->drawPixel(x + col, y + row, bgcolor);
            }
            
            // 绘制低字节部分(后8位)
            if (low_byte & (0x80 >> col)) {
                sprite->drawPixel(x + col + 8, y + row, fgcolor);
            } else if (bgcolor != fgcolor) {
                sprite->drawPixel(x + col + 8, y + row, bgcolor);
            }
        }
    }
}

// 绘制中文字符串
// text: UTF-8编码的中文字符串
// font_map: 字体映射表, 包含所有可用汉字的点阵数据
void LcdSpriteManager::drawChineseString(int32_t x, int32_t y, const char *text, const uint8_t **font_map, uint32_t fgcolor, uint32_t bgcolor) {
    if (sprite == nullptr || text == nullptr || font_map == nullptr) return;
    
    int pos_x = x;
    int pos_y = y;
    size_t len = strlen(text);
    
    // 逐个解析UTF-8字符
    for (size_t i = 0; i < len;) {
        if ((text[i] & 0x80) == 0) {
            // ASCII字符 (单字节)
            if (text[i] == '\n') {
                pos_x = x;
                pos_y += 16; // 换行
            } else if (text[i] == '\r') {
                // 忽略回车
            } else {
                // 如果是ASCII字符, 使用默认的drawChar方法
                sprite->drawChar(text[i], pos_x, pos_y, 1);
                pos_x += 8; // ASCII字符宽度
            }
            i++;
        } else if ((text[i] & 0xE0) == 0xC0) {
            // 双字节UTF-8字符 (拉丁文扩展等)
            i += 2;
            pos_x += 8;
        } else if ((text[i] & 0xF0) == 0xE0) {
            // 三字节UTF-8字符 (中文、日文等)
            if (i + 2 < len) {
                // 计算汉字的索引
                uint8_t b1 = text[i];
                uint8_t b2 = text[i+1];
                uint8_t b3 = text[i+2];
                
                // 从UTF-8转换为Unicode
                uint16_t unicode = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
                
                // 这里需要根据实际字库实现字符查找
                // 假设font_map是一个直接映射Unicode到点阵数据的表
                // 实际情况可能需要更复杂的查找逻辑
                int font_index = unicode - 0x4E00; // 假设字库从中文"一"(U+4E00)开始
                
                if (font_index >= 0 && font_map[font_index] != nullptr) {
                    drawChinese(pos_x, pos_y, font_map[font_index], fgcolor, bgcolor);
                }
                
                pos_x += 16; // 汉字宽度
                i += 3;
            } else {
                i++; // 不完整的UTF-8字符, 跳过
            }
        } else if ((text[i] & 0xF8) == 0xF0) {
            // 四字节UTF-8字符 (表情符号等)
            i += 4;
            pos_x += 16;
        } else {
            // 无效的UTF-8字符
            i++;
        }
        
        // 如果文本超出屏幕宽度, 自动换行
        if (pos_x > width() - 16) {
            pos_x = x;
            pos_y += 16;
        }
    }
}

// 绘制单色位图
void LcdSpriteManager::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    if (sprite != nullptr && bitmap != nullptr) {
        sprite->drawBitmap(x, y, bitmap, w, h, color);
    }
}

// 绘制双色位图
void LcdSpriteManager::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor) {
    if (sprite != nullptr && bitmap != nullptr) {
        sprite->drawBitmap(x, y, bitmap, w, h, fgcolor, bgcolor);
    }
}

// 绘制水平渐变色矩形
void LcdSpriteManager::fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color1, uint16_t color2) {
    if (sprite != nullptr) {
        sprite->fillRectHGradient(x, y, w, h, color1, color2);
    }
}


// 绘制XBitmap
void LcdSpriteManager::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
    if (sprite != nullptr && bitmap != nullptr) {
        sprite->drawXBitmap(x, y, bitmap, w, h, color);
    }
}

// 加载字体
void LcdSpriteManager::loadFont(const uint8_t *font_data) {
    if (sprite != nullptr) {
        sprite->loadFont(font_data);
    }
}

// 获取文字边界框
TextBounds LcdSpriteManager::getTextBounds(const String &text, int32_t x, int32_t y, uint16_t textColor) {
    if (sprite == nullptr) {
        return {0, 0, 0, 0}; // 如果没有sprite，返回空边界
    }
    
    // 保存当前状态
    uint16_t originalTextColor = sprite->textcolor;
    uint16_t originalBgColor = sprite->textbgcolor;
    uint8_t originalDatum = sprite->getTextDatum();
    int32_t cursorX = sprite->getCursorX();
    int32_t cursorY = sprite->getCursorY();
    
    // 清除精灵缓冲区
    uint16_t bgColor = BLACK; // 背景色，与文本颜色对比明显
    if (textColor == BLACK) {
        bgColor = WHITE;
    }
    sprite->fillSprite(bgColor);
    
    // 设置文本属性
    sprite->setTextColor(textColor, bgColor);
    sprite->setTextDatum(TL_DATUM); // 左上角对齐
    
    // 绘制文本
    sprite->drawString(text, x, y);
    
    // 初始化边界值
    int16_t minX = sprite->width();
    int16_t minY = sprite->height();
    int16_t maxX = 0;
    int16_t maxY = 0;
    bool foundPixel = false;
    
    // 扫描整个sprite，寻找文本像素
    for (int16_t scanY = 0; scanY < sprite->height(); scanY++) {
        for (int16_t scanX = 0; scanX < sprite->width(); scanX++) {
            uint16_t pixelColor = sprite->readPixel(scanX, scanY);
            
            // 如果像素是文本颜色
            if (pixelColor == textColor) {
                foundPixel = true;
                
                // 更新边界
                if (scanX < minX) minX = scanX;
                if (scanY < minY) minY = scanY;
                if (scanX > maxX) maxX = scanX;
                if (scanY > maxY) maxY = scanY;
            }
        }
    }
    
    // 恢复原始状态
    sprite->setTextColor(originalTextColor, originalBgColor);
    sprite->setTextDatum(originalDatum);
    sprite->setCursor(cursorX, cursorY);
    
    // 如果没有找到文本像素
    if (!foundPixel) {
        return {x, y, 0, 0};
    }
    
    // 计算文本宽度和高度
    uint16_t textWidth = maxX - minX + 1;
    uint16_t textHeight = maxY - minY + 1;
    
    // 清除临时绘制的内容
    sprite->fillSprite(bgColor);
    
    // 返回文本边界
    return {minX, minY, textWidth, textHeight};
}

void LcdSpriteManager::set_scroll_window(int16_t top_fixed,int16_t scroll_content,int16_t bottom_fixed) {
    lcd_arduino_spi_set_scroll_window(top_fixed, scroll_content, bottom_fixed);
}

void LcdSpriteManager::scroll_start(int16_t address) {
    lcd_arduino_spi_scroll_start(address);
}

void LcdSpriteManager::draw_rectangle(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t front_color,uint16_t back_color) {
    //lcd_arduino_spi_draw_frame(x, y, x + w, y + h, front_color, back_color);
}

void LcdSpriteManager::draw_pixel(int16_t x,int16_t y,uint16_t color) {
    lcd_arduino_spi_draw_pixel(x, y, color);
}

void LcdSpriteManager::draw_centered_box(int16_t w,int16_t h,uint16_t color) {
    // 计算中心位置
  uint16_t centerX = LCD_WIDTH / 2;
  uint16_t centerY = LCD_HEIGHT / 2;

  // 计算方块的左上角和右下角坐标
  uint16_t x1 = centerX - (w / 2);
  uint16_t y1 = centerY - (h / 2);
  uint16_t x2 = x1 + w;
  uint16_t y2 = y1 + h;

  // 确保坐标不超出屏幕范围
  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 >= LCD_WIDTH) x2 = LCD_WIDTH - 1;
  if (y2 >= LCD_HEIGHT) y2 = LCD_HEIGHT - 1;

  // 绘制方块
  draw_rectangle(x1, y1, x2, y2, color, color);
}