# Esp32-S3 + AXS15231B Hardware Scrolling Demo

这是一个显示屏硬件滚屏测试项目。

- 使用 172x640 分辨率的 LCD 显示屏
- 屏幕驱动 IC 为：[AXS15231B](doc/AXS15231B_Datasheet_V0.5_20230306.pdf) 
- 实现硬件滚动功能
- 使用 SPI 接口与显示屏通信

## 硬件连接

| LCD引脚 | ESP32-S3引脚 |
|---------|--------------|
| MISO    | 10          |
| MOSI    | 17          |
| CLK     | 18          |
| CS      | 14          |
| DC      | 15          |
| RST     | 13          |

## 开发环境要求

- PlatformIO
- ESP32-S3 开发板
- Arduino 框架
- 16MB Flash
- PSRAM 支持 