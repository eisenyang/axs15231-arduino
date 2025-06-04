//https://github.com/espressif/esp-idf/tree/v5.4.1/examples/peripherals/spi_master/lcd
#include "lcd_scroll.h"
#define SCROLL_ROW 1 // 滚屏行数
#define SCREEN_ROW 1 // 开屏行数
#define MY_TTF "/AlibabaPuHuiTi-3-35-Thin.ttf"
scroll_info_t _scroll_info;
SpriteTextManager _spriteTextManager;
TruetypeManager _truetypeManager;
TimerHandle_t _xTimer;
SemaphoreHandle_t _xMutex;
uint64_t te_start_time=0;
static uint16_t _address = LCD_HEIGHT;
void set_scroll_string(const char *str){
    _truetypeManager.setDrawString(str);
}
bool init_scroll(){

    while (!_truetypeManager.initTruetype(MY_TTF, nullptr))
    {
        Serial.println("initTruetype failed");
        delay(100);
    }
    _spriteTextManager.init();
    _spriteTextManager.fillScreen(TFT_BLACK);
    _spriteTextManager.setScrollWindow(0, LCD_HEIGHT, 0);
    _xMutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(read_buf_from_truetype_task, "read_buf_from_truetype_task", 8048, NULL, 1, NULL, 1);

    _xTimer = xTimerCreate("read_buf_to_screen_task", pdMS_TO_TICKS(2), pdTRUE, NULL, read_buf_to_screen_task);
    if (_xTimer == NULL)
    {
        Serial.println("定时器创建失败！");
        return false;
    }

    if (xTimerStart(_xTimer, 0) != pdPASS)
    {
        Serial.println("定时器启动失败！");
        return false;
    }
    return true;
}
void start_scroll(){
    lcd_spi_set_te_callback(te_irs_task);
}
void read_buf_from_truetype_task(void *pvParameters)
{

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(10); // 2毫秒
  // 获取当前时间（系统启动后的 tick 数）
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    _truetypeManager.readTextToFramebuffer();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    //vTaskDelay(pdMS_TO_TICKS(2));
  }
}
void srcoll_screen(){
    if(_scroll_info.readCompleted){
        _scroll_info.readCompleted = false;
        if(_address <= 0){
            _address = LCD_HEIGHT;
        }
        ulong startTime = micros();
        _spriteTextManager.scrollStart(_address);
        _address = _address - SCROLL_ROW;
        ulong currentTime = micros();
        if (_scroll_info.y_scroll_offset >= HEIGHT_PIXELS)
        {
            _scroll_info.y_scroll_offset = 0;
            _truetypeManager.resetFramebuffer(_scroll_info.availableIndex);
            set_available_index(_scroll_info);
        }
        _scroll_info.y_scroll_offset++;
        _scroll_info.scrollCompleted = true;
    }
}

void set_available_index(scroll_info_t &scroll_info){
    if(scroll_info.availableIndex >= BUF_COUNT - 1){
        scroll_info.availableIndex = 0;
    }
    else{
        scroll_info.availableIndex++;
    }
}

void te_irs_task(void *pvParameters){

  if (xSemaphoreTake(_xMutex, 0) == pdTRUE)
    {
      int64_t current_time = esp_timer_get_time();
      int64_t te_time = current_time - te_start_time;
      if(te_time > 2000){
        srcoll_screen();
        te_start_time = current_time;
      }
      xSemaphoreGive(_xMutex);
    }
}

void read_buf_to_screen_task(TimerHandle_t pxTimer)
{

  //printInterruptTime();
  if(xSemaphoreTake(_xMutex, 0) == pdTRUE){
    if(!_scroll_info.scrollCompleted){
      xSemaphoreGive(_xMutex);
      return;
    }
    ulong startTime = micros();
    framebuffer_t *framebuffer_t = _truetypeManager.getFramebuffer(_scroll_info.availableIndex);
    uint8_t *framebuffer = framebuffer_t->framebuffer;
    if (!framebuffer_t->hasData)
    {
      xSemaphoreGive(_xMutex);
      return;
    }
    write_buf_to_screen(framebuffer, _scroll_info.y_scroll_offset);
    ulong currentTime = micros();
    _scroll_info.readCompleted = true;
    xSemaphoreGive(_xMutex);
  }
}

void write_buf_to_screen(uint8_t *framebuffer, uint16_t y)
{
  static uint16_t top_offset = 0;
    if (y%SCREEN_ROW == 0)
    {      
      _spriteTextManager.setRowAddress(top_offset, _address,SCREEN_ROW);      
    }

     // 一次准备一整行的数据
    
    static uint16_t color_buffer[LCD_WIDTH];  // 用于批量传输的缓冲区

    for (int x = 0; x < LCD_WIDTH; x++)
    {
        if (y < HEIGHT_PIXELS && x < WIDTH_PIXELS)
        {
            color_buffer[x] = _truetypeManager.getPixelColor(framebuffer, x, y);
        }
        else
        {
            color_buffer[x] = TFT_BLACK;
        }
    }
    
    // 批量传输一整行
    _spriteTextManager.writeColors(color_buffer, LCD_WIDTH);
    if (y >= HEIGHT_PIXELS)
    {
      // 一行结束，重新初始化
      y = 0;
      _spriteTextManager.clearSprite();
    }
   
}