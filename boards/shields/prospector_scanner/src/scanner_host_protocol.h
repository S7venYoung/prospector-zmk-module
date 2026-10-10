#pragma once
#include "scanner_host_status.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* Parse atomically: malformed frames must not overwrite the last good state.
 * v2: -1 denotes unknown independently for quota, total and weekly quota. */
static inline bool scanner_host_parse(const char *text, struct scanner_host_status *out) {
    bool v2 = strncmp(text, "CODEX2 ", 7) == 0;
    if (!v2 && strncmp(text, "CODEX ", 6) != 0) return false;
    const char *p = text + (v2 ? 7 : 6);
    int64_t values[3] = {-1, -1, -1};
    int count = 0;
    while (count < 3) {
        /* No empty fields, leading whitespace or '+' signs. */
        if (!((*p >= '0' && *p <= '9') || (v2 && *p == '-' && p[1] == '1')))
            return false;
        char *end;
        errno = 0;
        long long value = strtoll(p, &end, 10);
        int64_t maximum = count == 1 ? UINT32_MAX : 100;
        if (errno == ERANGE || end == p || value < (v2 ? -1 : 0) || value > maximum)
            return false;
        values[count++] = value;
        if (*end == '\0') break;
        if (*end != ' ') return false;
        p = end + 1;
        if (count == 3) return false;
    }
    if (count < 2 || (v2 && count != 3)) return false;
    struct scanner_host_status next = {
        .codex_left_percent = values[0] < 0 ? UINT8_MAX : (uint8_t)values[0],
        .codex_total_tokens = values[1] < 0 ? UINT32_MAX : (uint32_t)values[1],
        .codex_week_left_percent = values[2] < 0 ? 0 : (uint8_t)values[2],
        .codex_week_available = values[2] >= 0,
        .codex_left_available = values[0] >= 0,
        .codex_tokens_available = values[1] >= 0,
        .codex_available = true,
    };
    *out = next;
    return true;
}
