#include "scanner_retro_workstation_theme.h"

#include <lvgl.h>
#include <zephyr/sys/printk.h>

/* Rebuilt from the retro-workstation reference with LVGL primitives. This
 * avoids a 134 KB bitmap, allowing every scanner theme to fit the XIAO BLE. */
static lv_obj_t *layer_value;
static lv_obj_t *wpm_value;
static lv_obj_t *left_battery_value;
static lv_obj_t *right_battery_value;

static lv_obj_t *panel(lv_obj_t *parent, int x, int y, int w, int h,
                       uint32_t background, uint32_t border, int radius) {
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_color(obj, lv_color_hex(background), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(obj, lv_color_hex(border), 0);
    lv_obj_set_style_border_width(obj, 2, 0);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return obj;
}

static lv_obj_t *text(lv_obj_t *parent, const char *value, const lv_font_t *font,
                      uint32_t color, int x, int y) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, value);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_pos(label, x, y);
    return label;
}

static void divider(lv_obj_t *parent, int x, int y, int width, int height) {
    lv_obj_t *line = lv_obj_create(parent);
    lv_obj_remove_style_all(line);
    lv_obj_set_pos(line, x, y);
    lv_obj_set_size(line, width, height);
    lv_obj_set_style_bg_color(line, lv_color_hex(0x776F60), 0);
    lv_obj_set_style_bg_opa(line, LV_OPA_COVER, 0);
}

void scanner_retro_workstation_theme_create(lv_obj_t *screen) {
    const uint32_t paper = 0xE9DFC6;
    const uint32_t ink = 0x27231C;
    const uint32_t shadow = 0x8A806D;

    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, 280, 240);
    lv_obj_set_style_bg_color(screen, lv_color_hex(paper), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_style_radius(screen, 43, 0);
    lv_obj_set_style_clip_corner(screen, true, 0);

    text(screen, "[+]  ZMK Dongle", &lv_font_montserrat_12, ink, 17, 8);
    text(screen, "USB", &lv_font_montserrat_12, ink, 222, 8);
    divider(screen, 12, 27, 256, 2);

    panel(screen, 10, 39, 62, 72, 0xF5ECD9, ink, 5);
    panel(screen, 208, 39, 62, 72, 0xF5ECD9, ink, 5);
    text(screen, "L KB", &lv_font_montserrat_12, ink, 22, 47);
    text(screen, "R KB", &lv_font_montserrat_12, ink, 220, 47);
    panel(screen, 20, 65, 42, 20, 0xD8CFBC, ink, 2);
    panel(screen, 218, 65, 42, 20, 0xD8CFBC, ink, 2);
    text(screen, "|||", &lv_font_montserrat_16, ink, 27, 65);
    text(screen, "|||", &lv_font_montserrat_16, ink, 225, 65);
    left_battery_value = text(screen, "--%", &lv_font_montserrat_16, ink, 22, 89);
    right_battery_value = text(screen, "--%", &lv_font_montserrat_16, ink, 220, 89);

    panel(screen, 80, 33, 120, 80, 0xF8F0DF, ink, 5);
    panel(screen, 84, 38, 112, 64, 0xD9CFBB, shadow, 2);
    text(screen, "LAYER", &lv_font_montserrat_12, ink, 118, 48);
    layer_value = text(screen, "BASE", &lv_font_montserrat_28, ink, 101, 64);

    panel(screen, 79, 116, 27, 28, 0xF8F0DF, ink, 3);
    panel(screen, 108, 116, 27, 28, 0xF8F0DF, ink, 3);
    panel(screen, 137, 116, 27, 28, 0xF8F0DF, ink, 3);
    panel(screen, 166, 116, 27, 28, 0xF8F0DF, ink, 3);
    text(screen, "Ctrl", &lv_font_montserrat_12, ink, 80, 121);
    text(screen, "Alt", &lv_font_montserrat_12, ink, 114, 121);
    text(screen, "Gui", &lv_font_montserrat_12, ink, 143, 121);
    text(screen, "Sft", &lv_font_montserrat_12, ink, 173, 121);

    panel(screen, 10, 154, 260, 58, 0xF6EDDA, ink, 5);
    text(screen, "CH ALL", &lv_font_montserrat_12, ink, 19, 164);
    text(screen, "BLE", &lv_font_montserrat_12, ink, 20, 183);
    panel(screen, 43, 183, 26, 10, ink, ink, 1);
    text(screen, "WPM", &lv_font_montserrat_12, ink, 210, 164);
    wpm_value = text(screen, "0", &lv_font_montserrat_28, ink, 224, 177);
    divider(screen, 78, 160, 2, 45);
    divider(screen, 198, 160, 2, 45);
    panel(screen, 95, 164, 48, 39, 0xDDD3BE, ink, 3);
    text(screen, "[: ]", &lv_font_montserrat_20, ink, 103, 171);
    text(screen, "Type Happy.", &lv_font_montserrat_12, ink, 91, 218);
    text(screen, "13:37", &lv_font_montserrat_12, ink, 226, 218);
}

void scanner_retro_workstation_theme_destroy(void) {
    layer_value = NULL;
    wpm_value = NULL;
    left_battery_value = NULL;
    right_battery_value = NULL;
}

void scanner_retro_workstation_theme_update(const struct prospector_keyboard_data *data) {
    if (!data || !layer_value) return;
    lv_label_set_text(layer_value, data->current_layer_name[0] ? data->current_layer_name : "BASE");
    char value[8];
    snprintk(value, sizeof(value), "%u", data->wpm_value);
    lv_label_set_text(wpm_value, value);
    snprintk(value, sizeof(value), "%u%%", data->battery_level);
    lv_label_set_text(left_battery_value, value);
    snprintk(value, sizeof(value), "%u%%", data->peripheral_battery[0]);
    lv_label_set_text(right_battery_value, value);
}
