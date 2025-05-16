#include "SpriteTextManager.h"
#include "lcd_spi_api.h"
SpriteTextManager::SpriteTextManager(){
    tft = new TFT_eSPI();
}

SpriteTextManager::~SpriteTextManager(){
    delete tft;
}

void SpriteTextManager::init(uint16_t width, uint16_t height){
    //tft->init();
    //tft->setRotation(0);
    //tft->fillScreen(TFT_BLACK);
    lcd_spi_init();
    createSprite(width, height,16);
}
void SpriteTextManager::init(){
   init(SPRITE_WIDTH, SPRITE_HEIGHT);
}

// 创建精灵缓冲区
bool SpriteTextManager::createSprite(uint16_t width, uint16_t height, uint16_t colorDepth) {
    if (sprite != nullptr) {
        sprite->deleteSprite();
        delete sprite;
    }

    sprite = new TFT_eSprite(tft);
    sprite->setColorDepth(colorDepth);
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

void SpriteTextManager::fillScreen(uint16_t color){
    if (sprite != nullptr) {
        sprite->fillScreen(color);
    }
    lcd_spi_clear_screen(color);
}

void SpriteTextManager::loadFont(const uint8_t *font_data){
    if (sprite != nullptr) {
        sprite->loadFont(font_data);
    }
}

void SpriteTextManager::drawCenterString(const String &string){
    if (sprite != nullptr) {
          // 设置文本属性
        sprite->setTextColor(TFT_RED, TFT_BLACK);
        sprite->setTextFont(2); // 使用较大的内置字体
        sprite->setTextDatum(TL_DATUM);

        //   // 计算文本尺寸
        //String text = "Hello World";
        uint16_t textWidth = sprite->textWidth(string);
        uint16_t textHeight = sprite->fontHeight();

        // 居中显示文本
        uint16_t x = (sprite->width() - textWidth) / 2;
        uint16_t y = (sprite->height() - textHeight) / 2;

        // 绘制文本
        sprite->fillSprite(TFT_BLACK);
        sprite->drawString(string, x, y);
    }
}

void SpriteTextManager::setScrollWindow(int16_t top_fixed,int16_t scroll_content,int16_t bottom_fixed){
    if (sprite != nullptr) {
        lcd_spi_set_scroll_window(top_fixed, scroll_content, bottom_fixed); 
    }
}

void SpriteTextManager::scrollStart(int16_t address){
    if (sprite != nullptr) {
        lcd_spi_scroll_start(address);
    }
}

void SpriteTextManager::drawPixel2LCD(int16_t x,int16_t y,uint16_t color){
    lcd_spi_block_write(x, y, x, y);
    lcd_spi_write_color(color);
}
void SpriteTextManager::drawRow2LCD(uint16_t y,uint16_t color){
    lcd_spi_draw_row(y, color);
}
void SpriteTextManager::setRowAddress(uint16_t x_offset,uint16_t y){
    lcd_spi_block_write(x_offset,y,LCD_WIDTH-1,y);
}
void SpriteTextManager::setRowAddress(uint16_t x_offset,uint16_t y,uint16_t row){
    lcd_spi_block_write(x_offset,y,LCD_WIDTH-1,y+row-1);
   // Serial.println("申请屏显地址:"+String(x_offset)+","+String(y)+","+String(row)+","+String(y+row-1));
}
void SpriteTextManager::blockLcdWrite(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1){
    lcd_spi_block_write(x0, y0, x1, y1);
}
uint16_t SpriteTextManager::getPixel(int16_t x,int16_t y){
    if (sprite != nullptr) {
        return sprite->readPixel(x, y);
    }
    return 0;
}

void SpriteTextManager::clearSprite(){
    if (sprite != nullptr) {
        sprite->fillScreen(TFT_BLACK);
    }
}

// void SpriteTextManager::enableWriteColor(){
//     lcd_spi_start_write_color();
// }
// void SpriteTextManager::disableWriteColor(){
//     lcd_spi_end_write_color();
// }
void SpriteTextManager::writeColor(uint16_t color){   
    lcd_spi_write_color(color);
}
void SpriteTextManager::writeColors(const uint16_t* colors,size_t length){
    lcd_spi_write_colors(colors,length);
}
void SpriteTextManager::setPixel(int16_t x,int16_t y,uint16_t color){
    if (sprite != nullptr) {
        sprite->drawPixel(x, y, color);
    }
}