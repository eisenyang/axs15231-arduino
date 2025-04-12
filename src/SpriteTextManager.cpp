#include "SpriteTextManager.h"
#include "lcd.h"   
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
    lcd_init();
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
    lcd_clear_screen(color);
}

void SpriteTextManager::loadFont(const uint8_t *font_data){
    if (sprite != nullptr) {
        sprite->loadFont(font_data);
    }
}

void SpriteTextManager::drawString(const String &string){
    if (sprite != nullptr) {
        sprite->setTextColor(TFT_WHITE);
        sprite->drawString(string, 0, 0);
        //sprite->setTextDatum();
        //sprite->drawCentreString(string, 0, 0);
    }
}

void SpriteTextManager::setScrollWindow(int16_t top_fixed,int16_t scroll_content,int16_t bottom_fixed){
    if (sprite != nullptr) {
        lcd_set_scroll_window(top_fixed, scroll_content, bottom_fixed); 
    }
}

void SpriteTextManager::scrollStart(int16_t address){
    if (sprite != nullptr) {
        lcd_scroll_start(address);
    }
}

void SpriteTextManager::draw2LCD(int16_t x,int16_t y,uint16_t color){
    lcd_draw_pixel(x, y, color);
}

uint16_t SpriteTextManager::getPixel(int16_t x,int16_t y){
    if (sprite != nullptr) {
        return sprite->readPixel(x, y);
    }
    return 0;
}
