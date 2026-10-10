#include "scanner_retro_workstation_theme.h"

#include <stdint.h>
#include <lvgl.h>

#include "retro_workstation/reference_pixels.h"

/* This renderer intentionally uses the reference RGB565 artwork verbatim.
 * The retro layout is a fixed pixel-art composition, not a dynamic dashboard. */
static const lv_img_dsc_t retro_workstation_art = {
    .header = {
        /* ZMK's Zephyr 4.1 LVGL uses the v9 color-format enum. */
        .cf = LV_COLOR_FORMAT_RGB565,
        .w = 280,
        .h = 240,
    },
    .data_size = sizeof(reference_pixels),
    .data = (const uint8_t *)reference_pixels,
};

void scanner_retro_workstation_theme_create(lv_obj_t *screen) {
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, 280, 240);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x141414), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_style_radius(screen, 43, 0);
    lv_obj_set_style_clip_corner(screen, true, 0);

    lv_obj_t *art = lv_img_create(screen);
    lv_img_set_src(art, &retro_workstation_art);
    lv_obj_set_pos(art, 0, 0);
    lv_obj_clear_flag(art, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}

void scanner_retro_workstation_theme_destroy(void) {}

void scanner_retro_workstation_theme_update(const struct prospector_keyboard_data *data) {
    (void)data;
}
