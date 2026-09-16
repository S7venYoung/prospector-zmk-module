#include "scanner_idle_theme.h"

#define INK 0x0B1010
#define GRID 0x172022
#define STEEL 0x53636B
#define MUTED 0x81909B
#define PAPER 0xF3EEE5
#define YELLOW 0xFFDF36
#define GREEN 0x82F060

static lv_obj_t *root;

static void plain(lv_obj_t *obj, uint32_t color) {
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static lv_obj_t *label(lv_obj_t *parent, const char *value, const lv_font_t *font,
                       uint32_t color, int x, int y) {
    lv_obj_t *obj = lv_label_create(parent);
    lv_label_set_text(obj, value);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(color), 0);
    lv_obj_set_pos(obj, x, y);
    return obj;
}

static lv_obj_t *card(lv_obj_t *parent, int x, int y, int w, int h, int radius) {
    lv_obj_t *obj = lv_obj_create(parent);
    plain(obj, INK);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_border_width(obj, 1, 0);
    lv_obj_set_style_border_color(obj, lv_color_hex(STEEL), 0);
    return obj;
}

static void radar_ring(lv_obj_t *parent, int x, int y, int size) {
    lv_obj_t *ring = lv_obj_create(parent);
    plain(ring, INK);
    lv_obj_set_size(ring, size, size);
    lv_obj_set_pos(ring, x, y);
    lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(ring, 1, 0);
    lv_obj_set_style_border_color(ring, lv_color_hex(YELLOW), 0);
    lv_obj_set_style_border_opa(ring, LV_OPA_60, 0);
}

void scanner_idle_theme_create(lv_obj_t *screen) {
    root = screen;
    plain(screen, INK);
    lv_obj_set_size(screen, 280, 240);
    lv_obj_set_style_radius(screen, 24, 0);

    /* Header stays inside the physical rounded-glass safe area. */
    label(screen, "PROSPECTOR", &lv_font_montserrat_16, PAPER, 18, 8);
    label(screen, "//", &lv_font_montserrat_16, YELLOW, 124, 8);
    label(screen, "SCAN", &lv_font_montserrat_16, YELLOW, 148, 8);
    lv_obj_t *rule = lv_obj_create(screen);
    plain(rule, STEEL);
    lv_obj_set_size(rule, 244, 1);
    lv_obj_set_pos(rule, 18, 30);

    /* Concentric radar and the active sweep direction. */
    radar_ring(screen, 95, 36, 90);
    radar_ring(screen, 107, 48, 66);
    radar_ring(screen, 119, 60, 42);
    lv_obj_t *hline = lv_obj_create(screen);
    plain(hline, YELLOW); lv_obj_set_size(hline, 88, 1); lv_obj_set_pos(hline, 96, 81);
    lv_obj_set_style_bg_opa(hline, LV_OPA_40, 0);
    lv_obj_t *vline = lv_obj_create(screen);
    plain(vline, YELLOW); lv_obj_set_size(vline, 1, 88); lv_obj_set_pos(vline, 139, 37);
    lv_obj_set_style_bg_opa(vline, LV_OPA_40, 0);

    /* Sweep: bright arc plus three candidate pips. */
    lv_obj_t *sweep = lv_arc_create(screen);
    lv_obj_remove_style_all(sweep);
    lv_obj_set_size(sweep, 90, 90);
    lv_obj_set_pos(sweep, 95, 36);
    lv_arc_set_range(sweep, 0, 100);
    lv_arc_set_value(sweep, 25);
    lv_arc_set_bg_angles(sweep, 315, 55);
    lv_obj_set_style_arc_width(sweep, 3, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(sweep, lv_color_hex(YELLOW), LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(sweep, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(sweep, 0, LV_PART_MAIN);
    lv_obj_remove_flag(sweep, LV_OBJ_FLAG_CLICKABLE);

    const int pips[][2] = {{118, 62}, {160, 89}, {125, 102}};
    for (int i = 0; i < 3; i++) {
        lv_obj_t *pip = lv_obj_create(screen);
        plain(pip, YELLOW);
        lv_obj_set_size(pip, 7, 7);
        lv_obj_set_pos(pip, pips[i][0], pips[i][1]);
        lv_obj_set_style_radius(pip, LV_RADIUS_CIRCLE, 0);
    }
    lv_obj_t *core = lv_obj_create(screen);
    plain(core, YELLOW); lv_obj_set_size(core, 12, 12); lv_obj_set_pos(core, 134, 75);
    lv_obj_set_style_radius(core, LV_RADIUS_CIRCLE, 0);

    lv_obj_t *searching = label(screen, "SEARCHING", &lv_font_montserrat_20, YELLOW, 50, 132);
    lv_obj_set_width(searching, 180);
    lv_obj_set_style_text_align(searching, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *ble = card(screen, 14, 163, 122, 33, 8);
    label(ble, "BLE SCAN", &lv_font_montserrat_12, PAPER, 10, 4);
    label(ble, "ACTIVE", &lv_font_montserrat_12, MUTED, 10, 17);
    lv_obj_t *dot = lv_obj_create(ble);
    plain(dot, GREEN); lv_obj_set_size(dot, 8, 8); lv_obj_set_pos(dot, 101, 12);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);

    lv_obj_t *key = card(screen, 144, 163, 122, 33, 8);
    label(key, "WAITING", &lv_font_montserrat_12, PAPER, 10, 4);
    label(key, "KEYBOARD", &lv_font_montserrat_12, MUTED, 10, 17);

    lv_obj_t *telemetry = card(screen, 14, 202, 252, 25, 8);
    label(telemetry, "CH 0", &lv_font_montserrat_12, PAPER, 16, 6);
    label(telemetry, "RSSI --", &lv_font_montserrat_12, MUTED, 92, 6);
    label(telemetry, "0.0 HZ", &lv_font_montserrat_12, YELLOW, 177, 6);
}

void scanner_idle_theme_destroy(void) {
    root = NULL;
}
