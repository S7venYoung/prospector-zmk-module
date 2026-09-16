#pragma once

#include "prospector_layouts.h"
#include "scanner_host_status.h"

void scanner_codex_theme_create(lv_obj_t *screen);
void scanner_codex_theme_destroy(void);
void scanner_codex_theme_update(const struct prospector_keyboard_data *data);
void scanner_codex_theme_host_update(const struct scanner_host_status *status);
