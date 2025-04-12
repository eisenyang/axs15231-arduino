#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd.h"
#include "simkai28.h"
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
    spriteTextManager.loadFont(simkai28);
    spriteTextManager.drawString("福");
    spriteTextManager.setScrollWindow(0, LCD_HEIGHT,  0);
}

void loop()
{

    static uint16_t address = LCD_HEIGHT;
    static bool is_draw = true;
    static uint16_t y = 0;
    if (address <= 0)
    {
        address = LCD_HEIGHT;
    }

    is_draw = false;
    

    if(false){
        for (int x = 0; x < SPRITE_WIDTH; x++)
        {
            Serial.println("");
            for(int y = 0; y < SPRITE_HEIGHT; y++){
                uint16_t color = spriteTextManager.getPixel(x,y);
                spriteTextManager.draw2LCD(x,y,color);
                if(color != 0){
                    //Serial.print("*");
                }else{
                // Serial.print(" ");
                }
            }
        
        }
    }
    
    for (int x = 0; x < SPRITE_WIDTH; x++)
    { 

        if(y < SPRITE_HEIGHT){
            uint16_t color = spriteTextManager.getPixel(x,y);
            spriteTextManager.draw2LCD(x,address,color);
        }
    }
    spriteTextManager.scrollStart(address+1);
    y++;
    address--;
    delay(10);
    
}