#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>
#include "prospector_touch.h"

extern void prospector_set_brightness(uint8_t level);

struct prospector_preferences {
    uint8_t brightness;
    bool always_on;
} __packed;

static struct prospector_preferences preferences = {
    .brightness = 85,
    .always_on = IS_ENABLED(CONFIG_PROSPECTOR_SCREEN_ALWAYS_ON),
};
static bool loaded;
static bool dirty;

static int preferences_set(const char *name, size_t len, settings_read_cb read_cb, void *arg) {
    const char *next;
    if (!settings_name_steq(name, "display", &next) || *next || len != sizeof(preferences)) return -ENOENT;
    int rc = read_cb(arg, &preferences, sizeof(preferences));
    if (rc >= 0) preferences.brightness = CLAMP(preferences.brightness, 1, 100);
    return rc;
}
SETTINGS_STATIC_HANDLER_DEFINE(prospector, "prospector", NULL, preferences_set, NULL, NULL);

static int preferences_init(void) {
    settings_load_subtree("prospector");
    loaded = true;
    prospector_set_brightness(preferences.brightness);
    return 0;
}
SYS_INIT(preferences_init, APPLICATION, 80);

uint8_t prospector_settings_brightness(void) { return preferences.brightness; }
void prospector_settings_set_brightness(uint8_t value) {
    preferences.brightness = CLAMP(value, 1, 100); dirty = loaded; prospector_set_brightness(preferences.brightness);
}
bool prospector_settings_always_on(void) { return preferences.always_on; }
void prospector_settings_set_always_on(bool enabled) { preferences.always_on = enabled; dirty = loaded; }
void prospector_settings_save(void) {
    if (loaded && dirty && !settings_save_one("prospector/display", &preferences, sizeof(preferences))) dirty = false;
}
