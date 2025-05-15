#include "Arduino.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"
#include "esp_attr.h"
#include "lcd_idf_spi.h"
#include "esp_log.h"
#include <string.h>
#include "esp_timer.h"

#ifdef CONFIG_FREERTOS_ENABLE_BACKWARD_COMPATIBILITY
#define pdMS_TO_TICKS(ms) ((ms) / portTICK_PERIOD_MS)
#endif

static const char *TAG = "LCD";

// GPIO control macros
#define LCD_DC_H gpio_set_level((gpio_num_t)PIN_NUM_DC, HIGH)
#define LCD_DC_L gpio_set_level((gpio_num_t)PIN_NUM_DC, LOW)
#define LCD_CS_H gpio_set_level((gpio_num_t)PIN_NUM_CS, HIGH)
#define LCD_CS_L gpio_set_level((gpio_num_t)PIN_NUM_CS, LOW)
#define LCD_RST_H gpio_set_level((gpio_num_t)PIN_NUM_RST, HIGH)
#define LCD_RST_L gpio_set_level((gpio_num_t)PIN_NUM_RST, LOW)

static spi_device_handle_t spi;
static spi_transaction_t trans;

// Add these global variables after the existing ones
static volatile bool te_signal_received = false;
static volatile int64_t last_te_time = 0;
static const int64_t TE_TIMEOUT_US = 100000; // 100ms timeout
static te_callback_t user_te_callback = NULL;

// This function is called (in irq context!) just before a transmission starts
static void IRAM_ATTR lcd_idf_spi_pre_transfer_callback(spi_transaction_t *t)
{
    if (t && t->user) {
        gpio_set_level((gpio_num_t)PIN_NUM_DC, (int)t->user);
    }
}

static void lcd_idf_spi_reset(void)
{
    LCD_RST_H;
    esp_rom_delay_us(20000);  // 20ms
    LCD_RST_L;
    esp_rom_delay_us(200000); // 200ms
    LCD_RST_H;
    esp_rom_delay_us(200000); // 200ms
}

void lcd_idf_spi_set_te_callback(te_callback_t callback) {
    user_te_callback = callback;
}

void IRAM_ATTR et_isr(void* arg) {

    if (user_te_callback) {
        user_te_callback(arg);
    }
    int64_t current_time = esp_timer_get_time();
    if (current_time - last_te_time >= TE_TIMEOUT_US) {
        te_signal_received = true;
        last_te_time = current_time;
        
        // Call user callback if registered
 
    }
}

bool lcd_idf_spi_wait_for_te(uint32_t timeout_ms) {
    uint32_t start = xTaskGetTickCount();
    while (!te_signal_received) {
        if ((xTaskGetTickCount() - start) >= pdMS_TO_TICKS(timeout_ms)) {
            return false;
        }
        vTaskDelay(1);
    }
    te_signal_received = false;
    return true;
}

esp_err_t lcd_idf_spi_init(void)
{
    esp_err_t ret;

    // Initialize non-SPI GPIOs
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << (gpio_num_t)PIN_NUM_DC) | (1ULL << (gpio_num_t)PIN_NUM_RST) | (1ULL << (gpio_num_t)PIN_NUM_CS);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        Serial.println("GPIO config failed");
        return ret;
    }
    Serial.println("GPIO config success");

    // Configure TE pin as input with pull-up
    gpio_config_t te_conf = {
        .pin_bit_mask = (1ULL << (gpio_num_t)PIN_NUM_TE),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    
    ret = gpio_config(&te_conf);
    if (ret != ESP_OK) {
        Serial.println("TE pin config failed");
        return ret;
    }
    Serial.println("TE pin config success");

    // Install GPIO ISR service
    ret = gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        Serial.println("GPIO install isr service failed");
        return ret;
    }
    Serial.println("GPIO ISR service installed");

    // Add ISR handler for TE pin
    ret = gpio_isr_handler_add(PIN_NUM_TE, et_isr, NULL);
    if (ret != ESP_OK) {
        Serial.println("GPIO isr handler add failed");
        return ret;
    }
    Serial.println("TE pin ISR handler added");

    // Initialize SPI bus
    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(buscfg));
    buscfg.mosi_io_num = PIN_NUM_MOSI;
    buscfg.miso_io_num = -1;  // MISO not used in half-duplex mode
    buscfg.sclk_io_num = PIN_NUM_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = LCD_WIDTH * LCD_HEIGHT * 2 + 8;
    buscfg.flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MOSI;
    buscfg.intr_flags = 0;    // Disable interrupt flags

    ret = spi_bus_initialize(LCD_HOST, &buscfg, DMA_CHAN);
    if (ret != ESP_OK) {
        Serial.println("SPI bus initialize failed");
        return ret;
    }
    Serial.println("SPI bus initialized");

    // Attach LCD to SPI bus
    spi_device_interface_config_t devcfg;
    memset(&devcfg, 0, sizeof(devcfg));
    devcfg.command_bits = 0;
    devcfg.address_bits = 0;
    devcfg.dummy_bits = 0;
    devcfg.clock_speed_hz = SPI_FREQUENCY;
    devcfg.duty_cycle_pos = 128;      // 50% duty cycle
    devcfg.mode = 0;                  // SPI mode 0
    devcfg.spics_io_num = -1;         // CS pin handled manually
    devcfg.queue_size = 7;
    devcfg.pre_cb = lcd_idf_spi_pre_transfer_callback;
    devcfg.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY;

    ret = spi_bus_add_device(LCD_HOST, &devcfg, &spi);
    if (ret != ESP_OK) {
        Serial.println("SPI device add failed");
        spi_bus_free(LCD_HOST);
        return ret;
    }
    Serial.println("SPI device added");

    // Set initial pin states
    LCD_CS_H;  // CS inactive
    LCD_DC_H;  // Data mode
    LCD_RST_H; // No reset

    // Reset LCD
    lcd_idf_spi_reset();
    Serial.println("LCD reset complete");

    // Initialize LCD commands
    lcd_idf_spi_init_cmds();
    Serial.println("LCD initialization commands sent");

    return ESP_OK;
}

void lcd_idf_spi_deinit(void)
{
    if (spi) {
        spi_bus_remove_device(spi);
        spi_bus_free(LCD_HOST);
        spi = NULL;
    }
}

void lcd_idf_spi_write_cmd(uint8_t cmd)
{
    if (!spi) {
        ESP_LOGE(TAG, "SPI device not initialized");
        return;
    }

    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;                     // Command is 8 bits
    t.tx_buffer = &cmd;               // The data is the cmd itself
    t.user = (void*)0;                // D/C needs to be set to 0
    
    LCD_CS_L;  // Select the LCD
    LCD_DC_L;
    ret = spi_device_polling_transmit(spi, &t);
    LCD_CS_H;  // Deselect the LCD
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write command 0x%02X: %d", cmd, ret);
    }
}

void lcd_idf_spi_write_data(uint8_t data)
{
    if (!spi) {
        ESP_LOGE(TAG, "SPI device not initialized");
        return;
    }

    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;                     // Data is 8 bits
    t.tx_buffer = &data;              // Data
    t.user = (void*)1;                // D/C needs to be set to 1
    
    LCD_CS_L;  // Select the LCD
    LCD_DC_H;
    ret = spi_device_polling_transmit(spi, &t);
    LCD_CS_H;  // Deselect the LCD
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write data 0x%02X: %d", data, ret);
    }
}




void lcd_idf_spi_cs_and_dc_start(void)
{
    LCD_CS_L;
    LCD_DC_H;
}

void lcd_idf_spi_cs_and_dc_end(void)
{
    LCD_CS_H;
}

void lcd_idf_spi_scroll_start(uint16_t data_16)
{
if (!spi) {
        ESP_LOGE(TAG, "SPI device not initialized");
        return;
    }
    esp_err_t ret;
    uint8_t data[2] = {(uint8_t)(data_16 >> 8), (uint8_t)(data_16 & 0xFF)};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = data;
    t.user = (void*)1;
    ret = spi_device_polling_transmit(spi, &t);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write data 0x%04X: %d", data_16, ret);
    }
}

void lcd_idf_spi_write_data_16(uint16_t data_16)
{
    if (!spi) {
        ESP_LOGE(TAG, "SPI device not initialized");
        return;
    }

    esp_err_t ret;
    uint8_t data[2] = {(uint8_t)(data_16 >> 8), (uint8_t)(data_16 & 0xFF)};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = data;
    t.user = (void*)1;

    LCD_CS_L; // Select the LCD
    LCD_DC_H; 
    ret = spi_device_polling_transmit(spi, &t);
    LCD_CS_H;  // Deselect the LCD

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write data 0x%04X: %d", data_16, ret);
    }










    // esp_err_t ret;
    // spi_transaction_t t1,t2;
    // uint8_t data1 = (uint8_t)(data_16 >> 8);
    // uint8_t data2 = (uint8_t)(data_16 & 0xFF);
    // memset(&t1, 0, sizeof(t1));
    // t1.length = 8;                     // Data is 8 bits
    // t1.tx_buffer = &data1;              // Data
    // t1.user = (void*)1;                // D/C needs to be set to 1
    
    // memset(&t2, 0, sizeof(t2));
    // t2.length = 8;                     // Data is 8 bits
    // t2.tx_buffer = &data2;              // Data
    // t2.user = (void*)1;                // D/C needs to be set to 1


    // LCD_CS_L;  // Select the LCD
    // LCD_DC_H;
    // ret = spi_device_polling_transmit(spi, &t1);
    // ret = spi_device_polling_transmit(spi, &t2);
    // LCD_CS_H;  // Deselect the LCD
}


void lcd_idf_spi_write_color(uint16_t color)
{
    if (!spi) {
        ESP_LOGE(TAG, "SPI device not initialized");
        return;
    }

    esp_err_t ret;
    uint8_t data[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 16;
    t.tx_buffer = data;
    t.user = (void*)1;

    LCD_CS_L; // Select the LCD
    LCD_DC_H; 
    ret = spi_device_polling_transmit(spi, &t);
    LCD_CS_H;  // Deselect the LCD

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write color 0x%04X: %d", color, ret);
    }
}
void lcd_idf_spi_read_data(uint8_t command, uint8_t *data, uint16_t length)
{
    if (data == NULL || length == 0) return;
    
    esp_err_t ret;
    spi_transaction_t t;

    // Send command
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &command;
    t.user = (void*)0;
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);

    // Read data
    memset(&t, 0, sizeof(t));
    t.length = length * 8;
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);

    // Copy data from t.rx_data to data buffer
    for (int i = 0; i < length && i < 4; i++) {
        data[i] = t.rx_data[i];
    }
}

uint8_t lcd_idf_spi_read_register(uint8_t reg)
{
    uint8_t data[4];
    lcd_idf_spi_read_data(reg, data, 1);
    return data[0];
}

void lcd_idf_spi_read_registers(uint8_t reg, uint8_t *data, uint16_t length)
{
    while (length > 0) {
        uint16_t chunk_size = (length > 4) ? 4 : length;
        lcd_idf_spi_read_data(reg, data, chunk_size);
        data += chunk_size;
        length -= chunk_size;
    }
}

void lcd_idf_spi_block_write(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (!spi) {
        ESP_LOGE(TAG, "SPI device not initialized");
        return;
    }

    // Set column address
    lcd_idf_spi_write_cmd(0x2a);
    lcd_idf_spi_write_data(x1 >> 8);
    lcd_idf_spi_write_data(x1 & 0xff);
    lcd_idf_spi_write_data(x2 >> 8);
    lcd_idf_spi_write_data(x2 & 0xff);

    // Set row address
    lcd_idf_spi_write_cmd(0x2b);
    lcd_idf_spi_write_data(y1 >> 8);
    lcd_idf_spi_write_data(y1 & 0xff);
    lcd_idf_spi_write_data(y2 >> 8);
    lcd_idf_spi_write_data(y2 & 0xff);

    // Wait for TE signal before starting memory write
    if (!lcd_idf_spi_wait_for_te(100)) {
        ESP_LOGW(TAG, "TE signal timeout, proceeding with write");
    }

    // Begin memory write
    lcd_idf_spi_write_cmd(0x2c);
}


void lcd_idf_spi_set_scroll_window(uint16_t top_fixed, uint16_t scroll_content, uint16_t bottom_fixed)
{
    lcd_idf_spi_write_cmd(0x33);
    lcd_idf_spi_write_data(top_fixed >> 8);
    lcd_idf_spi_write_data(top_fixed & 0xff);
    lcd_idf_spi_write_data(scroll_content >> 8);
    lcd_idf_spi_write_data(scroll_content & 0xff);
    lcd_idf_spi_write_data(bottom_fixed >> 8);
    lcd_idf_spi_write_data(bottom_fixed & 0xff);
}


void lcd_idf_spi_scroll_starts(uint16_t line_num[],uint16_t length)
{
    lcd_idf_spi_write_cmd(0x37);



    
    for (int i = 0; i < length; i++) {
        lcd_idf_spi_write_data(line_num[i] >> 8);
        lcd_idf_spi_write_data(line_num[i] & 0xff);
    }
}

void lcd_idf_spi_clear_screen(uint16_t color)
{
    lcd_idf_spi_block_write(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    for (int i = 0; i < LCD_HEIGHT; i++) {
        for (int j = 0; j < LCD_WIDTH; j++) {
            lcd_idf_spi_write_color(color);
        }
    }
}

void lcd_idf_spi_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    lcd_idf_spi_block_write(x, y, x, y);
    lcd_idf_spi_write_color(color);
}

void lcd_idf_spi_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        lcd_idf_spi_draw_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void lcd_idf_spi_draw_row(uint16_t y, uint16_t color)
{
    if (y >= LCD_HEIGHT) return;
    lcd_idf_spi_block_write(0, y, LCD_WIDTH - 1, y);
    for (int x = 0; x < LCD_WIDTH; x++) {
        lcd_idf_spi_write_color(color);
    }
}

void lcd_idf_spi_block_write_row(uint16_t y)
{
    if (y >= LCD_HEIGHT) return;
    lcd_idf_spi_block_write(0, y, LCD_WIDTH - 1, y);
}

void lcd_idf_spi_start_write_color(void)
{
    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.user = (void*)1;
    gpio_set_level((gpio_num_t)PIN_NUM_CS, 0);  // CS Low
    gpio_set_level((gpio_num_t)PIN_NUM_DC, 1);  // DC High
}

void lcd_idf_spi_end_write_color(void)
{
    gpio_set_level((gpio_num_t)PIN_NUM_CS, 1);  // CS High
}

void lcd_idf_spi_continue_write_color(uint16_t color)
{
    if (!spi) {
        ESP_LOGE(TAG, "SPI device not initialized");
        return;
    }

    uint8_t data[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    trans.length = 16;
    trans.tx_buffer = data;
    esp_err_t ret = spi_device_polling_transmit(spi, &trans);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to continue write color 0x%04X: %d", color, ret);
        lcd_idf_spi_end_write_color();  // Clean up on error
    }
} 