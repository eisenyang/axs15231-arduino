#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd.h"
#include "hkjgxjh160.h"
SpriteTextManager spriteTextManager;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }
    spriteTextManager.init();
    spriteTextManager.fillScreen(TFT_BLACK);
    spriteTextManager.loadFont(hkjgxjh160);
    
    spriteTextManager.setScrollWindow(0, LCD_HEIGHT,  0);
}

void loop()
{

    static uint16_t address = LCD_HEIGHT;
    static bool is_draw = false;
    static uint16_t y = 0;
    static String draw_string[] = {"万","事","如","意","、","阖","家","幸","福","！"};
    //static String draw_string[] = {"阖"};
    static uint16_t draw_string_len = sizeof(draw_string)/sizeof(draw_string[0]);
    static uint16_t draw_string_index = 0;
    if (address <= 0)
    {
        address = LCD_HEIGHT;
    }

    //is_draw = false;
    

    if(false){
        for (int x = 0; x < SPRITE_WIDTH; x++)
        {
            Serial.println("");
            for(int y = 0; y < SPRITE_HEIGHT; y++){
                spriteTextManager.drawCenterString(draw_string[1]);
                uint16_t color = spriteTextManager.getPixel(x,y);
                spriteTextManager.draw2LCD(x,y,color);
                if(color != 0){
                    Serial.print("*");
                }else{
                    Serial.print(" ");
                }
            }
        
        }
    }
    
    for (int x = 0; x < SPRITE_WIDTH  && draw_string_index < draw_string_len; x++)
    { 

        if(y < SPRITE_HEIGHT){
            if(is_draw == false){
                spriteTextManager.drawCenterString(draw_string[draw_string_index]);
                //spriteTextManager.rotate90AndFlip180();
                is_draw = true;
            }
            uint16_t color = spriteTextManager.getPixel(x,y);
            spriteTextManager.draw2LCD(x,address+1,color);
        }else if(y >= SPRITE_HEIGHT){
            draw_string_index++;
            is_draw = false;
            y=0;
            spriteTextManager.clearSprite();
        }
    }
    if(draw_string_index >= draw_string_len){
        draw_string_index = 0;
        is_draw = false;
        y=0;
        spriteTextManager.clearSprite();
    }
    spriteTextManager.scrollStart(address);
    y++;
    address--;
    delay(2);
    
}