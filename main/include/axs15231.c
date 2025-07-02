#include "config.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_timer.h"
static spi_device_handle_t spi;
static spi_transaction_t trans;
typedef struct
{
    int cmd;               /*<! The specific LCD command */
    const void *data;      /*<! Buffer that holds the command specific data */
    size_t data_bytes;     /*<! Size of `data` in memory, in bytes */
    unsigned int delay_ms; /*<! Delay in milliseconds after this command */
} axs15231_lcd_init_cmd_t;
static const axs15231_lcd_init_cmd_t vendor_specific_init[] = {
    {0xBB, (uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A, 0xA5}, 8, 0},
    {0xCA, (uint8_t[]){0x21, 0x36, 0x00, 0x22}, 4, 0},
    {0xA0, (uint8_t[]){0x00, 0x30, 0x00, 0x02, 0x00, 0x00, 0x04, 0x3F, 0x20, 0x05, 0x3F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00}, 17, 0},
    {0xA2, (uint8_t[]){0x30, 0x19, 0x60, 0x64, 0x9B, 0x22, 0x50, 0x80, 0xAC, 0x28, 0x7F, 0x7F, 0x7F, 0x20, 0xF8, 0x10, 0x02, 0xFF, 0xFF, 0xF0, 0x90, 0x01, 0x32, 0xA0, 0x91, 0xC0, 0x20, 0x7F, 0xFF, 0x00, 0x04}, 31, 0},
    {0xD0, (uint8_t[]){0x80, 0xAC, 0x21, 0x24, 0x08, 0x09, 0x10, 0x01, 0x80, 0x12, 0xC2, 0x00, 0x22, 0x22, 0xAA, 0x03, 0x10, 0x12, 0x40, 0x14, 0x1E, 0x51, 0x15, 0x00, 0x40, 0x10, 0x00, 0x03, 0x7D, 0x12}, 30, 0},
    {0xA3, (uint8_t[]){0xA0, 0x06, 0xA9, 0x00, 0x08, 0x02, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x55, 0x55}, 21, 0},
    {0xC1, (uint8_t[]){0x33, 0x04, 0x02, 0x02, 0x71, 0x05, 0x24, 0x55, 0x02, 0x00, 0x41, 0x01, 0x53, 0xFF, 0xFF, 0xFF, 0x4F, 0x52, 0x00, 0x4F, 0x52, 0x00, 0x45, 0x3B, 0x0B, 0x02, 0x0D, 0x00, 0xFF, 0x40}, 30, 0},
    {0xC3, (uint8_t[]){0x00, 0x00, 0x00, 0x50, 0x0b, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01}, 11, 0},
    {0xC4, (uint8_t[]){0x00, 0x24, 0x33, 0x80, 0x11, 0xEA, 0x64, 0x32, 0xC8, 0x64, 0xC8, 0x32, 0x90, 0x90, 0x11, 0x06, 0xDC, 0xFA, 0x00, 0x00, 0x80, 0xFE, 0x10, 0x10, 0x00, 0x0A, 0x0A, 0x44, 0x50}, 29, 0},
    {0xC5, (uint8_t[]){0x18, 0x00, 0x00, 0x03, 0xFE, 0x08, 0x68, 0x30, 0x00, 0x98, 0x89, 0xDE, 0x0D, 0x08, 0x0F, 0x0F, 0x01, 0x08, 0x68, 0x30, 0x10, 0x10, 0x08}, 23, 0},
    {0xC6, (uint8_t[]){0x05, 0x0A, 0x05, 0x0A, 0x00, 0xE0, 0x2E, 0x0B, 0x12, 0x22, 0x12, 0x22, 0x01, 0x00, 0x00, 0x02, 0x6A, 0x18, 0xC8, 0x22}, 20, 0},
    {0xC7, (uint8_t[]){0x50, 0x36, 0x28, 0x00, 0xA2, 0x80, 0x8F, 0x00, 0x80, 0xFF, 0x07, 0x11, 0x9C, 0x6F, 0xFF, 0x24, 0x0C, 0x0D, 0x0E, 0x0F}, 20, 0},
    {0xC9, (uint8_t[]){0x33, 0x44, 0x44, 0x01}, 4, 0},
    {0xCF, (uint8_t[]){0x2C, 0x1E, 0x88, 0x58, 0x13, 0x18, 0x56, 0x18, 0x1E, 0x68, 0xF8, 0x00, 0x66, 0x0D, 0x22, 0xC4, 0x0C, 0x77, 0x22, 0x44, 0xAA, 0x55, 0x04, 0x04, 0x12, 0xA0, 0x08}, 27, 0},
    {0xD5, (uint8_t[]){0x50, 0x60, 0x8A, 0x00, 0x35, 0x04, 0x71, 0x02, 0x03, 0x03, 0x03, 0x00, 0x04, 0x02, 0x13, 0x46, 0x03, 0x03, 0x03, 0x03, 0x86, 0x00, 0x00, 0x00, 0x80, 0x52, 0x7C, 0x00, 0x00, 0x00}, 30, 0},
    {0xD6, (uint8_t[]){0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE, 0x00, 0x00, 0x01, 0x83, 0x03, 0x03, 0x33, 0x03, 0x03, 0x33, 0x3F, 0x03, 0x03, 0x03, 0x20, 0x20, 0x00, 0x24, 0x51, 0x23, 0x01, 0x00}, 30, 0},
    {0xD7, (uint8_t[]){0x18, 0x1A, 0x1B, 0x1F, 0x0A, 0x08, 0x0E, 0x0C, 0x00, 0x1F, 0x1D, 0x1F, 0x50, 0x60, 0x04, 0x00, 0x1F, 0x1F, 0x1F}, 19, 0},
    {0xD8, (uint8_t[]){0x18, 0x1A, 0x1B, 0x1F, 0x0B, 0x09, 0x0F, 0x0D, 0x01, 0x1F, 0x1D, 0x1F}, 12, 0},
    {0xD9, (uint8_t[]){0x0F, 0x09, 0x0B, 0x1F, 0x18, 0x19, 0x1F, 0x01, 0x1E, 0x1D, 0x1F}, 11, 0},
    {0xDD, (uint8_t[]){0x0E, 0x08, 0x0A, 0x1F, 0x18, 0x19, 0x1F, 0x00, 0x1E, 0x1A, 0x1F}, 11, 0},
    {0xDF, (uint8_t[]){0x44, 0x33, 0x4B, 0x69, 0x00, 0x0A, 0x90}, 7, 0},
    {0xE0, (uint8_t[]){0x35, 0x08, 0x19, 0x1C, 0x0C, 0x09, 0x13, 0x2A, 0x54, 0x21, 0x0B, 0x15, 0x13, 0x25, 0x27, 0x08, 0x00}, 17, 0},
    {0xE1, (uint8_t[]){0x3E, 0x08, 0x19, 0x1C, 0x0C, 0x08, 0x13, 0x2A, 0x54, 0x21, 0x0B, 0x14, 0x13, 0x26, 0x27, 0x08, 0x0F}, 17, 0},
    {0xE2, (uint8_t[]){0x19, 0x20, 0x0A, 0x11, 0x09, 0x06, 0x11, 0x25, 0xD4, 0x22, 0x0B, 0x13, 0x12, 0x2D, 0x32, 0x2F, 0x03}, 17, 0},
    {0xE3, (uint8_t[]){0x38, 0x20, 0x0A, 0x11, 0x09, 0x06, 0x11, 0x25, 0xC4, 0x21, 0x0A, 0x12, 0x11, 0x2C, 0x32, 0x2F, 0x27}, 17, 0},
    {0xE4, (uint8_t[]){0x19, 0x20, 0x0D, 0x14, 0x0D, 0x08, 0x12, 0x2A, 0xD4, 0x26, 0x0E, 0x15, 0x13, 0x34, 0x39, 0x2F, 0x03}, 17, 0},
    {0xE5, (uint8_t[]){0x38, 0x20, 0x0D, 0x13, 0x0D, 0x07, 0x12, 0x29, 0xC4, 0x25, 0x0D, 0x15, 0x12, 0x33, 0x39, 0x2F, 0x27}, 17, 0},
    {0xBB, (uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 8, 0},
    {0x35, (uint8_t[]){0x01}, 1, 200},
    {0x11, (uint8_t[]){0x00}, 1, 200},
    {0x29, (uint8_t[]){0x00}, 1, 100},
    {0x33, (uint8_t[]){0x00, 0X00, 0X02, 0X80, 0X00, 0X00}, 6, 100}};

void lcd_spi_write_cmd(uint8_t cmd)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;       // Command is 8 bits
    t.tx_buffer = &cmd; // The data is the cmd itself
    t.user = (void *)0; // D/C needs to be set to 0
    gpio_set_level(LCD_SPI_CS, 0);
    gpio_set_level(LCD_SPI_DC, 0);
    spi_device_polling_transmit(spi, &t);
    gpio_set_level(LCD_SPI_CS, 1);
}

void lcd_spi_write_datas(const uint8_t *data, size_t size)
{
    if (!data || size == 0)
        return;
    spi_transaction_t t = {
        .length = size * 8,
        .tx_buffer = data,
        .user = (void *)1,
    };
    // spi_transaction_t t;
    // memset(&t, 0, sizeof(t));
    // t.length = 8;        // Data is 8 bits
    // t.tx_buffer = &data; // Data
    // t.user = (void *)1;  // D/C needs to be set to 1
    gpio_set_level(LCD_SPI_CS, 0);
    gpio_set_level(LCD_SPI_DC, 1);
    spi_device_polling_transmit(spi, &t);
    gpio_set_level(LCD_SPI_CS, 1);
}
void lcd_spi_write_data(uint8_t data)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;        // Data is 8 bits
    t.tx_buffer = &data; // Data
    t.user = (void *)1;  // D/C needs to be set to 1
    gpio_set_level(LCD_SPI_CS, 0);
    gpio_set_level(LCD_SPI_DC, 1);
    spi_device_polling_transmit(spi, &t);
    gpio_set_level(LCD_SPI_CS, 1);
}
static void IRAM_ATTR lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    if (t && t->user)
    {
        gpio_set_level((gpio_num_t)LCD_SPI_DC, (int)t->user);
    }
}
static volatile int64_t last_te_time = 0;
static volatile bool te_signal_received = false;
static const int64_t TE_TIMEOUT_US = 100000;
void IRAM_ATTR et_isr(void *arg)
{
    ESP_LOGI("TE", "TE signal received");
    int64_t current_time = esp_timer_get_time();
    if (current_time - last_te_time >= TE_TIMEOUT_US)
    {
        te_signal_received = true;
        last_te_time = current_time;
        ESP_LOGI("TE", "TE %lld us", current_time - last_te_time);
        // Call user callback if registered
    }
}
// bool lcd_spi_wait_for_te(uint32_t timeout_ms)
// {
//     uint32_t start = xTaskGetTickCount();
//     while (!te_signal_received)
//     {
//         if ((xTaskGetTickCount() - start) >= pdMS_TO_TICKS(timeout_ms))
//         {
//             return false;
//         }
//         vTaskDelay(1);
//     }
//     te_signal_received = false;
//     return true;
// }

void axs15231_init()
{
    esp_err_t ret;
    // Initialize non-SPI GPIOs
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << (gpio_num_t)LCD_SPI_DC) | (1ULL << (gpio_num_t)LCD_SPI_RST) | (1ULL << (gpio_num_t)LCD_SPI_CS);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ret = gpio_config(&io_conf);

    // // Configure TE pin as input with pull-up
    // gpio_config_t te_conf = {
    //     .pin_bit_mask = (1ULL << (gpio_num_t)LCD_PIN_TE),
    //     .mode = GPIO_MODE_INPUT,
    //     .intr_type = GPIO_INTR_POSEDGE,
    // };

    // ret = gpio_config(&te_conf);
    // if (ret != ESP_OK)
    // {
    //     printf("TE pin config failed\n");
    //     return;
    // }

    // // Install GPIO ISR service
    // ret = gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
    // if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    // {
    //     printf("GPIO install isr service failed\n");
    //     return;
    // }

    // // Add ISR handler for TE pin
    // ret = gpio_isr_handler_add(LCD_PIN_TE, et_isr, NULL);
    // if (ret != ESP_OK)
    // {
    //     printf("GPIO isr handler add failed\n");
    //     return;
    // }

    // Initialize SPI bus
    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.mosi_io_num = LCD_SPI_MOSI;
    buscfg.miso_io_num = -1; // MISO not used in half-duplex mode
    buscfg.sclk_io_num = LCD_SPI_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = LCD_Widht * 2;
    buscfg.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI;
    buscfg.intr_flags = 0; // Disable interrupt flags

    ret = spi_bus_initialize(LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);

    // Attach LCD to SPI bus
    spi_device_interface_config_t devcfg;
    memset(&devcfg, 0, sizeof(devcfg));
    devcfg.command_bits = 0;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.clock_speed_hz = 80 * 1000 * 1000;
    devcfg.duty_cycle_pos = 128; // 50% duty cycle
    devcfg.mode = 0;             // SPI mode 0
    devcfg.spics_io_num = -1;    // CS pin handled manually
    devcfg.queue_size = 1;       // 非事务模式，队列大小为1
    devcfg.pre_cb = lcd_spi_pre_transfer_callback;
    devcfg.input_delay_ns = 0;
    devcfg.cs_ena_pretrans = 0;
    devcfg.cs_ena_posttrans = 0;
    devcfg.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY;

    ret = spi_bus_add_device(LCD_SPI_HOST, &devcfg, &spi);

    gpio_set_level(LCD_SPI_CS, 1);
    gpio_set_level(LCD_SPI_DC, 1);
    gpio_set_level(LCD_SPI_RST, 1);
    vTaskDelay(200);
    gpio_set_level(LCD_SPI_RST, 0);
    vTaskDelay(200);
    gpio_set_level(LCD_SPI_RST, 1);
    vTaskDelay(100);

    uint8_t init_cmds_size = sizeof(vendor_specific_init) / sizeof(axs15231_lcd_init_cmd_t);
    for (uint8_t i = 0; i < init_cmds_size; i++)
    {
        lcd_spi_write_cmd(vendor_specific_init[i].cmd);
        for (int j = 0; j < vendor_specific_init[i].data_bytes; j++)
        {
            lcd_spi_write_data(((uint8_t *)vendor_specific_init[i].data)[j]);
        }
        // lcd_spi_write_datas(vendor_specific_init[i].data, vendor_specific_init[i].data_bytes);
        vTaskDelay(vendor_specific_init[i].delay_ms); // 200ms
    }
}
void lcd_spi_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_spi_write_cmd(0x2a);
    lcd_spi_write_data(x1 >> 8);
    lcd_spi_write_data(x1 & 0xff);
    lcd_spi_write_data(x2 >> 8);
    lcd_spi_write_data(x2 & 0xff);

    lcd_spi_write_cmd(0x2b);
    lcd_spi_write_data(y1 >> 8);
    lcd_spi_write_data(y1 & 0xff);
    lcd_spi_write_data(y2 >> 8);
    lcd_spi_write_data(y2 & 0xff);

    lcd_spi_write_cmd(0x2c);
}

void lcd_spi_write_line_colors(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *colors, size_t len)
{
    lcd_spi_write_cmd(0x2a);
    lcd_spi_write_data(x1 >> 8);
    lcd_spi_write_data(x1 & 0xff);
    lcd_spi_write_data(x2 >> 8);
    lcd_spi_write_data(x2 & 0xff);

    lcd_spi_write_cmd(0x2b);
    lcd_spi_write_data(y1 >> 8);
    lcd_spi_write_data(y1 & 0xff);
    lcd_spi_write_data(y2 >> 8);
    lcd_spi_write_data(y2 & 0xff);

    lcd_spi_write_cmd(0x2c);
    lcd_spi_write_colors(colors, len);
}

void lcd_spi_write_colors(const uint16_t *colors, size_t len)
{
    gpio_set_level(LCD_SPI_CS, 0);
    gpio_set_level(LCD_SPI_DC, 1);

    static uint8_t data[SPI_MAX_DMA_LEN]; // 使用DMA最大长度
    size_t remaining = len;
    size_t offset = 0;

    while (remaining > 0)
    {
        // 计算本次传输的像素数量
        size_t batch_pixels = (remaining > (SPI_MAX_DMA_LEN / 2)) ? (SPI_MAX_DMA_LEN / 2) : remaining;
        size_t batch_bytes = batch_pixels * 2;

        // 转换颜色格式
        for (size_t i = 0; i < batch_pixels; i++)
        {
            data[i * 2] = (uint8_t)(colors[offset + i] >> 8);
            data[i * 2 + 1] = (uint8_t)(colors[offset + i] & 0xFF);
        }

        // 配置传输
        trans.length = batch_bytes * 8; // bits
        trans.tx_buffer = data;

        // 执行传输
        esp_err_t ret = spi_device_polling_transmit(spi, &trans);
        remaining -= batch_pixels;
        offset += batch_pixels;
    }

    gpio_set_level(LCD_SPI_CS, 1);
}
void lcd_spi_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed)
{
    lcd_spi_write_cmd(0x33);
    lcd_spi_write_data(top_fixed >> 8);        // 0
    lcd_spi_write_data(top_fixed & 0xff);      // 1   前2位是顶部不动的区域
    lcd_spi_write_data(scroll_content >> 8);   // 2
    lcd_spi_write_data(scroll_content & 0xff); // 3   中间2位是滚动的内容
    lcd_spi_write_data(bottom_fixed >> 8);     // 4
    lcd_spi_write_data(bottom_fixed & 0xff);   // 5   后两个是底部不动的区域
}
void lcd_spi_scroll_start(uint16_t line_num)
{
    lcd_spi_write_cmd(0x37);
    uint8_t data[2];
    data[0] = (line_num >> 8) & 0xFF; // MSB
    data[1] = line_num & 0xFF;        // LSB
    lcd_spi_write_datas(data, 2);
    // uint8_t data[2];
    // data[0] = (line_num >> 8) & 0xFF; // MSB First
    // data[1] = line_num & 0xFF;        // LSB
    // spi_transaction_t t = {
    //     .length = 16,
    //     .tx_buffer = data,
    //     .user = (void *)1,
    // };
    // gpio_set_level(LCD_SPI_CS, 0);
    // gpio_set_level(LCD_SPI_DC, 1);
    // spi_device_polling_transmit(spi, &t);
    // gpio_set_level(LCD_SPI_CS, 1);
}

uint16_t rgb888_to_rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    // 将每个颜色分量从 8 位缩减到指定的位数
    uint16_t red = (r & 0xF8) << 8;   // 红色：保留高5位，并移位到正确的位置
    uint16_t green = (g & 0xFC) << 3; // 绿色：保留高6位，并移位到正确的位置
    uint16_t blue = (b & 0xF8) >> 3;  // 蓝色：保留高5位，并移位到正确的位置

    return (red | green | blue); // 合并三个颜色分量
}
/**
 * @brief 将 RGB888 颜色（如 0xRRGGBB）转换为 RGB565 格式
 *
 * @param color_888 24 位 RGB 颜色值（例如：0x00ff00 表示绿色）
 * @return uint16_t 返回 16 位 RGB565 格式的颜色值
 */
uint16_t rgb_to_rgb565(uint32_t color_888)
{
    uint8_t r = (color_888 >> 16) & 0xFF; // 提取红色分量
    uint8_t g = (color_888 >> 8) & 0xFF;  // 提取绿色分量
    uint8_t b = color_888 & 0xFF;         // 提取蓝色分量

    // 转换为 RGB565 格式
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    uint8_t region, remainder, p, q, t;

    if (s == 0)
    {
        // Achromatic color (gray)
        *r = *g = *b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
    case 0:
        *r = v;
        *g = t;
        *b = p;
        break;
    case 1:
        *r = q;
        *g = v;
        *b = p;
        break;
    case 2:
        *r = p;
        *g = v;
        *b = t;
        break;
    case 3:
        *r = p;
        *g = q;
        *b = v;
        break;
    case 4:
        *r = t;
        *g = p;
        *b = v;
        break;
    default:
        *r = v;
        *g = p;
        *b = q;
        break;
    }
}
uint16_t hsv_to_rgb565(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t r, g, b;
    hsv_to_rgb(h, s, v, &r, &g, &b);
    return rgb888_to_rgb565(r, g, b);
}