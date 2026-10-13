#include "scanner_light_theme.h"
#include "mac_modifier_images.h"
#include "system_settings_widget.h"
#include <zephyr/sys/printk.h>

#define BG 0xFAFAFA
#define INK 0x202124
#define MUTED 0x808080
#define LINE 0xDDDDDD
#define BLUE 0x167DC5
#define SELECTED 0xDCEBFC
#define GREEN 0x38AF63
static lv_obj_t *layer, *wpm, *channel, *transport, *profile;
static lv_obj_t *battery_value[2], *battery_fill[2], *keys[4], *icons[4], *bars[8];
static uint8_t history[8];
static uint8_t last_modifiers = 0xFF;
static const uint8_t bluetooth_data[140] = {0,0,0,0,255,255,0,0,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,0,0,255,255,0,0,255,255,0,0,255,255,255,255,0,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,0,0,255,255,0,0,255,255,255,255,0,0,255,255,0,0,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,255,255,0,0,0,0,0,0,255,255,0,0,0,0,0,0,0,0,255,255,0,0,0,0};
static const lv_image_dsc_t bluetooth_image = {
    .header = {.magic=LV_IMAGE_HEADER_MAGIC, .cf=LV_COLOR_FORMAT_A8, .w=10, .h=14, .stride=10},
    .data_size=140, .data=bluetooth_data
};

static lv_obj_t *box(lv_obj_t *p, int x, int y, int w, int h, uint32_t color, int radius) {
    lv_obj_t *o = lv_obj_create(p);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y); lv_obj_set_size(o, w, h);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    return o;
}
static lv_obj_t *text(lv_obj_t *p, const char *s, const lv_font_t *font,
                       uint32_t color, int x, int y, int width, lv_text_align_t align) {
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_text(o, s); lv_label_set_long_mode(o, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_set_style_text_font(o, font, 0);
    lv_obj_set_style_text_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(o, align, 0);
    lv_obj_set_width(o, width); lv_obj_set_pos(o, x, y);
    return o;
}
static void battery(lv_obj_t *s, int x, int slot) {
    lv_obj_t *body = box(s, x, 172, 40, 20, BG, 4);
    lv_obj_set_style_border_width(body, 2, 0);
    lv_obj_set_style_border_color(body, lv_color_hex(INK), 0);
    box(s, x+40, 178, 3, 8, INK, 1);
    battery_fill[slot] = box(body, 4, 4, 32, 12, GREEN, 1);
    lv_obj_add_flag(battery_fill[slot], LV_OBJ_FLAG_HIDDEN);
    battery_value[slot] = text(s, "--", &lv_font_montserrat_20, INK,
                                x-7, 199, 57, LV_TEXT_ALIGN_CENTER);
}
static void refresh_channel(void) {
    uint8_t selected = scanner_get_runtime_channel();
    char value[12];
    if (selected >= 10) snprintk(value, sizeof(value), "CH ALL");
    else snprintk(value, sizeof(value), "CH %u", selected);
    lv_label_set_text(channel, value);
}
void scanner_light_theme_create(lv_obj_t *s) {
    lv_obj_remove_style_all(s);
    lv_obj_remove_flag(s, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(s, lv_color_hex(BG), 0);
    lv_obj_set_style_bg_opa(s, LV_OPA_COVER, 0);
    /* 280x240 panel: leave extra clearance at the four masked corners. */
    text(s, "ZMK", &lv_font_montserrat_16, MUTED, 16, 28, 44, LV_TEXT_ALIGN_LEFT);
    box(s, 66, 24, 1, 30, LINE, 0);
    lv_obj_t *bt = lv_image_create(s);
    lv_image_set_src(bt, &bluetooth_image); lv_obj_set_pos(bt, 75, 31);
    lv_obj_set_style_image_recolor(bt, lv_color_hex(BLUE), 0);
    lv_obj_set_style_image_recolor_opa(bt, LV_OPA_COVER, 0);
    transport = text(s, "BLE", &lv_font_montserrat_16, INK, 89, 28, 35, LV_TEXT_ALIGN_CENTER);
    lv_obj_t *badge = box(s, 126, 24, 29, 28, SELECTED, 8);
    profile = text(badge, "-", &lv_font_montserrat_16, BLUE, 0, 4, 29, LV_TEXT_ALIGN_CENTER);
    box(s, 165, 24, 1, 30, LINE, 0);
    channel = text(s, "CH ALL", &lv_font_montserrat_16, BLUE, 176, 28, 88, LV_TEXT_ALIGN_CENTER);
    refresh_channel();

    text(s, "LAYER", &lv_font_montserrat_12, MUTED, 14, 75, 100, LV_TEXT_ALIGN_LEFT);
    layer = text(s, "BASE", &lv_font_montserrat_36, INK, 12, 92, 123, LV_TEXT_ALIGN_LEFT);
    box(s, 137, 79, 1, 53, LINE, 0);
    for (int k = 0; k < 4; k++) {
        keys[k] = box(s, 148+k*28, 94, 25, 31, 0xEEEEEE, 5);
        icons[k] = lv_image_create(keys[k]);
        lv_image_set_src(icons[k], mac_modifier_images[k]);
        lv_obj_set_pos(icons[k], 3, 6);
        lv_obj_set_style_image_recolor(icons[k], lv_color_hex(MUTED), 0);
        lv_obj_set_style_image_recolor_opa(icons[k], LV_OPA_COVER, 0);
    }
    box(s, 14, 146, 252, 1, LINE, 0);
    box(s, 93, 156, 1, 65, LINE, 0);
    box(s, 186, 156, 1, 65, LINE, 0);
    text(s, "LEFT KB", &lv_font_montserrat_12, MUTED, 14, 153,   70, LV_TEXT_ALIGN_CENTER);
    text(s, "WPM", &lv_font_montserrat_12, MUTED, 100, 153, 80, LV_TEXT_ALIGN_CENTER);
    text(s, "RIGHT KB", &lv_font_montserrat_12, MUTED, 193, 153, 73, LV_TEXT_ALIGN_CENTER);
    battery(s, 29, 0); battery(s, 209, 1);
    wpm = text(s, "0", &lv_font_montserrat_36, INK, 99, 171, 82, LV_TEXT_ALIGN_CENTER);
    for (int i = 0; i < 8; i++) {
        history[i] = 0;
        bars[i] = box(s, 111+i*7, 217, 4, 2, 0xCCCCCC, 2);
    }
    last_modifiers = 0xFF;
}
void scanner_light_theme_destroy(void) {
    layer = wpm = channel = transport = profile = NULL;
    for (int i = 0; i < 2; i++) battery_value[i] = battery_fill[i] = NULL;
    for (int i = 0; i < 4; i++) keys[i] = icons[i] = NULL;
    for (int i = 0; i < 8; i++) bars[i] = NULL;
}
void scanner_light_theme_update(const struct prospector_keyboard_data *d) {
    if (!d || !layer) return;
    char value[16];
    lv_label_set_text(layer, d->current_layer_name[0] ? d->current_layer_name : "BASE");
    lv_label_set_text(transport, d->usb_connected ? "USB" : "BLE");
    snprintk(value, sizeof(value), "%u", d->profile_slot+1U);
    lv_label_set_text(profile, value);
    refresh_channel();
    snprintk(value, sizeof(value), "%u", d->wpm_value); lv_label_set_text(wpm, value);
    const uint8_t levels[2] = {d->battery_level, d->peripheral_battery[0]};
    for (int i = 0; i < 2; i++) {
        if (levels[i] > 100) {
            lv_label_set_text(battery_value[i], "--");
            lv_obj_add_flag(battery_fill[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            snprintk(value, sizeof(value), "%u%%", levels[i]);
            lv_label_set_text(battery_value[i], value);
            if (levels[i] == 0) lv_obj_add_flag(battery_fill[i], LV_OBJ_FLAG_HIDDEN);
            else {
                lv_obj_remove_flag(battery_fill[i], LV_OBJ_FLAG_HIDDEN);
                lv_obj_set_width(battery_fill[i], (32U*levels[i]+99U)/100U);
                lv_obj_set_style_bg_color(battery_fill[i], lv_color_hex(levels[i] <= 15 ? 0xDD5555 : GREEN), 0);
            }
        }
    }
    if (last_modifiers != d->modifier_flags) {
        const uint8_t masks[4] = {0x11,0x44,0x88,0x22};
        for (int i = 0; i < 4; i++) {
            bool pressed = (d->modifier_flags & masks[i]) != 0;
            lv_obj_set_style_bg_color(keys[i], lv_color_hex(pressed ? SELECTED : 0xEEEEEE), 0);
            lv_obj_set_style_image_recolor(icons[i], lv_color_hex(pressed ? BLUE : MUTED), 0);
        }
        last_modifiers = d->modifier_flags;
    }
    for (int i = 0; i < 7; i++) history[i] = history[i+1];
    history[7] = d->wpm_value;
    for (int i = 0; i < 8; i++) {
        int h = 2 + (history[i] > 120 ? 120 : history[i])/10;
        lv_obj_set_height(bars[i], h); lv_obj_set_y(bars[i], 219-h);
    }
}
