#include "scanner_idle_theme.h"
#include "fonts_carrefinho.h"
#include <stdio.h>

/* Runtime channel is owned by the scanner core; this idle page only renders it. */
extern uint8_t scanner_get_runtime_channel(void);

#define INK 0x0B1010
#define GRID 0x172022
#define STEEL 0x53636B
#define MUTED 0x81909B
#define PAPER 0xF3EEE5
#define YELLOW 0xFFDF36
#define GREEN 0x82F060

static lv_obj_t *root;
static lv_obj_t *beam_parts[12];
static lv_obj_t *target_pips[3];
static lv_timer_t *scan_timer;
static lv_obj_t *channel_value;
static uint16_t scan_angle;
static uint8_t scan_tick;
static uint8_t last_channel = 0xFF;
static char channel_text[8];

static void scanner_idle_refresh_channel(void) {
    uint8_t channel = scanner_get_runtime_channel();
    if (channel == last_channel || !channel_value) {
        return;
    }
    last_channel = channel;
    if (channel >= 10) {
        snprintf(channel_text, sizeof(channel_text), "CH ALL");
    } else {
        snprintf(channel_text, sizeof(channel_text), "CH %u", channel);
    }
    lv_label_set_text_static(channel_value, channel_text);
}

static void scanner_idle_scan_tick(lv_timer_t *timer) {
    ARG_UNUSED(timer);
    if (!root) {
        return;
    }
    scanner_idle_refresh_channel();
}

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

static void bluetooth_icon(lv_obj_t *parent, int x, int y) {
    /* Pixel-built Bluetooth rune: avoids an optional icon-font dependency. */
    static const char *const pixels[] = {
        "..X..", "..XX.", "X.X.X", ".XX..", "X.X.X", "..XX.", "..X.."
    };
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (pixels[row][col] != 'X') {
                continue;
            }
            lv_obj_t *pixel = lv_obj_create(parent);
            plain(pixel, YELLOW);
            lv_obj_set_size(pixel, 3, 3);
            lv_obj_set_pos(pixel, x + col * 3, y + row * 3);
            lv_obj_set_style_radius(pixel, 1, 0);
        }
    }
}

static void keyboard_icon(lv_obj_t *parent, int x, int y) {
    lv_obj_t *body = lv_obj_create(parent);
    plain(body, INK);
    lv_obj_set_size(body, 22, 15);
    lv_obj_set_pos(body, x, y);
    lv_obj_set_style_border_width(body, 2, 0);
    lv_obj_set_style_border_color(body, lv_color_hex(YELLOW), 0);
    lv_obj_set_style_radius(body, 3, 0);
    for (int col = 0; col < 3; col++) {
        lv_obj_t *key = lv_obj_create(body);
        plain(key, YELLOW);
        lv_obj_set_size(key, 3, 3);
        lv_obj_set_pos(key, 4 + col * 5, 4);
        lv_obj_set_style_radius(key, 1, 0);
    }
    lv_obj_t *space = lv_obj_create(body);
    plain(space, YELLOW);
    lv_obj_set_size(space, 10, 2);
    lv_obj_set_pos(space, 5, 10);
    lv_obj_set_style_radius(space, 1, 0);
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
    /* Embedded condensed display font: matches the bold Dongle/Codex treatment. */
    label(screen, "PROSPECTOR", &DINishCondensed_SemiBold_20, PAPER, 17, 6);
    label(screen, "//", &DINishCondensed_SemiBold_20, YELLOW, 127, 6);
    label(screen, "SCAN", &DINishCondensed_SemiBold_20, YELLOW, 157, 6);
    lv_obj_t *rule = lv_obj_create(screen);
    plain(rule, STEEL);
    lv_obj_set_size(rule, 244, 1);
    lv_obj_set_pos(rule, 18, 30);

    /* Concentric radar and the active sweep direction. */
    radar_ring(screen, 84, 34, 112);
    radar_ring(screen, 98, 48, 84);
    radar_ring(screen, 112, 62, 56);
    lv_obj_t *hline = lv_obj_create(screen);
    plain(hline, STEEL); lv_obj_set_size(hline, 108, 1); lv_obj_set_pos(hline, 86, 90);
    lv_obj_set_style_bg_opa(hline, LV_OPA_60, 0);
    lv_obj_t *vline = lv_obj_create(screen);
    plain(vline, STEEL); lv_obj_set_size(vline, 1, 108); lv_obj_set_pos(vline, 139, 36);
    lv_obj_set_style_bg_opa(vline, LV_OPA_60, 0);

    /* Centre-anchored scanning pointer. The stationary rings stay clean:
     * this is the only yellow moving element. */
    for (int i = 0; i < 12; i++) {
        int size = i < 5 ? 2 : 3;
        lv_obj_t *part = lv_obj_create(screen);
        plain(part, YELLOW);
        lv_obj_set_size(part, size, size);
        lv_obj_set_pos(part, 140 - size / 2, 83 - i * 4);
        lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_opa(part, i < 4 ? LV_OPA_50 : LV_OPA_COVER, 0);
        beam_parts[i] = part;
    }

    const int pips[][2] = {{109, 68}, {166, 102}, {122, 119}};
    for (int i = 0; i < 3; i++) {
        lv_obj_t *pip = lv_obj_create(screen);
        plain(pip, YELLOW);
        lv_obj_set_size(pip, 7, 7);
        lv_obj_set_pos(pip, pips[i][0], pips[i][1]);
        lv_obj_set_style_radius(pip, LV_RADIUS_CIRCLE, 0);
        target_pips[i] = pip;
    }
    lv_obj_t *core = lv_obj_create(screen);
    plain(core, YELLOW); lv_obj_set_size(core, 12, 12); lv_obj_set_pos(core, 134, 84);
    lv_obj_set_style_radius(core, LV_RADIUS_CIRCLE, 0);

    lv_obj_t *ble = card(screen, 14, 163, 122, 33, 8);
    bluetooth_icon(ble, 8, 5);
    label(ble, "BLE SCAN", &lv_font_montserrat_12, PAPER, 31, 4);
    label(ble, "ACTIVE", &lv_font_montserrat_12, MUTED, 31, 17);
    lv_obj_t *dot = lv_obj_create(ble);
    plain(dot, GREEN); lv_obj_set_size(dot, 8, 8); lv_obj_set_pos(dot, 101, 12);
    lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);

    lv_obj_t *key = card(screen, 144, 163, 122, 33, 8);
    keyboard_icon(key, 8, 9);
    label(key, "WAITING", &lv_font_montserrat_12, PAPER, 36, 4);
    label(key, "KEYBOARD", &lv_font_montserrat_12, MUTED, 36, 17);

    lv_obj_t *telemetry = card(screen, 14, 202, 252, 25, 8);
    channel_value = label(telemetry, "CH --", &lv_font_montserrat_12, PAPER, 16, 6);
    label(telemetry, "RSSI --", &lv_font_montserrat_12, MUTED, 92, 6);
    label(telemetry, "0.0 HZ", &lv_font_montserrat_12, YELLOW, 177, 6);

    scan_angle = 0;
    scan_tick = 0;
    last_channel = 0xFF;
    scanner_idle_refresh_channel();

    /* The ST7789 path flushes a whole frame for each moving object. Keeping
     * the unpaired idle screen static prevents visible LCD/backlight flashing.
     * Animation resumes only after a future partial-flush renderer is used. */
    scan_timer = NULL;
}

void scanner_idle_theme_destroy(void) {
    if (scan_timer) {
        lv_timer_del(scan_timer);
        scan_timer = NULL;
    }
    for (int i = 0; i < 12; i++) {
        beam_parts[i] = NULL;
    }
    channel_value = NULL;
    last_channel = 0xFF;
    for (int i = 0; i < 3; i++) {
        target_pips[i] = NULL;
    }
    root = NULL;
}
