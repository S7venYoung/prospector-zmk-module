#include "scanner_walle_theme.h"
#include <lvgl.h>
#include <zephyr/sys/printk.h>
static lv_obj_t *layer,*wpm,*lbat,*rbat;
static lv_obj_t *t(lv_obj_t*p,const char*s,const lv_font_t*f,uint32_t c,int x,int y){lv_obj_t*o=lv_label_create(p);lv_label_set_text(o,s);lv_obj_set_style_text_font(o,f,0);lv_obj_set_style_text_color(o,lv_color_hex(c),0);lv_obj_set_pos(o,x,y);return o;}
static lv_obj_t *box(lv_obj_t*p,int x,int y,int w,int h,uint32_t c){lv_obj_t*o=lv_obj_create(p);lv_obj_remove_style_all(o);lv_obj_set_size(o,w,h);lv_obj_set_pos(o,x,y);lv_obj_set_style_bg_color(o,lv_color_hex(c),0);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);lv_obj_set_style_radius(o,14,0);return o;}
void scanner_walle_theme_create(lv_obj_t*s){box(s,0,0,280,240,0x101411);box(s,12,10,256,42,0xFFC21A);t(s,"OO",&lv_font_montserrat_28,0x101411,24,13);t(s,"SOFLE // CODEX",&lv_font_montserrat_20,0x101411,84,20);t(s,"BASE",&lv_font_montserrat_28,0xF3EEE5,82,65);layer=t(s,"SCANNING",&lv_font_montserrat_16,0xFFC21A,86,106);t(s,"WPM",&lv_font_montserrat_16,0xFFC21A,108,145);wpm=t(s,"0",&lv_font_montserrat_28,0xF3EEE5,124,161);box(s,15,200,120,30,0xF3EEE5);box(s,145,200,120,30,0xF3EEE5);lbat=t(s,"L --%",&lv_font_montserrat_20,0x101411,30,204);rbat=t(s,"R --%",&lv_font_montserrat_20,0x101411,160,204);}
void scanner_walle_theme_destroy(void){layer=wpm=lbat=rbat=NULL;}
void scanner_walle_theme_update(const struct prospector_keyboard_data*d){if(!d||!layer)return;lv_label_set_text(layer,d->current_layer_name[0]?d->current_layer_name:"BASE");char b[12];snprintk(b,sizeof(b),"%u",d->wpm_value);lv_label_set_text(wpm,b);snprintk(b,sizeof(b),"L %u%%",d->battery_level);lv_label_set_text(lbat,b);snprintk(b,sizeof(b),"R %u%%",d->peripheral_battery[0]);lv_label_set_text(rbat,b);}
