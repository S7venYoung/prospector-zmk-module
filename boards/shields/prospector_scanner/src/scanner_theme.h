#pragma once

#include "prospector_layouts.h"

/*
 * One scanner data source for every display theme.
 * Scanner packets are normalized here, so theme renderers never depend on
 * split-central events used by dongle firmware.
 */
void prospector_scanner_theme_update(const struct prospector_keyboard_data *data);
const struct prospector_keyboard_data *prospector_scanner_theme_state(void);
