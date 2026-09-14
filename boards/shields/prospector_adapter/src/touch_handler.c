#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/kernel.h>
#include <lvgl.h>
#include "prospector_touch.h"

#define TOUCH_NODE DT_NODELABEL(touch_sensor)
#define SWIPE_DISTANCE 30
#define SWIPE_COOLDOWN_MS 350

static lv_obj_t *settings_panel;
static uint16_t x, y;
static bool touched;
static int16_t start_x, start_y;
static int64_t last_gesture;

static void settings_refresh(void) {
    if (!settings_panel) return;
    lv_obj_t *value = lv_obj_get_child(settings_panel, 2);
    lv_label_set_text_fmt(value, "%u%%", prospector_settings_brightness());
}
static void settings_show(bool show) {
    if (!settings_panel) return;
    if (show) { lv_obj_clear_flag(settings_panel, LV_OBJ_FLAG_HIDDEN); settings_refresh(); }
    else { lv_obj_add_flag(settings_panel, LV_OBJ_FLAG_HIDDEN); prospector_settings_save(); }
}
bool prospector_touch_settings_visible(void) { return settings_panel && !lv_obj_has_flag(settings_panel, LV_OBJ_FLAG_HIDDEN); }
static void handle_gesture(enum prospector_swipe_direction direction) {
    if (direction == PROSPECTOR_SWIPE_DOWN && !prospector_touch_settings_visible()) { settings_show(true); return; }
    if (direction == PROSPECTOR_SWIPE_UP && prospector_touch_settings_visible()) { settings_show(false); return; }
    if (!prospector_touch_settings_visible()) return;
    if (direction == PROSPECTOR_SWIPE_LEFT) prospector_settings_set_brightness(prospector_settings_brightness() > 10 ? prospector_settings_brightness() - 10 : 1);
    if (direction == PROSPECTOR_SWIPE_RIGHT) prospector_settings_set_brightness(MIN(100, prospector_settings_brightness() + 10));
    settings_refresh();
}
static void touch_input(struct input_event *evt, void *user_data) {
    ARG_UNUSED(user_data);
    if (evt->code == INPUT_ABS_X) { x = evt->value; return; }
    if (evt->code == INPUT_ABS_Y) { y = evt->value; return; }
    if (evt->code != INPUT_BTN_TOUCH) return;
    bool now = evt->value != 0;
    if (now && !touched) { start_x = x; start_y = y; }
    if (!now && touched && k_uptime_get() - last_gesture > SWIPE_COOLDOWN_MS) {
        int16_t dx = (int16_t)y - start_y, dy = start_x - (int16_t)x;
        int16_t ax = ABS(dx), ay = ABS(dy);
        enum prospector_swipe_direction direction;
        if (MAX(ax, ay) >= SWIPE_DISTANCE) { direction = ay > ax ? (dy > 0 ? PROSPECTOR_SWIPE_DOWN : PROSPECTOR_SWIPE_UP) : (dx > 0 ? PROSPECTOR_SWIPE_RIGHT : PROSPECTOR_SWIPE_LEFT); handle_gesture(direction); last_gesture = k_uptime_get(); }
    }
    touched = now;
}
INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(TOUCH_NODE), touch_input, NULL);

void prospector_touch_attach(lv_obj_t *screen) {
    if (settings_panel) return;
    settings_panel = lv_obj_create(screen);
    lv_obj_set_size(settings_panel, 244, 164); lv_obj_set_pos(settings_panel, 18, 38);
    lv_obj_set_style_bg_color(settings_panel, lv_color_hex(0x101411), 0);
    lv_obj_set_style_bg_opa(settings_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(settings_panel, lv_color_hex(0xFFBF18), 0);
    lv_obj_set_style_border_width(settings_panel, 2, 0); lv_obj_set_style_radius(settings_panel, 14, 0);
    lv_obj_t *title = lv_label_create(settings_panel); lv_label_set_text(title, "DISPLAY SETTINGS"); lv_obj_set_pos(title, 16, 14);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFBF18), 0);
    lv_obj_t *label = lv_label_create(settings_panel); lv_label_set_text(label, "BRIGHTNESS"); lv_obj_set_pos(label, 16, 56);
    lv_obj_set_style_text_color(label, lv_color_hex(0xF3EEE5), 0);
    lv_obj_t *value = lv_label_create(settings_panel); lv_obj_set_pos(value, 150, 56);
    lv_obj_set_style_text_color(value, lv_color_hex(0xFFBF18), 0);
    lv_obj_t *hint = lv_label_create(settings_panel); lv_label_set_text(hint, "LEFT / RIGHT  ADJUST\nUP  SAVE & CLOSE"); lv_obj_set_pos(hint, 16, 112);
    lv_obj_set_style_text_color(hint, lv_color_hex(0xA8A8A4), 0);
    lv_obj_add_flag(settings_panel, LV_OBJ_FLAG_HIDDEN); settings_refresh();
}
