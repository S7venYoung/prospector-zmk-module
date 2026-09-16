#include "scanner_codex_theme.h"
#include <lvgl.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <string.h>

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
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}
static lv_obj_t *label(lv_obj_t *p, const char *s, const lv_font_t *f, uint32_t color, int x, int y) {
    lv_obj_t *o = lv_label_create(p);
    lv_label_set_text(o, s);
    lv_obj_set_style_text_font(o, f, 0);
    lv_obj_set_style_text_color(o, lv_color_hex(color), 0);
    lv_obj_set_pos(o, x, y);
    return o;
}
static lv_obj_t *card(lv_obj_t *p, int x, int y, int w, int h, uint32_t border, int radius) {
    lv_obj_t *o = lv_obj_create(p);
    plain(o, INK); lv_obj_set_size(o,w,h); lv_obj_set_pos(o,x,y);
    lv_obj_set_style_radius(o,radius,0); lv_obj_set_style_border_width(o,2,0);
    lv_obj_set_style_border_color(o,lv_color_hex(border),0);
    return o;
}
static void battery_card(lv_obj_t *screen, int x, const char *side, lv_obj_t **value,
                         lv_obj_t **fill, lv_obj_t **dot) {
    lv_obj_t *o=card(screen,x,190,122,38,MUTED,11);
    label(o,side,&lv_font_montserrat_20,MUTED,8,7);
    *value=label(o,"--%",&lv_font_montserrat_20,PAPER,29,7);
    lv_obj_t *track=lv_obj_create(o); plain(track,PAPER); lv_obj_set_size(track,36,12);
    lv_obj_set_pos(track,63,13); lv_obj_set_style_radius(track,3,0);
    *fill=lv_obj_create(track); plain(*fill,GREEN); lv_obj_set_size(*fill,2,8);
    lv_obj_set_pos(*fill,2,2); lv_obj_set_style_radius(*fill,2,0);
    *dot=lv_obj_create(o); plain(*dot,RED); lv_obj_set_size(*dot,9,9);
    lv_obj_set_pos(*dot,105,14); lv_obj_set_style_radius(*dot,LV_RADIUS_CIRCLE,0);
}
void scanner_codex_theme_create(lv_obj_t *screen) {
    plain(screen,INK); lv_obj_set_size(screen,280,240); lv_obj_set_style_radius(screen,24,0);
    label(screen,"CODEX",&lv_font_montserrat_20,YELLOW,18,8);
    label(screen,"// SOFLE",&lv_font_montserrat_20,PAPER,86,8);
    label(screen,"SCANNER",&lv_font_montserrat_12,PAPER,206,12);
    lv_obj_t *rule=lv_obj_create(screen); plain(rule,YELLOW); lv_obj_set_size(rule,244,2); lv_obj_set_pos(rule,18,37);
    lv_obj_t *left=card(screen,14,48,122,96,YELLOW,14);
    label(left,"5 HOUR LEFT",&lv_font_montserrat_16,PAPER,8,8);
    label(left,"///",&lv_font_montserrat_16,YELLOW,91,8);
    host_left=label(left,"--%",&lv_font_montserrat_28,YELLOW,29,43);
    lv_obj_t *right=card(screen,144,48,122,96,YELLOW,14);
    label(right,"TODAY TOTAL",&lv_font_montserrat_16,PAPER,8,8);
    label(right,"///",&lv_font_montserrat_16,YELLOW,91,8);
    host_tokens=label(right,"--",&lv_font_montserrat_28,PAPER,40,43);
    lv_obj_t *bar=card(screen,14,151,252,31,MUTED,10);
    label(bar,"LAYER",&lv_font_montserrat_16,MUTED,10,7);
    layer_value=label(bar,"BASE",&lv_font_montserrat_20,YELLOW,76,4);
    lv_obj_t *divider=lv_obj_create(bar); plain(divider,MUTED); lv_obj_set_size(divider,1,21); lv_obj_set_pos(divider,136,5);
    label(bar,"WPM",&lv_font_montserrat_16,MUTED,164,7);
    wpm_value=label(bar,"0",&lv_font_montserrat_20,YELLOW,221,4);
    battery_card(screen,14,"L",&left_value,&left_fill,&left_dot);
    battery_card(screen,144,"R",&right_value,&right_fill,&right_dot);
}
void scanner_codex_theme_destroy(void) { layer_value=wpm_value=left_value=right_value=left_fill=right_fill=left_dot=right_dot=host_left=host_tokens=NULL; }
static void set_battery(lv_obj_t *value, lv_obj_t *fill, lv_obj_t *dot, uint8_t level) {
    char b[8]; snprintk(b,sizeof(b),"%u%%",level); lv_label_set_text(value,b);
    lv_obj_set_width(fill, MAX(2,(int)level*32/100));
    lv_obj_set_style_bg_color(fill,lv_color_hex(level<20?RED:GREEN),0);
    lv_obj_set_style_bg_color(dot,lv_color_hex(level?GREEN:RED),0);
}
void scanner_codex_theme_update(const struct prospector_keyboard_data *d) {
    if (!d || !layer_value) return;
    lv_label_set_text(layer_value,d->current_layer_name[0]?d->current_layer_name:"BASE");
    char w[8]; snprintk(w,sizeof(w),"%u",d->wpm_value); lv_label_set_text(wpm_value,w);
    set_battery(left_value,left_fill,left_dot,d->battery_level);
    set_battery(right_value,right_fill,right_dot,d->peripheral_battery[0]);
    /* Host values are populated by the scanner CDC bridge in the next step. */
}
