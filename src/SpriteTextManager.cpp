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
    
    if(sprite_rotate != nullptr){
        sprite_rotate->deleteSprite();
        delete sprite_rotate;
    }
    sprite = new TFT_eSprite(tft);
    sprite->setColorDepth(colorDepth);

    sprite_rotate = new TFT_eSprite(tft);
    sprite_rotate->setColorDepth(colorDepth);
    bool success = sprite->createSprite(width, height, colorDepth);
    bool success_rotate = sprite_rotate->createSprite(width, height, colorDepth);
    if (success) {
        sprite->setSwapBytes(true);
    } else {
        // 如果创建失败，确保sprite为nullptr
        delete sprite;
        sprite = nullptr;
    }
    if (success_rotate) {
        sprite_rotate->setSwapBytes(true);
    } else {
        // 如果创建失败，确保sprite_rotate为nullptr
        delete sprite_rotate;
        sprite_rotate = nullptr;
    }   
    return success;
}

void SpriteTextManager::fillScreen(uint16_t color){
    if (sprite != nullptr) {
        sprite->fillScreen(color);
    }
    if (sprite_rotate != nullptr) {
        sprite_rotate->fillScreen(color);
    }
    lcd_clear_screen(color);
}

void SpriteTextManager::loadFont(const uint8_t *font_data){
    if (sprite != nullptr) {
        sprite->loadFont(font_data);
    }
    if (sprite_rotate != nullptr) {
        sprite_rotate->loadFont(font_data);
    }
}

void SpriteTextManager::drawCenterString(const String &string){
    if (sprite != nullptr) {
          // 设置文本属性
        sprite->setTextColor(TFT_WHITE, TFT_BLACK);
        sprite->setTextFont(2); // 使用较大的内置字体
        sprite->setTextDatum(TL_DATUM);

          // 计算文本尺寸
        //String text = "Hello World";
        uint16_t textWidth = sprite->textWidth(string);
        uint16_t textHeight = sprite->fontHeight();

        // 居中显示文本
        uint16_t x = (sprite->width() - textWidth) / 2;
        uint16_t y = (sprite->height() - textHeight) / 2;

        // 绘制文本
        sprite->fillSprite(TFT_BLACK);
        sprite->drawString(string, x, y);
        //sprite->pushSprite(0, 0); 





                  // 设置文本属性
        sprite_rotate->setTextColor(TFT_WHITE, TFT_BLACK);
        sprite_rotate->setTextFont(2); // 使用较大的内置字体
        sprite_rotate->setTextDatum(TL_DATUM);

          // 计算文本尺寸
        //String text = "Hello World";
        textWidth = sprite_rotate->textWidth(string);
        textHeight = sprite_rotate->fontHeight();

        // 居中显示文本
        x = (sprite_rotate->width() - textWidth) / 2;
        y = (sprite_rotate->height() - textHeight) / 2;

        // 绘制文本
        sprite_rotate->fillSprite(TFT_BLACK);
        sprite_rotate->drawString(string, x, y);
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
        //return sprite_rotate->readPixel(x, y);
    }
    return 0;
}

void SpriteTextManager::clearSprite(){
    if (sprite != nullptr) {
        sprite->fillScreen(TFT_BLACK);
    }
    if (sprite_rotate != nullptr) {
        sprite_rotate->fillScreen(TFT_BLACK);
    }
}

void SpriteTextManager::rotate90AndFlip180(){
    uint16_t w = sprite->width();
    uint16_t h = sprite->height();
    //Serial.println(w);
    //Serial.println(h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int rotX = y;
            int rotY = w - 1 - x;
            int finalX = h - 1 - rotX;
            int finalY = w - 1 - rotY;
            // Serial.print(finalX);
            // Serial.print(",");
            // Serial.println(finalY);
            if (finalX >= 0 && finalX < h && finalY >= 0 && finalY < w) {
                bool pixel = sprite->readPixel(x, y);
                sprite_rotate->drawPixel(finalX, finalY, pixel);
            }else{
                Serial.print("Error!");
            }

        }
    }

            // for (int x = 0; x < SPRITE_WIDTH; x++)
            // {
            //     Serial.println("");
            //     for(int y = 0; y < SPRITE_HEIGHT; y++){
                    
            //         uint16_t color = sprite_rotate->readPixel(x,y);
            //         //draw2LCD(x,y,color);
            //         if(color != 0){
            //             Serial.print("*");
            //         }else{
            //             Serial.print(" ");
            //         }
            //     }
            
            // }
    //Serial.println("end--->");
    //sprite->deleteSprite();
    sprite = sprite_rotate;
}