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

## 分区表配置

ESP32-S3-A10芯片使用以下分区方案:

| 分区名称 | 类型 | 偏移量 | 大小 | 用途 |
|---------|------|-------|------|-----|
| nvs      | data | 0x9000  | 20KB | 存储非易失性数据 |
| otadata  | data | 0xe000  | 8KB  | OTA更新数据 |
| phy_init | data | 0x10000 | 4KB  | 物理层初始化数据 |
| ota_0    | app  | 0x20000 | 3.5MB | 应用程序分区1 |
| ota_1    | app  | 0x3A0000| 3.5MB | 应用程序分区2 |
| storage  | data | 0x720000| 8.875MB | 文件系统存储 | 