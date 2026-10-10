#pragma once

#include <stdbool.h>
#include <stdint.h>

/* Newline-delimited protocol over the scanner's existing USB CDC ACM port.
 * PING                      -> PROSPECTOR-SCANNER/2
 * CODEX <left-percent> <tokens> [week-left-percent] -> OK
 * CODEX2 <left-percent> <tokens> <week-left-percent> -> OK
 * CODEX2 allows -1 for each unknown field; legacy CODEX remains supported.
 * This is intentionally separate from ZMK Studio RPC: a scanner is a
 * standalone BLE observer and must not require DYA/Studio to be running. */
struct scanner_host_status {
    uint8_t codex_left_percent;
    uint32_t codex_total_tokens;
    uint8_t codex_week_left_percent;
    bool codex_week_available;
    bool codex_left_available;
    bool codex_tokens_available;
    bool codex_available;
};

const struct scanner_host_status *scanner_host_status_get(void);
bool scanner_host_status_take_changed(void);
