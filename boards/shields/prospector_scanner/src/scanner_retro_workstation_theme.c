#include "scanner_retro_workstation_theme.h"
#include "system_settings_widget.h"
#include "mac_modifier_images.h"

#include <lvgl.h>
#include <zephyr/sys/printk.h>

/* Compact LVGL redraw of the supplied 280 x 240 Macintosh reference. */
LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(impact_48);

#define PAPER 0xEDECE6
#define FACE 0xF7F6F0
#define INK 0x1B1A18
#define SHADE 0x85847E

static lv_obj_t *layer_value, *wpm_value;
static lv_obj_t *left_value, *right_value, *channel_value;
static lv_obj_t *modifier_dot[4];
static lv_obj_t *modifier_icon[4];

static void plain(lv_obj_t *obj, uint32_t color) {
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t *box(lv_obj_t *parent, int x, int y, int w, int h,
                     uint32_t background, uint32_t border, int width) {
    lv_obj_t *obj = lv_obj_create(parent);
    plain(obj, background);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_border_color(obj, lv_color_hex(border), 0);
    lv_obj_set_style_border_width(obj, width, 0);
    return obj;
}

static lv_obj_t *raised(lv_obj_t *parent, int x, int y, int w, int h) {
    box(parent, x + 3, y + 4, w, h, SHADE, INK, 1);
    return box(parent, x, y, w, h, FACE, INK, 2);
}

static lv_obj_t *label(lv_obj_t *parent, const char *value, const lv_font_t *font,
                       int x, int y, int width, lv_text_align_t align) {
    lv_obj_t *obj = lv_label_create(parent);
    lv_label_set_text(obj, value);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(INK), 0);
    lv_obj_set_width(obj, width);
    lv_obj_set_style_text_align(obj, align, 0);
    lv_obj_set_pos(obj, x, y);
    return obj;
}

static void rule(lv_obj_t *parent, int x, int y, int width) {
    box(parent, x, y, width, 2, SHADE, SHADE, 0);
}

static void battery_icon(lv_obj_t *parent, int x, int y, int cells) {
    box(parent, x, y, 38, 18, FACE, INK, 2);
    box(parent, x + 38, y + 5, 3, 8, INK, INK, 0);
    for (int i = 0; i < 3; i++)
        box(parent, x + 4 + i * 10, y + 4, 7, 10, i < cells ? INK : FACE, INK, 0);
}

static void side_card(lv_obj_t *screen, int x, const char *title, lv_obj_t **value) {
    lv_obj_t *card = raised(screen, x, 24, 61, 78);
    label(card, title, &lv_font_unscii_8, 4, 7, 53, LV_TEXT_ALIGN_CENTER);
    rule(card, 5, 21, 51);
    battery_icon(card, 10, 29, 3);
    *value = label(card, "--", &impact_20, 3, 50, 55, LV_TEXT_ALIGN_CENTER);
}

static void modifier(lv_obj_t *screen, int x, const char *name, int slot) {
    (void)name;
    lv_obj_t *key = raised(screen, x, 111, 34, 28);
    modifier_dot[slot] = key;
    modifier_icon[slot] = lv_image_create(key);
    lv_image_set_src(modifier_icon[slot], mac_modifier_images[slot]);
    lv_obj_set_pos(modifier_icon[slot], 8, 5);
    lv_obj_set_style_image_recolor(modifier_icon[slot], lv_color_hex(INK), 0);
    lv_obj_set_style_image_recolor_opa(modifier_icon[slot], LV_OPA_COVER, 0);
}

static void macintosh(lv_obj_t *screen) {
    /* Compact Macintosh silhouette: bezel, Happy Mac, floppy slot and foot. */
    lv_obj_t *mac = raised(screen, 119, 149, 39, 44);
    box(mac, 3, 3, 33, 31, PAPER, SHADE, 1);
    box(mac, 6, 6, 27, 24, FACE, INK, 2);
    box(mac, 11, 11, 2, 4, INK, INK, 0);
    box(mac, 26, 11, 2, 4, INK, INK, 0);
    box(mac, 19, 11, 2, 9, INK, INK, 0);
    box(mac, 16, 18, 5, 2, INK, INK, 0);
    box(mac, 12, 22, 2, 2, INK, INK, 0);
    box(mac, 25, 22, 2, 2, INK, INK, 0);
    box(mac, 14, 24, 11, 2, INK, INK, 0);
    box(mac, 7, 35, 3, 2, INK, INK, 0);
    box(mac, 21, 35, 11, 2, INK, INK, 0);
    box(mac, 5, 40, 29, 3, SHADE, INK, 1);
    for (int side = 0; side < 2; side++) {
        int x = side ? 162 : 111;
        box(screen, x, 168, 4, 2, INK, INK, 0);
        box(screen, x + (side ? 2 : -2), 156, 2, 3, INK, INK, 0);
        box(screen, x + (side ? 2 : -2), 180, 2, 3, INK, INK, 0);
    }
}

void scanner_retro_workstation_theme_create(lv_obj_t *screen) {
    plain(screen, PAPER);
    lv_obj_set_size(screen, 280, 240);

    /* The 280x240 panel is visibly masked by a round bezel.  Keep all
     * content inside a 10px horizontal and 7px vertical safe inset. */

    side_card(screen, 12, "L KB", &left_value);
    side_card(screen, 207, "R KB", &right_value);

    lv_obj_t *layer = raised(screen, 79, 18, 122, 84);
    box(layer, 6, 5, 10, 8, FACE, INK, 1);
    rule(layer, 24, 5, 88); rule(layer, 24, 10, 88);
    rule(layer, 5, 17, 112);
    label(layer, "LAYER", &lv_font_unscii_8, 18, 21, 86, LV_TEXT_ALIGN_CENTER);
    /* Keep the layer name within its own row; the former 48px font overran
     * both the label above and the modifier row on the real round display. */
    layer_value = label(layer, "BASE", &impact_48, 6, 31, 110, LV_TEXT_ALIGN_CENTER);

    modifier(screen, 72, "Ctrl", 0); modifier(screen, 108, "Alt", 1);
    modifier(screen, 144, "Gui", 2); modifier(screen, 180, "Sft", 3);

    lv_obj_t *dongle = raised(screen, 12, 148, 96, 47);
    label(dongle, "BLE CH", &lv_font_unscii_8, 7, 5, 80, LV_TEXT_ALIGN_CENTER);
    rule(dongle, 6, 19, 83);
    channel_value = label(dongle, "ALL", &impact_20, 6, 22, 83, LV_TEXT_ALIGN_CENTER);
    char channel[8];
    uint8_t selected = scanner_get_runtime_channel();
    if (selected) snprintk(channel, sizeof(channel), "%u", selected);
    else snprintk(channel, sizeof(channel), "ALL");
    lv_label_set_text(channel_value, channel);

    macintosh(screen);

    lv_obj_t *wpm = raised(screen, 173, 148, 95, 47);
    label(wpm, "WPM", &lv_font_unscii_8, 7, 5, 39, LV_TEXT_ALIGN_LEFT);
    rule(wpm, 5, 23, 85);
    label(wpm, "|||", &lv_font_unscii_8, 7, 29, 25, LV_TEXT_ALIGN_LEFT);
    wpm_value = label(wpm, "0", &impact_20, 45, 24, 44, LV_TEXT_ALIGN_CENTER);

    rule(screen, 10, 201, 260);
    label(screen, "Type Happy.", &lv_font_unscii_8, 15, 206, 110, LV_TEXT_ALIGN_LEFT);
    label(screen, "13:37", &lv_font_unscii_8, 217, 206, 48, LV_TEXT_ALIGN_RIGHT);
}

void scanner_retro_workstation_theme_destroy(void) {
    layer_value = wpm_value = left_value = right_value = channel_value = NULL;
    for (int i = 0; i < 4; i++) modifier_dot[i] = modifier_icon[i] = NULL;
}

void scanner_retro_workstation_theme_update(const struct prospector_keyboard_data *data) {
    if (!data || !layer_value) return;
    char value[8];
    lv_label_set_text(layer_value, data->current_layer_name[0] ? data->current_layer_name : "BASE");
    snprintk(value, sizeof(value), "%u", data->wpm_value); lv_label_set_text(wpm_value, value);
    snprintk(value, sizeof(value), "%u", data->battery_level);
    lv_label_set_text(left_value, value);
    snprintk(value, sizeof(value), "%u", data->peripheral_battery[0]); lv_label_set_text(right_value, value);
    uint8_t selected = scanner_get_runtime_channel();
    if (selected) snprintk(value, sizeof(value), "%u", selected);
    else snprintk(value, sizeof(value), "ALL");
    lv_label_set_text(channel_value, value);
    const uint8_t masks[4] = {0x11, 0x44, 0x88, 0x22};
    for (int i = 0; i < 4; i++) if (modifier_dot[i]) {
        bool pressed = (data->modifier_flags & masks[i]) != 0;
        lv_obj_set_style_bg_color(modifier_dot[i],
            lv_color_hex(pressed ? INK : FACE), 0);
        lv_obj_set_style_image_recolor(modifier_icon[i],
            lv_color_hex(pressed ? FACE : INK), 0);
    }
}
