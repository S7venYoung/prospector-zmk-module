#pragma once
#include "prospector_layouts.h"
void scanner_codex_theme_create(lv_obj_t *screen);
void scanner_codex_theme_destroy(void);
void scanner_codex_theme_update(const struct prospector_keyboard_data *data);


struct scanner_host_status {
    uint8_t codex_left_percent;
    uint32_t codex_total_tokens;
    bool codex_available;
};

void scanner_codex_theme_host_update(const struct scanner_host_status *status);
