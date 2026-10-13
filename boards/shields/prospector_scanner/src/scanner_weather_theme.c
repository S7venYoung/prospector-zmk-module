#include "scanner_weather_theme.h"
#include "mac_modifier_images.h"
#include <lvgl.h>
#include <zephyr/sys/printk.h>

LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(impact_48);
#define GOLD 0xF2CB55
#define DARK 0x171A1C
#define WHITE 0xF7F2DE
static lv_obj_t *wpm, *lbat, *rbat, *keys[4];
static lv_obj_t *icons[4];

static lv_obj_t *box(lv_obj_t *p, int x, int y, int w, int h, uint32_t c, int radius) {
    lv_obj_t *o = lv_obj_create(p);
    lv_obj_remove_style_all(o);
    lv_obj_set_pos(o, x, y); lv_obj_set_size(o, w, h);
    lv_obj_set_style_bg_color(o, lv_color_hex(c), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return o;
}
static lv_obj_t *text(lv_obj_t *p, const char *s, const lv_font_t *f,
                      uint32_t c, int x, int y, int w) {
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_text(o, s); lv_label_set_long_mode(o, LV_LABEL_LONG_MODE_CLIP);
    lv_obj_set_style_text_font(o, f, 0); lv_obj_set_style_text_color(o, lv_color_hex(c), 0);
    lv_obj_set_style_text_align(o, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(o, x, y); lv_obj_set_width(o, w);
    return o;
}
void scanner_weather_theme_create(lv_obj_t *s) {
    lv_obj_set_style_bg_color(s, lv_color_hex(DARK), 0);
    text(s, "--", &impact_20, GOLD, 44, 27, 40);
    text(s, "-- C", &impact_20, GOLD, 96, 27, 140);
    lv_obj_t *date = box(s, 34, 61, 212, 25, GOLD, 12);
    text(date, "WAIT HOST", &impact_20, DARK, 4, 1, 204);
    text(s, "--:--", &impact_48, WHITE, 32, 91, 216);
    lv_obj_t *panel = box(s, 14, 151, 252, 72, GOLD, 15);
    lv_obj_set_style_bg_grad_color(panel, lv_color_hex(0xA57D25), 0);
    lv_obj_set_style_bg_grad_dir(panel, LV_GRAD_DIR_VER, 0);
    text(panel, "WPM", &impact_20, DARK, 8, 4, 74);
    text(panel, "L", &impact_20, DARK, 89, 4, 74);
    text(panel, "R", &impact_20, DARK, 170, 4, 74);
    wpm = text(panel, "0", &impact_20, DARK, 8, 25, 74);
    lbat = text(panel, "--", &impact_20, DARK, 89, 25, 74);
    rbat = text(panel, "--", &impact_20, DARK, 170, 25, 74);
    box(panel, 83, 6, 1, 38, 0xA57D25, 0);
    box(panel, 165, 6, 1, 38, 0xA57D25, 0);
    const uint8_t order[4] = {2, 1, 0, 3};
    for (int k = 0; k < 4; k++) {
        keys[k] = box(panel, 8+k*60, 48, 56, 20, DARK, 4);
        icons[k] = lv_image_create(keys[k]);
        lv_image_set_src(icons[k], mac_modifier_images[order[k]]);
        lv_obj_set_pos(icons[k], 19, 1);
        lv_obj_set_style_image_recolor(icons[k], lv_color_hex(WHITE), 0);
        lv_obj_set_style_image_recolor_opa(icons[k], LV_OPA_COVER, 0);
    }
}
void scanner_weather_theme_destroy(void) {
    wpm = lbat = rbat = NULL;
    for (int i = 0; i < 4; i++) keys[i] = icons[i] = NULL;
}
void scanner_weather_theme_update(const struct prospector_keyboard_data *d) {
    if (!d || !wpm) return;
    char b[12];
    snprintk(b, sizeof(b), "%u", d->wpm_value); lv_label_set_text(wpm, b);
    snprintk(b, sizeof(b), "%u", d->battery_level); lv_label_set_text(lbat, b);
    snprintk(b, sizeof(b), "%u", d->peripheral_battery[0]); lv_label_set_text(rbat, b);
    const uint8_t masks[4] = {0x88, 0x44, 0x11, 0x22};
    for (int k = 0; k < 4; k++) {
        bool pressed = (d->modifier_flags & masks[k]) != 0;
        lv_obj_set_style_bg_color(keys[k], lv_color_hex(pressed ? GOLD : DARK), 0);
        lv_obj_set_style_image_recolor(icons[k], lv_color_hex(pressed ? DARK : WHITE), 0);
    }
}
