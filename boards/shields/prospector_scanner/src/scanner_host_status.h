#pragma once

#include <stdbool.h>
#include <stdint.h>

/* Newline-delimited protocol over the scanner's existing USB CDC ACM port.
 * PING                      -> PROSPECTOR-SCANNER/1
 * CODEX <left-percent> <tokens> -> OK
 * This is intentionally separate from ZMK Studio RPC: a scanner is a
 * standalone BLE observer and must not require DYA/Studio to be running. */
struct scanner_host_status {
    uint8_t codex_left_percent;
    uint32_t codex_total_tokens;
    bool codex_available;
};

const struct scanner_host_status *scanner_host_status_get(void);
bool scanner_host_status_take_changed(void);
