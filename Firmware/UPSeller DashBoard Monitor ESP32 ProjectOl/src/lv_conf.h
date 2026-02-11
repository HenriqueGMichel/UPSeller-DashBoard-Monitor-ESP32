#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_CONF_SKIP // Isso ignora a verificação de caminho interna da LVGL
#include <stdint.h>

/* Configure o resto do arquivo abaixo... */

#define LV_CONF_SKIP 
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0 // Teste 1 se as cores do SquareLine ficarem invertidas
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (32U * 1024U)
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
/* No arquivo lv_conf.h, deixe assim: */

#define LV_FONT_MONTSERRAT_14 1 // Geralmente já vem como 1
#define LV_FONT_MONTSERRAT_18 1 // Ative esta
#define LV_FONT_MONTSERRAT_22 1 // Ative esta
#define LV_FONT_MONTSERRAT_24 1 // Ative esta
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_36 1 // Ative esta
#endif