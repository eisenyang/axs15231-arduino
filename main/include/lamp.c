#include "config.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/timer.h"
#include <esp_timer.h>
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include <unistd.h>
#include "esp_log.h"

void init_led()
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_HS_MODE,          // Timer mode
        .duty_resolution = LEDC_TIMER_8_BIT, // Resolution of PWM duty
        .timer_num = LEDC_HS_TIMER,          // Timer index
        .freq_hz = 3000,                     // Frequency of PWM signal
        .clk_cfg = LEDC_AUTO_CLK,            // Auto select the source clock
    };

    // 配置定时器
    ledc_timer_config(&ledc_timer);

    // W LED 配置
    ledc_channel_config_t W_led_config = {
        .gpio_num = LED_PIN,
        .speed_mode = LEDC_HS_MODE,
        .channel = LEDC_HS_CH0_CHANNEL,
        .timer_sel = LEDC_HS_TIMER,
        .duty = 0,
        .hpoint = 0,
        .flags = {.output_invert = 0}};

    // W LED 配置
    ledc_channel_config_t led2_config = {
        .gpio_num = LED1_PIN,
        .speed_mode = LEDC_HS_MODE,
        .channel = LEDC_HS_CH1_CHANNEL,
        .timer_sel = LEDC_HS_TIMER,
        .duty = 0,
        .hpoint = 0,
        .flags = {.output_invert = 0}};

    // Y LED 配置
    ledc_channel_config_t Y_led_config = {
        .gpio_num = LCD_SPI_BL,
        .speed_mode = LEDC_HS_MODE,
        .channel = LEDC_HS_CH2_CHANNEL,
        .timer_sel = LEDC_HS_TIMER,
        .duty = 0,
        .hpoint = 0,
        .flags = {.output_invert = 0}};

    // 配置通道
    ledc_channel_config(&W_led_config);
    ledc_channel_config(&Y_led_config);
    ledc_channel_config(&led2_config);
}

void led_set(uint32_t val)
{
    ledc_set_duty(LEDC_HS_MODE, LEDC_HS_CH0_CHANNEL, val);
    ledc_update_duty(LEDC_HS_MODE, LEDC_HS_CH0_CHANNEL);
}
void led1_set(uint32_t val)
{
    ledc_set_duty(LEDC_HS_MODE, LEDC_HS_CH1_CHANNEL, val);
    ledc_update_duty(LEDC_HS_MODE, LEDC_HS_CH1_CHANNEL);
}

void SetBrightness(uint32_t val)
{
    ledc_set_duty(LEDC_HS_MODE, LEDC_HS_CH2_CHANNEL, val);
    ledc_update_duty(LEDC_HS_MODE, LEDC_HS_CH2_CHANNEL);
}