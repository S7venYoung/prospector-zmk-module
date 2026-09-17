/* Scanner adapter for the receiver's canonical Codex dashboard.
 * Geometry, colors and Impact font assets deliberately match themes/codex-status. */
#include "scanner_codex2_theme.h"
#include "scanner_host_status.h"
#include <lvgl.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

LV_FONT_DECLARE(impact_16);
LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(impact_48);
LV_FONT_DECLARE(impact_56);

#define INK 0x101411
#define YELLOW 0xFFBF18
#define PAPER 0xF3EEE5
#define MUTED 0x7C847D
#define GREEN 0x58E85D
#define RED 0xE23B2E

static lv_obj_t *layer_value, *wpm_value, *left_value, *right_value;
static lv_obj_t *left_fill, *right_fill, *left_dot, *right_dot;
static lv_obj_t *host_left, *host_tokens, *host_week; static lv_obj_t *week_segments[6];

static void plain(lv_obj_t *o, uint32_t color) {
    lv_obj_remove_style_all(o);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_radius(o, 0, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}
static lv_obj_t *text(lv_obj_t *parent, const char *value, const lv_font_t *font, uint32_t color) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, value);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}
static lv_obj_t *panel(lv_obj_t *screen, int x, int y, int w, int h, uint32_t color,
                       uint32_t border, int radius) {
    lv_obj_t *o = lv_obj_create(screen);
    plain(o, color);
    lv_obj_set_size(o, w, h);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(border), 0);
    return o;
}
static void battery_card(lv_obj_t *screen, int x, const char *side,
                         lv_obj_t **value, lv_obj_t **fill, lv_obj_t **dot) {
    lv_obj_t *card = panel(screen, x, 190, 122, 40, INK, MUTED, 12);
    lv_obj_t *side_label = text(card, side, &impact_20, MUTED);
    lv_obj_set_pos(side_label, 8, 7);
    *value = text(card, "--%", &impact_20, PAPER);
    lv_obj_set_pos(*value, 28, 6);
    *dot = lv_obj_create(card);
    plain(*dot, GREEN);
    lv_obj_set_size(*dot, 10, 10);
    lv_obj_set_pos(*dot, 104, 14);
    lv_obj_set_style_radius(*dot, LV_RADIUS_CIRCLE, 0);
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
}
static void center(lv_obj_t *o, int x, int y, int width) {
    lv_obj_set_pos(o, x, y);
    lv_obj_set_width(o, width);
    lv_obj_set_style_text_align(o, LV_TEXT_ALIGN_CENTER, 0);
}
void scanner_codex2_theme_create(lv_obj_t *screen) {
    plain(screen, INK);
    lv_obj_set_size(screen, 280, 240);
    lv_obj_set_style_radius(screen, 24, 0);

    lv_obj_t *brand = text(screen, "CODEX", &impact_20, YELLOW);
    lv_obj_set_pos(brand, 18, 8);
    lv_obj_t *suffix = text(screen, "// SOFLE", &impact_20, PAPER);
    lv_obj_set_pos(suffix, 83, 8);
    lv_obj_t *usb = text(screen, "USB", &impact_16, PAPER);
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
    lv_obj_t *used_caption = text(used, "5 HOUR LEFT", &impact_16, PAPER);
    lv_obj_set_pos(used_caption, 8, 8);
    lv_obj_t *used_marks = text(used, "///", &impact_16, YELLOW);
    lv_obj_set_pos(used_marks, 101, 8);
    host_left = text(used, "--%", &impact_48, YELLOW);
    center(host_left, 4, 27, 114);
    lv_obj_t *week_rule = lv_obj_create(used);
    plain(week_rule, MUTED); lv_obj_set_size(week_rule, 104, 1); lv_obj_set_pos(week_rule, 8, 70);
    lv_obj_set_style_bg_opa(week_rule, LV_OPA_60, 0);
    lv_obj_t *week_caption = text(used, "7 DAY LEFT", &impact_16, PAPER);
    lv_obj_set_pos(week_caption, 8, 74);
    host_week = text(used, "--%", &impact_20, YELLOW);
    lv_obj_set_pos(host_week, 73, 73);
    for (int i = 0; i < 6; i++) {
        week_segments[i] = lv_obj_create(used);
        plain(week_segments[i], MUTED);
        lv_obj_set_size(week_segments[i], 15, 5);
        lv_obj_set_pos(week_segments[i], 8 + i * 17, 88);
        lv_obj_set_style_radius(week_segments[i], 1, 0);
    }

    lv_obj_t *tokens = panel(screen, 144, 48, 122, 96, INK, YELLOW, 14);
    lv_obj_t *tokens_caption = text(tokens, "TODAY TOTAL", &impact_16, PAPER);
    lv_obj_set_pos(tokens_caption, 8, 8);
    lv_obj_t *tokens_marks = text(tokens, "///", &impact_16, YELLOW);
    lv_obj_set_pos(tokens_marks, 101, 8);
    host_tokens = text(tokens, "--", &impact_48, PAPER);
    center(host_tokens, 2, 38, 118);

    lv_obj_t *bar = panel(screen, 14, 150, 252, 32, INK, MUTED, 10);
    lv_obj_t *layer_caption = text(bar, "LAYER", &impact_16, MUTED);
    lv_obj_set_pos(layer_caption, 10, 8);
    layer_value = text(bar, "BASE", &impact_20, YELLOW);
    lv_obj_set_pos(layer_value, 76, 3);
    lv_obj_set_width(layer_value, 65);
    lv_obj_t *divider = lv_obj_create(bar);
    plain(divider, MUTED);
    lv_obj_set_size(divider, 1, 22);
    lv_obj_set_pos(divider, 136, 5);
    lv_obj_t *wpm_caption = text(bar, "WPM", &impact_16, MUTED);
    lv_obj_set_pos(wpm_caption, 164, 8);
    wpm_value = text(bar, "0", &impact_20, YELLOW);
    lv_obj_set_pos(wpm_value, 214, 3);
    lv_obj_set_width(wpm_value, 30);
    lv_obj_set_style_text_align(wpm_value, LV_TEXT_ALIGN_RIGHT, 0);
    battery_card(screen, 14, "L", &left_value, &left_fill, &left_dot);
    battery_card(screen, 144, "R", &right_value, &right_fill, &right_dot);

    /* The scanner keeps host metrics across page changes.  Repaint that cache
     * immediately when this view is recreated instead of waiting for a new
     * serial CODEX message from macOS. */
    scanner_codex2_theme_host_update(scanner_host_status_get());
}
void scanner_codex2_theme_destroy(void) {
    layer_value = wpm_value = left_value = right_value = NULL;
    left_fill = right_fill = left_dot = right_dot = NULL;
    host_left = host_tokens = host_week = NULL;
    for (int i = 0; i < 6; i++) week_segments[i] = NULL;
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
void scanner_codex2_theme_host_update(const struct scanner_host_status *status) {
    if (!status || !host_left || !host_tokens || !host_week || !status->codex_available) return;
    char left[8], tokens[16];
    snprintk(left, sizeof(left), "%u%%", status->codex_left_percent);
    if (status->codex_total_tokens >= 1000000U)
        snprintk(tokens, sizeof(tokens), "%u.%uM", status->codex_total_tokens / 1000000U,
                 (status->codex_total_tokens % 1000000U) / 100000U);
    else if (status->codex_total_tokens >= 1000U)
        snprintk(tokens, sizeof(tokens), "%u.%uK", status->codex_total_tokens / 1000U,
                 (status->codex_total_tokens % 1000U) / 100U);
    else snprintk(tokens, sizeof(tokens), "%u", status->codex_total_tokens);
    lv_label_set_text(host_left, left);
    lv_label_set_text(host_tokens, tokens);
    if (status->codex_week_available) {
        char week[8];
        snprintk(week, sizeof(week), "%u%%", status->codex_week_left_percent);
        lv_label_set_text(host_week, week);
        const uint8_t active = (status->codex_week_left_percent * 6U + 99U) / 100U;
        for (int i = 0; i < 6; i++) {
            if (week_segments[i]) lv_obj_set_style_bg_color(week_segments[i],
                lv_color_hex(i < active ? YELLOW : MUTED), 0);
        }
    } else {
        lv_label_set_text(host_week, "--%");
        for (int i = 0; i < 6; i++) if (week_segments[i])
            lv_obj_set_style_bg_color(week_segments[i], lv_color_hex(MUTED), 0);
    }
}
void scanner_codex2_theme_update(const struct prospector_keyboard_data *data) {
    if (!data || !layer_value) return;
    lv_label_set_text(layer_value, data->current_layer_name[0] ? data->current_layer_name : "BASE");
    char wpm[8];
    snprintk(wpm, sizeof(wpm), "%u", data->wpm_value);
    lv_label_set_text(wpm_value, wpm);
    set_battery(left_value, left_fill, left_dot, data->battery_level);
    set_battery(right_value, right_fill, right_dot, data->peripheral_battery[0]);
}