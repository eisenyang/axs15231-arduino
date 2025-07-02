#pragma once

#include <stdio.h>
#include "ft2build.h"
#include <stdlib.h>
#include "esp_spiffs.h"
#include FT_FREETYPE_H
void vector_font_init(void);
void vector_font_render_code(FT_ULong char_code, bool ref_map_buff);