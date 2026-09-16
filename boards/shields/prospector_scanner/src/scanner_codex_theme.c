#include "scanner_codex_theme.h"
#include "scanner_host_status.h"
#include <lvgl.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#define INK 0x101411
#define YELLOW 0xFFBF18
#define PAPER 0xF3EEE5
#define MUTED 0x7C847D
#define GREEN 0x58E85D
#define RED 0xE23B2E

static lv_obj_t *layer_value, *wpm_value, *left_value, *right_value;
static lv_obj_t *left_fill, *right_fill, *left_dot, *right_dot;
static lv_obj_t *host_left, *host_tokens;

static void plain(lv_obj_t *o, uint32_t color) {
    lv_obj_remove_style_all(o);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_radius(o, 0, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static lv_obj_t *text(lv_obj_t *parent, const char *value, const lv_font_t *font,
                      uint32_t color) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, value);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}

static lv_obj_t *panel(lv_obj_t *screen, int x, int y, int w, int h,
                       uint32_t color, uint32_t border, int radius) {
    lv_obj_t *o = lv_obj_create(screen);
    plain(o, color);
    lv_obj_set_size(o, w, h);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_set_style_border_width(o, 2, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(border), 0);
    return o;
}

static void battery_card(lv_obj_t *screen, int x, const char *side,
                         lv_obj_t **value, lv_obj_t **fill, lv_obj_t **dot) {
    lv_obj_t *card = panel(screen, x, 190, 122, 40, INK, MUTED, 12);
    lv_obj_t *side_label = text(card, side, &lv_font_montserrat_20, MUTED);
    lv_obj_set_pos(side_label, 8, 7);
    *value = text(card, "--%", &lv_font_montserrat_20, PAPER);
    lv_obj_set_pos(*value, 28, 6);

    lv_obj_t *track = lv_obj_create(card);
    plain(track, PAPER);
    lv_obj_set_size(track, 38, 14);
    lv_obj_set_pos(track, 63, 12);
    lv_obj_set_style_radius(track, 3, 0);
    lv_obj_set_style_border_width(track, 1, 0);
    lv_obj_set_style_border_color(track, lv_color_hex(PAPER), 0);

    *fill = lv_obj_create(card);
    plain(*fill, GREEN);
    lv_obj_set_size(*fill, 4, 9);
    lv_obj_set_pos(*fill, 65, 14);
    lv_obj_set_style_radius(*fill, 2, 0);

    *dot = lv_obj_create(card);
    plain(*dot, GREEN);
    lv_obj_set_size(*dot, 10, 10);
    lv_obj_set_pos(*dot, 104, 14);
    lv_obj_set_style_radius(*dot, LV_RADIUS_CIRCLE, 0);
}

static void value_center(lv_obj_t *value, int x, int y, int width) {
    lv_obj_set_pos(value, x, y);
    lv_obj_set_width(value, width);
    lv_obj_set_style_text_align(value, LV_TEXT_ALIGN_CENTER, 0);
}

void scanner_codex_theme_create(lv_obj_t *screen) {
    plain(screen, INK);
    lv_obj_set_size(screen, 280, 240);
    /* Keep all artwork in the 1.69-inch Waveshare rounded-glass safe area. */
    lv_obj_set_style_radius(screen, 24, 0);

    lv_obj_t *brand = text(screen, "CODEX", &lv_font_montserrat_20, YELLOW);
    lv_obj_set_pos(brand, 18, 8);
    lv_obj_t *suffix = text(screen, "// SOFLE", &lv_font_montserrat_20, PAPER);
    lv_obj_set_pos(suffix, 83, 8);
    lv_obj_t *usb = text(screen, "USB", &lv_font_montserrat_16, PAPER);
    lv_obj_set_pos(usb, 226, 8);
    lv_obj_t *usb_dot = lv_obj_create(screen);
    plain(usb_dot, GREEN);
    lv_obj_set_size(usb_dot, 10, 10);
    lv_obj_set_pos(usb_dot, 258, 14);
    lv_obj_set_style_radius(usb_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *rule = lv_obj_create(screen);
    plain(rule, YELLOW);
    lv_obj_set_size(rule, 244, 2);
    lv_obj_set_pos(rule, 18, 37);

    lv_obj_t *used = panel(screen, 14, 48, 122, 96, INK, YELLOW, 14);
    lv_obj_t *used_caption = text(used, "5 HOUR LEFT", &lv_font_montserrat_12, PAPER);
    lv_obj_set_pos(used_caption, 8, 9);
    lv_obj_t *used_marks = text(used, "///", &lv_font_montserrat_16, YELLOW);
    lv_obj_set_pos(used_marks, 94, 8);
    host_left = text(used, "--%", &lv_font_montserrat_36, YELLOW);
    value_center(host_left, 4, 31, 114);

    lv_obj_t *tokens = panel(screen, 144, 48, 122, 96, INK, YELLOW, 14);
    lv_obj_t *tokens_caption = text(tokens, "TODAY TOTAL", &lv_font_montserrat_12, PAPER);
    lv_obj_set_pos(tokens_caption, 8, 9);
    lv_obj_t *tokens_marks = text(tokens, "///", &lv_font_montserrat_16, YELLOW);
    lv_obj_set_pos(tokens_marks, 94, 8);
    host_tokens = text(tokens, "--", &lv_font_montserrat_28, PAPER);
    value_center(host_tokens, 2, 38, 118);

    lv_obj_t *bar = panel(screen, 14, 150, 252, 32, INK, MUTED, 10);
    lv_obj_t *layer_caption = text(bar, "LAYER", &lv_font_montserrat_16, MUTED);
    lv_obj_set_pos(layer_caption, 10, 8);
    layer_value = text(bar, "BASE", &lv_font_montserrat_20, YELLOW);
    value_center(layer_value, 76, 3, 60);
    lv_obj_t *divider = lv_obj_create(bar);
    plain(divider, MUTED);
    lv_obj_set_size(divider, 1, 22);
    lv_obj_set_pos(divider, 136, 5);
    lv_obj_t *wpm_caption = text(bar, "WPM", &lv_font_montserrat_16, MUTED);
    lv_obj_set_pos(wpm_caption, 164, 8);
    wpm_value = text(bar, "0", &lv_font_montserrat_20, YELLOW);
    value_center(wpm_value, 214, 3, 30);

    battery_card(screen, 14, "L", &left_value, &left_fill, &left_dot);
    battery_card(screen, 144, "R", &right_value, &right_fill, &right_dot);
}

void scanner_codex_theme_destroy(void) {
    layer_value = wpm_value = left_value = right_value = NULL;
    left_fill = right_fill = left_dot = right_dot = NULL;
    host_left = host_tokens = NULL;
}

static void set_battery(lv_obj_t *value, lv_obj_t *fill, lv_obj_t *dot, uint8_t level) {
    if (!value || !fill || !dot) return;
    char buffer[8];
    snprintk(buffer, sizeof(buffer), "%u%%", level);
    lv_label_set_text(value, buffer);
    lv_obj_set_width(fill, MAX(2, (int32_t)level * 34 / 100));
    lv_obj_set_style_bg_color(fill, lv_color_hex(level < 20 ? RED : GREEN), 0);
    lv_obj_set_style_bg_color(dot, lv_color_hex(level ? GREEN : RED), 0);
}

void scanner_codex_theme_host_update(const struct scanner_host_status *status) {
    if (!status || !host_left || !host_tokens || !status->codex_available) return;
    char left[8], tokens[16];
    snprintk(left, sizeof(left), "%u%%", status->codex_left_percent);
    if (status->codex_total_tokens >= 1000000U) {
        snprintk(tokens, sizeof(tokens), "%u.%uM", status->codex_total_tokens / 1000000U,
                 (status->codex_total_tokens % 1000000U) / 100000U);
    } else if (status->codex_total_tokens >= 1000U) {
        snprintk(tokens, sizeof(tokens), "%u.%uK", status->codex_total_tokens / 1000U,
                 (status->codex_total_tokens % 1000U) / 100U);
    } else {
        snprintk(tokens, sizeof(tokens), "%u", status->codex_total_tokens);
    }
    lv_label_set_text(host_left, left);
    lv_label_set_text(host_tokens, tokens);
}

void scanner_codex_theme_update(const struct prospector_keyboard_data *data) {
    if (!data || !layer_value) return;
    lv_label_set_text(layer_value,
                      data->current_layer_name[0] ? data->current_layer_name : "BASE");
    char wpm[8];
    snprintk(wpm, sizeof(wpm), "%u", data->wpm_value);
    lv_label_set_text(wpm_value, wpm);
    set_battery(left_value, left_fill, left_dot, data->battery_level);
    set_battery(right_value, right_fill, right_dot, data->peripheral_battery[0]);
}
