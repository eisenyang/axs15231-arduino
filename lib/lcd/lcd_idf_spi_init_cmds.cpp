#include "lcd_idf_spi.h"
////////// LCD init code ///////////////////
void lcd_idf_spi_init_cmds()
{
    lcd_idf_spi_write_cmd(0xBB);
    lcd_idf_spi_write_data(0x00); // 0
    lcd_idf_spi_write_data(0x00); // 1
    lcd_idf_spi_write_data(0x00); // 2
    lcd_idf_spi_write_data(0x00); // 3
    lcd_idf_spi_write_data(0x00); // 4
    lcd_idf_spi_write_data(0x00); // 5
    lcd_idf_spi_write_data(0x5A); // 6
    lcd_idf_spi_write_data(0xA5); // 7

    lcd_idf_spi_write_cmd(0xCA);
    lcd_idf_spi_write_data(0x21); // 0
    lcd_idf_spi_write_data(0x36); // 1
    lcd_idf_spi_write_data(0x00); // 2
    lcd_idf_spi_write_data(0x22); // 3

    lcd_idf_spi_write_cmd(0xA0);
    lcd_idf_spi_write_data(0x00); // 0
    lcd_idf_spi_write_data(0x30); // 1
    lcd_idf_spi_write_data(0x00); // 2
    lcd_idf_spi_write_data(0x02); // 3
    lcd_idf_spi_write_data(0x00); // 4
    lcd_idf_spi_write_data(0x00); // 5
    lcd_idf_spi_write_data(0x04); // 6
    lcd_idf_spi_write_data(0x3F); // 7
    lcd_idf_spi_write_data(0x20); // 8
    lcd_idf_spi_write_data(0x05); // 9
    lcd_idf_spi_write_data(0x3F); // 10
    lcd_idf_spi_write_data(0x3F); // 11
    lcd_idf_spi_write_data(0x00); // 12
    lcd_idf_spi_write_data(0x00); // 13
    lcd_idf_spi_write_data(0x00); // 14
    lcd_idf_spi_write_data(0x00); // 15
    lcd_idf_spi_write_data(0x00); // 16

    lcd_idf_spi_write_cmd(0xA2);
    lcd_idf_spi_write_data(0x30); // 0
    lcd_idf_spi_write_data(0x19); // 1
    lcd_idf_spi_write_data(0x60); // 2
    lcd_idf_spi_write_data(0x64); // 3
    lcd_idf_spi_write_data(0x9b); // 4-9b
    lcd_idf_spi_write_data(0x22); // 5
    lcd_idf_spi_write_data(0x50); // 6-50
    
    lcd_idf_spi_write_data(0x80); // 7
    lcd_idf_spi_write_data(0xAC); // 8
    lcd_idf_spi_write_data(0x28); // 9
    lcd_idf_spi_write_data(0x7F); // 10
    lcd_idf_spi_write_data(0x7F); // 11
    lcd_idf_spi_write_data(0x7F); // 12
    lcd_idf_spi_write_data(0x20); // 13
    lcd_idf_spi_write_data(0xF8); // 14
    lcd_idf_spi_write_data(0x10); // 15
    lcd_idf_spi_write_data(0x02); // 16
    lcd_idf_spi_write_data(0xFF); // 17
    lcd_idf_spi_write_data(0xFF); // 18
    lcd_idf_spi_write_data(0xF0); // 19
    lcd_idf_spi_write_data(0x90); // 20
    lcd_idf_spi_write_data(0x01); // 21
    lcd_idf_spi_write_data(0x32); // 22
    lcd_idf_spi_write_data(0xA0); // 23
    lcd_idf_spi_write_data(0x91); // 24
    lcd_idf_spi_write_data(0xC0); // 25
    lcd_idf_spi_write_data(0x20); // 26
    lcd_idf_spi_write_data(0x7F); // 27
    lcd_idf_spi_write_data(0xFF); // 28
    lcd_idf_spi_write_data(0x00); // 29
    lcd_idf_spi_write_data(0x04); // 30

    lcd_idf_spi_write_cmd(0xD0);
    lcd_idf_spi_write_data(0x80); // 0
    lcd_idf_spi_write_data(0xAC); // 1
    lcd_idf_spi_write_data(0x21); // 2
    lcd_idf_spi_write_data(0x24); // 3
    lcd_idf_spi_write_data(0x08); // 4
    lcd_idf_spi_write_data(0x09); // 5
    lcd_idf_spi_write_data(0x10); // 6
    lcd_idf_spi_write_data(0x01); // 7
    lcd_idf_spi_write_data(0x80); // 8
    lcd_idf_spi_write_data(0x12); // 9
    lcd_idf_spi_write_data(0xC2); // 10
    lcd_idf_spi_write_data(0x00); // 11
    lcd_idf_spi_write_data(0x22); // 12
    lcd_idf_spi_write_data(0x22); // 13
    lcd_idf_spi_write_data(0xAA); // 14
    lcd_idf_spi_write_data(0x03); // 15
    lcd_idf_spi_write_data(0x10); // 16
    lcd_idf_spi_write_data(0x12); // 17
    lcd_idf_spi_write_data(0x40); // 18
    lcd_idf_spi_write_data(0x14); // 19
    lcd_idf_spi_write_data(0x1E); // 20
    lcd_idf_spi_write_data(0x51); // 21
    lcd_idf_spi_write_data(0x15); // 22
    lcd_idf_spi_write_data(0x00); // 23
    lcd_idf_spi_write_data(0x40); // 24
    lcd_idf_spi_write_data(0x10); // 25
    lcd_idf_spi_write_data(0x00); // 26
    lcd_idf_spi_write_data(0x03); // 27
    lcd_idf_spi_write_data(0x7D); // 28
    lcd_idf_spi_write_data(0x12); // 29

    lcd_idf_spi_write_cmd(0xA3);
    lcd_idf_spi_write_data(0xA0); // 0
    lcd_idf_spi_write_data(0x06); // 1
    lcd_idf_spi_write_data(0xA9); // 2
    lcd_idf_spi_write_data(0x00); // 3
    lcd_idf_spi_write_data(0x08); // 4
    lcd_idf_spi_write_data(0x02); // 5
    lcd_idf_spi_write_data(0x0A); // 6
    lcd_idf_spi_write_data(0x04); // 7
    lcd_idf_spi_write_data(0x04); // 8
    lcd_idf_spi_write_data(0x04); // 9
    lcd_idf_spi_write_data(0x04); // 10
    lcd_idf_spi_write_data(0x04); // 11
    lcd_idf_spi_write_data(0x04); // 12
    lcd_idf_spi_write_data(0x04); // 13
    lcd_idf_spi_write_data(0x04); // 14
    lcd_idf_spi_write_data(0x04); // 15
    lcd_idf_spi_write_data(0x04); // 16
    lcd_idf_spi_write_data(0x04); // 17
    lcd_idf_spi_write_data(0x04); // 18
    lcd_idf_spi_write_data(0x00); // 19
    lcd_idf_spi_write_data(0x55); // 20
    lcd_idf_spi_write_data(0x55); // 21

    lcd_idf_spi_write_cmd(0xC1);
    lcd_idf_spi_write_data(0x33); // 0,0x31->0x33
    lcd_idf_spi_write_data(0x04); // 1
    lcd_idf_spi_write_data(0x02); // 2
    lcd_idf_spi_write_data(0x02); // 3
    lcd_idf_spi_write_data(0x71); // 4
    lcd_idf_spi_write_data(0x05); // 5
    lcd_idf_spi_write_data(0x24); // 6
    lcd_idf_spi_write_data(0x55); // 7
    lcd_idf_spi_write_data(0x02); // 8
    lcd_idf_spi_write_data(0x00); // 9
    lcd_idf_spi_write_data(0x41); // 10
    lcd_idf_spi_write_data(0x01); // 11
    lcd_idf_spi_write_data(0x53); // 12
    lcd_idf_spi_write_data(0xFF); // 13
    lcd_idf_spi_write_data(0xFF); // 14
    lcd_idf_spi_write_data(0xFF); // 15
    lcd_idf_spi_write_data(0x4F); // 16
    lcd_idf_spi_write_data(0x52); // 17
    lcd_idf_spi_write_data(0x00); // 18
    lcd_idf_spi_write_data(0x4F); // 19
    lcd_idf_spi_write_data(0x52); // 20
    lcd_idf_spi_write_data(0x00); // 21
    lcd_idf_spi_write_data(0x45); // 22
    lcd_idf_spi_write_data(0x3B); // 23
    lcd_idf_spi_write_data(0x0B); // 24
    lcd_idf_spi_write_data(0x02); // 25
    lcd_idf_spi_write_data(0x0D); // 26
    lcd_idf_spi_write_data(0x00); // 27
    lcd_idf_spi_write_data(0xFF); // 28
    lcd_idf_spi_write_data(0x40); // 29

    // lcd_idf_spi_write_cmd(0xC3); // Old
    // lcd_idf_spi_write_data(0x00);//0
    // lcd_idf_spi_write_data(0x00);//1
    // lcd_idf_spi_write_data(0x00);//2
    // lcd_idf_spi_write_data(0x50);//3
    // lcd_idf_spi_write_data(0x03);//4
    // lcd_idf_spi_write_data(0x00);//5
    // lcd_idf_spi_write_data(0x00);//6
    // lcd_idf_spi_write_data(0x00);//7
    // lcd_idf_spi_write_data(0x01);//8
    // lcd_idf_spi_write_data(0x80);//9
    // lcd_idf_spi_write_data(0x01);//10

    lcd_idf_spi_write_cmd(0xC3);  // New
    lcd_idf_spi_write_data(0x00); // 0
    lcd_idf_spi_write_data(0x00); // 1 cr_inv_en cr_bgr_en cr_sre_ne cr_sre_mode
    lcd_idf_spi_write_data(0x00); // 2
    lcd_idf_spi_write_data(0x50); // 3
    lcd_idf_spi_write_data(0x0B); // 4
    lcd_idf_spi_write_data(0x00); // 5
    lcd_idf_spi_write_data(0x00); // 6
    lcd_idf_spi_write_data(0x00); // 7
    lcd_idf_spi_write_data(0x01); // 8
    lcd_idf_spi_write_data(0x80); // 9
    lcd_idf_spi_write_data(0x01); // 10 cr_usr_pix_en

    lcd_idf_spi_write_cmd(0xC4);
    lcd_idf_spi_write_data(0x00); // 0
    lcd_idf_spi_write_data(0x24); // 1
    lcd_idf_spi_write_data(0x33); // 2
    lcd_idf_spi_write_data(0x80); // 3
    lcd_idf_spi_write_data(0x11); // 4
    lcd_idf_spi_write_data(0xea); // 5
    lcd_idf_spi_write_data(0x64); // 6
    lcd_idf_spi_write_data(0x32); // 7
    lcd_idf_spi_write_data(0xC8); // 8
    lcd_idf_spi_write_data(0x64); // 9
    lcd_idf_spi_write_data(0xC8); // 10
    lcd_idf_spi_write_data(0x32); // 11
    lcd_idf_spi_write_data(0x90); // 12
    lcd_idf_spi_write_data(0x90); // 13
    lcd_idf_spi_write_data(0x11); // 14
    lcd_idf_spi_write_data(0x06); // 15
    lcd_idf_spi_write_data(0xDC); // 16
    lcd_idf_spi_write_data(0xFA); // 17
    lcd_idf_spi_write_data(0x00); // 18
    lcd_idf_spi_write_data(0x00); // 19
    lcd_idf_spi_write_data(0x80); // 20
    lcd_idf_spi_write_data(0xFE); // 21
    lcd_idf_spi_write_data(0x10); // 22
    lcd_idf_spi_write_data(0x10); // 23
    lcd_idf_spi_write_data(0x00); // 24
    lcd_idf_spi_write_data(0x0A); // 25
    lcd_idf_spi_write_data(0x0A); // 26
    lcd_idf_spi_write_data(0x44); // 27
    lcd_idf_spi_write_data(0x50); // 28




    lcd_idf_spi_write_cmd(0xC5);
    lcd_idf_spi_write_data(0x18); // 0
    lcd_idf_spi_write_data(0x00); // 1
    lcd_idf_spi_write_data(0x00); // 2
    lcd_idf_spi_write_data(0x03); // 3
    lcd_idf_spi_write_data(0xFE); // 4
    lcd_idf_spi_write_data(0x08); // 5
    lcd_idf_spi_write_data(0x68); // 6
    lcd_idf_spi_write_data(0x30); // 7
    lcd_idf_spi_write_data(0x00); // 8-10
    lcd_idf_spi_write_data(0x98); // 9-05
    lcd_idf_spi_write_data(0x89); // 10
    lcd_idf_spi_write_data(0xDE); // 11
    lcd_idf_spi_write_data(0x0D); // 12
    lcd_idf_spi_write_data(0x08); // 13
    lcd_idf_spi_write_data(0x0F); // 14
    lcd_idf_spi_write_data(0x0F); // 15
    lcd_idf_spi_write_data(0x01); // 16
    lcd_idf_spi_write_data(0x08); // 17
    lcd_idf_spi_write_data(0x68); // 18
    lcd_idf_spi_write_data(0x30); // 19
    lcd_idf_spi_write_data(0x10); // 20
    lcd_idf_spi_write_data(0x10); // 21
    lcd_idf_spi_write_data(0x08); // 22 00-08

    lcd_idf_spi_write_cmd(0xC6);
    lcd_idf_spi_write_data(0x05); // 0
    lcd_idf_spi_write_data(0x0A); // 1
    lcd_idf_spi_write_data(0x05); // 2
    lcd_idf_spi_write_data(0x0A); // 3
    lcd_idf_spi_write_data(0x00); // 4
    lcd_idf_spi_write_data(0xE0); // 5
    lcd_idf_spi_write_data(0x2E); // 6
    lcd_idf_spi_write_data(0x0B); // 7
    lcd_idf_spi_write_data(0x12); // 8
    lcd_idf_spi_write_data(0x22); // 9
    lcd_idf_spi_write_data(0x12); // 10
    lcd_idf_spi_write_data(0x22); // 11
    lcd_idf_spi_write_data(0x01); // 12
    lcd_idf_spi_write_data(0x00); // 13
    lcd_idf_spi_write_data(0x00); // 14
    lcd_idf_spi_write_data(0x02); // 15
    lcd_idf_spi_write_data(0x6A); // 16
    lcd_idf_spi_write_data(0x18); // 17
    lcd_idf_spi_write_data(0xC8); // 18
    lcd_idf_spi_write_data(0x22); // 19

    lcd_idf_spi_write_cmd(0xC7);
    lcd_idf_spi_write_data(0x50); // 0
    lcd_idf_spi_write_data(0x36); // 1
    lcd_idf_spi_write_data(0x28); // 2
    lcd_idf_spi_write_data(0x00); // 3
    lcd_idf_spi_write_data(0xa2); // 4
    lcd_idf_spi_write_data(0x80); // 5
    lcd_idf_spi_write_data(0x8f); // 6
    lcd_idf_spi_write_data(0x00); // 7
    lcd_idf_spi_write_data(0x80); // 8
    lcd_idf_spi_write_data(0xff); // 9
    lcd_idf_spi_write_data(0x07); // 10
    lcd_idf_spi_write_data(0x11); // 11
    lcd_idf_spi_write_data(0x9c); // 12
    lcd_idf_spi_write_data(0x6f);     // 13
    lcd_idf_spi_write_data(0xff); // 14
    lcd_idf_spi_write_data(0x24); // 15
    lcd_idf_spi_write_data(0x0c); // 16
    lcd_idf_spi_write_data(0x0d); // 17
    lcd_idf_spi_write_data(0x0e); // 18
    lcd_idf_spi_write_data(0x0f); // 19

    lcd_idf_spi_write_cmd(0xC9);
    lcd_idf_spi_write_data(0x33); // 0
    lcd_idf_spi_write_data(0x44); // 1
    lcd_idf_spi_write_data(0x44); // 2
    lcd_idf_spi_write_data(0x01); // 3

    lcd_idf_spi_write_cmd(0xCF);
    lcd_idf_spi_write_data(0x2C); // 0
    lcd_idf_spi_write_data(0x1E); // 1
    lcd_idf_spi_write_data(0x88); // 2
    lcd_idf_spi_write_data(0x58); // 3
    lcd_idf_spi_write_data(0x13); // 4
    lcd_idf_spi_write_data(0x18); // 5
    lcd_idf_spi_write_data(0x56); // 6
    lcd_idf_spi_write_data(0x18); // 7
    lcd_idf_spi_write_data(0x1E); // 8
    lcd_idf_spi_write_data(0x68); // 9
    lcd_idf_spi_write_data(0xF8); // 10
    lcd_idf_spi_write_data(0x00); // 11
    lcd_idf_spi_write_data(0x66); // 12
    lcd_idf_spi_write_data(0x0d); // 13
    lcd_idf_spi_write_data(0x22); // 14
    lcd_idf_spi_write_data(0xC4); // 15
    lcd_idf_spi_write_data(0x0C); // 16
    lcd_idf_spi_write_data(0x77); // 17
    lcd_idf_spi_write_data(0x22); // 18
    lcd_idf_spi_write_data(0x44); // 19
    lcd_idf_spi_write_data(0xAA); // 20
    lcd_idf_spi_write_data(0x55); // 21
    lcd_idf_spi_write_data(0x04); // 22
    lcd_idf_spi_write_data(0x04); // 23
    lcd_idf_spi_write_data(0x12); // 24
    lcd_idf_spi_write_data(0xA0); // 25
    lcd_idf_spi_write_data(0x08); // 26

    lcd_idf_spi_write_cmd(0xD5);
    lcd_idf_spi_write_data(0x50); // 0
    lcd_idf_spi_write_data(0x60); // 1
    lcd_idf_spi_write_data(0x8a); // 2
    lcd_idf_spi_write_data(0x00); // 3
    lcd_idf_spi_write_data(0x35); // 4
    lcd_idf_spi_write_data(0x04); // 5
    lcd_idf_spi_write_data(0x71); // 6
    lcd_idf_spi_write_data(0x02); // 7
    lcd_idf_spi_write_data(0x03); // 8
    lcd_idf_spi_write_data(0x03); // 9
    lcd_idf_spi_write_data(0x03); // 10
    lcd_idf_spi_write_data(0x00); // 11
    lcd_idf_spi_write_data(0x04); // 12
    lcd_idf_spi_write_data(0x02); // 13
    lcd_idf_spi_write_data(0x13); // 14
    lcd_idf_spi_write_data(0x46); // 15
    lcd_idf_spi_write_data(0x03); // 16
    lcd_idf_spi_write_data(0x03); // 17
    lcd_idf_spi_write_data(0x03); // 18
    lcd_idf_spi_write_data(0x03); // 19
    lcd_idf_spi_write_data(0x86); // 20
    lcd_idf_spi_write_data(0x00); // 21
    lcd_idf_spi_write_data(0x00); // 22
    lcd_idf_spi_write_data(0x00); // 23
    lcd_idf_spi_write_data(0x80); // 24
    lcd_idf_spi_write_data(0x52); // 25
    lcd_idf_spi_write_data(0x7c); // 26
    lcd_idf_spi_write_data(0x00); // 27
    lcd_idf_spi_write_data(0x00); // 28
    lcd_idf_spi_write_data(0x00); // 29

    lcd_idf_spi_write_cmd(0xD6);
    lcd_idf_spi_write_data(0x10); // 0
    lcd_idf_spi_write_data(0x32); // 1
    lcd_idf_spi_write_data(0x54); // 2
    lcd_idf_spi_write_data(0x76); // 3
    lcd_idf_spi_write_data(0x98); // 4
    lcd_idf_spi_write_data(0xBA); // 5
    lcd_idf_spi_write_data(0xDC); // 6
    lcd_idf_spi_write_data(0xFE); // 7
    lcd_idf_spi_write_data(0x00); // 8
    lcd_idf_spi_write_data(0x00); // 9
    lcd_idf_spi_write_data(0x01); // 10
    lcd_idf_spi_write_data(0x83); // 11
    lcd_idf_spi_write_data(0x03); // 12
    lcd_idf_spi_write_data(0x03); // 13
    lcd_idf_spi_write_data(0x33); // 14
    lcd_idf_spi_write_data(0x03); // 15
    lcd_idf_spi_write_data(0x03); // 16
    lcd_idf_spi_write_data(0x33); // 17
    lcd_idf_spi_write_data(0x3F); // 18
    lcd_idf_spi_write_data(0x03); // 19
    lcd_idf_spi_write_data(0x03); // 20
    lcd_idf_spi_write_data(0x03); // 21
    lcd_idf_spi_write_data(0x20); // 22
    lcd_idf_spi_write_data(0x20); // 23
    lcd_idf_spi_write_data(0x00); // 24
    lcd_idf_spi_write_data(0x24); // 25
    lcd_idf_spi_write_data(0x51); // 26
    lcd_idf_spi_write_data(0x23); // 27
    lcd_idf_spi_write_data(0x01); // 28
    lcd_idf_spi_write_data(0x00); // 29

    lcd_idf_spi_write_cmd(0xD7);  // CG
    lcd_idf_spi_write_data(0x18); // 0
    lcd_idf_spi_write_data(0x1A); // 1
    lcd_idf_spi_write_data(0x1B); // 2
    lcd_idf_spi_write_data(0x1F); // 3
    lcd_idf_spi_write_data(0x0A); // 4
    lcd_idf_spi_write_data(0x08); // 5
    lcd_idf_spi_write_data(0x0E); // 6
    lcd_idf_spi_write_data(0x0C); // 7
    lcd_idf_spi_write_data(0x00); // 8
    lcd_idf_spi_write_data(0x1F); // 9
    lcd_idf_spi_write_data(0x1d); // 10
    lcd_idf_spi_write_data(0x1F); // 11
    lcd_idf_spi_write_data(0x50); // 12
    lcd_idf_spi_write_data(0x60); // 13
    lcd_idf_spi_write_data(0x04); // 14
    lcd_idf_spi_write_data(0x00); // 15
    lcd_idf_spi_write_data(0x1f); // 16
    lcd_idf_spi_write_data(0x1F); // 17
    lcd_idf_spi_write_data(0x1F); // 18

    lcd_idf_spi_write_cmd(0xD8);  // CG
    lcd_idf_spi_write_data(0x18); // 0
    lcd_idf_spi_write_data(0x1A); // 1
    lcd_idf_spi_write_data(0x1B); // 2
    lcd_idf_spi_write_data(0x1f); // 3
    lcd_idf_spi_write_data(0x0B); // 4
    lcd_idf_spi_write_data(0x09); // 5
    lcd_idf_spi_write_data(0x0F); // 6
    lcd_idf_spi_write_data(0x0D); // 7
    lcd_idf_spi_write_data(0x01); // 8
    lcd_idf_spi_write_data(0x1F); // 9
    lcd_idf_spi_write_data(0x1d); // 10
    lcd_idf_spi_write_data(0x1F); // 11

    lcd_idf_spi_write_cmd(0xD9);
    lcd_idf_spi_write_data(0x0F); // 1
    lcd_idf_spi_write_data(0x09); // 2
    lcd_idf_spi_write_data(0x0B); // 3
    lcd_idf_spi_write_data(0x1F); // 4
    lcd_idf_spi_write_data(0x18); // 5
    lcd_idf_spi_write_data(0x19); // 6
    lcd_idf_spi_write_data(0x1F); // 7
    lcd_idf_spi_write_data(0x01); // 8
    lcd_idf_spi_write_data(0x1E); // 9
    lcd_idf_spi_write_data(0x1d); // 10
    lcd_idf_spi_write_data(0x1F); // 11

    lcd_idf_spi_write_cmd(0xDD);
    lcd_idf_spi_write_data(0x0E); // 1
    lcd_idf_spi_write_data(0x08); // 2
    lcd_idf_spi_write_data(0x0A); // 3
    lcd_idf_spi_write_data(0x1F); // 4
    lcd_idf_spi_write_data(0x18); // 5
    lcd_idf_spi_write_data(0x19); // 6
    lcd_idf_spi_write_data(0x1F); // 7
    lcd_idf_spi_write_data(0x00); // 8
    lcd_idf_spi_write_data(0x1E); // 9
    lcd_idf_spi_write_data(0x1A); // 10
    lcd_idf_spi_write_data(0x1F); // 11

    lcd_idf_spi_write_cmd(0xDF);
    lcd_idf_spi_write_data(0x44); // 0
    lcd_idf_spi_write_data(0x33); // 1
    lcd_idf_spi_write_data(0x4B); // 2
    lcd_idf_spi_write_data(0x69); // 3
    lcd_idf_spi_write_data(0x00); // 4
    lcd_idf_spi_write_data(0x0A); // 5
    lcd_idf_spi_write_data(0x02); // 6
    lcd_idf_spi_write_data(0x90); // 7

    /////////////////2.2
    lcd_idf_spi_write_cmd(0xE0);
    lcd_idf_spi_write_data(0x35); // 0
    lcd_idf_spi_write_data(0x08); // 1
    lcd_idf_spi_write_data(0x19); // 2
    lcd_idf_spi_write_data(0x1c); // 3
    lcd_idf_spi_write_data(0x0c); // 4
    lcd_idf_spi_write_data(0x09); // 5
    lcd_idf_spi_write_data(0x13); // 6
    lcd_idf_spi_write_data(0x2a); // 7
    lcd_idf_spi_write_data(0x54); // 8
    lcd_idf_spi_write_data(0x21); // 9
    lcd_idf_spi_write_data(0x0b); // 10
    lcd_idf_spi_write_data(0x15); // 11
    lcd_idf_spi_write_data(0x13); // 12
    lcd_idf_spi_write_data(0x25); // 13
    lcd_idf_spi_write_data(0x27); // 14
    lcd_idf_spi_write_data(0x08); // 15
    lcd_idf_spi_write_data(0x00); // 16

    lcd_idf_spi_write_cmd(0xE1);
    lcd_idf_spi_write_data(0x3e); // 0
    lcd_idf_spi_write_data(0x08); // 1
    lcd_idf_spi_write_data(0x19); // 2
    lcd_idf_spi_write_data(0x1c); // 3
    lcd_idf_spi_write_data(0x0c); // 4
    lcd_idf_spi_write_data(0x08); // 5
    lcd_idf_spi_write_data(0x13); // 6
    lcd_idf_spi_write_data(0x2a); // 7
    lcd_idf_spi_write_data(0x54); // 8
    lcd_idf_spi_write_data(0x21); // 9
    lcd_idf_spi_write_data(0x0b); // 10
    lcd_idf_spi_write_data(0x14); // 11
    lcd_idf_spi_write_data(0x13); // 12
    lcd_idf_spi_write_data(0x26); // 13
    lcd_idf_spi_write_data(0x27); // 14
    lcd_idf_spi_write_data(0x08); // 15
    lcd_idf_spi_write_data(0x0f); // 16

    /////gamma 2.0/////
    lcd_idf_spi_write_cmd(0xE2);
    lcd_idf_spi_write_data(0x19); // 0
    lcd_idf_spi_write_data(0x20); // 1
    lcd_idf_spi_write_data(0x0A); // 2
    lcd_idf_spi_write_data(0x11); // 3
    lcd_idf_spi_write_data(0x09); // 4
    lcd_idf_spi_write_data(0x06); // 5
    lcd_idf_spi_write_data(0x11); // 6
    lcd_idf_spi_write_data(0x25); // 7
    lcd_idf_spi_write_data(0xD4); // 8
    lcd_idf_spi_write_data(0x22); // 9
    lcd_idf_spi_write_data(0x0B); // 10
    lcd_idf_spi_write_data(0x13); // 11
    lcd_idf_spi_write_data(0x12); // 12
    lcd_idf_spi_write_data(0x2D); // 13
    lcd_idf_spi_write_data(0x32); // 14
    lcd_idf_spi_write_data(0x2f); // 15
    lcd_idf_spi_write_data(0x03); // 16

    lcd_idf_spi_write_cmd(0xE3);
    lcd_idf_spi_write_data(0x38); // 0
    lcd_idf_spi_write_data(0x20); // 1
    lcd_idf_spi_write_data(0x0A); // 2
    lcd_idf_spi_write_data(0x11); // 3
    lcd_idf_spi_write_data(0x09); // 4
    lcd_idf_spi_write_data(0x06); // 5
    lcd_idf_spi_write_data(0x11); // 6
    lcd_idf_spi_write_data(0x25); // 7
    lcd_idf_spi_write_data(0xC4); // 8
    lcd_idf_spi_write_data(0x21); // 9
    lcd_idf_spi_write_data(0x0A); // 10
    lcd_idf_spi_write_data(0x12); // 11
    lcd_idf_spi_write_data(0x11); // 12
    lcd_idf_spi_write_data(0x2C); // 13
    lcd_idf_spi_write_data(0x32); // 14
    lcd_idf_spi_write_data(0x2f); // 15
    lcd_idf_spi_write_data(0x27); // 16

    lcd_idf_spi_write_cmd(0xE4);
    lcd_idf_spi_write_data(0x19); // 0
    lcd_idf_spi_write_data(0x20); // 1
    lcd_idf_spi_write_data(0x0D); // 2
    lcd_idf_spi_write_data(0x14); // 3
    lcd_idf_spi_write_data(0x0D); // 4
    lcd_idf_spi_write_data(0x08); // 5
    lcd_idf_spi_write_data(0x12); // 6
    lcd_idf_spi_write_data(0x2A); // 7
    lcd_idf_spi_write_data(0xD4); // 8
    lcd_idf_spi_write_data(0x26); // 9
    lcd_idf_spi_write_data(0x0E); // 10
    lcd_idf_spi_write_data(0x15); // 11
    lcd_idf_spi_write_data(0x13); // 12
    lcd_idf_spi_write_data(0x34); // 13
    lcd_idf_spi_write_data(0x39); // 14
    lcd_idf_spi_write_data(0x2f); // 15
    lcd_idf_spi_write_data(0x03); // 16

    lcd_idf_spi_write_cmd(0xE5);
    lcd_idf_spi_write_data(0x38); // 0
    lcd_idf_spi_write_data(0x20); // 1
    lcd_idf_spi_write_data(0x0D); // 2
    lcd_idf_spi_write_data(0x13); // 3
    lcd_idf_spi_write_data(0x0D); // 4
    lcd_idf_spi_write_data(0x07); // 5
    lcd_idf_spi_write_data(0x12); // 6
    lcd_idf_spi_write_data(0x29); // 7
    lcd_idf_spi_write_data(0xC4); // 8
    lcd_idf_spi_write_data(0x25); // 9
    lcd_idf_spi_write_data(0x0D); // 10
    lcd_idf_spi_write_data(0x15); // 11
    lcd_idf_spi_write_data(0x12); // 12
    lcd_idf_spi_write_data(0x33); // 13
    lcd_idf_spi_write_data(0x39); // 14
    lcd_idf_spi_write_data(0x2f); // 15
    lcd_idf_spi_write_data(0x27); // 16

    lcd_idf_spi_write_cmd(0xBB);
    lcd_idf_spi_write_data(0x00); // 0
    lcd_idf_spi_write_data(0x00); // 1
    lcd_idf_spi_write_data(0x00); // 2
    lcd_idf_spi_write_data(0x00); // 3
    lcd_idf_spi_write_data(0x00); // 4
    lcd_idf_spi_write_data(0x00); // 5
    lcd_idf_spi_write_data(0x00); // 6
    lcd_idf_spi_write_data(0x00); // 7


    //lcd_idf_spi_write_cmd(0x34);
    lcd_idf_spi_write_cmd(0x35);
    lcd_idf_spi_write_data(0x01); // 0-model1，2-model2
    //delay(200);
    esp_rom_delay_us(200000);  // 200ms
    lcd_idf_spi_write_cmd(0x11);
    lcd_idf_spi_write_data(0x00);
    //delay(200);
    esp_rom_delay_us(200000);  // 200ms
    lcd_idf_spi_write_cmd(0x29);
    lcd_idf_spi_write_data(0x00);
    //delay(100);
    esp_rom_delay_us(100000);  // 100ms

}
