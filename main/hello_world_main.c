#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <led_strip.h>
led_strip_handle_t led_strip;
#include "esp_timer.h"
static const char *TAG = "MAIN";
void LED(void *arg);
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "vector_font.h"
extern bool ref_map;
#define EX_UART_NUM UART_NUM_1
#define BAUD_RATE 9600
#define TX_PIN GPIO_NUM_38
#define RX_PIN GPIO_NUM_40
#define RE_DE_PIN GPIO_NUM_39 // 控制发送/接收的引脚

void uart_init()
{

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << (gpio_num_t)RE_DE_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_config(&io_conf);
    const uart_config_t uart_config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // 安装 UART 驱动
    uart_driver_install(EX_UART_NUM, 1024 * 2, 0, 0, NULL, 0);
    uart_param_config(EX_UART_NUM, &uart_config);
    uart_set_pin(EX_UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // 设置 RE/DE 控制引脚
    // gpio_set_direction(RE_DE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(RE_DE_PIN, 0);
}

// 发送数据
void send_data(const char *data)
{
    gpio_set_level(RE_DE_PIN, 1); // 设置为发送模式
    uart_write_bytes(EX_UART_NUM, data, strlen(data));
    vTaskDelay(20 / portTICK_PERIOD_MS); // 等待发送完成
    gpio_set_level(RE_DE_PIN, 0);        // 切换回接收模式
}
void RX485_task(void *param)
{
    uint8_t buffer[128];
    int length;

    while (1)
    {
        length = uart_read_bytes(EX_UART_NUM, buffer, sizeof(buffer), 20 / portTICK_PERIOD_MS);
        if (length > 0)
        {
            buffer[length] = '\0';
            ESP_LOGI("UART", "Received: %s", buffer);
        }
        send_data("Hello from ESP32\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}
#include "freertos/semphr.h"

SemaphoreHandle_t font_render_semaphore = NULL;
const uint16_t unicode_list[] = {0x89C0, 0x89C1, 0x89C2, 0x89C3, 0x89C4, 0x89C5, 0x89C6, 0x89C7, 0x89C8, 0x89C9, 0x89CA, 0x89CB, 0x89CC, 0x89CD, 0x89CE, 0x89CF, 0x89D0, 0x89D1, 0x89D2, 0x89D3, 0x89D4};
void sadsadad(void *param)
{
    vector_font_init();
    FT_ULong char_code = unicode_list[0];
    vector_font_render_code(char_code, 1);

    char_code = unicode_list[1];
    vector_font_render_code(char_code, 0);
    uint16_t a = 2;
    while (1)
    {
        if (xSemaphoreTake(font_render_semaphore, portMAX_DELAY)) // 等待信号量
        {
            char_code = unicode_list[a];
            a++;
            if (a >= sizeof(unicode_list) / sizeof(unicode_list[0]))
                a = 0;
            ref_map = !ref_map;
            vector_font_render_code(char_code, ref_map);
        }
    }
}

void display(void *param)
{
    init_led();
    SetBrightness(200);
    led_set(0);
    led1_set(0);
    init_display();
}
TaskHandle_t vector_code_task_handle = NULL;
void app_main(void)
{
    printf("Hello world!\n");
    // 创建信号量
    font_render_semaphore = xSemaphoreCreateBinary();
    if (font_render_semaphore == NULL)
    {
        ESP_LOGE(TAG, "无法创建信号量");
        return;
    }
    xTaskCreatePinnedToCore(sadsadad, "aaaa", 1024 * 8, &vector_code_task_handle, 20, NULL, 0);
    xTaskCreatePinnedToCore(display, "display", 1024 * 8, NULL, 21, NULL, 1);

    // xTaskCreatePinnedToCore(LED, "LED", 1024 * 2, NULL, 6, NULL, 0);
    // uart_init();

    // xTaskCreate(RX485_task, "RX485_task", 2048, NULL, 10, NULL);

    //

    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_PIN,
        .max_leds = RGB_MAX_NUMS,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .flags = {
            .with_dma = false,
        }};

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    for (int j = 0; j < RGB_MAX_NUMS; j++)
    {

        ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, j, 0x88, 0X00, 0x00));

        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
    }
}
#include "esp_random.h"
#define BRIGHTNESS_MIN 100
#define BRIGHTNESS_MAX 255
#define BRIGHTNESS_STEP 20
//// 随机亮度渐变 模拟火焰闪烁
void LED(void *arg)
{
    uint8_t current_brightness = BRIGHTNESS_MIN;
    uint8_t target_brightness;

    while (1)
    {
        // 生成新的目标亮度
        target_brightness = esp_random() % (BRIGHTNESS_MAX - BRIGHTNESS_MIN + 1) + BRIGHTNESS_MIN;

        // 平滑调整亮度
        if (current_brightness < target_brightness)
        {
            current_brightness = (current_brightness + BRIGHTNESS_STEP > target_brightness)
                                     ? target_brightness
                                     : current_brightness + BRIGHTNESS_STEP;
        }
        else if (current_brightness > target_brightness)
        {
            current_brightness = (current_brightness - BRIGHTNESS_STEP < target_brightness)
                                     ? target_brightness
                                     : current_brightness - BRIGHTNESS_STEP;
        }

        led_set(current_brightness);
        vTaskDelay(100 / portTICK_PERIOD_MS); // 控制更新频率
    }
}
