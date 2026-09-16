#include "scanner_theme.h"

#include <string.h>

/* Kept on the display thread: renderers may read this safely during LVGL updates. */
static struct prospector_keyboard_data scanner_theme_state;

void prospector_scanner_theme_update(const struct prospector_keyboard_data *data) {
    if (data == NULL) {
        memset(&scanner_theme_state, 0, sizeof(scanner_theme_state));
        return;
    }
    scanner_theme_state = *data;
}

const struct prospector_keyboard_data *prospector_scanner_theme_state(void) {
    return &scanner_theme_state;
}
