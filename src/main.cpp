#include <Arduino.h>
#include "lcd.h"

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }

    lcd_init();
    lcd_test_draw_frame(0, 0, LCD_WIDTH, LCD_HEIGHT, GREEN, BLACK);

    lcd_set_scroll_window(0, LCD_HEIGHT, 0);
}

void loop()
{
    static uint16_t address = LCD_HEIGHT;
    lcd_test_draw_top_marker(85, RED, BLUE);

    lcd_scroll_start(address);
    address--;

    if (address <= 0)
    {
        address = LCD_HEIGHT;
    }

    delay(5);
}