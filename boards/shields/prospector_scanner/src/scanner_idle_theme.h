#pragma once

#include <lvgl.h>

/* Scanner standby view shown until a compatible status advertisement is found. */
void scanner_idle_theme_create(lv_obj_t *screen);
void scanner_idle_theme_destroy(void);
