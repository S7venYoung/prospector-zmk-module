#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <lvgl.h>

enum prospector_swipe_direction {
    PROSPECTOR_SWIPE_UP,
    PROSPECTOR_SWIPE_DOWN,
    PROSPECTOR_SWIPE_LEFT,
    PROSPECTOR_SWIPE_RIGHT,
    PROSPECTOR_DOUBLE_TAP,
};

/* Call once after a theme creates its root display screen. */
void prospector_touch_attach(lv_obj_t *screen);
bool prospector_touch_settings_visible(void);

uint8_t prospector_settings_brightness(void);
void prospector_settings_set_brightness(uint8_t value);
bool prospector_settings_always_on(void);
void prospector_settings_set_always_on(bool enabled);
void prospector_settings_save(void);
