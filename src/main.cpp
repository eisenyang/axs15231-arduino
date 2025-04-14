#include <Arduino.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include "SpriteTextManager.h"
#include "lcd.h"
#include "simkai160.h"
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
    spriteTextManager.loadFont(simkai160);
    spriteTextManager.blockLcdWrite(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
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
                spriteTextManager.drawPixel2LCD(x,y,color);
                if(color != 0){
                    Serial.print("*");
                }else{
                    Serial.print(" ");
                }
            }
        
        }
    }
    
    unsigned long startTimeTotal = micros();
    for (int x = 0; x < SPRITE_WIDTH  && draw_string_index < draw_string_len; x++)
    { 

        if(y < SPRITE_HEIGHT){
            if(is_draw == false){
                spriteTextManager.drawCenterString(draw_string[draw_string_index]);
                is_draw = true;
            }
            
            // Start timing
            unsigned long startTime = micros();
            uint16_t color = spriteTextManager.getPixel(y,x);
            unsigned long getPixelTime = micros() - startTime;
            
            // Measure draw2LCD time
            startTime = micros();
            //spriteTextManager.drawPixel2LCD(x+20,address+1,color);
            if(x == 0){
                //spriteTextManager.blockRowWrite(address+1);
            }
            spriteTextManager.drawPixel2LCD(x+20,address+1,color);
            unsigned long drawTime = micros() - startTime;
            
            //Print timing information every 100 iterations
            static int counter = 0;
            if (++counter % 100 == 0) {
                Serial.print("One getPixel Function Run time: ");
                Serial.print(getPixelTime);
                Serial.print("µs, One draw2LCD Function Run time: ");
                Serial.print(drawTime);
                Serial.println("µs");
            }


        }else if(y >= SPRITE_HEIGHT){
            draw_string_index++;
            is_draw = false;
            y=0;
            spriteTextManager.clearSprite();
        }
    }
    unsigned long endTimeTotal = micros();
    Serial.print("One For Run Total time: ");
    Serial.print(endTimeTotal - startTimeTotal);
    Serial.println("µs");
    if(draw_string_index >= draw_string_len){
        draw_string_index = 0;
        is_draw = false;
        y=0;
        spriteTextManager.clearSprite();
    }
    spriteTextManager.scrollStart(address);
    y++;
    address--;

    unsigned long endTime = micros();
    Serial.print("One Loop Run Total time: ");
    Serial.print(endTime - startTimeTotal);
    Serial.println("µs");
    delay(10);
    
}